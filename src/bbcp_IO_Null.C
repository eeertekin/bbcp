/******************************************************************************/
/*                                                                            */
/*                        b b c p _ I O _ N u l l . C                         */
/*                                                                            */
/* (c) 2002 by the Board of Trustees of the Leland Stanford, Jr., University  */
/*      All Rights Reserved. See bbcp_Version.C for complete License Terms    *//*                            All Rights Reserved                             */
/*   Produced by Andrew Hanushevsky for Stanford University under contract    */
/*              DE-AC03-76-SFO0515 with the Department of Energy              */
/******************************************************************************/

#include <errno.h>
#include <string.h>
#include <unistd.h>
#include "bbcp_IO_Null.h"


/******************************************************************************/
/*                                  R e a d                                   */
/******************************************************************************/
  
ssize_t bbcp_IO_Null::Read(char *buff, size_t rdsz)
{

// Read data into the buffer
//
   xfrtime.Start();
   xfrbytes += rdsz; xfrseek = xfrbytes;
   if (*buff) memset(buff, 0, rdsz);
   xfrtime.Stop();

// All done
//
   return rdsz;
}

ssize_t bbcp_IO_Null::Read(const struct iovec *iovp, int iovn)
{
   ssize_t nbytes = 0;
   int i;

// Add up the bytes to be read
//
   for (i = 0; i < iovn; i++)
       {if (*((char *)iovp[i].iov_base))
            memset(iovp[i].iov_base, 0, iovp[i].iov_len);
        nbytes += iovp[i].iov_len;
       }

// Read data into the buffer. Note that logging only occurs once even though
// we may execute the read multiple times simply because a readv() is an all
// or nothing operation and EINTR's rarely if ever happen on a readv().
//
   xfrtime.Start();
   xfrbytes += nbytes; xfrseek = xfrbytes;
   xfrtime.Stop();

// All done
//
   return nbytes;
}

/******************************************************************************/
/*                                 W r i t e                                  */
/******************************************************************************/
  
ssize_t bbcp_IO_Null::Write(char *buff, size_t wrsz)
{

// Just add up the bytes
//
   xfrtime.Start();
   xfrbytes += wrsz;
   xfrtime.Stop();

// All done
//
   return wrsz;
}

ssize_t bbcp_IO_Null::Write(const struct iovec *iovp, int iovn)
{
   ssize_t iolen = 0;

// Add up bytes
//
   xfrtime.Start();
   while(iovn--) {iolen += iovp->iov_len; iovp++;}
   xfrbytes += iolen;
   xfrtime.Stop();

// All done
//
   return iolen;
}
