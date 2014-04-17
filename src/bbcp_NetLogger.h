#ifndef __BBCP_NETLOGGER_H__
#define __BBCP_NETLOGGER_H__
/******************************************************************************/
/*                                                                            */
/*                      b b c p _ N e t L o g g e r . h                       */
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
