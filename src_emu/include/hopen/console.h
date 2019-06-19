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
 * $Source: /cvs/hopencvs/src/include/hopen/console.h,v $
 * $Name:  $
 *
 * $Revision: 1.1.1.1 $     $Date: 2002/12/26 07:58:59 $
 * 
\**************************************************************************/

#ifndef _HOPEN_CONSOLE_H_
#define	_HOPEN_CONSOLE_H_

#include	"hopen/ioctl.h"

#define	CON_SETSYNCCHAR			_IO(200, 1)
#define	CON_STARTSESSION		_IO(200, 2)	
#define	CON_ENDSESSION			_IO(200, 3)

#endif /* _HOPEN_CONSOLE_H_ */
