/******************************************************************************/
/*                                                                            */
/*                        b b c p _ F S _ U n i x . C                         */
/*                                                                            */
/* (c) 2002 by the Board of Trustees of the Leland Stanford, Jr., University  */
/*      All Rights Reserved. See bbcp_Version.C for complete License Terms    *//*                            All Rights Reserved                             */
/*   Produced by Andrew Hanushevsky for Stanford University under contract    */
/*              DE-AC03-76-SFO0515 with the Department of Energy              */
/******************************************************************************/
  
#ifdef LINUX
#define _XOPEN_SOURCE 600
#endif

#include <errno.h>
#include <fcntl.h>
#include <grp.h>
#include <string.h>
#include <unistd.h>
#include <utime.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifndef FREEBSD
#include <sys/statvfs.h>
#endif
#include "bbcp_File.h"
#include "bbcp_FS_Unix.h"
#include "bbcp_Platform.h"
#include "bbcp_Pthread.h"
#include "bbcp_System.h"
#include "bbcp_Debug.h"

#if defined(MACOS) || defined (AIX)
#define S_IAMB      0x1FF
#endif

/******************************************************************************/
/*                        G l o b a l   O b j e c t s                         */
/******************************************************************************/

extern bbcp_System bbcp_OS;
  
/******************************************************************************/
/*                            A p p l i c a b l e                             */
/******************************************************************************/
  
int bbcp_FS_Unix::Applicable(const char *path)
{
#ifdef FREEBSD
   if (!fs_path) fs_path = strdup(path);
#else
   struct statvfs buf;

// To find out whether or not we are applicable, simply do a statvfs on the
// incomming path. If we can do it, then we are a unix filesystem.
//
   if (statvfs(path, &buf)) return 0;

// Set the sector size
//
   secSize = buf.f_frsize;

// Save the path to this filesystem if we don't have one. This is a real
// kludgy short-cut since in bbcp we only have a single output destination.
//
   if (!fs_path) 
      {fs_path = strdup(path); 
       memcpy((void *)&fs_id, (const void *)&buf.f_fsid, sizeof(fs_id));
      }
#endif
   return 1;
}

/******************************************************************************/
/*                                E n o u g h                                 */
/******************************************************************************/
  
int bbcp_FS_Unix::Enough(long long bytes, int numfiles)
{
#ifndef FREEBSD
    struct statvfs buf;
    long long free_space;

// Perform a stat call on the filesystem
//
   if (statvfs(fs_path, &buf)) return 0;

// Calculate free space
//
   free_space = (long long)buf.f_bsize * (long long)buf.f_bavail;

// Indicate whether there is enough space here
//
#ifdef LINUX
   if (!(buf.f_files | buf.f_ffree | buf.f_favail)) numfiles = 0;
#endif
   return (free_space > bytes) && buf.f_favail > numfiles;
#else
   return 1;
#endif
}

/******************************************************************************/
/*                               g e t S i z e                                */
/******************************************************************************/
  
long long bbcp_FS_Unix::getSize(int fd, long long *bsz)
{
   struct stat Stat;

// Get the size of the file
//
   if (fstat(fd, &Stat)) return -errno;
   if (bsz) *bsz = (secSize > 8192 ? 8192 : secSize);
   return Stat.st_size;
}

/******************************************************************************/
/*                                 M K D i r                                  */
/******************************************************************************/

int bbcp_FS_Unix::MKDir(const char *path, mode_t mode)
{
    if (mkdir(path, mode)) return -errno;

    if (chmod(path, 0755)) return -errno;

    return 0;
}
  
/******************************************************************************/
/*                                  O p e n                                   */
/******************************************************************************/

bbcp_File *bbcp_FS_Unix::Open(const char *fn, int opts, int mode)
{
    int FD;
    bbcp_IO *iob;

// Check for direct I/O
//
#ifdef O_DIRECT
   if (dIO) opts |= O_DIRECT;
#endif

// Open the file
//
   if ((FD = (mode ? open(fn, opts, mode) : open(fn, opts))) < 0) 
      return (bbcp_File *)0;

// Advise about file access in Linux
//
#ifdef LINUX
   posix_fadvise(FD, 0, 0, POSIX_FADV_SEQUENTIAL | POSIX_FADV_NOREUSE);
#endif

// Do direct I/O for Solaris
//
#ifdef SUN
   if (dIO && directio(FD, DIRECTIO_ON))
      {DEBUG(strerror(errno) <<" requesting direct i/o for " <<fn); dIO = 0;}
#endif

// Allocate a file object and return that
//
   iob =  new bbcp_IO(FD);
   return new bbcp_File(fn, iob, (bbcp_FileSystem *)this, (dIO ? secSize : 0));
}
  
/******************************************************************************/
/*                                    R M                                     */
/******************************************************************************/
  
int bbcp_FS_Unix::RM(const char *path)
{
    if (!unlink(path)) return 0;
    return -errno;
}

/******************************************************************************/
/*                              s e t G r o u p                               */
/******************************************************************************/
  
int bbcp_FS_Unix::setGroup(const char *path, const char *Group)
{
    gid_t gid;

// Convert the group name to a gid
//
   if (!Group || !Group[0]) return 0;
   gid = bbcp_OS.getGID(Group);

// Set the group code and return
//
   if (chown(path, (uid_t)-1, gid)) return -errno;
   return 0;
}

/******************************************************************************/
/*                               s e t M o d e                                */
/******************************************************************************/
  
int bbcp_FS_Unix::setMode(const char *path, mode_t mode)
{
    if (chmod(path, mode)) return -errno;
    return 0;
}

/******************************************************************************/
/*                              s e t T i m e s                               */
/******************************************************************************/
  
int bbcp_FS_Unix::setTimes(const char *path, time_t atime, time_t mtime)
{
    struct utimbuf ftimes;

    ftimes.actime = atime;
    ftimes.modtime= mtime;
    if (utime(path, &ftimes)) return -errno;
    return 0;
}
 
/******************************************************************************/
/*                                  S t a t                                   */
/******************************************************************************/
  
int bbcp_FS_Unix::Stat(const char *path, bbcp_FileInfo *sbuff)
{
   struct stat xbuff;

// Perform the stat function
//
   if (stat(path, &xbuff)) return -errno;
   if (!sbuff) return 0;

// Copy the stat info into our own structure
//
   sbuff->fileid = ((long long)xbuff.st_dev)<<32 | (long long)xbuff.st_ino;
   sbuff->mode   = xbuff.st_mode & (S_IAMB | 0xF00);
   sbuff->size   = xbuff.st_size;
   sbuff->atime  = xbuff.st_atime;
   sbuff->ctime  = xbuff.st_ctime;
   sbuff->mtime  = xbuff.st_mtime;

// Get type of object
//
// if (S_ISREG(xbuff.st_mode) || S_ISFIFO(xbuff.st_mode)) sbuff->Otype = 'f';
   if (S_ISREG(xbuff.st_mode)) sbuff->Otype = 'f';
      else if (S_ISDIR(xbuff.st_mode)) sbuff->Otype = 'd';
              else sbuff->Otype = '?';

// Convert gid to a group name
//
   if (sbuff->Group) free(sbuff->Group);
   sbuff->Group = bbcp_OS.getGNM(xbuff.st_gid);

// All done
//
   return 0;
}
