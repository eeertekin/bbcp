/******************************************************************************/
/*                                                                            */
/*                         b b c p _ C h k S u m . C                          */
/*                                                                            */
/*(c) 2010-14 by the Board of Trustees of the Leland Stanford, Jr., University*//*                            All Rights Reserved                             */
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
  
#include "bbcp_Config.h"
#include "bbcp_ChkSum.h"
#include "bbcp_A32.h"
#include "bbcp_A32_zlib.h"
#include "bbcp_C32.h"
#include "bbcp_MD5.h"
#include "bbcp_MD5_openssl.h"

/******************************************************************************/
/*                                 A l l o c                                  */
/******************************************************************************/

bbcp_ChkSum *bbcp_ChkSum::Alloc(int csType)
{
// Return correct object
//
   switch(csType)
         {case bbcp_csA32: return (bbcp_ChkSum *)new bbcp_A32_zlib;
          case bbcp_csC32: return (bbcp_ChkSum *)new bbcp_C32;
#if defined(LINUX) || defined(MACOS)
          case bbcp_csMD5: return (bbcp_ChkSum *)new bbcp_MD5_openssl;
#else
          case bbcp_csMD5: return (bbcp_ChkSum *)new bbcp_MD5;
#endif
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
