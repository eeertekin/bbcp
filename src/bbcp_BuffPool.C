/******************************************************************************/
/*                                                                            */
/*                       b b c p _ B u f f P o o l . C                        */
/*                                                                            */
/* (c) 2002 by the Board of Trustees of the Leland Stanford, Jr., University  */
/*      All Rights Reserved. See bbcp_Version.C for complete License Terms    *//*                            All Rights Reserved                             */
/*   Produced by Andrew Hanushevsky for Stanford University under contract    */
/*              DE-AC03-76-SFO0515 with the Department of Energy              */
/******************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <sys/mman.h>

#if defined(MACOS) || defined(AIX)
#define memalign(pgsz,amt) valloc(amt)
#else
#include <malloc.h>
#endif

#include <sys/types.h>
#include <netinet/in.h>
#include "bbcp_Debug.h"
#include "bbcp_Emsg.h"
#include "bbcp_Headers.h"
#include "bbcp_Pthread.h"
#include "bbcp_BuffPool.h"

/******************************************************************************/
/*                         G l o b a l   O b j e c t                          */
/******************************************************************************/

bbcp_BuffPool bbcp_APool("disk");
bbcp_BuffPool bbcp_BPool;
bbcp_BuffPool bbcp_CPool("C");
  
/******************************************************************************/
/*                           c o n s t r u c t o r                            */
/******************************************************************************/
  
bbcp_BuffPool::bbcp_BuffPool(const char *id) : EmptyBuffs(0), FullBuffs(0)
{

// Do gross initialization
//
   last_empty = 0;
   last_full  = 0;
   next_full  = 0;
   datasz     = 0;
   buffsz     = 0;
   numbuf     = 0;
   RU486      = 0;
   pname      = id;
}

/******************************************************************************/
/*                            D e s t r u c t o r                             */
/******************************************************************************/
  
bbcp_BuffPool::~bbcp_BuffPool()
{
   bbcp_Buffer *currp;
   int i=16, j = 1;

// Free all of the buffers in the empty queue
//
   while(currp = last_empty)
        {last_empty = last_empty->next; delete currp;}
//cerr <<bbcp_Debug.Who <<"Bdestroy num " <<j++ <<" @ " <<hex <<(int)currp <<dec <<endl;

// Free all full buffers
//
   FullPool.Lock();
   while(currp = next_full)
        {next_full = next_full->next; delete currp;}
   FullPool.UnLock();
}
  
/******************************************************************************/
/*                                 A b o r t                                  */
/******************************************************************************/

void bbcp_BuffPool::Abort()
{
   int Kicking;

// Set the abort flag and then post the empty buffer pool. This will cause
// anyone looking for an empty buffer to abort. They in turn will cascade
// that information on to the next person.
//
   EmptyPool.Lock();
   Kicking = !RU486; RU486 = 1;
   EmptyBuffs.Post();
   EmptyPool.UnLock();

// Do the same for the full buffer queue
//
   FullBuffs.Post();
   if (Kicking) {DEBUG("Aborting the " <<pname <<" buffer pool.");}
}
  
/******************************************************************************/
/*                              A l l o c a t e                               */
/******************************************************************************/

#ifndef MAP_ALIGN
#define MAP_ALIGN 0
#endif
  
int bbcp_BuffPool::Allocate(int buffnum, int  bsize, int Sink, int ovhd)
{
   static const int mapOpts = MAP_SHARED|MAP_ANON|MAP_ALIGN;
   static const int mapProt = PROT_WRITE|PROT_READ;
   static int PageSize = sysconf(_SC_PAGESIZE);
   bbcp_Buffer *new_empty;
   int bnum = buffnum, dOvhd = (Sink ? PageSize : 0);
   size_t totlsz;
   char *buffaddr;

// This is an initialization call, setup to allocate correctly. Note that old
// versions of bbcp incorrectly substracted out the size of the header from
// the 'bsize'. So, we need to do the same and continue the error.
//
   EmptyPool.Lock();
   datasz = (bsize - sizeof(bbcp_Header))/PageSize*PageSize + dOvhd;
   buffsz = (bsize + ovhd + (PageSize-1))/PageSize*PageSize + dOvhd;
   totlsz = static_cast<size_t>(buffsz) * buffnum;

// Allocate shared memory
//
   buffaddr = (char *)mmap(0, totlsz, mapProt, mapOpts, -1,  0);
   if (buffaddr == MAP_FAILED)
      {DEBUG(strerror(errno) <<" doing mmap for " <<totlsz <<' ' <<pname <<" bytes.");
       return Allocate(buffnum);
      }

// Carve up the memory
//
   while(bnum--)
        {if (!(new_empty = new bbcp_Buffer(this)))
            {bbcp_Emsg("BuffPool", ENOMEM, "allocating buffers."); break;}
          new_empty->data = (char *)buffaddr;
         *new_empty->data = 0xff;
          new_empty->next = last_empty;
          last_empty = new_empty;
          buffaddr += buffsz;
          EmptyBuffs.Post();
         }
   numbuf += buffnum-(bnum+1);
   EmptyPool.UnLock();

// All done
//
   DEBUG("Allocated " <<buffnum-(bnum+1) <<' ' <<buffsz <<" (" <<datasz <<
         ") byte buffs in the " <<pname <<" pool.");
   return bnum+1;
}
  
/******************************************************************************/
  
int bbcp_BuffPool::Allocate(int buffnum)
{
   static int PageSize = sysconf(_SC_PAGESIZE);
   bbcp_Buffer *new_empty;
   int bnum = buffnum;
   char *baddr;

// This is an initialization call, setup to allocate correctly
//
   EmptyPool.Lock();

// Allocate the appropriate number of buffers aligned on page boundaries
//
   while (bnum--)
         {if (!(baddr = (char *)memalign(PageSize,buffsz))
          ||  !(new_empty = new bbcp_Buffer(this, baddr)))
             {bbcp_Emsg("BuffPool", ENOMEM, "allocating buffers."); break;}
          new_empty->data = baddr;
         *new_empty->data = 0xff;
          new_empty->next = last_empty;
          last_empty = new_empty;
          EmptyBuffs.Post();
         }
   numbuf += buffnum-(bnum+1);
   EmptyPool.UnLock();

// All done.
//
   DEBUG("Allocated " <<buffnum-(bnum+1) <<' ' <<buffsz <<" (" <<datasz <<
         ") byte buffs in the " <<pname <<" pool.");
   return bnum+1;
}
  
/******************************************************************************/
/*                          g e t E m p t y B u f f                           */
/******************************************************************************/
  
bbcp_Buffer *bbcp_BuffPool::getEmptyBuff()
{
   bbcp_Buffer *buffp = 0;

// Do this until we get an empty buffer
//
   while(!buffp & !RU486)
        {
      // Wait for an empty buffer
      //
         EmptyBuffs.Wait();

      // Get the buffer
      //
         EmptyPool.Lock();
         if (!RU486 && (buffp = last_empty)) last_empty = buffp->next;
         EmptyPool.UnLock();
       }

// Return the buffer
//
   if (RU486) EmptyBuffs.Post();
   return buffp;
}

/******************************************************************************/
/*                          p u t E m p t y B u f f                           */
/******************************************************************************/
  
void bbcp_BuffPool::putEmptyBuff(bbcp_Buffer *buffp)
{

// Obtain the pool lock and hang the buffer
//
   EmptyPool.Lock();
   buffp->next = last_empty;
   last_empty  = buffp;
   EmptyPool.UnLock();

// Signal availability of an empty buffer
//
   EmptyBuffs.Post();
}
 
/******************************************************************************/
/*                           g e t F u l l B u f f                            */
/******************************************************************************/
  
bbcp_Buffer *bbcp_BuffPool::getFullBuff()
{
   bbcp_Buffer *buffp = 0;

// Do this until we get a full buffer
//
   while(!buffp & !RU486)
        {
      // Wait for a full buffer
      //
         FullBuffs.Wait();

      // Get the buffer
      //
         FullPool.Lock();
         if (!RU486 && (buffp = next_full))
            if (!(next_full = buffp->next)) last_full = 0;
         FullPool.UnLock();
        }

// Return the buffer
//
   if (RU486) FullBuffs.Post();
   return buffp;
}
 
/******************************************************************************/
/*                           p u t F u l l B u f f                            */
/******************************************************************************/
  
void bbcp_BuffPool::putFullBuff(bbcp_Buffer *buffp)
{

// Obtain the pool lock and hang the buffer
//
   FullPool.Lock();
   if (last_full) last_full->next = buffp;
      else next_full = buffp;
   last_full = buffp;
   buffp->next = 0;
   FullPool.UnLock();

// Signal availability of a full buffer
//
   FullBuffs.Post();
}
 
/******************************************************************************/
/*                       H e a d e r   H a n d l i n g                        */
/******************************************************************************/
/******************************************************************************/
/*  S e r a i l i z a t i o n / D e s e r i a l i z a t i o n   M a c r o s   */
/******************************************************************************/

#define USBUFF int  USbuffL, USbuff[2]; unsigned short USbuffS; char *UScp

/******************************************************************************/
  
#define Ser(x,y) memcpy((void *)y,(const void *)&x,sizeof(x));

#define SerC(x,y)  *y = x;

#define SerS(x,y)  USbuffS = htons(x); Ser(USbuffS,y)

#define SerL(x,y)  USbuffL = htonl(x); Ser(USbuffL,y)

#define SerLL(x,y) USbuff[0] = x >>32; USbuff[1] = x & 0xffffffff; \
                   SerL(USbuff[0],y); UScp = y+4; SerL(USbuff[1],UScp)

/******************************************************************************/

#define Uns(x,y) memcpy((void *)&x,(const void *)y,sizeof(x));

#define UnsC(x,y)  x = *y; y++

#define UnsS(x,y)  Uns(USbuffS,y); x = htons(USbuffS)

#define UnsL(x,y)  Uns(USbuffL,y); x = ntohl(USbuffL)

#define UnsLL(x,y) UnsL(USbuff[0],y); UScp = y+4; UnsL(USbuff[1],UScp); \
                   x = ((unsigned long long)USbuff[0]) << 32 | \
                        (unsigned      int )USbuff[1]

/******************************************************************************/
/*                                D e c o d e                                 */
/******************************************************************************/
  
int  bbcp_BuffPool::Decode(bbcp_Buffer *bp)
{
     USBUFF;
     bbcp_Header *hp = &bp->bHdr;
     bbcp_Headcs *cP = (bbcp_Headcs *)hp, csData;
     char hdcs = 0;
     int n;

// If a checksum was calculate (backward compatability) verify it (it should
// result in a zero after xoring all the bytes in the header including chksum).
//
   if (hp->flgs & BBCP_HDCS)
      {n = (hp->cmnd == (char)BBCP_CLCKS ? 1 : 0);
       do {csData.lVal[0]  = cP->lVal[0] ^ cP->lVal[1];
           csData.iVal[0] ^= csData.iVal[1];
           csData.sVal[0] ^= csData.sVal[1];
           hdcs ^= (csData.cVal[0] ^ csData.cVal[1]);
           cP = (bbcp_Headcs *)hp->cksm;
          } while(n--);
       if (hdcs) return 0;
      }

// Extract out the length
//
   UnsL(bp->blen, hp->blen);

// Extract out the offset
//
   UnsLL(bp->boff, hp->boff);
   return 1;
}
 
/******************************************************************************/
/*                                E n c o d e                                 */
/******************************************************************************/
  
void bbcp_BuffPool::Encode(bbcp_Buffer *bp, char xcmnd)
{
     USBUFF;
     bbcp_Header *hp = &bp->bHdr;
     bbcp_Headcs *cP = (bbcp_Headcs *)hp, csData;
     char hdcs = 0;
     int n = (xcmnd == (char)BBCP_CLCKS ? 1 : 0);

// Set the command
//
   hp->cmnd = xcmnd;

// Set the length
//
   SerL(bp->blen, hp->blen);

// Set the offset
//
   SerLL(bp->boff, hp->boff);

// Calculate an xor checksum of the header (order independent). We iterate
// twice if we need to include the data checksum in the header checksum.
//
   hp->hdcs = 0;
   hp->flgs |= BBCP_HDCS;
   do {csData.lVal[0]  = cP->lVal[0] ^ cP->lVal[1];
       csData.iVal[0] ^= csData.iVal[1];
       csData.sVal[0] ^= csData.sVal[1];
       hdcs ^= (csData.cVal[0] ^ csData.cVal[1]);
       cP = (bbcp_Headcs *)hp->cksm;
      } while(n--);

// Set the header checksum in the header
//
   hp->hdcs  = hdcs;
}
