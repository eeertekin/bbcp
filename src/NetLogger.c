/*
 * $Author$
 * $RCSfile$
 * $Revision$
 * $Locker$
 * $Date$
 */


/***********************************************************************
 * NetLogger client API.
 *
 * creates log messages in ULM (Universal Logger Message) format, as
 *	defined by the IETF Internet-Draft
 *	(see: ftp://ds.internic.net/internet-drafts/draft-abela-ulm-02.txt)
 *
 * The ULM format is as follows:
 *   DATE=YYYYMMDDhhmmss.38298 HOST=host PROG=progname F1= F2=......
 *
 *    and where F1, F2, etc are any number of user defined fields.
 *
 *  ULM recommends that field names are 4 or less characters.
 *
 * NetLogger always adds the following fields:
 *		NL.EVNT=event_name 
 *		NL.EVNT: NetLogger event name
 *
 * Sample ULM NetLogger message:
 *   DATE=19980401103010.28294 HOST=dpss.lbl.gov PROG=dpss_mast  
 *          NL.EVNT=ISS_MASTER_IN DPSS.SES=1 DPSS.BID=15.98.144.0
 *
 *  for more info see: http://www-didc.lbl.gov/NetLogger
 *
 *    Brian Tierney, LBNL
 ***********************************************************************
 *
 */

#ifndef lint
static const char rcsid[] =
    "@(#) $Id$";
#endif

#include <sys/types.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <sys/param.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

/* Uncomment so it works with "C" netlogd */
/* #define SEND_MESSAGE_SIZE  1 */

 /* VERBOSE: NetLogger lib prints some status messages about
    what it is doing.  This should not be defined 
    if you are using netlogger in a program started using inetd
    NOTE: to do: need to add option to send error messages to syslogd instead
    of to stderr  -blt
  */
/* #define VERBOSE */

/* this slows down netlogger by about 2%: but on by default to avoid
    subtle threading bugs */
/* if you turn this on, be sure to compile with threads too:
	e.g.:	gcc -D_REENTRANT -g -pthread -c logging.c
*/
#define NL_THREADSAFE 1

/* turn this on to turn off all NetLogger status messages: important
	to do this for servers started from inetd */
/*
#define NETLOGGER_SILENT
*/

/* ISO8601: use the Grid Forum proposed time standard format: 
	the ISO8601 format to improve readability. Note that this 
	slows down the NetLoggerWrite call by about 2%.
*/
/*
#define ISO8601
*/

/* BINARY_MSG_FORMAT: experimental binary format that keeps the
	timestamp in NTP binary format. This is about 10% faster,
	and also more compact. A lot more work could/should be done
        if it is determined that this is a valuable option. 
        For example: we can compress the message size a lot by
        adding a stream ID and only sending fields that change 
        (i.e: dont send hostname and program name in every message)
 
    NOTE: setting this overrides setting ISO8601
*/

/*
define BINARY_MSG_FORMAT
*/

#if defined(NL_THREADSAFE)
#include <pthread.h>
#endif

#include <NetLogger.h>

  /* if this is set, then NetLogger will use Globus-IO along with GSI
     Security for communication with netlogd.  */
#if USE_GLOBUS_GSI
#include "globus_io.h"
#endif

 /*
  * If you really want to see all the results in as close to real-time as
  * possible, set FLUSH_RESULTS. However this will slow you down quite a bit,
  * so use this with caution to ensure to are not affecting the performance of
  * your application using NetLogger
  */
/*
#define FLUSH_RESULTS
*/

/* Timestamps synchronization with netlogd: */
/* sort of a hack workaround incase NTP is not running. This will try to
 * adjust the timestamps to be in sync with the clock of the host running netlogd
*/
/* #define CORRECT_TIMESTAMPS */

#define ASCII 48		/* start of numeric ascii characters */

/* 
 * Internal functions which encapsulate reused
 * portions of the various NetLoggerWrite's
 * --dang 10/13/99
 */
static char *printStandardFieldsToBuffer (struct timeval *tvPtr,
					  NL_Handle * lp, char *keyword);
static int writeBuffer (NL_Handle * lp, char *outbuf, int buflen);

char     *globtag = NULL;	/* global "tag" that gets added to all messages.
				   This is set using the NetLoggerSetTag call */

/* globus for status messages */
char     *nl_hname = NULL;
char     *nl_pname = NULL;
static int nl_writen (int, char *, int);
static void nl_status (char *, ...);

#ifdef CORRECT_TIMESTAMPS
static int nl_readn (register int fd,
		     register char *ptr,
		     register int nbytes);
static void Netlog_correcttime (NL_Handle * lp);
#endif

#if USE_GLOBUS_GSI
globus_io_handle_t *SecureNetLogOpen (char *, unsigned short);
#endif

#if NL_THREADSAFE
pthread_mutex_t global_mutex = PTHREAD_MUTEX_INITIALIZER;
#endif


/**
 * 
 * Lookup the host name.
 */
char * lookup_host( )
{
   char *hn;
   struct hostent *hp = NULL;
   char shn[ MAXHOSTNAMELEN ];

   hn = (char *) malloc (MAXHOSTNAMELEN);

   /* get the local host name (probably does not include the domain) */
   if (gethostname (shn, MAXHOSTNAMELEN) < 0)
   {
      perror ("gethostname");
      strcpy (shn, FALLBACK_HOSTNAME);
   }
   /* now lookup the full hostname from the name server */
   if ((hp = gethostbyname (shn)) == NULL)
   {
      perror ("gethostbyname");
      strcpy (hn, FALLBACK_HOSTNAME);
   }
   else
   {
      /* now copy out the full hostname for the hostnet record */
      /* note: use strncpy to guard against bad DNS entries */
      strncpy (hn, hp->h_name, MAXHOSTNAMELEN);	
   }
   
   return hn;
}

/**
 * Set a flag so that NetLoggerClose will NOT close the
 * log file.
 */
int
setDoNotClose( NL_Handle  *netlogger_handle_p, int new_val )
{
    int old_val;

    if ( netlogger_handle_p == NULL )
        return -1;

    old_val = netlogger_handle_p->doNotClose;
    netlogger_handle_p->doNotClose = new_val;
    return old_val;
}

/**
 * Create and init NetLogger handle from environement
 * variables and host information.
 */
NL_Handle * create_handle( char *program_name )
{
   NL_Handle *lp;

   lp = (NL_Handle *) calloc (1,sizeof (NL_Handle));

   /* process NL_ON environment var */
   /* if it is unset, act as if it is on */
   lp->enabled = NetLogGetEnv (NL_ON_ENV);
   if (lp->enabled == NL_NOTSET)
      lp->enabled = NL_ON;
   else if (lp->enabled == NL_OFF)
   {
      free(lp);
      lp = NULL;
      goto bottom;
   }

#ifdef BINARY_MSG_FORMAT
    lp->binary = 1;
#else
    lp->binary = 0;
#endif
#ifdef CORRECT_TIMESTAMPS
    lp->tshift_sec = 0;
    lp->tshift_usec = 0;
#endif /* CORRECT_TIMESTAMPS */

    if (program_name == NULL) 	/* must set to something, so use pid */
    {
       int pid = (int) getpid ();
       program_name = (char *) malloc (32);
       sprintf (program_name, "%d", pid);
    }
    
    lp->curr_buffer_size = 0;
    lp->event_buffer = NULL;
    lp->last_date = malloc (DATE_LEN);
    lp->host_prog = malloc (128);
    lp->message_buffer = (char *) calloc (NL_MAX_MESSAGE, sizeof (char));
    lp->last_tv_sec = 0;
    lp->pname = strdup (program_name);
    lp->hname = lookup_host();
    if (lp->binary)
       sprintf (lp->host_prog, "%s;%s;", lp->hname, program_name);
    else
       sprintf (lp->host_prog, " HOST=%s PROG=%s NL.EVNT=",
		lp->hname, program_name);
    lp->failed = 0;
    lp->tag = NULL;
    setDoNotClose( lp, 0 );

 bottom:
    return lp;
}

/**
 * Open NetLogger handle to a file descriptor.
 */
NL_Handle *
NetLoggerOpenFd (char *p_name, int fd, int flags)
{
    NL_Handle     *lp;
    char          *mode;     /* file mode "a" or "w" */

    /* shared handle create/init */
    lp = create_handle(p_name);
    if ( lp == NULL )
       goto bottom;

#if defined(NL_THREADSAFE)
    /* Allocate, init, and lock handle's mutex */
    lp->mtx = (pthread_mutex_t *) malloc (sizeof (pthread_mutex_t));
    pthread_mutex_init (lp->mtx, NULL);
    pthread_mutex_lock (lp->mtx);
#endif

    setDoNotClose( lp, 1 );

    /* choose memory buffering */
    if ( flags & NL_MEM )
    {
       lp->method = NL_FILE_MEM;
#ifdef VERBOSE
       nl_status ("creating message hold buffer of size %d ",
		  NL_MAX_BUFFER);
#endif
       lp->event_buffer = (char *)calloc(NL_MAX_BUFFER,1);
    }
    else
       lp->method = NL_FILE;

    /* translate append flag into a mode character string */
    mode = (flags & NL_APPEND) ? "a" : "w";

    /* open FILE* for fd */
    lp->log_fp = fdopen( fd, mode );
    lp->log_fd = fd;

 bottom:
#   if defined(NL_THREADSAFE)
      pthread_mutex_unlock (lp->mtx);
#   endif
    return lp;
}

/**
 *
 * Open NetLogger handle to destination URL
 */
NL_Handle *
NetLoggerOpen (char *p_name, char *url, int flags)
{
    const char *func = "NetLoggerOpen";
    NL_Handle     *lp;
    char          *filename = NULL; /* parsed from URL */
    char          *hostname = NULL; /* parsed from URL */
    int            port;     /* parsed from URL */
    int            method;   /* parsed from URL */
    char          *mode = NULL;     /* file mode "a" or "w" */
    char          *env;      /* environment variable */

    /* shared handle create/init */
    lp = create_handle(p_name);
    if ( lp == NULL )
       goto bottom;

#  if defined(NL_THREADSAFE)
    /* Allocate, init, and lock handle's mutex */
    lp->mtx = (pthread_mutex_t *) malloc (sizeof (pthread_mutex_t));
    pthread_mutex_init (lp->mtx, NULL);
    pthread_mutex_lock (lp->mtx);
#  endif

    /* Get environment var holding NetLogger 'destination'*/
    env = getenv (NL_DEST_ENV);

    /*
     * Set the NetLogger method.
     * If the destination environment variable is set, it overrides
     * whatever is specified in the function call.
     */
    if ((flags & NL_ENV) && env != NULL)
    {
       NetLogSetMethod (env, flags, &method, &filename, &hostname, &port);
    }
    else
    {
       NetLogSetMethod (url, flags, &method, &filename, &hostname, &port);
    }

    /* Translate append flag into a mode character string */
    if (flags & NL_APPEND)
	mode = strdup("a");
    else
	mode = strdup("w");

    /* Open connection/path to destination */
    switch (method)
    {
	case NL_FILE:
	case NL_FILE_MEM:
	   if (!(lp->log_fp = LogOpen (filename, mode)))
	   {
	      goto error_return;
	   }
	   lp->method = method;
	   lp->log_socket = -1;
	   lp->filename = strdup(filename);
	   if (method == NL_FILE_MEM)
	   {
#ifdef VERBOSE
	      nl_status ("creating message hold buffer of size %d ",
			 NL_MAX_BUFFER);
#endif
	      lp->event_buffer =
		 (char *) calloc (NL_MAX_BUFFER, sizeof (char));
	   }
	   break;
	   
	case NL_SYSLOG:
	case NL_SYSLOG_MEM:
	   lp->method = method;
	   lp->log_fp = NULL;
	   lp->log_socket = -1;
	   lp->filename = NULL;
	   SysLogOpen (p_name, LOG_LOCAL0);
	   if (method == NL_SYSLOG_MEM)
	   {
	      lp->event_buffer =
		 (char *) calloc (NL_MAX_BUFFER, sizeof (char));
	   }
	   break;
	   
	case NL_HOST:
	case NL_HOST_MEM:
	   lp->method = method;
	   lp->log_fp = NULL;
	   lp->filename = NULL;
	   lp->hostname = strdup(hostname);
#ifdef USE_GLOBUS
	   if (use_globus ())
	   {
#ifdef VERBOSE
	      nl_status ("%s: Using secure connection..",func);
#endif
	      lp->handle = SecureNetLogOpen (hostname, port);
	      if (!lp->handle)
	      {
		 nl_status ("%s: SecureNetLogOpen failed",func);
		 goto error_return;
	      }
	      lp->log_socket = -1;
	   }
	   else			/* globus defined but env not set */
	   {
#ifdef VERBOSE
	      nl_status ("%s: Not Using secure connection", func);
#endif
	      lp->log_socket = NetLogOpen (hostname, port);
	      if (lp->log_socket < 0)
	      {
		 nl_status ("%s: NetLogOpen failed", func);
		 goto error_return;
	      }
	      lp->handle = NULL;
	   }
#else /* old style -- no globus */
	   lp->log_socket = NetLogOpen (hostname, port);
	   if (lp->log_socket < 0)
	   {
	      nl_status ("%s; NetLogOpen failed", func);
	      goto error_return;
	   }
           lp->failed = 0;

#endif /* USE_GLOBUS */
	   
#ifdef CORRECT_TIMESTAMPS
	   /* set time correction values based on the time on the netlogd host */
	   Netlog_correcttime (lp);
#endif /* CORRECT_TIMESTAMPS */

	   if (method == NL_HOST_MEM)
	   {
	      lp->event_buffer =
		 (char *) calloc (NL_MAX_BUFFER, sizeof (char));
	   }

	   NetLogWriteHeader( lp->log_socket, flags );

	   break;
	   
	default:
	   goto error_return;
	   break;
    } /* switch(method) */

  bottom:
#if defined(NL_THREADSAFE)
    pthread_mutex_unlock (lp->mtx);
#endif
    if (filename) free(filename);
    if (hostname) free(hostname);
    if (mode)     free(mode);
    return lp;

  error_return:
#if defined(NL_THREADSAFE)
    pthread_mutex_unlock (lp->mtx);
#endif
    if (filename) free(filename);
    if (hostname) free(hostname);
    if (mode) free(mode);
    free (lp);
    return NULL;
}


/**
 * Write with vararg format and user-provided timestamp.
 * This gets called by several of the other write()
 * variations, including NetLoggerWrite()
 *
 * Sample NetLogger message:
 * DATE=19980401103010 HOST=dpss.lbl.gov PROG=dpss_mast \
 * NL.EVNT=ISS_MASTER_IN NL.SEC=889644821 NL.USEC=333739 \
 * DPSS.SES=1 DPSS.BID=15.98.144.0
 */
#ifdef OLD_API
int
NetLoggerGTWrite (NL_Handle * lp,
		  long sec,
		  long usec,
		  char *keyword,
		  char *fmt,
		  ...)
#else
int
NetLoggerGTWrite (NL_Handle * lp, 
		  long sec,
		  long usec, 
		  char *keyword,
		  char *consts,
		  char *fmt,
		  ...)
#endif /* OLD_API */
{
#ifndef OLD_API
    int const_len;
#endif
    int       rval = 1;
    va_list   args;
    char     *endPtr;
    struct timeval tv;

    if ( ! CheckHandle(lp) )
	return 0;

    /* check for bad time stamps */
    if (sec <= 0)
    {
       nl_status ("Keyword: %s; Bad time value: seconds = %d",
		   keyword, sec);
       return -1;
    }
    tv.tv_sec = sec;
    tv.tv_usec = usec;

    /* Lock mutex */
#if defined(NL_THREADSAFE)
    pthread_mutex_lock (lp->mtx);
#endif

    /* Standard fields for all msgs. */
    endPtr = printStandardFieldsToBuffer (&tv, lp, keyword);
    if (endPtr == NULL)
    {
       rval = -1;
       goto error_return;
    }

    /* User-defined fields. */
    va_start (args, fmt);
#ifndef OLD_API
    if ( consts != NULL && *consts != '\0' )
    {
	const_len = strlen(consts);
	memcpy( endPtr,consts, const_len);
	endPtr += const_len;
	*endPtr++ = ' ';
    }
#endif
    endPtr += vsprintf (endPtr, fmt, args);
    va_end (args);

    /* Add newline */
    *endPtr++ = '\n';
    *endPtr = '\0';
 
    /* Write to host, file, syslog, etc. */
    if (lp->failed == 0) {
       rval = writeBuffer (lp, lp->message_buffer,
		       (int) (endPtr - lp->message_buffer));
       if (rval == -1)
          lp->failed = 1;
    }
  error_return:

    /* reset string buffer */
    *lp->message_buffer = '\0';

#if defined(NL_THREADSAFE) 
    pthread_mutex_unlock (lp->mtx);
#endif
    return rval;
}

/**
 * Build and write a NetLogger message.
 * 
 * Calls:
 *   NetLoggerGTWrite
 */
#ifdef OLD_API
int
NetLoggerWrite (NL_Handle * lp,
		char *keyword,
		char *fmt,
		...)
#else
int
NetLoggerWrite(NL_Handle * lp,
	       char *keyword,
	       char *consts,
	       char *fmt,
	       ... )
#endif
{
    int       rval = 1;
    va_list   args;
    char     *endPtr;
    struct timeval tv;
#ifndef OLD_API
    int const_len;
#endif

    if ( ! CheckHandle(lp) )
    {
	return 0;
    }

    gettimeofday(&tv,NULL);

    /* Lock mutex */
#if defined(NL_THREADSAFE)
    pthread_mutex_lock (lp->mtx);
#endif

    /* Standard fields for all msgs. */
    endPtr = printStandardFieldsToBuffer (&tv, lp, keyword);
    if (endPtr == NULL)
    {
       rval = -1;
       goto error_return;
    }

    /* User-defined fields. */
    va_start (args, fmt);
#ifndef OLD_API
    if ( consts != NULL && *consts != '\0' )
    {
	const_len = strlen(consts);
	memcpy( endPtr,consts, const_len);
	endPtr += const_len;
	*endPtr++ = ' ';
    }
#endif
    endPtr += vsprintf (endPtr, fmt, args);
    va_end (args);

    /* Add newline */
    *endPtr++ = '\n';
    *endPtr = '\0';
 
    /* Write to host, file, syslog, etc. */
    if (lp->failed == 0) {
       rval = writeBuffer (lp, lp->message_buffer,
		       (int) (endPtr - lp->message_buffer));
       if (rval == -1)
          lp->failed = 1;
    }
  error_return:

    /* reset string buffer */
    *lp->message_buffer = '\0';

#if defined(NL_THREADSAFE) 
    pthread_mutex_unlock (lp->mtx);
#endif
    return rval;
}

/**
 * Write a log message if the logging level is OK.
 *
 * Calls:
 *   NetLoggerGTWrite
 */
#ifdef OLD_API
int NetLoggerWriteLvl( NL_Handle * lp,
		       int lvl,
		       char *keyword,
		       char *fmt,
		       ... )
#else
int
NetLoggerWriteLvl( NL_Handle * lp, 
		   int lvl, 
		   char *keyword, 
		   char *consts,
		   char *fmt, 
		   ... )
#endif /* OLD_API */
{
    int rval = 1;
    struct timeval tv;
    char     *endPtr;
    va_list   args;
#ifndef OLD_API
    int const_len;
#endif

    if ( ! CheckHandle(lp) || ! CheckLogLevel(lp,lvl) )
    {
      rval = 0;
    }
    else
    {
	/* get system time */
	gettimeofday (&tv, 0);

	/* Lock mutex */
#if defined(NL_THREADSAFE)
	pthread_mutex_lock (lp->mtx);
#endif
	
	/* Standard fields for all msgs. */
	endPtr = printStandardFieldsToBuffer (&tv, lp, keyword);
	if (endPtr == NULL)
	{
	    rval = -1;
	    goto error_return;
	}

#ifndef OLD_API
	if ( consts != NULL && *consts != '\0' )
	{
	    const_len = strlen(consts);
	    memcpy( endPtr,consts, const_len);
	    endPtr += const_len;
	    *endPtr++ = ' ';
	}
#endif
	va_start(args,fmt);
	endPtr += vsprintf (endPtr, fmt, args);
	va_end (args);
	
	/* Add newline */
	*endPtr++ = '\n';
	*endPtr = '\0';
	
	/* Write to host, file, syslog, etc. */
	if (lp->failed == 0) {
	    rval = writeBuffer (lp, lp->message_buffer,
				(int) (endPtr - lp->message_buffer));
	    if (rval == -1)
		lp->failed = 1;
	}

      error_return:	
	/* reset string buffer */
	*lp->message_buffer = '\0';
	
#if defined(NL_THREADSAFE) 
	pthread_mutex_unlock (lp->mtx);
#endif
    }
    return rval;
}

/**
 * Write a log message if the logging level is OK.
 *
 * Calls:
 *   NetLoggerGTWrite
 */
#ifdef OLD_API
int
NetLoggerGTWriteLvl( NL_Handle * lp, 
		     long sec, 
		     long usec,
		     int lvl, 
		     char *keyword, 
		     char *fmt, 
		     ... )
#else
int
NetLoggerGTWriteLvl( NL_Handle * lp, 
		     long sec,
		     long usec,
		     int lvl, 
		     char *keyword, 
		     char *consts,
		     char *fmt, 
		     ... )
#endif /* OLD_API */
{
    int rval = 1;
    va_list   args;

    if ( !CheckHandle(lp) || !CheckLogLevel(lp,lvl) )
    {
      rval = 0;
    }
    else
    {
	va_start(args,fmt);
#ifdef OLD_API
	rval = NetLoggerGTWrite(lp,sec,usec,keyword,fmt,args);
#else
	rval = NetLoggerGTWrite(lp,sec, usec,keyword,consts,fmt,args);
#endif /* OLD_API */
	va_end(args);
    }
   return rval;
}

/**
 * Non-varargs version of write call with user timestamp.
 *
 * This is called by the other WriteString variations.
 *
 * Author: Martin Reddy, SRI
 *
 * Note:
 *   These where first added for OS's without varargs (ie: old
 *   versions of IRIX), but it is in fact about 15-20% faster
 *   than the standard NetLoggerWrite call, but not as flexible
 * 
 * Calls:
 *   NetLoggerGTWrite
 *
 * Return:
 *   1 - OK
 *   0 - Did nothing
 *  -1 - Failure while writing
 *
 */
#ifdef OLD_API
int
NetLoggerGTWriteString (NL_Handle * lp,
			long sec, long usec,
			char *keyword, char *data)
#else
int
NetLoggerGTWriteString(NL_Handle * lp, 
		       long sec, 
		       long usec,			       
		       char *keyword, 
		       char *const_data,
		       char *data)
    
#endif /* OLD_API */
{
    int       rval = 1;
    char     *endPtr = NULL;
    struct timeval tv;

    if ( ! CheckHandle(lp) )
    {
	return 0;
    }

    tv.tv_sec = sec;
    tv.tv_usec = usec;

#if defined(NL_THREADSAFE)
    pthread_mutex_lock (lp->mtx);
#endif

    /* Standard header fields */
    endPtr = printStandardFieldsToBuffer (&tv, lp, keyword);
    if (endPtr == NULL)
      {
	  rval = -1;
	  goto error_return;
      }

    /* add User-defined fields */
#ifndef OLD_API
    if ( const_data != NULL && *const_data != '\0' )
    {
	strcat(lp->message_buffer, const_data );
	strcat(lp->message_buffer," ");
    }
#endif /* OLD_API */
    strcat(lp->message_buffer, data);
    strcat (lp->message_buffer, "\n");

    /* Write to host, file, syslog, etc. */
    if (lp->failed == 0) {
       rval = writeBuffer (lp, lp->message_buffer,
			   strlen (lp->message_buffer));
       if (rval == -1)
          lp->failed = -1;
    }

  error_return:

    /* reset string buffer */
    *lp->message_buffer = '\0';

#if defined(NL_THREADSAFE)
    pthread_mutex_unlock (lp->mtx);
#endif

    return rval;
}


/**
 * Non var-args version with automatic timestamp.
 *
 * Calls: 
 *   NetLoggerGTWriteString()
 */

#ifdef OLD_API
int
NetLoggerWriteString (NL_Handle * lp,
		      char *keyword,
		      char *data)
#else
int
NetLoggerWriteString (NL_Handle * lp,
		      char *keyword, 
		      char *const_data,
		      char *data)
#endif /* OLD_API */
{
    int rval = 1;
    struct timeval tv;

    if ( ! CheckHandle(lp) )
	return 0;

    /* get system time */
    gettimeofday (&tv, 0);

#ifdef OLD_API
    rval = NetLoggerGTWriteString( lp, tv.tv_sec, tv.tv_usec, 
				   keyword, data );
#else 
    rval = NetLoggerGTWriteString( lp, tv.tv_sec, tv.tv_usec,
				   keyword, const_data, 
				   data );
#endif /* OLD_API */

    return rval;
}

/**
 * Non var-args version with user timestamp that
 * only writes the string if it is under the handle's 
 * logging level.
 *
 * Calls: 
 *   NetLoggerGTWriteString()
 */
#ifdef OLD_API
int
NetLoggerGTWriteStringLvl (NL_Handle * lp,
			   int lvl,
			   long sec, 
			   long usec, 
			   char *keyword, 
			   char *data)
#else
int
NetLoggerGTWriteStringLvl(NL_Handle * lp,
			  int lvl,
			  long sec,
			  long usec,
			  char *keyword,
			  char *const_data,
			  char *var_data)
#endif /* OLD_API */
{
    int rval = 1;
    if ( !CheckHandle(lp) || !CheckLogLevel(lp,lvl) )
    {
	rval = 0;
    }
    else
    {
#ifdef OLD_API
	rval=NetLoggerGTWriteString( lp, sec, usec, keyword, data );
#else
	rval=NetLoggerGTWriteString( lp, sec, usec, keyword, 
				     const_data, var_data );

#endif /* OLD_API */
    }
    return rval;
}

/**
 * Non var-args version with automatic timestmap
 * that only writes the string if it's under the logging level.
 *
 * Calls: 
 *   NetLoggerGTWriteString()
 */
#ifdef OLD_API
int
NetLoggerWriteStringLvl (NL_Handle * lp,
			   int lvl,
			   char *keyword, 
			   char *data)
#else
int
NetLoggerWriteStringLvl (NL_Handle * lp,
			 int lvl,
			 char *keyword, 
			 char *const_data,
			 char *data)
#endif /* OLD_API */
{
    int rval = 1;
    struct timeval tv;

    if ( !CheckHandle(lp) || !CheckLogLevel(lp,lvl) )
    {
	rval = 0;
    }
    else
    {
	/* get system time */
	gettimeofday (&tv, 0);
#ifdef OLD_API
	rval = NetLoggerGTWriteString( lp, 
				       tv.tv_sec, tv.tv_usec,
				       keyword, 
				       data);
#else
	rval = NetLoggerGTWriteString( lp, 
				       tv.tv_sec, tv.tv_usec,
				       keyword, 
				       const_data,
				       data);
#endif /* OLD_API */
    }
    return rval;
}


/**
 * Flush from memory to the specified destination.
 */
int
NetLoggerFlush (NL_Handle * lp)
{
    int       rval = 1;
    int       orginal_method;

    if (lp == NULL || lp->enabled == NL_OFF)
	return 0;

#ifdef VERBOSE
    nl_status ("Flushing events from NetLogger Buffer.");
#endif

    rval = 1;

    /* 
     * Translate to non-MEM method so that
     * writeBuffer won't call Flush in endless recursion.
     */
    orginal_method = lp->method;
    switch (orginal_method)
      {
      case NL_FILE_MEM:
	  lp->method = NL_FILE;
	  break;
      case NL_SYSLOG_MEM:
	  lp->method = NL_SYSLOG;
	  break;
      case NL_HOST_MEM:
	  lp->method = NL_HOST;
	  break;
      default:
	  rval = -1;
	  break;
      }

    /* Write stored buffer */
    if (lp->failed == 0) {
       rval = writeBuffer (lp, lp->event_buffer, lp->curr_buffer_size);
       if (rval == -1)
          lp->failed = 1;
    }

    lp->method = orginal_method;

    if (lp->method == NL_FILE_MEM)
	fflush (lp->log_fp);
    lp->curr_buffer_size = 0;

    return rval;
}

/**
 * Set the level that will be used in the
 * level-aware NetLoggerWrite and NetLoggerGTWrite calls.
 */
void
NetLoggerSetLevel (NL_Handle * lp, int log_level)
{
    if ( lp == NULL ) return;
    lp->loglvl = log_level;
}


/**
 * See: setDoNotClose()
 */
int doNotClose( NL_Handle * netlogger_handle_p )
{
   return ( netlogger_handle_p->doNotClose == 1 );
}

/**
 * Close the handle
 */
int
NetLoggerClose (NL_Handle * lp)
{
  int       rval = 1;

  if ( lp == NULL )
      return 0;

  if (lp->enabled != NL_OFF)
  {
    /* Flush */
    if (lp->curr_buffer_size > 0)
      NetLoggerFlush (lp);
    
    /* Close output */
    switch (lp->method)
    {
    case NL_FILE:
    case NL_FILE_MEM:
      if ( ! doNotClose(lp) )
        LogClose (lp->log_fp);
      break;
    case NL_SYSLOG:
    case NL_SYSLOG_MEM:
      SysLogClose ();
      break;
    case NL_HOST:
    case NL_HOST_MEM:
# ifdef USE_GLOBUS
      if (lp->handle != NULL)
      {
	SecureNetLogClose (&(lp->handle));
	break;
      }
#endif
      NetLogClose (lp->log_socket);
      break;
    default:
      rval = -1;
      break;
    }
  }

  /* Free the rest of the struct */
  if ( lp->filename ) free(lp->filename);
  if ( lp->event_buffer ) free(lp->event_buffer);
  if ( lp->pname ) free(lp->pname);
  if ( lp->hname ) free(lp->hname);
  if ( lp->message_buffer ) free(lp->message_buffer);
  if ( lp->last_date ) free(lp->last_date);
  if ( lp->host_prog ) free(lp->host_prog);
#if defined(NL_THREADSAFE)
  if ( lp->mtx ) free(lp->mtx);
#endif
#ifdef USE_GLOBUS
  /* XXX: Might need to call a globus free() first ?! */
  if ( lp->globus_io_handle_t ) free(lp->globus_io_handle_t);
#endif
  free(lp);

  return rval;
}

/**
 *
 * Set tag that we will use on repeated
 * writes. Overwrite if tag exists.
 *
 * This tag will be appended to ALL NetLogger writes.
 * See NetLoggerSetTag() for the per-handle version.
 *
 * Return:
 *   0  - OK
 *   -1 - Fatal error.
 */

int
NetLoggerSetGlobalTag (char *tag)
{
    int       ret = 0;
#if defined(NL_THREADSAFE)
    pthread_mutex_lock (&global_mutex);
#endif
    if (tag)
    {
       if (globtag)
       {
	  free (globtag);
       }
       globtag = (char *) calloc (sizeof (char), strlen (tag) + 1);
       if (!globtag)
       {
	  perror ("calloc");
	  ret = -1;
       }
       else
	  strcpy (globtag, tag);
    }
#if defined(NL_THREADSAFE)
    pthread_mutex_unlock (&global_mutex);
#endif
    return ret;
}

/**
 *
 * Set a string, or 'tag', which will be appended to all 
 * NetLoggerWrite()'s to this handle.
 *
 * Params:
 *   h_p - Handle
 *   tag - String in ULM format, e.g., "Nane1=Value1 Name2=Value2"
 *         The string is copied, so its memory is the responsibility
 *         of the caller.
 *
 * Return:
 *   1 - OK
 *   0 - Error
 */
int
NetLoggerSetTag(NL_Handle  *h_p,
		char       *tag )
{
    if ( h_p == NULL )
	return 0;
    if ( h_p->tag != NULL )
	free( h_p->tag );
    h_p->tag = strdup(tag);
    return 1;
}

/***********************************************************
 * INTERNAL FUNCTIONS
 ************************************************************
 */


/**
 * Print DATE, HOST, PROG, to the provided buffer.
 *
 * In     : tvPtr   - Time for DATE (r)
 *          lp      - NetLogger handle, state info (r)
 *          keyword - NetLogger event (r)
 * Return : Pointer to position to start printing user-defined
 *          fields, or NULL on an error.
 *
 * NOTE: lots of performance tricks in this routine. It would
 * be much easier to just use sprintf, but that is MUCH slower than
 * using strcat.
 */
char     *
printStandardFieldsToBuffer (struct timeval *tvPtr,
			     NL_Handle * lp, char *keyword)
{
    struct tm *tm = NULL;
    char      date_buffer[DATE_LEN];
    int       i, usec;
    char      n[7];
#ifdef CORRECT_TIMESTAMPS
    if (lp->tshift_usec != 0)
      {
	  tvPtr->tv_usec += lp->tshift_usec;
	  if (tvPtr->tv_usec >= 1000000)
	    {
		tvPtr->tv_usec -= 1000000;
		tvPtr->tv_sec++;
	    }
	  else if (tvPtr->tv_usec < 0)
	    {
		tvPtr->tv_usec += 1000000;
		tvPtr->tv_sec--;
	    }
      }
    tvPtr->tv_sec += lp->tshift_sec;
#endif
    if (lp->last_tv_sec != tvPtr->tv_sec)	/* only do this if seconds 
						   field has changed, otherwise
						   use the previous result */
      {
	  /* Build date string from the arg "sec" using "Univeral Coord. time" */
	  /* Note: gmtime and strftime are expensive, so do this
	     as little as possible.
	   */
	  if ((tm = gmtime ((time_t *) & tvPtr->tv_sec)) == NULL)
	    {
		perror ("localtime error");
		return NULL;
	    }

#ifdef ISO8601
	  /* Output date in this form: YYYY-MM-DDTHH:MM:SS  */
	  if (strftime (date_buffer, DATE_LEN, "%Y-%m-%dT%H:%M:%S.", tm) == 0)
	    {
		perror ("strftime error");
		return NULL;
	    }
#else
	  /* Output date in this form: YYYYMMDDHHMMSS  */
	  if (strftime (date_buffer, DATE_LEN, "%Y%m%d%H%M%S.", tm) == 0)
	    {
		perror ("strftime error");
		return NULL;
	    }
#endif
      }

    strcat (lp->message_buffer, "DATE=");
    if (lp->last_tv_sec == tvPtr->tv_sec)
      {
	  strcat (lp->message_buffer, lp->last_date);
      }
    else
      {
	  strcat (lp->message_buffer, date_buffer);
	  strcpy (lp->last_date, date_buffer);
      }
    /* sprintf is SLOW! New version speeds up NetLoggerWrite by about 15%! */
#ifdef OLD_VERSION
    sprintf (date_buffer_usec, "%06d", (int) tvPtr->tv_usec);
    strcat (lp->message_buffer, date_buffer_usec);
#else
    /* NOTE: hardcoded for 6 digits of micro-seconds at the moment.
       would need to change this for nano seconds, etc. */
    usec = (int) tvPtr->tv_usec;
    n[6] = '\0';		/* end-of-string marker */
    for (i = 0; i < 6; i++)	/* works for micro-sec res only */
      {
	  n[5 - i] = (char) (ASCII + (usec % 10));
	  usec /= 10;
      }
    strcat (lp->message_buffer, n);
#ifdef ISO8601
    /* indicates UTC timezone: + accuracy + precision */
    strcat (lp->message_buffer, "Zp.000001/.001");
#endif
#endif /* old version */
    strcat (lp->message_buffer, lp->host_prog);	/* contains HOST=h PROG=p NL.EVNT= */
    strcat (lp->message_buffer, keyword);
    strcat (lp->message_buffer, " ");
    /* prepend (optional) per-handle tag */
    if ( lp->tag != NULL )
    {
	strcat(lp->message_buffer, lp->tag );
	strcat(lp->message_buffer," ");
    }
    /* prepend (optional) global tag */
    if (globtag)
    {
	strcat (lp->message_buffer, globtag);
	strcat (lp->message_buffer, " ");
    }
    lp->last_tv_sec = tvPtr->tv_sec;
    /* debug 
       fprintf (stderr, "built string: %s \n", lp->message_buffer);
     */
    /* return pointer to end of message */
    return (lp->message_buffer + strlen (lp->message_buffer));
}

/**
 * Write a buffer to the proper NetLogger recipient.
 *
 * In: lp     - Handle to NetLogger state (r)
 *     outbuf - Address of buffer to write (rw)
 *     buflen - Length of buffer (save a strlen).
 *
 * Out: Success = 1; failure = -1
 *
 * Effects: If the user is using one of the memory buffering
 *          methods, we copy the buffer's address and set it
 *          to NULL here, to be freed when it is flushed.
 *          The caller can free the buffer, with no knowledge
 *          of our nefarious activities.
 */

static int
writeBuffer (NL_Handle * lp, char *outbuf, int buflen)
{
    char     *func = "writeBuffer";
    int       rval = 1;
    int       n;

    switch (lp->method)
      {
      case NL_FILE:
	  if ((fwrite (outbuf, buflen, sizeof (char), lp->log_fp)) <= 0)
	      nl_status ("%s: NL_FILE: Error sending Netlogger message to file.",
			 func);
#ifdef FLUSH_RESULTS
	  fflush (lp->log_fp);
#endif
	  break;
      case NL_SYSLOG:
	  syslog (LOG_INFO, "%s", outbuf);
	  break;
      case NL_HOST:
#if USE_GLOBUS
	  if (lp->handle != NULL) {
	     if (SecureNetLogSend (lp->handle, outbuf))
	        break;
	     else		/* error */
		return -1;
	  }
#endif

#ifdef SEND_MESSAGE_SIZE  /* this slows down netlogd: better to let server
			     figure out the size of the message by looking for newlines -blt */
/* also breaks C netlogd!!! -dang */
	  /* write size, then write message */
	  size = htonl (buflen);
	  n = nl_writen (lp->log_socket, (char *) &size, sizeof (int));
	  if (n != sizeof (int))
	  {
		nl_status ("%s: Error sending Netlogger message to socket.",
			   func); return -1;
	  }
#endif
          if (lp->failed == 0) {
	     n = nl_writen (lp->log_socket, outbuf, buflen);
	     if (n != buflen) {
		nl_status ("%s: Error sending Netlogger message to socket.",
			   func); return -1;
	     }
          }
	  break;
      case NL_FILE_MEM:
      case NL_SYSLOG_MEM:
      case NL_HOST_MEM:
/* debug 
          nl_status("writebuffer: buflen = %d, lp->curr_buffer_size %d, MAX = %d",
		buflen, lp->curr_buffer_size, NL_MAX_BUFFER);
          nl_status("writebuffer: outbuf = %s ", outbuf);
*/
	  if ((lp->curr_buffer_size + buflen) >= NL_MAX_BUFFER)
	    {
		NetLoggerFlush (lp);
	    }

	  /* copy to buffer for NetLoggerFlush call later */
	  memcpy ((lp->event_buffer + lp->curr_buffer_size), outbuf, buflen);
	  lp->curr_buffer_size += buflen;
	  break;
      default:
	  rval = -1;
	  break;
      }

    return rval;
}

FILE     *
LogOpen (char *logfile_name, char *mode)
{
    char     *func = "LogOpen()";
    FILE     *fp = NULL;
    if (logfile_name == NULL)
	return NULL;
    if (mode == NULL)
	mode = "w";
    if (!strcmp (mode, "a"))
      {
#ifdef VERBOSE
	  nl_status ("NetLogger: appending data to file %s", logfile_name);
#endif
	  if ((fp = fopen (logfile_name, "a")) == NULL)
{
		nl_status ("%s: Error opening log file %s", func,
			   logfile_name); return NULL;
	    }
      }
    else
      {
	  if ((fp = fopen (logfile_name, "w")) == NULL)
	    {
		nl_status ("%s: Error opening log file %s", func,
			   logfile_name); return NULL;
	    }
      }
      return fp;
}

/**
 * Open to syslog.
 */
int
SysLogOpen (char *prog_name, int syslog_facility)
{
    /* syslog_facility can be LOG_USER, LOG_LOCAL0, etc. See 'man syslog' */

    openlog (prog_name, LOG_PID, syslog_facility);
    /* this call has no error return vals */
    return 1;
}

/**
 * Adjust time to roughly that of the server.
 *
 * Author: Jean Tedesco, ANL
 * NOTE: might also want a version of this call that just 
 * warns the user if the
 *  clocks are out of sync  -blt  
 */

#ifdef CORRECT_TIMESTAMPS
void
Netlog_correcttime (NL_Handle * lp)
{
    char     *func = "Netlog_correcttime";
    struct Timebuf
    {
	int       size;
	char      buf[44];	/* let transmit the value in ascii, life is
				 * easier */
	/* 44 char is more than long enought to store 2 long integers        */
    }
    timebuf;
    struct timeval tvbuff[3];
    long      total_shift = 0;
    long      trans;
    long      tshift;		/* tshift can not be bigger than 2^33-1 =
				 * 8589 sec (2h23min) */
    int       i, n;
    ldiv_t    shift;
#define NBTRY 100
    for (i = 0; i < NBTRY; i++)
      {
	  /* write a size of -1 to trigger the time correction function */
	  timebuf.size = htonl (-1);
	  /* get the time */
	  gettimeofday (&tvbuff[0], 0);	/* tv.tv_sec, tv.tv_usec */
#if USE_GLOBUS
	  if (lp->log_socket == -1)
	    {
		if (SecureNetLogSend (lp->handle, timebuf))
		  {
		      n = 0;
		  }
		else
		  {
		      n = sizeof (timebuf);
		  }

	    }
	  else
	    {
		n =
		    nl_writen (lp->log_socket, (char *) &timebuf,
			       sizeof (timebuf));}
#else
	  n = nl_writen (lp->log_socket, (char *) &timebuf, sizeof (timebuf));
#endif
	  if (n != sizeof (timebuf))
	    {
		nl_status
		    ("NetLog: Error sending Netlogger time synch. message");
		lp->tshift_sec = 0;
		lp->tshift_usec = 0;
		return;
	    }

#if USE_GLOBUS
	  if (lp->log_socket == -1)
	    {
		if (SecureNetLogRead (lp->handle, timebuf.buf, 44))
		  {		/* error */
		      n = 0;
		  }
		else
		  {
		      n = 44;
		  }

	    }
	  else
	    {
		n = nl_readn (lp->log_socket, timebuf.buf, 44);
	    }
#else
	  n = nl_readn (lp->log_socket, timebuf.buf, 44);
#endif
	  if (n != 44)
	    {
		nl_status
		    ("NetLog: Error reading Netlogger time synch. message");
		lp->tshift_sec = 0;
		lp->tshift_usec = 0;
		return;
	    }
	  do
	    {
		char      pbuf[45];
		memcpy (pbuf, timebuf.buf, 44);
		pbuf[44] = '\0';
	    }
	  while (0);
	  /* get the time */
	  gettimeofday (&tvbuff[2], 0);	/* tv.tv_sec, tv.tv_usec  */
	  /*
	   * Extract timestamp from a string.
	   */
	  if (sizeof (time_t) == sizeof (long))
	    {
		sscanf (timebuf.buf, "%ld %ld",
			&(tvbuff[1].tv_sec), &(tvbuff[1].tv_usec));
	    }
	  else if (sizeof (time_t) == sizeof (int))
	    {
		sscanf (timebuf.buf, "%d %ld",
			&(tvbuff[1].tv_sec), &(tvbuff[1].tv_usec));
	    }
	  else
	    {
		nl_status ("%s: Cannot format time synch msg: "
			   "time_t is not an integer or long integer", func);
		exit (1);
	    }

	  /* calculate aproximation of the transmission time             */
	  /* lets hope the transmission time is equal in both directions */
	  trans = ((tvbuff[2].tv_sec - tvbuff[0].tv_sec) * 1000000 +
		   (tvbuff[2].tv_usec - tvbuff[0].tv_usec)) / 2;
	  tshift = ((tvbuff[1].tv_sec - tvbuff[0].tv_sec) * 1000000 +
		    (tvbuff[1].tv_usec - tvbuff[0].tv_usec)) - trans;
	  /* I do average here to not overflow total_shift */
	  total_shift += tshift / NBTRY;
      }
#ifdef DEBUG_TIME
    printf ("me: %ld sec\n", tvbuff[0].tv_sec);
    printf ("me: %ld usec\n", tvbuff[0].tv_usec);
    printf ("him: %ld sec\n", tvbuff[1].tv_sec);
    printf ("him: %ld usec\n", tvbuff[1].tv_usec);
    printf ("mea: %ld sec\n", tvbuff[2].tv_sec);
    printf ("mea: %ld usec\n", tvbuff[2].tv_usec);
    printf ("trans: %ld usec\n", trans);
    printf ("Shift: %ld usec\n", total_shift);
#endif
    shift = ldiv (total_shift, 1000000L);
    lp->tshift_sec = shift.quot;
    lp->tshift_usec = shift.rem;
#ifdef DEBUG_TIME
    printf ("Shift: %ld sec\n", lp->tshift_sec);
    printf ("Shift: %ld usec\n", lp->tshift_usec);
#endif
}				/* correcttime() */

#endif /* CORRECT_TIMESTAMPS */

/**
 * Open to a remote server on a host:port
 */
int
NetLogOpen (char *hostname, int port)
{
    struct hostent *hp = NULL;
    struct sockaddr_in sin;
    int       log_socket;

    if (hostname == NULL)
      {
	  hostname = getenv ("NL_DEST_ENV");
	  if (hostname == NULL)
	  {
	     nl_status
		("Error: no hostname, and environment var %s not set!",
		 NL_DEST_ENV); return -1;
	  }
#ifdef VERBOSE
	  nl_status
	     ("Logging to host specified by environment variable %s.",
	      NL_DEST_ENV);
#endif
      }
#ifdef VERBOSE
    nl_status
       ("Opening connect to netlog message server host %s, port %d",
	hostname, port);
#endif
    if ((hp = gethostbyname (hostname)) == 0)
    {
       perror ("gethostbyname");
       return -1;
    }
    memset ((void *) &sin, 0, sizeof (sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = ((struct in_addr *) (hp->h_addr))->s_addr;
    sin.sin_port = htons (port);
    if ((log_socket = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
       perror ("socket");
       return -1;
    }
#ifdef FLUSH_RESULTS
/*
    if (setsockopt
	(log_socket, IPPROTO_TCP, TCP_NODELAY, (char *) &i,
	 (int) sizeof (int)) < 0)
	perror ("setsockopt:tcp_nodelay");
*/
#endif


    if (connect (log_socket, (struct sockaddr *) &sin, sizeof (sin)) == -1)
    {
       perror ("connect");
       close (log_socket);
       log_socket = -1;
       return -1;
    }
    
    return log_socket;
}

/**
 * Write header to stream, for now just hardcode to ULM
 */
void
NetLogWriteHeader( int sockfd, int flags )
{
   static const char ulm_hdr[] = "ULM\0    ";
   write( sockfd, ulm_hdr, 8 );
}

/**
 * Close the log file.
 */
int
LogClose (FILE *fp)
{
    fclose (fp);
    return 1;
}

/**
 * Close syslog
 */
int
SysLogClose (void)
{
    closelog ();
    return 1;
}

/**
 * Close the socket.
 */
int
NetLogClose (int log_socket)
{
    close (log_socket);
    return 1;
}


/*******************************************************************/
/*  NOT DONE: This routine not finished, but contain most of the */
/*  required components to make it easy to finish it if someone */
/*  finds it usefull */

float
ulm_elapsed_time (char *t1, char *t2)
{
    struct timeval tv1, tv2;
    struct tm *tm = NULL;
    float     etime;
    /* test code to see how long it take to convert date back again */
    /* results: it take about 2 ms: which I think is too long! */
    /* convert time t1 */
    memset (tm, 0, sizeof (struct tm));
    sscanf (t1, "%4d%2d%2d%2d%2d%2d", &(tm->tm_year),
	    &(tm->tm_mon), &(tm->tm_mday), &(tm->tm_hour),
	    &(tm->tm_min), &(tm->tm_sec));
    tm->tm_year = tm->tm_year - 1900;
    tv1.tv_sec = (long) mktime (tm);
    /* convert time t2 */
    memset (tm, 0, sizeof (struct tm));
    sscanf (t2, "%4d%2d%2d%2d%2d%2d", &(tm->tm_year),
	    &(tm->tm_mon), &(tm->tm_mday), &(tm->tm_hour),
	    &(tm->tm_min), &(tm->tm_sec));
    tm->tm_year = tm->tm_year - 1900;
    tv2.tv_sec = (long) mktime (tm);
    etime =
	((tv2.tv_sec * 1000000) + tv2.tv_usec) -
	((tv1.tv_sec * 1000000) + tv1.tv_usec); return etime;
}


/*******************************************************************/
/********* routines orginally from DPSS utils.c  */
/***********************************************************/
static void
nl_status (char *fmt, ...)	/* use this one with X-windows programs to
				 * avoid name conflict */
{
    va_list   args;
#ifndef  NETLOGGER_SILENT
    if (nl_pname != NULL && nl_hname != NULL)
	fprintf (stderr, "%s@%s: ", nl_pname, nl_hname);
    else if (nl_pname != NULL)
	fprintf (stderr, "%s: ", nl_pname);
    va_start (args, fmt);
    vfprintf (stderr, fmt, args);
    va_end (args);
    fputc ('\n', stderr);
    fflush (stderr);
#endif
    return;
}

/**************************************************************/

static int
nl_writen (fd, ptr, nbytes)	/* use for writing n characters to a socket */
     register int fd;
     register char *ptr;
     register int nbytes;
{
    int       nleft, nwritten;
    nleft = nbytes;
    signal(SIGPIPE, SIG_IGN);
    while (nleft > 0)
      {
	  nwritten = write (fd, ptr, nleft);
	  if (nwritten < 0 && errno == 32) {
	     fprintf (stderr, "nl_writen: Write to socket failed. logging suspended\n");
             close(fd);
	     return (nwritten);	/* error */
          }
	  nleft -= nwritten;
	  ptr += nwritten;
      }
    return (nbytes - nleft);
}

/**************************************************************/
int
nl_readn (register int fd,
	  register char *ptr,
	  register int nbytes)
{
    int       nleft, nread;
    nleft = nbytes;
    while (nleft > 0)
      {
	  nread = read (fd, ptr, nleft);
	  if (nread < 0)
	      return (nread);	/* error */
	  nleft -= nread;
	  ptr += nread;
      }
    return (nbytes - nleft);
}


#ifdef USE_GLOBUS_GSI
/**************************************************************/
int
use_globus ()
{
    if (getenv ("USE_GLOBUS_SECURITY") == NULL)
	return 0;
    return 1;
}

/**************************************************************/
int
error_stuff (globus_result_t result, char *msg)
{
    globus_object_t *err = GLOBUS_NULL;
    char     *errstring = GLOBUS_NULL;
    if (result != GLOBUS_SUCCESS)
      {
	  err = globus_error_get (result);
	  if (!globus_io_eof (err))
	    {
		errstring = globus_object_printable_to_string (err);
		globus_libc_printf ("%s failed: %s\n", msg, errstring);
	    }
	  return 1;
      }
    else
      {
	  globus_libc_printf ("%s ok\n", msg);
      }
    return 0;
}

/**************************************************************/
globus_io_handle_t *
SecureNetLogOpen (char *host, unsigned short port)
{
    globus_io_attr_t attr;
    globus_io_handle_t *handle;
    globus_io_secure_authorization_data_t auth_data;
    globus_result_t result;
    handle = (globus_io_handle_t *) calloc (1, sizeof (globus_io_handle_t));
    /* active modules */
    globus_module_activate (GLOBUS_COMMON_MODULE);
    globus_module_activate (GLOBUS_IO_MODULE);
    /* setup auth */
    globus_io_tcpattr_init (&attr);
    globus_io_attr_set_secure_authentication_mode (&attr,
						   GLOBUS_IO_SECURE_AUTHENTICATION_MODE_GSSAPI,
						   GSS_C_NO_CREDENTIAL);
    globus_io_secure_authorization_data_initialize (&auth_data);
    /* what does this get set to? */
    /* Put the string somewhere more sensible: logging.h. --dang 10/13/99 */
    globus_io_secure_authorization_data_set_identity (&auth_data,
						      JRLEE_LBL_AUTH);
    globus_io_attr_set_secure_authorization_mode (&attr,
						  GLOBUS_IO_SECURE_AUTHORIZATION_MODE_IDENTITY,
						  &auth_data);
    result = globus_io_tcp_connect (host,
				    (unsigned short) port, &attr, handle);
    if (error_stuff (result, "connect"))
	return NULL;
    globus_io_tcpattr_destroy (&attr);
    return (handle);
}

/**************************************************************/
/* return 1 on good send, 0 on error */
int
SecureNetLogSend (globus_io_handle_t * handle, char *buf)
{
    globus_result_t result;
    globus_size_t nbytes;
    globus_size_t len;
    len = strlen (buf);
    if (len <= 0)
	return 0;
    result = globus_io_write (handle, (unsigned char *) buf, len, &nbytes);
    if (error_stuff (result, "SecureNetLogSend"))
      {
	  return 0;
      }

    if (nbytes != len)
      {
	  fprintf (stderr, "error with output %d instead of %d\n",
		   nbytes, len); return 0;
      }

    return 1;
}

/**************************************************************/
/* return 1 on good send, 0 on error */
int
SecureNetLogRead (globus_io_handle_t * handle, char *buf, int size)
{
    globus_result_t result;
    globus_size_t nbytes;
    result =
	globus_io_read (handle, (unsigned char *) buf, size, size, &nbytes);
    if (error_stuff (result, "SecureNetLogRead"))
      {
	  return 0;
      }

    if (nbytes != size)
      {
	  fprintf (stderr, "error with output %d instead of %d\n",
		   nbytes, size); return 0;
      }

    return 1;
}

/**************************************************************/
int
SecureNetLogClose (globus_io_handle_t * handle)
{
    globus_io_close (handle);
    globus_module_deactivate (GLOBUS_IO_MODULE);
    globus_module_deactivate (GLOBUS_COMMON_MODULE);
}

#endif /* USE_GLOBUS */

/**************************************************************/
/* Return: NL_ON, NL_OFF, or NL_NOTSET. 
*/
int
NetLogGetEnv (char *varname)
{
    char     *v;
    int       netlog_on;
    v = getenv (varname);
    if (v == NULL)
	netlog_on = NL_NOTSET;
    else
      {
	  int       i;
	  char     *onvals[] = {
	      "1", "on", "true", "yes"
	  };
	  for (i = 0, netlog_on = NL_OFF; netlog_on == NL_OFF && i < 4; i++)
	    {
		if (0 == strcasecmp (v, onvals[i]))
		    netlog_on = NL_ON;
	    }
      }

    return netlog_on;
}

/**************************************************************/
void
NetLogSetMethod (char *s,	/* Value of environment variable */
		 int flags,
		 int *method_p,
		 char **filename_p, char **hostname_p, int *port_p)
/* 
 * Set the NetLogger method and its args (hostname or filename)
 * from the value of the environment variable.
 */
{
    if (!s)
      {
	  nl_status ("NetLogger URL is NULL");
	  *method_p = 0; // static_cast<int *>(NULL);
	  return;
      }
    if (!strncmp (s, "file://", 7))
      {
	  if (flags & NL_MEM)
	      *method_p = NL_FILE_MEM;
	  else
	      *method_p = NL_FILE;
	  *filename_p = strdup (s + 7);
	  nl_status ("Logging to file %s", *filename_p);
      }
    else if (!strncmp (s, "x-netlog://", 11))
      {
	  int       n;
	  char     *p = strchr (s + 11, ':');
	  if (p && (n = atoi (p + 1)) > 0)
	    {
		*p = '\0';
		*port_p = n;
	    }
	  else			/*if (*port_p <= 0) */
	    {
		*port_p = DPSS_NETLOGGER_PORT;
	    }
	  if (flags & NL_MEM)
	      *method_p = NL_HOST_MEM;
	  else
	      *method_p = NL_HOST;
	  *hostname_p = strdup (s + 11);
	  nl_status ("Logging to host %s, port %d", *hostname_p, *port_p);
      }
    else if (!strncmp (s, "x-syslog://", 11))
      {
	  *method_p = NL_SYSLOG;
	  nl_status ("Logging to syslog");
      }
    else
      {
	  if (flags & NL_MEM)
	      *method_p = NL_FILE_MEM;
	  else
	      *method_p = NL_FILE;
	  *filename_p = strdup (s);
	  nl_status ("Logging to file %s", *filename_p);
      }
}

#ifdef NOT_NEEDED
/***********************************************************/
 /*
  * build and write a NetLogger message: but Date is just
  *  the results of gettimeofday(), and not in YYYYMMDDHHMM format
  *
  * it was hoped that this might be a lot faster than NetLoggerWrite,
  *   but its not really.
  */

int
NetLoggerFastWrite (NL_Handle * lp, char *keyword, char *fmt, ...)
{ 
    char     *func = "NetLoggerWrite";
    int       rval; 
    va_list   args;
    struct timeval tv;
    char      date_buffer[DATE_LEN];
    char      date_buffer_usec[DATE_LEN];
    char     *endPtr;
    int       i, usec, first;
    char      n[7];

    if (lp == NULL || lp->enabled == NL_OFF)
	return 0;

    /* Lock mutex */
#if defined(NL_THREADSAFE)
    pthread_mutex_lock (lp->mtx);
#endif

    /* get system time */
    gettimeofday (&tv, 0);

    if (lp->last_tv_sec != tv.tv_sec)
	sprintf (lp->last_date, "%d", (int) tv.tv_sec);

    strcat (lp->message_buffer, "DATE=");
    if (lp->last_tv_sec == tv.tv_sec)
    {
       strcat (lp->message_buffer, lp->last_date);
    }
    else
    {
       strcat (lp->message_buffer, date_buffer);
       strcpy (lp->last_date, date_buffer);
    }
    /* sprintf is SLOW! New version speeds up NetLoggerWrite by about 15%! */
#ifdef OLD_VERSION
    sprintf (date_buffer_usec, ".%06d", (int) tvPtr->tv_usec);
    strcat (lp->message_buffer, date_buffer_usec);
#else
    /* NOTE: hardcoded for 6 digits of micro-seconds at the moment.
       would need to change this for nano seconds, etc. */
    usec = (int) tv.tv_usec;
    n[6] = '\0';		/* end-of-string marker */
    for (i = 0; i < 6; i++)	/* works for micro-sec res only */
      {
	  n[5 - i] = (char) (ASCII + (usec % 10));
	  usec /= 10;
      }
    strcat (lp->message_buffer, ".");
    strcat (lp->message_buffer, n);
#endif

    strcat (lp->message_buffer, lp->host_prog);	/* contains HOST=h PROG=p NL.EVNT= */
    strcat (lp->message_buffer, keyword);
    /* prepend (optional) per-handle tag */
    if ( lp->tag != NULL )
    {
	strcat(lp->message_buffer," ");
	strcat(lp->message_buffer, lp->tag );
    }
    /* prepend (optional) global tag */
    if (globtag)
    {
	strcat (lp->message_buffer, " ");
	strcat (lp->message_buffer, globtag);
    }
    lp->last_tv_sec = tv.tv_sec;

    endPtr = lp->message_buffer + strlen (lp->message_buffer);

    /* Add user fields to buffer. */
    first = 0;
    va_start (args, fmt);
    /* put a space before the 1st field */
    if ( first++ == 0 )
    {
	*endPtr++ = ' ';
    }
    /* print the fields to the buffer */
    endPtr += vsprintf (endPtr, fmt, args);
    va_end (args);

    /* Add newline */
    *endPtr++ = '\n';
    *endPtr = '\0';

    /* Write to host, file, syslog, etc. */
    rval = writeBuffer (lp, lp->message_buffer,
			(int) (endPtr - lp->message_buffer));

  error_return:

    /* reset string buffer */
    *lp->message_buffer = '\0';

#if defined(NL_THREADSAFE)
    pthread_mutex_unlock (lp->mtx);
#endif

    return rval;
}
#endif /* NOT_NEEDED */

