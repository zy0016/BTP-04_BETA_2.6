/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : 
 *
\**************************************************************************/

#include "window.h"
#include "string.h"
#include "stdio.h"
#include "winpda.h"
#include "str_public.h"
#include "str_plx.h"
//#include "hpime.h"
#include "imesys.h"
#include "plx_pdaex.h"

#include "pubapp.h"
#include "mullang.h"
#include "malloc.h"
#ifndef _SSPASSCHCK_
#define _SSPASSCHCK_
#endif

#define PC_EXITLOOP    WM_USER + 100

#define IDC_EDIT        1     
#define IDM_EXIT		WM_USER+10  //退出
#define IDM_CONFIRM		WM_USER+12  //确定

#define TEXTLEFT   7//28
#define PROMTTOP    37//50//20
#define TEXTWIDTH  160//120//220
#define TEXTHEIGHT 81//23//60//60
#define EDITLEFT   8
#define EDITTOP    124//105//60
#define EDITWIDTH  160//169//220
#define EDITHEIGHT 25

//Define data structure for passcheck control
typedef struct tagPASSDATA
{
	HWND	hSuperWnd;
	WNDPROC		OldEditProc;
	HWND			hEdit;
	HWND			hPassWnd;
	HWND			hTimesWnd;
    PSTR    szCaption;
    char    szPrompt[50];
    PSTR    szPassword;
    PSTR    szOk;
    PSTR    szCancel;
    int     nSizeMin;
    int     nSizeMax;
    BOOL    bClear;
    BOOL    bConfirm;
	int		nTimes;
} PASSDATA , *PPASSDATA;
static	BOOL	bPassWord = FALSE;
BOOL    CreateSSPassBox(PPASSDATA pPassData);
LRESULT CALLBACK SSPASSCHECK_WndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam,
                              LPARAM lParam);
static LRESULT CALLBACK MyEdit_WndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam,
                              LPARAM lParam);
static RECT timeRect = {7,153,167,176};
#define		CODEICON	"/rom/pim/na_43x28.bmp" //"/rom/public/code_entry_icon.bmp"
//static BOOL			bRepaint = FALSE;
//static	char	pstrtxt[20] ;
//extern	char	*SOSPhoneNum[];
/*********************************************************************\
* Function:	PLXVerifyPassword   
* Purpose:  Recieve application input and returns the check password      
* Params:	szCaption ---- Caption text
*           szPwd ---- The buffer restore the user input password.
*           nSize ---- Size of the szPwd buffer.
*           szOk,szCancle ---- Texts display on the buttons.  
* Return:   TRUE if successful,FALSE if cancel the check.		   
* Remarks	   
**********************************************************************/

BOOL SSPLXVerifyPassword(HWND hParentHwnd, PCSTR szCaption, PCSTR szPrompt, PSTR szPwd, DWORD dwSizeRange, 
                      PCSTR szOk, PCSTR szCancel, int nTimes)
{
	PASSDATA PassData;
    BOOL     bResult = FALSE;

#ifdef _PASSCHCK_
//    StartObjectDebug();
#endif
	memset(&PassData, 0, sizeof(PASSDATA));
	PassData.hSuperWnd	= hParentHwnd;
    PassData.szCaption  = (PSTR)szCaption;
	if (szPrompt) {			
		strcpy(PassData.szPrompt, szPrompt);
	}
    PassData.szPassword = szPwd;
    PassData.szOk       = (PSTR)szOk;
    PassData.szCancel   = (PSTR)szCancel;
    PassData.nSizeMin   = (int)LOWORD(dwSizeRange);
    PassData.nSizeMax   = (int)HIWORD(dwSizeRange);
	PassData.nTimes		= nTimes;
    
	if (!szPwd) {
		return FALSE;
	}
	PassData.szPassword[0] = NULL;
	
    //Show the password window.
    CreateSSPassBox(&PassData); 
    
    if(strcmp(PassData.szPassword , "") == 0)
        bResult = FALSE;
    else
        bResult = TRUE;
	

#ifdef _PASSCHCK_
//    EndObjectDebug();
#endif

    return(bResult);

}

/*********************************************************************\
* Function:	CreateSSPassBox   
* Purpose: Create and show the pass check control      
* Params:  pPassData ---- Application input data.	   
* Return:  TRUE if successful ,FALSE if cancel the check.		   
* Remarks	   
**********************************************************************/

BOOL CreateSSPassBox(PPASSDATA pPassData)
{

	MSG msg;
    WNDCLASS wc;
	HWND	hPassWnd;
	
	
    //Register window class.
    wc.style            = CS_OWNDC;
    wc.lpfnWndProc      = SSPASSCHECK_WndProc;
    wc.cbClsExtra       = 0;
    wc.cbWndExtra       = sizeof(PASSDATA);
    wc.hInstance        = NULL;
    wc.hIcon            = NULL;
    wc.hCursor          = NULL;
    wc.lpszMenuName     = NULL;
    wc.lpszClassName    = "SSPASS";
    wc.hbrBackground    = GetStockObject(WHITE_BRUSH);

    if (!RegisterClass(&wc))
        return FALSE;

    //Set MyEdit
	GetClassInfo(NULL, "EDIT", &wc);
    
    pPassData->OldEditProc         = wc.lpfnWndProc;

    wc.lpfnWndProc      = MyEdit_WndProc;
    wc.lpszClassName    = "MyEdit2";

    if (!RegisterClass(&wc))
        return FALSE;

	//Create window.
    hPassWnd = CreateWindow("SSPASS",pPassData->szCaption,
		       WS_VISIBLE | WS_POPUP | PWS_STATICBAR/*|WS_CAPTION*/,
		       //0, /*24*/18, WNDWIDTH, WNDHEIGHT-38,
			   PLX_WIN_POSITION,
		       pPassData->hSuperWnd,NULL,NULL,(LPVOID)pPassData);
	if (!hPassWnd) {
		return FALSE;
	}
	bPassWord = TRUE;
	SendMessage(hPassWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDM_CONFIRM,1), (LPARAM)""); //pPassData->szOk);	
	SendMessage(hPassWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDM_EXIT,0) , (LPARAM)pPassData->szCancel);
	SendMessage(hPassWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"");

	//MessageLoop.
    while (TRUE)
    {
        GetMessage(&msg, 0, 0, 0);

        if (msg.hwnd == hPassWnd && (msg.message == PC_EXITLOOP))
		{
			DestroyWindow(hPassWnd);
			bPassWord = FALSE;
            break;
		}
        TranslateMessage(&msg);
        DispatchMessage(&msg);

        if (!IsWindowVisible(hPassWnd))
            break;
    }

    UnregisterClass("SSPASS",NULL);
	UnregisterClass("MyEdit2", NULL);
    
    return(TRUE);

}


/*********************************************************************\
* Function:	SSPASSCHECK_WndProc   
* Purpose:  Treat the messages.      
* Params:   hWnd ---- Handle of the control
*           wMsgCmd ---- Message
*           wParam,lParam ---- Parameter of the message.	   
* Return		   
* Remarks	   
**********************************************************************/

LRESULT CALLBACK SSPASSCHECK_WndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam,
                              LPARAM lParam)
{
	LRESULT			  lResult;
	HDC				  hdc;


    static HWND       hLastFocus;
//    static HWND       hKeys;
    int               nCharCount;
	PPASSDATA	p;
//	int		nTxtLen;
/*
	IMEEDIT ime;

	memset(&ime, 0, sizeof(IMEEDIT));
	ime.hwndNotify	= (HWND)hWnd;    // 可以指定为应用程序主窗口句柄
	ime.dwAttrib	= NULL; // 调用通用输入界面
	ime.pszImeName	= "数字";            // 指定使用默认输入法
	ime.uMsgSetText = 0;//IME_MSG_NAMEEXIT;
	ime.pszTitle = "";
	ime.pszCharSet = NULL;
*/
    lResult = 0;
	p = GetUserData(hWnd);
	switch (wMsgCmd)	
    {
    case WM_CREATE :
        {
            PCREATESTRUCT   pCreateData;

            pCreateData = (PCREATESTRUCT)lParam;
            memcpy(p , (PPASSDATA)pCreateData->lpCreateParams, sizeof(PASSDATA));
            p->bClear = FALSE;
            p->bConfirm = FALSE;

            p->hEdit = CreateWindow("MyEdit2", NULL, //"EDIT",NULL,
		               WS_VISIBLE | ES_PASSWORD | WS_CHILD | ES_NUMBER|ES_AUTOHSCROLL|ES_CENTER,
		               EDITLEFT, EDITTOP, EDITWIDTH, EDITHEIGHT, //120,10,110,30,
		               hWnd,(HMENU)IDC_EDIT,NULL,NULL//(PVOID)&ime
		               );
			p->hPassWnd = hWnd;

		    SendMessage(p->hEdit, EM_LIMITTEXT, (WPARAM)p->nSizeMax, 0);


        }
        break;

    case WM_ACTIVATE:
        switch(LOWORD(wParam))
        {
        case WA_ACTIVE:
			bPassWord = TRUE;
            SetFocus(p->hEdit);
            break;

		case WA_INACTIVE:
			bPassWord = FALSE;
			break;
        }
        
        break;
/*		
    case WM_DESTROY:
        if (p->szOk)
        {
			free(p->szOk);
        }
		if (p->szCancel)
		{
			free(p->szCancel);
		}
		if (p->szCaption)
		{
			free(p->szCaption);
		}
		break;
*/
	case WM_PAINT:
		{
		    RECT rect,rcClient,rcCapicon;
            HPEN hPen, hOldPen;
			HBRUSH	newbrush,oldbrush;
			char	strTime[5];
			int	oldbm;
			HDC		hMemDc;

			hdc = BeginPaint(hWnd, NULL);
			GetClientRect(hWnd, &rcClient);

			hMemDc = CreateMemoryDC(rcClient.right, rcClient.bottom);
			FillRect(hMemDc, &rcClient, (HBRUSH)GetStockObject(WHITE_BRUSH));

			hPen = CreatePen(PS_SOLID, 1, RGB(163,176,229));
            hOldPen = (HPEN)SelectObject(hMemDc, hPen);

			newbrush	= CreateBrush(BS_SOLID,RGB(163,176,229),NULL);
			oldbrush	= SelectObject(hMemDc,newbrush);

			SetRect(&rcCapicon,rcClient.left,rcClient.top,rcClient.right,31);
			DrawRect(hMemDc,&rcCapicon);

			newbrush	= SelectObject(hMemDc,oldbrush);
			DeleteObject(newbrush);

			SelectObject(hMemDc, hOldPen);
			DeleteObject(hPen);

			oldbm = SetBkMode(hMemDc, NEWTRANSPARENT);
			DrawImageFromFile(hMemDc,CODEICON, 65, 5,ROP_SRC);
			SetBkMode(hMemDc, oldbm);

		    //hMemDc = BeginPaint(hWnd, NULL);
		
            hPen = CreatePen(PS_SOLID, 2, RGB(0,0,0));
            hOldPen = (HPEN)SelectObject(hMemDc, hPen);
		    
            rect.left = TEXTLEFT; //10;
		    rect.top  = PROMTTOP; //10;
		    rect.right = rect.left + TEXTWIDTH; //120;
		    rect.bottom = rect.top + TEXTHEIGHT; //40;
			if ( p->nTimes > -1 )
			{
				sprintf(strTime,"%d",p->nTimes);
				DrawText(hMemDc,strTime,-1,&timeRect,DT_HCENTER | DT_VCENTER);
			}
        
            if (p->szPrompt) {
				DrawText(hMemDc,p->szPrompt,-1,&rect, DT_HCENTER | DT_VCENTER | ES_MULTILINE);
            }
            

            SelectObject(hMemDc, hOldPen);
            DeleteObject(hPen);
			BitBlt(hdc, 0, 0, rcClient.right, rcClient.bottom, hMemDc, 0, 0, ROP_SRC);
			DeleteObject(hMemDc);
		    EndPaint(hWnd,NULL);
		}
		break;

	case WM_COMMAND:
        switch(LOWORD(wParam))
        {
        case IDC_EDIT:
			if(HIWORD(wParam) == EN_CHANGE)
			{
				nCharCount = SendMessage(p->hEdit, WM_GETTEXTLENGTH, 0, 0);
				//change right soft key
				if(nCharCount > 0 && p->bClear == FALSE)
				{
					SendMessage(hWnd, PWM_SETBUTTONTEXT, 0 , (LPARAM)ML("Clear")); //"清除"); //GetString(IDS_CLEAR));
					p->bClear = TRUE;
				}
				else if(nCharCount == 0 && p->bClear == TRUE)
				{
					SendMessage(hWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)p->szCancel);
					p->bClear = FALSE;
				}
				//change left soft key
				if(nCharCount >= p->nSizeMin && p->bConfirm == FALSE)
				{
					//
					SendMessage(hWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)p->szOk);
					p->bConfirm = TRUE;
					
				}
				else if(nCharCount < p->nSizeMin && p->bConfirm == TRUE)
				{
					//
					SendMessage(hWnd, PWM_SETBUTTONTEXT, 1 , (LPARAM)"");
					p->bConfirm = FALSE;
				}
				/*
				nTxtLen = GetWindowTextLength(hEdit);
				GetWindowText(hEdit,pstrtxt,nTxtLen+1);
				if (findstr(SOSPhoneNum, pstrtxt)) {
					bRepaint = TRUE;
					InvalidateRect(hEdit, NULL, TRUE);
				}
				*/
			}
			else
				PDADefWindowProc(hWnd,wMsgCmd,wParam,lParam);
            break;
		default:
			PDADefWindowProc(hWnd,wMsgCmd,wParam,lParam);
			break;
        }
		break;

	case WM_KEYDOWN:
        switch(wParam)
		{
		case VK_RETURN: //
			{
                int  nInputLen;
//                PSTR szButtonText;
 /*               
                szButtonText = (PSTR)malloc(strlen(p->szOk)+2);
                //szButtonText = (PSTR)malloc(strlen(p->szOk)+2);
                if(szButtonText == NULL)
                    return FALSE;
                SendMessage(hWnd, PWM_GETBUTTONTEXT, MAKEWPARAM(1,IDM_CONFIRM), (LPARAM)szButtonText);

                if(strcmp(szButtonText, "SOS") == 0)
                {
                    free(szButtonText);
					szButtonText = NULL;
                    //Free(szButtonText);
                    return FALSE;
                }
*/
                nInputLen = SendMessage(p->hEdit,WM_GETTEXTLENGTH,0,0);
				if (nInputLen < p->nSizeMin) 
				{
					return FALSE;
				}
				if(nInputLen > p->nSizeMax)
				{
                    //PLXTipsWin(NULL, hWnd, 0, ML("Password is too long"),ML("Pass Check"),Notify_Info,ML("OK"), NULL, 20 );
                    SendMessage(p->hEdit,WM_SETTEXT,0,(LPARAM)"");
                    return FALSE;
				}
                
				SendMessage(p->hEdit,WM_GETTEXT,(WPARAM)(nInputLen+1),(LPARAM)p->szPassword);
				
                if(strcmp(p->szPassword , "") == 0)
                {
                   // PLXTipsWin(NULL, hWnd, 0, ML("password can't be empty"),ML("Pass Check"),Notify_Info,NULL, NULL, 20 );
					//PLXTipsWin(NULL, hWnd, 0, ML("Please,give\r\nall codes"),NULL, Notify_Alert, ML("Ok"), NULL,WAITTIMEOUT);
                    
                    break;
                }
                
                PostMessage(hWnd,PC_EXITLOOP,0,0);

                break;
			}
			break;
		case VK_F10:	//取消
            {
				int nLen;
				nLen = SendMessage(p->hEdit,EM_GETSEL,0,0);
				
                if(nLen == 0)
                {
					
                    PostMessage(hWnd,PC_EXITLOOP,0,0);
					//					SendMessage(p->hSuperWnd, WM_KEYDOWN, VK_F10, 0);
                }
                else
                {
					p->OldEditProc(p->hEdit,WM_KEYDOWN,VK_BACK,0);					
                }
				
            }
            break;
		default :
			
			lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
			
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
	WNDCLASS wc;
	WNDPROC		OldEditProc;
	GetClassInfo(NULL, "EDIT", &wc);
    
    OldEditProc         = wc.lpfnWndProc;
    switch (wMsgCmd)
    {
    case WM_CHAR :
        {
            if((WORD)wParam >= '0' && (WORD)wParam <= '9')
                lResult = CallWindowProc(OldEditProc, hWnd, wMsgCmd, wParam, lParam);
        }
        break;
		/*
	case WM_PAINT:
		if (bRepaint) {
			GetClientRect(hWnd, &rcClient);
			hdc = BeginPaint(hWnd, NULL);
			DrawText(hdc, pstrtxt, -1, &rcClient, DT_VCENTER|DT_CENTER);
			EndPaint(hWnd,NULL);
			bRepaint = FALSE;
		}
		else
		{
			lResult = OldEditProc(hWnd, wMsgCmd, wParam, lParam);
		}
		break;
		*/
    default:
        
        lResult = CallWindowProc(OldEditProc, hWnd, wMsgCmd, wParam, lParam);
        break;
    }
    return lResult;
}
BOOL IsSSPassCheck()
{
	return bPassWord;
}
