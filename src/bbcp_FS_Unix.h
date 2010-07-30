#ifndef __BBCP_FS_UNIX_H__
#define __BBCP_FS_UNIX_H__
/******************************************************************************/
/*                                                                            */
/*                        b b c p _ F S _ U n i x . h                         */
/*                                                                            */
/* (c) 2002 by the Board of Trustees of the Leland Stanford, Jr., University  */
/*      All Rights Reserved. See bbcp_Version.C for complete License Terms    *//*                            All Rights Reserved                             */
/*   Produced by Andrew Hanushevsky for Stanford University under contract    */
/*              DE-AC03-76-SFO0515 with the Department of Energy              */
/******************************************************************************/
  
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include "bbcp_FileSystem.h"

class bbcp_FS_Unix : public bbcp_FileSystem
{
public:

int        Applicable(const char *path);

int        Enough(long long bytes, int numfiles=1);

long long  getSize(int fd, long long *blksz=0);

bbcp_File *Open(const char *fn, int opts, int mode=0);

int        MKDir(const char *path, mode_t mode);

int        RM(const char *path);

int        setGroup(const char *path, const char *Group);

int        setMode(const char *path, mode_t mode);

int        setTimes(const char *path, time_t atime, time_t mtime);

int        Stat(const char *path, bbcp_FileInfo *finfo=0);

           bbcp_FS_Unix() {}
          ~bbcp_FS_Unix() {}

protected:
};
#endif
