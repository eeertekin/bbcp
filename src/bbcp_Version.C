/******************************************************************************/
/*                                                                            */
/*                        b b c p _ V e r s i o n . h                         */
/*                                                                            */
/* (c) 2010 by the Board of Trustees of the Leland Stanford, Jr., University  */
/*      All Rights Reserved. Scroll to end for Terms and Conditions of use    */
/*   Produced by Andrew Hanushevsky for Stanford University under contract    */
/*              DE-AC03-76-SFO0515 with the Department of Energy              */
/******************************************************************************/
  
#include <strings.h>

#include "bbcp_Config.h"
#include "bbcp_Emsg.h"
#include "bbcp_Version.h"

/******************************************************************************/
/*                                                                            */
/*                        G l o b a l   O b j e c t s                         */
/*                                                                            */
/******************************************************************************/

extern bbcp_Config  bbcp_Config;

       bbcp_Version bbcp_Version;
  
/******************************************************************************/
/*                           C o n s t r u c t o r                            */
/******************************************************************************/
  
bbcp_Version::bbcp_Version()
{
Copyright = "(c) 2010 by the Board of Trustees of the Leland Stanford, Jr., University";
Author    = "Andrew Hanushevsky";
Version   = "Version: 10.08.04.00.0";
//           0123456789
VData     = Version+9;
};

/******************************************************************************/
/*                                 C h e c k                                  */
/******************************************************************************/
  
int bbcp_Version::Check(char *otherver)
{
    return strcmp(otherver, VData);
};

/******************************************************************************/
/*                                V e r i f y                                 */
/******************************************************************************/
  
int bbcp_Version::Verify(char *node, char *nodever)
{
    int vchk = Check(nodever);
    char *vset;

    if (vchk < 0)         vset = (char *)"an older";
       else if (vchk > 0) vset = (char *)"a newer";
               else return 1;

    bbcp_Fmsg("Version", "Warning:",node,"is running",vset,"version of bbcp");

    if (bbcp_Config.Options & bbcp_BLAB)
       {bbcp_Fmsg("Version", node, "running version", nodever);
        bbcp_Fmsg("Version", bbcp_Config.MyHost, "running version", VData);
       }
    return 0;
};
 
/******************************************************************************/
/*           L i c e n s e   T e r m   a n d   C o n d i t i o n s            */
/******************************************************************************/
  
const char *bbcp_License =
"Copyright (c) 2005-2010, Board of Trustees of the Leland Stanford, Jr. University.\n"
"Produced under contract DE-AC03-76-SF00515 with the US Department of Energy. \n"
"All rights reserved.                                                         \n"
"Redistribution and use in source and binary forms, with or without           \n"
"modification, are permitted provided that the following conditions are met:  \n"
"  a. Redistributions of source code must retain the above copyright notice,  \n"
"this list of conditions and the following disclaimer.                        \n"
"  b. Redistributions in binary form must reproduce the above copyright       \n"
"notice, this list of conditions and the following disclaimer in the          \n"
"documentation and/or other materials provided with the distribution.         \n"
"  c. Neither the name of the Leland Stanford, Jr. University nor the names   \n"
"of its contributors may be used to endorse or promote products derived from  \n"
"this software without specific prior written permission.                     \n"
"THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 'AS IS'  \n"
"AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE    \n"
"IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE   \n"
"ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE     \n"
"LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR          \n"
"CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF         \n"
"SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS     \n"
"INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN      \n"
"CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)      \n"
"ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE   \n"
"POSSIBILITY OF SUCH DAMAGE.";
