/******************************************************************************/
/*                                                                            */
/*                         b b c p _ C h k S u m . C                          */
/*                                                                            */
/* (c) 2010 by the Board of Trustees of the Leland Stanford, Jr., University  */
/*                            All Rights Reserved                             */
/*   Produced by Andrew Hanushevsky for Stanford University under contract    */
/*              DE-AC02-76-SFO0515 with the Department of Energy              */
/******************************************************************************/
  
#include "bbcp_Config.h"
#include "bbcp_ChkSum.h"
#include "bbcp_A32.h"
#include "bbcp_C32.h"
#include "bbcp_MD5.h"

/******************************************************************************/
/*                                 A l l o c                                  */
/******************************************************************************/

bbcp_ChkSum *bbcp_ChkSum::Alloc(int csType)
{
// Return correct object
//
   switch(csType)
         {case bbcp_csA32: return (bbcp_ChkSum *)new bbcp_A32;
          case bbcp_csC32: return (bbcp_ChkSum *)new bbcp_C32;
          case bbcp_csMD5: return (bbcp_ChkSum *)new bbcp_MD5;
          default:         break;
         }

// Unknown checksum
//
   return 0;
}

/******************************************************************************/
/*                                   x 2 a                                    */
/******************************************************************************/
  
char *bbcp_ChkSum::x2a(char *inCS)
{
   static char hv[] = "0123456789abcdef";
   int i, j = 0, csLen = csSize();

// Truncate the checksum if need be
//
   if (csLen*2 >= sizeof(csBuff)) csLen = sizeof(csBuff)*2-1;

// Convert checksum to text
//
     for (i = 0; i < csLen; i++) {
         csBuff[j++] = hv[(inCS[i] >> 4) & 0x0f];
         csBuff[j++] = hv[ inCS[i]       & 0x0f];
         }
     csBuff[j] = '\0';
     return csBuff;
}
