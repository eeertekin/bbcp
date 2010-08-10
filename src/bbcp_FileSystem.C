/******************************************************************************/
/*                                                                            */
/*                     b b c p _ F i l e S y s t e m . C                      */
/*                                                                            */
/* (c) 2002 by the Board of Trustees of the Leland Stanford, Jr., University  */
/*      All Rights Reserved. See bbcp_Version.C for complete License Terms    *//*                            All Rights Reserved                             */
/*   Produced by Andrew Hanushevsky for Stanford University under contract    */
/*              DE-AC03-76-SFO0515 with the Department of Energy              */
/******************************************************************************/
  
#include "bbcp_Headers.h"
#include "bbcp_FS_Null.h"
#include "bbcp_FS_Unix.h"

// This file holds the function that returns an applicable filesystem object
// for any given path (or null if none can be found). We do this by simply
// statically instantiating all of the supported filesystems and returning
// the first one that claims to be applicable to the given path.

/******************************************************************************/
/*                        G l o b a l   O b j e c t s                         */
/******************************************************************************/

bbcp_FS_Null bbcp_NULL;
  
bbcp_FS_Unix bbcp_UFS;

/******************************************************************************/
/*                    b b c p _ g e t F i l e S y s t e m                     */
/******************************************************************************/
  
bbcp_FileSystem *bbcp_getFileSystem(const char *path)
{

// Simply try each supported filesystem (add more as they are defined)
//
   if (bbcp_NULL.Applicable(path))  return (bbcp_FileSystem *)&bbcp_NULL;
   if (bbcp_UFS.Applicable(path))   return (bbcp_FileSystem *)&bbcp_UFS;

// All done, nothing is applicable
//
   return (bbcp_FileSystem *)0;
}
