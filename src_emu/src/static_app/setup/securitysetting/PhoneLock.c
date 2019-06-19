/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : setup
 *
 * Purpose  : security setting
 *            
\**************************************************************************/

#include "PhoneLock.h"

BOOL  CallPhoneLockWnd(HWND hWnd)
{
	WNDCLASS wc;	
	HWND                 hPhoneLockwnd = NULL;
	RECT       rect;
	
	wc.style         = CS_OWNDC;
	wc.lpfnWndProc   = AppPhoneLockWndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = NULL;
	wc.hIcon         = NULL;
	wc.hCursor       = NULL;
	wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = "PhoneLockWndClass";
	
	if (!RegisterClass(&wc))
	{
		return FALSE;
	}
	if ( hPhoneLockwnd )
	{
		ShowWindow ( hPhoneLockwnd, SW_SHOW );
		SetFocus ( hPhoneLockwnd );
	}
	else
	{   
		GetClientRect(hFrameWin, &rect);

		hPhoneLockwnd = CreateWindow(
			"PhoneLockWndClass", 
			NULL,
			WS_CHILD | WS_VISIBLE, 
			rect.left,rect.top,rect.right-rect.left,rect.bottom-rect.top,
			hFrameWin, 
			NULL,
			NULL, 
			NULL
			);
		
		if (NULL == hPhoneLockwnd)
		{
			UnregisterClass("PhoneLockWndClass",NULL);
			return FALSE;
		}

		ShowWindow(hPhoneLockwnd, SW_SHOW);
		UpdateWindow(hPhoneLockwnd);
	
	}

	SendMessage(hFrameWin, PWM_SETBUTTONTEXT, (WPARAM)0, (LPARAM)ML(IDS_BACK)); 
	
	SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
	
	SetWindowText(hFrameWin, ML("Phone lock"));
	 
	return TRUE;


}
static LRESULT  AppPhoneLockWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult;


    lResult = (LRESULT)TRUE;

    switch ( wMsgCmd )
    {

    case WM_CREATE :
		PhoneLock_OnCreate(hWnd);
	    break;

	case WM_SETFOCUS:
		{
			hFocusPhoneLock = GetFocus();
			if(GetParent(hFocusPhoneLock) != hWnd)
			{
				hFocusPhoneLock = GetParent(hFocusPhoneLock);
			}
			SetFocus(hFocusPhoneLock);
		}
		break;

	case WM_KILLFOCUS:
		KillTimer(hWnd, iRepeatKeyTimerId_PC);
		v_nRepeats_PC = 0;
		nKeyCode_PC   = 0;
		break;

	case PWM_SHOWWINDOW:
		{
			SendMessage(hFrameWin, PWM_SETBUTTONTEXT, (WPARAM)1, (LPARAM)ML(""));

			SendMessage(hFrameWin, PWM_SETBUTTONTEXT, (WPARAM)0, (LPARAM)ML(IDS_BACK));
			
			SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
			
			SetWindowText(hFrameWin, ML("Phone lock"));

			SetFocus(hFocusPhoneLock);
		}
		break;

    case WM_CLOSE:
		SendMessage(hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hWnd,0);
        SendMessage ( hWnd, WM_COMMAND, (WPARAM)IDC_PHONELOCK_QUIT, 0 ); 
        break;

   	case WM_PAINT:
		PhoneLock_OnPaint(hWnd);
		break;

    case WM_DESTROY :
        UnregisterClass("PhoneLockWndClass",NULL);
        break;
		
	case CALLBACK_PERIOD:
		PhoneLock_OnCallBack(hWnd, lParam);
		break;
		
    case WM_KEYDOWN:
		if (v_nRepeats_PC > 0 && nKeyCode_PC != LOWORD(wParam))
		{
			KillTimer(hWnd, iRepeatKeyTimerId_PC);
			v_nRepeats_PC = 0;
		}
		v_nRepeats_PC++;
		
		nKeyCode_PC = LOWORD(wParam);
		PhoneLock_OnKeydown(hWnd, wMsgCmd, wParam, lParam);
		break;

	case WM_KEYUP:
		{	
			if(iRepeatKeyTimerId_PC !=0)
			{
				KillTimer(NULL, iRepeatKeyTimerId_PC);
				iRepeatKeyTimerId_PC = 0;
				nKeyCode_PC = 0;
			}
			v_nRepeats_PC = 0;
		}
		break;

	case WM_COMMAND:
		PhoneLock_OnCommand(hWnd, wParam, lParam);
		break;
	
	default :
			lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
			break;
	}
	return lResult;

}
/********************************************************************
* Function   PhoneLock_OnCreate  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
static BOOL   PhoneLock_OnCreate(HWND hWnd)
{
	HWND    hlockPeriod, hLockcode;
	char    pLockTime[20];
	
	hLockcode = CreateWindow("SETUPBUTTON", SECURITYLOCK,
		WS_VISIBLE|WS_CHILD|WS_TABSTOP|WS_BITMAP|CS_NOSYSCTRL,
		WIN_X, 0, ITEMWINDTH, ITEMHIGH,
		hWnd, (HMENU)IDC_LOCK, NULL,NULL);
	
	if(hLockcode == NULL)
		return FALSE;
	
	hlockPeriod = CreateWindow("SETUPBUTTON",SECURITYLOCKPERIOD,
		WS_VISIBLE | WS_CHILD | WS_TABSTOP |WS_BITMAP|CS_NOSYSCTRL,
		WIN_X, ITEMHIGH, ITEMWINDTH, ITEMHIGH,
		hWnd,(HMENU)IDC_LOCKPERIOD,NULL,NULL);
	
	if(hlockPeriod == NULL)
		return FALSE;
	
//	SendMessage(hlockPeriod, WM_SETARROW, 0, 0);
	
	memset(pLockTime, 0, 20);
	GetLockCodePeriod(pLockTime);
	
	if(strcmp(pLockTime, "0") == 0)
		SendMessage(hlockPeriod,WM_SETVALUE , 0, (LPARAM)ML(IDS_OFF));
	else
		SendMessage(hlockPeriod, WM_SETVALUE , 0, (LPARAM)ML(pLockTime));
	
	hFocusPhoneLock = hLockcode;

	SetFocus(hFocusPhoneLock);

	return TRUE;
}
/********************************************************************
* Function   PhoneLock_OnPaint  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
static void PhoneLock_OnPaint(HWND hWnd)
{
	HDC hdc;
	
	hdc = BeginPaint( hWnd, NULL);

	EndPaint(hWnd, NULL);
}
/********************************************************************
* Function   PhoneLock_OnKeydown  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
static void  PhoneLock_OnKeydown(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
	   switch(LOWORD(wParam))
	   {
	   case VK_F10:
		   if(bLockRequst)
		   {
			   bLockRequst = FALSE;
			   break;
		   }
		   SendMessage(hWnd,WM_CLOSE,0,0);
		   break;
	   case VK_RETURN:
		   {
			   
			   
		   }
		   break;
		   
	   case VK_DOWN:
		   
		   hFocusPhoneLock = GetFocus();
		   
		   while(GetParent(hFocusPhoneLock) != hWnd)
			   hFocusPhoneLock = GetParent(hFocusPhoneLock);
		   
		   hFocusPhoneLock = GetNextDlgTabItem(hWnd, hFocusPhoneLock, FALSE);
		   SetFocus(hFocusPhoneLock);
		   
		   SendMessage(hWnd, WM_VSCROLL, SB_LINEDOWN, NULL);
		   // InvalidateRect(hWnd, NULL, TRUE);
		   if(v_nRepeats_PC == 1)
		   {
			   iRepeatKeyTimerId_PC = SetTimer(NULL, 0,  ET_REPEAT_FIRST, f_PCTimerProc);
		   }
		   break;
	   case VK_UP:
		   
		   hFocusPhoneLock = GetFocus();
		   
		   while(GetParent(hFocusPhoneLock) != hWnd)
			   hFocusPhoneLock = GetParent(hFocusPhoneLock);
		   
		   hFocusPhoneLock = GetNextDlgTabItem(hWnd, hFocusPhoneLock, TRUE);
		   SetFocus(hFocusPhoneLock);
		   SendMessage(hWnd, WM_VSCROLL, SB_LINEUP, NULL);
		   // InvalidateRect(hWnd, NULL, TRUE);
		   if(v_nRepeats_PC == 1)
		   {
			   iRepeatKeyTimerId_PC = SetTimer(NULL, 0,  ET_REPEAT_FIRST, f_PCTimerProc);
		   }
		   break;
	   case VK_LEFT:
	   case VK_RIGHT:
		   {
//			   BOOL bLeft = FALSE;
//			   
//			   if(LOWORD(wParam) == VK_LEFT)
//				   bLeft = TRUE;
//			   
//			   if(GetFocus() == GetDlgItem(hWnd, IDC_LOCKPERIOD))
//			   {
//				   char pLockTime[20];
//				   int i;
//				   
//				   memset(pLockTime, 0, 20);
//				   GetLockCodePeriod(pLockTime);
//				   
//				   i = 0;
//				   while(i < MAX_PERIODTIME)
//				   {
//					   if(strcmp(selecttime[i], pLockTime) == 0)
//						   break;
//					   i++;
//				   }
//				   if(i == MAX_PERIODTIME)
//					   i = 0;
//				   
//				   if(bLeft)
//				   {
//					   if(i == 0)
//					   {
//						   SendMessage(GetDlgItem(hWnd, IDC_LOCKPERIOD),WM_SETVALUE , 0, (LPARAM)selecttime[MAX_PERIODTIME -1]);
//						   SetLockCodePeriod(selecttime[MAX_PERIODTIME-1]);
//					   }
//					   else
//					   {
//						   SendMessage(GetDlgItem(hWnd, IDC_LOCKPERIOD),WM_SETVALUE , 0, (LPARAM)selecttime[i-1]);
//						   SetLockCodePeriod(selecttime[i-1]);
//					   }
//				   }
//				   else
//				   {
//					   if(i== MAX_PERIODTIME -1)
//					   {
//						   SendMessage(GetDlgItem(hWnd, IDC_LOCKPERIOD),WM_SETVALUE , 0, (LPARAM)selecttime[0]);
//						   SetLockCodePeriod(selecttime[0]);
//					   }
//					   else
//					   {
//						   SendMessage(GetDlgItem(hWnd, IDC_LOCKPERIOD),WM_SETVALUE , 0, (LPARAM)selecttime[i+1]);
//						   SetLockCodePeriod(selecttime[i+1]);
//					   }
//					   
//				   }
//			   }
//			   InvalidateRect(hWnd, NULL, TRUE);
		   }
		   break;
		   
	   default:
		   PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
		   break;
	   }
	   
}
/********************************************************************
* Function   PhoneLock_OnCommand  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
static void PhoneLock_OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch( LOWORD( wParam ))
	{
	case IDC_LOCKPERIOD:
		if(HIWORD(wParam) == BN_CLICKED)
		{
			char strCode[EDITMAXLEN+1];
			char cLockcode[EDITMAXLEN +1];
			
			
			memset(strCode, 0, EDITMAXLEN + 1);
			memset(cLockcode, 0, EDITMAXLEN + 1);
			
 			if(SSPLXVerifyPassword(hFrameWin, NULL, ML(PHONELOCK), 
				strCode, MAKEWPARAM(EDITMINLEN,EDITMAXLEN),ML(IDS_OK),ML(IDS_CANCEL), -1))
			{
			
				GetSecurity_code(cLockcode);
				
				if (strCode[0] != 0 && cLockcode[0] !=0)
				{
					if(strcmp(strCode,cLockcode) != 0)
					{
						PLXTipsWin(hFrameWin, hWnd, NULL, ML(WRONG_PHONELOCK), ML(PHONELOCKCODE),
							Notify_Failure, ML(IDS_OK),  NULL, WAITTIMEOUT);								
						
					}
					else
					{
						bLockRequst = FALSE;
						CreateLockTimeWnd(hWnd, CALLBACK_PERIOD);
					}
				}
			}

		}
		break;
		
	case IDC_LOCK:
		if(HIWORD(wParam) == BN_CLICKED)
		{
			BOOL bLockState = TRUE;
			
			if(bLockState)
			{
				bModifyCode = TRUE;
				nQueryCodeType = REQUEST_LOCKCODE;
				CallModifyCodeWindow(hWnd,0, PHONELOCKCODE, CALLBACK_CODE);
			}
		}
		break;
		
	case IDC_PHONELOCK_QUIT:
		DestroyWindow(hWnd);
		break;
	}
}
/********************************************************************
* Function   PhoneLock_OnActive  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
static void PhoneLock_OnActive(HWND hWnd, WPARAM wParam)
{

	if (WA_INACTIVE != LOWORD(wParam))
	{
		SetFocus(hFocusPhoneLock);
	}
	else
	{
		hFocusPhoneLock = GetFocus();
		if(GetParent(hFocusPhoneLock) != hWnd)
		{
			hFocusPhoneLock = GetParent(hFocusPhoneLock);
		}
	}
}
/********************************************************************
* Function   PhoneLock_OnCallBack  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
static void PhoneLock_OnCallBack(HWND hWnd, LPARAM lParam)
{
	if(lParam != NULL)
	{
		char strTime[20];
		
		memset(strTime, 0, 20);
		strcpy(strTime, (char*)lParam);
		
		SetLockCodePeriod((char*)strTime);
		SendMessage(GetDlgItem(hWnd, IDC_LOCKPERIOD), WM_SETVALUE, 1, lParam);
	}
}
/********************************************************************
* Function   f_PCTimerProc  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
static void CALLBACK f_PCTimerProc(HWND hWnd, UINT uMsg, UINT idEvent,
                                 DWORD dwTime)
{
	if(v_nRepeats_PC == 1)
	{
		KillTimer(NULL, iRepeatKeyTimerId_PC);
		iRepeatKeyTimerId_PC = SetTimer(NULL,0,  ET_REPEAT_LATER,f_PCTimerProc );
	}
	keybd_event(nKeyCode_PC, 0, 0, 0);
}
/********************************************************************
* Function   CreateLockTimeWnd  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
static BOOL CreateLockTimeWnd(HWND hWnd, UINT wMsg)
{
	WNDCLASS wc;
	HWND					hLockTimeWnd = NULL;
	RECT rect;

	wc.style         = CS_OWNDC;
	wc.lpfnWndProc   = AppLockTimeWndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = NULL;
	wc.hIcon         = NULL;
	wc.hCursor       = NULL;
	wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = "LockTimeWndClass";
	
	if (!RegisterClass(&wc))
	{
		return FALSE;
	}
	if ( hLockTimeWnd )
	{
		ShowWindow ( hLockTimeWnd, SW_SHOW );
		SetFocus ( hLockTimeWnd );
	}
	else
	{    
        GetClientRect(hFrameWin, &rect);

		hLockTimeWnd = CreateWindow(
			"LockTimeWndClass", 
			NULL,
			WS_CHILD | WS_VISIBLE, 
			rect.left,rect.top,rect.right-rect.left,rect.bottom-rect.top,
			hFrameWin, 
			NULL,
			NULL, 
			NULL
			);
		
		if (NULL == hLockTimeWnd)
		{
			UnregisterClass("LockTimeWndClass",NULL);
			return FALSE;
		}

		ShowWindow(hLockTimeWnd, SW_SHOW);
		UpdateWindow(hLockTimeWnd);
	}
	hCallwnd = hWnd;
	CallMsg = wMsg;

	return TRUE;
	
}
static LRESULT AppLockTimeWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
	LRESULT lResult;
	HDC hdc;

    lResult = (LRESULT)TRUE;
    switch ( wMsgCmd )
    {
    case WM_CREATE :
		LockTime_OnCreate(hWnd, &hList);
        break;
	case PWM_SHOWWINDOW:
		{
			SendMessage(hFrameWin, PWM_SETBUTTONTEXT, (WPARAM)1, (LPARAM)ML(""));

			SendMessage(hFrameWin, PWM_SETBUTTONTEXT, (WPARAM)0, (LPARAM)ML(IDS_CANCEL));
			
			SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
			
			SetWindowText(hFrameWin, ML("Autolock period"));

			SetFocus(hTimeOutFocus);
		}
		break;

    case WM_CLOSE:
		SendMessage(hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hWnd,0);
        SendMessage ( hWnd, WM_COMMAND, (WPARAM)IDC_LOCKTIME_QUIT, 0 ); 
        break;

   	case WM_PAINT:
		hdc = BeginPaint( hWnd, NULL);
        EndPaint(hWnd, NULL);
		break;

    case WM_DESTROY :
		LockTime_OnDestroy(hWnd);
        break;

	case WM_TIMER:
		{
			int nFocusItem;
			
			nFocusItem = SendMessage(hList, LB_GETCURSEL, 0, 0);
			SendMessage(hCallwnd, CallMsg, 1, (LPARAM)selecttime[nFocusItem]);
			PostMessage(hWnd, WM_CLOSE, 0, 0);
		}
		break;
		
    case WM_KEYDOWN:
		LockTime_OnKeydown(hWnd, wMsgCmd, wParam, lParam);
        break;

    case WM_COMMAND:
        LockTime_OnCommand(hWnd, wMsgCmd, wParam, lParam);
		break;
		
	default :
		lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
			break;
	}
	return lResult;
}
/********************************************************************
* Function   LockTime_OnCreate  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
static BOOL LockTime_OnCreate(HWND hWnd, HWND* hList)
{
	HDC hdc;
	
	int i = 0;

	char pLockTime[20];
	
	SendMessage(hFrameWin, PWM_SETBUTTONTEXT, (WPARAM)0, (LPARAM)ML(IDS_CANCEL)); 
	
	SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
	
	SetWindowText(hFrameWin, ML("Autolock period"));
	
	*hList = CreateWindow("LISTBOX","", 
        WS_VISIBLE | WS_CHILD | WS_TABSTOP | LBS_BITMAP |WS_VSCROLL,
        0, 0 , PLX_WIN_WIDTH, PLX_WIN_HIGH - 46,
        hWnd, (HMENU)IDC_TIMEOUT, NULL, NULL );
	
	if(*hList == NULL)
		return FALSE;
	
	while(i< MAX_PERIODTIME)
	{
		SendMessage(*hList,LB_ADDSTRING,NULL,(LPARAM)ML(selecttime[i]));
		i++;
	}
	
	memset(pLockTime, 0, 20);
	GetLockCodePeriod(pLockTime);

	i = 0;
	while(i < MAX_PERIODTIME)
	{
		if(strcmp(selecttime[i], pLockTime) == 0)
			break;
		i++;
	}
	
	if(i == MAX_PERIODTIME)
		i = 0;
	
	hdc         = GetDC(hWnd);
	hIconNormal = LoadImageEx(hdc,NULL,ICONORMAL,IMAGE_BITMAP,17,16,LR_LOADFROMFILE);
	hIconSel = LoadImageEx(hdc,NULL,ICONSEL,IMAGE_BITMAP,17,16,LR_LOADFROMFILE);
	ReleaseDC(hWnd,hdc);
	Load_Icon_SetupList(*hList,hIconNormal,hIconSel, MAX_PERIODTIME,i);
	SendMessage(*hList, LB_SETCURSEL, i, 0);
	hTimeOutFocus = *hList;

	SetFocus(hTimeOutFocus);


	return TRUE;
}
/********************************************************************
* Function   LockTime_OnKeydown  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
static void LockTime_OnKeydown(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
	
	switch(LOWORD(wParam))
	{
	case VK_F10:
		SendMessage(hWnd,WM_CLOSE,0,0);
		break;

	case VK_F5:
		{
			int iIndex = 0;
			
			iIndex = SendMessage(hList,LB_GETCURSEL,0,0);
			Load_Icon_SetupList(hList,hIconNormal,hIconSel, MAX_PERIODTIME,iIndex);
			SetTimer(hWnd, 1, 500, NULL);
		}
		break;

	default:
		PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
	}
}
/********************************************************************
* Function   LockTime_OnCommand  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
static void LockTime_OnCommand(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
	switch( LOWORD( wParam ))
	{
		char strTime[5];
		memset(strTime, 0, 5);
		
	case IDC_TIMEOUT:
		if(HIWORD(wParam) == LBN_SELCHANGE)
		{
			int iIndex = 0;
			
//			iIndex = SendMessage(hList,LB_GETCURSEL,0,0);
//			Load_Icon_SetupList(hList,hIconNormal,hIconSel, MAX_PERIODTIME,iIndex);
		}
		else
			PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
		break;
		
	case IDC_LOCKTIME_QUIT:
		DestroyWindow(hWnd);
		break;
	}

}
/********************************************************************
* Function   LockTime_OnDestroy  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
static void LockTime_OnDestroy(HWND hWnd)
{
	KillTimer(hWnd, 1);
	if(hIconNormal!= NULL)
		DeleteObject(hIconNormal);
	if(hIconSel != NULL)
		DeleteObject(hIconSel);
	UnregisterClass("LockTimeWndClass",NULL);
}
