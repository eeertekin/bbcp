/******************************************************************************/
/*                                                                            */
/*                          b b c p _ T i m e r . C                           */
/*                                                                            */
/* (c) 2002 by the Board of Trustees of the Leland Stanford, Jr., University  */
/*      All Rights Reserved. See bbcp_Version.C for complete License Terms    *//*                            All Rights Reserved                             */
/*   Produced by Andrew Hanushevsky for Stanford University under contract    */
/*              DE-AC03-76-SFO0515 with the Department of Energy              */
/******************************************************************************/
 
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <poll.h>
#include "bbcp_Timer.h"

/******************************************************************************/
/*                                F o r m a t                                 */
/******************************************************************************/
  
int bbcp_Timer::Format(char *tbuff)
{
   struct tm *tmp;
   time_t Now = time(0);

// This routine is used by a single thread, so it's ok to be sloppy here
//
   tmp = localtime((const time_t *)&Now);
   return sprintf(tbuff, "%02d%02d%02d %02d:%02d:%02d ",
                  tmp->tm_year%100, tmp->tm_mon+1, tmp->tm_mday,
                  tmp->tm_hour,     tmp->tm_min,   tmp->tm_sec);
}

/******************************************************************************/
/*                                  S t o p                                   */
/******************************************************************************/

void bbcp_Timer::Stop()
{
   struct timeval tod;

// Get current time of day and compute running total
//
   gettimeofday(&tod, 0);
   TotalTime += (static_cast<long long>(tod.tv_sec) -
                 static_cast<long long>(StopWatch.tv_sec)) * 1000000LL
              + (static_cast<long long>(tod.tv_usec) -
                 static_cast<long long>(StopWatch.tv_usec));
/*
   TotalTime.tv_sec  += tod.tv_sec - StopWatch.tv_sec;
   TotalTime.tv_usec += tod.tv_usec- StopWatch.tv_usec;
   if (TotalTime.tv_usec > 1000000) {TotalTime.tv_sec++;
                                      TotalTime.tv_usec -= 1000000;
                                     }
*/

// Reset the stop watch so that we have a consistent timer
//
   StopWatch.tv_sec  = tod.tv_sec;
   StopWatch.tv_usec = tod.tv_usec;
}

/******************************************************************************/
/*                                  W a i t                                   */
/******************************************************************************/

void bbcp_Timer::Wait(int mills) 
{
   struct timespec naptime, pantime;

// Compute amount to wait
//
   naptime.tv_sec  =  mills/1000;
   naptime.tv_nsec = (mills%1000)*1000000;

// Wait for at least the specified number of milliseconds
//

   while(nanosleep(&naptime, &pantime) && EINTR == errno)
        {naptime.tv_sec  = pantime.tv_sec;
         naptime.tv_nsec = pantime.tv_nsec;
        }
}

/******************************************************************************/

void bbcp_Timer::Wait(long long mics)
{
   struct timespec naptime, pantime;

// Compute amount to wait
//
   naptime.tv_sec  =  mics/1000000;
   naptime.tv_nsec = (mics%1000000)*1000;

// Wait for at least the specified number of microseconds
//
   while(nanosleep(&naptime,&pantime) && EINTR == errno)
        {naptime.tv_sec  = pantime.tv_sec;
         naptime.tv_nsec = pantime.tv_nsec;
        }
}
