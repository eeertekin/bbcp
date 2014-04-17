#ifndef __BBCP_IO_PIPE_H__
#define __BBCP_IO_PIPE_H__
/******************************************************************************/
/*                                                                            */
/*                          b b c p _ I O _ P i p e                           */
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

#include "bbcp_IO.h"
  
class bbcp_IO_Pipe : public virtual bbcp_IO
{
public:
        int     Close();

        int     Seek(long long offv) {return 0;}

        ssize_t Write(char *buff, size_t wrsz)
                     {return bbcp_IO::Write(buff, wrsz);}

        ssize_t Write(char *buff, size_t wrsz, off_t offs)
                     {return bbcp_IO::Write(buff, wrsz);}

        ssize_t Write(const struct iovec *iovp, int iovn)
                     {return bbcp_IO::Write(iovp, iovn);}

             bbcp_IO_Pipe(int fd, pid_t pnum=0) : bbcp_IO(fd), thePid(pnum) {}
            ~bbcp_IO_Pipe() {}

private:
pid_t thePid;
};
#endif
