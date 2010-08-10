/******************************************************************************/
/*                                                                            */
/*                         b b c p _ S t r e a m . c                          */
/*                                                                            */
/* (C) 2002 by the Board of Trustees of the Leland Stanford, Jr., University  */
/*      All Rights Reserved. See bbcp_Version.C for complete License Terms    *//*                          All Rights Reserved                               */
/*   Produced by Andrew Hanushevsky for Stanford University under contract    */
/*              DE-AC03-76-SFO0515 with the Deprtment of Energy               */
/******************************************************************************/

#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>
#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#if defined(SUN) || defined(LINUX)
#include <stropts.h>
#endif

#include "bbcp_Debug.h"
#include "bbcp_Emsg.h"
#include "bbcp_Headers.h"
#include "bbcp_Stream.h"
#include "bbcp_System.h"
  
/******************************************************************************/
/*                         l o c a l   d e f i n e s                          */
/******************************************************************************/
  
#define MaxARGC 64
#define bbcp_Stream_EOM  0x01
#define bbcp_Stream_BUSY 0x02

/******************************************************************************/
/*                        G l o b a l   O b j e c t s                         */
/******************************************************************************/
  
extern bbcp_System bbcp_OS;

/******************************************************************************/
/*               o o u c _ S t r e a m   C o n s t r u c t o r                */
/******************************************************************************/
  
bbcp_Stream::bbcp_Stream()
{
     FD     = -1;
     FE     = -1;
     bsize  = 0;
     buff   = 0;
     bnext  = 0;
     bleft  = 0;
     recp   = 0;
     token  = 0;
     flags  = 0;
     child  = 0;
     ecode  = 0;
     notabs = 0;
     xcont  = 1;
     xline  = 0;
}

/******************************************************************************/
/*                                A t t a c h                                 */
/******************************************************************************/
  
int bbcp_Stream::Attach(int FileDescriptor, int bsz) 
{

    // Close the current stream. Close will handle unopened streams.
    //
    Close();

    // Allocate a new buffer for this stream
    //
    if (!bsz) buff = 0;
       else if (!(buff = (char *)malloc(bsz+1)))
               return bbcp_Emsg("Attach", errno, "allocating stream buffer.");

    // Initialize the stream
    //
    FD = FE= FileDescriptor;
    bnext  = buff;
    bsize  = bsz+1;
    bleft  = 0;
    recp   = 0;
    token  = 0;
    flags  = 0;
    ecode  = 0;
    xcont  = 1;
    xline  = 0;
    return  0;
}
  
/******************************************************************************/
/*                                 C l o s e                                  */
/******************************************************************************/

void bbcp_Stream::Close() 
{
    int retc=0;

    // Wait for any associated process on this stream
    //
    Drain();

    // Close the associated file descriptor if it was open
    //
    if (FD >= 0)             close(FD);
    if (FE >= 0 && FE != FD) close(FE);

    // Release the buffer if it was allocated.
    //
    if (buff) free(buff);

    // Clear all data values by attaching a dummy FD
    //
    FD = FE = -1;
    buff = 0;
}

/******************************************************************************/
/*                                 D r a i n                                  */
/******************************************************************************/
  
int bbcp_Stream::Drain() 
{
    int Status = 0, retc;

    // Drain any outstanding processes.
    //
    if (child) {do {retc = waitpid(child, &Status, 0);}
                    while(retc > 0 || (retc == -1 && errno == EINTR));
                child = 0;
               }
    return Status;
}
  
/******************************************************************************/
/*                               E   x   e   c                                */
/******************************************************************************/
  
int bbcp_Stream::Exec(char *cmd, char *carg, int inrd, int inerr)
{
    int j;
    char *parm[MaxARGC];
  
    // Construct the argv array based on passed command line.
    //
    for (j = 0; j < MaxARGC-2 && *cmd; j++)
        {while(*cmd == ' ') cmd++;
         if (!(*cmd)) break;
         parm[j] = cmd;
         while(*cmd && *cmd != ' ') cmd++;
         if (*cmd) {*cmd = '\0'; cmd++;}
        }
    if (carg) parm[j++] = carg;
    parm[j] = (char *)0;

    // Continue with normal processing
    //
    return Exec(parm, inrd, inerr);
}

int bbcp_Stream::Exec(char **parm, int inrd, int inerr)
{
    int fildes_In[2], fildes_Out[2] = {-1,-1}, fildes_Err[2] = {0,0};
    int retc, Child_Out = FD, Child_In = FE, Child_Err = 0;

    // Wait for any previous command to finish on this stream
    //
    Drain();

    // Create a pipe if we have no attached FD. Recall that FD is the
    // descriptor we read from and FE is the one we write to. This is
    // true for sockets. For pipes, the relationship is reversed in
    // the child. Plus, we need to get two pipes if the child's STDIN
    // is to be redirected. This is because pipes suffer from backflow.
    //
    if (FD < 0)
       {if (pipe(fildes_Out) || (inrd  && pipe(fildes_In))
                            || (inerr && pipe(fildes_Err)))
           return bbcp_Emsg("Exec",errno,"creating a pipe for",parm[0]);
        Child_In=fildes_In[0]; Child_Out=fildes_Out[1]; Child_Err=fildes_Err[1];
        fildes_Out[1] = (inrd ? fildes_In[1] : -1);
        if (retc = Attach(fildes_Out)) return retc;
       }

    // Fork a process first so we can pick up the next request.
    //
    if ((child = bbcp_OS.Fork()) > 0)
       {close(Child_Out);
        if (inrd)  close(Child_In);
        if (inerr) close(Child_Err);
        return fildes_Err[0];
       }
    if (child < 0) 
       {retc=bbcp_Emsg("Exec",-errno,"forking request process for",parm[0]);
                   close(fildes_In[1]);
        if (inrd)  close(fildes_Out[0]);
        if (inerr) close(fildes_Err[0]);
        return retc;
       }

    /*****************************************************************/
    /*                  C h i l d   P r o c e s s                    */
    /*****************************************************************/

    // Close the parent end of the pipe
    //
    if (fildes_In[1]  >= 0) close(fildes_In[1]);
    if (fildes_Out[0] >= 0) close(fildes_Out[0]);
    if (fildes_Err[0] >  0) close(fildes_Err[0]);


    // Redirect standard in if so requested
    //
    if (inrd && dup2(Child_In, STDIN_FILENO) < 0)
       {bbcp_Emsg("Exec", errno, "setting up standard in for", parm[0]);
        exit(255);
       }

    // Reassign the stream to be standard out to capture all of the output.
    //
    if (dup2(Child_Out, STDOUT_FILENO) < 0)
       {bbcp_Emsg("Exec", errno, "setting up standard out for", parm[0]);
        exit(255);
       }

    // Reassign the stream to be standard err to capture all of the output.
    //
    if (inerr && Child_Err && dup2(Child_Err, STDERR_FILENO) < 0)
       {bbcp_Emsg("Exec", errno, "setting up standard err for", parm[0]);
        exit(255);
       }

    // Invoke the command never to return
    //
    DEBUG("PATH=" <<getenv("PATH"));
    execvp(parm[0], parm);
    bbcp_Emsg("Exec", errno, "executing", parm[0], parm[1]);
    exit(255);
    return(255);  // some compilers demand a return in int functions
}

/******************************************************************************/
/*                                  K i l l                                   */
/******************************************************************************/
  
int bbcp_Stream::Kill()
{

// Kill the attached process, if we have one
//
   if (child) 
      {DEBUG("Killing process " <<child <<" signum=" <<SIGTERM);
       if (kill(child, SIGTERM)) return -errno;
          else return 0;
      }
   return 1;
}

/******************************************************************************/
/*                               G e t L i n e                                */
/******************************************************************************/
  
char *bbcp_Stream::GetLine()
{
   int bcnt, retc;
   char *bp;

// Check if end of message has been reached.
//
   if (flags & bbcp_Stream_EOM) return (char *)NULL;

// Find the next record in the buffer
//
   if (bleft > 0)
      {recp = bnext; bcnt = bleft;
       for (bp = bnext; bcnt--; bp++)
           if (!*bp || *bp == '\n')
               {if (!*bp) flags |= bbcp_Stream_EOM;
                *bp = '\0';
                bnext = ++bp;
                bleft = bcnt;
                token = recp;
                return recp;
               }
               else if (notabs && *bp == '\t') *bp = ' ';
  
   // There is no next record, so move up data in the buffer.
   //
      strncpy(buff, bnext, bleft);
      bnext = buff + bleft;
      }
      else bnext = buff;

// Prepare to read in more data.
//
    bcnt = bsize - (bnext - buff) -1;
    bp = bnext;

// Read up to the maximum number of bytes. Stop reading should we see a
// new-line character or a null byte -- the end of a record.
//
   ecode = 0;
   recp  = token = buff; // This will always be true at this point
   while(bcnt)
        {do { retc = read(FD, (void *)bp, (size_t)bcnt); }
            while (retc < 0 && errno == EINTR);

         if (retc < 0) {bbcp_Emsg("GetLine", errno, "reading request.");
                        return (char *)0;}
         if (!retc)
            {*bp = '\0';
             flags |= bbcp_Stream_EOM;
             bnext = ++bp;
             bleft = 0;
             return buff;
            }

         bcnt -= retc;
         while(retc--)
             if (!*bp || *bp == '\n')
                {if (!*bp) flags |= bbcp_Stream_EOM;
                    else *bp = '\0';
                 bnext = ++bp;
                 bleft = retc;
                 return buff;
                } else {
                 if (notabs && *bp == '\t') *bp = ' ';
                 bp++;
                }
         }

// All done, force an end of record.
//
   bbcp_Emsg("GetLine", EMSGSIZE, "record truncated.");
   buff[bsize-1] = '\0';
   return buff;
}

/******************************************************************************/
/*                              G e t T o k e n                               */
/******************************************************************************/
  
char *bbcp_Stream::GetToken(int lowcase) {
     char *tpoint;

     // Verify that we have a token to return;
     //
     if (!token) return (char *)NULL;

     // Skip to the first non-blank character.
     //
     while (*token && *token == ' ') token ++;
     if (!*token) {token = 0; return 0;}
     tpoint = token;

     // Find the end of the token.
     //
     if (lowcase) while (*token && *token != ' ')
                        {*token = (char)tolower((int)*token); token++;}
        else      while (*token && *token != ' ') {token++;}
     if (*token) {*token = '\0'; token++;}

     // All done here.
     //
     return tpoint;
}

char *bbcp_Stream::GetToken(char **rest, int lowcase)
{
     char *tpoint;

     // Get the next token
     //
     if (!(tpoint = GetToken(lowcase))) return tpoint;

     // Skip to the first non-blank character.
     //
     while (*token && *token == ' ') token ++;
     *rest = token;


     // All done.
     //
     return tpoint;
}

/******************************************************************************/
/*                          G e t F i r s t W o r d                           */
/******************************************************************************/

char *bbcp_Stream::GetFirstWord(int lowcase)
{
      // If in the middle of a line, flush to the end of the line
      //
      if (xline) while(GetWord(lowcase));
      return GetWord(lowcase);
}

/******************************************************************************/
/*                               G e t W o r d                                */
/******************************************************************************/
  
char *bbcp_Stream::GetWord(int lowcase)
{
     char *wp, *ep;

     // If we have a token, return it
     //
     xline = 1;
     if (wp = GetToken(lowcase)) return wp;

     // If no continuation allowed, return a null (but only once)
     //
     if (!xcont) {xcont = 1; xline = 0; return (char *)0;}

     // Find the next non-blank non-comment line
     //
     while(GetLine())
        {// Get the first token (none if it is a blank line)
         //
         if (!(wp = GetToken(lowcase))) continue;

         // If token starts with a pound sign, skip the line
         //
         if (*wp == '#') continue;

         // Process continuations (last non-blank character is a back-slash)
         //
         ep = bnext-2;
         while (ep >= buff && *ep == ' ') ep--;
         if (ep < buff) continue;
         if (*ep == '\\') {xcont = 1; *ep = '\0';}
            else xcont = 0;
         return wp;
         }
      xline = 0;
      return (char *)0;
}

/******************************************************************************/
/*                              R e t T o k e n                               */
/******************************************************************************/
  
void bbcp_Stream::RetToken()
{
     // Check if we can back up
     //
     if (!token || token == recp) return;

     // Backup to the first null byte, then backup to first space.
     //
     while(*token && token != recp) token--;
     if (token != recp) *token-- = ' ';
     while(*token && *token != ' ' && token != recp) token--;
     if (*token == '\0') token++;
}

/******************************************************************************/
/*                                   P u t                                    */
/******************************************************************************/

int bbcp_Stream::Put(const char *data, int dlen) {
    int dcnt = dlen, retc;

    if (flags & bbcp_Stream_BUSY) {ecode = ETXTBSY; return -1;}

    while(dcnt)
         {do { retc = write(FE, (const void *)data, (size_t)dlen);}
              while (retc < 0 && errno == EINTR);
          if (retc >= 0) dcnt -= retc;
             else {flags |= bbcp_Stream_BUSY;
                   bbcp_Emsg("Put", errno, "writing to stream.");
                   flags &= ~bbcp_Stream_BUSY;
                   return -1;
                  }
         }
    return 0;
}

int bbcp_Stream::Put(char *datavec[], int dlenvec[]) {
    int i, retc, dlen;
    const char *data;

    if (flags & bbcp_Stream_BUSY) {ecode = ETXTBSY; return -1;}

    for (i = 0; datavec[i]; i++)
        {data = datavec[i]; dlen = dlenvec[i];
         while(dlen)
              {do { retc = write(FE, (const void *)data, (size_t)dlen);}
                   while (retc < 0 && errno == EINTR);
               if (retc >= 0) {data += retc; dlen -= retc;}
                  else {flags |= bbcp_Stream_BUSY;
                        bbcp_Emsg("Put", errno, "writing to stream.");
                        flags &= ~bbcp_Stream_BUSY;
                        return -1;
                       }
              }
        }
    return 0;
}
