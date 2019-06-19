#include   "winpda.h"
#include   "plx_pdaex.h"
#include   "stdlib.h"
#include   "mullang.h"
#include   "MBPublic.h"
#include   "MB_control.h"

#define     IDC_SEND                3
#define     IDC_BACK                4
#define     IDC_EDIT                5
#define     MAX_EDITLEN             41
#define     WM_ME_SUPERADDDIAL WM_USER + 110
#define     WM_AB_GETNUMBER    WM_USER + 120
#define     IDS_SEND  ML("Send")
#define     IDS_BACK  ML("Cancel")
#define     CAP_SENDDTMF  ML("Send DTMF")
#define     CAP_EDIT_SENDDTMF   ML("DTMF number:")
#define     PLEASEWAITING       ML("Please wait...")
#define     NOTIFY_WILDCLEWEMPTY    ML("Please define number")

static const char * pSendDTMFClassName = "SendDTMFWndClass";
static HWND  hNotifyWnd;
static UINT  wNotifyMsg;
static HWND  hSendDTMFWnd;
static HWND  hSendDTMFEdit;
static HWND  hFocus;
static int iDTMFLen;
static int iExtension;
static HWND hFrameWnd;
static char cSendDTMFNumber[PHONENUMMAXLEN + 1]="";

static LRESULT SendDTMFAppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
extern void SetFlagStarInValid(void);

BOOL    CreateSendDTMFWindow(char * pszDTMF)
{
    WNDCLASS    wc;
	RECT   rClient;

    memset(cSendDTMFNumber,0x00,sizeof(cSendDTMFNumber));
	if (pszDTMF != NULL)
	{
		if (strlen(pszDTMF)>PHONENUMMAXLEN)
			return FALSE;
		else
			strcpy(cSendDTMFNumber, pszDTMF);
	}

	hFrameWnd =CreateFrameWindow( WS_CAPTION |PWS_STATICBAR|PWS_NOTSHOWPI);
	if (hFrameWnd == NULL) 
		return FALSE;
	CALLWND_InsertDataByHandle(pGlobalCallListBuf, hFrameWnd);
    wc.style         = CS_OWNDC;
    wc.lpfnWndProc   = SendDTMFAppWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = pSendDTMFClassName;

    if (!RegisterClass(&wc))
        return FALSE;

	if (hFrameWnd != NULL) 
	{
		GetClientRect(hFrameWnd, &rClient);
		hSendDTMFWnd = CreateWindow(pSendDTMFClassName, "",
			WS_VISIBLE | WS_CHILD,
			rClient.left, 
            0,//rClient.top, 
            rClient.right-rClient.left, 
            rClient.bottom / 3,//rClient.bottom-rClient.top,
			hFrameWnd, NULL, NULL, NULL);
	}
    if (NULL == hSendDTMFWnd)
    {
        UnregisterClass(pSendDTMFClassName,NULL);
        return FALSE;
    }
    ShowWindow(hFrameWnd, SW_SHOW);
    UpdateWindow(hFrameWnd); 
	if (hSendDTMFEdit != NULL)
		SetFocus(hSendDTMFEdit);
	else
		SetFocus(hSendDTMFWnd); 
    return (TRUE);
}

static LRESULT SendDTMFAppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
    static  IMEEDIT   IReply;
//    int         nEnd = 0,nPos = 0;
//    int         iLen;
    HWND        hCurWin = 0;
    LRESULT     lResult;
	DWORD       dwStyle;
	RECT rClient;
			
    lResult = (LRESULT)TRUE;
    switch ( wMsgCmd )
    {
    case WM_CREATE :
		iExtension = 0;
		memset(&IReply, 0, sizeof(IMEEDIT));
		IReply.hwndNotify = hWnd;
		IReply.dwAttrib = 0;
		IReply.dwAscTextMax = 0;
		IReply.dwUniTextMax = 0;
		IReply.pszImeName ="Phone";//"ºÅÂë";
		IReply.pszCharSet = NULL;
		IReply.pszTitle	= NULL;
		IReply.uMsgSetText	= 0;

		dwStyle = WS_VISIBLE | WS_CHILD | WS_VSCROLL |  ES_TITLE | ES_MULTILINE;
		GetClientRect(hWnd, &rClient);
        hSendDTMFEdit = CreateWindow ("IMEEDIT", NULL, 
			dwStyle,
			rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
            hWnd,(HMENU)IDC_EDIT,NULL,(PVOID)&IReply);

        if (hSendDTMFEdit == NULL)
            return FALSE;
		hFocus = hSendDTMFEdit;
		if (strlen(cSendDTMFNumber)> 0) 
		{
			SetWindowText(hSendDTMFEdit, cSendDTMFNumber);
			SendMessage(hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_SEND, 1), (LPARAM)IDS_SEND);
		}
		else
			SendMessage(hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_SEND, 1), (LPARAM)"");
        SendMessage(hSendDTMFEdit, EM_LIMITTEXT, (WPARAM)MAX_EDITLEN, NULL);        
        SendMessage(hSendDTMFEdit, EM_SETTITLE, 0, (LPARAM)CAP_EDIT_SENDDTMF);    
		SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
		SendMessage(hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_BACK, 0), (LPARAM)IDS_BACK);
		SetWindowText(hFrameWnd, CAP_SENDDTMF);
        break;

	case PWM_SHOWWINDOW:
		SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
		SetWindowText(hFrameWnd, CAP_SENDDTMF);
		if (strlen(cSendDTMFNumber) == 0)
		{
			SendMessage(hFrameWnd,PWM_SETBUTTONTEXT,1,(LPARAM)"");
			SendMessage(hFrameWnd,PWM_SETBUTTONTEXT,0,(LPARAM)IDS_BACK);
		}
		else if (strlen(cSendDTMFNumber) >=1) 
		{
			SendMessage(hFrameWnd,PWM_SETBUTTONTEXT,1,(LPARAM)IDS_SEND);
			SendMessage(hFrameWnd,PWM_SETBUTTONTEXT,0,(LPARAM)IDS_BACK);
		}
		break;

    case WM_SETRBTNTEXT:
		SendMessage (hFrameWnd, PWM_SETBUTTONTEXT, 0, lParam);
		break;


/*
    case WM_ACTIVATE:
        if (WA_INACTIVE != LOWORD(wParam))
			SetFocus(hFocus);
		else
			hFocus = GetFocus();
        break;*/


    case WM_CLOSE: 
        DestroyWindow(hWnd);
        break;

    case WM_DESTROY :
        WaitWindowStateEx(NULL, FALSE, PLEASEWAITING, NULL, NULL, NULL);
		SetFlagStarInValid();
		hSendDTMFWnd = NULL;
		hSendDTMFEdit = NULL;
		hFocus = NULL;
        UnregisterClass(pSendDTMFClassName,NULL);
        break;

	case WM_ME_SUPERADDDIAL:
		{
			switch(wParam)
			{
			case ME_RS_SUCCESS:
				if ((cSendDTMFNumber[iExtension] != 0) && 
					(!GetListCurCalling()) &&                   
					(!IsAllType_Specific(CALL_STAT_HELD)))
				{
                    while ((cSendDTMFNumber[iExtension] == '+') ||
                        (cSendDTMFNumber[iExtension] == 'P') ||
                        (cSendDTMFNumber[iExtension] == 'S'))
                        iExtension++;

                    if (cSendDTMFNumber[iExtension] == '\0')
                    {
                        PostMessage(hWnd,WM_ME_SUPERADDDIAL,ME_RS_SUCCESS,0);
                        break;
                    }

					if ((cSendDTMFNumber[iExtension] == '+') ||
                        (cSendDTMFNumber[iExtension] == 'P') ||
                        (cSendDTMFNumber[iExtension] == 'S'))
					{
						iExtension++;
					}
					else
					{	
						ME_SendDTMF(hWnd,WM_ME_SUPERADDDIAL,cSendDTMFNumber[iExtension++]);
					}
				}
				else if (cSendDTMFNumber[iExtension] == 0)
				{
                    WaitWindowStateEx(NULL, FALSE, PLEASEWAITING, NULL, NULL, NULL);
					CALLWND_DeleteDataByHandle(pGlobalCallListBuf, hFrameWnd);			
					SendMessage(hFrameWnd,PWM_CLOSEWINDOW,  (WPARAM) hWnd, 0);
					// add after frame modifyed
					SendMessage(hWnd, WM_CLOSE, 0,0);	
				}
				break;
			default:
                WaitWindowStateEx(NULL, FALSE, PLEASEWAITING, NULL, NULL, NULL);
				MBListCurCalls(LISTCURCALLS_REPEAT,wParam,lParam);				
				CALLWND_DeleteDataByHandle(pGlobalCallListBuf, hFrameWnd);			
				SendMessage(hFrameWnd,PWM_CLOSEWINDOW,  (WPARAM) hWnd, 0);
				// add after frame modifyed
				SendMessage(hWnd, WM_CLOSE, 0,0);	
				break;
			}
		}
		break;

	case WM_AB_GETNUMBER:
		{
			ABNAMEOREMAIL abnameinfo;
			memset(&abnameinfo, 0, sizeof(ABNAMEOREMAIL));
			if (wParam == TRUE)
			{
				memcpy(&abnameinfo, (void * )lParam, sizeof(ABNAMEOREMAIL));
				if ((strlen(abnameinfo.szTelOrEmail) <=MAX_EDITLEN) 
					&& (strlen(abnameinfo.szTelOrEmail) >0) 
					&& (abnameinfo.nType ==AB_NUMBER))
				{
					SetWindowText(hSendDTMFEdit, abnameinfo.szTelOrEmail);
					SetFocus(hSendDTMFEdit);
				}
				else
					SetFocus(hSendDTMFEdit);
			}
			else
				SetFocus(hSendDTMFEdit);
		}
		break;
		
    case WM_KEYDOWN:
        switch(LOWORD(wParam))
        {
		case VK_F1:
		case VK_RETURN:
			iDTMFLen = GetWindowTextLength(hSendDTMFEdit);
			if (iDTMFLen > 0) 
			{
				PostMessage(hWnd,WM_ME_SUPERADDDIAL,ME_RS_SUCCESS,0);	
                WaitWindowStateEx(NULL, TRUE, PLEASEWAITING, NULL, NULL, NULL);
			}			
            else
            {
                PLXTipsWin(NULL, NULL, NULL, NOTIFY_WILDCLEWEMPTY,
                    NULL, Notify_Alert, ML("Ok"), NULL,WAITTIMEOUT);
            }
			break;
		case VK_F10:
			CALLWND_DeleteDataByHandle(pGlobalCallListBuf, hFrameWnd);			
			SendMessage(hFrameWnd,PWM_CLOSEWINDOW,  (WPARAM) hWnd, 0);
			 // add after frame modifyed
			 SendMessage(hWnd, WM_CLOSE, 0,0);			
			break;
        case VK_F5:
			// call ab
			APP_GetOnePhoneOrEmail(hFrameWnd, hWnd, WM_AB_GETNUMBER, PICK_NUMBER);
            break;
        default:
            lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        }
        break;
    case WM_COMMAND:
        switch( LOWORD( wParam ))
        {
		case IDC_EDIT:
//			if (HIWORD(wParam) == EN_CHANGE) 
			{
				iDTMFLen = GetWindowTextLength(hSendDTMFEdit);
				if (iDTMFLen == 0)
				{
					SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
				}
				if (iDTMFLen > 0) 
				{
					SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_SEND);
					if (iDTMFLen < PHONENUMMAXLEN) 
					{
						GetWindowText(hSendDTMFEdit, cSendDTMFNumber, iDTMFLen+1);
						//cSendDTMFNumber[iDTMFLen + 1] = '\0';
					}   
					else
					{
						GetWindowText(hSendDTMFEdit, cSendDTMFNumber, PHONENUMMAXLEN);
						//cSendDTMFNumber[PHONENUMMAXLEN] = '\0';
					}
				}
			}			
			break;

        case IDC_BACK:
			CALLWND_DeleteDataByHandle(pGlobalCallListBuf, hFrameWnd);			
			SendMessage(hFrameWnd,PWM_CLOSEWINDOW,  (WPARAM) hWnd, 0);	
			// add after frame modifyed
			SendMessage(hWnd, WM_CLOSE, 0,0);			
            break;
        }
        break;

    default :
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
    return lResult;
}

void CloseSendDTMFWnd()
{
	if (IsWindow(hSendDTMFWnd))
	{
		CALLWND_DeleteDataByHandle(pGlobalCallListBuf, hFrameWnd);			
		SendMessage(hFrameWnd,PWM_CLOSEWINDOW,  (WPARAM) hSendDTMFWnd, 0);	
		// add after frame modifyed
		SendMessage(hSendDTMFWnd, WM_CLOSE, 0,0);
	}
}
