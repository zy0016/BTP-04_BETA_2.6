#ifndef _DDI_FLASH_H
#define _DDI_FLASH_H

#include "stype.h"

#if 0 //#ifndef IO_FLH_SETMODE
#define IO_FLH_SETMODE		0x0060	/*	funtion 0010H: get graphic engine 
										pBuf:	pointer to a double word
										size:	buffer size = 4
									*/
#define IO_FLH_GETMODE		0x8060	/*	funtion 0010H: get graphic engine 
										pBuf:	pointer to a double word
										size:	buffer size = 4
									*/
#endif	// IO_FLH_SETMODE

#define FLH_MODE_NORMAL			1
#define FLH_MODE_DIRECTWRITE	2

#endif // _DDI_FLASH_H
