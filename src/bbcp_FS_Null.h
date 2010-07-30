#ifndef __BBCP_FS_NULL_H__
#define __BBCP_FS_NULL_H__
/******************************************************************************/
/*                                                                            */
/*                        b b c p _ F S _ N u l l . h                         */
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

class bbcp_FS_Null : public bbcp_FileSystem
{
public:

int        Applicable(const char *path);

int        Enough(long long bytes, int numfiles=1) {return 1;}

long long  getSize(int fd, long long *bsz=0);

bbcp_File *Open(const char *fn, int opts, int mode=0);

int        MKDir(const char *path, mode_t mode) {return 0;}

int        RM(const char *path) {return 0;}

int        setGroup(const char *path, const char *Group) {return 0;}

int        setMode(const char *path, mode_t mode) {return 0;}

int        setTimes(const char *path, time_t atime, time_t mtime) {return 0;}

int        Stat(const char *path, bbcp_FileInfo *finfo=0);

           bbcp_FS_Null() {}
          ~bbcp_FS_Null() {}

protected:
};
#endif
