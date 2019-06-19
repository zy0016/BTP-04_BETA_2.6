/**************************************************************************\
 *
 *                      Hopen Software System
 *
 * Copyright (c) 1999-2000 by Hopen Software Engineering Co., Ltd. 
 *                       All Rights Reserved
 *
 * Model   :
 *
 * Purpose :     
 *  
 * Author  :     LiHejia
 *
 *-------------------------------------------------------------------------
 *
 * $Source: /cvs/hopencvs/src/include/sys/reent.h,v $
 * $Name:  $
 *
 * $Revision: 1.1.1.1 $     $Date: 2002/12/26 07:59:00 $
 * 
\**************************************************************************/

/* This header file provides the reentrancy.  */

/* WARNING: All identifiers here must begin with an underscore.  This file is
   included by stdio.h and others and we therefore must only use identifiers
   in the namespace allotted to us.  */

#ifndef __SYS_REENT_H_
#define __SYS_REENT_H_

struct _glue 
{
	struct _glue *_next;
	int _niobs;
	struct __sFILE *_iobs;
};

/*
 * Stdio buffers.
 *
 * This and __sFILE are defined here because we need them for struct _reent,
 * but we don't want stdio.h included when stdlib.h is.
 */

struct __sbuf {
	unsigned char *_base;
	int	_size;
};

/*
 * We need fpos_t for the following, but it doesn't have a leading "_",
 * so we use _fpos_t instead.
 */

typedef long _fpos_t;		/* XXX must match off_t in <sys/types.h> */
				/* (and must be `long' for now) */

/*
 * Stdio state variables.
 *
 * The following always hold:
 *
 *	if (_flags&(__SLBF|__SWR)) == (__SLBF|__SWR),
 *		_lbfsize is -_bf._size, else _lbfsize is 0
 *	if _flags&__SRD, _w is 0
 *	if _flags&__SWR, _r is 0
 *
 * This ensures that the getc and putc macros (or inline functions) never
 * try to write or read from a file that is in `read' or `write' mode.
 * (Moreover, they can, and do, automatically switch from read mode to
 * write mode, and back, on "r+" and "w+" files.)
 *
 * _lbfsize is used only to make the inline line-buffered output stream
 * code as compact as possible.
 *
 * _ub, _up, and _ur are used when ungetc() pushes back more characters
 * than fit in the current _bf, or when ungetc() pushes back a character
 * that does not match the previous one in _bf.  When this happens,
 * _ub._base becomes non-nil (i.e., a stream has ungetc() data iff
 * _ub._base!=NULL) and _up and _ur save the current values of _p and _r.
 */

struct __sFILE {
	unsigned char *	_p;		/* current position in buffer */
	int		_r;		/* read space left for getc() */
	int		_w;		/* write space left for putc() */
	short		_flags;		/* flags, below; this FILE is free if 0 */
	short		_file;		/* fileno, if Unix descriptor, else -1 */
	struct __sbuf	_bf;		/* the buffer (at least 1 byte, if !NULL) */
	int		_lbfsize;	/* 0 or -_bf._size, for inline putc */
	unsigned char	_ubuf[1];	/* guarantee an ungetc() buffer */
	unsigned char	_nbuf[1];	/* guarantee a getc() buffer */
	unsigned char	unused[2];
	/* Unix stdio files get aligned to block boundaries on fseek() */
	int		_blksize;	/* stat.st_blksize (may be != _bf._size) */
	int		_offset;	/* current offset */
};

#endif /* __SYS_REENT_H_ */
