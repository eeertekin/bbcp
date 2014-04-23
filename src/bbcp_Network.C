/******************************************************************************/
/*                                                                            */
/*                        b b c p _ N e t w o r k . C                         */
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
#include "bbcp_NetAddr.h"
#include "bbcp_Platform.h"

/******************************************************************************/
/*                      E x t e r n a l   O b j e c t s                       */
/******************************************************************************/

       bbcp_Network bbcp_Net;

/******************************************************************************/
/*                        S t a t i c   O b j e c t s                         */
/******************************************************************************/
  
int    bbcp_Network::pFirst = 0;
int    bbcp_Network::pLast  = 0;

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
   bbcp_NetAddr     PeerInfo;
   bbcp_NetSockAddr PeerAddr;
   const char *newfn, *eText;
   int newfd, retc;
   bbcp_Link *newconn;
   struct pollfd sfd[1];
   socklen_t addrLen =  sizeof(PeerAddr);

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

       do {newfd = accept(iofd, &PeerAddr.Addr, &addrLen);}
          while(newfd < 0 && errno == EINTR);

       if (newfd < 0)
          {char buff[16];
           sprintf(buff,"%d", Portnum);
           bbcp_Emsg("Accept", errno, "performing accept on port", buff);
           continue;
          }

    // Get the peer name and set as the pathname
    //
       PeerInfo.Set(&PeerAddr.Addr);
       if (!(newfn = PeerInfo.Name(0, &eText)))
          {bbcp_Fmsg("Accept", "Unable to determine peer name; ", eText);
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
   return newconn;
}
  
/******************************************************************************/
/*                                  B i n d                                   */
/******************************************************************************/
  
int bbcp_Network::Bind(int minport, int maxport, int tries, int timeout)
{
    bbcp_NetAddr InetAddr;
    const char *eText;
    char *action;
    int retc, One = 1, btry = 1;
    unsigned short port = minport;

// Close any open socket here
//
   unBind();

// Allocate a socket descriptor and set the options
//
   InetAddr.Set((const char *)0,0);
   if ((iofd = socket(InetAddr.Protocol(), SOCK_STREAM, 0)) < 0)
      return bbcp_Emsg("Bind", -errno, "creating inet socket");
   setOpts("bind", iofd);

// Attempt to do a bind
//
   action = (char *)"binding";
   do {if (port > maxport && timeout > 0) sleep(timeout);
       port = minport;
       do {InetAddr.Port(port);
           if (!(retc = bind(iofd, InetAddr.SockAddr(), InetAddr.SockSize())))
              break;
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
   if ((eText = InetAddr.Set(iofd, false)))
      return bbcp_Emsg("Bind",-errno, "determining bound port number;", eText);
   Portnum = InetAddr.Port();
   return Portnum;
}

/******************************************************************************/
/*                               C o n n e c t                                */
/******************************************************************************/

bbcp_Link *bbcp_Network::Connect(char *host, int port, int retries, int rwait)
{
    bbcp_NetAddr InetAddr;
    const char *eText, *hName;
    int newfd, retc;
    bbcp_Link *newlink;

// Determine the host address
//
   if ((eText = InetAddr.Set(host, port))
   || !(hName = InetAddr.Name(0, &eText)))
      {bbcp_Fmsg("Connect", "Unable to connect to ", host, "; ", eText);
       return (bbcp_Link *)0;
      }

// Allocate a socket descriptor
//
   if ((newfd = socket(InetAddr.Protocol(), SOCK_STREAM, 0)) < 0)
      {bbcp_Emsg("Connect", errno, "creating inet socket to", hName);
       return (bbcp_Link *)0;
      }

// Allocate a new lnk for this socket and set the options
//
   setOpts("connect", newfd);
   newlink = new bbcp_Link(newfd, hName);

// Connect to the host
//
   do {retc = connect(newfd, InetAddr.SockAddr(), InetAddr.SockSize());}
      while(retc < 0 && (errno == EINTR || 
           (errno == ECONNREFUSED && (retries = Retry(retries, rwait)))));
   if (retc)
      {char buff[1024];
       delete newlink;
       sprintf(buff, "port %s:%d", hName, port);
       bbcp_Emsg("Connect", errno, "unable to connect to", buff);
       return (bbcp_Link *)0;
      }

// Some debugging
//
   DEBUG("Connected to: " <<hName <<':' <<port);

// Return the link
//
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

// Use command line ports if present
//
   if (pFirst)
      {minport = pFirst;
       maxport = pLast;
       return;
      }

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
   bbcp_NetAddr InetAddr((int)0);
   char myname[260];
  
// Check if we should set something other than ourselves
//
   if (host) InetAddr.Set(host,0);

// Format name as needed
//
   if (!InetAddr.Format(myname, sizeof(myname),
                        (asipadr ? bbcp_NetAddrInfo::fmtAddr
                                 : bbcp_NetAddrInfo::fmtName),
                        bbcp_NetAddrInfo::noPort)) return 0;

// Return the name
//
   return strdup(myname);
}

/******************************************************************************/
/*                                  I P V 4                                   */
/******************************************************************************/
  
void bbcp_Network::IPV4() {bbcp_NetAddr::SetIPV4();}

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
/*                              s e t P o r t s                               */
/******************************************************************************/

int bbcp_Network::setPorts(int pnum1, int pnum2)
{

   if (pnum1 < 1 || pnum1 > 65535 || pnum2 < 1 || pnum2 > 65535
   ||  pnum1 > pnum2) return 0;
   pFirst = pnum1;
   pLast  = pnum2;
   return 1;
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
/*
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
*/
/******************************************************************************/
/*                           g e t H o s t N a m e                            */
/******************************************************************************/
/*
char *bbcp_Network::getHostName(struct sockaddr_in &addr)
{
   struct hostent hent, *hp;
   char *hname, hbuff[1024];
   int rc;

// Convert it to a host name
//
   if (GETHOSTBYADDR((const char *)&addr.sin_addr, sizeof(addr.sin_addr),
                     AF_INET, &hent, hbuff, sizeof(hbuff), hp, &rc) && hp)
             hname = strdup(hp->h_name);
        else hname = strdup(inet_ntoa(addr.sin_addr));

// Return the name
//
   return hname;
}
*/
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
       socklen_t szseg = (socklen_t)sizeof(maxSegment);
       socklen_t szx   = (socklen_t)sizeof(xsz);
       if (getsockopt(xfd, IPPROTO_TCP, TCP_MAXSEG, (gsval_t)&maxSegment, &szseg))
       bbcp_Emsg("MaxWSize", errno, "getting TCP maxseg.");
       if (getsockopt(xfd, SOL_SOCKET, WinSOP, &xsz, &szx)) xsz = -errno;
       DEBUG(who <<(Sender ? " send" : " recv") <<" window set to " <<wbsz
                 <<" (actual=" <<xsz <<" segsz=" <<maxSegment <<")");
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
