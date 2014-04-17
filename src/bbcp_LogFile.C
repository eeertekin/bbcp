/******************************************************************************/
/*                                                                            */
/*                        b b c p _ L o g F i l e . C                         */
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
#include <fcntl.h>
#include <poll.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "bbcp_Debug.h"
#include "bbcp_Emsg.h"
#include "bbcp_Headers.h"
#include "bbcp_LogFile.h"
#include "bbcp_Platform.h"

/******************************************************************************/
/*                         L o c a l   C l a s s e s                          */
/******************************************************************************/
  
class bbcp_LogFiler
{
public:

bbcp_LogFiler *Next;
bbcp_LogFile  *LogF;
char          *Name;
pthread_t      LogT;
int            ErFD;  //  Input file descriptor

      bbcp_LogFiler(bbcp_LogFile *lfP, char *name, int efd)
                        : Next(0), LogF(lfP), Name(strdup(name)), LogT(0),
                          ErFD(efd) {}
     ~bbcp_LogFiler() {if (Name)      free(Name);
                       if (ErFD >= 0) close(ErFD);
                       if (LogT)      bbcp_Thread_Cancel(LogT);
                       if (Next)      delete Next;
                      }
};

/******************************************************************************/
/*           E x t e r n a l   I n t e r f a c e   R o u t i n e s            */
/******************************************************************************/
  
extern "C"
{
void *bbcp_FileLog(void *op)
{
     bbcp_LogFiler *lrP = (bbcp_LogFiler *)op;
     bbcp_LogFile::Record(lrP);
     return (void *)0;
}
}

/******************************************************************************/
/*                            D e s t r u c t o r                             */
/******************************************************************************/
  
bbcp_LogFile::~bbcp_LogFile()
{
   static const char *Eol = "=-=-=-=-=-=-=-=\n";

// Close the log file and delete the log list which will clean everything
//
   if (Logfd >= 0)
      {write(Logfd, Eol, strlen(Eol));
       close(Logfd);
      }
   if (Logfn) free(Logfn);
   delete Loggers;
}

/******************************************************************************/
/*                                  O p e n                                   */
/******************************************************************************/
  
int bbcp_LogFile::Open(const char *fname)
{

// Check if we have a logfile already
//
   if (Logfd >= 0) return -ETXTBSY;

// Open the log file
//
   if ((Logfd = open(fname, O_WRONLY | O_CREAT | O_APPEND | O_DSYNC, 0644)) < 0)
      return bbcp_Emsg("LogFile", -errno, "opening", fname);

// Set up for logging
//
   Logfn = strdup(fname);

// All done
//
   return 0;
}
  
/******************************************************************************/
/*                               M o n i t o r                                */
/******************************************************************************/
  
void bbcp_LogFile::Monitor(int fdnum, char *fdname)
{
   bbcp_LogFiler *lrP = new bbcp_LogFiler(this, fdname, fdnum);
   int retc;

// Start a log file thread (we loose storage upon failure)
//
   if ((retc = bbcp_Thread_Run(bbcp_FileLog, (void *)lrP, &(lrP->LogT))))
      {bbcp_Emsg("LogFile", errno, "start logging thread to", Logfn);
       return;
      }

// Chain this logger into out list of loggers
//
   Flog.Lock();
   lrP->Next = Loggers; Loggers = lrP;
   Flog.UnLock();
   DEBUG("Thread " <<lrP->LogT <<" assigned to logging " <<fdname);
}
 
/******************************************************************************/
/*                                R e c o r d                                 */
/******************************************************************************/

void bbcp_LogFile::Record(bbcp_LogFiler *lrP)
{
   static bbcp_Mutex logMutex;
   bbcp_Timer  Mytime;
   bbcp_Stream inData;
   char *inLine, tbuff[24];
   struct iovec iolist[3] = {{(caddr_t)tbuff, 0}, {0,0}, {(char *)"\n", 1}};
   int LogFD = lrP->LogF->Logfd;

// Attach the file descriptor to the stream
//
   inData.Attach(lrP->ErFD);

// Get a full line from the stream to avoid line splittage in the log and
// write it out to the lof file adding appropriate headers.
//
   while((inLine = inData.GetLine()))
        {if (!(*inLine)) continue;
         logMutex.Lock();
         if (LogFD)
            {tbuff[0] = '\0';
             iolist[0].iov_len   = Mytime.Format(tbuff);
             iolist[1].iov_base  = inLine;
             iolist[1].iov_len   = strlen(inLine);

             if (writev(LogFD, (const struct iovec *)&iolist, 3) < 0)
                {bbcp_Emsg("LogFile",errno,"writing log to",lrP->LogF->Logfn);
                 LogFD = 0;
                }
            } else cerr <<inLine <<endl;
         logMutex.UnLock();
        }
}
