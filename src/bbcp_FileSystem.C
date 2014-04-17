/******************************************************************************/
/*                                                                            */
/*                     b b c p _ F i l e S y s t e m . C                      */
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
  
#include "bbcp_Headers.h"
#include "bbcp_FS_Null.h"
#include "bbcp_FS_Pipe.h"
#include "bbcp_FS_Unix.h"

// This file holds the function that returns an applicable filesystem object
// for any given path (or null if none can be found). We do this by simply
// statically instantiating all of the supported filesystems and returning
// the first one that claims to be applicable to the given path.

/******************************************************************************/
/*                    b b c p _ g e t F i l e S y s t e m                     */
/******************************************************************************/
  
bbcp_FileSystem *bbcp_FileSystem::getFS(const char *path, int Opts)
{

   static bbcp_FS_Null bbcp_NULL;
   static bbcp_FS_Pipe bbcp_PIPE;
   static bbcp_FS_Unix bbcp_UFS;

// Simply try each supported filesystem (add more as they are defined)
//
   if (bbcp_NULL.Applicable(path))  return (bbcp_FileSystem *)&bbcp_NULL;
   if (Opts & getFS_Pipe)           return (bbcp_FileSystem *)&bbcp_PIPE;
   if (bbcp_UFS.Applicable(path))   return (bbcp_FileSystem *)&bbcp_UFS;

// All done, nothing is applicable
//
   return (bbcp_FileSystem *)0;
}
