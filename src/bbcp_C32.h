#ifndef __BBCP_C32_H__
#define __BBCP_C32_H__
/******************************************************************************/
/*                                                                            */
/*                            b b c p _ C 3 2 . h                             */
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

typedef unsigned int uint32;
  
class bbcp_C32 : public bbcp_ChkSum
{
public:

void  Init() {C32Result = CRC32_XINIT; TotLen = 0;}

void  Update(const char *Buff, int BLen);

int   csSize() {return sizeof(C32Result);}

char *Final(char **Text=0)
               {char buff[sizeof(long long)];
                long long tLcs = TotLen;
                int i = 0;
                if (tLcs)
                   {while(tLcs) {buff[i++] = tLcs & 0xff ; tLcs >>= 8;}
                    Update(buff, i);
                   }
                TheResult = C32Result ^ CRC32_XOROT;
#ifndef BBCP_BIG_ENDIAN
                TheResult = htonl(TheResult);
#endif
                if (Text) *Text = x2a((char *)&TheResult);
                return (char *)&TheResult;
               }

const char *Type() {return "c32";}

            bbcp_C32() {Init();}
virtual    ~bbcp_C32() {}

private:
static const uint CRC32_XINIT = 0;
static const uint CRC32_XOROT = 0xffffffff;
static       uint crctable[256];
             uint C32Result;
             uint TheResult;
       long  long TotLen;
};
#endif
