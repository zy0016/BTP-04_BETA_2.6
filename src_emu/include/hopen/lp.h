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
 * Author  :     WangXinshe
 *
 *-------------------------------------------------------------------------
 *
 * $Source: /cvs/hopencvs/src/include/hopen/lp.h,v $
 * $Name:  $
 *
 * $Revision: 1.1.1.1 $     $Date: 2002/12/26 07:59:00 $
 *
\**************************************************************************/

#ifndef _HOPEN_LP_H
#define _HOPEN_LP_H

/*
 * Per POSIX guidelines, this module reserves the LP and lp prefixes
 * These are the lp_table[minor].flags flags...
 */
#define LP_EXIST	0x0001
#define LP_SELEC	0x0002
#define LP_BUSY		0x0004
#define LP_OFFL		0x0008
#define LP_NOPA		0x0010
#define LP_ERR		0x0020
#define LP_ABORT	0x0040
#define LP_CAREFUL	0x0080 /* obsoleted -arca */
#define LP_ABORTOPEN	0x0100
#define	LP_TRUST_IRQ	0x0200

/* IOCTL numbers */
#define LPCHAR		0x0601  /* corresponds to LP_INIT_CHAR */
#define LPTIME		0x0602  /* corresponds to LP_INIT_TIME */
#define LPABORT		0x0604  /* call with TRUE arg to abort on error,
				    FALSE to retry.  Default is retry.  */
#define LPSETIRQ	0x0605  /* call with new IRQ number,
				    or 0 for polling (no IRQ) */
#define LPGETIRQ	0x0606  /* get the current IRQ number */
#define LPWAIT		0x0608  /* corresponds to LP_INIT_WAIT */
/* NOTE: LPCAREFUL is obsoleted and it' s always the default right now -arca */
#define LPCAREFUL	0x0609  /* call with TRUE arg to require out-of-paper, off-
				    line, and error indicators good on all writes,
				    FALSE to ignore them.  Default is ignore. */
#define LPABORTOPEN	0x060a  /* call with TRUE arg to abort open() on error,
				    FALSE to ignore error.  Default is ignore.  */
#define LPGETSTATUS	0x060b  /* return LP_S(minor) */
#define LPRESET		0x060c  /* reset printer */
#define LPGETSTATS	0x060d  /* get statistics (struct lp_stats) */
#define LPGETFLAGS	0x060e  /* get status flags */
#define LPTRUSTIRQ	0x060f  /* set/unset the LP_TRUST_IRQ flag */

#endif		/* HOPEN_LP_H */

