#ifdef NETLOGGER
/******************************************************************************/
/*                                                                            */
/*                      b b c p _ N e t L o g g e r . C                       */
/*                                                                            */
/* (c) 2002 by the Board of Trustees of the Leland Stanford, Jr., University  */
/*      All Rights Reserved. See bbcp_Version.C for complete License Terms    *//*                            All Rights Reserved                             */
/*   Produced by Andrew Hanushevsky for Stanford University under contract    */
/*              DE-AC03-76-SFO0515 with the Department of Energy              */
/******************************************************************************/

#include <unistd.h>
#include <stdarg.h>
#include "bbcp_Headers.h"

#include "bbcp_NetLogger.h"
  
/******************************************************************************/
/*                                  E m i t                                   */
/******************************************************************************/
  
int bbcp_NetLogger::Emit(const char *key, const char *data, const char *fmt, ...)
{
    char buff[1024];
    va_list arg_pointer;
    va_start(arg_pointer, fmt);

    vsnprintf(buff, sizeof(buff), fmt, arg_pointer);
    return NetLoggerWrite(nl_handle, (char *)key, (char *)data, buff) == 1;

    va_end(arg_pointer);
}

/******************************************************************************/
/*                                  O p e n                                   */
/******************************************************************************/

int bbcp_NetLogger::Open(const char *pgm, char *url, int opts)
{
    return (nl_handle = NetLoggerOpen((char *)pgm, url,
           (url ? opts | openopts : opts | openopts | NL_ENV))) != 0;
}
#else
int bbcp_NetLoggerDisabled;
#endif
