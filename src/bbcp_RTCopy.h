#ifndef __BBCP_RTCopy_H__
#define __BBCP_RTCopy_H__
/******************************************************************************/
/*                                                                            */
/*                         b b c p _ R T C o p y . h                          */
/*                                                                            */
/* (c) 2010 by the Board of Trustees of the Leland Stanford, Jr., University  */
/*      All Rights Reserved. See bbcp_Version.C for complete License Terms    *//*                            All Rights Reserved                             */
/*   Produced by Andrew Hanushevsky for Stanford University under contract    */
/*              DE-AC03-76-SFO0515 with the Department of Energy              */
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
