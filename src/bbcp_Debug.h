#ifndef __BBCP_DEBUG_H__
#define __BBCP_DEBUG_H__
/******************************************************************************/
/*                                                                            */
/*                          b b c p _ D e b u g . h                           */
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
