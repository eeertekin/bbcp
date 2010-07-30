#ifndef __BBCP_SYSTEM_H__
#define __BBCP_SYSTEM_H__
/******************************************************************************/
/*                                                                            */
/*                         b b c p _ S y s t e m . h                          */
/*                                                                            */
/* (c) 2002 by the Board of Trustees of the Leland Stanford, Jr., University  */
/*      All Rights Reserved. See bbcp_Version.C for complete License Terms    *//*                            All Rights Reserved                             */
/*   Produced by Andrew Hanushevsky for Stanford University under contract    */
/*              DE-AC03-76-SFO0515 with the Department of Energy              */
/******************************************************************************/
  
#include <sys/types.h>
#include "bbcp_Pthread.h"

class bbcp_System
{
public:

// Spawn a new process and return the process id (0 if child)
//
pid_t     Fork();

// Convert a group name to a GID return -1 if failed.
//
gid_t      getGID(const char *group);

// Convert a GID to a group name return "nogroup" if failed.
//
char      *getGNM(gid_t gid);

// Get the home directory
//
char      *getHomeDir();

// Get the process id of the grandparent process
//
pid_t      getGrandP();

// Get total cpu time spent in seconds
//
int       Usage(int  &sys, int  &usr);

// Get my username
//
char      *UserName();

// Wait for a process to end
//
int        Waitpid(pid_t *pvec, int *ent=0, int nomsg=0);

           bbcp_System();
          ~bbcp_System() {}

char     **EnvP;        // Global environment pointer
long long  FreeRAM;     // Free ememory at startup
int        FreePag;     // Number of free pages at startup
int        TotPag;      // Maximum number of readv/writev elements
int        PageSize;    // Page size

private:

bbcp_Mutex Glookup;
};
#endif
