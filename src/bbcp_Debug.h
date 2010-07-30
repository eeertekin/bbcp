#ifndef __BBCP_DEBUG_H__
#define __BBCP_DEBUG_H__
/******************************************************************************/
/*                                                                            */
/*                          b b c p _ D e b u g . h                           */
/*                                                                            */
/* (c) 2002 by the Board of Trustees of the Leland Stanford, Jr., University  */
/*      All Rights Reserved. See bbcp_Version.C for complete License Terms    *//*                            All Rights Reserved                             */
/*   Produced by Andrew Hanushevsky for Stanford University under contract    */
/*              DE-AC03-76-SFO0515 with the Department of Energy              */
/******************************************************************************/

#include <unistd.h>

#include "bbcp_Headers.h"

struct bbcp_Debug
       {int   Trace;
        char *Who;
        int   mypid;

        bbcp_Debug() {Trace = 0; Who = (char *)"CTL"; mypid = (int)getpid();}
       ~bbcp_Debug() {}
      };

#ifndef BBCP_CONFIG_DEBUG
extern bbcp_Debug bbcp_Debug;
#endif
  
#define DEBUGON    (bbcp_Debug.Trace)

#define DEBUG(x)  {if (bbcp_Debug.Trace) \
                      cerr << "bbcp_" <<bbcp_Debug.Who <<' ' <<bbcp_Debug.mypid <<": " <<x <<endl <<flush;}
#endif
