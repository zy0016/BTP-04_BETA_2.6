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


#ifndef _STDIO_H_
#define	_STDIO_H_

#ifndef NULL
#ifdef __cplusplus
	#define NULL	0
#else
	#define NULL	((void *)0)
#endif
#endif

#include <stdarg.h>

/*
 * <sys/reent.h> defines __sFILE, _fpos_t.
 * They must be defined there because struct _reent needs them (and we don't
 * want reent.h to include this file.
 */

#include <sys/reent.h>

typedef _fpos_t fpos_t;

#ifndef __SIZE_T
#define __SIZE_T
#if defined(__i386) || !defined (__GNUC__)
typedef unsigned int	size_t;
#else
typedef unsigned long	size_t;
#endif
#endif

typedef struct __sFILE FILE;

#define	__SLBF	0x0001		/* line buffered */
#define	__SNBF	0x0002		/* unbuffered */
#define	__SRD	0x0004		/* OK to read */
#define	__SWR	0x0008		/* OK to write */
	/* RD and WR are never simultaneously asserted */
#define	__SRW	0x0010		/* open for reading & writing */
#define	__SEOF	0x0020		/* found EOF */
#define	__SERR	0x0040		/* found error */
#define	__SMBF	0x0080		/* _buf is from malloc */
#define	__SAPP	0x0100		/* fdopen()ed in append mode - so must  write to end */
#define	__SOPT	0x0400		/* do fseek() optimisation */
#define	__SNPT	0x0800		/* do not do fseek() optimisation */
#define	__SOFF	0x1000		/* set iff _offset is in fact correct */
#define __SUBF	0x4000		/* ubuf is valid */

#define	BUFSIZ	1024
#define	EOF	(-1)

#define _IOFBF 0 /* Fully buffered. */
#define _IOLBF 1 /* Line buffered. */
#define _IONBF 2 /* No buffering. */

#define	SEEK_SET	0
#define	SEEK_CUR	1
#define	SEEK_END	2

#define	L_tmpnam	255	/* XXX must be == PATH_MAX */
#define P_tmpdir        "/tmp"
#define	TMP_MAX		26

extern FILE *stdin, *stdout, *stderr;

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Routines internal to the implementation.
 */

int	__srget(FILE *);
int	__swbuf(int, FILE *);

extern int __sgetc(FILE *p);
extern int __sputc(int c, FILE *p);

#define	__sfeof(p)	(((p)->_flags & __SEOF) != 0)
#define	__sferror(p)	(((p)->_flags & __SERR) != 0)
#define	__sclearerr(p)	((void)((p)->_flags &= ~(__SERR|__SEOF)))
#define	__sfileno(p)	((p)->_file)

/*
 * Functions defined in ANSI C standard.
 */

int	remove(const char *);
int	rename(const char *, const char *);
FILE *	tmpfile(void);
char *	tmpnam(char *);
int	fclose(FILE *);
int	fflush(FILE *);
FILE *	freopen(const char *, const char *, FILE *);
void	setbuf(FILE *, char *);
int	setvbuf(FILE *, char *, int, size_t);
int	fprintf(FILE *, const char *, ...);
int	fscanf(FILE *, const char *, ...);
void	printf(const char *, ...);
void PLXPrintf( const char *fmt, ... );
extern int  snprintf(char *, size_t, const char *,...);
int	scanf(const char *, ...);
int	sscanf(const char *, const char *, ...);
int	vfprintf(FILE *, const char *, va_list);
int	vprintf(const char *, va_list);
int	vsprintf(char *, const char *, va_list);
int vsnprintf(char *, size_t, const char *, va_list);
int	fgetc(FILE *);
char *	fgets(char *, int, FILE *);
int	fputc(int, FILE *);
int	fputs(const char *, FILE *);
int	getc(FILE *);
int	getchar(void);
char *	gets(char *);
int	putc(int, FILE *);
int	putchar(int);
int	puts(const char *);
int	ungetc(int, FILE *);
size_t	fread(void *, size_t, size_t, FILE *);
size_t	fwrite(const void *, size_t, size_t, FILE *);
int	fgetpos(FILE *, fpos_t *);
int	fseek(FILE *, long, int);
int	fsetpos(FILE *, const fpos_t *);
long	ftell( FILE *);
void	rewind(FILE *);
void	clearerr(FILE *);
int	feof(FILE *);
int	ferror(FILE *);
void    perror(const char *);
FILE *	fopen(const char *, const char *);
int	sprintf(char *, const char *, ...);
FILE *	fdopen(int, const char *);
int	fileno(FILE *);

char *	tempnam(const char *, const char *);
int	fcloseall(void);

#ifdef __cplusplus
}
#endif

#define getc(fp)	__sgetc(fp)
#define putc(x, fp)	__sputc(x, fp)

#define	getchar()	getc(stdin)
#define	putchar(x)	putc(x, stdout)

#endif
