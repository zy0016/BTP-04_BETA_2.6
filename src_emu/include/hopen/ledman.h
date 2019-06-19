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
 * Author  :     
 *
 *-------------------------------------------------------------------------
 *
 * $Source: /cvs/hopencvs/src/include/hopen/ledman.h,v $
 * $Name:  $
 *
 * $Revision: 1.1 $     $Date: 2003/08/13 04:58:57 $
 * 
\**************************************************************************/

#ifndef __HOPEN_LEDMAN_H__
#define __HOPEN_LEDMAN_H__ 1


#define LEDMAN_ALL			0	/* special case, all LED's */

/****************************************************************************/
/*
 *	ioctl cmds
 */

#define LEDMAN_CMD_SET		0x01	/* turn on briefly to show activity */
#define LEDMAN_CMD_ON		0x02	/* turn LED on permanently */
#define LEDMAN_CMD_OFF		0x03	/* turn LED off permanently */
#define LEDMAN_CMD_FLASH	0x04	/* flash this LED */
#define LEDMAN_CMD_RESET	0x05	/* reset LED to default behaviour */

/****************************************************************************/

#define LEDMAN_MAJOR	126

/****************************************************************************/
#ifndef __KERNEL__

#include	<fcntl.h>

#define ledman_cmd(cmd, led) ({ \
	int fd; \
	if ((fd = open("/dev/ledman", O_RDWR)) != -1) { \
		ioctl(fd, cmd, led); \
		close(fd); \
	} \
})

#endif //__KERNEL__

/****************************************************************************/
#endif /* __HOPEN_LEDMAN_H__ */
