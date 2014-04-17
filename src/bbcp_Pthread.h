#ifndef __BBCP_PTHREAD__
#define __BBCP_PTHREAD__
/******************************************************************************/
/*                                                                            */
/*                        b b c p _ P t h r e a d . h                         */
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
  
#include <errno.h>
#include <pthread.h>
#include <time.h>
#include <semaphore.h>

class bbcp_CondVar
{
public:

inline void  Lock()           {pthread_mutex_lock(&cmut);}

inline void  Signal()         {if (relMutex) pthread_mutex_lock(&cmut);
                               pthread_cond_signal(&cvar);
                               if (relMutex) pthread_mutex_unlock(&cmut);
                              }

inline void  Broadcast()      {if (relMutex) pthread_mutex_lock(&cmut);
                               pthread_cond_broadcast(&cvar);
                               if (relMutex) pthread_mutex_unlock(&cmut);
                              }

inline void  UnLock()         {pthread_mutex_unlock(&cmut);}

       int   Wait();
       int   Wait(int sec);
       int   WaitMS(int msec);

      bbcp_CondVar(int   relm=1 // 0->Caller will handle lock/unlock
                  ) {pthread_cond_init(&cvar, NULL);
                     pthread_mutex_init(&cmut, NULL);
                     relMutex = relm;
                    }
     ~bbcp_CondVar() {pthread_cond_destroy(&cvar);
                      pthread_mutex_destroy(&cmut);
                     }
private:

pthread_cond_t  cvar;
pthread_mutex_t cmut;
int             relMutex;
};

class bbcp_Mutex
{
public:

inline int CondLock()
       {if (pthread_mutex_trylock( &cs )) return 0;
        return 1;
       }

inline void   Lock() {pthread_mutex_lock(&cs);}

inline void UnLock() {pthread_mutex_unlock(&cs);}

        bbcp_Mutex() {pthread_mutex_init(&cs, NULL);}
       ~bbcp_Mutex() {pthread_mutex_destroy(&cs);}

private:

pthread_mutex_t cs;
};

class bbcp_MutexMon
{
public:

void    UnLock() {if (monMutex) {monMutex->UnLock(); monMutex = 0;}}

        bbcp_MutexMon(bbcp_Mutex *theMutex) : monMutex( theMutex)
                     {theMutex->Lock();}
        bbcp_MutexMon(bbcp_Mutex &theMutex) : monMutex(&theMutex)
                     {theMutex. Lock();}
       ~bbcp_MutexMon() {if (monMutex) {monMutex->UnLock(); monMutex = 0;}}

private:
bbcp_Mutex *monMutex;
};

#if defined(MACOS) || defined(AIX)
class bbcp_Semaphore
{
public:

       int  CondWait();

       void Post();

       void Wait();

  bbcp_Semaphore(int semval=1) : semVar(0), semVal(semval), semWait(0) {}
 ~bbcp_Semaphore() {}

private:

bbcp_CondVar  semVar;
int           semVal;
int           semWait;
};

#else

class bbcp_Semaphore
{
public:

inline int  CondWait()
       {int rc;
        do {rc=sem_trywait( &h_semaphore );} while (rc && errno==EINTR);
           if (rc)
              {if (errno == EBUSY) return 0;
                  {throw "sem_CondWait() failed", errno;}
              }
        return 1;
       }

inline void Post() {if (sem_post(&h_semaphore))
                       {throw "sem_post() failed", errno;}
                   }

inline void Wait() {int rc;
                    do {rc=sem_wait(&h_semaphore);} while (rc && errno==EINTR);
                    if (rc) {throw "sem_wait() failed", errno;}
                   }

  bbcp_Semaphore(int semval=1) {if (sem_init(&h_semaphore, 0, semval))
                                   {throw "sem_init() failed", errno;}
                               }
 ~bbcp_Semaphore() {if (sem_destroy(&h_semaphore))
                       {throw "sem_destroy() failed", errno;}
                   }

private:

sem_t h_semaphore;
};
#endif

extern "C"
{
int   bbcp_Thread_Cancel(pthread_t tid);
int   bbcp_Thread_CanType(int Async);
int   bbcp_Thread_Detach(pthread_t tid);
void  bbcp_Thread_MT(int mtlvl);
int   bbcp_Thread_Run(  void *(*proc)(void *), void *arg, pthread_t *tid);
int   bbcp_Thread_Signal(pthread_t tid, int snum);
int   bbcp_Thread_Start(void *(*proc)(void *), void *arg, pthread_t *tid);
void *bbcp_Thread_Wait(pthread_t tid);
}
#endif
