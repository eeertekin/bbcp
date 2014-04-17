/******************************************************************************/
/*                                                                            */
/*                         b b c p _ R T C o p y . C                          */
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

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "bbcp_Config.h"
#include "bbcp_Emsg.h"
#include "bbcp_FileSystem.h"
#include "bbcp_Pthread.h"
#include "bbcp_RTCopy.h"

#ifndef EADV
#ifdef MACOS
#define EADV ENOATTR
#else
#define EADV EPERM
#endif
#endif

/******************************************************************************/
/*                         G l o b a l   V a l u e s                          */
/******************************************************************************/
  
extern bbcp_Config   bbcp_Config;

       bbcp_RTCopy   bbcp_RTCopy;

/******************************************************************************/
/*            E x t e r n a l   T h r e a d   I n t e r f a c e s             */
/******************************************************************************/
  
extern "C"
{
void *bbcp_RTCopyLK(void *pp)
{
   return bbcp_RTCopy.Lock((bbcp_Semaphore *)pp);
}
}

/******************************************************************************/
/*                                  L o c k                                   */
/******************************************************************************/
  
void *bbcp_RTCopy::Lock(bbcp_Semaphore *semP)
{
   struct flock fl;
   long long lfSize = 1;
   int rc;

// Initialize the lock structure
//
   fl.l_type = F_RDLCK;
   fl.l_whence = SEEK_SET;
   fl.l_start = 0;
   fl.l_len = 0;

// Indicate that we are all initialized
//
   if (semP) semP->Post();

// Obtain a shared lock on the file
//
   do {rc = fcntl(lkFD, F_SETLKW, &fl);} while(rc < 0 && errno == EINTR);

// Check how we ended here. Note that since we are permanent object, thread
// locking considerations do not apply here.
//
   if (rc && Grow)
      {rc = errno;
       bbcp_Emsg("RTCopy", rc, "locking", lkFN);
       Grow = -rc;
       return 0;
      }

// Check if we should verify this copy
//
   if (Grow && bbcp_Config.Options & bbcp_RTCVERC)
      {lfSize = FSp->getSize(lkFD);
       if (lfSize < 0)
          {rc = static_cast<int>(-lfSize);
           bbcp_Emsg("RTCopy", rc, "stating", lkFN);
           Grow = -rc;
           return 0;
          }
        if (!lfSize) {Grow = -EADV; return 0;}
      }

// Obtain the final size of the source file
//
   if ((xLim = FSp->getSize(ioFD)) < 0) Grow = static_cast<int>(xLim);
      else Grow = 0;

// All done
//
   return 0;
}

/******************************************************************************/
/* Public:                          P r e p                                   */
/******************************************************************************/
  
long long bbcp_RTCopy::Prep(long long rdsk, int rdsz, ssize_t &rlen)
{
   long long Hwm, cSize, pSize;
   int tLim, tVal;

// If we need not wait then return bytes left
//
   if (!Grow) return (xLim < rdsk ? 0 : xLim - rdsk);

// Check if we can satisfy this read with what we already have
//
   if (Grow > 0 && Left >= rdsz)
      {Left -= rdsz;
       return (size_t)0x7fffffff;
      }

// Check if we should adjust read size to blocking to number of streams
//
   if (Blok) rdsz *= Blok;

// Setup high watermark and the time limit, if any
//
   Hwm   = rdsk + rdsz;
   tVal  = bbcp_Config.rtLimit;
   pSize = -1;

// We continue waiting for enough data to fully satisfy this read or until
// writes to the file are finished (then we need not wait).
//
   while(Grow > 0)
        {if ((cSize = FSp->getSize(ioFD)) < 0) {rlen = -errno; return -1;}
         if (cSize >= Hwm)
            {Left = cSize - Hwm;
             return (size_t)0x7fffffff;
            }
         sleep(bbcp_Config.rtCheck);
         if (tVal)
            {if (cSize != pSize) {tLim = tVal; pSize = cSize;}
                 else if ((tLim -= bbcp_Config.rtCheck) <= 0)
                         {rlen = -ETIMEDOUT; return -1;}
            }
        }

// If we ended with an error, return no bytes left
//
   if (Grow < 0) {rlen = Grow; return -1;}

// If we no longer need to wait, return actual bytes left
//
   return (xLim < rdsk ? 0 : xLim - rdsk);
}

/******************************************************************************/
/* Public:                         S t a r t                                  */
/******************************************************************************/
  
int bbcp_RTCopy::Start(bbcp_FileSystem *fsp, const char *iofn, int iofd)
{
   bbcp_Semaphore xSem(0);
   int rc;

// Initialize the common variables
//
   ioFD = iofd;
   Grow = 1;
   Left = 0;
   Blok = (bbcp_Config.Options & bbcp_RTCBLOK ? bbcp_Config.Streams : 0);
   FSp  = fsp;

// Initialize variable dependent on how we will do locking
//
   if (bbcp_Config.rtLockf)
      {lkFN = bbcp_Config.rtLockf;
       lkFD = bbcp_Config.rtLockd;
      } else {
       lkFN = iofn;
       lkFD = dup(iofd);
      }

// Now start a thread that will try to obtain a shared lock
//
   if ((rc = bbcp_Thread_Run(bbcp_RTCopyLK, (void *)&xSem, &Tid)) < 0)
      {bbcp_Emsg("RTCopy", rc, "starting file r/t lock thread.");
       Grow = -rc;
       return 0;
      }

// Wait for the thread to set up
//
   xSem.Wait();
   return (Grow >= 0);
}

/******************************************************************************/
/*                                  S t o p                                   */
/******************************************************************************/
  
void bbcp_RTCopy::Stop()
{
// Now if we have a thread, kill it
//
   if (Grow > 0) {Grow = 0; pthread_cancel(Tid);}

// Now close our lock file
//
   if (lkFD >= 0) {close(lkFD); lkFD = -1;}
}
