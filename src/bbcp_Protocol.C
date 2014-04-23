/******************************************************************************/
/*                                                                            */
/*                       b b c p _ P r o t o c o l . C                        */
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

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include "bbcp_Config.h"
#include "bbcp_Emsg.h"
#include "bbcp_Headers.h"
#include "bbcp_Network.h"
#include "bbcp_Node.h"
#include "bbcp_Protocol.h"
#include "bbcp_Pthread.h"
#include "bbcp_Version.h"
 
/******************************************************************************/
/*                         L o c a l   C l a s s e s                          */
/******************************************************************************/
  
class bbcp_Login_Stream
{
public:

bbcp_Node *np;

      bbcp_Login_Stream(bbcp_Link *Net) 
           {np = new bbcp_Node(Net);}
     ~bbcp_Login_Stream()
           {if (np) delete np;}
};

/******************************************************************************/
/*                      E x t e r n a l   O b j e c t s                       */
/******************************************************************************/
  
extern bbcp_Config   bbcp_Config;

extern bbcp_Network  bbcp_Net;

extern bbcp_Version  bbcp_Version;

/******************************************************************************/
/*                     T h r e a d   I n t e r f a c e s                      */
/******************************************************************************/

extern "C"
{
void *bbcp_ProtocolLS(void *pp)
{
   bbcp_FileSpec  *fP = bbcp_Config.srcSpec;
   char xBuff[128];
   time_t tNow, xMsg = time(0)+bbcp_Config.Progint;
   int numD = 0, numF = 0;
   int Blab = (bbcp_Config.Options & bbcp_VERBOSE) || bbcp_Config.Progint;

// Extend all directories with the files therein
//
   if (Blab) bbcp_Fmsg("Dirlist", "Indexing files to be copied...");
   while(fP)
        {if ('d' == fP->Info.Otype)
            {numD++; fP->ExtendFileSpec(bbcp_Config.srcSpec, numF);}
         fP = fP->next;
         if (bbcp_Config.Progint && (tNow = time(0)) >= xMsg)
            {sprintf(xBuff, "%d file%s in %d director%s so far...",
                     numF, (numF == 1 ? "" : "s"),
                     numD, (numD == 1 ? "y": "ies"));
             bbcp_Fmsg("Dirlist", "Found", xBuff);
             xMsg = tNow+bbcp_Config.Progint;
            }
        }

// Indicate what we found if so wanted
//
   if (Blab)
      {sprintf(xBuff, "%d file%s in %d director%s.",
               numF, (numF == 1 ? "" : "s"),
               numD, (numD == 1 ? "y": "ies"));
       bbcp_Fmsg("Source", "Copying", xBuff);
      }

// All done
//
   return 0;
}
}
  
/******************************************************************************/
/*                              S c h e d u l e                               */
/******************************************************************************/
  
int bbcp_Protocol::Schedule(bbcp_Node *Fnode, bbcp_FileSpec *Ffs, 
                                 char *Fcmd,           char *Ftype,
                            bbcp_Node *Lnode, bbcp_FileSpec *Lfs, 
                                 char *Lcmd,           char *Ltype,
                            bbcp_Node *Tnode)
{
   int retc;

   char *cbhost, *addOpt[2];
   bool fcbh = false;

// Start-up the first node
//
   if (retc = Fnode->Run(Ffs->username, Ffs->hostname, Fcmd, Ftype))
      return retc;

// Determine additional options
//
   if (Ftype[1] == 'R')
      {addOpt[0] = bbcp_Config.CopyOSrc;
       addOpt[1] = bbcp_Config.CopyOTrg;
      } else {
       addOpt[0] = bbcp_Config.CopyOTrg;
       addOpt[1] = bbcp_Config.CopyOSrc;
      }

// Send the arguments
//
   if (retc = SendArgs(Fnode, Ffs, (char *)"none", 0, addOpt[0])) return retc;

// Get the callback port from the first host
//
   if (retc = getCBPort(Fnode)) return retc;

// Start the second node
//
   if (retc = Lnode->Run(Lfs->username, Lfs->hostname, Lcmd, Ltype))
      return retc;

// Compute callback hostname and reset callback port
//
        if (!(Ffs->hostname)) cbhost = bbcp_Config.MyAddr;
   else if ((bbcp_Config.Options & bbcp_NODNS && isdigit(Ffs->hostname[0]))
           ||  Ffs->hostname[0] == '[') cbhost = Ffs->hostname;
   else fcbh = (cbhost = bbcp_Net.FullHostName(Ffs->hostname,1)) !=0;

// Send the arguments
//
   retc = SendArgs(Lnode, Lfs, cbhost, bbcp_Config.CBport, addOpt[1]);
   if (fcbh) free(cbhost);
   if (retc) return retc;

// The the final ending
//
   getEnd(Tnode);

// Wait for the any node to finish
//
   retc = Fnode->Wait(Lnode);

// All done
//
   sleep(1);     // Delay the Stop just/s e n to allow errors to be reported
   Fnode->Stop(retc==0);
   Lnode->Stop(retc==0);
   return retc;
}

/******************************************************************************/
/*                             g e t C B P o r t                              */
/******************************************************************************/

int bbcp_Protocol::getCBPort(bbcp_Node *Node)
{
   char *wp;
   int  pnum;

// The remote program should hve started, get the call back port
//
   if (wp = Node->GetLine())
      {if ((wp = Node->GetToken()) && !strcmp(wp, "200")
       &&  (wp = Node->GetToken()) && !strcmp(wp, "Port:")
       &&  (wp = Node->GetToken())
       &&  bbcp_Config.a2n("callback port", wp, pnum, 0, 65535) == 0)
          {bbcp_Config.CBport = pnum; return 0;}
      }

// Invalid response
//
   return bbcp_Fmsg("Protocol", "bbcp unexpectedly terminated on",
                     Node->NodeName());
}
  
/******************************************************************************/
/*                             s e t C B P o r t                              */
/******************************************************************************/
  
int bbcp_Protocol::setCBPort(int pnum)
{

// The port number simply gets sent via standard out
//
   cout <<"200 Port: " <<pnum <<endl;
   return 0;
}

/******************************************************************************/
/*                                g e t E n d                                 */
/******************************************************************************/
  
void bbcp_Protocol::getEnd(bbcp_Node *Node)
{
   char *wp, csVal[64];
   int n;

// Preset values
//
   Node->TotFiles = 0;
   Node->TotBytes = 0;

// The remote program should hve started, get checksums or ending tag
//
   while((wp = Node->GetLine())
      && (wp = Node->GetToken()) && !strcmp(wp, "200")
      && (wp = Node->GetToken()))
        {if (!strcmp(wp, "cks:"))
            {if (!(wp=Node->GetToken()) || (n = strlen(wp)) > sizeof(csVal))
                continue;
             strcpy(csVal, wp);
             if (!(wp = Node->GetToken())) continue;
             putCSV(Node->NodeName(), wp, csVal, n);
             continue;
            }

         if (strcmp(wp, "End:") || !(wp = Node->GetToken())) break;
         errno = 0; Node->TotFiles = strtol(wp, (char **)NULL, 10);
         if (errno) {Node->TotFiles = 0; break;}
         if (!(wp = Node->GetToken()))   break;
         errno = 0; Node->TotBytes = strtoll(wp, (char **)NULL, 10);
         if (errno) Node->TotBytes = 0;
         break;
        }
    DEBUG("At end files=" <<Node->TotFiles <<" bytes=" <<Node->TotBytes);
}
  
/******************************************************************************/
/* Source:                       P r o c e s s                                */
/******************************************************************************/
  
int bbcp_Protocol::Process(bbcp_Node *Node)
{
   bbcp_FileSpec *fp = bbcp_Config.srcSpec;
   pthread_t Tid;
   int rc, NoGo = 0;
   char *cp;

// If there is a r/t lock file, make sure it exists
//
   if ((bbcp_Config.Options & bbcp_RTCSRC) && bbcp_Config.rtLockf
   &&  (bbcp_Config.rtLockd = open(bbcp_Config.rtLockf, O_RDONLY)) < 0)
      {rc = errno, NoGo = 1;
       bbcp_Emsg("Config", rc, "opening lock file", bbcp_Config.rtLockf);
      }

// Make sure all of the source files exist at this location. If there is an
// error, defer exiting until after connecting to prevent a hang-up. We
// make sure that we are not trying to copy a directory.
//
   while(fp)
        {NoGo |= fp->Stat();
         if (fp->Info.Otype == 'd' && !(bbcp_Config.Options & bbcp_RECURSE))
            {bbcp_Fmsg("Source", fp->pathname, "is a directory.");
             NoGo = 1; break;
            }

         fp = fp->next;
        }

// If this is a recursive list, do it in the bacground while we try to connect.
// This avoids time-outs when large number of files are enumerated.
//
   if (!NoGo && bbcp_Config.Options & bbcp_RECURSE)
      if ((rc = bbcp_Thread_Start(bbcp_ProtocolLS, 0, &Tid)) < 0)
         {bbcp_Emsg("Protocol", rc, "starting file enumeration thread.");
          NoGo = 1;
         }

// Establish all connections
//
   if (Node->Start(this, (bbcp_Config.Options & bbcp_CON2SRC))
   ||  Node->getBuffers(0)) return 2;
   Local = Node;

// At this point, if we're running with the -r recursive option, our list of
// file specs (bbcp_Config.srcSpec) is being extended recursively to include
// all subdirs and their contents. We must wait for the thread to finish.
//
   if (!NoGo && bbcp_Config.Options & bbcp_RECURSE) bbcp_Thread_Wait(Tid);

// If there was a fatal error, we can exit now, the remote side will exit
//
   if (NoGo)
      {char buff[8];
       strcpy(buff, "eol\n");
       Remote->Put(buff, (ssize_t)4);
       Node->Stop();
       return 2;
      }
   rc = 0;

// Process all control connection requests and return
//
   while(!rc && Remote->GetLine())
      {if (!(cp = Remote->GetToken())) continue;
            if (!strcmp(cp, "flist")) rc = Process_flist();
       else if (!strcmp(cp, "get"))   rc = Process_get();
       else if (!strcmp(cp, "exit")) {rc = Process_exit(); break;}
       else {bbcp_Fmsg("Process", "Invalid command, '", cp, "'.");
             rc = 1;
            }
      }

// Dismantle this node and return
//
   Node->Stop();
   if (cp) return rc;
   bbcp_Fmsg("Source", "Unexpected end of control stream from",
                             Remote->NodeName());
   return 32;
}
 
/******************************************************************************/
/*                       P r o c e s s   M e t h o d s                        */
/******************************************************************************/
/******************************************************************************/
/*                          P r o c e s s _ e x i t                           */
/******************************************************************************/
  
int bbcp_Protocol::Process_exit()
{
    int  retc;
    char *wp;

// Get the return code (none indicates erro)
//
   if (!(wp = Remote->GetToken()))
      {bbcp_Fmsg("Process_exit", "missing return code."); retc = 22;}
      else if (bbcp_Config.a2n("return code", wp, retc, 0, 255)) retc = 22;

// Return to caller
//
   return (int)retc;
}
  
/******************************************************************************/
/*                         P r o c e s s _ f l i s t                          */
/******************************************************************************/
  
int bbcp_Protocol::Process_flist()
{
   bbcp_FileSpec *dp = bbcp_Config.srcPath;
   bbcp_FileSpec *fp = bbcp_Config.srcSpec;
   char buff[4096];
   int blen;
   const char eoltag[] = "eol\n";
   const int  eoltsz   = strlen(eoltag);

// Simply go through the list of paths and report them back to the caller

   while(dp)
      {if ((blen = dp->Encode(buff,(size_t)sizeof(buff))) < 0) return -1;
       if (Remote->Put(buff, blen)) return -1;
       dp = dp->next;
      }

// Simply go through the list of files and report them back to the caller

   while(fp) 
      {if (fp->Info.Otype != 'd' || *(fp->filename))
          {if ((blen = fp->Encode(buff,(size_t)sizeof(buff))) < 0) return -1;
           if (Remote->Put(buff, blen)) return -1;
          }
       fp = fp->next;
      }

// Indicate that this is the end of the list
//
   return Remote->Put(eoltag, eoltsz);
}
 
/******************************************************************************/
/*                           P r o c e s s _ g e t                            */
/******************************************************************************/
  
int bbcp_Protocol::Process_get()
{
   bbcp_FileSpec *pp = 0, *fp = bbcp_Config.srcSpec;
   char *wp;
   int retc;
   int   fnum, snum;
   long long foffset = 0;
   const char getack[] = "204 getok\n";
   const int  getaln = strlen(getack);

// Get the file number and name from "get <strm> <fnum> <fname> [<offset>]"
//
   if (!(wp = Remote->GetToken()))
      {bbcp_Fmsg("Process_get", "missing stream number."); return 19;}
   if (bbcp_Config.a2n("stream number", wp, snum, 0, 255))  return 19;
   if (!(wp = Remote->GetToken()))
      {bbcp_Fmsg("Process_get", "missing file number."); return 19;}
   if (bbcp_Config.a2n("file number", wp, fnum, 0, 0x7ffffff))  return 19;
   if (!(wp = Remote->GetToken()))
      {bbcp_Fmsg("Process_get", "missing file name.");   return 19;}

// Locate the file
//
   while(fp && (fp->seqno != fnum || strcmp(wp, fp->filereqn)))
        {pp = fp; fp=fp->next;}
   if (!fp)
      {char buff[64];
       sprintf(buff, "%d", fnum);
       bbcp_Fmsg("Process_get", "file '", buff, wp, "' not found.");
       return 2;
      }

// Unchain the file specification (get allowed only once)
//
   if (pp) pp->next = fp->next;
      else bbcp_Config.srcSpec = fp->next;
   fp->next = 0;

// Get the optional offset
//
   if (wp = Remote->GetToken())
      {if (bbcp_Config.a2ll("file offset", wp, foffset, 0, -1)) return 22;
       if (foffset > fp->Info.size)
          {char buff[128];
           sprintf(buff, "(%lld>%lld)", foffset, fp->Info.size);
           bbcp_Fmsg("Process_get","Invalid offset",buff,"for",fp->pathname);
           return 29;
          }
       fp->targetsz = foffset;
      }

// Send the response
//
   if (Remote->Put(getack, getaln)) return 1;

// Send the file changing the sequence number to that requested by the caller
//
   fp->seqno = snum;
   retc = Local->SendFile(fp);
   delete fp;
   return retc;
}
  
/******************************************************************************/
/*                         P r o c e s s _ l o g i n                          */
/******************************************************************************/

// The following class is here to allow graceful error exits
//
int bbcp_Protocol::Process_login(bbcp_Link *Net)
{
   char buff[256], *tp, *bp, *vp, *wp, *id;
   int retc, blen, respWS;
   bbcp_Login_Stream loginStream(Net);
   bbcp_Node *np = loginStream.np;

// Get the first line of the login stream
//
   if (!(np->GetLine()))
      {if (retc = np->LastError())
          return bbcp_Emsg("Process_Login", retc, "processing login from",
                                 Net->LinkName());
       return bbcp_Fmsg("Process_Login", "Bad login from", Net->LinkName());
     }

// Determine the id we want (the control stream must login first)
//
   id = (Remote ? (char *)"data" : (char *)"ctlr");

// Process the login request: login <id> <password>
//
   if (!(wp = np->GetToken()) || strcmp(wp, "login")
   ||  !(wp = np->GetToken()) || strcmp(wp, id)
   ||  !(wp = np->GetToken()) || strcmp(wp, bbcp_Config.SecToken))
      return bbcp_Fmsg("Process_Login", "Invalid login from", Net->LinkName());

// We are all done if this is not a control stream
//
   if (*id != 'c') {np->Detach(); return 0;}

// Pickup all parameters.
//
   bp = vp = wp = 0;
   while((tp = np->GetToken()))
        {     if (!strcmp(tp, "wsz:"))
                 {if (!(wp = np->GetToken()))
                     return bbcp_Fmsg("Login", "Window size is missing.");
                 }
         else if (!strcmp(tp, "ver:"))
                 {if (!(vp = np->GetToken()))
                     return bbcp_Fmsg("Login", "Version is missing.");
                 }
         else if (!strcmp(tp, "bsz:"))
                 {if (!(bp = np->GetToken()))
                     return bbcp_Fmsg("Login", "Buffer size is missing.");
                 }
        }

// Verify that our version is the same on the other side
//
   if (vp) bbcp_Version.Verify(Net->LinkName(), vp);
      else bbcp_Version.Verify(Net->LinkName(),(char *)"02.01.12.00.0");

// We can now do a window/buffer adjustment
//
   if (!wp) respWS = bbcp_Config.Wsize;
      else if (!(respWS = AdjustWS(wp, bp, 0))) return -1;

// Respond to this login request (control only gets a response)
//
   blen = sprintf(buff, "204 loginok wsz: %d %d\n",respWS,bbcp_Config.RWBsz);
   if ((retc = np->Put(buff, blen)) < 0) return -1;

// All done
//
   Remote = np;
   loginStream.np = 0;
   return 1;
}

/******************************************************************************/
/* Target:                       R e q u e s t                                */
/******************************************************************************/
  
int bbcp_Protocol::Request(bbcp_Node *Node)
{
   long long totsz=0;
   int  retc, numfiles, texists;
   int  outDir = (bbcp_Config.Options & bbcp_OUTDIR) != 0;
   bbcp_FileSpec *fp;
   char buff[1024];

// Establish all connections
//
   if (Node->Start(this, !(bbcp_Config.Options & bbcp_CON2SRC))
   ||  Node->getBuffers(1)) return 2;
   Local = Node;

// Establish the target directory
//
   if ((texists = !bbcp_Config.snkSpec->Stat(0))
   &&  bbcp_Config.snkSpec->Info.Otype == 'd')
       tdir = bbcp_Config.snkSpec->pathname;
      else {int plen;
            if (plen = bbcp_Config.snkSpec->filename -
                       bbcp_Config.snkSpec->pathname)
               strncpy(buff, bbcp_Config.snkSpec->pathname, plen-1);
               else {buff[0] = '.'; plen = 2;}
            tdir = buff; buff[plen-1] = '\0';
           }

// Generate a target directory ID. This will also uncover a missing directory.
//
   fs_obj = bbcp_Config.snkSpec->FSys();
   if (texists &&  bbcp_Config.snkSpec->Info.Otype == 'd')
      tdir_id = bbcp_Config.snkSpec->Info.fileid;
      else {bbcp_FileInfo Tinfo;
            if (!fs_obj || (!(retc = fs_obj->Stat(tdir, &Tinfo))
            && Tinfo.Otype != 'd') && outDir) retc = ENOTDIR;
            if (retc) {bbcp_Fmsg("Request","Target directory",
                                 bbcp_Config.snkSpec->pathname,"not found");
                       return Request_exit(2);
                      }
            tdir_id = Tinfo.fileid;
           }

// The first step is to perform an flist to get the list of files
//
   if ((numfiles = Request_flist(totsz)) <= 0) 
      return Request_exit((numfiles  < 0 ? 22 : 0));

// If we have a number files, the target had better be a directory
//
   if (numfiles > 1 || outDir)
      {if (!texists)
          {bbcp_Fmsg("Request", "Target directory",
                     bbcp_Config.snkSpec->pathname, "not found.");
           return Request_exit(2);
          }
       if (bbcp_Config.snkSpec->Info.Otype != 'd')
          {bbcp_Fmsg("Request", "Target", bbcp_Config.snkSpec->pathname,
                     "is not a directory.");
           return Request_exit(20);
          }
       bbcp_Config.Options |= bbcp_OUTDIR;
      }

// Make sure we have enough space in the filesystem
//
   DEBUG("Preparing to copy " <<numfiles <<" file(s); bytes=" <<totsz);
   if (!(bbcp_Config.Options & bbcp_NOSPCHK) && !fs_obj->Enough(totsz, numfiles))
      {bbcp_Fmsg("Sink", "Insufficient space to copy all the files from",
                               Remote->NodeName());
       return Request_exit(28);
      }

// Create all of the required directories
//
   retc = 0;
   fp = bbcp_Config.srcPath;
   while(fp && !(retc = fp->Create_Path()))  fp = fp->next;
   if (retc) return Request_exit(retc);

// Get each source file
//
   fp = bbcp_Config.srcSpec;
   while(fp && !(retc=Request_get(fp)) && !(retc=Local->RecvFile(fp,Remote)))
        {if (bbcp_Config.Options & bbcp_APPEND) totsz -= fp->targetsz;
         fp = fp->next;
        }

// Now determine if we need to reset the stat info on any paths we created
//
   if ((fp = bbcp_Config.srcPath))
      {if (bbcp_Config.Options & bbcp_PCOPY)
          {while(fp && fp->setStat()) fp = fp->next;}
          else if (bbcp_Config.ModeD != bbcp_Config.ModeDC)
                  {while(fp && fp->setMode(bbcp_Config.ModeD)) fp = fp->next;}
      }

// Report back how many files and bytes were received
//
   cout <<"200 End: " <<numfiles <<' ' <<totsz <<endl;

// All done
//
   return Request_exit(retc);
}

/******************************************************************************/
/*                       R e q u e s t   M e t h o d s                        */
/******************************************************************************/
/******************************************************************************/
/*                          R e q u e s t _ e x i t                           */
/******************************************************************************/
  
int bbcp_Protocol::Request_exit(int retc)
{
    char buff[256];
    int blen;

// Send the exit command (we don't care how it completes)
//
   blen = sprintf(buff, "exit %d\n", retc);
   Remote->Put(buff, (ssize_t)blen);
   Local->Stop();

// Return to caller
//
   return retc;
}
  
/******************************************************************************/
/*                         R e q u e s t _ f l i s t                          */
/******************************************************************************/
  
int bbcp_Protocol::Request_flist(long long &totsz)
{
   int retc, noteol, numfiles = 0;
   char *lp, *tfn;;
   int   tdln = strlen(tdir);
   bbcp_FileSpec *fp, *lastfp = 0, *lastdp = 0;
   const char flcmd[] = "flist\n";
   const int  flcsz   = sizeof(flcmd)-1;

// Set correct target file name
//
   if (bbcp_Config.snkSpec->Info.Otype == 'd') tfn = 0;
      else tfn = bbcp_Config.snkSpec->filename;

// Send request
//
   if (Remote->Put(flcmd, flcsz)) return -1;

// Now start getting all of the files to be copied
//
   while((lp = Remote->GetLine()) && (noteol = strcmp(lp, "eol")))
        {fp = new bbcp_FileSpec(fs_obj, Remote->NodeName());
         if (fp->Decode(lp)) {numfiles = -1; break;}

               if (fp->Compose(tdir_id, tdir, tdln, tfn)
               &&  (retc = fp->Xfr_Done()))
                  {delete fp; if (retc < 0) {numfiles = -1; break;}}
          else if (fp->Info.Otype == 'd')
                    {if (lastdp) lastdp->next = fp;
                        else bbcp_Config.srcPath = fp;
                     lastdp = fp;
                    }
/*PIPE*/  else if (fp->Info.Otype == 'f' || fp->Info.Otype == 'p')
                  {numfiles++;
                   totsz += fp->Info.size;
                   if (lastfp) lastfp->next = fp;
                   else bbcp_Config.srcSpec = fp;
                   lastfp = fp;
                  }
        }

// Flush the input queue if need be
//
   while(lp && strcmp(lp, "eol")) lp = Remote->GetLine();

// Check how we terminated
//
   if (numfiles >= 0 && noteol)
      return bbcp_Fmsg("Request_flist", "Premature end of file list from",
                       Remote->NodeName());

// All done
//
   return numfiles;
}
 
/******************************************************************************/
/*                           R e q u e s t _ g e t                            */
/******************************************************************************/
  
int bbcp_Protocol::Request_get(bbcp_FileSpec *fp)
{
   int blen;
   char *wp, buff[2048];

// Make sure there is enough space for this file
//
   if (!(bbcp_Config.Options & bbcp_NOSPCHK) && !(fs_obj->Enough(fp->Info.size,1)))
      {bbcp_Fmsg("Request_get", "Insufficient space to create file",
                       fp->targpath);
       return 28;
      }

// Construct the get command
//
   blen = snprintf(buff, sizeof(buff)-1, "get 0 %d %s %lld\n",
                   fp->seqno, fp->filereqn, fp->targetsz);

// Send off the command
//
   if (Remote->Put(buff, blen)) return 1;

// Get the response
//
   if (Remote->GetLine()        && Remote->GetToken()
   && (wp = Remote->GetToken()) && !strcmp(wp, "getok")) return 0;

// Improper response, we can't create the file
//
   bbcp_Fmsg("Request_get", "get from", Remote->NodeName(),
             "failed to create", fp->targpath);
   return 1;
}
  
/******************************************************************************/
/*                         R e q u e s t _ l o g i n                          */
/******************************************************************************/
  
int bbcp_Protocol::Request_login(bbcp_Link *Net)
{
   const char *CtlLogin = "login ctlr %s wsz: %s%d ver: %s dsz: %d\n";
   const char *DatLogin = "login data %s\n";
   char buff[512], *id, *wp;
   int retc, blen;
   bbcp_Login_Stream loginStream(Net);
   bbcp_Node *np = loginStream.np;

// Determine wether this is a control or data path
//
   id = (Remote ? (char *)DatLogin : (char *)CtlLogin);

// Prepare the login request
//
   blen = sprintf(buff,id,bbcp_Config.SecToken,(bbcp_Net.AutoTune() ? "+" : ""),
                          bbcp_Config.Wsize, bbcp_Version.VData,
                          bbcp_Config.RWBsz);

// Send the request
//
   if ((retc = np->Put(buff, (ssize_t)blen)) < 0)
      return bbcp_Emsg( "Request_Login",-(np->LastError()),
                        "requesting", id, (char *)"path.");

// If this is a data stream, then tell caller to hold on to the net link
//
   if (Remote) {np->Detach(); return 0;}

// For a control connection, read the acknowledgement below
// nnn loginok wsz: <num> [<dsz>]
//
   if (np->GetLine())
      {if (np->GetToken()        && (wp = np->GetToken())
       && !strcmp(wp, "loginok") && (wp = np->GetToken())
       && !strcmp(wp, "wsz:")    && (wp = np->GetToken())
       &&  AdjustWS(wp, np->GetToken(), 1))
          {Remote = np;
           loginStream.np = 0;
           return 1;
          }
      }

// Invalid response
//
   return bbcp_Fmsg("Request_Login", "Invalid login ack sequence.");
}
 
/******************************************************************************/
/*                       P r i v a t e   M e t h o d s                        */
/******************************************************************************/
/******************************************************************************/
/*                              A d j u s t W S                               */
/******************************************************************************/
  
int bbcp_Protocol::AdjustWS(char *wp, char *bp, int Final)
{
   int  xWS, xAT, sWS, sAT, tWS, tAT, xBS = 0;
   int isSRC = bbcp_Config.Options & bbcp_SRC;

// New version tell us if they can auto-tune via a leading plus
//
   xAT = (*wp == '+');

// Get window size in binary
//
   if (bbcp_Config.a2n("window size", wp, xWS, 1, -1)) return 0;

// New versions also tell us the I/O buffer size as well. Older versions
// do not give a buffer size and it was historically the window size.
//
   if (bp && *bp)
      {if (bbcp_Config.a2n("buffer size", bp, xBS, 1, -1)) return 0;}
      else xBS = xWS;

// If this is a login response and the value is the window that must be used.
// The receiver rules and establishes the window and I/O buffer size.
//
   if (Final)
      {if (isSRC)
          {if (xWS < bbcp_Config.Wsize)
              {bbcp_Config.Wsize = bbcp_Net.setWindow(xWS, 1);
               if (bbcp_Config.Options & bbcp_VERBOSE)
                  bbcp_Config.WAMsg("Logon","Source window size reduced",xWS);
              }
           if (xBS < bbcp_Config.RWBsz) bbcp_Config.setRWB(xBS);
          } else
           if (xBS > bbcp_Config.RWBsz) bbcp_Config.setRWB(xBS);
       return 1;
      }

// This is the initial login so we are being asked if the proposed window
// is acceptable and we must respond with the window that should be used.
// In all cases, we must either set the source buffer to be no greater that the
// target or the traget buffer to be no smaller than the source buffer.
//
   if (isSRC)
      {if (xBS < bbcp_Config.RWBsz) bbcp_Config.setRWB(xBS);
       sWS = bbcp_Config.Wsize; sAT = bbcp_Net.AutoTune();
       tWS = xWS;               tAT = xAT;
      } else {
       if (xBS > bbcp_Config.RWBsz) bbcp_Config.setRWB(xBS);
       sWS = xWS;               sAT = xAT;
       tWS = bbcp_Config.Wsize; tAT = bbcp_Net.AutoTune();
      }

// If source and target autotune then check if the target window >= source (warn
// o/w) and return back what the sender would have expected to keep autotuning.
//
   if (sAT && tAT)
      {if (tWS < sWS && bbcp_Config.Options & bbcp_VERBOSE)
          bbcp_Config.WAMsg("Login","Target autotuning may be misconfigured; "
                                    "max set", tWS);
       return (isSRC ? tWS : sWS);
      }

// If we are the source, hence we must use a window <= the target window.
//
   if (isSRC)
      {if (sWS > tWS)
          {bbcp_Config.Wsize = bbcp_Net.setWindow(tWS, 1);
           if (bbcp_Config.Options & bbcp_VERBOSE)
               bbcp_Config.WAMsg("Login", "Source window size reduced", tWS);
          }
       return tWS;
      }

// We are the target, request that the source always use the smaller window
//
   xWS = (sWS < tWS ? sWS : tWS);
   return xWS;
}
  
/******************************************************************************/
/*                                p u t C S V                                 */
/******************************************************************************/

void bbcp_Protocol::putCSV(char *Host, char *csFn, char *csVal, int csVsz)
{                                //1234567890123
   struct iovec iov[] = {{(char *)"Checksum: ", 10},
                        {bbcp_Config.csName,strlen(bbcp_Config.csName)},
                        {(char *)" ", 1}, {csVal, csVsz},
                        {(char *)" ", 1}, {Host, strlen(Host)},
                        {(char *)":", 1}, {csFn, strlen(csFn)},
                        {(char *)"\n",1}};
   int n = sizeof(iov)/sizeof(iov[0]);

// Write the checksum to a special file if it exists
//
   if (bbcp_Config.csPath)
      {if (writev(bbcp_Config.csFD, iov, n) < 0)
          {bbcp_Emsg("Protocol",errno,"writing checksum to",bbcp_Config.csPath);
           close(bbcp_Config.csFD); bbcp_Config.csFD = -1;
          }
      } else writev(STDERR_FILENO, iov, n);
}
  
/******************************************************************************/
/*                              S e n d A r g s                               */
/******************************************************************************/
  
int bbcp_Protocol::SendArgs(bbcp_Node *Node, bbcp_FileSpec *fsp,
                            char *cbhost, int cbport, char *addOpt)
{
   char buff[512], *apnt[6];
   int alen[6], i = 0;

// The remote program should be running at this point, setup the args
//
   if (bbcp_Config.CopyOpts)
      {apnt[i]   = bbcp_Config.CopyOpts;
       alen[i++] = strlen(bbcp_Config.CopyOpts);
      }
   if (addOpt) {apnt[i] = addOpt; alen[i++] = strlen(addOpt);}
   apnt[i]   = buff;
   alen[i++] = snprintf(buff, sizeof(buff)-1, " -H %s:%d\n", cbhost, cbport);
   apnt[i] = 0; alen[i] = 0;

// Send the argumnets via the stdout/stdin stream for the node
//
   if (Node->Put(apnt, alen) < 0)
      return bbcp_Emsg("Protocol", errno, "sending arguments to",
                        Node->NodeName());

// Send the file arguments now
//
   apnt[1] = (char *)"\n"; alen[1] = 1; apnt[2] = 0; alen[2] = 0;
   while(fsp)
        {apnt[0] = fsp->pathname; alen[0] = strlen(fsp->pathname);
         if (Node->Put(apnt, alen) < 0)
             return bbcp_Emsg("Protocol", errno, "sending file arguments to",
                               Node->NodeName());
         fsp = fsp->next;
        }

// Send eol
//
   apnt[0] = (char *)"\0"; alen[0] = 1; apnt[1] = 0; alen[1] = 0;
   if (Node->Put(apnt, alen) < 0)
      return bbcp_Emsg("Protocol", errno, "sending eol to", Node->NodeName());

// All done
//
   return 0;
}
