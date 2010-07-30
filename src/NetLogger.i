 


 

static const char rcsid[] =
    "@(#) $Id$";

 
 

 
 
 

 



#ident	"@(#)errno.h	1.16	99/07/26 SMI"

 

 
 

 
 
 


#ident	"@(#)errno.h	1.20	00/02/14 SMI"

 


 


 

 


 

 



 


 

 
	 
				 
	 
				 
 

 






extern int *___errno();


 
 

 
 
 

 

 


#ident	"@(#)netdb.h	1.23	99/12/06 SMI"

 
 

 
 
 

 


#ident	"@(#)types.h	1.66	00/02/14 SMI"

 


#ident	"@(#)isa_defs.h	1.20	99/05/04 SMI"

 



 

 

 

 

 

 

 

 

 


 

 



 

 
 
 


#ident	"@(#)feature_tests.h	1.18	99/07/26 SMI"

 



 

 

 

 

 

 

 



 
 

 
 

 
 
 


#ident	"@(#)machtypes.h	1.13	99/05/04 SMI"

 

 
 
 



 


typedef	struct	_label_t { long val[2]; } label_t;


typedef	unsigned char	lock_t;		 



 
 


#ident	"@(#)int_types.h	1.6	97/08/20 SMI"

 

 



 
typedef char			int8_t;
typedef short			int16_t;
typedef int			int32_t;
typedef	long long		int64_t;

typedef unsigned char		uint8_t;
typedef unsigned short		uint16_t;
typedef unsigned int		uint32_t;
typedef unsigned long long	uint64_t;

 
typedef int64_t			intmax_t;
typedef uint64_t		uintmax_t;

 
typedef	int			intptr_t;
typedef	unsigned int		uintptr_t;

 
typedef char			int_least8_t;
typedef short			int_least16_t;
typedef int			int_least32_t;
typedef long long		int_least64_t;

typedef unsigned char		uint_least8_t;
typedef unsigned short		uint_least16_t;
typedef unsigned int		uint_least32_t;
typedef unsigned long long	uint_least64_t;





 
typedef	long long		longlong_t;
typedef	unsigned long long	u_longlong_t;

 
typedef long		t_scalar_t;	 
typedef unsigned long	t_uscalar_t;

 
typedef	unsigned char	uchar_t;
typedef	unsigned short	ushort_t;
typedef	unsigned int	uint_t;
typedef	unsigned long	ulong_t;

typedef	char		*caddr_t;	 
typedef	long		daddr_t;	 
typedef	short		cnt_t;		 

typedef	ulong_t		paddr_t;	 

typedef	int	ptrdiff_t;		 

 
typedef	ulong_t		pfn_t;		 
typedef	ulong_t		pgcnt_t;	 
typedef	long		spgcnt_t;	 

typedef	uchar_t		use_t;		 
typedef	short		sysid_t;
typedef	short		index_t;
typedef void		*timeout_id_t;	 
typedef void		*bufcall_id_t;	 

 

typedef longlong_t	off_t;		 

typedef longlong_t	off64_t;	 


typedef u_longlong_t	ino_t;		 
typedef longlong_t	blkcnt_t;	 
typedef u_longlong_t	fsblkcnt_t;	 
typedef u_longlong_t	fsfilcnt_t;	 

typedef u_longlong_t	ino64_t;	 
typedef longlong_t	blkcnt64_t;	 
typedef u_longlong_t	fsblkcnt64_t;	 
typedef u_longlong_t	fsfilcnt64_t;	 

typedef	long		blksize_t;	 

typedef enum { B_FALSE, B_TRUE } boolean_t;

 
typedef int64_t		pad64_t;
typedef	uint64_t	upad64_t;

typedef	longlong_t	offset_t;
typedef	u_longlong_t	u_offset_t;
typedef u_longlong_t	len_t;
typedef	longlong_t	diskaddr_t;

 

typedef union {
	offset_t	_f;	 
	struct {
		int32_t	_u;	 
		int32_t	_l;	 
	} _p;
} lloff_t;


typedef union {
	diskaddr_t	_f;	 
	struct {
		int32_t	_u;	 
		int32_t	_l;	 
	} _p;
} lldaddr_t;

typedef uint_t k_fltset_t;	 

 
typedef	long		id_t;		 

 
typedef uint_t		useconds_t;	 

typedef long	suseconds_t;	 

 
typedef ulong_t	major_t;	 
typedef ulong_t	minor_t;	 

 
typedef short	pri_t;

 
typedef	ushort_t o_mode_t;		 
typedef short	o_dev_t;		 
typedef	ushort_t o_uid_t;		 
typedef	o_uid_t	o_gid_t;		 
typedef	short	o_nlink_t;		 
typedef short	o_pid_t;		 
typedef ushort_t o_ino_t;		 


 
typedef	int	key_t;			 
typedef	ulong_t	mode_t;			 

typedef	long	uid_t;			 

typedef	uid_t	gid_t;			 

typedef id_t    taskid_t;
typedef id_t    projid_t;

 
typedef	unsigned int	pthread_t;	 
typedef	unsigned int	pthread_key_t;	 

typedef	struct	_pthread_mutex {		 
	struct {
		uint16_t	__pthread_mutex_flag1;
		uint8_t		__pthread_mutex_flag2;
		uint8_t		__pthread_mutex_ceiling;
		uint32_t 	__pthread_mutex_type;
	} __pthread_mutex_flags;
	union {
		struct {
			uint8_t	__pthread_mutex_pad[8];
		} __pthread_mutex_lock64;
		upad64_t __pthread_mutex_owner64;
	} __pthread_mutex_lock;
	upad64_t __pthread_mutex_data;
} pthread_mutex_t;

typedef	struct	_pthread_cond {		 
	struct {
		uint8_t		__pthread_cond_flag[4];
		uint32_t 	__pthread_cond_type;
	} __pthread_cond_flags;
	upad64_t __pthread_cond_data;
} pthread_cond_t;

 
typedef	struct _pthread_rwlock {	 
	int32_t		__pthread_rwlock_readers;
	uint16_t	__pthread_rwlock_type;
	uint16_t	__pthread_rwlock_magic;
	upad64_t	__pthread_rwlock_pad1[3];
	upad64_t	__pthread_rwlock_pad2[2];
	upad64_t	__pthread_rwlock_pad3[2];
} pthread_rwlock_t;

 
typedef struct _pthread_attr {
	void	*__pthread_attrp;
} pthread_attr_t;


 
typedef struct _pthread_mutexattr {
	void	*__pthread_mutexattrp;
} pthread_mutexattr_t;


 
typedef struct _pthread_condattr {
	void	*__pthread_condattrp;
} pthread_condattr_t;

 
typedef	struct	_once {
	upad64_t	__pthread_once_pad[4];
} pthread_once_t;

 
typedef struct _pthread_rwlockattr {
	void	*__pthread_rwlockattrp;
} pthread_rwlockattr_t;

typedef ulong_t	dev_t;			 

typedef	ulong_t	nlink_t;		 
typedef	long	pid_t;			 

typedef	uint_t	size_t;		 

typedef int	ssize_t;	 

typedef	long		time_t;	 

typedef	long		clock_t;  

typedef	int	clockid_t;	 

typedef	int	timer_t;	 


 
typedef	unsigned char	unchar;
typedef	unsigned short	ushort;
typedef	unsigned int	uint;
typedef	unsigned long	ulong;
 



 


 

 
typedef unsigned char	u_char;
typedef unsigned short	u_short;
typedef unsigned int	u_int;
typedef unsigned long	u_long;
typedef struct _quad { int val[2]; } quad_t;	 
typedef quad_t		quad;			 
 

 
 
 
 

 
 
 


#ident	"@(#)select.h	1.16	98/04/27 SMI"

 

 
 
 


 
 

 
 
 

 

 


#ident	"@(#)time.h	2.66	01/01/17 SMI"

 

 
 
 


 





struct timeval {
	time_t		tv_sec;		 
	suseconds_t	tv_usec;	 
};



struct timezone {
	int	tz_minuteswest;	 
	int	tz_dsttime;	 
};



 
 
 

 
 
 

 





 



 
				 
				 
				 
				 

struct	itimerval {
	struct	timeval it_interval;	 
	struct	timeval it_value;	 
};



 



 
typedef	longlong_t	hrtime_t;


int adjtime(struct timeval *, struct timeval *);



int getitimer(int, struct itimerval *);
int utimes(const char *, const struct timeval *);
int setitimer(int, struct itimerval *, struct itimerval *);


 

int settimeofday(struct timeval *, void *);
hrtime_t	gethrtime(void);
hrtime_t	gethrvtime(void);



int gettimeofday(struct timeval *, void *);


 
 
 

 
 
 

 


#ident	"@(#)time.h	1.39	99/08/10 SMI"

 

 
 
 

 
 

 
 
 

 

 
 

 
 
 

 

 


#ident	"@(#)time_iso.h	1.1	99/08/09 SMI"

 

 
 
 









struct	tm {	 
	int	tm_sec;
	int	tm_min;
	int	tm_hour;
	int	tm_mday;
	int	tm_mon;
	int	tm_year;
	int	tm_wday;
	int	tm_yday;
	int	tm_isdst;
};



extern char *asctime(const struct tm *);
extern clock_t clock(void);
extern char *ctime(const time_t *);
extern double difftime(time_t, time_t);
extern struct tm *gmtime(const time_t *);
extern struct tm *localtime(const time_t *);
extern time_t mktime(struct tm *);
extern time_t time(time_t *);
extern size_t strftime(char *, size_t, const char *, const struct tm *);





 





extern struct tm *gmtime_r(const time_t *, struct tm *);
extern struct tm *localtime_r(const time_t *, struct tm *);

extern char *strptime(const char *, const char *, struct tm *);



 

 


#ident	"@(#)time_impl.h	1.5	99/10/05 SMI"

 

 
 
 





 

typedef struct  timespec {		 
	time_t		tv_sec;		 
	long		tv_nsec;	 
} timespec_t;


typedef struct timespec timestruc_t;	 


 


 
typedef struct itimerspec {		 
	struct timespec	it_interval;	 
	struct timespec	it_value;	 
} itimerspec_t;







 





 
union sigval {
	int	sival_int;	 
	void	*sival_ptr;	 
};

struct sigevent {
	int		sigev_notify;	 
	int		sigev_signo;	 
	union sigval	sigev_value;	 
	void		(*sigev_notify_function)(union sigval);
	pthread_attr_t	*sigev_notify_attributes;
	int		__sigev_pad2;
};

extern int clock_getres(clockid_t, struct timespec *);
extern int clock_gettime(clockid_t, struct timespec *);
extern int clock_settime(clockid_t, const struct timespec *);
extern int timer_create(clockid_t, struct sigevent *, timer_t *);
extern int timer_delete(timer_t);
extern int timer_getoverrun(timer_t);
extern int timer_gettime(timer_t, struct itimerspec *);
extern int timer_settime(timer_t, int, const struct itimerspec *,
		struct itimerspec *);
extern int nanosleep(const struct timespec *, struct timespec *);

extern void tzset(void);

extern char *tzname[2];

extern long _sysconf(int);	 
				 

extern long timezone;
extern int daylight;


extern int cftime(char *, char *, const time_t *);
extern int ascftime(char *, const char *, const struct tm *);
extern long altzone;

extern struct tm *getdate(const char *);

extern int *_getdate_err_addr(void);


 

 




extern char *asctime_r(const struct tm *, char *, int);
extern char *ctime_r(const time_t *, char *, int);






 
 
 
 

 
 
 






 

typedef	long	fd_mask;
typedef	long	fds_mask;

 



typedef	struct fd_set {
	long	fds_bits[ ( ( ( 1024 ) + ( ( ( sizeof ( fds_mask ) * 8 ) ) - 1 ) ) / ( ( sizeof ( fds_mask ) * 8 ) ) )];
} fd_set;





extern int select(int, fd_set *, fd_set *, fd_set *, struct timeval *);




 
 


 
 

 


#ident	"@(#)in.h	1.30	00/08/10 SMI"

 

 
 
 


 
 

 
 
 

 



 
 

 
 
 

 


#ident	"@(#)stream.h	1.86	00/10/23 SMI"

 
 

 
 

 
 
 

 


#ident	"@(#)vnode.h	1.85	99/07/30 SMI"

 
 

 
 
 

 

 

 


#ident	"@(#)t_lock.h	1.45	98/02/01 SMI"

 


#ident	"@(#)machlock.h	1.21	00/04/27 SMI"



 
 

 
 
 

 




typedef	lock_t	disp_lock_t;		 

 

 

 
extern	int	hres_lock;


 

 


 

 
 

 
 
 

 


#ident	"@(#)param.h	1.76	00/02/14 SMI"

 
 

 
 
 

 

 




 












				 
				 
				 

 

 

 

 





 

 


 



 

 


 


 

 

 


 


 

 

 

 


#ident	"@(#)unistd.h	1.37	98/10/28 SMI"

 

 
 
 



 


 
 
 

 


 

 
 
 
 
 

 

 

 


 


 

 

 

 
 

 










extern long _sysconf(int);	 




 


#ident	"@(#)mutex.h	1.20	98/02/01 SMI"

 
 

 
 
 

 




 
typedef enum {
	MUTEX_ADAPTIVE = 0,	 
	MUTEX_SPIN = 1,		 
	MUTEX_DRIVER = 4,	 
	MUTEX_DEFAULT = 6	 
} kmutex_type_t;

typedef struct mutex {
	void	*_opaque[2];
} kmutex_t;




 


#ident	"@(#)rwlock.h	1.9	98/02/18 SMI"

 

 
 

 
 
 

 




typedef enum {
	RW_DRIVER = 2,		 
	RW_DEFAULT = 4		 
} krw_type_t;

typedef enum {
	RW_WRITER,
	RW_READER
} krw_t;

typedef struct _krwlock {
	void	*_opaque[1];
} krwlock_t;




 


#ident	"@(#)semaphore.h	1.5	98/02/01 SMI"

 




typedef enum {
	SEMA_DEFAULT,
	SEMA_DRIVER
} ksema_type_t;

typedef struct _ksema {
	void	* _opaque[2];	 
} ksema_t;




 

 


#ident	"@(#)condvar.h	1.12	01/01/17 SMI"

 
 

 
 
 

 

 
 

 
 
 

 

 




 

typedef struct _kcondvar {
	ushort_t	_opaque;
} kcondvar_t;

typedef	enum {
	CV_DEFAULT,
	CV_DRIVER
} kcv_type_t;








 




 

 

 
 

 
 
 

 


#ident	"@(#)cred.h	1.21	97/01/09 SMI"



 

typedef struct cred {
	uint_t	cr_ref;			 
	uid_t	cr_uid;			 
	gid_t	cr_gid;			 
	uid_t	cr_ruid;		 
	gid_t	cr_rgid;		 
	uid_t	cr_suid;		 
	gid_t	cr_sgid;		 
	uint_t	cr_ngroups;		 
	gid_t	cr_groups[1];		 
} cred_t;



 
 

 
 
 

 


#ident	"@(#)uio.h	1.29	97/06/29 SMI"

 

 
 
 



 
 

 
 
 

 


 

typedef struct iovec {
	caddr_t	iov_base;
	long	iov_len;
} iovec_t;


 
typedef enum uio_seg { UIO_USERSPACE, UIO_SYSSPACE, UIO_USERISPACE } uio_seg_t;

typedef struct uio {
	iovec_t	*uio_iov;	 
	int	uio_iovcnt;	 
	lloff_t	_uio_offset;	 
	uio_seg_t uio_segflg;	 
	short	uio_fmode;	 
	lldaddr_t _uio_limit;	 
	ssize_t	uio_resid;	 
} uio_t;



 
typedef enum uio_rw { UIO_READ, UIO_WRITE } uio_rw_t;



extern ssize_t readv(int, const struct iovec *, int);
extern ssize_t writev(int, const struct iovec *, int);




 
 

 
 
 

 


#ident	"@(#)resource.h	1.25	98/06/30 SMI"

 

 
 
 


 
 

 
 
 

 

 
 

 
 
 

 

 



 



 




 

typedef u_longlong_t	rlim_t;





struct rlimit {
	rlim_t	rlim_cur;		 
	rlim_t	rlim_max;		 
};

 

typedef u_longlong_t	rlim64_t;


struct rlimit64 {
	rlim64_t	rlim_cur;	 
	rlim64_t	rlim_max;	 
};




struct	rusage {
	struct timeval ru_utime;	 
	struct timeval ru_stime;	 
	long	ru_maxrss;		 
	long	ru_ixrss;		 
	long	ru_idrss;		 
	long	ru_isrss;		 
	long	ru_minflt;		 
	long	ru_majflt;		 
	long	ru_nswap;		 
	long	ru_inblock;		 
	long	ru_oublock;		 
	long	ru_msgsnd;		 
	long	ru_msgrcv;		 
	long	ru_nsignals;		 
	long	ru_nvcsw;		 
	long	ru_nivcsw;		 
};

 
#pragma	redefine_extname	setrlimit	setrlimit64
#pragma	redefine_extname	getrlimit	getrlimit64



extern int setrlimit(int, const struct rlimit *);
extern int getrlimit(int, struct rlimit *);

 
extern int setrlimit64(int, const struct rlimit64 *);
extern int getrlimit64(int, struct rlimit64 *);

extern int getpriority(int, id_t);
extern int setpriority(int, id_t, int);
extern int getrusage(int, struct rusage *);




 
 

 
 
 

 


#ident	"@(#)seg_enum.h	1.3	95/12/22 SMI"


 

 
enum fault_type {
	F_INVAL,		 
	F_PROT,			 
	F_SOFTLOCK,		 
	F_SOFTUNLOCK		 
};

 
enum lock_type {
	L_PAGELOCK,		 
	L_PAGEUNLOCK,		 
	L_PAGERECLAIM		 
};

 
enum seg_rw {
	S_OTHER,		 
	S_READ,			 
	S_WRITE,		 
	S_EXEC,			 
	S_CREATE		 
};





 

 
typedef enum vtype {
	VNON	= 0,
	VREG	= 1,
	VDIR	= 2,
	VBLK	= 3,
	VCHR	= 4,
	VLNK	= 5,
	VFIFO	= 6,
	VDOOR	= 7,
	VPROC	= 8,
	VSOCK	= 9,
	VBAD	= 10
} vtype_t;

 
 
typedef struct vnode {
	kmutex_t	v_lock;			 
	ushort_t	v_flag;			 
	uint_t		v_count;		 
	struct vfs	*v_vfsmountedhere;	 
	struct vnodeops	*v_op;			 
	struct vfs	*v_vfsp;		 
	struct stdata	*v_stream;		 
	struct page	*v_pages;		 
	enum vtype	v_type;			 
	dev_t		v_rdev;			 
	caddr_t		v_data;			 
	struct filock	*v_filocks;		 
	struct shrlocklist *v_shrlocks;		 
	kcondvar_t	v_cv;			 
	void		*v_locality;		 
} vnode_t;

 


 

 

 

 


 

 

typedef struct vattr {
	uint_t		va_mask;	 
	vtype_t		va_type;	 
	mode_t		va_mode;	 
	uid_t		va_uid;		 
	gid_t		va_gid;		 
	dev_t		va_fsid;	 
	u_longlong_t	va_nodeid;	 
	nlink_t		va_nlink;	 
	u_offset_t	va_size;	 
	timestruc_t	va_atime;	 
	timestruc_t	va_mtime;	 
	timestruc_t	va_ctime;	 
	dev_t		va_rdev;	 
	uint_t		va_blksize;	 
	u_longlong_t	va_nblocks;	 
	uint_t		va_vcode;	 
} vattr_t;

typedef vattr_t		vattr32_t;

 





 

 


 


 
enum rm		{ RMFILE, RMDIRECTORY };	 
enum symfollow	{ NO_FOLLOW, FOLLOW };		 
enum vcexcl	{ NONEXCL, EXCL };		 
enum create	{ CRCREAT, CRMKNOD, CRMKDIR };	 

typedef enum rm		rm_t;
typedef enum symfollow	symfollow_t;
typedef enum vcexcl	vcexcl_t;
typedef enum create	create_t;

 

typedef struct vsecattr {
	uint_t		vsa_mask;	 
	int		vsa_aclcnt;	 
	void		*vsa_aclentp;	 
	int		vsa_dfaclcnt;	 
	void		*vsa_dfaclentp;	 
} vsecattr_t;

 

 
struct pathname;
struct fid;
struct flock64;
struct shrlock;
struct page;
struct seg;
struct as;
struct pollhead;

 
typedef struct vnodeops {
	int	(*vop_open)(struct vnode **vpp, int flag, struct cred *cr);
	int	(*vop_close)(struct vnode *vp, int flag, int count,
				offset_t offset, struct cred *cr);
	int	(*vop_read)(struct vnode *vp, struct uio *uiop, int ioflag,
			    struct cred *cr);
	int	(*vop_write)(struct vnode *vp, struct uio *uiop, int ioflag,
				struct cred *cr);
	int	(*vop_ioctl)(struct vnode *vp, int cmd, intptr_t arg, int flag,
				struct cred *cr, int *rvalp);
	int	(*vop_setfl)(struct vnode *vp, int oflags, int nflags,
				struct cred *cr);
	int	(*vop_getattr)(struct vnode *vp, struct vattr *vap, int flags,
				struct cred *cr);
	int	(*vop_setattr)(struct vnode *vp, struct vattr *vap, int flags,
				struct cred *cr);
	int	(*vop_access)(struct vnode *vp, int mode, int flags,
				struct cred *cr);
	int	(*vop_lookup)(struct vnode *dvp, char *nm, struct vnode **vpp,
				struct pathname *pnp, int flags,
				struct vnode *rdir, struct cred *);
	int	(*vop_create)(struct vnode *dvp, char *name, struct vattr *vap,
				vcexcl_t excl, int mode, struct vnode **vpp,
				struct cred *cr, int flag);
	int	(*vop_remove)(struct vnode *vp, char *nm, struct cred *cr);
	int	(*vop_link)(struct vnode *tdvp, struct vnode *svp, char *tnm,
				struct cred *cr);
	int	(*vop_rename)(struct vnode *sdvp, char *snm,
				struct vnode *tdvp, char *tnm, struct cred *cr);
	int	(*vop_mkdir)(struct vnode *dvp, char *dirname,
				struct vattr *vap, struct vnode **vpp,
				struct cred *cr);
	int	(*vop_rmdir)(struct vnode *vp, char *nm, struct vnode *cdir,
				struct cred *cr);
	int	(*vop_readdir)(struct vnode *vp, struct uio *uiop,
				struct cred *cr, int *eofp);
	int	(*vop_symlink)(struct vnode *dvp, char *linkname,
				struct vattr *vap, char *target,
				struct cred *cr);
	int	(*vop_readlink)(struct vnode *vp, struct uio *uiop,
				struct cred *cr);
	int	(*vop_fsync)(struct vnode *vp, int syncflag, struct cred *cr);
	void	(*vop_inactive)(struct vnode *vp, struct cred *cr);
	int	(*vop_fid)(struct vnode *vp, struct fid *fidp);
	void	(*vop_rwlock)(struct vnode *vp, int write_lock);
	void	(*vop_rwunlock)(struct vnode *vp, int write_lock);
	int	(*vop_seek)(struct vnode *vp, offset_t ooff, offset_t *noffp);
	int	(*vop_cmp)(struct vnode *vp1, struct vnode *vp2);
	int	(*vop_frlock)(struct vnode *vp, int cmd, struct flock64 *bfp,
				int flag, offset_t offset, struct cred *cr);
	int	(*vop_space)(struct vnode *vp, int cmd, struct flock64 *bfp,
				int flag, offset_t offset, struct cred *cr);
	int	(*vop_realvp)(struct vnode *vp, struct vnode **vpp);
	int	(*vop_getpage)(struct vnode *vp, offset_t off, size_t len,
				uint_t *protp, struct page **plarr, size_t plsz,
				struct seg *seg, caddr_t addr, enum seg_rw rw,
				struct cred *cr);
	int	(*vop_putpage)(struct vnode *vp, offset_t off, size_t len,
				int flags, struct cred *cr);
	int	(*vop_map)(struct vnode *vp, offset_t off, struct as *as,
			    caddr_t *addrp, size_t len, uchar_t prot,
			    uchar_t maxprot, uint_t flags, struct cred *cr);
	int	(*vop_addmap)(struct vnode *vp, offset_t off, struct as *as,
				caddr_t addr, size_t len, uchar_t prot,
				uchar_t maxprot, uint_t flags, struct cred *cr);
	int	(*vop_delmap)(struct vnode *vp, offset_t off, struct as *as,
				caddr_t addr, size_t len, uint_t prot,
				uint_t maxprot, uint_t flags, struct cred *cr);
	int	(*vop_poll)(struct vnode *vp, short ev, int any, short *revp,
			struct pollhead **phpp);
	int	(*vop_dump)(struct vnode *vp, caddr_t addr, int lbdn,
				int dblks);
	int	(*vop_pathconf)(struct vnode *vp, int cmd, ulong_t *valp,
				struct cred *cr);
	int	(*vop_pageio)(struct vnode *vp, struct page *pp,
				u_offset_t io_off, size_t io_len, int flags,
				struct cred *cr);
	int	(*vop_dumpctl)(struct vnode *vp, int action, int *blkp);
	void	(*vop_dispose)(struct vnode *vp, struct page *pp, int flag,
				int dn, struct cred *cr);
	int	(*vop_setsecattr)(struct vnode *vp, vsecattr_t *vsap, int flag,
				    struct cred *cr);
	int	(*vop_getsecattr)(struct vnode *vp, vsecattr_t *vsap, int flag,
				    struct cred *cr);
	int	(*vop_shrlock)(struct vnode *vp, int cmd, struct shrlock *shr,
				int flag);
} vnodeops_t;


 

 

 




 

 

 



 
 

 
 
 

 


#ident	"@(#)poll.h	1.28	98/11/23 SMI"


 
typedef struct pollfd {
	int fd;				 
	short events;			 
	short revents;			 
} pollfd_t;

typedef unsigned long	nfds_t;

 


 




int poll(struct pollfd *, nfds_t, int);


 

 
 

 
 
 


#ident	"@(#)strmdep.h	1.10	98/01/06 SMI"


 

 

 

 


 
 

 
 
 

 

 

 

 


#ident	"@(#)model.h	1.20	97/09/22 SMI"



 



 
typedef unsigned int model_t;




 


#ident	"@(#)strft.h	1.2	00/10/23 SMI"

 


 

 
 

 
 
 

 

 


typedef hrtime_t tdelta_t [ 12 ][2];

typedef hrtime_t ts_t;

extern ts_t _tstamp_c;	 
			 










 









 
struct ftevnt {
	ts_t ts;	 
	char *mid;	 
	ushort_t evnt;	 
	ushort_t data;	 
};

 
struct ftblk {
	struct ftblk *nxt;	 
	int ix;			 
	struct ftevnt ev[ 0x9 ];
};

 
struct fthdr {
	struct ftblk *tail;
	unsigned hash;		 
	void *thread;
	int cpu_seqid;
	struct ftblk first;
};

 
struct ftflwe {
	char *mid;
	ushort_t evnt;
	ushort_t data;
	tdelta_t td;
};

struct ftflw {
	struct ftflw *nxt;
	int hash;
	int count;
	struct ftflwe ev[1];	 
};

typedef struct ftevnt ftevnt_t;
typedef struct ftblk ftblk_t;
typedef struct fthdr fthdr_t;
typedef struct ftflwe ftflwe_t;
typedef struct ftflw ftflw_t;
typedef struct ftevents ftevents_t;





 

 
typedef struct	queue {
	struct	qinit	*q_qinfo;	 
	struct	msgb	*q_first;	 
	struct	msgb	*q_last;	 
	struct	queue	*q_next;	 
	struct	queue	*q_link;	 
	void		*q_ptr;		 
	size_t		q_count;	 
	uint_t		q_flag;		 
	ssize_t		q_minpsz;	 
					 
	ssize_t		q_maxpsz;	 
					 
	size_t		q_hiwat;	 
	size_t		q_lowat;	 
	struct qband	*q_bandp;	 
	kmutex_t	q_lock;		 
	struct stdata 	*q_stream;	 
	struct	syncq	*q_syncq;	 
	unsigned char	q_nband;	 
	kcondvar_t	q_wait;		 
	kcondvar_t	q_sync;		 
	struct	queue	*q_nfsrv;	 
	struct	queue	*q_nbsrv;	 
	ushort_t	q_draining;	 
	short		q_struiot;	 
	uint_t		q_syncqmsgs;	 
	size_t		q_mblkcnt;	 
	 
	struct msgb	*q_sqhead;	 
	struct msgb	*q_sqtail;	 
	uint_t		q_sqflags;	 
	size_t		q_refcnt;	 
					 
	 
	struct queue	*q_sqnext;	 
	struct queue	*q_sqprev;	 
	pri_t		q_spri;		 
} queue_t;

 
					 
					 
					 

 
					 
					 
					 

 
typedef struct qband {
	struct qband	*qb_next;	 
	size_t		qb_count;	 
	struct msgb	*qb_first;	 
	struct msgb	*qb_last;	 
	size_t		qb_hiwat;	 
	size_t		qb_lowat;	 
	uint_t		qb_flag;	 
	size_t		qb_mblkcnt;	 
} qband_t;

 

 

 
typedef enum qfields {
	QHIWAT	= 0,		 
	QLOWAT	= 1,		 
	QMAXPSZ	= 2,		 
	QMINPSZ	= 3,		 
	QCOUNT	= 4,		 
	QFIRST	= 5,		 
	QLAST	= 6,		 
	QFLAG	= 7,		 
	QSTRUIOT = 8,		 
	QBAD	= 9
} qfields_t;

 
struct module_info {
	ushort_t mi_idnum;		 
	char 	*mi_idname;		 
	ssize_t	mi_minpsz;		 
	ssize_t	mi_maxpsz;		 
	size_t	mi_hiwat;		 
	size_t 	mi_lowat;		 
};

 
struct	qinit {
	int	(*qi_putp)();		 
	int	(*qi_srvp)();		 
	int	(*qi_qopen)();		 
	int	(*qi_qclose)();		 
	int	(*qi_qadmin)();		 
	struct module_info *qi_minfo;	 
	struct module_stat *qi_mstat;	 
	int	(*qi_rwp)();		 
	int	(*qi_infop)();		 
	int	qi_struiot;		 
};

 

 

struct streamtab {
	struct qinit *st_rdinit;
	struct qinit *st_wrinit;
	struct qinit *st_muxrinit;
	struct qinit *st_muxwinit;
};

 

struct linkblk {
	queue_t *l_qtop;	 
				 
	queue_t *l_qbot;	 
	int	l_index;	 
};

 
typedef struct free_rtn {
	void	(*free_func)();
	caddr_t	free_arg;
} frtn_t;

 


typedef struct datab {
	frtn_t		*db_frtnp;
	unsigned char	*db_base;
	unsigned char	*db_lim;
	unsigned char	db_ref;
	unsigned char	db_type;
	unsigned char	db_flags;
	unsigned char	db_struioflag;
	void		*db_cache;	 
	struct msgb	*db_mblk;
	void		(*db_free)(struct msgb *, struct datab *);
	void		(*db_lastfree)(struct msgb *, struct datab *);
	void		*db_pad;	 
	unsigned char	*db_struiobase;
	unsigned char	*db_struiolim;
	unsigned char	*db_struioptr;
	union {
		double enforce_alignment;
		unsigned char data[8];
		unsigned u16:16;	 
		 
	} db_struioun;
	fthdr_t		*db_fthdr;
	ftflw_t		***db_ftflw;
	uid_t		db_uid;		 
	 
	uid_t		db_lid;
} dblk_t;

 
typedef struct	msgb {
	struct	msgb	*b_next;
	struct  msgb	*b_prev;
	struct	msgb	*b_cont;
	unsigned char	*b_rptr;
	unsigned char	*b_wptr;
	struct datab 	*b_datap;
	unsigned char	b_band;
	unsigned char	b_ftflag;	 
	unsigned short	b_flag;
	queue_t		*b_queue;	 
} mblk_t;

 

typedef	struct	bcache {
	kmutex_t		mutex;
	struct kmem_cache	*buffer_cache;
	struct kmem_cache	*dblk_cache;
	int			alloc;
	int			destroy;
	size_t			size;
	uint_t			align;
} bcache_t;

 

 

 
					 

 

 

 

 

 

 
struct iocblk {
	int 	ioc_cmd;		 
	cred_t	*ioc_cr;		 
	uint_t	ioc_id;			 
	size_t	ioc_count;		 
	int	ioc_error;		 
	int	ioc_rval;		 
	int	ioc_fill1;
	uint_t	ioc_flag;		 
	int	ioc_filler[2];		 
};

typedef	struct iocblk	*IOCP;


 


 

 
struct copyreq {
	int	cq_cmd;			 
	cred_t	*cq_cr;			 
	uint_t	cq_id;			 
	caddr_t	cq_addr;		 
	size_t	cq_size;		 
	uint_t	cq_flag;		 
	mblk_t	*cq_private;		 
	int	cq_filler[4];		 
};


 

					 
					 
					 

 
struct copyresp {
	int	cp_cmd;			 
	cred_t	*cp_cr;			 
	uint_t	cp_id;			 
	caddr_t	cp_rval;		 
					 
	size_t	cp_pad1;
	uint_t	cp_pad2;
	mblk_t *cp_private;		 
	uint_t	cp_flag;		 
	int	cp_filler[3];
};


 
union ioctypes {
	struct iocblk	iocblk;
	struct copyreq	copyreq;
	struct copyresp	copyresp;
};

 

struct stroptions {
	uint_t	so_flags;		 
	short	so_readopt;		 
	ushort_t so_wroff;		 
	ssize_t	so_minpsz;		 
	ssize_t	so_maxpsz;		 
	size_t	so_hiwat;		 
	size_t	so_lowat;		 
	unsigned char so_band;		 
	ushort_t so_erropt;		 
	ssize_t	so_maxblk;		 
	ushort_t so_copyopt;		 
};

 



 

 

 

 
					 

 

 

 

 

 

 

 

 

 

 

 

 

 

 


 
extern int nstrpush;			 


 
 
 

 
 
 


#ident	"@(#)byteorder.h	1.14	98/04/19 SMI"

 



 

 

 




typedef	uint16_t	in_port_t;

 
typedef	uint32_t	in_addr_t;

typedef uint32_t ipaddr_t;


struct in6_addr {
	union {
		 
		uint8_t		_S6_u8[16];	 
		uint32_t	_S6_u32[4];	 
		uint32_t	__S6_align;	 
	} _S6_un;
};


typedef struct in6_addr in6_addr_t;


typedef	unsigned short	sa_family_t;

 


 

 

 

 

 

 

 


struct in_addr {
	union {
		struct { uint8_t s_b1, s_b2, s_b3, s_b4; }  S_un_b ;
		struct { uint16_t s_w1, s_w2; }  S_un_w ;
		uint32_t  S_addr ;
	}  S_un ;
};

 








 

 
struct sockaddr_in {
	sa_family_t	sin_family;
	in_port_t	sin_port;
	struct	in_addr sin_addr;
	char		sin_zero[8];
};

 
struct sockaddr_in6 {
	sa_family_t	sin6_family;
	in_port_t	sin6_port;
	uint32_t	sin6_flowinfo;
	struct in6_addr	sin6_addr;
	uint32_t	sin6_scope_id;   
	uint32_t	__sin6_src_id;	 
};

 

 


 




 

 







 

 


 







 

 

 

 



 





 
 

typedef struct ipsec_req {
	uint_t 		ipsr_ah_req;		 
	uint_t 		ipsr_esp_req;		 
	uint_t		ipsr_self_encap_req;	 
	uint8_t		ipsr_auth_alg;		 
	uint8_t		ipsr_esp_alg;		 
	uint8_t		ipsr_esp_auth_alg;	 
} ipsec_req_t;

 

 



 

 

 
struct ip_mreq {
	struct in_addr	imr_multiaddr;	 
	struct in_addr	imr_interface;	 
};

 
struct ipv6_mreq {
	struct in6_addr	ipv6mr_multiaddr;	 
	unsigned int	ipv6mr_interface;	 
};

 
struct in6_pktinfo {
	struct in6_addr		ipi6_addr;	 
	unsigned int		ipi6_ifindex;	 
};

 


 
typedef struct {
	struct in_addr	in_prefix_addr;
	unsigned int	in_prefix_len;
} in_prefix_t;

 
					 
					 
					 
					 
					 
					 
					 
					 
					 
					 
 
					 
					 

					 
					 
					 


 
					 
					 

 
					 
					 


 
extern const struct in6_addr in6addr_any;
extern const struct in6_addr in6addr_loopback;


 

 
 

 
 
 


#ident	"@(#)socket.h	1.53	99/11/07 SMI"

 
 
 

 
 
 

 

 
 

 
 
 

 

 

 
 
 

 
 

 
 
 


#ident	"@(#)netconfig.h	1.20	99/04/27 SMI"



struct  netconfig {
	char		*nc_netid;	 
	unsigned int	nc_semantics;	 
	unsigned int	nc_flag;	 
	char		*nc_protofmly;	 
	char		*nc_proto;	 
	char		*nc_device;	 
	unsigned int	nc_nlookups;	 
	char		**nc_lookups;	 
	unsigned int	nc_unused[8];	 
};

typedef struct {
	struct netconfig **nc_head;
	struct netconfig **nc_curr;
} NCONF_HANDLE;

 


 


 


 



extern void		*setnetconfig(void);
extern int		endnetconfig(void *);
extern struct netconfig	*getnetconfig(void *);
extern struct netconfig	*getnetconfigent(char *);
extern void		freenetconfigent(struct netconfig *);
extern void		*setnetpath(void);
extern int		endnetpath(void *);
extern struct netconfig *getnetpath(void *);
extern void		nc_perror(const char *);
extern char		*nc_sperror(void);







typedef	uint32_t	socklen_t;

typedef	void		*Psocklen_t;


 

 

 

 

 

 


 
struct	linger {
	int	l_onoff;		 
	int	l_linger;		 
};

 

 


 
struct sockaddr {
	sa_family_t	sa_family;	 
	char		sa_data[14];	 
};

 
 

 

 
 

 
 
 

 


#ident	"@(#)un.h	1.9	96/07/12 SMI"



 
struct	sockaddr_un {
	sa_family_t	sun_family;		 
	char		sun_path[108];		 
};



 
 


#ident	"@(#)if_dl.h	1.7	98/01/06 SMI"
 


 

 
struct sockaddr_dl {
	ushort_t sdl_family;	 
	ushort_t sdl_index;	 
	uchar_t	sdl_type;	 
	uchar_t	sdl_nlen;	 
	uchar_t	sdl_alen;	 
	uchar_t	sdl_slen;	 
	char	sdl_data[244];	 
};




 

 

 
typedef	double		sockaddr_maxalign_t;


 

struct sockaddr_storage {
	sa_family_t	ss_family;	 
	 
	char		_ss_pad1[ ( ( sizeof ( sockaddr_maxalign_t ) ) - sizeof ( sa_family_t ) )];
	sockaddr_maxalign_t _ss_align;
	char		_ss_pad2[ ( 256 - ( sizeof ( sa_family_t ) + ( ( sizeof ( sockaddr_maxalign_t ) ) - sizeof ( sa_family_t ) ) + ( sizeof ( sockaddr_maxalign_t ) ) ) )];
};

 


 

 
struct msghdr {
	void		*msg_name;		 
	socklen_t	msg_namelen;		 
	struct iovec	*msg_iov;		 
	int		msg_iovlen;		 

	caddr_t		msg_accrights;	 
	int		msg_accrightslen;
};


 
 


 

struct cmsghdr {
	socklen_t	cmsg_len;	 
	int		cmsg_level;	 
	int		cmsg_type;	 
};





extern int accept(int, struct sockaddr *, Psocklen_t);
extern int bind(int, const struct sockaddr *, socklen_t);
extern int connect(int, const struct sockaddr *, socklen_t);
extern int getpeername(int, struct sockaddr *, Psocklen_t);
extern int getsockname(int, struct sockaddr *, Psocklen_t);
extern int getsockopt(int, int, int, void *, Psocklen_t);
extern int listen(int, int);	 
extern int socketpair(int, int, int, int *);
extern ssize_t recv(int, void *, size_t, int);
extern ssize_t recvfrom(int, void *, size_t, int, struct sockaddr *,
	Psocklen_t);
extern ssize_t recvmsg(int, struct msghdr *, int);
extern ssize_t send(int, const void *, size_t, int);
extern ssize_t sendmsg(int, const struct msghdr *, int);
extern ssize_t sendto(int, const void *, size_t, int, const struct sockaddr *,
	socklen_t);
extern int setsockopt(int, int, int, const void *, socklen_t);
extern int shutdown(int, int);
extern int socket(int, int, int);


 

 
 
 




struct	hostent {
	char	*h_name;	 
	char	**h_aliases;	 
	int	h_addrtype;	 
	int	h_length;	 
	char	**h_addr_list;	 
};


 

struct addrinfo {
	int ai_flags;		 
	int ai_family;		 
	int ai_socktype;	 
	int ai_protocol;	 
	size_t ai_addrlen;		 
	char *ai_canonname;	 
	struct sockaddr *ai_addr;	 
	struct addrinfo *ai_next;	 
};
 

 

 

 


 



 
struct	netent {
	char		*n_name;	 
	char		**n_aliases;	 
	int		n_addrtype;	 
	in_addr_t	n_net;		 
};

struct	protoent {
	char	*p_name;	 
	char	**p_aliases;	 
	int	p_proto;	 
};

struct	servent {
	char	*s_name;	 
	char	**s_aliases;	 
	int	s_port;		 
	char	*s_proto;	 
};

struct hostent	*gethostbyname_r
	(const char *, struct hostent *, char *, int, int *h_errnop);
struct hostent	*gethostbyaddr_r
	(const char *, int, int, struct hostent *, char *, int, int *h_errnop);
struct hostent	*getipnodebyname(const char *, int, int, int *);
struct hostent	*getipnodebyaddr(const void *, size_t, int, int *);
void		freehostent(struct hostent *);
struct hostent	*gethostent_r(struct hostent *, char *, int, int *h_errnop);

struct servent	*getservbyname_r
	(const char *name, const char *, struct servent *, char *, int);
struct servent	*getservbyport_r
	(int port, const char *, struct servent *, char *, int);
struct servent	*getservent_r(struct	servent *, char *, int);

struct netent	*getnetbyname_r
	(const char *, struct netent *, char *, int);
struct netent	*getnetbyaddr_r(long, int, struct netent *, char *, int);
struct netent	*getnetent_r(struct netent *, char *, int);

struct protoent	*getprotobyname_r
	(const char *, struct protoent *, char *, int);
struct protoent	*getprotobynumber_r
	(int, struct protoent *, char *, int);
struct protoent	*getprotoent_r(struct protoent *, char *, int);

int getnetgrent_r(char **, char **, char **, char *, int);
int innetgr(const char *, const char *, const char *, const char *);

 
struct hostent	*gethostbyname(const char *);
struct hostent	*gethostent(void);
struct netent *getnetbyaddr(in_addr_t, int);
struct netent	*getnetbyname(const char *);
struct netent	*getnetent(void);
struct protoent	*getprotobyname(const char *);
struct protoent	*getprotobynumber(int);
struct protoent	*getprotoent(void);
struct servent	*getservbyname(const char *, const char *);
struct servent	*getservbyport(int, const char *);
struct servent	*getservent(void);

int		 getnetgrent(char **, char **, char **);
struct hostent	*gethostbyaddr(const char *, int, int);

int endhostent(void);
int endnetent(void);
int endprotoent(void);
int endservent(void);
int sethostent(int);
int setnetent(int);
int setprotoent(int);
int setservent(int);




int setnetgrent(const char *);
int endnetgrent(void);
int rcmd(char **, unsigned short,
	const char *, const char *, const char *, int *);
int rcmd_af(char **, unsigned short,
	const char *, const char *, const char *, int *, int);
int rresvport_af(int *, int);
int rexec(char **, unsigned short,
	const char *, const char *, const char *, int *);
int rexec_af(char **, unsigned short,
	const char *, const char *, const char *, int *, int);
int rresvport(int *);
int ruserok(const char *, int, const char *, const char *);
 
struct hostent	*gethostbyname2(const char *, int);
void		herror(const char *);
const char	*hstrerror(int);
 

 
int		getaddrinfo(const char *, const char *,
			const struct addrinfo *, struct addrinfo **);
void		freeaddrinfo(struct addrinfo *);
char		*gai_strerror(int);
int		getnameinfo(const struct sockaddr *, socklen_t,
			char *, size_t, char *, size_t, int);
 



 

extern  int h_errno;



 
 




 
 

 

 
 

 
 
 

 



#ident	"@(#)signal.h	1.38	99/08/10 SMI"

 

 
 
 


 
 

 
 
 

 


 
 

 
 
 

 

 


#ident	"@(#)signal_iso.h	1.1	99/08/09 SMI"
 

 

 
 

 
 
 

 


#ident	"@(#)signal_iso.h	1.1	99/08/09 SMI"

 

 

 




 
extern long _sysconf(int);	 










typedef int	sig_atomic_t;


extern void (*signal(int, void (*)(int)))(int);
extern int raise(int);




 

 
 

 
 
 


#ident	"@(#)signal.h	1.54	99/07/26 SMI"

 

 
 
 

 

 
 

 
 
 

 




 
 
 

 

 
 
 


#ident	"@(#)siginfo.h	1.54	98/03/27 SMI"

 

 
 
 

 
 

 
 
 

 




 




 

 



 



 

 
 

 
 
 

 


#ident	"@(#)machsig.h	1.15	99/08/15 SMI"

 

 
 
 



 

 


 



 


 


 


 





 


 



 




 





 
 

 


 
 
 

 
 
 

 


typedef struct siginfo { 		 
	int	si_signo;			 
	int 	si_code;			 
	int	si_errno;			 
	union {

		int	__pad[ ( ( 128 / sizeof ( int ) ) - 3 )];		 

		struct {			 
			pid_t	__pid;		 
			union {
				struct {
					uid_t	__uid;
					union sigval	__value;
				} __kill;
				struct {
					clock_t __utime;
					int	__status;
					clock_t __stime;
				} __cld;
			} __pdata;
		} __proc;

		struct {	 
			void 	*__addr;	 
			int	__trapno;	 
			caddr_t	__pc;		 
		} __fault;

		struct {			 
		 
			int	__fd;		 
			long	__band;
		} __file;

		struct {			 
			caddr_t	__faddr;	 
			timestruc_t __tstamp;	 
			short	__syscall;	 
			char	__nsysarg;	 
			char	__fault;	 
			long	__sysarg[8];	 
			int	__mstate[10];	 
		} __prof;

	} __data;

} siginfo_t;


 


typedef struct k_siginfo {
	int	si_signo;			 
	int 	si_code;			 
	int	si_errno;			 
	union {
		struct {			 
			pid_t	__pid;		 
			union {
				struct {
					uid_t	__uid;
					union sigval	__value;
				} __kill;
				struct {
					clock_t __utime;
					int	__status;
					clock_t __stime;
				} __cld;
			} __pdata;
		} __proc;

		struct {	 
			void 	*__addr;	 
			int	__trapno;	 
			caddr_t	__pc;		 
		} __fault;

		struct {			 
		 
			int	__fd;		 
			long	__band;
		} __file;

		struct {			 
			caddr_t	__faddr;	 

			timestruc_t __tstamp;	 
			short	__syscall;	 
			char	__nsysarg;	 
			char	__fault;	 
			 
			 
			 
		} __prof;

	} __data;

} k_siginfo_t;

typedef struct sigqueue {
	struct sigqueue	*sq_next;
	k_siginfo_t	sq_info;
	void		(*sq_func)(struct sigqueue *);  
	void		*sq_backptr;	 
					 
} sigqueue_t;

 









 
typedef struct {		 
	unsigned int	__sigbits[4];
} sigset_t;



typedef	struct {
	unsigned int	__sigbits[2];
} k_sigset_t;

 

 
struct sigaction {
	int sa_flags;
	union {
		void (*_handler)();
		void (*_sigaction)(int, siginfo_t *, void *);
	}	_funcptr;
	sigset_t sa_mask;
	int sa_resv[2];
};


 


			 

 



 

 

 
			 
			 
			 




 
typedef struct sigaltstack {
	void	*ss_sp;
	size_t	ss_size;
	int	ss_flags;
} stack_t;





 
typedef struct signotify_id {		 
	pid_t	sn_pid;			 
	int	sn_index;		 
	int	sn_pad;			 
} signotify_id_t;


 



 
struct sigstack {
	void	*ss_sp;
	int	ss_onstack;
};

 




 




extern const char	**_sys_siglistp;	 
extern const int	_sys_siglistn;		 


extern int kill(pid_t, int);
extern int sigaction(int, const struct sigaction *, struct sigaction *);
extern int sigaddset(sigset_t *, int);
extern int sigdelset(sigset_t *, int);
extern int sigemptyset(sigset_t *);
extern int sigfillset(sigset_t *);
extern int sigismember(const sigset_t *, int);
extern int sigpending(sigset_t *);
extern int sigprocmask(int, const sigset_t *, sigset_t *);
extern int sigsuspend(const sigset_t *);

 

 
 

 
 
 


#ident	"@(#)procset.h	1.20	00/02/14 SMI"


 

 
 
 

 
 

 
 
 

 


 


 
typedef enum
	idtype		 
		{
	P_PID,		 
	P_PPID,		 
	P_PGID,		 
			 
	P_SID,		 
	P_CID,		 
	P_UID,		 
	P_GID,		 
	P_ALL,		 
	P_LWPID,	 
	P_TASKID,	 
	P_PROJID	 
} idtype_t;


 
typedef enum idop {
	POP_DIFF,	 
			 
			 
	POP_AND,	 
			 
			 
	POP_OR,		 
			 
			 
	POP_XOR		 
			 
			 
} idop_t;


 
typedef struct procset {
	idop_t		p_op;	 
				 
				 
				 

	idtype_t	p_lidtype;
				 
				 
	id_t		p_lid;	 

	idtype_t	p_ridtype;
				 
				 
	id_t		p_rid;	 
} procset_t;

 




extern int gsignal(int);
extern int (*ssignal(int, int (*)(int)))(int);
extern int sigsend(idtype_t, id_t, int);
extern int sigsendset(const procset_t *, int);
extern int sig2str(int, char *);
extern int str2sig(const char *, int *);

extern void (*bsd_signal(int, void (*)(int)))(int);
extern int killpg(pid_t, int);
extern int siginterrupt(int, int);
extern int sigaltstack(const stack_t *, stack_t *);
extern int sighold(int);
extern int sigignore(int);
extern int sigpause(int);
extern int sigrelse(int);
extern void (*sigset(int, void (*)(int)))(int);
extern int sigstack(struct sigstack *, struct sigstack *);

 
 

 

 
 
 

 
 

 
 
 

 

extern int pthread_kill(pthread_t, int);
extern int pthread_sigmask(int, const sigset_t *, sigset_t *);
extern int sigwaitinfo(const sigset_t *, siginfo_t *);
extern int sigtimedwait(const sigset_t *, siginfo_t *, const struct timespec *);
extern int sigqueue(pid_t, int, const union sigval);


 




extern int sigwait(sigset_t *);






 
 

 
 
 

 


#ident	"@(#)stdarg.h	1.45	99/08/10 SMI"


 
 

 
 
 

 

 


#ident	"@(#)stdarg_iso.h	1.1	99/08/09 SMI"

 


#ident	"@(#)va_list.h	1.12	99/05/04 SMI"

 


typedef void *__va_list;




 

typedef __va_list va_list;


 

 


extern void va_end(__va_list);



 


 





 
 

 
 
 

 

 


#ident	"@(#)stdio.h	1.78	99/12/08 SMI"

 
 

 
 
 

 

 

 


#ident	"@(#)stdio_iso.h	1.2	99/10/25 SMI"
 

 

 
 
 

 

 


#ident	"@(#)stdio_tag.h	1.3	98/04/20 SMI"


typedef struct  __FILE __FILE;


 


#ident	"@(#)stdio_impl.h	1.8	99/06/10 SMI"

 





struct  __FILE	 
{
	ssize_t		_cnt;	 
	unsigned char	*_ptr;	 
	unsigned char	*_base;	 
	unsigned char	_flag;	 
	unsigned char	_file;	 
	unsigned	__orientation:2;  
	unsigned	__ionolock:1;	 
	unsigned	__filler:5;
};




 


 
typedef	long long	__longlong_t;


typedef	__FILE FILE;


typedef	__longlong_t	fpos_t;




 








extern __FILE	__iob[ 20 ];



extern int	remove(const char *);
extern int	rename(const char *, const char *);
extern FILE	*tmpfile(void);
extern char	*tmpnam(char *);
extern int	fclose(FILE *);
extern int	fflush(FILE *);
extern FILE	*fopen(const char *, const char *);
extern FILE	*freopen(const char *, const char *, FILE *);
extern void	setbuf(FILE *, char *);
extern int	setvbuf(FILE *, char *, int, size_t);
 
extern int	fprintf(FILE *, const char *, ...);
 
extern int	fscanf(FILE *, const char *, ...);
 
extern int	printf(const char *, ...);
 
extern int	scanf(const char *, ...);
 
extern int	sprintf(char *, const char *, ...);
 
extern int	sscanf(const char *, const char *, ...);
extern int	vfprintf(FILE *, const char *, __va_list);
extern int	vprintf(const char *, __va_list);
extern int	vsprintf(char *, const char *, __va_list);
extern int	fgetc(FILE *);
extern char	*fgets(char *, int, FILE *);
extern int	fputc(int, FILE *);
extern int	fputs(const char *, FILE *);
extern int	getc(FILE *);
extern int	putc(int, FILE *);
extern int	getchar(void);
extern int	putchar(int);
extern char	*gets(char *);
extern int	puts(const char *);
extern int	ungetc(int, FILE *);
extern size_t	fread(void *, size_t, size_t, FILE *);
extern size_t	fwrite(const void *, size_t, size_t, FILE *);
extern int	fgetpos(FILE *, fpos_t *);
extern int	fseek(FILE *, long, int);
extern int	fsetpos(FILE *, const fpos_t *);
extern long	ftell(FILE *);
extern void	rewind(FILE *);
extern void	clearerr(FILE *);
extern int	feof(FILE *);
extern int	ferror(FILE *);
extern void	perror(const char *);

extern int	__filbuf(FILE *);
extern int	__flsbuf(int, FILE *);








 

 



typedef __longlong_t	fpos64_t;

 





extern unsigned char	 _sibuf[], _sobuf[];

 
#pragma	redefine_extname	fopen	fopen64
#pragma	redefine_extname	freopen	freopen64
#pragma	redefine_extname	tmpfile	tmpfile64
#pragma	redefine_extname	fgetpos	fgetpos64
#pragma	redefine_extname	fsetpos	fsetpos64
#pragma	redefine_extname	fseeko	fseeko64
#pragma	redefine_extname	ftello	ftello64

extern unsigned char	*_bufendtab[];
extern FILE		*_lastbuf;

 


extern char	*tmpnam_r(char *);
extern void setbuffer(FILE *, char *, size_t);
extern int setlinebuf(FILE *);
 
extern int	snprintf(char *, size_t, const char *, ...);
extern int	vsnprintf(char *, size_t, const char *, __va_list);

 

extern FILE	*fdopen(int, const char *);
extern char	*ctermid(char *);
extern int	fileno(FILE *);


 
extern void	flockfile(FILE *);
extern int	ftrylockfile(FILE *);
extern void	funlockfile(FILE *);
extern int	getc_unlocked(FILE *);
extern int	getchar_unlocked(void);
extern int	putc_unlocked(int, FILE *);
extern int	putchar_unlocked(int);


 

extern FILE	*popen(const char *, const char *);
extern char	*cuserid(char *);
extern char	*tempnam(const char *, const char *);
extern int	getopt(int, char *const *, const char *);
extern int	getsubopt(char **, char *const *, char **);
extern char	*optarg;
extern int	optind, opterr, optopt;
extern int	getw(FILE *);
extern int	putw(int, FILE *);
extern int	pclose(FILE *);


 
extern int	fseeko(FILE *, off_t, int);
extern off_t	ftello(FILE *);

 
extern FILE	*fopen64(const char *, const char *);
extern FILE	*freopen64(const char *, const char *, FILE *);
extern FILE	*tmpfile64(void);
extern int	fgetpos64(FILE *, fpos64_t *);
extern int	fsetpos64(FILE *, const fpos64_t *);
extern int	fseeko64(FILE *, off64_t, int);
extern off64_t	ftello64(FILE *);






 

 
 

 
 
 


#ident	"@(#)stdlib.h	1.47	99/11/03 SMI"

 

 
 

 
 
 

 


#ident	"@(#)stdlib_iso.h	1.1	99/08/09 SMI"

 

 
 
 




typedef	struct {
	int	quot;
	int	rem;
} div_t;

typedef struct {
	long	quot;
	long	rem;
} ldiv_t;




typedef long	wchar_t;


extern unsigned char	__ctype[];

extern void abort(void);
extern int abs(int);
extern int atexit(void (*)(void));
extern double atof(const char *);
extern int atoi(const char *);
extern long int atol(const char *);
extern void *bsearch(const void *, const void *, size_t, size_t,
	int (*)(const void *, const void *));
extern void *calloc(size_t, size_t);
extern div_t div(int, int);
extern void exit(int);
extern void free(void *);
extern char *getenv(const char *);
extern long int labs(long);
extern ldiv_t ldiv(long, long);
extern void *malloc(size_t);
extern int mblen(const char *, size_t);
extern size_t mbstowcs(wchar_t *, const char *, size_t);
extern int mbtowc(wchar_t *, const char *, size_t);
extern void qsort(void *, size_t, size_t,
	int (*)(const void *, const void *));
extern int rand(void);
extern void *realloc(void *, size_t);
extern void srand(unsigned int);
extern double strtod(const char *, char **);
extern long int strtol(const char *, char **, int);
extern unsigned long int strtoul(const char *, char **, int);
extern int system(const char *);
extern int wctomb(char *, wchar_t);
extern size_t wcstombs(char *, const wchar_t *, size_t);






 


typedef struct {
	long long	quot;
	long long	rem;
} lldiv_t;



 

#pragma	redefine_extname	mkstemp		mkstemp64


 

extern int rand_r(unsigned int *);

extern void _exithandle(void);

extern double drand48(void);
extern double erand48(unsigned short *);
extern long jrand48(unsigned short *);
extern void lcong48(unsigned short *);
extern long lrand48(void);
extern long mrand48(void);
extern long nrand48(unsigned short *);
extern unsigned short *seed48(unsigned short *);
extern void srand48(long);
extern int putenv(char *);
extern void setkey(const char *);




extern void swab(const char *, char *, ssize_t);

extern int	mkstemp(char *);

extern int	mkstemp64(char *);

extern long a64l(const char *);
extern char *ecvt(double, int, int *, int *);
extern char *fcvt(double, int, int *, int *);
extern char *gcvt(double, int, char *);
extern int getsubopt(char **, char *const *, char **);
extern int  grantpt(int);
extern char *initstate(unsigned, char *, size_t);
extern char *l64a(long);
extern char *mktemp(char *);
extern char *ptsname(int);
extern long random(void);
extern char *realpath(const char *, char *);
extern char *setstate(const char *);
extern void srandom(unsigned);
extern int ttyslot(void);
extern int  unlockpt(int);
extern void *valloc(size_t);

extern int dup2(int, int);
extern char *qecvt(long double, int, int *, int *);
extern char *qfcvt(long double, int, int *, int *);
extern char *qgcvt(long double, int, char *);
extern char *getcwd(char *, size_t);
extern const char *getexecname(void);
extern char *getlogin(void);
extern int getopt(int, char *const *, const char *);
extern char *optarg;
extern int optind, opterr, optopt;
extern char *getpass(const char *);
extern char *getpassphrase(const char *);
extern int getpw(uid_t, char *);
extern int isatty(int);
extern void *memalign(size_t, size_t);
extern char *ttyname(int);

extern long long atoll(const char *);
extern long long llabs(long long);
extern lldiv_t lldiv(long long, long long);
extern char *lltostr(long long, char *);
extern long long strtoll(const char *, char **, int);
extern unsigned long long strtoull(const char *, char **, int);
extern char *ulltostr(unsigned long long, char *);




 
 

 
 
 

 


#ident	"@(#)string.h	1.24	99/08/10 SMI"

 
 

 
 
 

 

 


#ident	"@(#)string_iso.h	1.2	99/11/09 SMI"
 

 

 
 
 







extern int memcmp(const void *, const void *, size_t);
extern void *memcpy(void *, const void *, size_t);
extern void *memmove(void *, const void *, size_t);
extern void *memset(void *, int, size_t);
extern char *strcat(char *, const char *);
extern int strcmp(const char *, const char *);
extern char *strcpy(char *, const char *);
extern int strcoll(const char *, const char *);
extern size_t strcspn(const char *, const char *);
extern char *strerror(int);
extern size_t strlen(const char *);
extern char *strncat(char *, const char *, size_t);
extern int strncmp(const char *, const char *, size_t);
extern char *strncpy(char *, const char *, size_t);
extern size_t strspn(const char *, const char *);
extern char *strtok(char *, const char *);
extern size_t strxfrm(char *, const char *, size_t);
extern void *memchr(const void *, int, size_t);
extern char *strchr(const char *, int);
extern char *strpbrk(const char *, const char *);
extern char *strrchr(const char *, int);
extern char *strstr(const char *, const char *);





 



extern char *strtok_r(char *, const char *, char **);

extern void *memccpy(void *, const void *, int, size_t);

extern char *strsignal(int);
extern int ffs(int);
extern int strcasecmp(const char *, const char *);
extern int strncasecmp(const char *, const char *, size_t);
extern size_t strlcpy(char *, const char *, size_t);
extern size_t strlcat(char *, const char *, size_t);

extern char *strdup(const char *);



 
 

 
 
 


#ident	"@(#)syslog.h	1.11	96/05/17 SMI"

 

 
 
 

 
 

 
 
 

 


#ident	"@(#)syslog.h	1.10	92/07/14 SMI"


 
	 


 


 

 


 




void openlog(const char *, int, int);
void syslog(int, const char *, ...);
void closelog(void);
int setlogmask(int);
void vsyslog(int, const char *, __va_list);



 

 
 

 
 
 

 

 
 

 
 
 

 

 

 
 

 
 
 

 

 

 
 

 
 
 

 
 

 
 
 

 

 
 

 
 
 

 


#ident	"@(#)unistd.h	1.58	99/11/11 SMI"

 

 
 
 


 
 

 
 
 

 

 

 

 



 


 




 

 
 

 




 

 
#pragma	redefine_extname	ftruncate	ftruncate64
#pragma	redefine_extname	lseek		lseek64
#pragma	redefine_extname	pread		pread64
#pragma	redefine_extname	pwrite		pwrite64
#pragma	redefine_extname	truncate	truncate64
#pragma	redefine_extname	lockf		lockf64
#pragma	redefine_extname	tell		tell64

 


extern int access(const char *, int);
extern int acct(const char *);
extern unsigned alarm(unsigned);
extern int brk(void *);
extern int chdir(const char *);
extern int chown(const char *, uid_t, gid_t);
extern int chroot(const char *);
extern int close(int);
extern char *ctermid(char *);
extern char *ctermid_r(char *);
extern char *cuserid(char *);
extern int dup(int);
extern int dup2(int, int);
extern int execl(const char *, const char *, ...);
extern int execle(const char *, const char *, ...);
extern int execlp(const char *, const char *, ...);
extern int execv(const char *, char *const *);
extern int execve(const char *, char *const *, char *const *);
extern int execvp(const char *, char *const *);
extern void _exit(int);
 
extern int fattach(int, const char *);
extern int fchdir(int);
extern int fchown(int, uid_t, gid_t);
extern int fchroot(int);
extern int fdatasync(int);
 
extern int fdetach(const char *);
extern pid_t fork(void);
extern pid_t fork1(void);
extern long fpathconf(int, int);
extern int fsync(int);
extern int ftruncate(int, off_t);
extern char *getcwd(char *, size_t);
extern int getdtablesize(void);
extern gid_t getegid(void);
extern uid_t geteuid(void);
extern gid_t getgid(void);
extern int getgroups(int, gid_t *);
extern long gethostid(void);
extern int gethostname(char *, int);
extern char *getlogin(void);
extern int getpagesize(void);
extern pid_t getpgid(pid_t);
extern pid_t getpid(void);
extern pid_t getppid(void);
extern pid_t getpgrp(void);
char *gettxt(const char *, const char *);
extern pid_t getsid(pid_t);
extern uid_t getuid(void);
extern char *getwd(char *);
 
extern int ioctl(int, int, ...);
extern int isaexec(const char *, char *const *, char *const *);
extern int isatty(int);
extern int link(const char *, const char *);
extern int lchown(const char *, uid_t, gid_t);
extern int lockf(int, int, off_t);
extern int readlink(const char *, char *, size_t);
extern off_t lseek(int, off_t, int);
extern int nice(int);
extern long pathconf(const char *, int);
extern int pause(void);
extern int pipe(int *);
extern offset_t llseek(int, offset_t, int);
extern off_t tell(int);
extern int mincore(caddr_t, size_t, char *);
extern ssize_t pread(int, void *, size_t, off_t);
extern void profil(unsigned short *, size_t, unsigned long, unsigned int);
extern int pthread_atfork(void (*) (void), void (*) (void), void (*) (void));
extern long ptrace(int, pid_t, long, long);
extern ssize_t pwrite(int, const void *, size_t, off_t);
extern ssize_t read(int, void *, size_t);
extern int rename(const char *, const char *);
extern int resolvepath(const char *, char *, size_t);
extern int rmdir(const char *);
extern void *sbrk(intptr_t);
extern int setgid(gid_t);
extern int setegid(gid_t);
extern int setgroups(int, const gid_t *);
extern int setpgid(pid_t, pid_t);
extern pid_t setpgrp(void);
extern int setregid(gid_t, gid_t);
extern int setreuid(uid_t, uid_t);
extern pid_t setsid(void);
extern int setuid(uid_t);
extern int seteuid(uid_t);
extern unsigned sleep(unsigned);
extern int stime(const time_t *);
extern int symlink(const char *, const char *);
extern void sync(void);
extern long sysconf(int);
extern pid_t tcgetpgrp(int);
extern int tcsetpgrp(int, pid_t);
extern off_t tell(int);
extern int truncate(const char *, off_t);
extern char *ttyname(int);
extern useconds_t ualarm(useconds_t, useconds_t);
extern int unlink(const char *);
extern int usleep(useconds_t);
extern pid_t vfork(void);
extern void vhangup(void);
extern ssize_t write(int, const void *, size_t);
extern void yield(void);

 
extern int ftruncate64(int, off64_t);
extern off64_t lseek64(int, off64_t, int);
extern ssize_t	pread64(int, void *, size_t, off64_t);
extern ssize_t	pwrite64(int, const void *, size_t, off64_t);
extern off64_t	tell64(int);
extern int	truncate64(const char *, off64_t);
extern int	lockf64(int, int, off64_t);


 
#pragma	unknown_control_flow(vfork)

 

 




extern char *getlogin_r(char *, int);
extern char *ttyname_r(int, char *, int);






 
 

  
 

 
 

 
 

 
 

 

 

 


#ident	"@(#)pthread.h	1.28	99/11/15 SMI"

 
 

 
 
 

 

 
 

 
 
 

 

 
 

 
 
 


#ident	"@(#)sched.h	1.9	98/01/16 SMI"

 
 

 
 
 

 

 
 

 
 
 

 



struct sched_param {
	int	sched_priority;	 
	int	sched_nicelim;	 
	int	sched_nice;	 
	int	sched_pad[6];	 
				 
				 
				 
};

 

 
int	sched_getparam(pid_t pid, struct sched_param *param);
int	sched_setparam(pid_t pid, const struct sched_param *param);
int	sched_getscheduler(pid_t pid);
int	sched_setscheduler(pid_t pid, int policy,
		const struct sched_param *param);
int	sched_yield(void);
int	sched_get_priority_max(int policy);
int	sched_get_priority_min(int policy);
int	sched_rr_get_interval(pid_t pid, struct timespec *interval);




 
 
 

 
 

 

 

 

 

 
 



 

 


 
typedef struct _cleanup {
	uintptr_t	pthread_cleanup_pad[4];
} _cleanup_t;


void	__pthread_cleanup_push(void (*routine)(void *), void *args,
					caddr_t fp, _cleanup_t *info);
void	__pthread_cleanup_pop(int ex, _cleanup_t *info);
caddr_t	_getfp(void);





 

extern int pthread_attr_init(pthread_attr_t *);
extern int pthread_attr_destroy(pthread_attr_t *);
extern int pthread_attr_setstacksize(pthread_attr_t *, size_t);
extern int pthread_attr_getstacksize(const pthread_attr_t *, size_t *);
extern int pthread_attr_setstackaddr(pthread_attr_t *, void *);
extern int pthread_attr_getstackaddr(const pthread_attr_t *, void **);
extern int pthread_attr_setdetachstate(pthread_attr_t *, int);
extern int pthread_attr_getdetachstate(const pthread_attr_t *, int *);
extern int pthread_attr_setscope(pthread_attr_t *, int);
extern int pthread_attr_getscope(const pthread_attr_t *, int *);
extern int pthread_attr_setinheritsched(pthread_attr_t *, int);
extern int pthread_attr_getinheritsched(const pthread_attr_t *, int *);
extern int pthread_attr_setschedpolicy(pthread_attr_t *, int);
extern int pthread_attr_getschedpolicy(const pthread_attr_t *, int *);
extern int pthread_attr_setschedparam(pthread_attr_t *,
					const struct sched_param *);
extern int pthread_attr_getschedparam(const pthread_attr_t *,
					struct sched_param *);
extern int pthread_create(pthread_t *, const pthread_attr_t *,
				void * (*)(void *), void *);
extern int pthread_once(pthread_once_t *, void (*)(void));
extern int pthread_join(pthread_t, void **);
extern int pthread_detach(pthread_t);
extern void pthread_exit(void *);
extern int pthread_cancel(pthread_t);
extern int pthread_setschedparam(pthread_t, int, const struct sched_param *);
extern int pthread_getschedparam(pthread_t, int *, struct sched_param *);
extern int pthread_setcancelstate(int, int *);
extern int pthread_setcanceltype(int, int *);
extern void pthread_testcancel(void);
extern int pthread_equal(pthread_t, pthread_t);
extern int pthread_key_create(pthread_key_t *, void (*)(void *));
extern int pthread_key_delete(pthread_key_t);
extern int pthread_setspecific(pthread_key_t, const void *);
extern void *pthread_getspecific(pthread_key_t);
extern pthread_t pthread_self(void);

 
extern int pthread_mutexattr_init(pthread_mutexattr_t *);
extern int pthread_mutexattr_destroy(pthread_mutexattr_t *);
extern int pthread_mutexattr_setpshared(pthread_mutexattr_t *, int);
extern int pthread_mutexattr_getpshared(const pthread_mutexattr_t *, int *);
extern int pthread_mutexattr_setprotocol(pthread_mutexattr_t *, int);
extern int pthread_mutexattr_getprotocol(const pthread_mutexattr_t *, int *);
extern int pthread_mutexattr_setprioceiling(pthread_mutexattr_t *, int);
extern int pthread_mutexattr_getprioceiling(const pthread_mutexattr_t *, int *);
extern int pthread_mutexattr_setrobust_np(pthread_mutexattr_t *, int);
extern int pthread_mutexattr_getrobust_np(const pthread_mutexattr_t *, int *);
extern int pthread_mutex_init(pthread_mutex_t *, const pthread_mutexattr_t *);
extern int pthread_mutex_consistent_np(pthread_mutex_t *);
extern int pthread_mutex_destroy(pthread_mutex_t *);
extern int pthread_mutex_lock(pthread_mutex_t *);
extern int pthread_mutex_unlock(pthread_mutex_t *);
extern int pthread_mutex_trylock(pthread_mutex_t *);
extern int pthread_mutex_setprioceiling(pthread_mutex_t *, int, int *);
extern int pthread_mutex_getprioceiling(const pthread_mutex_t *, int *);
extern int pthread_condattr_init(pthread_condattr_t *);
extern int pthread_condattr_destroy(pthread_condattr_t *);
extern int pthread_condattr_setpshared(pthread_condattr_t *, int);
extern int pthread_condattr_getpshared(const pthread_condattr_t *, int *);
extern int pthread_cond_init(pthread_cond_t *, const pthread_condattr_t *);
extern int pthread_cond_destroy(pthread_cond_t *);
extern int pthread_cond_broadcast(pthread_cond_t *);
extern int pthread_cond_signal(pthread_cond_t *);
extern int pthread_cond_wait(pthread_cond_t *, pthread_mutex_t *);
extern int pthread_cond_timedwait(pthread_cond_t *, pthread_mutex_t *,
					const struct timespec *);
extern int pthread_attr_getguardsize(const pthread_attr_t *, size_t *);
extern int pthread_attr_setguardsize(pthread_attr_t *, size_t);
extern int pthread_getconcurrency(void);
extern int pthread_setconcurrency(int newval);
extern int pthread_mutexattr_settype(pthread_mutexattr_t *, int);
extern int pthread_mutexattr_gettype(const pthread_mutexattr_t *, int *);
extern int pthread_rwlock_init(pthread_rwlock_t *,
				const pthread_rwlockattr_t *);
extern int pthread_rwlock_destroy(pthread_rwlock_t *);
extern int pthread_rwlock_rdlock(pthread_rwlock_t *);
extern int pthread_rwlock_tryrdlock(pthread_rwlock_t *);
extern int pthread_rwlock_wrlock(pthread_rwlock_t *);
extern int pthread_rwlock_trywrlock(pthread_rwlock_t *);
extern int pthread_rwlock_unlock(pthread_rwlock_t *);
extern int pthread_rwlockattr_init(pthread_rwlockattr_t *);
extern int pthread_rwlockattr_destroy(pthread_rwlockattr_t *);
extern int pthread_rwlockattr_getpshared(const pthread_rwlockattr_t *, int *);
extern int pthread_rwlockattr_setpshared(pthread_rwlockattr_t *, int);







 

 
 

 
 
 

 

 


 



 

 

 
   typedef enum 
   { 
      NL_ENV = 1,
      NL_MEM = 8,
      NL_APPEND = 16 
   } 
   NL_openflag_t;

 

 



 
enum { NL_ON=1, NL_OFF=0, NL_NOTSET=2 };

 

 
typedef struct _nlbin {
    int size;   
    int id;     
	 
    int seconds;
    int frac_secs;
    int precision;
    int accuracy;
    char *ascii_data;   
} NL_binary;


typedef struct _loginfo {
   int enabled;
   int loglvl;  
   int method;
   int binary;
    FILE *log_fp;  
   int   log_fd;
   int doNotClose;  
   int log_socket;
   int failed;  
   char *pname;
   char *hname;
   char *filename;
   char *hostname;
   int curr_buffer_size;
   char *event_buffer;    
   char *message_buffer;    
   char *last_date;   
   char *host_prog;    
   long last_tv_sec;

    char *tag;

   pthread_mutex_t *mtx;
} NL_Handle;

 
    NL_Handle * NetLoggerOpen( char *program_name, char *url, int flags );
    NL_Handle * NetLoggerOpenFd (char *p_name, int fd, int flags);
     
     
    int NetLoggerWrite(NL_Handle * lp,
		       char *keyword,
		       char *const_fmt,
		       char *variable_fmt, 
		       ... );
     
    int NetLoggerGTWrite(NL_Handle *lp, 
			 long sec,
			 long usec,
			 char *keyword, 
			 char *const_fmt,
			 char *variable_fmt,
			 ...);
     
    int NetLoggerWriteLvl( NL_Handle * lp, 
			   int lvl, 
			   char *keyword, 
			   char *const_fmt,
			   char *variable_fmt, 
			   ... );
     
    int NetLoggerGTWriteLvl( NL_Handle * lp, 
			     long sec,
			     long usec,
			     int lvl, 
			     char *keyword, 
			     char *const_fmt,
			     char *variable_fmt, 
			     ... );
     
    int NetLoggerWriteString (NL_Handle * lp,
			      char *keyword, 
			      char *const_data,
			      char *variable_data);
     
    int NetLoggerWriteStringLvl (NL_Handle * lp,
				 int lvl,
				 char *keyword, 
				 char *const_data,
				 char *variable_data);
     
    int NetLoggerGTWriteString(NL_Handle * lp, 
			       long sec, 
			       long usec,			       
			       char *keyword, 
			       char *const_data,
			       char *variable_data);
     
    int NetLoggerGTWriteStringLvl(NL_Handle * lp,
				  int lvl,
				  long sec, long usec,
				  char *keyword,
				  char *const_data,
				  char *variable_data);
    int       NetLoggerFastWrite(NL_Handle *lp, char *keyword, char *fmt,...);
    int       NetLoggerFlush(NL_Handle *lp);
    int       NetLoggerClose(NL_Handle *lp);
    int       NetLoggerSetGlobalTag(char *tag);
    int       NetLoggerSetTag(NL_Handle *lp, char *tag );
    void      NetLoggerSetLevel(NL_Handle *lp, int log_level);

 
FILE  *LogOpen(char *logfile_name, char *mode);
int   SysLogOpen(char *prog_name, int syslog_facility);
int   NetLogOpen(char *hostname, int port);
void NetLogSetMethod( char *s,  
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

 

 

 

 

 





   

  
 

 
 
 


 
static char *printStandardFieldsToBuffer (struct timeval *tvPtr,
					  NL_Handle * lp, char *keyword);
static int writeBuffer (NL_Handle * lp, char *outbuf, int buflen);

char     *globtag =  0 ;	 

 
char     *nl_hname =  0 ;
char     *nl_pname =  0 ;
static int nl_writen (int, char *, int);
static void nl_status (char *, ...);



pthread_mutex_t global_mutex =  { { 0 , 0 , 0 , 0 } , { { { 0 } } } , 0 };


 
char * lookup_host( )
{
   char *hn;
   struct hostent *hp =  0 ;
   char shn[  256 ];

   hn = (char *) malloc ( 256 );

    
   if (gethostname (shn,  256 ) < 0)
   {
      perror ("gethostname");
      strcpy (shn,  "127.0.0.1" );
   }
    
   if ((hp = gethostbyname (shn)) ==  0 )
   {
      perror ("gethostbyname");
      strcpy (hn,  "127.0.0.1" );
   }
   else
   {
       
       
      strncpy (hn, hp->h_name,  256 );	
   }
   
   return hn;
}

 
int
setDoNotClose( NL_Handle  *netlogger_handle_p, int new_val )
{
    int old_val;

    if ( netlogger_handle_p ==  0 )
        return -1;

    old_val = netlogger_handle_p->doNotClose;
    netlogger_handle_p->doNotClose = new_val;
    return old_val;
}

 
NL_Handle * create_handle( char *program_name )
{
   NL_Handle *lp;

   lp = (NL_Handle *) calloc (1,sizeof (NL_Handle));

    
    
   lp->enabled = NetLogGetEnv ( "NETLOGGER_ON" );
   if (lp->enabled == NL_NOTSET)
      lp->enabled = NL_ON;
   else if (lp->enabled == NL_OFF)
   {
      free(lp);
      lp =  0 ;
      goto bottom;
   }

    lp->binary = 0;

    if (program_name ==  0 ) 	 
    {
       int pid = (int) getpid ();
       program_name = (char *) malloc (32);
       sprintf (program_name, "%d", pid);
    }
    
    lp->curr_buffer_size = 0;
    lp->event_buffer =  0 ;
    lp->last_date = malloc ( 22 );
    lp->host_prog = malloc (128);
    lp->message_buffer = (char *) calloc ( 1024 , sizeof (char));
    lp->last_tv_sec = 0;
    lp->pname = strdup (program_name);
    lp->hname = lookup_host();
    if (lp->binary)
       sprintf (lp->host_prog, "%s;%s;", lp->hname, program_name);
    else
       sprintf (lp->host_prog, " HOST=%s PROG=%s NL.EVNT=",
		lp->hname, program_name);
    lp->failed = 0;
    lp->tag =  0 ;
    setDoNotClose( lp, 0 );

 bottom:
    return lp;
}

 
NL_Handle *
NetLoggerOpenFd (char *p_name, int fd, int flags)
{
    NL_Handle     *lp;
    char          *mode;      

     
    lp = create_handle(p_name);
    if ( lp ==  0 )
       goto bottom;

     
    lp->mtx = (pthread_mutex_t *) malloc (sizeof (pthread_mutex_t));
    pthread_mutex_init (lp->mtx,  0 );
    pthread_mutex_lock (lp->mtx);

    setDoNotClose( lp, 1 );

     
    if ( flags & NL_MEM )
    {
       lp->method =  3 ;
       lp->event_buffer = (char *)calloc( 10 * 1024 * 1024 ,1);
    }
    else
       lp->method =  0 ;

     
    mode = (flags & NL_APPEND) ? "a" : "w";

     
    lp->log_fp = fdopen( fd, mode );
    lp->log_fd = fd;

 bottom:
      pthread_mutex_unlock (lp->mtx);
    return lp;
}

 
NL_Handle *
NetLoggerOpen (char *p_name, char *url, int flags)
{
    const char *func = "NetLoggerOpen";
    NL_Handle     *lp;
    char          *filename =  0 ;  
    char          *hostname =  0 ;  
    int            port;      
    int            method;    
    char          *mode =  0 ;      
    char          *env;       

     
    lp = create_handle(p_name);
    if ( lp ==  0 )
       goto bottom;

     
    lp->mtx = (pthread_mutex_t *) malloc (sizeof (pthread_mutex_t));
    pthread_mutex_init (lp->mtx,  0 );
    pthread_mutex_lock (lp->mtx);

     
    env = getenv ( "NETLOGGER_DEST" );

     
    if ((flags & NL_ENV) && env !=  0 )
    {
       NetLogSetMethod (env, flags, &method, &filename, &hostname, &port);
    }
    else
    {
       NetLogSetMethod (url, flags, &method, &filename, &hostname, &port);
    }

     
    if (flags & NL_APPEND)
	mode = strdup("a");
    else
	mode = strdup("w");

     
    switch (method)
    {
	case  0 :
	case  3 :
	   if (!(lp->log_fp = LogOpen (filename, mode)))
	   {
	      goto error_return;
	   }
	   lp->method = method;
	   lp->log_socket = -1;
	   lp->filename = strdup(filename);
	   if (method ==  3 )
	   {
	      lp->event_buffer =
		 (char *) calloc ( 10 * 1024 * 1024 , sizeof (char));
	   }
	   break;
	   
	case  1 :
	case  4 :
	   lp->method = method;
	   lp->log_fp =  0 ;
	   lp->log_socket = -1;
	   lp->filename =  0 ;
	   SysLogOpen (p_name,  ( 16 << 3 ));
	   if (method ==  4 )
	   {
	      lp->event_buffer =
		 (char *) calloc ( 10 * 1024 * 1024 , sizeof (char));
	   }
	   break;
	   
	case  2 :
	case  5 :
	   lp->method = method;
	   lp->log_fp =  0 ;
	   lp->filename =  0 ;
	   lp->hostname = strdup(hostname);
	   lp->log_socket = NetLogOpen (hostname, port);
	   if (lp->log_socket < 0)
	   {
	      nl_status ("%s; NetLogOpen failed", func);
	      goto error_return;
	   }
           lp->failed = 0;

	   

	   if (method ==  5 )
	   {
	      lp->event_buffer =
		 (char *) calloc ( 10 * 1024 * 1024 , sizeof (char));
	   }

	   NetLogWriteHeader( lp->log_socket, flags );

	   break;
	   
	default:
	   goto error_return;
	   break;
    }  

  bottom:
    pthread_mutex_unlock (lp->mtx);
    if (filename) free(filename);
    if (hostname) free(hostname);
    if (mode)     free(mode);
    return lp;

  error_return:
    pthread_mutex_unlock (lp->mtx);
    if (filename) free(filename);
    if (hostname) free(hostname);
    if (mode) free(mode);
    free (lp);
    return  0 ;
}


 
int
NetLoggerGTWrite (NL_Handle * lp, 
		  long sec,
		  long usec, 
		  char *keyword,
		  char *consts,
		  char *fmt,
		  ...)
{
    int const_len;
    int       rval = 1;
    va_list   args;
    char     *endPtr;
    struct timeval tv;

    if ( !  ( ( lp ) != 0 && ( lp ) -> enabled != NL_OFF ) )
	return 0;

     
    if (sec <= 0)
    {
       nl_status ("Keyword: %s; Bad time value: seconds = %d",
		   keyword, sec);
       return -1;
    }
    tv.tv_sec = sec;
    tv.tv_usec = usec;

     
    pthread_mutex_lock (lp->mtx);

     
    endPtr = printStandardFieldsToBuffer (&tv, lp, keyword);
    if (endPtr ==  0 )
    {
       rval = -1;
       goto error_return;
    }

     
     ( void ) ( args = ( __va_list ) & __builtin_va_alist );
    if ( consts !=  0 && *consts != '\0' )
    {
	const_len = strlen(consts);
	memcpy( endPtr,consts, const_len);
	endPtr += const_len;
	*endPtr++ = ' ';
    }
    endPtr += vsprintf (endPtr, fmt, args);
     ( void ) 0 ;

     
    *endPtr++ = '\n';
    *endPtr = '\0';
 
     
    if (lp->failed == 0) {
       rval = writeBuffer (lp, lp->message_buffer,
		       (int) (endPtr - lp->message_buffer));
       if (rval == -1)
          lp->failed = 1;
    }
  error_return:

     
    *lp->message_buffer = '\0';

    pthread_mutex_unlock (lp->mtx);
    return rval;
}

 
int
NetLoggerWrite(NL_Handle * lp,
	       char *keyword,
	       char *consts,
	       char *fmt,
	       ... )
{
    int       rval = 1;
    va_list   args;
    char     *endPtr;
    struct timeval tv;
    int const_len;

    if ( !  ( ( lp ) != 0 && ( lp ) -> enabled != NL_OFF ) )
    {
	return 0;
    }

    gettimeofday(&tv, 0 );

     
    pthread_mutex_lock (lp->mtx);

     
    endPtr = printStandardFieldsToBuffer (&tv, lp, keyword);
    if (endPtr ==  0 )
    {
       rval = -1;
       goto error_return;
    }

     
     ( void ) ( args = ( __va_list ) & __builtin_va_alist );
    if ( consts !=  0 && *consts != '\0' )
    {
	const_len = strlen(consts);
	memcpy( endPtr,consts, const_len);
	endPtr += const_len;
	*endPtr++ = ' ';
    }
    endPtr += vsprintf (endPtr, fmt, args);
     ( void ) 0 ;

     
    *endPtr++ = '\n';
    *endPtr = '\0';
 
     
    if (lp->failed == 0) {
       rval = writeBuffer (lp, lp->message_buffer,
		       (int) (endPtr - lp->message_buffer));
       if (rval == -1)
          lp->failed = 1;
    }
  error_return:

     
    *lp->message_buffer = '\0';

    pthread_mutex_unlock (lp->mtx);
    return rval;
}

 
int
NetLoggerWriteLvl( NL_Handle * lp, 
		   int lvl, 
		   char *keyword, 
		   char *consts,
		   char *fmt, 
		   ... )
{
    int rval = 1;
    struct timeval tv;
    char     *endPtr;
    va_list   args;
    int const_len;

    if ( !  ( ( lp ) != 0 && ( lp ) -> enabled != NL_OFF ) || !  ( ( ( lp ) -> loglvl > 0 ) && ( ( lvl ) <= ( lp ) -> loglvl ) ) )
    {
      rval = 0;
    }
    else
    {
	 
	gettimeofday (&tv, 0);

	 
	pthread_mutex_lock (lp->mtx);
	
	 
	endPtr = printStandardFieldsToBuffer (&tv, lp, keyword);
	if (endPtr ==  0 )
	{
	    rval = -1;
	    goto error_return;
	}

	if ( consts !=  0 && *consts != '\0' )
	{
	    const_len = strlen(consts);
	    memcpy( endPtr,consts, const_len);
	    endPtr += const_len;
	    *endPtr++ = ' ';
	}
	 ( void ) ( args = ( __va_list ) & __builtin_va_alist );
	endPtr += vsprintf (endPtr, fmt, args);
	 ( void ) 0 ;
	
	 
	*endPtr++ = '\n';
	*endPtr = '\0';
	
	 
	if (lp->failed == 0) {
	    rval = writeBuffer (lp, lp->message_buffer,
				(int) (endPtr - lp->message_buffer));
	    if (rval == -1)
		lp->failed = 1;
	}

      error_return:	
	 
	*lp->message_buffer = '\0';
	
	pthread_mutex_unlock (lp->mtx);
    }
    return rval;
}

 
int
NetLoggerGTWriteLvl( NL_Handle * lp, 
		     long sec,
		     long usec,
		     int lvl, 
		     char *keyword, 
		     char *consts,
		     char *fmt, 
		     ... )
{
    int rval = 1;
    va_list   args;

    if ( ! ( ( lp ) != 0 && ( lp ) -> enabled != NL_OFF ) || ! ( ( ( lp ) -> loglvl > 0 ) && ( ( lvl ) <= ( lp ) -> loglvl ) ) )
    {
      rval = 0;
    }
    else
    {
	 ( void ) ( args = ( __va_list ) & __builtin_va_alist );
	rval = NetLoggerGTWrite(lp,sec, usec,keyword,consts,fmt,args);
	 ( void ) 0 ;
    }
   return rval;
}

 
int
NetLoggerGTWriteString(NL_Handle * lp, 
		       long sec, 
		       long usec,			       
		       char *keyword, 
		       char *const_data,
		       char *data)
    
{
    int       rval = 1;
    char     *endPtr =  0 ;
    struct timeval tv;

    if ( !  ( ( lp ) != 0 && ( lp ) -> enabled != NL_OFF ) )
    {
	return 0;
    }

    tv.tv_sec = sec;
    tv.tv_usec = usec;

    pthread_mutex_lock (lp->mtx);

     
    endPtr = printStandardFieldsToBuffer (&tv, lp, keyword);
    if (endPtr ==  0 )
      {
	  rval = -1;
	  goto error_return;
      }

     
    if ( const_data !=  0 && *const_data != '\0' )
    {
	strcat(lp->message_buffer, const_data );
	strcat(lp->message_buffer," ");
    }
    strcat(lp->message_buffer, data);
    strcat (lp->message_buffer, "\n");

     
    if (lp->failed == 0) {
       rval = writeBuffer (lp, lp->message_buffer,
			   strlen (lp->message_buffer));
       if (rval == -1)
          lp->failed = -1;
    }

  error_return:

     
    *lp->message_buffer = '\0';

    pthread_mutex_unlock (lp->mtx);

    return rval;
}


 

int
NetLoggerWriteString (NL_Handle * lp,
		      char *keyword, 
		      char *const_data,
		      char *data)
{
    int rval = 1;
    struct timeval tv;

    if ( !  ( ( lp ) != 0 && ( lp ) -> enabled != NL_OFF ) )
	return 0;

     
    gettimeofday (&tv, 0);

    rval = NetLoggerGTWriteString( lp, tv.tv_sec, tv.tv_usec,
				   keyword, const_data, 
				   data );

    return rval;
}

 
int
NetLoggerGTWriteStringLvl(NL_Handle * lp,
			  int lvl,
			  long sec,
			  long usec,
			  char *keyword,
			  char *const_data,
			  char *var_data)
{
    int rval = 1;
    if ( ! ( ( lp ) != 0 && ( lp ) -> enabled != NL_OFF ) || ! ( ( ( lp ) -> loglvl > 0 ) && ( ( lvl ) <= ( lp ) -> loglvl ) ) )
    {
	rval = 0;
    }
    else
    {
	rval=NetLoggerGTWriteString( lp, sec, usec, keyword, 
				     const_data, var_data );

    }
    return rval;
}

 
int
NetLoggerWriteStringLvl (NL_Handle * lp,
			 int lvl,
			 char *keyword, 
			 char *const_data,
			 char *data)
{
    int rval = 1;
    struct timeval tv;

    if ( ! ( ( lp ) != 0 && ( lp ) -> enabled != NL_OFF ) || ! ( ( ( lp ) -> loglvl > 0 ) && ( ( lvl ) <= ( lp ) -> loglvl ) ) )
    {
	rval = 0;
    }
    else
    {
	 
	gettimeofday (&tv, 0);
	rval = NetLoggerGTWriteString( lp, 
				       tv.tv_sec, tv.tv_usec,
				       keyword, 
				       const_data,
				       data);
    }
    return rval;
}


 
int
NetLoggerFlush (NL_Handle * lp)
{
    int       rval = 1;
    int       orginal_method;

    if (lp ==  0 || lp->enabled == NL_OFF)
	return 0;


    rval = 1;

     
    orginal_method = lp->method;
    switch (orginal_method)
      {
      case  3 :
	  lp->method =  0 ;
	  break;
      case  4 :
	  lp->method =  1 ;
	  break;
      case  5 :
	  lp->method =  2 ;
	  break;
      default:
	  rval = -1;
	  break;
      }

     
    if (lp->failed == 0) {
       rval = writeBuffer (lp, lp->event_buffer, lp->curr_buffer_size);
       if (rval == -1)
          lp->failed = 1;
    }

    lp->method = orginal_method;

    if (lp->method ==  3 )
	fflush (lp->log_fp);
    lp->curr_buffer_size = 0;

    return rval;
}

 
void
NetLoggerSetLevel (NL_Handle * lp, int log_level)
{
    if ( lp ==  0 ) return;
    lp->loglvl = log_level;
}


 
int doNotClose( NL_Handle * netlogger_handle_p )
{
   return ( netlogger_handle_p->doNotClose == 1 );
}

 
int
NetLoggerClose (NL_Handle * lp)
{
  int       rval = 1;

  if ( lp ==  0 )
      return 0;

  if (lp->enabled != NL_OFF)
  {
     
    if (lp->curr_buffer_size > 0)
      NetLoggerFlush (lp);
    
     
    switch (lp->method)
    {
    case  0 :
    case  3 :
      if ( ! doNotClose(lp) )
        LogClose (lp->log_fp);
      break;
    case  1 :
    case  4 :
      SysLogClose ();
      break;
    case  2 :
    case  5 :
      NetLogClose (lp->log_socket);
      break;
    default:
      rval = -1;
      break;
    }
  }

   
  if ( lp->filename ) free(lp->filename);
  if ( lp->event_buffer ) free(lp->event_buffer);
  if ( lp->pname ) free(lp->pname);
  if ( lp->hname ) free(lp->hname);
  if ( lp->message_buffer ) free(lp->message_buffer);
  if ( lp->last_date ) free(lp->last_date);
  if ( lp->host_prog ) free(lp->host_prog);
  if ( lp->mtx ) free(lp->mtx);
  free(lp);

  return rval;
}

 

int
NetLoggerSetGlobalTag (char *tag)
{
    int       ret = 0;
    pthread_mutex_lock (&global_mutex);
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
    pthread_mutex_unlock (&global_mutex);
    return ret;
}

 
int
NetLoggerSetTag(NL_Handle  *h_p,
		char       *tag )
{
    if ( h_p ==  0 )
	return 0;
    if ( h_p->tag !=  0 )
	free( h_p->tag );
    h_p->tag = strdup(tag);
    return 1;
}

 


 
char     *
printStandardFieldsToBuffer (struct timeval *tvPtr,
			     NL_Handle * lp, char *keyword)
{
    struct tm *tm =  0 ;
    char      date_buffer[ 22 ];
    int       i, usec;
    char      n[7];
    if (lp->last_tv_sec != tvPtr->tv_sec)	 
      {
	   
	   
	  if ((tm = gmtime ((time_t *) & tvPtr->tv_sec)) ==  0 )
	    {
		perror ("localtime error");
		return  0 ;
	    }

	   
	  if (strftime (date_buffer,  22 , "%Y%m%d%H%M%S.", tm) == 0)
	    {
		perror ("strftime error");
		return  0 ;
	    }
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
     
     
    usec = (int) tvPtr->tv_usec;
    n[6] = '\0';		 
    for (i = 0; i < 6; i++)	 
      {
	  n[5 - i] = (char) ( 48 + (usec % 10));
	  usec /= 10;
      }
    strcat (lp->message_buffer, n);
    strcat (lp->message_buffer, lp->host_prog);	 
    strcat (lp->message_buffer, keyword);
    strcat (lp->message_buffer, " ");
     
    if ( lp->tag !=  0 )
    {
	strcat(lp->message_buffer, lp->tag );
	strcat(lp->message_buffer," ");
    }
     
    if (globtag)
    {
	strcat (lp->message_buffer, globtag);
	strcat (lp->message_buffer, " ");
    }
    lp->last_tv_sec = tvPtr->tv_sec;
     
     
    return (lp->message_buffer + strlen (lp->message_buffer));
}

 

static int
writeBuffer (NL_Handle * lp, char *outbuf, int buflen)
{
    char     *func = "writeBuffer";
    int       rval = 1;
    int       n;

    switch (lp->method)
      {
      case  0 :
	  if ((fwrite (outbuf, buflen, sizeof (char), lp->log_fp)) <= 0)
	      nl_status ("%s: NL_FILE: Error sending Netlogger message to file.",
			 func);
	  break;
      case  1 :
	  syslog ( 6 , "%s", outbuf);
	  break;
      case  2 :

          if (lp->failed == 0) {
	     n = nl_writen (lp->log_socket, outbuf, buflen);
	     if (n != buflen) {
		nl_status ("%s: Error sending Netlogger message to socket.",
			   func); return -1;
	     }
          }
	  break;
      case  3 :
      case  4 :
      case  5 :
 
	  if ((lp->curr_buffer_size + buflen) >=  10 * 1024 * 1024 )
	    {
		NetLoggerFlush (lp);
	    }

	   
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
    FILE     *fp =  0 ;
    if (logfile_name ==  0 )
	return  0 ;
    if (mode ==  0 )
	mode = "w";
    if (!strcmp (mode, "a"))
      {
	  if ((fp = fopen (logfile_name, "a")) ==  0 )
{
		nl_status ("%s: Error opening log file %s", func,
			   logfile_name); return  0 ;
	    }
      }
    else
      {
	  if ((fp = fopen (logfile_name, "w")) ==  0 )
	    {
		nl_status ("%s: Error opening log file %s", func,
			   logfile_name); return  0 ;
	    }
      }
      return fp;
}

 
int
SysLogOpen (char *prog_name, int syslog_facility)
{
     

    openlog (prog_name,  0x01 , syslog_facility);
     
    return 1;
}

 


 
int
NetLogOpen (char *hostname, int port)
{
    struct hostent *hp =  0 ;
    struct sockaddr_in sin;
    int       log_socket;

    if (hostname ==  0 )
      {
	  hostname = getenv ("NL_DEST_ENV");
	  if (hostname ==  0 )
	  {
	     nl_status
		("Error: no hostname, and environment var %s not set!",
		  "NETLOGGER_DEST" ); return -1;
	  }
      }
    if ((hp = gethostbyname (hostname)) == 0)
    {
       perror ("gethostbyname");
       return -1;
    }
    memset ((void *) &sin, 0, sizeof (sin));
    sin.sin_family =  2 ;
    sin.sin_addr. S_un . S_addr = ((struct in_addr *) (hp-> h_addr_list [ 0 ]))-> S_un . S_addr ;
    sin.sin_port =  ( port );
    if ((log_socket = socket ( 2 ,  2 , 0)) == -1)
    {
       perror ("socket");
       return -1;
    }


    if (connect (log_socket, (struct sockaddr *) &sin, sizeof (sin)) == -1)
    {
       perror ("connect");
       close (log_socket);
       log_socket = -1;
       return -1;
    }
    
    return log_socket;
}

 
void
NetLogWriteHeader( int sockfd, int flags )
{
   static const char ulm_hdr[] = "ULM\0    ";
   write( sockfd, ulm_hdr, 8 );
}

 
int
LogClose (FILE *fp)
{
    fclose (fp);
    return 1;
}

 
int
SysLogClose (void)
{
    closelog ();
    return 1;
}

 
int
NetLogClose (int log_socket)
{
    close (log_socket);
    return 1;
}


 
 
 
 

float
ulm_elapsed_time (char *t1, char *t2)
{
    struct timeval tv1, tv2;
    struct tm *tm =  0 ;
    float     etime;
     
     
     
    memset (tm, 0, sizeof (struct tm));
    sscanf (t1, "%4d%2d%2d%2d%2d%2d", &(tm->tm_year),
	    &(tm->tm_mon), &(tm->tm_mday), &(tm->tm_hour),
	    &(tm->tm_min), &(tm->tm_sec));
    tm->tm_year = tm->tm_year - 1900;
    tv1.tv_sec = (long) mktime (tm);
     
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


 
 
 
static void
nl_status (char *fmt, ...)	 
{
    va_list   args;
    if (nl_pname !=  0 && nl_hname !=  0 )
	fprintf ( ( & __iob [ 2 ] ), "%s@%s: ", nl_pname, nl_hname);
    else if (nl_pname !=  0 )
	fprintf ( ( & __iob [ 2 ] ), "%s: ", nl_pname);
     ( void ) ( args = ( __va_list ) & __builtin_va_alist );
    vfprintf ( ( & __iob [ 2 ] ), fmt, args);
     ( void ) 0 ;
    fputc ('\n',  ( & __iob [ 2 ] ));
    fflush ( ( & __iob [ 2 ] ));
    return;
}

 

static int
nl_writen (fd, ptr, nbytes)	 
     register int fd;
     register char *ptr;
     register int nbytes;
{
    int       nleft, nwritten;
    nleft = nbytes;
    signal( 13 ,  ( void ( * ) ( ) ) 1 );
    while (nleft > 0)
      {
	  nwritten = write (fd, ptr, nleft);
	  if (nwritten < 0 &&  ( * ( ___errno ( ) ) ) == 32) {
	     fprintf ( ( & __iob [ 2 ] ), "nl_writen: Write to socket failed. logging suspended\n");
             close(fd);
	     return (nwritten);	 
          }
	  nleft -= nwritten;
	  ptr += nwritten;
      }
    return (nbytes - nleft);
}

 
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
	      return (nread);	 
	  nleft -= nread;
	  ptr += nread;
      }
    return (nbytes - nleft);
}



 
 
int
NetLogGetEnv (char *varname)
{
    char     *v;
    int       netlog_on;
    v = getenv (varname);
    if (v ==  0 )
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

 
void
NetLogSetMethod (char *s,	 
		 int flags,
		 int *method_p,
		 char **filename_p, char **hostname_p, int *port_p)
 
{
    if (!s)
      {
	  nl_status ("NetLogger URL is NULL");
	  *method_p = (int)  0 ;
	  return;
      }
    if (!strncmp (s, "file://", 7))
      {
	  if (flags & NL_MEM)
	      *method_p =  3 ;
	  else
	      *method_p =  0 ;
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
	  else			 
	    {
		*port_p =  14830 ;
	    }
	  if (flags & NL_MEM)
	      *method_p =  5 ;
	  else
	      *method_p =  2 ;
	  *hostname_p = strdup (s + 11);
	  nl_status ("Logging to host %s, port %d", *hostname_p, *port_p);
      }
    else if (!strncmp (s, "x-syslog://", 11))
      {
	  *method_p =  1 ;
	  nl_status ("Logging to syslog");
      }
    else
      {
	  if (flags & NL_MEM)
	      *method_p =  3 ;
	  else
	      *method_p =  0 ;
	  *filename_p = strdup (s);
	  nl_status ("Logging to file %s", *filename_p);
      }
}


#ident "acomp: Sun WorkShop 6 2000/06/19 C 5.1 Patch 109491-02"
