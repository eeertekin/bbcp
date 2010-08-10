/******************************************************************************/
/*                                                                            */
/*                        b b c p _ P t h r e a d . C                         */
/*                                                                            */
/* (c) 2002 by the Board of Trustees of the Leland Stanford, Jr., University  */
/*      All Rights Reserved. See bbcp_Version.C for complete License Terms    *//*                            All Rights Reserved                             */
/*   Produced by Andrew Hanushevsky for Stanford University under contract    */
/*              DE-AC03-76-SFO0515 with the Department of Energy              */
/******************************************************************************/
  
#include <errno.h>
#include <signal.h>
#include <sys/time.h>
#include "bbcp_Debug.h"
#include "bbcp_Pthread.h"

/******************************************************************************/
/*                          b b c p _ C o n d V a r                           */
/******************************************************************************/
/******************************************************************************/
/*                                  W a i t                                   */
/******************************************************************************/
  
int bbcp_CondVar::Wait()
{
 int retc;

// Wait for the condition
//
   if (relMutex) Lock();
   retc = pthread_cond_wait(&cvar, &cmut);
   if (relMutex) UnLock();
   return retc;
}

/******************************************************************************/
  
int bbcp_CondVar::Wait(int sec)
{
 struct timespec tval;
 int retc;

// Get the mutex before calculating the time
//
   if (relMutex) Lock();

// Simply adjust the time in seconds
//
   tval.tv_sec  = time(0) + sec;
   tval.tv_nsec = 0;

// Wait for the condition or timeout
//
   do {retc = pthread_cond_timedwait(&cvar, &cmut, &tval);}
   while (retc && (retc != ETIMEDOUT));

   if (relMutex) UnLock();
   return retc == ETIMEDOUT;
}

/******************************************************************************/
/*                                W a i t M S                                 */
/******************************************************************************/
  
int bbcp_CondVar::WaitMS(int msec)
{
 int sec, retc, usec;
 struct timeval tnow;
 struct timespec tval;

// Adjust millseconds
//
   if (msec < 1000) sec = 0;
      else {sec = msec / 1000; msec = msec % 1000;}
   usec = msec * 1000;

// Get the mutex before getting the time
//
   if (relMutex) Lock();

// Get current time of day
//
   gettimeofday(&tnow, 0);

// Add the second and microseconds
//
   tval.tv_sec  = tnow.tv_sec  +  sec;
   tval.tv_nsec = tnow.tv_usec + usec;
   if (tval.tv_nsec > 1000000)
      {tval.tv_sec += tval.tv_nsec / 1000000;
       tval.tv_nsec = tval.tv_nsec % 1000000;
      }
   tval.tv_nsec *= 1000;


// Now wait for the condition or timeout
//
   do {retc = pthread_cond_timedwait(&cvar, &cmut, &tval);}
   while (retc && (retc != ETIMEDOUT));

   if (relMutex) UnLock();
   return retc == ETIMEDOUT;
}
 
/******************************************************************************/
/*                        b b c p _ S e m a p h o r e                         */
/******************************************************************************/
/******************************************************************************/
/*                              C o n d W a i t                               */
/******************************************************************************/
  
#if defined(MACOS) || defined(AIX)

int bbcp_Semaphore::CondWait()
{
   int rc;

// Get the semaphore only we can get it without waiting
//
   semVar.Lock();
   if ((rc = (semVal > 0) && !semWait)) semVal--;
   semVar.UnLock();
   return rc;
}

/******************************************************************************/
/*                                  P o s t                                   */
/******************************************************************************/
  
void bbcp_Semaphore::Post()
{
// Add one to the semaphore counter. If we the value is > 0 and there is a
// thread waiting for the sempagore, signal it to get the semaphore.
//
   semVar.Lock();
   semVal++;
   if (semVal && semWait) semVar.Signal();
   semVar.UnLock();
}

/******************************************************************************/
/*                                  W a i t                                   */
/******************************************************************************/
  
void bbcp_Semaphore::Wait()
{

// Wait until the sempahore value is positive. This will not be starvation
// free is the OS implements an unfair mutex;
//
   semVar.Lock();
   if (semVal < 1 || semWait)
      while(semVal < 1)
           {semWait++;
            semVar.Wait();
            semWait--;
           }

// Decrement the semaphore value and return
//
   semVal--;
   semVar.UnLock();
}
#endif
 
extern "C"
{
int bbcp_Thread_Cancel(pthread_t tid)
    {return pthread_cancel(tid);}

int bbcp_Thread_CanType(int Async)
    {int oldType;
     return pthread_setcanceltype((Async ? PTHREAD_CANCEL_ASYNCHRONOUS
                                         : PTHREAD_CANCEL_DEFERRED), &oldType);
    }

int bbcp_Thread_Detach(pthread_t tid)
    {return pthread_detach(tid);}

int  bbcp_Thread_Run(void *(*proc)(void *), void *arg, pthread_t *tid)
     {int retc = bbcp_Thread_Start(proc, arg, tid);
      if (!retc) pthread_detach(*tid);
      return retc;
     }

int bbcp_Thread_Signal(pthread_t tid, int snum)
    {
     return pthread_kill(tid, snum);
    }

int  bbcp_Thread_Start(void *(*proc)(void *), void *arg, pthread_t *tid)
     {int rc = pthread_create(tid, NULL, proc, arg);
      return (rc ? -rc : 0);
     }

void *bbcp_Thread_Wait(pthread_t tid)
    {long retc;
     void *tstat;
     if (retc = pthread_join(tid, (void **)&tstat)) tstat = (void *)retc;
     return tstat;
    }

void bbcp_Thread_MT(int mtval)
   {
#ifdef SUN
#ifndef SUN6 // Ranch version added this #ifndef SUN6
    int rc, oldmt = pthread_getconcurrency(), newmt = mtval;

    while(mtval)
         if ((rc = pthread_setconcurrency(mtval)) && rc == EAGAIN) mtval--;
            else break;

    DEBUG("MT set rc=" <<rc <<" omt=" <<oldmt <<" rmt=" <<newmt <<" nmt=" <<mtval);
#endif
#endif
   }
}
