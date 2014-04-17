#ifndef __BBCP_Network_H__
#define __BBCP_Network_H__
/******************************************************************************/
/*                                                                            */
/*                        b b c p _ N e t w o r k . h                         */
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
#include <sys/socket.h>
#include "bbcp_Link.h"
#include "bbcp_Pthread.h"

// The bbcp_Network class defines a generic network where we can define common
// tcp/ip operations. This class is used by the global network object.
//
class bbcp_Network
{
public:

bbcp_Link   *Accept();

int          AutoTune() {return ATune;}

int          Bind(int minport, int maxport, int tries=1, int timeout=-1);

bbcp_Link   *Connect(char *host, int port, int retries=0, int rwait=1);

void         findPort(int &minport, int &maxport);

void         Flow(int isSRC);

char        *FullHostName(char *host=0, int asipaddr=0);

void         IPV4();

int          MaxWSize(int isSink);

int          MaxSSize() {return maxSegment;}

int          getWBSize(int xfd, int srwant);

int          QoS(int newQoS=-1);

static int   setPorts(int pnum1, int pnum2);

int          setWindow(int wsz, int noAT=0);

void         unBind() {if (iofd >= 0) {close(iofd); iofd = Portnum = -1;}}

             bbcp_Network();
            ~bbcp_Network() {unBind();}

private:

static int pFirst;
static int pLast;

int        accWait;
int        ATune;
int        iofd;
int        maxRcvBuff;
int        maxSndBuff;
int        maxSegment;
int        netQoS;
int        Portnum;
int        protID;
int        Sender;
int        Window;
int        WinSOP;

//int   getHostAddr(char *hostname, struct sockaddr_in &InetAddr);
//char *getHostName(struct sockaddr_in &addr);
int   Retry(int retries, int rwait);
void  setOpts(const char *who, int iofd);
int   setSegSz(const char *who, int iofd);
};
#endif
