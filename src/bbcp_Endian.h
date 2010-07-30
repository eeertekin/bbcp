#ifndef __BBCP_ENDIAN_H
#define __BBCP_ENDIAN_H
/******************************************************************************/
/*                                                                            */
/*                         b b c p _ E n d i a n . h                          */
/*                                                                            */
/* (c) 2010 by the Board of Trustees of the Leland Stanford, Jr., University  */
/*                            All Rights Reserved                             */
/*   Produced by Andrew Hanushevsky for Stanford University under contract    */
/*              DE-AC02-76-SFO0515 with the Department of Energy              */
/******************************************************************************/

// Include stdlib and sys/param so that ENDIAN macros are defined properly
//
#include <stdlib.h>
#include <sys/param.h>
  
#if defined(_BIG_ENDIAN) || defined(__BIG_ENDIAN__) || \
   defined(__IEEE_BIG_ENDIAN) || \
   (defined(__BYTE_ORDER) && __BYTE_ORDER == __BIG_ENDIAN)
#define BBCP_BIG_ENDIAN
#endif
#endif
