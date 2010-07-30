#ifndef __BBCP_LOGFILE_H__
#define __BBCP_LOGFILE_H__
/******************************************************************************/
/*                                                                            */
/*                        b b c p _ L o g F i l e . h                         */
/*                                                                            */
/* (c) 2002 by the Board of Trustees of the Leland Stanford, Jr., University  */
/*      All Rights Reserved. See bbcp_Version.C for complete License Terms    *//*                            All Rights Reserved                             */
/*   Produced by Andrew Hanushevsky for Stanford University under contract    */
/*              DE-AC03-76-SFO0515 with the Department of Energy              */
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
