#ifndef __BBCP_LOGFILE_H__
#define __BBCP_LOGFILE_H__
/******************************************************************************/
/*                                                                            */
/*                        b b c p _ L o g F i l e . h                         */
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
  
#include <unistd.h>
#include <sys/uio.h>
#include "bbcp_Timer.h"
#include "bbcp_Stream.h"
#include "bbcp_Pthread.h"

class bbcp_LogFiler;

class bbcp_LogFile
{
friend class bbcp_LogFiler;

public:

       int  Open(const char *fname);

       void Monitor(int fdnum, char *fdname);

static void Record(bbcp_LogFiler *lfP);

            bbcp_LogFile() : Logfd(-1), Logfn(0), Loggers(0) {}
           ~bbcp_LogFile();

private:

bbcp_Mutex     Flog;
int            Logfd;
char          *Logfn;
bbcp_LogFiler *Loggers;
};
#endif
