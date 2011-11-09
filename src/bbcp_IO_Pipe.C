/******************************************************************************/
/*                                                                            */
/*                        b b c p _ I O _ P i p e . C                         */
/*                                                                            */
/* (c) 2011 by the Board of Trustees of the Leland Stanford, Jr., University  */
/*                            All Rights Reserved                             */
/*   Produced by Andrew Hanushevsky for Stanford University under contract    */
/*              DE-AC02-76-SFO0515 with the Department of Energy              */
/******************************************************************************/

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "bbcp_Headers.h"
#include "bbcp_IO_Pipe.h"
#include "bbcp_System.h"

/******************************************************************************/
/*                        G l o b a l   O b j e c t s                         */
/******************************************************************************/
  
extern bbcp_System bbcp_OS;
  
/******************************************************************************/
/*                                 C l o s e                                  */
/******************************************************************************/
  
int bbcp_IO_Pipe::Close()
{
   int oifd, rc, tc;

// If we are already closed, return
//
   if (iofd < 0) return 0;

// Close the file descriptor
//
   oifd = iofd; iofd = -1;
   do {rc = close(oifd);} while(rc < 0 && errno == EINTR);
   if (rc) rc = errno;

// Check if we should wait for the process to end
//
   tc = (thePid ? bbcp_OS.Waitpid(thePid) : 0);

// Return the result
//
   if (tc < 0) return tc;
   if (tc > 0) return -EPIPE;
   return rc;
}
