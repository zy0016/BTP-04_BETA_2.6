/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : 
 *
 * Purpose  : 
 *
\**************************************************************************/

#ifndef _UNISTD_H
#define _UNISTD_H

/* These may be used to determine what facilities are present at compile time.
   Their values can be obtained at run time from `sysconf'.  */

/* POSIX Standard approved as ISO/IEC 9945-1 as of August, 1988 and
   extended by POSIX-1b (aka POSIX-4) and POSIX-1c (aka POSIX threads).  */
#define	_POSIX_VERSION	199607L

#include <machine/posix_opt.h>
#include <machine/types.h>
#include <stddef.h>

#include <sys/types.h>

# define	F_OK	0
# define	R_OK	4
# define	W_OK	2
# define	X_OK	1

/* Values for the WHENCE argument to lseek.  */
#ifndef	_STDIO_H		/* <stdio.h> has the same definitions.  */
# define SEEK_SET	0	/* Seek from beginning of file.  */
# define SEEK_CUR	1	/* Seek from current position.  */
# define SEEK_END	2	/* Seek from end of file.  */
#endif

#define STDIN_FILENO    0       /* standard input file descriptor */
#define STDOUT_FILENO   1       /* standard output file descriptor */
#define STDERR_FILENO   2       /* standard error file descriptor */

# define	_SC_ARG_MAX	0
# define	_SC_CHILD_MAX	1
# define	_SC_CLK_TCK	2
# define	_SC_NGROUPS_MAX	3
# define	_SC_OPEN_MAX	4
/* no _SC_STREAM_MAX */
# define	_SC_JOB_CONTROL	5
# define	_SC_SAVED_IDS	6
# define	_SC_VERSION	7
# define        _SC_PAGESIZE    8

# define	_PC_LINK_MAX	0
# define	_PC_MAX_CANON	1
# define	_PC_MAX_INPUT	2
# define	_PC_NAME_MAX	3
# define	_PC_PATH_MAX	4
# define	_PC_PIPE_BUF	5
# define	_PC_CHOWN_RESTRICTED	6
# define	_PC_NO_TRUNC	7
# define	_PC_VDISABLE	8

# ifndef	_POSIX_SOURCE
#  define	MAXNAMLEN	1024
# endif		/* _POSIX_SOURCE */

/* FIXME: This is temporary until winsup gets sorted out.  */
# define	MAXPATHLEN	1024

#define environ	_getenvptr()

#ifdef __cplusplus
extern "C" {
#endif

char **_getenvptr();
void _setenvptr(char ** envptr);

void _exit (int __status );

int  access (const char * path, int amode);
int  alarm (unsigned secs);
int  chdir (const char * path);

int  opendev (int major, int minor, int flags);

off_t lseek (int fildes, off_t offset, int whence);
int  read (int fildes, void * buf, size_t nbyte);
int  write  (int fildes, const void * buf, size_t nbyte);
int  close (int fildes);
int  fsync(int fd);
int  sync(void);

int  link (const char * path1, const char * path2);
int  unlink (const char * path);
int  symlink (const char * oldname, const char * newname);

int  readlink (const char * path, char * buf, int bufsize);

int  truncate(const char *path, off_t length);
int  ftruncate(int fd, off_t length);

int  dup (int fildes);
int  dup2 (int fildes, int fildes2);

char * getcwd (char * buf, int size);

int  isatty (int fildes);
int  pause (void);

int  pipe (int fildes[2]);

int  rmdir (const char * path);

int  brk (void * endaddr);
void * sbrk (long incr);
unsigned sleep (unsigned int seconds);
void usleep (unsigned long usec);

pid_t getpid();

size_t getpagesize(void);

int execve (const char *path, char * const argv[], char * const envp[]);
int execv( const char *path, char *const argv[]);
int execl (const char *path, const char *arg, ...);

int getopt(int argc, char * const argv[],
                  const char *optstring);
 
extern char *optarg;
extern int optind, opterr, optopt;

#ifdef __cplusplus
}
#endif

#endif /* _SYS_UNISTD_H */
