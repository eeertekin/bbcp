#ifndef __BBCP_IO_H__
#define __BBCP_IO_H__
/******************************************************************************/
/*                                                                            */
/*                               b b c p _ I O                                */
/*                                                                            */
/* (c) 2002 by the Board of Trustees of the Leland Stanford, Jr., University  */
/*      All Rights Reserved. See bbcp_Version.C for complete License Terms    *//*                            All Rights Reserved                             */
/*   Produced by Andrew Hanushevsky for Stanford University under contract    */
/*              DE-AC03-76-SFO0515 with the Department of Energy              */
/******************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <sys/uio.h>
#include "bbcp_Timer.h"

// This is a standard I/O class that is inherited by the Unix File and Net
// classes. For strange filesystems, define a new filesystem type that
// overrides the methods provided in this class. The I/O class is returned
// by the FileSystem class and is particular to each filesystem.
//
  
class bbcp_IO
{
public:

virtual int          Close();

        int          FD() {return iofd;}

virtual long long    ioStats() {return xfrbytes;}

virtual long long    ioStats(double &iotime)
                            {xfrtime.Report(iotime); return xfrbytes;}

virtual void         Log(const char *rk, const char *wk);

virtual int          Seek(long long offv);

virtual ssize_t      Read(char *buff, size_t rdsz);

virtual ssize_t      Read(const struct iovec *iovp, int iovn);

virtual ssize_t      Write(char *buff, size_t wrsz);

virtual ssize_t      Write(char *buff, size_t wrsz, off_t offs);

virtual ssize_t      Write(const struct iovec *iovp, int iovn);

             bbcp_IO(int fd=-1)
                    : RKeyA(0), RKeyZ(0), WKeyA(0), WKeyZ(0),
                      xfrbytes(0), xfrseek(0), iofd(fd) {}
virtual     ~bbcp_IO()          {Close();
                                 if (RKeyA) free(RKeyA);
                                 if (RKeyZ) free(RKeyZ);
                                 if (WKeyA) free(WKeyA);
                                 if (WKeyZ) free(WKeyZ);
                                }
protected:

char      *RKeyA, *RKeyZ, *WKeyA, *WKeyZ;
bbcp_Timer xfrtime;
long long  xfrbytes;
long long  xfrseek;
int        iofd;
};
#endif
