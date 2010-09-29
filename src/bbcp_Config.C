/******************************************************************************/
/*                                                                            */
/*                         b b c p _ C o n f i g . C                          */
/*                                                                            */
/* (C) 2002 by the Board of Trustees of the Leland Stanford, Jr., University  */
/*      All Rights Reserved. See bbcp_Version.C for complete License Terms    *//*                            All Rights Reserved                             */
/*   Produced by Andrew Hanushevsky for Stanford University under contract    */
/*               DE-AC03-76-SFO0515 with the Deprtment of Energy              */
/******************************************************************************/

/*
   The routines in this file handle oofs() initialization. They get the
   configuration values either from configuration file or oofs_config.h (in that
   order of precedence).

   These routines are thread-safe if compiled with:
   AIX: -D_THREAD_SAFE
   SUN: -D_REENTRANT
*/
  
#include <unistd.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <netdb.h>
#include <signal.h>
#include <stdlib.h>
#include <strings.h>
#include <stdio.h>
#include <sys/param.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <sys/uio.h>

#define  BBCP_CONFIG_DEBUG
#define  BBCP_IOMANIP
#include "bbcp_Platform.h"
#include "bbcp_Args.h"
#include "bbcp_Config.h"
#include "bbcp_Debug.h"
#include "bbcp_Emsg.h"
#include "bbcp_Headers.h"
#include "bbcp_NetLogger.h"
#include "bbcp_Network.h"
#include "bbcp_Platform.h"
#include "bbcp_Stream.h"
#include "bbcp_System.h"

#include "bbcp_Version.h"

/******************************************************************************/
/*                               d e f i n e s                                */
/******************************************************************************/

#define TS_Xeq(x,m)   if (!strcmp(x,var)) return m(Config);

#define TS_Str(x,m)   if (!strcmp(x,var)) {free(m); m = strdup(val); return 0;}

#define TS_Chr(x,m)   if (!strcmp(x,var)) {m = val[0]; return 0;}

#define TS_Bit(x,m,v) if (!strcmp(x,var)) {m |= v; return 0;}

/******************************************************************************/
/*                        G l o b a l   O b j e c t s                         */
/******************************************************************************/

       bbcp_Config    bbcp_Config;

       bbcp_Debug     bbcp_Debug;

       bbcp_NetLogger bbcp_NetLog;

extern bbcp_Network   bbcp_Net;

extern bbcp_System    bbcp_OS;

extern bbcp_Version   bbcp_Version;
  
/******************************************************************************/
/*                           C o n s t r u c t o r                            */
/******************************************************************************/

bbcp_Config::bbcp_Config()
{

   SrcBuff   = 0;
   SrcBase   = 0;
   SrcUser   = 0;
   SrcHost   = 0;
   SrcBlen   = 0;
   srcPath   = 0;
   srcSpec   = 0;
   srcLast   = 0;
   snkSpec   = 0;
   CBhost    = 0;
   CBport    = 0;
   CopyOpts  = 0;
   LogSpec   = 0;
   RepSpec   = 0;
   bindtries = 1;
   bindwait  = 0;
   Options   = 0;
   Mode      = S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH;
   ModeD     = 0;
   ModeDC    = Mode   |S_IXUSR|S_IXGRP|S_IXOTH;
   BAdd      = 0;
   Bfact     = 0;
   BNum      = 0;
   Streams   = 4;
   Xrate     = 0;
   Complvl   = 1;
   Progint   = 0;
   RWBsz     = 0;
// SrcXeq    = strdup("ssh -x -a -oFallBackToRsh=no -oServerAliveInterval=10 "
   SrcXeq    = strdup("ssh -x -a -oFallBackToRsh=no "
                      "%I -l %U %H bbcp");
// SnkXeq    = strdup("ssh -x -a -oFallBackToRsh=no -oServerAliveInterval=10 "
   SnkXeq    = strdup("ssh -x -a -oFallBackToRsh=no "
                      "%I -l %U %H bbcp");
   Logurl    = 0;
   Logfn     = 0;
   MLog      = 0;
   MyAddr    = bbcp_Net.FullHostName((char *)0, 1);
   MyHost    = bbcp_Net.FullHostName((char *)0);
   MyUser    = bbcp_OS.UserName();
   MyProg    = 0;
   SecToken  = Rtoken();
   MaxSegSz  = bbcp_Net.MaxSSize();
   Wsize     = 131072;
   MaxWindow = 0;
   lastseqno = 0;
   SrcArg    = "SRC";
   SnkArg    = "SNK";
   CKPdir    = 0;
   IDfn      = 0;
   TimeLimit = 0;
   MTLevel   = 0;
   csOpts    = 0;
   csSize    = 16;
   csType    = bbcp_csMD5;
   csSpec    = 0;
  *csString  = 0;
   csPath    = 0;
   csFD      = -1;
   rtCheck   = 3;
   rtLimit   = 0;
   rtLockd   = -1;
   rtLockf   = 0;
   ubSpec[0] = ' '; ubSpec[1] = ' '; ubSpec[2] = 0;
}

/******************************************************************************/
/*                            D e s t r u c t o r                             */
/******************************************************************************/

bbcp_Config::~bbcp_Config()
{
   bbcp_FileSpec *nsp, *fsp;

// Delete all file spec objects chained from us
//
   fsp = srcPath;
   while(fsp) {nsp = fsp->next; delete fsp; fsp = nsp;}
   fsp = srcSpec;
   while(fsp) {nsp = fsp->next; delete fsp; fsp = nsp;}
   fsp = snkSpec;
   while(fsp) {nsp = fsp->next; delete fsp; fsp = nsp;}

// Delete all character strings
//
   if (SrcBuff)  free(SrcBuff);
   if (SrcXeq)   free(SrcXeq);
   if (SnkXeq)   free(SnkXeq);
   if (Logurl)   free(Logurl);
   if (Logfn)    free(Logfn);
   if (LogSpec)  free(LogSpec);
   if (CBhost)   free(CBhost);
   if (CopyOpts) free(CopyOpts);
   if (SecToken) free(SecToken);
   if (MyHost)   free(MyHost);
   if (MyUser)   free(MyUser);
   if (MyProg)   free(MyProg);
   if (CKPdir)   free(CKPdir);
   if (csSpec)   free(csSpec);
   if (rtLockf)  free(rtLockf);
}
/******************************************************************************/
/*                             A r g u m e n t s                              */
/******************************************************************************/

#define Add_Opt(x) {cbp[0]=' '; cbp[1]='-'; cbp[2]=x; cbp[3]='\0'; cbp+=3;}
#define Add_Num(x) {cbp[0]=' '; cbp=n2a(x,&cbp[1]);}
#define Add_Nul(x) {cbp[0]=' '; cbp=n2a(x,&cbp[1],"=%d");}
#define Add_Nup(x) {cbp[0]=' '; cbp=n2a(x,&cbp[1],"+%d");}
#define Add_Oct(x) {cbp[0]=' '; cbp=n2a(x,&cbp[1],"%o");}
#define Cat_Oct(x) {            cbp=n2a(x,&cbp[0],"%o");}
#define Add_Str(x) {cbp[0]=' '; strcpy(&cbp[1], x); cbp+=strlen(x)+1;}

#define bbcp_VALIDOPTS (char *)"-a.B:b:C:c.d:DeE:fFhi:I:kKl:L:m:nopP:q:rR.s:S:t:T:u:U:vVw:W:x:z"
#define bbcp_SSOPTIONS bbcp_VALIDOPTS "MH:Y:"

#define Hmsg1(a)   {bbcp_Fmsg("Config", a);    help(1);}
#define Hmsg2(a,b) {bbcp_Fmsg("Config", a, b); help(1);}

/******************************************************************************/

void bbcp_Config::Arguments(int argc, char **argv, int cfgfd)
{
   bbcp_FileSpec *lfsp;
   int n, retc, xTrace = 1, infiles = 0, notctl = 0, rwbsz = 0;
   char *Slash, *inFN=0, c, cbhname[MAXHOSTNAMELEN+1];
   bbcp_Args arglist((char *)"bbcp: ");

// Make sure we have at least one argument
//
   if (argc < 2) Hmsg1("Copy arguments not specified.");
   notctl = (Options & (bbcp_SRC | bbcp_SNK));

// Establish valid options and the source of those options
//
   if (notctl)            arglist.Options(bbcp_SSOPTIONS, STDIN_FILENO);
      else if (cfgfd < 0) arglist.Options(bbcp_VALIDOPTS, argc-1, ++argv);
              else        arglist.Options(bbcp_SSOPTIONS, cfgfd, 1);

// Process the options
//
   while (c=arglist.getopt())
     { switch(c)
       {
       case 'a': Options |= bbcp_APPEND | bbcp_ORDER;
                 if (CKPdir) {free(CKPdir); CKPdir = 0;}
                 if (arglist.argval) CKPdir = strdup(arglist.argval);
                 break;
       case 'B': if (a2sz("window size", arglist.argval,
                         rwbsz, 1024, ((int)1)<<30))
                    Cleanup(1, argv[0], cfgfd);
                 break;
       case 'b': retc = (*arglist.argval == '+'
                      ? a2n("additional buff",arglist.argval,BAdd ,1,4096)
                      : a2n("blocking factor",arglist.argval,Bfact,1,IOV_MAX));
                 if (retc) Cleanup(1, argv[0], cfgfd);
                 break;
       case 'c': if (arglist.argval)
                    {if (a2n("compression level",arglist.argval,Complvl,0,9))
                        Cleanup(1, argv[0], cfgfd);
                    } else Complvl = 1;
                 if (Complvl) Options |=  bbcp_COMPRESS;
                    else      Options &= ~bbcp_COMPRESS;
                 break;
       case 'C': if (Configure(arglist.argval)) Cleanup(1, argv[0], cfgfd);
                 break;
       case 'd': if (SrcBuff) free(SrcBuff);
                 if (Options & (bbcp_SRC | bbcp_SNK))
                    {SrcBase = SrcBuff = strdup(arglist.argval);
                     SrcBlen = strlen(SrcBase);
                     SrcUser = SrcHost = 0;
                    } else ParseSB(arglist.argval);
                 break;
       case 'D': Options |= bbcp_TRACE;
                 break;
       case 'e': csOpts  |= bbcp_csDashE|bbcp_csLink;
                 break;
       case 'E': if (csSpec) free(csSpec);
                 csSpec = strdup(arglist.argval);
                 break;
       case 'f': Options |= bbcp_FORCE;
                 break;
       case 'F': Options |= bbcp_NOSPCHK;
                 break;
       case 'h': help(0);
                 break;
       case 'H': if (CBhost) free(CBhost);
                 if ((CBport = HostAndPort("callback",arglist.argval,
                                   cbhname, sizeof(cbhname))) < 0) exit(1);
                 CBhost = strdup(cbhname);
                 break;
       case 'i': if (IDfn) free(IDfn);
                 IDfn = strdup(arglist.argval);
                 break;
       case 'I': if (inFN) free(inFN);
                 inFN = strdup(arglist.argval);
                 break;
       case 'k': Options |= bbcp_KEEP | bbcp_ORDER;
                 break;
       case 'K': Options |= bbcp_NOUNLINK;
                 break;
       case 'l': if (Logfn) free(Logfn);
                 Logfn = strdup(arglist.argval);
                 break;
       case 'L': if (LogOpts(arglist.argval)) Cleanup(1, argv[0], cfgfd);
                 break;
       case 'm': if ((Slash = index(arglist.argval, '/')))
                    {*Slash = '\0';
                     if (a2o("dirmode", arglist.argval, ModeD, 1, 07777))
                        Cleanup(1, argv[0], cfgfd);
                     if (*(Slash+1) && a2o("mode", Slash+1, Mode, 1, 07777))
                        Cleanup(1, argv[0], cfgfd);
                    } else if (a2o("mode", arglist.argval, Mode, 1, 07777))
                              Cleanup(1, argv[0], cfgfd);
                 break;
       case 'M': Options |= bbcp_OUTDIR;
                 break;
       case 'n': Options |= bbcp_NODNS;
                 break;
       case 'o': Options |= bbcp_ORDER;
                 break;
       case 'p': Options |= bbcp_PCOPY;
                 break;
       case 'P': if (a2n("seconds",arglist.argval,Progint,BBCP_MINPMONSEC,-1))
                    Cleanup(1, argv[0], cfgfd);
                 break;
       case 'q': if (a2n("qos",arglist.argval,n,0, 255))
                    Cleanup(1, argv[0], cfgfd);
                 bbcp_Net.QoS(n);
                 break;
       case 'r': Options |= bbcp_RECURSE;
                 break;
       case 'R': Options |= bbcp_RTCOPY;
                 if (rtSpec) free(rtSpec);
                 rtSpec = (arglist.argval ? strdup(arglist.argval) : 0);
                 break;
       case 's': if (a2n("streams", arglist.argval,
                         Streams,1,BBCP_MAXSTREAMS)) Cleanup(1, argv[0], cfgfd);
                 break;
       case 'S': if (SrcXeq) free(SrcXeq);
                 SrcXeq = strdup(arglist.argval);
                 break;
       case 't': if (a2sz("time limit", arglist.argval,
                         TimeLimit,1,((int)1)<<30)) Cleanup(1, argv[0], cfgfd);
                 break;
       case 'T': if (SnkXeq) free(SnkXeq);
                 SnkXeq = strdup(arglist.argval);
                 break;
       case 'v': Options |= bbcp_VERBOSE;
                 if (xTrace < 2) xTrace = 2;
                 break;
       case 'V': Options |= (bbcp_BLAB | bbcp_VERBOSE); xTrace = 3;
                 break;
       case 'u': if (Unbuff(arglist.argval)) Cleanup(1, argv[0], cfgfd);
                 break;
       case 'U': if (a2sz("forced window size", arglist.argval,
                         Wsize, 8192, ((int)1)<<30)) Cleanup(1, argv[0], cfgfd);
                 MaxWindow = Wsize;
                 break;
       case 'W': // Backward compatibility
       case 'w': if (*arglist.argval == '=')
                    {Options |= bbcp_MANTUNE; arglist.argval++;}
                 if (a2sz("window size", arglist.argval,
                         Wsize, 8192, ((int)1)<<30)) Cleanup(1, argv[0], cfgfd);
                 break;
       case 'x': if (a2sz("transfer rate", arglist.argval,
                         Xrate,1024,((int)1)<<30)) Cleanup(1, argv[0], cfgfd);
                 break;
       case 'Y': if (SecToken) free(SecToken);
                 SecToken = strdup(arglist.argval);
                 break;
       case 'z': Options |= bbcp_CON2SRC;
                 break;
       case '-': break;
       default:  if (!notctl)
                    {if (cfgfd < 0) help(255);
                     Cleanup(1, argv[0], cfgfd);
                    }
       }
     }

// If we were processing the configuration file, return
//
   if (!notctl && cfgfd >= 0) return;

// If there is a checksum specification, process it now
//
   if (csSpec && EOpts(csSpec)) Cleanup(1, argv[0], cfgfd);

// If there is a realtime specification, process it now
//
   if (rtSpec && ROpts(rtSpec)) Cleanup(1, argv[0], cfgfd);

// Enforce the order option if we need to compute checksum at the destination
//
   if ((csOpts & bbcp_csVerOut) || (Options & bbcp_COMPRESS))
      Options |= bbcp_ORDER;

// Check for options mutually exclsuive with '-a'
//
   if ((Options & bbcp_APPEND) && (*csString || (csOpts & bbcp_csPrint)))
      {bbcp_Fmsg("Config", "-a and -E ...= are mutually exclusive.");
       Cleanup(1, argv[0], cfgfd);
      }

// Check for options mutually exclusice with '-r'
//
   if ((Options & bbcp_RECURSE) && *csString)
      {bbcp_Fmsg("Config", "'-E ...=' and -r are mutually exclusive.");
           Cleanup(1, argv[0], cfgfd);
      }

// Check for options mutually exclusive with '-k'
//
   if ((Options & bbcp_KEEP) && (Options & bbcp_NOUNLINK))
      {bbcp_Fmsg("Config", "-k and -K are mutually exclusive.");
       Cleanup(1, argv[0], cfgfd);
      }

// Check for options mutually exclusive with '-R'
//
   if ((Options & bbcp_RTCOPY) && (Options & bbcp_IDIO))
      {bbcp_Fmsg("Config", "-R and '-u s' are mutually exclusive.");
       Cleanup(1, argv[0], cfgfd);
      }

// Get all of the filenames in the input file list (only on agent)
//
   if (inFN)
      {int infd;
       bbcp_Stream inStream;
       char *lp;
       if ((infd = open(inFN, O_RDONLY)) < 0)
          {bbcp_Emsg("Config", errno, "opening", inFN); exit(2);}
       inStream.Attach(infd);
       while ((lp = inStream.GetLine()) && (*lp != '\0'))
             {lfsp = srcLast;
              srcLast = new bbcp_FileSpec;
              if (lfsp) lfsp->next = srcLast;
                 else srcSpec = srcLast;
              srcLast->Parse(lp);
              if (srcLast->username && !srcLast->hostname)
                  Hmsg2("Missing host name for user", srcLast->username);
              if (srcLast->hostname && !srcLast->pathname)
                  Hmsg1("Piped I/O incompatible with hostname specification.");
              infiles++;
             }
       inStream.Close(); free(inFN);
      }

// Get all of the file names
//
   
   while(arglist.getarg(notctl))
        {lfsp = srcLast;
         srcLast = new bbcp_FileSpec;
         if (lfsp) lfsp->next = srcLast;
            else srcSpec = srcLast;
         srcLast->Parse(arglist.argval);
         if (srcLast->username && !srcLast->hostname)
            Hmsg2("Missing host name for user", srcLast->username);
         if (srcLast->hostname && !srcLast->pathname)
            Hmsg1("Piped I/O incompatible with hostname specification.");
         infiles++;
        }

// Perform sanity checks based on who we are
//
        if (Options & bbcp_SRC)
           {if (!srcSpec)
               {bbcp_Fmsg("Config", "Source file not specified."); exit(3);}
            Options &= ~bbcp_RTCSNK;
           }
   else if (Options & bbcp_SNK)
           {if (infiles > 1)
               {bbcp_Fmsg("Config", "Ambiguous sink data target."); exit(3);}
            if (!(snkSpec = srcSpec))
               {bbcp_Fmsg("Config", "Target file not specified."); exit(3);}
            srcSpec = srcLast = 0;
            Options &= ~bbcp_RTCSRC;
           }
   else    {     if (infiles == 0) Hmsg1("Copy source not specified.")
            else if (infiles == 1) Hmsg1("Copy target not specified.")
            else if (infiles >  2) Options |= bbcp_OUTDIR;
            snkSpec  = srcLast;
            lfsp->next = 0;
            srcLast = lfsp;
           }

// Set appropriate debugging level
//
   if (Options & bbcp_TRACE) bbcp_Debug.Trace = xTrace;

// Do Source/Sink configuration or assemble the final options
//
   if (notctl) Config_Xeq(rwbsz);
      else     Config_Ctl(rwbsz);

// Get the correct directory creation mode setting
//
   if (!ModeD) ModeD = ModeDC;
      else if ((ModeD & S_IRWXU) == S_IRWXU) ModeDC = ModeD;

// Set checkpoint directory if we do not have one here and must have one
//
   if (!CKPdir && Options & bbcp_APPEND && Options & bbcp_SNK)
      {const char *ckpsfx = "/.bbcp";
       char *homedir = bbcp_OS.getHomeDir();
       CKPdir = (char *)malloc(strlen(homedir) + sizeof(ckpsfx) + 1);
       strcpy(CKPdir, homedir); strcat(CKPdir, ckpsfx);
       if (mkdir(CKPdir, 0644) && errno != EEXIST)
          {bbcp_Emsg("Config",errno,"creating restart directory",CKPdir);
           exit(100);
          }
       DEBUG("Restart directory is " <<CKPdir);
      }

// Compute proper MT level
//
   MTLevel = Streams + (Complvl ?  (Options & bbcp_SNK  ? 3 : 2) : 1)
                     + (Progint && (Options & bbcp_SNK) ? 1 : 0) + 2;
   DEBUG("Optimum MT level is " <<MTLevel);

// All done
//
   return;
}

/******************************************************************************/
/*                                  h e l p                                   */
/******************************************************************************/

#define H(x)    cerr <<x <<endl;
#define I(x)    cerr <<'\n' <<x <<endl;

/******************************************************************************/
  
void bbcp_Config::help(int rc)
{
H("Usage:   bbcp [Options] [Inspec] Outspec")
I("Options: [-a [dir]] [-b [+]bf] [-B bsz] [-c [lvl]] [-C cfn] [-D] [-d path]")
H("         [-e] [-E csa] [-f] [-F] [-h] [-i idfn] [-I slfn] [-k] [-K]")
H("         [-L opts[@logurl]] [-l logf] [-m mode] [-p] [-P sec] [-r] [-R [args]]")
H("         [-q qos] [-s snum] [-S srcxeq] [-T trgxeq] [-t sec] [-v] [-V]")
H("         [-u loc] [-U wsz] [-w [=]wsz] [-x rate] [-z] [--]")
I("I/Ospec: [user@][host:]file")
if (rc) exit(rc);
I("Function: Secure and fast copy utility.")
I("-a dir  append mode to restart a previously failed copy.")
H("-b bf   sets the read blocking factor (default is 1).")
H("-b +bf  adds additional output buffers to mitigate ordering stalls.")
H("-B bsz  sets the read/write I/O buffer size (default is wsz).")
H("-c lvl  compress data before sending across the network (default lvl is 1).")
H("-C cfn  process the named configuration file at time of encounter.")
H("-d path requests relative source path addressing and target path creation.")
H("-D      turns on debugging.")
H("-e      error check data for transmission errors using md5 checksum.")
H("-E csa  specify checksum alorithm and optionally report or verify checksum.")
H("        csa: [%]{a32|c32|md5}[=[<value> | <outfile>]]")
H("-f      forces the copy by first unlinking the target file before copying.")
H("-F      does not check to see if there is enough space on the target node.")
H("-h      print help information.")
H("-i idfn is the name of the ssh identify file for source and target.")
H("-I slfn is the name of the file that holds the list of files to be copied.")
H("        With -I no source files need be specified on the command line.")
H("-k      keep the destination file even when the copy fails.")
H("-K      do not rm the file when -f specified, only truncate it.")
H("-l logf logs standard error to the specified file.")
H("-L args sets the logginng level and log message destination.")
H("-m mode target file mode as [dmode/][fmode] but one mode must be present.")
H("        Default dmode is 0755 and fmode is 0644 or it comes via -p option.")
H("-s snum number of network streams to use (default is 4).")
H("-p      preserve source mode, ownership, and dates.")
H("-P sec  produce a progress message every sec seconds (15 sec minimum).")
H("-q lvl  specifies the quality of service for routers that support QOS.")
H("-r      copy subdirectories and their contents (actual files only).")
H("-R args enables real-time copy where args specific handling options.")
H("-S cmd  command to start bbcp on the source node.")
H("-T cmd  command to start bbcp on the target node.")
H("-t sec  sets the time limit for the copy to complete.")
H("-v      verbose mode (provides per file transfer rate).")
H("-V      very verbose mode (excruciating detail).")
H("-u loc  use unbuffered I/O at source or target, if possible.")
H("        loc: s | t | st")
H("-U wsz  unnegotiated window size (sets maximum and default for all parties).")
H("-w wsz  desired window size for transmission (the default is 128K).")
H("        Prefixing wsz with '=' disables autotuning in favor of a fixed wsz.")
H("-x rate is the maximum transfer rate allowed in bytes, K, M, or G.")
H("-z      use reverse connection protocol (i.e., target to source).")
H("--      allows an option with a defaulted optional arg to appear last.")
I("user    the user under which the copy is to be performed. The default is")
H("        to use the current login name.")
H("host    the location of the file. The default is to use the current host.")
H("Inspec  the name of the source file(s) (also see -I).")
H("Outspec the name of the target file or directory (required if >1 input file).")
I(bbcp_Version.Version)
exit(rc);
}
  
/******************************************************************************/
/*                            C o n f i g I n i t                             */
/******************************************************************************/
  
int bbcp_Config::ConfigInit(int argc, char **argv)
{
/*
  Function: Establish default values using a configuration file.

  Input:    None.

  Output:   0 upon success or !0 otherwise.
*/
   char *homedir, *cfn = (char *)"/.bbcp.cf";
   int  retc;
   char *ConfigFN;

// Make sure we have at least one argument to determine who we are
//
   if (argc >= 2)
           if (!strcmp(argv[1], SrcArg))
            {Options |= bbcp_SRC; bbcp_Debug.Who = (char *)"SRC"; return 0;}
      else if (!strcmp(argv[1], SnkArg))
            {Options |= bbcp_SNK; bbcp_Debug.Who = (char *)"SNK"; return 0;}
      else MyProg = strdup(argv[0]);

// Use the config file, if present
//
   if (ConfigFN = getenv("bbcp_CONFIGFN"))
      {if (retc = Configure(ConfigFN)) return retc;}
      else {
      // Use configuration file in the home directory, if any
      //
      struct stat buf;
      homedir = bbcp_OS.getHomeDir();
      ConfigFN = (char *)malloc(strlen(homedir) + strlen(cfn) + 1);
      strcpy(ConfigFN, homedir); strcat(ConfigFN, cfn);
      if (stat(ConfigFN, &buf))
         {retc = 0; free(ConfigFN); ConfigFN = 0;}
         else if (retc = Configure(ConfigFN)) return retc;
     }

// Establish the FD limit
//
   {struct rlimit rlim;
    if (getrlimit(RLIMIT_NOFILE, &rlim) < 0)
       bbcp_Emsg("Config",-errno,"getting FD limit");
       else {rlim.rlim_cur=(rlim.rlim_max == RLIM_INFINITY ? 255:rlim.rlim_max);
             if (setrlimit(RLIMIT_NOFILE, &rlim) < 0)
                 bbcp_Emsg("config", errno, "setting FD limit");
            }
   }

// Ignore sigpipe
//
   signal(SIGPIPE, SIG_IGN);

// All done
//
   return retc;
}
  
/******************************************************************************/
/*                             C o n f i g u r e                              */
/******************************************************************************/
  
int bbcp_Config::Configure(const char *cfgFN)
{
/*
  Function: Establish default values using a configuration file.

  Input:    None.

  Output:   0 upon success or !0 otherwise.
*/
   static int depth = 3;
          int  cfgFD;

// Check if we are recursing too much
//
   if (!(depth--))
      return bbcp_Fmsg("Config", "Too many embedded configuration files.");

// Try to open the configuration file.
//
   if ( (cfgFD = open(cfgFN, O_RDONLY, 0)) < 0)
      return bbcp_Emsg("Config",errno,"opening config file",cfgFN);

// Process the arguments from the config file
//
   Arguments(2, (char **)&cfgFN, cfgFD);
   return 0;
}

/******************************************************************************/
/*                               D i s p l a y                                */
/******************************************************************************/
  
void bbcp_Config::Display()
{

// Display the option values (used during debugging)
//
   cerr <<"badd " <<BAdd <<endl;
   cerr <<"bfact " <<Bfact <<endl;
   cerr <<"bsize " <<RWBsz <<endl;
   cerr <<"bindtries " <<bindtries <<" bindwait " <<bindwait <<endl;
   if (CBhost)
   cerr <<"cbhost:port " <<CBhost <<":" <<CBport <<endl;
   if (CKPdir)
   cerr <<"ckpdir " <<CKPdir <<endl;
   if (Complvl)
   cerr <<"compress " <<Complvl <<endl;
   if (CopyOpts)
   cerr <<"copyopts " <<CopyOpts <<endl;
   if (IDfn)
   cerr <<"idfn " <<IDfn <<endl;
   if (Logfn)
   cerr <<"logfn " <<Logfn <<endl;
   cerr <<"mode " <<oct <<Mode <<dec <<endl;
   cerr <<"myhost " <<MyHost <<endl;
   cerr <<"myuser " <<MyUser <<endl;
   cerr <<"ssectoken " <<SecToken <<endl;
   cerr <<"streams " <<Streams <<endl;
   cerr <<"wsize " <<Wsize <<" maxwindow " <<MaxWindow  <<" maxss " <<MaxSegSz <<endl;
   cerr <<"srcxeq " <<SrcXeq <<endl;
   cerr <<"trgxeq " <<SnkXeq <<endl;
   if (Xrate)
   cerr <<"xfr " <<Xrate <<endl;
}

/******************************************************************************/
/*                                 S c a l e                                  */
/******************************************************************************/

const char *bbcp_Config::Scale(double &xVal)
{
    static const double Kilo = 1024.0;

    if (xVal < Kilo) return "";
    xVal /= Kilo;
    if (xVal < Kilo) return "K";
    xVal /= Kilo;
    if (xVal < Kilo) return "M";
    xVal /= Kilo;
    if (xVal < Kilo) return "G";
    xVal /= Kilo;
    if (xVal < Kilo) return "T";
    xVal /= Kilo;
    if (xVal < Kilo) return "E";
    xVal /= Kilo;
    if (xVal < Kilo) return "Z";
    xVal /= Kilo;
    if (xVal < Kilo) return "Y";
    return "";
}
 
/******************************************************************************/
/*                                 W A M s g                                  */
/******************************************************************************/
  
void bbcp_Config::WAMsg(const char *who, const char *act, int newsz)
{
    char buff[128];
    sprintf(buff, "to %d", newsz);
    bbcp_Fmsg(who, act, buff, "bytes.");
}

/******************************************************************************/
/*                     p r i v a t e   f u n c t i o n s                      */
/******************************************************************************/
/******************************************************************************/
/*                            C o n f i g _ C t l                             */
/******************************************************************************/

void bbcp_Config::Config_Ctl(int rwbsz)
{
   char cbuff[4096], *cbp = cbuff;
   int n;

// If a checksum output file exists, open it now
//
   if (csPath)
      {if ((csFD = open(csPath, O_CREAT|O_APPEND|O_WRONLY, 0644)) < 0)
          {bbcp_Emsg("Config", errno, "opening checksum file", csPath);
           exit(2);
          }
      }

// Now generate all the options we will send to the source and sink
//
   if (Options & bbcp_APPEND)    Add_Opt('a');
   if (CKPdir)                                 Add_Str(CKPdir);
   if (BAdd)                    {Add_Opt('b'); Add_Nup(BAdd);}
   if (Bfact)                   {Add_Opt('b'); Add_Num(Bfact);}
   if (rwbsz)                   {Add_Opt('B'); Add_Num(rwbsz);}
   if (Options & bbcp_COMPRESS) {Add_Opt('c'); Add_Num(Complvl);}
   if (SrcBase)                 {Add_Opt('d'); Add_Str(SrcBase);}
   if (Options & bbcp_TRACE)     Add_Opt('D');
   if (csOpts  & bbcp_csDashE)   Add_Opt('e');
   if (Options & bbcp_FORCE)     Add_Opt('f');
   if (Options & bbcp_NOSPCHK)   Add_Opt('F');
   if (Options & bbcp_KEEP)      Add_Opt('k');
   if (LogSpec)                 {Add_Opt('L'); Add_Str(LogSpec);}
                                 Add_Opt('m');
   if (ModeD)                   {              Add_Oct(ModeD); *cbp++ ='/';
                                               Cat_Oct(Mode);
                                }
      else                                     Add_Oct(Mode);
   if (Options & bbcp_OUTDIR || (Options & bbcp_RELATIVE && SrcBase))
                                 Add_Opt('M');
// if (Options & bbcp_NODNS)     Add_Opt('n');
   if (Options & bbcp_ORDER)     Add_Opt('o');
   if (Options & bbcp_PCOPY)     Add_Opt('p');
   if (Progint)                 {Add_Opt('P'); Add_Num(Progint);}
   if ((n = bbcp_Net.QoS()))    {Add_Opt('q'); Add_Num(n); }
   if (Options & bbcp_RECURSE)   Add_Opt('r');
   if (Options & bbcp_RTCOPY)   {Add_Opt('R');
                                 if (rtSpec) Add_Str(rtSpec);
                                }
   if (Streams)                 {Add_Opt('s'); Add_Num(Streams);}
   if (TimeLimit)               {Add_Opt('t'); Add_Num(TimeLimit);}
   if (Options & bbcp_VERBOSE)   Add_Opt('v');
   if (Options & bbcp_BLAB)      Add_Opt('V');
   if (MaxWindow)               {Add_Opt('U'); Add_Num(MaxWindow);}
   else
   if (Wsize)                   {Add_Opt('W');
                                 if (Options & bbcp_MANTUNE) {Add_Nul(Wsize);}
                                    else      {Add_Num(Wsize);}
                                }
                                 Add_Opt('Y'); Add_Str(SecToken);
   if (Xrate)                   {Add_Opt('x'); Add_Num(Xrate);}
   if (Options & bbcp_CON2SRC)   Add_Opt('z');
   CopyOptt = cbuff - cbp;
   if (csSpec)                  {Add_Opt('E'); Add_Str(csSpec);}
   if (Options & (bbcp_IDIO | bbcp_ODIO))
                                {Add_Opt('u'); Add_Str(ubSpec);}
   CopyOpts = strdup(cbuff);
}
  
/******************************************************************************/
/*                            C o n f i g _ X e q                             */
/******************************************************************************/

void bbcp_Config::Config_Xeq(int rwbsz)
{
   long long memreq;

// Set network flow. This is indepedent of connection direction.
//
   bbcp_Net.Flow(Options & bbcp_SRC);

// Start up the logging if we need to
//
   if (Options & bbcp_LOGGING && !bbcp_NetLog.Open("bbcp", Logurl))
      {bbcp_Fmsg("Config", "Unable to initialize netlogger.");
       exit(4);
      }

// Get maximum window size if not specified
//
   if (!MaxWindow) MaxWindow = bbcp_Net.MaxWSize(Options & bbcp_SNK);

// Do penultimate adjustement to the window size
//
   if (Wsize > MaxWindow)
      {Wsize = MaxWindow;
       if (Options & bbcp_BLAB && Options & bbcp_SRC)
          WAMsg("Config", "Window size reduced", Wsize);
      }
   bbcp_Net.setWindow(Wsize, Options & bbcp_MANTUNE);

// Set the initial I/O buffer size
//
   setRWB(rwbsz);

// Set the blocking factor to one if it has not been specified or if direct
// or real-time input is in effect.
//
   if (!Bfact || (Options & (bbcp_IDIO | bbcp_RTCOPY))) Bfact = 1;

// Compute the number of buffers we will obtain
//
   if (Options & bbcp_SRC) BNum = (Streams > Bfact ? Streams : Bfact)*3;
      else BNum = (Streams > Bfact ? Streams : Bfact)*3
                + (Options & bbcp_ORDER ? Streams*3 : 0) + BAdd;

// Check if we have exceeded memory limitations
//
   memreq = RWBsz * BNum + (Options & bbcp_COMPRESS ? Bfact * RWBsz : 0);
   if (memreq > bbcp_OS.FreeRAM/4)
      {char mbuff[80];
       sprintf(mbuff, "I/O buffers (%" FMTLL "dK) > 25%% of available free memory (%" FMTLL "dK);",
                      memreq/1024, bbcp_OS.FreeRAM/1024);
       bbcp_Fmsg("Config", (Options & bbcp_SRC ? "Source" : "Sink"), mbuff,
                           "copy may be slow");
      }
}
  
/******************************************************************************/
/*                                R t o k e n                                 */
/******************************************************************************/
  
char *bbcp_Config::Rtoken()
{
   int mynum = (int)getpid();
   struct timeval tod;
   char mybuff[sizeof(mynum)*2+1];

// Get current time of day and add into the random equation
//
   gettimeofday(&tod, 0);
   mynum = mynum ^ tod.tv_sec ^ tod.tv_usec ^ (tod.tv_usec<<((mynum & 0x0f)+1));

// Convert to a printable string
//
   tohex((char *)&mynum, sizeof(mynum), mybuff);
   return strdup(mybuff);
}

/******************************************************************************/
/*                                  a 2 x x                                   */
/******************************************************************************/

int bbcp_Config::a2sz(const char *etxt, char *item, int  &result,
                      int  minv, int  maxv)
{   int i = strlen(item)-1; char cmult = item[i];
    int  val, qmult = 1;
    char *endC;
         if (cmult == 'k' || cmult == 'K') qmult = 1024;
    else if (cmult == 'm' || cmult == 'M') qmult = 1024*1024;
    else if (cmult == 'g' || cmult == 'G') qmult = 1024*1024*1024;
    if (qmult > 1) item[i] = '\0';
    val  = strtol(item, &endC, 10) * qmult;
    if (*endC || (maxv != -1 && (val > maxv || val < 1)) || val < minv)
       {if (qmult > 1) item[i] = cmult;
        return bbcp_Fmsg("Config", "Invalid", etxt, "-",item);
       }
    if (qmult > 1) item[i] = cmult;
    result = val;
    return 0;
}

int bbcp_Config::a2tm(const char *etxt, char *item, int  &result,
                      int  minv, int  maxv)
{   int i = strlen(item)-1; char cmult = item[i];
    int  val, qmult = 1;
    char *endC;
         if (cmult == 's' || cmult == 'S') qmult =  1;
    else if (cmult == 'm' || cmult == 'M') qmult = 60;
    else if (cmult == 'h' || cmult == 'H') qmult = 60*60;
    if (qmult > 1) item[i] = '\0';
    val  = strtol(item, &endC, 10) * qmult;
    if (*endC || (maxv != -1 && (val > maxv || val < 1)) || val < minv)
       {if (qmult > 1) item[i] = cmult;
        return bbcp_Fmsg("Config", "Invalid", etxt, "-",item);
       }
    if (qmult > 1) item[i] = cmult;
    result = val;
    return 0;
}

int bbcp_Config::a2ll(const char *etxt, char *item, long long &result,
                      long long minv, long long maxv)
{   long long val;
    char *endC;
    val  = strtoll(item, &endC, 10);
    if (*endC || (maxv != -1 && val > maxv) || val  < minv)
       return bbcp_Fmsg("Config", "Invalid", etxt, "-", item);
    result = val;
    return 0;
}

int bbcp_Config::a2n(const char *etxt, char *item, int  &result,
                     int  minv, int  maxv)
{   int  val;
    char *endC;
    val  = strtol(item, &endC, 10);
    if (*endC || (maxv != -1 && val > maxv) || val  < minv)
       return bbcp_Fmsg("Config", "Invalid", etxt, "-", item);
    result = val;
    return 0;
}

int bbcp_Config::a2o(const char *etxt, char *item, int  &result,
                     int  minv, int  maxv)
{   int  val;
    char *endC;
    val  = strtol(item, &endC, 8);
    if (*endC || (maxv != -1 && val > maxv) || val < minv)
       return bbcp_Fmsg("Config", "Invalid", etxt, "-", item);
    result = val;
    return 0;
}

int bbcp_Config::a2x(char *result, char *item, int ilen)
{   int  xVal, i, j = 0;

    for (i = 0; i < ilen; i++)
        {if (isdigit(item[i])) xVal = item[i] - int('0');
            else if (item[i] >= 'a' && item[i] <= 'f')
                    xVal = item[i] - int('a') + 10;
                    else return 1;
         if (i & 0x01) result[j++] |= xVal;
            else result[j] = xVal << 4;
        }
    return 0;
}

/******************************************************************************/
/*                                   n 2 a                                    */
/******************************************************************************/

char *bbcp_Config::n2a(int  val, char *buff, const char *fmt)
      {return buff + sprintf(buff, fmt, val);}

char *bbcp_Config::n2a(long long  val, char *buff, const char *fmt)
      {return buff + sprintf(buff, fmt, val);}

/******************************************************************************/
/*                               P a r s e S B                                */
/******************************************************************************/
  
void bbcp_Config::ParseSB(char *spec)
{
   char *up = 0, *hp = 0, *cp, *sp;
   int i = strlen(spec);

// Make sure spec ends with a slash
//
   if (spec[i-1] == '/') SrcBuff = strdup(spec);
      else {SrcBuff = (char *)malloc(i+2);
            strcpy(SrcBuff, spec); strcpy(SrcBuff+i, "/");
           }

// Prepare to parse the spec
//
   sp = cp = SrcBuff;
   while(*cp)
      {     if (*cp == '@' && !SrcUser)
               {SrcUser = sp; *cp = '\0'; sp = cp+1;}
       else if (*cp == ':' && !SrcHost)
               {SrcHost = sp; *cp = '\0'; sp = cp+1;}
       else if (*cp == '/') break;
       cp++;
      }
   SrcBase = sp;

// Ignore null users and hosts
//
   if (SrcUser && !(*SrcUser)) SrcUser = 0;
   if (SrcHost && !(*SrcHost)) SrcHost = 0;
}
  
/******************************************************************************/
/*                                 E O p t s                                  */
/******************************************************************************/
  
int bbcp_Config::EOpts(char *opts)
{
   int csLen, csPrt = 0, csSrc = 0;
   char *Equal, *csArg = 0;

// Reset the checksum option area
//
   csFD = -1; *csString = 0;
   if (csPath) {free(csPath); csPath = 0;}

// If the type starts with a dot then source computation is indicated
//
   if (*opts == '%')
      {csSrc = 1; opts++;
       if (!(*opts))
          {bbcp_Fmsg("Config", "Checksum type not specified."); return -1;}
      }

// Find the equals and see what follows
//
   if ((Equal = index(opts, '=')))
      {*Equal = 0; csArg = Equal+1;
       if (!isxdigit(*csArg))
          {if (*csArg) csPath = strdup(csArg); csPrt = 1; csArg = 0;}
      } else csOpts = bbcp_csDashE|bbcp_csLink;

// Verify that this is a supported checksum
//
   csLen = strlen(opts);
   if (csLen >= (int)sizeof(csName)) *csName = 0;
      else strcpy(csName, opts);
        if (!strcmp("a32", csName)) {csType = bbcp_csA32; csSize =  4;}
   else if (!strcmp("c32", csName)) {csType = bbcp_csC32; csSize =  4;}
   else if (!strcmp("md5", csName)) {csType = bbcp_csMD5; csSize = 16;}
   else {bbcp_Fmsg("Config", "Invalid checksum type -", opts); return -1;}

// Verify the checksum value if one was actually specified
//
   if (csArg)
      {csLen = strlen(csArg);
       if (csSize*2 != csLen || a2x(csValue, csArg, csLen))
          {bbcp_Fmsg("Config","Invalid",csName,"checksum value -",csArg);
           return -1;
          }
       strcpy(csString, csArg);
      }

// Indicate where the checksum generation will occur
//
   if (Equal)
      {if (csOpts & bbcp_csLink)
          if (Options & bbcp_ORDER || !csSrc)
                                    csOpts  =  bbcp_csVerAll|bbcp_csDashE;
             else                   csOpts |=  bbcp_csVerIn;
          else if (csSrc)           csOpts  =  bbcp_csVerIn;
                  else              csOpts  =  bbcp_csVerOut;
      }

// Establish print and echo options
//
   if (csPrt)
      csOpts |= bbcp_csPrint | (csOpts & bbcp_csVerOut ? 0 : bbcp_csSend);

// All done
//
   if (Equal) *Equal = '=';
   return 0;
}

/******************************************************************************/
/*                               L o g O p t s                                */
/******************************************************************************/
  
int bbcp_Config::LogOpts(char *opts)
{
    char *opt = opts;
    int myopts = 0, nlopts = 0;

    while(*opt && *opt != '@')
         {switch(*opt)
                {case 'a': nlopts |= NL_APPEND;    break;
                 case 'b': nlopts |= NL_MEM;       break;
                 case 'c': myopts |= bbcp_LOGCMP;  break;
                 case 'i': myopts |= bbcp_LOGIN;   break;
                 case 'o': myopts |= bbcp_LOGOUT;  break;
                 case 'r': myopts |= bbcp_LOGRD;   break;
                 case 'w': myopts |= bbcp_LOGWR;   break;
                 case 'x': myopts |= bbcp_LOGEXP;  break;
                 case '@':                         break;
                 default:  bbcp_Fmsg("Config","Invalid logging options -",opts);
                           return -1;
                }
          opt++;
         }

    if (!myopts) {bbcp_Fmsg("Config", "no logging events selected.");
                  return -1;
                 }

    Options &= ~bbcp_LOGGING;
    Options |= myopts;
    bbcp_NetLog.setOpts(nlopts);
    if (Logurl) {free(Logurl); Logurl = 0;}
    LogSpec = strdup(opts);
    if (*opt == '@' && opt[1]) {opt++; Logurl = strdup(opt);}

    return 0;
}

/******************************************************************************/
/*                          H o s t A n d   P o r t                           */
/******************************************************************************/
  
int bbcp_Config::HostAndPort(const char *what, char *path, char *buff, int bsz)
{   int hlen, pnum = 0;
    char *hn = path;

 // Extract the host name from the path
 //
    while (*hn && *hn != ':') hn++;
    if (!(hlen = hn - path))
       {bbcp_Fmsg("Config", what, "host not specified in", path);
        return -1;
       }
    if (hlen >= bsz)
       {bbcp_Fmsg("Config", what, "hostname too long in", path);
        return -1;
       }
    if (path != buff) strncpy(buff, path, hlen);

 // Extract the port number from the path
 //
    if (*hn == ':' && hn++ && *hn)
       {errno = 0;
        pnum = strtol(hn, (char **)NULL, 10);
        if (!pnum && errno || pnum > 65535)
           {bbcp_Fmsg("Config",what,"port invalid -", hn); return -1;}
       }

// All done.
//
    buff[hlen] = '\0';
    return pnum;
}
  
/******************************************************************************/
/*                                 R O p t s                                  */
/******************************************************************************/
  
int bbcp_Config::ROpts(char *opts)
{
   struct auto_RO {char *S; auto_RO(char *x) : S(strdup(x)) {}
                           ~auto_RO() {free(S);}
                  } Arg(opts);
   char *rNext, *rOpts = Arg.S;

// Set values to defaults
//
   rtCheck   = 3;
   rtLimit   = 0;
   if (rtLockf) {free(rtLockf); rtLockf = 0;}
   Options &= ~(bbcp_RTCBLOK | bbcp_RTCHIDE);

// Grab all items in opts (these may be separated by a comma)
//
   while(rOpts && *rOpts)
        {if ((rNext = index(rOpts, ','))) while(*rNext == ',') *rNext++ = '\0';
         switch(*rOpts)
               {case 'c': if (*(rOpts+1) != '=' 
                          ||  (rtCheck = atoi(rOpts+2)) <= 0)
                             return ROptsErr(rOpts);
                          break;
                case 'b': Options |= bbcp_RTCBLOK;
                          break;
                case 'h': Options |= bbcp_RTCHIDE;
                          break;
                case 'i': if (*(rOpts+1) != '='
                          ||  (rtLimit = atoi(rOpts+2)) <= 0)
                             return ROptsErr(rOpts);
                          break;
                case 'v': Options |= bbcp_RTCVERC;
                          break;
                case '/':
                case '.': rtLockf = strdup(rOpts);
                          break;
                default:  return ROptsErr(rOpts);
               }
          rOpts = rNext;
         }

// All done
//
   return 0;
}

/******************************************************************************/
/*                              R O p t s E r r                               */
/******************************************************************************/
  
int bbcp_Config::ROptsErr(char *eTxt)
{
    bbcp_Fmsg("Config", "Invalid -R argument -", eTxt);
    return -1;
}
 
/******************************************************************************/
/*                                 s e t C S                                  */
/******************************************************************************/
  
void bbcp_Config::setCS(char *inCS)
{
// Set checksum
//
   memcpy(csValue,inCS,csSize);
   tohex(inCS,csSize,csString);
}
  
/******************************************************************************/
/*                                s e t R W B                                 */
/******************************************************************************/

void bbcp_Config::setRWB(int rwbsz)
{
   static const int MaxRWB = 524288;
   static const int xyzRWB = 418816;

// Now compute the possible R/W buffer size
//
   if (rwbsz) RWBsz = rwbsz;
      else RWBsz = (Wsize > xyzRWB ? MaxRWB : Wsize + Wsize/4);
   RWBsz = (RWBsz < bbcp_OS.PageSize ? bbcp_OS.PageSize
                                     : RWBsz/bbcp_OS.PageSize*bbcp_OS.PageSize);

   if (rwbsz && RWBsz != rwbsz && (Options & bbcp_BLAB))
      WAMsg("Config", "Buffer size changed", RWBsz);
}

/******************************************************************************/
/*                                 t o h e x                                  */
/******************************************************************************/

char *bbcp_Config::tohex(char *inbuff, int inlen, char *outbuff) {
     static char hv[] = "0123456789abcdef";
     int i, j = 0;
     for (i = 0; i < inlen; i++) {
         outbuff[j++] = hv[(inbuff[i] >> 4) & 0x0f];
         outbuff[j++] = hv[ inbuff[i]       & 0x0f];
         }
     outbuff[j] = '\0';
     return outbuff;
     }

/******************************************************************************/
/*                                U n b u f f                                 */
/******************************************************************************/
  
int bbcp_Config::Unbuff(char *opts)
{
    char *opt = opts;
    int myopts = 0, nlopts = 0;

    while(*opt)
         {switch(*opt)
                {case 't': Options |= bbcp_ODIO; ubSpec[0] = 't'; break;
                 case 's': Options |= bbcp_IDIO; ubSpec[1] = 's'; break;
                 default:  bbcp_Fmsg("Config","Invalid unbuffered options -",opts);
                           return -1;
                }
          opt++;
         }
    return 0;
}
  
/******************************************************************************/
/*                               C l e a n u p                                */
/******************************************************************************/
  
void bbcp_Config::Cleanup(int rc, char *cfgfn, int cfgfd)
{
if (cfgfd >= 0 && cfgfn)
   if (rc > 1)
        bbcp_Fmsg("Config","Check config file",cfgfn,"for conflicts.");
   else bbcp_Fmsg("Config", "Error occured processing config file", cfgfn);
exit(rc);
}
