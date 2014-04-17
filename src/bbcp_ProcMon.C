/******************************************************************************/
/*                                                                            */
/*                        b b c p _ P r o c M o n . C                         */
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
  
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include "bbcp_Config.h"
#include "bbcp_Debug.h"
#include "bbcp_Emsg.h"
#include "bbcp_Headers.h"
#include "bbcp_Node.h"
#include "bbcp_ProcMon.h"
  
/******************************************************************************/
/*           E x t e r n a l   I n t e r f a c e   R o u t i n e s            */
/******************************************************************************/
  
extern "C"
{
void *bbcp_MonProc(void *pp)
{
     bbcp_ProcMon *pmp = (bbcp_ProcMon *)pp;
     if (pmp->MonPool) pmp->Limit();
        else pmp->Monitor();
     return (void *)0;
}
}
  
/******************************************************************************/
/*                                 L i m i t                                  */
/******************************************************************************/
  
void bbcp_ProcMon::Limit()
{

// Wait until the time limit or the process is done
//
   CondMon.Wait(TimeLimit);

   if (!alldone)
      {MonPool->Abort();
       bbcp_Fmsg("Limit", "Time limit exceeded");
      }

// Post the semaphore to indicate we are done. This is a far safer way to
// synchronize exits than using pthread_join().
//
   monDone.Post();
}

/******************************************************************************/
/*                               M o n i t o r                                */
/******************************************************************************/
  
void bbcp_ProcMon::Monitor()
{
   static const int wtime = 5;   // 5 second periods
   static const int pitvl =36;   // 3 minute interval for pings
                int pleft =36;   // Expressed as number of 5 second intervals
   bbcp_Node    *pingDest;

// Allow immediate cancellations to avoid hang-ups
//
   bbcp_Thread_CanType(1);

// Run a loop until we are killed checking that process we want is still alive
//
   while(!alldone && CondMon.Wait(wtime))
        {if (kill(monPID, 0) && ESRCH == errno)
            {DEBUG("Process " <<monPID <<" has died");
             _exit(8);
            }
         if (pingNode && !pleft--)
            {pingMutex.Lock(); pingDest = pingNode; pingMutex.UnLock();
             if (pingDest) pingDest->Put("\n", 1);
             pleft = pitvl;
            }
        }

// Post the semaphore to indicate we are done. This is a far safer way to
// synchronize exits than using pthread_join().
//
   monDone.Post();
}

/******************************************************************************/
/*                                 S t a r t                                  */
/******************************************************************************/
  
void bbcp_ProcMon::Start(pid_t monit, bbcp_Node *Remote)
{
   int retc;

// If we are already monitoring, issue a stop
//
   if (mytid) Stop();

// Preset all values
//
   alldone = 0;
   monPID = (monit ? monit : getppid());
   pingMutex.Lock();
   pingNode = Remote;
   pingMutex.UnLock();

// Run a thread to start the monitor
//
   if (retc = bbcp_Thread_Run(bbcp_MonProc, (void *)this, &mytid))
      {DEBUG("Error " <<retc <<" starting MonProc thread.");}
      else {DEBUG("Thread " <<mytid <<" monitoring process " <<monPID);}
   return;
}
  
void bbcp_ProcMon::Start(int seclim, bbcp_BuffPool *buffpool)
{
   int retc;

// If we are already monitoring, issue a stop
//
   if (mytid) Stop();

// Preset all values
//
   alldone   = 0;
   TimeLimit = seclim;
   MonPool   = buffpool;

// Run a thread to start the monitor
//
   if (retc = bbcp_Thread_Run(bbcp_MonProc, (void *)this, &mytid))
      {DEBUG("Error " <<retc <<" starting buffpool monitor thread.");}
      else {DEBUG("Thread " <<mytid <<" monitoring buffpool.");}
   return;
}
/******************************************************************************/
/*                                  S t o p                                   */
/******************************************************************************/
  
void bbcp_ProcMon::Stop()
{

// Stop any pings
//
   pingMutex.Lock();
   pingNode = 0;
   pingMutex.UnLock();

// Simply issue a kill to the thread running the progress monitor
//
   alldone = 1;
   if (mytid) 
      {DEBUG("Process monitor " <<mytid <<" stopped.");
       CondMon.Signal();
       monDone.Wait();
       mytid = 0;
      }
   return;
}
 
