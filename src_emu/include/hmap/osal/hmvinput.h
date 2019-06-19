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

#ifndef _VINPUT_H_
#define _VINPUT_H_

#define VINPUT_MOUSE     0
#define VINPUT_KEYBOARD  1
#define VINPUT_HARDWARE  2

#define MOUSEEVENTF_MOVE        0x0001 /* mouse move */
#define MOUSEEVENTF_LEFTDOWN    0x0002 /* left button down */
#define MOUSEEVENTF_LEFTUP      0x0004 /* left button up */
#define MOUSEEVENTF_RIGHTDOWN   0x0008 /* right button down */
#define MOUSEEVENTF_RIGHTUP     0x0010 /* right button up */
#define MOUSEEVENTF_MIDDLEDOWN  0x0020 /* middle button down */
#define MOUSEEVENTF_MIDDLEUP    0x0040 /* middle button up */
#define MOUSEEVENTF_WHEEL       0x0800 /* wheel button rolled */
#define MOUSEEVENTF_ABSOLUTE    0x8000 /* absolute move */

#define KEYEVENTF_EXTENDEDKEY 0x0001
#define KEYEVENTF_KEYUP       0x0002

typedef struct{
  long    dx;
  long    dy;
  unsigned long   mouseData;
  unsigned long   dwFlags;
  unsigned long   time;
  unsigned long   dwExtraInfo;
}VMOUSEINPUT;

typedef struct{
  unsigned short    wVk;
  unsigned short    wScan;
  unsigned long     dwFlags;
  unsigned long     time;
  unsigned long     dwExtraInfo;
}VKEYBDINPUT;

typedef struct{
  unsigned long     uMsg;
  unsigned short    wParamL;
  unsigned short    wParamH;
}VHARDWAREINPUT;

typedef struct tagVINPUT {
	unsigned long type;
	union{
		VMOUSEINPUT mi;
		VKEYBDINPUT ki;
		VHARDWAREINPUT hi;
		}u;
}VINPUT;

#ifdef __cplusplus
extern "C" {
#endif

int VinputInit(int (*input_event_handler)(VINPUT *event));
void VinputExit ();

#ifdef __cplusplus
}
#endif


#endif //_VINPUT_H_

