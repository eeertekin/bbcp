#ifndef __bbcp_ZCX_H__
#define __bbcp_ZCX_H__
/******************************************************************************/
/*                                                                            */
/*                            b b c p _ Z C X . h                             */
/*                                                                            */
/* (c) 2002 by the Board of Trustees of the Leland Stanford, Jr., University  */
/*      All Rights Reserved. See bbcp_Version.C for complete License Terms    *//*                            All Rights Reserved                             */
/*   Produced by Andrew Hanushevsky for Stanford University under contract    */
/*              DE-AC03-76-SFO0515 with the Department of Energy              */
/******************************************************************************/
  
#include <stdlib.h>

class bbcp_BuffPool;

class bbcp_ZCX
{
public:

long long    Bytes() {return cbytes;}

int          Process(void);

pthread_t    TID;

      bbcp_ZCX(bbcp_BuffPool *ib, bbcp_BuffPool *rb, bbcp_BuffPool *ob,
               int clvl, int xfd, int logit);
     ~bbcp_ZCX() {}

private:

int Zfailure(int zerr, const char *oper, char *Zmsg);

bbcp_BuffPool   *Ibuff;
bbcp_BuffPool   *Rbuff;
bbcp_BuffPool   *Obuff;
long long        cbytes;
int              Clvl;
int              iofd;
char            *LogIDbeg;
char            *LogIDend;
};
#endif
