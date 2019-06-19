/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : 
 *
 * Purpose  : 
 *
\**************************************************************************/

#include    "winpda.h"
#include    "plx_pdaex.h"
#include    "str_public.h"
#include    "str_plx.h"
#include    "setup/setup.h"
#include    "OptionSelect.h"
#include	"callapp/MBPublic.h"

#define     IDC_BUTTON_QUIT         3//�˳���ť
#define     IDC_BUTTON_SET          4//ȷ����ť

#define     IDC_RADIO_OPEN          5
#define     IDC_RADIO_CLOSE         6
#define     IDC_CHECKBOX            7
#define     IDC_STRSPINBOX          8

#define     BUTTONOK                (LPARAM)ML("OK")    //ȷ��
#define     BUTTONQUIT              (LPARAM)ML("Cancel")  //�˳�
#define     ERROR1                  ML("Error")//GetString(STR_SETUP_ERROR1)         //������ʾ
#define     WINDOWICO               (LPARAM)""              //����ͼ��

#define     OPTIONSELECT_OPENTXT    ML("Open")//GetString(STR_INIT_PUB_OPEN)//��
#define     OPTIONSELECT_CLOSETXT   ML("Close")//GetString(STR_INIT_PUB_CLOSE)//�ر�

static const char * pClassName  = "OptionSelectWndClass";
static const int    iControlX   = 20;//�ؼ�������
static const int    iClewX      = 5;

static HINSTANCE    hInstance;
static OPTIONSELECT optionselect;

static  LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
BOOL    CallOptionWindow(const OPTIONSELECT * pOptionSelect)
{
    HWND hwnd;
    WNDCLASS wc;

    if (NULL == pOptionSelect)
        return FALSE;

    memset(&optionselect,0x00,sizeof(OPTIONSELECT));
    memcpy(&optionselect,pOptionSelect,sizeof(OPTIONSELECT));

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

    if ((optionselect.ControlType != RADIOBUTTON) &&
        (optionselect.ControlType != CHECKBOXBUTTON) &&
        (optionselect.ControlType != STRSPINBOX))
        return FALSE;

    if (optionselect.ClewPosition != OPTION_TOP)
        return FALSE;

    if ((optionselect.RadioPosition != POSITION_ROW) && 
        (optionselect.RadioPosition != POSITION_COL))
        return FALSE;

    if (!RegisterClass(&wc))
        return FALSE;

    hwnd = CreateWindow(pClassName,optionselect.titlecaption, 
        WS_CAPTION|WS_BORDER |PWS_STATICBAR, 
        PLX_WIN_POSITION , optionselect.hCallWnd, NULL, NULL, NULL);

    if (NULL == hwnd)
    {
        UnregisterClass(pClassName,NULL);
        return FALSE;
    }

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd); 

    return TRUE;
}
static LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
    static      HWND    hFocus = 0;
                RECT    rect;
                POINT   pRadioOn;   //��ѡ��ť�򿪵�λ��
                POINT   pRadioOff;  //��ѡ��ť�رյ�λ��

                HWND    hRadio_ON;
                HWND    hRadio_OFF;
                HWND    hCheckbox;
                HWND    hStrSpinBox;
                int     iscreenw,iscreenh,iControlH,iRadioWidth;
                int     iresult = 0;
                int     iRecty;//�ؼ����õ�λ��(�������)
                LRESULT lResult;
                HDC     hdc;

    lResult = (LRESULT)TRUE;
    switch ( wMsgCmd )
    {
    case WM_CREATE :
        SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,(WPARAM)IDC_BUTTON_QUIT,BUTTONQUIT);
        SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDC_BUTTON_SET,1),BUTTONOK);
        SendMessage(hWnd,PWM_SETAPPICON,0,WINDOWICO);//Ϊ���ڼ�ͼ��

        iscreenw  = GetScreenUsableWH1(SM_CXSCREEN);
        iscreenh  = GetScreenUsableWH1(SM_CYSCREEN);
        
        iControlH = GetButtonControlHeight();

        switch (optionselect.ClewPosition)
        {
        case OPTION_TOP://��ʾ�����ڿؼ����ϱ�
            switch (optionselect.ControlType)
            {
            case CHECKBOXBUTTON:
                hCheckbox = CreateWindow("BUTTON",optionselect.CheckTxt,
                    BS_AUTOCHECKBOX|WS_CHILD|WS_VISIBLE|WS_TABSTOP,
                    iControlX,
                    (strlen(optionselect.Clew) > 0) ? iscreenh / 3 : iscreenh / 2,
                    (iscreenw - iControlX) / 2,iControlH,
                    hWnd,(HMENU)IDC_CHECKBOX,hInstance,NULL);

                if (hCheckbox == NULL)
                    return FALSE;

                if (optionselect.ControlValue)
                    SendMessage(hCheckbox,BM_SETCHECK,1,0);

                break;
            case STRSPINBOX:
                hStrSpinBox = CreateWindow("StrSpinBox", "", //����״̬�ؼ�
                    WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | WS_BORDER | WS_TABSTOP,
                    iControlX,
                    (strlen(optionselect.Clew) > 0) ? iscreenh / 3 : iscreenh / 2,
                    (iscreenw - iControlX) / 2,iControlH, 
                    hWnd, (HMENU)IDC_STRSPINBOX, NULL, NULL);

                if (hStrSpinBox == NULL)
                    return FALSE;

                SendMessage(hStrSpinBox,SSBM_ADDSTRING,0,
                    strlen(optionselect.cOpen) > 0 ? (LPARAM)optionselect.cOpen : (LPARAM)OPTIONSELECT_OPENTXT);

                SendMessage(hStrSpinBox,SSBM_ADDSTRING,0,
                    strlen(optionselect.cClose) > 0 ? (LPARAM)optionselect.cClose : (LPARAM)OPTIONSELECT_CLOSETXT);

                SendMessage(hStrSpinBox,SSBM_SETCURSEL,optionselect.ControlValue ? 1 : 0,0);

                SendMessage(hWnd,PWM_SETSCROLLSTATE,SCROLLLEFT|SCROLLRIGHT, MASKALL );

                break;
            case RADIOBUTTON:
                iRecty = iscreenh / 3;
                switch (optionselect.RadioPosition)
                {
                case POSITION_ROW://������ѡ��ť����һ��
                    iRadioWidth = (iscreenw - 3 * iControlX) / 2;

                    pRadioOff.x = iControlX; 
                    pRadioOff.y = (strlen(optionselect.Clew) > 0) ? iRecty : (iscreenh / 3);

                    pRadioOn.x  = (pRadioOff.x + iRadioWidth + iControlX);
                    pRadioOn.y  = pRadioOff.y;
                    break;
                case POSITION_COL://������ѡ��ť����һ��
                    iRadioWidth = (iscreenw - iControlX) / 2;

                    pRadioOn.x  = iControlX * 2;
                    pRadioOn.y  = (strlen(optionselect.Clew) > 0) ? iRecty : (iscreenh / 4);

                    pRadioOff.x = pRadioOn.x;
                    pRadioOff.y = pRadioOn.y + iControlH + 10;
                    break;
                }
                hRadio_ON = CreateWindow("BUTTON",
                    (strlen(optionselect.cOpen) > 0) ? optionselect.cOpen : OPTIONSELECT_OPENTXT,//��
                    BS_AUTORADIOBUTTON|WS_CHILD|WS_VISIBLE|WS_TABSTOP,
                    pRadioOn.x,pRadioOn.y,iRadioWidth,iControlH,
                    hWnd,(HMENU)IDC_RADIO_OPEN,hInstance,NULL);

                hRadio_OFF = CreateWindow("BUTTON",
                    (strlen(optionselect.cClose) > 0) ? optionselect.cClose : OPTIONSELECT_CLOSETXT,//��
                    BS_AUTORADIOBUTTON|WS_CHILD|WS_VISIBLE|WS_TABSTOP,
                    pRadioOff.x,pRadioOff.y,iRadioWidth,iControlH,
                    hWnd,(HMENU)IDC_RADIO_CLOSE,hInstance,NULL);

                if ((hRadio_OFF == NULL) || (hRadio_ON == NULL))
                    return FALSE;

                if (optionselect.ControlValue)
                    CheckRadioButton(hWnd,IDC_RADIO_OPEN,IDC_RADIO_CLOSE,IDC_RADIO_OPEN);
                else
                    CheckRadioButton(hWnd,IDC_RADIO_OPEN,IDC_RADIO_CLOSE,IDC_RADIO_CLOSE);

                SendMessage(hWnd,PWM_SETSCROLLSTATE,
                    SCROLLLEFT|SCROLLRIGHT|SCROLLDOWN|SCROLLUP,MASKALL);
                break;
            }
            break;
        default:
            return FALSE;
        }

        switch(optionselect.ControlType)
        {
        case RADIOBUTTON:
            hFocus = optionselect.ControlValue ? hRadio_ON : hRadio_OFF;
            break;
        case CHECKBOXBUTTON:
            hFocus = hCheckbox;
            break;
        case STRSPINBOX:
            hFocus = hStrSpinBox;
            break;
        }
        break;

    case WM_SETFOCUS:
        SetFocus(hFocus);
        break;

    case WM_ACTIVATE://ȥ�������Ϣ�������Ҽ������˳��������˳�ʧ��
        if (WA_INACTIVE != LOWORD(wParam))
        {
            SetFocus(hFocus);//��������ý���
            InvalidateRect(hWnd,NULL,TRUE);
        }
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    case WM_CLOSE://������Ͻǵ��˳���ťʱΪ�����˳�;
        SendMessage( hWnd, WM_COMMAND, (WPARAM)IDC_BUTTON_QUIT, 0 );   
        break;

    case WM_DESTROY ://֪ͨ���������,�˳����˳�;
        UnregisterClass(pClassName,NULL);
        DlmNotify((WPARAM)PES_STCQUIT, (LPARAM)hInstance);
        break;

    case WM_PAINT :
        hdc = BeginPaint( hWnd, NULL);
        if (strlen(optionselect.Clew) > 0)
        {
            iscreenw = GetScreenUsableWH1(SM_CXSCREEN);
            iRecty   = GetScreenUsableWH1(SM_CYSCREEN) / 3;
            switch (optionselect.ClewPosition)
            {
            case OPTION_TOP://��ʾ�����ڿؼ����ϱ�
                SetRect(&rect,iClewX,iRecty / 2,iscreenw - iClewX,iRecty);
                StrAutoNewLine(hdc,optionselect.Clew,NULL,&rect,DT_VCENTER|DT_HCENTER);
                break;
            }
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
        case IDC_RADIO_OPEN:
            if(HIWORD(wParam) == BN_SETFOCUS)
                CheckRadioButton(hWnd,IDC_RADIO_OPEN,IDC_RADIO_CLOSE,IDC_RADIO_OPEN);
            break;
        case IDC_RADIO_CLOSE:
            if(HIWORD(wParam) == BN_SETFOCUS)
                CheckRadioButton(hWnd,IDC_RADIO_OPEN,IDC_RADIO_CLOSE,IDC_RADIO_CLOSE);
            break;

        case IDC_BUTTON_SET://����
            WaitWindowState(hWnd,TRUE);

            switch (optionselect.ControlType)
            {
            case RADIOBUTTON:
                hRadio_ON  = GetDlgItem(hWnd,IDC_RADIO_OPEN);
                hRadio_OFF = GetDlgItem(hWnd,IDC_RADIO_CLOSE);

                ShowWindow(hRadio_ON,SW_HIDE);
                ShowWindow(hRadio_OFF,SW_HIDE);

                iresult = SendMessage(hRadio_ON,BM_GETCHECK,0,0);
                break;
            case CHECKBOXBUTTON:
                iresult = SendMessage(GetDlgItem(hWnd,IDC_CHECKBOX),BM_GETCHECK,0,0);
                break;
            case STRSPINBOX:
                iresult = SendMessage(GetDlgItem(hWnd,IDC_STRSPINBOX),SSBM_GETCURSEL,0,0);
                break;
            }
            PostMessage(optionselect.hCallWnd,optionselect.hCallMsg,1,iresult);

            WaitWindowState(hWnd,FALSE);

            DestroyWindow(hWnd);  
            break;
        case IDC_BUTTON_QUIT://�˳�
            PostMessage(optionselect.hCallWnd,optionselect.hCallMsg,0,0);
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
