#ifndef __BBCP_PROCMON_H__
#define __BBCP_PROCMON_H__
/******************************************************************************/
/*                                                                            */
/*                        b b c p _ P r o c M o n . h                         */
/*                                                                            */
/* (c) 2002 by the Board of Trustees of the Leland Stanford, Jr., University  */
/*      All Rights Reserved. See bbcp_Version.C for complete License Terms    *//*                            All Rights Reserved                             */
/*   Produced by Andrew Hanushevsky for Stanford University under contract    */
/*              DE-AC03-76-SFO0515 with the Department of Energy              */
/******************************************************************************/
  
#include "bbcp_BuffPool.h"
#include "bbcp_File.h"
#include "bbcp_Pthread.h"

class bbcp_ProcMon
{
public:

void  Limit();

void  Monitor();

void  Start(pid_t monit=0);
void  Start(int seclim, bbcp_BuffPool *);

void  Stop();

      bbcp_ProcMon() : MonPool(0),mytid(0),alldone(0),TimeLimit(0),monDone(0)
                     {}
     ~bbcp_ProcMon() {Stop();}

bbcp_BuffPool *MonPool;

private:

pthread_t      mytid;
int            alldone;
int            TimeLimit;
pid_t          monPID;
bbcp_CondVar   CondMon;
bbcp_Semaphore monDone;
};
#endif
