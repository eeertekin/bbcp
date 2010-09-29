/******************************************************************************/
/*                                                                            */
/*                           b b c p _ A r g s . C                            */
/*                                                                            */
/* (c) 2002 by the Board of Trustees of the Leland Stanford, Jr., University  */
/*      All Rights Reserved. See bbcp_Version.C for complete License Terms    *//*                            All Rights Reserved                             */
/*   Produced by Andrew Hanushevsky for Stanford University under contract    */
/*              DE-AC03-76-SFO0515 with the Department of Energy              */
/******************************************************************************/
  
#include <stdlib.h>
#include <strings.h>
#include "bbcp_Args.h"
#include "bbcp_Config.h"
#include "bbcp_Headers.h"

/******************************************************************************/
/*                        G l o b a l   O b j e c t s                         */
/******************************************************************************/

extern bbcp_Config bbcp_Config;

/******************************************************************************/
/*                         L o c a l   C l a s s e s                          */
/******************************************************************************/

class bbcp_Opt
{
public:

int  operator==(char *optarg)
       {int i = strlen(optarg);
        return i <= Optmaxl && i >= Optminl &&
               !strncmp(Optword, optarg, i);
       }

char *operator%(char *optarg)
        {int i = strlen(optarg);
         bbcp_Opt *p = this;
         do if (i <= p->Optmaxl && i >= p->Optminl &&
               !strncmp(p->Optword, optarg, i)) return p->Optvalu;
            while(p = p->Optnext);
         return 0;
        }

friend void bbcp_Args::Option(const char *opw, int minl, char optv, char opta);

      bbcp_Opt(const char *opw, int minl, char optv, char opta)
              {Optword = strdup(opw);
               Optminl = minl; Optmaxl = strlen(opw);
               Optvalu[0] = optv; Optvalu[1] = opta;
               Optnext = 0;
              }
     ~bbcp_Opt()
              {if (Optword) free(Optword);
               if (Optnext) delete Optnext;
              }
private:
bbcp_Opt *Optnext;
char     *Optword;
int       Optmaxl;
int       Optminl;
char      Optvalu[2];
};
  
/******************************************************************************/
/*              C o n s t r u c t o r   &   D e s t r u c t o r               */
/******************************************************************************/
  
bbcp_Args::bbcp_Args(char *etxt)
             {MOA = inStream = Argc = Aloc = 0; vopts = curopt = 0; endopts = 1;
              optp = 0; epfx = strdup(etxt);
             }

bbcp_Args::~bbcp_Args()
             {if (vopts) free(vopts);
              if (optp) delete optp;
              if (inStream) arg_stream.Detach();
              free(epfx);
             }

/******************************************************************************/
/*                               g e t a r g s                                */
/******************************************************************************/
  
char *bbcp_Args::getarg(int newln)
{

// Process argument array
//
   if (!inStream)
      {if (Aloc >= Argc) return (char *)0;
       argval = Argv[Aloc++];
       return argval;
      }

// Process stream or line arguments
//
   if (newln) {if ((argval = arg_stream.GetLine()) && *argval)  return argval;}
      else do {if ((argval = arg_stream.GetToken()))            return argval;
              }   while(arg_stream.GetLine());

// All done, force continual end of arguments
//
   arg_stream.Detach();
   argval   = 0;
   inStream = 0;
   Argc     = 0;
   Aloc     = 0;
   return (char *)0;
}

/******************************************************************************/
/*                                g e t o p t                                 */
/******************************************************************************/
  
char bbcp_Args::getopt()
{
   char optval, *optspec, *arglist;

// Check if we really have any more options
//
   if (endopts) return '\0';

// Get next option from whatever source we have
//
   if (curopt && curopt[1]) curopt++;
      else if (inStream)
              {if (MOA) arglist = arg_stream.GetLine();
                  else  arglist = (char *)"";
               if (arglist && (curopt = arg_stream.GetToken(&arglist)))
                  {if (*curopt != '-') {arg_stream.RetToken(); curopt = 0;}
                      else curopt++;
                  }
              }
              else if (Aloc >= Argc || *Argv[Aloc] != '-') curopt = 0;
                      else curopt = Argv[Aloc++]+1;

// Check if we really have an option here
//
   if (!curopt) {endopts = 1; return '\0';}
   if (!*curopt)
      {cerr <<epfx <<"Option letter missing after '-'." <<endl;
       endopts = 1;
       return '?';
      }

// Check for extended options or single letter option
//
   if (optp && strlen(curopt) > 2 && *curopt == '-'
   && (optspec = *optp%curopt)) {}
      else if (!(optspec = index(vopts, (int)(*curopt)))
           || *curopt == ':' || *curopt == '.')
              {cerr <<epfx <<"Invalid option, '-" <<*curopt <<"'." <<endl;
               endopts = 1;
               return '?';
              }

// Check if this option requires an argument
//
   if (optspec[1] != ':' && optspec[1] != '.') return *optspec;

// Get the argument from whatever source we have
//
   if (inStream) argval = (MOA ? arglist : arg_stream.GetToken());
      else argval = (Aloc < Argc ? Argv[Aloc++] : 0);

// If we have a valid argument, then we are all done
//
   if (argval)
      if (!*argval) argval = 0;
         else if (*argval != '-') return *optspec;

// If argument is optional, let it go
//
   if (optspec[1] == '.')
      {if (argval && *argval == '-')
          if (inStream) arg_stream.RetToken();
             else Aloc--;
       argval = 0;
       return *optspec;
      }

// Complain about a missing argument
//
   cerr <<epfx <<"Value not specified for '-" <<*curopt <<"'." << endl;
   endopts = 1;
   return '?';
}
 
/******************************************************************************/
/*                                O p t i o n                                 */
/******************************************************************************/

void bbcp_Args::Option(const char *opw, int minl, char opv, char opta)
{
     bbcp_Opt *p = new bbcp_Opt(opw, minl, opv, opta);
     p->Optnext = optp;
     optp = p;
}
  
/******************************************************************************/
/*                               O p t i o n s                                */
/******************************************************************************/

void bbcp_Args::Options(char *opts, int fd, int moa)
{
   inStream = 1; 
   arg_stream.Attach(fd);
   if (vopts) free(vopts);
   vopts = strdup(opts);
   curopt = 0;
   endopts = (moa ? 0 : !arg_stream.GetLine());
   MOA = moa;
}

void bbcp_Args::Options(char *opts, int argc, char **argv)
{
   inStream = 0; 
   Argc = argc; Argv = argv; Aloc = 0;
   if (vopts) free(vopts);
   vopts = strdup(opts);
   curopt = 0; endopts = 0;
   endopts = !argc;
}
