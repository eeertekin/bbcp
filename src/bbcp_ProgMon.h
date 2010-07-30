#ifndef __BBCP_PROGMON_H__
#define __BBCP_PROGMON_H__
/******************************************************************************/
/*                                                                            */
/*                        b b c p _ P r o g M o n . h                         */
/*                                                                            */
/* (c) 2002 by the Board of Trustees of the Leland Stanford, Jr., University  */
/*      All Rights Reserved. See bbcp_Version.C for complete License Terms    *//*                            All Rights Reserved                             */
/*   Produced by Andrew Hanushevsky for Stanford University under contract    */
/*              DE-AC03-76-SFO0515 with the Department of Energy              */
/******************************************************************************/
  
#include "bbcp_Pthread.h"

class bbcp_File;
class bbcp_ZCX;

class bbcp_ProgMon
{
public:

void  Monitor();

void  Start(bbcp_File *fs_obj, bbcp_ZCX *cx_obj, int pint, long long xfrbytes);

void  Stop();

      bbcp_ProgMon() : FSp(0),CXp(0),wtime(0),Tbytes(0),mytid(0),monDone(0)
                     {}
     ~bbcp_ProgMon() {Stop();}

private:
bbcp_File     *FSp;
bbcp_ZCX      *CXp;
int            wtime;
long long      Tbytes;
pthread_t      mytid;
int            alldone;
bbcp_CondVar   CondMon;
bbcp_Semaphore monDone;
};
#endif
