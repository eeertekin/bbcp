/******************************************************************************/
/*                                                                            */
/*                        b b c p _ N e t w o r k . C                         */
/*                                                                            */
/* (c) 2002 by the Board of Trustees of the Leland Stanford, Jr., University  */
/*      All Rights Reserved. See bbcp_Version.C for complete License Terms    *//*                            All Rights Reserved                             */
/*   Produced by Andrew Hanushevsky for Stanford University under contract    */
/*              DE-AC03-76-SFO0515 with the Department of Energy              */
/******************************************************************************/

// Contributions:

// 15-Oct-02: Corrections to findPort() contributed by Thomas Schenk @ ea.com
  
#include <errno.h>
#include <netdb.h>
#include <poll.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#ifdef SUN
#include <sys/stropts.h>
#include <inet/nd.h>
#endif
#include "bbcp_Debug.h"
#include "bbcp_Emsg.h"
#include "bbcp_Network.h"
#include "bbcp_Platform.h"

/******************************************************************************/
/*                      E x t e r n a l   O b j e c t s                       */
/******************************************************************************/

       bbcp_Network bbcp_Net;

/******************************************************************************/
/*                           C o n s t r u c t o r                            */
/******************************************************************************/
  
bbcp_Network::bbcp_Network()
{
   static const int BBCP_IPPROTO_TCP = 6;
   struct protoent *pp;
   char netBuff[256];
   int  netFD;

// Establish this network as having no known attributes
//
   iofd = Portnum = -1;
   maxSegment = 0;
   maxRcvBuff = 0;
   maxSndBuff = 0;
   ATune      = 0;
   netQoS     = 0;
   accWait    = 30000;
   Sender     = 0;
   Window     = 0;
   WinSOP     = 0;

// Get the value for the tcp protocol
//
   protID = ((pp = getprotobyname("tcp")) ? pp->p_proto : BBCP_IPPROTO_TCP);


#ifdef LINUX
// We are a Linux host, see if autotuning is enabled
//
   if ((netFD = open("/proc/sys/net/ipv4/tcp_moderate_rcvbuf", O_RDONLY)) > -1)
      {if (read(netFD, netBuff, sizeof(netBuff)) > 0 && *netBuff == '1')
          ATune = 1;
       close(netFD);
      }

// Get maximum receive buffer
//
   if ((netFD = open("/proc/sys/net/core/rmem_max", O_RDONLY)) > -1)
      {if (read(netFD, netBuff, sizeof(netBuff)) > 0)
          {maxRcvBuff = atoi(netBuff);
           maxRcvBuff = maxRcvBuff*2/8192*8192;
          }
       close(netFD);
      }

// Get maximum send buffer
//
   if ((netFD = open("/proc/sys/net/core/wmem_max", O_RDONLY)) > -1)
      {if (read(netFD, netBuff, sizeof(netBuff)) > 0)
          {maxSndBuff = atoi(netBuff);
           maxSndBuff = maxSndBuff*2/8192*8192;
          }
       close(netFD);
      }
#endif
#ifdef SUN
// For Solaris there is no autotuning but we can get the max snd/rcv netBuffer
//
   if ((netFD = open("/dev/tcp", O_RDWR)) > -1)
      {struct strioctl stri;
       memset(netBuff, 0, sizeof(netBuff));
       strcpy(netBuff, "tcp_max_buf");
       stri.ic_cmd = ND_GET;
       stri.ic_timout = 0;
       stri.ic_len = sizeof(netBuff);
       stri.ic_dp = netBuff;
       if (ioctl(netFD, I_STR, &stri) >= 0)
          {maxRcvBuff = atoi(netBuff); maxSndBuff = maxRcvBuff;}
       close(netFD);
      }
#endif

// One of these days we will do this for MacOS and FreeBSD as well
//
}

/******************************************************************************/
/*                                A c c e p t                                 */
/******************************************************************************/

bbcp_Link *bbcp_Network::Accept()
{
   char *newfn;
   int newfd, retc;
   bbcp_Link *newconn;
   struct pollfd sfd[1];

// Make sure we are bound to a port
//
   if (iofd < 0) {bbcp_Fmsg("Accept", "Network not bound to a port.");
                  return (bbcp_Link *)0;
                 }

// Setup up the poll structure to wait for new connections
//
   do {if (accWait >= 0)
          {sfd[0].fd     = iofd;
           sfd[0].events = POLLIN|POLLRDNORM|POLLRDBAND|POLLPRI|POLLHUP;
           sfd[0].revents = 0;
           do {retc = poll(sfd, 1, accWait);}
                      while(retc < 0 && (errno == EAGAIN || errno == EINTR));
           if (!sfd[0].revents)
              {char buff[16];
               sprintf(buff,"%d", Portnum);
               bbcp_Fmsg("Accept", "Accept timed out on port", buff);
               return (bbcp_Link *)0;
              }
          }

       do {newfd = accept(iofd, (struct sockaddr *)0, (size_t)0);}
          while(newfd < 0 && errno == EINTR);

       if (newfd < 0)
          {char buff[16];
           sprintf(buff,"%d", Portnum);
           bbcp_Emsg("Accept", errno, "performing accept on port", buff);
           continue;
          }

    // Get the peer name and set as the pathname
    //
       if (!(newfn = Peername(newfd)))
          {bbcp_Fmsg("Accept", "Unable to determine peer name.");
           close(newfd); continue;
          }
       setOpts("accept", newfd);

    // Allocate a new network object
    //
       if (!(newconn = new bbcp_Link(newfd, newfn)))
          {bbcp_Fmsg("Accept", "Unable to allocate new link.");
           close(newfd); newfd = 0;
          }
    } while(newfd < 0);

// Return new net object
//
   delete newfn;
   return newconn;
}
  
/******************************************************************************/
/*                                  B i n d                                   */
/******************************************************************************/
  
int bbcp_Network::Bind(int minport, int maxport, int tries, int timeout)
{
    struct sockaddr_in InetAddr;
    socklen_t iln = sizeof(InetAddr);
    struct sockaddr *SockAddr = (struct sockaddr *)&InetAddr;
    int SockSize = sizeof(InetAddr);
    char *action;
    int retc, One = 1, btry = 1;
    unsigned short port = minport;

// Close any open socket here
//
   unBind();

// Allocate a socket descriptor and set the options
//
   if ((iofd = socket(PF_INET, SOCK_STREAM, 0)) < 0)
      return bbcp_Emsg("Bind", -errno, "creating inet socket");
   setOpts("bind", iofd);

// Attempt to do a bind
//
   action = (char *)"binding";
   do {if (port > maxport && timeout > 0) sleep(timeout);
       port = minport;
       do {
           InetAddr.sin_family = AF_INET;
           InetAddr.sin_addr.s_addr = INADDR_ANY;
           InetAddr.sin_port = htons(port);
           if (!(retc = bind(iofd, SockAddr, SockSize))) break;
          } while(++port <= maxport);
      } while(++btry <= tries);

// If we bound, put up a listen
//
   if (!retc)
      {action = (char *)"listening on";
       retc = listen(iofd, 8);
      }

// Check for any errors and return.
//
   if (retc) 
      {if (timeout < 0) return -1;
       return bbcp_Emsg("Bind",-errno,action,"socket");
      }

// Return the port number being used
//
   Portnum = port;
   if (port) return (int)port;
   if (getsockname(iofd, SockAddr, &iln) < 0)
      return bbcp_Emsg("Bind",-errno, "determing bound port number.");
   Portnum = static_cast<int>(ntohs(InetAddr.sin_port));
   return Portnum;
}

/******************************************************************************/
/*                               C o n n e c t                                */
/******************************************************************************/

bbcp_Link *bbcp_Network::Connect(char *host, int port, int retries, int rwait)
{
    struct sockaddr_in InetAddr;
    struct sockaddr *SockAddr = (struct sockaddr *)&InetAddr;
    int SockSize = sizeof(InetAddr);
    int newfd, retc;
    char *hName;
    bbcp_Link *newlink;

// Determine the host address
//
   if (getHostAddr(host, InetAddr))
      {bbcp_Emsg("Connect", EHOSTUNREACH, "unable to find", host);
       return (bbcp_Link *)0;
      }
   InetAddr.sin_port = htons(port);
   hName = getHostName(InetAddr);

// Allocate a socket descriptor
//
   if ((newfd = socket(PF_INET, SOCK_STREAM, 0)) < 0)
      {bbcp_Emsg("Connect", errno, "creating inet socket to", hName);
       free(hName);
       return (bbcp_Link *)0;
      }

// Allocate a new lnk for this socket and set the options
//
   setOpts("connect", newfd);
   newlink = new bbcp_Link(newfd, hName);

// Connect to the host
//
   do {retc = connect(newfd, SockAddr, SockSize);}
      while(retc < 0 && (errno == EINTR || 
           (errno == ECONNREFUSED && (retries = Retry(retries, rwait)))));
   if (retc)
      {char buff[16];
       delete newlink;
       sprintf(buff, "port %d", port);
       bbcp_Emsg("Connect", errno, "unable to connect to", hName, buff);
       free(hName);
       return (bbcp_Link *)0;
      }

// Return the link
//
   free(hName);
   return newlink;
}

/******************************************************************************/
/*                              f i n d P o r t                               */
/******************************************************************************/


void bbcp_Network::findPort(int &minport, int &maxport)
{
   static const char *MinPort = "bbcpfirst";
   static const char *MaxPort = "bbcplast";
   struct servent sent, *sp;
   char sbuff[1024];

// Try to find minimum port number
//
   minport = maxport = 0;
   if (GETSERVBYNAME(MinPort, "tcp", &sent, sbuff, sizeof(sbuff), sp))
      {minport = ntohs(sp->s_port);
       if (GETSERVBYNAME(MaxPort, "tcp", &sent, sbuff, sizeof(sbuff), sp))
          maxport = ntohs(sp->s_port);
          else maxport = 0;
      }

// Do final port check and return
//
   if (minport > maxport) minport = maxport = 0;
}

/******************************************************************************/
/*                                  F l o w                                   */
/******************************************************************************/
  
void bbcp_Network::Flow(int isSrc)
{
   Sender = isSrc;
   WinSOP = (isSrc ? SO_SNDBUF : SO_RCVBUF);
}

/******************************************************************************/
/*                          F u l l H o s t N a m e                           */
/******************************************************************************/

char *bbcp_Network::FullHostName(char *host, int asipadr)
{
   char myname[260], *hp;
   struct sockaddr_in InetAddr;
  
// Identify ourselves if we don't have a passed hostname
//
   if (host) hp = host;
      else if (gethostname(myname, sizeof(myname))) hp = (char *)"";
              else hp = myname;

// Get our address
//
   if (getHostAddr(hp, InetAddr)) return strdup(hp);

// Convert it to a fully qualified host name or IP address
//
   return (asipadr ? strdup(inet_ntoa(InetAddr.sin_addr))
                   : getHostName(InetAddr));
}

/******************************************************************************/
/*                              M a x W S i z e                               */
/******************************************************************************/

int bbcp_Network::MaxWSize(int isSink)
{
   int sfd, cursz, nowwsz, maxwsz = 4*1024*1024, minwsz = 0;
   int rsOpt = (isSink ? SO_RCVBUF : SO_SNDBUF);
   socklen_t szcur = (socklen_t)sizeof(cursz);
   socklen_t szseg = (socklen_t)sizeof(maxSegment);
   socklen_t sznow;
       int rcs, rcg;

// Define a socket for discovering the maximum window size
//
   if ((sfd = socket(PF_INET, SOCK_STREAM, 0)) < 0)
      {bbcp_Emsg("MaxWSize", errno, "creating inet socket."); return 0;}

// Get maximum segment size
//
   if (getsockopt(sfd, IPPROTO_TCP, TCP_MAXSEG, (gsval_t)&maxSegment, &szseg))
      bbcp_Emsg("MaxWSize", errno, "getting TCP maxseg.");

// If we already have the maximum window size (via constructor) then no need
// to do a discovery.
//
   if ((nowwsz = (isSink ? maxRcvBuff : maxSndBuff)))
      {DEBUG("MaxWin=" <<nowwsz <<" MaxSeg=" <<maxSegment <<" AT=" <<ATune);
       close(sfd);
       return nowwsz;
      }

// Calculate the largest allowed quantity
//
   do {sznow = (socklen_t)sizeof(nowwsz);
       maxwsz = maxwsz<<1;
      } while(!(rcs=setsockopt(sfd,SOL_SOCKET,rsOpt,(ssval_t)&maxwsz, szcur))
           && !     getsockopt(sfd,SOL_SOCKET,rsOpt,(gsval_t)&nowwsz,&sznow)
           && (maxwsz <= nowwsz) && !(maxwsz & 0x20000000));

// Perform binary search to find the largest buffer allowed
//
   if (!rcs) minwsz = nowwsz;
      else while(maxwsz - minwsz > 1024)
                {cursz = minwsz + (maxwsz - minwsz)/2;
                 if (setsockopt(sfd,SOL_SOCKET,rsOpt,(ssval_t)&cursz,szcur))
                         maxwsz = cursz;
                    else minwsz = cursz;
                }

// Close the socket and return the buffer size
//
   close(sfd);
   DEBUG("Max Window=" <<minwsz <<" MaxSeg=" <<maxSegment <<" AT=" <<ATune);
   return minwsz;
}

/******************************************************************************/
/*                             g e t W B S i z e                              */
/******************************************************************************/
  
int bbcp_Network::getWBSize(int xfd, int srwant)
{
   int bsz;
   socklen_t szb = (socklen_t)sizeof(bsz);

   if (getsockopt(xfd,SOL_SOCKET,(srwant ? SO_SNDBUF:SO_RCVBUF),&bsz,&szb))
      return -errno;
   return bsz;
}

/******************************************************************************/
/*                                   Q o S                                    */
/******************************************************************************/
  
int bbcp_Network::QoS(int newQoS)
{
   int oldQoS = netQoS;
   if (newQoS >= 0) netQoS = newQoS;
   return oldQoS;
}

/******************************************************************************/
/*                             s e t W i n d o w                              */
/******************************************************************************/

int bbcp_Network::setWindow(int WSize, int noAT)
{
   Window = WSize;
   if (noAT) ATune = 0;
   DEBUG("Window set to " <<WSize <<" AT=" << ATune);
   return Window;
}
 
/******************************************************************************/
/*                       P r i v a t e   M e t h o d s                        */
/******************************************************************************/
/******************************************************************************/
/*                           g e t H o s t A d d r                            */
/******************************************************************************/
  
int bbcp_Network::getHostAddr(char *hostname, struct sockaddr_in &InetAddr)
{
    unsigned int addr;
    struct hostent hent, *hp = 0;
    char hbuff[1024];
    int rc, byaddr = 0;

// Make sure we have something to lookup here
//
    InetAddr.sin_family = AF_INET;
    if (!hostname || !hostname[0]) 
       {InetAddr.sin_addr.s_addr = INADDR_ANY; return 0;}

// Try to resulve the name
//
    if (hostname[0] > '9' || hostname[0] < '0')
       GETHOSTBYNAME(hostname,&hent,hbuff,sizeof(hbuff),hp,&rc);
       else if ((int)(addr = inet_addr(hostname)) == -1) errno = EINVAL;
               else {GETHOSTBYADDR((const char *)&addr,sizeof(addr),AF_INET,
                                   &hent, hbuff, sizeof(hbuff), hp, &rc);
                     byaddr = 1;
                    }

// Check if we resolved the name
//
   if (hp) memcpy((void *)&InetAddr.sin_addr, (const void *)hp->h_addr,
                 hp->h_length);
      else if (byaddr) memcpy((void *)&InetAddr.sin_addr, (const void *)&addr,
                 sizeof(addr));
              else return bbcp_Emsg("getHostAddr", errno,
                                    "obtaining address for", hostname);
   return 0;
}

/******************************************************************************/
/*                           g e t H o s t N a m e                            */
/******************************************************************************/

char *bbcp_Network::getHostName(struct sockaddr_in &addr)
{
   struct hostent hent, *hp;
   char *hname, hbuff[1024];
   int rc;

// Convert it to a host name
//
   if (GETHOSTBYADDR((const char *)&addr.sin_addr, sizeof(addr.sin_addr),
                     AF_INET, &hent, hbuff, sizeof(hbuff), hp, &rc))
             hname = strdup(hp->h_name);
        else hname = strdup(inet_ntoa(addr.sin_addr));

// Return the name
//
   return hname;
}
  
/******************************************************************************/
/*                              P e e r n a m e                               */
/******************************************************************************/
  
char *bbcp_Network::Peername(int snum)
{
      struct sockaddr_in addr;
      socklen_t addrlen = sizeof(addr);
      char *hname;

// Get the address on the other side of this socket
//
   if (getpeername(snum, (struct sockaddr *)&addr, &addrlen) < 0)
      {bbcp_Emsg("Peername", errno, "obtaining peer name.");
       return (char *)0;
      }

// Convert it to a host name
//
   return getHostName(addr);
}

/******************************************************************************/
/*                                 R e t r y                                  */
/******************************************************************************/
  
int bbcp_Network::Retry(int retries, int rwait)
{

// Sleep if retry allowed
//
   if ((retries -= 1) >= 0) sleep(rwait);
   return retries;
}

/******************************************************************************/
/*                               s e t O p t s                                */
/******************************************************************************/

void bbcp_Network::setOpts(const char *who, int xfd)
{
   int one = 1, wbsz;
   static const socklen_t szone = (socklen_t)sizeof(one);
   static const socklen_t szwb  = (socklen_t)sizeof(wbsz);

   if (setsockopt(xfd,SOL_SOCKET,SO_REUSEADDR,(ssval_t)&one,szone))
      bbcp_Emsg(who, errno, "setting REUSEADDR");

   if (setsockopt(xfd,protID,TCP_NODELAY,(ssval_t)&one,szone))
      bbcp_Emsg(who, errno, "setting NODELAY");

   if (netQoS && setsockopt(xfd, IPPROTO_IP, IP_TOS,
                           (ssval_t)&netQoS, sizeof(netQoS)))
      bbcp_Emsg(who, errno, "setting IP_TOS");

// Set the window if need be. If we are the sender, then reduce our window
// by 20% to be kind and not over-run the receiver.
//
   if (ATune || !(wbsz = Window)) wbsz = -1;
      else {if (Sender) wbsz = Window - (Window/5);
            if (setsockopt(xfd, SOL_SOCKET, WinSOP, &wbsz, szwb))
               bbcp_Emsg(who,errno,"setting",(Sender?"sndbuf":"rcvbuf","size"));
           }

// If debug is on, we verify that the window was set as we wanted
//
   if DEBUGON
      {int xsz;
       socklen_t szx = (socklen_t)sizeof(xsz);
       if (getsockopt(xfd, SOL_SOCKET, WinSOP, &xsz, &szx)) xsz = -errno;
       DEBUG(who <<(Sender ? " send" : " recv") <<" window set to " <<wbsz
                 <<" (actual=" <<xsz <<")");
      }
}
  
/******************************************************************************/
/*                              s e t S e g S z                               */
/******************************************************************************/

int bbcp_Network::setSegSz(const char *who, int xfd)
{
    int       segsz = 65535;
    socklen_t szseg = (socklen_t)sizeof(maxSegment);

    if (setsockopt(xfd, IPPROTO_TCP, TCP_MAXSEG, (ssval_t)&segsz, szseg)
       && !(errno == ENOPROTOOPT || errno == EINVAL))
       return bbcp_Emsg(who, -errno, "setting TCP maxseg.");
    return 0;
}
