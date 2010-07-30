/******************************************************************************/
/*                                                                            */
/*                           b b c p _ E m s g . C                            */
/*                                                                            */
/* (c) 2002 by the Board of Trustees of the Leland Stanford, Jr., University  */
/*      All Rights Reserved. See bbcp_Version.C for complete License Terms    *//*                            All Rights Reserved                             */
/*   Produced by Andrew Hanushevsky for Stanford University under contract    */
/*              DE-AC03-76-SFO0515 with the Department of Energy              */
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
