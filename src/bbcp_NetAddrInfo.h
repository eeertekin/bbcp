#ifndef __BBCP_NETADDRINFO_H__
#define __BBCP_NETADDRINFO_H__
/******************************************************************************/
/*                                                                            */
/*                    b b c p _ N e t A d d r I n f o . h                     */
/*                                                                            */
/*(c) 2013-14 by the Board of Trustees of the Leland Stanford, Jr., University*//*                            All Rights Reserved                             */
/*   Produced by Andrew Hanushevsky for Stanford University under contract    */
/*              DE-AC02-76-SFO0515 with the Department of Energy              */
/*                                                                            */
/* bbcp is free software: you can redistribute it and/or modify it under      */
/* the terms of the GNU Lesser General Public License as published by the     */
/* Free Software Foundation, either version 3 of the License, or (at your     */
/* option) any later version. This file is derived from XrdNetAddrInfo.hh part*/
/* of the XRootD software suite which is identically copyrighted and licensed.*/
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
  
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "bbcp_NetSockAddr.h"
  
//------------------------------------------------------------------------------
//! The bbcp_NetAddrInfo class is meant to provide read/only access to members
//! that only class bbcp_NetAddr can manipulate. This allows the generic use of
//! the bbcp_NetAddr class but provides a means of exporting that information to
//! other objects without allowing those objects to modify the information.
//------------------------------------------------------------------------------

struct addrinfo;

class bbcp_NetAddrInfo
{
public:

//------------------------------------------------------------------------------
//! Provide our address family.
//!
//! @return Success: Returns AF_INET, AF_INET6, or AF_UNIX.
//!         Failure: Returns 0, address is not valid.
//------------------------------------------------------------------------------

inline int  Family() const {return static_cast<int>(IP.Addr.sa_family);}

//------------------------------------------------------------------------------
//! Format our address into a supplied buffer with one of the following layouts
//! (the ':<port>' or ':/path' can be omitted if desired, see fmtOpts param):
//! IP.xx:   host_name:<port>
//! IP.v4:   a.b.c.d:<port>
//! IP.4to6: [::ffff:a.b.c.d]:<port>  | [::a.b.c.d]:<port>
//! IP.v6:   [a:b:c:d:e:f:g:h]:<port>
//! IP.Unix: localhost:/<path>
//!
//! @param  bAddr    address of buffer for result
//! @param  bLen     length  of buffer
//! @param  fmtType  specifies the type of format desired via fmtUse enum.
//! @param  fmtOpts  additional formatting options (can be or'd):
//!                  noPort    - do not append the port number to the address.
//!                  noPortRaw - no port and no brackets for IPv6.
//!                  old6Map4  - use deprecated IPV6 mapped format '[::x.x.x.x]'
//!
//! @return Success: The number of characters (less null) in Buff.
//! @return Failure: 0 (buffer is too small or not a valid address). However,
//!                  if bLen > 0 the buffer will contain a null terminated
//!                  string of up to 8 question marks.
//------------------------------------------------------------------------------

enum fmtUse {fmtAuto=0, //!< Hostname if already resolved o/w use fmtAddr
             fmtName,   //!< Hostname if it is resolvable o/w use fmtAddr
             fmtAddr,   //!< Address using suitable ipv4 or ipv6 format
             fmtAdv6};  //!< Address only in ipv6 format

static const int noPort    = 0x0000001; //!< Do not add port number
static const int noPortRaw = 0x0000002; //!< Use raw address format (no port)
static const int old6Map4  = 0x0000004; //!< Use deprecated IPV6 mapped format

int         Format(char *bAddr, int bLen, fmtUse fmtType=fmtAuto, int fmtOpts=0);

//------------------------------------------------------------------------------
//! Indicate whether or not our address is the loopback address. Use this
//! method to gaurd against UDP packet spoofing.
//!
//! @return True:    This is     the loopback address.
//! @return False:   This is not the loopback address.
//------------------------------------------------------------------------------

bool        isLoopback();

//------------------------------------------------------------------------------
//! Indicate whether or not our address is if the desired type.
//!
//! @param ipType    The IP address version to test (see enum below).
//!
//! @return True:    This is     the address version of ipType.
//! @return False:   This is not the address version of ipType.
//------------------------------------------------------------------------------

enum IPType {IPv4 = AF_INET, IPv6 = AF_INET6, IPuX = AF_UNIX};

inline bool isIPType(IPType ipType) const {return IP.Addr.sa_family == ipType;}

//------------------------------------------------------------------------------
//! Indicate whether or not our address is an IPv4 mapped to IPv6 address.
//!
//! @return True:  The address is     a mapped IPv4 address.
//!         False: The address is not a mapped IPv4 address.
//------------------------------------------------------------------------------

inline bool isMapped() const {return IP.Addr.sa_family == AF_INET6
                                  && IN6_IS_ADDR_V4MAPPED(&IP.v6.sin6_addr);
                             }

//------------------------------------------------------------------------------
//! Indicate whether or not our address is private.
//!
//! @return True:    This address is     private.
//!         False:   This address is not private.
//------------------------------------------------------------------------------

bool        isPrivate();

//------------------------------------------------------------------------------
//! Indicate whether or not our address is registered in the DNS.
//!
//! @return True:    This address is     registered.
//!         False:   This address is not registered.
//------------------------------------------------------------------------------

bool        isRegistered();

//------------------------------------------------------------------------------
//! Obtain the location of this address.
//!
//! @return !0       pointer to the unverified location information. Not all
//!                  fields may be set.
//! @return =0       location information is not available.
//------------------------------------------------------------------------------

struct LocInfo
      {unsigned char  Country[2]; //!< Two letter TLD country code
       unsigned char  Region;     //!< Region (may combine adjacent countries)
       unsigned char  Locale;     //!< Locale (may combine adjacent regions)
                char  TimeZone;   //!< +/- hours from GMT (-128 if not set)
                char  Reserved;
                short Speed;      //!< I/F speed (Gb*1024/100)(not supported)
                int   Latitude;   //!< Degrees +/- xx.xxxxxx  (not supported)
                int   Longtitude; //!< Degrees +/- xx.xxxxxx  (not supported)

                LocInfo() : Region(0), Locale(0), TimeZone(-128), Reserved(0),
                            Speed(0), Latitude(0), Longtitude(0) {}
       };

const struct
LocInfo    *Location() {return (addrLoc.Country[0] ? &addrLoc : 0);}

//------------------------------------------------------------------------------
//! Convert our IP address to the corresponding [host] name.
//!
//! @param  eName    value to return when the name cannot be determined.
//! @param  eText    when not null, the reason for a failure is returned.
//!
//! @return Success: Pointer to the name or ip address with eText, if supplied,
//!                  set to zero. The memory is owned by the object and is
//!                  deleted when the object is deleted or Set() is called.
//!         Failure: eName param and if eText is not zero, returns a pointer
//!                  to a message describing the reason for the failure. The
//!                  message is in persistent storage and cannot be modified.
//------------------------------------------------------------------------------

const char *Name(const char *eName=0, const char **eText=0);

//------------------------------------------------------------------------------
//! Provide a pointer to our socket address suitable for use in calls to methods
//! that require our internal format of sock addr. A value is only returned for
//! IPV6/4 addresses and is nill otherwise. The pointer refers to memory
//! allocated by this object and becomes invalid should the object be deleted.
//! Use SockSize() to get its logical length.
//------------------------------------------------------------------------------

inline const
bbcp_NetSockAddr *NetAddr() {return (sockAddr == (void *)&IP ? &IP : 0);}

//------------------------------------------------------------------------------
//! Return the port number for our address.
//!
//! @return Success: The port number, which may be 0 if not set.
//!         Failure: -1 address is not an internet address or port is invalid.
//------------------------------------------------------------------------------

int         Port();

//------------------------------------------------------------------------------
//! Provide our protocol family.
//!
//! @return Success: Returns PF_INET, PF_INET6, or PF_UNIX.
//!         Failure: Returns 0, address is not valid.
//------------------------------------------------------------------------------

inline int  Protocol() {return static_cast<int>(protType);}

//------------------------------------------------------------------------------
//! Check if the IP address in this object is the same as the one passed.
//!
//! @param ipAddr    points to the network address object to compare.
//! @param plusPort  when true, port values must also match. In any case, both
//!                  addresses must be of the same address family.
//!
//! @return Success: True  (addresses are     the same).
//!         Failure: False (addresses are not the same).
//-----------------------------------------------------------------------------sav-

int         Same(const bbcp_NetAddrInfo *ipAddr, bool plusPort=false);

//------------------------------------------------------------------------------
//! Provide a pointer to our socket address suitable for use in calls to
//! functions that require one (e.g. bind() etc). The pointer refers to memory
//! allocated by this object and becomes invalid should the object be deleted
//! or when Set() is called. Use SockSize() to get its length.
//------------------------------------------------------------------------------

inline const
sockaddr   *SockAddr() {return sockAddr;}

//------------------------------------------------------------------------------
//! Provide the length of our socket adress. Useful for system calls needing it.
//!
//! @return Success: Returns the length of the address returned by SockAddr().
//!         Failure: Returns 0, address is not valid.
//------------------------------------------------------------------------------
inline
socklen_t   SockSize() {return addrSize;}

//------------------------------------------------------------------------------
//! Get the associated file descriptor.
//!
//! @return The associated file descriptor. If negative, no association exists.
//------------------------------------------------------------------------------

inline int  SockFD() {return sockNum;}

//------------------------------------------------------------------------------
//! Assignment operator
//------------------------------------------------------------------------------

bbcp_NetAddrInfo &operator=(bbcp_NetAddrInfo const &rhs)
               {if (&rhs != this)
                   {memcpy(&IP, &rhs.IP, sizeof(IP));
                    addrSize = rhs.addrSize; sockNum = rhs.sockNum;
                    if (hostName) free(hostName);
                    hostName = (rhs.hostName ? strdup(rhs.hostName):0);
                    if (rhs.sockAddr != &rhs.IP.Addr)
                       {if (!unixPipe) unixPipe = new sockaddr_un;
                        memcpy(unixPipe, rhs.unixPipe, sizeof(sockaddr_un));
                       } else sockAddr = &IP.Addr;
                   }
                addrLoc = rhs.addrLoc;
                return *this;
               }

//------------------------------------------------------------------------------
//! Copy constructor
//------------------------------------------------------------------------------

               bbcp_NetAddrInfo(bbcp_NetAddrInfo const &oP)
                             {hostName = 0;
                              unixPipe = 0;
                              *this = oP;
                             }

//------------------------------------------------------------------------------
//! Constructor
//------------------------------------------------------------------------------

            bbcp_NetAddrInfo() : hostName(0), addrSize(0), protType(0), sockNum(-1)
                           {IP.Addr.sa_family = 0;
                            sockAddr = &IP.Addr;
                           }

            bbcp_NetAddrInfo(const bbcp_NetAddrInfo *addr) : hostName(0) {*this = *addr;}

//------------------------------------------------------------------------------
//! Destructor
//------------------------------------------------------------------------------

           ~bbcp_NetAddrInfo() {if (hostName) free(hostName);
                                if (sockAddr != &IP.Addr) delete unixPipe;
                               }

protected:
       char               *LowCase(char *str);
       int                 QFill(char *bAddr, int bLen);
       int                 Resolve();

// For optimization this union should be the first member of this class as we
// compare "unixPipe" with "&IP" and want it optimized to "unixPipe == this".
//
bbcp_NetSockAddr           IP;
union {struct sockaddr    *sockAddr;
       struct sockaddr_un *unixPipe;
      };
char                      *hostName;
LocInfo                    addrLoc;
socklen_t                  addrSize;
short                      protType;
short                      sockNum;
};
#endif
