#ifndef __BBCP_NETLOGGER_H__
#define __BBCP_NETLOGGER_H__
/******************************************************************************/
/*                                                                            */
/*                      b b c p _ N e t L o g g e r . h                       */
/*                                                                            */
/* (c) 2002 by the Board of Trustees of the Leland Stanford, Jr., University  */
/*      All Rights Reserved. See bbcp_Version.C for complete License Terms    *//*                            All Rights Reserved                             */
/*   Produced by Andrew Hanushevsky for Stanford University under contract    */
/*              DE-AC03-76-SFO0515 with the Department of Energy              */
/******************************************************************************/

#ifdef NETLOGGER
#include "NetLogger.h"
  
class bbcp_NetLogger
{
public:

int  Close() {if (nl_handle) return NetLoggerClose(nl_handle); return -1;}

int  Emit(const char *key, const char *data, const char *fmt, ...);

int  Flush() {if (nl_handle) return NetLoggerFlush(nl_handle); return -1;}

int  Open(const char *pgm, char *url=0, int opts=0);

int  setOpts(int opts=0) {return openopts |= opts;}

     bbcp_NetLogger() {nl_handle = (NLhandle *)0; openopts = 0;}
    ~bbcp_NetLogger() {Close();}

private:

int       openopts;
NLhandle *nl_handle;
};
#else
  
class bbcp_NetLogger
{
public:

int  Close() {return 0;}

int  Emit(const char *key, const char *data, const char *fmt, ...) {return 0;}

int  Flush() {return 0;}

int  Open(const char *pgm, char *url=0, int opts=0) {return 0;}

int  setOpts(int opts=0) {return 0;}

     bbcp_NetLogger() {}
    ~bbcp_NetLogger() {}

};

#define NL_APPEND 0
#define NL_MEM    0
#endif
#endif
