/**************************************************************************\
 *                      Hopen Software System
 *
 * Copyright (c) 1999-2000 by Hopen Software Engineering Co., Ltd. 
 *                       All Rights Reserved
 *
 * Model   :	HOPEN Device Drive
 *
 * Purpose :	Device Drive
 *  
 * Author  :	Di Ling
 *
 *-------------------------------------------------------------------------
 *
 * $Archive:: /hopen2.0/include/DI_Mouse.h			                $
 * $Workfile::DI_Mouse.h                                            $
 * $Revision:: 4  $     $Date:: 03-04-26 3:28p						$
 *
\**************************************************************************/


/***************************************************************************
 *
 * Hopen Device Driver Interface
 *
 * $Revision: 1 $     $Date: 99-11-08 14:21 $    
 *
 * $Log: /Hopen/include/ddi/DI_Mouse.h $
 * 
 * 1     99-11-08 14:21 Dling
 * 
 * 1     99-11-08 13:38 Dling
 * 
 ***************************************************************************
 */

#ifndef _DDI_MOUSE_H
#define _DDI_MOUSE_H

/*
typedef struct MOUSE_Mode {
	unsigned short slen;		// 该结构的长度
	unsigned short unused_1;	// 未用
	unsigned short samplerate;	// 每秒钟采样的次数
	unsigned short resolution;	// 移动的分辨率
} MOUSE_Mode;
*/

/* Device state structure */
/*
typedef struct MOUSE_Event {
	unsigned short	slen;
	unsigned short	btnstate;
	short	dx;
	short	dy;
} MOUSE_Event;*/

typedef struct MOUSEEVENT {
	short	dx;
	short	dy;
	WORD	btn_state;
	WORD	time;
}MOUSEEVENT;

/* Define mouse event */
#define	MOUSE_INPUTEVENT	1

#endif // _DDI_MOUSE_H
