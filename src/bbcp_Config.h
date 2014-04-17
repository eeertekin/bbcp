#ifndef _BBCP_CONFIG_H
#define _BBCP_CONFIG_H
/******************************************************************************/
/*                                                                            */
/*                         b b c p _ C o n f i g . h                          */
/*                                                                            */
/*(c) 2002-14 by the Board of Trustees of the Leland Stanford, Jr., University*//*      All Rights Reserved. See bbcp_Version.C for complete License Terms    *//*                            All Rights Reserved                             */
/*   Produced by Andrew Hanushevsky for Stanford University under contract    */
/*               DE-AC02-76-SFO0515 with the Deprtment of Energy              */
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

#include "bbcp_Debug.h"
#include "bbcp_FileSpec.h"
#include "bbcp_LogFile.h"
#include "bbcp_Stream.h"

class bbcp_Args;

class bbcp_Config
{
public:

long long     Options;
int           bindtries;
int           bindwait;
int           Mode;
int           ModeD;
int           ModeDC;
int           Streams;
int           BAdd;
int           Bfact;
int           BNum;
int           Progint;
int           Complvl;
int           Wsize;
int           MaxWindow;
int           MaxSegSz;
int           Xrate;
int           RWBsz;

char         *SrcBuff;
char         *SrcBase;
int           SrcBlen;
char         *SrcUser;
char         *SrcHost;
char         *SrcXeq;
char         *SrAddr;
char         *SnkXeq;
char         *Logurl;
char         *CBhost;
int           CBport;
char         *SecToken;
char         *MyAddr;
char         *MyHost;
char         *MyUser;
char         *MyProg;
char         *CKPdir;
char         *IDfn;
char         *Logfn;
char         *LogSpec;
char         *RepSpec;
char         *SynSpec;
char         *PorSpec;
bbcp_LogFile *MLog;
char         *CopyOpts;
char         *CopyOSrc;
char         *CopyOTrg;
int           lastseqno;
int           TimeLimit;
int           MTLevel;

char         *rtSpec;
char         *rtLockf;
int           rtCheck;
int           rtLimit;
int           rtLockd;

int           csOpts;
#define bbcp_csVerIn  0x00000001
#define bbcp_csVerOut 0x00000002
#define bbcp_csVerIO  0x00000004
#define bbcp_csVerAll 0x00000007
#define bbcp_csPrint  0x00000008
#define bbcp_csSend   0x00000010
#define bbcp_csLink   0x00000080
#define bbcp_csDashE  0x00000800

int           csSize;
int           csType;
#define bbcp_csNOP    0
#define bbcp_csA32    1
#define bbcp_csC32    2
#define bbcp_csMD5    3

int           csFD;
char         *csPath;
char         *csSpec;
char          csName[8];
char          csValue[16];
char          csString[44];

char          ubSpec[4];
char          upSpec[4];

bbcp_FileSpec  *srcPath;
bbcp_FileSpec  *srcSpec;
bbcp_FileSpec  *srcLast;
bbcp_FileSpec  *snkSpec;

const char *SrcArg;
const char *SnkArg;

void  Arguments(int argc, char **argv, int cfgfd=-1);
int   ConfigInit(int argc, char **argv);
int   Configure(const char *cfn);
void  Display();
void  setCS(char *buff);
void  setRWB(int rwbsz);
int   a2sz(const char *etxt, char *item, int  &result, int  minv, int  maxv);
int   a2tm(const char *etxt, char *item, int  &result, int  minv, int  maxv);
int   a2ll(const char *etxt, char *item, long long &result,
                         long long minv, long long maxv);
int   a2n(const char *etxt, char *item, int  &result, int  minv, int  maxv);
int   a2o(const char *etxt, char *item, int  &result, int  minv, int  maxv);
int   a2x(char *result, char *item, int ilen);
char *n2a(int  val, char *buff, const char *fmt="%d");
char *n2a(long long  val, char *buff, const char *fmt="%lld");
void  WAMsg(const char *who, const char *act, int newsz);

static const char *Scale(double &xVal);

      bbcp_Config();
     ~bbcp_Config();

private:
char *OPp;
void  Cleanup(int rc, char *cfgfn, int cfgfd);
void  Config_Ctl(int);
void  Config_Xeq(int);
int   EOpts(char *Opts);
void  help(int rc);
int   HostAndPort(const char *what, char *path, char *buff, int bsz);
int   LogOpts(char *opts);
void  ParseSB(char *spec);
int   ROpts(char *Opts);
int   ROptsErr(char *Opts);
char *Rtoken(void);
int   setIPV4(char *Opts);
void  setOpts(bbcp_Args &Args);
int   setPorts(char *pspec);
char *tohex(char *inbuff, int inlen, char *outbuff);
int   Unbuff(char *Opts);
int   Unpipe(char *Opts);
};

/******************************************************************************/
/*                   O p t i o n   D e f i n o i t i o n s                    */
/******************************************************************************/
  
#define bbcp_APPEND   0x0000000000000001LL
#define bbcp_COMPRESS 0x0000000000000002LL
#define bbcp_NOUNLINK 0x0000000000000004LL
#define bbcp_FORCE    0x0000000000000008LL

#define bbcp_KEEP     0x0000000000000010LL
#define bbcp_PCOPY    0x0000000000000020LL
#define bbcp_VERBOSE  0x0000000000000040LL
#define bbcp_TRACE    0x0000000000000080LL

#define bbcp_OUTDIR   0x0000000000000100LL
#define bbcp_CON2SRC  0x0000000000000200LL
#define bbcp_BLAB     0x0000000000000400LL
#define bbcp_ORDER    0x0000000000000800LL

#define bbcp_RECURSE  0x0000000000001000LL
#define bbcp_IDIO     0x0000000000002000LL
#define bbcp_ODIO     0x0000000000004000LL
#define bbcp_MANTUNE  0x0000000000008000LL

#define bbcp_RELATIVE 0x0000000000010000LL
#define bbcp_SRC      0x0000000000020000LL
#define bbcp_SNK      0x0000000000040000LL
#define bbcp_SSDATA   0x0000000000060000LL
#define bbcp_DOBIND   0x0000000000080000LL

#define bbcp_REPBUFFS 0x0000000000100000LL
#define bbcp_REPUSAGE 0x0000000000200000LL
#define bbcp_NOSPCHK  0x0000000000400000LL
#define bbcp_NODNS    0x0000000000800000LL

#define bbcp_LOGIN    0x0000000001000000LL
#define bbcp_LOGOUT   0x0000000002000000LL
#define bbcp_LOGRD    0x0000000004000000LL
#define bbcp_LOGWR    0x0000000008000000LL

#define bbcp_LOGCMP   0x0000000010000000LL
#define bbcp_LOGEXP   0x0000000020000000LL
#define bbcp_LOGGING  0x000000003f000000LL

#define bbcp_RTCOPY   0x0000000300000000LL
#define bbcp_RTCSRC   0x0000000100000000LL
#define bbcp_RTCSNK   0x0000000200000000LL
#define bbcp_RTCBLOK  0x0000000400000000LL
#define bbcp_RTCHIDE  0x0000000800000000LL

#define bbcp_RTCVERC  0x0000001000000000LL
#define bbcp_OMIT     0x0000002000000000LL
#define bbcp_IPIPE    0x0000004000000000LL
#define bbcp_OPIPE    0x0000008000000000LL

#define bbcp_XPIPE    0x000000c000000000LL

#define bbcp_NOFSZCHK 0x0000010000000000LL

#define bbcp_FSYNC    0x0000020000000000LL
#define bbcp_DSYNC    0x0000060000000000LL

#define bbcp_IPV4     0x0000400000000000LL

#define BBCP_MAXSTREAMS 64
#define BBCP_MINPMONSEC  1

#define BBCP_DFLTMINPORT 5031
#define BBCP_DFLTMAXPORT 5039
#endif
