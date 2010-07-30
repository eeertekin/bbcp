#ifndef __BBCP_MD5_H__
#define __BBCP_MD5_H__
/******************************************************************************/
/*                                                                            */
/*                            b b c p _ M D 5 . h                             */
/*                                                                            */
/* (c) 2010 by the Board of Trustees of the Leland Stanford, Jr., University  */
/*      All Rights Reserved. See bbcp_Version.C for complete License Terms    *//*                            All Rights Reserved                             */
/*   Produced by Andrew Hanushevsky for Stanford University under contract    */
/*              DE-AC03-76-SFO0515 with the Department of Energy              */
/******************************************************************************/

#include <string.h>

typedef unsigned int uint32;

/******************************************************************************/
/*                  M D 5 C o n t e x t   S t r u c t u r e                   */
/******************************************************************************/
  
struct MD5Context {
       uint32 buf[4];
       uint32 bits[2];
       unsigned char in[64];
      };
  
/******************************************************************************/
/*                             M D 5   C l a s s                              */
/******************************************************************************/

#include <stdio.h>

#include "bbcp_Headers.h"
#include "bbcp_ChkSum.h"
  
class bbcp_MD5 : public bbcp_ChkSum
{
public:

int   Chk_MD5(char *buff, int  blen, char MD5Val[16])
             {MD5Init(&MyContext);
              MD5Update(&MyContext, (unsigned char *)buff, (unsigned)blen);
              MD5Final(MyDigest, &MyContext);
              return memcmp((const void *)MD5Val, (const void *)MyDigest,
                            (size_t)sizeof(MyDigest));
             }

void  Set_MD5(char *buff, int  blen, char MD5Val[16])
             {MD5Init(&MyContext);
              MD5Update(&MyContext, (unsigned char *)buff, (unsigned)blen);
              MD5Final((unsigned char *)MD5Val, &MyContext);
             }

char *csCurr(char **Text=0)
             {MD5Context currCTX = MyContext;
              MD5Final(MyDigest, &currCTX);
              if (Text) *Text = x2a((char *)MyDigest);
              return (char *)MyDigest;
             }

int   csSize() {return sizeof(MyDigest);}

char *csVal();

void  Init() {MD5Init(&MyContext);}

void  Update(const char *Buff, int BLen)
            {MD5Update(&MyContext, (unsigned char *)Buff, (unsigned)BLen);}

char *Final(char **Text=0)
            {MD5Final(MyDigest, &MyContext);
             if (Text) *Text = x2a((char *)MyDigest);
             return (char *)MyDigest;
            }

const char *Type() {return "md5";}

      bbcp_MD5() {MD5Init(&MyContext);}
     ~bbcp_MD5() {}

private:

MD5Context    MyContext;
unsigned char MyDigest[16];

void byteReverse(unsigned char *buf, unsigned longs);
void MD5Init(struct MD5Context *context);
void MD5Update(struct MD5Context *context, unsigned char const *buf,
               unsigned len);
void MD5Final(unsigned char digest[16], struct MD5Context *context);
void MD5Transform(uint32 buf[4], uint32 const in[16]);
};
#endif
