#ifndef __BBCP_IO_NULL_H__
#define __BBCP_IO_NULL_H__
/******************************************************************************/
/*                                                                            */
/*                          b b c p _ I O _ N u l l                           */
/*                                                                            */
/* (c) 2002 by the Board of Trustees of the Leland Stanford, Jr., University  */
/*      All Rights Reserved. See bbcp_Version.C for complete License Terms    *//*                            All Rights Reserved                             */
/*   Produced by Andrew Hanushevsky for Stanford University under contract    */
/*              DE-AC03-76-SFO0515 with the Department of Energy              */
/******************************************************************************/

#include "bbcp_IO.h"
  
class bbcp_IO_Null : public bbcp_IO
{
public:

        int          Close() {return 0;}

        int          Seek(long long offv) {return 0;}

        ssize_t      Read(char *buff, size_t rdsz);

        ssize_t      Read(const struct iovec *iovp, int iovn);

        ssize_t      Write(char *buff, size_t wrsz);

        ssize_t      Write(char *buff, size_t wrsz, off_t offs)
                          {return Write(buff, wrsz);}

        ssize_t      Write(const struct iovec *iovp, int iovn);

             bbcp_IO_Null(int fd) : bbcp_IO(fd) {}
            ~bbcp_IO_Null() {}

protected:
};
#endif
