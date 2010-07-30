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

       int   Format(char *tbuff);

       void  Report(unsigned int  &Total_Time);
       void  Report(long     long &Total_Time);
       void  Report(double        &Total_Time);

inline void  Reset()   {TotalTime.tv_sec = TotalTime.tv_usec = 0; Start();}

inline void  Start()   {gettimeofday(&StopWatch, 0);}

       void  Stop();

       void Wait(int millisconds);

       bbcp_Timer() {Reset();}
      ~bbcp_Timer() {}

private:
      struct timeval StopWatch;     // Current running clock
      struct timeval TotalTime;     // Total time so far
};
#endif
