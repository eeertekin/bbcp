#ifndef __BBCP_VERSION_H__
#define __BBCP_VERSION_H__
/******************************************************************************/
/*                                                                            */
/*                        b b c p _ V e r s i o n . h                         */
/*                                                                            */
/* (c) 2002 by the Board of Trustees of the Leland Stanford, Jr., University  */
/*      All Rights Reserved. See bbcp_Version.C for complete License Terms    *//*                            All Rights Reserved                             */
/*   Produced by Andrew Hanushevsky for Stanford University under contract    */
/*              DE-AC03-76-SFO0515 with the Department of Energy              */
/******************************************************************************/
  
class bbcp_Version
      {public:
       const char  *Copyright;
       const char  *Author;
       const char  *Version;
       const char  *VData;

             int    Check(char *vno);

             int    Verify(char *node, char *vno);

      ~bbcp_Version() {}
       bbcp_Version();
       };
#endif
