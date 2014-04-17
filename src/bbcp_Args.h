#ifndef __BBCP_ARGS_H__
#define __BBCP_ARGS_H__
/******************************************************************************/
/*                                                                            */
/*                           b b c p _ A r g s . h                            */
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

#include <stdlib.h>
#include <string.h>
#include "bbcp_Stream.h"

class bbcp_Opt;
  
class bbcp_Args
{
public:

char *getarg(int newln=0);

char  getopt();

void  Option(const char *optw, int minl, char optv, char opta); // For extended -- options

void  Options(char *opts, int fd, int moa=0);

void  Options(char *opts, int argc, char **argv);

      bbcp_Args(char *etxt=(char *)"Args: ");

    ~bbcp_Args();

char *argval;

private:

char            *epfx;
bbcp_Opt        *optp;
char            *vopts;
char            *curopt;
int              MOA;
int              inStream;
int              endopts;
int              Argc;
int              Aloc;
char           **Argv;
bbcp_Stream      arg_stream;
};
#endif
