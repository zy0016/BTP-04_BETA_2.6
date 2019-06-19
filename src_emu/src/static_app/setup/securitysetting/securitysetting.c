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

#include    "securitysetting.h"


BOOL  CreateSecuritysettingWnd(HWND hParentwnd )
{
    WNDCLASS wc;
	HWND hWndApp = NULL;
	RECT rect;

	hFrameWin = NULL;
	hFrameWin = hParentwnd;
	bModifyCode  =FALSE;
    
//	StartObjectDebug();
	SETUPBUTTON_RegisterClass();
	
 	wc.style         = CS_OWNDC;
	wc.lpfnWndProc   = AppWndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = NULL;
	wc.hIcon         = NULL;
	wc.hCursor       = NULL;
	wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = pClassName;
	
	if (!RegisterClass(&wc))
	{
		return FALSE;
	}
	if ( hWndApp )
	{
		ShowWindow ( hWndApp, SW_SHOW );
		SetFocus ( hWndApp );	
	}
	else
	{   
		GetClientRect(hFrameWin, &rect);
		hWndApp = CreateWindow(
			pClassName, 
			NULL,
			WS_CHILD|WS_VISIBLE, 
			rect.left,rect.top,rect.right-rect.left,rect.bottom-rect.top,
			hParentwnd, 
			NULL,
			NULL, 
			NULL
			);
		
		if (NULL == hWndApp)
		{
			UnregisterClass(pClassName,NULL);
			return FALSE;
		}
		ShowWindow(hWndApp, SW_SHOW);
		UpdateWindow(hWndApp);
	}
	return TRUE;
}

static LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{

	LRESULT lResult;
	HDC     hdc;
	
    lResult = (LRESULT)TRUE;
    switch ( wMsgCmd )
    {
    case WM_CREATE :
		OnCreate(hWnd);
        break;

	 case PWM_SHOWWINDOW:
		 {
			 SendMessage(hFrameWin, PWM_SETBUTTONTEXT, (WPARAM)1, (LPARAM)ML(""));

			 SendMessage(hFrameWin, PWM_SETBUTTONTEXT, (WPARAM)0, (LPARAM)ML(IDS_BACK));
			 SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
			 SetWindowText(hFrameWin,ML(TITLECAPTION));
			 
			 if(bFDStatus)
				 SendMessage(GetDlgItem(hWnd, IDC_FIXEDDIAL), WM_SETVALUE , 0, (LPARAM)ML(IDS_ON));
			 else
				 SendMessage(GetDlgItem(hWnd, IDC_FIXEDDIAL), WM_SETVALUE, 0, (LPARAM)ML(IDS_OFF));
			 SetFocus(hSetFocuswnd);
		 }
		 break;
	case CALLBACKFDSET:
		if(wParam == NULL)
			break;
		if(lParam == 1)
			bFDStatus = TRUE;
		else
			bFDStatus = FALSE;
		break;
	case WM_SIMREAD:
		{
			CPHS_struct result;
			
			memset(&result, 0, sizeof(CPHS_struct));
			
			memset(&SimFun, 0x00, sizeof(SimFunctions));	
			
			if(ME_GetResult(&result, sizeof(CPHS_struct)) < 0)
			{
				break;
			}
			
			if(result.Type == FieldNotFound)	
				break;
			else if(result.Type == RecordContent)
			{
				;
			}
			else if(result.Type == FileContent)	
			{
				bReadFileType = TRUE;
				Contentlen = 2 * result.Content_Len;
				
				ME_ReadCPHSFile( hWnd, WM_READFILE, 
					0x6F38, result.Content_Len );//result.Content_Len unsigned char	
				
			}

		}
		break;
	case WM_READFILE:
		{
			unsigned char *result = NULL;
			
			BYTE    istatus =0;
			
			result = (unsigned char *)malloc(Contentlen *sizeof(unsigned char));
			
			if(result == NULL)
			{
				break;;
			}
			
			memset(result, 0, Contentlen *sizeof(unsigned char));
			
			if(bReadFileType)
			{
				if(ME_GetResult(result, Contentlen* sizeof(unsigned char))<0)
				{
					
					if(result)
					{
						free(result);
						result = NULL;
					}
					break;
				}
				ST_ParseStatus(result,Contentlen);
			}
			
			if(result)
			{
				free(result);
				result = NULL;
			}

		}
		break;
		
	case CALLBACK_FNSTATUS:
		OnCallBack_FnStatus(hWnd, wParam);
		break;

	case CALLBACK_STATUS:
		OnCallBack_Status(hWnd, wParam, lParam);
		break;
		
	case WM_PIN_CONFIRM:
		{
			switch (wParam)
			{
			case ME_RS_SUCCESS:
				{
					if(bQueryPuk)
					{
						iPinStatus.blockCHV1 = FALSE;
						PLXTipsWin(hFrameWin, hWnd, NULL,ML(PINCODE_CHANGED), ML(TITLECAPTION), Notify_Success,
							ML(IDS_OK), NULL,DEFAULT_TIMEOUT);
						bQueryPuk = FALSE;
						bModifyCode  =FALSE;
					}
					if(bQueryPuk2)
					{
						iPinStatus.blockCHV2 = FALSE;
						PLXTipsWin(hFrameWin, hWnd, NULL,ML(PIN2CODE_CHANGED), ML(TITLECAPTION), Notify_Success,
							ML(IDS_OK), NULL,DEFAULT_TIMEOUT);
						bQueryPuk2 = FALSE;
						bModifyCode = FALSE;
					}
					WaitWindowState(hWnd,FALSE);
				}
				break;
			case ME_RS_FAILURE:
				{
					if(bQueryPuk)
					{
						ME_GetPassInputLimit(hWnd, CALLBACK_CODE_REMAIN, PUK1);
					}
					if(bQueryPuk2)
					{
						ME_GetPassInputLimit(hWnd, CALLBACK_CODE_REMAIN, PUK2);
					}
					
				}
				break;
			}	
		}
	//	ME_GetCurWaitingPassword(hWnd, WM_PIN_QUERY);
		break;
		
	case WM_PIN_QUERY:
		Codes_OnPinQuery(hWnd);
		break;

	case CALLBACK_NEWCODE:
		Codes_OnCallbackNewCode(hWnd, wParam, lParam);
		break;

	case CALLBACK_CODE:
		Codes_OnCallbackCode(hWnd, wParam, lParam);
		break;

	case CALLBACK_CODE_REMAIN:
		Codes_OnCallbackRemains(hWnd,wParam,lParam);
		break;

	case GETPSCODE:
		{
			unsigned char value[ME_MBID_LEN];
			int i;

			for(i = 0; i< ME_MBID_LEN;i++)
				value[i] = 0;

			printf("\r\n set if sim card changde!");

			if(ME_GetResult(&value, sizeof(unsigned char)*ME_MBID_LEN)<0)
				break;

			SetPSCode(value);
		}
		break;

	case REQUEST_CODE:
		{
			char strCode[EDITMAXLEN+1];
			
			if(!bQueryPin2 && !bQueryPuk2)
				break;
			
			memset(strCode, 0, EDITMAXLEN + 1);
			
			if(bQueryFNStatus)
			{
				if(SSPLXVerifyPassword(hFrameWin, NULL, ML(PIN2CODE), strCode, 
					MAKEWPARAM(EDITMINLEN, EDITMAXLEN), ML(IDS_OK),ML(IDS_CANCEL), -1))
				{
					nQueryCodeType = REQUEST_PIN2;
					strcpy(strPin2,strCode);
					ME_GetPhoneLockStatus (hWnd, CALLBACK_FNSTATUS, LOCK_FD, strPin2);
				}
				else
					bQueryFNStatus = FALSE;
				
				break;
			}
			if(bQueryPuk2)
			{
				if(SSPLXVerifyPassword(hFrameWin,NULL, ML(PUK2CODE), strCode,
						MAKEWPARAM(EDITMAXLEN,EDITMAXLEN), ML(IDS_OK),ML(IDS_CANCEL), -1))
				{
					nQueryCodeType = REQUEST_PUK2;
					strcpy(strPuk2, strCode);
					CallNewCodeWindow(hWnd,REQUEST_PIN2, PIN2_CODE, CALLBACK_NEWCODE);
				}
				else
					bQueryPuk2 = FALSE;
			
				break;
			}
				
		}
		break;

    case WM_CLOSE:
		SendMessage(hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hWnd,0); 
        SendMessage ( hWnd, WM_COMMAND, (WPARAM)IDC_MAINSECURITY_QUIT, 0 ); 
        break;

    case WM_DESTROY :
	//	EndObjectDebug();
		hSetFocuswnd = NULL;
        UnregisterClass(pClassName,NULL);
        break;
		
    case WM_KEYDOWN:
		if (v_nRepeats_ST > 0 && nKeyCode_ST != LOWORD(wParam))
		{
			KillTimer(hWnd, iRepeatKeyTimerId_ST);
			v_nRepeats_ST = 0;
		}
		v_nRepeats_ST++;
		
		nKeyCode_ST = LOWORD(wParam);
		OnKeydown(hWnd, wMsgCmd, wParam, lParam);
        break;
		
	case WM_KEYUP:
		{	
			if(iRepeatKeyTimerId_ST !=0)
			{
				KillTimer(NULL, iRepeatKeyTimerId_ST);
				iRepeatKeyTimerId_ST = 0;
				nKeyCode_ST = 0;
			}
			v_nRepeats_ST = 0;
		}
		break;


	case WM_PAINT:
		hdc = BeginPaint( hWnd, NULL);
		EndPaint(hWnd, NULL);	
		break;

    case WM_COMMAND:
		OnCommand(hWnd, wMsgCmd, wParam, lParam);
		break;
			
	default :
		lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
		break;
	}
	return lResult;
}
/********************************************************************
* Function   OnCreate  
* Purpose     
* Params     hwnd£º
* Return     
* Remarks      
**********************************************************************/
static BOOL OnCreate(HWND hwnd)
{ 	
	HWND  hCodeLocks, hFixedDial;
	
	int  LineY;

	hCodeLocks = CreateWindow("SETUPBUTTON", SECURITYCODE ,
		WS_VISIBLE|WS_CHILD|WS_TABSTOP|WS_BITMAP|CS_NOSYSCTRL,
		WIN_X, WIN_Y, ITEMWINDTH, ITEMHIGH,
		hwnd, (HMENU)IDC_CODELOCK, NULL,NULL);
	
	if(hCodeLocks == NULL)
		return FALSE;
	
	LineY = WIN_Y + ITEMHIGH;

	hFixedDial = CreateWindow("SETUPBUTTON",SECURITYFIXEDDIAL,
		WS_VISIBLE | WS_CHILD | WS_TABSTOP|WS_BITMAP|CS_NOSYSCTRL,
		WIN_X, LineY, ITEMWINDTH, ITEMHIGH,
		hwnd,(HMENU)IDC_FIXEDDIAL,NULL,NULL);
	
	if(hFixedDial == NULL)
		return FALSE;

	if(bFDStatus)
		SendMessage(hFixedDial, WM_SETVALUE , 0, (LPARAM)ML(IDS_ON));
	else
		SendMessage(hFixedDial, WM_SETVALUE, 0, (LPARAM)ML(IDS_OFF));
	
	hSetFocuswnd = hCodeLocks;
	SetFocus(hSetFocuswnd);
	
	SendMessage(hFrameWin , PWM_CREATECAPTIONBUTTON, 
		MAKEWPARAM(IDC_BUTTON_SET,1), (LPARAM)ML(""));
	
	SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, 
		(WPARAM)IDC_MAINSECURITY_QUIT,(LPARAM)ML(IDS_BACK));
	
	SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
	
	SetWindowText(hFrameWin, ML(TITLECAPTION));
	
	if(GetSIMState() == 0)
	{
		;//PLXTipsWin(hFrameWin, hwnd, NULL, ML(INSERT_SIMCARD), ML(TITLECAPTION), Notify_Alert, ML(IDS_OK), NULL, DEFAULT_TIMEOUT);
	}
	else
	{
		ME_GetPhoneLockStatus (hwnd, CALLBACK_FNSTATUS, LOCK_FD, NULL);
	//	ME_GetCPHSstatus( hwnd, CALLBACK_STATUS);
		ME_GetCPHSparameters( hwnd, WM_SIMREAD, 0x6F38 );
		WaitWindowState(hwnd,TRUE);
	
	}

	return TRUE;
}
/********************************************************************
* Function   OnCallBack_FnStatus  
* Purpose     
* Params     hwnd£º
* Return     
* Remarks      
**********************************************************************/
static void	OnCallBack_FnStatus(HWND hWnd, WPARAM wParam)
{
	BOOL bFD = FALSE;
	
	bFDStatus = FALSE;
	
	switch (wParam)
	{
	case ME_RS_SUCCESS:
		ME_GetResult(&bFD,sizeof(BOOL));
		if (bFD)
		{
			SendMessage(GetDlgItem(hWnd, IDC_FIXEDDIAL), WM_SETVALUE, 0, (LPARAM)ML(IDS_ON));
			bFDStatus = TRUE;
		}
		break;
	}
	ME_GetCPHSstatus( hWnd, CALLBACK_STATUS);
}
/********************************************************************
* Function   OnCallBack_Status  
* Purpose     
* Params     hwnd£º
* Return     
* Remarks      
**********************************************************************/
static void	OnCallBack_Status(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	CPHSstatus_t  iStatus;
		
	WaitWindowState(hWnd,FALSE);
	memset(&iPinStatus, 0, sizeof(PINSstatus));
	memset(&iStatus, 0, sizeof(CPHSstatus_t));

	if(wParam == 1)
	{
		switch(lParam)
		{
		case 12:
			iPinStatus.blockCHV1 = TRUE;
			break;
		case 13:
			iPinStatus.blockPUK1 = TRUE;
			break;
		case 18:
			iPinStatus.blockCHV2 = TRUE;
			break;
			
		}
	}
	else
	{
		if(ME_GetResult(&iStatus, sizeof(CPHSstatus_t)) <0)
			return;
		
		if(iStatus.UnblockCHV1 & 0x07) 
			iPinStatus.blockPUK1 = FALSE;
		else
			iPinStatus.blockPUK1 = TRUE;//puk blocked
		
		if(iStatus.UnblockCHV2 & 0x07)
			iPinStatus.blockPUK2 = FALSE;
		else
			iPinStatus.blockPUK2 = TRUE; //puk2 blocked

		if(iStatus.CHV1Status & 0x80)
			iPinStatus.CHV1Status = TRUE;//enable
		else
			iPinStatus.CHV1Status = FALSE; //disable

		if(iStatus.CHV1Status & 0x07)
			iPinStatus.blockCHV1 = FALSE; 
		else
			iPinStatus.blockCHV1 = TRUE;//pin1 blocked
		
		if(iStatus.CHV2Status & 0x80)
			iPinStatus.CHV2Status = TRUE;//enable
		else
			iPinStatus.CHV2Status = FALSE;//disable

		if(iStatus.CHV2Status & 0x07)
			iPinStatus.blockCHV2 = FALSE;
		else
			iPinStatus.blockCHV2 = TRUE;//pin2 blocked
	}
    bReadStatus = TRUE;

	
//	memset(&SimFun, 0, sizeof(SimService));
//	GetSimStatus_CHV1(&SimFun);
//	if(SimFun.SupportStatus == Allocated && SimFun.CurrentStatus == Activated)
	{
//		bPin1Active = TRUE;
	}	
}
/********************************************************************
* Function   OnKeydown  
* Purpose     
* Params     hwnd£º
* Return     
* Remarks      
**********************************************************************/
static void OnKeydown(HWND hWnd,UINT wMsgCmd,WPARAM wParam, LPARAM lParam)
{
	switch(LOWORD(wParam))
	{
	case VK_F10:
		{
			SendMessage(hWnd,WM_CLOSE,0,0);
		}
		break;
	case VK_DOWN:
		
		hSetFocuswnd = GetFocus();
		
		while(GetParent(hSetFocuswnd) != hWnd)
			hSetFocuswnd = GetParent(hSetFocuswnd);
		
		hSetFocuswnd = GetNextDlgTabItem(hWnd, hSetFocuswnd, FALSE);
		SetFocus(hSetFocuswnd);
	//	InvalidateRect(hWnd, NULL, TRUE);
		if(v_nRepeats_ST == 1)
		{
			iRepeatKeyTimerId_ST = SetTimer(NULL, 0,  ET_REPEAT_FIRST, f_STTimerProc);
		}
		break;
	case VK_UP:
		
		hSetFocuswnd = GetFocus();
		
		while(GetParent(hSetFocuswnd) != hWnd)
			hSetFocuswnd = GetParent(hSetFocuswnd);
		
		hSetFocuswnd = GetNextDlgTabItem(hWnd, hSetFocuswnd, TRUE);
		SetFocus(hSetFocuswnd);
	//	InvalidateRect(hWnd,NULL, TRUE);
		if(v_nRepeats_ST == 1)
		{
			iRepeatKeyTimerId_ST = SetTimer(NULL, 0,  ET_REPEAT_FIRST, f_STTimerProc);
		}
		break;
		
		
	default:
		PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
	}
}
/********************************************************************
* Function   f_STTimerProc  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
static void CALLBACK f_STTimerProc(HWND hWnd, UINT uMsg, UINT idEvent,
                                 DWORD dwTime)
{
	if(v_nRepeats_ST == 1)
	{
		KillTimer(NULL, iRepeatKeyTimerId_ST);
		iRepeatKeyTimerId_ST = SetTimer(NULL,0,  ET_REPEAT_LATER,f_STTimerProc );
	}
	keybd_event(nKeyCode_ST, 0, 0, 0);
}
/********************************************************************
* Function   OnCommand  
* Purpose     
* Params     hwnd£º
* Return     
* Remarks      
**********************************************************************/
static void OnCommand(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
	switch( LOWORD( wParam ))
	{
		
	case IDC_CODELOCK:
		if(HIWORD(wParam) == BN_CLICKED)
			CreateCodesandLocksWnd(hWnd);
		break;

	case IDC_FIXEDDIAL:
		if(HIWORD(wParam) == BN_CLICKED)
		{
			
			if(GetSIMState() == 0)
			{
				PLXTipsWin(hFrameWin, hWnd, NULL, ML(INSERT_SIMCARD), ML(TITLECAPTION), Notify_Alert, ML(IDS_OK), NULL, DEFAULT_TIMEOUT);
				break;
			}
			
			if(!bFDAllowded)
			{
				PLXTipsWin(hFrameWin, hWnd, NULL,ML(FEATURE_NOTAVAILABLE), ML(TITLECAPTION),
					Notify_Failure, ML(IDS_OK), NULL, DEFAULT_TIMEOUT);
				break;
			}
			
			if(bReadStatus && iPinStatus.blockCHV2)
			{
				bQueryPuk2 = TRUE;
				nQueryCodeType = REQUEST_PUK2;

				bNotNotify = TRUE;

					
				if( -1 != ME_GetPassInputLimit(hWnd, CALLBACK_CODE_REMAIN, PUK2))
					WaitWindowState(hWnd, TRUE);
				break;
			}
			if(iPinStatus.blockPUK2)
			{
				PLXTipsWin(hFrameWin, hWnd, NULL,ML(PUK2_BLOCKED), ML(TITLECAPTION), Notify_Alert,ML(IDS_OK),
					NULL, DEFAULT_TIMEOUT);
				break;
			}
			CreateFixedDialWnd(hWnd, bFDStatus, CALLBACKFDSET);
			
		}
		break;
	case IDC_MAINSECURITY_QUIT:
		DestroyWindow(hWnd);
		break;
	}
}
/********************************************************************
* Function   CreateCodesandLocksWnd  
* Purpose     
* Params     hwnd£º
* Return     
* Remarks      
**********************************************************************/
static BOOL  CreateCodesandLocksWnd(HWND hParentwnd )
{
    WNDCLASS wc;
	HWND hWndCode = NULL;
	RECT rect;

 	wc.style         = CS_OWNDC;
	wc.lpfnWndProc   = CodesandLocksWndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = NULL;
	wc.hIcon         = NULL;
	wc.hCursor       = NULL;
	wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = pCodeClassName;
	
	if (!RegisterClass(&wc))
	{
		return FALSE;
	}
	if ( hWndCode )
	{
		ShowWindow ( hWndCode, SW_SHOW );
		SetFocus ( hWndCode );	
	}
	else
	{   
		GetClientRect(hFrameWin, &rect);
		hWndCode = CreateWindow(
			pCodeClassName, 
			NULL,
			WS_CHILD|WS_VISIBLE|WS_VSCROLL, 
			rect.left,rect.top,rect.right-rect.left,rect.bottom-rect.top,
			hFrameWin, 
			NULL,
			NULL, 
			NULL
			);
		
		if (NULL == hWndCode)
		{
			UnregisterClass(pCodeClassName,NULL);
			return FALSE;
		}
		ShowWindow(hWndCode, SW_SHOW);
		UpdateWindow(hWndCode);

	
	}
	return TRUE;
}
/********************************************************************
* Function   CodesandLocksWndProc  
* Purpose     
* Params     hwnd£º
* Return     
* Remarks      
**********************************************************************/
static LRESULT CodesandLocksWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{

	LRESULT lResult;
	HDC     hdc;

    lResult = (LRESULT)TRUE;
    switch ( wMsgCmd )
    {
    case WM_CREATE :
		Codes_OnCreate(hWnd);
		break;

	case PWM_SHOWWINDOW:
		{
			SendMessage(hFrameWin, PWM_SETBUTTONTEXT, (WPARAM)1, (LPARAM)ML(""));
			SendMessage(hFrameWin, PWM_SETBUTTONTEXT, (WPARAM)0, (LPARAM)ML(IDS_BACK));
			
			SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
			
			SetWindowText(hFrameWin, ML("Codes and locks"));

			SetFocus(hFocuswnd);
		}
			break;
	case WM_SETFOCUS:
		{
			if(hFocuswnd == GetDlgItem(hWnd, IDC_PHONELOCK))
				break;
			hFocuswnd = GetFocus();
			
			if(GetParent(hFocuswnd) != hWnd)
			{
				hFocuswnd = GetParent(hFocuswnd);
			}
			SetFocus(hFocuswnd);
		}
		break;
	
		 
	case CALLBACK_STATUS:
		OnCallBack_Status(hWnd, wParam, lParam);
		break;

  	case WM_VSCROLL:
		SecuritySet_OnVScroll(hWnd, wParam);
		break;

    case WM_CLOSE:
		SendMessage(hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hWnd,0);
        SendMessage ( hWnd, WM_COMMAND, (WPARAM)IDC_SECURITY_QUIT, 0 ); 
        break;

    case WM_DESTROY :
		hFocuswnd = NULL;
        UnregisterClass(pCodeClassName,NULL);
        break;

	case WM_KILLFOCUS:
		printf("\r\n kill focus setting!");
		KillTimer(hWnd, iRepeatKeyTimerId);
		v_nRepeats = 0;
		nKeyCode   = 0;
		break;
		
    case WM_KEYDOWN:

		if (v_nRepeats > 0 && nKeyCode != LOWORD(wParam))
		{
			KillTimer(hWnd, iRepeatKeyTimerId);
			v_nRepeats = 0;
		}
		v_nRepeats++;
		
		nKeyCode = LOWORD(wParam);
		Codes_OnKeydown(hWnd, wMsgCmd, wParam, lParam);
		break;

	case WM_KEYUP:
		{	
			printf("\r\n keyup setting!");
			if(iRepeatKeyTimerId !=0)
			{
				printf("\r\n kill timer setting!");
				KillTimer(NULL, iRepeatKeyTimerId);
				iRepeatKeyTimerId = 0;
				nKeyCode = 0;
			}
			v_nRepeats = 0;
		}
		break;

	case WM_PAINT:
		hdc = BeginPaint( hWnd, NULL);
		EndPaint(hWnd, NULL);
		break;
		
	case CALLBACK_NEWCODE:
		Codes_OnCallbackNewCode(hWnd, wParam, lParam);
		break;
		
	case WM_PIN_CONFIRM:
		{
			switch (wParam)
			{
			case ME_RS_SUCCESS:
				{
					if(bQueryPuk)
					{
						iPinStatus.blockCHV1 = FALSE;
						PLXTipsWin(hFrameWin, hWnd, NULL,ML(PINCODE_CHANGED), ML(TITLECAPTION), Notify_Success,
							ML(IDS_OK), NULL,DEFAULT_TIMEOUT);
						bQueryPuk = FALSE;
						bModifyCode  =FALSE;
					}
					if(bQueryPuk2)
					{
						iPinStatus.blockCHV2 = FALSE;
						PLXTipsWin(hFrameWin, hWnd, NULL,ML(PIN2CODE_CHANGED), ML(TITLECAPTION), Notify_Success,
							ML(IDS_OK), NULL,DEFAULT_TIMEOUT);
						bQueryPuk2 = FALSE;
						bModifyCode = FALSE;
					}
					WaitWindowState(hWnd,FALSE);
				}
				break;
			case ME_RS_FAILURE:
				{
					if(bQueryPuk)
					{
						ME_GetPassInputLimit(hWnd, CALLBACK_CODE_REMAIN, PUK1);
					}
					if(bQueryPuk2)
					{
						ME_GetPassInputLimit(hWnd, CALLBACK_CODE_REMAIN, PUK2);
					}
					
				}
				break;
			}	
		}
		//ME_GetCurWaitingPassword(hWnd, WM_PIN_QUERY);
		break;
		
	case WM_PIN_QUERY:
		Codes_OnPinQuery(hWnd);
		break;

	case CALLBACK_CODE:
		Codes_OnCallbackCode(hWnd, wParam, lParam);
		break;
		
		
	case CALLBACK_CODE_REMAIN:
		Codes_OnCallbackRemains(hWnd,wParam,lParam);
		break;
		
	case REQUEST_CODE:
		{
			char strCode[EDITMAXLEN+1];
			
			if(!bQueryPin2 && !bQueryPin && !bQueryPuk &&!bQueryPuk2 && !bQuerySimLockCode)
				break;

			memset(strCode, 0, EDITMAXLEN + 1);

			if(bQueryPuk)
			{
				if(bModifyCode)
					bModifyCode = FALSE;

				if(SSPLXVerifyPassword(hFrameWin, NULL, ML(PUKCODE), strCode, 
					MAKEWPARAM(EDITMAXLEN,EDITMAXLEN), ML(IDS_OK),ML(IDS_CANCEL),-1))
				{
					nQueryCodeType = REQUEST_PUK;
					strcpy(strPuk, strCode);
					CallNewCodeWindow(hWnd,REQUEST_PIN, PIN, CALLBACK_NEWCODE);
				}
				else
					bQueryPuk = FALSE;
				break;
			}

			if(bQueryPin)
			{
				if(Codes_iremains == 0 || Codes_iremains ==PUKNUMS)
				{
					if(SSPLXVerifyPassword(hFrameWin, NULL, ML(PUKCODE), strCode, 
						MAKEWPARAM(EDITMAXLEN,EDITMAXLEN), ML(IDS_OK),ML(IDS_CANCEL),-1))
					{
						nQueryCodeType = REQUEST_PUK;
						strcpy(strPuk, strCode);
						CallNewCodeWindow(hWnd,REQUEST_PIN2, PIN2_CODE, CALLBACK_NEWCODE);
					}
					else
						bQueryPin = FALSE;

				}
				else
				{
					
					if(!bModifyCode)
					{
						if(SSPLXVerifyPassword(hFrameWin, NULL, ML(PINCODE), strCode, MAKEWPARAM(EDITMINLEN,EDITMAXLEN), 
							ML(IDS_OK),ML(IDS_CANCEL), -1))
						{
							nQueryCodeType = REQUEST_PIN;
							ME_SetPhoneLockStatus(hWnd,CALLBACK_CODE,LOCK_SC,(char*)strCode,!bPIN1_state);	
							WaitWindowState(hWnd,TRUE);
						}
						else
							bQueryPin = FALSE;
					}
					else
						CallModifyCodeWindow(hWnd,1, PIN, CALLBACK_CODE);
				}
			
				break;
			}
			if(bQueryPin2)
			{
				if(Codes_iremains == 0 || Codes_iremains ==PUKNUMS)
				{
					if(	SSPLXVerifyPassword(hFrameWin,NULL, ML(PUK2CODE), strCode,
						MAKEWPARAM(EDITMAXLEN,EDITMAXLEN), ML(IDS_OK),ML(IDS_CANCEL), -1))
					{
						nQueryCodeType = REQUEST_PUK2;
						strcpy(strPuk2, strCode);
						CallNewCodeWindow(hWnd,REQUEST_PIN2, PIN2_CODE, CALLBACK_NEWCODE);
					}
					else
						bQueryPin2 = FALSE;
					
				}
				else
				{
					if(!bModifyCode)
					{
						if(SSPLXVerifyPassword(hFrameWin, NULL, ML(PIN2CODE), strCode, 
							MAKEWPARAM(EDITMINLEN, EDITMAXLEN), ML(IDS_OK),ML(IDS_CANCEL), -1))
						{
							nQueryCodeType = REQUEST_PIN2;
							strcpy(strPin2,strCode);	
						}
						else
							bQueryPin2 = FALSE;
					}
					else
						CallModifyCodeWindow(hWnd,2, PIN2_CODE, CALLBACK_CODE);

				}
				
				break;				
			}
			if(bQueryPuk2)
			{
				if(bModifyCode)
					bModifyCode = FALSE;
				
				if(	SSPLXVerifyPassword(hFrameWin,NULL, ML(PUK2CODE), strCode,
						MAKEWPARAM(EDITMAXLEN,EDITMAXLEN), ML(IDS_OK),ML(IDS_CANCEL), -1))
				{
					nQueryCodeType = REQUEST_PUK2;
					strcpy(strPuk2, strCode);
					CallNewCodeWindow(hWnd,REQUEST_PIN2, PIN2_CODE, CALLBACK_NEWCODE);
				}
				else
					bQueryPuk2 = FALSE;
			
			}
			if(bQuerySimLockCode)
			{	
				char strPhonecode[EDITMAXLEN + 1];
				
				memset(strPhonecode, 0, EDITMAXLEN + 1);
				
				GetSecurity_code(strPhonecode);

				if(SSPLXVerifyPassword(hFrameWin, NULL, PHONELOCK, strCode,
				   MAKEWPARAM(EDITMINLEN,EDITMINLEN), ML(IDS_OK),ML(IDS_CANCEL), -1))
				{	
					if (strCode[0] != 0 &&  strPhonecode[0] !=0)
					{
						if(strcmp(strCode,strPhonecode) != 0)
						{
							PLXTipsWin(hFrameWin, hWnd, REQUEST_CODE,ML(WRONG_PHONELOCK), ML(PHONELOCKCODE), Notify_Failure, ML(IDS_OK),
								NULL, 0);								
						}
						else
						{					
							CreateSimLockWnd(hWnd, CALLBACK_CODE);
						}
					}

				}
				
			}
			
		}
	  break;
	case CODE_BLOCK:
		{
			ME_GetPassInputLimit(hWnd, CALLBACK_CODE_REMAIN, PUK2);
		}
		break;
	case GETPSCODE:
		{
			unsigned char value[ME_MBID_LEN];
			int i;
			
			for(i = 0; i< ME_MBID_LEN;i++)
				value[i] = 0;
			
			printf("\r\n set if sim card changde!");
			
			if(ME_GetResult(&value, sizeof(unsigned char)*ME_MBID_LEN)<0)
				break;
			
			SetPSCode(value);
		}
		break;

    case WM_COMMAND:
		Codes_OnCommand(hWnd, wMsgCmd, wParam, lParam);
		break;
		
	default :
		lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
		break;
	}
	return lResult;
}

/*********************************************************************\
* Function        SecuritySet_InitVScroll
* Purpose      
* Params          
* Return         
* Remarks        
**********************************************************************/
static void SecuritySet_InitVScroll(HWND hWnd)
{
    SCROLLINFO          vsi;

    memset(&vsi, 0, sizeof(SCROLLINFO));

    vsi.cbSize = sizeof(vsi);
    vsi.fMask  = SIF_ALL ;
    vsi.nMin   = 0;
    vsi.nPage  = 3;
    vsi.nMax   = (MAX_ITEMNUM-1);
    vsi.nPos   = 0;

    SetScrollInfo(hWnd, SB_VERT, &vsi, TRUE);
	nCurFocus =0;
	return;
}
/*********************************************************************\
* Function        SecuritySet_OnVScroll
* Purpose      
* Params          
* Return         
* Remarks        
**********************************************************************/
static void SecuritySet_OnVScroll(HWND hWnd,  UINT wParam)
{
    int  nY;
    RECT rcClient;
    SCROLLINFO      vsi;

    switch(wParam)
    {
    case SB_LINEDOWN:
		
		memset(&vsi, 0, sizeof(SCROLLINFO));
		
        vsi.fMask  = SIF_ALL ;
        GetScrollInfo(hWnd, SB_VERT, &vsi);
		
        GetClientRect(hWnd,&rcClient);
        nY = (rcClient.bottom - rcClient.top)/3;
		
        nCurFocus++;
		
        if(nCurFocus > vsi.nMax)	
        {
            ScrollWindow(hWnd,0,(vsi.nMax-vsi.nPage+1)*nY,NULL,NULL);	
            UpdateWindow(hWnd);
            nCurFocus = 0;
			vsi.nPos = 0;
            vsi.fMask  = SIF_POS ;
            SetScrollInfo(hWnd, SB_VERT, &vsi, TRUE);
            break;
        }
	
        if((int)(vsi.nPos + vsi.nPage -1) <= nCurFocus && nCurFocus != vsi.nMax)	
        { 
            ScrollWindow(hWnd,0,-nY,NULL,NULL);
            UpdateWindow(hWnd);
            vsi.nPos++;
            vsi.fMask  = SIF_POS ;
            SetScrollInfo(hWnd, SB_VERT, &vsi, TRUE);
            break;
        }
		break;
		
    case SB_LINEUP:
		
        memset(&vsi, 0, sizeof(SCROLLINFO));
        vsi.fMask  = SIF_ALL ;
        GetScrollInfo(hWnd, SB_VERT, &vsi);
		
        GetClientRect(hWnd,&rcClient);
		
        nY = (rcClient.bottom - rcClient.top)/3;
		
        nCurFocus--;
		
        if(nCurFocus < vsi.nMin)	
        {
            ScrollWindow(hWnd,0,(vsi.nPage-vsi.nMax-1)*nY,NULL,NULL);
			
            UpdateWindow(hWnd);
			nCurFocus = vsi.nMax;
		    vsi.nPos = vsi.nMax-vsi.nPage+1;
            vsi.fMask  = SIF_POS ;
            SetScrollInfo(hWnd, SB_VERT, &vsi, TRUE);
            break;	
        }
		
        if((int)vsi.nPos == nCurFocus && nCurFocus != vsi.nMin)	
        { 
            ScrollWindow(hWnd,0,nY,NULL,NULL);
            UpdateWindow(hWnd);
            vsi.nPos--;
            vsi.fMask  = SIF_POS ;
            SetScrollInfo(hWnd, SB_VERT, &vsi, TRUE);   
            break;	
        }
		break;
    case SB_PAGEDOWN:
        break;
    case SB_PAGEUP:
        break;
    default:
        break;	
    }	
}
/********************************************************************
* Function   Codes_OnCreate  
* Purpose     
* Params     hwnd£º
* Return     
* Remarks      
**********************************************************************/
static void Codes_OnCreate(HWND hwnd)
{	
	HWND hPinQuery, hPin, hPin2, hPhoneLock;
	HWND  hSimLock;
	int  LineY;
	
	
	hPinQuery =CreateWindow("SETUPBUTTON", SECURITYPINREQUEST,
		WS_VISIBLE | WS_CHILD | WS_TABSTOP |WS_BITMAP|CS_NOSYSCTRL ,
		WIN_X, WIN_Y, ITEMWINDTH - GetSystemMetrics(SM_CXVSCROLL), ITEMHIGH,
		hwnd,(HMENU)IDC_PINQUERY,NULL,NULL);
	
	if(hPinQuery == NULL)
		return;

#ifndef _EMULATE_

	if(/*GetSecurity_Open() ||*/GetPin1Status())
	{
		SendMessage(hPinQuery, WM_SETVALUE, 0, (LPARAM)ML(IDS_ON));
		bPIN1_state = TRUE;
	}
	else
	{
		SendMessage(hPinQuery, WM_SETVALUE, 0, (LPARAM)ML(IDS_OFF));
		bPIN1_state = FALSE;
	}

#else

	if(GetSecurity_Open() ||GetPin1Status())
	{
		SendMessage(hPinQuery, WM_SETVALUE, 0, (LPARAM)ML(IDS_ON));
		bPIN1_state = TRUE;
	}
	else
	{
		SendMessage(hPinQuery, WM_SETVALUE, 0, (LPARAM)ML(IDS_OFF));
		bPIN1_state = FALSE;
	}

#endif

	
	LineY = WIN_Y + ITEMHIGH;
	
	hPin = CreateWindow("SETUPBUTTON", SECURITYPIN ,
		WS_VISIBLE|WS_CHILD|WS_TABSTOP|WS_BITMAP|CS_NOSYSCTRL,
		WIN_X, LineY, ITEMWINDTH - GetSystemMetrics(SM_CXVSCROLL), ITEMHIGH,
		hwnd, (HMENU)IDC_PIN, NULL,NULL);
	
	if(hPin == NULL)
		return;
	
	LineY +=ITEMHIGH;
	
	hPin2 = CreateWindow("SETUPBUTTON", SECURITYPIN2,
		WS_VISIBLE|WS_CHILD|WS_TABSTOP|WS_BITMAP|CS_NOSYSCTRL,
		WIN_X, LineY, ITEMWINDTH - GetSystemMetrics(SM_CXVSCROLL), ITEMHIGH,
		hwnd, (HMENU)IDC_PIN2, NULL,NULL);
	
	if(hPin2 == NULL)
		return ;
	
	LineY +=ITEMHIGH;
	
	hPhoneLock = CreateWindow("SETUPBUTTON", SECURITYPHOENLOCK,
		WS_VISIBLE|WS_CHILD|WS_TABSTOP|WS_BITMAP|CS_NOSYSCTRL,
		WIN_X, LineY, ITEMWINDTH - GetSystemMetrics(SM_CXVSCROLL), ITEMHIGH,
		hwnd, (HMENU)IDC_PHONELOCK, NULL,NULL);
	
	if(hPhoneLock == NULL)
		return;
	
	LineY +=ITEMHIGH;
    
	hSimLock = CreateWindow("SETUPBUTTON",SECURITYSIMLOCK,
		WS_VISIBLE | WS_CHILD | WS_TABSTOP|CS_NOSYSCTRL,
		WIN_X, LineY, ITEMWINDTH - GetSystemMetrics(SM_CXVSCROLL), ITEMHIGH,
		hwnd,(HMENU)IDC_SIMLOCK,NULL,NULL);
	
	if(hSimLock == NULL)
		return;
	
	if(GetPS_Open())
		SendMessage(hSimLock, WM_SETVALUE, 0, (LPARAM)ML(IDS_ON));
	else
		SendMessage(hSimLock, WM_SETVALUE, 0, (LPARAM)ML(IDS_OFF));
	
//	SendMessage(hSimLock, WM_SETARROW, 0, 0);
	
	hFocuswnd = hPinQuery;
	SetFocus(hFocuswnd);
	
	SendMessage(hFrameWin, PWM_SETBUTTONTEXT, (WPARAM)0, (LPARAM)ML(IDS_BACK)); 
	
	SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
	
	SetWindowText(hFrameWin, ML("Codes and locks"));
	
	SecuritySet_InitVScroll(hwnd);
	
	if(GetSIMState() == 0)
	{
		;//PLXTipsWin(hFrameWin, hwnd, NULL, ML(INSERT_SIMCARD), ML(TITLECAPTION), Notify_Alert, ML(IDS_OK), NULL, DEFAULT_TIMEOUT);
	}
	else
	{
		ME_GetCPHSstatus( hwnd, CALLBACK_STATUS);
		WaitWindowState(hwnd,TRUE);
	}
	
}
/********************************************************************
* Function   Codes_OnActivate  
* Purpose     
* Params     hwnd£º
* Return     
* Remarks      
**********************************************************************/
static void	Codes_OnActivate(HWND hWnd,WPARAM wParam)
{
	if (WA_INACTIVE != LOWORD(wParam))
	{
		SetFocus(hFocuswnd);
	}
	else
	{
		if(hFocuswnd == GetDlgItem(hWnd, IDC_PHONELOCK))
			return;
		hFocuswnd = GetFocus();
		
		if(GetParent(hFocuswnd) != hWnd)
		{
			hFocuswnd = GetParent(hFocuswnd);
		}
	}
}
/********************************************************************
* Function   f_TimerProc  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
static void CALLBACK f_TimerProc(HWND hWnd, UINT uMsg, UINT idEvent,
                                 DWORD dwTime)
{
	if(v_nRepeats == 1)
	{
		KillTimer(NULL, iRepeatKeyTimerId);
		printf("\r\n enter second timer setting!");
		iRepeatKeyTimerId = SetTimer(NULL,0,  ET_REPEAT_LATER,f_TimerProc );
	}
	printf("\r\n send keyevents setting!");
	keybd_event(nKeyCode, 0, 0, 0);
}
/********************************************************************
* Function   Codes_OnKeydown  
* Purpose     
* Params     hwnd£º
* Return     
* Remarks      
**********************************************************************/
static void Codes_OnKeydown(HWND hWnd, UINT wMsgCmd,WPARAM wParam, LPARAM lParam)
{
	switch(LOWORD(wParam))
	{

	case VK_F10:
		{	
//			if(bQueryPin || bQuerySimLockCode || bQueryPuk || bQueryPuk2)
//			{
//				bQueryPin = FALSE;
//				bQuerySimLockCode = FALSE;
//				bQueryPuk = FALSE;
//				bQueryPuk2 = FALSE;
//				break;
//			}
			SendMessage(hWnd,WM_CLOSE,0,0);
		}
		break;
	case VK_DOWN:
		
		SendMessage(hWnd, WM_VSCROLL, SB_LINEDOWN, NULL);

		hFocuswnd = GetFocus();
		
		while(GetParent(hFocuswnd) != hWnd)
			hFocuswnd = GetParent(hFocuswnd);
		
		hFocuswnd = GetNextDlgTabItem(hWnd, hFocuswnd, FALSE);
		SetFocus(hFocuswnd);
	//	InvalidateRect(hWnd, NULL, TRUE);

		if(v_nRepeats == 1)
		{
			printf("\r\n set timer setting!");
			iRepeatKeyTimerId = SetTimer(NULL, 0,  ET_REPEAT_FIRST, f_TimerProc);
		}
	
		break;

	case VK_UP:
		
		SendMessage(hWnd, WM_VSCROLL, SB_LINEUP, NULL);
		hFocuswnd = GetFocus();
		
		while(GetParent(hFocuswnd) != hWnd)
			hFocuswnd = GetParent(hFocuswnd);
		
		hFocuswnd = GetNextDlgTabItem(hWnd, hFocuswnd, TRUE);
		SetFocus(hFocuswnd);
		
	//	InvalidateRect(hWnd, NULL, TRUE);

		if(v_nRepeats == 1)
		{
			printf("\r\n set timer setting!");
			iRepeatKeyTimerId = SetTimer(NULL, 0,  ET_REPEAT_FIRST, f_TimerProc);
		}
		break;
//	case VK_LEFT:
//	case VK_RIGHT:
//		{
//			if(GetFocus() == GetDlgItem(hWnd, IDC_SIMLOCK))
//			{
//				BOOL bSimLockState  =FALSE;
//	
//				if(GetPS_Open())
//				{
//					bSimLockState = TRUE;
//				}
//				SetPS_Open(!bSimLockState);
//				
//				if(!bSimLockState)
//					SendMessage(GetDlgItem(hWnd, IDC_SIMLOCK), WM_SETVALUE, 0, (LPARAM)ML(IDS_ON));
//				else
//					SendMessage(GetDlgItem(hWnd, IDC_SIMLOCK), WM_SETVALUE, 0, (LPARAM)ML(IDS_OFF));
//
//				if(!bSimLockState)
//				{
//					char strPsCode[ME_MBID_LEN];
//					
//					memset(strPsCode, 0, ME_MBID_LEN);
//
//					InvalidateRect(hWnd, NULL, TRUE);
//					
//					GetPSCode(strPsCode);
//					
//					if(strPsCode[0] == 0)
//					{
//						ME_GetSubscriberId(hWnd, GETPSCODE);
//						WaitWindowState(hWnd, TRUE);
//						break;
//					}
//				}
//			}
//			InvalidateRect(hWnd, NULL, TRUE);
//		}
//		break;				
	
	default:
		PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
	}
}
/********************************************************************
* Function   Codes_OnCallbackNewCode  
* Purpose     
* Params     hwnd£º
* Return     
* Remarks      
**********************************************************************/
static void	 Codes_OnCallbackNewCode(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if(lParam == NULL)
		return;
				
	if(LOWORD(wParam) == REQUEST_PIN)
	{
		strcpy(strPin1, (char*)lParam);
		bQueryPuk = TRUE;
		ME_PasswordValidation(hWnd, WM_PIN_CONFIRM, PUK1, strPuk, strPin1);
	}
	if(LOWORD(wParam) == REQUEST_PIN2)
	{
		strcpy(strPin2, (char*)lParam);
		bQueryPuk2 = TRUE;
		ME_PasswordValidation(hWnd, WM_PIN_CONFIRM, PUK2, strPuk2, strPin2);
	}			
}
/********************************************************************
* Function   Codes_OnPinQuery  
* Purpose     
* Params     hwnd£º
* Return     
* Remarks      
**********************************************************************/
static void	Codes_OnPinQuery(HWND hWnd)
{			
	int type;			
	ME_GetResult(&type, sizeof(int));
	if (type == NOPIN)
	{
		if(bQueryPuk)
		{
			iPinStatus.blockCHV1 = FALSE;
			PLXTipsWin(hFrameWin, hWnd, NULL,ML(PINCODE_CHANGED), ML(TITLECAPTION), Notify_Success,
				ML(IDS_OK), NULL,DEFAULT_TIMEOUT);
			bQueryPuk = FALSE;
			bModifyCode  =FALSE;
		}
		if(bQueryPuk2)
		{
			iPinStatus.blockCHV2 = FALSE;
			PLXTipsWin(hFrameWin, hWnd, NULL,ML(PIN2CODE_CHANGED), ML(TITLECAPTION), Notify_Success,
				ML(IDS_OK), NULL,DEFAULT_TIMEOUT);
			bQueryPuk2 = FALSE;
			bModifyCode = FALSE;
		}
		WaitWindowState(hWnd,FALSE);
		return;
	}
	else if (type == PUK1)
	{
		ME_GetPassInputLimit(hWnd, CALLBACK_CODE_REMAIN, PUK1);
		return;
	}
	else if(type == PUK2)
	{
		ME_GetPassInputLimit(hWnd, CALLBACK_CODE_REMAIN, PUK2);
		return;
	}
}
/********************************************************************
* Function   Codes_OnCommand  
* Purpose     
* Params     hwnd£º
* Return     
* Remarks      
**********************************************************************/
static void Codes_OnCommand(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
	switch( LOWORD( wParam ))
	{
	case IDC_PINQUERY:
		if(HIWORD(wParam) == BN_CLICKED)
		{
						
			if(GetSIMState() == 0)
			{
				PLXTipsWin(hFrameWin, hWnd, NULL,ML(INSERT_SIMCARD), ML(TITLECAPTION), 
					Notify_Alert, ML(IDS_OK), NULL, DEFAULT_TIMEOUT);
				break;
			}
			
			if(iPinStatus.blockCHV1)
			{
				bQueryPuk = TRUE;
				nQueryCodeType = REQUEST_PUK;
				bNotNotify = TRUE;
				if(-1 != ME_GetPassInputLimit(hWnd, CALLBACK_CODE_REMAIN, PUK1))
					WaitWindowState(hWnd, TRUE);
				break;
			}
			if(/*!(istatusinfo->iStatus & 0x80)*/!bPin1Active)
			{
				SendMessage(GetDlgItem(hWnd,IDC_PINQUERY),SSBM_SETCURSEL, bPIN1_state, 0);
				PLXTipsWin(hFrameWin, hWnd, NULL,ML(FEATURE_NOTAVAILABLE), ML(TITLECAPTION), 
					Notify_Failure, ML(IDS_OK), NULL, DEFAULT_TIMEOUT);
				break;
			}
		
			bQueryPin = TRUE;
			nQueryCodeType = REQUEST_PIN;
			bNotNotify = TRUE;
			if(-1 != ME_GetPassInputLimit(hWnd, CALLBACK_CODE_REMAIN, PIN1))
				WaitWindowState(hWnd, TRUE);
		
		}
		break;
	case IDC_PIN:
		if(HIWORD(wParam) == BN_CLICKED)
		{
			int  nCurPinState;

			if(GetSIMState() == 0)
			{
				PLXTipsWin(hFrameWin, hWnd, NULL, ML(INSERT_SIMCARD), ML(TITLECAPTION), Notify_Alert, ML(IDS_OK), NULL, DEFAULT_TIMEOUT);
				break;
			}
#ifndef _EMULATE_

			nCurPinState = /*GetSecurity_Open() ||*/GetPin1Status();
#else 
			nCurPinState = GetSecurity_Open() ||GetPin1Status();
#endif

			if(iPinStatus.blockCHV1)
			{
				bQueryPuk = TRUE;
				nQueryCodeType = REQUEST_PUK;
				bNotNotify = TRUE;
				if(-1 != ME_GetPassInputLimit(hWnd, CALLBACK_CODE_REMAIN, PUK1))
					WaitWindowState(hWnd, TRUE);
			}
			else
			{
				if(!nCurPinState)
					PLXTipsWin(hFrameWin, hWnd, NULL, ML(SWITCH_PINCODE_REQUEST),ML(TITLECAPTION),
					Notify_Alert, ML(IDS_OK), NULL, DEFAULT_TIMEOUT);
				else
				{
					bModifyCode = TRUE;
					nQueryCodeType = REQUEST_PIN;
					CallModifyCodeWindow(hWnd,1, PIN, CALLBACK_CODE);	
				}
			}
		}
		//input pin
		break;
	case IDC_PIN2:
		if(HIWORD(wParam) == BN_CLICKED)
		{
			char strCode[EDITMAXLEN+1];
			
			if(GetSIMState() == 0)
			{
				PLXTipsWin(hFrameWin, hWnd, NULL, ML(INSERT_SIMCARD), ML(TITLECAPTION), Notify_Alert, ML(IDS_OK), NULL, DEFAULT_TIMEOUT);
				break;
			}

			if(!iPinStatus.CHV2Status)
			{
				PLXTipsWin(hFrameWin, hWnd, NULL, ML(FEATURE_NOTAVAILABLE), ML(TITLECAPTION),
				Notify_Failure, ML(IDS_OK), NULL, DEFAULT_TIMEOUT);
				break;
			}

			if(bReadStatus && iPinStatus.blockCHV2)
			{
				bNotNotify  =TRUE;
				nQueryCodeType = REQUEST_PUK2;
				bQueryPuk2 = TRUE;
				PLXTipsWin(hFrameWin, hWnd, CODE_BLOCK, ML(PIN2_BLOCKED), ML(TITLECAPTION), Notify_Failure,ML(IDS_OK),
				NULL, 0);
				break;
			}

			if(bReadStatus && iPinStatus.blockPUK2)
			{
				PLXTipsWin(hFrameWin, hWnd, NULL, ML(PUK2_BLOCKED), ML(TITLECAPTION), Notify_Alert,ML(IDS_OK),
					NULL, DEFAULT_TIMEOUT);
				break;
			}
			
			memset(strCode, 0, EDITMAXLEN+1);
			bModifyCode = TRUE;	
			nQueryCodeType = REQUEST_PIN2;
			CallModifyCodeWindow(hWnd,2, PIN2_CODE, CALLBACK_CODE);
		}
		//input pin2
		break;
	case IDC_PHONELOCK:
		if(HIWORD(wParam) == BN_CLICKED)
		{
			CallPhoneLockWnd(hWnd);
		}
		break;
		
	   case IDC_SIMLOCK:

		   if(GetSIMState() == 0)
			{
				PLXTipsWin(hFrameWin, hWnd, NULL, ML(NOTALLOWEDNOSIM), ML(TITLECAPTION), Notify_Info, ML(IDS_OK), NULL, DEFAULT_TIMEOUT);
				break;
			}

		   if(HIWORD(wParam) == BN_CLICKED)
		   {
			   char strCode[EDITMAXLEN + 1];
			   char cLockcode[EDITMAXLEN +1];
			   BOOL bPsLock = FALSE;
			   
			   memset(strCode, 0, EDITMAXLEN+1);
			   memset(cLockcode, 0, EDITMAXLEN+1);
			   
			   bQuerySimLockCode = TRUE;
			   
			   if(SSPLXVerifyPassword(hFrameWin, NULL, PHONELOCK, strCode,
				   MAKEWPARAM(EDITMINLEN,EDITMINLEN), ML(IDS_OK),ML(IDS_CANCEL), -1))
			   {
				   GetSecurity_code(cLockcode);
				   
				   if (strCode[0] != 0 && cLockcode[0] !=0)
				   {
					   if(strcmp(strCode,cLockcode) != 0)
					   {
						   PLXTipsWin(hFrameWin, hWnd, REQUEST_CODE, ML(WRONG_PHONELOCK), ML(PHONELOCKCODE),
							   Notify_Failure, ML(IDS_OK),  NULL, WAITTIMEOUT);								
						   
					   }
					   else
					   {
						   CreateSimLockWnd(hWnd, CALLBACK_CODE);		  
					   }
				   }
				 
			   }
		   }
		   break;
	  case IDC_SECURITY_QUIT:
		   DestroyWindow(hWnd);
		   break;
	}
}

/********************************************************************
* Function   Codes_OnCallbackCode  
* Purpose     
* Params     hwnd£º
* Return     
* Remarks      
**********************************************************************/
static void Codes_OnCallbackCode(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	int  nType;
	
	WaitWindowState(hWnd,FALSE);
	switch (wParam)
	{
	case ME_RS_SUCCESS://on or off pin
		if(bModifyCode)
		{
			bModifyCode = FALSE;
			switch(nQueryCodeType)
			{
			case REQUEST_PIN:
				PLXTipsWin(hFrameWin, hWnd, NULL, ML(PINCODE_CHANGED), ML(PIN),Notify_Success, 
					ML(IDS_OK),NULL, DEFAULT_TIMEOUT);	
				break;
			case REQUEST_PIN2:
				{
					SetPin2Change();//PIN2 changed
					PLXTipsWin(hFrameWin, hWnd, NULL,ML(PIN2CODE_CHANGED), ML(PIN2_CODE),
						Notify_Success, ML(IDS_OK),NULL, DEFAULT_TIMEOUT);			
				}
				break;
			case REQUEST_LOCKCODE:
				PLXTipsWin(hFrameWin, hWnd, NULL,ML(PHONELOCK_CHANGED), ML(PHONELOCKCODE), 
					Notify_Success, ML(IDS_OK), NULL,NULL);
				break;
			}

			break;
		}
		
		if(bQueryPin)
		{
			printf("\r\n setpin----------------->set pin satus success!\r\n");
			bQueryPin = FALSE;
			SetSecurity_Open(!bPIN1_state);
			SetPin1Status(!bPIN1_state);
			if(!bPIN1_state)
				SendMessage(GetDlgItem(hWnd, IDC_PINQUERY), WM_SETVALUE, 0, (LPARAM)ML(IDS_ON) );
			else
				SendMessage(GetDlgItem(hWnd, IDC_PINQUERY), WM_SETVALUE, 0, (LPARAM)ML(IDS_OFF) );
			bPIN1_state = !bPIN1_state;
		}

		if(bQueryPin2)
		{
			SendMessage(GetDlgItem(hWnd,IDC_FIXEDDIAL),SSBM_SETCURSEL, LOWORD(wParam), 0);
			bQueryPin2 =FALSE;
		}
		
		break;	
	default:
		switch(nQueryCodeType)
		{
		case REQUEST_PIN:
			nType = PIN1;
			break;
		case REQUEST_PIN2:
			nType = PIN2;
			break;
		case REQUEST_PUK:
			nType = PUK1;
			break;			
		case REQUEST_PUK2:
			nType = PUK2;
			break;
//		case REQUEST_LOCKCODE:
//			nType = SIMPHLOCK;
//			break;
		}
		
		if (-1 != ME_GetPassInputLimit(hWnd,CALLBACK_CODE_REMAIN,nType))
			WaitWindowState(hWnd,TRUE);
	}
}
/********************************************************************
* Function   Codes_OnCallbackRemains  
* Purpose     
* Params     hwnd£º
* Return     
* Remarks      
**********************************************************************/
static void Codes_OnCallbackRemains(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
 
	WaitWindowState(hWnd,FALSE);
	
	Codes_iremains = 0;

	ME_GetResult(&Codes_iremains,sizeof(int));
	if ((Codes_iremains == 0) || (Codes_iremains == PUKNUMS))
	{
		switch(nQueryCodeType)
		{
		case REQUEST_PIN:
			{
				iPinStatus.blockCHV1 = TRUE;
				printf("\r\n setpin--------------->pin blocked!\r\n");
				nQueryCodeType = REQUEST_PUK;
				bQueryPin= FALSE;
				bQueryPuk = TRUE;
				if(!bNotNotify)
				{
					PLXTipsWin(hFrameWin, hWnd, REQUEST_CODE,ML(PIN_BLOCKED),ML(PIN), Notify_Failure, ML(IDS_OK), NULL, 0);
				}
				else
				{
					bNotNotify = FALSE;	
					SendMessage(hWnd, REQUEST_CODE, 0, 0);
				}
			}					
			break;
			
		case REQUEST_PIN2:
			{
				iPinStatus.blockCHV2 = TRUE;
				nQueryCodeType = REQUEST_PUK2;
				bQueryPin2=FALSE;
				bQueryPuk2 = TRUE;

				if(!bNotNotify)
				{
					PLXTipsWin(hFrameWin, hWnd, REQUEST_CODE, ML(PIN2_BLOCKED),ML(PIN2_CODE), 
						Notify_Failure, ML(IDS_OK), NULL, 0);
				}
				else
				{
					bNotNotify = FALSE;
					SendMessage(hWnd, REQUEST_CODE, 0, 0);
				}
			}
			break;

		case REQUEST_PUK:
			{
				char strCode[EDITMAXLEN + 1];
				
				memset(strCode, 0, EDITMAXLEN+1);
				
				if(Codes_iremains == 0)
					iPinStatus.blockPUK1 = TRUE;
				bQueryPuk = FALSE;
				if(!bNotNotify && Codes_iremains == 0)
				{
					PLXTipsWin(hFrameWin, hWnd, NULL, ML(SIM_BLOCKED),ML(PUK), Notify_Alert, ML(IDS_OK), NULL, 0);
				}
				else
					bNotNotify = FALSE;

				if(!iPinStatus.blockPUK1)
				{
					if(SSPLXVerifyPassword(hFrameWin, ML(PUK), ML(PUKCODE), strCode,
						MAKEWPARAM(EDITMAXLEN,EDITMAXLEN), ML(IDS_OK),ML(IDS_CANCEL), -1))
					{
						nQueryCodeType = REQUEST_PUK;
						strcpy(strPuk, strCode);
						CallNewCodeWindow(hWnd,REQUEST_PIN, PIN, CALLBACK_NEWCODE);
					}

				}
		
			}
			break;
		case REQUEST_PUK2:
			{
				if(!bNotNotify && Codes_iremains == 0)
				{
					PLXTipsWin(hFrameWin, hWnd, NULL, ML(PUK2_BLOCKED),ML(PUK2_CODE), Notify_Alert, ML(IDS_OK), NULL, 0);
				}
				else
				{
					bNotNotify = FALSE;
					bQueryPuk2 = TRUE;
					PLXTipsWin(hFrameWin, hWnd, REQUEST_CODE,ML(PIN2_BLOCKED), ML(TITLECAPTION),
					Notify_Failure, ML(IDS_OK), NULL, 0);
					break;
				}
				
				if(Codes_iremains == 0)
					iPinStatus.blockPUK2 = TRUE;
				
				bQueryPuk2 = FALSE;
				if(!iPinStatus.blockPUK2)
				{
					char strCode[EDITMAXLEN + 1];

					memset(strCode, 0, EDITMAXLEN+1);

					if(SSPLXVerifyPassword(hFrameWin, ML(PUK2_CODE), ML(PUK2CODE), strCode,
					MAKEWPARAM(EDITMAXLEN,EDITMAXLEN),ML(IDS_OK),ML(IDS_CANCEL), -1))
					{
						nQueryCodeType = REQUEST_PUK2;
						strcpy(strPuk2, strCode);
						CallNewCodeWindow(hWnd,REQUEST_PIN2, PIN2_CODE, CALLBACK_NEWCODE);
					}
				}
			}
			break;	
		}
		
	}
	else
	{
		switch(nQueryCodeType)
		{
		case REQUEST_PIN:
			
			bQueryPin = TRUE;
			if(!bNotNotify)
			{
				PLXTipsWin(hFrameWin, hWnd, REQUEST_CODE, ML(WRONG_PINCODE), ML(PIN), Notify_Failure, ML(IDS_OK), NULL, DEFAULT_TIMEOUT);
			}
			else
			{
				bNotNotify = FALSE;
				SendMessage(hWnd, REQUEST_CODE, 0, 0);
			}
		
			break;

		case REQUEST_PIN2:
			
			bQueryPin2 = TRUE;
			if(!bNotNotify)
			{
				PLXTipsWin(hFrameWin, hWnd, REQUEST_CODE,ML(WRONG_PIN2), ML(PIN2_CODE), 
					Notify_Failure, ML(IDS_OK), NULL, DEFAULT_TIMEOUT);
			}
			else
			{
				bNotNotify  = FALSE;
				SendMessage(hWnd, REQUEST_CODE, 0, 0);
			}
		
			break;

		case REQUEST_PUK:
			
			bQueryPuk = TRUE;
			if(!bNotNotify)
			{
				PLXTipsWin(hFrameWin, hWnd, REQUEST_CODE, ML(WRONG_PUKCODE), ML(PUK), 
					Notify_Failure, ML(IDS_OK), NULL, 0);
			}
			else
			{
				bNotNotify = FALSE;
				SendMessage(hWnd, REQUEST_CODE, 0, 0);
			}
		
			break;

		case REQUEST_PUK2:

			bQueryPuk2 = TRUE;
			if(!bNotNotify)
			{
				PLXTipsWin(hFrameWin, hWnd, REQUEST_CODE, ML(WRONG_PUK2CODE), ML(PUK2_CODE), 
					Notify_Failure, ML(IDS_OK), NULL, 0);
			}
			else
			{
				bNotNotify  = FALSE;
				PLXTipsWin(hFrameWin, hWnd, REQUEST_CODE, ML(PIN2_BLOCKED), ML(PIN2_CODE), 
					Notify_Failure, ML(IDS_OK), NULL, 0);
			}

			break;

		case REQUEST_LOCKCODE:
			bQuerySimLockCode = TRUE;
			PLXTipsWin(hFrameWin, hWnd, NULL,ML(WRONG_PHONELOCK), ML(PHONELOCKCODE), Notify_Failure, 
				ML(IDS_OK), NULL, 0);
			break;
		}
	}

}
/********************************************************************
* Function   ST_ParseStatus  
* Purpose    parse the sim file content
* Params     
* Return     
* Remarks      
**********************************************************************/
static void   ST_ParseStatus(unsigned char *strCode, int iconter)
{
	#define IFALLOCATED	0x01
	#define IFACTIVATED	0x02	
		
	unsigned char *temp = NULL;
	unsigned char CharTempH = NULL;
	unsigned char CharTempL = NULL;
	unsigned char *str = NULL, i = 0;
	BOOL bSet= FALSE;
	int  j = 0;

	temp = (unsigned char *)malloc((iconter/2)*sizeof(unsigned char));
	if(temp == NULL)
		return;	
	
	for(j=0; j<(iconter/2); j++)
	{
		CharTempH = *(strCode+2*j);	
		if(CharTempH >= '0' && CharTempH <= '9')
			CharTempH = CharTempH - '0';
		else if(CharTempH >= 'a' && CharTempH <= 'f')
			CharTempH = CharTempH - 'a' + 10;	
		else if(CharTempH >= 'A' && CharTempH <= 'F')	 	
			CharTempH = CharTempH - 'A' + 10;
		else 
		{
			//something wrong
			free(temp);
			return;	
		}	
		
		CharTempL = *(strCode+2*j+1);	
		if(CharTempL >= '0' && CharTempL <= '9')
			CharTempL = CharTempL - '0';
		else if(CharTempL >= 'a' && CharTempL <= 'f')
			CharTempL = CharTempL - 'a' + 10;	
		else if(CharTempL >= 'A' && CharTempL <= 'F')	 	
			CharTempL = CharTempL - 'A' + 10;
		else 
		{
			//something wrong
			free(temp);
			return;	
		}		
		
		//convert the service string
		temp[j] = CharTempL + (CharTempH<<4);
	}	

	//FDN service
	if((temp[0] >> 4) & IFALLOCATED)
	{
		SimFun.FDN.SupportStatus = Allocated;
		if((temp[0] >> 4) & IFACTIVATED)
			SimFun.FDN.CurrentStatus = Activated;
		else
			SimFun.FDN.CurrentStatus = Inactivated;	
	}
	else
		SimFun.FDN.SupportStatus = NotAllocated;

	//CHV1 service
	if((temp[0]) & IFALLOCATED)
	{
		SimFun.CHV1.SupportStatus = Allocated;
		if((temp[0]) & IFACTIVATED)
			SimFun.CHV1.CurrentStatus = Activated;
		else
			SimFun.CHV1.CurrentStatus = Inactivated;	
	}
	else
		SimFun.CHV1.SupportStatus = NotAllocated;

	if(SimFun.CHV1.SupportStatus == Allocated && SimFun.CHV1.CurrentStatus == Activated)
	{
		bPin1Active = TRUE;
	}	

	if(SimFun.FDN.SupportStatus == Allocated/* && SimFun.CurrentStatus == Activated*/)
	{
		bFDAllowded = TRUE;
	}


	free(temp);
	temp = NULL;
}
/********************************************************************
* Function   GetSecurity_Open  
* Purpose    

* Params     
* Return     0 close,1open
* Remarks      
**********************************************************************/
BOOL    GetSecurity_Open(void)
{
    BOOL bres = FALSE;
    bres = FS_GetPrivateProfileInt(SN_PWS,KN_PWS_OPEN,bres,CODEFILENAME);

    return bres;
}
/********************************************************************
* Function   SetSecurity_Open  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
static  BOOL   SetSecurity_Open(BOOL bAuto)
{
    FS_WritePrivateProfileInt(SN_PWS,KN_PWS_OPEN,bAuto,CODEFILENAME);
    return TRUE;
}
/********************************************************************
* Function   GetSecurity_Open  
* Purpose    
* Params     
* Return     0 close,1open
* Remarks      
**********************************************************************/
BOOL    GetPhoneLock_Open(void)
{
    BOOL bres = FALSE;
    bres = FS_GetPrivateProfileInt(SN_PWS,KN_PMS_PHONELOCK,bres,CODEFILENAME);

    return bres;
}
/********************************************************************
* Function   SetSecurity_Open  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
BOOL    SetPhone_Open(BOOL bAuto)
{
    FS_WritePrivateProfileInt(SN_PWS,KN_PMS_PHONELOCK,bAuto,CODEFILENAME);
    return TRUE;
}
/********************************************************************
* Function   GetSecurity_code  
* Purpose    
* Params     cpassword:length is PWSEDITMAXLEN + 1
* Return     
* Remarks      
**********************************************************************/
void    GetSecurity_code(char * cpassword)
{
    char    cPws[EDITMAXLEN + 1] = "";
    FS_GetPrivateProfileString(SN_PWS,KN_PWS_PHONE, PASSWORD_DEFAULT,
                                          cPws, sizeof(cPws), CODEFILENAME );
	strcpy(cpassword,cPws);
}
/********************************************************************
* Function   SetSecurity_code  
* Purpose    
* Params     cpassword:length is PWSEDITMAXLEN + 1
* Return     
* Remarks      
**********************************************************************/
BOOL    SetSecurity_code(char * cpassword)
{
    FS_WritePrivateProfileString(SN_PWS, KN_PWS_PHONE,cpassword,CODEFILENAME);
    return TRUE;
}
/********************************************************************
* Function   SetLockCodePeriod  
* Purpose    
* Params     cpassword:length is PWSEDITMAXLEN + 1
* Return     
* Remarks      
**********************************************************************/
void   SetLockCodePeriod(char* pLockPeriod)
{
	 FS_WritePrivateProfileString(SN_PWS, KN_PWS_LOCKPERIOD,pLockPeriod,CODEFILENAME);

}
/********************************************************************
* Function   GetLockCodePeriod  
* Purpose    
* Params     cpassword:length is PWSEDITMAXLEN + 1
* Return     
* Remarks      
**********************************************************************/
BOOL GetLockCodePeriod(char* pLockPeriodTime)
{
    FS_GetPrivateProfileString(SN_PWS,KN_PWS_LOCKPERIOD,LOCKPEROID,
		pLockPeriodTime, 20,CODEFILENAME );
    return TRUE;
}
/********************************************************************
* Function   GetPS_Open  
* Purpose    
* Params     
* Return     0 close,1open
* Remarks      
**********************************************************************/
BOOL    GetPS_Open(void)
{
    BOOL bres = FALSE;
    bres = FS_GetPrivateProfileInt(SN_PWS,KN_PMS_PS,bres,CODEFILENAME);
	
    return bres;
}
/********************************************************************
* Function   SetPS_Open  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
static  BOOL    SetPS_Open(BOOL bAuto)
{
    FS_WritePrivateProfileInt(SN_PWS,KN_PMS_PS,bAuto,CODEFILENAME);
    return TRUE;
}
/********************************************************************
* Function   GetPSCode  
* Purpose    
* Params     
* Return     0 close,1open
* Remarks      
**********************************************************************/
BOOL GetPSCode(char* pPSCode)
{
	FS_GetPrivateProfileString(SN_PWS,KN_PWS_PSCODE,LOCKPEROID,
		pPSCode, 30,CODEFILENAME );

    return TRUE;

}
/********************************************************************
* Function   SetPSCode  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
void   SetPSCode(char* pPSCode)
{
    FS_WritePrivateProfileString(SN_PWS, KN_PWS_PSCODE,pPSCode,CODEFILENAME);
}
/********************************************************************
* Function   GetPSData  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
BOOL GetPSData(char* pPSCode)
{
	FS_GetPrivateProfileString(SN_PWS,KN_PWS_PSDATA,LOCKPEROID,
		pPSCode, 30,CODEFILENAME );

    return TRUE;

}
/********************************************************************
* Function   SetPSData  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
void   SetPSData(char* pPSCode)
{
    FS_WritePrivateProfileString(SN_PWS, KN_PWS_PSDATA,pPSCode,CODEFILENAME);
}

/********************************************************************
* Function   GetNetSimLockEnableStatus  
* Purpose    
* Params     
* Return     0 disable,1 enable
* Remarks      
**********************************************************************/
BOOL    GetNetSimLockEnableStatus(void)
{
    BOOL bres = FALSE;
    bres = FS_GetPrivateProfileInt(SN_PWS,KN_PWS_NCKENABLE,bres,CODEFILENAME);
	
    return bres;
}
/********************************************************************
* Function   SetNetSimLockEnableStatus  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
BOOL    SetNetSimLockEnableStatus(BOOL bAuto)
{
    FS_WritePrivateProfileInt(SN_PWS,KN_PWS_NCKENABLE,bAuto,CODEFILENAME);
    return TRUE;
}
/********************************************************************
* Function   GetSPSimLockEnableStatus  
* Purpose    
* Params     
* Return     0 disable,1 enable
* Remarks      
**********************************************************************/
BOOL    GetSPSimLockEnableStatus(void)
{
    BOOL bres = FALSE;
    bres = FS_GetPrivateProfileInt(SN_PWS,KN_PWS_SPCKENABLE,bres,CODEFILENAME);
	
    return bres;
}
/********************************************************************
* Function   SetNetSimLockEnableStatus  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
BOOL    SetSPSimLockEnableStatus(BOOL bAuto)
{
    FS_WritePrivateProfileInt(SN_PWS,KN_PWS_SPCKENABLE,bAuto,CODEFILENAME);
    return TRUE;
}
/********************************************************************
* Function   GetNCKRemains 
* Purpose    
* Params     
* Return     0 close,1open
* Remarks      
**********************************************************************/
BOOL GetNCKRemains(char* strRemains)
{
	FS_GetPrivateProfileString(SN_PWS,KN_NCK_REMAIN,"10",
		strRemains, 3,CODEFILENAME );

    return TRUE;
}
/********************************************************************
* Function   SetNCKRemains  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
void   SetNCKRemains(char* strRemains)
{
    FS_WritePrivateProfileString(SN_PWS, KN_NCK_REMAIN,strRemains,CODEFILENAME);
}
/********************************************************************
* Function   GetSPCKRemains 
* Purpose    
* Params     
* Return     0 close,1open
* Remarks      
**********************************************************************/
BOOL GetSPCKRemains(char* strRemains)
{
	FS_GetPrivateProfileString(SN_PWS,KN_SPCK_REMAIN,LOCKPEROID,
		strRemains, 3,CODEFILENAME );

    return TRUE;
}
/********************************************************************
* Function   SetSPCKRemains  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
void   SetSPCKRemains(char* strRemains)
{
    FS_WritePrivateProfileString(SN_PWS, KN_SPCK_REMAIN,strRemains,CODEFILENAME);
}

/********************************************************************
* Function   GetNetSimLock_Open  
* Purpose    
* Params     
* Return     0 close,1open
* Remarks      
**********************************************************************/
BOOL    GetNETSimLock_Open(void)
{
    BOOL bres = FALSE;
    bres = FS_GetPrivateProfileInt(SN_PWS,KN_PWS_NETSIMLOCK,bres,CODEFILENAME);
	
    return bres;
}
extern void GetIdleOprName(char * strOprName);
void SetOprName(char * strOprName)
{
	if(strOprName[0] != 0)
		FS_WritePrivateProfileString(SN_PWS, KN_SL_OPRNAME,strOprName,CODEFILENAME);
}
void GetOprName(char * strOprName)
{
	FS_GetPrivateProfileString(SN_PWS,KN_SL_OPRNAME,LOCKPEROID,
		strOprName, 20,CODEFILENAME );
}
/********************************************************************
* Function   SetNETSimLock_Open  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
BOOL    SetNETSimLock_Open(BOOL bAuto)
{
//	char strOprName[20];

    FS_WritePrivateProfileInt(SN_PWS,KN_PWS_NETSIMLOCK,bAuto,CODEFILENAME);
	
//	strOprName[0] =0;
//	GetIdleOprName(strOprName);
//	SetOprName(strOprName);
    return TRUE;
}
/********************************************************************
* Function   GetNetSimLockBlock  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
BOOL GetNetSimLockBlock(void)
{
	BOOL bres = FALSE;
    bres = FS_GetPrivateProfileInt(SN_PWS,KN_NCK_BLOCK,bres,CODEFILENAME);
	
    return bres;
}
/********************************************************************
* Function   SetNetSimLockBlock  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
extern HWND GetDeskWnd(void);
void SetNetSimLockBlock(BOOL bStatus)
{
	HWND hWnd = NULL;

//	bNetSimLockBlock = bStatus;

	hWnd = GetDeskWnd();

	if(hWnd == NULL)
		return;

	FS_WritePrivateProfileInt(SN_PWS,KN_NCK_BLOCK,bStatus,CODEFILENAME);

	if(bStatus)
		ME_SetOpratorMode( hWnd, WM_USER+607, 2, NULL);
}

/********************************************************************
* Function   GetSPSimLockBlock  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
//static BOOL bSPSimLockBlock;
BOOL GetSPSimLockBlock(void)
{
	//return bSPSimLockBlock;
	BOOL bres = FALSE;
    bres = FS_GetPrivateProfileInt(SN_PWS,KN_SPCK_BLOCK,bres,CODEFILENAME);
	
    return bres;
}
/********************************************************************
* Function   SetSPSimLockBlock  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
void SetSPSimLockBlock(BOOL bStatus)
{
	HWND hWnd = NULL;

	hWnd = GetDeskWnd();

	if(hWnd == NULL)
		return;

	FS_WritePrivateProfileInt(SN_PWS,KN_SPCK_BLOCK,bStatus,CODEFILENAME);


	if(bStatus)
		ME_SetOpratorMode( hWnd, WM_USER+607, 2, NULL);//forbid network operation
}

/********************************************************************
* Function   ReadNetSimLock  
* Purpose    
* Params     
* Return     0 close,1open
* Remarks      
**********************************************************************/
#define  NetSimLock    "/mnt/flash/simlock.ini"
typedef struct tagNETSLC
{
	char strMCC[3 +1];
	char strMNC[2 + 1];
}NETSLC, *PNETSLC;

int ReadNetSimLock(PNSLC pNetSimlockCode)
{
    int handle = -1, iSize = 0,nRead = 0, num = 0;
	PNSLC pDataCode = NULL, pHead = NULL;
	PNETSLC pCode =NULL;

	handle = open(NetSimLock,O_RDONLY);
	if(handle == -1)
		return -1;

	pDataCode = malloc(sizeof(NSLC));
	
	if(pDataCode == NULL)
	{
		close(handle);
		return -1;
	}
	
	memset(pDataCode,0, sizeof(NSLC));

	pCode  = malloc(sizeof(NETSLC));

	if(pCode == NULL)
	{
		free(pDataCode);
		close(handle);
		return -1;
	}

	memset(pCode, 0, sizeof(NETSLC));

	iSize = sizeof(NETSLC);
	nRead = read(handle,pCode,iSize);

	while(nRead == iSize)
	{
		num++;
		strcpy(pDataCode->strMCC, pCode->strMCC);
		strcpy(pDataCode->strMNC, pCode->strMNC);
		pDataCode->pNext = NULL;

		if(pHead == NULL)
			pHead =  pDataCode;
		else
		{
			PNSLC p = NULL;

			p = pHead;
			while(p->pNext)
			{
				p = p->pNext;
			}
			p->pNext = pDataCode;
		}
		pDataCode = malloc(sizeof(NSLC));
		
		if(pDataCode == NULL)
		{
			PNSLC p = NULL;
			while(pHead)
			{
				p = pHead;
				pHead = pHead->pNext;
				free(p);
			}
			free(pCode);
			close(handle);
		}
		memset(pDataCode,0, sizeof(NSLC));
		memset(pCode, 0, sizeof(NETSLC));
		nRead = read(handle,pCode,iSize);
		if(nRead != iSize)
		{
			free(pDataCode);
			pDataCode = NULL;
		}
	}

	if(pNetSimlockCode == NULL)
	{
		PNSLC p = NULL;
		while(pHead)
		{
			p = pHead;
			pHead = pHead->pNext;
			free(p);
			p = NULL;
		}
		free(pCode);
		close(handle);
		return num;//return node number
	}
	else
	{
		PNSLC p = NULL;

		p = pHead;
		while(p)
		{
			memcpy(pNetSimlockCode, p, sizeof(NSLC));
			//pNetSimlockCode = pNetSimlockCode->pNext;
			pNetSimlockCode++;
			p = p->pNext;
		}

		p = NULL;
		while(pHead)
		{
			p = pHead;
			pHead = pHead->pNext;
			free(p);
		}
		free(pCode);
		close(handle);
		return 1;
	}
}
#define NetSimLockData "/mnt/flash/simlockdata.ini"
int ReadNetSimLockData(PNSLC pNetSimlockCode)
{
    int handle = -1, iSize = 0,nRead = 0, num = 0;
	PNSLC pDataCode = NULL, pHead = NULL;
	PNETSLC pCode =NULL;

	handle = open(NetSimLockData,O_RDONLY);
	if(handle == -1)
		return -1;

	pDataCode = malloc(sizeof(NSLC));
	
	if(pDataCode == NULL)
	{
		close(handle);
		return -1;
	}
	
	memset(pDataCode,0, sizeof(NSLC));

	pCode  = malloc(sizeof(NETSLC));

	if(pCode == NULL)
	{
		free(pDataCode);
		close(handle);
		return -1;
	}

	memset(pCode, 0, sizeof(NETSLC));

	iSize = sizeof(NETSLC);
	nRead = read(handle,pCode,iSize);

	while(nRead == iSize)
	{
		num++;
		strcpy(pDataCode->strMCC, pCode->strMCC);
		strcpy(pDataCode->strMNC, pCode->strMNC);
		pDataCode->pNext = NULL;

		if(pHead == NULL)
			pHead =  pDataCode;
		else
		{
			PNSLC p = NULL;

			p = pHead;
			while(p->pNext)
			{
				p = p->pNext;
			}
			p->pNext = pDataCode;
		}
		pDataCode = malloc(sizeof(NSLC));
		
		if(pDataCode == NULL)
		{
			PNSLC p = NULL;
			while(pHead)
			{
				p = pHead;
				pHead = pHead->pNext;
				free(p);
			}
			free(pCode);
			close(handle);
			return -1;
		}
		memset(pDataCode,0, sizeof(NSLC));
		memset(pCode, 0, sizeof(NETSLC));
		nRead = read(handle,pCode,iSize);
	}

	if(pNetSimlockCode == NULL)
	{
		PNSLC p = NULL;
		while(pHead)
		{
			p = pHead;
			pHead = pHead->pNext;
			free(p);
			p = NULL;
		}
		free(pCode);
		close(handle);
		return num;
	}
	else
	{
		PNSLC p = NULL;

		p = pHead;
		while(p)
		{
			memcpy(pNetSimlockCode, p, sizeof(NSLC));
		//	pNetSimlockCode = pNetSimlockCode->pNext;
			pNetSimlockCode++;
			p = p->pNext;
		}

		p = NULL;
		while(pHead)
		{
			p = pHead;
			pHead = pHead->pNext;
			free(p);
		}
		free(pCode);
		close(handle);
		return 1;
	}
}
/********************************************************************
* Function   ClearNetSimLockData  
* Purpose    sim str
* Params     
* Return     
* Remarks      
**********************************************************************/
BOOL ClearNetSimLockData(void)
{
	int handle = -1;

	handle = open(NetSimLockData, O_RDWR);
	if(handle== -1)
		return FALSE;

	close(handle);
	remove(NetSimLockData);
	return TRUE;
}
/********************************************************************
* Function   ClearNetSimLockCode  
* Purpose    sim str
* Params     
* Return     
* Remarks      
**********************************************************************/
BOOL ClearNetSimLockCode(void)
{
	int handle = -1;

	handle = open(NetSimLock, O_RDWR);
	if(handle== -1)
		return FALSE;

	close(handle);
	remove(NetSimLock);
	return TRUE;
}
/********************************************************************
* Function   SetNetSimLockData  
* Purpose    sim str
* Params     
* Return     
* Remarks      
**********************************************************************/
BOOL   SetNetSimLockData(PNSLC pNetSimlockCode, int n)
{
	int handle = -1;
	PNSLC pHead = NULL;
	PNETSLC pCode = NULL;
	
	handle = open(NetSimLockData, O_RDWR);
	if(handle == -1)
		handle = open(NetSimLockData, O_RDWR|O_CREAT, S_IRWXU);
	if(handle == -1)
		return FALSE;

	pCode = malloc(sizeof(NETSLC));

	if(pCode == NULL)
	{
		close(handle);
		return FALSE;
	}

	pHead = pNetSimlockCode;
	while(pHead)
	{
		memset(pCode, 0, sizeof(NETSLC));
		strcpy(pCode->strMCC, pHead->strMCC);
		strcpy(pCode->strMNC, pHead->strMNC);
		write(handle, pCode, sizeof(NETSLC));
		pHead = pHead->pNext;
	}

	free(pCode);
	close(handle);
    return TRUE;
}
/********************************************************************
* Function   SetNetSimLockCode  
* Purpose    sim str
* Params     
* Return     
* Remarks      
**********************************************************************/
BOOL   SetNetSimLockCode(PNSLC pNetSimlockCode, int n)
{
	int handle = -1;
	PNSLC pHead = NULL;
	PNETSLC pCode = NULL;
	
	handle = open(NetSimLock, O_RDWR);
	if(handle == -1)
		handle = open(NetSimLock, O_RDWR|O_CREAT, S_IRWXU);
	if(handle == -1)
		return FALSE;

	pCode = malloc(sizeof(NETSLC));

	if(pCode == NULL)
	{
		close(handle);
		return FALSE;
	}

	pHead = pNetSimlockCode;
	while(pHead)
	{
		memset(pCode, 0, sizeof(NETSLC));
		strcpy(pCode->strMCC, pHead->strMCC);
		strcpy(pCode->strMNC, pHead->strMNC);
		write(handle, pCode, sizeof(NETSLC));
		pHead = pHead->pNext;
	}

	free(pCode);
	close(handle);
    return TRUE;
}
/********************************************************************
* Function   GetNCK 
* Purpose    
* Params     
* Return     0 close,1open
* Remarks      
**********************************************************************/
BOOL GetNCK(char* pNck)
{
	FS_GetPrivateProfileString(SN_PWS,KN_PWS_NCK,LOCKPEROID,
		pNck, 8,CODEFILENAME );

    return TRUE;
}
/********************************************************************
* Function   SetNCK  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
void   SetNCK(char* pNck)
{
    FS_WritePrivateProfileString(SN_PWS, KN_PWS_NCK,pNck,CODEFILENAME);
}
/********************************************************************
* Function   NCKValidate  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
BOOL NCKValidate(char* strNck)
{
	char strCode[12];
//	BYTE byteCode[4];
  //  int  t1,t2;

	if(NULL == strNck)
		return FALSE;

	strCode[0] =0 ;
	GetNCK( strCode);
//	t1 = atoi(strCode[1]);
//	t2 = atoi(strCode[0]);
//	byteCode = (BYTE)t1 | ((BYTE)t2 <<4);


	if(stricmp(strNck, strCode) != NULL)
		return FALSE;

	return TRUE;
}
/********************************************************************
* Function   GetSPSimLock_Open  
* Purpose    
* Params     
* Return     0 close,1open
* Remarks      
**********************************************************************/
BOOL    GetSPSimLock_Open(void)
{
    BOOL bres = FALSE;
    bres = FS_GetPrivateProfileInt(SN_PWS,KN_PWS_SPSIMLOCK,bres,CODEFILENAME);
	
    return bres;
}
/********************************************************************
* Function   SetSPSimLock_Open  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
BOOL    SetSPSimLock_Open(BOOL bAuto)
{
	char strOprName[20];
    FS_WritePrivateProfileInt(SN_PWS,KN_PWS_SPSIMLOCK,bAuto,CODEFILENAME);

	strOprName[0] =0;
	GetIdleOprName(strOprName);
	SetOprName(strOprName);

    return TRUE;
}
/********************************************************************
* Function   ReadSPSimLock  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
#define  SPSimLock    "/mnt/flash/spsimlock.ini"
int ReadSPSimLock(PSPSIMLOCKCODE pSPLC)
{
	int handle = -1, nRead = 0,nSize = 0;
	PSPSIMLOCKCODE pCode = NULL;

	if(pSPLC == NULL)
		return -1;

	handle = open(SPSimLock,O_RDONLY);
	if(handle == -1)
		return -1;

	nSize =sizeof(SPSIMLOCKCODE);
	memset(pSPLC, 0, sizeof(SPSIMLOCKCODE));
	nRead = read(handle, pSPLC, sizeof(SPSIMLOCKCODE));

	if(nRead != nSize)
	{
		close(handle);
		return -1;
	}
	close(handle);

	return 1;
}
/********************************************************************
* Function   ClearSPSimLockCode  
* Purpose    sim str
* Params     
* Return     
* Remarks      
**********************************************************************/
BOOL ClearSPSimLockCode(void)
{
	int handle = -1;

	handle = open(SPSimLock, O_RDWR);
	if(handle== -1)
		return FALSE;

	close(handle);
	remove(SPSimLock);
	return TRUE;
}
/********************************************************************
* Function   IsSimLockContradict  
* Purpose    sim str
* Params     
* Return     TRUE : contradict  FALSH : not contradict
* Remarks      
**********************************************************************/
BOOL IsSimLockContradict(int nType, char* strMCC, char*strMNC)
{
	PSPSIMLOCKCODE pSPLC = NULL;
	PNSLC          pNTLC = NULL, p= NULL;
	int            num = 0;

	if(strMCC == NULL || strMNC == NULL)
		return FALSE;
	
	if(nType == NETWORK_SC)
	{
		if(GetSPSimLock_Open())
		{
			pSPLC = malloc(sizeof(SPSIMLOCKCODE));
			if(pSPLC == NULL)
				return FALSE;
			
			if(ReadSPSimLock(pSPLC) == 1)
			{
				if(stricmp(strMCC, pSPLC->strMCC) != NULL 
					|| stricmp(strMNC, pSPLC->strMNC) != NULL)
				{
					free(pSPLC);
					pSPLC = NULL;
					return TRUE;
				}
				else
				{
					free(pSPLC);
					pSPLC = NULL;
					return FALSE;
				}
			}
			else
			{
				free(pSPLC);
				pSPLC = NULL;
				return FALSE;
			}
		}
		else
			return FALSE;

	}
	else if(nType == SP_SC)
	{
		if(GetNETSimLock_Open())
		{
			num = ReadNetSimLock(NULL);

			if(num == -1)
				return FALSE;
			else if(num == 1)
			{
				pNTLC = malloc(sizeof(NSLC));
				if(pNTLC == NULL)
					return FALSE;
				ReadNetSimLock(pNTLC);
				if(stricmp(pNTLC->strMCC, strMCC) != NULL
					|| stricmp(pNTLC->strMNC, strMNC) != NULL)
				{
					free(pNTLC);
					pNTLC = NULL;
					return TRUE;
				}
				else
				{
					free(pNTLC);
					pNTLC = NULL;
					return FALSE;
				}
			}
			else if(num > 1)
			{
				pNTLC = malloc(sizeof(NSLC) * num);
				if(pNTLC == NULL)
					return FALSE;

				ReadNetSimLock(pNTLC);
				p = pNTLC;
				while(p)
				{
					if(stricmp(p->strMCC, strMCC) != NULL
					|| stricmp(p->strMNC, strMNC) != NULL)
					{
						free(pNTLC);
						pNTLC = NULL;
						return TRUE;
					}
					else
					{
						p++;
					}
				}
				free(pNTLC);
				pNTLC = NULL;
				return FALSE;

			}
		}
		else
			return FALSE;

	}

	return FALSE;
}
/********************************************************************
* Function   SetSPSimLockCode  
* Purpose    sim str
* Params     
* Return     
* Remarks      
**********************************************************************/
BOOL   SetSPSimLockCode(PSPSIMLOCKCODE pSPSimlockCode)
{
	int handle = -1, nWrite = 0, nSize = 0;
	
	handle = open(SPSimLock, O_RDWR);
	if(handle == -1)
		handle = open(SPSimLock, O_RDWR|O_CREAT, S_IRWXU);
	if(handle == -1)
		return FALSE;
		
	nWrite = write(handle, pSPSimlockCode, sizeof(SPSIMLOCKCODE));
	if(nWrite != sizeof(SPSIMLOCKCODE))
	{
		close(handle);
	    return FALSE;
	}
	close(handle);
    return TRUE;
}
/********************************************************************
* Function   ReadSPSimLock  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
#define SPSimLockCode "/mnt/flash/spsimlockcode.ini"
int ReadSPSimLockDate(PSPSIMLOCKCODE pSPLC)
{
	int handle = -1, nRead = 0,nSize = 0;
	PSPSIMLOCKCODE pCode = NULL;

	if(pSPLC == NULL)
		return -1;

	handle = open(SPSimLockCode,O_RDONLY);
	if(handle == -1)
		return -1;

	nSize =sizeof(SPSIMLOCKCODE);
	memset(pSPLC, 0, sizeof(SPSIMLOCKCODE));
	nRead = read(handle, pSPLC, sizeof(SPSIMLOCKCODE));

	if(nRead != nSize)
	{
		close(handle);
		return -1;
	}
	close(handle);

	return 1;
}
/********************************************************************
* Function   ClearSPSimLockData  
* Purpose    sim str
* Params     
* Return     
* Remarks      
**********************************************************************/
BOOL ClearSPSimLockData(void)
{
	int handle = -1;

	handle = open(SPSimLockCode, O_RDWR);
	if(handle== -1)
		return FALSE;

	close(handle);
	remove(SPSimLockCode);
	return TRUE;
}
/********************************************************************
* Function   SetSPSimLockData  
* Purpose    sim str
* Params     
* Return     
* Remarks      
**********************************************************************/
BOOL   SetSPSimLockData(PSPSIMLOCKCODE pSPSimlockCode)
{
	int handle = -1, nWrite = 0, nSize = 0;
	
	handle = open(SPSimLockCode, O_RDWR);
	if(handle == -1)
		handle = open(SPSimLockCode, O_RDWR|O_CREAT, S_IRWXU);
	if(handle == -1)
		return FALSE;
		
	nWrite = write(handle, pSPSimlockCode, sizeof(SPSIMLOCKCODE));
	if(nWrite != sizeof(SPSIMLOCKCODE))
	{
		close(handle);
	    return FALSE;
	}
	close(handle);
    return TRUE;
}
/********************************************************************
* Function   GetSPCK 
* Purpose    
* Params     
* Return     0 close,1open
* Remarks      
**********************************************************************/
BOOL GetSPCK(char* pSpck)
{
	FS_GetPrivateProfileString(SN_PWS,KN_PWS_SPCK,LOCKPEROID,
		pSpck, 8,CODEFILENAME );

    return TRUE;
}
/********************************************************************
* Function   SetSPCK  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
void   SetSPCK(char* pSpck)
{
    FS_WritePrivateProfileString(SN_PWS, KN_PWS_SPCK,pSpck,CODEFILENAME);
}
/********************************************************************
* Function   SPCKValidate  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
BOOL SPCKValidate(char* strNck)
{
	char strCode[12];

	strCode[0] = 0;
	
	if(strNck == NULL)
		return FALSE;

	GetSPCK(strCode);

	if(strCode[0] == 0)
		return TRUE;

	if(stricmp(strCode, strNck) != NULL)
		return FALSE;

	return TRUE;
}
/********************************************************************
* Function   CreateSimLockWnd  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
static BOOL  CreateSimLockWnd(HWND hParentwnd,UINT msg )
{
    WNDCLASS wc;
	HWND hSimLockWnd = NULL;
	RECT rect;
	
 	wc.style         = CS_OWNDC;
	wc.lpfnWndProc   = SimlockWndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = NULL;
	wc.hIcon         = NULL;
	wc.hCursor       = NULL;
	wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = "simlockclass";
	
	if (!RegisterClass(&wc))
	{
		return FALSE;
	}
	if ( hSimLockWnd )
	{
		ShowWindow ( hSimLockWnd, SW_SHOW );
		SetFocus ( hSimLockWnd );	
	}
	else
	{   
		GetClientRect(hFrameWin, &rect);

		hSimLockWnd = CreateWindow(
			"simlockclass", 
			NULL,
			WS_CHILD|WS_VISIBLE, 
			rect.left,rect.top,rect.right-rect.left,rect.bottom-rect.top,
			hFrameWin, 
			NULL,
			NULL, 
			NULL
			);
		
		if (NULL == hSimLockWnd)
		{
			UnregisterClass("simlockclass",NULL);
			return FALSE;
		}
		ShowWindow(hSimLockWnd, SW_SHOW);
		UpdateWindow(hSimLockWnd);
		
	}
	callsimlockmsg = msg;
	hSimlockParentWnd = hParentwnd;

	SendMessage(hFrameWin, PWM_SETBUTTONTEXT, (WPARAM)0, (LPARAM)ML(IDS_CANCEL)); 
	
	SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
	
	SetWindowText(hFrameWin, ML("Lock if SIM changed"));
	
	SetFocus(hSimlockFocusWnd);

	return TRUE;
}

static LRESULT SimlockWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
	
	LRESULT lResult;
	HDC     hdc;
	static HWND hList;
	
    lResult = (LRESULT)TRUE;
    switch ( wMsgCmd )
    {
    case WM_CREATE :
		SimLock_OnCreate(hWnd, &hList);
        break;
		
	case PWM_SHOWWINDOW:
		{
			SendMessage(hFrameWin, PWM_SETBUTTONTEXT, (WPARAM)1, (LPARAM)ML(""));

			SendMessage(hFrameWin, PWM_SETBUTTONTEXT, (WPARAM)0, (LPARAM)ML(IDS_CANCEL));
			
			SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
			
			SetWindowText(hFrameWin, ML("Lock if SIM changed"));

			SetFocus(hSimlockFocusWnd);
		}
		break;
  
    case WM_CLOSE:
		SendMessage(hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hWnd,0);
        SendMessage ( hWnd, WM_COMMAND, (WPARAM)IDC_SIMLOCK_QUIT, 0 ); 
        break;

    case WM_DESTROY :
		SimLock_OnDestory();
	    break;
		
    case WM_KEYDOWN:
		SimLock_OnKeydown(hWnd, &hList, wMsgCmd, wParam, lParam);
	    break;

	case WM_PAINT:
		hdc = BeginPaint( hWnd, NULL);
		EndPaint(hWnd, NULL);	
		break;

    case WM_COMMAND:
		SimLock_OnCommand(hWnd, wParam);
		break;

	default :
		lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
		break;
	}
	return lResult;
}
/********************************************************************
* Function   SimLock_OnCreate  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
static BOOL	SimLock_OnCreate(HWND hWnd, HWND *hList)
{
	int i = 0, nFocus = 0;
	HDC hdc;
	char* seletitem[]=
	{
		"On",
		"Off"
	};
	

	*hList = CreateWindow("LISTBOX","", 
		WS_VISIBLE | WS_CHILD | WS_TABSTOP | LBS_BITMAP |WS_VSCROLL,
		0, 0 , PLX_WIN_WIDTH, PLX_WIN_HIGH - 46,
		hWnd, (HMENU)IDC_SIMLOCK_LIST, NULL, NULL );
	
	if(*hList == NULL)
		return FALSE;
	
	while(i< 2)
	{
		SendMessage(*hList,LB_ADDSTRING,NULL,(LPARAM)ML(seletitem[i]));
		i++;
	}
	
	if(GetPS_Open())
		SendMessage(*hList,LB_SETCURSEL,0,0);
	else
	{
		SendMessage(*hList,LB_SETCURSEL,1,0);
		nFocus = 1;
	}
	
	hdc         = GetDC(hWnd);
	hIconNormal = LoadImageEx(hdc,NULL,ICONORMAL,IMAGE_BITMAP,17,16,LR_LOADFROMFILE);
	hIconSel = LoadImageEx(hdc,NULL,ICONSEL,IMAGE_BITMAP,17,16,LR_LOADFROMFILE);
	ReleaseDC(hWnd,hdc);

	Load_Icon_SetupList(*hList,hIconNormal,hIconSel, 2,nFocus);

	hSimlockFocusWnd = *hList;
	SetFocus(hSimlockFocusWnd);
	return TRUE;
}
/********************************************************************
* Function   SimLock_OnDestory  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
static void 	SimLock_OnDestory(void)
{
	hSimlockFocusWnd = NULL;
	if(hIconNormal != NULL)
		DeleteObject(hIconNormal);
	if(hIconSel != NULL)
		DeleteObject(hIconSel);
	UnregisterClass("simlockclass",NULL);
}
/********************************************************************
* Function   SimLock_OnKeydown  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
static void  SimLock_OnKeydown(HWND hWnd, HWND *hList, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
	switch(LOWORD(wParam))
	{
	case VK_F5:
		{
			int nFocusItem;
			BOOL bPsLock = FALSE;
			
			nFocusItem = SendMessage(*hList, LB_GETCURSEL, 0, 0);
			
			if(nFocusItem == 0)
				bPsLock = TRUE;
			
			SetPS_Open(bPsLock);

			if(bPsLock)
				SendMessage(GetDlgItem(hSimlockParentWnd, IDC_SIMLOCK), WM_SETVALUE, 0, (LPARAM)ML(IDS_ON));
			else
				SendMessage(GetDlgItem(hSimlockParentWnd, IDC_SIMLOCK), WM_SETVALUE, 0, (LPARAM)ML(IDS_OFF));
		
			if(bPsLock)
			{
				char strPsCode[ME_MBID_LEN];
				
				memset(strPsCode, 0, ME_MBID_LEN);
				
				GetPSCode(strPsCode);
				
				if(strPsCode[0] == 0 || strPsCode[0] == '0')
				{
					ME_GetSubscriberId(hSimlockParentWnd, GETPSCODE);
					break;
				}
			}
			PostMessage(hWnd, WM_CLOSE, 0, 0);
		}
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
* Function   SimLock_OnCommand  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
static void	SimLock_OnCommand(HWND hWnd, WPARAM wParam)
{
	switch( LOWORD( wParam ))
	{
		
	case IDC_SIMLOCK_LIST:
		if(HIWORD(wParam) == BN_CLICKED)
		{
			
		}
		break;
		
	case IDC_SIMLOCK_QUIT:
		DestroyWindow(hWnd);
		break;
	}
}
