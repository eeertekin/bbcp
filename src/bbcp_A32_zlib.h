#ifndef __BBCP_X32_ZLIB_H__
#define __BBCP_X32_ZLIB_H__
/******************************************************************************/
/*                                                                            */
/*                            b b c p _ A 3 2 . h                             */
/*                                                                            */
/* (c) 2010 by the Board of Trustees of the Leland Stanford, Jr., University  */
/*      All Rights Reserved. See bbcp_Version.C for complete License Terms    *//*                            All Rights Reserved                             */
/*   Produced by Andrew Hanushevsky for Stanford University under contract    */
/*              DE-AC03-76-SFO0515 with the Department of Energy              */
/******************************************************************************/

#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <inttypes.h>

#include "bbcp_ChkSum.h"
#include "bbcp_Endian.h"
#include "zlib.h"

typedef unsigned int uint32;

class bbcp_A32_zlib : public bbcp_ChkSum
{
 public:
    
    void  Init() { adler = adler32(0L, Z_NULL, 0); }

    void  Update(const char *Buff, int BLen) {
        adler = adler32(adler, (const Bytef*)Buff, BLen);
    }
    
    int   csSize() {return sizeof(adler);}

    char *Final(char **Text=0) {
       
#ifndef BBCP_BIG_ENDIAN
        adler = htonl(adler);
#endif
        if (Text) *Text = x2a((char *)&adler);
        return (char *)&adler;
    }

    const char *Type() {return "a32";}

    bbcp_A32_zlib() {Init();}
    virtual    ~bbcp_A32_zlib() {}

 private:
    uLong adler;
    
};
#endif
