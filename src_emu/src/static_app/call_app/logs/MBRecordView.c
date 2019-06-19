 

#include    "MBRecordView.h"
#include    "winpda.h"
#include    "plx_pdaex.h"
#include  "mullang.h"
#include "MBPublic.h"
#include "prebrowhead.h"

#define    AB_NAME_LEN  AB_MAXLEN_NAME
#define     VIEWINFONUM             4

#define     IDC_BUTTON_QUIT         3
#define     IDC_OK                   5
#define     IDS_OK                  ML("Ok")
#define     IDS_LAB_NUMBBER         ML("Number:")
#define     IDS_LAB_TIME            ML("Time:")
#define     IDS_LAB_DURATION        ML("Duration:")

#define     BUTTONQUIT              ML("Exit") 

#define     MBRECORDLISTDELFAIL     ML("Deletefail") 
#define     IDS_MBRECORD_CALLBACK   ML("Call") 
#define     MBRECORDDELETEONE       ML("Deleteornot") 

#define     MONDAY                  ML("Monday") 
#define     TUESDAY                 ML("Tuesday") 
#define     WEDNESDAY               ML("Wednesday") 
#define     THURSDAY                ML("Thursday") 
#define     FRIDAY                  ML("Friday") 
#define     SATURDAY                ML("Saturday") 
#define     SUNDAY                  ML("Sunday") 
#define     AM                      ML("AM") 
#define     PM                      ML("PM") 

#define     MENU_DELETE             ML("Delete") 
#define     MENU_PHONE              ML("Phone") 
#define     MENU_LINKMAN            ML("Phonebook") 
#define     MENU_SMS                ML("SMS") 
#define     MENU_MMS                ML("MMS") 

#define     IDM_PHONE               100
#define     IDM_LINKMAN             110
#define     IDM_SMS                 120
#define     IDM_MMS                 130
#define     IDM_DELETE              140
static const MENUITEMTEMPLATE MBRecordViewMenu[] = 
{
    
    { MF_STRING, IDM_DELETE ,"", NULL},
    { MF_STRING, IDM_LINKMAN,"", NULL},
    { MF_STRING, IDM_SMS    ,"", NULL},
    { MF_STRING, IDM_MMS    ,"", NULL},
    { MF_END, 0, NULL, NULL }
};

static const MENUTEMPLATE MBRecordViewMenuTemplate =
{
    0,
    (PMENUITEMTEMPLATE)MBRecordViewMenu
};

static HINSTANCE        hInstance;
static MBRECORD_RS      BackValue;
static MBRECORD_VIEW    MBRecord_View;
static HWND             hDetailsWnd;
static HWND             hDetailFormView;

static const char * pClassName      = "MBRecordViewWndClass";
static const char * pTimeNormal     = "%s:%04d-%02d-%02d %02d:%02d";        
static const char * pTimeNormal_Un  = "%04d-%02d-%02d %02d:%02d";           
static const char * pTime_Week      = "%s:%04d-%02d-%02d %s %02d:%02d";     
static const char * pTime_Week_Un   = "%04d-%02d-%02d %s %02d:%02d";        
static const char * pTime_Week_M    = "%s:%04d-%02d-%02d %s %02d:%02d %s";  
static const char * pTime_Week_M_Un = "%04d-%02d-%02d %s %02d:%02d %s";     
static const char * pTime_M         = "%s:%04d-%02d-%02d %02d:%02d %s";     
static const char * pTime_M_Un      = "%04d-%02d-%02d %02d:%02d %s";        

static const char * pTimeUsed       = "%s:%02d:%02d:%02d";
static const char * pTimeUsed_Un    = "%02d:%02d:%02d";
static const char * pString         = "%s:%s";
static const char * pString_Un      = "%s";
static const int    iRectX          = 10;
static const int    iAM_PM          = 12;

static  LRESULT AppWndProc(HWND hWnd,UINT wMsgCmd,WPARAM wParam,LPARAM lParam);
//static  void    ShowMBRecorInfo(HWND hWnd,HDC hdc,CallRecord * pMBRecord);
static  BOOL    GetWeekInfo(const SYSTEMTIME * pTime,char * pString);
extern BOOL APP_SaveToPhoneBook(HWND hParent, const char* pPhoneNo);
void CloseDetailWnd(void);
MBRECORD_RS CallMBLogViewWindow(MBRECORD_VIEW * pMBRecord_Type)
{
//    HWND        hWnd;
    WNDCLASS    wc;
//    HMENU       hMenu;
//    MSG         msg;
	RECT        rClient;

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

    if (!RegisterClass(&wc))
    {
        return MBRECORD_RS_CREATEFAIL;
    }
    memset(&MBRecord_View,0x00,sizeof(MBRECORD_VIEW));
    if (NULL == pMBRecord_Type)
        return MBRECORD_RS_CREATEFAIL;

    memcpy(&MBRecord_View,pMBRecord_Type,sizeof(MBRECORD_VIEW));

/*
    hMenu = LoadMenuIndirect((PMENUTEMPLATE)&MBRecordViewMenuTemplate);
    ModifyMenu(hMenu,IDM_PHONE  ,MF_BYCOMMAND|MF_ENABLED,IDM_PHONE  ,MENU_PHONE  );
    ModifyMenu(hMenu,IDM_DELETE ,MF_BYCOMMAND|MF_ENABLED,IDM_DELETE ,MENU_DELETE );
    ModifyMenu(hMenu,IDM_LINKMAN,MF_BYCOMMAND|MF_ENABLED,IDM_LINKMAN,MENU_LINKMAN);
    ModifyMenu(hMenu,IDM_SMS    ,MF_BYCOMMAND|MF_ENABLED,IDM_SMS    ,MENU_SMS    );
    ModifyMenu(hMenu,IDM_MMS    ,MF_BYCOMMAND|MF_ENABLED,IDM_MMS    ,MENU_MMS    );

*/
	GetClientRect(MBRecord_View.hWnd, &rClient);
	hDetailsWnd = CreateWindow(pClassName, "",
		WS_CHILD | WS_VISIBLE/* | WS_VSCROLL*/, 
		rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
		MBRecord_View.hWnd, NULL, NULL, NULL);

    if (NULL == hDetailsWnd)
    {
        UnregisterClass(pClassName,NULL);
        return MBRECORD_RS_CREATEFAIL;
    }
    ShowWindow(MBRecord_View.hWnd, SW_SHOW);
    UpdateWindow(MBRecord_View.hWnd); 
	if (hDetailFormView != NULL)
		SetFocus(hDetailFormView);
	else
		SetFocus(hDetailsWnd);
    return (BackValue);
}

#define MAX_ITEMLEN  30

static LRESULT AppWndProc(HWND hWnd,UINT wMsgCmd,WPARAM wParam,LPARAM lParam)
{
    static      CallRecord  MBRecord;
                LRESULT     lResult;
//                HDC         hdc;

    lResult = (LRESULT)TRUE;
    switch ( wMsgCmd )
    {
    case WM_CREATE :
		{
			RECT rClient;
			LISTBUF	Listtmp;
			char  cTimedata[MAX_ITEMLEN] = "";
			char  cTimeUseddata[MAX_ITEMLEN] = "";
			char  cTime[15] = "";
			char  cData[15] = "";
			int len, len1;
			int nday, dhour;
//			DWORD dhour;
			GetClientRect(hWnd, &rClient);
			SendMessage(MBRecord_View.hWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)"");
			//lanlan fix
			SendMessage(MBRecord_View.hWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
			//end
			SendMessage(MBRecord_View.hWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_OK, 1), (LPARAM)IDS_OK);
			SendMessage(MBRecord_View.hWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON), (LPARAM)NULL);
			SendMessage(MBRecord_View.hWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON), (LPARAM)NULL);
			SetWindowText(MBRecord_View.hWnd, MBRecord_View.TitleInfo.cTitle);
			PDASetMenu(MBRecord_View.hWnd, NULL);
			GetMBRecord(&MBRecord,sizeof(CallRecord),
				MBRecord_View.MBRecordType,MBRecord_View.index);
			PREBROW_InitListBuf (&Listtmp);
			PREBROW_AddData(&Listtmp, IDS_LAB_NUMBBER, MBRecord.PhoneNo);
			cData[0] = 0;
			cTime[0] = 0;
			GetTimeDisplay(MBRecord.Time, cTime, cData);
			len = strlen(cData);
			len1 = strlen(cTime);
			if (len+len1+1> MAX_ITEMLEN) 
				return FALSE;
			sprintf(cTimedata,"%s %s", cData, cTime);
			PREBROW_AddData(&Listtmp, IDS_LAB_TIME, cTimedata);
			if (MBRecord_View.MBRecordType != UNANSWER_REC ) 
			{			
				
				cData[0] = 0;
				cTime[0] = 0;
				nday = MBRecord.TimeUsed.wHour / 24;
				dhour = MBRecord.TimeUsed.wHour % 24;
				GetTimeDisplay(MBRecord.TimeUsed, cTime, cData);
				if(nday > 1)
					sprintf(cTimeUseddata,"%02dd %02d:%02d:%02d", nday, dhour, MBRecord.TimeUsed.wMinute,MBRecord.TimeUsed.wSecond);
				else
					sprintf(cTimeUseddata,"%02d:%02d:%02d", dhour, MBRecord.TimeUsed.wMinute,MBRecord.TimeUsed.wSecond);
				
				PREBROW_AddData(&Listtmp, IDS_LAB_DURATION, cTimeUseddata);
				
			}
			FormView_Register();
			hDetailFormView = CreateWindow(WC_FORMVIEW,
				NULL,
				WS_VISIBLE  | WS_CHILD | WS_VSCROLL,
				rClient.left, rClient.top, rClient.right - rClient.left, rClient.bottom - rClient.top,
				hWnd, NULL, NULL, (PVOID)&Listtmp);
			if (hDetailFormView == NULL)
				return FALSE;
		}
        break;

	case PWM_SHOWWINDOW:
			SendMessage(MBRecord_View.hWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)"");
			//lanlan fix
			SendMessage(MBRecord_View.hWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
			//end
			SendMessage(MBRecord_View.hWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_OK, 1), (LPARAM)IDS_OK);
			SendMessage(MBRecord_View.hWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON), (LPARAM)NULL);
			SendMessage(MBRecord_View.hWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON), (LPARAM)NULL);
			SetWindowText(MBRecord_View.hWnd, MBRecord_View.TitleInfo.cTitle);
			PDASetMenu(MBRecord_View.hWnd, NULL);
			SetFocus(hDetailFormView);
			break;
		

    case WM_CLOSE:
		DestroyWindow(hWnd);
        break;
		
    case WM_DESTROY :
		hDetailFormView = NULL;
        hWnd = NULL;
        UnregisterClass(pClassName,NULL);
        break;


    case WM_KEYDOWN:
        switch(LOWORD(wParam))
        {
		case VK_RETURN:
			SendMessage(MBRecord_View.hWnd, PWM_CLOSEWINDOW,  (WPARAM)hWnd, 0);			
			SendMessage(hWnd, WM_CLOSE, 0,0);
			break;
        case VK_F1:
			APP_CallPhoneNumber(MBRecord.PhoneNo);
            BackValue = MBRECORD_RS_DIALUP;
            break;
        default:
            lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        }
        break;

    default :
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
    return lResult;
}

/*
static  void    ShowMBRecorInfo(HWND hWnd,HDC hdc,CallRecord * pMBRecord)
{
    RECT   rRect[VIEWINFONUM];
    int    iSW,iy[VIEWINFONUM],iFontHeight,i;
    char   cBufPhone[PHONENUMMAXLEN + MBRECORD_CLEW_NAMEMAXLEN + 2] = "";
    char   cBufName[MBRECORD_CLEW_NAMEMAXLEN + AB_NAME_LEN + 2] = "";
    char   cBufTime[MBRECORD_CLEW_TIMEMAXLEN + 31] = "";
    char   cBufTimeUsed[MBRECORD_CLEW_DURATIONMAXLEN + 11] = "";
    char   cWeek[10] = "";
    ABINFO pbNameIcon;

    
    if ((MBRecord_View.ItemStyle & MBV_WS_NUMBER) && 
        (strlen(MBRecord_View.TitleInfo.cNumber) > 0))
        sprintf(cBufPhone,pString,MBRecord_View.TitleInfo.cNumber,pMBRecord->PhoneNo);
    else
        sprintf(cBufPhone,pString_Un,pMBRecord->PhoneNo);
    
    memset(&pbNameIcon,0x00, sizeof(ABINFO));
    if (MB_GetInfoFromPhonebook(pMBRecord->PhoneNo,&pbNameIcon))
    {
        if ((MBRecord_View.ItemStyle & MBV_WS_NAME) &&
            (strlen(MBRecord_View.TitleInfo.cName) > 0))
            sprintf(cBufName,pString,MBRecord_View.TitleInfo.cName,pbNameIcon.szName);
        else
            sprintf(cBufName,pString_Un,pbNameIcon.szName);
    }
    else
    {
        if ((MBRecord_View.ItemStyle & MBV_WS_NAME) &&
            (strlen(MBRecord_View.TitleInfo.cName) > 0))
            sprintf(cBufName,pString,MBRecord_View.TitleInfo.cName,MBRecord_View.cUnknow);
        else
            sprintf(cBufName,pString_Un,MBRecord_View.cUnknow);
    }
    
    switch (MBRecord_View.TimeStyle)
    {
    case TS_NORMAL:
        if ((MBRecord_View.ItemStyle & MBV_WS_TIME) &&
            (strlen(MBRecord_View.TitleInfo.cTime) > 0))
        {
            sprintf(cBufTime,pTimeNormal,MBRecord_View.TitleInfo.cTime,
                pMBRecord->Time.wYear,pMBRecord->Time.wMonth, pMBRecord->Time.wDay,
                pMBRecord->Time.wHour,pMBRecord->Time.wMinute);
        }
        else
        {
            sprintf(cBufTime,pTimeNormal_Un,
                pMBRecord->Time.wYear,pMBRecord->Time.wMonth,pMBRecord->Time.wDay,
                pMBRecord->Time.wHour,pMBRecord->Time.wMinute);
        }
        break;
    case TS_NORMAL_WEEK:
        GetWeekInfo(&pMBRecord->Time,cWeek);
        if ((MBRecord_View.ItemStyle & MBV_WS_TIME) &&
            (strlen(MBRecord_View.TitleInfo.cTime) > 0))
        {
            sprintf(cBufTime,pTime_Week,MBRecord_View.TitleInfo.cTime,
                pMBRecord->Time.wYear,pMBRecord->Time.wMonth,pMBRecord->Time.wDay,
                cWeek,pMBRecord->Time.wHour,pMBRecord->Time.wMinute);
        }
        else
        {
            sprintf(cBufTime,pTime_Week_Un,
                pMBRecord->Time.wYear,pMBRecord->Time.wMonth,pMBRecord->Time.wDay,
                cWeek,pMBRecord->Time.wHour,pMBRecord->Time.wMinute);
        }
        break;
    case TS_NORMAL_WEEK_M:
        GetWeekInfo(&pMBRecord->Time,cWeek);
        if ((MBRecord_View.ItemStyle & MBV_WS_TIME) &&
            (strlen(MBRecord_View.TitleInfo.cTime) > 0))
        {
            sprintf(cBufTime,pTime_Week_M,MBRecord_View.TitleInfo.cTime,
                pMBRecord->Time.wYear,pMBRecord->Time.wMonth,pMBRecord->Time.wDay,
                cWeek,pMBRecord->Time.wHour,pMBRecord->Time.wMinute,
                pMBRecord->Time.wHour >= iAM_PM ? PM : AM);
        }
        else
        {
            sprintf(cBufTime,pTime_Week_M_Un,
                pMBRecord->Time.wYear,pMBRecord->Time.wMonth,pMBRecord->Time.wDay,
                cWeek,pMBRecord->Time.wHour,pMBRecord->Time.wMinute,
                pMBRecord->Time.wHour >= iAM_PM ? PM : AM);
        }
        break;
    case TS_NORMAL_M:
        if ((MBRecord_View.ItemStyle & MBV_WS_TIME) &&
            (strlen(MBRecord_View.TitleInfo.cTime) > 0))
        {
            sprintf(cBufTime,pTime_M,MBRecord_View.TitleInfo.cTime,
                pMBRecord->Time.wYear,pMBRecord->Time.wMonth, pMBRecord->Time.wDay,
                pMBRecord->Time.wHour,pMBRecord->Time.wMinute,
                pMBRecord->Time.wHour >= iAM_PM ? PM : AM);
        }
        else
        {
            sprintf(cBufTime,pTime_M_Un,
                pMBRecord->Time.wYear,pMBRecord->Time.wMonth,pMBRecord->Time.wDay,
                pMBRecord->Time.wHour,pMBRecord->Time.wMinute,
                pMBRecord->Time.wHour >= iAM_PM ? PM : AM);
        }
        break;
    }
    
    if ((MBRecord_View.ItemStyle & MBV_WS_DURATION) && 
        (strlen(MBRecord_View.TitleInfo.cDuration) > 0))
    {
        sprintf(cBufTimeUsed,pTimeUsed,MBRecord_View.TitleInfo.cDuration, 
            pMBRecord->TimeUsed.wHour,pMBRecord->TimeUsed.wMinute,
            pMBRecord->TimeUsed.wSecond);
    }
    else
    {
        sprintf(cBufTimeUsed,pTimeUsed_Un,
            pMBRecord->TimeUsed.wHour,pMBRecord->TimeUsed.wMinute,
            pMBRecord->TimeUsed.wSecond);
    }

    iSW         = GetScreenUsableWH1(SM_CXSCREEN);
    iFontHeight = GetCharactorHeight(hWnd);
    CountControlsYaxis(iy,iFontHeight * 2,sizeof(iy));

    for (i = 0;i < VIEWINFONUM;i++)
        SetRect(&rRect[i],iRectX,iy[i],iSW - iRectX,iy[i] + iFontHeight * 2);

    StrAutoNewLine(hdc,cBufPhone   ,NULL,&rRect[0],DT_VCENTER|DT_LEFT);
    StrAutoNewLine(hdc,cBufName    ,NULL,&rRect[1],DT_VCENTER|DT_LEFT);
    StrAutoNewLine(hdc,cBufTime    ,NULL,&rRect[2],DT_VCENTER|DT_LEFT);
    StrAutoNewLine(hdc,cBufTimeUsed,NULL,&rRect[3],DT_VCENTER|DT_LEFT);    
}*/


static  BOOL    GetWeekInfo(const SYSTEMTIME * pTime,char * pString)
{
    if ((pTime == NULL) || (pString == NULL))
        return FALSE;

    switch (pTime->wDayOfWeek)
    {
    case 0:
        strcpy(pString,SUNDAY);
        return TRUE;
    case 1:
        strcpy(pString,MONDAY);
        return TRUE;
    case 2:
        strcpy(pString,TUESDAY);
        return TRUE;
    case 3:
        strcpy(pString,WEDNESDAY);
        return TRUE;
    case 4:
        strcpy(pString,THURSDAY);
        return TRUE;
    case 5:
        strcpy(pString,FRIDAY);
        return TRUE;
    case 6:
        strcpy(pString,SATURDAY);
        return TRUE;
    default:
        return FALSE;
    }
}

void CloseDetailWnd(void)
{
	if (IsWindow(hDetailsWnd))
		SendMessage(hDetailsWnd, WM_CLOSE, 0, 0);
}
