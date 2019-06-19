/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : MMS	
 *
 * Purpose  : Setting
 *            
\**************************************************************************/

#include "MmsSetting.h"

#define IDC_OK                      310 
#define IDC_CANCEL                  311

#define IDC_MMS_CONN                301
#define IDC_MMS_HOMENET             302
#define IDC_MMS_VISITNET            303
#define IDC_MMS_ANONY               304
#define IDC_MMS_ADVERT              305
#define IDC_MMS_REPORT_REC          306
#define IDC_MMS_REPORT_SEND         307
#define IDC_MMS_VALIDITY            308
#define IDC_MMS_PICSIZE             309    

#define MMS_SETSEL_RETURN           WM_USER + 300

#define MMS_BMP_SELECT          "/rom/message/sms/rb_select.bmp"
#define MMS_BMP_NORMAL          "/rom/message/sms/rb_normal.bmp"

LRESULT MMSSetWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static BOOL MMSSetting_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct);
static void MMSSetting_OnActivate(HWND hwnd, UINT state);
static void MMSSetting_OnPaint(HWND hWnd);
static void MMSSetting_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags);
static void MMSSetting_OnCommand(HWND hWnd, int id, UINT codeNotify);
static void MMSSetting_OnDestroy(HWND hWnd);
static void MMSSetting_OnClose(HWND hWnd);
static void MMSSetting_OnVScroll(HWND hWnd, HWND hwndCtl, UINT code, int pos);
static void MMSSetting_InitVScroll(HWND hWnd);
static void MMSSetting_OnSelReturn(HWND hWnd, WPARAM wParam, LPARAM lParam);
static BOOL MmsSet_ModifyConnection(void);

BOOL MMS_GetSetting(MMSSETTING *pSet);
BOOL MMS_SaveSetting(MMSSETTING *pSet);

extern BOOL MYBUTTON_RegisterClass(void);
extern int     IspGetNum(void);
extern BOOL    IspReadInfo(UDB_ISPINFO  * uIspInfo ,int iNum);
extern BOOL	   IspReadInfoByID(UDB_ISPINFO * uIspInfo, int iIDnum) ;

//sel
static BOOL MMSSetting_OnSelect(HWND hWndFrame, HWND hParent, int msg, int id, int nCurSel);
/*********************************************************************\
* Function	   SMS_CreateSettingWnd
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL MMS_CreateSettingWnd(HWND hWndFrame, HWND hParent)
{
    WNDCLASS wc;
    HWND hWndMmsSet = NULL;
	RECT rClient;
    MMS_SETCREATEDATA  SetData;
        
    wc.style         = 0;
    wc.lpfnWndProc   = MMSWindowClass[3].wndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = sizeof(MMS_SETCREATEDATA);
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName  = MMSWindowClass[3].szClassName;
    
    RegisterClass(&wc);

	if (hWndFrame == NULL)
		hWndFrame = MuGetFrame();

	SetData.hWndFrame = hWndFrame;

	GetClientRect(hWndFrame, &rClient);

    hWndMmsSet = CreateWindow(
        MMSWindowClass[3].szClassName, 
        STR_MMS_MMS,
        WS_VISIBLE | WS_CHILD | WS_VSCROLL, 
        rClient.left, rClient.top, rClient.right-rClient.left, rClient.bottom-rClient.top,
        hWndFrame,
        NULL,
        NULL, 
        (PVOID)&SetData
        );
    
    if (!hWndMmsSet)
    {
        UnregisterClass(MMSWindowClass[3].szClassName, NULL);
        return FALSE;
    }

	SetFocus(hWndMmsSet);

	SetWindowText(hWndFrame, STR_MMS_MMS);
    
	SendMessage(hWndFrame, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(312,1), 
        (LPARAM)ML(""));
    SendMessage(hWndFrame, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_CANCEL,0), 
        (LPARAM)ML("Back"));
	SendMessage(hWndFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
	SendMessage(hWndFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, LEFTICON), 
			(LPARAM)NULL);

    return TRUE;
}
/*********************************************************************\
* Function	MMSSetWndProc
* Purpose   Main window proc
* Params
*			hWnd: Handle of the window
*			wMsgCmd: Message ID
* Return
*			TRUE: Success
*			FALSE: Fail
* Remarks
**********************************************************************/
LRESULT MMSSetWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    LRESULT lResult = TRUE;
    HDC     hdc;

	switch (wMsgCmd)
    {
    case WM_CREATE:
        lResult = MMSSetting_OnCreate(hWnd,(LPCREATESTRUCT)(lParam));
        break;

    case WM_ACTIVATE:
	case PWM_SHOWWINDOW:
        MMSSetting_OnActivate(hWnd,(UINT)LOWORD(wParam));
        break;

	case WM_SETFOCUS:
		{
			PMMS_SETCREATEDATA  pSetData;

			pSetData = GetUserData(hWnd);

			SetFocus(pSetData->hFocus);
		}
		break;

    case WM_PAINT:
        hdc = BeginPaint(hWnd, NULL);        
        EndPaint(hWnd, NULL);
        break;

	case WM_KEYUP:
		KillTimer(hWnd, 1);
		break;

	case WM_TIMER:
		{
			PMMS_SETCREATEDATA  pSetData;
			HWND hWndFocus;
			
			KillTimer(hWnd, 1);
			SetTimer(hWnd, 1, 100, NULL);
			pSetData = GetUserData(hWnd);
			hWndFocus = GetFocus();
			while(GetParent(hWndFocus) != hWnd)
				hWndFocus = GetParent(hWndFocus);

			if(pSetData->iRepeatType == 0)
			{
				//down
				hWndFocus = GetNextDlgTabItem(hWnd, hWndFocus, FALSE);
				SetFocus(hWndFocus);
				SendMessage(hWnd, WM_VSCROLL, SB_LINEDOWN, NULL);
			}
			else if(pSetData->iRepeatType == 1)
			{	
				//up
				hWndFocus = GetNextDlgTabItem(hWnd, hWndFocus, TRUE);
				SetFocus(hWndFocus);                
				SendMessage(hWnd, WM_VSCROLL, SB_LINEUP, NULL);
			
			}
		}
		break;

    case WM_KEYDOWN:
        MMSSetting_OnKey(hWnd,(UINT)(wParam),(int)(short)LOWORD(lParam),(UINT)HIWORD(lParam));
        break;

    case WM_COMMAND:
        MMSSetting_OnCommand(hWnd,(int)(LOWORD(wParam)),(UINT)HIWORD(wParam));
        break;

	case WM_VSCROLL:		
		MMSSetting_OnVScroll(hWnd, (HWND)(lParam),(UINT)(LOWORD(wParam)), 
            (int)(short)HIWORD(wParam));
		break;

    case MMS_SETSEL_RETURN:
        MMSSetting_OnSelReturn(hWnd, wParam, lParam);
        break;

    case WM_CLOSE:
    {
        MMSSETTING  mmsSet;

		MMS_GetSetting(&mmsSet);
        mmsSet.homerec = SendMessage(GetDlgItem(hWnd, IDC_MMS_HOMENET), 
            SSBM_GETCURSEL, 0, 0);
        mmsSet.visitrec = SendMessage(GetDlgItem(hWnd, IDC_MMS_VISITNET), 
            SSBM_GETCURSEL, 0, 0);
        mmsSet.nAnonymity = SendMessage(GetDlgItem(hWnd, IDC_MMS_ANONY), 
            SSBM_GETCURSEL, 0, 0);
        mmsSet.nAd = SendMessage(GetDlgItem(hWnd, IDC_MMS_ADVERT), 
            SSBM_GETCURSEL, 0, 0);
        mmsSet.nReportRec = SendMessage(GetDlgItem(hWnd, IDC_MMS_REPORT_REC), 
            SSBM_GETCURSEL, 0, 0);
        mmsSet.nReportSend = SendMessage(GetDlgItem(hWnd, IDC_MMS_REPORT_SEND), 
            SSBM_GETCURSEL, 0, 0);
        mmsSet.indexExpire = SendMessage(GetDlgItem(hWnd, IDC_MMS_VALIDITY), 
            SSBM_GETCURSEL, 0, 0);
        mmsSet.nImageSize = SendMessage(GetDlgItem(hWnd, IDC_MMS_PICSIZE), 
            SSBM_GETCURSEL, 0, 0);
        MMS_SaveSetting(&mmsSet);
        MMSPro_ModifySetting(&mmsSet);
        DestroyWindow(hWnd);
        break;
    }

    case WM_DESTROY:
	{
		PMMS_SETCREATEDATA pSetData;

		pSetData = GetUserData(hWnd);
		SendMessage(pSetData->hWndFrame,PWM_CLOSEWINDOW,(WPARAM)hWnd,0);
        UnregisterClass(MMSWindowClass[3].szClassName, NULL);
        break;
	}

    default:     
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
	}

    return lResult;

}
/*********************************************************************\
* Function	MMSSetting_OnCreate
* Purpose   WM_CREATE message handler of the main window
* Params
*			hWnd: Handle of the window
*			lpCreateStruct: Create Structure
* Return
*			TRUE: Success
*			FALSE: Fail
* Remarks
**********************************************************************/
static BOOL MMSSetting_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
{   
    int  y,nHeight,nWidth;
    RECT rcClient;
    MMSSETTING      MMSSetting;
    HWND hSpinConn = NULL,hSpinHome = NULL,hSpinVisit = NULL;
    HWND hSpinAnony = NULL, hSpinAdvert = NULL, hSpinReportRec = NULL;
    HWND hSpinReportSend = NULL, hSpinValidity = NULL, hSpinPicsize = NULL;
    PMMS_SETCREATEDATA  pSetData;
    
    pSetData = GetUserData(hWnd);
    memcpy(pSetData,lpCreateStruct->lpCreateParams,sizeof(MMS_SETCREATEDATA));

    GetClientRect(hWnd,&rcClient);

    memset(&MMSSetting,0,sizeof(MMSSETTING));

    MMS_GetSetting(&MMSSetting);
    
    y = 0;
    nHeight = (rcClient.bottom - rcClient.top)/3;
    nWidth = rcClient.right - rcClient.left;
    
    hSpinConn = CreateWindow(
        "SPINBOXEX", 
        STR_MMS_CONN, 
        WS_CHILD | WS_VISIBLE | SSBS_LEFT | WS_TABSTOP | SSBS_ARROWRIGHT |CS_NOSYSCTRL,
        0,y,nWidth,nHeight,
        hWnd, 
        (HMENU)IDC_MMS_CONN, 
        NULL, 
        NULL);

    if (MMSSetting.ISPName[0] == 0)
		SendMessage(hSpinConn, SSBM_ADDSTRING, 0, (LPARAM)"Required"); //0
	else
	{
		UDB_ISPINFO uIspInfo;
		memset(&uIspInfo, 0x0, sizeof(UDB_ISPINFO));
        if(IspReadInfoByID(&uIspInfo, MMSSetting.ISPID))
			SendMessage(hSpinConn, SSBM_ADDSTRING, 0, (LPARAM)uIspInfo.ISPName);
		else
			SendMessage(hSpinConn, SSBM_ADDSTRING, 0, (LPARAM)"Required");
		//SendMessage(hSpinConn, SSBM_ADDSTRING, 0, (LPARAM)MMSSetting.ISPName); //0
	}

    y +=  nHeight;

    hSpinHome = CreateWindow(
        "SPINBOXEX", 
        STR_MMS_HOMENET, 
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT | CS_NOSYSCTRL,
        0,y,nWidth,nHeight,
        hWnd, 
        (HMENU)IDC_MMS_HOMENET, 
        NULL, 
        NULL);
    
    SendMessage(hSpinHome, SSBM_ADDSTRING, 0, (LPARAM)STR_MMS_REC_ATONCE); //0
    SendMessage(hSpinHome, SSBM_ADDSTRING, 0, (LPARAM)STR_MMS_REC_CONFIRM);//1
    SendMessage(hSpinHome, SSBM_ADDSTRING, 0, (LPARAM)STR_MMS_REC_NEVER);  //2

    SendMessage(hSpinHome, SSBM_SETCURSEL, MMSSetting.homerec, 0);
    
    y +=  nHeight;    
    
    hSpinVisit = CreateWindow(
        "SPINBOXEX", 
        STR_MMS_VISITNET, 
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT | CS_NOSYSCTRL,
        0,y,nWidth,nHeight,
        hWnd, 
        (HMENU)IDC_MMS_VISITNET, 
        NULL, 
        NULL);
    
    SendMessage(hSpinVisit, SSBM_ADDSTRING, 0, (LPARAM)STR_MMS_REC_ATONCE); //0
    SendMessage(hSpinVisit, SSBM_ADDSTRING, 0, (LPARAM)STR_MMS_REC_CONFIRM);//1
    SendMessage(hSpinVisit, SSBM_ADDSTRING, 0, (LPARAM)STR_MMS_REC_NEVER);  //2
    
    SendMessage(hSpinVisit, SSBM_SETCURSEL, MMSSetting.visitrec, 0);
    
    y +=  nHeight;
    
    hSpinAnony = CreateWindow(
        "SPINBOXEX", 
        STR_MMS_ANONY,
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT | CS_NOSYSCTRL,
        0,y,nWidth,nHeight,
        hWnd, 
        (HMENU)IDC_MMS_ANONY, 
        NULL, 
        NULL);

    SendMessage(hSpinAnony, SSBM_ADDSTRING, 0, (LPARAM)STR_MMS_YES);
	SendMessage(hSpinAnony, SSBM_ADDSTRING, 0, (LPARAM)STR_MMS_NO);
    
    SendMessage(hSpinAnony, SSBM_SETCURSEL, MMSSetting.nAnonymity, 0);
    
    y +=  nHeight;
  
    hSpinAdvert = CreateWindow(
        "SPINBOXEX", 
        STR_MMS_ADVERT, 
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT | CS_NOSYSCTRL,
        0,y,nWidth,nHeight,
        hWnd, 
        (HMENU)IDC_MMS_ADVERT, 
        NULL, 
        NULL);
        
    SendMessage(hSpinAdvert, SSBM_ADDSTRING, 0, (LPARAM)STR_MMS_YES);
	SendMessage(hSpinAdvert, SSBM_ADDSTRING, 0, (LPARAM)STR_MMS_NO);
    
    SendMessage(hSpinAdvert, SSBM_SETCURSEL, MMSSetting.nAd, 0);

    y +=  nHeight;    
    hSpinReportRec = CreateWindow(
        "SPINBOXEX", 
        STR_MMS_REPORT_RECEP, 
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT | CS_NOSYSCTRL,
        0,y,nWidth,nHeight,
        hWnd, 
        (HMENU)IDC_MMS_REPORT_REC, 
        NULL, 
        NULL);

    SendMessage(hSpinReportRec, SSBM_ADDSTRING, 0, (LPARAM)STR_MMS_ON);
	SendMessage(hSpinReportRec, SSBM_ADDSTRING, 0, (LPARAM)STR_MMS_OFF);
    
    SendMessage(hSpinReportRec, SSBM_SETCURSEL, MMSSetting.nReportRec, 0);

    y +=  nHeight;    
    hSpinReportSend = CreateWindow(
        "SPINBOXEX", 
        STR_MMS_REPORT_SEND, 
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT | CS_NOSYSCTRL,
        0,y,nWidth,nHeight,
        hWnd, 
        (HMENU)IDC_MMS_REPORT_SEND, 
        NULL, 
        NULL);

    SendMessage(hSpinReportSend, SSBM_ADDSTRING, 0, (LPARAM)STR_MMS_ON);
	SendMessage(hSpinReportSend, SSBM_ADDSTRING, 0, (LPARAM)STR_MMS_OFF);
    
    SendMessage(hSpinReportSend, SSBM_SETCURSEL, MMSSetting.nReportSend, 0);

    y +=  nHeight;    
    hSpinValidity = CreateWindow(
        "SPINBOXEX", 
        STR_MMS_VALID, 
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT | CS_NOSYSCTRL,
        0,y,nWidth,nHeight,
        hWnd, 
        (HMENU)IDC_MMS_VALIDITY, 
        NULL, 
        NULL);

    SendMessage(hSpinValidity, SSBM_ADDSTRING, 0, (LPARAM)STR_MMS_1HOUR);
    SendMessage(hSpinValidity, SSBM_ADDSTRING, 0, (LPARAM)STR_MMS_6HOUR);
    SendMessage(hSpinValidity, SSBM_ADDSTRING, 0, (LPARAM)STR_MMS_24HOUR);
    SendMessage(hSpinValidity, SSBM_ADDSTRING, 0, (LPARAM)STR_MMS_1WEEK);
    SendMessage(hSpinValidity, SSBM_ADDSTRING, 0, (LPARAM)STR_MMS_MAXTIME);

    SendMessage(hSpinValidity, SSBM_SETCURSEL, MMSSetting.indexExpire, 0);

//    y +=  nHeight;    
//    hSpinPicsize = CreateWindow(
//        "SPINBOXEX", 
//        STR_MMS_PICSIZE, 
//        WS_CHILD | WS_VISIBLE | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT | CS_NOSYSCTRL,
//        0,y,nWidth,nHeight,
//        hWnd, 
//        (HMENU)IDC_MMS_PICSIZE, 
//        NULL, 
//        NULL);
//
//    SendMessage(hSpinPicsize, SSBM_ADDSTRING, 0, (LPARAM)STR_MMS_SMALL);
//    SendMessage(hSpinPicsize, SSBM_ADDSTRING, 0, (LPARAM)STR_MMS_LARGE);
//
//    SendMessage(hSpinPicsize, SSBM_SETCURSEL, MMSSetting.nImageSize, 0);

    MMSSetting_InitVScroll(hWnd);
    
    pSetData->hFocus = hSpinConn;

    return TRUE;    
}
/*********************************************************************\
* Function	MMSSetting_OnActivate
* Purpose   WM_ACTIVATE message handler of the main window
* Params
* Return    None
* Remarks
**********************************************************************/
static void MMSSetting_OnActivate(HWND hWnd, UINT state)
{
    PMMS_SETCREATEDATA pSetData;

    pSetData = GetUserData(hWnd);

    if(state == WA_ACTIVE)
        SetFocus(pSetData->hFocus);
    else
        pSetData->hFocus = GetFocus();

	SetWindowText(pSetData->hWndFrame, STR_MMS_MMS);
	SendMessage(pSetData->hWndFrame, PWM_SETBUTTONTEXT, 1, 
        (LPARAM)ML(""));         
    SendMessage(pSetData->hWndFrame, PWM_SETBUTTONTEXT, 0,
        (LPARAM)ML("Back"));
	SendMessage(pSetData->hWndFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
	SendMessage(pSetData->hWndFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, LEFTICON), 
			(LPARAM)NULL);
    return;
}
/*********************************************************************\
* Function	MMSSetting_OnKey
* Purpose   WM_KEYDOWN message handler of the main window
* Params
*			hWnd: Handle of the window
*			vk:	Virtual key code
*			fDown: Is key donw
*			cRepeat: Key repeat rate
*			flags:	flag of key down
* Return	None
* Remarks
**********************************************************************/
static void MMSSetting_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags)
{
    HWND hWndFocus;
    int  id, nSel;
	PMMS_SETCREATEDATA  pSetData;
    
	pSetData = GetUserData(hWnd);
    hWndFocus = GetFocus();    
    while(GetParent(hWndFocus) != hWnd)
        hWndFocus = GetParent(hWndFocus);

	switch (vk)
	{
	case VK_F10:
        PostMessage(hWnd,WM_CLOSE,0,0);
        break;

    case VK_DOWN:
        hWndFocus = GetNextDlgTabItem(hWnd, hWndFocus, FALSE);
        SetFocus(hWndFocus);
        SendMessage(hWnd, WM_VSCROLL, SB_LINEDOWN, NULL);
		SetTimer(hWnd, 1, 300, NULL);
		pSetData->iRepeatType = 0;
        break;

    case VK_UP:  
        hWndFocus = GetNextDlgTabItem(hWnd, hWndFocus, TRUE);
        SetFocus(hWndFocus);                
        SendMessage(hWnd, WM_VSCROLL, SB_LINEUP, NULL);
		SetTimer(hWnd, 1, 300, NULL);
		pSetData->iRepeatType = 1;
        break;

    case VK_F5:
        id = GetWindowLong(hWndFocus, GWL_ID);
		nSel = SendMessage(hWndFocus, SSBM_GETCURSEL, 0, 0);
        MMSSetting_OnSelect(pSetData->hWndFrame, hWnd, MMS_SETSEL_RETURN, id, nSel);
        break;

	default:
		PDADefWindowProc(hWnd, WM_KEYDOWN, vk, MAKELPARAM(cRepeat, flags));
		break;
	}

	return;
}
/*********************************************************************\
* Function	MMSSetting_OnCommand
* Purpose   WM_COMMAND message handler of the main window
* Params
*			hWnd:	Handle of the window
*			id:		Id of command message
*			hwndCtl: Handle of the window which sended this message
*			codeNotify:Notify code of the message
* Return	None
* Remarks
**********************************************************************/
static void MMSSetting_OnCommand(HWND hWnd, int id, UINT codeNotify)
{
    HWND hWndFocus;
    int  nSel = 0;
	PMMS_SETCREATEDATA  pSetData;
    
	pSetData = GetUserData(hWnd);
    hWndFocus = GetFocus();
	switch(id)
	{
    case IDC_OK:
        while(GetParent(hWndFocus) != hWnd)
            hWndFocus = GetParent(hWndFocus);
        PostMessage(hWndFocus,WM_KEYDOWN,VK_F5,0);
        break;

    case IDC_CANCEL:
        PostMessage(hWnd,WM_CLOSE,0,0);
        break;

    case IDC_MMS_CONN:
        // call connection
		if (codeNotify == BN_CLICKED)
		{
			nSel = SendMessage(hWndFocus, SSBM_GETCURSEL, 0, 0);
            MMSSetting_OnSelect(pSetData->hWndFrame, hWnd, MMS_SETSEL_RETURN, id, nSel);
		}
        break;

//    case IDC_MMS_HOMENET:
//    case IDC_MMS_VISITNET:
//    case IDC_MMS_ANONY:
//    case IDC_MMS_ADVERT:
//    case IDC_MMS_REPORT_REC:
//    case IDC_MMS_REPORT_SEND:
//    case IDC_MMS_VALIDITY:
//    case IDC_MMS_PICSIZE:
//        if (codeNotify == SSBN_CLICKED)
//        {
//            nSel = SendMessage(hWndFocus, SSBM_GETCURSEL, 0, 0);
//            MMSSetting_OnSelect(hWnd, MMS_SETSEL_RETURN, id, nSel);
//        }
//        break;
        
    default:
        break;
	}

	return;
}

// on select return
static void MMSSetting_OnSelReturn(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    HWND hWndCtl;

    hWndCtl = GetDlgItem(hWnd, (int)wParam);
	if ((int)lParam >= 0)
		SendMessage(hWndCtl, SSBM_SETCURSEL, (int)lParam, 0);
}
/*********************************************************************\
* Function	MMSSetting_OnVScroll
* Purpose   WM_VSCROLL
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void MMSSetting_OnVScroll(HWND hWnd, HWND hwndCtl, UINT code, int pos)
{
    int                 nY;
    RECT                rcClient;
    SCROLLINFO          vsi;
    PMMS_SETCREATEDATA  pSetData;

    pSetData = GetUserData(hWnd);

    switch(code)
    {
    case SB_LINEDOWN:        
        memset(&vsi, 0, sizeof(SCROLLINFO));
        vsi.fMask  = SIF_ALL ;
        GetScrollInfo(hWnd, SB_VERT, &vsi);
        
        GetClientRect(hWnd,&rcClient);
        nY = (rcClient.bottom - rcClient.top)/3;
        
        pSetData->nCurFocus++;

        if(pSetData->nCurFocus > vsi.nMax)
        {
            ScrollWindow(hWnd,0,(vsi.nMax-vsi.nPage+1)*nY,NULL,NULL);
            UpdateWindow(hWnd);
            pSetData->nCurFocus = 0;
            vsi.nPos = 0;
            vsi.fMask  = SIF_POS ;
            SetScrollInfo(hWnd, SB_VERT, &vsi, TRUE);
            break;
        }
        
        //if((int)(vsi.nPos + vsi.nPage) <= pSetData->nCurFocus)
		if((int)(vsi.nPos + vsi.nPage - 1) <= pSetData->nCurFocus 
			&& pSetData->nCurFocus != vsi.nMax)
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
        
        pSetData->nCurFocus--;

        if(pSetData->nCurFocus < vsi.nMin)
        {
            ScrollWindow(hWnd,0,(vsi.nPage-vsi.nMax-1)*nY,NULL,NULL);
            UpdateWindow(hWnd);
            pSetData->nCurFocus = vsi.nMax;
            vsi.nPos = vsi.nMax-vsi.nPage+1;
            vsi.fMask  = SIF_POS ;
            SetScrollInfo(hWnd, SB_VERT, &vsi, TRUE);
            break;
        }
        
        //if((int)vsi.nPos > pSetData->nCurFocus)
		if(vsi.nPos == pSetData->nCurFocus && pSetData->nCurFocus != vsi.nMin)
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
/*********************************************************************\
* Function	MMS_GetSetting
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
BOOL MMS_GetSetting(MMSSETTING *pSet)
{   
    char szOldPath[MAX_PATH_LEN];
    int f;

    szOldPath[0] = 0;
    
    getcwd(szOldPath,MAX_PATH_LEN);

    chdir(MMSSET_FILEPATH);

    f = open(MMS_SETFILE,O_RDONLY);

    if( f == -1 )
    {
        MMSSETTING mSet;

        f = open(MMS_SETFILE,O_RDWR|O_CREAT,S_IRWXU);
        if( f == -1 )
            return FALSE;
        memset(&mSet,0,sizeof(MMSSETTING));

        mSet.homerec = 1;       // confirm
        mSet.indexExpire = 4;   //max
        mSet.indexReply = 0;    //no use
        mSet.nAd = 1;           //no
        mSet.nAnonymity = 1;    //no
        mSet.nImageSize = 0;    //small
        mSet.nPriority = 0;     // no use
        mSet.nReportRec = 1;    // off
        mSet.nReportSend = 1;   // off
        mSet.visitrec = 1;      // confirm
        mSet.szIp[0] = 0;
        mSet.szUrl[0] = 0;

		mSet.port = 9201;
		mSet.DtType = 1;
		strcpy(mSet.ISPPhoneNum1, "cmwap");
		mSet.ISPID = 0;
		mSet.ISPName[0] = 0;
		mSet.ISPPassword[0] = 0;
		mSet.ISPUserName[0] = 0;
		// for debug
		//strcpy(mSet.szIp, MMS_GATEWAY_IP);
		//strcpy(mSet.szUrl, MMS_MMSC_URL);
		// end
        write(f,&mSet,sizeof(MMSSETTING));
        
        close(f);
        memcpy(pSet,&mSet,sizeof(MMSSETTING));
    }
    else
    {
        read(f,pSet,sizeof(MMSSETTING));        
        close(f);
    }

    chdir(szOldPath);

    return TRUE;
}
/*********************************************************************\
* Function	MMS_SaveSetting
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
BOOL MMS_SaveSetting(MMSSETTING *pSet)
{
    char szOldPath[MAX_PATH_LEN];
    int f;

    memset(szOldPath,0,MAX_PATH_LEN);
    
    getcwd(szOldPath,MAX_PATH_LEN);

    chdir(MMSSET_FILEPATH);

    if(pSet == NULL)
        return FALSE;
    else
    {
        f = open(MMS_SETFILE,O_RDWR|O_CREAT,S_IRWXU);
        
        if( f == -1 )
            return FALSE;

        write(f,pSet,sizeof(MMSSETTING));

        close(f);
    }

    chdir(szOldPath);

    return TRUE;
}
/*********************************************************************\
* Function	   MMSSetting_InitVScroll
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void MMSSetting_InitVScroll(HWND hWnd)
{
    SCROLLINFO              vsi;
    PMMS_SETCREATEDATA      pSetData;

    pSetData = GetUserData(hWnd);
    
    memset(&vsi, 0, sizeof(SCROLLINFO));
   
    vsi.cbSize = sizeof(vsi);
    vsi.fMask  = SIF_ALL ;
    vsi.nMin   = 0;
    vsi.nPage  = 3;
    vsi.nMax   = 7;
    vsi.nPos   = 0;
    
    SetScrollInfo(hWnd, SB_VERT, &vsi, TRUE);
    pSetData->nCurFocus = 0;
	return;
}
//////////////////////////////////////////////////////////////////////////
#define  IDC_MMSSETSEL_LIST        351
static   PCSTR              title[9] ={"Connection",
									  "In home network",
                                      "In visiting network",
                                      "Allow anonymous",
                                      "Allow adverts",
                                      "Report reception",
                                      "Report sending",
                                      "Message validity",
                                      "Picture size"};

static LRESULT MMSSetSelWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
/*********************************************************************\
* Function	   MMSSetting_OnSelect
* Purpose      response the selectaction 
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL MMSSetting_OnSelect(HWND hWndFrame, HWND hParent, int msg, int id, int nCurSel)
{
    WNDCLASS                wc;
    HWND                    hMmsSetSel = NULL;
    MMSSET_SELCREATEDATA    SelData;
	RECT                    rClient;
	SIZE					size;

    SelData.hWndCall = hParent;
    SelData.msg = msg;
    SelData.id = id;
    SelData.nCurSel = nCurSel;
	SelData.hWndFrame = hWndFrame;

	GetImageDimensionFromFile(MMS_BMP_NORMAL, &size);
	SelData.hBmpNormal = LoadImage(NULL, MMS_BMP_NORMAL, IMAGE_BITMAP,
		size.cx, size.cy, LR_LOADFROMFILE);
	GetImageDimensionFromFile(MMS_BMP_SELECT, &size);
	SelData.hBmpSel = LoadImage(NULL, MMS_BMP_SELECT, IMAGE_BITMAP,
		size.cx, size.cy, LR_LOADFROMFILE);

    wc.style         = 0;
    wc.lpfnWndProc   = MMSSetSelWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = sizeof(MMSSET_SELCREATEDATA);
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName  = "MmsSetSelClassName";
    
    RegisterClass(&wc);

	GetClientRect(hWndFrame, &rClient);

    hMmsSetSel = CreateWindow(
        "MmsSetSelClassName", 
        ML(title[id%100-1]),
        WS_VISIBLE | WS_CHILD, 
        rClient.left, rClient.top, rClient.right-rClient.left, rClient.bottom-rClient.top,
        hWndFrame,
        NULL,
        NULL, 
        (PVOID)&SelData
        );
    
    if (!hMmsSetSel)
    {
        UnregisterClass("MmsSetSelClassName", NULL);
        return FALSE;
    }
	
	ShowWindow( hMmsSetSel, SW_SHOW );
	UpdateWindow( hMmsSetSel );
	
	SetWindowText(hWndFrame, title[id%100-1]);

	SendMessage(hWndFrame, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(312,1), 
        (LPARAM)ML(""));         
    SendMessage(hWndFrame, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_CANCEL,0), 
        (LPARAM)ML("Cancel"));
	SendMessage(hWndFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
	SetFocus(hMmsSetSel);
    return TRUE;
}

// on mms setting select create
static BOOL MMSSetSel_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
{
    HWND                    hWndList;
    PMMSSET_SELCREATEDATA   pSelData;
	MMSSETTING				MMSSetting;
	int                     nCursel = 0, index = 0;

    pSelData = GetUserData(hWnd);
    memcpy(pSelData,lpCreateStruct->lpCreateParams,sizeof(MMSSET_SELCREATEDATA));

	memset(&MMSSetting, 0, sizeof(MMSSETTING));

	MMS_GetSetting(&MMSSetting);

    hWndList = CreateWindow(
        "LISTBOX",
        "", 
        WS_CHILD | WS_VISIBLE | WS_VSCROLL | LBS_BITMAP,
        0,
        0,
        SCREEN_WIDTH,
        168,
        hWnd, 
        (HMENU)IDC_MMSSETSEL_LIST,
        NULL, 
        NULL ); 

    if (NULL == hWndList)
        return FALSE;

    switch(pSelData->id) 
    {
	case IDC_MMS_CONN:
	{
		int ispnum = 0, i = 0, j = 1;
		UDB_ISPINFO     info;

		ispnum = IspGetNum();
		if (ispnum == 0)
		{
            PLXTipsWin(NULL, NULL, 0, ML("No connections"), NULL, Notify_Alert, 
                ML("OK"), NULL, 20);

            PostMessage(hWnd, WM_CLOSE, 0, 0);
            return FALSE;
        }

		SendMessage(hWndList, LB_ADDSTRING, 0, (LPARAM)ML("None"));
		SendMessage(hWndList, LB_SETIMAGE, 
				MAKEWPARAM(IMAGE_BITMAP, 0),(LPARAM)pSelData->hBmpNormal);
		for (i = 0; i < ispnum; i++)
		{
			IspReadInfo(&info,i);
//			if (!info.DtType)		// csd
//				continue;
			if (info.ISPID == MMSSetting.ISPID)
				nCursel = j;
			j++;
			index = SendMessage(hWndList, LB_ADDSTRING, 0, (LPARAM)info.ISPName);
			SendMessage(hWndList, LB_SETITEMDATA, index, (LPARAM)info.ISPID);
			SendMessage(hWndList, LB_SETIMAGE, 
				MAKEWPARAM(IMAGE_BITMAP, index),(LPARAM)pSelData->hBmpNormal);
		}
		
//		if (MMSSetting.ISPName[0] == 0)
//			nCursel = 0;
//		else
// 			nCursel ++;   // because the first item is None
//		if(nCursel != 0)
//			nCursel++;
		break;
	}

    case IDC_MMS_HOMENET:
    case IDC_MMS_VISITNET:
        index = SendMessage(hWndList, LB_ADDSTRING, 0, (LPARAM)STR_MMS_REC_ATONCE);
		SendMessage(hWndList, LB_SETIMAGE, 
				MAKEWPARAM(IMAGE_BITMAP, index),(LPARAM)pSelData->hBmpNormal);
        index = SendMessage(hWndList, LB_ADDSTRING, 0, (LPARAM)STR_MMS_REC_CONFIRM);
		SendMessage(hWndList, LB_SETIMAGE, 
				MAKEWPARAM(IMAGE_BITMAP, index),(LPARAM)pSelData->hBmpNormal);
        index = SendMessage(hWndList, LB_ADDSTRING, 0, (LPARAM)STR_MMS_REC_NEVER);
		SendMessage(hWndList, LB_SETIMAGE, 
				MAKEWPARAM(IMAGE_BITMAP, index),(LPARAM)pSelData->hBmpNormal);
    	break;

    case IDC_MMS_ANONY:
    case IDC_MMS_ADVERT:
        index = SendMessage(hWndList, LB_ADDSTRING, 0, (LPARAM)STR_MMS_YES);
		SendMessage(hWndList, LB_SETIMAGE, 
				MAKEWPARAM(IMAGE_BITMAP, index),(LPARAM)pSelData->hBmpNormal);
        index = SendMessage(hWndList, LB_ADDSTRING, 0, (LPARAM)STR_MMS_NO);
		SendMessage(hWndList, LB_SETIMAGE, 
				MAKEWPARAM(IMAGE_BITMAP, index),(LPARAM)pSelData->hBmpNormal);
    	break;

    case IDC_MMS_REPORT_REC:
    case IDC_MMS_REPORT_SEND:
        index = SendMessage(hWndList, LB_ADDSTRING, 0, (LPARAM)STR_MMS_ON);
		SendMessage(hWndList, LB_SETIMAGE, 
				MAKEWPARAM(IMAGE_BITMAP, index),(LPARAM)pSelData->hBmpNormal);
        index = SendMessage(hWndList, LB_ADDSTRING, 0, (LPARAM)STR_MMS_OFF);
		SendMessage(hWndList, LB_SETIMAGE, 
				MAKEWPARAM(IMAGE_BITMAP, index),(LPARAM)pSelData->hBmpNormal);
        break;

    case IDC_MMS_VALIDITY:
        index = SendMessage(hWndList, LB_ADDSTRING, 0, (LPARAM)STR_MMS_1HOUR);
		SendMessage(hWndList, LB_SETIMAGE, 
				MAKEWPARAM(IMAGE_BITMAP, index),(LPARAM)pSelData->hBmpNormal);
        index = SendMessage(hWndList, LB_ADDSTRING, 0, (LPARAM)STR_MMS_6HOUR);
		SendMessage(hWndList, LB_SETIMAGE, 
				MAKEWPARAM(IMAGE_BITMAP, index),(LPARAM)pSelData->hBmpNormal);
        index = SendMessage(hWndList, LB_ADDSTRING, 0, (LPARAM)STR_MMS_24HOUR);
		SendMessage(hWndList, LB_SETIMAGE, 
				MAKEWPARAM(IMAGE_BITMAP, index),(LPARAM)pSelData->hBmpNormal);
        index = SendMessage(hWndList, LB_ADDSTRING, 0, (LPARAM)STR_MMS_1WEEK);
		SendMessage(hWndList, LB_SETIMAGE, 
				MAKEWPARAM(IMAGE_BITMAP, index),(LPARAM)pSelData->hBmpNormal);
        index = SendMessage(hWndList, LB_ADDSTRING, 0, (LPARAM)STR_MMS_MAXTIME);
		SendMessage(hWndList, LB_SETIMAGE, 
				MAKEWPARAM(IMAGE_BITMAP, index),(LPARAM)pSelData->hBmpNormal);
        break;

    case IDC_MMS_PICSIZE:
        index = SendMessage(hWndList, LB_ADDSTRING, 0, (LPARAM)STR_MMS_SMALL);
		SendMessage(hWndList, LB_SETIMAGE, 
				MAKEWPARAM(IMAGE_BITMAP, index),(LPARAM)pSelData->hBmpNormal);
        index = SendMessage(hWndList, LB_ADDSTRING, 0, (LPARAM)STR_MMS_LARGE);
		SendMessage(hWndList, LB_SETIMAGE, 
				MAKEWPARAM(IMAGE_BITMAP, index),(LPARAM)pSelData->hBmpNormal);
        break;
    }

	if (pSelData->id == IDC_MMS_CONN)
	{
		SendMessage(hWndList, LB_SETCURSEL, nCursel, 0);
		//if (nCursel != 0)
		SendMessage(hWndList, LB_SETIMAGE, 
			MAKEWPARAM(IMAGE_BITMAP, nCursel),(LPARAM)pSelData->hBmpSel);
	}
	else
	{
		SendMessage(hWndList, LB_SETCURSEL, pSelData->nCurSel, 0);
		SendMessage(hWndList, LB_SETIMAGE, 
			MAKEWPARAM(IMAGE_BITMAP, pSelData->nCurSel),(LPARAM)pSelData->hBmpSel);
	}

    return TRUE;
}


static int MMSSetSel_OnConn(HWND hSelList, HWND hSpinConn)
{
	int index = 0, ret = 0, nSpace = 0;
	int ispid = 0;
	UDB_ISPINFO		ispinfo;
	MMSSETTING      mmsSet;
	DWORD			dwOctet[4] = {0};

	index = SendMessage(hSelList, LB_GETCURSEL, 0, 0);
	if (index == -1 || index == 0)
		nSpace = 1;
	ispid = SendMessage(hSelList, LB_GETITEMDATA, index, 0);
	if (FALSE == IspReadInfoByID(&ispinfo, ispid))
		nSpace = 1;

	MMS_GetSetting(&mmsSet);

	if ( 1== nSpace)
	{
		mmsSet.ISPID = 0;
		mmsSet.DtType = 0;
		mmsSet.ISPName[0] = 0;
		mmsSet.ISPPassword[0] = 0;
		mmsSet.ISPUserName[0] = 0;
		mmsSet.ISPPhoneNum1[0] = 0;
		mmsSet.szUrl[0] = 0;
		mmsSet.port = 0;		
		mmsSet.szIp[0] = 0;
	}
	else
	{
		mmsSet.ISPID = ispid;
		mmsSet.DtType = ispinfo.DtType;
		strcpy(mmsSet.ISPName, ispinfo.ISPName);
		strcpy(mmsSet.ISPPassword, ispinfo.ISPPassword);
		strcpy(mmsSet.ISPUserName, ispinfo.ISPUserName);
		strcpy(mmsSet.ISPPhoneNum1, ispinfo.ISPPhoneNum1);
		strcpy(mmsSet.szUrl, ispinfo.ConnectHomepage);
		mmsSet.port = atoi(ispinfo.ConnPort);
		dwOctet[0]=FIRST_IPADDRESS(ispinfo.ConnectGateway);
		dwOctet[1]=SECOND_IPADDRESS(ispinfo.ConnectGateway);
		dwOctet[2]=THIRD_IPADDRESS(ispinfo.ConnectGateway);
		dwOctet[3]=FOURTH_IPADDRESS(ispinfo.ConnectGateway);
		sprintf(mmsSet.szIp, "%03d.%03d.%03d.%03d",dwOctet[0],dwOctet[1],
			dwOctet[2],dwOctet[3]);
	}
	
	SetConnectionUsage(DIALDEF_MMS, index >= 0? (index - 1) : -1);

	MMS_SaveSetting(&mmsSet);
    MMSPro_ModifySetting(&mmsSet);

	ret = (mmsSet.ISPName[0] != 0);
	if (ret == 0)
	{
		SendMessage(hSpinConn, SSBM_DELETESTRING, 0, (LPARAM)""); //0
		SendMessage(hSpinConn, SSBM_ADDSTRING, 0, (LPARAM)"Required"); //0
	}
	else
	{
		SendMessage(hSpinConn, SSBM_DELETESTRING, 0, (LPARAM)""); //0
		SendMessage(hSpinConn, SSBM_ADDSTRING, 0, (LPARAM)mmsSet.ISPName);
	}
	return ret;
}
// on keydown
static BOOL MMSSetSel_OnKeyDown(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
    HWND  hSelList;
	HWND  hSpinConn;
    int   index, ret;
    PMMSSET_SELCREATEDATA  pSelData;

    hSelList = GetDlgItem(hWnd, IDC_MMSSETSEL_LIST);
    pSelData = GetUserData(hWnd);

    switch(LOWORD(wParam))
    {
	case VK_F10:
        PostMessage(hWnd, WM_CLOSE, 0, 0);
		break;

    case VK_F5:
        index = SendMessage(hSelList, LB_GETCURSEL, 0, 0);
		if (pSelData->id == IDC_MMS_CONN)
		{			
			hSpinConn = GetDlgItem(pSelData->hWndCall, IDC_MMS_CONN);
			ret = MMSSetSel_OnConn(hSelList, hSpinConn);			
		}
        PostMessage(pSelData->hWndCall, pSelData->msg, pSelData->id, (LPARAM)index);
        PostMessage(hWnd, WM_CLOSE, 0, 0);
    	break;

    default:
        return PDADefWindowProc(hWnd, WM_KEYDOWN, wParam, lParam);
    }

    return TRUE;
}
/*********************************************************************\
* Function	MMSSetSelWndProc
* Purpose   on select window proc
* Params
*			hWnd: Handle of the window
*			wMsgCmd: Memmmmssage ID
* Return
*			TRUE: Success
*			FALSE: Fail
* Remarks
**********************************************************************/
static LRESULT MMSSetSelWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    LRESULT lResult = TRUE;
    HDC     hdc;

	switch (wMsgCmd)
    {
    case WM_CREATE:
        lResult = MMSSetSel_OnCreate(hWnd,(LPCREATESTRUCT)(lParam));
        break;

    case WM_ACTIVATE:
	case PWM_SHOWWINDOW:
        {
            HWND hSelList;
			PMMSSET_SELCREATEDATA pSelData;

			pSelData = GetUserData(hWnd);
			
            hSelList = GetDlgItem(hWnd, IDC_MMSSETSEL_LIST);
            SetFocus(hSelList);

			SetWindowText(pSelData->hWndFrame, ML(title[pSelData->id%100-1]));
			SendMessage(pSelData->hWndFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)ML(""));         
			SendMessage(pSelData->hWndFrame, PWM_SETBUTTONTEXT, 0, (LPARAM)ML("Cancel"));
			SendMessage(pSelData->hWndFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
        }
        break;
        
	case WM_SETFOCUS:
		SetFocus(GetDlgItem(hWnd, IDC_MMSSETSEL_LIST));
		break;

    case WM_PAINT:        
        hdc = BeginPaint(hWnd, NULL);        
        EndPaint(hWnd, NULL);
        break;
 
    case WM_KEYDOWN:
        MMSSetSel_OnKeyDown(hWnd,wParam,lParam);
        break;

    case WM_CLOSE:
        DestroyWindow(hWnd);
        break;

    case WM_DESTROY:
	{
		PMMSSET_SELCREATEDATA		pSelData;

		pSelData = GetUserData(hWnd);

		DeleteObject(pSelData->hBmpNormal);
		DeleteObject(pSelData->hBmpSel);
		SendMessage(pSelData->hWndFrame, PWM_CLOSEWINDOW,(WPARAM)hWnd,0);
        UnregisterClass("MmsSetSelClassName", NULL);
        break;
	}

    default:     
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
	}

    return lResult;

}

void Mms_ChangeConnect(void)
{
	MmsSet_ModifyConnection();
}

static BOOL MmsSet_ModifyConnection(void)
{
	UDB_ISPINFO		ispinfo;
	MMSSETTING      mmsSet;
	DWORD			dwOctet[4] = {0};
	BOOL nSpace;

	MMS_GetSetting(&mmsSet);

	if (FALSE == IspReadInfoByID(&ispinfo, mmsSet.ISPID))
		nSpace = 1;

	if ( 1== nSpace)
	{
		mmsSet.ISPID = 0;
		mmsSet.DtType = 0;
		mmsSet.ISPName[0] = 0;
		mmsSet.ISPPassword[0] = 0;
		mmsSet.ISPUserName[0] = 0;
		mmsSet.ISPPhoneNum1[0] = 0;
		mmsSet.szUrl[0] = 0;
		mmsSet.port = 0;		
		mmsSet.szIp[0] = 0;
	}
	else
	{
		mmsSet.DtType = ispinfo.DtType;
		strcpy(mmsSet.ISPName, ispinfo.ISPName);
		strcpy(mmsSet.ISPPassword, ispinfo.ISPPassword);
		strcpy(mmsSet.ISPUserName, ispinfo.ISPUserName);
		strcpy(mmsSet.ISPPhoneNum1, ispinfo.ISPPhoneNum1);
		strcpy(mmsSet.szUrl, ispinfo.ConnectHomepage);
		mmsSet.port = atoi(ispinfo.ConnPort);
		dwOctet[0]=FIRST_IPADDRESS(ispinfo.ConnectGateway);
		dwOctet[1]=SECOND_IPADDRESS(ispinfo.ConnectGateway);
		dwOctet[2]=THIRD_IPADDRESS(ispinfo.ConnectGateway);
		dwOctet[3]=FOURTH_IPADDRESS(ispinfo.ConnectGateway);
		sprintf(mmsSet.szIp, "%03d.%03d.%03d.%03d",dwOctet[0],dwOctet[1],
			dwOctet[2],dwOctet[3]);
	}

	MMS_SaveSetting(&mmsSet);
    MMSPro_ModifySetting(&mmsSet);

	return TRUE;
}
