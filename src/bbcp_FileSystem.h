#ifndef __BBCP_FILESYSTEM_H__
#define __BBCP_FILESYSTEM_H__
/******************************************************************************/
/*                                                                            */
/*                     b b c p _ F i l e S y s t e m . h                      */
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
#include <sys/stat.h>
#include <sys/types.h>

struct bbcp_FileInfo
{
       long long fileid;    // Unique file identifier in the system (dev/inode)
       long long size;      // Size of file in bytes
       mode_t    mode;      // File mode (i.e., rwx)
       time_t    atime;     // Access time
       time_t    mtime;     // Modification time
       time_t    ctime;     // Create time
       char     *Group;     // -> Group name
       char      Otype;     // 'd' | 'f' | 'p' | '?'
       char      Xtype;     // 'x' | 0

       bbcp_FileInfo() : Group(0), Otype('?'), Xtype(0) {}
      ~bbcp_FileInfo() {if (Group) free(Group);}
};

class bbcp_File;

class bbcp_FileSystem
{
public:

// Returns true (1) if the path belongs to the filesystem, false(0) o/w.
//
virtual int        Applicable(const char *path)=0;

// Indicates that direct I/O, if available, should be used. This needs to
// be called prior to open(). Argument 0 turns it off, 1 turns it on.
//
virtual int        DirectIO(int On) {int oIo = dIO; dIO = On; return oIo;}
virtual int        DirectIO()       {return dIO;}

// Returns true (1) if the filesystem has enough space for the number of
// bytes amd the number of files, false (0) otherwise.
//
virtual int        Enough(long long bytes, int numfiles=1)=0;

        long long  FSID() {return fs_id;}

// Return 0 if fsync was successful. Otherwise, -errno. If a filename is passed,
// then an fsync is done on the directory as well.
//
virtual int        Fsync(const char *fn, int fd) {return 0;}

// Returns the appropriate file system object for a path. Null if no filesystem
// can handle the path. This is a static method specific to this class.
//
static const int        getFS_Pipe= 0x0001;

static bbcp_FileSystem *getFS(const char *path, int opts=0);

// Obtain the size of a file and return desired block size
//
virtual long long  getSize(int fd, long long *blksz=0) = 0;

// Opens a file and returns the associated file object. Upon an error, returns
// null with errno set to the error code (see Unix open()).
//
virtual bbcp_File *Open(const char *fn,int opts,int mode=0,const char *fa=0)=0;

// MKDir create a directory identified by path. Upon error, returns -errno.
// Otherwise returns a zero.
//
virtual int        MKDir(const char *path, mode_t mode)=0;

// Returns the path bound to the filesystem
//
char              *Path() {return (fs_path ? fs_path : (char *)"");}

// Removes a file and return 0 upon success or -errno.
//
virtual int        RM(const char *path)=0;

// Set the group information for a file & return 0. Returns -errno upon error.
//
virtual int        setGroup(const char *path, const char *Group)=0;

// Set the mode on a file & return 0. Returns -errno upon error.
//
virtual int        setMode(const char *path, mode_t mode)=0;

// Set access and modification time & return 0. Returns -errno upon error.
//
virtual int        setTimes(const char *path, time_t atime, time_t mtime)=0;

// Returns information about a file or directory in the FileInfo structure and
// zero if all went well. Otherwise return -errno.
//
virtual int        Stat(const char *path, bbcp_FileInfo *finfo=0)=0;

virtual int        Stat(const char *path, const char *dent, int fd,
                        int nolnks=1, bbcp_FileInfo *finfo=0)=0;

              bbcp_FileSystem() : fs_path(0), fs_id(0), secSize(0), dIO(0) {}
virtual      ~bbcp_FileSystem() {if (fs_path) free(fs_path);}

protected:

char        *fs_path;    // Valid path to filesystem
long long    fs_id;      // Unique filesystem identifier
size_t       secSize;
int          dIO;        // Direct I/O requested
};
#endif
