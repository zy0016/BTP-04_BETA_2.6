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


#ifndef _PPP_H_
#define _PPP_H_

/**************************
		macro
***************************/
/*============================================
for op_code parameter of LCP_Setting function
==============================================*/
#define LCP_PASSIVE 				0x101
#define LCP_SLIENT					0x102
#define LCP_NO_OPTION				0x103
#define LCP_ASYNCMAP				0x104
#define LCP_NO_MAGICNUMBER			0x105
#define LCP_NO_MRU					0x106
#define LCP_MRU						0x107
#define LCP_MTU						0x108
#define LCP_COMP_NOACCOMP			0x109
#define LCP_COMP_NOPCOMP			0x10a
#define LCP_TIMER_TIMEOUT			0x10b
#define LCP_COUNTER_TERM			0x10c
#define LCP_COUNTER_CONF			0x10d
#define LCP_COUNTER_FAIL			0x10e
#define LCP_ECHO_INTERVAL			0x10f
#define LCP_ECHO_FAILS				0x110

/*============================================
for op_code parameter of UPAP_Setting function
==============================================*/

#define PAP_REQ_PAP				0x201	
#define PAP_NAME				0x202
#define PAP_PASSWORD			0x203
#define PAP_TIMER_TIMEOUT		0x204
#define PAP_TIMER_REQ_TIMEOUT	0x205
#define PAP_COUNTER_REQS		0x206

/*============================================
for op_code parameter of CHAP_Setting function
==============================================*/

#define CHAP_REQ_CHAP					0x301
#define CHAP_LOCAL_NAME					0x302
#define CHAP_REMOTE_NAME				0x303
#define CHAP_TIMER_CHALLENGE_INERVAL	0x304
#define CHAP_TIMER_TIMEOUT				0x305
#define CHAP_COUNTER_CHALLENGE			0x306

/*=============================================
for op_code parameter of IPCP_Setting function
===============================================*/

#define IPCP_NO_DEFAULT_IPADDR		0x401
#define IPCP_NO_IPADDR				0x402
#define IPCP_LOCAL_IPADDR			0x403	
#define IPCP_REMOTE_IPADDR			0x404
#define IPCP_NET_MASK				0x405
#define IPCP_DEFAULT_ROUTER			0x406	
#define IPCP_DNS					0x407
#define IPCP_COMP_NO_VJ				0x408
#define IPCP_COMP_NO_CONNECTID_VJ	0x409
#define IPCP_COMP_VJ_SLOTS			0x40a
#define IPCP_TIMER_TIMEOUT			0x40b
#define IPCP_COUNTER_MAX_TERM		0x40c
#define IPCP_COUNTER_MAX_REQ		0x40d
#define IPCP_COUNTER_MAX_FAIL		0x40e
#define IPCP_NO_DEFAULT_ROUTER		0X40F

/*==============================================
for op_code parameter of UTILS_Setting function
=================================================*/

#define SET_DEBUG			0x601
#define SET_KERNAL_DEBUG	0x602
#define SET_AUTH			0x603
#define SET_SCAPE			0x604
#define SET_DOMAIN			0x605
#define SET_PERSIST			0x606
#define SET_USE_HOSTNAME	0x607
#define SET_ROLE			0x608

/*===================
Return values of interface functions
====================*/

#define ERR_SUCCESS				0
#define ERR_INVALID_PARAM		1
#define ERR_INVALID_VALUE		2
#define ERR_SYS					3
#define ERR_STATE				4

/*====================================================
states of pppd thread. When pppd thread terminates with
state error, such as returns STATE_ERR_SYS or 
STATE_ERR_NEGOTIATION, the user program need to process 
it further. 
======================================================*/
#define STATE_NO_START				0
#define STATE_START					1
#define STATE_ERR_SYS				2
#define STATE_ERR_NEGOTIATION		3
#define STATE_BEGIN_NEGOTIATING		4
#define STATE_LCP_UP				5
#define STATE_LCP_DOWN				6
#define STATE_PAP_UP				7
#define STATE_PAP_DOWN				8
#define STATE_IPCP_UP				9
#define STATE_IPCP_DOWN				10
#define STATE_THREAD_DOWN			11

typedef void (*StateShowingFun)(int state);

/*====================
Functions prototype
=====================*/
int PPP_Setting					(int op_code,int yes,void *param);
int PPP_Start					(int handle, int bxonxoff);
int PPP_Run						(void);
int PPP_Stop					(void);
int PPP_RegStateShowingFun		(StateShowingFun pf);
#endif
