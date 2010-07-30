#ifndef _BBCP_CONFIG_H
#define _BBCP_CONFIG_H
/******************************************************************************/
/*                                                                            */
/*                         b b c p _ C o n f i g . h                          */
/*                                                                            */
/* (C) 2002 by the Board of Trustees of the Leland Stanford, Jr., University  */
/*      All Rights Reserved. See bbcp_Version.C for complete License Terms    *//*                            All Rights Reserved                             */
/*   Produced by Andrew Hanushevsky for Stanford University under contract    */
/*               DE-AC03-76-SFO0515 with the Deprtment of Energy              */
/******************************************************************************/

#include "bbcp_Debug.h"
#include "bbcp_FileSpec.h"
#include "bbcp_LogFile.h"
#include "bbcp_Stream.h"

class bbcp_Config
{
public:

int           Options;
int           bindtries;
int           bindwait;
int           Mode;
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
int           lastseqno;
char         *Logfn;
char         *LogSpec;
char         *RepSpec;
bbcp_LogFile *MLog;
char         *CopyOpts;
int           CopyOptt;
int           TimeLimit;
int           MTLevel;
signed int    Jitter;

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
void  ParseSB();
char *Rtoken(void);
char *tohex(char *inbuff, int inlen, char *outbuff);
int   Unbuff(char *Opts);
};

/******************************************************************************/
/*                   O p t i o n   D e f i n o i t i o n s                    */
/******************************************************************************/
  
#define bbcp_APPEND   0x00000001
#define bbcp_COMPRESS 0x00000002
#define bbcp_NOUNLINK 0x00000004
#define bbcp_FORCE    0x00000008
#define bbcp_KEEP     0x00000010
#define bbcp_PCOPY    0x00000020
#define bbcp_VERBOSE  0x00000040
#define bbcp_TRACE    0x00000080
#define bbcp_OUTDIR   0x00000100
#define bbcp_CON2SRC  0x00000200
#define bbcp_BLAB     0x00000400
#define bbcp_ORDER    0x00000800
#define bbcp_RECURSE  0x00001000
#define bbcp_IDIO     0x00002000
#define bbcp_ODIO     0x00004000
#define bbcp_MANTUNE  0x00008000

#define bbcp_RELATIVE 0x00010000
#define bbcp_SRC      0x00020000
#define bbcp_SNK      0x00040000
#define bbcp_SSDATA   0x00060000
#define bbcp_DOBIND   0x00080000

#define bbcp_REPBUFFS 0x00100000
#define bbcp_REPUSAGE 0x00200000
#define bbcp_NOSPCHK  0x00400000
#define bbcp_NODNS    0x00800000

#define bbcp_LOGIN    0x01000000
#define bbcp_LOGOUT   0x02000000
#define bbcp_LOGRD    0x04000000
#define bbcp_LOGWR    0x08000000
#define bbcp_LOGCMP   0x10000000
#define bbcp_LOGEXP   0x20000000
#define bbcp_LOGGING  0x3f000000

#define BBCP_MAXSTREAMS 64
#define BBCP_MINPMONSEC  1

#define BBCP_DFLTMINPORT 5031
#define BBCP_DFLTMAXPORT 5039
#endif
