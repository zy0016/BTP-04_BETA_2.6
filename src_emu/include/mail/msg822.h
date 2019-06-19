/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Pollex E-mail System Program
 *
 * Purpose  : construct RFC822 formed internet message
 *            
\**************************************************************************/

#ifndef _MAIL_MSG822_H_
#define _MAIL_MSG822_H_

#ifdef __cplusplus
extern "C" {
#endif

/*************************************************************************\
*
*	常量定义
*
\*************************************************************************/

/* 定义模块返回值 */
#define MSG822_SUCCESS		0
#define MSG822_FAILURE		1

/* 定义RFC822字段名称 */
#define MSG822_From			(1 << 0)
#define MSG822_To			(1 << 1)
#define MSG822_Cc			(1 << 2)
#define MSG822_Subject		(1 << 3)
#define MSG822_ReplyTo		(1 << 4)
#define MSG822_RetPath		MSG822_ReplyTo
/* Revision Q(4) */
#define MSG822_Date			(1 << 5)

/*************************************************************************\
*
*	数据结构及数据类型定义
*
\*************************************************************************/

/*************************************************************************\
*
*	函数声明
*
\*************************************************************************/

int	MSG822_Create(int fh);
int MSG822_Destroy(int handle);
int MSG822_SetHeader(int handle, int field, char* content);
int MSG822_SetText(int handle, char* text);
int MSG822_AttachText(int handle, char* text, char* charset);
int MSG822_AttachFile(int handle, char* filename, char* name, char* type, 
					  char* charset);
int MSG822_AttachMessage(int handle, char* filename);
int MSG822_Complete(int handle);

#ifdef __cplusplus
} //extern "C" {
#endif
/************* End Of Head File ******************************************/
#endif /* _MAIL_MSG822_H_ */
