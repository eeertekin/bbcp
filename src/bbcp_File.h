#ifndef __BBCP_FILE_H__
#define __BBCP_FILE_H__
/******************************************************************************/
/*                                                                            */
/*                           b b c p _ F i l e . h                            */
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

#include <string.h>
#include "bbcp_BuffPool.h"
#include "bbcp_IO.h"
#include "bbcp_Pthread.h"
#include "bbcp_Timer.h"

// The bbcp_File class describes the set of operations that are needed to copy
// a "file". The actual I/O are determined by the associated filesystem and
// are specified during instantiation.

class  bbcp_FileChkSum;
class  bbcp_FileSystem;

class bbcp_File
{
public:

// Return target file system
//
bbcp_FileSystem *Fsys() {return FSp;}

// Return FD number
//
int          ioFD() {return IOB->FD();}

// Read a record from a file
//
ssize_t      Get(char *buff, size_t blen) {return IOB->Read(buff, blen);}

// Internal buffer-to-buffer passthrough function
//
int          Passthru(bbcp_BuffPool *iBP, bbcp_BuffPool *oBP,
                      bbcp_FileChkSum *csP, int nstrms);

// Return path to the file
//
char        *Path() {return iofn;}

// Write a record to a file
//
ssize_t      Put(char *buff, size_t blen) {return IOB->Write(buff, blen);}

// Read_All() reads the file until eof and returns 0 (success) or -errno.
//
int          Read_All(bbcp_BuffPool &buffpool, int Bfact);

// Write_All() writes the file until eof and return 0 (success) or -errno.
//
int          Write_All(bbcp_BuffPool &buffpool, int nstrms);

// Sets the file pointer to read or write from an offset
//
int          Seek(long long offv) {nextoffset = offv; return IOB->Seek(offv);}

// setSize() sets the expected file size
//
void         setSize(long long top) {lastoff = top;}

// Stats() reports the i/o time and buffer wait time in milliseconds and
//         returns the total number of bytes transfered.
//
long long    Stats(double &iotime) {return IOB->ioStats(iotime);}

long long    Stats()               {return IOB->ioStats();}

             bbcp_File(const char *path, bbcp_IO *iox,
                       bbcp_FileSystem *fsp, int secSize=0);

            ~bbcp_File() {if (iofn) {free(iofn); iofn = 0;}
                          if (IOB)  {delete IOB; IOB  = 0;}
                         }

int          bufreorders;
int          maxreorders;

private:

bbcp_Buffer     *nextbuff;
long long        nextoffset;
long long        lastoff;
long long        bytesLeft;
long long        blockSize;
long long        PaceTime;
bbcp_Timer       Ticker;
int              rtCopy;
int              curq;
bbcp_IO         *IOB;
bbcp_FileSystem *FSp;
char            *iofn;

bbcp_Buffer *getBuffer(long long offset);
int          Read_Direct (bbcp_BuffPool *inB, bbcp_BuffPool *otP);
int          Read_Pipe   (bbcp_BuffPool *inB, bbcp_BuffPool *otP);
int          Read_Normal (bbcp_BuffPool *inB, bbcp_BuffPool *otP);
int          Read_Vector (bbcp_BuffPool *inB, bbcp_BuffPool *otP, int vN);
void         Read_Wait   (int rdsz);
void         Read_Wait   ();
int          verChkSum(bbcp_FileChkSum *csP);
int          Write_Direct(bbcp_BuffPool *iBP, bbcp_BuffPool *oBP, int nstrms);
int          Write_Normal(bbcp_BuffPool *iBP, bbcp_BuffPool *oBP, int nstrms);
};
#endif
