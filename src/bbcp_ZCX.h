#ifndef __bbcp_ZCX_H__
#define __bbcp_ZCX_H__
/******************************************************************************/
/*                                                                            */
/*                            b b c p _ Z C X . h                             */
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

class bbcp_BuffPool;

class bbcp_ZCX
{
public:

long long    Bytes() {return cbytes;}

int          Process(void);

pthread_t    TID;

      bbcp_ZCX(bbcp_BuffPool *ib, bbcp_BuffPool *rb, bbcp_BuffPool *ob,
               int clvl, int xfd, int logit);
     ~bbcp_ZCX() {}

private:

int Zfailure(int zerr, const char *oper, char *Zmsg);

bbcp_BuffPool   *Ibuff;
bbcp_BuffPool   *Rbuff;
bbcp_BuffPool   *Obuff;
long long        cbytes;
int              Clvl;
int              iofd;
char            *LogIDbeg;
char            *LogIDend;
};
#endif
