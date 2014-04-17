#ifndef __BBCP_FS_PIPE_H__
#define __BBCP_FS_PIPE_H__
/******************************************************************************/
/*                                                                            */
/*                        b b c p _ F S _ P i p e . h                         */
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
  
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include "bbcp_FileSystem.h"

class bbcp_FS_Pipe : public bbcp_FileSystem
{
public:

int        Applicable(const char *path) {return 1;}

int        Enough(long long bytes, int numfiles=1) {return 1;}

long long  getSize(int fd, long long *bsz=0) {return 0;}

bbcp_File *Open(const char *fn, int opts, int mode=0, const char *fa=0);

int        MKDir(const char *path, mode_t mode) {return 0;}

int        RM(const char *path) {return 0;}

int        setGroup(const char *path, const char *Group) {return 0;}

int        setMode(const char *path, mode_t mode) {return 0;}

int        setTimes(const char *path, time_t atime, time_t mtime) {return 0;}

int        Stat(const char *path, bbcp_FileInfo *finfo=0);

int        Stat(const char *path, const char *dent, int fd,
                int nolnks=1, bbcp_FileInfo *finfo=0) {return -ENOENT;}

           bbcp_FS_Pipe() {}
          ~bbcp_FS_Pipe() {}

private:

bbcp_File *Exec(const char *prog, const char *fa, int opts);
int        Setup(const char *args, char **Arg, int maxArgs);
bool       Validate(const char *pgm);
};
#endif
