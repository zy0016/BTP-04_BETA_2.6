

#include    "winpda.h"
#include    "stdlib.h "
#include    "mullang.h"
#include    "plx_pdaex.h"
#include    "hpimage.h"
#include    "MBPublic.h"
#include    "stdio.h"
#include    "MB_control.h"
#include    "MBCallEnd.h"

#define     TIMER_ID_FRESH          2
#define     IDC_TIMER               3
#define     IDC_BUTTON_OK           5
#define     TIMER_DURATION          2000
#define     TITLECAPTIONONE         ML("Callended") 
#define     DIALLING_FARE           ML("Reachcostlimit") 
#define     CALLTIME                ML("Calltime:") 
#define     STATUS_INFO_END         ML("Disconnected")
#define     UNKNOWNUMBER            ML("Unknow number")

static const char * pClassNameOne   = "MBCallOneEndWndClass";     
static const char * PicCallEnd      = "";

static const int    iSXSpace        = 10;
static const int    iSYSpace        = 5;
static HWND         hCallEndWnd;
static HINSTANCE    hInstanceOne;
static char         cName[PHONENUMMAXLEN + 1];
static char         cPhoneNumber[PHONENUMMAXLEN + 1];
static char         cLastTimer[DURATIONLEN+1];
static char         cCostInfo[COSTLEN+1];
static INITPAINTSTRUCT endif_initpaint;
static MBConfig mbconfig;
static BOOL  bIcon;
static HWND  hPreWnd;
static 	char  endshowinfo[ME_PHONENUM_LEN];
static 	char  endstatusinfo[20];
static  char  endtimeinfo[DURATIONLEN];
static  char  endcostinfo[COSTLEN];
static  PCALLENDNODE pEndNodeHead = NULL;
static  PCALLENDNODE pShowEndNode = NULL;
static  HGIFANIMATE hendGif;
static  RECT        rName;
static  RECT        rPhone;
static  RECT        rLastTimer;
static  RECT        rCostInfo;
static  SIZE        sCartoon; 

static  LRESULT AppWndProcOne( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
extern BOOL IsTimeChanged (void);
static BOOL CallEnd_OnCreat(HWND hWnd);
static void CallEnd_OnShowNextTimer(HWND hWnd);
extern void SetShowSummaryFlag(BOOL f);
extern BOOL GetShowSummaryFlag();

BOOL MBCallEndWindow(HWND hpre, const char * pName,const char * pPhoneCode,const char * pDuration,const char * pCost)
{

    WNDCLASS    wc;
    int         iSour;

//    if (GetUsedItemNum() != 0)
//        return FALSE;

    wc.style         = CS_OWNDC;
    wc.lpfnWndProc   = AppWndProcOne;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = pClassNameOne;

    DlmNotify(PMM_NEWS_ENABLE,PMF_MAIN_BKLIGHT);//点亮背光

    if (!RegisterClass(&wc))
    {
        return FALSE;
    }
    memset(cName        ,0x00,sizeof(cName       ));
    memset(cPhoneNumber ,0x00,sizeof(cPhoneNumber));
    memset(cLastTimer   ,0x00,sizeof(cLastTimer  ));
    memset(cCostInfo    ,0x00,sizeof(cCostInfo   ));

    if (pName != NULL)
    {
        iSour = strlen(pName);
        strncpy(cName,pName,sizeof(cName) > iSour ? iSour + 1 : sizeof(cName) - 1);
    }
    if (pPhoneCode != NULL)
    {
        iSour = strlen(pPhoneCode);
        strncpy(cPhoneNumber,pPhoneCode,sizeof(cPhoneNumber) > iSour ? iSour + 1 : sizeof(cPhoneNumber) - 1);
    }
    if (pDuration != NULL)
    {
        strcpy(cLastTimer,pDuration);
    }
    if (pCost != NULL)
    {
        iSour = strlen(pCost);
        strncpy(cCostInfo,pCost,sizeof(cCostInfo) > iSour ? iSour + 1 : sizeof(cCostInfo) - 1);
    }

    hCallEndWnd = CreateWindow(pClassNameOne,"", 
		WS_CAPTION|PWS_STATICBAR|PWS_NOTSHOWPI,  PLX_WIN_POSITION , 
        NULL, NULL, NULL, NULL);
	hPreWnd = hpre;
	Set_CallWndOrder(hPreWnd, hCallEndWnd);
	CALLWND_InsertDataByHandle(pGlobalCallListBuf, hCallEndWnd);
    if (NULL == hCallEndWnd)
    {
        UnregisterClass(pClassNameOne,NULL);
        return FALSE;
    }
    ShowWindow(hCallEndWnd, SW_SHOW);
    UpdateWindow(hCallEndWnd); 

    return (TRUE);
}
static LRESULT AppWndProcOne ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
    static  HGIFANIMATE hendGif;
    static  RECT        rName;
    static  RECT        rPhone;
    static  RECT        rLastTimer;
    static  RECT        rCostInfo;
	static  SIZE        sCartoon; 


            LRESULT     lResult;
            HDC         hdc;

    lResult = (LRESULT)TRUE;
    switch ( wMsgCmd )
    {
    case WM_CREATE :
		{
			SYSTEMTIME	SystemTime;
			char aTimeBuf[25], aDateBuf[25];
			
			GetLocalTime(&SystemTime);	
			GetTimeDisplay(SystemTime, aTimeBuf, aDateBuf);
			SetWindowText(hWnd, aTimeBuf);
			SendMessage(hWnd, PWM_SETAPPICON,MAKEWPARAM(IMAGE_BITMAP, POWERICON), 0);
			SendMessage(hWnd, PWM_SETAPPICON,MAKEWPARAM(IMAGE_BITMAP, SIGNALICON), 0);
			SetTimer(hWnd, TIMER_ID_FRESH, 5000, NULL);

			SetTimer(hWnd,IDC_TIMER,TIMER_DURATION,0);
			SendMessage(hWnd,PWM_SETBUTTONTEXT,0,(LPARAM)"");
			//		SendMessage(hWnd,PWM_SETBUTTONTEXT,1,(LPARAM)ML("Ok"));
			SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDC_BUTTON_OK,1),(LPARAM)ML("Ok"));
			ReadMobileConfigFile(&mbconfig);		
			if (mbconfig.ALS)
				bIcon = TRUE;
			else
				bIcon = FALSE;
			InitPaint(&endif_initpaint, FALSE, bIcon);
			hdc = GetDC(hWnd);
			GetImageDimensionFromFile(MBPIC_CALLEND,&sCartoon);
			hendGif = StartAnimatedGIFFromFile(hWnd,MBPIC_CALLEND,
				(GetScreenUsableWH1(SM_CXSCREEN) -sCartoon.cx) / 2,//endif_initpaint.subcapmid.left,
				endif_initpaint.subcapmid.top, DM_NONE);
			if (strlen(cName) == 0) 
			{
				if (strlen(cPhoneNumber)== 0) 
					sprintf(endshowinfo, UNKNOWNUMBER);
				else
					InitTxt(hdc,cPhoneNumber,endshowinfo, 176, FALSE);
			}
			else
				InitTxt(hdc,(LPSTR)cName,(LPSTR)endshowinfo, 176, TRUE);	
			ReleaseDC(hWnd,hdc);
			sprintf(endstatusinfo, STATUS_INFO_END);
			if (strlen(cLastTimer) != 0) 
				sprintf(endtimeinfo, cLastTimer);
			else
				strcpy(endtimeinfo, "");	
			if (strlen(cCostInfo) != 0)
				sprintf(endcostinfo, cCostInfo);
			else
				strcpy(endcostinfo, "");
			InvalidateRect(hWnd,NULL,TRUE);	
		}
        break;
		
    case WM_TIMER:
        switch (wParam)
        {
		case TIMER_ID_FRESH:
			{
				SYSTEMTIME	SystemTime;
				char aTimeBuf[25], aDateBuf[25];
				//update time
				if(IsTimeChanged())
				{
					GetLocalTime(&SystemTime);	
					GetTimeDisplay(SystemTime, aTimeBuf, aDateBuf);
					SetWindowText(hWnd, aTimeBuf);
				}
			}
		break;
		
        case IDC_TIMER:
            DestroyWindow(hWnd);
            break;
        }
        break;
    case WM_DESTROY :
		CALLWND_DeleteDataByHandle(pGlobalCallListBuf, hCallEndWnd);
        EndAnimatedGIF(hendGif);
        KillTimer(hWnd,IDC_TIMER);
		hCallEndWnd = NULL;

        UnregisterClass(pClassNameOne,NULL);
//        DlmNotify ((WPARAM)PES_STCQUIT, (LPARAM)hInstanceOne );

        break;

    case WM_PAINT :
		{
			HBRUSH  hbrush;
			HBRUSH  holdbrush;
			HFONT   hfont;
			HFONT   holdfont;
//			char  showinfo[ME_PHONENUM_LEN];
//			char  statusinfo[20];
			
			hdc = BeginPaint(hWnd, NULL);
			hbrush = CreateBrush(BS_SOLID, RGB(166,202,240),0);
			holdbrush = SelectObject(hdc, hbrush);
			FillRect(hdc, &(endif_initpaint.subcap), hbrush);
			SelectObject(hdc, holdbrush);
			DeleteObject(hbrush);
			PaintAnimatedGIF(hdc,hendGif);

			if (GetFontHandle(&hfont, 1))
			{
				holdfont = SelectObject(hdc, hfont);
			}
			
			
			if ((strlen(endtimeinfo)!=0)&&(strlen(endcostinfo)!=0))
			{
				DrawText(hdc,endshowinfo,strlen(endshowinfo),&(endif_initpaint.line1),DT_HCENTER|DT_VCENTER);
				DrawText(hdc,endstatusinfo,strlen(endstatusinfo),&(endif_initpaint.line2),DT_HCENTER|DT_VCENTER);
				DrawText(hdc,endtimeinfo,strlen(endtimeinfo),&(endif_initpaint.line3),DT_HCENTER|DT_VCENTER);
				DrawText(hdc,endcostinfo,strlen(endcostinfo),&(endif_initpaint.line4),DT_HCENTER|DT_VCENTER);
			}
			else if((strlen(endtimeinfo)!=0)&&(strlen(endcostinfo)==0))
			{
				DrawText(hdc,endshowinfo,strlen(endshowinfo),&(endif_initpaint.line1),DT_HCENTER|DT_VCENTER);
				DrawText(hdc,endstatusinfo,strlen(endstatusinfo),&(endif_initpaint.line2),DT_HCENTER|DT_VCENTER);
				DrawText(hdc,endtimeinfo,strlen(endtimeinfo),&(endif_initpaint.line3),DT_HCENTER|DT_VCENTER);
			}
			else if((strlen(endtimeinfo)==0)&&(strlen(endcostinfo)!=0))
			{
				DrawText(hdc,endshowinfo,strlen(endshowinfo),&(endif_initpaint.line1),DT_HCENTER|DT_VCENTER);
				DrawText(hdc,endstatusinfo,strlen(endstatusinfo),&(endif_initpaint.line2),DT_HCENTER|DT_VCENTER);
				DrawText(hdc,endcostinfo,strlen(endcostinfo),&(endif_initpaint.line3),DT_HCENTER|DT_VCENTER);
			}
			else if((strlen(endtimeinfo)==0)&&(strlen(endcostinfo)==0))
			{
				DrawText(hdc,endshowinfo,strlen(endshowinfo),&(endif_initpaint.line2),DT_HCENTER|DT_VCENTER);
				DrawText(hdc,endstatusinfo,strlen(endstatusinfo),&(endif_initpaint.line3),DT_HCENTER|DT_VCENTER);
			}		
			if (GetFontHandle(&hfont, 1))
			{
				SelectObject(hdc, holdfont);
			}
			
			EndPaint(hWnd, NULL);
		}
        break;

	case WM_KEYDOWN:
		switch(LOWORD(wParam))
		{
		case VK_RETURN:
			DestroyWindow(hCallEndWnd);
			break;
		}
		break;

    case WM_TODESKTOP:
        DestroyWindow(hWnd);
        return FALSE;
    default :
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
    return lResult;
}

static const char * pEndClassName   = "MBCallEndWndClass";
static HWND hCallEndNodeWnd;
static  LRESULT CallEndNodeWndProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
BOOL  MBCallEndNodeWindow()
{

    WNDCLASS    wc;

//    if (GetUsedItemNum() != 0)
//        return FALSE;

    wc.style         = CS_OWNDC;
    wc.lpfnWndProc   = CallEndNodeWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = pEndClassName;

    DlmNotify(PMM_NEWS_ENABLE,PMF_MAIN_BKLIGHT);//点亮背光

    if (!RegisterClass(&wc))
    {
        return FALSE;
    }
    hCallEndNodeWnd = CreateWindow(pEndClassName,"", 
		WS_CAPTION|PWS_STATICBAR|PWS_NOTSHOWPI,  PLX_WIN_POSITION , 
        NULL, NULL, NULL, NULL);

	CALLWND_InsertDataByHandle(pGlobalCallListBuf, hCallEndNodeWnd);
    if (NULL == hCallEndNodeWnd)
    {
        UnregisterClass(pClassNameOne,NULL);
        return FALSE;
    }
	if (GetShowSummaryFlag())
		SetShowSummaryFlag(FALSE);
	else
	{
		ShowWindow(hCallEndNodeWnd, SW_SHOW);
		UpdateWindow(hCallEndNodeWnd); 
	}
    return (TRUE);
}

static LRESULT CallEndNodeWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{

	LRESULT     lResult;
	HDC         hdc;
	
    lResult = (LRESULT)TRUE;
    switch ( wMsgCmd )
    {
    case WM_CREATE :
		CallEnd_OnCreat(hWnd);
        break;
		
    case WM_TIMER:
        switch (wParam)
        {
		case TIMER_ID_FRESH:
			{
				SYSTEMTIME	SystemTime;
				char aTimeBuf[25], aDateBuf[25];
				//update time
				if(IsTimeChanged())
				{
					GetLocalTime(&SystemTime);	
					GetTimeDisplay(SystemTime, aTimeBuf, aDateBuf);
					SetWindowText(hWnd, aTimeBuf);
				}
			}
		break;
		
        case IDC_TIMER:
			CallEnd_OnShowNextTimer(hWnd);
            //DestroyWindow(hWnd);
            break;
        }
        break;

	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;

    case WM_DESTROY :
		{
			PCALLENDNODE ptmp;
			KillTimer(hWnd,IDC_TIMER);
			ptmp = pEndNodeHead;
			while (pEndNodeHead)
			{
				pEndNodeHead = ptmp->pNext;
				free(ptmp);
				ptmp = pEndNodeHead;
			}
			pEndNodeHead = NULL;
			pShowEndNode = NULL;
			CALLWND_DeleteDataByHandle(pGlobalCallListBuf, hCallEndNodeWnd);
			EndAnimatedGIF(hendGif);
			hCallEndWnd = NULL;
			UnregisterClass(pEndClassName,NULL);
		}
        break;

    case WM_PAINT :
		{
			HBRUSH  hbrush;
			HBRUSH  holdbrush;
			HFONT   hfont;
			HFONT   holdfont;
//			char  showinfo[ME_PHONENUM_LEN];
//			char  statusinfo[20];
			
			hdc = BeginPaint(hWnd, NULL);
			hbrush = CreateBrush(BS_SOLID, RGB(166,202,240),0);
			holdbrush = SelectObject(hdc, hbrush);
			FillRect(hdc, &(endif_initpaint.subcap), hbrush);
			SelectObject(hdc, holdbrush);
			DeleteObject(hbrush);
			PaintAnimatedGIF(hdc,hendGif);

			if (GetFontHandle(&hfont, 1))
			{
				holdfont = SelectObject(hdc, hfont);
			}
			if ((strlen(endtimeinfo)!=0)&&(strlen(endcostinfo)!=0))
			{
				DrawText(hdc,endshowinfo,strlen(endshowinfo),&(endif_initpaint.line1),DT_HCENTER|DT_VCENTER);
				DrawText(hdc,endstatusinfo,strlen(endstatusinfo),&(endif_initpaint.line2),DT_HCENTER|DT_VCENTER);
				DrawText(hdc,endtimeinfo,strlen(endtimeinfo),&(endif_initpaint.line3),DT_HCENTER|DT_VCENTER);
				DrawText(hdc,endcostinfo,strlen(endcostinfo),&(endif_initpaint.line4),DT_HCENTER|DT_VCENTER);
			}
			else if((strlen(endtimeinfo)!=0)&&(strlen(endcostinfo)==0))
			{
				DrawText(hdc,endshowinfo,strlen(endshowinfo),&(endif_initpaint.line1),DT_HCENTER|DT_VCENTER);
				DrawText(hdc,endstatusinfo,strlen(endstatusinfo),&(endif_initpaint.line2),DT_HCENTER|DT_VCENTER);
				DrawText(hdc,endtimeinfo,strlen(endtimeinfo),&(endif_initpaint.line3),DT_HCENTER|DT_VCENTER);
			}
			else if((strlen(endtimeinfo)==0)&&(strlen(endcostinfo)!=0))
			{
				DrawText(hdc,endshowinfo,strlen(endshowinfo),&(endif_initpaint.line1),DT_HCENTER|DT_VCENTER);
				DrawText(hdc,endstatusinfo,strlen(endstatusinfo),&(endif_initpaint.line2),DT_HCENTER|DT_VCENTER);
				DrawText(hdc,endcostinfo,strlen(endcostinfo),&(endif_initpaint.line3),DT_HCENTER|DT_VCENTER);
			}
			else if((strlen(endtimeinfo)==0)&&(strlen(endcostinfo)==0))
			{
				DrawText(hdc,endshowinfo,strlen(endshowinfo),&(endif_initpaint.line2),DT_HCENTER|DT_VCENTER);
				DrawText(hdc,endstatusinfo,strlen(endstatusinfo),&(endif_initpaint.line3),DT_HCENTER|DT_VCENTER);
			}		

/*
			if (strlen(cName) == 0) 
			{
				if (strlen(cPhoneNumber)== 0) 
					sprintf(showinfo, UNKNOWNUMBER);
				else
					InitTxt(hdc,cPhoneNumber,showinfo, 176, FALSE);
			}
			else
				InitTxt(hdc,cName,showinfo, 176, TRUE);
			if (GetFontHandle(&hfont, 1))
			{
				holdfont = SelectObject(hdc, hfont);
			}
			DrawText(hdc,showinfo,strlen(showinfo),&(endif_initpaint.line2),DT_HCENTER|DT_VCENTER);
			sprintf(statusinfo, STATUS_INFO_END);
			DrawText(hdc,statusinfo,strlen(statusinfo),&(endif_initpaint.line3),DT_HCENTER|DT_VCENTER);
			*/

			if (GetFontHandle(&hfont, 1))
			{
				SelectObject(hdc, holdfont);
			}
			
			EndPaint(hWnd, NULL);
		}
        break;

	case WM_KEYDOWN:
		switch(LOWORD(wParam))
		{
		case VK_RETURN:
			CallEnd_OnShowNextTimer(hWnd);
//			DestroyWindow(hCallEndWnd);
			break;
		}
		break;

    default :
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
    return lResult;
}

static BOOL CallEnd_OnCreat(HWND hWnd)
{
	HDC hdc;
	SYSTEMTIME	SystemTime;
	char aTimeBuf[25], aDateBuf[25];
	int iSour;

	if (NULL ==pEndNodeHead) 
		return FALSE;
	else
		pShowEndNode = pEndNodeHead;

	memset(cName        ,0x00,sizeof(cName       ));
    memset(cPhoneNumber ,0x00,sizeof(cPhoneNumber));
    memset(cLastTimer   ,0x00,sizeof(cLastTimer  ));
    memset(cCostInfo    ,0x00,sizeof(cCostInfo   ));
	
	iSour = strlen(pShowEndNode->dspName);
	strncpy(cName,pShowEndNode->dspName,sizeof(cName) > iSour ? iSour + 1 : sizeof(cName) - 1);
	iSour = strlen(pShowEndNode->dspNumber);
	strncpy(cPhoneNumber,pShowEndNode->dspNumber,sizeof(cPhoneNumber) > iSour ? iSour + 1 : sizeof(cPhoneNumber) - 1);
	iSour = strlen(pShowEndNode->dspTime);
	strncpy(cLastTimer,pShowEndNode->dspTime,sizeof(cLastTimer) > iSour ? iSour + 1 : sizeof(cLastTimer) - 1);
	iSour = strlen(pShowEndNode->dspCost);
	strncpy(cCostInfo,pShowEndNode->dspCost,sizeof(cCostInfo) > iSour ? iSour + 1 : sizeof(cCostInfo) - 1);
	GetLocalTime(&SystemTime);	
	GetTimeDisplay(SystemTime, aTimeBuf, aDateBuf);
	SetWindowText(hWnd, aTimeBuf);
	SendMessage(hWnd, PWM_SETAPPICON,MAKEWPARAM(IMAGE_BITMAP, POWERICON), 0);
	SendMessage(hWnd, PWM_SETAPPICON,MAKEWPARAM(IMAGE_BITMAP, SIGNALICON), 0);
	SetTimer(hWnd, TIMER_ID_FRESH, 5000, NULL);
	
	SetTimer(hWnd,IDC_TIMER,TIMER_DURATION,0);
	SendMessage(hWnd,PWM_SETBUTTONTEXT,0,(LPARAM)"");
	//		SendMessage(hWnd,PWM_SETBUTTONTEXT,1,(LPARAM)ML("Ok"));
	SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDC_BUTTON_OK,1),(LPARAM)ML("Ok"));
	ReadMobileConfigFile(&mbconfig);		
	if (mbconfig.ALS)
		bIcon = TRUE;
	else
		bIcon = FALSE;
	InitPaint(&endif_initpaint, FALSE, bIcon);
	hdc = GetDC(hWnd);
	GetImageDimensionFromFile(MBPIC_CALLEND,&sCartoon);
	hendGif = StartAnimatedGIFFromFile(hWnd,MBPIC_CALLEND,
		(GetScreenUsableWH1(SM_CXSCREEN) - sCartoon.cx) / 2,//endif_initpaint.subcapmid.left,
		endif_initpaint.subcapmid.top, DM_NONE);
	if (strlen(cName) == 0) 
	{
		if (strlen(cPhoneNumber)== 0) 
			sprintf(endshowinfo, UNKNOWNUMBER);
		else
			InitTxt(hdc,cPhoneNumber,endshowinfo, 176, FALSE);
	}
	else
		InitTxt(hdc,(LPSTR)cName,(LPSTR)endshowinfo, 176, TRUE);	
	ReleaseDC(hWnd,hdc);
	sprintf(endstatusinfo, STATUS_INFO_END);
	if (strlen(cLastTimer) != 0) 
		sprintf(endtimeinfo, cLastTimer);
	else
		strcpy(endtimeinfo, "");	
	if (strlen(cCostInfo) != 0)
		sprintf(endcostinfo, cCostInfo);
	else
		strcpy(endcostinfo, "");
	InvalidateRect(hWnd,NULL,TRUE);	
	return TRUE;
}

static void CallEnd_OnShowNextTimer(HWND hWnd)
{
	HDC hdc;
	int iSour;

	KillTimer(hWnd, IDC_TIMER);
	if (NULL ==pShowEndNode->pNext) 
	{
		SendMessage(hWnd, WM_CLOSE, 0, 0);
		return;
	}
	else
		pShowEndNode = pShowEndNode->pNext;

	memset(cName        ,0x00,sizeof(cName       ));
    memset(cPhoneNumber ,0x00,sizeof(cPhoneNumber));
    memset(cLastTimer   ,0x00,sizeof(cLastTimer  ));
    memset(cCostInfo    ,0x00,sizeof(cCostInfo   ));
	
	iSour = strlen(pShowEndNode->dspName);
	strncpy(cName,pShowEndNode->dspName,sizeof(cName) > iSour ? iSour + 1 : sizeof(cName) - 1);
	iSour = strlen(pShowEndNode->dspNumber);
	strncpy(cPhoneNumber,pShowEndNode->dspNumber,sizeof(cPhoneNumber) > iSour ? iSour + 1 : sizeof(cPhoneNumber) - 1);
	iSour = strlen(pShowEndNode->dspTime);
	strncpy(cLastTimer,pShowEndNode->dspTime,sizeof(cLastTimer) > iSour ? iSour + 1 : sizeof(cLastTimer) - 1);
	iSour = strlen(pShowEndNode->dspCost);
	strncpy(cCostInfo,pShowEndNode->dspCost,sizeof(cCostInfo) > iSour ? iSour + 1 : sizeof(cCostInfo) - 1);
	
	SetTimer(hWnd,IDC_TIMER,TIMER_DURATION,0);
	ReadMobileConfigFile(&mbconfig);		
	if (mbconfig.ALS)
		bIcon = TRUE;
	else
		bIcon = FALSE;
	InitPaint(&endif_initpaint, FALSE, bIcon);
	hdc = GetDC(hWnd);
	GetImageDimensionFromFile(MBPIC_CALLEND,&sCartoon);
	hendGif = StartAnimatedGIFFromFile(hWnd,MBPIC_CALLEND,
		(GetScreenUsableWH1(SM_CXSCREEN) - sCartoon.cx) / 2,//endif_initpaint.subcapmid.left,
		endif_initpaint.subcapmid.top, DM_NONE);
	if (strlen(cName) == 0) 
	{
		if (strlen(cPhoneNumber)== 0) 
			sprintf(endshowinfo, UNKNOWNUMBER);
		else
			InitTxt(hdc,cPhoneNumber,endshowinfo, 176, FALSE);
	}
	else
		InitTxt(hdc,(LPSTR)cName,(LPSTR)endshowinfo, 176, TRUE);	
	ReleaseDC(hWnd,hdc);
	sprintf(endstatusinfo, STATUS_INFO_END);
	if (strlen(cLastTimer) != 0) 
		sprintf(endtimeinfo, cLastTimer);
	else
		strcpy(endtimeinfo, "");	
	if (strlen(cCostInfo) != 0)
		sprintf(endcostinfo, cCostInfo);
	else
		strcpy(endcostinfo, "");
	InvalidateRect(hWnd,NULL,TRUE);	
}

BOOL MBInsertMBEndNode(const char * pPhoneCode,const char * pName,const char * pDuration,const char * pCost)
{
	PCALLENDNODE ptmpnode = NULL;
	PCALLENDNODE ptailnode = NULL;

	if (NULL == pEndNodeHead) 
	{
		ptmpnode = malloc(sizeof(CALLENDNODE));
		if ( NULL==ptmpnode) 
			return FALSE;
		else
		{
			memset(ptmpnode, 0, sizeof(CALLENDNODE));
			if (strlen(pPhoneCode)> PHONENUMMAXLEN) 
			{
				strncpy(ptmpnode->dspNumber, pPhoneCode, PHONENUMMAXLEN);
				ptmpnode->dspNumber[PHONENUMMAXLEN] = 0;
			}
			else
				strcpy(ptmpnode->dspNumber, pPhoneCode);
			if (strlen(pName)> PHONENUMMAXLEN) 
			{
				strncpy(ptmpnode->dspName, pName, PHONENUMMAXLEN);
				ptmpnode->dspName[PHONENUMMAXLEN] = 0;
			}
			else
				strcpy(ptmpnode->dspName, pName);
			if (strlen(pDuration)> DURATIONLEN) 
			{
				strncpy(ptmpnode->dspTime, pDuration, DURATIONLEN);
				ptmpnode->dspTime[DURATIONLEN] = 0;
			}
			else
				strcpy(ptmpnode->dspTime, pDuration);
			if (strlen(pCost)> COSTLEN) 
			{
				strncpy(ptmpnode->dspCost, pCost, COSTLEN);
				ptmpnode->dspCost[COSTLEN] = 0;
			}
			else
				strcpy(ptmpnode->dspCost, pCost);
			ptmpnode->pNext = NULL;
			pEndNodeHead = ptmpnode;
			return TRUE;
		}
	}
	else
	{
		ptailnode = pEndNodeHead;
		ptmpnode = pEndNodeHead;
		while (ptmpnode) 
		{
			ptailnode = ptmpnode;
			ptmpnode = ptmpnode->pNext;
		}
		ptmpnode = malloc(sizeof(CALLENDNODE));
		if ( NULL==ptmpnode) 
			return FALSE;
		else
		{
			memset(ptmpnode, 0, sizeof(CALLENDNODE));
			if (strlen(pPhoneCode)> PHONENUMMAXLEN) 
			{
				strncpy(ptmpnode->dspNumber, pPhoneCode, PHONENUMMAXLEN);
				ptmpnode->dspNumber[PHONENUMMAXLEN] = 0;
			}
			else
				strcpy(ptmpnode->dspNumber, pPhoneCode);
			if (strlen(pName)> PHONENUMMAXLEN) 
			{
				strncpy(ptmpnode->dspName, pName, PHONENUMMAXLEN);
				ptmpnode->dspName[PHONENUMMAXLEN] = 0;
			}
			else
				strcpy(ptmpnode->dspName, pName);
			if (strlen(pDuration)> DURATIONLEN) 
			{
				strncpy(ptmpnode->dspTime, pDuration, DURATIONLEN);
				ptmpnode->dspTime[DURATIONLEN] = 0;
			}
			else
				strcpy(ptmpnode->dspTime, pDuration);
			if (strlen(pCost)> COSTLEN) 
			{
				strncpy(ptmpnode->dspCost, pCost, COSTLEN);
				ptmpnode->dspCost[COSTLEN] = 0;
			}
			else
				strcpy(ptmpnode->dspCost, pCost);
			ptailnode->pNext = ptmpnode;
			ptmpnode->pNext = NULL;
			return TRUE;
		}
	}
}

BOOL IsCallEndWnd()
{
	return (IsWindow(hCallEndWnd)||IsWindow(hCallEndNodeWnd)); 
}

void CloseCallEndWnd()
{
	if (IsWindow(hCallEndWnd)) 
		DestroyWindow(hCallEndWnd);
	if (IsWindow(hCallEndNodeWnd))
		DestroyWindow(hCallEndNodeWnd);
}
