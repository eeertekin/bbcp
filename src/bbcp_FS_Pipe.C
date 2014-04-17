/******************************************************************************/
/*                                                                            */
/*                        b b c p _ F S _ P i p e . C                         */
/*                                                                            */
/* (c) 2011 by the Board of Trustees of the Leland Stanford, Jr., University  */
/*      All Rights Reserved. See bbcp_Version.C for complete License Terms    *//*                            All Rights Reserved                             */
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
  
#ifdef LINUX
#define _XOPEN_SOURCE 600
#endif

#include <fcntl.h>
#include <grp.h>
#include <strings.h>
#include <unistd.h>
#include <utime.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "bbcp_Emsg.h"
#include "bbcp_File.h"
#include "bbcp_FS_Unix.h"
#include "bbcp_FS_Pipe.h"
#include "bbcp_IO_Pipe.h"
#include "bbcp_Platform.h"
#include "bbcp_Pthread.h"
#include "bbcp_System.h"
#include "bbcp_Debug.h"

/******************************************************************************/
/*                        G l o b a l   O b j e c t s                         */
/******************************************************************************/

extern bbcp_System bbcp_OS;

/******************************************************************************/
/* Private:                         E x e c                                   */
/******************************************************************************/

bbcp_File *bbcp_FS_Pipe::Exec(const char *prog, const char *fa, int opts)
{
   static const int maxArgs = 128;
   struct ArgList {char *Vec[maxArgs];
                         ArgList() {Vec[1] = 0;}
                        ~ArgList() {if (Vec[1]) free(Vec[1]);}
                  } Args;
   bbcp_IO *iob;
   pid_t Child;
   int n, rc, gildes[2], fildes[2], fdLocal, fdRemote, fdTarget;

// Issue some debugging here
//
   DEBUG("Running " <<prog <<' ' <<(fa ? fa : ""));

// Validate that we can actually run this program
//
   if (!Validate(prog)) return 0;

// First tokenize the arguments
//
   Args.Vec[0] = (char *)prog;
   if (!Setup(fa, Args.Vec, maxArgs)) {errno = E2BIG; return 0;}

// Create a pipe. Minimize file descriptor leaks.
//
   if (pipe(fildes) || pipe(gildes)) return 0;

// Determine how the other file descriptor is assigned
//
   if (opts & O_WRONLY)
      {fdTarget = STDIN_FILENO;
       fdLocal  = fildes[1];
       fdRemote = fildes[0];
      } else {
       fdTarget = STDOUT_FILENO;
       fdLocal  = fildes[0];
       fdRemote = fildes[1];
      }

// Fork a process first so we can pick up the next request.
//
   if ((Child = bbcp_OS.Fork()))
      {rc = errno;     close(fdRemote); close(gildes[1]);
       if (Child < 0) {close(fdLocal);  close(gildes[0]); errno = rc; return 0;}
       n = read(gildes[0], &rc, sizeof(rc)); close(gildes[0]);
       if (n == (int)sizeof(rc)) {errno = rc; return 0;}
       iob =  new bbcp_IO_Pipe(fdLocal, Child);
       return new bbcp_File(Args.Vec[0], iob, (bbcp_FileSystem *)this, 0);
      }

// Attach file descriptor to stdin (for targets) to stdout (for source)
//
   if (dup2(fdRemote, fdTarget) < 0) exit(255);
   close(fdLocal);
   fcntl(gildes[1], F_SETFD, FD_CLOEXEC);

// Invoke the command never to return
//
   execvp(Args.Vec[0], Args.Vec);
   rc = errno;
   write(gildes[1], &rc, sizeof(rc));
   exit(255);
}
  
/******************************************************************************/
/*                                  O p e n                                   */
/******************************************************************************/

bbcp_File *bbcp_FS_Pipe::Open(const char *fn, int opts, int mode, const char *fa)
{
   static const int rwxMask = S_ISUID|S_ISGID|S_ISVTX|S_IRWXU|S_IRWXG|S_IRWXO;
   struct stat xbuff;
   bbcp_IO *iob;
   int FD;

// If this is not a FIFO, then it must be a program
//
   if (!(mode & S_IFIFO)) return Exec(fn, fa, opts);

// Open the fifo (at this point we presume it's a fifo)
//
   mode &= rwxMask;
   if ((FD = (mode ? open(fn, opts, mode) : open(fn, opts))) < 0) return 0;

// Verify that this is a fifo
//
   if (fstat(FD, &xbuff)) {close(FD); return 0;}
   if (!(S_ISFIFO(xbuff.st_mode)))
      {close(FD); errno = EPROTONOSUPPORT; return 0;}

// Allocate a file object and return that
//
   iob =  new bbcp_IO_Pipe(FD);
   return new bbcp_File(fn, iob, (bbcp_FileSystem *)this, 0);
}
 

/******************************************************************************/
/* Private:                        S e t u p                                  */
/******************************************************************************/
  
int bbcp_FS_Pipe::Setup(const char *args, char **Arg, int maxArgs)
{
   char *pp;
   int j;

// Check if we have any args
//
   if (!args) return 1;
   while(*args && *args == ' ') args++;
   if (!(*args)) return 1;

// Prepare to tokenize the args
//
   pp = Arg[1] = strdup(args);
   j = strlen(pp)-1;
   while(j && pp[j] == ' ') pp[j--] = '\0';

// Construct the argv array based on passed command line.
//
   for (j = 2; j < maxArgs && *pp; j++)
       {while(*pp && *pp != ' ') pp++;
        if (!(*pp)) break;
        *pp++ = '\0';
        while(*pp == ' ') pp++;
        if (!(*pp)) break;
        Arg[j] = pp;
       }

// Make sure we did not overflow the buffer
//
   if (j < maxArgs-1) Arg[j] = 0;
      else free(Arg[0]);
   return (j < maxArgs);
}

/******************************************************************************/
/*                                  S t a t                                   */
/******************************************************************************/
  
int bbcp_FS_Pipe::Stat(const char *path, bbcp_FileInfo *sbuff)
{
   static bbcp_FS_Unix fsUnix;
   const char *pP;
   char pBuff[1024];
   int rc;

// We are here only if pipes are enabled. Pipe paths cannot have spaces in
// them. If they do, then it's likely to be an program. We check for this.
//
   if ((pP = index(path, ' ')))
      {int n = pP - path;
       if (n < (int)sizeof(pBuff))
          {strncpy(pBuff, path, n); pBuff[n] = 0; path = pBuff;}
      }

// Perform the stat function
//
   if (!(rc = fsUnix.Stat(path, sbuff)) || rc != -ENOENT) return rc;

// File was not found, fake this as an executable. An error may occur later.
//
   sbuff->fileid = 0;
   sbuff->mode   = S_IXUSR|S_IXGRP|S_IXOTH;
   sbuff->size   = 0;
   sbuff->atime  = time(0);
   sbuff->ctime  = sbuff->atime;
   sbuff->mtime  = sbuff->atime;
   sbuff->Otype = 'f';
   sbuff->Xtype = 'x';
   sbuff->Group = strdup("any");
   return 0;
}

/******************************************************************************/
/*                              V a l i d a t e                               */
/******************************************************************************/
  
bool bbcp_FS_Pipe::Validate(const char *pgm)
{
   extern const char *bbcp_HostName;
   const char *aList = getenv("BBCP_ALLOWPP");
   char *bList, *xList, *chkPgm;

// Check if we are restricting program pipes
//
   if (!aList) return true;

// Check if no program pipes are allowed
//
   if (!(*aList) || !strcmp(aList, "0"))
      {cerr <<"bbcp: " <<bbcp_HostName <<" disallows program pipes." <<endl;
       errno = EPERM;
       return false;
      }

// Prepare to scan the allowed program list
//
   xList = bList = strdup(aList);
   while(*bList)
        {while(*bList && *bList == ' ') bList++;
         chkPgm = bList; bList++;
         while(*bList && *bList != ' ') bList++;
         *bList = 0; bList++;
         if (!strcmp(pgm, chkPgm)) {free(xList); return true;}
        }

// This program is not allowed
//
   free(xList);
   cerr <<"bbcp: " <<bbcp_HostName <<" disallows " <<pgm
                   <<" as a program pipe." <<endl;
   errno = EPERM;
   return false;
}
