#ifndef __BBCP_HEADERS_H
#define __BBCP_HEADERS_H
/******************************************************************************/
/*                                                                            */
/*                        b b c p _ H e a d e r s . h                         */
/*                                                                            */
/* (c) 2009 by the Board of Trustees of the Leland Stanford, Jr., University  */
/*                            All Rights Reserved                             */
/*   Produced by Andrew Hanushevsky for Stanford University under contract    */
/*              DE-AC02-76-SFO0515 with the Department of Energy              */
/******************************************************************************/
  
#if __GNUC__ >= 3 && __GNUC_MINOR__ >= 4
#include <iostream>
#ifdef BBCP_IOMANIP
#include <iomanip>
#endif
using namespace std;
#else
#include <iostream.h>
#ifdef BBCP_IOMANIP
#include <iomanip.h>
#endif
#endif
#endif
