/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Pollex E-mail System Program
 *
 * Purpose  : implement e-mail management    
 *            
\**************************************************************************/

#ifndef _MAIL_ADDRACNT_H_
#define _MAIL_ADDRACNT_H_

#ifdef __cplusplus
extern "C" {
#endif

/*************************************************************************\
*
*	常量定义
*
\*************************************************************************/

#define ADDR_LEN			40

/* 定义返回值 */
#define ADDR_SUCCESS		1
#define ADDR_FAILURE		0

/*************************************************************************\
*
*	数据结构及数据类型定义
*
\*************************************************************************/

/* 定义地址簿数据结构 */
typedef struct
{
	char name [ADDR_LEN + 1];
	char phone[ADDR_LEN + 1];
	char addr [ADDR_LEN + 1];

}	MAILADDR;

/* 定义帐号类数据结构 */
typedef struct
{
	char username[ADDR_LEN + 1];
	char password[ADDR_LEN + 1];
	char pop3svr [ADDR_LEN + 1];
	char imap4svr[ADDR_LEN + 1];
	char smtpsvr [ADDR_LEN + 1];
	char retaddr [ADDR_LEN + 1];
	char leaveinserver;

}	MAILACCOUNT;

/*************************************************************************\
*
*	函数声明
*
\*************************************************************************/

/* 地址簿类成员函数 */
int		ADDR_Create(void);
int		ADDR_Destroy(int);
int		ADDR_Append(int, MAILADDR*);
int		ADDR_Remove(int, int);
int		ADDR_Modify(int, int, MAILADDR*);
int		ADDR_GetNextAddr(int, int, MAILADDR*);
int		ADDR_GetAddrInfo(int, int, MAILADDR*);

/* 帐号类成员函数 */
MAILACCOUNT*ACNT_CreateAccount(void);
void	ACNT_DestroyAccount(MAILACCOUNT*);

#ifdef __cplusplus
} //extern "C" {
#endif
/************* End Of Head File ******************************************/
#endif /* _MAIL_ADDRACNT_H_ */
