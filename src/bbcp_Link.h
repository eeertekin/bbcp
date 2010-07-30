#ifndef __BBCP_LINK_H__
#define __BBCP_LINK_H__
/******************************************************************************/
/*                                                                            */
/*                           b b c p _ L i n k . h                            */
/*                                                                            */
/* (c) 2002 by the Board of Trustees of the Leland Stanford, Jr., University  */
/*      All Rights Reserved. See bbcp_Version.C for complete License Terms    *//*                            All Rights Reserved                             */
/*   Produced by Andrew Hanushevsky for Stanford University under contract    */
/*              DE-AC03-76-SFO0515 with the Department of Energy              */
/******************************************************************************/

#include <errno.h>
#include <sys/types.h>
#include <fcntl.h>
#include "bbcp_BuffPool.h"
#include "bbcp_ChkSum.h"
#include "bbcp_File.h"
#include "bbcp_IO.h"
#include "bbcp_Pthread.h"

// The bbcp_Link class defines the operations on a network link.
//
class bbcp_Link
{
public:

int          Close() {if (csObj) delete csObj; return IOB.Close();}

int          FD() {return IOB.FD();}

char        *LinkName() {return Lname;}

void         setBuddy(bbcp_Link *mybuddy) {Buddy = mybuddy;}

int          Buff2Net();

int          Net2Buff();

             bbcp_Link(int newfd=-1, const char *newfn="");
            ~bbcp_Link() {Close(); if (Lname) free(Lname);}

int          LinkNum;

private:

bbcp_Link      *Buddy;
bbcp_Semaphore  Rendezvous;
bbcp_IO         IOB;
bbcp_ChkSum    *csObj;
char           *Lname;

int        ClockData();
int        Control_In( bbcp_Buffer *inbuff);
int        Control_Out(bbcp_Buffer *outbuff);
};
#endif
