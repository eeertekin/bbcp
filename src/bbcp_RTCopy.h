#ifndef __BBCP_RTCopy_H__
#define __BBCP_RTCopy_H__
/******************************************************************************/
/*                                                                            */
/*                         b b c p _ R T C o p y . h                          */
/*                                                                            */
/*(c) 2010-14 by the Board of Trustees of the Leland Stanford, Jr., University*//*      All Rights Reserved. See bbcp_Version.C for complete License Terms    *//*                            All Rights Reserved                             */
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

#include <pthread.h>

class bbcp_FileSystem;
class bbcp_Semaphore;
  
class bbcp_RTCopy
{
public:

void     *Lock(bbcp_Semaphore *semP);

long long Prep(long long rdsk, int rdsz, ssize_t &rlen);

int       Start(bbcp_FileSystem *fsp, const char *iofn, int iofd);

void      Stop();

          bbcp_RTCopy() : FSp(0), xLim(0), Left(0), lkFN(0), lkFD(-1),
                          Blok(0), Grow(0), ioFD(-1), Tid(0) {}
         ~bbcp_RTCopy() {} // Never gets deleted

private:

bbcp_FileSystem *FSp;
long long        xLim;
long long        Left;
const char      *lkFN;
int              lkFD;
int              Blok;
int              Grow;
int              ioFD;
pthread_t        Tid;
};
#endif
