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


#ifndef _TFTP_SERVER_H_
#define _TFTP_SERVER_H_


/*-------------------------------------------------
 *
 * Define Param
 * DWORD PdaSvr_Control(int nCode, void* pInstance, WPARAM wParam, LPARAM lParam)
 *
 *----------------------------------------------------*/

//nCode == APP_ACTIVE, Define wParam as AUTHENTICATION Type

#define	AUTHEN_COMMON		0	//普通文件传输
#define	AUTHEN_JAVA			1	//JAVA文件传输


//nCode == APP_SETOPTION, Define wParam as OPERATION LOCK FLAG for PUT
//At this time, lParam = 0:means permit PUT
//				lParam = 1:means forbid PUT
#define	TFTP_PUT_LOCK	0


//nCode == APP_GETOPTION, Define wParam as a flag of com init result
// when return value == 0, Com Init Success
//		return value < 0, Com Init Failure
#define AS_TFTP_COMREADY		100


/*------------------------------------------------------
 *
 * Define Java Notify Message
 * BOOL NotifyReceivedFile(short status, short detail,  const char *fileName);
 *
 *----------------------------------------------------*/
//#define status
#define TFTPSVR_FAILURE		0
#define TFTPSVR_SUCCESS		1
#define TFTPSVR_USERSTOP	2

//#define detail
//reserve

#endif //_TFTP_SERVER_H_
