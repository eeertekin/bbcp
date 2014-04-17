/******************************************************************************/
/*                                                                            */
/*                           b b c p _ E m s g . C                            */
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
  
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include "bbcp_Platform.h"
#include "bbcp_Headers.h"
#include "bbcp_Debug.h"
#include "bbcp_Emsg.h"

/******************************************************************************/

int bbcp_Emsg(const char *sfx, int ecode, const char *txt1, 
                                   const char *txt2, const char *txt3)
{
    int xcode;
    char ebuff[16], *etxt;

    xcode = (ecode < 0 ? -ecode : ecode);
    etxt = (char *)strerror(xcode);
    if (!strncmp(etxt, "Unknown", 7))
       {snprintf(ebuff, sizeof(ebuff), "Error %d", ecode);
        etxt = ebuff;
       }

    bbcp_Fmsg(sfx, etxt, txt1, txt2, txt3);

    return (ecode ? ecode : -1);
}

/******************************************************************************/

int bbcp_Fmsg(const char *sfx, const char *txt1, const char *txt2,
                               const char *txt3, const char *txt4,
                               const char *txt5, const char *txt6)
{
   const char *bV[] = {txt2, txt3, txt4, txt5, txt6};
   char *bP, buff[2048];
   int i;

   bP = buff + (sfx && bbcp_Debug.Trace
      ? sprintf(buff, "bbcp_%s.%s: %s", bbcp_Debug.Who, sfx, txt1)
      : sprintf(buff, "bbcp: %s", txt1));

   for (i = 0; i < 5 && bV[i]; i++) bP += sprintf(bP, " %s", bV[i]);
   strcpy(bP, "\n");
   cerr <<buff;
   return -1;
}
