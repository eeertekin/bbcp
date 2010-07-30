
#ifndef _LOGGING_H_
#define _LOGGING_H_

/*****************************************************************

This software is copyright (C) by the Lawrence Berkeley National Laboratory.
Permission is granted to reproduce this software for non-commercial
purposes provided that this notice is left intact.

It is acknowledged that the U.S. Government has rights to this software
under Contract DE-AC03-76SF00098 between the U.S.  Department of Energy
and the University of California.

This software is provided as a professional and academic contribution
for joint exchange. Thus, it is experimental, and is provided ``as is'',
with no warranties of any kind whatsoever, no support, no promise of
updates, or printed documentation. By using this software, you
acknowledge that the Lawrence Berkeley Laboratory and Regents of the
University of California shall have no liability with respect to the
infringement of other copyrights by any part of this software.

For further information about this notice, contact Brian L. Tierney
Bld. 50B, Rm. 2239, Lawrence Berkeley National Laboratory, Berkeley, CA, 94720
(bltierney@lbl.gov)

*******************************************************/

#include <stdio.h>

#if defined(NL_THREADSAFE)
#include <pthread.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/*
 * Backwards compatibility on variable name
 */
#define NLhandle NL_Handle

/* 
 * Error codes 
 */
#define NL_OK 0
#define NL_ERR -1

/*
 * Flags for NetLoggerOpen
 */
   typedef enum 
   { 
      NL_ENV = 1,
      NL_MEM = 8,
      NL_APPEND = 16 
   } 
   NL_openflag_t;

/* NetLogger method codes */
#define NL_FILE 0
#define NL_SYSLOG 1
#define NL_HOST 2
#define NL_FILE_MEM 3
#define NL_SYSLOG_MEM 4
#define NL_HOST_MEM 5
#define NL_ENV_METHOD -1

/* Constants */
#define NL_MAX_BUFFER 10 * 1024 * 1024 /* maximum number of events to buffer in memory
		  	      before requiring a NetLoggerFlush call */
#define NL_MAX_MESSAGE 1024 /* Maximum length for 1 message! */

#define DATE_LEN 22 /* Length of value of DATE= field */
#define FALLBACK_HOSTNAME "127.0.0.1" /* Hostname if network down */

#ifndef  DPSS_NETLOGGER_PORT
#define DPSS_NETLOGGER_PORT 14830
#endif

/* NetLogger environment variable */
#ifndef NL_ON_ENV
#    define NL_ON_ENV "NETLOGGER_ON"
#endif
enum { NL_ON=1, NL_OFF=0, NL_NOTSET=2 };

/* NetLogger log destination environemnt var */
#ifndef NL_DEST_ENV
#    define NL_DEST_ENV "NETLOGGER_DEST"
#endif

/* struct for binary NetLogger Format option */
typedef struct _nlbin {
    int size;  /* number of bytes in this message */
    int id;    /* unique ID for this stream of events*/
	/* NOTE: what is the best way to generate this? This is only needed
		by netlogd, so maybe netlogd should generate it? */
    int seconds;
    int frac_secs;
    int precision;
    int accuracy;
    char *ascii_data;  /* host, program, event_name, and user_data */
} NL_binary;


typedef struct _loginfo {
   int enabled;
   int loglvl; /* level of messages that will be
		* logged. 0=all */
   int method;
   int binary;
    FILE *log_fp; /* File * and fd refer to same thing */
   int   log_fd;
   int doNotClose; /* Flag for NetLoggerClose */
   int log_socket;
   int failed; /* If server dies, this flag will keep track of a dead connection. */
   char *pname;
   char *hname;
   char *filename;
   char *hostname;
   int curr_buffer_size;
   char *event_buffer;   /* for the log to memory option */
   char *message_buffer;   /* current message */
   char *last_date;  /* can speed things up a lot if dont have to 
			rebuild these */
   char *host_prog;   /* part of message containing HOST and PROG info */
   long last_tv_sec;

    char *tag;

#if defined(NL_THREADSAFE)
   pthread_mutex_t *mtx;
#endif 
#  ifdef CORRECT_TIMESTAMPS
   long tshift_sec;
   long tshift_usec;
#   endif /* CORRECT_TIMESTAMPS */
#ifdef USE_GLOBUS
   globus_io_handle_t * handle;
#endif
} NL_Handle;

/*---------------- NetLogger Routines --------------------*/
    NL_Handle * NetLoggerOpen( char *program_name, char *url, int flags );
    NL_Handle * NetLoggerOpenFd (char *p_name, int fd, int flags);
#ifndef OLD_API
    /*
     * All the variations on NetLoggerWrite():
     */
    /* Basic call, automatic timestamp */
    int NetLoggerWrite(NL_Handle * lp,
		       char *keyword,
		       char *const_fmt,
		       char *variable_fmt, 
		       ... );
    /* User-provided timestamp */
    int NetLoggerGTWrite(NL_Handle *lp, 
			 long sec,
			 long usec,
			 char *keyword, 
			 char *const_fmt,
			 char *variable_fmt,
			 ...);
    /* With log(ging) level */
    int NetLoggerWriteLvl( NL_Handle * lp, 
			   int lvl, 
			   char *keyword, 
			   char *const_fmt,
			   char *variable_fmt, 
			   ... );
    /* With log(ging) level and user timestamp */
    int NetLoggerGTWriteLvl( NL_Handle * lp, 
			     long sec,
			     long usec,
			     int lvl, 
			     char *keyword, 
			     char *const_fmt,
			     char *variable_fmt, 
			     ... );
    /* Write a string (not varargs) */
    int NetLoggerWriteString (NL_Handle * lp,
			      char *keyword, 
			      char *const_data,
			      char *variable_data);
    /* Write a string with log level */
    int NetLoggerWriteStringLvl (NL_Handle * lp,
				 int lvl,
				 char *keyword, 
				 char *const_data,
				 char *variable_data);
    /* Write a string with user-provided timestamp */
    int NetLoggerGTWriteString(NL_Handle * lp, 
			       long sec, 
			       long usec,			       
			       char *keyword, 
			       char *const_data,
			       char *variable_data);
    /* Write a string with log level and user timestamp */
    int NetLoggerGTWriteStringLvl(NL_Handle * lp,
				  int lvl,
				  long sec, long usec,
				  char *keyword,
				  char *const_data,
				  char *variable_data);
#else
/* Basic call */
    int NetLoggerWrite (NL_Handle * lp, char *keyword,
			char *fmt, ...);
/* user passes in timing info */
    int NetLoggerGTWrite(NL_Handle *lp, long sec, long usec, char *keyword, 
			 char *fmt,...);
/* non-varargs versions of call */
    int NetLoggerWriteString (NL_Handle * lp, char *keyword, char *data);
    int NetLoggerGTWriteString(NL_Handle * lp, long sec, long usec,
			       char *keyword, char *data);
    int NetLoggerWriteStringLvl (NL_Handle * lp,
				 int lvl,
				 char *keyword, 
				 char *data);
    int NetLoggerGTWriteStringLvl(NL_Handle * lp,
				  int lvl,
				  long sec, long usec,
				  char *keyword,
				  char *data);
/* Write if lvl <= handle's logging level */
    int NetLoggerWriteLvl( NL_Handle * lp, 
			   int lvl, 
			   char *keyword, 
			   char *fmt, 
			   ... );
#endif
    int       NetLoggerFastWrite(NL_Handle *lp, char *keyword, char *fmt,...);
    int       NetLoggerFlush(NL_Handle *lp);
    int       NetLoggerClose(NL_Handle *lp);
    int       NetLoggerSetGlobalTag(char *tag);
    int       NetLoggerSetTag(NL_Handle *lp, char *tag );
    void      NetLoggerSetLevel(NL_Handle *lp, int log_level);

/*--------------------- internal NetLogger routines --------------*/
FILE  *LogOpen(char *logfile_name, char *mode);
int   SysLogOpen(char *prog_name, int syslog_facility);
int   NetLogOpen(char *hostname, int port);
void NetLogSetMethod( char *s, /* Value of environment variable */
                      int flags,
		      int *method_p,
		      char **filename_p,
		      char **hostname_p,
		      int   *port_p);
int   LogClose(FILE * fp);
int   SysLogClose(void);
int   NetLogClose(int socket);
int   NetLogGetEnv(char *envvar);
void NetLogWriteHeader( int sockfd, int flags );

/** Return 1 if netlogger should log, 0 otherwise */
#define CheckLogLevel(H,L) \
  (((H)->loglvl > 0) && ((L) <= (H)->loglvl))

/** Return 1 if handle is valid and active */
#define CheckHandle(H) ((H) != NULL && (H)->enabled != NL_OFF)

/* void dead_handler(int sig); */

/* OLD **
   NL_Handle *NetLoggerOpen(int method, char *pname, char *filename, char *hostname, int port);
*/

/* for globus-io testing */
#define JRLEE_LBL_AUTH "/C=US/O=Globus/O=Lawrence Berkeley National " \
                       "Laboratory/OU=Future Technologies Group/CN=" \
		       "Jason Lee (FTG staff)"


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LOGGING_H_ */

