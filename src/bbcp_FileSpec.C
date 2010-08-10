/******************************************************************************/
/*                                                                            */
/*                       b b c p _ F i l e S p e c . C                        */
/*                                                                            */
/* (c) 2002 by the Board of Trustees of the Leland Stanford, Jr., University  */
/*      All Rights Reserved. See bbcp_Version.C for complete License Terms    *//*                            All Rights Reserved                             */
/*   Produced by Andrew Hanushevsky for Stanford University under contract    */
/*              DE-AC03-76-SFO0515 with the Department of Energy              */
/******************************************************************************/

#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "bbcp_Config.h"
#include "bbcp_Emsg.h"
#include "bbcp_FileSpec.h"
#include "bbcp_FileSystem.h"
#include "bbcp_FS_Unix.h"
#include "bbcp_Pthread.h"

/******************************************************************************/
/*                      E x t e r n a l   O b j e c t s                       */
/******************************************************************************/

extern bbcp_Config   bbcp_Config;

extern bbcp_FS_Unix  bbcp_UFS;

/******************************************************************************/
/*                     L o c a l   D e f i n i t i o n s                      */
/******************************************************************************/

// <seqno> <fnode> <inode> <mode> <size> <acctime> <modtime> <group> <fname>
//
#define bbcp_ENFMT "%d %c %lld %o %lld %lx %lx %s %s\n"
#define bbcp_DEFMT "%d %c %lld %o %lld %lx %lx %31s %1025s"
#define bbcp_DEGMT "%d %c %Ld  %o %Ld  %lx %lx %31s %1025s"
  
/******************************************************************************/
/*                               C o m p o s e                                */
/******************************************************************************/

int bbcp_FileSpec::Compose(long long did, char *dpath, int dplen, char *fname)
{
   int retc, n;
   char *fn = (fname ? fname : filename);
   bbcp_FileInfo Targ;

// Set up the target file name
//
   n = dplen + 1 + strlen(fn) + 1;
   if (targpath) free(targpath);
   targpath = (char *)malloc(n);
   strcpy(targpath, dpath);
   targetfn = targpath + dplen;
   if (dpath[dplen-1] != '/') {*targetfn = '/'; targetfn++;}
   strcpy(targetfn, fn);

// Create signature filename if append mode is enabled
//
   if ((bbcp_Config.Options & bbcp_SNK) && (bbcp_Config.Options & bbcp_APPEND))
      {char buff[1025], *rp;
       if ((rp = rindex(targetfn,'/'))) rp++;
          else rp = targetfn;
       snprintf(buff, sizeof(buff)-1, "%s/bbcp.%s.%llx.%s",
                bbcp_Config.CKPdir, hostname, did, rp);
       buff[sizeof(buff)-1] = '\0';
       targsigf = strdup(buff);
       DEBUG("Append signature file is " <<targsigf);
      }

// Get the current state of the file
//
   if (retc = FSp->Stat(targpath, &Targ)) targetsz = 0;
      else if (Targ.Otype != 'f') targetsz = -1;
              else targetsz = (long long)Targ.size;
   return retc == 0;
}

/******************************************************************************/
/*                           C r e a t e _ P a t h                            */
/******************************************************************************/

int bbcp_FileSpec::Create_Path()
{
   int retc;

// Create the path and accept "already exists" errors. Note that we use the
// temporary creation mode which gaurentees that we can actually place files
// in the directory. This will later be set to the true mode if it differs.
//
   DEBUG("Make path " <<Info.mode <<' ' <<targpath);
   if (retc = FSp->MKDir(targpath, bbcp_Config.ModeDC))
      if (retc == -EEXIST) return 0;
         else return bbcp_Emsg("Create_Path", retc, "creating path", targpath);

// All done
//
   return 0;
}

/******************************************************************************/
/*                                D e c o d e                                 */
/******************************************************************************/
  
int bbcp_FileSpec::Decode(char *buff, char *xName)
{
   char gnbuff[64], *sP;
   char fnbuff[1032], *fmt = (char *)bbcp_DEFMT;
   int xtry=1, n;

// Decode the specification
//
   do {n = sscanf(buff, fmt, &seqno, &Info.Otype, &Info.fileid, &Info.mode,
                  &Info.size, &Info.atime, &Info.mtime, gnbuff, fnbuff);
       fmt = (char *)bbcp_DEGMT;
      } while(xtry-- && n != 9);

// Make sure it is correct
//
   if (n != 9) 
      {sprintf(fnbuff,"Unable to decode item %d in file specification from",n+1);
       return bbcp_Fmsg("Decode", fnbuff, (xName ? xName : hostname));
      }

// Set up our structure
//
   pathname = filename = fspec = strdup(fnbuff);
   if (Info.Group) free(Info.Group);
   Info.Group = strdup(gnbuff);

// All done
//
   return 0;
}

/******************************************************************************/
/*                                E n c o d e                                 */
/******************************************************************************/

int bbcp_FileSpec::Encode(char *buff, size_t blen)
{
   int n;

// Format the specification
//
   n = snprintf(buff, blen, bbcp_ENFMT, seqno, Info.Otype, Info.fileid,
                Info.mode, Info.size, Info.atime, Info.mtime, Info.Group,
                filename);

// Make sure all went well
//
   if (n < 0)     return bbcp_Fmsg("Encode", "stat format error.");
   if (n >= blen) return bbcp_Fmsg("Encode", "buffer overflow.");
   return n;
}
 
/******************************************************************************/
/*                        E x t e n d F i l e S p e c                         */
/******************************************************************************/

void bbcp_FileSpec::ExtendFileSpec(bbcp_FileSpec* headp)
{
   struct dirent* d;
   bbcp_FileSpec* newp;
   bbcp_FileSpec* lastp;
   char*          buf;
   int            bufsz;
   char           relative_name[256];
   struct stat    sbuf;
   DIR           *dirp;

   // To avoid potentially serious copy loops, if pathname is not a true 
   // directory, we do not extend it. Eventually, we will recreate symlinks 
   // in the target node.
   //
   if (lstat(pathname,&sbuf) || (sbuf.st_mode & S_IFMT) != S_IFDIR)
      {if ((sbuf.st_mode & S_IFMT) != S_IFREG) Info.Otype = '?';
       return;
      }

   //
   // Advance lastp to the last node in the list
   //
   for (lastp = headp ; lastp->next != NULL ; lastp = lastp->next)
      ; // nothing to do -- just setting lastp

   //
   // This loop walks the tree rooted at pathname, adding each file it
   // finds to the list at headp.
   //
   if ((dirp = opendir(pathname)))
   for (d = readdir(dirp) ; NULL != d ; d = readdir(dirp))
   {
      //
      // Ignore "." and ".."
      //
      if ((0 != strcmp(".", d->d_name)) && (0 != strcmp("..", d->d_name)))
      {
         //
         // Initialize a new FileSpec object to represent this file
         //
         newp = new bbcp_FileSpec();
         snprintf(relative_name, sizeof(relative_name), "%s/%s",
                  filename, d->d_name);
         newp->filename = strdup(relative_name);

         bufsz = strlen(pathname) + strlen(d->d_name) + 2;
         buf = (char*) malloc(bufsz);
         if (NULL == buf)
         {
            cout << "out of memory" << endl;
            exit(2);
         }
         snprintf(buf, bufsz, "%s/%s", pathname, d->d_name);

         newp->pathname = buf;
         newp->targetsz = 0;
         newp->seqno = lastp->seqno + 1;

         //
         // Fill in the file's stat information, bail on failure
         //
         if (0 != FSp->Stat(newp->pathname, &(newp->Info)))
         {
            free(newp->filename);
            delete newp;
            continue;
         }

         newp->FSp = FSp;

         newp->next = NULL;
         lastp->next = newp;
         lastp = newp;
      }
   }

   if (dirp) closedir(dirp);
}

/******************************************************************************/
/*                              F i n a l i z e                               */
/******************************************************************************/
  
int bbcp_FileSpec::Finalize(int retc)
{

// If an error occured, see what we should do
//
   if (retc)
      {if (bbcp_Config.Options & (bbcp_KEEP|bbcp_NOUNLINK)) return retc;
       FSp->RM(targpath);
      }
      else if (bbcp_Config.Options & bbcp_PCOPY) setStat();
              else if (Info.Otype == 'f')
                      FSp->setMode(targpath, bbcp_Config.Mode);

// Delete the signature file if one exists
//
   if (targsigf) FSp->RM(targsigf);

// All done
//
   return retc;
}

/******************************************************************************/
/*                                 P a r s e                                  */
/******************************************************************************/
  
void bbcp_FileSpec::Parse(char *spec)
{
   char *sp, *cp;
   int i, isrel;

// Create a copy of the spec
//
   if (fspec) free(fspec);
   fspec = strdup(spec);
   username = hostname = pathname = filename = 0;
   seqno = bbcp_Config.lastseqno++;

// Prepare to parse the spec
//
   sp = cp = fspec;
   while(*cp)
      {     if (*cp == '@' && !username)
               {username = sp; *cp = '\0'; sp = cp+1;}
       else if (*cp == ':' && !hostname)
               {hostname = sp; *cp = '\0'; sp = cp+1;}
       else if (*cp == '/') break;
       cp++;
      }
   pathname = sp;

// Point to the filename component
//
   i = strlen(pathname)-1;
   while(i >= 0 && pathname[i] != '/') i--;
   if (i >= 0) filename = &pathname[i+1];
      else if (bbcp_Config.SrcBase) filename = pathname;
      else {fspec2 = (char *)malloc(strlen(pathname)+3);
            fspec2[0] = '.'; fspec2[1] = '/';
            strcpy(&fspec2[2], pathname);
            filename = &fspec2[2];
            pathname = fspec2;
           }

// Check if we have a relative pathname here
//
   if (pathname[0] != '/')
      {bbcp_Config.Options |= bbcp_RELATIVE;
       if (!username) username = bbcp_Config.SrcUser;
       if (!hostname) hostname = bbcp_Config.SrcHost;
       if (bbcp_Config.Options & bbcp_SRC && bbcp_Config.SrcBase)
          {CompleteSpec();
           BuildPaths(); 
           filename = pathrltv;
          }
      }
}

/******************************************************************************/
/*                               s e t M o d e                                */
/******************************************************************************/

int bbcp_FileSpec::setMode(mode_t Mode)
{
   int retc;

// Make sure we have a filesystem here
//
   if (!FSp) return bbcp_Fmsg("setStat", "no filesystem for", targpath);

// Set the mode
//
   if ((retc = FSp->setMode (targpath, Mode)))
      return bbcp_Emsg("setStat", -retc, "setting mode on", targpath);
   return 0;
}

/******************************************************************************/
/*                               s e t S t a t                                */
/******************************************************************************/

int bbcp_FileSpec::setStat()
{
   char *act =  0;
   int retc;

// Make sure we have a filesystem here
//
   if (!FSp) return bbcp_Fmsg("setStat", "no filesystem for", targpath);

// Set the times, mode, and group
//
        if ((retc = FSp->setTimes(targpath, Info.atime, Info.mtime)))
           act = (char *)"setting time on";
   else if ((retc = FSp->setMode (targpath, Info.mode)))
           act = (char *)"setting mode on";
   else if (Info.Group
        && (retc = FSp->setGroup (targpath, Info.Group)))
           act = (char *)"setting group on";

// Check if any errors occured
//
   if (act) return bbcp_Emsg("setStat", -retc, act, targpath);
   return 0;
}
  
/******************************************************************************/
/*                                  S t a t                                   */
/******************************************************************************/
  
int bbcp_FileSpec::Stat(int complain)
{
    int retc;

// Get the filesystem for this file if we do not have one (note that the
// filespec object only becomes valid after the first stat() call).
//
   if (!FSp && !(FSp = bbcp_getFileSystem(pathname)))
      {char savefn = *filename;
       *filename = '\0';
       FSp = bbcp_getFileSystem(pathname);
       *filename = savefn;
      }

// Get info for the file
//
   if (!FSp
   || (!(retc = FSp->Stat(pathname, &Info)) && Info.Otype == '?')) retc=ENOENT;

// Check if we have info and, if not, whether we should complain about it
//
   if (retc && complain) bbcp_Emsg("Stat", retc, "processing", pathname);

// All done
//
   return retc;
}

/******************************************************************************/
/*                          W r i t e S i g F i l e                           */
/******************************************************************************/
  
int bbcp_FileSpec::WriteSigFile()
{
    int outfd, blen;
    char buff[2048];
    bbcp_Stream sfStream;

// Open the sigfile
//
   if ((outfd = open(targsigf, O_WRONLY|O_CREAT|O_TRUNC,0600))
      < 0) return bbcp_Emsg("WriteSigFile", -errno, "opening", targsigf);

// Create a signature and write it out
//
   sfStream.Attach(outfd);
   if ((blen = Encode(buff, sizeof(buff))) < 0
   ||  sfStream.Put(buff, (size_t)blen)    < 0)
      return bbcp_Fmsg("WriteSigFile","Unable to create restart file",targsigf);

// All done
//
   sfStream.Close();
   return 0;
}

/******************************************************************************/
/*                              X f r _ D o n e                               */
/******************************************************************************/
  
int bbcp_FileSpec::Xfr_Done()
{
   int rc, Force = bbcp_Config.Options & bbcp_FORCE;

// If this is an APPEND request, build the signature file
//
//cerr <<"tsz=" <<targetsz <<" isz=" <<Info.size <<" sigf=" <<targsigf <<endl;
   if (bbcp_Config.Options & bbcp_APPEND)
      {if (!bbcp_UFS.Stat(targsigf))
          {rc = Xfr_Fixup();
           if (rc >= 0 || !Force) return rc;
          } else {
           if (targetsz == Info.size || targetsz < 0)
              {if (targetsz >= 0) bbcp_Fmsg("Xfr_Done", "File", targpath,
                  "appears to have already been copied; copy skipped.");
                  return (Finalize(0) ? -1 : 1);
              }
          }

     // Unless force is in effect, we cannot append.
     //
       if (!Force) return bbcp_Fmsg("Xfr_Done", "File", targpath,
                 "changed since the copy completed; append not possible.");
       bbcp_Fmsg("Xfr_Done", "File", targpath,
                 "changed since the copy completed; copy restarting.");
      }

// The file exists, complain unless force has been specified
//
   if (!Force) return bbcp_Fmsg("Xfr_Done","File",targpath,"already exists.");

// All done, we can try to copy this file
//
   targetsz = 0;
   return 0;
}
 
/******************************************************************************/
/*                       P r i v a t e   M e t h o d s                        */
/******************************************************************************/
/******************************************************************************/
/*                            B u i l d P a t h s                             */
/******************************************************************************/
  
void bbcp_FileSpec::BuildPaths()
{
   char delim, *cp = pathrltv, *Slush;
   int plen, same = 0;
   bbcp_FileSpec *PS_New, *PS_Prv = 0, *PS_Cur = bbcp_Config.srcPath;

// Make sure we have at least one slash here
//
   if (!(Slush = rindex(cp, '/'))) return;

// Construct a path list
//
   while(*cp && cp < Slush)
        {while(*cp && *cp != '/') cp++;
         delim = *cp; *cp = '\0';
         plen = cp - pathrltv;
         while(PS_Cur && plen >= PS_Cur->seqno)
              {if (plen == PS_Cur->seqno
               && (same =! strcmp(pathrltv, PS_Cur->pathrltv))) break;
               PS_Prv = PS_Cur;
               PS_Cur = PS_Cur->next;
              }
         if (!same)
            {PS_New = new bbcp_FileSpec();
             PS_New->fspec = PS_New->pathname = strdup(pathname);
             PS_New->filename = PS_New->fspec+(cp-pathrltv-1);
             PS_New->seqno = plen;
             if (PS_New->Stat(0))
                {DEBUG("Path " <<pathname <<" not found.");
                 delete PS_New; return;
                }
             if (PS_Cur) {PS_New->next = PS_Cur->next; PS_Cur->next = PS_New;}
                else if (PS_Prv) PS_Prv->next = PS_New;
                        else bbcp_Config.srcPath = PS_New;
             PS_Prv = PS_New; PS_Cur = PS_New->next;
            }
         if (*cp = delim) cp++;
        }
}

/******************************************************************************/
/*                          C o m p l e t e S p e c                           */
/******************************************************************************/
  
void bbcp_FileSpec::CompleteSpec()
{
   char *newspec, *newpath, *np;
   int len;

// Extend the path with the base path
//
   len = bbcp_Config.SrcBlen + strlen(pathname) + 2;
   if (username) len += strlen(username) + 1;
   if (hostname) len += strlen(hostname) + 1;
   np = newspec = (char *)malloc(len);

// Relocate the username
//
   if (username)
      {strcpy(np, username); username = np;
       np += strlen(username); *np = '\0'; np++;
      }

// Relocate the hostname
//
   if (hostname)
      {strcpy(np, hostname); hostname = np;
       np += strlen(hostname); *np = '\0'; np++;
      }

// Construct full path
//
   strcpy(np, bbcp_Config.SrcBase);
   newpath = np;
   np += bbcp_Config.SrcBlen-1;
   if (*np != '/') {np++; *np = '/';}
   np++;
   pathrltv = np;

// Copy the full path but remove duplicate slashes
//
   while(*pathname)
        {if (*pathname == '/' && pathname[1] == '/') filename--;
            else {*np = *pathname; np++;}
         pathname++;
        }
   *np = '\0';

// Replace the existing spec with the new spec
//
   free(fspec);
   fspec = newspec;
   filename = newspec + (filename - pathname);
   pathname = newpath;
}

/******************************************************************************/
/*                             X f r _ F i x u p                              */
/******************************************************************************/
  
int bbcp_FileSpec::Xfr_Fixup()
{
   char *lp;
   int infd, retc;
   bbcp_Stream TSigstream;
   bbcp_FileSpec TSpec;

// Read the contents of the signature file asnd decode it
//
   if ((infd = open(targsigf, O_RDONLY)) < 0)
      return bbcp_Emsg("Xfr_Fixup", -errno, "opening file", targsigf);
   TSigstream.Attach(infd);
   if (!(lp = TSigstream.GetLine()) || TSpec.Decode(lp,targsigf))
      return bbcp_Fmsg("Xfr_Fixup",
                  "Unable to determine append state for", targetfn);

// Determine if the source file is still the same
//
   if (TSpec.Info.fileid != Info.fileid
   ||  TSpec.Info.size   != Info.size
   ||  TSpec.Info.mtime  != Info.mtime
   ||  TSpec.Info.Otype  != Info.Otype
   ||  strcmp(TSpec.filename, filename))
      {bbcp_Fmsg("Xfr_Fixup", "Source file '", filename, "' on", hostname,
                 "appears to have changed after the previous copy ended.");
       return bbcp_Fmsg("Xfr_Fixup", "Cannot append to ", targetfn);
      }

// Is the file sizes are identical then, finalize the copy now
//
   if (Info.size == targetsz)
      {bbcp_Fmsg("Xfr_Fixup", "File", targpath,
                 "copy appears to have completed; finalizing copy now.");
       return (Finalize(0) ? -1 : 1);
      }
   return 0;
}
