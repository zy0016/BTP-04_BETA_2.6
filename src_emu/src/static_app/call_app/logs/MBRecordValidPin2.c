

#include    "window.h"
#include    "string.h"
#include    "stdlib.h"
#include    "stdio.h" 
#include    "winpda.h"
#include "mullang.h"

#include    "plx_pdaex.h"

#include    "RcCommon.h"
#include    "me_wnd.h"

#define     IDC_BUTTON_QUIT         3
#define     IDC_BUTTON_SET          4
#define     IDC_EDITPIN2            5
#define     IDC_VALIDPIN2           WM_USER + 100
#define     IDC_ME_PIN2LIMIT        WM_USER + 110
#define     IDC_VALIDPUK2           WM_USER + 120

#define     TITLECAPTION            ML("Invaldpin2") 
#define     BUTTONOK                (LPARAM)ML("Ok") 
#define     BUTTONQUIT              (LPARAM)ML("Exit") 
#define     ERROR1                  ML("Createwindowfail") 
#define     CLEWPIN2                ML("Inputpin2") 
#define     VALIDPIN2FAIL           ML("Invaldpin2fail") 
#define     VALIDPUK2FAIL           ML("Puk2error") 
#define     CLEWPUK2                ML("Inputpuk2") 
#define     CLEWPIN2_NEW            ML("Inputnewpin2") 
#define     CLEWPIN2_CON            ML("Confirmpin2") 
#define     CLEW_ERROR              ML("Inputthecode") 
#define     CLEW_ERROR2             ML("Affirmcodeerror") 

#define PIN2MAXLEN                  10

static const char * pClassName = "MBValidPin2WndClass";
static HINSTANCE    hInstance;
static BOOL         bPin2Valid;
static char         cValidPin2[PIN2MAXLEN];
static int          iOperateType;
static BOOL         bPin2Check = FALSE;

#define TYPE_PIN2       0
#define TYPE_PUK2       1
#define TYPE_NEWPIN2    2
#define TYPE_NEWCONPIN2 3

static  LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static  BOOL    CreateControl(HWND hWnd,HWND * hEdit);

BOOL    CallValidPin2(HWND hwndCall)
{
    HWND        hWnd;
    WNDCLASS    wc;
    MSG         msg;

    wc.style         = CS_OWNDC;
    wc.lpfnWndProc   = AppWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = pClassName;

    bPin2Valid       = FALSE;

    if (!RegisterClass(&wc))
        return FALSE;

    hWnd = CreateWindow(pClassName,TITLECAPTION, 
        WS_CAPTION |PWS_STATICBAR, 
        PLX_WIN_POSITION , hwndCall, NULL, NULL, NULL);

    if (NULL == hWnd)
    {
        UnregisterClass(pClassName,NULL);
        return FALSE;
    }

    ShowWindow(hWnd, SW_SHOW);
    UpdateWindow(hWnd); 

    while (IsWindow(hWnd))
    {
        GetMessage(&msg, NULL, 0, 0);
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (bPin2Valid);
}

static LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
    static  HWND        hEdit;
    static  HWND        hFocus = 0;
    static  char        cPuk2[PIN2MAXLEN] = "";
    static  char        cPin2_new[PIN2MAXLEN] = "";
            char        cPin2[PIN2MAXLEN] = "";
            LRESULT     lResult;
            int         nEnd = 0,nPos = 0;
            HWND        hCurWin = 0;
            HDC         hdc;
            RECT        rect;
            int         iPassLimit = 0;

    lResult = (LRESULT)TRUE;
    switch ( wMsgCmd )
    {
    case WM_CREATE :
        if (!CreateControl(hWnd,&hEdit))
        {
            AppMessageBox(NULL,ERROR1, TITLECAPTION, WAITTIMEOUT);
            return -1;
        }
        hFocus       = hEdit;
        iOperateType = TYPE_PIN2;


        ME_GetPassInputLimit(hWnd,IDC_ME_PIN2LIMIT,PIN2);
        break;

    case WM_SETFOCUS:
        SetFocus(hFocus);
        break;

    case WM_SETRBTNTEXT:
        SendMessage (hWnd, PWM_SETBUTTONTEXT, 0, lParam);
        break;

    case WM_ACTIVATE:
        if (WA_INACTIVE != LOWORD(wParam))
            SetFocus(hFocus);
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);

        break;
    case WM_CLOSE:
        SendMessage ( hWnd, WM_COMMAND, (WPARAM)IDC_BUTTON_QUIT, 0 );   
        break;

    case WM_DESTROY :
        UnregisterClass(pClassName,NULL);
        DlmNotify ((WPARAM)PES_STCQUIT, (LPARAM)hInstance );
        break;

    case WM_PAINT :
        hdc = BeginPaint( hWnd, NULL);
        SetRect(&rect,10,10,GetScreenUsableWH1(SM_CXSCREEN) - 20,30);
        switch (iOperateType)
        {
        case TYPE_PIN2:
            DrawText(hdc,CLEWPIN2,strlen(CLEWPIN2),&rect,DT_CENTER);
            break;
        case TYPE_PUK2:
            DrawText(hdc,CLEWPUK2,strlen(CLEWPUK2),&rect,DT_CENTER);
            break;
        case TYPE_NEWPIN2:
            DrawText(hdc,CLEWPIN2_NEW,strlen(CLEWPIN2_NEW),&rect,DT_CENTER);
            break;
        case TYPE_NEWCONPIN2:
            DrawText(hdc,CLEWPIN2_CON,strlen(CLEWPIN2_CON),&rect,DT_CENTER);
            break;
        }
        EndPaint(hWnd, NULL);
        break;

    case WM_KEYDOWN:
        switch(LOWORD(wParam))
        {
        case VK_F10:
            SendMessage(hWnd,WM_COMMAND,IDC_BUTTON_QUIT,0);
            break;
        case VK_RETURN:
            SendMessage(hWnd,WM_COMMAND,IDC_BUTTON_SET,0);
            break;
        default:
            lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        }
        break;

    case WM_COMMAND:
        switch( LOWORD( wParam ))
        {
        case IDC_BUTTON_SET:
            GetWindowText(hEdit,cPin2,GetWindowTextLength(hEdit) + 1);
            if (strlen(cPin2) == 0)
            {
                AppMessageBox(hWnd,CLEW_ERROR,TITLECAPTION, WAITTIMEOUT);
                break;
            }
            switch (iOperateType)
            {
            case TYPE_PIN2:
                if (bPin2Check)
                {
                    char cPinChecked[PIN2MAXLEN] = "";
                    GetValidPin2(cPinChecked);
                    if (strcmp(cPin2,cPinChecked) == 0)
                    {
                        PostMessage(hWnd,IDC_VALIDPIN2,ME_RS_SUCCESS,0);
                    }
                    else
                    {
                        AppMessageBox(hWnd,VALIDPIN2FAIL,TITLECAPTION, WAITTIMEOUT);
                        SetWindowText(hEdit,"");
                        SetFocus(hEdit);
                    }
                }
                else
                {
                    if (-1 == ME_PasswordValidation(hWnd,IDC_VALIDPIN2,PIN2,cPin2,NULL))
                    {
#ifndef _EMULATE_
                        AppMessageBox(hWnd,VALIDPIN2FAIL,TITLECAPTION, WAITTIMEOUT);
#else
                        PostMessage(hWnd,IDC_VALIDPIN2,ME_RS_SUCCESS,0);
#endif
                    }
                }
                break;
            case TYPE_PUK2:
                GetWindowText(hEdit,cPuk2,GetWindowTextLength(hEdit) + 1);

                iOperateType = TYPE_NEWPIN2;
                SetWindowText(hEdit,"");
                SetFocus(hEdit);
                InvalidateRect(hWnd,NULL,TRUE);
                break;
            case TYPE_NEWPIN2:
                GetWindowText(hEdit,cPin2_new,GetWindowTextLength(hEdit) + 1);

                iOperateType = TYPE_NEWCONPIN2;
                SetWindowText(hEdit,"");
                SetFocus(hEdit);
                InvalidateRect(hWnd,NULL,TRUE);
                break;
            case TYPE_NEWCONPIN2:
                if (strcmp(cPin2_new,cPin2) != 0)
                {
                    AppMessageBox(hWnd,CLEW_ERROR2,TITLECAPTION, WAITTIMEOUT);
                    iOperateType = TYPE_NEWPIN2;
                    SetWindowText(hEdit,"");
                    SetFocus(hEdit);
                    InvalidateRect(hWnd,NULL,TRUE);
                    break;
                }
                if (-1 == ME_PasswordValidation(hWnd,IDC_VALIDPUK2,PUK2,cPuk2,cPin2_new))
                    AppMessageBox(hWnd,VALIDPUK2FAIL,TITLECAPTION, WAITTIMEOUT);
                break;
            }
            break;
        case IDC_BUTTON_QUIT:
            hCurWin = GetFocus();
            SendMessage(hCurWin, EM_GETSEL, (WPARAM)&nPos, (LPARAM)&nEnd);
            if (!nEnd)
            {
                switch (iOperateType)
                {
                case TYPE_PIN2:
                    bPin2Valid = FALSE;
                    DestroyWindow(hWnd);  
                    break;
                case TYPE_PUK2:
                    AppMessageBox(hWnd,CLEWPUK2,TITLECAPTION, WAITTIMEOUT);
                    SetFocus(hEdit);
                    break;
                case TYPE_NEWPIN2:
                    iOperateType = TYPE_PUK2;
                    SetWindowText(hEdit,"");
                    SetFocus(hEdit);
                    InvalidateRect(hWnd,NULL,TRUE);
                    break;
                case TYPE_NEWCONPIN2:
                    iOperateType = TYPE_NEWPIN2;
                    SetWindowText(hEdit,"");
                    SetFocus(hEdit);
                    InvalidateRect(hWnd,NULL,TRUE);
                    break;
                }
            }
            else
            {
                SendMessage(hCurWin, WM_KEYDOWN, VK_BACK, 0);
            }
            break;
        }
        break;
    
    case IDC_VALIDPIN2:
        switch (wParam)
        {
        case ME_RS_SUCCESS:
            GetWindowText(hEdit,cValidPin2,GetWindowTextLength(hEdit) + 1);
            bPin2Valid = TRUE;
            bPin2Check = TRUE;
            DestroyWindow(hWnd);
            break;
        default:
            ME_GetPassInputLimit(hWnd,IDC_ME_PIN2LIMIT,PIN2);
            AppMessageBox(hWnd,VALIDPIN2FAIL,TITLECAPTION, WAITTIMEOUT);
            SetWindowText(hEdit,"");
            SetFocus(hEdit);
            break;
        }
        break;

    case IDC_ME_PIN2LIMIT:
        switch (wParam)
        {
        case ME_RS_SUCCESS:
            ME_GetResult(&iPassLimit,sizeof(int));
            if (iPassLimit == 0)
            {
                iOperateType = TYPE_PUK2;
                SetWindowText(hEdit,"");
                SetFocus(hEdit);
                InvalidateRect(hWnd,NULL,TRUE);
            }
            break;
        default:
            break;
        }
        break;

    case IDC_VALIDPUK2:
        switch (wParam)
        {
        case ME_RS_SUCCESS:
            strcpy(cValidPin2,cPin2_new);
            bPin2Valid = TRUE;
            bPin2Check = TRUE;
            DestroyWindow(hWnd);
            break;
        default:
            AppMessageBox(hWnd,VALIDPUK2FAIL,TITLECAPTION, WAITTIMEOUT);
            iOperateType = TYPE_PUK2;
            SetWindowText(hEdit,"");
            SetFocus(hEdit);
            InvalidateRect(hWnd,NULL,TRUE);
            break;
        }
        break;

    default :
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
    return lResult;
}
void    GetValidPin2(char * pPin2)
{
    strcpy(pPin2,cValidPin2);
}
void    SetPin2Change(void)
{
    bPin2Check = FALSE;
}

static  BOOL    CreateControl(HWND hWnd,HWND * hEdit)
{
    int icontrolx = 10;
	static IMEEDIT Ime;
    SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, 
                (WPARAM)IDC_BUTTON_QUIT,BUTTONQUIT );

    SendMessage(hWnd , PWM_CREATECAPTIONBUTTON, 
                MAKEWPARAM(IDC_BUTTON_SET,1),BUTTONOK );
	memset(&Ime, 0, sizeof(IMEEDIT));
	Ime.hwndNotify = hWnd;
	Ime.dwAttrib = 0;
	Ime.dwAscTextMax = 0;
	Ime.dwUniTextMax = 0;
	Ime.pszImeName = "Phone";
	Ime.pszCharSet = "0123456789*#";
	Ime.pszTitle	= NULL;
	
	
	Ime.uMsgSetText	= 0;

    * hEdit = CreateWindow (
        "IMEEDIT",NULL,
        WS_VISIBLE | WS_CHILD  | ES_CENTER |WS_TABSTOP|ES_PASSWORD,
        icontrolx, 40, GetScreenUsableWH1(SM_CXSCREEN) - icontrolx * 2, GetEditControlHeight(),
        hWnd,(HMENU)IDC_EDITPIN2,NULL,(PVOID)&Ime);

    if (* hEdit == NULL)
        return FALSE;

    SendMessage(* hEdit,  EM_LIMITTEXT, 8, 0);

    return TRUE;
}
