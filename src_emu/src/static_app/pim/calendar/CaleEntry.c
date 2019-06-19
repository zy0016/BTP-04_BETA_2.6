/***************************************************************************
*
*                      Pollex Mobile Platform
*
* Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
*                       All Rights Reserved 
*
* Module   : CaleEditEntry.c
*
* Purpose  : 
*
\**************************************************************************/

#include "CaleHeader.h"


static HWND HwndCaleMeet;               //meeting display window
static HWND HwndCaleAnni;               //anniversary display window
static HWND HwndCaleEvent;              //event display window

static HWND HwndCaleMeetOpen;
static HWND HwndCaleAnniOpen;
static HWND HwndCaleEventOpen;

static HWND HwndCaleMeetAlarm;          //meeting alarm sub window
static HWND HwndCaleAnniAlarm;          //annivesary alarm sub window
static HWND HwndCaleMeetRep;            //meeting repeation sub window

static HWND HwndList = NULL;            //listbox handle window
static int CurAlarmRep = 0;             //current alarm or repeation station
static BOOL CurRead;                    //whether read style

static HWND hListWnd = NULL;

static HBITMAP hSelect;
static HBITMAP hUnSelect;

//sub display window
static SYSTEMTIME CurSysTime;           //the systime that day view window transfer into
static BOOL bNewDate = FALSE;           //whether new date
static int CurCaleType = 0;             //current calendar type
static int CurRecord = 0;               //current calendar record(save)
static CaleMeetingNode CurMeetInfo;
static CaleAnniNode CurAnniInfo;
static CaleEventNode CurEventInfo;
static int ConfirmRet;

static HWND hSubject;
static HWND hLocation;
static HWND hStartTime;
static HWND hEndTime;
static HWND hStartDate;
static HWND hEndDate;
static HWND hAlarm;
static HWND hRepeat;

static BOOL bUpdated = FALSE;

#ifdef _NOKIA_CALE_
static HWND hSync;
#endif

static IMEEDIT ImeEdit;

BOOL CreateDisplayMeet(HWND hwnd, int recordpt, SYSTEMTIME *caltime, CaleMeetingNode *setmeet, BOOL bCreate, BOOL bRead);
BOOL CreateDisplayAnni(HWND hwnd, int recordpt, SYSTEMTIME *caltime, CaleAnniNode *setanni, BOOL bCreate, BOOL bRead);
BOOL CreateDisplayEvent(HWND hwnd, int recordpt, SYSTEMTIME *caltime, CaleEventNode *setevent, BOOL bCreate, BOOL bRead);
static LRESULT CaleDisplayWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static int CALE_JudgeMeetSave(CaleMeetingNode *pTmpMeet, SYSTEMTIME *pSysTime);
static int CALE_JudgeAnniSave(CaleAnniNode *pTmpAnni, SYSTEMTIME *pSysTime);
static int CALE_JudgeEventSave(CaleEventNode *pTmpEvent, SYSTEMTIME *pSysTime);
BOOL CreateMeetAlarmWnd(HWND hwnd);
static LRESULT CaleMeetAlarmWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
BOOL CreateMeetRepWnd(HWND hwnd);
static LRESULT CaleMeetRepWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
BOOL CreateAnniAlarmWnd(HWND hwnd);
static LRESULT CaleAnniAlarmWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static DWORD RegisterCaleOpenClass(void);
void UnRegisterCaleOpenClass(void);
static LRESULT CaleOpenWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);

extern int CALE_CalcWkday(SYSTEMTIME* pSysInfo);
extern int CALE_OpenApi(int CaleEntry);
extern BOOL CALE_CloseApi(int Cal_OpenFile);
//extern int CALE_ReadApi(int Cal_Open, int itemID, BYTE *pRecord, int size);
extern int CALE_AddApi(int CaleEntry, int Cal_Open, BYTE *pRecord, int size);
extern BOOL CALE_DelApi(int CaleEntry, int Cal_Open, int itemID, BYTE *pRecord, int size);
extern BOOL CALE_NewMeet(CaleMeetingNode *CurMeetDate, DWORD nItemID, int nFlag);
extern BOOL CALE_NewAnni(CaleAnniNode *CurAnniDate, DWORD nItemID, int nFlag);
extern int CALE_KillAlarm(int CaleType, int nAlmID);
extern void CALE_DateTimeSysToChar(SYSTEMTIME *pSys, char *pChar);
extern void CALE_DateTimeCharToSys(char *pChar, SYSTEMTIME *pSys);
extern int CALE_CmpSysDate(SYSTEMTIME *pFirstDate, SYSTEMTIME *pSecondDate);
extern BOOL CALE_DayIncrease(SYSTEMTIME *psystime, unsigned int day);
extern int CALE_CmpOnlyDate(SYSTEMTIME *pFirstDate, SYSTEMTIME *pSecondDate);
extern int CALE_CmpOnlyTime(SYSTEMTIME *pFirstDate, SYSTEMTIME *pSecondDate);

static void CALE_OpenEntry(HWND hListBox, PCALE_VIEWCREATEDATA pCreateData);

static DWORD RegisterCaleDisplayClass(void)
{
    WNDCLASS wc;
    
    wc.style        = 0;
    wc.lpfnWndProc  = CaleDisplayWndProc;
    wc.cbClsExtra   = 0;
    wc.cbWndExtra   = sizeof(CALE_ENTRYCREATEDATA);
    wc.hInstance    = NULL;
    wc.hIcon        = NULL;
    wc.hCursor      = NULL;
    wc.hbrBackground= NULL;//GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName = NULL;
    wc.lpszClassName= "CaleDisplayWndClass";
    
    if (!RegisterClass(&wc))
    {
        UnregisterClass("CaleDisplayWndClass", NULL);
        if(!RegisterClass(&wc))
            return FALSE;
    }
    return TRUE;
}

void UnRegisterCaleDiaplayClass(void)
{
    UnregisterClass("CaleDisplayWndClass", NULL);
    return;
}

BOOL CreateDisplayMeet(HWND hwnd, int recordpt, SYSTEMTIME *caltime, CaleMeetingNode *setmeet, BOOL bCreate, BOOL bRead)
{
    RECT rClient;

    memset(&CurMeetInfo, 0x0, sizeof(CaleMeetingNode));
    memset(&CurSysTime, 0x0, sizeof(SYSTEMTIME));
    
    CurCaleType = CALE_MEETING;
    bNewDate = bCreate;
    CurRead = bRead;
    memcpy(&CurSysTime, caltime, sizeof(SYSTEMTIME));

    if(!bNewDate)
    {
        CurRecord = recordpt;
        memcpy(&CurMeetInfo, setmeet, sizeof(CaleMeetingNode));
    }

    if(!bRead)
    {
        if(IsWindow(HwndCaleMeetOpen))
        {
            ShowWindow(HwndCaleMeetOpen, SW_SHOW);
            BringWindowToTop(HwndCaleMeetOpen);
            UpdateWindow(HwndCaleMeetOpen);
        }
        else
        {
            CALE_VIEWCREATEDATA CreateData;

            if(!RegisterCaleOpenClass())
                return FALSE;
            
            memset(&CreateData, 0x0, sizeof(CALE_VIEWCREATEDATA));
            CreateData.hFrameWnd = CALE_GetFrame();
            CreateData.hParent = hwnd;
			CreateData.hFocus = NULL;

            GetClientRect(CreateData.hFrameWnd, &rClient);

            HwndCaleMeetOpen = CreateWindow(
                "CaleOpenWndClass",
                "",//IDP_IME_MEETING,
                WS_VISIBLE | WS_CHILD,
                rClient.left, 
                rClient.top, 
                rClient.right - rClient.left,
                rClient.bottom - rClient.top,
                CreateData.hFrameWnd,
                NULL,
                NULL, 
                (PVOID)&CreateData
                );
            
            if(!HwndCaleMeetOpen)
            {
                UnRegisterCaleOpenClass();
                return FALSE;
            }
            SendMessage(CreateData.hFrameWnd, PWM_CREATECAPTIONBUTTON, 
                MAKEWPARAM(IDM_BUTTON_COMPLETE, 1), (LPARAM)IDP_IME_EDIT);
            SendMessage(CreateData.hFrameWnd, PWM_CREATECAPTIONBUTTON, 
                MAKEWPARAM(IDM_BUTTON_QUIT, 0), (LPARAM)IDP_IME_BACK);
            SendMessage(CreateData.hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"");

            SetWindowText(CreateData.hFrameWnd, IDP_IME_MEETING);
            ShowWindow(CreateData.hFrameWnd, SW_SHOW);
            UpdateWindow(CreateData.hFrameWnd);
        }
    }
    else
    {
        if(IsWindow(HwndCaleMeet))
        {
            ShowWindow(HwndCaleMeet, SW_SHOW);
            BringWindowToTop(HwndCaleMeet);
            UpdateWindow(HwndCaleMeet);
        }
        else
        {
            CALE_ENTRYCREATEDATA CreateData;
            
            if(!RegisterCaleDisplayClass())
                return FALSE;
            
            memset(&CreateData, 0x0, sizeof(CALE_ENTRYCREATEDATA));
            CreateData.hFrameWnd = CALE_GetFrame();
            CreateData.hParent = hwnd;
			CreateData.hFocus = NULL;
            
            GetClientRect(CreateData.hFrameWnd, &rClient);
            
            HwndCaleMeet = CreateWindow(
                "CaleDisplayWndClass",
                "",//IDP_IME_MEETING,
                WS_VISIBLE | WS_CHILD | WS_VSCROLL,
                rClient.left, 
                rClient.top, 
                rClient.right - rClient.left,
                rClient.bottom - rClient.top,
                CreateData.hFrameWnd,
                NULL,
                NULL, 
                (PVOID)&CreateData
                );

            if(!HwndCaleMeet)
            {
                UnRegisterCaleDiaplayClass();
                return FALSE;
            }

            SendMessage(CreateData.hFrameWnd, PWM_CREATECAPTIONBUTTON, 
                MAKEWPARAM(IDM_BUTTON_COMPLETE, 1), (LPARAM)IDP_IME_SAVE);
            SendMessage(CreateData.hFrameWnd, PWM_CREATECAPTIONBUTTON, 
                MAKEWPARAM(IDM_BUTTON_QUIT, 0), (LPARAM)IDP_IME_CANCEL);
            SendMessage(CreateData.hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
            
            SetWindowText(CreateData.hFrameWnd, IDP_IME_NEWMEETING);
            ShowWindow(CreateData.hFrameWnd, SW_SHOW);
            UpdateWindow(HwndCaleMeet);
        }
    }
    
    return TRUE;
}

BOOL CreateDisplayAnni(HWND hwnd, int recordpt, SYSTEMTIME *caltime, CaleAnniNode *setanni, BOOL bCreate, BOOL bRead)
{
    RECT rClient;
    
    memset(&CurAnniInfo, 0x0, sizeof(CaleAnniNode));
    memset(&CurSysTime, 0x0, sizeof(SYSTEMTIME));
    
    CurCaleType = CALE_ANNI;
    bNewDate = bCreate;
    CurRead = bRead;
    memcpy(&CurSysTime, caltime, sizeof(SYSTEMTIME));
    
    if(!bNewDate)
    {
        CurRecord = recordpt;
        memcpy(&CurAnniInfo, setanni, sizeof(CaleAnniNode));
    }
    
    if(!bRead)
    {
        if(IsWindow(HwndCaleAnniOpen))
        {
            ShowWindow(HwndCaleAnniOpen, SW_SHOW);
            BringWindowToTop(HwndCaleAnniOpen);
            UpdateWindow(HwndCaleAnniOpen);
        }
        else
        {
            CALE_VIEWCREATEDATA CreateData;
            
            if(!RegisterCaleOpenClass())
                return FALSE;
            
            memset(&CreateData, 0x0, sizeof(CALE_VIEWCREATEDATA));
            CreateData.hFrameWnd = CALE_GetFrame();
            CreateData.hParent = hwnd;
			CreateData.hFocus = NULL;
            
            GetClientRect(CreateData.hFrameWnd, &rClient);
            
            HwndCaleAnniOpen = CreateWindow(
                "CaleOpenWndClass",
                "",//IDP_IME_MEETING,
                WS_VISIBLE | WS_CHILD,
                rClient.left, 
                rClient.top, 
                rClient.right - rClient.left,
                rClient.bottom - rClient.top,
                CreateData.hFrameWnd,
                NULL,
                NULL, 
                (PVOID)&CreateData
                );
            
            if(!HwndCaleAnniOpen)
            {
                UnRegisterCaleOpenClass();
                return FALSE;
            }
            SendMessage(CreateData.hFrameWnd, PWM_CREATECAPTIONBUTTON, 
                MAKEWPARAM(IDM_BUTTON_COMPLETE, 1), (LPARAM)IDP_IME_EDIT);
            SendMessage(CreateData.hFrameWnd, PWM_CREATECAPTIONBUTTON, 
                MAKEWPARAM(IDM_BUTTON_QUIT, 0), (LPARAM)IDP_IME_BACK);
            SendMessage(CreateData.hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
            
            SetWindowText(CreateData.hFrameWnd, IDP_IME_ANNIVERARY);
            ShowWindow(CreateData.hFrameWnd, SW_SHOW);
            UpdateWindow(CreateData.hFrameWnd);
        }
    }
    else
    {
        if(IsWindow(HwndCaleAnni))
        {
            ShowWindow(HwndCaleAnni, SW_SHOW);
            BringWindowToTop(HwndCaleAnni);
            UpdateWindow(HwndCaleAnni);
        }
        else
        {
            CALE_ENTRYCREATEDATA CreateData;
            
            if(!RegisterCaleDisplayClass())
                return FALSE;
            
            memset(&CreateData, 0x0, sizeof(CALE_ENTRYCREATEDATA));
            CreateData.hFrameWnd = CALE_GetFrame();
            CreateData.hParent = hwnd;
			CreateData.hFocus = NULL;
            
            GetClientRect(CreateData.hFrameWnd, &rClient);
            
            HwndCaleAnni = CreateWindow(
                "CaleDisplayWndClass",
                "",//IDP_IME_MEETING,
                WS_VISIBLE | WS_CHILD | WS_VSCROLL,
                rClient.left, 
                rClient.top, 
                rClient.right - rClient.left,
                rClient.bottom - rClient.top,
                CreateData.hFrameWnd,
                NULL,
                NULL, 
                (PVOID)&CreateData
                );
            
            if(!HwndCaleAnni)
            {
                UnRegisterCaleDiaplayClass();
                return FALSE;
            }
            
            SendMessage(CreateData.hFrameWnd, PWM_CREATECAPTIONBUTTON, 
                MAKEWPARAM(IDM_BUTTON_COMPLETE, 1), (LPARAM)IDP_IME_SAVE);
            SendMessage(CreateData.hFrameWnd, PWM_CREATECAPTIONBUTTON, 
                MAKEWPARAM(IDM_BUTTON_QUIT, 0), (LPARAM)IDP_IME_CANCEL);
            SendMessage(CreateData.hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
            
            SetWindowText(CreateData.hFrameWnd, IDP_IME_NEWANNI);
            ShowWindow(CreateData.hFrameWnd, SW_SHOW);
            UpdateWindow(HwndCaleAnni);
        }
    }
    
    return TRUE;
}

BOOL CreateDisplayEvent(HWND hwnd, int recordpt, SYSTEMTIME *caltime, CaleEventNode *setevent, BOOL bCreate, BOOL bRead)
{
    RECT rClient;
    
    memset(&CurEventInfo, 0x0, sizeof(CaleEventNode));
    memset(&CurSysTime, 0x0, sizeof(SYSTEMTIME));

    CurCaleType = CALE_EVENT;
    bNewDate = bCreate;
    CurRead = bRead;
    memcpy(&CurSysTime, caltime, sizeof(SYSTEMTIME));
    
    if(!bNewDate)
    {
        CurRecord = recordpt;
        memcpy(&CurEventInfo, setevent, sizeof(CaleEventNode));
    }
    
    if(!bRead)
    {
        if(IsWindow(HwndCaleEventOpen))
        {
            ShowWindow(HwndCaleEventOpen, SW_SHOW);
            BringWindowToTop(HwndCaleEventOpen);
            UpdateWindow(HwndCaleEventOpen);
        }
        else
        {
            CALE_VIEWCREATEDATA CreateData;
            
            if(!RegisterCaleOpenClass())
                return FALSE;
            
            memset(&CreateData, 0x0, sizeof(CALE_VIEWCREATEDATA));
            CreateData.hFrameWnd = CALE_GetFrame();
            CreateData.hParent = hwnd;
			CreateData.hFocus = NULL;
            
            GetClientRect(CreateData.hFrameWnd, &rClient);
            
            HwndCaleEventOpen = CreateWindow(
                "CaleOpenWndClass",
                "",//IDP_IME_MEETING,
                WS_VISIBLE | WS_CHILD,
                rClient.left, 
                rClient.top, 
                rClient.right - rClient.left,
                rClient.bottom - rClient.top,
                CreateData.hFrameWnd,
                NULL,
                NULL, 
                (PVOID)&CreateData
                );
            
            if(!HwndCaleEventOpen)
            {
                UnRegisterCaleOpenClass();
                return FALSE;
            }
            SendMessage(CreateData.hFrameWnd, PWM_CREATECAPTIONBUTTON, 
                MAKEWPARAM(IDM_BUTTON_COMPLETE, 1), (LPARAM)IDP_IME_EDIT);
            SendMessage(CreateData.hFrameWnd, PWM_CREATECAPTIONBUTTON, 
                MAKEWPARAM(IDM_BUTTON_QUIT, 0), (LPARAM)IDP_IME_BACK);
            SendMessage(CreateData.hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
            
            SetWindowText(CreateData.hFrameWnd, IDP_IME_EVENT);
            ShowWindow(CreateData.hFrameWnd, SW_SHOW);
            UpdateWindow(CreateData.hFrameWnd);
        }
    }
    else
    {
        if(IsWindow(HwndCaleEvent))
        {
            ShowWindow(HwndCaleEvent, SW_SHOW);
            BringWindowToTop(HwndCaleEvent);
            UpdateWindow(HwndCaleEvent);
        }
        else
        {
            CALE_ENTRYCREATEDATA CreateData;
            
            if(!RegisterCaleDisplayClass())
                return FALSE;
            
            memset(&CreateData, 0x0, sizeof(CALE_ENTRYCREATEDATA));
            CreateData.hFrameWnd = CALE_GetFrame();
            CreateData.hParent = hwnd;
			CreateData.hFocus = NULL;
            
            GetClientRect(CreateData.hFrameWnd, &rClient);
            
            HwndCaleEvent = CreateWindow(
                "CaleDisplayWndClass",
                "",//IDP_IME_MEETING,
                WS_VISIBLE | WS_CHILD | WS_VSCROLL,
                rClient.left, 
                rClient.top, 
                rClient.right - rClient.left,
                rClient.bottom - rClient.top,
                CreateData.hFrameWnd,
                NULL,
                NULL, 
                (PVOID)&CreateData
                );
            
            if(!HwndCaleEvent)
            {
                UnRegisterCaleDiaplayClass();
                return FALSE;
            }
            
            SendMessage(CreateData.hFrameWnd, PWM_CREATECAPTIONBUTTON, 
                MAKEWPARAM(IDM_BUTTON_COMPLETE, 1), (LPARAM)IDP_IME_SAVE);
            SendMessage(CreateData.hFrameWnd, PWM_CREATECAPTIONBUTTON, 
                MAKEWPARAM(IDM_BUTTON_QUIT, 0), (LPARAM)IDP_IME_CANCEL);
            SendMessage(CreateData.hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
            
            SetWindowText(CreateData.hFrameWnd, IDP_IME_NEWEVENT);
            ShowWindow(CreateData.hFrameWnd, SW_SHOW);
            UpdateWindow(HwndCaleEvent);
        }
    }
    
    return TRUE;
}

static LRESULT CaleDisplayWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    PCALE_ENTRYCREATEDATA pCreateData;
    
    LRESULT lResult = TRUE;
    HDC hdc;

    static SYSTEMTIME StartSysTime;
    static SYSTEMTIME EndSysTime;
    static SYSTEMTIME TmpSysTime;
    static SYSTEMTIME DisplayTime;
    static CaleMeetingNode TmpMeet;
    static CaleAnniNode TmpAnni;
    static CaleEventNode TmpEvent;
	static BOOL bKeyDown = FALSE;
    
    SCROLLINFO tmpScroll;
    HWND TmpFocus;

    SCROLLINFO vsi;

    int hOpen = 0;
    int CmpResult = 0;
    //int TempPos;

    pCreateData = GetUserData(hWnd);

    switch(wMsgCmd)
    {
    case WM_CREATE:
        {
            DATEFORMAT forDate;
            TIMEFORMAT forTime;
			char szTmp[32];
          
			bKeyDown = FALSE;
            memcpy(pCreateData, ((LPCREATESTRUCT)lParam)->lpCreateParams, sizeof(CALE_ENTRYCREATEDATA));
            
            ConfirmRet = 0;
            hSelect = LoadImage(NULL, CALE_ICON_SELECT, IMAGE_BITMAP,0, 0, LR_LOADFROMFILE);
            hUnSelect = LoadImage(NULL, CALE_ICON_UNSELECT, IMAGE_BITMAP,0, 0, LR_LOADFROMFILE);

			SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
			CALE_SetSystemIcons(pCreateData->hFrameWnd);

			tmpScroll.cbSize = sizeof(SCROLLINFO);
            tmpScroll.fMask = SIF_ALL;
            tmpScroll.nMin = 0;
            tmpScroll.nPage = 3;
            tmpScroll.nPos = 0;

            forDate = GetDateFormt();
            forTime = GetTimeFormt();
            
            if(CurCaleType == CALE_MEETING)
            {
                tmpScroll.nMax = 7;

                if(bNewDate)
                {
                    //GetLocalTime(&StartSysTime);
                    //GetLocalTime(&EndSysTime);
                    memcpy(&StartSysTime, &CurSysTime, sizeof(SYSTEMTIME));
                    memcpy(&EndSysTime, &CurSysTime, sizeof(SYSTEMTIME));
                    EndSysTime.wHour ++;
                    if(EndSysTime.wHour > 23)
                    {
                        EndSysTime.wHour = 0;
                        CALE_DayIncrease(&EndSysTime, 1);
                    }
                }
                else
                {
                    StartSysTime.wYear = CurMeetInfo.MeetStartYear;
                    StartSysTime.wMonth = CurMeetInfo.MeetStartMonth;
                    StartSysTime.wDay = CurMeetInfo.MeetStartDay;
                    StartSysTime.wHour = CurMeetInfo.MeetStartHour;
                    StartSysTime.wMinute = CurMeetInfo.MeetStartMinute;

                    EndSysTime.wYear = CurMeetInfo.MeetEndYear;
                    EndSysTime.wMonth = CurMeetInfo.MeetEndMonth;
                    EndSysTime.wDay = CurMeetInfo.MeetEndDay;
                    EndSysTime.wHour = CurMeetInfo.MeetEndHour;
                    EndSysTime.wMinute = CurMeetInfo.MeetEndMinute;
                }

                memset(&ImeEdit, 0x0, sizeof(IMEEDIT));
                ImeEdit.hwndNotify = (HWND)hWnd;
                ImeEdit.dwAttrib = 0;
                ImeEdit.dwAscTextMax = 0;
                ImeEdit.dwUniTextMax = 0;
                ImeEdit.wPageMax = 0;
                ImeEdit.pszImeName = NULL;//"英文小写";
                ImeEdit.pszCharSet = NULL;
                ImeEdit.pszTitle = NULL;
                ImeEdit.uMsgSetText = 0;

                hSubject = CreateWindow(
                    "IMEEDIT",
                    "",
                    WS_CHILD | WS_VISIBLE | WS_TABSTOP | 
                    ES_TITLE | CS_NOSYSCTRL | ES_AUTOHSCROLL,
                    PARTWND_X, PARTWND_Y, PARTWND_WIDTH, PARTWND_HEIGHT,
                    hWnd,
                    (HMENU)ID_EDIT_SUBJECT,
                    NULL,
                    (PVOID)&ImeEdit
                    );
				sprintf(szTmp, "%s%c", IDP_IME_SUBJECT, ':');
                SendMessage(hSubject, EM_SETTITLE, 0, (LPARAM)szTmp/*(IDP_IME_SUBJECT)*/);
                SendMessage(hSubject, EM_LIMITTEXT, MAX_CALE_SUBJECT_LEN, 0);

                hLocation = CreateWindow(
                    "IMEEDIT",
                    "",
                    WS_CHILD | WS_VISIBLE | WS_TABSTOP | 
                    ES_TITLE | CS_NOSYSCTRL | ES_AUTOHSCROLL,
                    PARTWND_X, PARTWND_Y + PARTWND_HEIGHT, PARTWND_WIDTH, PARTWND_HEIGHT,
                    hWnd,
                    (HMENU)ID_EDIT_LOCATION,
                    NULL,
                    (PVOID)&ImeEdit
                    );
				sprintf(szTmp, "%s%c", IDP_IME_LOCATION, ':');
                SendMessage(hLocation, EM_SETTITLE, 0, (LPARAM)szTmp/*(IDP_IME_LOCATION)*/);
                SendMessage(hLocation, EM_LIMITTEXT, MAX_CALE_LOCATION_LEN, 0);

				sprintf(szTmp, "%s%c", IDP_IME_STARTDATE, ':');
                if((forDate == DF_DMY_DOT) || (forDate == DF_DMY_SLD) || (forDate == DF_DMY_DSH))
                {
                    hStartDate = CreateWindow(
                        "TIMEEDIT",
                        szTmp,//IDP_IME_STARTDATE,
                        WS_CHILD | WS_VISIBLE | WS_TABSTOP | 
                        TES_TITLE | CS_NOSYSCTRL | TES_DATE_DMY,
                        PARTWND_X, PARTWND_Y + PARTWND_HEIGHT * 2, PARTWND_WIDTH, PARTWND_HEIGHT,
                        hWnd,
                        (HMENU)ID_EDIT_STARTDATE,
                        NULL,
                        (PVOID)&StartSysTime
                        );
                }
                else if((forDate == DF_MDY_DOT) || (forDate == DF_MDY_SLD) || (forDate == DF_MDY_DSH))
                {
                    hStartDate = CreateWindow(
                        "TIMEEDIT",
                        szTmp,//IDP_IME_STARTDATE,
                        WS_CHILD | WS_VISIBLE | WS_TABSTOP | 
                        TES_TITLE | CS_NOSYSCTRL | TES_DATE_MDY,
                        PARTWND_X, PARTWND_Y + PARTWND_HEIGHT * 2, PARTWND_WIDTH, PARTWND_HEIGHT,
                        hWnd,
                        (HMENU)ID_EDIT_STARTDATE,
                        NULL,
                        (PVOID)&StartSysTime
                        );
                }
                else
                {
                    hStartDate = CreateWindow(
                        "TIMEEDIT",
                        szTmp,//IDP_IME_STARTDATE,
                        WS_CHILD | WS_VISIBLE | WS_TABSTOP | 
                        TES_TITLE | CS_NOSYSCTRL | TES_DATE_YMD,
                        PARTWND_X, PARTWND_Y + PARTWND_HEIGHT * 2, PARTWND_WIDTH, PARTWND_HEIGHT,
                        hWnd,
                        (HMENU)ID_EDIT_STARTDATE,
                        NULL,
                        (PVOID)&StartSysTime
                        );
                }
                if((forDate == DF_DMY_DOT) || (forDate == DF_MDY_DOT) || (forDate == DF_YMD_DOT))
                {
                    SendMessage(hStartDate, TEM_SETSEPARATOR, 0, (LPARAM)'.');
                }
                else if((forDate == DF_DMY_SLD) || (forDate == DF_MDY_SLD) || (forDate == DF_YMD_SLD))
                {
                    SendMessage(hStartDate, TEM_SETSEPARATOR, 0, (LPARAM)'/');
                }
                else
                {
                    SendMessage(hStartDate, TEM_SETSEPARATOR, 0, (LPARAM)'-');
                }

 				sprintf(szTmp, "%s%c", IDP_IME_STARTTIME, ':');
				if(forTime == TF_24)
                {
                    hStartTime = CreateWindow(
                        "TIMEEDIT",
                        szTmp,//IDP_IME_STARTTIME,
                        WS_CHILD | WS_VISIBLE | WS_TABSTOP | 
                        TES_TITLE | CS_NOSYSCTRL | TES_TIME_24HR,
                        PARTWND_X, PARTWND_Y + PARTWND_HEIGHT * 3, PARTWND_WIDTH, PARTWND_HEIGHT,
                        hWnd,
                        (HMENU)ID_EDIT_STARTTIME,
                        NULL,
                        (PVOID)&StartSysTime
                        );
                }
                else
                {
                    hStartTime = CreateWindow(
                        "TIMEEDIT",
                        szTmp,//IDP_IME_STARTTIME,
                        WS_CHILD | WS_VISIBLE | WS_TABSTOP | 
                        TES_TITLE | CS_NOSYSCTRL | TES_TIME_12HR,
                        PARTWND_X, PARTWND_Y + PARTWND_HEIGHT * 3, PARTWND_WIDTH, PARTWND_HEIGHT,
                        hWnd,
                        (HMENU)ID_EDIT_STARTTIME,
                        NULL,
                        (PVOID)&StartSysTime
                        );
                }

   				sprintf(szTmp, "%s%c", IDP_IME_ENDDATE, ':');
				if((forDate == DF_DMY_DOT) || (forDate == DF_DMY_SLD) || (forDate == DF_DMY_DSH))
                {
                    hEndDate = CreateWindow(
                        "TIMEEDIT",
                        szTmp,//IDP_IME_ENDDATE,
                        WS_CHILD | WS_VISIBLE | WS_TABSTOP | 
                        TES_TITLE | CS_NOSYSCTRL | TES_DATE_DMY,
                        PARTWND_X, PARTWND_Y + PARTWND_HEIGHT * 4, PARTWND_WIDTH, PARTWND_HEIGHT,
                        hWnd,
                        (HMENU)ID_EDIT_ENDDATE,
                        NULL,
                        (PVOID)&EndSysTime
                        );
                }
                else if((forDate == DF_MDY_DOT) || (forDate == DF_MDY_SLD) || (forDate == DF_MDY_DSH))
                {
                    hEndDate = CreateWindow(
                        "TIMEEDIT",
                        szTmp,//IDP_IME_ENDDATE,
                        WS_CHILD | WS_VISIBLE | WS_TABSTOP | 
                        TES_TITLE | CS_NOSYSCTRL | TES_DATE_MDY,
                        PARTWND_X, PARTWND_Y + PARTWND_HEIGHT * 4, PARTWND_WIDTH, PARTWND_HEIGHT,
                        hWnd,
                        (HMENU)ID_EDIT_ENDDATE,
                        NULL,
                        (PVOID)&EndSysTime
                        );
                }
                else
                {
                    hEndDate = CreateWindow(
                        "TIMEEDIT",
                        szTmp,//IDP_IME_ENDDATE,
                        WS_CHILD | WS_VISIBLE | WS_TABSTOP | 
                        TES_TITLE | CS_NOSYSCTRL | TES_DATE_YMD,
                        PARTWND_X, PARTWND_Y + PARTWND_HEIGHT * 4, PARTWND_WIDTH, PARTWND_HEIGHT,
                        hWnd,
                        (HMENU)ID_EDIT_ENDDATE,
                        NULL,
                        (PVOID)&EndSysTime
                        );
                }
                if((forDate == DF_DMY_DOT) || (forDate == DF_MDY_DOT) || (forDate == DF_YMD_DOT))
                {
                    SendMessage(hEndDate, TEM_SETSEPARATOR, 0, (LPARAM)'.');
                }
                else if((forDate == DF_DMY_SLD) || (forDate == DF_MDY_SLD) || (forDate == DF_YMD_SLD))
                {
                    SendMessage(hEndDate, TEM_SETSEPARATOR, 0, (LPARAM)'/');
                }
                else
                {
                    SendMessage(hEndDate, TEM_SETSEPARATOR, 0, (LPARAM)'-');
                }

 				sprintf(szTmp, "%s%c", IDP_IME_ENDTIME, ':');
                if(forTime == TF_24)
                {
                    hEndTime = CreateWindow(
                        "TIMEEDIT",
                        szTmp,//IDP_IME_ENDTIME,
                        WS_CHILD | WS_VISIBLE | WS_TABSTOP | 
                        TES_TITLE | CS_NOSYSCTRL | TES_TIME_24HR,
                        PARTWND_X, PARTWND_Y + PARTWND_HEIGHT * 5, PARTWND_WIDTH, PARTWND_HEIGHT,
                        hWnd,
                        (HMENU)ID_EDIT_ENDTIME,
                        NULL,
                        (PVOID)&EndSysTime
                        );
                }
                else
                {
                    hEndTime = CreateWindow(
                        "TIMEEDIT",
                        szTmp,//IDP_IME_ENDTIME,
                        WS_CHILD | WS_VISIBLE | WS_TABSTOP | 
                        TES_TITLE | CS_NOSYSCTRL | TES_TIME_12HR,
                        PARTWND_X, PARTWND_Y + PARTWND_HEIGHT * 5, PARTWND_WIDTH, PARTWND_HEIGHT,
                        hWnd,
                        (HMENU)ID_EDIT_ENDTIME,
                        NULL,
                        (PVOID)&EndSysTime
                        );
                }
                
                sprintf(szTmp, "%s%c", IDP_IME_REPEAT, ':');
				hRepeat = CreateWindow(
                    "SPINBOXEX",
                    szTmp,//IDP_IME_REPEAT,
                    WS_CHILD | WS_VISIBLE | WS_TABSTOP | SSBS_SWAPFONT |
                    CS_NOSYSCTRL | SSBS_LEFT | SSBS_ARROWRIGHT,
                    PARTWND_X, PARTWND_Y + PARTWND_HEIGHT * 6, PARTWND_WIDTH, PARTWND_HEIGHT,
                    hWnd,
                    (HMENU)ID_SPIN_REP,
                    NULL,
                    NULL
                    );

				SendMessage(hRepeat, SSBM_ADDSTRING, 0, (LPARAM)(IDP_IME_NONE));
                SendMessage(hRepeat, SSBM_ADDSTRING, 0, (LPARAM)(IDP_IME_EVERYDAY));
                SendMessage(hRepeat, SSBM_ADDSTRING, 0, (LPARAM)(IDP_IME_EVERYWEEK));
                SendMessage(hRepeat, SSBM_ADDSTRING, 0, (LPARAM)(IDP_IME_EVERYTWOWEEK));
                SendMessage(hRepeat, SSBM_ADDSTRING, 0, (LPARAM)(IDP_IME_EVERYMONTH));

                sprintf(szTmp, "%s%c", IDP_IME_ALARM, ':');
                hAlarm = CreateWindow(
                    "SPINBOXEX",
                    szTmp,//IDP_IME_ALARM,
                    WS_CHILD | WS_VISIBLE | WS_TABSTOP | SSBS_SWAPFONT |
                    CS_NOSYSCTRL | SSBS_LEFT | SSBS_ARROWRIGHT,
                    PARTWND_X, PARTWND_Y + PARTWND_HEIGHT * 7, PARTWND_WIDTH, PARTWND_HEIGHT,
                    hWnd,
                    (HMENU)ID_SPIN_ALARM,
                    NULL,
                    NULL
                    );

                SendMessage(hAlarm, SSBM_ADDSTRING, 0, (LPARAM)(IDP_ALARM_NO));
                SendMessage(hAlarm, SSBM_ADDSTRING, 0, (LPARAM)(IDP_ALARM_ON));
                SendMessage(hAlarm, SSBM_ADDSTRING, 0, (LPARAM)(IDP_ALARM_5MIN));
                SendMessage(hAlarm, SSBM_ADDSTRING, 0, (LPARAM)(IDP_ALARM_10MIN));
                SendMessage(hAlarm, SSBM_ADDSTRING, 0, (LPARAM)(IDP_ALARM_15MIN));
                SendMessage(hAlarm, SSBM_ADDSTRING, 0, (LPARAM)(IDP_ALARM_30MIN));
                SendMessage(hAlarm, SSBM_ADDSTRING, 0, (LPARAM)(IDP_ALARM_45MIN));
                SendMessage(hAlarm, SSBM_ADDSTRING, 0, (LPARAM)(IDP_ALARM_1HOUR));
                SendMessage(hAlarm, SSBM_ADDSTRING, 0, (LPARAM)(IDP_ALARM_6HOUR));
                SendMessage(hAlarm, SSBM_ADDSTRING, 0, (LPARAM)(IDP_ALARM_12HOUR));
                SendMessage(hAlarm, SSBM_ADDSTRING, 0, (LPARAM)(IDP_ALARM_1DAY));
                SendMessage(hAlarm, SSBM_ADDSTRING, 0, (LPARAM)(IDP_ALARM_1WEEK));

#ifdef _NOKIA_CALE_
                hSync = CreateWindow(
                    "SPINBOXEX",
                    IDP_IME_SYNC,
                    WS_CHILD | WS_VISIBLE | WS_TABSTOP | 
                    CS_NOSYSCTRL | SSBS_LEFT | SSBS_ARROWRIGHT,
                    PARTWND_X, PARTWND_Y + PARTWND_HEIGHT * 8, PARTWND_WIDTH, PARTWND_HEIGHT,
                    hWnd,
                    (HMENU)ID_SPIN_SYNC,
                    NULL,
                    NULL
                    );
                SendMessage(hSync, SSBM_ADDSTRING, 0, (LPARAM)(IDP_IME_NOSYNC));
                SendMessage(hSync, SSBM_ADDSTRING, 0, (LPARAM)(IDP_IME_NOTPUB));
                SendMessage(hSync, SSBM_ADDSTRING, 0, (LPARAM)(IDP_IME_PUB));
#endif

                if(bNewDate)
                {
                    SetWindowText(hSubject, "");
                    SetWindowText(hLocation, "");
                    SendMessage(hAlarm, SSBM_SETCURSEL, 1, 0);
                    SendMessage(hRepeat, SSBM_SETCURSEL, 0, 0);
#ifdef _NOKIA_CALE_
                    SendMessage(hSync, SSBM_SETCURSEL, 0, 0);
#endif
                }
                else
                {
                    SetWindowText(hSubject, CurMeetInfo.MeetSubject);
                    SetWindowText(hLocation, CurMeetInfo.MeetLocation);
					PostMessage(hWnd, IDM_IME_SETCURSOR, (WPARAM)hSubject, 0);
					PostMessage(hWnd, IDM_IME_SETCURSOR, (WPARAM)hLocation, 0);
				//	SendMessage(hSubject, EM_SETSEL, -1, -1);
				//	SendMessage(hLocation, EM_SETSEL, -1, -1);
                    if(CurMeetInfo.MeetAlarmFlag < 12)
                    {
                        SendMessage(hAlarm, SSBM_SETCURSEL, CurMeetInfo.MeetAlarmFlag, 0);
                    }
                    else
                    {
                        SendMessage(hAlarm, SSBM_SETCURSEL, 0, 0);
                    }

                    if(!CurMeetInfo.MeetRepeatFlag)
                    {
                        SendMessage(hRepeat, SSBM_SETCURSEL, 0, 0);
                    }
                    else
                    {
                        SendMessage(hRepeat, SSBM_SETCURSEL, CurMeetInfo.MeetRepeatStyle, 0);
                    }
#ifdef _NOKIA_CALE_
                    SendMessage(hSync, SSBM_SETCURSEL, CurMeetInfo.MeetSync, 0);
#endif
                }
			//	SetFocus(hSubject);
				PostMessage(hWnd, IDM_MEET_SETSEL, (WPARAM)(pCreateData->hFocus), 0);
				if(1 == (int)pCreateData->hFocus)
					pCreateData->hFocus = hLocation;
				else if(2 == (int)pCreateData->hFocus)
				{
					pCreateData->hFocus = hStartDate;
					tmpScroll.nPos = 1;
				}
				else if(3 == (int)pCreateData->hFocus)
				{
					pCreateData->hFocus = hStartTime;
					tmpScroll.nPos = 2;
				}
				else if(4 == (int)pCreateData->hFocus)
				{
					pCreateData->hFocus = hEndDate;
					tmpScroll.nPos = 3;
				}
				else if(5 == (int)pCreateData->hFocus)
				{
					pCreateData->hFocus = hEndTime;
					tmpScroll.nPos = 4;
				}
				else if(6 == (int)pCreateData->hFocus)
				{
					pCreateData->hFocus = hRepeat;
					tmpScroll.nPos = 5;
					SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_SELECT);
				}
				else if(7 == (int)pCreateData->hFocus)
				{
					pCreateData->hFocus = hAlarm;
					tmpScroll.nPos = 5;
					SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_SELECT);
				}
				else
					pCreateData->hFocus = hSubject;

            }
            else if(CurCaleType == CALE_ANNI)
            {
                tmpScroll.nMax = 2;

                if(bNewDate)
                {
                    //GetLocalTime(&StartSysTime);
                    memcpy(&StartSysTime, &CurSysTime, sizeof(SYSTEMTIME));
                    memcpy(&DisplayTime, &CurSysTime, sizeof(SYSTEMTIME));
                }
                else
                {
                    DisplayTime.wYear = CurAnniInfo.AnniDisYear;
                    DisplayTime.wMonth = CurAnniInfo.AnniDisMonth;
                    DisplayTime.wDay = CurAnniInfo.AnniDisDay;
                    DisplayTime.wHour = 8;
                    DisplayTime.wMinute = 0;
                    if(CurAnniInfo.AnniAlarmFlag)
                    {
                        StartSysTime.wYear = CurAnniInfo.AnniYear;
                        StartSysTime.wMonth = CurAnniInfo.AnniMonth;
                        StartSysTime.wDay = CurAnniInfo.AnniDay;
                        StartSysTime.wHour = CurAnniInfo.AnniHour;
                        StartSysTime.wMinute = CurAnniInfo.AnniMinute;
                    }
                    else
                    {
                        memcpy(&StartSysTime, &CurSysTime, sizeof(SYSTEMTIME));
                    }
                }

                memset(&ImeEdit, 0x0, sizeof(IMEEDIT));
                ImeEdit.hwndNotify = (HWND)hWnd;
                ImeEdit.dwAttrib = 0;
                ImeEdit.dwAscTextMax = 0;
                ImeEdit.dwUniTextMax = 0;
                ImeEdit.wPageMax = 0;
                ImeEdit.pszImeName = NULL;//"英文小写";
                ImeEdit.pszCharSet = NULL;
                ImeEdit.pszTitle = NULL;
                ImeEdit.uMsgSetText = 0;

                hSubject = CreateWindow(
                    "IMEEDIT",
                    "",
                    WS_CHILD | WS_VISIBLE | WS_TABSTOP | 
                    ES_TITLE | CS_NOSYSCTRL | ES_AUTOHSCROLL,
                    PARTWND_X, PARTWND_Y, PARTWND_WIDTH, PARTWND_HEIGHT,
                    hWnd,
                    (HMENU)ID_EDIT_SUBJECT,
                    NULL,
                    (PVOID)&ImeEdit
                    );
				sprintf(szTmp, "%s%c", IDP_IME_SUBJECT, ':');
				SendMessage(hSubject, EM_SETTITLE, 0, (LPARAM)szTmp/*(IDP_IME_SUBJECT)*/);
                SendMessage(hSubject, EM_LIMITTEXT, MAX_CALE_SUBJECT_LEN, 0);

				sprintf(szTmp, "%s%c", IDP_IME_DATE, ':');
                if((forDate == DF_DMY_DOT) || (forDate == DF_DMY_SLD) || (forDate == DF_DMY_DSH))
                {
                    hStartDate = CreateWindow(
                        "TIMEEDIT",
                        szTmp,//IDP_IME_DATE,
                        WS_CHILD | WS_VISIBLE | WS_TABSTOP | 
                        TES_TITLE | CS_NOSYSCTRL | TES_DATE_DMY,
                        PARTWND_X, PARTWND_Y + PARTWND_HEIGHT * 1, PARTWND_WIDTH, PARTWND_HEIGHT,
                        hWnd,
                        (HMENU)ID_EDIT_STARTDATE,
                        NULL,
                        (PVOID)&DisplayTime
                        );
                }
                else if((forDate == DF_MDY_DOT) || (forDate == DF_MDY_SLD) || (forDate == DF_MDY_DSH))
                {
                    hStartDate = CreateWindow(
                        "TIMEEDIT",
                        szTmp,//IDP_IME_DATE,
                        WS_CHILD | WS_VISIBLE | WS_TABSTOP | 
                        TES_TITLE | CS_NOSYSCTRL | TES_DATE_MDY,
                        PARTWND_X, PARTWND_Y + PARTWND_HEIGHT * 1, PARTWND_WIDTH, PARTWND_HEIGHT,
                        hWnd,
                        (HMENU)ID_EDIT_STARTDATE,
                        NULL,
                        (PVOID)&DisplayTime
                        );
                }
                else
                {
                    hStartDate = CreateWindow(
                        "TIMEEDIT",
                        szTmp,//IDP_IME_DATE,
                        WS_CHILD | WS_VISIBLE | WS_TABSTOP | 
                        TES_TITLE | CS_NOSYSCTRL | TES_DATE_YMD,
                        PARTWND_X, PARTWND_Y + PARTWND_HEIGHT * 1, PARTWND_WIDTH, PARTWND_HEIGHT,
                        hWnd,
                        (HMENU)ID_EDIT_STARTDATE,
                        NULL,
                        (PVOID)&DisplayTime
                        );
                }
                if((forDate == DF_DMY_DOT) || (forDate == DF_MDY_DOT) || (forDate == DF_YMD_DOT))
                {
                    SendMessage(hStartDate, TEM_SETSEPARATOR, 0, (LPARAM)'.');
                }
                else if((forDate == DF_DMY_SLD) || (forDate == DF_MDY_SLD) || (forDate == DF_YMD_SLD))
                {
                    SendMessage(hStartDate, TEM_SETSEPARATOR, 0, (LPARAM)'/');
                }
                else
                {
                    SendMessage(hStartDate, TEM_SETSEPARATOR, 0, (LPARAM)'-');
                }
                

				sprintf(szTmp, "%s%c", IDP_IME_ALARM, ':');
                hAlarm = CreateWindow(
                    "SPINBOXEX",
                    szTmp,//IDP_IME_ALARM,
                    WS_CHILD | WS_VISIBLE | WS_TABSTOP | SSBS_SWAPFONT |
                    CS_NOSYSCTRL | SSBS_LEFT | SSBS_ARROWRIGHT,
                    PARTWND_X, PARTWND_Y + PARTWND_HEIGHT * 2, PARTWND_WIDTH, PARTWND_HEIGHT,
                    hWnd,
                    (HMENU)ID_SPIN_ALARM,
                    NULL,
                    NULL
                    );
                SendMessage(hAlarm, SSBM_ADDSTRING, 0, (LPARAM)(IDP_IME_OFF));
                
                {
                    char cTmpDis[17];
                    
                    memset(cTmpDis, 0x0, sizeof(char) * 17);
				//	CALE_DateTimeSysToChar(&StartSysTime, cTmpDis);
					sprintf(cTmpDis, "%02d.%02d.%4d %02d:%02d", StartSysTime.wDay, StartSysTime.wMonth,
						StartSysTime.wYear, StartSysTime.wHour, StartSysTime.wMinute);
                    SendMessage(hAlarm, SSBM_ADDSTRING, 0, (LPARAM)cTmpDis);
                }

#ifdef _NOKIA_CALE_
                hSync = CreateWindow(
                    "SPINBOXEX",
                    IDP_IME_SYNC,
                    WS_CHILD | WS_VISIBLE | WS_TABSTOP | 
                    CS_NOSYSCTRL | SSBS_LEFT | SSBS_ARROWRIGHT,
                    PARTWND_X, PARTWND_Y + PARTWND_HEIGHT * 3, PARTWND_WIDTH, PARTWND_HEIGHT,
                    hWnd,
                    (HMENU)ID_SPIN_SYNC,
                    NULL,
                    NULL
                    );
                SendMessage(hSync, SSBM_ADDSTRING, 0, (LPARAM)(IDP_IME_NOSYNC));
                SendMessage(hSync, SSBM_ADDSTRING, 0, (LPARAM)(IDP_IME_NOTPUB));
                SendMessage(hSync, SSBM_ADDSTRING, 0, (LPARAM)(IDP_IME_PUB));
#endif

                if(bNewDate)
                {
                    SetWindowText(hSubject, "");
                    SendMessage(hAlarm, SSBM_SETCURSEL, 0, 0);
#ifdef _NOKIA_CALE_
                    SendMessage(hSync, SSBM_SETCURSEL, 0, 0);
#endif
                }
                else
                {
                    SetWindowText(hSubject, CurAnniInfo.AnniOccasion);
				//	SendMessage(hSubject, EM_SETSEL, -1, -1);
					PostMessage(hWnd, IDM_IME_SETCURSOR, (WPARAM)hSubject, 0);
                    if(CurAnniInfo.AnniAlarmFlag)
                    {
                        SendMessage(hAlarm, SSBM_SETCURSEL, 1, 0);
                    }
                    else
                    {
                        SendMessage(hAlarm, SSBM_SETCURSEL, 0, 0);
                    }
#ifdef _NOKIA_CALE_
                    SendMessage(hSync, SSBM_SETCURSEL, CurAnniInfo.AnniSync, 0);
#endif
                }
            //    SetFocus(hSubject);
				if(1 == (int)pCreateData->hFocus)
					pCreateData->hFocus = hStartDate;
				else if(2 == (int)pCreateData->hFocus)
				{
					pCreateData->hFocus = hAlarm;
					SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_SELECT);
				}
				else
					pCreateData->hFocus = hSubject;
            }
            else if(CurCaleType == CALE_EVENT)
            {
                tmpScroll.nMax = 2;
                
                if(bNewDate)
                {
                    //GetLocalTime(&StartSysTime);
                    //GetLocalTime(&EndSysTime);
                    memcpy(&StartSysTime, &CurSysTime, sizeof(SYSTEMTIME));
                    memcpy(&EndSysTime, &CurSysTime, sizeof(SYSTEMTIME));
                }
                else
                {
                    StartSysTime.wYear = CurEventInfo.EventStartYear;
                    StartSysTime.wMonth = CurEventInfo.EventStartMonth;
                    StartSysTime.wDay = CurEventInfo.EventStartDay;
                    StartSysTime.wHour = CurEventInfo.EventStartHour;
                    StartSysTime.wMinute = CurEventInfo.EventStartMinute;
                    
                    EndSysTime.wYear = CurEventInfo.EventEndYear;
                    EndSysTime.wMonth = CurEventInfo.EventEndMonth;
                    EndSysTime.wDay = CurEventInfo.EventEndDay;
                    EndSysTime.wHour = CurEventInfo.EventEndHour;
                    EndSysTime.wMinute = CurEventInfo.EventEndMinute;
                }

                memset(&ImeEdit, 0x0, sizeof(IMEEDIT));
                ImeEdit.hwndNotify = (HWND)hWnd;
                ImeEdit.dwAttrib = 0;
                ImeEdit.dwAscTextMax = 0;
                ImeEdit.dwUniTextMax = 0;
                ImeEdit.wPageMax = 0;
                ImeEdit.pszImeName = NULL;//"英文小写";
                ImeEdit.pszCharSet = NULL;
                ImeEdit.pszTitle = NULL;
                ImeEdit.uMsgSetText = 0;
                
                hSubject = CreateWindow(
                    "IMEEDIT",
                    "",
                    WS_CHILD | WS_VISIBLE | WS_TABSTOP | 
                    ES_TITLE | CS_NOSYSCTRL | ES_AUTOHSCROLL,
                    PARTWND_X, PARTWND_Y, PARTWND_WIDTH, PARTWND_HEIGHT,
                    hWnd,
                    (HMENU)ID_EDIT_SUBJECT,
                    NULL,
                    (PVOID)&ImeEdit
                    );
				sprintf(szTmp, "%s%c", IDP_IME_SUBJECT, ':');
                SendMessage(hSubject, EM_SETTITLE, 0, (LPARAM)szTmp/*(IDP_IME_SUBJECT)*/);
                SendMessage(hSubject, EM_LIMITTEXT, MAX_CALE_SUBJECT_LEN, 0);
                
				sprintf(szTmp, "%s%c", IDP_IME_STARTDATE, ':');
                if((forDate == DF_DMY_DOT) || (forDate == DF_DMY_SLD) || (forDate == DF_DMY_DSH))
                {
                    hStartDate = CreateWindow(
                        "TIMEEDIT",
                        szTmp,//IDP_IME_STARTDATE,
                        WS_CHILD | WS_VISIBLE | WS_TABSTOP | 
                        TES_TITLE | CS_NOSYSCTRL | TES_DATE_DMY,
                        PARTWND_X, PARTWND_Y + PARTWND_HEIGHT * 1, PARTWND_WIDTH, PARTWND_HEIGHT,
                        hWnd,
                        (HMENU)ID_EDIT_STARTDATE,
                        NULL,
                        (PVOID)&StartSysTime
                        );
                }
                else if((forDate == DF_MDY_DOT) || (forDate == DF_MDY_SLD) || (forDate == DF_MDY_DSH))
                {
                    hStartDate = CreateWindow(
                        "TIMEEDIT",
                        szTmp,//IDP_IME_STARTDATE,
                        WS_CHILD | WS_VISIBLE | WS_TABSTOP | 
                        TES_TITLE | CS_NOSYSCTRL | TES_DATE_MDY,
                        PARTWND_X, PARTWND_Y + PARTWND_HEIGHT * 1, PARTWND_WIDTH, PARTWND_HEIGHT,
                        hWnd,
                        (HMENU)ID_EDIT_STARTDATE,
                        NULL,
                        (PVOID)&StartSysTime
                        );
                }
                else
                {
                    hStartDate = CreateWindow(
                        "TIMEEDIT",
                        szTmp,//IDP_IME_STARTDATE,
                        WS_CHILD | WS_VISIBLE | WS_TABSTOP | 
                        TES_TITLE | CS_NOSYSCTRL | TES_DATE_YMD,
                        PARTWND_X, PARTWND_Y + PARTWND_HEIGHT * 1, PARTWND_WIDTH, PARTWND_HEIGHT,
                        hWnd,
                        (HMENU)ID_EDIT_STARTDATE,
                        NULL,
                        (PVOID)&StartSysTime
                        );
                }
                if((forDate == DF_DMY_DOT) || (forDate == DF_MDY_DOT) || (forDate == DF_YMD_DOT))
                {
                    SendMessage(hStartDate, TEM_SETSEPARATOR, 0, (LPARAM)'.');
                }
                else if((forDate == DF_DMY_SLD) || (forDate == DF_MDY_SLD) || (forDate == DF_YMD_SLD))
                {
                    SendMessage(hStartDate, TEM_SETSEPARATOR, 0, (LPARAM)'/');
                }
                else
                {
                    SendMessage(hStartDate, TEM_SETSEPARATOR, 0, (LPARAM)'-');
                }

				sprintf(szTmp, "%s%c", IDP_IME_ENDDATE, ':');
                if((forDate == DF_DMY_DOT) || (forDate == DF_DMY_SLD) || (forDate == DF_DMY_DSH))
                {
                    hEndDate = CreateWindow(
                        "TIMEEDIT",
                        szTmp,//IDP_IME_ENDDATE,
                        WS_CHILD | WS_VISIBLE | WS_TABSTOP | 
                        TES_TITLE | CS_NOSYSCTRL | TES_DATE_DMY,
                        PARTWND_X, PARTWND_Y + PARTWND_HEIGHT * 2, PARTWND_WIDTH, PARTWND_HEIGHT,
                        hWnd,
                        (HMENU)ID_EDIT_ENDDATE,
                        NULL,
                        (PVOID)&EndSysTime
                        );
                }
                else if((forDate == DF_MDY_DOT) || (forDate == DF_MDY_SLD) || (forDate == DF_MDY_DSH))
                {
                    hEndDate = CreateWindow(
                        "TIMEEDIT",
                        szTmp,//IDP_IME_ENDDATE,
                        WS_CHILD | WS_VISIBLE | WS_TABSTOP | 
                        TES_TITLE | CS_NOSYSCTRL | TES_DATE_MDY,
                        PARTWND_X, PARTWND_Y + PARTWND_HEIGHT * 2, PARTWND_WIDTH, PARTWND_HEIGHT,
                        hWnd,
                        (HMENU)ID_EDIT_ENDDATE,
                        NULL,
                        (PVOID)&EndSysTime
                        );
                }
                else
                {
                    hEndDate = CreateWindow(
                        "TIMEEDIT",
                        szTmp,//IDP_IME_ENDDATE,
                        WS_CHILD | WS_VISIBLE | WS_TABSTOP | 
                        TES_TITLE | CS_NOSYSCTRL | TES_DATE_YMD,
                        PARTWND_X, PARTWND_Y + PARTWND_HEIGHT * 2, PARTWND_WIDTH, PARTWND_HEIGHT,
                        hWnd,
                        (HMENU)ID_EDIT_ENDDATE,
                        NULL,
                        (PVOID)&EndSysTime
                        );
                }
                if((forDate == DF_DMY_DOT) || (forDate == DF_MDY_DOT) || (forDate == DF_YMD_DOT))
                {
                    SendMessage(hEndDate, TEM_SETSEPARATOR, 0, (LPARAM)'.');
                }
                else if((forDate == DF_DMY_SLD) || (forDate == DF_MDY_SLD) || (forDate == DF_YMD_SLD))
                {
                    SendMessage(hEndDate, TEM_SETSEPARATOR, 0, (LPARAM)'/');
                }
                else
                {
                    SendMessage(hEndDate, TEM_SETSEPARATOR, 0, (LPARAM)'-');
                }

#ifdef _NOKIA_CALE_
                hSync = CreateWindow(
                    "SPINBOXEX",
                    IDP_IME_SYNC,
                    WS_CHILD | WS_VISIBLE | WS_TABSTOP | 
                    CS_NOSYSCTRL | SSBS_LEFT | SSBS_ARROWRIGHT,
                    PARTWND_X, PARTWND_Y + PARTWND_HEIGHT * 3, PARTWND_WIDTH, PARTWND_HEIGHT,
                    hWnd,
                    (HMENU)ID_SPIN_SYNC,
                    NULL,
                    NULL
                    );
                SendMessage(hSync, SSBM_ADDSTRING, 0, (LPARAM)(IDP_IME_NOSYNC));
                SendMessage(hSync, SSBM_ADDSTRING, 0, (LPARAM)(IDP_IME_NOTPUB));
                SendMessage(hSync, SSBM_ADDSTRING, 0, (LPARAM)(IDP_IME_PUB));
#endif

                if(bNewDate)
                {
                    SetWindowText(hSubject, "");
#ifdef _NOKIA_CALE_
                    SendMessage(hSync, SSBM_SETCURSEL, 0, 0);
#endif
                }
                else
                {
                    SetWindowText(hSubject, CurEventInfo.EventSubject);
				//	SendMessage(hSubject, EM_SETSEL, -1, -1);
					PostMessage(hWnd, IDM_IME_SETCURSOR, (WPARAM)hSubject, 0);
#ifdef _NOKIA_CALE_
                    SendMessage(hSync, SSBM_SETCURSEL, CurEventInfo.EventSync, 0);
#endif
                }
            //    SetFocus(hSubject);
				if(1 == (int)pCreateData->hFocus)
					pCreateData->hFocus = hStartDate;
				else if(2 == (int)pCreateData->hFocus)
					pCreateData->hFocus = hEndDate;
				else
					pCreateData->hFocus = hSubject;
            }

       //     pCreateData->hFocus = hSubject;
            SetFocus(pCreateData->hFocus);

            SetScrollInfo(hWnd, SB_VERT, &tmpScroll, TRUE);
        }
    	break;
    
	case IDM_IME_SETCURSOR:
		SendMessage((HWND)wParam, EM_SETSEL, -1, -1);
		break;

	case IDM_MEET_SETSEL:
		switch(wParam)
		{
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
			ScrollWindow(hWnd, 0, (PARTWND_HEIGHT * (1-wParam)), NULL, NULL);
			break;

		case 7:
			ScrollWindow(hWnd, 0, (PARTWND_HEIGHT * (-5)), NULL, NULL);
			break;

		default:
			break;
		}
		UpdateWindow(hWnd);
		break;

    case WM_INITMENU:
        {
        }
        break;

    case WM_PAINT:
        {
            hdc = BeginPaint(hWnd, NULL);
            EndPaint(hWnd, NULL);
        }
    	break;

    case WM_ACTIVATE:
    case PWM_SHOWWINDOW:
        {
            HWND hTmpAlarm;
            HWND hTmpRep;

			CALE_SetSystemIcons(pCreateData->hFrameWnd);

            hTmpRep = GetDlgItem(hWnd, ID_SPIN_REP);
            hTmpAlarm = GetDlgItem(hWnd, ID_SPIN_ALARM);

            SetFocus(pCreateData->hFocus);
            
            SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDP_IME_SAVE);
            SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDP_IME_CANCEL);

            if((pCreateData->hFocus == hTmpRep) || (pCreateData->hFocus == hTmpAlarm))
            {
                SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_SELECT);
            }
            else
            {
                SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
            }
            
            if(CurCaleType == CALE_MEETING)
            {
                SetWindowText(pCreateData->hFrameWnd, IDP_IME_NEWMEETING);
            }
            else if(CurCaleType == CALE_ANNI)
            {
                SetWindowText(pCreateData->hFrameWnd, IDP_IME_NEWANNI);
            }
            else if(CurCaleType == CALE_EVENT)
            {
                SetWindowText(pCreateData->hFrameWnd, IDP_IME_NEWEVENT);
            }
        }
        break;

    case WM_COMMAND:
        {
            switch(LOWORD(wParam))
            {
            case IDM_BUTTON_COMPLETE:
                {
					bUpdated = TRUE;
                    SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
                    
                    if(CurCaleType == CALE_MEETING)
                    {
                        memset(&TmpMeet, 0x0, sizeof(CaleMeetingNode));
                        GetWindowText(hSubject, TmpMeet.MeetSubject, MAX_CALE_SUBJECT_LEN + 1);
                        GetWindowText(hLocation, TmpMeet.MeetLocation, MAX_CALE_LOCATION_LEN + 1);

                        SendMessage(hStartTime, TEM_GETTIME, 0, (LPARAM)&TmpSysTime);
                        TmpMeet.MeetStartHour = TmpSysTime.wHour;
                        TmpMeet.MeetStartMinute = TmpSysTime.wMinute;
                        SendMessage(hStartDate, TEM_GETTIME, 0, (LPARAM)&TmpSysTime);
                        TmpMeet.MeetStartYear = TmpSysTime.wYear;
                        TmpMeet.MeetStartMonth = TmpSysTime.wMonth;
                        TmpMeet.MeetStartDay = TmpSysTime.wDay;
                        SendMessage(hEndTime, TEM_GETTIME, 0, (LPARAM)&TmpSysTime);
                        TmpMeet.MeetEndHour = TmpSysTime.wHour;
                        TmpMeet.MeetEndMinute = TmpSysTime.wMinute;
                        SendMessage(hEndDate, TEM_GETTIME, 0, (LPARAM)&TmpSysTime);
                        TmpMeet.MeetEndYear = TmpSysTime.wYear;
                        TmpMeet.MeetEndMonth = TmpSysTime.wMonth;
                        TmpMeet.MeetEndDay = TmpSysTime.wDay;
                        TmpMeet.MeetStartWeek = CALE_CalcWkday(&TmpSysTime);

                        TmpMeet.MeetAlarmFlag = SendMessage(hAlarm, SSBM_GETCURSEL, NULL, NULL);
                        TmpMeet.MeetRepeatStyle = SendMessage(hRepeat, SSBM_GETCURSEL, NULL, NULL);
                        if(TmpMeet.MeetRepeatStyle == 0)
                        {
                            TmpMeet.MeetRepeatFlag = CALE_UNREP;
                        }
                        else
                        {
                            TmpMeet.MeetRepeatFlag = CALE_REP;
                        }
#ifdef _NOKIA_CALE_
                        TmpMeet.MeetSync = SendMessage(hSync, SSBM_GETCURSEL, NULL, NULL);
#endif
                        
                        //plxconfirmwin return
                        if(ConfirmRet == 1)
                        {
                            ConfirmRet = 0;
                            TmpMeet.LocalFlag = CALE_LOCAL;
                            hOpen = CALE_OpenApi(CALE_MEETING);
                            if(hOpen == CALE_FAIL)
                            {
                                //memset(&CurMeetInfo, 0x0, sizeof(CaleMeetingNode));
                                memset(&TmpMeet, 0x0, sizeof(CaleMeetingNode));
                                break;
                            }
                            if(bNewDate)
                            {
                                CurRecord = CALE_AddApi(CALE_MEETING, hOpen, 
                                    (BYTE *)&TmpMeet, sizeof(CaleMeetingNode));

                                if(CurRecord != CALE_FAIL)
                                {
                                    memset(&CurMeetInfo, 0x0, sizeof(CaleMeetingNode));
                                    memcpy(&CurMeetInfo, &TmpMeet, sizeof(CaleMeetingNode));
                                    PLXTipsWin(NULL, NULL, NULL, IDP_STRING_SAVESUCC, NULL, 
                                        Notify_Success, IDP_IME_OK, NULL, WAITTIMEOUT);
                                }
                                else
                                {
                                    PLXTipsWin(NULL, NULL, NULL, IDP_STRING_SAVEFAIL, NULL,
                                        Notify_Failure, IDP_IME_OK, NULL, WAITTIMEOUT);
                                }
                                CALE_CloseApi(hOpen);
                                
                                if(TmpMeet.MeetAlarmFlag)
                                {
                                    CALE_NewMeet(&TmpMeet, CurRecord, 0);
                                }
                            }
                            else
                            {
                                if(CurMeetInfo.MeetAlarmFlag)
                                {
                                    CALE_KillAlarm(CALE_MEETING, CurRecord);
                                }
                                CurMeetInfo.LocalFlag = CALE_UNLOCAL;
                                if(CALE_DelApi(CALE_MEETING, hOpen, CurRecord, (BYTE *)&CurMeetInfo, 
                                    sizeof(CaleMeetingNode)))
                                {
                                    CurRecord = CALE_AddApi(CALE_MEETING, hOpen, 
                                        (BYTE *)&TmpMeet, sizeof(CaleMeetingNode));
                                    
                                    if(CurRecord != CALE_FAIL)
                                    {
                                        memset(&CurMeetInfo, 0x0, sizeof(CaleMeetingNode));
                                        memcpy(&CurMeetInfo, &TmpMeet, sizeof(CaleMeetingNode));
                                        PLXTipsWin(NULL, NULL, NULL, IDP_STRING_SAVESUCC, NULL,
                                            Notify_Success, IDP_IME_OK, NULL, WAITTIMEOUT);
                                    }
                                    else
                                    {
										CALE_Print("\r\n[calendar]: CALE_AddApi failed!\r\n");
                                        PLXTipsWin(NULL, NULL, NULL, IDP_STRING_SAVEFAIL, NULL,
                                            Notify_Failure, IDP_IME_OK, NULL, WAITTIMEOUT);
                                    }
                                }
                                else
                                {
									CALE_Print("\r\n[calendar]: CALE_DelApi failed!\r\n");
                                    PLXTipsWin(NULL, NULL, NULL, IDP_STRING_SAVEFAIL, NULL,
                                        Notify_Failure, IDP_IME_OK, NULL, WAITTIMEOUT);
                                }
                                CALE_CloseApi(hOpen);
                                
                                if(TmpMeet.MeetAlarmFlag)
                                {
                                    CALE_NewMeet(&TmpMeet, CurRecord, 0);
                                }
                            }
                            SendMessage(pCreateData->hParent, CALE_APP_NEWC, 
                                MAKEWPARAM(CALE_MEETING, CurRecord), (LPARAM)&TmpMeet);
                            PostMessage(hWnd, WM_CLOSE, 0, 0);
                            break;
                        }
                        //save and setalarm
                        CmpResult = CALE_JudgeMeetSave(&TmpMeet, &CurSysTime);
                        if(CmpResult == CALE_RETURN_SUCCESS)
                        {
                            TmpMeet.LocalFlag = CALE_LOCAL;
                            hOpen = CALE_OpenApi(CALE_MEETING);
                            if(hOpen == CALE_FAIL)
                            {
                                //memset(&CurMeetInfo, 0x0, sizeof(CaleMeetingNode));
                                memset(&TmpMeet, 0x0, sizeof(CaleMeetingNode));
                                break;
                            }
                            if(bNewDate)
                            {
                                CurRecord = CALE_AddApi(CALE_MEETING, hOpen, 
                                    (BYTE *)&TmpMeet, sizeof(CaleMeetingNode));

                                if(CurRecord != CALE_FAIL)
                                {
                                    memset(&CurMeetInfo, 0x0, sizeof(CaleMeetingNode));
                                    memcpy(&CurMeetInfo, &TmpMeet, sizeof(CaleMeetingNode));
                                    PLXTipsWin(NULL, NULL, NULL, IDP_STRING_SAVESUCC, NULL,
                                        Notify_Success, IDP_IME_OK, NULL, WAITTIMEOUT);//pCreateData->hFrameWnd
                                }
                                else
                                {
                                    PLXTipsWin(NULL, NULL, NULL, IDP_STRING_SAVEFAIL, NULL,
                                        Notify_Failure, IDP_IME_OK, NULL, WAITTIMEOUT);
                                }
                                CALE_CloseApi(hOpen);
                                
                                if(TmpMeet.MeetAlarmFlag)
                                {
                                    CALE_NewMeet(&TmpMeet, CurRecord, 0);
                                }
                            }
                            else
                            {
                                if(CurMeetInfo.MeetAlarmFlag)
                                {
                                    CALE_KillAlarm(CALE_MEETING, CurRecord);
                                }
                                CurMeetInfo.LocalFlag = CALE_UNLOCAL;
                                if(CALE_DelApi(CALE_MEETING, hOpen, CurRecord, (BYTE *)&CurMeetInfo, 
                                    sizeof(CaleMeetingNode)))
                                {
                                    CurRecord = CALE_AddApi(CALE_MEETING, hOpen, 
                                        (BYTE *)&TmpMeet, sizeof(CaleMeetingNode));

                                    if(CurRecord != CALE_FAIL)
                                    {
                                        memset(&CurMeetInfo, 0x0, sizeof(CaleMeetingNode));
                                        memcpy(&CurMeetInfo, &TmpMeet, sizeof(CaleMeetingNode));
                                        PLXTipsWin(NULL, NULL, NULL, IDP_STRING_SAVESUCC, NULL,
                                            Notify_Success, IDP_IME_OK, NULL, WAITTIMEOUT);
                                    }
                                    else
                                    {
                                        PLXTipsWin(NULL, NULL, NULL, IDP_STRING_SAVEFAIL, NULL,
                                            Notify_Failure, IDP_IME_OK, NULL, WAITTIMEOUT);
                                    }
                                }
                                else
                                {
                                    PLXTipsWin(NULL, NULL, NULL, IDP_STRING_SAVEFAIL, NULL,
                                        Notify_Failure, IDP_IME_OK, NULL, WAITTIMEOUT);
                                }
                                CALE_CloseApi(hOpen);
                                
                                if(TmpMeet.MeetAlarmFlag)
                                {
                                    CALE_NewMeet(&TmpMeet, CurRecord, 0);
                                }
                            }
                            SendMessage(pCreateData->hParent, CALE_APP_NEWC, 
                                MAKEWPARAM(CALE_MEETING, CurRecord), (LPARAM)&TmpMeet);
                            PostMessage(hWnd, WM_CLOSE, 0, 0);
                        }
                        else if(CmpResult == CALE_ERROR_EDIT)
                        {
                            
                            memset(&vsi, 0, sizeof(SCROLLINFO));
                            vsi.cbSize = sizeof(vsi);
                            vsi.fMask  = SIF_ALL;
                            GetScrollInfo(hWnd, SB_VERT, &vsi);

							if(strlen(TmpMeet.MeetSubject) <= 0)
							{
								pCreateData->hFocus = hSubject;
								SetFocus(pCreateData->hFocus);
								if(strlen(TmpMeet.MeetLocation) <= 0)
									PLXTipsWin(NULL, NULL, NULL, IDP_STRING_EDITERROR, NULL,
										Notify_Alert, IDP_IME_OK, NULL, WAITTIMEOUT);
								else
		                            PLXTipsWin(NULL, NULL, NULL, IDP_STRING_SUBJERROR, NULL,
				                        Notify_Alert, IDP_IME_OK, NULL, WAITTIMEOUT);

							}
							else
							{
								pCreateData->hFocus = hLocation;
								SetFocus(pCreateData->hFocus);
								PLXTipsWin(NULL, NULL, NULL, IDP_STRING_LOCAERROR, NULL,
									Notify_Alert, IDP_IME_OK, NULL, WAITTIMEOUT);
							}
							
							memset(&TmpMeet, 0x0, sizeof(CaleMeetingNode));

							//TempPos = vsi.nPos / vsi.nPage;
							ScrollWindow(hWnd, 0, (PARTWND_HEIGHT * (int)vsi.nPos), NULL, NULL);//vsi.nPage * TempPos
							UpdateWindow(hWnd);
							vsi.nPos = 0;
							vsi.fMask  = SIF_POS;
							SetScrollInfo(hWnd, SB_VERT, &vsi, TRUE);

//                            PLXTipsWin(NULL, NULL, NULL, IDP_STRING_EDITERROR, NULL,
//                                Notify_Alert, IDP_IME_OK, NULL, WAITTIMEOUT);
                        }
                        else if(CmpResult == CALE_ERROR_BADDATE)
                        {
							memset(&vsi, 0, sizeof(SCROLLINFO));
							vsi.cbSize = sizeof(vsi);
							vsi.fMask  = SIF_ALL;
							GetScrollInfo(hWnd, SB_VERT, &vsi);
							
							pCreateData->hFocus = hStartDate;
							SetFocus(pCreateData->hFocus);
							//TempPos = vsi.nPos / vsi.nPage - 1;
							ScrollWindow(hWnd, 0, (PARTWND_HEIGHT * (int)(vsi.nPos - 1)), NULL, NULL);
							UpdateWindow(hWnd);
							vsi.nPos = 1;
							vsi.fMask  = SIF_POS;
							SetScrollInfo(hWnd, SB_VERT, &vsi, TRUE);
							
							PLXTipsWin(NULL, NULL, NULL, IDP_STRING_ENDDTERROR, NULL, 
								Notify_Alert, IDP_IME_OK, NULL, WAITTIMEOUT);
							
							memset(&TmpMeet, 0x0, sizeof(CaleMeetingNode));
						}
						else if(CmpResult == CALE_ERROR_STARTDATE)
						{
							memset(&TmpMeet, 0x0, sizeof(CaleMeetingNode));
							PLXConfirmWinEx(pCreateData->hFrameWnd, hWnd, IDP_STRING_STARTERROR, 
								Notify_Request, NULL, IDP_IME_YES, IDP_IME_NO, CALE_CONFIRM_MESSAGE);
                        }
                        else if(CmpResult == CALE_ERROR_STARTTIME)
                        {
                            memset(&TmpMeet, 0x0, sizeof(CaleMeetingNode));
                            PLXConfirmWinEx(pCreateData->hFrameWnd, hWnd, IDP_STRING_STARTERROR,
                                Notify_Request, NULL, IDP_IME_YES, IDP_IME_NO, CALE_CONFIRM_MESSAGE);
                        }
                        else if(CmpResult == CALE_ERROR_ENDDATE)
                        {
                         //   char szDis[20];

                            memset(&vsi, 0, sizeof(SCROLLINFO));
                            vsi.cbSize = sizeof(vsi);
                            vsi.fMask  = SIF_ALL;
                            GetScrollInfo(hWnd, SB_VERT, &vsi);

                            pCreateData->hFocus = hEndDate;
                            SetFocus(pCreateData->hFocus);
                            //TempPos = vsi.nPos / vsi.nPage - 1;
                            ScrollWindow(hWnd, 0, (PARTWND_HEIGHT * (int)(vsi.nPos - 3)), NULL, NULL);
                            UpdateWindow(hWnd);
                            vsi.nPos = 3;
                            vsi.fMask  = SIF_POS;
                            SetScrollInfo(hWnd, SB_VERT, &vsi, TRUE);

                            PLXTipsWin(NULL, NULL, NULL, IDP_STRING_ENDDTERROR, NULL, 
                                Notify_Alert, IDP_IME_OK, NULL, WAITTIMEOUT);

                            memset(&TmpMeet, 0x0, sizeof(CaleMeetingNode));
                        }
                        else if(CmpResult == CALE_ERROR_ENDTIME)
                        {
                            char szDis[20];

                            memset(&vsi, 0, sizeof(SCROLLINFO));
                            vsi.cbSize = sizeof(vsi);
                            vsi.fMask  = SIF_ALL;
                            GetScrollInfo(hWnd, SB_VERT, &vsi);

                            pCreateData->hFocus = hEndTime;
                            SetFocus(pCreateData->hFocus);
                            //TempPos = vsi.nPos / vsi.nPage - 1;
                            ScrollWindow(hWnd, 0, (PARTWND_HEIGHT * (int)(vsi.nPos - 4)), NULL, NULL);
                            UpdateWindow(hWnd);
                            vsi.nPos = 4;
                            vsi.fMask  = SIF_POS;
                            SetScrollInfo(hWnd, SB_VERT, &vsi, TRUE);

                            memset(szDis, 0x0, 20);
						//	sprintf(szDis, "%s %04d-%02d-%02d", IDP_STRING_ENDERROR, 
						//		TmpMeet.MeetEndYear, TmpMeet.MeetEndMonth, TmpMeet.MeetEndDay);
							
						//	sprintf(szDis, "%s%c", IDP_STRING_ENDERROR, ' ');
						//	CALE_GetFormatedTime(szDis+strlen(szDis), TmpMeet.MeetEndHour, TmpMeet.MeetEndMinute);
                            PLXTipsWin(NULL, NULL, NULL, IDP_STRING_ENDTMERROR, NULL, 
                                Notify_Alert, IDP_IME_OK, NULL, WAITTIMEOUT);

                            memset(&TmpMeet, 0x0, sizeof(CaleMeetingNode));
                        }
                    }
                    else if(CurCaleType == CALE_ANNI)
                    {
						CALE_Print("\r\n[calendar]: saving anniversary item.\r\n");
                        memset(&TmpAnni, 0x0, sizeof(CaleAnniNode));
                        GetWindowText(hSubject, TmpAnni.AnniOccasion, MAX_CALE_OCCASION_LEN + 1);
                        
                        SendMessage(hStartDate, TEM_GETTIME, 0, (LPARAM)&TmpSysTime);
                        TmpAnni.AnniDisYear = TmpSysTime.wYear;
                        TmpAnni.AnniDisMonth = TmpSysTime.wMonth;
                        TmpAnni.AnniDisDay = TmpSysTime.wDay;
                        
                        TmpAnni.AnniAlarmFlag = SendMessage(hAlarm, SSBM_GETCURSEL, NULL, NULL);
                        if(TmpAnni.AnniAlarmFlag == 1)
                        {
                            SYSTEMTIME STmpSave;
                            char cTmpSave[17];

                            memset(cTmpSave, 0x0, sizeof(char) * 17);
                            SendMessage(hAlarm, SSBM_GETTEXT, 1, (LPARAM)cTmpSave);
                            CALE_DateTimeCharToSys(cTmpSave, &STmpSave);
                            TmpAnni.AnniYear = STmpSave.wYear;
                            TmpAnni.AnniMonth = STmpSave.wMonth;
                            TmpAnni.AnniDay = STmpSave.wDay;
                            TmpAnni.AnniHour = STmpSave.wHour;
                            TmpAnni.AnniMinute = STmpSave.wMinute;
                        }
#ifdef _NOKIA_CALE_
                        TmpAnni.AnniSync = SendMessage(hSync, SSBM_GETCURSEL, NULL, NULL);
#endif

                        //save and setalarm
                        CmpResult = CALE_JudgeAnniSave(&TmpAnni, &CurSysTime);
                        if(CmpResult == CALE_RETURN_SUCCESS)
                        {
							CALE_Print("\r\n[calendar]: anniversary item is available to be saved.\r\n");
                            TmpAnni.LocalFlag = CALE_LOCAL;
                            hOpen = CALE_OpenApi(CALE_ANNI);
                            if(hOpen == CALE_FAIL)
                            {
								CALE_Print("\r\n[calendar]: CALE_OpenApi failed.\r\n");
                                memset(&TmpAnni, 0x0, sizeof(CaleAnniNode));
                                break;
                            }
                            if(bNewDate)
                            {
								CALE_Print("\r\n[calendar]: adding new anniversary item.\r\n");
                                CurRecord = CALE_AddApi(CALE_ANNI, hOpen, 
                                    (BYTE *)&TmpAnni, sizeof(CaleAnniNode));

                                if(CurRecord != CALE_FAIL)
                                {
									CALE_Print("\r\n[calendar]: anniversary item added successfully.\r\n");
                                    memset(&CurAnniInfo, 0x0, sizeof(CaleAnniNode));
                                    memcpy(&CurAnniInfo, &TmpAnni, sizeof(CaleAnniNode));
                                    PLXTipsWin(NULL, NULL, NULL, IDP_STRING_SAVESUCC, NULL,
                                        Notify_Success, IDP_IME_OK, NULL, WAITTIMEOUT);
                                }
                                else
                                {
									CALE_Print("\r\n[calendar]: failed to add anniversary item.\r\n");
                                    PLXTipsWin(NULL, NULL, NULL, IDP_STRING_SAVEFAIL, NULL,
                                        Notify_Failure, IDP_IME_OK, NULL, WAITTIMEOUT);
                                }
                                CALE_CloseApi(hOpen);
                                
                                if(TmpAnni.AnniAlarmFlag)
                                {
                                    CALE_NewAnni(&TmpAnni, CurRecord, 0);
                                }
                            }
                            else
                            {
                                if(CurAnniInfo.AnniAlarmFlag)
                                {
                                    CALE_KillAlarm(CALE_ANNI, CurRecord);
                                }
                                CurAnniInfo.LocalFlag = CALE_UNLOCAL;
                                if(CALE_DelApi(CALE_ANNI, hOpen, CurRecord, (BYTE *)&CurAnniInfo, 
                                    sizeof(CaleAnniNode)))
                                {
                                    CurRecord = CALE_AddApi(CALE_ANNI, hOpen, 
                                        (BYTE *)&TmpAnni, sizeof(CaleAnniNode));

                                    if(CurRecord != CALE_FAIL)
                                    {
                                        memset(&CurAnniInfo, 0x0, sizeof(CaleAnniNode));
                                        memcpy(&CurAnniInfo, &TmpAnni, sizeof(CaleAnniNode));
                                        PLXTipsWin(NULL, NULL, NULL, IDP_STRING_SAVESUCC, NULL,
                                            Notify_Success, IDP_IME_OK, NULL, WAITTIMEOUT);
                                    }
                                    else
                                    {
                                        PLXTipsWin(NULL, NULL, NULL, IDP_STRING_SAVEFAIL, NULL,
                                            Notify_Failure, IDP_IME_OK, NULL, WAITTIMEOUT);
                                    }
                                }
                                else
                                {
                                    PLXTipsWin(NULL, NULL, NULL, IDP_STRING_SAVEFAIL, NULL,
                                        Notify_Failure, IDP_IME_OK, NULL, WAITTIMEOUT);
                                }
                                CALE_CloseApi(hOpen);
                                
                                if(TmpAnni.AnniAlarmFlag)
                                {
                                    CALE_NewAnni(&TmpAnni, CurRecord, 0);
                                }
                            }
                            SendMessage(pCreateData->hParent, CALE_APP_NEWC, 
                                MAKEWPARAM(CALE_ANNI, CurRecord), (LPARAM)&TmpAnni);
                            PostMessage(hWnd, WM_CLOSE, 0, 0);
                        }
                        else if(CmpResult == CALE_ERROR_EDIT)
                        {
                            memset(&TmpAnni, 0x0, sizeof(CaleAnniNode));
                            
                            memset(&vsi, 0, sizeof(SCROLLINFO));
                            vsi.cbSize = sizeof(vsi);
                            vsi.fMask  = SIF_ALL;
                            GetScrollInfo(hWnd, SB_VERT, &vsi);

                            pCreateData->hFocus = hSubject;
                            SetFocus(pCreateData->hFocus);
                            //TempPos = vsi.nPos / vsi.nPage;
						//	ScrollWindow(hWnd, 0, (PARTWND_HEIGHT * (int)vsi.nPos), NULL, NULL);
                            UpdateWindow(hWnd);
                            vsi.nPos = 0;
                            vsi.fMask  = SIF_POS;
                            SetScrollInfo(hWnd, SB_VERT, &vsi, TRUE);

                            PLXTipsWin(NULL, NULL, NULL, IDP_STRING_SUBJERROR, NULL,
                                Notify_Alert, IDP_IME_OK, NULL, WAITTIMEOUT);
                        }
                        else if(CmpResult == CALE_ERROR_STARTDATE)
                        {
                         //   char szDis[20];

                            memset(&vsi, 0, sizeof(SCROLLINFO));
                            vsi.cbSize = sizeof(vsi);
                            vsi.fMask  = SIF_ALL;
                            GetScrollInfo(hWnd, SB_VERT, &vsi);

                            pCreateData->hFocus = hStartDate;
                            SetFocus(pCreateData->hFocus);
                            //TempPos = vsi.nPos / vsi.nPage;
						//	ScrollWindow(hWnd, 0, (PARTWND_HEIGHT * (int)vsi.nPos), NULL, NULL);
                            UpdateWindow(hWnd);
                            vsi.nPos = 0;
                            vsi.fMask  = SIF_POS;
                            SetScrollInfo(hWnd, SB_VERT, &vsi, TRUE);

                            PLXTipsWin(NULL, NULL, NULL, IDP_STRING_ENDDTERROR, NULL, 
                                Notify_Alert, IDP_IME_OK, NULL, WAITTIMEOUT);

                            memset(&TmpAnni, 0x0, sizeof(CaleAnniNode));
                        }
/*						else if(CmpResult == CALE_ERROR_ALARM)
                        {
                            char szDis[25];

                            memset(&vsi, 0, sizeof(SCROLLINFO));
                            vsi.cbSize = sizeof(vsi);
                            vsi.fMask  = SIF_ALL;
                            GetScrollInfo(hWnd, SB_VERT, &vsi);

                            pCreateData->hFocus = hAlarm;
                            SetFocus(pCreateData->hFocus);
                            //TempPos = vsi.nPos / vsi.nPage;
                            ScrollWindow(hWnd, 0, (PARTWND_HEIGHT * (int)vsi.nPos), NULL, NULL);
                            UpdateWindow(hWnd);
                            vsi.nPos = 0;
                            vsi.fMask  = SIF_POS;
                            SetScrollInfo(hWnd, SB_VERT, &vsi, TRUE);

                            memset(szDis, 0x0, 20);
//                            sprintf(szDis, "%s %04d-%02d-%02d", IDP_STRING_ENDERROR, 
//                                TmpAnni.AnniYear, TmpAnni.AnniMonth, TmpAnni.AnniDay);
                            sprintf(szDis, "%s %04d-%02d-%02d &02d:%02d", IDP_STRING_ENDERROR, 
                                TmpAnni.AnniYear, TmpAnni.AnniMonth, TmpAnni.AnniDay, 
								TmpAnni.AnniHour, TmpAnni.AnniMinute);
                            PLXTipsWin(NULL, NULL, NULL, szDis, NULL, 
                                Notify_Alert, IDP_IME_OK, NULL, WAITTIMEOUT);

                            memset(&TmpAnni, 0x0, sizeof(CaleAnniNode));
                        }*/
                    }
                    else if(CurCaleType == CALE_EVENT)
                    {
                        memset(&TmpEvent, 0x0, sizeof(CaleEventNode));
                        GetWindowText(hSubject, TmpEvent.EventSubject, MAX_CALE_SUBJECT_LEN + 1);
                        
                        TmpEvent.EventStartHour = 8;
                        TmpEvent.EventStartMinute = 0;
                        SendMessage(hStartDate, TEM_GETTIME, 0, (LPARAM)&TmpSysTime);
                        TmpEvent.EventStartYear = TmpSysTime.wYear;
                        TmpEvent.EventStartMonth = TmpSysTime.wMonth;
                        TmpEvent.EventStartDay = TmpSysTime.wDay;
                        TmpEvent.EventEndHour = 8;
                        TmpEvent.EventEndMinute = 0;
                        SendMessage(hEndDate, TEM_GETTIME, 0, (LPARAM)&TmpSysTime);
                        TmpEvent.EventEndYear = TmpSysTime.wYear;
                        TmpEvent.EventEndMonth = TmpSysTime.wMonth;
                        TmpEvent.EventEndDay = TmpSysTime.wDay;

#ifdef _NOKIA_CALE_
                        TmpEvent.EventSync = SendMessage(hSync, SSBM_GETCURSEL, NULL, NULL);
#endif
                        
                        //save and setalarm
                        CmpResult = CALE_JudgeEventSave(&TmpEvent, &CurSysTime);
                        if(CmpResult == CALE_RETURN_SUCCESS)
                        {
                            TmpEvent.LocalFlag = CALE_LOCAL;
                            hOpen = CALE_OpenApi(CALE_EVENT);
                            if(hOpen == CALE_FAIL)
                            {
                                //memset(&CurEventInfo, 0x0, sizeof(CaleEventNode));
                                memset(&TmpEvent, 0x0, sizeof(CaleEventNode));
                                break;
                            }
                            if(bNewDate)
                            {
                                CurRecord = CALE_AddApi(CALE_EVENT, hOpen, 
                                    (BYTE *)&TmpEvent, sizeof(CaleEventNode));

                                if(CurRecord != CALE_FAIL)
                                {
                                    memset(&CurEventInfo, 0x0, sizeof(CaleEventNode));
                                    memcpy(&CurEventInfo, &TmpEvent, sizeof(CaleEventNode));
                                    PLXTipsWin(NULL, NULL, NULL, IDP_STRING_SAVESUCC, NULL,
                                        Notify_Success, IDP_IME_OK, NULL, WAITTIMEOUT);
                                }
                                else
                                {
                                    PLXTipsWin(NULL, NULL, NULL, IDP_STRING_SAVEFAIL, NULL,
                                        Notify_Failure, IDP_IME_OK, NULL, WAITTIMEOUT);
                                }
                                CALE_CloseApi(hOpen);
                            }
                            else
                            {
                                CurEventInfo.LocalFlag = CALE_UNLOCAL;
                                if(CALE_DelApi(CALE_EVENT, hOpen, CurRecord, (BYTE *)&CurEventInfo, 
                                    sizeof(CaleEventNode)))
                                {
                                    CurRecord = CALE_AddApi(CALE_EVENT, hOpen, 
                                        (BYTE *)&TmpEvent, sizeof(CaleEventNode));

                                    if(CurRecord != CALE_FAIL)
                                    {
                                        memset(&CurEventInfo, 0x0, sizeof(CaleEventNode));
                                        memcpy(&CurEventInfo, &TmpEvent, sizeof(CaleEventNode));
                                        PLXTipsWin(NULL, NULL, NULL, IDP_STRING_SAVESUCC, NULL,
                                            Notify_Success, IDP_IME_OK, NULL, WAITTIMEOUT);
                                    }
                                    else
                                    {
                                        PLXTipsWin(NULL, NULL, NULL, IDP_STRING_SAVEFAIL, NULL,
                                            Notify_Failure, IDP_IME_OK, NULL, WAITTIMEOUT);
                                    }
                                }
                                else
                                {
                                    PLXTipsWin(NULL, NULL, NULL, IDP_STRING_SAVEFAIL, NULL, 
                                        Notify_Failure, IDP_IME_OK, NULL, WAITTIMEOUT);
                                }
                                CALE_CloseApi(hOpen);
                            }
                            SendMessage(pCreateData->hParent, CALE_APP_NEWC, 
                                MAKEWPARAM(CALE_EVENT, CurRecord), (LPARAM)&TmpEvent);
                            PostMessage(hWnd, WM_CLOSE, 0, 0);
                        }
                        else if(CmpResult == CALE_ERROR_EDIT)
                        {
                            memset(&TmpEvent, 0x0, sizeof(CaleEventNode));
                            
                            memset(&vsi, 0, sizeof(SCROLLINFO));
                            vsi.cbSize = sizeof(vsi);
                            vsi.fMask  = SIF_ALL;
                            GetScrollInfo(hWnd, SB_VERT, &vsi);

                            pCreateData->hFocus = hSubject;
                            SetFocus(pCreateData->hFocus);
                            //TempPos = vsi.nPos / vsi.nPage;
                      //	ScrollWindow(hWnd, 0, (PARTWND_HEIGHT * (int)vsi.nPos), NULL, NULL);
                            UpdateWindow(hWnd);
                            vsi.nPos = 0;
                            vsi.fMask  = SIF_POS;
                            SetScrollInfo(hWnd, SB_VERT, &vsi, TRUE);

                            PLXTipsWin(NULL, NULL, NULL, IDP_STRING_SUBJERROR, NULL,
                                Notify_Alert, IDP_IME_OK, NULL, WAITTIMEOUT);
                        }
                        else if(CmpResult == CALE_ERROR_STARTDATE)
                        {
                         //   char szDis[20];

                            memset(&vsi, 0, sizeof(SCROLLINFO));
                            vsi.cbSize = sizeof(vsi);
                            vsi.fMask  = SIF_ALL;
                            GetScrollInfo(hWnd, SB_VERT, &vsi);

                            pCreateData->hFocus = hStartDate;
                            SetFocus(pCreateData->hFocus);
                            //TempPos = vsi.nPos / vsi.nPage;
                       //	ScrollWindow(hWnd, 0, (PARTWND_HEIGHT * (int)vsi.nPos), NULL, NULL);
                            UpdateWindow(hWnd);
                            vsi.nPos = 0;
                            vsi.fMask  = SIF_POS;
                            SetScrollInfo(hWnd, SB_VERT, &vsi, TRUE);

                            PLXTipsWin(NULL, NULL, NULL, IDP_STRING_ENDDTERROR, NULL, 
                                Notify_Alert, IDP_IME_OK, NULL, WAITTIMEOUT);

                            memset(&TmpEvent, 0x0, sizeof(CaleEventNode));
                        }
                        else if(CmpResult == CALE_ERROR_ENDDATE)
                        {
                         //   char szDis[20];
                            
                            memset(&vsi, 0, sizeof(SCROLLINFO));
                            vsi.cbSize = sizeof(vsi);
                            vsi.fMask  = SIF_ALL;
                            GetScrollInfo(hWnd, SB_VERT, &vsi);

                            pCreateData->hFocus = hEndDate;
                            SetFocus(pCreateData->hFocus);
                            //TempPos = vsi.nPos / vsi.nPage;
                        //	ScrollWindow(hWnd, 0, (PARTWND_HEIGHT * (int)(vsi.nPos - 1)), NULL, NULL);
                            UpdateWindow(hWnd);
                            vsi.nPos = 1;
                            vsi.fMask  = SIF_POS;
                            SetScrollInfo(hWnd, SB_VERT, &vsi, TRUE);

                            PLXTipsWin(NULL, NULL, NULL, IDP_STRING_ENDDTERROR, NULL, 
                                Notify_Alert, IDP_IME_OK, NULL, WAITTIMEOUT);

                            memset(&TmpEvent, 0x0, sizeof(CaleEventNode));
                        }
                    }
                }
            	break;

            case IDM_BUTTON_QUIT:
                {
                    PostMessage(hWnd, WM_CLOSE, 0, 0);
                }
                break;

            default:
                lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
                break;
            }
        }
        break;

    case WM_KEYDOWN:
        {
            switch(LOWORD(wParam))
            {
            case VK_RETURN:
                {
                    PostMessage(hWnd, WM_COMMAND, MAKEWPARAM(IDM_BUTTON_COMPLETE, 0), 0);
                }
                break;

            case VK_F5:
                {
                    if((pCreateData->hFocus == hAlarm) && (CurCaleType == CALE_MEETING))
                    {
                        CurAlarmRep = SendMessage(hAlarm, SSBM_GETCURSEL, NULL, NULL);
                        if(CurAlarmRep < 0)
							CurAlarmRep = 0;
                        CreateMeetAlarmWnd(hWnd);
                    }
                    else if((pCreateData->hFocus == hAlarm) && (CurCaleType == CALE_ANNI))
                    {
                        CreateAnniAlarmWnd(hWnd);
                    }
                    else if((pCreateData->hFocus == hRepeat) && (CurCaleType == CALE_MEETING))
                    {
                        CurAlarmRep = SendMessage(hRepeat, SSBM_GETCURSEL, NULL, NULL);
                        if(CurAlarmRep < 0)
							CurAlarmRep = 0;
                        CreateMeetRepWnd(hWnd);
                    }
                    else
                    {
                        break;
                        //lResult = PDADefWindowProc(pCreateData->hFrameWnd, wMsgCmd, wParam, lParam);
                    }
                }
            	break;

            case VK_F10:
                {
                    PostMessage(hWnd, WM_COMMAND, MAKEWPARAM(IDM_BUTTON_QUIT, 0), 0);
                }
            	break;

            case VK_UP:
				{
					memset(&vsi, 0, sizeof(SCROLLINFO));
					vsi.cbSize = sizeof(vsi);
					vsi.fMask  = SIF_ALL;
					GetScrollInfo(hWnd, SB_VERT, &vsi);
					
					TmpFocus = GetNextDlgTabItem(hWnd, pCreateData->hFocus, TRUE);
					if(NULL == TmpFocus)
					{
#ifdef _CALENDAR_LOCAL_TEST_
						ASSERT(0);
#endif
						break;
					}

					if(pCreateData->hFocus == hSubject)//vsi.nPos == vsi.nMin)
					{
						if(CALE_MEETING == CurCaleType)
							ScrollWindow(hWnd, 0, -5*PARTWND_HEIGHT, NULL, NULL);
						vsi.nPos = 5;
					}
#ifdef _BENEFON_CALE_
                    else if((pCreateData->hFocus == hAlarm) && 
                        ((CurCaleType == CALE_MEETING) || (CurCaleType == CALE_ANNI)))
                    {
                        ScrollWindow(hWnd, 0, 0, NULL, NULL);
                    }
                    else if((pCreateData->hFocus == hEndDate) && (CurCaleType == CALE_EVENT))
                    {
                        ScrollWindow(hWnd, 0, 0, NULL, NULL);
                    }
#endif
#ifdef _NOKIA_CALE_
                    if(pCreateData->hFocus == hSync)
                    {
                        ScrollWindow(hWnd, 0, 0, NULL, NULL);
                    }
#endif
                    else if(GetNextDlgTabItem(hWnd, pCreateData->hFocus, TRUE) == hSubject)
                    {
                        ScrollWindow(hWnd, 0, 0, NULL, NULL);
                    }
                    else
                    {
						if(CALE_MEETING == CurCaleType)
	                        ScrollWindow(hWnd, 0, PARTWND_HEIGHT, NULL, NULL);
                        vsi.nPos --;
                    }
                    
					pCreateData->hFocus = TmpFocus;
                    SetFocus(pCreateData->hFocus);

                    if((pCreateData->hFocus == hRepeat) || (pCreateData->hFocus == hAlarm))
                    {
                        SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_SELECT);
                    }
                    else
                    {
                        SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
                    }

                    UpdateWindow(hWnd);
                    vsi.fMask  = SIF_POS;
                    SetScrollInfo(hWnd, SB_VERT, &vsi, TRUE);

					if(!bKeyDown)
					{
						bKeyDown = TRUE;
						SetTimer(hWnd, TIMERID_VK_UP, TIMER_KEY_LONG, NULL);
					}
                }
                break;

            case VK_DOWN:
                {
                    memset(&vsi, 0, sizeof(SCROLLINFO));
                    vsi.cbSize = sizeof(vsi);
                    vsi.fMask  = SIF_ALL;
                    GetScrollInfo(hWnd, SB_VERT, &vsi);
					
					TmpFocus = GetNextDlgTabItem(hWnd, pCreateData->hFocus, FALSE);
					if(NULL == TmpFocus)
					{
#ifdef _CALENDAR_LOCAL_TEST_
						ASSERT(0);
#endif
						break;
					}

#ifdef _NOKIA_CALE_
                    if(pCreateData->hFocus == hSync)
                    {
                        break;
                    }
#endif
#ifdef _BENEFON_CALE_
					if(TmpFocus == hSubject)
					{
						if(CALE_MEETING == CurCaleType)
							ScrollWindow(hWnd, 0, 5*PARTWND_HEIGHT, NULL, NULL);
						vsi.nPos = 0;
					}
                    else if(pCreateData->hFocus == hSubject)
                    {
                        ScrollWindow(hWnd, 0, 0, NULL, NULL);
                    }
                    else if((GetNextDlgTabItem(hWnd, pCreateData->hFocus, FALSE) == hAlarm) && 
                        ((CurCaleType == CALE_MEETING) || (CurCaleType == CALE_ANNI)))//vsi.nPos == vsi.nMax)
                    {
                        ScrollWindow(hWnd, 0, 0, NULL, NULL);
                    }
                    else if((GetNextDlgTabItem(hWnd, pCreateData->hFocus, FALSE) == hEndDate) && 
                        (CurCaleType == CALE_EVENT))
                    {
                        ScrollWindow(hWnd, 0, 0, NULL, NULL);
                    }
#endif
#ifdef _NOKIA_CALE_
                    else if(GetNextDlgTabItem(hWnd, pCreateData->hFocus, FALSE) == hSync)
                    {
                        ScrollWindow(hWnd, 0, 0, NULL, NULL);
                    }
#endif
                    else
                    {
						if(CALE_MEETING == CurCaleType)
	                        ScrollWindow(hWnd, 0, -PARTWND_HEIGHT, NULL, NULL);
                        vsi.nPos ++;
                    }

					pCreateData->hFocus = TmpFocus;
                    SetFocus(pCreateData->hFocus);

                    if((pCreateData->hFocus == hRepeat) || (pCreateData->hFocus == hAlarm))
                    {
                        SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_SELECT);
                    }
                    else
                    {
                        SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
                    }

                    UpdateWindow(hWnd);
                    vsi.fMask  = SIF_POS;
                    SetScrollInfo(hWnd, SB_VERT, &vsi, TRUE);

					if(!bKeyDown)
					{
						bKeyDown = TRUE;
						SetTimer(hWnd, TIMERID_VK_DOWN, TIMER_KEY_LONG, NULL);
					}
                }
                break;

            default:
                lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
                break;
            }
        }
        break;

	case PWM_ACTIVATE:
		if(!wParam)
		{
			bKeyDown = FALSE;
			KillTimer(hWnd, TIMERID_VK_UP);
			KillTimer(hWnd, TIMERID_VK_DOWN);
		}
		lResult = PDADefWindowProc(hWnd, PWM_ACTIVATE, wParam, lParam);
		break;
		
	case WM_KEYUP:
		bKeyDown = FALSE;
		KillTimer(hWnd, TIMERID_VK_UP);
		KillTimer(hWnd, TIMERID_VK_DOWN);
		break;

	case WM_TIMER:
		if(TIMERID_VK_DOWN == wParam)
		{
			KillTimer(hWnd, TIMERID_VK_DOWN);
			PostMessage(hWnd, WM_KEYDOWN, VK_DOWN, 0);
			SetTimer(hWnd, TIMERID_VK_DOWN, TIMER_KEY_REPEAT, NULL);
		}
		else if(TIMERID_VK_UP == wParam)
		{
			KillTimer(hWnd, TIMERID_VK_UP);
			PostMessage(hWnd, WM_KEYDOWN, VK_UP, 0);
			SetTimer(hWnd, TIMERID_VK_UP, TIMER_KEY_REPEAT, NULL);
		}
		break;

    case WM_CLOSE:
        {
			bKeyDown = FALSE;
			KillTimer(hWnd, TIMERID_VK_UP);
			KillTimer(hWnd, TIMERID_VK_DOWN);
            PostMessage(pCreateData->hFrameWnd, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
            DestroyWindow(hWnd);
        }
        break;
        
    case WM_DESTROY:
        {
            HwndCaleMeet = NULL;
            HwndCaleAnni = NULL;
            HwndCaleEvent = NULL;
            if(hSelect)
            {
                DeleteObject(hSelect);
                hSelect = NULL;
            }
            if(hUnSelect)
            {
                DeleteObject(hUnSelect);
                hUnSelect = NULL;
            }
            UnRegisterCaleDiaplayClass();
        }
        break;
        
    case CALE_ALARM_CHANGED:
        {
            if((pCreateData->hFocus == hAlarm) && (CurCaleType == CALE_MEETING))
            {
                SendMessage(hAlarm, SSBM_SETCURSEL, CurAlarmRep, NULL);
            }
            else if((pCreateData->hFocus == hAlarm) && (CurCaleType == CALE_ANNI))
            {
                char TmpDis[17];
				SYSTEMTIME *tm = (SYSTEMTIME *)lParam;

                memset(TmpDis, 0x0, sizeof(char) * 17);
			//	CALE_DateTimeSysToChar((SYSTEMTIME *)lParam, TmpDis);
			//	CALE_GetFormatedDate((SYSTEMTIME*)lParam, TmpDis);

				sprintf(TmpDis, "%02d.%02d.%4d %02d:%02d", tm->wDay, tm->wMonth,
					tm->wYear, tm->wHour, tm->wMinute);

                SendMessage(hAlarm, SSBM_SETTEXT, 1, (LPARAM)TmpDis);
                SendMessage(hAlarm, SSBM_SETCURSEL, 1, NULL);
            }
            else if((pCreateData->hFocus == hRepeat) && (CurCaleType == CALE_MEETING))
            {
                SendMessage(hRepeat, SSBM_SETCURSEL, CurAlarmRep, NULL);
            }
        }
        break;

    case CALE_CONFIRM_MESSAGE:
        {
            switch(lParam)
            {
            case 0:
                {
                    if(CurCaleType == CALE_MEETING)
                    {
                        memset(&vsi, 0, sizeof(SCROLLINFO));
                        vsi.cbSize = sizeof(vsi);
                        vsi.fMask  = SIF_ALL;
                        GetScrollInfo(hWnd, SB_VERT, &vsi);
                        
                        pCreateData->hFocus = hStartDate;
                        SetFocus(pCreateData->hFocus);

                        SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"");

                        //TempPos = vsi.nPos / vsi.nPage;
                        ScrollWindow(hWnd, 0, (PARTWND_HEIGHT * (int)(vsi.nPos - 1)), NULL, NULL);
                        UpdateWindow(hWnd);
                        vsi.nPos = 1;
                        vsi.fMask  = SIF_POS;
                        SetScrollInfo(hWnd, SB_VERT, &vsi, TRUE);
                    }
                }
            	break;

            case 1:
                {
                    ConfirmRet = 1;
                    SendMessage(hWnd, WM_COMMAND, MAKEWPARAM(IDM_BUTTON_COMPLETE, 0), 0);
                }
            	break;

            default:
                break;
            }
        }
        break;

    default:
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
    return lResult;
}

static int CALE_JudgeMeetSave(CaleMeetingNode *pTmpMeet, SYSTEMTIME *pSysTime)
{
    int iCmpRes = 0;
    SYSTEMTIME tempStartSys;
    SYSTEMTIME tempEndSys;
    SYSTEMTIME LocalSys;

/*
    iCmpRes = 0;
    iCmpRes = strlen(pTmpMeet->MeetSubject);
    if(iCmpRes ==  0)
    {
        iCmpRes = strlen(pTmpMeet->MeetLocation);
        if(iCmpRes == 0)
        {
            return CALE_ERROR_EDIT;
        }
    }
*/
	if(strlen(pTmpMeet->MeetSubject) <= 0)
		return CALE_ERROR_EDIT;
	if(strlen(pTmpMeet->MeetLocation) <= 0)
		return CALE_ERROR_EDIT;

    GetLocalTime(&LocalSys);
    tempStartSys.wYear = pTmpMeet->MeetStartYear;
    tempStartSys.wMonth = pTmpMeet->MeetStartMonth;
    tempStartSys.wDay = pTmpMeet->MeetStartDay;
    tempStartSys.wHour = pTmpMeet->MeetStartHour;
    tempStartSys.wMinute = pTmpMeet->MeetStartMinute;

    tempEndSys.wYear = pTmpMeet->MeetEndYear;
    tempEndSys.wMonth = pTmpMeet->MeetEndMonth;
    tempEndSys.wDay = pTmpMeet->MeetEndDay;
    tempEndSys.wHour = pTmpMeet->MeetEndHour;
    tempEndSys.wMinute = pTmpMeet->MeetEndMinute;
    
	if(!CALE_CheckDate(&tempStartSys))
		return CALE_ERROR_BADDATE;

	if(!CALE_CheckDate(&tempEndSys))
		return CALE_ERROR_ENDDATE;
	
    iCmpRes = CALE_CmpOnlyDate(&tempEndSys, &tempStartSys);
    if(iCmpRes == Early)
    {
        return CALE_ERROR_ENDDATE;
    }
    
    iCmpRes = CALE_CmpSysDate(&tempEndSys, &tempStartSys);
    if(iCmpRes == Early)
    {
        return CALE_ERROR_ENDTIME;
    }

    iCmpRes = CALE_CmpOnlyDate(&tempStartSys, &LocalSys);
    if(iCmpRes == Early)
    {
        return CALE_ERROR_STARTDATE;
    }
	else if(Late == iCmpRes)
		return CALE_RETURN_SUCCESS;

    iCmpRes = CALE_CmpOnlyTime(&tempStartSys, &LocalSys);
    if(iCmpRes != Late)
    {
        return CALE_ERROR_STARTTIME;
    }

    return CALE_RETURN_SUCCESS;
}

static int CALE_JudgeAnniSave(CaleAnniNode *pTmpAnni, SYSTEMTIME *pSysTime)
{
    int iCmpRes;
    SYSTEMTIME tempStartSys;
    SYSTEMTIME LocalSys;

    GetLocalTime(&LocalSys);
    
    iCmpRes = 0;
    iCmpRes = strlen(pTmpAnni->AnniOccasion);
    if(iCmpRes ==  0)
    {
        return CALE_ERROR_EDIT;
    }
    
    tempStartSys.wYear = pTmpAnni->AnniDisYear;
    tempStartSys.wMonth = pTmpAnni->AnniDisMonth;
    tempStartSys.wDay = pTmpAnni->AnniDisDay;
    tempStartSys.wHour = 8;
    tempStartSys.wMinute = 0;
    
	if(!CALE_CheckDate(&tempStartSys))
		return CALE_ERROR_STARTDATE;

//    iCmpRes = CALE_CmpOnlyDate(&tempStartSys, pSysTime);
//    if(iCmpRes == Early)
//    {
//        return CALE_ERROR_STARTDATE;
//    }

    iCmpRes = CALE_CmpOnlyDate(&tempStartSys, &LocalSys);
    if(iCmpRes == Early)
    {
        return CALE_ERROR_STARTDATE;
    }
/*
	if(pTmpAnni->AnniAlarmFlag)
	{
		tempStartSys.wYear = pTmpAnni->AnniYear;
		tempStartSys.wMonth = pTmpAnni->AnniMonth;
		tempStartSys.wDay = pTmpAnni->AnniDay;
		tempStartSys.wHour = pTmpAnni->AnniHour;
		tempStartSys.wMinute = pTmpAnni->AnniMinute;
		iCmpRes = CALE_CmpOnlyDate(&tempStartSys, &LocalSys);
		if(iCmpRes == Early)
		{
			return CALE_ERROR_ALARM;
		}
		else if(Late == iCmpRes)
			return CALE_RETURN_SUCCESS;
		
		iCmpRes = CALE_CmpOnlyTime(&tempStartSys, &LocalSys);
		if(iCmpRes == Early)
		{
			return CALE_ERROR_ALARM;
		}
	}*/
    return CALE_RETURN_SUCCESS;
}

static int CALE_JudgeEventSave(CaleEventNode *pTmpEvent, SYSTEMTIME *pSysTime)
{
    int iCmpRes;
    SYSTEMTIME tempStartSys;
    SYSTEMTIME tempEndSys;
    SYSTEMTIME LocalSys;

    GetLocalTime(&LocalSys);
    
    iCmpRes = 0;
    iCmpRes = strlen(pTmpEvent->EventSubject);
    if(iCmpRes ==  0)
    {
        return CALE_ERROR_EDIT;
    }
    
    tempStartSys.wYear = pTmpEvent->EventStartYear;
    tempStartSys.wMonth = pTmpEvent->EventStartMonth;
    tempStartSys.wDay = pTmpEvent->EventStartDay;
    tempStartSys.wHour = pTmpEvent->EventStartHour;
    tempStartSys.wMinute = pTmpEvent->EventStartMinute;
    
    tempEndSys.wYear = pTmpEvent->EventEndYear;
    tempEndSys.wMonth = pTmpEvent->EventEndMonth;
    tempEndSys.wDay = pTmpEvent->EventEndDay;
    tempEndSys.wHour = pTmpEvent->EventEndHour;
    tempEndSys.wMinute = pTmpEvent->EventEndMinute;
    
	if(!CALE_CheckDate(&tempStartSys))
		return CALE_ERROR_STARTDATE;

	if(!CALE_CheckDate(&tempEndSys))
		return CALE_ERROR_ENDDATE;
	
    iCmpRes = CALE_CmpOnlyDate(&tempEndSys, &tempStartSys);
    if(iCmpRes == Early)
    {
        return CALE_ERROR_ENDDATE;
    }
    
//    iCmpRes = CALE_CmpOnlyDate(&tempStartSys, pSysTime);
//    if(iCmpRes == Early)
//    {
//        return CALE_ERROR_STARTDATE;
//    }
    
	iCmpRes = CALE_CmpOnlyDate(&tempStartSys, &LocalSys);
    if(iCmpRes == Early)
    {
        return CALE_ERROR_STARTDATE;
    }
//	else if(Late == iCmpRes)
//		return CALE_RETURN_SUCCESS;
//
//    iCmpRes = CALE_CmpOnlyTime(&tempStartSys, &LocalSys);
//    if(iCmpRes == Early)
//    {
//        return CALE_ERROR_STARTTIME;
//    }

	return CALE_RETURN_SUCCESS;
}

static DWORD RegisterCaleMeetAlarmClass(void)
{
    WNDCLASS wc;
    
    wc.style        = 0;
    wc.lpfnWndProc  = CaleMeetAlarmWndProc;
    wc.cbClsExtra   = 0;
    wc.cbWndExtra   = 0;
    wc.hInstance    = NULL;
    wc.hIcon        = NULL;
    wc.hCursor      = NULL;
    wc.hbrBackground= NULL;//GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName = NULL;
    wc.lpszClassName= "CaleMeetAlarmWndClass";
    
    if (!RegisterClass(&wc))
    {
        UnregisterClass("CaleMeetAlarmWndClass", NULL);
        if(!RegisterClass(&wc))
            return FALSE;
    }
    return TRUE;
}

void UnRegisterCaleMeetAlarmClass(void)
{
    UnregisterClass("CaleMeetAlarmWndClass", NULL);

    return;
}

BOOL CreateMeetAlarmWnd(HWND hwnd)
{
    if(IsWindow(HwndCaleMeetAlarm))
    {
        ShowWindow(HwndCaleMeetAlarm, SW_SHOW);
        BringWindowToTop(HwndCaleMeetAlarm);
        UpdateWindow(HwndCaleMeetAlarm);
    }
    else
    {
        HWND hFrameWnd;
        RECT rClient;

        if(!RegisterCaleMeetAlarmClass())
            return FALSE;
        
        hFrameWnd = CALE_GetFrame();
        GetClientRect(hFrameWnd, &rClient);
        
        HwndCaleMeetAlarm = CreateWindow(
            "CaleMeetAlarmWndClass",
            "",
            WS_VISIBLE | WS_CHILD, 
            rClient.left,
            rClient.top,
            rClient.right-rClient.left,
            rClient.bottom-rClient.top,
            hFrameWnd, 
            NULL,
            NULL,
            NULL
            );

        if(!HwndCaleMeetAlarm)
        {
            UnRegisterCaleMeetAlarmClass();
            return FALSE;
        }
        SendMessage(hFrameWnd, PWM_CREATECAPTIONBUTTON, 
            MAKEWPARAM(IDM_BUTTON_COMPLETE, 1), (LPARAM)"");
        SendMessage(hFrameWnd, PWM_CREATECAPTIONBUTTON, 
            MAKEWPARAM(IDM_BUTTON_QUIT, 0), (LPARAM)IDP_IME_CANCEL);
        SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_SELECT);
        
        SetWindowText(hFrameWnd, IDP_IME_ALARM);
        ShowWindow(hFrameWnd, SW_SHOW);
        UpdateWindow(hFrameWnd);
    }
    
    return TRUE;
}

static LRESULT CaleMeetAlarmWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    LRESULT lResult = TRUE;
    HDC hdc;
    RECT rcClient;
    static HWND hwndFocus;
    int i;
    
    GetClientRect(hWnd, &rcClient);

    switch(wMsgCmd)
    {
    case WM_CREATE:
        {
            HwndList = CreateWindow(
                "LISTBOX",
                "",
                WS_VISIBLE | WS_CHILD | WS_VSCROLL | LBS_NOTIFY | LBS_BITMAP,
                rcClient.left,
                rcClient.top,
                rcClient.right-rcClient.left,
                rcClient.bottom-rcClient.top,
                hWnd,
                NULL, NULL, NULL);

            SendMessage(HwndList, LB_ADDSTRING, NULL, (LPARAM)(IDP_ALARM_NO));
            SendMessage(HwndList, LB_ADDSTRING, NULL, (LPARAM)(IDP_ALARM_ON));
            SendMessage(HwndList, LB_ADDSTRING, NULL, (LPARAM)(IDP_ALARM_5MIN));
            SendMessage(HwndList, LB_ADDSTRING, NULL, (LPARAM)(IDP_ALARM_10MIN));
            SendMessage(HwndList, LB_ADDSTRING, NULL, (LPARAM)(IDP_ALARM_15MIN));
            SendMessage(HwndList, LB_ADDSTRING, NULL, (LPARAM)(IDP_ALARM_30MIN));
            SendMessage(HwndList, LB_ADDSTRING, NULL, (LPARAM)(IDP_ALARM_45MIN));
            SendMessage(HwndList, LB_ADDSTRING, NULL, (LPARAM)(IDP_ALARM_1HOUR));
            SendMessage(HwndList, LB_ADDSTRING, NULL, (LPARAM)(IDP_ALARM_6HOUR));
            SendMessage(HwndList, LB_ADDSTRING, NULL, (LPARAM)(IDP_ALARM_12HOUR));
            SendMessage(HwndList, LB_ADDSTRING, NULL, (LPARAM)(IDP_ALARM_1DAY));
            SendMessage(HwndList, LB_ADDSTRING, NULL, (LPARAM)(IDP_ALARM_1WEEK));
            for(i=0; i<12; i++)
            {
                SendMessage(HwndList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, i), (LPARAM)hUnSelect);
            }
            
            SetFocus(HwndList);
            hwndFocus = HwndList;
            SendMessage(HwndList, LB_SETCURSEL, CurAlarmRep, 0);
            SendMessage(HwndList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, CurAlarmRep), (LPARAM)hSelect);
			CALE_SetSystemIcons(CALE_GetFrame());
        }
        break;
        
    case WM_INITMENU:
        {
        }
        break;
        
    case WM_PAINT:
        {
            hdc = BeginPaint(hWnd, NULL);
            EndPaint(hWnd, NULL);
        }
        break;
        
    case WM_ACTIVATE:
    case PWM_SHOWWINDOW:
        {
            HWND hFrameWnd;

            hFrameWnd = CALE_GetFrame();
            
            SetFocus(HwndList);
			CALE_SetSystemIcons(hFrameWnd);
            
            SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
            SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDP_IME_CANCEL);
            SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_SELECT);
            
            SetWindowText(hFrameWnd, IDP_IME_ALARM);
        }
        break;

    case WM_COMMAND:
        {
            switch(LOWORD(wParam))
            {                
            case IDM_BUTTON_QUIT:
                {
                    PostMessage(hWnd, WM_CLOSE, 0, 0);
                }
                break;
                
            default:
                lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
                break;
            }
        }
        break;
        
    case WM_KEYDOWN:
        {
            switch(LOWORD(wParam))
            {
            case VK_RETURN:
                {
                }
                break;
                
            case VK_F5:
                {
                    int temp;
                    
                    temp = SendMessage(HwndList, LB_GETCURSEL, 0, 0);
                    if((temp >= 0) && (temp <= 12) && (temp != CurAlarmRep))
                    {
                        CurAlarmRep = temp;
                        SendMessage(HwndCaleMeet, CALE_ALARM_CHANGED, 0, 0);
                    }
                    PostMessage(hWnd, WM_CLOSE, 0, 0);
                }
                break;
                
            case VK_F10:
                {
                    PostMessage(hWnd, WM_COMMAND, MAKEWPARAM(IDM_BUTTON_QUIT, 0), 0);
                }
                break;
                
            default:
                lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
                break;
            }
        }
        break;

    case WM_CLOSE:
        {
            PostMessage(CALE_GetFrame(), PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
            DestroyWindow(hWnd);
        }
        break;
        
    case WM_DESTROY:
        {
            HwndCaleMeetAlarm = NULL;
            UnRegisterCaleMeetAlarmClass();
        }
        break;
        
    default:
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
    return lResult;
}

static DWORD RegisterCaleMeetRepClass(void)
{
    WNDCLASS wc;
    
    wc.style        = 0;
    wc.lpfnWndProc  = CaleMeetRepWndProc;
    wc.cbClsExtra   = 0;
    wc.cbWndExtra   = 0;
    wc.hInstance    = NULL;
    wc.hIcon        = NULL;
    wc.hCursor      = NULL;
    wc.hbrBackground= NULL;//GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName = NULL;
    wc.lpszClassName= "CaleMeetRepWndClass";
    
    if (!RegisterClass(&wc))
    {
        UnregisterClass("CaleMeetRepWndClass", NULL);
        if(!RegisterClass(&wc))
            return FALSE;
    }
    return TRUE;
}

void UnRegisterCaleMeetRepClass(void)
{
    UnregisterClass("CaleMeetRepWndClass", NULL);
    return;
}

BOOL CreateMeetRepWnd(HWND hwnd)
{
    if(IsWindow(HwndCaleMeetRep))
    {
        ShowWindow(HwndCaleMeetRep, SW_SHOW);
        BringWindowToTop(HwndCaleMeetRep);
        UpdateWindow(HwndCaleMeetRep);
    }
    else
    {
        HWND hFrameWnd;
        RECT rClient;
        
        if(!RegisterCaleMeetRepClass())
            return FALSE;
        
        hFrameWnd = CALE_GetFrame();
        GetClientRect(hFrameWnd, &rClient);
        
        HwndCaleMeetRep = CreateWindow(
            "CaleMeetRepWndClass",
            "",
            WS_VISIBLE | WS_CHILD /*| WS_VSCROLL*/, 
            rClient.left,
            rClient.top,
            rClient.right-rClient.left,
            rClient.bottom-rClient.top,
            hFrameWnd, 
            NULL,
            NULL,
            NULL
            );
        
        if(!HwndCaleMeetRep)
        {
            UnRegisterCaleMeetRepClass();
            return FALSE;
        }
        SendMessage(hFrameWnd, PWM_CREATECAPTIONBUTTON, 
            MAKEWPARAM(IDM_BUTTON_COMPLETE, 1), (LPARAM)"");
        SendMessage(hFrameWnd, PWM_CREATECAPTIONBUTTON, 
            MAKEWPARAM(IDM_BUTTON_QUIT, 0), (LPARAM)IDP_IME_CANCEL);
        SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_SELECT);
        
        SetWindowText(hFrameWnd, IDP_IME_REPEAT);
        ShowWindow(hFrameWnd, SW_SHOW);
        UpdateWindow(hFrameWnd);
    }
    
    return TRUE;
}

static LRESULT CaleMeetRepWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    LRESULT lResult = TRUE;
    HDC hdc;
    RECT rcClient;
    static HWND hwndFocus;
    int i;
    
    GetClientRect(hWnd, &rcClient);

    switch(wMsgCmd)
    {
    case WM_CREATE:
        {
            HwndList = CreateWindow(
                "LISTBOX",
                "",
                WS_VISIBLE | WS_CHILD | LBS_NOTIFY | LBS_BITMAP,
                rcClient.left,
                rcClient.top,
                rcClient.right-rcClient.left,
                rcClient.bottom-rcClient.top,
                hWnd,
                NULL, NULL, NULL);

            SendMessage(HwndList, LB_ADDSTRING, NULL, (LPARAM)(IDP_IME_NONE));
            SendMessage(HwndList, LB_ADDSTRING, NULL, (LPARAM)(IDP_IME_EVERYDAY));
            SendMessage(HwndList, LB_ADDSTRING, NULL, (LPARAM)(IDP_IME_EVERYWEEK));
            SendMessage(HwndList, LB_ADDSTRING, NULL, (LPARAM)(IDP_IME_EVERYTWOWEEK));
            SendMessage(HwndList, LB_ADDSTRING, NULL, (LPARAM)(IDP_IME_EVERYMONTH));

            for(i=0; i<5; i++)
            {
                SendMessage(HwndList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, i), (LPARAM)hUnSelect);
            }

            SetFocus(HwndList);
            hwndFocus = HwndList;
            SendMessage(HwndList, LB_SETCURSEL, CurAlarmRep, 0);
            SendMessage(HwndList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, CurAlarmRep), (LPARAM)hSelect);
			CALE_SetSystemIcons(CALE_GetFrame());
        }
        break;
        
    case WM_INITMENU:
        {
        }
        break;
        
    case WM_PAINT:
        {
            hdc = BeginPaint(hWnd, NULL);
            EndPaint(hWnd, NULL);
        }
        break;
        
    case WM_ACTIVATE:
    case PWM_SHOWWINDOW:
        {
            HWND hFrameWnd;
            
            hFrameWnd = CALE_GetFrame();
            
            SetFocus(HwndList);
			CALE_SetSystemIcons(hFrameWnd);
            
            SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
            SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDP_IME_CANCEL);
            SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_SELECT);
            
            SetWindowText(hFrameWnd, IDP_IME_REPEAT);
        }
        break;

    case WM_COMMAND:
        {
            switch(LOWORD(wParam))
            {                
            case IDM_BUTTON_QUIT:
                {
                    PostMessage(hWnd, WM_CLOSE, 0, 0);
                }
                break;
                
            default:
                lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
                break;
            }
        }
        break;
        
    case WM_KEYDOWN:
        {
            switch(LOWORD(wParam))
            {
            case VK_RETURN:
                {
                }
                break;
                
            case VK_F5:
                {
                    int temp;
                    
                    temp = SendMessage(HwndList, LB_GETCURSEL, 0, 0);
                    if((temp >= 0) && (temp <= 6) && (temp != CurAlarmRep))
                    {
                        CurAlarmRep = temp;
                        SendMessage(HwndCaleMeet, CALE_ALARM_CHANGED, 0, 0);
                    }
                    PostMessage(hWnd, WM_CLOSE, 0, 0);                    
                }
                break;
                
            case VK_F10:
                {
                    PostMessage(hWnd, WM_COMMAND, MAKEWPARAM(IDM_BUTTON_QUIT, 0), 0);
                }
                break;
                
            default:
                lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
                break;
            }
        }
        break;
        
    case WM_CLOSE:
        {
            PostMessage(CALE_GetFrame(), PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
            DestroyWindow(hWnd);
        }
        break;
        
    case WM_DESTROY:
        {
            HwndCaleMeetRep = NULL;
            UnRegisterCaleMeetRepClass();
        }
        break;
        
    default:
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
    return lResult;
}

static DWORD RegisterCaleAnniAlarmClass(void)
{
    WNDCLASS wc;
    
    wc.style        = 0;
    wc.lpfnWndProc  = CaleAnniAlarmWndProc;
    wc.cbClsExtra   = 0;
    wc.cbWndExtra   = 0;
    wc.hInstance    = NULL;
    wc.hIcon        = NULL;
    wc.hCursor      = NULL;
    wc.hbrBackground= NULL;//GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName = NULL;
    wc.lpszClassName= "CaleAnniAlarmWndClass";
    
    if (!RegisterClass(&wc))
    {
        UnregisterClass("CaleAnniAlarmWndClass", NULL);
        if(!RegisterClass(&wc))
            return FALSE;
    }
    return TRUE;
}

void UnRegisterCaleAnniAlarmClass(void)
{
    UnregisterClass("CaleAnniAlarmWndClass", NULL);
}

BOOL CreateAnniAlarmWnd(HWND hwnd)
{
    if(IsWindow(HwndCaleAnniAlarm))
    {
        ShowWindow(HwndCaleAnniAlarm, SW_SHOW);
        BringWindowToTop(HwndCaleAnniAlarm);
        UpdateWindow(HwndCaleAnniAlarm);
    }
    else
    {
        HWND hFrameWnd;
        RECT rClient;
        
        if(!RegisterCaleAnniAlarmClass())
            return FALSE;
        
        hFrameWnd = CALE_GetFrame();
        GetClientRect(hFrameWnd, &rClient);
        
        HwndCaleAnniAlarm = CreateWindow(
            "CaleAnniAlarmWndClass",
            "",
            WS_VISIBLE | WS_CHILD, 
            rClient.left,
            rClient.top,
            rClient.right-rClient.left,
            rClient.bottom-rClient.top,
            hFrameWnd, 
            NULL,
            NULL,
            NULL
            );
        
        if(!HwndCaleAnniAlarm)
        {
            UnRegisterCaleAnniAlarmClass();
            return FALSE;
        }
        SendMessage(hFrameWnd, PWM_CREATECAPTIONBUTTON, 
            MAKEWPARAM(IDM_BUTTON_COMPLETE, 1), (LPARAM)IDP_IME_SAVE);
        SendMessage(hFrameWnd, PWM_CREATECAPTIONBUTTON, 
            MAKEWPARAM(IDM_BUTTON_QUIT, 0), (LPARAM)IDP_IME_CANCEL);
        SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
        
        SetWindowText(hFrameWnd, IDP_IME_ALARM);
        ShowWindow(hFrameWnd, SW_SHOW);
        UpdateWindow(hFrameWnd);
    }
    
    return TRUE;
}

static LRESULT CaleAnniAlarmWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    LRESULT lResult = TRUE;
    HDC hdc;
    RECT rcClient;
    SYSTEMTIME StartSysTime;
    SYSTEMTIME TmpTime;
    SYSTEMTIME ReturnTime;
    static HWND hwndFocus = NULL;
	static BOOL bKeyDown = FALSE;
    
    GetClientRect(hWnd, &rcClient);

    switch(wMsgCmd)
    {
    case WM_CREATE:
        {
            DATEFORMAT forDate;
            TIMEFORMAT forTime;

            forDate = GetDateFormt();
            forTime = GetTimeFormt();
            
            if(bNewDate || (CurAnniInfo.AnniAlarmFlag == 0))
            {
                memcpy(&StartSysTime, &CurSysTime, sizeof(SYSTEMTIME));
            }
            else
            {
                StartSysTime.wYear = CurAnniInfo.AnniYear;
                StartSysTime.wMonth = CurAnniInfo.AnniMonth;
                StartSysTime.wDay = CurAnniInfo.AnniDay;
                StartSysTime.wHour = CurAnniInfo.AnniHour;
                StartSysTime.wMinute = CurAnniInfo.AnniMinute;
            }
            
            memset(&ImeEdit, 0x0, sizeof(IMEEDIT));
            ImeEdit.hwndNotify = (HWND)hWnd;
            ImeEdit.dwAttrib = 0;
            ImeEdit.dwAscTextMax = 0;
            ImeEdit.dwUniTextMax = 0;
            ImeEdit.wPageMax = 0;
            ImeEdit.pszImeName = NULL;//"英文小写";
            ImeEdit.pszCharSet = NULL;
            ImeEdit.pszTitle = NULL;
            ImeEdit.uMsgSetText = 0;
            
            if((forDate == DF_DMY_DOT) || (forDate == DF_DMY_SLD) || (forDate == DF_DMY_DSH))
            {
                hEndDate = CreateWindow(
                    "TIMEEDIT",
                    IDP_IME_ALERTDATE,
                    WS_CHILD | WS_VISIBLE | WS_TABSTOP | 
                    TES_TITLE | CS_NOSYSCTRL | TES_DATE_DMY,
                    PARTWND_X, PARTWND_Y, PARTWND_WIDTH, PARTWND_HEIGHT,
                    hWnd,
                    NULL,
                    NULL,
                    (PVOID)&StartSysTime
                    );
            }
            else if((forDate == DF_MDY_DOT) || (forDate == DF_MDY_SLD) || (forDate == DF_MDY_DSH))
            {
                hEndDate = CreateWindow(
                    "TIMEEDIT",
                    IDP_IME_ALERTDATE,
                    WS_CHILD | WS_VISIBLE | WS_TABSTOP | 
                    TES_TITLE | CS_NOSYSCTRL | TES_DATE_MDY,
                    PARTWND_X, PARTWND_Y, PARTWND_WIDTH, PARTWND_HEIGHT,
                    hWnd,
                    NULL,
                    NULL,
                    (PVOID)&StartSysTime
                    );
            }
            else
            {
                hEndDate = CreateWindow(
                    "TIMEEDIT",
                    IDP_IME_ALERTDATE,
                    WS_CHILD | WS_VISIBLE | WS_TABSTOP | 
                    TES_TITLE | CS_NOSYSCTRL | TES_DATE_YMD,
                    PARTWND_X, PARTWND_Y, PARTWND_WIDTH, PARTWND_HEIGHT,
                    hWnd,
                    NULL,
                    NULL,
                    (PVOID)&StartSysTime
                    );
            }
            if((forDate == DF_DMY_DOT) || (forDate == DF_MDY_DOT) || (forDate == DF_YMD_DOT))
            {
                SendMessage(hEndDate, TEM_SETSEPARATOR, 0, (LPARAM)'.');
            }
            else if((forDate == DF_DMY_SLD) || (forDate == DF_MDY_SLD) || (forDate == DF_YMD_SLD))
            {
                SendMessage(hEndDate, TEM_SETSEPARATOR, 0, (LPARAM)'/');
            }
            else
            {
                SendMessage(hEndDate, TEM_SETSEPARATOR, 0, (LPARAM)'-');
            }
            
            if(forTime == TF_24)
            {
                hEndTime = CreateWindow(
                    "TIMEEDIT",
                    IDP_IME_ALERTTIME,
                    WS_CHILD | WS_VISIBLE | WS_TABSTOP | 
                    TES_TITLE | CS_NOSYSCTRL | TES_TIME_24HR,
                    PARTWND_X, PARTWND_Y + PARTWND_HEIGHT, PARTWND_WIDTH, PARTWND_HEIGHT,
                    hWnd,
                    NULL,
                    NULL,
                    (PVOID)&StartSysTime
                    );
            }
            else
            {
                hEndTime = CreateWindow(
                    "TIMEEDIT",
                    IDP_IME_ALERTTIME,
                    WS_CHILD | WS_VISIBLE | WS_TABSTOP | 
                    TES_TITLE | CS_NOSYSCTRL | TES_TIME_12HR,
                    PARTWND_X, PARTWND_Y + PARTWND_HEIGHT, PARTWND_WIDTH, PARTWND_HEIGHT,
                    hWnd,
                    NULL,
                    NULL,
                    (PVOID)&StartSysTime
                    );
            }
            
            hwndFocus = hEndDate;
            SetFocus(hEndDate);
			bKeyDown = FALSE;
			CALE_SetSystemIcons(CALE_GetFrame());
        }
    	break;
        
    case WM_INITMENU:
        {
        }
        break;

    case WM_PAINT:
        {
            hdc = BeginPaint(hWnd, NULL);
            EndPaint(hWnd, NULL);
        }
    	break;

    case WM_ACTIVATE:
    case PWM_SHOWWINDOW:
        {
            HWND hFrameWnd;
            
            hFrameWnd = CALE_GetFrame();
            
            SetFocus(hwndFocus);
			CALE_SetSystemIcons(hFrameWnd);
            
            SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDP_IME_SAVE);
            SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDP_IME_CANCEL);
            SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
            
            SetWindowText(hFrameWnd, IDP_IME_ALARM);
        }
        break;

    case WM_COMMAND:
        {
            switch(LOWORD(wParam))
            {
            case IDM_BUTTON_COMPLETE:
                {
                    SendMessage(hEndDate, TEM_GETTIME, 0, (LPARAM)(&TmpTime));
                    ReturnTime.wYear = TmpTime.wYear;
                    ReturnTime.wMonth = TmpTime.wMonth;
                    ReturnTime.wDay = TmpTime.wDay;
					if(!CALE_CheckDate(&ReturnTime))
					{
						PLXTipsWin(CALE_GetFrame(), NULL, NULL, IDP_STRING_ENDDTERROR, NULL, 
							Notify_Failure, IDP_IME_OK, NULL, WAITTIMEOUT);
						break;
					}

                    SendMessage(hEndTime, TEM_GETTIME, 0, (LPARAM)(&TmpTime));
                    ReturnTime.wHour = TmpTime.wHour;
                    ReturnTime.wMinute = TmpTime.wMinute;
                    SendMessage(HwndCaleAnni, CALE_ALARM_CHANGED, 0, (LPARAM)(&ReturnTime));
                    PostMessage(hWnd, WM_CLOSE, 0, 0);
                }
            	break;

            case IDM_BUTTON_QUIT:
                {
                    PostMessage(hWnd, WM_CLOSE, 0, 0);
                }
                break;

            default:
                lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
                break;
            }
        }
        break;

    case WM_KEYDOWN:
        {
            switch(LOWORD(wParam))
            {
            case VK_RETURN:
                {
                    PostMessage(hWnd, WM_COMMAND, MAKEWPARAM(IDM_BUTTON_COMPLETE, 0), 0);
                }
                break;

            case VK_F5:
                {
                }
            	break;

            case VK_F10:
                {
                    PostMessage(hWnd, WM_COMMAND, MAKEWPARAM(IDM_BUTTON_QUIT, 0), 0);
                }
            	break;

            case VK_UP:
            case VK_DOWN:
                {
					if(!bKeyDown)
					{
						SetTimer(hWnd, TIMERID_VK_UP, TIMER_KEY_LONG, NULL);
						bKeyDown = TRUE;
					}
					if(hwndFocus != hEndDate)
	                    hwndFocus = hEndDate;
					else
						hwndFocus = hEndTime;

                    SetFocus(hwndFocus);
                }
                break;

            default:
                lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
                break;
            }
        }
        break;

	case PWM_ACTIVATE:
		if(!wParam)
		{
			bKeyDown = FALSE;
			KillTimer(hWnd, TIMERID_VK_UP);
		}
		lResult = PDADefWindowProc(hWnd, PWM_ACTIVATE, wParam, lParam);
		break;

	case WM_KEYUP:
		bKeyDown = FALSE;
		KillTimer(hWnd, TIMERID_VK_UP);
		break;

	case WM_TIMER:
		KillTimer(hWnd, TIMERID_VK_UP);
		PostMessage(hWnd, WM_KEYDOWN, VK_UP, 0);
		SetTimer(hWnd, TIMERID_VK_UP, TIMER_KEY_REPEAT, NULL);
		break;

    case WM_CLOSE:
        {
			bKeyDown = FALSE;
			KillTimer(hWnd, TIMERID_VK_UP);
            PostMessage(CALE_GetFrame(), PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
            DestroyWindow(hWnd);
        }
        break;
        
    case WM_DESTROY:
        {
            HwndCaleAnniAlarm = NULL;
            UnRegisterCaleAnniAlarmClass();
        }
        break;

    default:
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
    return lResult;
}

static DWORD RegisterCaleOpenClass(void)
{
    WNDCLASS wc;
    
    wc.style        = 0;
    wc.lpfnWndProc  = CaleOpenWndProc;
    wc.cbClsExtra   = 0;
    wc.cbWndExtra   = sizeof(CALE_VIEWCREATEDATA);
    wc.hInstance    = NULL;
    wc.hIcon        = NULL;
    wc.hCursor      = NULL;
    wc.hbrBackground= NULL;//GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName = NULL;
    wc.lpszClassName= "CaleOpenWndClass";
    
    if (!RegisterClass(&wc))
    {
        UnregisterClass("CaleOpenWndClass", NULL);
        if(!RegisterClass(&wc))
            return FALSE;
    }
    return TRUE;
}

void UnRegisterCaleOpenClass(void)
{
    UnregisterClass("CaleOpenWndClass", NULL);
}

static LRESULT CaleOpenWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    PCALE_VIEWCREATEDATA pCreateData;

    LRESULT lResult = TRUE;
    HDC		hdc;
    RECT	rcClient;
    static HWND hwndFocus = NULL;
    
    pCreateData = GetUserData(hWnd);
    
    switch(wMsgCmd)
    {
    case WM_CREATE:
        {
            memcpy(pCreateData, ((LPCREATESTRUCT)lParam)->lpCreateParams, sizeof(CALE_VIEWCREATEDATA));
            GetClientRect(hWnd, &rcClient);

            hListWnd = CreateWindow(
                "LISTBOX",
                "",
               // WS_VISIBLE | WS_CHILD | WS_VSCROLL | LBS_NOTIFY | LBS_BITMAP | LBS_MULTILINE,
				WS_VISIBLE | WS_CHILD | WS_VSCROLL | LBS_BITMAP | LBS_MULTILINE,
				rcClient.left,
                rcClient.top,
                rcClient.right-rcClient.left,
                rcClient.bottom-rcClient.top,
                hWnd,
                (HMENU)IDM_LISTBOX_ENTRY, 
				NULL, NULL);

            SetFocus(hListWnd);
            hwndFocus = hListWnd;
			bUpdated = TRUE;
            SendMessage(hWnd, PWM_SHOWWINDOW, 0, 0);
			CALE_SetSystemIcons(pCreateData->hFrameWnd);
        }
        break;

    case WM_INITMENU:
        {
        }
        break;
  
    case WM_PAINT:
        {
            hdc = BeginPaint(hWnd, NULL);
            EndPaint(hWnd, NULL);
        }
        break;
        
    case WM_ACTIVATE:
    case PWM_SHOWWINDOW:		
		if(bUpdated)
		{
			CALE_OpenEntry(hListWnd, pCreateData);
			bUpdated = FALSE;
		}
		else
		{
			if(CurCaleType == CALE_MEETING)
				SetWindowText(pCreateData->hFrameWnd, IDP_IME_MEETING);
			else if(CurCaleType == CALE_ANNI)
				SetWindowText(pCreateData->hFrameWnd, IDP_IME_ANNIVERARY);
			else if(CurCaleType == CALE_EVENT)
				SetWindowText(pCreateData->hFrameWnd, IDP_IME_EVENT);
		}
		//	SetFocus(hFormView);
		
		SendMessage(pCreateData->hFrameWnd, PWM_CREATECAPTIONBUTTON, 
			MAKEWPARAM(IDM_BUTTON_COMPLETE, 1), (LPARAM)IDP_IME_EDIT);
		SendMessage(pCreateData->hFrameWnd, PWM_CREATECAPTIONBUTTON, 
			MAKEWPARAM(IDM_BUTTON_QUIT, 0), (LPARAM)IDP_IME_BACK);
		SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
		
		CALE_SetSystemIcons(pCreateData->hFrameWnd);
		
		break;

	case WM_SETFOCUS:
		if(IsWindow(hListWnd))
			SetFocus(hListWnd);
		break;

    case WM_COMMAND:
        {
            switch(LOWORD(wParam))
            {
            case IDM_BUTTON_COMPLETE:
                {
                    if(CurCaleType == CALE_MEETING)
                    {
                        if(IsWindow(HwndCaleMeet))
                        {
                            ShowWindow(HwndCaleMeet, SW_SHOW);
                            BringWindowToTop(HwndCaleMeet);
                            UpdateWindow(HwndCaleMeet);
                        }
                        else
                        {
                            CALE_ENTRYCREATEDATA CreateData;
                            RECT rTemp;
                            
                            if(!RegisterCaleDisplayClass())
                                return FALSE;
                            
                            memset(&CreateData, 0x0, sizeof(CALE_ENTRYCREATEDATA));
                            CreateData.hFrameWnd = CALE_GetFrame();
                            CreateData.hParent = pCreateData->hParent;
							CreateData.hFocus = (HWND)SendMessage(hListWnd, LB_GETCURSEL, 0, 0);
                            
                            GetClientRect(CreateData.hFrameWnd, &rTemp);
                            
                            HwndCaleMeet = CreateWindow(
                                "CaleDisplayWndClass",
                                "",//IDP_IME_MEETING,
                                WS_VISIBLE | WS_CHILD | WS_VSCROLL,
                                rTemp.left, 
                                rTemp.top, 
                                rTemp.right - rTemp.left,
                                rTemp.bottom - rTemp.top,
                                CreateData.hFrameWnd,
                                NULL,
                                NULL, 
                                (PVOID)&CreateData
                                );
                            
                            if(!HwndCaleMeet)
                            {
                                UnRegisterCaleDiaplayClass();
                                return FALSE;
                            }
                            
                            SendMessage(CreateData.hFrameWnd, PWM_CREATECAPTIONBUTTON, 
                                MAKEWPARAM(IDM_BUTTON_COMPLETE, 1), (LPARAM)IDP_IME_SAVE);
                            SendMessage(CreateData.hFrameWnd, PWM_CREATECAPTIONBUTTON, 
                                MAKEWPARAM(IDM_BUTTON_QUIT, 0), (LPARAM)IDP_IME_CANCEL);
                        //	SendMessage(CreateData.hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
                            
                            SetWindowText(CreateData.hFrameWnd, IDP_IME_NEWMEETING);
                            ShowWindow(CreateData.hFrameWnd, SW_SHOW);
                        }
                    }
                    else if(CurCaleType == CALE_ANNI)
                    {
                        if(IsWindow(HwndCaleAnni))
                        {
                            ShowWindow(HwndCaleAnni, SW_SHOW);
                            BringWindowToTop(HwndCaleAnni);
                            UpdateWindow(HwndCaleAnni);
                        }
                        else
                        {
                            CALE_ENTRYCREATEDATA CreateData;
                            RECT rTemp;
                            
                            if(!RegisterCaleDisplayClass())
                                return FALSE;
                            
                            memset(&CreateData, 0x0, sizeof(CALE_ENTRYCREATEDATA));
                            CreateData.hFrameWnd = CALE_GetFrame();
                            CreateData.hParent = pCreateData->hParent;
							CreateData.hFocus = (HWND)SendMessage(hListWnd, LB_GETCURSEL, 0, 0);
                            
                            GetClientRect(CreateData.hFrameWnd, &rTemp);
                            
                            HwndCaleAnni = CreateWindow(
                                "CaleDisplayWndClass",
                                "",//IDP_IME_MEETING,
                                WS_VISIBLE | WS_CHILD | WS_VSCROLL,
                                rTemp.left, 
                                rTemp.top, 
                                rTemp.right - rTemp.left,
                                rTemp.bottom - rTemp.top,
                                CreateData.hFrameWnd,
                                NULL,
                                NULL, 
                                (PVOID)&CreateData
                                );
                            
                            if(!HwndCaleAnni)
                            {
                                UnRegisterCaleDiaplayClass();
                                return FALSE;
                            }
                            
                            SendMessage(CreateData.hFrameWnd, PWM_CREATECAPTIONBUTTON, 
                                MAKEWPARAM(IDM_BUTTON_COMPLETE, 1), (LPARAM)IDP_IME_SAVE);
                            SendMessage(CreateData.hFrameWnd, PWM_CREATECAPTIONBUTTON, 
                                MAKEWPARAM(IDM_BUTTON_QUIT, 0), (LPARAM)IDP_IME_CANCEL);
						//	SendMessage(CreateData.hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
                            
                            SetWindowText(CreateData.hFrameWnd, IDP_IME_NEWANNI);
                            ShowWindow(CreateData.hFrameWnd, SW_SHOW);
                        }
                    }
                    else if(CurCaleType == CALE_EVENT)
                    {
                        if(IsWindow(HwndCaleEvent))
                        {
                            ShowWindow(HwndCaleEvent, SW_SHOW);
                            BringWindowToTop(HwndCaleEvent);
                            UpdateWindow(HwndCaleEvent);
                        }
                        else
                        {
                            CALE_ENTRYCREATEDATA CreateData;
                            RECT rTemp;
                            
                            if(!RegisterCaleDisplayClass())
                                return FALSE;
                            
                            memset(&CreateData, 0x0, sizeof(CALE_ENTRYCREATEDATA));
                            CreateData.hFrameWnd = CALE_GetFrame();
                            CreateData.hParent = pCreateData->hParent;
							CreateData.hFocus = (HWND)SendMessage(hListWnd, LB_GETCURSEL, 0, 0);
                            
                            GetClientRect(CreateData.hFrameWnd, &rTemp);
                            
                            HwndCaleEvent = CreateWindow(
                                "CaleDisplayWndClass",
                                "",//IDP_IME_MEETING,
                                WS_VISIBLE | WS_CHILD | WS_VSCROLL,
                                rTemp.left, 
                                rTemp.top, 
                                rTemp.right - rTemp.left,
                                rTemp.bottom - rTemp.top,
                                CreateData.hFrameWnd,
                                NULL,
                                NULL, 
                                (PVOID)&CreateData
                                );
                            
                            if(!HwndCaleEvent)
                            {
                                UnRegisterCaleDiaplayClass();
                                return FALSE;
                            }
                            
                            SendMessage(CreateData.hFrameWnd, PWM_CREATECAPTIONBUTTON, 
                                MAKEWPARAM(IDM_BUTTON_COMPLETE, 1), (LPARAM)IDP_IME_SAVE);
                            SendMessage(CreateData.hFrameWnd, PWM_CREATECAPTIONBUTTON, 
                                MAKEWPARAM(IDM_BUTTON_QUIT, 0), (LPARAM)IDP_IME_CANCEL);
						//	SendMessage(CreateData.hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
                            
                            SetWindowText(CreateData.hFrameWnd, IDP_IME_NEWEVENT);
                            ShowWindow(CreateData.hFrameWnd, SW_SHOW);
                        }
                    }
                }
                break;
                
            case IDM_BUTTON_QUIT:
                {
                    PostMessage(hWnd, WM_CLOSE, 0, 0);
                }
                break;
                
			case IDM_LISTBOX_ENTRY:
				if(HIWORD(wParam) == LBN_SETFONT)
				{            
					HFONT hFont = NULL;

					GetFontHandle(&hFont, (lParam == 0) ? SMALL_FONT : LARGE_FONT);

					return (LRESULT)hFont;
				}
				break;

            default:
                lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
                break;
            }
        }
        break;
        
    case WM_KEYDOWN:
        {
            switch(LOWORD(wParam))
            {
            case VK_RETURN:
                {
                    PostMessage(hWnd, WM_COMMAND, MAKEWPARAM(IDM_BUTTON_COMPLETE, 0), 0);
                }
                break;
                
            case VK_F5:
                {                   
                }
                break;
                
            case VK_F10:
                {
                    PostMessage(hWnd, WM_COMMAND, MAKEWPARAM(IDM_BUTTON_QUIT, 0), 0);
                }
                break;
                
            default:
                lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
                break;
            }
        }
        break;
        
    case WM_CLOSE:
        {
            PostMessage(pCreateData->hFrameWnd, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
            DestroyWindow(hWnd);
        }
        break;
        
    case WM_DESTROY:
        {
            HwndCaleMeetOpen = NULL;
            HwndCaleAnniOpen = NULL;
            HwndCaleEventOpen = NULL;
			hListWnd = NULL;
            UnRegisterCaleOpenClass();
        }
        break;
        
    default:
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
    return lResult;
}

static void CALE_OpenEntry(HWND hListBox, PCALE_VIEWCREATEDATA pCreateData)
{
	SYSTEMTIME TmpSys;
	char TmpDate[15];
	char TmpTime[15];
	char TmpDateTime[30];
	int  i = 0;
	
	SetFocus(hListBox);
	
	SendMessage(hListBox, LB_RESETCONTENT, 0, 0);
	
	if(CurCaleType == CALE_MEETING)
	{
		SendMessage(hListBox, LB_ADDSTRING, NULL, (LPARAM)IDP_IME_SUBJECT);
		SendMessage(hListBox, LB_SETAUXTEXT, MAKEWPARAM(i, -1), (LPARAM)CurMeetInfo.MeetSubject);
		i ++;
		
		SendMessage(hListBox, LB_ADDSTRING, NULL, (LPARAM)IDP_IME_LOCATION);
		SendMessage(hListBox, LB_SETAUXTEXT, MAKEWPARAM(i, -1), (LPARAM)CurMeetInfo.MeetLocation);
		i ++;
		
		TmpSys.wYear = (WORD)CurMeetInfo.MeetStartYear;
		TmpSys.wMonth = (WORD)CurMeetInfo.MeetStartMonth;
		TmpSys.wDay = (WORD)CurMeetInfo.MeetStartDay;
		TmpSys.wHour = (WORD)CurMeetInfo.MeetStartHour;
		TmpSys.wMinute = (WORD)CurMeetInfo.MeetStartMinute;
		memset(TmpDate, 0x0, 15);
		memset(TmpTime, 0x0, 15);
		memset(TmpDateTime, 0x0, 30);
		GetTimeDisplay(TmpSys, TmpTime, TmpDate);
		
		SendMessage(hListBox, LB_ADDSTRING, NULL, (LPARAM)IDP_IME_STARTDATE);
		SendMessage(hListBox, LB_SETAUXTEXT, MAKEWPARAM(i, -1), (LPARAM)TmpDate);
		i ++;
		
		SendMessage(hListBox, LB_ADDSTRING, NULL, (LPARAM)IDP_IME_STARTTIME);
		SendMessage(hListBox, LB_SETAUXTEXT, MAKEWPARAM(i, -1), (LPARAM)TmpTime);
		i ++;
		
		TmpSys.wYear = (WORD)CurMeetInfo.MeetEndYear;
		TmpSys.wMonth = (WORD)CurMeetInfo.MeetEndMonth;
		TmpSys.wDay = (WORD)CurMeetInfo.MeetEndDay;
		TmpSys.wHour = (WORD)CurMeetInfo.MeetEndHour;
		TmpSys.wMinute = (WORD)CurMeetInfo.MeetEndMinute;
		memset(TmpDate, 0x0, 15);
		memset(TmpTime, 0x0, 15);
		memset(TmpDateTime, 0x0, 30);
		GetTimeDisplay(TmpSys, TmpTime, TmpDate);
		
		SendMessage(hListBox, LB_ADDSTRING, NULL, (LPARAM)IDP_IME_ENDDATE);
		SendMessage(hListBox, LB_SETAUXTEXT, MAKEWPARAM(i, -1), (LPARAM)TmpDate);
		i ++;
		
		SendMessage(hListBox, LB_ADDSTRING, NULL, (LPARAM)IDP_IME_ENDTIME);
		SendMessage(hListBox, LB_SETAUXTEXT, MAKEWPARAM(i, -1), (LPARAM)TmpTime);
		i ++;
		
		SendMessage(hListBox, LB_ADDSTRING, NULL, (LPARAM)IDP_IME_REPEAT);
		if(CurMeetInfo.MeetRepeatStyle == CALE_REP_NONE)
			SendMessage(hListBox, LB_SETAUXTEXT, MAKEWPARAM(i, -1), (LPARAM)IDP_IME_NONE);
		else if(CurMeetInfo.MeetRepeatStyle == CALE_REP_DAY)
			SendMessage(hListBox, LB_SETAUXTEXT, MAKEWPARAM(i, -1), (LPARAM)IDP_IME_EVERYDAY);
		else if(CurMeetInfo.MeetRepeatStyle == CALE_REP_WEEK)
			SendMessage(hListBox, LB_SETAUXTEXT, MAKEWPARAM(i, -1), (LPARAM)IDP_IME_EVERYWEEK);
		else if(CurMeetInfo.MeetRepeatStyle == CALE_REP_TWOWEEK)
			SendMessage(hListBox, LB_SETAUXTEXT, MAKEWPARAM(i, -1), (LPARAM)IDP_IME_EVERYTWOWEEK);
		else if(CurMeetInfo.MeetRepeatStyle == CALE_REP_MONTH)
			SendMessage(hListBox, LB_SETAUXTEXT, MAKEWPARAM(i, -1), (LPARAM)IDP_IME_EVERYMONTH);

		i ++;
		
		SendMessage(hListBox, LB_ADDSTRING, NULL, (LPARAM)IDP_IME_ALARM);
		if(CurMeetInfo.MeetAlarmFlag == CALE_NO_ALARM)
			SendMessage(hListBox, LB_SETAUXTEXT, MAKEWPARAM(i, -1), (LPARAM)IDP_ALARM_NO);
		else if(CurMeetInfo.MeetAlarmFlag == CALE_ON_TIME)
			SendMessage(hListBox, LB_SETAUXTEXT, MAKEWPARAM(i, -1), (LPARAM)IDP_ALARM_ON);
		else if(CurMeetInfo.MeetAlarmFlag == CALE_BEFORE_5MIN)
			SendMessage(hListBox, LB_SETAUXTEXT, MAKEWPARAM(i, -1), (LPARAM)IDP_ALARM_5MIN);
		else if(CurMeetInfo.MeetAlarmFlag == CALE_BEFORE_10MIN)
			SendMessage(hListBox, LB_SETAUXTEXT, MAKEWPARAM(i, -1), (LPARAM)IDP_ALARM_10MIN);
		else if(CurMeetInfo.MeetAlarmFlag == CALE_BEFORE_15MIN)
			SendMessage(hListBox, LB_SETAUXTEXT, MAKEWPARAM(i, -1), (LPARAM)IDP_ALARM_15MIN);
		else if(CurMeetInfo.MeetAlarmFlag == CALE_BEFORE_30MIN)
			SendMessage(hListBox, LB_SETAUXTEXT, MAKEWPARAM(i, -1), (LPARAM)IDP_ALARM_30MIN);
		else if(CurMeetInfo.MeetAlarmFlag == CALE_BEFORE_45MIN)
			SendMessage(hListBox, LB_SETAUXTEXT, MAKEWPARAM(i, -1), (LPARAM)IDP_ALARM_45MIN);
		else if(CurMeetInfo.MeetAlarmFlag == CALE_BEFORE_1HOUR)
			SendMessage(hListBox, LB_SETAUXTEXT, MAKEWPARAM(i, -1), (LPARAM)IDP_ALARM_1HOUR);
		else if(CurMeetInfo.MeetAlarmFlag == CALE_BEFORE_6HOUR)
			SendMessage(hListBox, LB_SETAUXTEXT, MAKEWPARAM(i, -1), (LPARAM)IDP_ALARM_6HOUR);
		else if(CurMeetInfo.MeetAlarmFlag == CALE_BEFORE_12HOUR)
			SendMessage(hListBox, LB_SETAUXTEXT, MAKEWPARAM(i, -1), (LPARAM)IDP_ALARM_12HOUR);
		else if(CurMeetInfo.MeetAlarmFlag == CALE_BEFORE_1DAY)
			SendMessage(hListBox, LB_SETAUXTEXT, MAKEWPARAM(i, -1), (LPARAM)IDP_ALARM_1DAY);
		else if(CurMeetInfo.MeetAlarmFlag == CALE_BEFORE_1WEEK)
			SendMessage(hListBox, LB_SETAUXTEXT, MAKEWPARAM(i, -1), (LPARAM)IDP_ALARM_1WEEK);
		i ++;
		
		SetWindowText(pCreateData->hFrameWnd, IDP_IME_MEETING);
	}
	else if(CurCaleType == CALE_ANNI)
	{
		SendMessage(hListBox, LB_ADDSTRING, NULL, (LPARAM)IDP_IME_SUBJECT);
		SendMessage(hListBox, LB_SETAUXTEXT, MAKEWPARAM(i, -1), (LPARAM)CurAnniInfo.AnniOccasion);
		i ++;
		
		TmpSys.wYear = (WORD)CurAnniInfo.AnniDisYear;
		TmpSys.wMonth = (WORD)CurAnniInfo.AnniDisMonth;
		TmpSys.wDay = (WORD)CurAnniInfo.AnniDisDay;
		memset(TmpDate, 0x0, 15);
		memset(TmpTime, 0x0, 15);
		GetTimeDisplay(TmpSys, TmpTime, TmpDate);
		
		SendMessage(hListBox, LB_ADDSTRING, NULL, (LPARAM)IDP_IME_STARTDATE);
		SendMessage(hListBox, LB_SETAUXTEXT, MAKEWPARAM(i, -1), (LPARAM)TmpDate);
		i ++;
		
		SendMessage(hListBox, LB_ADDSTRING, NULL, (LPARAM)IDP_IME_ALARM);
		if(CurAnniInfo.AnniAlarmFlag)
		{
			char cTmpDis[17];
			
			memset(cTmpDis, 0x0, sizeof(char) * 17);
			
			TmpSys.wYear = (WORD)CurAnniInfo.AnniYear;
			TmpSys.wMonth = (WORD)CurAnniInfo.AnniMonth;
			TmpSys.wDay = (WORD)CurAnniInfo.AnniDay;
			TmpSys.wHour = (WORD)CurAnniInfo.AnniHour;
			TmpSys.wMinute = (WORD)CurAnniInfo.AnniMinute;
			
		//	CALE_DateTimeSysToChar(&TmpSys, cTmpDis);
		//	CALE_GetFormatedDate(&TmpSys, cTmpDis);
			// fixed format here
			sprintf(cTmpDis, "%02d.%02d.%04d %02d:%02d", TmpSys.wDay, 
				TmpSys.wMonth, TmpSys.wYear, TmpSys.wHour, TmpSys.wMinute);
			SendMessage(hListBox, LB_SETAUXTEXT, MAKEWPARAM(i, -1), (LPARAM)cTmpDis);
		}
		else
		{
			SendMessage(hListBox, LB_SETAUXTEXT, MAKEWPARAM(i, -1), (LPARAM)IDP_IME_OFF);
		}
		i ++;
		
		SetWindowText(pCreateData->hFrameWnd, IDP_IME_ANNIVERARY);
	}
	else if(CurCaleType == CALE_EVENT)
	{
		SendMessage(hListBox, LB_ADDSTRING, NULL, (LPARAM)IDP_IME_SUBJECT);
		SendMessage(hListBox, LB_SETAUXTEXT, MAKEWPARAM(i, -1), (LPARAM)CurEventInfo.EventSubject);
		i ++;
		
		TmpSys.wYear = (WORD)CurEventInfo.EventStartYear;
		TmpSys.wMonth = (WORD)CurEventInfo.EventStartMonth;
		TmpSys.wDay = (WORD)CurEventInfo.EventStartDay;
		memset(TmpDate, 0x0, 15);
		memset(TmpTime, 0x0, 15);
		GetTimeDisplay(TmpSys, TmpTime, TmpDate);
		
		SendMessage(hListBox, LB_ADDSTRING, NULL, (LPARAM)IDP_IME_STARTDATE);
		SendMessage(hListBox, LB_SETAUXTEXT, MAKEWPARAM(i, -1), (LPARAM)TmpDate);
		i ++;
		
		TmpSys.wYear = (WORD)CurEventInfo.EventEndYear;
		TmpSys.wMonth = (WORD)CurEventInfo.EventEndMonth;
		TmpSys.wDay = (WORD)CurEventInfo.EventEndDay;
		memset(TmpDate, 0x0, 15);
		memset(TmpTime, 0x0, 15);
		GetTimeDisplay(TmpSys, TmpTime, TmpDate);
		
		SendMessage(hListBox, LB_ADDSTRING, NULL, (LPARAM)IDP_IME_ENDDATE);
		SendMessage(hListBox, LB_SETAUXTEXT, MAKEWPARAM(i, -1), (LPARAM)TmpDate);
		i ++;
		
		SetWindowText(pCreateData->hFrameWnd, IDP_IME_EVENT);
	}
	SendMessage(hListBox, LB_SETCURSEL, 0, 0);
	
}
