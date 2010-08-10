/******************************************************************************/
/*                                                                            */
/*                           b b c p _ L i n k . C                            */
/*                                                                            */
/* (c) 2002 by the Board of Trustees of the Leland Stanford, Jr., University  */
/*      All Rights Reserved. See bbcp_Version.C for complete License Terms    *//*                            All Rights Reserved                             */
/*   Produced by Andrew Hanushevsky for Stanford University under contract    */
/*              DE-AC03-76-SFO0515 with the Department of Energy              */
/******************************************************************************/
  
#include <sys/types.h>
#include <stdlib.h>
#include <strings.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include "bbcp_BuffPool.h"
#include "bbcp_Config.h"
#include "bbcp_Debug.h"
#include "bbcp_Emsg.h"
#include "bbcp_Link.h"
#include "bbcp_Network.h"
#include "bbcp_Platform.h"
#include "bbcp_Timer.h"

/******************************************************************************/
/*                     E x t e r n a l   P o i n t e r s                      */
/******************************************************************************/
  
extern bbcp_BuffPool bbcp_BPool;
  
extern bbcp_Config   bbcp_Config;

extern bbcp_Network  bbcp_Net;

/******************************************************************************/
/*                           C o n s t r u c t o r                            */
/******************************************************************************/

bbcp_Link::bbcp_Link(int fd, const char *lname)
          : LinkNum(0), Buddy(0),  Rendezvous(0), IOB(fd), Lname(strdup(lname))
{
   csObj = (bbcp_Config.csOpts & bbcp_csLink
         ? bbcp_ChkSum::Alloc(bbcp_Config.csType?bbcp_Config.csType:bbcp_csMD5)
         : 0);
}
  
/******************************************************************************/
/*                              B u f f 2 N e t                               */
/******************************************************************************/
  
int bbcp_Link::Buff2Net()
{
    bbcp_Buffer *outbuff;
    ssize_t wrsz, wlen = 0;
    const ssize_t hdrsz = (ssize_t)sizeof(bbcp_Header);
    int retc = 0, NotDone = 1, csLen = (csObj ? csObj->csSize() : 0);
    struct iovec iov[2] = {0, hdrsz, 0, 0};

// Check if this is a clocking link
//
   if (LinkNum < 0) return ClockData();

// Establish logging options
//
   if (bbcp_Config.Options & bbcp_LOGOUT) IOB.Log(0, "NET");

// Do this until an error of eof
//
   while(NotDone)
      {
      // Obtain a buffer
      //
         if (!(outbuff = bbcp_BPool.getFullBuff()))
            {NotDone = -1; retc = ENOBUFS; break;}

      // Compose the header and see if control operation required
      //
         if (outbuff->blen <= 0)
            {if ((NotDone = Control_Out(outbuff)) < 0) {retc = 255; break;}}
            else bbcp_BPool.Encode(outbuff, BBCP_IO);

      // Check if we should generate a checksum
      //
         if (csObj && outbuff->blen) memcpy(outbuff->bHdr.cksm,
            csObj->Calc(outbuff->data,outbuff->blen), csLen);

      // Write all of the data (header & data are sequential)
      //
         iov[0].iov_base = (char *)&outbuff->bHdr;
         iov[1].iov_base =  outbuff->data; iov[1].iov_len = outbuff->blen;
         wrsz = (ssize_t)outbuff->blen + hdrsz;
         if ((wlen = IOB.Write(iov, 2)) != wrsz) break;

      // Queue buffer for re-use
      //
         if (NotDone) bbcp_BPool.putEmptyBuff(outbuff);
            else      bbcp_BPool.putFullBuff(outbuff);
         outbuff = 0;

      // Tell our buddy that it's ok to continue then do a rendezvous
      //
         if (Buddy) {Buddy->Rendezvous.Post(); Rendezvous.Wait();}
      }

// Check how we ended this loop
//
   if (outbuff) bbcp_BPool.putEmptyBuff(outbuff);
   if (NotDone > 0 && !wlen)
      {bbcp_BPool.Abort();
       if (wlen < 0)  retc=bbcp_Emsg("Link",-wlen,"writing data for",Lname);
          else if (wlen > 0) {bbcp_Fmsg("Link","Data lost on link", Lname);
                              retc = 100;
                             }
      } else if (NotDone) bbcp_BPool.Abort();

// All done
//
   if (Buddy) Buddy->Rendezvous.Post();
   return (retc < 0 ? -retc : retc);
}

/******************************************************************************/
/*                              N e t 2 B u f f                               */
/******************************************************************************/
  
int bbcp_Link::Net2Buff()
{
    static const char *Etxt[] = {"",
                                 "Invalid header length",   // 1
                                 "Invalid buffer length",   // 2
                                 "Invalid data length",     // 3
                                 "Invalid checksum",        // 4
                                 "Invalid hdr checksum"     // 5
                                };
    enum err_type {NONE = 0, IHL = 1, IBL = 2, IDL = 3, ICS = 4, IHS = 5};
    err_type ecode = NONE;
    bbcp_Buffer *inbuff;
    ssize_t rdsz, rlen = 0;
    ssize_t hdrsz = (ssize_t)sizeof(bbcp_Header);
    int  maxrdsz  = bbcp_BPool.DataSize();
    int i, notdone = 1, csLen = (csObj ? csObj->csSize() : 0);

// Check if this is a clocking link
//
   if (LinkNum < 0) return ClockData();

// Establish logging options
//
   if (bbcp_Config.Options & bbcp_LOGIN) IOB.Log("NET", 0);

// Do this until an error of eof
//
   while(notdone)
      {
      // Obtain a buffer
      //
         if (!(inbuff = bbcp_BPool.getEmptyBuff()))
            {rlen = ENOBUFS; notdone = 0; break;}

      // Read the header information into the header buffer
      //
         if ((rlen = IOB.Read((char *)&inbuff->bHdr, hdrsz)) != hdrsz)
            {if (rlen > 0) {ecode = IHL; rlen = EINVAL;} break;}

      // Decode the header and make sure it decoded correctly
      //
         if (!bbcp_BPool.Decode(inbuff)) {ecode = IHS; break;}

      // Make sure the read length does not overflow our buffer
      //
         if ((rdsz = inbuff->blen) > maxrdsz) {ecode = IBL; break;}

      // Read data into the buffer and do checksum if needed
      //
         if (rdsz)
            {if ((rlen = IOB.Read(inbuff->data, rdsz)) != rdsz)
                {if (rlen > 0) ecode = IDL; break;}
             if (csObj && memcmp(csObj->Calc(inbuff->data,inbuff->blen),
                                 inbuff->bHdr.cksm,csLen)) {ecode = ICS; break;}
            }

      // Check if this is a control operation or data operation
      //
         if (inbuff->bHdr.cmnd == (char)BBCP_IO)
            bbcp_BPool.putFullBuff(inbuff);
            else if ((notdone = Control_In(inbuff)) <= 0) break;

      // Tell our buddy that it's ok to continue then do a rendezvous
      //
         if (Buddy) {Buddy->Rendezvous.Post(); Rendezvous.Wait();}
      }

// If we ended the loop with an error, abort the buffer pool to force all
// threads dependent on the queue to abnormally terminate. Otherwise, post
// the buddy thread twice since that is the most that it may need to read.
//

   if (Buddy) {i = bbcp_Config.Streams;
               do {Buddy->Rendezvous.Post();} while(i--);
              }
   if (notdone)
      {bbcp_BPool.Abort();
      if (ecode != NONE)
         {bbcp_Fmsg("Net2Buff", Etxt[(int)ecode], "from", Lname);
          return 128;
         }
      if (rlen >=0) return EPIPE;
      bbcp_Emsg("Net2Buff", rlen, "reading data from", Lname);
      return -rlen;
      }
   return 0;
}

/******************************************************************************/
/*                       P r i v a t e   M e t h o d s                        */
/******************************************************************************/
/******************************************************************************/
/*                             C l o c k D a t a                              */
/******************************************************************************/

#define absVal(x) (x < 0 ? -x : x)

int bbcp_Link::ClockData()
{
   bbcp_Timer Ticker;
   int  TotBytes;
   unsigned int  Window, Etime;
     signed int  Old_Etime = 0, Diff, Old_Dev = 0;
   int Settle_tics = 100;

// Calculate number of bytes being sent per clock period
//
   TotBytes = bbcp_Config.Streams * bbcp_Config.Wsize;

// Calculate clock window in milliseconds for each data spurt
//
   Window = (unsigned int )((double)TotBytes/(double)bbcp_Config.Xrate*1000.0);
   DEBUG("Clock Window=" <<Window <<" TotBytes=" <<TotBytes);

// The clocking loop is relatively straightforward. Unlike fixed clocking
// pulses, we adjust the clocking rate to achieve no more than the desired
// performance for each spurt. If we were interested in average performance,
// we would simply clock the data out at a fixed rate.
//
   while(1)
        {Ticker.Reset();
         Rendezvous.Wait();
         Ticker.Stop();
         Ticker.Report(Etime);
         if (Settle_tics) Settle_tics--;
//          else {Diff = (signed int )Etime - (signed int )Old_Etime;
//                Old_Dev = Old_Dev + (absVal(Diff)-Old_Dev)>>2;
//                bbcp_Config.Jitter = Old_Dev;
//               }
         Old_Etime = Etime;
         if (Etime < Window) Ticker.Wait(Window-Etime);
         if (Buddy) Buddy->Rendezvous.Post();
        }

// All done
//
   return 0;
}
  
/******************************************************************************/
/*                            C o n t r o l _ I n                             */
/******************************************************************************/

int bbcp_Link::Control_In(bbcp_Buffer *bp)
{
    int  newsz;
    bbcp_Header *hp = &bp->bHdr;

// Check if this is a vanilla close request.
//
   if (hp->cmnd == (char)BBCP_CLOSE)
      {DEBUG("Close request received on link " <<LinkNum);
       bp->blen = 0;
       bbcp_BPool.putFullBuff(bp);
       return 0;
      }

// Check if this is a checksum close request.
//
   if (hp->cmnd == (char)BBCP_CLCKS)
      {DEBUG("Checksum close request received on link " <<LinkNum);
       if (bbcp_Config.csOpts & bbcp_csSend)
          {DEBUG("Setting checksum from link " <<LinkNum);
           bbcp_Config.setCS(bp->bHdr.cksm);
           bp->blen = 0;
          }
       bbcp_BPool.putFullBuff(bp);
       return 0;
      }

// Check if this is an abort equest. If so, queue the message and terminate
//
   if (hp->cmnd == (char)BBCP_ABORT)
      {DEBUG("Abort request received on link " <<LinkNum);
       bbcp_BPool.putFullBuff(bp);
       return -1;
      }

// Unknown command here
//
   return bbcp_Emsg("Control_In", EINVAL, "invalid command code", Lname);
}

/******************************************************************************/
/*                           C o n t r o l _ O u t                            */
/******************************************************************************/
  
int bbcp_Link::Control_Out(bbcp_Buffer *bp)
{

// We are called because the buffer length is <= 0 which is the action code.
//

// EOF condition
//
   if ((!(bp->blen) && bp->boff >= 0) || bp->blen == -BBCP_CLOSE)
      {bbcp_BPool.Encode(bp, BBCP_CLOSE);
       DEBUG("Sending close request on link " <<LinkNum);
       return 0;
      }

// CLOSE with checksum
//
   if (bp->blen == -BBCP_CLCKS)
      {bp->blen = 0;
       bbcp_BPool.Encode(bp, BBCP_CLCKS);
       DEBUG("Sending close+chksum request on link " <<LinkNum);
       return 0;
      }

// ABORT
//
   bp->blen = 0;
   bbcp_BPool.Encode(bp, BBCP_ABORT);
   DEBUG("Sending abort request on link " <<LinkNum);
   return -1;
}
