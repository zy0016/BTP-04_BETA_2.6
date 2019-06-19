                
#include "FixedDial.h"


BOOL CreateFixedDialWnd(HWND hParentWnd, BOOL bFD_Status, UINT Msg)
{
	WNDCLASS wc;
	HWND hFixedDialWnd = NULL;
	RECT rect;
	
	bFDStatus = bFD_Status;
	CallMsgMain = Msg;

 	wc.style         = CS_OWNDC;
	wc.lpfnWndProc   = FixedDialWndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = NULL;
	wc.hIcon         = NULL;
	wc.hCursor       = NULL;
	wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = "fixeddialclass";
	
	if (!RegisterClass(&wc))
	{
		return FALSE;
	}
	if ( hFixedDialWnd )
	{
		ShowWindow ( hFixedDialWnd, SW_SHOW );
		SetFocus ( hFixedDialWnd );	
	}
	else
	{       
		GetClientRect(hFrameWin, &rect);

		hFixedDialWnd = CreateWindow(
			"fixeddialclass", 
			NULL,
			WS_CHILD|WS_VISIBLE, 
			rect.left,rect.top,rect.right-rect.left,rect.bottom-rect.top,
			hFrameWin, 
			NULL,
			NULL, 
			NULL
			);
		
		if (NULL == hFixedDialWnd)
		{
			UnregisterClass("fixeddialclass",NULL);
			return FALSE;
		}
		ShowWindow(hFixedDialWnd, SW_SHOW);
		UpdateWindow(hFixedDialWnd);
		
	}
	hFixedDialParentWnd = hParentWnd;

	return TRUE;
	
}
static LRESULT FixedDialWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
	
	BOOL    bPin = FALSE;
	LRESULT lResult;
	HDC     hdc;
	
    lResult = (LRESULT)TRUE;
    switch ( wMsgCmd )
    {
    case WM_CREATE :
        CreateFixedDialControl(hWnd);
        break;

	case PWM_SHOWWINDOW:
		{
			SendMessage(hFrameWin, PWM_SETBUTTONTEXT, (WPARAM)1, (LPARAM)ML(""));

			SendMessage(hFrameWin, PWM_SETBUTTONTEXT, (WPARAM)0, (LPARAM)ML(IDS_BACK));
			SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
			
			SetWindowText(hFrameWin, ML("Fixed dialing"));

			SetFocus(hFixedDialFocusWnd);
		}
		break;
 
    case WM_CLOSE:
		SendMessage(hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hWnd,0);
        SendMessage ( hWnd, WM_COMMAND, (WPARAM)IDC_FIXEDDIAL_QUIT, 0 ); 
        break;

    case WM_DESTROY :
		hFixedDialFocusWnd = NULL;
	    UnregisterClass("fixeddialclass",NULL);
        break;
		
	case CALLBACK_FDSET:
		FD_OnCallback_FDSet(hWnd, wParam, lParam);
		break;

	case CALLBACK_CODE_REMAIN:
		FD_OnCallbackCodeRemain(hWnd);
		break;

	case REQUEST_CODE:
		{
			char strCode[EDITMAXLEN + 1];
			
			
			if(!bGivePin2 && !bGivePuk2)
				break;
					
			memset(strCode, 0, EDITMAXLEN + 1);

			if(bGivePin2)
			{
				if((iremains == 0)|| (iremains == PUKNUMS) || (iremains > PINCODE_MAXLIMIT))
				{
					if(	SSPLXVerifyPassword(hFrameWin,NULL, ML(PUK2CODE), strCode,
						MAKEWPARAM(EDITMAXLEN,EDITMAXLEN), ML(IDS_OK),ML(IDS_CANCEL), -1))
					{
						strcpy(strPuk2, strCode);
						nGiveCodeType = REQUEST_PUK2;
						CallNewCodeWindow(hWnd,REQUEST_PIN2, PIN2_CODE, CALLBACK_NEWCODE);		
					}
					else
						bGivePin2 = FALSE;
					
				}
				else
				{
					if(	SSPLXVerifyPassword(hFrameWin, NULL, ML(PIN2CODE), strCode, 
						MAKEWPARAM(EDITMINLEN,EDITMAXLEN), ML(IDS_OK),ML(IDS_CANCEL), -1))
					{
						nGiveCodeType = REQUEST_PIN2;
						strcpy(strPin2, strCode);
						ME_PasswordValidation(hWnd, CALLBACK_CODE, PIN2, strCode, NULL);
						WaitWindowState(hWnd,TRUE);
					}
					else
						bGivePin2 = FALSE;
				}
				
			}
			if(bGivePuk2)
			{
				if((iremains != 0))
				{
					if(	SSPLXVerifyPassword(hFrameWin, NULL, ML(PUK2CODE), strCode, 
						MAKEWPARAM(EDITMAXLEN,EDITMAXLEN), ML(IDS_OK),ML(IDS_CANCEL), -1))
					{
						strcpy(strPuk2, strCode);
						nGiveCodeType = REQUEST_PUK2;
						CallNewCodeWindow(hWnd,REQUEST_PIN2, PIN2_CODE, CALLBACK_NEWCODE);
						
					}
					else
						bGivePuk2 = FALSE;
				}
			}
			
		}
		break;
		
	case CALLBACK_NEWCODE:
		FD_OnCallbackNewCode(hWnd, wParam, lParam);
		break;

	case WM_PIN_CONFIRM:
		switch (wParam)
		{
		case ME_RS_SUCCESS:
			{	
				iPinStatus.blockCHV2 = FALSE;
				bHaveRequestPIN2 = TRUE;
				PLXTipsWin(hFrameWin, hWnd, NULL,ML(PIN2CODE_CHANGED), ML(TITLECAPTION), Notify_Success,
					ML(IDS_OK), NULL,DEFAULT_TIMEOUT);
						
				WaitWindowState(hWnd,FALSE);
			}
			break;
		case ME_RS_FAILURE:
			{
				ME_GetPassInputLimit(hWnd, CALLBACK_CODE_REMAIN, PUK2);	
			}
			break;
		}	
//		ME_GetCurWaitingPassword(hWnd, WM_PIN_QUERY);
		break;
		
	case WM_PIN_QUERY:
		FD_OnPinQuery(hWnd);
		break;

	case CALLBACK_CODE:
		FD_OnCallbackCode(hWnd,wParam, lParam);
		break;

    case WM_KEYDOWN:
		if (v_nRepeats_FD > 0 && nKeyCode_FD != LOWORD(wParam))
		{
			KillTimer(hWnd, iRepeatKeyTimerId_FD);
			v_nRepeats_FD = 0;
		}
		v_nRepeats_FD++;
		
		nKeyCode_FD = LOWORD(wParam);
		FD_OnKeyDown(hWnd, wMsgCmd, wParam, lParam);
        break;

	case WM_KEYUP:
		{	
			if(iRepeatKeyTimerId_FD !=0)
			{
				KillTimer(NULL, iRepeatKeyTimerId_FD);
				iRepeatKeyTimerId_FD = 0;
				nKeyCode_FD = 0;
			}
			v_nRepeats_FD = 0;
		}
		break;

	case WM_PAINT:
		hdc = BeginPaint( hWnd, NULL);
		EndPaint(hWnd, NULL);	
		break;

    case WM_COMMAND:
		FD_OnCommand(hWnd, wParam, lParam);
		break;

	default :
		lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
		break;
	}
	return lResult;
}
/********************************************************************
* Function   f_FDTimerProc  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
static void CALLBACK f_FDTimerProc(HWND hWnd, UINT uMsg, UINT idEvent,
                                 DWORD dwTime)
{
	if(v_nRepeats_FD == 1)
	{
		KillTimer(NULL, iRepeatKeyTimerId_FD);
		iRepeatKeyTimerId_FD = SetTimer(NULL,0,  ET_REPEAT_LATER,f_FDTimerProc );
	}
	keybd_event(nKeyCode_FD, 0, 0, 0);
}
/********************************************************************
* Function   CreateFixedDialControl  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
static BOOL CreateFixedDialControl(HWND hWnd)
{
	HWND  hFixedDial, hFixedList;
	
	int  LineY;
	
	SendMessage(hFrameWin, PWM_SETBUTTONTEXT, (WPARAM)0, (LPARAM)ML(IDS_BACK)); 
	
	SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");

	SetWindowText(hFrameWin, ML("Fixed dialing"));
	
	hFixedDial = CreateWindow("SETUPBUTTON",SECURITYFIXEDDIAL,
		WS_VISIBLE | WS_CHILD | WS_TABSTOP|WS_BITMAP|CS_NOSYSCTRL,
		WIN_X, WIN_Y, ITEMWINDTH, ITEMHIGH,
		hWnd,(HMENU)IDC_FIXEDDIALSET,NULL,NULL);
	
	if(hFixedDial == NULL)
		return FALSE;
		
	LineY = WIN_Y + ITEMHIGH;
    
	if(bFDStatus)
		SendMessage(hFixedDial, WM_SETVALUE , 0, (LPARAM)ML(IDS_ON));
	else
		SendMessage(hFixedDial, WM_SETVALUE , 0, (LPARAM)ML(IDS_OFF));

	//SendMessage(hFixedDial, WM_SETARROW, 0, 0);

	hFixedList = CreateWindow("SETUPBUTTON", ML("Fixed nr list") ,
		WS_VISIBLE|WS_CHILD|WS_TABSTOP|WS_BITMAP|CS_NOSYSCTRL,
		WIN_X, LineY, ITEMWINDTH, ITEMHIGH,
		hWnd, (HMENU)IDC_FIXEDLIST, NULL,NULL);
	
	if(hFixedList == NULL)
		return FALSE;

	hFixedDialFocusWnd = hFixedDial;

	SetFocus(hFixedDialFocusWnd);

    return TRUE;

}
/********************************************************************
* Function   FD_OnCallback_FDSet  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
extern void SetSimStatus_FDN(SimService *pSimSer);
static void FD_OnCallback_FDSet(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	SimService SimSer;

	memset(&SimSer, 0, sizeof(SimService));

	WaitWindowState(hWnd,FALSE);
	switch (wParam)
	{
	case ME_RS_SUCCESS:
		bFDStatus = !bFDStatus;
		if(bSetFD)
		{
			bSetFD = FALSE;
			if(bFDStatus)
				SendMessage(GetDlgItem(hWnd, IDC_FIXEDDIALSET) , WM_SETVALUE, 0, (LPARAM)ML(IDS_ON));
			else
				SendMessage(GetDlgItem(hWnd, IDC_FIXEDDIALSET) , WM_SETVALUE, 0, (LPARAM)ML(IDS_OFF));
		}

		SendMessage(hFixedDialParentWnd, CallMsgMain, 1, bFDStatus);
		if(bFDStatus)
			SimSer.CurrentStatus = Activated;
		else
			SimSer.CurrentStatus = Inactivated;
		SimSer.SupportStatus = Allocated;
        SetSimStatus_FDN(&SimSer);

		break;	

	case ME_RS_FAILURE:
		{
			if(lParam == 3)
			{
				bFDStatus = !bFDStatus;
				if(bSetFD)
				{
					bSetFD = FALSE;
					if(bFDStatus)
						SendMessage(GetDlgItem(hWnd, IDC_FIXEDDIALSET) , WM_SETVALUE, 0, (LPARAM)ML(IDS_ON));
					else
						SendMessage(GetDlgItem(hWnd, IDC_FIXEDDIALSET) , WM_SETVALUE, 0, (LPARAM)ML(IDS_OFF));
				}
				SendMessage(hFixedDialParentWnd, CallMsgMain, 1, bFDStatus);
			}
			else
			{
				if (-1 != ME_GetPassInputLimit(hWnd,CALLBACK_CODE_REMAIN,PIN2))
					WaitWindowState(hWnd,TRUE);
			}
		}
		break;
	default:
		{
			if (-1 != ME_GetPassInputLimit(hWnd,CALLBACK_CODE_REMAIN,PIN2))
				WaitWindowState(hWnd,TRUE);
		}
		break;
	}
}
/********************************************************************
* Function   FD_OnCallbackCodeRemain  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
static void FD_OnCallbackCodeRemain(HWND hWnd)
{
	iremains = 0;      
	
	WaitWindowState(hWnd,FALSE);
	
	ME_GetResult(&iremains,sizeof(int));
	if ((iremains == 0) || (iremains == PUKNUMS) /*|| (iremains > PINCODE_MAXLIMIT)*/)
	{
		switch(nGiveCodeType)
		{
		case REQUEST_PIN2:
			{
				iPinStatus.blockCHV2 = TRUE;

				nGiveCodeType = REQUEST_PUK2;
				bGivePin2=FALSE;
				bGivePuk2 = TRUE;
				bSetFD = FALSE;

				if(!bNotNotify)
					PLXTipsWin(hFrameWin, hWnd, REQUEST_CODE, ML(PIN2_BLOCKED),ML(PIN2_CODE), Notify_Failure, ML(IDS_OK), NULL, 0);
				else
				{
					bNotNotify = FALSE;
					PLXTipsWin(hFrameWin, hWnd, REQUEST_CODE, ML(PIN2_BLOCKED),ML(PIN2_CODE), Notify_Failure, ML(IDS_OK), NULL, 0);
					//SendMessage(hWnd, REQUEST_CODE, 0, 0);
				}
		
			}
			break;
			
		case REQUEST_PUK2:
			{
				iPinStatus.blockPUK2 = TRUE;
				bGivePuk2 = FALSE;
				if(iremains == 0)
					PLXTipsWin(hFrameWin, hWnd, NULL,ML(PUK2_BLOCKED),ML(PUK2_CODE), Notify_Alert, ML(IDS_OK), NULL, 0);
//				else
//				{
//					bNotNotify = FALSE;
//					PLXTipsWin(hFrameWin, hWnd, NULL,ML(WRONG_PUK2CODE),ML(PUK2_CODE), Notify_Alert, ML(IDS_OK), NULL, 0);
//					SendMessage(hWnd, REQUEST_CODE, 0, 0);
//				}
				
			}
			break;	
		}	
	}
	else
	{
		switch(nGiveCodeType)
		{
		case REQUEST_PIN2:

			nGiveCodeType = REQUEST_PIN2;
			bGivePin2=TRUE;

			if(!bEdit && ! bDelete && !bNotNotify)
				PLXTipsWin(hFrameWin, hWnd, REQUEST_CODE, ML(WRONG_PIN2), ML(PIN2_CODE), 
				Notify_Failure, ML(IDS_OK), NULL, DEFAULT_TIMEOUT);
		    else
			{
				bNotNotify = FALSE;
				SendMessage(hWnd, REQUEST_CODE, 0, 0);
			}
			break;
			
		case REQUEST_PUK2:
			if(!bNotNotify)
				PLXTipsWin(hFrameWin, hWnd, REQUEST_CODE, ML(WRONG_PUK2CODE), ML(PUK2_CODE), 
				Notify_Failure, ML(IDS_OK), NULL, 0);
		
			break;
			
		}
	}
}
/********************************************************************
* Function   FD_OnCallbackNewCode  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
static void	FD_OnCallbackNewCode(HWND hWnd, WPARAM wParam,LPARAM lParam)
{
	if(lParam == NULL)
		return;
	
	if(LOWORD(wParam) == REQUEST_PIN2)
	{
		memset(strPin2, 0, EDITMAXLEN+1);
		strcpy(strPin2, (char*)lParam);
		ME_PasswordValidation(hWnd, WM_PIN_CONFIRM, PUK2, strPuk2, strPin2);
	}			
}
/********************************************************************
* Function   FD_OnCallbackCode  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
static void	FD_OnCallbackCode(HWND hWnd,WPARAM wParam, LPARAM lParam)
{
	int nType;
	
	WaitWindowState(hWnd,FALSE);
	switch (wParam)
	{
	case ME_RS_SUCCESS://on or off pin
		if(bGivePin2)
		{
			bGivePin2 =FALSE;
			if(bSetFD)
			{
				ME_SetPhoneLockStatus(hWnd, CALLBACK_FDSET,	LOCK_FD, strPin2 , !bFDStatus );			
			}
			else
				CreateFixedDialSetWnd(hWnd);
			return;
		}
		break;	
	case ME_RS_FAILURE:
		if(lParam == 3)
		{
			bGivePin2 =FALSE;
			if(bSetFD)
			{
				ME_SetPhoneLockStatus(hWnd, CALLBACK_FDSET,	LOCK_FD, strPin2 , !bFDStatus );			
			}
			else
				CreateFixedDialSetWnd(hWnd);
			return;
		}
		else
		{
			switch(nGiveCodeType)
			{
			case REQUEST_PIN2:
				nType = PIN2;
				break;
				
			case REQUEST_PUK2:
				nType = PUK2;
				break;
			}
			if (-1 != ME_GetPassInputLimit(hWnd,CALLBACK_CODE_REMAIN,nType))
				WaitWindowState(hWnd,TRUE);
			return;
		}
		break;
	default:
		switch(nGiveCodeType)
		{
		case REQUEST_PIN2:
			nType = PIN2;
			break;
			
		case REQUEST_PUK2:
			nType = PUK2;
			break;
		}
	}
	if (-1 != ME_GetPassInputLimit(hWnd,CALLBACK_CODE_REMAIN,nType))
		WaitWindowState(hWnd,TRUE);
}

/********************************************************************
* Function   FD_OnPinQuery  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
static void	FD_OnPinQuery(HWND hWnd)
{			
	int type;			
	ME_GetResult(&type, sizeof(int));
	if (type == NOPIN)
	{
		
		if(bGivePuk2)
		{
			PLXTipsWin(hFrameWin, hWnd, NULL, ML(PIN2CODE_CHANGED), ML(""), Notify_Success,
				ML(IDS_OK), NULL,DEFAULT_TIMEOUT);
			bGivePuk2 = FALSE;
		}
		WaitWindowState(hWnd ,FALSE);
		return;
	}
	else if(type == PUK2)
	{
		ME_GetPassInputLimit(hWnd, CALLBACK_NEWCODE, PUK1);
		return;
	}
}

/********************************************************************
* Function   FD_OnKeyDown  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
static void 	FD_OnKeyDown(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
	switch(LOWORD(wParam))
	{

	case VK_F10:
		{
//			if(bGivePin2 || bGivePuk2)
//			{
//				bGivePin2 = FALSE;
//				bGivePuk2 = FALSE;
//				bSetFD = FALSE;
//				break;
//			}
			SendMessage(hWnd,WM_CLOSE,0,0);
		}
		break;
	case VK_DOWN:
		
		hFixedDialFocusWnd = GetFocus();
		
		while(GetParent(hFixedDialFocusWnd) != hWnd)
			hFixedDialFocusWnd = GetParent(hFixedDialFocusWnd);
		
		hFixedDialFocusWnd = GetNextDlgTabItem(hWnd, hFixedDialFocusWnd, FALSE);
		SetFocus(hFixedDialFocusWnd);
		//	InvalidateRect(hWnd, NULL, TRUE);
		if(v_nRepeats_FD == 1)
		{
			iRepeatKeyTimerId_FD = SetTimer(NULL, 0,  ET_REPEAT_FIRST, f_FDTimerProc);
		}
		break;
	case VK_UP:
		
		hFixedDialFocusWnd = GetFocus();
		
		while(GetParent(hFixedDialFocusWnd) != hWnd)
			hFixedDialFocusWnd = GetParent(hFixedDialFocusWnd);
		
		hFixedDialFocusWnd = GetNextDlgTabItem(hWnd, hFixedDialFocusWnd, TRUE);
		SetFocus(hFixedDialFocusWnd);
	//	InvalidateRect(hWnd,NULL, TRUE);
		if(v_nRepeats_FD == 1)
		{
			iRepeatKeyTimerId_FD = SetTimer(NULL, 0,  ET_REPEAT_FIRST, f_FDTimerProc);
		}
		break;
//	case VK_LEFT:
//	case VK_RIGHT:
//		{
//			if(GetFocus() == GetDlgItem(hWnd, IDC_FIXEDDIALSET))
//			{
//				nGiveCodeType = REQUEST_PIN2;
//				bNotNotify  = TRUE;
//				bSetFD = TRUE;
//				if (-1 != ME_GetPassInputLimit(hWnd,CALLBACK_CODE_REMAIN,PIN2))
//					WaitWindowState(hWnd,TRUE);
//			}
//			InvalidateRect(hWnd, NULL, TRUE);
//		}
//		break;				
		
	default:
		PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
	}
}
	
/********************************************************************
* Function   FD_OnCommand  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
static void FD_OnCommand(HWND hWnd, WPARAM wParam , LPARAM lParam)
{
	switch( LOWORD( wParam ))
	{
		
	case IDC_FIXEDDIALSET:
		if(HIWORD(wParam) == BN_CLICKED)
		{
			nGiveCodeType = REQUEST_PIN2;
			bNotNotify  = TRUE;
			if (-1 != ME_GetPassInputLimit(hWnd,CALLBACK_CODE_REMAIN,PIN2))
				WaitWindowState(hWnd,TRUE);
		}
		break;
	case IDC_FIXEDLIST:
		if(HIWORD(wParam) == BN_CLICKED)
		{
			if(bFDStatus)
				CreateFixedDialListWnd(hWnd);
			else
				PLXTipsWin(hFrameWin, hWnd, NULL, ML("Fixed dialing\r\nis not on"), 
				ML("Fixed dialing"),Notify_Info, ML(IDS_OK), NULL, DEFAULT_TIMEOUT);
		}
		break;
	case IDC_FIXEDDIAL_QUIT:
		DestroyWindow(hWnd);
		break;
	}
}
/********************************************************************
* Function   CreateFixedDialSetWnd  
* Purpose    Set fixed dialing
* Params     
* Return     
* Remarks      
**********************************************************************/
static BOOL CreateFixedDialSetWnd(HWND hParentWnd)
{
	WNDCLASS wc;
	HWND hFDSetWnd = NULL;
	RECT rect;
	
	wc.style         = CS_OWNDC;
	wc.lpfnWndProc   = FDSetWndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = NULL;
	wc.hIcon         = NULL;
	wc.hCursor       = NULL;
	wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = "FDSetclass";
	
	if (!RegisterClass(&wc))
	{
		return FALSE;
	}
	if ( hFDSetWnd )
	{
		ShowWindow ( hFDSetWnd, SW_SHOW );
		SetFocus ( hFDSetWnd );	
	}
	else
	{     
		GetClientRect(hFrameWin, &rect);

		hFDSetWnd = CreateWindow(
			"FDSetclass", 
			NULL,
			WS_CHILD|WS_VISIBLE, 
			rect.left,rect.top,rect.right-rect.left,rect.bottom-rect.top,
			hFrameWin, 
			NULL,
			NULL, 
			NULL
			);
		
		if (NULL == hFDSetWnd)
		{
			UnregisterClass("FDSetclass",NULL);
			return FALSE;
		}
		ShowWindow(hFDSetWnd, SW_SHOW);
		UpdateWindow(hFDSetWnd);
		
	}
	hFDSetParentWnd = hParentWnd;
	return TRUE;
}

static LRESULT FDSetWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
	
	LRESULT lResult;
	HDC     hdc;
	static HWND hList;
	
    lResult = (LRESULT)TRUE;
    switch ( wMsgCmd )
    {
    case WM_CREATE :
		FDSet_OnCreate(hWnd, &hList);
        break;

	case PWM_SHOWWINDOW:
		{
			SendMessage(hFrameWin, PWM_SETBUTTONTEXT, (WPARAM)1, (LPARAM)ML(""));

			SendMessage(hFrameWin, PWM_SETBUTTONTEXT, (WPARAM)0, (LPARAM)ML(IDS_CANCEL));
			SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
			
			SetWindowText(hFrameWin, ML("Fixed dialing"));

			SetFocus(hFDSetFocusWnd);
		}
		break;
    		
    case WM_CLOSE:
		SendMessage(hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hWnd,0);
        SendMessage ( hWnd, WM_COMMAND, (WPARAM)IDC_FDSET_QUIT, 0 ); 
        break;
		
    case WM_DESTROY :
		hFDSetFocusWnd = NULL;
		if(hIconNormal != NULL)
		{
			DeleteObject(hIconNormal);
			hIconNormal  =NULL;
		}
		if(hIconSel != NULL)
		{
			DeleteObject(hIconSel);
			hIconSel = NULL;
		}
        UnregisterClass("FDSetclass",NULL);
        break;

		
    case WM_KEYDOWN:
		FDSet_OnKeydown(hWnd,&hList, wMsgCmd, wParam, lParam);
		break;

	case WM_PAINT:
		hdc = BeginPaint( hWnd, NULL);
		EndPaint(hWnd, NULL);	
		break;

    case WM_COMMAND:
		switch( LOWORD( wParam ))
		{
			
		case IDC_FDSET_LIST:
			if(HIWORD(wParam) == BN_CLICKED)
			{
			
			}
			break;
	
		case IDC_FDSET_QUIT:
			DestroyWindow(hWnd);
			break;
		}
		
		break;

	default :
		lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
		break;
	}
	return lResult;
}
/********************************************************************
* Function   FDSet_OnCreate  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
static BOOL FDSet_OnCreate(HWND hWnd, HWND *hList)
{
	int i = 0;
	HDC  hdc;
	RECT rect;
	char* seletitem[]=
	{
		"On",
		"Off"
	};
	
	SendMessage(hFrameWin, PWM_SETBUTTONTEXT, (WPARAM)0, (LPARAM)ML(IDS_CANCEL)); 
	
	SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
	
	SetWindowText(hFrameWin, ML("Fixed dialing"));
	
	GetClientRect(hWnd,&rect);
	
	*hList = CreateWindow("LISTBOX","", 
        WS_VISIBLE | WS_CHILD | WS_TABSTOP | LBS_BITMAP |WS_VSCROLL,
        rect.left, rect.top , rect.right - rect.left, rect.bottom - rect.top,
        hWnd, (HMENU)IDC_FDSET_LIST, NULL, NULL );
	
	if(*hList == NULL)
		return FALSE;
	
	while(i< 2)
	{
		SendMessage(*hList,LB_ADDSTRING,NULL,(LPARAM)ML(seletitem[i]));
		i++;
	}
	
	SendMessage(*hList,LB_SETCURSEL,0,0);
	
	hFDSetFocusWnd = *hList;
	SetFocus(hFDSetFocusWnd);

	hdc         = GetDC(hWnd);
	hIconNormal = LoadImageEx(hdc,NULL,ICONORMAL,IMAGE_BITMAP,17,16,LR_LOADFROMFILE);
	hIconSel = LoadImageEx(hdc,NULL,ICONSEL,IMAGE_BITMAP,17,16,LR_LOADFROMFILE);
	ReleaseDC(hWnd,hdc);

	Load_Icon_SetupList(*hList,hIconNormal,hIconSel, 2,!bFDStatus);
    SendMessage(*hList, LB_SETCURSEL, 0, 0);
	hFDSetFocusWnd = *hList;
	
	return TRUE;
}
/********************************************************************
* Function   FDSet_OnKeydown  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
static void FDSet_OnKeydown(HWND hWnd, HWND *hList,UINT wMsgCmd, WPARAM wParam, LPARAM lParam)

{
	switch(LOWORD(wParam))
	{
	case VK_F5:
		{
			int nFocusItem;
			
			nFocusItem = SendMessage(*hList, LB_GETCURSEL, 0, 0);
			if(nFocusItem == 0)
				SendMessage(GetDlgItem(hFDSetParentWnd, IDC_FIXEDDIALSET) , WM_SETVALUE, 0, (LPARAM)ML(IDS_ON));
			else
				SendMessage(GetDlgItem(hFDSetParentWnd, IDC_FIXEDDIALSET) , WM_SETVALUE, 0, (LPARAM)ML(IDS_OFF));
			
			PostMessage(hWnd, WM_CLOSE, 0, 0);
			
			if(!nFocusItem == bFDStatus)
				break;
			ME_SetPhoneLockStatus(hFDSetParentWnd, CALLBACK_FDSET, LOCK_FD, strPin2 , !nFocusItem );
		}
		break;
	case VK_F10:
		{
			SendMessage(hWnd,WM_CLOSE,0,0);
		}
		break;
		
	default:
		PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
	}
}
/********************************************************************
* Function   CreateFixedDialListWnd  
* Purpose    Fixed nr list
* Params     
* Return     
* Remarks      
**********************************************************************/
static BOOL   CreateFixedDialListWnd(HWND hParentWnd)
{
	WNDCLASS wc;
	HMENU hMenu;
	RECT  rect;

	wc.style         = CS_OWNDC;
	wc.lpfnWndProc   = FixedListWndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = NULL;
	wc.hIcon         = NULL;
	wc.hCursor       = NULL;
	wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = "fixedlistclass";
	
	if (!RegisterClass(&wc))
	{
		return FALSE;
	}
	if ( hFixedDialListWnd )
	{
		ShowWindow ( hFixedDialListWnd, SW_SHOW );
		SetFocus ( hFixedDialListWnd );	
	}
	else
	{ 
	    GetClientRect(hFrameWin, &rect);

		hMenu = CreateMenu();
		AppendMenu(hMenu,MF_ENABLED, IDM_EDIT, IDS_EDIT);
		AppendMenu(hMenu,MF_ENABLED, IDM_DELETE, IDS_DELETE );
		SetMenu(hFrameWin, hMenu);

		hFixedDialListWnd = CreateWindow(
			"fixedlistclass", 
			NULL,
			WS_CHILD | WS_VISIBLE, 
			rect.left,rect.top,rect.right-rect.left,rect.bottom-rect.top,
			hFrameWin, 
			NULL,
			NULL, 
			NULL
			);
		
		if (NULL == hFixedDialListWnd)
		{
			UnregisterClass("fixedlistclass",NULL);
			return FALSE;
		}
		ShowWindow(hFixedDialListWnd, SW_SHOW);
		UpdateWindow(hFixedDialListWnd);
		
	}

	FNItem_Init();
	return TRUE;
}

static LRESULT FixedListWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
	

	LRESULT lResult;
	HDC     hdc;
	static HWND hList;
	
    lResult = (LRESULT)TRUE;

    switch ( wMsgCmd )
    {
    case WM_CREATE :
		FDList_OnCreate(hWnd, &hList);
       
        break;
	case PWM_SHOWWINDOW:
		{
			int iIndex = 0;
			
					
			iIndex = SendMessage(hList,LB_GETCURSEL,0,0);
						
			if(iIndex == 0)
			{
				SendMessage(hFrameWin, PWM_SETBUTTONTEXT, (WPARAM)1, (LPARAM)ML(""));
				SendMessage(hFrameWin, PWM_SETBUTTONTEXT, (WPARAM)0, (LPARAM)ML(IDS_BACK));
				SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");

			}
			else
			{
				SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 1, (LPARAM)ML(IDS_EDIT));
				SendMessage(hFrameWin, PWM_SETBUTTONTEXT, (WPARAM)0, (LPARAM)ML(IDS_BACK));
				SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Options");
			}			
			
			SetWindowText(hFrameWin, ML("Fixed nr list"));
			
			SetFocus(hFixedDialFocusListWnd);
		}
		break;

	case GETFDPBOOKMEM:
		FDList_OnCallBackRPBMEM(hWnd,wParam);
		break;

	case READFDPBOOK:
		FDList_OnCallBackRPB(hWnd, &hList,wParam);
		break;

    case WM_CLOSE:
		SendMessage(hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hWnd,0);
        SendMessage ( hWnd, WM_COMMAND, (WPARAM)IDC_FIXEDLIST_QUIT, 0 ); 
        break;
		
    case WM_DESTROY :
		hFixedDialListWnd = NULL;
		hFixedDialFocusListWnd = NULL;
		bHaveRequestPIN2 = FALSE;
		if(hIconNormal != NULL)
		{
			DeleteObject(hIconNormal);
			hIconNormal  =NULL;
		}
		if(hIconSel != NULL)
		{
			DeleteObject(hIconSel);
			hIconSel = NULL;
		}
		FreeFN_NodeData();
        UnregisterClass("fixedlistclass",NULL);
        break;
		
    case WM_KEYDOWN:
		FDList_OnKeydown(hWnd, &hList, wMsgCmd, wParam, lParam);
        break;

	case CALLBACK_CODE_REMAIN:
		FD_OnCallbackCodeRemain(hWnd);
		break;

	case REQUEST_CODE:
		{
			char strCode[EDITMAXLEN + 1];
			
		
			memset(strCode, 0, EDITMAXLEN + 1);

			if(bGivePin2)
			{
				if((iremains == 0)|| (iremains == PUKNUMS) || (iremains > PINCODE_MAXLIMIT))
				{
					if(	SSPLXVerifyPassword(hFrameWin,NULL, ML(PUK2CODE), strCode,
						MAKEWPARAM(EDITMAXLEN,EDITMAXLEN), ML(IDS_OK),ML(IDS_CANCEL), -1))
					{
						strcpy(strPuk2, strCode);
						nGiveCodeType = REQUEST_PUK2;
						CallNewCodeWindow(hWnd,REQUEST_PIN2, PIN2_CODE, CALLBACK_NEWCODE);		
					}
					else
						bGivePin2 = FALSE;

				}
				else
				{
					if(	SSPLXVerifyPassword(hFrameWin, NULL, ML(PIN2CODE), strCode, 
						MAKEWPARAM(EDITMINLEN,EDITMAXLEN), ML(IDS_OK),ML(IDS_CANCEL), -1))
					{
						nGiveCodeType = REQUEST_PIN2;
						strcpy(strPin2, strCode);
						ME_PasswordValidation(hWnd, CALLBACK_CODE, PIN2, strCode, NULL);
						WaitWindowState(hWnd,TRUE);
						break;
					}
					else
						bGivePin2 = FALSE;
				}
				
			}
			if(bGivePuk2)
			{
				if((iremains != 0))
				{
					if(	SSPLXVerifyPassword(hFrameWin, NULL, ML(PUK2CODE), strCode, 
						MAKEWPARAM(EDITMINLEN,EDITMAXLEN), ML(IDS_OK),ML(IDS_CANCEL), -1))
					{
						strcpy(strPuk2, strCode);
						nGiveCodeType = REQUEST_PUK2;
						CallNewCodeWindow(hWnd,REQUEST_PIN2, PIN2_CODE, CALLBACK_NEWCODE);
						
					}
					else
						bGivePuk2 = FALSE;
				}
			}

		}
		break;
		
	case CALLBACK_NEWCODE:
		{
			if(lParam == NULL)
				break;
			
			
			if(LOWORD(wParam) == REQUEST_PIN2)
			{
				memset(strPin2, 0, EDITMAXLEN+1);
				strcpy(strPin2, (char*)lParam);
				ME_PasswordValidation(hWnd, WM_PIN_CONFIRM, PUK2, strPuk2, strPin2);
			}			
		}
		break;
	case WM_PIN_CONFIRM:
		switch (wParam)
		{
		case ME_RS_SUCCESS:
			{
				iPinStatus.blockCHV2 = FALSE;
				bHaveRequestPIN2 = TRUE;
				PLXTipsWin(hFrameWin, hWnd, NULL,ML(PIN2CODE_CHANGED), ML(TITLECAPTION), Notify_Success,
					ML(IDS_OK), NULL,DEFAULT_TIMEOUT);
						
				WaitWindowState(hWnd,FALSE);
			}
			break;
		case ME_RS_FAILURE:
			{
				ME_GetPassInputLimit(hWnd, CALLBACK_CODE_REMAIN, PUK2);	
			}
			break;
		}	
	//	ME_GetCurWaitingPassword(hWnd, WM_PIN_QUERY);
		break;
		
	case WM_PIN_QUERY:
		{			
			int type;			
			ME_GetResult(&type, sizeof(int));
			if (type == NOPIN)
			{
				
				if(bGivePuk2)
				{
					PLXTipsWin(hFrameWin, hWnd, NULL, ML(PIN2CODE_CHANGED), NULL, Notify_Success,
						ML(IDS_OK), NULL,DEFAULT_TIMEOUT);
					bGivePuk2 = FALSE;
				}
				WaitWindowState(hWnd,FALSE);
				break;
			}
			else if(type == PUK2)
			{
				ME_GetPassInputLimit(hWnd, CALLBACK_CODE_REMAIN, PUK1);
				break;
			}
		}
		break;
	case CALLBACK_CODE:
		FDList_OnCallBackCode(hWnd, wParam, lParam);
		break;

	case WM_PAINT:
		hdc = BeginPaint( hWnd, NULL);
		EndPaint(hWnd, NULL);	
		break;

	case DELETENODE_MSG:
		{
			PHONEBOOK Record ;

			memset(&Record, 0, sizeof(PHONEBOOK));

			if(LOWORD(lParam) == 0)
			{
				bDelete =FALSE;
				break;
			}
			Record.Index = iEditId;
			ME_WritePhonebook( hWnd, CALLBACK_DPB, PBOOK_FD, &Record);
		}
		break;
	case ADDNUMBER_FAIL:
		{
			if(lParam == 1)
			{
				char pName[NAME_MAXLEN+1];
				char pNumber[NUMBER_MAXLEN + 2];
				FN_Item *pFN_Node = NULL;
				PHONEBOOK Record ;
				int  id = 0,len = 0;
				
				GetFN_NodeCount(&id);
				
				pFN_Node = GetFN_NodeFromId(id-1);
				
				if(!pFN_Node)
					break;
				
				pName[0] = 0;
				pNumber[0] = 0;
				memset(&Record,0, sizeof(PHONEBOOK));
				Record.Index = pFN_Node->id;
				len = strlen(pFN_Node->Number);
				strncpy(Record.PhoneNum, pFN_Node->Number,(int)len*2/3);
				Record.PhoneNum[(int)len*2/3] = '\0';
				ChangeFN_Node(id-1,Record.PhoneNum);
				strcpy(Record.Text, pFN_Node->Name);
				iNewId = pFN_Node->id;
				printf("\r\n wirte new FD id= %d, name = %s, number = %s", pFN_Node->id,
					pFN_Node->Name, pFN_Node->Number);
				ME_WritePhonebook(hWnd, CALLBACK_DPB, PBOOK_FD, &Record);
				WaitWindowState(hWnd,TRUE);
				
			}
			else
			{
				PostMessage(hWnd, WM_CLOSE, 0, 0);
			}
		}
		break;

	case CALLBACK_DPB:
		{
			WaitWindowState(hWnd,FALSE);
			switch (wParam)
			{
			case ME_RS_SUCCESS:
				if(bEdit)
				{
					bEdit = FALSE;
					SendMessage(hWnd, EDITNODE_MSG, 1, 
						(LPARAM)iEditId);
				}
				if(bDelete)
				{
					bDelete = FALSE;
					SendMessage(hWnd, DELETENODE, 1, (LPARAM)iEditId);
					PLXTipsWin(hFrameWin, hWnd, NULL, ML("Deleted"), NULL, Notify_Success, 
						ML(IDS_OK), NULL, DEFAULT_TIMEOUT);
				}
				bEditEnable = FALSE;
				break;	
			case ME_RS_FAILURE:
				{
					if(lParam == 0x00000011)
					{
						if(bEdit)
						{
							bEdit = FALSE;
							SendMessage(hWnd, EDITNODE_MSG, 1, 
								(LPARAM)iEditId);
						}
						if(bDelete)
						{
							bDelete = FALSE;
							SendMessage(hWnd, DELETENODE, 1, (LPARAM)iNewId);
							PLXTipsWin(hFrameWin, hWnd, NULL, ML("Deleted"), NULL, Notify_Success, 
								ML(IDS_OK), NULL, DEFAULT_TIMEOUT);
						}
					}
					else
					{
						if(bEdit)
						{
							if(lParam == 26 || lParam == 260)
							{
								PLXTipsWin(hFrameWin, hWnd, ADDNUMBER_FAIL, ML("Number is too long.\r\nTruncate?"), NULL,
									Notify_Request, ML("Yes"), ML("No"), NULL);
								break;		
							}
							printf("\r\nEdit fail id = %d, error code = %d", iEditId, lParam);
						}
						if(bDelete)
							printf("\r\nDelete fail id = %d, error code = %d", iEditId, lParam);
						bEdit = FALSE;
						bDelete = FALSE;
						bEditEnable = FALSE;
					}
				}
				break;
			}
		}
		break;
		
	case EDITNODE_MSG:
		FDList_OnEditNode(hWnd, &hList, wParam, lParam);
		break;
		
	case ADDNODE_MSG:
		FDList_OnAddNode(hWnd, &hList, wParam, lParam);
		break;
		
	case DELETENODE:
		FDList_OnDelNode(hWnd, &hList, wParam, lParam);
		break;
		
    case WM_COMMAND:
		FDList_OnCommand(hWnd, &hList,wMsgCmd, wParam, lParam);
		break;

	default :
		lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
		break;
	}
	return lResult;
}
/********************************************************************
* Function   FDList_OnCreate  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
static void FDList_OnCreate(HWND hWnd, HWND *hList)
{
	int i = 0, num = 0;
    FN_Item *pFN_Node = NULL;
	RECT rect;
	HDC   hdc;
	
	SendMessage(hFrameWin, PWM_SETBUTTONTEXT, (WPARAM)1, (LPARAM)ML("")); 

	SendMessage(hFrameWin, PWM_SETBUTTONTEXT, (WPARAM)0, (LPARAM)ML(IDS_BACK)); 
	
	SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
	
	SetWindowText(hFrameWin, ML("Fixed nr list"));
	
	GetClientRect(hWnd,&rect);
	
	*hList = CreateWindow("LISTBOX","", 
		WS_VISIBLE | WS_CHILD | WS_VSCROLL | LBS_MULTILINE,
		rect.left, rect.top , rect.right - rect.left, rect.bottom - rect.top,
        hWnd, (HMENU)IDC_FIXEDDIAL_LIST, NULL, NULL );
	
	if(*hList == NULL)
		return;
	
	SendMessage(*hList,LB_ADDSTRING, -1,(LPARAM)ML("New number"));
  
	
	SendMessage(*hList,LB_SETCURSEL,0,0);
	
	hFixedDialFocusListWnd = *hList;
	SetFocus(hFixedDialFocusListWnd);
	
	hdc         = GetDC(hWnd);
	hIconNormal = LoadImageEx(hdc,NULL,ICONORMAL,IMAGE_BITMAP,17,16,LR_LOADFROMFILE);
	hIconSel = LoadImageEx(hdc,NULL,ICONSEL,IMAGE_BITMAP,17,16,LR_LOADFROMFILE);
	ReleaseDC(hWnd,hdc);
	Load_Icon_SetupList(*hList,hIconNormal,hIconSel, 1,0);
	
	if( ME_GetPhonebookMemStatus( hWnd, GETFDPBOOKMEM, PBOOK_FD ) >= 0 )
		WaitWindowStateEx(hFrameWin, TRUE, ML("Reading SIM card. Please wait..."),
		NULL,ML("Ok"),NULL);

}
/********************************************************************
* Function   FDList_OnKeydown  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
static void FDList_OnKeydown(HWND hWnd, HWND *hList,UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
	switch(LOWORD(wParam))
	{
	case VK_F5:
		{
			int nFocusItem;
			char strCode[EDITMAXLEN + 1];

			memset(strCode, 0, EDITMAXLEN + 1);

			nFocusItem = SendMessage(*hList, LB_GETCURSEL, 0, 0);

	
			if(nFocusItem ==0)
			{
				if(bHaveRequestPIN2)
				{
					CreateNewNumberWnd(hWnd, -1);
				}
				else
				{
					bGivePin2 = TRUE;
					nGiveCodeType = REQUEST_PIN2;
					bNotNotify = TRUE;
					
					if (-1 != ME_GetPassInputLimit(hWnd,CALLBACK_CODE_REMAIN,PIN2))
						WaitWindowState(hWnd,TRUE);

				}
				
				break;
			}
			else
				SendMessage(hFrameWin, WM_KEYDOWN, VK_F5, 0);
			
		}
		break;
	case VK_RETURN:
		{
			int nFocus = 0;
			
			nFocus = SendMessage(hWnd, LB_GETCURSEL, 0, 0);
			
			if(nFocus == 0)
				break;
			
			//if(!bGivePin2 && !bGivePuk2)
			{
				SendMessage(hWnd, WM_COMMAND, IDM_EDIT, 0);
				break;
			}

		}
		break;
	case VK_F10:
		{
//			if(bGivePin2)
//			{
//				bGivePin2  =FALSE;
//				bEdit  = FALSE;
//				bDelete = FALSE;
//				break;
//			}
			SendMessage(hWnd,WM_CLOSE,0,0);
		}
		break;
		
	default:
		PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
	}

}
/********************************************************************
* Function   FDList_OnCallBackCodeRemain
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
static void FDList_OnCallBackCodeRemain(HWND hWnd)
{
     
	iremains = 0;
	WaitWindowState(hWnd,FALSE);
	
	ME_GetResult(&iremains,sizeof(int));
	if ((iremains == 0) || (iremains == PUKNUMS))
	{
		switch(nGiveCodeType)
		{
		case REQUEST_PIN2:
			{
				nGiveCodeType = REQUEST_PUK2;
				bGivePin2=FALSE;
				bGivePuk2 = TRUE;

				PLXTipsWin(hFrameWin, hWnd, REQUEST_CODE, ML(PIN2_BLOCKED),ML(PIN2_CODE), 
					Notify_Failure, ML(IDS_OK), NULL, 0);
	
			}
			break;
			
		case REQUEST_PUK2:
			{
				PLXTipsWin(hFrameWin, hWnd, NULL, ML(PUK2_BLOCKED),ML(PUK2_CODE), Notify_Alert, 
					ML(IDS_OK), NULL, 0);
				bGivePuk2 = FALSE;
			}
			break;	
		}	
	}
	else
	{
		switch(nGiveCodeType)
		{
		case REQUEST_PIN2:

			nGiveCodeType = REQUEST_PIN2;
			bGivePin2=TRUE;
			PLXTipsWin(hFrameWin, hWnd, REQUEST_CODE, ML(WRONG_PIN2), ML(PIN2_CODE), 
				Notify_Failure, ML(IDS_OK), NULL, DEFAULT_TIMEOUT);
			break;
			
		case REQUEST_PUK2:
			PLXTipsWin(hFrameWin, hWnd, REQUEST_CODE, ML(WRONG_PUK2CODE), ML(PUK2_CODE),
				Notify_Failure, ML(IDS_OK), NULL, 0);
		
			break;
			
		}
	}
	
}
/********************************************************************
* Function   FDList_OnCommand
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
static void FDList_OnCommand(HWND hWnd, HWND *hList, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
	switch( LOWORD( wParam ))
	{
		
	case IDC_FIXEDDIAL_LIST:
		if(HIWORD(wParam) == LBN_SELCHANGE)
		{
			int iIndex = 0;
			
			iIndex = SendMessage(*hList,LB_GETCURSEL,0,0);
			Load_Icon_SetupList(*hList,hIconNormal,hIconSel, iTotalId, 0);
			
			if(iIndex == 0)
			{
				SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 1, (LPARAM)ML(""));
				SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
			}
			else
			{
				SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 1, (LPARAM)ML("Edit"));
				SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Options");
			}
		}
		else
			PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
		
		break;
	case IDM_EDIT:
		{
			FN_Item *pFN_Node = NULL;
			int nFocus = 0, id = 0;

			bEdit = TRUE;
			nGiveCodeType = REQUEST_PIN2;

			nFocus = SendMessage(*hList,LB_GETCURSEL,0,0);
			id = SendMessage(*hList, LB_GETITEMDATA, nFocus, 0);
			if(id > 0)
				pFN_Node = GetFN_NodeFromId(id);
			else
				break;;
			
			if(pFN_Node == NULL)
				break;
			iEditId = pFN_Node->id ;

			bNotNotify = TRUE;
			if(bHaveRequestPIN2)
			{
				CreateNewNumberWnd(hWnd, iEditId);
			}
			else
			{
				bNotNotify = TRUE;
				if (-1 != ME_GetPassInputLimit(hWnd,CALLBACK_CODE_REMAIN,PIN2))
					WaitWindowState(hWnd,TRUE);
			}
		
		}
		
		break;
		
	case IDM_DELETE:
		{
			FN_Item *pFN_Node = NULL;
			int nFocus = 0, id= 0;
			
			bDelete = TRUE;
			nGiveCodeType = REQUEST_PIN2;
			
			nFocus = SendMessage(*hList,LB_GETCURSEL,0,0);
			id = SendMessage(*hList,LB_GETITEMDATA, nFocus, 0);
			if(id > 0)
				pFN_Node = GetFN_NodeFromId(id);
			else
				break;
			
			if(pFN_Node == NULL)
				break;
			
			iEditId = pFN_Node->id;

			bNotNotify  =TRUE;

			if(bHaveRequestPIN2)
			{
				FN_Item *pFN_Node = NULL;
				char strPromt[NUMBER_MAXLEN + 9];
				
				strPromt[0] = 0;
				
				pFN_Node = GetFN_NodeFromId(iEditId);
				
				if(pFN_Node->Name[0] != 0)
					sprintf(strPromt, "%s:\r\nDelete?", pFN_Node->Name);
				else if(pFN_Node->Number[0] != 0)
					sprintf(strPromt, "%s:\r\nDelete?", pFN_Node->Number);
				
				PLXConfirmWinEx(hFrameWin, hWnd, strPromt, Notify_Request, NULL,ML(IDS_YES),
					ML(IDS_NO), DELETENODE_MSG);
			
			}
			else
			{
				bNotNotify = TRUE;
				if (-1 != ME_GetPassInputLimit(hWnd,CALLBACK_CODE_REMAIN,PIN2))
					WaitWindowState(hWnd,TRUE);
			}

		}
		
		break;
		
	case IDC_FIXEDLIST_QUIT:
		DestroyWindow(hWnd);
		break;
	}
}
/********************************************************************
* Function   FDList_OnDelNode
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
static void FDList_OnDelNode(HWND hWnd, HWND *hList, WPARAM wParam, LPARAM lParam)
{
	FN_Item *pFN_Node = NULL;
	int i = 0;
	
	if(wParam == NULL)
		return;
	pFN_Node = GetFN_NodeFromId((int)lParam);
	DelteFN_Node(pFN_Node->id);
	iTotalId--;
	
	SendMessage(*hList, LB_RESETCONTENT, 0 , 0);
	SendMessage(*hList,LB_ADDSTRING, -1,(LPARAM)ML("New number"));
    i++;
	
    pFN_Node = pFN_NodeHead;
	while(i<= iTotalId && pFN_Node)
	{
		SendMessage(*hList,LB_ADDSTRING,NULL,(LPARAM)pFN_Node->Name);
		SendMessage(*hList, LB_SETAUXTEXT, MAKEWPARAM(i, -1), (LPARAM)(pFN_Node->Number));
		SendMessage(*hList, LB_SETITEMDATA, i, pFN_Node->id);
		pFN_Node = pFN_Node->pNext;
		i++;
	}
	SendMessage(*hList, LB_SETCURSEL, 0, 0);
	
}
/********************************************************************
* Function   FDList_OnAddNode
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
static void FDList_OnAddNode(HWND hWnd, HWND *hList, WPARAM wParam, LPARAM lParam)
{
	FN_Item *pFN_Node = NULL;
	int nCount;
	
	if(wParam == NULL)
		return;
	pFN_Node = GetFN_NodeFromId((int)lParam);
	iTotalId ++;
	nCount = SendMessage(*hList, LB_GETCOUNT, 0, 0);
	
	if(pFN_Node != NULL)
	{
		SendMessage(*hList,LB_ADDSTRING,NULL,(LPARAM)ML(pFN_Node->Name));
		SendMessage(*hList, LB_SETAUXTEXT, MAKEWPARAM(nCount, -1), (LPARAM)(pFN_Node->Number));
		SendMessage(*hList, LB_SETITEMDATA, nCount, (LPARAM)pFN_Node->id);
		SendMessage(*hList, LB_SETCURSEL,  nCount,0);
		
	}
}
/********************************************************************
* Function   FDList_OnEditNode
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
static void FDList_OnEditNode(HWND hWnd, HWND *hList, WPARAM wParam, LPARAM lParam)
{
	
	FN_Item *pFN_Node = NULL;
	int nFocus = 0;

	if(wParam == NULL)
		return;

	pFN_Node = GetFN_NodeFromId((int)(lParam));
	nFocus = SendMessage(*hList, LB_GETCURSEL, 0, 0);

	if(pFN_Node != NULL)
	{
		SendMessage(*hList,LB_SETTEXT, nFocus,(LPARAM)pFN_Node->Name);
		SendMessage(*hList, LB_SETAUXTEXT, MAKEWPARAM(nFocus, -1), (LPARAM)(pFN_Node->Number));
	}	
}
/********************************************************************
* Function   FDList_OnCallBackRPBMEM
* Purpose    read memory info of FD
* Params     
* Return     
* Remarks      
**********************************************************************/
static void FDList_OnCallBackRPBMEM(HWND hWnd, WPARAM wParam)
{	
	PBMEM_INFO MemInfo;
	
	memset( &MemInfo, 0, sizeof( PBMEM_INFO ));
	MemInfo.Type = PBOOK_FD;
	
	if(( wParam != ME_RS_SUCCESS )||
		( ME_GetResult( &MemInfo, sizeof( MemInfo )) == -1 ))
	{
		WaitWindowStateEx(hFrameWin, FALSE, ML("Reading SIM card. Please wait..."),
			NULL,ML("Ok"),NULL);
	//	WaitWindowState(hWnd, FALSE);
		return;
	}
	
	iTotalId = MemInfo.Used;
	
	if(iTotalId == 0)
	{
		WaitWindowStateEx(hFrameWin, FALSE, ML("Reading SIM card. Please wait..."),
			NULL,ML("Ok"),NULL);
	//	WaitWindowState(hWnd, FALSE);
		return;
	}
	
	if(ME_ReadPhonebook( hWnd, READFDPBOOK, PBOOK_FD, 1, MAX_FDITEM ) < 0)
	{
		WaitWindowStateEx(hFrameWin, FALSE, ML("Reading SIM card. Please wait..."),
			NULL,ML("Ok"),NULL);
	//	WaitWindowState(hWnd, FALSE);
		return;
	}	
}
/********************************************************************
* Function   FDList_OnCallBackRPB
* Purpose    read pb info(FD) from sim card
* Params     
* Return     
* Remarks      
**********************************************************************/
static void FDList_OnCallBackRPB(HWND hWnd, HWND *hList, WPARAM wParam)
{
	PPHONEBOOK pPhoneBookHead = NULL;
	
	WaitWindowStateEx(hFrameWin, FALSE, ML("Reading SIM card. Please wait..."),
		NULL,ML("Ok"),NULL);
//	WaitWindowState(hWnd, FALSE);
	
	pPhoneBookHead = malloc(sizeof(PHONEBOOK) * iTotalId);
	
	if(pPhoneBookHead == NULL)
		return;
	
	memset(pPhoneBookHead, 0, sizeof(PHONEBOOK) * iTotalId);
	
	if( wParam == ME_RS_SUCCESS )
	{        
		ME_GetResult(pPhoneBookHead , sizeof(PHONEBOOK) * iTotalId);
	}
	
	InitFDData(pPhoneBookHead, iTotalId, hList);

	free(pPhoneBookHead);
	pPhoneBookHead = NULL;
}
/********************************************************************
* Function   FDList_OnCallBackCode  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
static void FDList_OnCallBackCode(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	
	int nType;
	
	WaitWindowState(hWnd,FALSE);
	switch (wParam)
	{
	case ME_RS_SUCCESS://on or off pin
		if(bGivePin2)
		{
			bGivePin2 =FALSE;
			bHaveRequestPIN2 = TRUE;
			if(bEdit)//enter edit 
			{
				CreateNewNumberWnd(hWnd, iEditId);
				break;
			}
			if(bDelete)//enter delete
			{
				FN_Item *pFN_Node = NULL;
				char strPromt[NUMBER_MAXLEN + 9];
				
				strPromt[0] = 0;
				
				pFN_Node = GetFN_NodeFromId(iEditId);
			
				if(pFN_Node->Name[0] != 0)
					sprintf(strPromt, "%s:\r\nDelete?", pFN_Node->Name);
				else if(pFN_Node->Number[0] != 0)
					sprintf(strPromt, "%s:\r\nDelete?", pFN_Node->Number);
				
				PLXConfirmWinEx(hFrameWin, hWnd, strPromt, Notify_Request, NULL,ML(IDS_YES),
					ML(IDS_NO), DELETENODE_MSG);
				break;
			}
			CreateNewNumberWnd(hWnd, -1);
		}
		break;	
	case ME_RS_FAILURE:
		if(lParam == 3)
		{
			bGivePin2 =FALSE;
			bHaveRequestPIN2 = TRUE;
			if(bEdit)
			{
				CreateNewNumberWnd(hWnd, iEditId);
				break;
			}
			if(bDelete)
			{
				FN_Item *pFN_Node = NULL;
				char strPromt[NUMBER_MAXLEN + 9];
				
				strPromt[0] = 0;
				
				pFN_Node = GetFN_NodeFromId(iEditId);
				
				if(pFN_Node->Name[0] != 0)
					sprintf(strPromt, "%s:\r\nDelete?", pFN_Node->Name);
				else if(pFN_Node->Number[0] != 0)
					sprintf(strPromt, "%s:\r\nDelete?", pFN_Node->Number);
				
				PLXConfirmWinEx(hFrameWin, hWnd, strPromt, Notify_Request, NULL,ML(IDS_YES),
					ML(IDS_NO), DELETENODE_MSG);
				break;
			}
			
			CreateNewNumberWnd(hWnd, -1);
		}
		else
		{
			switch(nGiveCodeType)
			{
			case REQUEST_PIN2:
				nType = PIN2;
				break;
				
			case REQUEST_PUK2:
				nType = PUK2;
				break;
			}
			if (-1 != ME_GetPassInputLimit(hWnd,CALLBACK_CODE_REMAIN,nType))
				WaitWindowState(hWnd,TRUE);
		}
		break;
	default:
		{
			switch(nGiveCodeType)
			{
			case REQUEST_PIN2:
				nType = PIN2;
				break;
				
			case REQUEST_PUK2:
				nType = PUK2;
				break;
			}
			
			if (-1 != ME_GetPassInputLimit(hWnd,CALLBACK_CODE_REMAIN,nType))
				WaitWindowState(hWnd,TRUE);
		}
	
	}
	
}
/********************************************************************
* Function   CreateNewNumberWnd  
* Purpose    add new number 
* Params     id = -1 new number; id != -1 Edit number(id is numberid)
* Return     
* Remarks      
**********************************************************************/
static char strEditName[NAME_MAXLEN +1];
static char strEditNumber[NUMBER_MAXLEN + 1];
static BOOL CreateNewNumberWnd(HWND hParentWnd, int id)
{
	WNDCLASS wc;
	HWND     hWnd;
	RECT     rect;
	
	FN_Item *pFN_Node = NULL;
	memset(strEditNumber, 0, NUMBER_MAXLEN + 1);
	memset(strEditName, 0, NAME_MAXLEN + 1);
	
	memset(strTitle, 0, NAME_MAXLEN + 1);
	hNewNameNumberParentWnd = hParentWnd;

	wc.style         = CS_OWNDC;
	wc.lpfnWndProc   = NewNumberWndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = NULL;
	wc.hIcon         = NULL;
	wc.hCursor       = NULL;
	wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = "NewNumberWndClass";
	
	if (!RegisterClass(&wc))
	{
		return FALSE;
	}
	iEditId = id;
	if(id == -1)
		strcpy(strTitle, "Fixed nr list");
	else
	{
		pFN_Node = GetFN_NodeFromId(id);
		if(pFN_Node!= NULL)
		{
			strcpy(strTitle, pFN_Node->Name);
			strcpy(strEditName, pFN_Node->Name);
			strcpy(strEditNumber, pFN_Node->Number);
		}
		else
			strcpy(strTitle, "Fixed nr list");
	}

	GetClientRect(hFrameWin, &rect);

	hWnd = CreateWindow(
		"NewNumberWndClass", 
		NULL,
		WS_VISIBLE | WS_CHILD, 
		rect.left,rect.top,rect.right-rect.left,rect.bottom-rect.top,
		hFrameWin, 
		NULL,
		NULL, 
		NULL);
	
	if (NULL == hWnd)
	{
		UnregisterClass("NewNumberWndClass",NULL);
		return FALSE;
	}
	
	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);
	
	SendMessage(hFrameWin, PWM_SETBUTTONTEXT, (WPARAM)1, (LPARAM)ML(IDS_SAVE)); 
	
	SendMessage(hFrameWin, PWM_SETBUTTONTEXT, (WPARAM)0, (LPARAM)ML(IDS_CANCEL)); 
	
	SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"");

	SetWindowText(hFrameWin, ML(strTitle));

	
	return TRUE;
}
static LRESULT NewNumberWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
	LRESULT lResult;
	HDC hdc;

    lResult = (LRESULT)TRUE;
    switch ( wMsgCmd )
    {
    case WM_CREATE :
        CreateNewNumberControl(hWnd);
        break;

	case PWM_SHOWWINDOW:
		{
			SendMessage(hFrameWin, PWM_SETBUTTONTEXT, (WPARAM)1, (LPARAM)ML(IDS_SAVE));	
			SendMessage(hFrameWin, PWM_SETBUTTONTEXT, (WPARAM)0, (LPARAM)ML(IDS_CANCEL));
			SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
			
			SetWindowText(hFrameWin, ML(strTitle));
			
			SetFocus(hNewNameNumberFocus);
			SendMessage(hNewNameNumberFocus, EM_SETSEL, -1, -1);
			
		}
		break;
  
    case WM_CLOSE:
		if(!bEditEnable)
			bEdit = FALSE;
		SendMessage(hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hWnd,0);
        DestroyWindow(hWnd);
        break;
		
   	case WM_PAINT:
		hdc = BeginPaint( hWnd, NULL);
        EndPaint(hWnd, NULL);
		break;

    case WM_DESTROY :
		hNewNameNumberFocus = NULL;
        UnregisterClass("NewNumberWndClass",NULL);
        break;
		
    case WM_KEYDOWN:
        switch(LOWORD(wParam))
        {
        case VK_F10:
			{
				HWND hCurWin = NULL;
				int nPos = 0, nEnd = 0;

				hCurWin = GetFocus();
				SendMessage(hCurWin, EM_GETSEL, (WPARAM)&nPos, (LPARAM)&nEnd);
				bEditEnable = FALSE;
				if (!nEnd)
					PostMessage(hWnd, WM_CLOSE, 0,0);
				else
					SendMessage(hCurWin, WM_KEYDOWN, VK_BACK, 0);
			}
            break;
		case VK_RETURN:
			SendMessage(hWnd, WM_COMMAND, IDC_NEWNUMBER_SAVE, 0);
		case VK_F5:
			break;
	     default:
            lResult = PDADefWindowProc(hFrameWin, wMsgCmd, wParam, lParam);;
        }
        break;
	case ADDNUMBER_FAIL:
		{
			if(lParam == 1)
			{
				char pName[NAME_MAXLEN+1];
				char pNumber[NUMBER_MAXLEN + 2];
				FN_Item *pFN_Node = NULL;
				PHONEBOOK Record ;
				int  id = 0,len = 0 ;

				GetFN_NodeCount(&id);

				pFN_Node = GetFN_NodeFromId(id-1);
				
				if(!pFN_Node)
					break;
				
				pName[0] = 0;
				pNumber[0] = 0;
				memset(&Record,0, sizeof(PHONEBOOK));
				Record.Index = pFN_Node->id;
				len = strlen(pFN_Node->Number);
				strncpy(Record.PhoneNum, pFN_Node->Number,(int)len*2/3);
				Record.PhoneNum[(int)len*2/3] = '\0';
				ChangeFN_Node(id-1,Record.PhoneNum);
				strcpy(Record.Text, pFN_Node->Name);

				iNewId = pFN_Node->id;
				printf("\r\n wirte new FD id= %d, name = %s, number = %s", pFN_Node->id,
					pFN_Node->Name, pFN_Node->Number);
				ME_WritePhonebook(hWnd, CALLBACK_WPB, PBOOK_FD, &Record);
				WaitWindowState(hWnd,TRUE);
			}
			else
			{
				PostMessage(hWnd, WM_CLOSE, 0, 0);
			}
		}
		break;
	case CALLBACK_WPB://add new item 
		FDNewItem_OnCallBackWPB(hWnd, wParam, lParam);
		break;

	case NOMEMORY:
		SendMessage(hWnd, WM_CLOSE, 0, 0);
		break;

    case WM_COMMAND:
		FDNewItem_OnCommand(hWnd,wMsgCmd, wParam, lParam);
		break;
		
	default :
		lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
		break;
	}
	return lResult;
}
/********************************************************************
* Function   CreateNewNumberControl  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
static BOOL CreateNewNumberControl(HWND hWnd)
{
	
	HWND hNewName =NULL, hNewNumber= NULL;
	IMEEDIT ie_Name, ie_Number;

	int num = 0;

	memset(&ie_Name, 0, sizeof(IMEEDIT));
    
    ie_Name.hwndNotify   = hWnd;
	ie_Name.dwAttrib |= 0;

    ie_Name.dwAscTextMax = 0;
    ie_Name.dwUniTextMax = 0;
    
    ie_Name.pszImeName = NULL;
    ie_Name.pszTitle = NULL;

	
	hNewName = CreateWindow(
		"IMEEDIT",
		ML(""),
		WS_VISIBLE|WS_CHILD|WS_TABSTOP|ES_TITLE|ES_AUTOHSCROLL,
		0,0, PLX_WIN_WIDTH -2, 50,                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        
		hWnd,
		(HMENU)IDC_NAME,
		NULL,
		(PVOID)&ie_Name);
	if (NULL == hNewName)
		return FALSE;

	SendMessage(hNewName, EM_SETTITLE, 0, (LPARAM)ML("Name:"));
	SendMessage(hNewName, EM_LIMITTEXT, NAME_MAXLEN, 0);
	SetWindowText(hNewName, strEditName);
	SendMessage(hNewName, EM_SETSEL, -1, -1);

	memset(&ie_Number, 0, sizeof(IMEEDIT));
    
    ie_Number.hwndNotify   = hWnd;
	ie_Number.dwAttrib |= 0;
	
    ie_Number.dwAscTextMax = 0;
    ie_Number.dwUniTextMax = 0;

	ie_Number.pszCharSet	= "0123456789";
    
    ie_Number.pszTitle = NULL;
	ie_Number.pszImeName = "Phone";
	
	hNewNumber = CreateWindow(
		"IMEEDIT",
		ML(""),
		WS_VISIBLE|WS_CHILD|WS_TABSTOP|ES_TITLE|ES_AUTOHSCROLL,
		0,51, PLX_WIN_WIDTH -2, 50,                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        
		hWnd,
		(HMENU)IDC_NUMBER,
		NULL,
		(PVOID)&ie_Number);
	if (NULL == hNewNumber)
		return FALSE;
	
	SendMessage(hNewNumber, EM_SETTITLE, 0, (LPARAM)ML("Number:"));
	SendMessage(hNewNumber, EM_LIMITTEXT, NUMBER_MAXLEN + 1, 0);
	SetWindowText(hNewNumber, strEditNumber);
	SendMessage(hNewNumber, EM_SETSEL, -1, -1);

	hNewNameNumberFocus = hNewName ;
	SetFocus(hNewNameNumberFocus);
	SendMessage(hNewNameNumberFocus, EM_SETSEL, -1, -1);
		
	return TRUE;
}
/********************************************************************
* Function   FDNewItem_OnCommand  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
static void FDNewItem_OnCommand(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
	   switch( LOWORD( wParam ))
	   {	
	   case IDC_NEWNUMBER_SAVE:
		   {
			   char pName[NAME_MAXLEN+1];
			   char pNumber[NUMBER_MAXLEN + 2];
			   FN_Item *pFN_Node = NULL;
			   PHONEBOOK Record ;
			   
			   pName[0] = 0;
			   pNumber[0] = 0;
			   
			   GetWindowText(GetDlgItem(hWnd, IDC_NAME), pName,NAME_MAXLEN + 1);
			   
			   GetWindowText(GetDlgItem(hWnd, IDC_NUMBER), pNumber, NUMBER_MAXLEN + 2);
			   
			   if((pName[0] == 0) ||(pNumber[0] == 0))
			   {
				   PLXTipsWin(hFrameWin, hWnd, NULL, ML(NOTDEFINDNAME),NULL, Notify_Alert, ML(IDS_OK), NULL, DEFAULT_TIMEOUT);
				   break;
			   }
			   
			   if(iEditId == -1)//new fixed dialnumber
			   {
				   if(iTotalId == MAX_FDITEM)
				   {
					   PLXTipsWin(hFrameWin, hWnd, NOMEMORY, ML("Not enough memory \r\non SIM card"),NULL,Notify_Failure,
					   ML("Ok"), NULL,DEFAULT_TIMEOUT);
					   break;
				   }
				   pFN_Node = malloc(sizeof(FN_Item));
				   if(pFN_Node == NULL)
					   break;
				   memset(pFN_Node, 0, sizeof(FN_Item));
				   strcpy(pFN_Node->Name, pName);
				   strcpy(pFN_Node->Number, pNumber);
				   GetFN_NodeCount(&pFN_Node->id);
				   pFN_Node->pNext = NULL;
				   
				   if(!pFN_NodeHead)
				   {
					   pFN_NodeHead = pFN_Node;
				   }
				   else
				   {
					   FN_Item *p = NULL;
					   
					   p = pFN_NodeHead;
					   while(p->pNext)
					   {
						   p = p->pNext;
					   }
					   p->pNext = pFN_Node;
				   }
				   memset(&Record,0, sizeof(PHONEBOOK));
				   Record.Index = pFN_Node->id;
				   strcpy(Record.PhoneNum, pFN_Node->Number);
				   strcpy(Record.Text, pFN_Node->Name);
				   iNewId = pFN_Node->id;
				   printf("\r\n wirte new FD id= %d, name = %s, number = %s", pFN_Node->id,
					   pFN_Node->Name, pFN_Node->Number);
				   ME_WritePhonebook(hWnd, CALLBACK_WPB, PBOOK_FD, &Record);
				   WaitWindowState(hWnd,TRUE);
				   
			   }
			   else//edit fixed dialnumber
			   {
				   pFN_Node = GetFN_NodeFromId(iEditId);
				   if(pFN_Node == NULL)
					   break;
				   strcpy(pFN_Node->Name, pName);
				   strcpy(pFN_Node->Number, pNumber);

				   memset(&Record, 0, sizeof(PHONEBOOK));
				   Record.Index = iEditId;
				   strcpy(Record.PhoneNum, pFN_Node->Number);
				   strcpy(Record.Text, pFN_Node->Name);
				  
				   bEditEnable = TRUE;
				   PostMessage(hWnd, WM_CLOSE, 0, 0);
				   ME_WritePhonebook( hNewNameNumberParentWnd, CALLBACK_DPB, PBOOK_FD, &Record);
				  
				   WaitWindowState(hWnd,TRUE);
				   
			   }
			   
		   }
		   break;

	   }
}
/********************************************************************
* Function   FDNewItem_OnCallBackWPB  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
static void FDNewItem_OnCallBackWPB(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	WaitWindowState(hWnd,FALSE);
	switch (wParam)
	{
	case ME_RS_SUCCESS:
		{
			SendMessage(hNewNameNumberParentWnd, ADDNODE_MSG, 1, (LPARAM)iNewId);
			PostMessage(hWnd, WM_CLOSE, 0, 0);
			//DestroyWindow(hWnd);	
		}
		break;	
	case ME_RS_FAILURE:
		{
			if(lParam == 3)
			{
				SendMessage(hNewNameNumberParentWnd, ADDNODE_MSG, 1, (LPARAM)iNewId);
				PostMessage(hWnd, WM_CLOSE, 0, 0);
				//DestroyWindow(hWnd);
			}
			else
			{
				if(lParam == 26|| lParam == 260)
				{
					PLXTipsWin(hFrameWin, hWnd, ADDNUMBER_FAIL, ML("Number is too long.\r\nTruncate?"), NULL,
					Notify_Request, ML("Yes"), ML("No"), NULL);
					break;
				}
				printf("\r\nFDsetting New fail!------------->error code = %d\r\n", lParam);
				PostMessage(hWnd, WM_CLOSE, 0, 0);
			}
		}
		break;
	default:
		{
			printf("\r\nFDsetting New fail!------------->wparam = %d,error code = %d\r\n", wParam,lParam);
			PostMessage(hWnd, WM_CLOSE, 0, 0);
		}

		break;
	}
}
/********************************************************************
* Function   FNItem_Init  
* Purpose    init fixed dial numbers info
* Params     
* Return     
* Remarks      
**********************************************************************/
static void  FNItem_Init(void)
{
	
	if(pFN_NodeHead)
		pFN_NodeHead = NULL;
	
}
/********************************************************************
* Function   GetFN_NodeCount  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
static void   GetFN_NodeCount(int* number)
{
	FN_Item *p =NULL;
	int i = 1;
	

	if(pFN_NodeHead )
	{
		p = pFN_NodeHead;
		while (p)
		{
			if(p->id == i)
			{
				i++;
				*number = i;
				return;
			}
			p = p->pNext;
		}
	}
    *number = i;
}
/********************************************************************
* Function   GetFN_NodeFromId  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
static FN_Item *GetFN_NodeFromId(int nFocus)
{
	FN_Item *p =NULL;

	if(pFN_NodeHead == NULL)
	{
		return NULL;
	}
	else
	{
		p = pFN_NodeHead;
		while(p)
		{
			if(p->id == nFocus)
			{
				return p;
			}
			p = p->pNext;
		}
	}
	return NULL;
}
/********************************************************************
* Function   DelteFN_Node  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
static BOOL DelteFN_Node(int nFocus)
{
	FN_Item *p =NULL, *pFN_Node = NULL;

	if(pFN_NodeHead == NULL)
	{
		return NULL;
	}
	else
	{
		p = pFN_NodeHead;
		while(p)
		{
			if(p->id == nFocus)
			{
				if(pFN_Node != NULL)
				{
					pFN_Node->pNext = p->pNext;
				}
				else
					pFN_NodeHead = p->pNext;

				free(p);
				return TRUE;
			}
			pFN_Node = p;
			p = p->pNext;
		}
	}
	return FALSE;
}
/********************************************************************
* Function   ChangeFN_Node  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
static BOOL ChangeFN_Node(int nFocus, char *strNumber)
{
	FN_Item *p =NULL, *pFN_Node = NULL;

	if(pFN_NodeHead == NULL)
	{
		return NULL;
	}
	else
	{
		p = pFN_NodeHead;
		while(p)
		{
			if(p->id == nFocus)
			{
				if(pFN_Node != NULL)
				{
					strcpy(pFN_Node->Number, strNumber);
				}
				else
				{
					pFN_Node = pFN_NodeHead;
					strcpy(pFN_Node->Number, strNumber);
				}

				return TRUE;
			}
			pFN_Node = p;
			p = p->pNext;
		}
	}
	return FALSE;
}
/********************************************************************
* Function   FreeFN_NodeData  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
static void FreeFN_NodeData(void)
{
   FN_Item *p = NULL, *pFN_Node = NULL;

	if(pFN_NodeHead == NULL)
	{
		return ;
	}
	else
	{
		p = pFN_NodeHead;
		while(p)
		{
			pFN_Node = p;
			p = p->pNext;
			free(pFN_Node);
		}
		pFN_NodeHead  =NULL;
	}
}
/********************************************************************
* Function   InitFDData  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
static void     InitFDData(PPHONEBOOK pPBData , int count, HWND *hList)
{
	FN_Item *FN_Node = NULL, *p = NULL;
	int  i= 1;
	PPHONEBOOK pPhoneBookData = NULL;

	pPhoneBookData = pPBData ;

	if(pPhoneBookData == NULL)
		return;

	if(count <= 0)
		return;

    while(i <= count && pPhoneBookData != NULL)
	{
		if(pPhoneBookData->PhoneNum[0] == 0 && pPhoneBookData->Text[0] ==0)
			return;
		FN_Node = malloc(sizeof(FN_Item));

		if(FN_Node == NULL)
			return;

		memset(FN_Node, 0, sizeof(FN_Item));
		FN_Node->id = pPhoneBookData->Index;
		if(pPhoneBookData->Text[0] == 0)
			strcpy(FN_Node->Name, "None");
		else
			strcpy(FN_Node->Name, pPhoneBookData->Text);
		strcpy(FN_Node->Number, pPhoneBookData->PhoneNum);
		FN_Node->pNext = NULL;

		SendMessage(*hList,LB_ADDSTRING,NULL,(LPARAM)ML(FN_Node->Name));
		SendMessage(*hList, LB_SETAUXTEXT, MAKEWPARAM(i, -1), (LPARAM)(FN_Node->Number));
		SendMessage(*hList,LB_SETITEMDATA, i, (LPARAM)FN_Node->id);

		if(pFN_NodeHead == NULL)
			pFN_NodeHead = FN_Node;
		else
		{
			p = pFN_NodeHead;
			while(p->pNext)
			{
				p = p->pNext;
			}
			p->pNext = FN_Node;
		}

		i++;
		pPhoneBookData ++;
	}
	Load_Icon_SetupList(*hList,hIconNormal,hIconSel, count + 1,0);

}
