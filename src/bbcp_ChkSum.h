#ifndef __BBCP_ChkSum_H__
#define __BBCP_ChkSum_H__
/******************************************************************************/
/*                                                                            */
/*                         b b c p _ C h k S u m . h                          */
/*                                                                            */
/* (c) 2010 by the Board of Trustees of the Leland Stanford, Jr., University  */
/*                            All Rights Reserved                             */
/*   Produced by Andrew Hanushevsky for Stanford University under contract    */
/*              DE-AC02-76-SFO0515 with the Department of Energy              */
/******************************************************************************/
  
class bbcp_ChkSum
{
public:

static bbcp_ChkSum *Alloc(int csType);

// The below calculates a one-time checksum
//
virtual       char *Calc(const char *Buff, int BLen)
                        {Init(); Update(Buff, BLen); return Final();}

// The below returns the current checksum value (defaults to final)
//
virtual       char *csCurr(char **Text=0) {return Final(Text);}

// csLen() returns the length of a checksum
//
virtual       int   csSize() = 0;

// At the end call Final that will return the actual checksum
//
virtual       char *Final(char **Text=0) = 0;

// Init() to initialize data structures (always called by constructor)
//
virtual       void  Init() = 0;

// Call this to get the character name of the checksum object
//
virtual const char *Type() = 0;

// Continue calling Update() to compute running checksum
//
virtual       void  Update(const char *Buff, int BLen) = 0;

                    bbcp_ChkSum() {}
virtual            ~bbcp_ChkSum() {}

protected:
char *x2a(char *inX);

char csBuff[40];
};
#endif
