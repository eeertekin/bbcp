#ifndef __BBCP_PROCMON_H__
#define __BBCP_PROCMON_H__
/******************************************************************************/
/*                                                                            */
/*                        b b c p _ P r o c M o n . h                         */
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
  
#include "bbcp_BuffPool.h"
#include "bbcp_File.h"
#include "bbcp_Pthread.h"

class bbcp_Node;

class bbcp_ProcMon
{
public:

void  Limit();

void  Monitor();

void  Start(pid_t monit=0, bbcp_Node *Remote=0);
void  Start(int seclim, bbcp_BuffPool *);

void  Stop();

      bbcp_ProcMon() : pingNode(0), MonPool(0), mytid(0),
                       alldone(0), TimeLimit(0), monDone(0) {}

     ~bbcp_ProcMon() {Stop();}

bbcp_BuffPool *MonPool;

private:

pthread_t      mytid;
int            alldone;
int            TimeLimit;
pid_t          monPID;
bbcp_CondVar   CondMon;
bbcp_Semaphore monDone;
bbcp_Mutex     pingMutex;
bbcp_Node     *pingNode;
};
#endif
