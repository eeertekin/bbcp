#ifndef __BBCP_Network_H__
#define __BBCP_Network_H__
/******************************************************************************/
/*                                                                            */
/*                        b b c p _ N e t w o r k . h                         */
/*                                                                            */
/* (c) 2002 by the Board of Trustees of the Leland Stanford, Jr., University  */
/*      All Rights Reserved. See bbcp_Version.C for complete License Terms    *//*                            All Rights Reserved                             */
/*   Produced by Andrew Hanushevsky for Stanford University under contract    */
/*              DE-AC03-76-SFO0515 with the Department of Energy              */
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

int          MaxWSize(int isSink);

int          MaxSSize() {return maxSegment;}

int          getWBSize(int xfd, int srwant);

int          QoS(int newQoS=-1);

int          setWindow(int wsz, int noAT=0);

void         unBind() {if (iofd >= 0) {close(iofd); iofd = Portnum = -1;}}

             bbcp_Network();
            ~bbcp_Network() {unBind();}

private:

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

int   getHostAddr(char *hostname, struct sockaddr_in &InetAddr);
char *getHostName(struct sockaddr_in &addr);
int   Retry(int retries, int rwait);
void  setOpts(const char *who, int iofd);
int   setSegSz(const char *who, int iofd);
char *Peername(int snum);
};
#endif
