#ifndef __BBCP_TIMER_H__
#define __BBCP_TIMER_H__
/******************************************************************************/
/*                                                                            */
/*                          b b c p _ T i m e r . h                           */
/*                                                                            */
/* (c) 2002 by the Board of Trustees of the Leland Stanford, Jr., University  */
/*      All Rights Reserved. See bbcp_Version.C for complete License Terms    *//*                            All Rights Reserved                             */
/*   Produced by Andrew Hanushevsky for Stanford University under contract    */
/*              DE-AC03-76-SFO0515 with the Department of Energy              */
/******************************************************************************/

#include <time.h>
#include <sys/time.h>

// This include file describes the oo elapsed time interval interface.
//

class bbcp_Timer {

public:

       int  Format(char *tbuff);

inline void Report(unsigned int  &Total_Time) // Report in milliseconds
                   {Total_Time = static_cast<unsigned int>(TotalTime/1000);}

inline void Report(long     long &Total_Time) // Report in microseconds
                   {Total_Time = TotalTime;}

inline void Report(double        &Total_Time) // Report in millisecond
                   {Total_Time = static_cast<double>(TotalTime)/1000.0;}

inline void Reset()   {TotalTime = 0; gettimeofday(&StopWatch, 0);}

inline void Start()   {gettimeofday(&StopWatch, 0);}

       void Stop();

       void Wait(int       milliseconds);

       void Wait(long long microseconds);

       bbcp_Timer() {Reset();}
      ~bbcp_Timer() {}

private:
      long long      TotalTime;     // Total time so far in microseconds
      struct timeval StopWatch;     // Current running clock
};
#endif
