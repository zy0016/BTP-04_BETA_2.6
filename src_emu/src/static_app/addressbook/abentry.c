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

#include "abglobal.h"

static HINSTANCE   hAddressBookInstance;
static HWND        hAbookFrameWnd = NULL;
static BOOL        bSpecial;
/*********************************************************************\
* Function	   ADDRESSBOOK_AppControl
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
DWORD ADDRESSBOOK_AppControl(int nCode, void* pInstance, WPARAM wParam, LPARAM lParam)
{
    DWORD   dwRet;
    BOOL    bShow;
    
    dwRet = TRUE;

    switch (nCode)
    {
    case APP_INIT:
            
        mkdir(PATH_DIR_AB,S_IRUSR);

        AB_MaybeDelete();

        hAddressBookInstance = (HINSTANCE)pInstance;     
        
        AB_InitStirng();

        AB_PickRegisterClass();

        AB_EditRegisterClass();

		AB_MultiPickRegisterClass();

		AB_ContactPickRegisterClass();

        AB_MultiNoOrEmailPickRegisterClass();

        AB_MultiSelectPickRegisterClass();

        AB_SaveContactRegisterClass();

        AB_SelectFieldRegisterClass();

        AB_QDialRegisterClass();

        AB_SetQDialRegisterClass();
        
        AB_InitData();
        
        AB_InitSIM();
        
        break;

    case APP_INITINSTANCE :
			
        break;
        
        
    case APP_GETOPTION:
        
        switch(wParam)
        {
        case AS_APPWND:
            
            dwRet = (DWORD)hAbookFrameWnd;
            
            break;
        case AS_STATE:
            
            if (hAbookFrameWnd)
            {
                if (IsWindowVisible(hAbookFrameWnd))
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
        if (IsWindow(hAbookFrameWnd))    
        {     
            if(wParam == ADBOOK_APP)
            {
                AB_SetFlag(TRUE);

                DestroyWindow(hAbookFrameWnd);
                
                hAbookFrameWnd = CreateFrameWindow(WS_CAPTION | PWS_STATICBAR);
                
                return AB_CreateMainWindow(hAbookFrameWnd,hAddressBookInstance);
            }
            else
            {
                AB_SetFlag(FALSE);

                ShowWindow(hAbookFrameWnd, SW_SHOW);
                
                ShowOwnedPopups(hAbookFrameWnd, SW_SHOW);
                
                UpdateWindow(hAbookFrameWnd);
            }
        }
        else
        {    
            //StartObjectDebug();
            BOOL rtn;

            if(wParam == ADBOOK_APP)
            {
                AB_SetFlag(TRUE);
            }
            else
            {
                AB_SetFlag(FALSE);
            }
            hAbookFrameWnd = CreateFrameWindow(WS_CAPTION | PWS_STATICBAR);

            WaitWindowStateEx(NULL, TRUE, IDS_OPENING, NULL, NULL, IDS_CANCEL);
            rtn = AB_CreateMainWindow(hAbookFrameWnd,hAddressBookInstance);
			WaitWindowStateEx(NULL, FALSE, IDS_OPENING, NULL, NULL, IDS_CANCEL);

			return rtn;

        }
        break;
            
    case APP_INACTIVE :
        
        ShowOwnedPopups(hAbookFrameWnd, SW_HIDE);

        ShowWindow(hAbookFrameWnd, SW_HIDE);
        
        bShow = FALSE;
        
        break;

    }

    return dwRet;
}

HWND AB_GetFrameWnd(void)
{
    if(IsWindow(hAbookFrameWnd))
        return hAbookFrameWnd;
    else
        return NULL;
}

void AB_SetFlag(BOOL bSpec)
{
    bSpecial = bSpec;
}

BOOL AB_GetFlag(void)
{
    return bSpecial;
}
