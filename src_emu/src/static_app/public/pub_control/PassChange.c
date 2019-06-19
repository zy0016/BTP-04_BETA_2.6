/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Pass Change
 *
 * Purpose  : 
 *
\**************************************************************************/

#include "window.h"
#include "string.h"
#include "winpda.h"
#include "str_public.h"
#include "str_plx.h"
//#include "hpime.h"
#include "imesys.h"
#include "plx_pdaex.h"
#include "pubapp.h"
#include "mullang.h"
#include "malloc.h"

#ifndef _PASSCHG_
#define _PASSCHG_
#endif

#define PC_EXITLOOP     WM_USER + 100

#define IDS_OLDPASS     "old password:"
#define IDS_NEWPASS     "new password:"
#define IDS_CONFIRMPASS "confirm:"
#define IDS_NEWPASSERR  "Passwords not match"
#define IDS_OLDPASSERR  "wrong old password"
#define IDS_EXIT        ML("Exit")
#define IDS_CONFIRM     ML("OK")

#define IDM_EXIT		WM_USER+1001  //取消
#define IDM_CONFIRM		WM_USER+1002  //确定
#define IDC_EDITOLD     1
#define IDC_EDITNEW     2
#define IDC_EDITCONF    3

#define EDITWIDTH       150 //220
#define EDITHEIGHT      25
#define TEXTWIDTH       150 //220
#define TEXTHEIGHT      25

//Define data structure for passcheck control
typedef struct tagPASSCHGDATA
{
    PSTR    szCaption;
    PSTR    szPwdOld;
    PSTR    szPwdNew;
    PSTR    szOk;
    PSTR    szCancel;
    int     nNewSizeMin;
    int     nNewSizeMax;
    int     nOldSizeMin;
    int     nOldSizeMax;
    BOOL    bClear;
    BOOL    bConfirm;
    //int     nSizeOld;
    //int     nSizeNew;
} PASSCHGDATA , *PPASSCHGDATA;

static WNDPROC    OldEditProc;

BOOL    CreatePassChgBox(PPASSCHGDATA pPassChgData);
LRESULT CALLBACK PassChg_WndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam,
                              LPARAM lParam);
static LRESULT CALLBACK MyEdit_WndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam,
                              LPARAM lParam);

/*********************************************************************\
* Function: ChangePassword.  
* Purpose : Get the new password,called by user.      
* Params  : PSTR oldpw ---- the old password
            int oldpwlen ---- length of the old password
            PSTR newpw ---- the new password
            int newpwlen ---- length of the new password
* Return  : 		   
* Remarks :	   
**********************************************************************/
BOOL PLXModifyPassword(PCSTR szCaption, PSTR szPwdOld, DWORD dwOldSizeRange, //int nSizeOld, 
                      PSTR szPwdNew, /*int nSizeNew*/DWORD dwNewSizeRange, PCSTR szOk, 
                      PCSTR szCancel)

{
    PASSCHGDATA PassData;
    BOOL        bResult = FALSE;

#ifdef _PASSCHG_
    //  //  //  StartObjectDebug();
#endif

    PassData.szCaption = (PSTR)szCaption;
    PassData.szOk      = (PSTR)szOk;
    PassData.szCancel  = (PSTR)szCancel;
    PassData.szPwdOld  = szPwdOld;
    PassData.szPwdNew  = szPwdNew;
    PassData.nOldSizeMin = (int)LOWORD(dwOldSizeRange);
    PassData.nOldSizeMax = (int)HIWORD(dwOldSizeRange);
    PassData.nNewSizeMin = (int)LOWORD(dwNewSizeRange);
    PassData.nNewSizeMax = (int)HIWORD(dwNewSizeRange);
    //PassData.nSizeNew  = nSizeNew;
    //PassData.nSizeOld  = nSizeOld;

//    strcpy(PassData.pstrNewPass,PassData.pstrOldPass);
    CreatePassChgBox(&PassData);

    if( strcmp(PassData.szPwdNew , "") == 0 )
        bResult = FALSE;
    else
        bResult = TRUE;

#ifdef _PASSCHG_
//    EndObjectDebug();
#endif

    return bResult;

}

/*********************************************************************\
* Function:	CreatePassChgBox   
* Purpose:  Create and show the Pass Change interface      
* Params:   pPassChgData ---- Application input data	   
* Return:   TRUE if successful,FALSE if fail.		   
* Remarks	   
**********************************************************************/
BOOL CreatePassChgBox(PPASSCHGDATA pPassChgData)
{
    HWND hPassWnd;
	MSG msg;
    WNDCLASS wc;
	
	
    //Register window class.
    wc.style            = CS_OWNDC;
    wc.lpfnWndProc      = PassChg_WndProc;
    wc.cbClsExtra       = 0;
    wc.cbWndExtra       = 0;
    wc.hInstance        = NULL;
    wc.hIcon            = NULL;
    wc.hCursor          = NULL;
    wc.lpszMenuName     = NULL;
    wc.lpszClassName    = "PASSCHG";
    wc.hbrBackground    = GetStockObject(WHITE_BRUSH);

    if (!RegisterClass(&wc))
        return FALSE;

    //Set MyEdit
    GetClassInfo(NULL, "IMEEDIT", &wc);
    
    OldEditProc         = wc.lpfnWndProc;
    wc.lpfnWndProc      = MyEdit_WndProc;
    wc.lpszClassName    = "MyEdit";

    if (!RegisterClass(&wc))
        return FALSE;

	//Create window.
    hPassWnd = CreateWindow("PASSCHG",pPassChgData->szCaption,
		       WS_VISIBLE | WS_POPUP | WS_CAPTION | PWS_STATICBAR,
		       0, /*24*/18, WNDWIDTH, WNDHEIGHT-38,
		       NULL,NULL,NULL,(LPVOID)pPassChgData
		       );
	
	SendMessage(hPassWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDM_CONFIRM,1), (LPARAM)""); //pPassChgData->szOk);//IDS_CONFIRM);	
	SendMessage(hPassWnd, PWM_CREATECAPTIONBUTTON, (WPARAM)IDM_EXIT , (LPARAM)pPassChgData->szCancel);//IDS_EXIT);


	//MessageLoop.
    while (TRUE)
    {
        GetMessage(&msg, 0, 0, 0);

        if (msg.hwnd == hPassWnd && (msg.message == PC_EXITLOOP))
            break;

        TranslateMessage(&msg);
        DispatchMessage(&msg);

        if (!IsWindowVisible(hPassWnd))
            break;
    }

    UnregisterClass("PASSCHG",NULL);
	UnregisterClass("MyEdit", NULL);
    
    return(TRUE);

}

/*********************************************************************\
* Function:	PassChg_WndProc   
* Purpose:  Treat messages     
* Params:	hWnd ---- Handle of the control
*           wMsgCmd ---- Message
*           wParam,lParam ----Message parameter 
* Return:		   
* Remarks:	   
**********************************************************************/
LRESULT CALLBACK PassChg_WndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam,
                              LPARAM lParam)
{
	LRESULT			     lResult;
	HDC				     hdc;
    static HWND          hEditOld;
    static HWND          hEditNew;
    static HWND          hEditConf;
    static HWND          hFocus;// = NULL;
    static HWND          hKeys;
    static PPASSCHGDATA  p;
    
    static PSTR strOldPass;
    static PSTR strNewPass;
    static PSTR strConfPass;
    int         nCharCount;
    int         nCharCountOld;
    int         nCharCountNew;
    int         nCharCountConf;
	IMEEDIT ime;

	memset(&ime, 0, sizeof(IMEEDIT));
	ime.hwndNotify	= (HWND)hWnd;    // 可以指定为应用程序主窗口句柄
//	ime.dwAttrib	= IME_ATTRIB_RECEIVER; // 调用通用输入界面
	ime.pszImeName	= "Digit";            // 指定使用默认输入法
	ime.uMsgSetText = 0;//IME_MSG_NAMEEXIT;
	ime.pszTitle = "";
	ime.pszCharSet = NULL;

    lResult = 0;

    
    switch (wMsgCmd)	
    {
    case WM_CREATE :
        {
            PCREATESTRUCT   pCreateData;

            pCreateData = (PCREATESTRUCT)lParam;
            p = (PPASSCHGDATA)pCreateData->lpCreateParams;
            p->bClear = FALSE;
            p->bConfirm = FALSE;

            hEditOld = CreateWindow("IMEEDIT", NULL, //"EDIT",NULL,
		               WS_VISIBLE | ES_PASSWORD | WS_CHILD | ES_CENTER | WS_BORDER | WS_TABSTOP|ES_NUMBER,
		               10,/*30*/25,EDITWIDTH,EDITHEIGHT,
		               hWnd,(HMENU)IDC_EDITOLD,NULL,(PVOID)&ime
		               );
		    hEditNew = CreateWindow("IMEEDIT", NULL, //"EDIT",NULL,
		               WS_VISIBLE | ES_PASSWORD | WS_CHILD | ES_CENTER | WS_BORDER | WS_TABSTOP|ES_NUMBER,
		               10,/*70*/65,EDITWIDTH,EDITHEIGHT,
		               hWnd,(HMENU)IDC_EDITNEW,NULL,(PVOID)&ime
		               );
            hEditConf = CreateWindow("IMEEDIT", NULL, //"EDIT",NULL,
		               WS_VISIBLE | ES_PASSWORD | WS_CHILD | ES_CENTER | WS_BORDER | WS_TABSTOP|ES_NUMBER,
		               10,/*110*/105,EDITWIDTH,EDITHEIGHT,
		               hWnd,(HMENU)IDC_EDITCONF,NULL,(PVOID)&ime
		               );
            SendMessage(hEditOld, EM_LIMITTEXT, (WPARAM)p->nOldSizeMax, 0);
            SendMessage(hEditNew, EM_LIMITTEXT, (WPARAM)p->nNewSizeMax, 0);
            SendMessage(hEditConf, EM_LIMITTEXT, (WPARAM)p->nNewSizeMax, 0);

            hFocus = hEditOld;
//            hKeys  = (HWND)CreateDinputSoftKeyboard(hWnd, 135);

            strOldPass  = (PSTR)malloc(p->nOldSizeMax+1);
            //strOldPass  = (PSTR)malloc(p->nOldSizeMax+1);
            if(strOldPass == NULL)
            {
                DestroyWindow(hEditOld);
                return FALSE;
            }
            strNewPass  = (PSTR)malloc( p->nNewSizeMax+1);
            //strNewPass  = (PSTR)malloc(p->nNewSizeMax+1);
            if(strNewPass == NULL)
            {
                DestroyWindow(hEditNew);
                return FALSE;
            }
            strConfPass = (PSTR)malloc(p->nNewSizeMax+1);
            //strConfPass = (PSTR)malloc(p->nNewSizeMax+1);
            if(strConfPass == NULL)
            {
                DestroyWindow(hEditConf);
                return FALSE;
            }

            lResult = 0;

        }
        break;

    case WM_ACTIVATE:
        switch(LOWORD(wParam))
        {
        case WA_ACTIVE:
            SetFocus(hFocus);
            hFocus = NULL;
            break;
        case WA_INACTIVE:
            hFocus = GetFocus();
            break;
        }

        break;

    case WM_DESTROY:
        DestroyWindow(hKeys);
        free(strOldPass);
        //Free(strOldPass);
        free(strNewPass);
        //Free(strNewPass);
        free(strConfPass);
	//Free(strConfPass);
        break;

    case WM_SETFOCUS:
        break;
	case WM_PAINT:
		{
		    RECT rect;

		    hdc = BeginPaint(hWnd, NULL);
		
		    rect.left = 10;
		    rect.top  = 5;//10;
		    rect.right = rect.left+TEXTWIDTH;
		    rect.bottom = rect.top+TEXTHEIGHT;
            DrawText(hdc,IDS_OLDPASS/*GetString(STR_PUBLICCON_OLDPASS)*/,
                strlen(IDS_OLDPASS), //strlen(GetString(STR_PUBLICCON_OLDPASS)),
                &rect,DT_LEFT | DT_VCENTER);

            rect.left = 10;
		    rect.top  = 45;//50;
		    rect.right = rect.left+TEXTWIDTH;
		    rect.bottom = rect.top+TEXTHEIGHT;
            DrawText(hdc,IDS_NEWPASS/*GetString(STR_PUBLICCON_NEWPASS)*/,
                strlen(IDS_NEWPASS), //strlen(GetString(STR_PUBLICCON_NEWPASS)),
                &rect,DT_LEFT | DT_VCENTER);

            rect.left = 10;
		    rect.top  = 85;//90;
		    rect.right = rect.left+TEXTWIDTH;
		    rect.bottom = rect.top+TEXTHEIGHT;
            DrawText(hdc,IDS_CONFIRMPASS/*GetString(STR_PUBLICCON_CONFIRMPASS)*/,
                strlen(IDS_CONFIRMPASS), //strlen(GetString(STR_PUBLICCON_CONFIRMPASS)),
                &rect,DT_LEFT | DT_VCENTER);
	
		    EndPaint(hWnd,NULL);
		}
		break;

	case WM_COMMAND:
        nCharCountOld = SendMessage(hEditOld, WM_GETTEXTLENGTH, 0, 0);
        nCharCountNew = SendMessage(hEditNew, WM_GETTEXTLENGTH, 0, 0);
        nCharCountConf = SendMessage(hEditConf, WM_GETTEXTLENGTH, 0, 0);
            switch(HIWORD(wParam))
            {
            case EN_CHANGE:
                nCharCount = SendMessage(GetFocus(), WM_GETTEXTLENGTH, 0, 0);
                

                //改变右softkey
                if(nCharCount > 0 && p->bClear == FALSE)
                {
                    SendMessage(hWnd, PWM_SETBUTTONTEXT, MAKEWPARAM(0,IDM_EXIT) , (LPARAM)ML("Clear")); //"清除");
                    p->bClear = TRUE;
                }
                else if(nCharCount == 0 && p->bClear == TRUE)
                {
                    SendMessage(hWnd, PWM_SETBUTTONTEXT, MAKEWPARAM(0,IDM_EXIT) , (LPARAM)p->szCancel);
                    p->bClear = FALSE;
                }
                //改变左softkey
                if( (nCharCountOld < p->nOldSizeMin  ||
                    nCharCountNew < p->nNewSizeMin   ||
                    nCharCountConf < p->nNewSizeMin) && p->bConfirm == FALSE)
                {
                    SendMessage(hWnd, PWM_SETBUTTONTEXT, MAKEWPARAM(1,IDM_CONFIRM) , (LPARAM)"");
                    p->bConfirm = TRUE;
                }
                else if(nCharCountOld >= p->nOldSizeMin  &&
                        nCharCountNew >= p->nNewSizeMin  &&
                        nCharCountConf >= p->nNewSizeMin && p->bConfirm == TRUE)
                {
                    SendMessage(hWnd, PWM_SETBUTTONTEXT, MAKEWPARAM(1,IDM_CONFIRM) , (LPARAM)p->szOk);
                    p->bConfirm = FALSE;
                }

                break;

            case EN_SETFOCUS:
                nCharCount = SendMessage((HWND)lParam, WM_GETTEXTLENGTH, 0, 0);
                //改变右softkey
                if(nCharCount > 0 && p->bClear == FALSE)
                {
                    SendMessage(hWnd, PWM_SETBUTTONTEXT, MAKEWPARAM(0,IDM_EXIT) , (LPARAM)ML("Clear")); //"清除");
                    p->bClear = TRUE;
                }
                else if(nCharCount == 0 && p->bClear == TRUE)
                {
                    SendMessage(hWnd, PWM_SETBUTTONTEXT, MAKEWPARAM(0,IDM_EXIT) , (LPARAM)p->szCancel);
                    p->bClear = FALSE;
                }
                //改变左softkey
                if( (nCharCountOld < p->nOldSizeMin  ||
                    nCharCountNew < p->nNewSizeMin   ||
                    nCharCountConf < p->nNewSizeMin) && p->bConfirm == FALSE)
                {
                    SendMessage(hWnd, PWM_SETBUTTONTEXT, MAKEWPARAM(1,IDM_CONFIRM) , (LPARAM)"");
                    p->bConfirm = TRUE;
                }
                else if(nCharCountOld >= p->nOldSizeMin  &&
                        nCharCountNew >= p->nNewSizeMin  &&
                        nCharCountConf >= p->nNewSizeMin && p->bConfirm == TRUE)
                {
                    SendMessage(hWnd, PWM_SETBUTTONTEXT, MAKEWPARAM(1,IDM_CONFIRM) , (LPARAM)p->szOk);
                    p->bConfirm = FALSE;
                }

                break;
            }

        switch(LOWORD(wParam))
		{
		case IDM_CONFIRM: //确定
			{
                int nInputLen;
                int nInputLen1;
                int nInputOldLen;
                PSTR szButtonText;

                szButtonText = (PSTR)malloc(strlen(p->szOk)+2);
                //szButtonText = (PSTR)malloc(strlen(p->szOk)+2);
                if(szButtonText == NULL)
                    return FALSE;
                SendMessage(hWnd, PWM_GETBUTTONTEXT, MAKEWPARAM(1,IDM_CONFIRM), (LPARAM)szButtonText);

                if(strcmp(szButtonText, "") == 0)
                {
                    free(szButtonText);
		    //Free(szButtonText);	
                    break;
                }

                //旧密码长度检查提示
                nInputOldLen = SendMessage(hEditOld,WM_GETTEXTLENGTH,0,0);
                
                if( nInputOldLen == 0 )
                {
                    SendMessage(hEditNew,WM_SETTEXT,(WPARAM)0,(LPARAM)"");
                    SendMessage(hEditConf,WM_SETTEXT,(WPARAM)0,(LPARAM)"");
                    SendMessage(hEditOld,WM_SETTEXT,(WPARAM)0,(LPARAM)"");
                    
                    SetFocus(hEditOld);
                    
                    PLXTipsWin(NULL, hWnd, 0, ML("Can't be empty"),ML("Change Pass"), Notify_Info, ML("OK"), NULL, WAITTIMEOUT);

                    free(szButtonText);
                    //Free(szButtonText);
                    break;
                }
                if( nInputOldLen > p->nOldSizeMax )
                {
                    SendMessage(hEditNew,WM_SETTEXT,(WPARAM)0,(LPARAM)"");
                    SendMessage(hEditConf,WM_SETTEXT,(WPARAM)0,(LPARAM)"");
                    SendMessage(hEditOld,WM_SETTEXT,(WPARAM)0,(LPARAM)"");
                    
                    SetFocus(hEditOld);
                    
                    PLXTipsWin(NULL, hWnd, 0,ML("Old password is too long"),ML("Change Pass"),Notify_Info,ML("OK"), NULL, 20 );

                    free(szButtonText);
                    //Free(szButtonText);
                    break;
                }

                //新密码长度
                nInputLen = SendMessage(hEditNew,WM_GETTEXTLENGTH,0,0);
                nInputLen1 = SendMessage(hEditConf,WM_GETTEXTLENGTH,0,0);
                
                //新密码与验证密码长度不同，则判为不匹配
                if( nInputLen != nInputLen1 )
                {
                    SendMessage(hEditNew,WM_SETTEXT,(WPARAM)0,(LPARAM)"");
                    SendMessage(hEditConf,WM_SETTEXT,(WPARAM)0,(LPARAM)"");
                    SetFocus(hEditNew);
                 
                    PLXTipsWin(NULL, hWnd, 0,ML("passwords not match") ,ML("Change Pass"),Notify_Info,ML("OK"), NULL, 20 );

                    free(szButtonText);
                    //Free(szButtonText);
                    break;
                }

                //新密码为空
                if( nInputLen == 0 )
                {
                    SendMessage(hEditNew,WM_SETTEXT,(WPARAM)0,(LPARAM)"");
                    SendMessage(hEditConf,WM_SETTEXT,(WPARAM)0,(LPARAM)"");
                    SetFocus(hEditNew);
                    
                    PLXTipsWin(NULL, hWnd, 0,ML("Can't be Empty!"),ML("Change Pass"),Notify_Alert,ML("OK"),NULL, 20 );

                    free(szButtonText);
                    //Free(szButtonText);
                    break;
                }
                //新密码过长时提示用户，截取密码
                if( nInputLen > p->nNewSizeMax )
                {
					// Notification [8/3/2005]
                    if(PLXConfirmWin(NULL, NULL, /*GetString(STR_PUBLICCON_NEWTOOLONG)*/ML("new is too long"), Notify_Info, NULL, 
                        ML("OK"), ML("Cancel")))
                    {
                        SendMessage(hEditNew,WM_SETTEXT,(WPARAM)0,(LPARAM)"");
                        SendMessage(hEditConf,WM_SETTEXT,(WPARAM)0,(LPARAM)"");
                        
                        SetFocus(hEditNew);

                        free(szButtonText);
                        //Free(szButtonText);
                        break;
                    }
                    else
                        nInputLen = p->nNewSizeMax;
                }

                SendMessage(hEditNew,WM_GETTEXT,(WPARAM)(nInputLen+1),(LPARAM)strNewPass);
                SendMessage(hEditConf,WM_GETTEXT,(WPARAM)(nInputLen+1),(LPARAM)strConfPass);
                SendMessage(hEditOld,WM_GETTEXT,(WPARAM)(nInputOldLen+1),(LPARAM)strOldPass);

                //新密码内容匹配判断
                if( strcmp(strNewPass , strConfPass) != 0 )
                {
                    SendMessage(hEditNew,WM_SETTEXT,(WPARAM)0,(LPARAM)"");
                    SendMessage(hEditConf,WM_SETTEXT,(WPARAM)0,(LPARAM)"");
                    SetFocus(hEditNew);
                    PLXTipsWin(NULL, hWnd, 0,ML("Passes not match") ,ML("Change Pass"),Notify_Alert,ML("Ok"), NULL, 20);
                   
                    free(szButtonText);
                    //Free(szButtonText);
                    break;
                }

                //所有验证通过后，将输入串复制给新密码与旧密码
                strcpy(p->szPwdNew,strNewPass);
                strcpy(p->szPwdOld,strOldPass);

                DestroyWindow(hWnd);
                PostMessage(hWnd,PC_EXITLOOP,0,0);
                //lResult = TRUE;
                
                free(szButtonText);
                //Free(szButtonText);
                break;
			}
		case IDM_EXIT:		//取消
            {
                PSTR szButtonText;
                PSTR szText;
                int  nCount;
                int  nCount1;

                nCount = strlen(p->szCancel);
                nCount1 = strlen(ML("Clear"));
                nCount = nCount > nCount1 ? nCount : nCount1;

                szButtonText = (PSTR)malloc( nCount+2);
                //szButtonText = (PSTR)malloc(nCount+2);
                if(szButtonText == NULL)
                    return FALSE;
                SendMessage(hWnd, PWM_GETBUTTONTEXT, MAKEWPARAM(0,IDM_EXIT), (LPARAM)szButtonText);
            
                if(strcmp(szButtonText, p->szCancel) == 0) //不修改密码退出，新密码置空
                {
                    strcpy(p->szPwdNew,"");
		    
                    DestroyWindow(hWnd);
                    PostMessage(hWnd,PC_EXITLOOP,0,0);

                    lResult = FALSE;
                }
                else
                {
                    nCount = SendMessage(GetFocus(), WM_GETTEXTLENGTH, 0, 0);
                    if(nCount > 0)
                    {
                        szText = (PSTR)malloc(nCount+1);
                        //szText = (PSTR)malloc(nCount+1);
                        if(szText == NULL)
                        {
                            free(szButtonText);
                            //Free(szButtonText);
                            return FALSE;
                        }
                    }
                    SendMessage(GetFocus(), WM_GETTEXT, (WPARAM)nCount+1, (LPARAM)szText);
                    szText[nCount-1] = '\0';
                    SendMessage(GetFocus(), WM_SETTEXT, 0, (LPARAM)szText);
                    SendMessage(GetFocus(), EM_SETSEL, (WPARAM)nCount-1, (LPARAM)nCount-1);

                    if(nCount > 0)
                        free(szText);
                        //Free(szText);
                }
                free(szButtonText);
                //Free(szButtonText);
            }
            break;
            
		};
	
        break;
    
	default :

        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);

        break;
	
    };

	return lResult;
}

LRESULT CALLBACK MyEdit_WndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam,
                              LPARAM lParam)
{
    LRESULT lResult;
    
    switch (wMsgCmd)
    {
    case WM_CHAR :
        {
            if((WORD)wParam >= '0' && (WORD)wParam <= '9')
                lResult = CallWindowProc(OldEditProc, hWnd, wMsgCmd, wParam, lParam);
        }
        break;
    default:
        
        lResult = CallWindowProc(OldEditProc, hWnd, wMsgCmd, wParam, lParam);
        break;
    }
    return lResult;
}
