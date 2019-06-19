/***************************************************************************
*
*                      Pollex Mobile Platform
*
* Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
*                       All Rights Reserved 
*
* Module   : MailEntry.c
*
* Purpose  : 
*
\**************************************************************************/

#include "MailHeader.h"

static HINSTANCE   hWriteEMAILInstance;
static HWND        hWriteEMAILFrameWnd = NULL;

extern BOOL CreateMailEditWnd(HWND hParent, const char *PSZRECIPIENT, const char *PSZRECIPIENT2, 
                       const char *PSZTITLE, const char *PSZCONTENT, const char *PSZATTENMENT, 
                       const SYSTEMTIME *PTIME, DWORD handle, int folderid);
/*********************************************************************\
* Function	   WriteEMAIL_AppControl
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
DWORD EditMail_AppControl(int nCode, void* pInstance, WPARAM wParam, LPARAM lParam)
{
    DWORD   dwRet;
    BOOL    bShow;
    
    dwRet = TRUE;

    switch (nCode)
    {
    case APP_INIT:
        { 
            mkdir(MAIL_FILE_PATH, S_IRWXU);
            hWriteEMAILInstance = (HINSTANCE)pInstance;
        }    
        break;

    case APP_INITINSTANCE :	
        break;
        
    case APP_ALARM:
        {
            switch(wParam)
            {
            case ALMN_TIMEOUT:
                {
                    SendMessage(HwndMailGet, WM_RTCTIMER, lParam, 0);
                }
                break;
                
            case ALMN_TIMECHANGED:
                {
                }
                break;
                
            case ALMN_SETERROR:
                {
                }
                break;
                
            default:
                break;
            }
        }
        break;
        
    case APP_GETOPTION:
        {
            switch(wParam)
            {
            case AS_APPWND:
                {
                    dwRet = (DWORD)hWriteEMAILFrameWnd;
                }
                break;
            case AS_STATE:
                {
                    if (hWriteEMAILFrameWnd)
                    {
                        if (IsWindowVisible(hWriteEMAILFrameWnd))
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
                }
                break;
                
            case AS_HELP:
                break;
            }
        }
        break;
   
    case APP_ACTIVE:
        if (IsWindow(hWriteEMAILFrameWnd))    
        {     
            ShowWindow(hWriteEMAILFrameWnd, SW_SHOW);
            ShowOwnedPopups(hWriteEMAILFrameWnd, SW_SHOW);
            UpdateWindow(hWriteEMAILFrameWnd);
        }
        else
        {
            SYSTEMTIME Sy;
            BOOL bOK;

            hWriteEMAILFrameWnd = CreateFrameWindow(WS_CAPTION | PWS_STATICBAR);
            GetLocalTime(&Sy);
            bOK = CreateMailEditWnd(hWriteEMAILFrameWnd, NULL, NULL, NULL, NULL,
                NULL, &Sy, -2, (int)hWriteEMAILInstance);
            
            if(bOK == FALSE)
            {
                DestroyWindow(hWriteEMAILFrameWnd);
            }
            return bOK;
        }
        break;
            
    case APP_INACTIVE:
        {
            ShowOwnedPopups(hWriteEMAILFrameWnd, SW_HIDE);
            ShowWindow(hWriteEMAILFrameWnd, SW_HIDE);
            bShow = FALSE;
        }
        break;
    }

    return dwRet;
}
