/******************************************************************************/
/*                                                                            */
/*                           b b c p _ F i l e . C                            */
/*                                                                            */
/* (c) 2002 by the Board of Trustees of the Leland Stanford, Jr., University  */
/*      All Rights Reserved. See bbcp_Version.C for complete License Terms    *//*                            All Rights Reserved                             */
/*   Produced by Andrew Hanushevsky for Stanford University under contract    */
/*              DE-AC03-76-SFO0515 with the Department of Energy              */
/******************************************************************************/
  
#include <errno.h>
#include <limits.h>
#include <string.h>
#include <sys/types.h>
#include <sys/uio.h>
#include "bbcp_Platform.h"
#include "bbcp_ChkSum.h"
#include "bbcp_Config.h"
#include "bbcp_Debug.h"
#include "bbcp_Emsg.h"
#include "bbcp_File.h"
#include "bbcp_Headers.h"
#include "bbcp_RTCopy.h"

/******************************************************************************/
/*                         L o c a l   C l a s s e s                          */
/******************************************************************************/
  
class bbcp_FileChkSum
{
public:

bbcp_File       *csFile;
bbcp_BuffPool   *otPool;
bbcp_BuffPool    csPool;
bbcp_ChkSum     *csObj;
int              csVer;
int              csRC;

     bbcp_FileChkSum(bbcp_BuffPool *obP,bbcp_File *fP,int csT,int csV,int n=0)
                    : csFile(fP), otPool(obP), csObj(bbcp_ChkSum::Alloc(csT)),
                      csVer(csV), csRC(n) {}
    ~bbcp_FileChkSum() {if (csObj) delete csObj;}
};

/******************************************************************************/
/*                         G l o b a l   V a l u e s                          */
/******************************************************************************/
  
extern bbcp_BuffPool bbcp_BPool;

extern bbcp_BuffPool bbcp_CPool;

extern bbcp_Config   bbcp_Config;

extern bbcp_RTCopy   bbcp_RTCopy;
 
/******************************************************************************/
/*            E x t e r n a l   T h r e a d   I n t e r f a c e s             */
/******************************************************************************/
  
extern "C"
{
void *bbcp_FileCSX(void *pp)
{
   bbcp_FileChkSum *csP = (bbcp_FileChkSum *)pp;
   bbcp_Buffer *bP;
   int csLen = csP->csObj->csSize(), csCopy = csP->csVer;

// Simple loop to do checksumming
//
   while((bP = csP->csPool.getFullBuff()) && bP->blen)
        {csP->csObj->Update(bP->data, bP->blen);
         if (csCopy) memcpy(bP->bHdr.cksm, csP->csObj->csCurr(), csLen);
         csP->otPool->putFullBuff(bP);
        }

// Send checksum to the target if so needed
//
   if ((bbcp_Config.csOpts & bbcp_csSend) && bP && bP->boff >= 0)
      {bP->blen = -BBCP_CLCKS;
       memcpy(bP->bHdr.cksm, csP->csObj->csCurr(), csLen);
      }

// Either we aborted or ended. Make sure to cascade null buffer to signal eof.
//
   if (bP) csP->otPool->putFullBuff(bP);
   return 0;
}

void *bbcp_FileCSY(void *pp)
{
   bbcp_FileChkSum *csP = (bbcp_FileChkSum *)pp;

   csP->csRC = csP->csFile->Passthru(csP->otPool, &csP->csPool, csP, csP->csRC);
   return 0;
}
}

/******************************************************************************/
/*                           C o n s t r u c t o r                            */
/******************************************************************************/

bbcp_File::bbcp_File(const char *path, bbcp_IO *iox,
                     bbcp_FileSystem *fsp, int secSize)
{
   nextbuff    = 0;
   nextoffset  = 0;
   lastoff     = 0;
   blockSize   = secSize;
   curq        = 0;
   iofn        = strdup(path);
   IOB         = iox;
   FSp         = fsp;
   bufreorders = 0;
   maxreorders = 0;
   rtCopy      = (bbcp_Config.Options & bbcp_RTCSRC ? 1 : 0);
}
  
/******************************************************************************/
/* Public:                         C l o s e                                  */
/******************************************************************************/
  
int bbcp_File::Close()
{

// Stop the rtCopy object if we may have started one.
//
   if (rtCopy) bbcp_RTCopy.Stop();

// Prevent infinite loops
//
   if (IOB->FD() >= 0) IOB->Close();
   return 0;
}

/******************************************************************************/
/* Private:                    g e t B u f f e r                              */
/******************************************************************************/

bbcp_Buffer *bbcp_File::getBuffer(long long offset)
{
   bbcp_Buffer *bp, *pp=0;

// Find a buffer
//
   if (bp = nextbuff)
      while(bp && bp->boff != offset) {pp = bp; bp = bp->next;}

// If we found a buffer, unchain it
//
   if (bp) {curq--;
            if (pp) pp->next = bp->next;
                else nextbuff = bp->next;
//if (!curq) {DEBUG("Queue has been emptied at offset " <<offset);}
           }

// Return what we have
//
   return bp;
}

/******************************************************************************/
/* Public:                   P a s s t h r o u g h                            */
/******************************************************************************/
  
int bbcp_File::Passthru(bbcp_BuffPool *iBP, bbcp_BuffPool *oBP, 
                        bbcp_FileChkSum *csP, int nstrms)
{
    bbcp_Buffer *outbuff;
    bbcp_ChkSum *csObj;
    long long Offset = nextoffset;
    int csLen, csVer, numadd, maxbufs, maxadds = nstrms;
    int rc = 0, unordered = !(bbcp_Config.Options & bbcp_ORDER);

// Determine if we will be piggy-backing checksumming here
//
   if (csP && (csObj = csP->csObj))
      {csVer = csP->csVer; csLen = csObj->csSize();}
      else csVer = csLen = 0;

// Record the maximum number of buffers we have here
//
   maxbufs = iBP->BuffCount();
   if (!(numadd = nstrms)) numadd = 1;

// Read all of the data until eof (note that we are single threaded)
//
   while(nstrms)
      {
      // Obtain a full buffer
      //
         if (!(outbuff = iBP->getFullBuff())) break;

     // Check if this is an eof marker
     //
     //  cerr <<nstrms <<" Passt " <<outbuff->blen <<'@' <<outbuff->boff <<endl;
        if (!(outbuff->blen))
           {iBP->putEmptyBuff(outbuff); nstrms--; continue;}

      // Do an unordered write if allowed
      //
         if (unordered) {oBP->putFullBuff(outbuff); continue;}

      // Check if this buffer is in the correct sequence
      //
         if (outbuff->boff != Offset)
            {if (outbuff->boff < 0) {rc = -ESPIPE; break;}
             outbuff->next = nextbuff;
             nextbuff = outbuff;
             bufreorders++;
             if (++curq > maxreorders) 
                {maxreorders = curq;
                 DEBUG("Buff disorder " <<curq <<" rcvd " <<outbuff->boff <<" want " <<Offset);
                }
             if (curq >= maxbufs)
                {if (!(--maxadds)) {rc = -ENOBUFS; break;}
                 DEBUG("Too few buffs; adding " <<numadd <<" more.");
                 bbcp_BPool.Allocate(numadd);
                 maxbufs += numadd;
                }
             continue;
            }

      // Pass through any queued buffers
      //
      do {Offset += outbuff->blen;
          if (csObj)
             {csObj->Update(outbuff->data, outbuff->blen);
              if (csVer && memcmp(outbuff->bHdr.cksm, csObj->csCurr(), csLen))
                 {char buff[32];
                  sprintf(buff, "%lld", outbuff->boff);
                  bbcp_Fmsg("Write",iofn, "xfr checksum error at offset",buff);
                  rc = -EILSEQ; nstrms = 0; break;
                 }
             }
          oBP->putFullBuff(outbuff);
         } while(nextbuff && (outbuff = getBuffer(Offset)));
      }

// Check if we should print an error here
//
   if (rc && rc != -EILSEQ) bbcp_Emsg("Write",rc, "unable to write", iofn);

// Queue an empty buffer indicating eof or abort the stream
//
   if (!rc && (outbuff = iBP->getEmptyBuff()))
      {outbuff->blen = 0; outbuff->boff = Offset;
       oBP->putFullBuff(outbuff);
      } else {
       if (!rc) rc = -ENOBUFS;
       oBP->Abort(); iBP->Abort();
      }

// All done
//
   return rc;
}
  
/******************************************************************************/
/* Public:                      R e a d _ A l l                               */
/******************************************************************************/
  
int bbcp_File::Read_All(bbcp_BuffPool &inPool, int Vn)
{
    bbcp_FileChkSum *csP = 0;
    bbcp_BuffPool *outPool;
    bbcp_Buffer   *bP;
    pthread_t tid;
    int rc = 0;

// Get the size of the file
//
   if ((bytesLeft = FSp->getSize(IOB->FD())) < 0)
      {bbcp_Emsg("Read", static_cast<int>(-bytesLeft), "stat", iofn);
       inPool.Abort(); return 200;
      }

// Adjust bytes left based on where we will be reading from
//
   bytesLeft -= nextoffset;
   if (bytesLeft < 0)
      {bbcp_Emsg("Read", ESPIPE, "stat", iofn);
       inPool.Abort(); return 200;
      }

// If this is a real-time copy operation, start the rtcopy object
//
   if (rtCopy && !bbcp_RTCopy.Start(FSp, iofn, IOB->FD()))
      {inPool.Abort(); return 200;
      }

// Set up checksumming. We would prefer to do this in the calling thread but
// this is easier. One day we will generalize buffer piping.
//
   if (bbcp_Config.csOpts & bbcp_csVerIn)
      {csP = new bbcp_FileChkSum(&inPool, this, bbcp_Config.csType,
                                 bbcp_Config.csOpts & bbcp_csVerIO);
       if ((rc = bbcp_Thread_Start(bbcp_FileCSX, (void *)csP, &tid)) < 0)
           {bbcp_Emsg("File", rc, "starting file checksum thread.");
            delete csP;
            if (rtCopy) bbcp_RTCopy.Stop();
            inPool.Abort(); return 201;
           }
       outPool =  &csP->csPool;
      } else outPool = &inPool;

// Establish logging options
//
   if (bbcp_Config.Options & bbcp_LOGRD) IOB->Log("DISK", 0);

// Determine what kind of reading we will do here and do it
//
// cerr <<"BLOCKSIZE " <<blockSize <<endl;
   if (blockSize ) rc = Read_Direct(&inPool, outPool);
      else rc=(Vn > 1 ? Read_Vector(&inPool, outPool, Vn)
                      : Read_Normal(&inPool, outPool));

// Delete the real-time copy object if we have one to kill possible thread
//
   if (rtCopy) bbcp_RTCopy.Stop();

// Check if we ended because with an error
//
   IOB->Close();
   if (rc && rc != -ENOBUFS) bbcp_Emsg("Read", -rc, "reading", iofn);

// Prepare an empty buffer to shutdown the buffer pipeline. The offet indicates
// how much data should have been sent and received. A negative offset implies
// that we encountered an error.
//
   if (!(bP = inPool.getEmptyBuff())) return 255;
   bP->blen = 0;
   bP->boff = (rc ? -1 : nextoffset);

// Verify check sum if so wanted. Note that is link-level check summing is in
//
   if (csP)
      {csP->csPool.putFullBuff(bP);
       bbcp_Thread_Wait(tid);
       if (!rc && *bbcp_Config.csString)
          {char *csTxt, *csVal = csP->csObj->Final(&csTxt);
           if (memcmp(csVal, bbcp_Config.csValue, csP->csObj->csSize()))
              {bbcp_Fmsg("Read", iofn, "source checksum", bbcp_Config.csString,
                         "does not match", csTxt);
               rc = EILSEQ;
              } else {DEBUG(csP->csObj->Type() <<": " <<csTxt <<' ' <<iofn);}
          }
       delete csP;
      } else inPool.putFullBuff(bP);

// All done
//
   DEBUG("EOF offset=" <<nextoffset <<" rc=" <<rc <<" fn=" <<iofn);
   return rc;
}

/******************************************************************************/
/* Private:                  R e a d _ D i r e c t                            */
/******************************************************************************/

int bbcp_File::Read_Direct(bbcp_BuffPool *iBP, bbcp_BuffPool *oBP)
{
    bbcp_Buffer  *bP;
    ssize_t rlen;
    int Trunc = 0, rdsz = iBP->DataSize();

// Simply read one buffer at a time, that's the fastest way to do this
//
// cerr <<"DIRECT READ SIZE=" <<rdsz <<endl;
   while(bytesLeft > 0)
      {
      // Obtain buffer
      //
         if (!(bP = iBP->getEmptyBuff())) return -ENOBUFS;

      // Check if we are reading the last segment. Inflate it to the block
      // size value (we have room in the buffer) and indicate trunc() needed.
      // Otherwise, do a normal full-sized read.
      //
         if (bytesLeft < rdsz)
            {rdsz = bytesLeft + (blockSize - 1) & ~(blockSize - 1);
             if ((rlen = IOB->Read(bP->data, rdsz)) < bytesLeft)
                {iBP->putEmptyBuff(bP); break;}
             Trunc = 1; rlen = bytesLeft;
            } else {
             if ((rlen = IOB->Read(bP->data, rdsz)) <= 0)
                {iBP->putEmptyBuff(bP); break;}
            }

      // Queue a filled buffer for further processing
      //
         bytesLeft -= rlen;
         bP->boff   = nextoffset; nextoffset += rlen;
         bP->blen   = rlen;
         oBP->putFullBuff(bP);
      }
  
// All done
//
   if (bytesLeft) return (rlen ? static_cast<int>(rlen) : -ENODATA);
   return 0;
}

/******************************************************************************/
/* Private:                  R e a d _ N o r m a l                            */
/******************************************************************************/

int bbcp_File::Read_Normal(bbcp_BuffPool *iBP, bbcp_BuffPool *oBP)
{
    bbcp_Buffer  *bP;
    ssize_t rlen;
    int rdsz = iBP->DataSize();

// Simply read one buffer at a time, that's the fastest way to do this
//
// cerr <<"NORMAL READ SIZE=" <<rdsz <<endl;
   do {
      // Do real-time copy if need be
      //
         if (rtCopy && (bytesLeft=bbcp_RTCopy.Prep(nextoffset,rdsz,rlen)) <= 0)
            break;

      // Obtain buffer
      //
         if (!(bP = iBP->getEmptyBuff())) return -ENOBUFS;

      // Read data into the buffer
      //
         if ((rlen = IOB->Read(bP->data, rdsz)) <= 0)
            {iBP->putEmptyBuff(bP); break;}

      // Queue a filled buffer for further processing
      //
         bytesLeft -= rlen;
         bP->boff   =  nextoffset; nextoffset += rlen;
         bP->blen   = rlen;
         oBP->putFullBuff(bP);
      } while(rlen == rdsz && bytesLeft > 0);

// All done
//
   if (bytesLeft) return (rlen ? static_cast<int>(rlen) : -ENODATA);
   return 0;
}

/******************************************************************************/
/* Private:                  R e a d _ V e c t o r                            */
/******************************************************************************/

int bbcp_File::Read_Vector(bbcp_BuffPool *iBP, bbcp_BuffPool *oBP, int vNum)
{
    bbcp_Buffer *ioBuff[IOV_MAX];
    struct iovec ioVec [IOV_MAX];
    ssize_t blen, rlen;
    int rdsz = iBP->DataSize(), numV = (vNum > IOV_MAX ? IOV_MAX : vNum);
    int i, ivN, eof = 0;

// Simply read one buffer at a time, that's the fastest way to do this
//
// cerr <<"VECTOR READ SIZE=" <<rdsz <<" v=" <<numV <<endl;
   while(!eof)
      {
      // Obtain buffers
      //
         for (ivN = 0; ivN < numV && bytesLeft > 0; ivN++)
             {if (!(ioBuff[ivN] = iBP->getEmptyBuff())) return -ENOBUFS;
              ioVec[ivN].iov_base = (caddr_t)(ioBuff[ivN]->data);
              ioVec[ivN].iov_len  = (rdsz < bytesLeft ? rdsz : bytesLeft);
              bytesLeft -= rdsz;
             }

      // Read data into the buffer
      //
         if ((rlen = IOB->Read((const struct iovec *)&ioVec, ivN)) <= 0)
            {for (i = 0; i < ivN; i++) iBP->putEmptyBuff(ioBuff[i]);
             eof = !rlen; break;
            }

      // Queue a filled buffers for further processing
      //
         for (i = 0; i < ivN && rlen; i++)
             {blen = (rlen >= rdsz ? rdsz : rlen);
              ioBuff[i]->boff = nextoffset; nextoffset += blen;
              ioBuff[i]->blen = blen;       rlen       -= blen;
              oBP->putFullBuff(ioBuff[i]);
             }

      // Return any empty buffers
      //
         eof = i < ivN || bytesLeft <= 0;
         while(i < ivN) iBP->putEmptyBuff(ioBuff[i++]);
      }

// All done
//
   if (!eof) return (rlen ? static_cast<int>(rlen) : -ENODATA);
   return 0;
}

/******************************************************************************/
/* Private:                    v e r C h k S u m                              */
/******************************************************************************/
  
int bbcp_File::verChkSum(bbcp_FileChkSum *csP)
{

// If there was a transmission error, simply return the rc
//
   if (csP->csRC) return csP->csRC;

// Check if we should verify this against a known value
//
   if (bbcp_Config.csOpts & bbcp_csVerOut)
      {int   csLen         = csP->csObj->csSize();
       char *csTxt, *csVal = csP->csObj->Final(&csTxt);
       if (*bbcp_Config.csString)
          {if (memcmp(csVal, bbcp_Config.csValue, csP->csObj->csSize()))
              {bbcp_Fmsg("Write", iofn, "target checksum", bbcp_Config.csString,
                         "does not match", csTxt);
               return -EILSEQ;
              }
          } else {
           memcpy(bbcp_Config.csValue, csVal, csLen);
           strcpy(bbcp_Config.csString, csTxt);
          }
      }

// All done
//
   DEBUG(csP->csObj->Type() <<": '" <<bbcp_Config.csString <<"' " <<iofn);
   return 0;
}

/******************************************************************************/
/* Public:                     W r i t e _ A l l                              */
/******************************************************************************/
  
int bbcp_File::Write_All(bbcp_BuffPool &inPool, int nstrms)
{
    bbcp_FileChkSum *csP = 0;
    bbcp_BuffPool   *iBP;
    pthread_t tid;
    int rc, csType;

// If we have no IOB, then do a simple in-line passthru
//
   if (!IOB) return Passthru(&inPool, &bbcp_CPool, 0, nstrms);

// Establish checksum options as well as ordering options. Note that we do
// not support checksums in unordered streams and should have been prohibited.
//
   csType = bbcp_Config.csOpts & bbcp_csVerOut ? bbcp_Config.csType:bbcp_csNOP;
   if (bbcp_Config.csOpts & bbcp_csVerOut || bbcp_Config.Options & bbcp_ORDER)
      {csP = new bbcp_FileChkSum(&inPool, this, csType,
                                 bbcp_Config.csOpts & bbcp_csVerIO,nstrms);
       nstrms = 1;
       if ((rc = bbcp_Thread_Start(bbcp_FileCSY, (void *)csP, &tid)) < 0)
           {bbcp_Emsg("File", rc, "starting file checksum thread.");
            if (csP) delete csP;
            return 201;
           } else iBP = &(csP->csPool);
      }      else iBP = &inPool;

// Establish logging options
//
   if (bbcp_Config.Options & bbcp_LOGRD) IOB->Log(0, "DISK");

// Determine what kind of writing we will do here and do it
//
   rc = (blockSize ? Write_Direct(iBP, &inPool, nstrms)
                   : Write_Normal(iBP, &inPool, nstrms));

// Check if we ended because of an error or end of file
//
   if (rc < 0 && rc != -ENOBUFS) bbcp_Emsg("Write", -rc, "writing", iofn);

// Check if we should verify a checksum
//
   if (csP)
      {bbcp_Thread_Wait(tid);
       if (!rc && csP->csObj) rc = verChkSum(csP);
       delete csP;
      }

// If checksums are being printed, send off ours if we have it
//
   if (bbcp_Config.csOpts & bbcp_csPrint && *bbcp_Config.csString)
      cout <<"200 cks: " <<bbcp_Config.csString <<' ' <<iofn <<endl;

// Finish up
//
   if (IOB) IOB->Close();
   return rc;
}

/******************************************************************************/
/* Private:                 W r i t e _ D i r e c t                           */
/******************************************************************************/

int bbcp_File::Write_Direct(bbcp_BuffPool *iBP, bbcp_BuffPool *oBP, int nstrms)
{
   bbcp_Buffer *bP;
   ssize_t tlen, wlen = -1, sMask = blockSize - 1;

// Read all of the data until eof (note that we are single threaded)
//
// cerr <<"Write Direct! " <<blockSize <<endl;
   while(nstrms)
      {
      // Obtain a full buffer
      //
         if (!(bP = iBP->getFullBuff())) return -ENOBUFS;

      // Check if this is an eof marker
      //
         if (!(bP->blen)) {iBP->putEmptyBuff(bP); nstrms--; continue;}

      // Make sure we are writing a sector's worth of data
      //
         if (!(bP->blen & sMask)) {wlen = bP->blen; tlen = 0;}
            else {wlen = (bP->blen + blockSize) & ~sMask;
                  tlen =  bP->blen + bP->boff;
                 }

      // Do a normal write
      //
      // cerr <<"Dwrt " <<bP->blen <<'@' <<bP->boff <<endl;
         if ((wlen = IOB->Write(bP->data, wlen, bP->boff)) <= 0) break;

      // Truncate to the correct size if need be and recycle the buffer
      //
      // if (tlen) cerr <<"TRUNC to " <<tlen <<endl;
         if (tlen && ftruncate(IOB->FD(), tlen)) return -errno;
         oBP->putEmptyBuff(bP);
      }

// All done
//
   if (!nstrms) return  0;
   if (!wlen)   return -1;
   return static_cast<int>(wlen);
}

/******************************************************************************/
/* Private:                 W r i t e _ N o r m a l                           */
/******************************************************************************/

int bbcp_File::Write_Normal(bbcp_BuffPool *iBP, bbcp_BuffPool *oBP, int nstrms)
{
   bbcp_Buffer *bP;
   ssize_t wlen = -1;

// Read all of the data until eof (note that we are single threaded)
//
   while(nstrms)
      {
      // Obtain a full buffer
      //
         if (!(bP = iBP->getFullBuff())) return -ENOBUFS;

      // Check if this is an eof marker
      //
      // cerr <<nstrms <<" Write " <<bP->blen <<'@' <<bP->boff <<endl;
         if (!(bP->blen)) {iBP->putEmptyBuff(bP); nstrms--; continue;}

      // Do a normal write
      //
         if ((wlen=IOB->Write(bP->data, bP->blen, bP->boff)) <= 0) break;
         oBP->putEmptyBuff(bP);
      }

// All done
//
   if (!nstrms) return  0;
   if (!wlen)   return -1;
   return static_cast<int>(wlen);
}
