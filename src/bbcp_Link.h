#ifndef __BBCP_LINK_H__
#define __BBCP_LINK_H__
/******************************************************************************/
/*                                                                            */
/*                           b b c p _ L i n k . h                            */
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

#include <errno.h>
#include <sys/types.h>
#include <fcntl.h>
#include "bbcp_BuffPool.h"
#include "bbcp_ChkSum.h"
#include "bbcp_File.h"
#include "bbcp_IO.h"
#include "bbcp_Pthread.h"

// The bbcp_Link class defines the operations on a network link.
//
class bbcp_Link
{
public:

int          Close() {if (csObj) delete csObj; return IOB.Close();}

int          FD() {return IOB.FD();}

char        *LinkName() {return Lname;}

void         setBuddy(bbcp_Link *mybuddy) {Buddy = mybuddy;}

static void  setNudge() {Nudge = 1; Wait = 1;}

int          Buff2Net();

int          Net2Buff();

             bbcp_Link(int newfd=-1, const char *newfn="");
            ~bbcp_Link() {Close(); if (Lname) free(Lname);}

int          LinkNum;

private:

static int      Nudge;
static int      Wait;

bbcp_Link      *Buddy;
bbcp_Semaphore  Rendezvous;
bbcp_IO         IOB;
bbcp_ChkSum    *csObj;
char           *Lname;

int        Control_In( bbcp_Buffer *inbuff);
int        Control_Out(bbcp_Buffer *outbuff);
};
#endif
