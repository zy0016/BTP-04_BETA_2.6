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

#include "muglobal.h"

HINSTANCE   hMUInstance;
//static HWND        hMUMainWnd = NULL;

MU_INITSTATUS   mu_initstatus;
MU_IEmailbox    *mu_emailbox_interface;
MU_IMessage *mu_mms_interface;
MU_IMessage *mu_sms_interface;
MU_IMessage *mu_push_interface;
MU_IMessage *mu_email_interface;
MU_IMessage *mu_file_interface;
MU_ISMSEx   *mu_smsex_interface;
MU_IMessage *mu_bt_interface;

HWND hMuFrame;
static HWND hWndHide;

//extern function
extern HWND MU_CreateMainWindow(HINSTANCE hInstance);
extern LRESULT MUListWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
extern BOOL SelectMailBox(void);
extern BOOL mailbox_read_default(HWND hwndmu);

BOOL EnterInBox(void);
BOOL EnterMailBox(void);
BOOL EnterDefaultMailBox(void);
BOOL bDirectExit = FALSE;
static LRESULT HideProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
/*********************************************************************\
* Function	   MessageUnibox_AppControl
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
DWORD MessageUnibox_AppControl(int nCode, void* pInstance, WPARAM wParam, LPARAM lParam)
{
    DWORD   dwRet;
    BOOL    bShow;
    WNDCLASS wc;
    
    dwRet = TRUE;

    switch (nCode)
    {
    case APP_INIT:

        hMUInstance = (HINSTANCE)pInstance; 
        
        //create dir
        mkdir(PATH_DIR_MSG,S_IRWXU);
        mkdir(PATH_DIR_MU,S_IRWXU);
                
        //registration
#ifdef _TEST_SMS_
        mu_initstatus.bSMS = sms_register(&mu_sms_interface);
        smsex_register(&mu_smsex_interface);
		printf("sms_register\r\n");
#else
        mu_initstatus.bSMS = FALSE;
#endif

#ifdef  _TEST_BLUETOOTH_
		mu_initstatus.bBT = bt_register(&mu_bt_interface);
		printf("bt_register\r\n");
#else
		mu_initstatus.bBT = FALSE;
#endif
		
#ifdef _TEST_MMS_
        mu_initstatus.bMMS = mms_register(&mu_mms_interface);
		printf("mms_register\r\n");
#else
        mu_initstatus.bMMS = FALSE;
#endif

#ifdef _TEST_EMAIL_
        mu_initstatus.bEmail = email_register(&mu_email_interface);
        emailbox_register(&mu_emailbox_interface);
		printf("email_register\r\n");
#else
        mu_initstatus.bEmail = FALSE;
#endif

#ifdef _TEST_PUSH_
        mu_initstatus.bPush = push_register(&mu_push_interface);
#else
        mu_initstatus.bPush = FALSE;
#endif
        
        //initialization
        if(mu_initstatus.bSMS)
            mu_sms_interface->msg_init();
        if(mu_initstatus.bMMS)
            mu_mms_interface->msg_init();
        if(mu_initstatus.bEmail)
            mu_email_interface->msg_init();
        if(mu_initstatus.bPush)
            mu_push_interface->msg_init();
        if(mu_initstatus.bBT)
			mu_bt_interface->msg_init();
		
        wc.style         = 0;
        wc.lpfnWndProc   = MUListWndProc;
        wc.cbClsExtra    = 0;
        wc.cbWndExtra    = sizeof(MU_LISTCREATEDATA);
        wc.hInstance     = NULL;
        wc.hIcon         = NULL;
        wc.hCursor       = NULL;
        wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
        wc.lpszMenuName  = NULL;
        wc.lpszClassName  = "MUListWndClass";
        
        if (!RegisterClass(&wc))
            return FALSE;

		wc.style         = 0;
        wc.lpfnWndProc   = HideProc;
        wc.cbClsExtra    = 0;
        wc.cbWndExtra    = 0;
        wc.hInstance     = NULL;
        wc.hIcon         = NULL;
        wc.hCursor       = NULL;
        wc.hbrBackground = 0;
        wc.lpszMenuName  = NULL;
        wc.lpszClassName  = "MUHideWndClass";
        
        if (!RegisterClass(&wc))
            return FALSE;

		hWndHide = CreateWindow("MUHideWndClass", "", 0, 0,0,0,0, NULL, NULL, NULL, NULL);

        break;

    case APP_INITINSTANCE :
			
        break;
        
        
    case APP_GETOPTION:
        
        switch(wParam)
        {
        case AS_APPWND:
            
            dwRet = (DWORD)hMuFrame;
            
            break;

        case AS_STATE:

            if (hMuFrame)
            {
                if (IsWindowVisible(hMuFrame))
                {
                    dwRet = AS_ACTIVE;
                }
                else
                {
                    dwRet = AS_INACTIVE;
                }
            }
            else
            {
                dwRet = AS_NOTRUNNING;
            }

            break;

        case AS_HELP:
                        
            break;
        }

        break;
   
    case APP_ACTIVE :
//		StartObjectDebug();
		if(wParam == MSG_APP || (wParam == 0 && lParam == 1))
		{
			return EnterInBox();
		}

        if(wParam == 0 && lParam == 2)
        {
            return EnterDefaultMailBox();
        }

		if(wParam == MAIL_APP)
		{
			return EnterMailBox();	
		}

		bDirectExit = FALSE;
		
		if(IsWindow(hMuFrame))
		{
			ShowWindow(hMuFrame,SW_SHOW);

            //ShowOwnedPopups(hMuFrame,SW_SHOW);

            UpdateWindow(hMuFrame);
		}
		else
		{
			
			if(SMS_Opening()==FALSE && GetSIMState())
            {
                DlmNotify((WPARAM)PES_STCQUIT, (LPARAM)hMUInstance);
                //PLXTipsWin(hMuFrame, NULL, 0, IDS_INITING,IDS_SMS,Notify_Info,IDS_OK,NULL,WAITTIMEOUT);
                break;
            }
            
            hMuFrame = MU_CreateMainWindow(hMUInstance);
                        
            if (!hMuFrame)
            {
                return FALSE;
            }
		}
		
		if(GetSIMState() == 0)
		{
			PLXTipsWin(NULL, NULL, 0, (char*)IDS_INSERT_SIM, "", 
				Notify_Alert, (char*)IDS_OK, "", WAITTIMEOUT);
		}
        break;
            
    case APP_INACTIVE :
        
        //ShowOwnedPopups(hMuFrame,SW_HIDE);

        ShowWindow(hMuFrame,SW_HIDE);
    
        bShow = FALSE;
        
		bDirectExit = FALSE;
        break;

    }

    return dwRet;
}

BOOL EnterInBox(void)
{
	int n;

	if(IsWindow(hMuFrame))
	{
		SendMuChildMessage(PWM_MSG_FORCE_CLOSE, 0, 0);
		DestroyWindow(hMuFrame);
		hMuFrame = NULL;
	}

	if(SMS_Opening()==FALSE && GetSIMState())
	{
		//PLXTipsWin(hMuFrame, NULL, 0, IDS_INITING,IDS_SMS,Notify_Info,IDS_OK,NULL,WAITTIMEOUT);
		return TRUE;
	}
	
	hMuFrame = MU_CreateMainWindow(hMUInstance);
	
	if (!hMuFrame)
	{
		return FALSE;
	}

	bDirectExit = TRUE;

	n = MU_INBOX;

	MU_CreateList(NULL, &n, FALSE);
	
	return TRUE;
}

BOOL EnterMailBox(void)
{
	if(IsWindow(hMuFrame))
	{
		SendMuChildMessage(PWM_MSG_FORCE_CLOSE, 0, 0);
		DestroyWindow(hMuFrame);
		hMuFrame = NULL;
	}

	if(SMS_Opening()==FALSE && GetSIMState())
	{
		//PLXTipsWin(hMuFrame, NULL, 0, IDS_INITING,IDS_SMS,Notify_Info,IDS_OK,NULL,WAITTIMEOUT);
		return TRUE;
	}
	
	hMuFrame = MU_CreateMainWindow(hMUInstance);
	
	if (!hMuFrame)
	{
		return FALSE;
	}

	bDirectExit = TRUE;
	
	SelectMailBox();
	
	return FALSE;
}

BOOL EnterDefaultMailBox(void)
{
	if(IsWindow(hMuFrame))
	{
		SendMuChildMessage(PWM_MSG_FORCE_CLOSE, 0, 0);
		DestroyWindow(hMuFrame);
		hMuFrame = NULL;
	}

	if(SMS_Opening()==FALSE && GetSIMState())
	{
		return TRUE;
	}
	
	hMuFrame = MU_CreateMainWindow(hMUInstance);
	
	if (!hMuFrame)
	{
		return FALSE;
	}

	bDirectExit = TRUE;
	
	return mailbox_read_default(hMuFrame);
}

static LRESULT HideProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult;

	switch(wMsgCmd) 
	{
	default:
		lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
	}

	return lResult;
}
