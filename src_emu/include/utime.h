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

#ifndef _UTIME_H_
#define	_UTIME_H_

/* Structure describing file times.  */
struct utimbuf
{
	unsigned long	actime;		/* Access time.  */
	unsigned long	modtime;	/* Modification time.  */
};

#ifdef __cplusplus
extern "C" {
#endif

/* Set the access and modification times of FILE to those given in
   *FILE_TIMES.  If FILE_TIMES is NULL, set them to the current time.  */
int utime (const char *__file, const struct utimbuf * file_times);

#ifdef __cplusplus
}
#endif

#endif /* _UTIME_H */
