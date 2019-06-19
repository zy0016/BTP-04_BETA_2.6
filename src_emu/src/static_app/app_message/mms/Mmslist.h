/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : MMS
 *
 * Purpose  : define the MACRO and function
 *            
\**************************************************************************/

#ifndef  MMS_LIST_H
#define  MMS_LIST_H

#include "MmsGlobal.h"
#include "MmsInterface.h"
/******************************************************************** 
* 
*		constant definition
* 
**********************************************************************/

#define MMS_LIST_TRANSMIT   WM_USER + 31
#define MMS_LIST_EDIT       WM_USER + 32

#define MMS_LIST_DISP       WM_USER + 33

#define CALL_DISP_DRAFT     0
#define CALL_DISP_REST      1

#define IDC_EXIT			101
#define IDM_INFOBACK        102
#define IDM_INFORECV        103

#define IDM_SEE				110
#define IDM_DOWNLOAD		111
#define IDM_DELETE			112
#define IDM_SEND			115
#define	IDM_RECV			116
#define	IDM_DELALL			117
#define IDM_CANCEL          119

#define IDM_RECV_REPLY		113
#define IDM_RECV_REPLYALL	114
#define IDM_TRANSMIT		118
#define IDM_REPLY			119
#define IDM_SAVETEMP        120
/* list ID */
#define	IDC_MMS_LIST		401
/******************************************************************** 
* 
*		string define
* 
**********************************************************************/
#define STR_MMS_NOTIFICATION	 ML("Notification")
#define STR_MMS_TYPE			 ML("Type:")
#define STR_MMS_FROM1            ML("From:")
#define STR_MMS_NOSUBJECT		 ML("No Subject")
#define STR_MMS_SIZE             ML("Size:")
#define STR_MMS_EXPIRY           ML("Expiry:")
#define STR_MMS_NONE             ML("None")
#define STR_MMS_TO1              ML("To:")

#define IDS_SUBJECT				 ML("Subject:")
#define IDS_DATE                 ML("Date:")
#define IDS_TIME				 ML("Time:")
#define IDS_TYPE                 ML("Type:")
#define IDS_SIZE			     ML("Size:")
#define STR_MMS_DSTATUS			 ML("Delivery status")
#define STR_MMS_RECIPIENT_TITLE		 ML("Recipient")
#define STR_MMS_TIME_TITLE			 ML("Time")
#define STR_MMS_DATE_TITLE			 ML("Date")
#define MAX_TITLE		   18


#ifdef SEF_BAIYUNSHAN
    #define OUT_START           2
    #define OUT_SEP             18
    #define OUT_X               50
    #define LINE_LEN            12
#endif

    #define OUT_START           2
    #define OUT_SEP             20
    #define OUT_X               62
    #define CLIENT_HEIGHT       LIST_HEIGHT
    #define LINE_LEN            12
/******************************************************************** 
* 
*		function definition
* 
**********************************************************************/
static LRESULT InfoWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);

#endif
