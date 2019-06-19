/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : MMS
 *
 * Purpose  : define
 *            
\**************************************************************************/

#ifndef   MMSSETTING_
#define   MMSSETTING_

#include "MmsGlobal.h"
/******************************************************************** 
*               constant define    
**********************************************************************/
#define STR_MMS_SET             ML("MMS Settings")
#define STR_MMS_CONN            ML("Connection")
#define STR_MMS_HOMENET         ML("In home network")
#define STR_MMS_VISITNET        ML("In visiting network") 
#define STR_MMS_ANONY           ML("Allow anonymous")
#define STR_MMS_ADVERT          ML("Allow adverts")
#define STR_MMS_REPORT_RECEP    ML("Report reception")
#define STR_MMS_REPORT_SEND     ML("Report sending")
#define STR_MMS_VALID           ML("Message Validity")
#define STR_MMS_PICSIZE         ML("Picture size")

#define STR_MMS_REQUIRE         ML("Required")
#define STR_MMS_REC_ATONCE      ML("Receive at once")
#define STR_MMS_REC_CONFIRM     ML("Confirm reception")
#define STR_MMS_REC_NEVER       ML("Never")
#define STR_MMS_ON              ML("On")
#define STR_MMS_OFF             ML("Off")
#define STR_MMS_1HOUR           ML("1 hour")
#define STR_MMS_6HOUR           ML("6 hours")
#define STR_MMS_24HOUR          ML("24 hours")
#define STR_MMS_1WEEK           ML("1 week")
#define STR_MMS_MAXTIME         ML("Maximum time")
#define STR_MMS_SMALL           ML("Small")
#define STR_MMS_LARGE           ML("Large")
/******************************************************************** 
*               Datastructure define
**********************************************************************/
typedef struct tagMMS_SETCreateData
{
	HWND      hWndFrame;
    HWND      hFocus;
    int       nCurFocus;
	int		  iRepeatType;	  
}MMS_SETCREATEDATA,*PMMS_SETCREATEDATA;

typedef struct tagMMSSET_SELCreateData
{
	HWND      hWndFrame;
    HWND      hWndCall;
    int       msg;
    int       id;
    int       nCurSel;
	HBITMAP   hBmpSel;
	HBITMAP   hBmpNormal;
}MMSSET_SELCREATEDATA,*PMMSSET_SELCREATEDATA;
/******************************************************************** 
*               function statement     
**********************************************************************/
LRESULT MMSSetWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);

extern MmsWndClass MMSWindowClass[MAX_CLASS_NUM];

#endif
