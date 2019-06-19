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

#include "smsglobal.h"

static HINSTANCE   hWriteSMSInstance;
static HWND        hWriteSMSFrameWnd = NULL;
/*********************************************************************\
* Function	   WriteSMS_AppControl
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
DWORD WriteSMS_AppControl(int nCode, void* pInstance, WPARAM wParam, LPARAM lParam)
{
    DWORD   dwRet;
    BOOL    bShow;
    
    dwRet = TRUE;

    switch (nCode)
    {
    case APP_INIT:
            
        mkdir(PATH_DIR_SMS,S_IRWXU);

        hWriteSMSInstance = (HINSTANCE)pInstance;     
                
        break;

    case APP_INITINSTANCE :
			
        break;
        
        
    case APP_GETOPTION:
        
        switch(wParam)
        {
        case AS_APPWND:
            
            dwRet = (DWORD)hWriteSMSFrameWnd;
            
            break;
        case AS_STATE:
            
            if (hWriteSMSFrameWnd)
            {
                if (IsWindowVisible(hWriteSMSFrameWnd))
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
        if (IsWindow(hWriteSMSFrameWnd))    
        {     
            ShowWindow(hWriteSMSFrameWnd, SW_SHOW);
            
            ShowOwnedPopups(hWriteSMSFrameWnd, SW_SHOW);
            
            UpdateWindow(hWriteSMSFrameWnd);
        }
        else
        {    
            //StartObjectDebug();
            
            if( SMS_Opening() == TRUE )
            {
                BOOL bOK;
                
                hWriteSMSFrameWnd = CreateFrameWindow(WS_CAPTION | PWS_STATICBAR);
                
                bOK = APP_EntryEditSMS(hWriteSMSFrameWnd,(char*)lParam,hWriteSMSInstance);

                if(bOK == FALSE)
                {
                    DlmNotify((WPARAM)PES_STCQUIT, (LPARAM)hWriteSMSInstance);
                    DestroyWindow(hWriteSMSFrameWnd);
                }

                return bOK;
            }
            else
            {
                DlmNotify((WPARAM)PES_STCQUIT, (LPARAM)hWriteSMSInstance);
                return FALSE;
            }
        }
        break;
            
    case APP_INACTIVE :
        
        ShowOwnedPopups(hWriteSMSFrameWnd, SW_HIDE);

        ShowWindow(hWriteSMSFrameWnd, SW_HIDE);
        
        bShow = FALSE;
        
        break;

    }

    return dwRet;
}



static HINSTANCE   hInstantViewSMSInstance;
static HWND        hInstantViewSMSFrameWnd = NULL;
/*********************************************************************\
* Function	   InstantViewSMS_AppControl
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
DWORD InstantViewSMS_AppControl(int nCode, void* pInstance, WPARAM wParam, LPARAM lParam)
{
    DWORD   dwRet;
    BOOL    bShow;
    
    dwRet = TRUE;

    switch (nCode)
    {
    case APP_INIT:
            
        hInstantViewSMSInstance = (HINSTANCE)pInstance;     
                
        break;

    case APP_INITINSTANCE :
			
        break;
        
        
    case APP_GETOPTION:
        
        switch(wParam)
        {
        case AS_APPWND:
            
            dwRet = (DWORD)hInstantViewSMSFrameWnd;
            
            break;
        case AS_STATE:
            
            if (hInstantViewSMSFrameWnd)
            {
                if (IsWindowVisible(hInstantViewSMSFrameWnd))
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
        if (IsWindow(hInstantViewSMSFrameWnd))    //Update New Message
        {   
            HWND hWnd;

            ShowWindow(hInstantViewSMSFrameWnd, SW_SHOW);
            
            ShowOwnedPopups(hInstantViewSMSFrameWnd, SW_SHOW);
            
            UpdateWindow(hInstantViewSMSFrameWnd);

            hWnd = GetDlgItem(hInstantViewSMSFrameWnd,IDC_CLASS0_VIEW);

            if(SMS_UpdateView(hWnd) == FALSE)
                PostMessage(hWnd,WM_CLOSE,NULL,NULL);
        }
        else
        {       
            BOOL bOK;

            hInstantViewSMSFrameWnd = CreateFrameWindow(WS_CAPTION | PWS_STATICBAR);
            
            bOK = SMS_CreateClass0Wnd(hInstantViewSMSFrameWnd,hInstantViewSMSInstance);

            if(bOK == FALSE)
                DestroyWindow(hInstantViewSMSFrameWnd);

            return bOK;
        }
        break;
            
    case APP_INACTIVE :
        
        ShowOwnedPopups(hInstantViewSMSFrameWnd, SW_HIDE);

        ShowWindow(hInstantViewSMSFrameWnd, SW_HIDE);
        
        bShow = FALSE;
        
        break;

    }

    return dwRet;
}
