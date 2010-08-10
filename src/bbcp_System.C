/******************************************************************************/
/*                                                                            */
/*                         b b c p _ S y s t e m . C                          */
/*                                                                            */
/* (c) 2002 by the Board of Trustees of the Leland Stanford, Jr., University  */
/*      All Rights Reserved. See bbcp_Version.C for complete License Terms    *//*                            All Rights Reserved                             */
/*   Produced by Andrew Hanushevsky for Stanford University under contract    */
/*              DE-AC03-76-SFO0515 with the Department of Energy              */
/******************************************************************************/
  
#include <sys/time.h>
#include <sys/times.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <errno.h>
#include <grp.h>
#include <limits.h>
#include <pwd.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "bbcp_Config.h"
#include "bbcp_Debug.h"
#include "bbcp_Emsg.h"
#include "bbcp_Platform.h"
#include "bbcp_System.h"

/******************************************************************************/
/*                  O S   D e p e n d e n t   S e c t i o n                   */
/******************************************************************************/
  
#ifdef SUN
#define NewProc fork1()
#else
#define NewProc fork()
#endif

/******************************************************************************/
/*                        G l o b a l   O b j e c t s                         */
/******************************************************************************/
  
       bbcp_System bbcp_OS;

extern bbcp_Config bbcp_Config;


/******************************************************************************/
/*                           C o n s t r u c t o r                            */
/******************************************************************************/

bbcp_System::bbcp_System()
{
     EnvP     = 0;
     PageSize = sysconf(_SC_PAGESIZE);
#if !defined(FREEBSD) && !defined(MACOS) && !defined(CRAY_X1E) && !defined(AIX)
     FreePag  = sysconf(_SC_AVPHYS_PAGES);
     TotPag   = sysconf(_SC_PHYS_PAGES);

     FreeRAM  = (long long)FreePag * (long long)PageSize;
#else
     FreePag = 0x00ffffff; TotPag = 0x00ffffff; FreeRAM = 0x7fffffff;
#endif
}
  
/******************************************************************************/
/*                                  F o r k                                   */
/******************************************************************************/
  
pid_t bbcp_System::Fork()
{
   pid_t pid = NewProc;

   if (pid == 0) bbcp_Debug.mypid = getpid();

   return pid;
}

/******************************************************************************/
/*                                g e t G I D                                 */
/******************************************************************************/
  
gid_t bbcp_System::getGID(const char *group)
{
   gid_t gid;
   struct group *gp;

// Convert the group name to a gid
//
   Glookup.Lock();
   if (gp = getgrnam(group)) gid = gp->gr_gid;
      else gid = (gid_t)-1;
   Glookup.UnLock();
   return gid;
}

/******************************************************************************/
/*                                g e t G N M                                 */
/******************************************************************************/
  
char *bbcp_System::getGNM(gid_t gid)
{
   char *gnmp;
   struct group *gp;
 
// Get the group name
//
   Glookup.Lock();
   if (gp = getgrgid(gid)) gnmp = gp->gr_name;
      else gnmp = (char *)"nogroup";
   Glookup.UnLock();

// Return a copy of the group name
//
   return strdup(gnmp);
}
 
/******************************************************************************/
/*                            g e t H o m e D i r                             */
/******************************************************************************/
  
char *bbcp_System::getHomeDir()
{
   uid_t myuid;
   struct passwd *pwp;
   char *homedir;

// Get the effective uid
//
   myuid = geteuid();

// Get the password entry for this uid
//
   if (pwp = getpwuid(myuid)) homedir = pwp->pw_dir;
      else homedir = (char *)"/tmp";

// Return a copy of the directory
//
   return strdup(homedir);
}
 
/******************************************************************************/
/*                             g e t G r a n d P                              */
/******************************************************************************/
  
pid_t bbcp_System::getGrandP()
{
    char cmd[256], *lp;
    bbcp_Stream cmdstream;
    int rc, grandpa;

    sprintf(cmd, PS_CMD, getppid());
    if (rc = cmdstream.Exec(cmd)) rc = cmdstream.LastError();
       else if (!cmdstream.GetLine() || !(lp = cmdstream.GetLine())) rc = -33;
               else {errno = 0;
                     grandpa = strtol(lp, (char **)NULL, 10);
                     if (errno) rc = -22;
                    }
     return (!rc && grandpa > 1 && (!kill(grandpa, 0) || ESRCH != errno) ?
            grandpa : (pid_t)0);
}

/******************************************************************************/
/*                                 U s a g e                                  */
/******************************************************************************/

int  bbcp_System::Usage(int   &Scpu,  int   &Ucpu)
{     struct rusage Pfo, Cfo;

      if (getrusage(RUSAGE_CHILDREN,&Cfo)<0 || getrusage(RUSAGE_SELF,&Pfo)<0)
         Scpu = Ucpu = 0;
         else {Ucpu = Pfo.ru_utime.tv_sec *1000 +
                      Pfo.ru_utime.tv_usec/1000 +
                      Cfo.ru_utime.tv_sec *1000 +
                      Cfo.ru_utime.tv_usec/1000;
               Scpu = Pfo.ru_stime.tv_sec *1000 +
                      Pfo.ru_stime.tv_usec/1000 +
                      Cfo.ru_stime.tv_sec *1000 +
                      Cfo.ru_stime.tv_usec/1000;
              }

       return Scpu + Ucpu;
}
  
/******************************************************************************/
/*                              U s e r N a m e                               */
/******************************************************************************/

char *bbcp_System::UserName()
{
   uid_t myuid;
   struct passwd *pwp;
   char *uname;

// Get the effective uid
//
   myuid = geteuid();

// Get the password entry for this uid
//
   if (pwp = getpwuid(myuid)) uname = pwp->pw_name;
      else uname = (char *)"anonymous";

// Return a copy of the directory
//
   return strdup(uname);
}

  
/******************************************************************************/
/*                               W a i t p i d                                */
/******************************************************************************/
  
int bbcp_System::Waitpid(pid_t *pvec, int *ent, int nomsg)
{
   pid_t Pdone;
   int i, retc = 0, sval;
#ifdef AIX
   union wait estat;
#else
   int estat;
#endif

// If we are waiting for only one pid, wait for that one, else wait for any
//
   while(1)
      {do {if (pvec[1]) Pdone = waitpid(     -1, (int *)&estat, 0);
              else      Pdone = waitpid(pvec[0], (int *)&estat, 0);
          }   while((Pdone < 0 && errno == EINTR) 
                 || (Pdone > 0 && WIFSTOPPED(estat)));

       if (Pdone < 0) break;

       i = 0;
       while(pvec[i] && pvec[i] != Pdone) i++;
       if (pvec[i])
          {if (WIFEXITED(estat)) retc = WEXITSTATUS(estat);
               else {retc = 255;
                     if (WIFSIGNALED(estat))
                        sval = WTERMSIG(estat);
                        if (nomsg) retc = (sval ? -sval : -255);
                           else {char pbuff[16], sbuff[16];
                                 sprintf(pbuff, "%d", Pdone);
                                 sprintf(sbuff, "%d", sval);
                                 bbcp_Fmsg("Waitpid", "Copy process", pbuff,
                                 (char *)"was killed via signal", sbuff);
                                }
                    }
           DEBUG("Process " <<Pdone <<" ended; rc=" <<retc);
           if (ent) *ent = i;
           return retc;
          }
      }

// An unknown error occured
//
   DEBUG("Waitpid ended with errno=" <<errno);
   if (ent) *ent = 0; 
   return -1;
}
