 /***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : 
 *
 * Purpose  : modify code(include phone code,PIN1,PIN2,call barring code)
 *            
\**************************************************************************/

#include    "ModifyPassword.h"

BOOL CallModifyCodeWindow(HWND hwndCall,int itype,char * caption, UINT CallMsg)
{
    HWND hwnd;
    WNDCLASS wc;
	RECT rect;

    memset(cTitle,0x00,sizeof(cTitle));
    strcpy(cTitle,caption);
    iMP_Type = itype;
	MsgCall = CallMsg;
	hCallwnd  = hwndCall;

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

	GetClientRect(hFrameWin, &rect);
    hwnd = CreateWindow(pClassName,NULL, 
        WS_CHILD|WS_VISIBLE, 
        rect.left,rect.top,rect.right-rect.left,rect.bottom-rect.top,
		hFrameWin, NULL, NULL, NULL);

    if (!hwnd)
    {
        UnregisterClass(pClassName,NULL);
        return FALSE;
    }
    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd); 

	SendMessage(hFrameWin, PWM_SETBUTTONTEXT, (WPARAM)1, (LPARAM)ML("")); 
	
	SendMessage(hFrameWin, PWM_SETBUTTONTEXT, (WPARAM)0, (LPARAM)ML(IDS_CANCEL)); 

	SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"");

	SetWindowText(hFrameWin, ML(cTitle));

    return TRUE;
}
static LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
    static      HWND    hEditOld;
    static      HWND    hEditNew;
    static      HWND    hEditAffirm;
                LRESULT lResult;
                int     nEnd = 0,nPos = 0;
                HWND    hCurWin = 0;
                HDC     hdc;    
               

    lResult = (LRESULT)TRUE;
    switch ( wMsgCmd )
    {
    case WM_CREATE :

        CreateControl(hWnd,&hEditOld,&hEditNew,&hEditAffirm);
        hModifyCodeFocusWnd = hEditOld;
		SetFocus(hModifyCodeFocusWnd);
        break;

	case PWM_SHOWWINDOW:
		{
			int strOldCodeLen = 0, strNewCodeLen = 0, strRepeatCodeLen = 0;
			
			strOldCodeLen = GetWindowTextLength(hEditOld);
			strNewCodeLen = GetWindowTextLength(hEditNew);
			strRepeatCodeLen = GetWindowTextLength(hEditAffirm);
			
			if(strOldCodeLen >= EDITMINLEN && strNewCodeLen >= EDITMINLEN
				&& strRepeatCodeLen >= EDITMINLEN)
			{
				SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 1, (LPARAM)ML(IDS_OK));
			}
			else
			{
				SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 1, (LPARAM)ML(""));
			}

			SendMessage(hFrameWin, PWM_SETBUTTONTEXT, (WPARAM)0, (LPARAM)ML(IDS_CANCEL));
			SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
			
			SetWindowText(hFrameWin, ML(cTitle));
			SetFocus(hModifyCodeFocusWnd);
		}
		break;

    case WM_CLOSE:
		SendMessage(hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hWnd,0);
		DestroyWindow(hWnd);
        break;

    case WM_DESTROY :
        UnregisterClass(pClassName,NULL);
        break;

    case WM_PAINT :
        hdc = BeginPaint( hWnd, NULL);  
        EndPaint(hWnd, NULL);
        break;

    case WM_KEYDOWN:
        switch(LOWORD(wParam))
        {
        case VK_F10:
			{
				int strCodeLen = 0;
				
				strCodeLen = GetWindowTextLength(GetFocus());
			
				if(strCodeLen == 0)
					bModifyCode = FALSE;
				SendMessage(hWnd,WM_COMMAND,IDC_BUTTON_QUIT,0);
			}
			break;
        case VK_RETURN:
			{
				int strOldCodeLen = 0, strNewCodeLen = 0, strRepeatCodeLen = 0;
				
				strOldCodeLen = GetWindowTextLength(hEditOld);
				strNewCodeLen = GetWindowTextLength(hEditNew);
				strRepeatCodeLen = GetWindowTextLength(hEditAffirm);
				
				if(strOldCodeLen >= EDITMINLEN && strNewCodeLen >= EDITMINLEN
					&& strRepeatCodeLen >= EDITMINLEN)	
					SendMessage(hWnd,WM_COMMAND,IDC_BUTTON_SET,0);
			}
            break;
        default:
            lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        }
        break;
	case CODE_NOTMATCH:
		{
			SetWindowText(hEditNew, "");
			SetWindowText(hEditAffirm,"");
			hModifyCodeFocusWnd = hEditNew;
			SetFocus(hModifyCodeFocusWnd);
		}
			break;

	case REQUEST_CODE:
		{
			if(iMP_Type == 0)//phone lock code 
			{
				hModifyCodeFocusWnd =hEditOld;
				SetFocus(hModifyCodeFocusWnd);
				SendMessage(hFrameWin, PWM_SETBUTTONTEXT, (WPARAM)1, (LPARAM)ML("")); 
			}
		}
		break;

    case WM_COMMAND:
        switch( LOWORD( wParam ))
        {
        case IDC_BUTTON_SET:
			
			if(ModifyCode(hWnd,hEditOld,hEditNew,hEditAffirm))
			{
				PostMessage(hWnd, WM_CLOSE, 0, 0);
				//DestroyWindow(hWnd);  
			}
            break;
			
        case IDC_BUTTON_QUIT:
            hCurWin = GetFocus();
            SendMessage(hCurWin, EM_GETSEL, (WPARAM)&nPos, (LPARAM)&nEnd);
            if (!nEnd)
                PostMessage(hWnd, WM_CLOSE, 0,0);//DestroyWindow( hWnd );  
            else
                SendMessage(hCurWin, WM_KEYDOWN, VK_BACK, 0);
            break;

		case IDC_OLD:
			if(HIWORD(wParam) == EN_CHANGE)
			{
				int strOldCodeLen = 0, strNewCodeLen = 0, strRepeatCodeLen = 0;

				strOldCodeLen = GetWindowTextLength(hEditOld);
				strNewCodeLen = GetWindowTextLength(hEditNew);
				strRepeatCodeLen = GetWindowTextLength(hEditAffirm);

				if(strOldCodeLen >= EDITMINLEN && strNewCodeLen >= EDITMINLEN
					&& strRepeatCodeLen >= EDITMINLEN)
				{
					SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 1, (LPARAM)ML(IDS_OK));
				}
				else
				{
					SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 1, (LPARAM)ML(""));
				}

			}
			break;
		case IDC_NEW:
			if(HIWORD(wParam) == EN_CHANGE)
			{
				int strOldCodeLen = 0, strNewCodeLen = 0, strRepeatCodeLen = 0;
				
				strOldCodeLen = GetWindowTextLength(hEditOld);
				strNewCodeLen = GetWindowTextLength(hEditNew);
				strRepeatCodeLen = GetWindowTextLength(hEditAffirm);
				
				if(strOldCodeLen >= EDITMINLEN && strNewCodeLen >= EDITMINLEN
					&& strRepeatCodeLen >= EDITMINLEN)
				{
					SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 1, (LPARAM)ML(IDS_OK));
				}
				else
				{
					SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 1, (LPARAM)ML(""));
				}

			}
			break;
		case IDC_AFFIRM:
			if(HIWORD(wParam) == EN_CHANGE)
			{
				int strOldCodeLen = 0, strNewCodeLen = 0, strRepeatCodeLen = 0;
				
				strOldCodeLen = GetWindowTextLength(hEditOld);
				strNewCodeLen = GetWindowTextLength(hEditNew);
				strRepeatCodeLen = GetWindowTextLength(hEditAffirm);
				
				if(strOldCodeLen >= EDITMINLEN && strNewCodeLen >= EDITMINLEN
					&& strRepeatCodeLen >= EDITMINLEN)
				{
					SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 1, (LPARAM)ML(IDS_OK));
				}
				else
				{
					SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 1, (LPARAM)ML(""));
				}
			}
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
* Function     ModifyCode
* Purpose      modify code(include phone code,PIN1,PIN2,call barring code,except PUK)
* Params       
* Return           
* Remarks      
**********************************************************************/
static  BOOL    ModifyCode(HWND hwnd,HWND hEditOld,HWND hEditNew,HWND hEditAffirm)
{
    char    cold[EDITMAXLEN + 1] = "",cnew[EDITMAXLEN + 1] = "",caffirm[EDITMAXLEN + 1] = "";
    char    cmobileold[EDITMAXLEN + 1] = "";
    int     Type;

    GetWindowText(hEditOld,cold,GetWindowTextLength(hEditOld) + 1);
    GetWindowText(hEditNew,cnew,GetWindowTextLength(hEditNew) + 1);
    GetWindowText(hEditAffirm,caffirm,GetWindowTextLength(hEditAffirm) + 1);

//	if(strlen(cold) ==0 || strlen(cnew) == 0 ||strlen(caffirm)==0)
//	{
//		PLXTipsWin(ML("Please, give\r\n all codes"), ML(cTitle), Notify_Alert, ML("Ok"),NULL, DEFAULT_TIMEOUT);
//		
//		if(strlen(cold) == 0)
//			SetFocus(hEditOld);
//		else if(strlen(cnew)==0)
//			SetFocus(hEditNew);
//		else
//			SetFocus(hEditAffirm);
//	}
	
    if (strcmp(cnew,caffirm) != 0)
    {
		PLXTipsWin(hFrameWin, hwnd, CODE_NOTMATCH, ML(CODESNOTMATCH), ML(cTitle), Notify_Failure, 
			ML(IDS_OK),NULL, DEFAULT_TIMEOUT);
		
	     return FALSE;
    }

    switch (iMP_Type)
    {
    case 0://modify phone code
        GetSecurity_code(cmobileold);

        if (strcmp(cold,cmobileold) != 0)//code error!
        {
            SetWindowText(hEditOld,"");
			SetWindowText(hEditNew,"");
			SetWindowText(hEditAffirm, "");
            PLXTipsWin(hFrameWin, hwnd, REQUEST_CODE, ML(WRONG_PHONELOCK), ML(cTitle), 
				Notify_Failure, ML(IDS_OK),NULL, 0);
             return FALSE;
        }
        SetSecurity_code(cnew);
        PLXTipsWin(hFrameWin, hwnd, 0, ML(PASSWORDMODIFY), ML(cTitle), Notify_Success,
			ML(IDS_OK),NULL, DEFAULT_TIMEOUT);
        return TRUE;

    case 1://modify PIN1
    case 2://modify PIN2
    case 3://modify barring
        if (iMP_Type == 1)     
            Type = LOCK_SC;
        else if (iMP_Type == 2) 
            Type = LOCK_P2;

        ME_ChangeLockPassword(hCallwnd,MsgCall,Type,cold,cnew);
        WaitWindowState(hwnd,TRUE);
        return TRUE;
    }
    return TRUE;
}
/********************************************************************
* Function   CreateControl  
* Purpose    
* Params     hwnd£º
* Return     
* Remarks     
**********************************************************************/
static  BOOL    CreateControl(HWND hwnd,HWND * hEditOld,HWND * hEditNew,HWND * hEditAffirm)
{
    int icontrolw = 0,istep = 20,icontrolh;

    icontrolh = 50;

    icontrolw = PLX_WIN_WIDTH;

       
    * hEditOld = CreateWindow (  //old code
        "EDIT", NULL, 
        WS_VISIBLE | WS_CHILD  | ES_CENTER |WS_TABSTOP/*|WS_BORDER*/|ES_PASSWORD|ES_NUMBER|ES_TITLE,
        CONTROL_X, 0, icontrolw, icontrolh,
        hwnd,(HMENU)IDC_OLD,NULL,NULL);
    
    if (* hEditOld  == NULL)
        return FALSE;
	
	if(iMP_Type == 3)//barring password
		SendMessage(*hEditOld, EM_SETTITLE, 0, (LPARAM)ML(OLDPASS_TXT));
	else
		SendMessage(*hEditOld, EM_SETTITLE, 0, (LPARAM)ML(OLDCODE_TXT));

    * hEditNew = CreateWindow (  //new code
        "EDIT",NULL, 
        WS_VISIBLE | WS_CHILD  | ES_CENTER |WS_TABSTOP/*|WS_BORDER*/|ES_PASSWORD|ES_NUMBER|ES_TITLE,
        CONTROL_X, icontrolh, icontrolw, icontrolh,
        hwnd,(HMENU)IDC_NEW,NULL,NULL);
    
    if (* hEditNew == NULL)
        return FALSE;

	if(iMP_Type == 3)
		SendMessage(*hEditNew, EM_SETTITLE, 0, (LPARAM)ML(NEWPASS_TXT));
	else
		SendMessage(*hEditNew, EM_SETTITLE, 0, (LPARAM)ML(NEWCODE_TXT));


    * hEditAffirm = CreateWindow (  //affirm code
        "EDIT",NULL, 
        WS_VISIBLE | WS_CHILD  | ES_CENTER |WS_TABSTOP/*|WS_BORDER*/|ES_PASSWORD|ES_NUMBER|ES_TITLE,
        CONTROL_X,icontrolh*2, icontrolw, icontrolh,
        hwnd,(HMENU)IDC_AFFIRM,NULL,NULL);
    
    if (* hEditAffirm == NULL)
        return FALSE;

	if(iMP_Type == 3)
		SendMessage(*hEditAffirm, EM_SETTITLE, 0, (LPARAM)ML(AFFIRMPASS_TXT));
	else
		SendMessage(*hEditAffirm, EM_SETTITLE, 0, (LPARAM)ML(AFFIRMCODE_TXT));


    if (iMP_Type == 0)//modify phone code
    {
        SendMessage(* hEditOld   ,EM_LIMITTEXT, EDITMAXLEN , 0);
        SendMessage(* hEditNew   ,EM_LIMITTEXT, EDITMAXLEN , 0);
        SendMessage(* hEditAffirm,EM_LIMITTEXT, EDITMAXLEN, 0);
    }
    else
    {
        SendMessage(* hEditOld   ,EM_LIMITTEXT, EDITMAXLEN , 0);
        SendMessage(* hEditNew   ,EM_LIMITTEXT, EDITMAXLEN , 0);
        SendMessage(* hEditAffirm,EM_LIMITTEXT, EDITMAXLEN , 0);
    }
    return TRUE;
}
