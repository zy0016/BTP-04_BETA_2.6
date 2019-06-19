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
#ifndef _PASSCHCK_
#define _PASSCHCK_
#endif

#define PC_EXITLOOP    WM_USER + 100

#define IDC_EDIT        100     
#define IDM_EXIT		WM_USER + 1001  //退出
#define IDM_CONFIRM		WM_USER + 1002  //确定

#define TEXTLEFT   6//28
#define TEXTTOP    50//20
#define TEXTWIDTH  160//120//220
#define TEXTHEIGHT 84//23//60//60
#define EDITLEFT   8
#define EDITTOP    139//94//105//60
#define EDITWIDTH  160//169//220
#define EDITHEIGHT 25


//Define data structure for passcheck control
typedef struct tagPASSDATA
{
	HWND	hSuperWnd;
    PSTR    szCaption;
    char    szPrompt[50];
    PSTR    szPassword[10];
    PSTR    szOk;
    PSTR    szCancel;
    int     nSizeMin;
    int     nSizeMax;
	int		nTime;
    BOOL    bClear;
    BOOL    bConfirm;
	UINT	uMsg;
} PASSDATA , *PPASSDATA;

static char	*SOSPhoneNum[]=
{
	"112",
	"911",
	"08",
	"999",
	""
};

static WNDPROC		OldEditProc;
static HWND			hEdit;
static HWND			hPassWnd;

static BOOL			bRepaint = FALSE;//if the phone number exist in sos table,
static UINT			nTimerId;
static	HBITMAP	hbmpFocus;
//repaint the edit myself.
BOOL    CreatePassBox(PPASSDATA pPassData);
LRESULT CALLBACK PASSCHECK_WndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam,
                              LPARAM lParam);
static LRESULT CALLBACK MyEdit_WndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam,
                              LPARAM lParam);
static RECT timeRect = {6,168,166,191};
static	char	pstrtxt[20] ;
#define		CODEICON	"/rom/pim/na_43x28.bmp" //"/rom/public/code_entry_icon.bmp"
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

BOOL PLXVerifyPassword(HWND hParentHwnd, PCSTR szCaption, PCSTR szPrompt, PSTR szPwd, DWORD dwSizeRange, //int nSize, 
                      PCSTR szOk, PCSTR szCancel,int nTimes, UINT uMsg)
{
	PASSDATA PassData;
    BOOL     bResult = FALSE;

#ifdef _PASSCHCK_
//    StartObjectDebug();
#endif
	memset(&PassData, 0, sizeof(PASSDATA));
	PassData.hSuperWnd	= hParentHwnd;
    PassData.szCaption  = (PSTR)szCaption;
	if(szPrompt)
		strcpy(PassData.szPrompt  ,szPrompt);
    //PassData.szPassword = szPwd;
    PassData.szOk       = (PSTR)szOk;
    PassData.szCancel   = (PSTR)szCancel;
    PassData.nSizeMin   = (int)LOWORD(dwSizeRange);
    PassData.nSizeMax   = (int)HIWORD(dwSizeRange);
	PassData.nTime		= nTimes;
	PassData.uMsg		= uMsg;
    //PassData.nSize      = nSize;
	/*
	if (!PassData.szPassword) {
		PassData.szPassword = (PSTR)malloc(sizeof(char)*(PassData.nSizeMax+2));
		szPwd = PassData.szPassword;
	}
	*/
	if (!szPwd) {
		return	FALSE;
	}
	PassData.szPassword[0] = 0;
	
    //Show the password window.
    CreatePassBox(&PassData); 
    
    if(strcmp((PSTR)PassData.szPassword , "") == 0)
        bResult = FALSE;
    else
        bResult = TRUE;


/*
	if (!szPwd) {
		free(PassData.szPassword);
	}
	*/
    return(bResult);

}

/*********************************************************************\
* Function:	CreatePassBox   
* Purpose: Create and show the pass check control      
* Params:  pPassData ---- Application input data.	   
* Return:  TRUE if successful ,FALSE if cancel the check.		   
* Remarks	   
**********************************************************************/

BOOL CreatePassBox(PPASSDATA pPassData)
{

//	MSG msg;
    WNDCLASS wc;
	
	
    //Register window class.
    wc.style            = CS_OWNDC;
    wc.lpfnWndProc      = PASSCHECK_WndProc;
    wc.cbClsExtra       = 0;
    wc.cbWndExtra       = sizeof(PASSDATA);
    wc.hInstance        = NULL;
    wc.hIcon            = NULL;
    wc.hCursor          = NULL;
    wc.lpszMenuName     = NULL;
    wc.lpszClassName    = "PASS";
    wc.hbrBackground    = GetStockObject(WHITE_BRUSH);

    if (!RegisterClass(&wc))
        return FALSE;

    //Set MyEdit
	GetClassInfo(NULL, "EDIT", &wc);
    
    OldEditProc         = wc.lpfnWndProc;

    wc.lpfnWndProc      = MyEdit_WndProc;
    wc.lpszClassName    = "MyEdit";

    if (!RegisterClass(&wc))
        return FALSE;

	//Create window.
    hPassWnd = CreateWindow("PASS",pPassData->szCaption,
		       WS_VISIBLE | WS_POPUP | PWS_STATICBAR/*|WS_CAPTION*/,
		       //0, /*24*/18, WNDWIDTH, WNDHEIGHT-38,
			   0,0,176,220,//PLX_WIN_POSITION,
		       NULL,NULL,NULL,(LPVOID)pPassData
		       );
	
	SendMessage(hPassWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDM_CONFIRM,1), (LPARAM)ML("SOS")); //pPassData->szOk);	
	SendMessage(hPassWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDM_EXIT,0) , (LPARAM)"");

/*
	//MessageLoop.
    while (TRUE)
    {
        GetMessage(&msg, 0, 0, 0);

        if (msg.hwnd == hPassWnd && (msg.message == PC_EXITLOOP))
		{
			if (hbmpFocus) {
				DeleteObject(hbmpFocus);
			}
			DestroyWindow(hPassWnd);
            break;
		}

        TranslateMessage(&msg);
        DispatchMessage(&msg);

        if (!IsWindowVisible(hPassWnd))
            break;
    }
*/

    
    return(TRUE);

}


/*********************************************************************\
* Function:	PASSCHECK_WndProc   
* Purpose:  Treat the messages.      
* Params:   hWnd ---- Handle of the control
*           wMsgCmd ---- Message
*           wParam,lParam ---- Parameter of the message.	   
* Return		   
* Remarks	   
**********************************************************************/

LRESULT CALLBACK PASSCHECK_WndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam,
                              LPARAM lParam)
{
	LRESULT			  lResult;
	


    static HWND       hLastFocus;

    int               nCharCount;
	int		nTxtLen;
	PPASSDATA	p;
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
			hbmpFocus = LoadImage(NULL, "/rom/progman/listfocus.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

            hEdit = CreateWindow("MyEdit", NULL, //"EDIT",NULL,
		               WS_VISIBLE | ES_PASSWORD | WS_CHILD | ES_CENTER | ES_NUMBER|ES_AUTOVSCROLL,
		               EDITLEFT, EDITTOP, EDITWIDTH, EDITHEIGHT, //120,10,110,30,
		               hWnd,(HMENU)IDC_EDIT,NULL,NULL//(PVOID)&ime
		               );
/*			OldEditProc = GetWindowLong(hEdit,GWL_WNDPROC);
			SetWindowLong(hEdit,GWL_WNDPROC,MyEdit_WndProc);
			*/		    
			SendMessage(hEdit, EM_LIMITTEXT, (WPARAM)p->nSizeMax, 0);

            //hKeys = (HWND)CreateDinputSoftKeyboard(hWnd, /*105*/43);
            
            hLastFocus = hEdit;

        //    lResult = 0;

        }
        break;

    case WM_ACTIVATE:
        switch(LOWORD(wParam))
        {
        case WA_ACTIVE:
            SetFocus(hLastFocus);
            hLastFocus = NULL;
            break;
        case WA_INACTIVE:
            hLastFocus = GetFocus();
            
            break;
        }
        
        break;


	case WM_PAINT:
		{
		    RECT rect,rcClient,rcCapicon,rGps;
            HPEN hPen, hOldPen;
			HBRUSH	newbrush,oldbrush;
			char	strTime[5];
			int	oldbm;
			HDC		hWdc, hdc;

			hWdc = BeginPaint(hWnd, NULL);
			GetClientRect(hWnd, &rcClient);
			
			hdc = CreateMemoryDC(rcClient.right, rcClient.bottom);
			FillRect(hdc, &rcClient, (HBRUSH)GetStockObject(WHITE_BRUSH));

			hPen = CreatePen(PS_SOLID, 1, RGB(163,176,229));
            hOldPen = (HPEN)SelectObject(hdc, hPen);

			newbrush	= CreateBrush(BS_SOLID,RGB(163,176,229),NULL);
			oldbrush	= SelectObject(hdc,newbrush);
			
			SetRect(&rcCapicon,rcClient.left,15,rcClient.right,46);
			DrawRect(hdc,&rcCapicon);

			newbrush	= SelectObject(hdc,oldbrush);
			DeleteObject(newbrush);

			SelectObject(hdc, hOldPen);
			DeleteObject(hPen);
			
			oldbm = SetBkMode(hdc, NEWTRANSPARENT);
			DrawImageFromFile(hdc,CODEICON, 65,20,ROP_SRC);
			SetBkMode(hdc, oldbm);
			SetRect(&rGps, 0, 0,176, 15);
			ClearRect(hdc, &rGps, RGB(206, 211, 214));//grey
			
			DrawIconFromFile(hdc, "/rom/progman/icon/icon_off.ico", 0, 0, 14, 14);

		    //hdc = BeginPaint(hWnd, NULL);
		
            hPen = CreatePen(PS_SOLID, 2, RGB(0,0,0));
            hOldPen = (HPEN)SelectObject(hdc, hPen);
		    
            rect.left = TEXTLEFT; //10;
		    rect.top  = TEXTTOP; //10;
		    rect.right = rect.left + TEXTWIDTH; //120;
		    rect.bottom = rect.top + TEXTHEIGHT; //40;
			SetBkMode(hdc, TRANSPARENT);
			if (-1 < p->nTime )
			{
				sprintf(strTime,"%d",p->nTime);
				DrawText(hdc,strTime,-1,&timeRect,DT_HCENTER | DT_VCENTER);
			}		
          
			if (p->szPrompt) {
				DrawText(hdc,p->szPrompt,-1,&rect, DT_HCENTER | DT_VCENTER /*| ES_MULTILINE*/);
			}
            
			SetBkMode(hdc, oldbm);

            SelectObject(hdc, hOldPen);
            DeleteObject(hPen);
			BitBlt(hWdc, 0, 0, rcClient.right, rcClient.bottom, hdc, 0, 0, ROP_SRC);
			DeleteObject(hdc);
		    EndPaint(hWnd,NULL);
		}
		break;
	case WM_SETRBTNTEXT:
		{
			PSTR	rBt;
			rBt = (PSTR)lParam;
			if (stricmp(rBt, ML("Cancel")) == 0) {
				SendMessage(hWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)"");
			}
			
			
			else
				SendMessage(hWnd,  PWM_SETBUTTONTEXT, 0, (LPARAM)ML("Clear"));
			
        }
		break;

	case WM_COMMAND:
        switch(LOWORD(wParam))
        {
        case IDC_EDIT:
			if(HIWORD(wParam) == EN_CHANGE)
			{
				nCharCount = SendMessage(hEdit, WM_GETTEXTLENGTH, 0, 0);
				/*
				//change right soft key
				if(nCharCount > 0 && p->bClear == FALSE)
				{
					SendMessage(hWnd, PWM_SETBUTTONTEXT, MAKEWPARAM(0,IDM_EXIT) , (LPARAM)ML("Clear")); //"清除"); //GetString(IDS_CLEAR));
					p->bClear = TRUE;
				}
				else if(nCharCount == 0 && p->bClear == TRUE)
				{
					SendMessage(hWnd, PWM_SETBUTTONTEXT, MAKEWPARAM(0,IDM_EXIT) , (LPARAM)"");
					p->bClear = FALSE;
				}
				*/
				//change left soft key
				if(nCharCount >= p->nSizeMin && p->bConfirm == FALSE)
				{
					//
					SendMessage(hWnd, PWM_SETBUTTONTEXT, MAKEWPARAM(1,IDM_CONFIRM) , (LPARAM)p->szOk);
					p->bConfirm = TRUE;
					
				}
				else if(nCharCount < p->nSizeMin && p->bConfirm == TRUE)
				{
					//
					SendMessage(hWnd, PWM_SETBUTTONTEXT, MAKEWPARAM(1,IDM_CONFIRM) , (LPARAM)ML("SOS"));
					p->bConfirm = FALSE;
				}

		        nTxtLen = GetWindowTextLength(hEdit);
				GetWindowText(hEdit,pstrtxt,nTxtLen+1);
				if (findstr(SOSPhoneNum, pstrtxt)) {
					bRepaint = TRUE;
					InvalidateRect(hEdit, NULL, TRUE);
				}
			}
            break;
		default:
			PDADefWindowProc(hWnd,wMsgCmd,wParam,lParam);
			break;
        }
		break;
		
	case  WM_CLOSE:
		
		DestroyWindow(hWnd);
		UnregisterClass("PASS",NULL);
		UnregisterClass("MyEdit", NULL);
		break;

	case WM_DESTROY:
		if (hbmpFocus) 
		{
			DeleteObject(hbmpFocus);
		}
		/*
		if (p->szCancel)
		{
			free(p->szCancel);
		}
		if (p->szOk)
		{
			free(p->szOk);
		}
		if (p->szCaption)
		{
			free(p->szCaption);
		}
		*/
		break;

	case WM_KEYDOWN:
        switch(wParam)
		{
		case VK_RETURN: //
			{
                int  nInputLen;

                nInputLen = SendMessage(hEdit,WM_GETTEXTLENGTH,0,0);
//				if (nInputLen < p->nSizeMin) 
//				{
//					return FALSE;
//				}
				if(nInputLen > p->nSizeMax)
				{
                    //PLXTipsWin(NULL, hWnd, 0, ML("Password is too long"),ML("Pass Check"),Notify_Info,ML("Ok"), NULL, 20 );
                    SendMessage(hEdit,WM_SETTEXT,0,(LPARAM)"");
                    return FALSE;
				}
                
				SendMessage(hEdit,WM_GETTEXT,(WPARAM)(nInputLen+1),(LPARAM)p->szPassword);
				
                if(strcmp((PSTR)p->szPassword , "") == 0)
                {
                   // PLXTipsWin(NULL, hWnd, 0, ML("password can't be empty"),ML("Pass Check"),Notify_Info,NULL, NULL, 20 );
					//PLXTipsWin(NULL, hWnd, 0, ML("Please,give\r\nall codes"),NULL, Notify_Alert, ML("Ok"), NULL,WAITTIMEOUT);
                    
                    break;
                }
                //PostMessage(p->hSuperWnd,WM_KEYDOWN,VK_RETURN,(LPARAM)p->szPassword);
                //Free(szButtonText);
                
                //DestroyWindow(hWnd);
				if (strlen((PSTR)p->szPassword) >=4) {
					
					PostMessage(p->hSuperWnd, p->uMsg, 1, (LPARAM)p->szPassword);
					PostMessage(hWnd,WM_CLOSE,0,0);
				}
				else
				{
					SendMessage(p->hSuperWnd,WM_KEYDOWN,VK_RETURN,(LPARAM)p->szPassword);
				}
                break;
			}
			break;
		case VK_F10:	//取消
            {
				int nLen;
				nLen = SendMessage(hEdit,WM_GETTEXTLENGTH,0,0);

                if(nLen == 0)
                {
                   // strcpy(p->szPassword,"");
                    /*
                    DestroyWindow(hWnd);
                    PostMessage(hWnd,PC_EXITLOOP,0,0);
					SendMessage(p->hSuperWnd, WM_KEYDOWN, VK_F10, 0);
					*/
                }
                else
                {
					OldEditProc(hEdit,WM_KEYDOWN,VK_BACK,0);					
                }
          
            }
            break;
		case VK_F1:
			{
				int  nInputLen;
				
				nInputLen = SendMessage(hEdit,WM_GETTEXTLENGTH,0,0);
				
				SendMessage(hEdit,WM_GETTEXT,(WPARAM)(nInputLen+1),(LPARAM)p->szPassword);
				
				
				SendMessage(p->hSuperWnd,WM_KEYDOWN,VK_F1,(LPARAM)p->szPassword);
				/*@**#---2005-07-20 16:20:58 (mxlin)---#**@

				DestroyWindow(hWnd);
				PostMessage(hWnd,PC_EXITLOOP,0,0);*/
			}
				
			break;
			
//		default:
//			lResult = OldEditProc(hWnd,WM_KEYDOWN,wParam,lParam);
//			break;            
		};	
        break;
    
	default :

        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);

        break;
	
    };

	return lResult;

}
#if 1
LRESULT CALLBACK MyEdit_WndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam,
                              LPARAM lParam)
{
    LRESULT lResult;

	int		oldbm;
	RECT	rcClient;
	HDC		hdc;
	COLORREF	cr;
    
    switch (wMsgCmd)
    {
    case WM_CHAR :  
		
        if((WORD)wParam >= '0' && (WORD)wParam <= '9')
            lResult = OldEditProc(hWnd, wMsgCmd, wParam, lParam);
        break;

	case WM_PAINT:
		if (bRepaint) {
			GetClientRect(hWnd, &rcClient);
			hdc = BeginPaint(hWnd, NULL);
			oldbm = SetBkMode(hdc, TRANSPARENT);
			cr= SetTextColor(hdc, COLOR_WHITE);
			StretchBlt(hdc, rcClient.left, rcClient.top, rcClient.right - rcClient.left,
				rcClient.bottom - rcClient.top, (HDC)hbmpFocus, 0, 0, rcClient.right - rcClient.left,
				rcClient.bottom - rcClient.top, ROP_SRC);
			DrawText(hdc, pstrtxt, -1, &rcClient, DT_VCENTER|DT_CENTER);
			SetBkMode(hdc,oldbm);
			SetTextColor(hdc,cr);
			EndPaint(hWnd,NULL);
			bRepaint = FALSE;
			nTimerId = SetTimer(hWnd, 1, 2000, NULL);
		}
		else
		{
			lResult = OldEditProc(hWnd, wMsgCmd, wParam, lParam);
		}
		break;

	case WM_TIMER:
		KillTimer(hWnd, 1);
		bRepaint = FALSE;
		InvalidateRect(hWnd, NULL, TRUE);
		break;

    default:
        
        lResult = CallWindowProc(OldEditProc, hWnd, wMsgCmd, wParam, lParam);
        break;
    }
    return lResult;
}
#endif

BOOL findstr(char **src,char *dec)
{
	char	**tmp;
	tmp = src;
	while (stricmp(*tmp,dec)!=0)
	{
		*tmp++;
		if (strlen(*tmp)<=0) {
			return FALSE;
		}
	}
	return TRUE;
}
