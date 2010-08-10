/******************************************************************************/
/*                                                                            */
/*                            b b c p _ Z C X . C                             */
/*                                                                            */
/* (c) 2002 by the Board of Trustees of the Leland Stanford, Jr., University  */
/*      All Rights Reserved. See bbcp_Version.C for complete License Terms    *//*                            All Rights Reserved                             */
/*   Produced by Andrew Hanushevsky for Stanford University under contract    */
/*              DE-AC03-76-SFO0515 with the Department of Energy              */
/******************************************************************************/
  
extern "C"
{
#include <zlib.h>
}
#include "bbcp_BuffPool.h"
#include "bbcp_Emsg.h"
#include "bbcp_Headers.h"
#include "bbcp_NetLogger.h"
#include "bbcp_Platform.h"
#include "bbcp_ZCX.h"

/******************************************************************************/
/*                        G l o b a l   O b j e c t s                         */
/******************************************************************************/
  
extern bbcp_NetLogger bbcp_NetLog;

/******************************************************************************/
/*                     L o c a l   D e f i n i t i o n s                      */
/******************************************************************************/

#define LOGSTART \
        if (LogIDbeg) {xfrseek = (Clvl ? inbytes : outbytes); LOGIT(LogIDbeg);}

#define LOGEND \
        if (LogIDend) LOGIT(LogIDend)

#define LOGIT(a) \
        bbcp_NetLog.Emit(a, (char *)"", "BBCP.FD=%d BBCP.SK=%lld",iofd,xfrseek)
  
/******************************************************************************/
/*                           C o n s t r u c t o r                            */
/******************************************************************************/
  
bbcp_ZCX::bbcp_ZCX(bbcp_BuffPool *ib, bbcp_BuffPool *rb, bbcp_BuffPool *ob,
          int clvl, int xfd, int logit)
{  
    Ibuff  = ib;
    Rbuff  = rb;
    Obuff  = ob;
    Clvl   = clvl;
    iofd   = xfd;
    cbytes = 0; 
    TID    = 0;

    if (!logit) LogIDbeg = LogIDend = 0;
       else    {LogIDbeg = (clvl ? (char *)"STARTCMP" : (char *)"STARTEXP");
                LogIDend = (clvl ? (char *)"ENDCMP"   : (char *)"ENDEXP");
               }
}

/******************************************************************************/
/*                               P r o c e s s                                */
/******************************************************************************/
  
int bbcp_ZCX::Process()
{
     z_stream ZStream;
     uInt outsz = (uInt)Obuff->DataSize();
     bbcp_Buffer *ibp, *obp;
     int rc = 0, ZFlag = 0;
     long long inbytes = 0, outbytes = 0, xfrseek = 0;

// Initialize the compression stream
//
   ZStream.zalloc    = Z_NULL;
   ZStream.zfree     = Z_NULL;
   ZStream.opaque    = Z_NULL;
   ZStream.msg       = 0;
   ZStream.data_type = Z_BINARY;

   if (Clvl)rc = deflateInit(&ZStream, Clvl);
      else rc = inflateInit(&ZStream);
   if (rc != Z_OK) return Zfailure(rc, "initializing", ZStream.msg);

// Get the initial inbuff and outbuff
//
   if (!(obp = Obuff->getEmptyBuff())) return ENOBUFS;
   ZStream.next_out  = (Bytef *)obp->data;
   ZStream.avail_out = outsz;

   if (!(ibp = Ibuff->getFullBuff())) return ENOBUFS;
   ZStream.next_in  = (Bytef *)ibp->data;
   if (!(ZStream.avail_in = (uInt)ibp->blen)) rc = Z_STREAM_END;
   inbytes = ibp->blen;

// Perform compression/expansion
//
   while(rc != Z_STREAM_END)
        {LOGSTART;
         if (Clvl) rc = deflate(&ZStream, ZFlag);
            else   rc = inflate(&ZStream, ZFlag);
         LOGEND;
         if (rc != Z_OK && rc != Z_STREAM_END)
            return Zfailure(rc, "performing", ZStream.msg);

         if (!ZStream.avail_in && !ZFlag)
            {Rbuff->putEmptyBuff(ibp);
             if (!(ibp = Ibuff->getFullBuff())) return ENOBUFS;
             ZStream.next_in  = (Bytef *)ibp->data;
             if (!(ZStream.avail_in = (uInt)ibp->blen)) ZFlag = Z_FINISH;
                else inbytes += ibp->blen;
            }

         if (!ZStream.avail_out)
            {obp->blen = outsz;
             obp->boff = outbytes; outbytes += outsz;
             Obuff->putFullBuff(obp);
             if (!(obp = Obuff->getEmptyBuff())) return ENOBUFS;
             ZStream.next_out  = (Bytef *)obp->data;
             ZStream.avail_out = outsz;
            }
         cbytes = Clvl ? outbytes : inbytes - ZStream.avail_in;
        }

// If we have gotten here then all went well so far flush output
//
   if (obp->blen = outsz - ZStream.avail_out)
      {obp->boff = outbytes; outbytes += obp->blen;
       Obuff->putFullBuff(obp);
       if (!(obp = Obuff->getEmptyBuff())) return ENOBUFS;
      }

// Complete compression/expansion processing
//
   if (Clvl)
      {if ((rc = deflateEnd(&ZStream)) != Z_OK)
          return Zfailure(rc, "ending", ZStream.msg);
      } else {
       if ((rc = inflateEnd(&ZStream)) != Z_OK)
          return Zfailure(rc, "ending", ZStream.msg);
      }

// Record the total number of compressed bytes we've processed
//
   cbytes = Clvl ? outbytes : inbytes;

// Free up buffers
//
   Rbuff->putEmptyBuff(ibp);
   obp->blen = 0;
   obp->boff = outbytes;
   Obuff->putFullBuff(obp);
   return 0;
}

/******************************************************************************/
/*                              Z f a i l u r e                               */
/******************************************************************************/
  
int bbcp_ZCX::Zfailure(int zerr, const char *oper, char *Zmsg)
{
    char *txt2 = (Clvl ? (char *)"compression" : (char *)"expansion");
    switch(zerr)
          {case Z_ERRNO:         zerr = errno;      break;
           case Z_STREAM_ERROR:  zerr = ENOSR;      break;
           case Z_DATA_ERROR:    zerr = EOVERFLOW;  break;
           case Z_MEM_ERROR:     zerr = ENOMEM;     break;
           case Z_BUF_ERROR:     zerr = EBADSLT;    break;
           case Z_VERSION_ERROR: zerr = EPROTO;     break;
           default:              zerr = (zerr < 0 ? 10000-zerr : 20000+zerr);
          }

    Ibuff->Abort();
    if (Ibuff != Rbuff) Rbuff->Abort();
    Obuff->Abort();

    if (Zmsg) return bbcp_Fmsg("Zlib", Zmsg, oper, txt2);
    return bbcp_Emsg("Zlib", zerr, oper,  txt2, (Zmsg ? Zmsg : ""));
}
