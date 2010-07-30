#ifndef __BBCP_PROTOCOL_H__
#define __BBCP_PROTOCOL_H__
/******************************************************************************/
/*                                                                            */
/*                       b b c p _ P r o t o c o l . h                        */
/*                                                                            */
/* (c) 2002 by the Board of Trustees of the Leland Stanford, Jr., University  */
/*      All Rights Reserved. See bbcp_Version.C for complete License Terms    *//*                            All Rights Reserved                             */
/*   Produced by Andrew Hanushevsky for Stanford University under contract    */
/*              DE-AC03-76-SFO0515 with the Department of Energy              */
/******************************************************************************/

#include <unistd.h>
#include <sys/types.h>
#include "bbcp_Config.h"
#include "bbcp_FileSpec.h"
#include "bbcp_FileSystem.h"
#include "bbcp_Link.h"

class bbcp_Node;
  
class bbcp_Protocol
{
public:

int    setCBPort(int pnum);

int    Schedule(bbcp_Node *Fnode, bbcp_FileSpec *Ffs, char *Fcmd, char *Ftype,
                bbcp_Node *Lnode, bbcp_FileSpec *Lfs, char *Lcmd, char *Ltype,
                bbcp_Node *Tnode);

int    Login(bbcp_Link *Net, int incomming)
            {if (incomming) return Process_login(Net);
             return Request_login(Net);
            }

int    Process(bbcp_Node *Node);

int    Request(bbcp_Node *Node);

       bbcp_Protocol() {Local = 0; Remote = 0; tdir = 0;}
      ~bbcp_Protocol() {if (Remote) delete Remote;}

private:

bbcp_Node       *Local;
bbcp_Node       *Remote;
bbcp_FileSystem *fs_obj;
char            *tdir;
long long        tdir_id;

int   AdjustWS(char *wp, char *bp, int Final);
int   getCBPort(bbcp_Node *Node);
void  getEnd(bbcp_Node *Node);
void  putCSV(char *Host, char *csFn, char *csVal, int csVsz);

int   Process_exit();
int   Process_flist();
int   Process_get();
int   Process_login(bbcp_Link *Net);

int   Request_exit(int retc);
int   Request_flist(long long &totsz);
int   Request_get(bbcp_FileSpec *fp);
int   Request_login(bbcp_Link *Net);

int   SendArgs(bbcp_Node *Node, bbcp_FileSpec *fsp, char *cbhost, int cbport);
};
#endif
