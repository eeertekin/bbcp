#ifndef __BBCP_FS_UNIX_H__
#define __BBCP_FS_UNIX_H__
/******************************************************************************/
/*                                                                            */
/*                        b b c p _ F S _ U n i x . h                         */
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
  
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include "bbcp_FileSystem.h"

struct stat;

class bbcp_FS_Unix : public bbcp_FileSystem
{
public:

int        Applicable(const char *path);

int        Enough(long long bytes, int numfiles=1);

int        Fsync(const char *fn, int fd);

long long  getSize(int fd, long long *blksz=0);

bbcp_File *Open(const char *fn, int opts, int mode=0, const char *fa=0);

int        MKDir(const char *path, mode_t mode);

int        RM(const char *path);

int        setGroup(const char *path, const char *Group);

int        setMode(const char *path, mode_t mode);

int        setTimes(const char *path, time_t atime, time_t mtime);

int        Stat(const char *path, bbcp_FileInfo *finfo=0);

int        Stat(const char *path, const char *dent, int fd,
                int nolnks=1, bbcp_FileInfo *finfo=0);

           bbcp_FS_Unix() {}
          ~bbcp_FS_Unix() {}

protected:
int        Stat(struct stat &xbuff, bbcp_FileInfo *sbuff);
};
#endif
