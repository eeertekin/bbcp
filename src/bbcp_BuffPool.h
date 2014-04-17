#ifndef __BBCP_BUFFPOOL_H__
#define __BBCP_BUFFPOOL_H__
/******************************************************************************/
/*                                                                            */
/*                       b b c p _ B u f f P o o l . h                        */
/*                                                                            */
/*(c) 2002-14 by the Board of Trustees of the Leland Stanford, Jr., University*//*      All Rights Reserved. See bbcp_Version.C for complete License Terms    *//*                            All Rights Reserved                             */
/*   Produced by Andrew Hanushevsky for Stanford University under contract    */
/*              DE-AC02-76-SFO0515 with the Department of Energy              */
/*                                                                            */
/* bbcp is free software: you can redistribute it and/or modify it under      */
/* the terms of the GNU Lesser General Public License as published by the     */
/* Free Software Foundation, either version 3 of the License, or (at your     */
/* option) any later version.                                                 */
/*                                                                            */
/* bbcp is distributed in the hope that it will be useful, but WITHOUT        */
/* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or      */
/* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public       */
/* License for more details.                                                  */
/*                                                                            */
/* You should have received a copy of the GNU Lesser General Public License   */
/* along with bbcp in a file called COPYING.LESSER (LGPL license) and file    */
/* COPYING (GPL license).  If not, see <http://www.gnu.org/licenses/>.        */
/*                                                                            */
/* The copyright holder's institutional names and contributor's names may not */
/* be used to endorse or promote products derived from this software without  */
/* specific prior written permission of the institution or contributor.       */
/******************************************************************************/

#include <stdlib.h>
#include <strings.h>
#include "bbcp_Pthread.h"

class bbcp_Buffer;
  
/******************************************************************************/
/*                   C l a s s   b b c p _ B u f f P o o l                    */
/******************************************************************************/
  
class bbcp_BuffPool
{
public:
void         Abort();

int          Allocate(int buffnum, int bsize, int sink, int ovhd=0);
int          Allocate(int buffnum);

int          BuffCount() {return numbuf;}

int          DataSize() {return datasz;}

int          Decode(bbcp_Buffer *bp);

void         Encode(bbcp_Buffer *bp, char xcmnd);

bbcp_Buffer *getEmptyBuff();

void         putEmptyBuff(bbcp_Buffer *buff);

bbcp_Buffer *getFullBuff();

void         putFullBuff(bbcp_Buffer *buff);

             bbcp_BuffPool(const char *id="net");
            ~bbcp_BuffPool();

private:

bbcp_Mutex EmptyPool;
bbcp_Mutex FullPool;
bbcp_Semaphore EmptyBuffs;
bbcp_Semaphore FullBuffs;

int         numbuf;
int         datasz;
int         buffsz;
int         RU486;
const char *pname;

bbcp_Buffer *next_full;
bbcp_Buffer *last_full;
bbcp_Buffer *last_empty;
};

/******************************************************************************/
/*                     C l a s s   b b c p _ H e a d c s                      */
/******************************************************************************/

struct bbcp_Headcs
      {union {long long lVal[2];
              int       iVal[2];
              short     sVal[2];
              char      cVal[2];
             };
      };

/******************************************************************************/
/*                     C l a s s   b b c p _ H e a d e r                      */
/******************************************************************************/

  
struct bbcp_Header
      {char cmnd;          // Command
       char hdcs;          // Header checksum
       char flgs;          // Flags
       char rsv1;          // Reserved
       char blen[4];       // int       buffer length
       char boff[8];       // Long long buffer offset
       char cksm[16];      // MD5 check sum (optional)
       bbcp_Header() {bzero(cksm, sizeof(cksm));}
      ~bbcp_Header() {}
       };

// Valid commands
//
#define BBCP_IO       0x00
#define BBCP_CLCKS    0x03
#define BBCP_CLOSE    0x04
#define BBCP_ABORT    0xFF

// Valid flags in flgs
//
#define BBCP_HDCS     0x80

/******************************************************************************/
/*                     C l a s s   b b c p _ B u f f e r                      */
/******************************************************************************/
  
class bbcp_Buffer
{
public:
bbcp_Buffer  *next;
long long     boff;
int           blen;
int           rsvd;
bbcp_Header   bHdr;
char         *data;

inline void   Recycle() {Owner->putEmptyBuff(this);}

              bbcp_Buffer(bbcp_BuffPool *oP, char *bP=0)
                         : next(0), boff(0),   blen(0), rsvd(0),
                           data(0), Owner(oP), Buff(bP) {}
             ~bbcp_Buffer() {if (Buff) free(Buff);}

private:
bbcp_BuffPool *Owner;
char          *Buff;
};
#endif
