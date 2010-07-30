/******************************************************************************/
/*                                                                            */
/*                        b b c p _ P r o g M o n . C                         */
/*                                                                            */
/* (c) 2002 by the Board of Trustees of the Leland Stanford, Jr., University  */
/*      All Rights Reserved. See bbcp_Version.C for complete License Terms    *//*                            All Rights Reserved                             */
/*   Produced by Andrew Hanushevsky for Stanford University under contract    */
/*              DE-AC03-76-SFO0515 with the Department of Energy              */
/******************************************************************************/
  
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include "bbcp_Config.h"
#include "bbcp_Debug.h"
#include "bbcp_File.h"
#include "bbcp_Headers.h"
#include "bbcp_ProgMon.h"
#include "bbcp_Timer.h"
#include "bbcp_ZCX.h"

/******************************************************************************/
/*                      G l o b a l   V a r i a b l e s                       */
/******************************************************************************/

extern bbcp_Config   bbcp_Config;

/******************************************************************************/
/*           E x t e r n a l   I n t e r f a c e   R o u t i n e s            */
/******************************************************************************/
  
extern "C"
{
void *bbcp_MonProg(void *pp)
{
     bbcp_ProgMon *pmp = (bbcp_ProgMon *)pp;
     pmp->Monitor();
     return (void *)0;
}
}

/******************************************************************************/
/*                               M o n i t o r                                */
/******************************************************************************/
  
void bbcp_ProgMon::Monitor()
{
   unsigned int  elptime, lasttime, intvtime;
   long long     cxbytes = 0, curbytes, lastbytes = 0;
   int           pdone;
   float         cratio;
   double        xfrtime, xfrtnow;
   bbcp_Timer    etime;
   char          buff[200], tbuff[24], cxinfo[40], *cxip;
   const char    *xtXB, *xaXB;
   int           bewordy = bbcp_Config.Options & bbcp_VERBOSE;

// Determine whether we need to report compression ratio
//
   if (CXp) cxip = cxinfo;
      else cxip = (char *)"";

// Run a loop until we are killed, reporting what we see
//
   etime.Start(); etime.Report(lasttime);
   while(!alldone && CondMon.Wait(wtime))
      {etime.Stop(); etime.Report(elptime);

       curbytes = FSp->Stats();
       pdone = curbytes*100/Tbytes;
       xfrtime = curbytes/(((double)elptime)/1000.0);

       if (CXp)
          {if (!(cxbytes = CXp->Bytes())) cratio = 0.0;
              else cratio = ((float)(curbytes*10/cxbytes))/10.0;
           sprintf(cxinfo, " compressed %.1f", cratio);
          }

       if (bewordy)
          {intvtime = elptime - lasttime;
           xfrtnow = (curbytes-lastbytes)/(((double)intvtime)/1000.0);
           xaXB = bbcp_Config::Scale(xfrtnow);
          }
       lastbytes = curbytes;
       lasttime  = elptime;
       xtXB = bbcp_Config::Scale(xfrtime);

       if (bbcp_Config.Logfn) *tbuff = 0;
          else etime.Format(tbuff);
       if (bewordy)
          sprintf(buff, "bbcp: %s %d%% done; %.1f %sB/s, "
                        "avg %.1f %sB/s%s\n",
                        tbuff,  pdone, xfrtnow, xtXB, xfrtime, xaXB, cxip);
          else
          sprintf(buff, "bbcp: %s %d%% done; %.1f %sB/s%s\n",
                         tbuff,  pdone, xfrtime, xtXB, cxip);
       cerr <<buff <<flush;
      }

// Post the semaphore to indicate we are done. This is a far safer way to
// synchronize exits than using pthread_join().
//
   monDone.Post();
}
  
/******************************************************************************/
/*                                 S t a r t                                  */
/******************************************************************************/
  
void bbcp_ProgMon::Start(bbcp_File *fs_obj, bbcp_ZCX *cx_obj, int pint,
                         long long xfrbytes)
{  int retc;

// If we are already monitoring, issue a stop
//
   if (mytid) Stop();

// Preset all values
//
   alldone = 0;
   FSp     = fs_obj;
   CXp     = cx_obj;
   wtime   = pint;
   Tbytes  = xfrbytes;

// Run a thread to start the monitor
//
   if (retc = bbcp_Thread_Run(bbcp_MonProg, (void *)this, &mytid))
      {DEBUG("Error " <<retc <<" starting progress monitor thread.");}
      else {DEBUG("Thread " <<mytid <<" monitoring progress.");}
   return;
}

/******************************************************************************/
/*                                  S t o p                                   */
/******************************************************************************/
  
void bbcp_ProgMon::Stop()
{

// Simply issue a cancel to the thread running the progress monitor
//
   alldone = 1;
   if (mytid) 
      {DEBUG("Progress monitor " <<mytid <<" stopped.");
       CondMon.Signal(); 
       monDone.Wait();
       mytid = 0;
      }
}
