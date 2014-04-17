#ifndef __BBCP_FILESPEC_H__
#define __BBCP_FILESPEC_H__
/******************************************************************************/
/*                                                                            */
/*                       b b c p _ F i l e S p e c . h                        */
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
#include <sys/types.h>
#include <utime.h>
#include "bbcp_FileSystem.h"
  
// Parse a spec formed as: [username@][hostname:]fname
//
class bbcp_FileSpec
{
public:

bbcp_FileSpec        *next;
char                 *username;
char                 *hostname;
char                 *pathname;
char                 *filename;
char                 *filereqn;
char                 *fileargs;
char                 *targpath;
char                 *targetfn;
long long             targetsz;
char                 *targsigf;
int                   seqno;
struct bbcp_FileInfo  Info;

int              Compose(long long did, char *dpath, int dplen, char *fname);

int              Create_Path();

int              Decode(char *buff, char *xName=0);

int              Encode(char *buff, size_t blen);

void             ExtendFileSpec(bbcp_FileSpec* headp, int &numF);

int              Finalize(int retc=0);

bbcp_FileSystem *FSys() {return FSp;}

void             Parse(char *spec, int isPipe=0);

int              setMode(mode_t Mode);

int              setStat();

int              Stat(int complain=1);

int              WriteSigFile();

int              Xfr_Done();

     bbcp_FileSpec(bbcp_FileSystem *fsp=0, char *hname = 0, char *uname=0)
                  : next(0), username(uname), hostname(hname), pathname(0),
                    filename(0), filereqn(0), fileargs(0),
                    targpath(0), targetfn(0), targetsz(0), targsigf(0),
                    fspec(0), fspec1(0), fspec2(0), FSp(fsp) {}
    ~bbcp_FileSpec() {if (fspec)    free(fspec);
                      if (fspec1)   free(fspec1);
                      if (fspec2)   free(fspec2);
                      if (targpath) free(targpath);
                      if (targsigf) free(targsigf);
                     }

private:

char            *fspec;
char            *fspec1;
char            *fspec2;
bbcp_FileSystem *FSp;
int              Xfr_Fixup();
void             BuildPaths();

};
#endif
