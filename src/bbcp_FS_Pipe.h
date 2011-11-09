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
};
#endif
