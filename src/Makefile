# These are the default value for all archetures. The makefile appears
# cumbersome but only because we wanted to have one makefile that would
# work in a variety of disparate architecture and various release levels.
# Additionally, this make file is easily modifiable for test environments.
#

BINDIR     = ../bin/$(OSVER)

OBJDIR     = ../obj/$(OSVER)

ENVCFLAGS  =    -Dunix -D_BSD -D_ALL_SOURCE

ENVINCLUDE = -I. -I/usr/local/include -I/usr/include

AIXLIBS    =  -q64 -L/usr/lib -lz -lpthreads

BSDLIBS    =  /usr/lib/libz.a -lc_r -pthread

LNXLIBS32  =  /usr/lib/libz.a   -lnsl -lpthread -lrt
LNXLIBS64  =  /usr/lib64/libz.a -lnsl -lpthread -lrt

MACLIBS    =  -dynamic -L/usr/lib -lz                       -lpthread

SUNLIBS    =  /usr/local/lib/libz.a -lposix4 -lsocket -lnsl -lpthread
SUNLIBSO   =  -L/usr/lib -lz -lposix4 -lsocket -lnsl -lpthread

MKPARMS  = doitall

NLGR       = -DNL_THREADSAFE

SUN64      = -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64
SUNMT      = -D_REENTRANT -DOO_STD -mt
SUNOPT     = -O2 -fsimple -fast
SUNOPT     = -g  -fsimple $(SUNMT) $(SUN64) $(NLGR) -DSUN

SUNCC      = CC
SUNcc      = cc

S86OPT     = -D_REENTRANT -DOO_STD $(NLGR) -DSUN -DSUNX86 -Wno-deprecated
S86OPT     = $(SUNOPT) -DSUNX86 

S86CC      = CC
S86cc      = gcc

SUNGCC64   = -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64
SUNGCCMT   = -D_REENTRANT -DOO_STD
SUNGCCOPT  = -L/usr/local/lib -g \
             $(SUNGCCMT) $(SUNGCC64) $(NLGR) -DSUN

SUNGCC     = g++
SUNGcc     = gcc

LNXOPT     = $(SUN64) $(NLGR) -D_REENTRANT -DOO_STD -DLINUX -Wno-deprecated \
             -D_GNU_SOURCE -g
LNXOPT_B   = $(SUN64) $(NLGR) -D_REENTRANT -DOO_STD -DLINUX \
             -D_GNU_SOURCE -g

LNXCC      = g++
LNXcc      = gcc

BSDOPT     = $(SUN64) $(NLGR) -D_REENTRANT -DOO_STD -DFREEBSD -pthread

BSDCC      = g++
BSDcc      = gcc

MACOPT     = $(SUN64) $(NLGR) -D_REENTRANT -DOO_STD -DMACOS -Wno-deprecated -g

MACCC      = g++
MACcc      = gcc

AIXCC      = xlC
AIXcc      = xlc

AIXOPT     = -q64 $(SUN64) $(NLGR) -D_REENTRANT -DOO_STD -DAIX -g

SOURCE =      \
     bbcp.C \
     bbcp_Args.C \
     bbcp_BuffPool.C \
     bbcp_C32.C \
     bbcp_ChkSum.C \
     bbcp_Config.C \
     bbcp_Emsg.C \
     bbcp_File.C \
     bbcp_FileSpec.C \
     bbcp_FileSystem.C \
     bbcp_FS_Null.C \
     bbcp_FS_Unix.C \
     bbcp_IO.C \
     bbcp_IO_Null.C \
     bbcp_Link.C \
     bbcp_LogFile.C \
     bbcp_MD5.C \
     bbcp_NetLogger.C \
     bbcp_Network.C \
     bbcp_Node.C \
     bbcp_ProcMon.C \
     bbcp_ProgMon.C \
     bbcp_Protocol.C \
     bbcp_Pthread.C \
     bbcp_RTCopy.C \
     bbcp_Stream.C \
     bbcp_System.C \
     bbcp_Timer.C \
     bbcp_Version.C \
     bbcp_ZCX.C \
     NetLogger.c

OBJECT =      \
     $(OBJDIR)/bbcp.o \
     $(OBJDIR)/bbcp_Args.o \
     $(OBJDIR)/bbcp_BuffPool.o \
     $(OBJDIR)/bbcp_C32.o \
     $(OBJDIR)/bbcp_ChkSum.o \
     $(OBJDIR)/bbcp_Config.o \
     $(OBJDIR)/bbcp_Emsg.o \
     $(OBJDIR)/bbcp_File.o \
     $(OBJDIR)/bbcp_FileSpec.o \
     $(OBJDIR)/bbcp_FileSystem.o \
     $(OBJDIR)/bbcp_FS_Null.o \
     $(OBJDIR)/bbcp_FS_Unix.o \
     $(OBJDIR)/bbcp_IO.o \
     $(OBJDIR)/bbcp_IO_Null.o \
     $(OBJDIR)/bbcp_Link.o \
     $(OBJDIR)/bbcp_LogFile.o \
     $(OBJDIR)/bbcp_MD5.o \
     $(OBJDIR)/bbcp_NetLogger.o \
     $(OBJDIR)/bbcp_Network.o \
     $(OBJDIR)/bbcp_Node.o \
     $(OBJDIR)/bbcp_ProcMon.o \
     $(OBJDIR)/bbcp_ProgMon.o \
     $(OBJDIR)/bbcp_Protocol.o \
     $(OBJDIR)/bbcp_Pthread.o \
     $(OBJDIR)/bbcp_RTCopy.o \
     $(OBJDIR)/bbcp_Stream.o \
     $(OBJDIR)/bbcp_System.o \
     $(OBJDIR)/bbcp_Timer.o \
     $(OBJDIR)/bbcp_Version.o \
     $(OBJDIR)/bbcp_ZCX.o \
     $(OBJDIR)/NetLogger.o

TARGET  = $(BINDIR)/bbcp

all:
	@make `uname` OSVER=`../MakeSname`
	@echo Make done.

doitall: $(TARGET)

clean:
	@make cleanall OSVER=`../MakeSname`

cleanall:
	@rm -f $(OBJECT) $(BINDIR)/core $(TARGET) $(OBJDIR)/*

usage:
	@echo "Usage: make [echo | usage] [OPT=-DSUN6] [Sungcc]"

echo:		
	@for f in $(SOURCE); do \
	echo $$f ;\
	done

AIX:
	@make $(MKPARMS) \
	CC=$(AIXCC) \
	BB=$(AIXcc) \
	BFLAGS="$(ENVCFLAGS) $(AIXOPT)" \
	CFLAGS="$(ENVCFLAGS) $(AIXOPT)" \
	INCLUDE="$(ENVINCLUDE)"  \
	LIBS="$(AIXLIBS)"

FreeBSD:
	@make $(MKPARMS)  \
	CC=$(BSDCC) \
	BB=$(BSDcc) \
	CFLAGS="$(ENVCFLAGS) $(BSDOPT) $(OPT)" \
	INCLUDE="$(ENVINCLUDE)" \
	LIBS="$(BSDLIBS)"

Linux:
	@make makeLinux`/bin/uname -i`

makeLinuxi386:
	@make $(MKPARMS)  \
	CC=$(LNXCC) \
	BB=$(LNXcc) \
	CFLAGS="$(ENVCFLAGS) $(LNXOPT)" \
	BFLAGS="$(ENVCFLAGS) $(LNXOPT_B)" \
	INCLUDE="$(ENVINCLUDE)" \
	LIBS="$(LNXLIBS32)"

makeLinuxx86_64:
	@make $(MKPARMS)  \
	CC=$(LNXCC) \
	BB=$(LNXcc) \
	CFLAGS="$(ENVCFLAGS) $(LNXOPT)" \
	BFLAGS="$(ENVCFLAGS) $(LNXOPT_B)" \
	INCLUDE="$(ENVINCLUDE)" \
	LIBS="$(LNXLIBS64)"

Darwin:
	@MACOSX_DEPLOYMENT_TARGET=10.5;\
	make $(MKPARMS)  \
	CC=$(MACCC) \
	BB=$(MACcc) \
	CFLAGS="$(ENVCFLAGS) $(MACOPT) $(OPT)" \
	INCLUDE="$(ENVINCLUDE)" \
	LIBS="$(MACLIBS)"

UNICOS/mp:
	@make $(MKPARMS)  \
 	CC=$(LNXCC) \
 	BB=$(LNXcc) \
 	CFLAGS="$(ENVCFLAGS) $(LNXOPT)" \
	BFLAGS="$(ENVCFLAGS) $(LNXOPT_B)" \
 	INCLUDE="$(ENVINCLUDE)" \
 	LIBS="$(LNXLIBS)"

SunOS:
	@make $(MKPARMS)  \
	CC=$(SUNCC) \
	BB=$(SUNcc) \
	CFLAGS="$(ENVCFLAGS) $(SUNOPT) $(OPT)" \
	INCLUDE="$(ENVINCLUDE)" \
	LIBS="$(SUNLIBS)"

SunX86:
	@make $(MKPARMS)  \
	CC=$(S86CC) \
	BB=$(S86cc) \
	CFLAGS="$(ENVCFLAGS) $(S86OPT) $(OPT)" \
	INCLUDE="$(ENVINCLUDE)" \
	LIBS="$(SUNLIBSO)"

Sungcc:
	@make $(MKPARMS)  \
	CC=$(SUNGCC) \
	BB=$(SUNGcc) \
	CFLAGS="$(ENVCFLAGS) $(SUNGCCOPT) $(OPT)" \
	INCLUDE="$(ENVINCLUDE)" \
	LIBS="$(SUNLIBSO)" \
	OSVER=`../MakeSname`

$(TARGET): $(OBJECT)
	@echo Creating executable $(TARGET) ...
	@$(CC) $(OBJECT) $(LIBS) -o $(TARGET)

$(OBJDIR)/bbcp.o: bbcp.C bbcp_Args.h bbcp_Config.h bbcp_Debug.h bbcp_FileSpec.h \
                  bbcp_LogFile.h bbcp_Node.h bbcp_Protocol.h bbcp_System.h \
                  bbcp_Headers.h
	@echo Compiling bbcp.C
	@$(CC) -c $(CFLAGS) $(INCLUDE) $(*F).C -o $(OBJDIR)/$(*F).o

$(OBJDIR)/bbcp_Args.o: bbcp_Args.C bbcp_Args.h bbcp_Config.h bbcp_Stream.h
	@echo Compiling bbcp_Args.C
	@$(CC) -c $(CFLAGS) $(INCLUDE) $(*F).C -o $(OBJDIR)/$(*F).o

$(OBJDIR)/bbcp_BuffPool.o: bbcp_BuffPool.C bbcp_BuffPool.h bbcp_Debug.h \
                           bbcp_Emsg.h bbcp_Pthread.h bbcp_Headers.h
	@echo Compiling bbcp_BuffPool.C
	@$(CC) -c $(CFLAGS) $(INCLUDE) $(*F).C -o $(OBJDIR)/$(*F).o

$(OBJDIR)/bbcp_C32.o: bbcp_C32.C bbcp_C32.h bbcp_ChkSum.h bbcp_Endian.h
	@echo Compiling bbcp_C32.C
	@$(CC) -c $(CFLAGS) $(INCLUDE) $(*F).C -o $(OBJDIR)/$(*F).o

$(OBJDIR)/bbcp_ChkSum.o: bbcp_ChkSum.C bbcp_ChkSum.h bbcp_Endian.h \
                         bbcp_A32.h bbcp_C32.h bbcp_MD5.h
	@echo Compiling bbcp_ChkSum.C
	@$(CC) -c $(CFLAGS) $(INCLUDE) $(*F).C -o $(OBJDIR)/$(*F).o

$(OBJDIR)/bbcp_Config.o: bbcp_Config.C bbcp_Config.h bbcp_Args.h \
                         bbcp_Debug.h bbcp_Emsg.h bbcp_FileSpec.h bbcp_LogFile.h \
                         bbcp_NetLogger.h bbcp_Network.h bbcp_Platform.h \
                         bbcp_Stream.h bbcp_System.h bbcp_Version.h \
                         bbcp_Headers.h  bbcp_Pthread.h
	@echo Compiling bbcp_Config.C
	@$(CC) -c $(CFLAGS) $(INCLUDE) $(*F).C -o $(OBJDIR)/$(*F).o

$(OBJDIR)/bbcp_Emsg.o: bbcp_Emsg.C bbcp_Emsg.h bbcp_Debug.h bbcp_Platform.h \
                       bbcp_Headers.h
	@echo Compiling bbcp_Emsg.C
	@$(CC) -c $(CFLAGS) $(INCLUDE) $(*F).C -o $(OBJDIR)/$(*F).o

$(OBJDIR)/bbcp_File.o: bbcp_File.C bbcp_File.h bbcp_IO.h bbcp_BuffPool.h \
                       bbcp_Config.h bbcp_Debug.h bbcp_Emsg.h bbcp_Pthread.h \
                       bbcp_Platform.h bbcp_Headers.h bbcp_ChkSum.h \
                       bbcp_RTCopy.h
	@echo Compiling bbcp_File.C
	@$(CC) -c $(CFLAGS) $(INCLUDE) $(*F).C -o $(OBJDIR)/$(*F).o

$(OBJDIR)/bbcp_FileSpec.o: bbcp_FileSpec.C bbcp_FileSpec.h bbcp_Config.h \
                           bbcp_Emsg.h bbcp_FileSystem.h bbcp_FS_Unix.h \
                           bbcp_Pthread.h
	@echo Compiling bbcp_FileSpec.C
	@$(CC) -c $(CFLAGS) $(INCLUDE) $(*F).C -o $(OBJDIR)/$(*F).o

$(OBJDIR)/bbcp_FileSystem.o: bbcp_FileSystem.C bbcp_FileSystem.h \
                             bbcp_FS_Null.h bbcp_FS_Unix.h
	@echo Compiling bbcp_FileSystem.C
	@$(CC) -c $(CFLAGS) $(INCLUDE) $(*F).C -o $(OBJDIR)/$(*F).o

$(OBJDIR)/bbcp_FS_Null.o: bbcp_FS_Null.C bbcp_FS_Null.h \
                          bbcp_IO_Null.h bbcp_FileSystem.h bbcp_System.h \
                          bbcp_Platform.h bbcp_Pthread.h
	@echo Compiling bbcp_FS_Null.C
	@$(CC) -c $(CFLAGS) $(INCLUDE) $(*F).C -o $(OBJDIR)/$(*F).o

$(OBJDIR)/bbcp_FS_Unix.o: bbcp_FS_Unix.C bbcp_FS_Unix.h bbcp_FileSystem.h \
                          bbcp_Config.h  bbcp_IO.h \
                          bbcp_Platform.h bbcp_System.h bbcp_Pthread.h
	@echo Compiling bbcp_FS_Unix.C
	@$(CC) -c $(CFLAGS) $(INCLUDE) $(*F).C -o $(OBJDIR)/$(*F).o

$(OBJDIR)/bbcp_IO.o: bbcp_IO.C bbcp_IO.h bbcp_NetLogger.h bbcp_Timer.h
	@echo Compiling bbcp_IO.C
	@$(CC) -c $(CFLAGS) $(INCLUDE) $(*F).C -o $(OBJDIR)/$(*F).o

$(OBJDIR)/bbcp_IO_Null.o: bbcp_IO_Null.C bbcp_IO_Null.h bbcp_IO.h
	@echo Compiling bbcp_IO_Null.C
	@$(CC) -c $(CFLAGS) $(INCLUDE) $(*F).C -o $(OBJDIR)/$(*F).o

$(OBJDIR)/bbcp_Link.o:  bbcp_Link.C bbcp_Link.h bbcp_File.h bbcp_IO.h \
                        bbcp_BuffPool.h bbcp_ChkSum.h  bbcp_Config.h \
                        bbcp_Debug.h bbcp_Emsg.h \
                        bbcp_Link.h bbcp_Network.h bbcp_Platform.h \
                        bbcp_Timer.h
	@echo Compiling bbcp_Link.C
	@$(CC) -c $(CFLAGS) $(INCLUDE) $(*F).C -o $(OBJDIR)/$(*F).o

$(OBJDIR)/bbcp_LogFile.o:  bbcp_LogFile.C bbcp_LogFile.h bbcp_Pthread.h \
                           bbcp_Debug.h bbcp_Emsg.h bbcp_Platform.h \
                           bbcp_Headers.h bbcp_Timer.h
	@echo Compiling bbcp_LogFile.C
	@$(CC) -c $(CFLAGS) $(INCLUDE) $(*F).C -o $(OBJDIR)/$(*F).o

$(OBJDIR)/bbcp_MD5.o: bbcp_MD5.C bbcp_MD5.h bbcp_ChkSum.h bbcp_Endian.h
	@echo Compiling bbcp_MD5.C
	@$(CC) -c $(CFLAGS) $(INCLUDE) $(*F).C -o $(OBJDIR)/$(*F).o

$(OBJDIR)/bbcp_NetLogger.o:  bbcp_NetLogger.C bbcp_NetLogger.h NetLogger.h
	@echo Compiling bbcp_Netlogger.c
	@$(CC) -c $(CFLAGS) $(INCLUDE) $(*F).C -o $(OBJDIR)/$(*F).o

$(OBJDIR)/bbcp_Network.o:  bbcp_Network.C bbcp_Network.h bbcp_Debug.h \
                           bbcp_Emsg.h bbcp_Link.h bbcp_Pthread.h bbcp_Platform.h
	@echo Compiling bbcp_Network.C
	@$(CC) -c $(CFLAGS) $(INCLUDE) $(*F).C -o $(OBJDIR)/$(*F).o

$(OBJDIR)/bbcp_Node.o: bbcp_Node.C bbcp_Node.h bbcp_Config.h bbcp_Emsg.h \
                       bbcp_Debug.h bbcp_File.h bbcp_FileSpec.h bbcp_Link.h \
                       bbcp_BuffPool.h bbcp_Network.h bbcp_Protocol.h \
                       bbcp_ProcMon.h bbcp_ProgMon.h bbcp_Stream.h \
                       bbcp_System.h bbcp_ZCX.h bbcp_Headers.h
	@echo Compiling bbcp_Node.C
	@$(CC) -c $(CFLAGS) $(INCLUDE) $(*F).C -o $(OBJDIR)/$(*F).o

$(OBJDIR)/bbcp_ProcMon.o: bbcp_ProcMon.C bbcp_ProcMon.h bbcp_Debug.h \
                          bbcp_Emsg.h bbcp_File.h bbcp_Pthread.h bbcp_Headers.h
	@echo Compiling bbcp_ProcMon.C
	@$(CC) -c $(CFLAGS) $(INCLUDE) $(*F).C -o $(OBJDIR)/$(*F).o

$(OBJDIR)/bbcp_ProgMon.o: bbcp_ProgMon.C bbcp_ProgMon.h bbcp_Config.h \
                          bbcp_Debug.h bbcp_File.h bbcp_Headers.h \
                          bbcp_Pthread.h bbcp_Timer.h bbcp_ZCX.h
	@echo Compiling bbcp_ProgMon.C
	@$(CC) -c $(CFLAGS) $(INCLUDE) $(*F).C -o $(OBJDIR)/$(*F).o

$(OBJDIR)/bbcp_Protocol.o: bbcp_Protocol.C bbcp_Protocol.h bbcp_Config.h  \
                           bbcp_Node.h bbcp_FileSpec.h bbcp_FileSystem.h  \
                           bbcp_IO.h bbcp_File.h bbcp_Link.h bbcp_Debug.h \
                           bbcp_Pthread.h bbcp_Stream.h bbcp_Emsg.h \
                           bbcp_Network.h bbcp_Version.h bbcp_Headers.h
	@echo Compiling bbcp_Protocol.C
	@$(CC) -c $(CFLAGS) $(INCLUDE) $(*F).C -o $(OBJDIR)/$(*F).o

$(OBJDIR)/bbcp_Pthread.o: bbcp_Pthread.C bbcp_Pthread.h
	@echo Compiling bbcp_Pthread.C
	@$(CC) -c $(CFLAGS) $(INCLUDE) $(*F).C -o $(OBJDIR)/$(*F).o

$(OBJDIR)/bbcp_RTCopy.o: bbcp_RTCopy.C bbcp_RTCopy.h bbcp_Config.h \
                         bbcp_Emsg.h bbcp_FileSystem.h bbcp_Pthread.h
	@echo Compiling bbcp_RTCopy.C
	@$(CC) -c $(CFLAGS) $(INCLUDE) $(*F).C -o $(OBJDIR)/$(*F).o

$(OBJDIR)/bbcp_Stream.o: bbcp_Stream.C bbcp_Stream.h bbcp_Config.h bbcp_Debug.h \
                         bbcp_Emsg.h bbcp_System.h
	@echo Compiling bbcp_Stream.C
	@$(CC) -c $(CFLAGS) $(INCLUDE) $(*F).C -o $(OBJDIR)/$(*F).o

$(OBJDIR)/bbcp_System.o: bbcp_System.C bbcp_System.h bbcp_Config.h \
                         bbcp_Debug.h bbcp_Emsg.h bbcp_Platform.h bbcp_Pthread.h
	@echo Compiling bbcp_System.C
	@$(CC) -c $(CFLAGS) $(INCLUDE) $(*F).C -o $(OBJDIR)/$(*F).o

$(OBJDIR)/bbcp_Timer.o: bbcp_Timer.C bbcp_Timer.h
	@echo Compiling bbcp_Timer.C
	@$(CC) -c $(CFLAGS) $(INCLUDE) $(*F).C -o $(OBJDIR)/$(*F).o

$(OBJDIR)/bbcp_Version.o: bbcp_Version.C bbcp_Version.h bbcp_Config.h bbcp_Emsg.h
	@echo Compiling bbcp_Version.C
	@$(CC) -c $(CFLAGS) $(INCLUDE) $(*F).C -o $(OBJDIR)/$(*F).o

$(OBJDIR)/bbcp_ZCX.o: bbcp_ZCX.C bbcp_ZCX.h bbcp_BuffPool.h bbcp_Emsg.h
	@echo Compiling bbcp_ZCX.C
	@$(CC) -c $(CFLAGS) $(INCLUDE) $(*F).C -o $(OBJDIR)/$(*F).o

$(OBJDIR)/NetLogger.o: NetLogger.c NetLogger.h
	@echo Compiling NetLogger.c
	@$(BB) -c $(BFLAGS) $(INCLUDE) $(*F).c -o $(OBJDIR)/$(*F).o
