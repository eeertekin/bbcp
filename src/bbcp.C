/******************************************************************************/
/*                                                                            */
/*                                b b c p . C                                 */
/*                                                                            */
/* (c) 2010 by the Board of Trustees of the Leland Stanford, Jr., University  */
/*      All Rights Reserved. See bbcp_Version.C for complete License Terms    */
/*   Produced by Andrew Hanushevsky for Stanford University under contract    */
/*              DE-AC03-76-SFO0515 with the Department of Energy              */
/******************************************************************************/
  
/* bbcp provides a secure fast parallel copy utility. See bbcp_Config.C for
   the actual list of options. The general syntax is:

   bbcp [options] inspec outspec
*/
/******************************************************************************/
/*                         i n c l u d e   f i l e s                          */
/******************************************************************************/
  
#include <unistd.h>
#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <stdio.h>
#include <sys/param.h>

#include "bbcp_Args.h"
#include "bbcp_Config.h"
#include "bbcp_Debug.h"
#include "bbcp_FileSpec.h"
#include "bbcp_Headers.h"
#include "bbcp_LogFile.h"
#include "bbcp_Node.h"
#include "bbcp_ProcMon.h"
#include "bbcp_Protocol.h"
#include "bbcp_System.h"
#include "bbcp_Timer.h"
  
/******************************************************************************/
/*                    L O C A L   D E F I N I T I O N S                       */
/******************************************************************************/

#define Same(s1, s2) (s1 == s2 || (s1 && s2 && !strcmp(s1, s2)))

/******************************************************************************/
/*                      G l o b a l   V a r i a b l e s                       */
/******************************************************************************/

extern bbcp_Config   bbcp_Config;

extern bbcp_BuffPool bbcp_BuffPool;
  
extern bbcp_System   bbcp_OS;

/******************************************************************************/
/*                                  m a i n                                   */
/******************************************************************************/
  
main(int argc, char *argv[], char *envp[])
{
   bbcp_Node     *Source, *Sink;
   bbcp_Protocol  Protocol;
   bbcp_FileSpec *fsp, *psp, *sfs, *tfs;
   int retc;
   int        TotFiles = 0;
   long long  TotBytes = 0;
   double     xRate;
   bbcp_Timer Elapsed_Timer;
   const char *xType;

// Process configuration file
//
   bbcp_OS.EnvP = envp;
   if (bbcp_Config.ConfigInit(argc, argv)) exit(1);

// Process the arguments
//
   bbcp_Config.Arguments(argc, argv);

// Process final source/sink actions here
//
   if (bbcp_Config.Options & (bbcp_SRC | bbcp_SNK))
      {int retc;
       bbcp_ProcMon theAgent;
       theAgent.Start(bbcp_OS.getGrandP());
           {bbcp_Node     SS_Node;
            retc = (bbcp_Config.Options & bbcp_SRC
                 ?  Protocol.Process(&SS_Node)
                 :  Protocol.Request(&SS_Node));
           }
       exit(retc);
      }

// Do some debugging here
//
   Elapsed_Timer.Start();
   if (bbcp_Debug.Trace > 2) bbcp_Config.Display();

// Allocate the source and sink node and common protocol
//
   Source = new bbcp_Node;
   Sink   = new bbcp_Node;
   tfs    = bbcp_Config.snkSpec;

// Allocate the log file
//
   if (bbcp_Config.Logfn)
      {bbcp_Config.MLog = new bbcp_LogFile();
       if (bbcp_Config.MLog->Open(bbcp_Config.Logfn)) exit(5);
      }

// Grab all source files for each particular user/host and copy them
//
   retc = 0;
   while(!retc && (psp = bbcp_Config.srcSpec))
      {fsp = psp->next;
       while(fsp && Same(fsp->username, psp->username)
             && Same(fsp->hostname, psp->hostname)) 
            {psp = fsp; fsp = fsp->next;}
       psp->next = 0;
       sfs = bbcp_Config.srcSpec;
       bbcp_Config.srcSpec = fsp;
         if (bbcp_Config.Options & bbcp_CON2SRC)
            retc = Protocol.Schedule(Source, sfs, 
                                     (char *)bbcp_Config.SrcXeq,
                                     (char *)bbcp_Config.SrcArg,
                                     Sink,   tfs,
                                     (char *)bbcp_Config.SnkXeq,
                                     (char *)bbcp_Config.SnkArg, Sink);
            else
            retc = Protocol.Schedule(Sink,   tfs,
                                     (char *)bbcp_Config.SnkXeq,
                                     (char *)bbcp_Config.SnkArg,
                                     Source, sfs,
                                     (char *)bbcp_Config.SrcXeq,
                                     (char *)bbcp_Config.SrcArg, Sink);
       TotFiles += Sink->TotFiles;
       TotBytes += Sink->TotBytes;
      }

// All done
//
   delete Source;
   delete Sink;

// Report final statistics if wanted
//
   DEBUG("Ending; rc=" <<retc <<" files=" <<TotFiles <<" bytes=" <<TotBytes);
   if (bbcp_Config.Options & bbcp_VERBOSE
   && !retc && TotFiles && TotBytes)
      {double ttime;
       char buff[128];
       Elapsed_Timer.Stop();
       Elapsed_Timer.Report(ttime);
       xRate = ((double)TotBytes)/ttime; xType = bbcp_Config::Scale(xRate);
       sprintf(buff, "%.1f %sB/s", xRate, xType);
       cerr <<TotFiles <<(TotFiles != 1 ? " files" : " file");
       cerr <<" copied at effectively " <<buff <<endl;
      }
   if (bbcp_Config.MLog) delete bbcp_Config.MLog;
   exit(retc);
}
