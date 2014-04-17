#ifndef __BBCP_X32_ZLIB_H__
#define __BBCP_X32_ZLIB_H__
/******************************************************************************/
/*                                                                            */
/*                            b b c p _ A 3 2 . h                             */
/*                                                                            */
/*(c) 2010-14 by the Board of Trustees of the Leland Stanford, Jr., University*//*      All Rights Reserved. See bbcp_Version.C for complete License Terms    *//*                            All Rights Reserved                             */
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
