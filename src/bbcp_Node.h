#ifndef __BBCP_NODE_H__
#define __BBCP_NODE_H__
/******************************************************************************/
/*                                                                            */
/*                           b b c p _ N o d e . h                            */
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

#include <unistd.h>
#include <sys/types.h>
#include "bbcp_FileSpec.h"
#include "bbcp_File.h"
#include "bbcp_Link.h"
#include "bbcp_ProcMon.h"
#include "bbcp_Stream.h"

class  bbcp_Protocol;
class  bbcp_ZCX;
  
class bbcp_Node
{
public:

void   Detach() {NStream.Detach(); dlcount = 0; data_link[0] = 0;}

int    Drain() {return NStream.Drain();}

int    getBuffers(int isTrg, int isLZO=0);

char  *GetLine();

char  *GetToken() {return NStream.GetToken();}

int    LastError() {return NStream.LastError();}

char  *NodeName() {return nodename;}

int    Put(const char *data, int dlen)
          {char *dpnt[] = {(char *)data, 0}; int lpnt[] = {dlen, 0};
           return Put(dpnt, lpnt);
          }
int    Put(char *data[], int dlen[]);

int    Run(char *user, char *host, char *prog, char *parg);

int    RecvFile(bbcp_FileSpec *fspec, bbcp_Node *Remote);

int    SendFile(bbcp_FileSpec *fspec);

int    Start(bbcp_Protocol *protocol, int incomming)
            {if (incomming) return Incomming(protocol);
                 else       return  Outgoing(protocol);
            }

void   Stop(int Report=0);

int    Wait(bbcp_Node *other=0);

int       TotFiles;
long long TotBytes;

       bbcp_Node(bbcp_Link *netLink=0);
      ~bbcp_Node() {Stop(); if (nodename) free(nodename);}

private:

bbcp_ProcMon Parent_Monitor;
bbcp_Link   *data_link[BBCP_MAXSTREAMS];
bbcp_File   *comp1File, *comp2File;
bbcp_Stream NStream;
char       *nodename;
int         dlcount;
int         iocount;

void       chkWsz(int fd, int Final=0);
int        Incomming(bbcp_Protocol *protocol);
int         Outgoing(bbcp_Protocol *protocol);
int        Recover(const char *who);
void       Report(double, bbcp_FileSpec *, bbcp_File *, bbcp_ZCX *);
bbcp_ZCX  *setup_CX(int deflating, int iofd);
int        Usage(const char *who, char *buff, int blen);
};
#endif
