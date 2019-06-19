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

#include    "NewCode.h"

BOOL    CallNewCodeWindow(HWND hwndCall,int itype,char * caption,UINT CallMsg)
{
    HWND hwnd;
    WNDCLASS wc;
	RECT rect;

    memset(cTitle,0x00,sizeof(cTitle));
    strcpy(cTitle,caption);
    iMP_Type = itype;
	hParentWnd = hwndCall;
	CallBackMsg = CallMsg;

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

	
	SendMessage(hFrameWin, PWM_SETBUTTONTEXT, (WPARAM)0, (LPARAM)ML(IDS_CANCEL)); 
	
	SendMessage(hFrameWin, PWM_SETBUTTONTEXT, (WPARAM)1, (LPARAM)ML("")); 

	SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"");

	SetWindowText(hFrameWin, ML(cTitle));


    return TRUE;
}
static LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
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
        CreateControl(hWnd,&hEditNew,&hEditAffirm);
        hNewCodeFocusWnd = hEditNew;
		SetFocus(hNewCodeFocusWnd);     
        break;
	case PWM_SHOWWINDOW:
		{
			int strNewCodelen=0, strRepeatCodelen = 0;
				
			strNewCodelen = GetWindowTextLength(hEditNew);
			strRepeatCodelen = GetWindowTextLength(hEditAffirm);
			
			if(strNewCodelen >= EDITMINLEN && strRepeatCodelen >= EDITMINLEN)
			{
				SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 1, (LPARAM)ML(IDS_OK));
			}
			else
				SendMessage(hFrameWin, PWM_SETBUTTONTEXT, (WPARAM)1, (LPARAM)ML(""));
			
			SendMessage(hFrameWin, PWM_SETBUTTONTEXT, (WPARAM)0, (LPARAM)ML(IDS_CANCEL));

			SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
			
			SetWindowText(hFrameWin, ML(cTitle));

			SetFocus(hNewCodeFocusWnd);
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
            SendMessage(hWnd,WM_COMMAND,IDC_BUTTON_QUIT,0);
            break;
        case VK_RETURN:
			{
				int strNewCodelen=0, strRepeatCodelen = 0;
				
				strNewCodelen = GetWindowTextLength(hEditNew);
				strRepeatCodelen = GetWindowTextLength(hEditAffirm);
				
				if(strNewCodelen >= EDITMINLEN && strRepeatCodelen >= EDITMINLEN)
					SendMessage(hWnd,WM_COMMAND,IDC_BUTTON_SET,0);
			}
            break;
        default:
            lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        }
        break;

	case REQUEST_CODE:
		{	
			SetWindowText(hEditNew, "");
			SetWindowText(hEditAffirm,"");
			hNewCodeFocusWnd = hEditNew;
			SendMessage(hFrameWin, PWM_SETBUTTONTEXT, (WPARAM)1, (LPARAM)ML("")); 
		}
		break;

    case WM_COMMAND:
        switch( LOWORD( wParam ))
        {
        case IDC_BUTTON_SET:
			if (ModifyCode(hWnd,hEditNew,hEditAffirm))
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
		case IDC_NEW:
			
			if(HIWORD(wParam) == EN_CHANGE)
			{
				int strNewCodelen=0, strRepeatCodelen = 0;
				
				strNewCodelen = GetWindowTextLength(hEditNew);
				strRepeatCodelen = GetWindowTextLength(hEditAffirm);

				if(strNewCodelen >= EDITMINLEN && strRepeatCodelen >= EDITMINLEN)
				{
					SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 1, (LPARAM)ML(IDS_OK));
				}
				
			}

			break;
		case IDC_AFFIRM:
			if(HIWORD(wParam) == EN_CHANGE)
			{
				int strNewCodelen=0, strRepeatCodelen = 0;
				
				strNewCodelen = GetWindowTextLength(hEditNew);
				strRepeatCodelen = GetWindowTextLength(hEditAffirm);

				if(strNewCodelen >= EDITMINLEN && strRepeatCodelen >= EDITMINLEN)
				{
					SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 1, (LPARAM)ML(IDS_OK));
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
static  BOOL    ModifyCode(HWND hwnd,HWND hEditNew,HWND hEditAffirm)
{
    char    cnew[EDITMAXLEN + 1] = "",caffirm[EDITMAXLEN + 1] = "";
    char    cmobileold[EDITMAXLEN + 1] = "";
  
	GetWindowText(hEditNew,cnew,GetWindowTextLength(hEditNew) + 1);
	GetWindowText(hEditAffirm,caffirm,GetWindowTextLength(hEditAffirm) + 1);

//	if(strlen(cnew) == 0 ||strlen(caffirm)==0)
//	{
//		PLXTipsWin(ML("Please, give\r\n all codes"), ML(cTitle), Notify_Alert, ML("Ok"),NULL, DEFAULT_TIMEOUT);
//		
//		if(strlen(cnew)==0)
//			SetFocus(hEditNew);
//		else
//			SetFocus(hEditAffirm);
//	}
	
	   if (strcmp(cnew,caffirm) != 0)
	   {
		   PLXTipsWin(hFrameWin, hwnd, REQUEST_CODE, ML(CODESNOTMATCH), ML(cTitle), Notify_Failure, ML(IDS_OK),NULL, DEFAULT_TIMEOUT);
		   
		   return FALSE;
	   }
    
   
    switch (iMP_Type)
    {
    case 1://modify PIN1
    case 2://modify PIN2
        SendMessage(hParentWnd,CallBackMsg,(WPARAM)iMP_Type,(LPARAM)cnew);
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
static  BOOL    CreateControl(HWND hwnd, HWND * hEditNew,HWND * hEditAffirm)
{
    int icontrolw,istep = 15,icontrolh;

    icontrolh = 50;

   icontrolw = PLX_WIN_WIDTH;
    
   

    * hEditNew = CreateWindow (  //new code
        "EDIT",NULL, 
        WS_VISIBLE | WS_CHILD  | ES_CENTER |WS_TABSTOP/*|WS_BORDER*/|ES_PASSWORD|ES_TITLE|ES_NUMBER,
        0, 0, icontrolw, icontrolh,
        hwnd,(HMENU)IDC_NEW,NULL,NULL);
    
    if (* hEditNew == NULL)
        return FALSE;
	
	SendMessage(*hEditNew, EM_SETTITLE, 0, (LPARAM)ML(NEWCODE_TXT));

    * hEditAffirm = CreateWindow (  //affirm code
        "EDIT",NULL, 
        WS_VISIBLE | WS_CHILD  | ES_CENTER |WS_TABSTOP/*|WS_BORDER*/|ES_PASSWORD|ES_TITLE|ES_NUMBER,
        0, icontrolh, icontrolw, icontrolh,
        hwnd,(HMENU)IDC_AFFIRM,NULL,NULL);
    
    if (* hEditAffirm == NULL)
        return FALSE;

	SendMessage(*hEditAffirm, EM_SETTITLE, 0, (LPARAM)ML(AFFIRMCODE_TXT));

    if (iMP_Type == 0)//modify phone code
    {
        SendMessage(* hEditNew   ,EM_LIMITTEXT, EDITMINLEN  , 0);
        SendMessage(* hEditAffirm,EM_LIMITTEXT, EDITMINLEN , 0);
    }
    else
    {
        SendMessage(* hEditNew   ,EM_LIMITTEXT, EDITMAXLEN , 0);
        SendMessage(* hEditAffirm,EM_LIMITTEXT, EDITMAXLEN , 0);
    }
    return TRUE;
}
