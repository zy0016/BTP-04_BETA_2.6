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
#include "smsglobal.h"

#define IDC_OK      100

#define SMS_DETAIL_POSITION  0,60,176,160
#define SMS_DETAIL_SUBLEN    (50+1)

#define IDC_SMSVIEW_LIST    300

typedef struct tagSMS_Detail
{
	int		Stat;
    char*   pszPhone;
    char*   pszReci;
    DWORD   dwDateTime;
    int     nSize;
    int     nMsgType;
    char    szContent[SMS_DETAIL_SUBLEN];
}SMS_DETAIL, *PSMS_DETAIL;

static HWND hDetailWnd = NULL;

LRESULT SMSDetailWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static BOOL SMSDetail_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct);
static void SMSDetail_OnActivate(HWND hwnd, UINT state);
static void SMSDetail_OnSetFocus(HWND hWnd);
static void SMSDetail_OnPaint(HWND hWnd);
static void SMSDetail_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags);
static void SMSDetail_OnCommand(HWND hWnd, int id, UINT codeNotify);
static void SMSDetail_OnDestroy(HWND hWnd);
static void SMSDetail_OnClose(HWND hWnd);
static void SMS_FormatDetail(HWND hWnd,PSMS_DETAIL psmsdetail,PSMS_STORE psms_store);

/*********************************************************************\
* Function	   SMS_DetailRegisterClass
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL SMS_DetailRegisterClass(void)
{
    WNDCLASS wc;
        
    wc.style         = 0;
    wc.lpfnWndProc   = SMSDetailWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = sizeof(SMS_DETAIL);
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName  = "SMSDetailWndClass";
    
    if (!RegisterClass(&wc))
        return FALSE;

    return TRUE;
}
/*********************************************************************\
* Function	   SMS_CreateDetailWnd
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL SMS_CreateDetailWnd(HWND hFrameWnd,SMS_STORE* psms_store)
{
    SMS_DETAIL smsdetail;
	RECT	 rcClient;
        
    SMS_DetailRegisterClass();

    memset(&smsdetail,0,sizeof(SMS_DETAIL));

    SMS_FormatDetail(hFrameWnd,&smsdetail,psms_store);

	GetClientRect(hFrameWnd,&rcClient);

    hDetailWnd = CreateWindow(
        "SMSDetailWndClass", 
        "",
        WS_VISIBLE | WS_CHILD,
        rcClient.left,
		rcClient.top,
		rcClient.right - rcClient.left,
		rcClient.bottom - rcClient.top,
        hFrameWnd,
        NULL,
        NULL, 
        (PVOID)&smsdetail
        );
    
    if (!hDetailWnd)
    {
        return FALSE;
    }
          
	SetFocus(hDetailWnd);

	SetWindowText(hFrameWnd,IDS_MSGINFO);

    SendMessage(hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_CANCEL,0), (LPARAM)"");
    SendMessage(hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_OK,1), (LPARAM)IDS_OK);
    SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"");

    SendMessage(hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON),(LPARAM)NULL);
    SendMessage(hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON),(LPARAM)NULL);

    return TRUE;
}

/*********************************************************************\
* Function	SMSDetailWndProc
* Purpose   Main window proc
* Params
*			hWnd: Handle of the window
*			wMsgCmd: Message ID
* Return
*			TRUE: Success
*			FALSE: Fail
* Remarks
**********************************************************************/
static LRESULT SMSDetailWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    LRESULT lResult = TRUE;

	switch (wMsgCmd)
    {
    case WM_CREATE:
        lResult = SMSDetail_OnCreate(hWnd,(LPCREATESTRUCT)(lParam));
        break;

    case WM_ACTIVATE:
	case PWM_SHOWWINDOW:
        SMSDetail_OnActivate(hWnd,(UINT)LOWORD(wParam));
        break;
        
    case WM_SETFOCUS:
        SMSDetail_OnSetFocus(hWnd);
        break;

    case WM_PAINT:
        SMSDetail_OnPaint(hWnd);
        break;

    case WM_KEYDOWN:
        SMSDetail_OnKey(hWnd,(UINT)(wParam),(int)(short)LOWORD(lParam),(UINT)HIWORD(lParam));
        break;

    case WM_COMMAND:
        SMSDetail_OnCommand(hWnd,(int)(LOWORD(wParam)),(UINT)HIWORD(wParam));
        break;
        
    case WM_CLOSE:
        SMSDetail_OnClose(hWnd);
        break;

    case WM_DESTROY:
        SMSDetail_OnDestroy(hWnd);
        break;

    default:     
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
	}

    return lResult;

}
/*********************************************************************\
* Function	SMSDetail_OnCreate
* Purpose   WM_CREATE message handler of the main window
* Params
*			hWnd: Handle of the window
*			lpCreateStruct: Create Structure
* Return
*			TRUE: Success
*			FALSE: Fail
* Remarks
**********************************************************************/
static BOOL SMSDetail_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
{    
    PSMS_DETAIL pCreateData;
    HWND hFromView = NULL;
    int  nIndex = 0,nKilobyte = 0;
    SYSTEMTIME sy;
    char szData[20],szTime[20],szSize[20];
    RECT rect;
    LISTBUF Listtmp;
    char  Array[SMS_RECMAX*102];
    int   nCount = 0;
    ABNAMEOREMAIL ABName;
    char* p1,*p;
    int   nlen,i;
    char* pType = NULL;

    pCreateData = GetUserData(hWnd);

    memcpy(pCreateData,lpCreateStruct->lpCreateParams,sizeof(SMS_DETAIL));
    
    GetClientRect(hWnd,&rect);
        
	PREBROW_InitListBuf(&Listtmp);
    
    memset(Array,0,102*SMS_RECMAX);

    p1 = p = pCreateData->pszPhone;
    
    while(*p !='\0')
    {
        if(*p == ';')
        {
            *p = '\0';
                        
            if(strlen(p1) > ME_PHONENUM_LEN-1) //now
                continue;
            
            memset(&ABName,0,sizeof(ABNAMEOREMAIL));
            
            if(APP_GetNameByPhone(p1,&ABName))
                strcpy(Array+(nCount*102),ABName.szName);
            else
                strcpy(Array+(nCount*102),p1);

            nCount++;
                        
            p1 = p+1;
            if( *p1 == '\0')
                break;
        }
        p++;
    }
    
    if(*p1 != '\0')
    {   
        memset(&ABName,0,sizeof(ABNAMEOREMAIL));
        
        if(APP_GetNameByPhone(p1,&ABName))
            strcpy(Array+(nCount*102),ABName.szName);
        else
            strcpy(Array+(nCount*102),p1);
        
        nCount++;
    }

    nlen = 0;

    for(i = 0 ; i < nCount ; i++)
    {
        nlen += strlen(Array+(i*102));
        nlen += 1;
    }
    
    pCreateData->pszReci = (char*)malloc(nlen+1);

    if(pCreateData->pszReci == NULL)
        return FALSE;

    memset(pCreateData->pszReci,0,nlen+1);
    
    for(i = 0 ; i < nCount ; i++)
    {
        strcat(pCreateData->pszReci,Array+(i*102));
        strcat(pCreateData->pszReci,";");
    }
    pCreateData->pszReci[nlen] = '\0';

    if(pCreateData->Stat == MU_STU_UNREAD || pCreateData->Stat == MU_STU_READ)
        PREBROW_AddData(&Listtmp, IDS_FROM, pCreateData->pszReci);
    else
        PREBROW_AddData(&Listtmp, IDS_TO, pCreateData->pszReci);
    
    if(pCreateData->nMsgType == MU_MSG_SMS)
        PREBROW_AddData(&Listtmp, IDS_SUBJECT, pCreateData->szContent);
    else if(pCreateData->nMsgType == MU_MSG_VCARD)
        PREBROW_AddData(&Listtmp, IDS_SUBJECT, IDS_VCARD);
    else if(pCreateData->nMsgType == MU_MSG_VCAL)
        PREBROW_AddData(&Listtmp, IDS_SUBJECT, IDS_VCAL);
    
    memset(&sy,0,sizeof(SYSTEMTIME));
    sy.wYear = (WORD)INTYEAR(pCreateData->dwDateTime);
    sy.wMonth = (WORD)INTMONTH(pCreateData->dwDateTime);
    sy.wDay = (WORD)INTDAY(pCreateData->dwDateTime);
    sy.wHour = (WORD)INTHOUR(pCreateData->dwDateTime);
    sy.wMinute = (WORD)INTMINUTE(pCreateData->dwDateTime);
    sy.wSecond = (WORD)INTSECOND(pCreateData->dwDateTime);
    
    szData[0] = 0;
    szTime[0] = 0;
    GetTimeDisplay(sy,szTime,szData);

    PREBROW_AddData(&Listtmp, IDS_DATE, szData);
    PREBROW_AddData(&Listtmp, IDS_TIME, szTime);
    
    if(pCreateData->nMsgType == MU_MSG_SMS)
        pType = (char*)IDS_SMS;
    else if(pCreateData->nMsgType == MU_MSG_VCARD)
        pType = (char*)IDS_VCARD;
    else if(pCreateData->nMsgType == MU_MSG_VCAL)
        pType = (char*)IDS_VCAL;

    PREBROW_AddData(&Listtmp, IDS_TYPE, pType);

    szSize[0] = 0;
    nKilobyte = pCreateData->nSize/1024;
    if(nKilobyte > 0)
        sprintf(szSize,"%d kB",nKilobyte);
    else
    {
        nKilobyte = pCreateData->nSize * 10 /1024;
        if(nKilobyte > 0)
            sprintf(szSize,"0.%d kB",nKilobyte);
        else
        {
            nKilobyte = pCreateData->nSize * 100 /1024;
            sprintf(szSize,"0.0%d kB",nKilobyte);
        }
    }

    PREBROW_AddData(&Listtmp, IDS_SIZE, szSize);
    
	hFromView = CreateWindow(
        FORMVIEWER,
        "",
		WS_VISIBLE  |WS_CHILD | WS_VSCROLL,
        rect.left,
        rect.top,
        rect.right - rect.left,
        rect.bottom - rect.top,
		hWnd,
        (HMENU)IDC_SMSVIEW_LIST,
        NULL, 
        (PVOID)&Listtmp);

	if (hFromView == NULL) 
		return FALSE;

    return TRUE;
    
}
/*********************************************************************\
* Function	SMSDetail_OnActivate
* Purpose   WM_ACTIVATE message handler of the main window
* Params
* Return    None
* Remarks
**********************************************************************/
static void SMSDetail_OnActivate(HWND hWnd, UINT state)
{
    HWND hList;
	HWND hFrameWnd;

    hList = GetDlgItem(hWnd,IDC_SMSVIEW_LIST);
        
	SetFocus(hList);

	hFrameWnd = GetParent(hWnd);

	SetWindowText(hFrameWnd,IDS_MSGINFO);

    SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)"");
    SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_OK);
    SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"");

    SendMessage(hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON),(LPARAM)NULL);
    SendMessage(hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON),(LPARAM)NULL);

    return;
}
/*********************************************************************\
* Function	SMSDetail_OnSetFocus
* Purpose   WM_ACTIVATE message handler of the main window
* Params
* Return    None
* Remarks
**********************************************************************/
static void SMSDetail_OnSetFocus(HWND hWnd)
{
    HWND hList;

    hList = GetDlgItem(hWnd,IDC_SMSVIEW_LIST);
        
	SetFocus(hList);

    return;
}
/*********************************************************************\
* Function	SMSDetail_OnPaint
* Purpose   WM_PAINT message handler of the main window
* Params	hWnd: Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void SMSDetail_OnPaint(HWND hWnd)
{
	HDC hdc = BeginPaint(hWnd, NULL);

	EndPaint(hWnd, NULL);

	return;
}

/*********************************************************************\
* Function	SMSDetail_OnKey
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
static void SMSDetail_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags)
{
	switch (vk)
	{
    case VK_RETURN:
		PostMessage(hWnd,WM_CLOSE,NULL,NULL);
        break;

	default:
		PDADefWindowProc(hWnd, WM_KEYDOWN, vk, MAKELPARAM(cRepeat, flags));
		break;
	}

	return;
}
/*********************************************************************\
* Function	SMSDetail_OnCommand
* Purpose   WM_COMMAND message handler of the main window
* Params
*			hWnd:	Handle of the window
*			id:		Id of command message
*			hwndCtl: Handle of the window which sended this message
*			codeNotify:Notify code of the message
* Return	None
* Remarks
**********************************************************************/
static void SMSDetail_OnCommand(HWND hWnd, int id, UINT codeNotify)
{
	switch(id)
	{        
    default:
        break;
	}

	return;
}
/*********************************************************************\
* Function	SMSDetail_OnDestroy
* Purpose   WM_DESTROY message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void SMSDetail_OnDestroy(HWND hWnd)
{
    PSMS_DETAIL pCreateData;

    pCreateData = GetUserData(hWnd);

    SMS_FREE(pCreateData->pszPhone);

    SMS_FREE(pCreateData->pszReci);

    hDetailWnd = NULL;

    return;

}
/*********************************************************************\
* Function	SMSDetail_OnClose
* Purpose   WM_CLOSE message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void SMSDetail_OnClose(HWND hWnd)
{

	SendMessage(GetParent(hWnd),PWM_CLOSEWINDOW,(WPARAM)hWnd,NULL);

    DestroyWindow (hWnd);
	
    return;

}
/*********************************************************************\
* Function	 SMS_FormatDetail  
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void SMS_FormatDetail(HWND hWnd,PSMS_DETAIL psmsdetail,PSMS_STORE psms_store)
{
    HDC hdc;
    int nConLen = 0;
    int nReciLen = 0;

    psmsdetail->Stat = psms_store->fix.Stat;
    psmsdetail->dwDateTime = psms_store->fix.dwDateTime;

    hdc = GetDC(hWnd);
    
    GetExtentFittedText(hdc,psms_store->pszContent,-1,psmsdetail->szContent,SMS_DETAIL_SUBLEN,
        TEXTDETAIL_WIDTH,OMIT_SUFFIX,SUFFIX_REPEAT);
    
    ReleaseDC(hWnd,hdc);
    
    psmsdetail->szContent[SMS_DETAIL_SUBLEN-1] = 0;

    psmsdetail->pszPhone = malloc(psms_store->fix.Phonelen);
    if(psmsdetail->pszPhone)
        strcpy(psmsdetail->pszPhone,psms_store->pszPhone);

    if(psms_store->pszContent)
        nConLen = strlen(psms_store->pszContent);

    if(psms_store->pszPhone)
        nReciLen = strlen(psms_store->pszPhone);

    if( (nConLen+nReciLen) == 0)
        psmsdetail->nSize = 0;
    else
        psmsdetail->nSize = sizeof(SMS_STOREFIX) + psms_store->fix.Conlen 
        + psms_store->fix.Udhlen + psms_store->fix.Phonelen;

    psmsdetail->nMsgType = SMS_IsVcradOrVcal(psms_store);

    return;
}
