/***************************************************************************
*
*                      Pollex Mobile Platform
*
* Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
*                       All Rights Reserved 
*
* Module   : CaleStruct.h
*
* Purpose  : 
*
\**************************************************************************/

#include "CaleHeader.h"

//date struct
typedef struct tagCaleDate
{
    WORD    bYear;
    BYTE    bMonth;
    BYTE    bDay;
    BYTE    bHour;
    BYTE    bMinute;
    BYTE    bSecond;
    WORD    wUnused;
}CaleDATE;

//alarm list node
typedef struct tagCaleAlarmNode
{
    BYTE    CaleType;                   //meeting\anniversary\event
    int     iSaveIndex;                 //save index
    int     SnoozeNum;
    BOOL    AlarmFlag;
    
    WORD    wCaldYear;                  //mark year
    BYTE    bCaldMonth;                 //mark month
    BYTE    bCaldDay;                   //mark day
    BYTE    bCaldHour;                  //mark hour
    BYTE    bCaldMinute;                //mark minute
    BYTE    bCaldWeek;                  //mark week
    
    WORD    wAlmYear;                   //alarm year
    BYTE    bAlmMonth;
    BYTE    bAlmDay;
    BYTE    bAlmHour;
    BYTE    bAlmMinute;    

    BYTE    bRepStyle;                  //repeat style
    BYTE    bRepMask;                   //whether the repeated end date is available

    CaleDATE    RepEndDate;             //repeat end date
    
    BYTE    bSound;
    char    CaldSubject[MAX_CALE_SUBJECT_LEN + 1];      //meeting subject
    char    CaldLocation[MAX_CALE_LOCATION_LEN + 1];    //meeting location

    struct  tagCaleAlarmNode *pNext;
}CaleAlarmNode;

//alarm list head
typedef struct tagCaleAlarmHead
{
    int     nNodeNum;
    CaleAlarmNode   *pNext;
}CaleAlarmHead;

//month schedule
typedef struct tagCaleMonthSch
{
    WORD        wYear;
    WORD        wMonth;
    WORD        wDay;
    WORD        wWeek;
    BYTE        bFlag;
}CaleMonthSch;

//week schedule node
typedef struct tagCaleWeekSchNode
{
    int         nStart;
    int         nEnd;
    struct tagCaleWeekSchNode *pNext;
}CaleWeekSchNode;

//week schedule head
typedef struct tagCaleWeekSchHead
{
    int         Num;
    struct tagCaleWeekSchNode *pNext;
}CaleWeekSchHead;

//calendar month struct
typedef struct tagCaleMonth
{
    LONG        dwStyle;
    WORD        wID;
    HWND        hwndParent;
    HFONT       hFont;
    int         x;
    int         y;
    int         width;
    int         height;
    
    WORD        MaxYear;
    WORD        MinYear;
    SYSTEMTIME  sysCurTime;

    int nWeekStart;
    int CurSch;
    CaleMonthSch MonthSch[MAX_SCH];
}CaleMonth, *PCaleMonth;

//calendar week struct
typedef struct tagCaleWeek
{
    LONG        dwStyle;
    WORD        wID;
    HWND        hwndParent;
    HFONT       hFont;
    int         x;
    int         y;
    int         width;
    int         height;
    
    WORD        MaxYear;
    WORD        MinYear;
    SYSTEMTIME  sysCurTime;
    SYSTEMTIME  StartTime;
    SYSTEMTIME  EndTime;
    
    int         nWeekStart;
    int         nTimeStart;
    int         CurX;
    int         CurY;
    BYTE        IconRow[MAX_WEEK_LINE];
    CaleWeekSchHead TimeRow[MAX_WEEK_ROW][MAX_WEEK_LINE];
    BYTE        bAnEv;
    //HBITMAP     hMeet;
    HBITMAP     hAnni;
    HBITMAP     hEvent;
    HWND        hScroll;
}CaleWeek, *PCaleWeek;

//calendar day struct
typedef struct tagCaleDay
{
    LONG        dwStyle;
    WORD        wID;
    HWND        hwndParent;
    HFONT       hFont;
    int         x;
    int         y;
    int         width;
    int         height;
    
    WORD        MaxYear;
    WORD        MinYear;
    SYSTEMTIME  sysCurTime;

    HBITMAP     hNewEn; 
    HBITMAP     hMeet;
    HBITMAP     hAnni;
    HBITMAP     hEvent;

    HWND        hCaleList;
    int         CurListNo;
}CaleDay, *PCaleDay;

//day view list node
typedef struct tagCaleDayNode
{
    char        Subject[MAX_CALE_SUBJECT_LEN + 1];
    char        Location[MAX_CALE_LOCATION_LEN + 1];
    SYSTEMTIME  DisTime;
    SYSTEMTIME  CurStartTime;
    SYSTEMTIME  CurEndTime;
    int         CaleType;
    int         itemID;
    struct tagCaleDayNode *pNext;
}CaleDayNode;

//day view list head
typedef struct tagCaleDayHead
{
    int CurNum;
    struct tagCaleDayNode *pNext;
}CaleDayHead;

//delete select list node
typedef struct tagCaleDelNode
{
    char        Subject[MAX_CALE_SUBJECT_LEN + 1];
    int         CaleType;
    int         itemID;
    BOOL        DelFlag;
    struct tagCaleDelNode *pNext;
}CaleDelNode;

//delete select list head
typedef struct tagCaleDelHead
{
    int TotalNum;
    int DelNum;
    struct tagCaleDelNode *pNext;
}CaleDelHead;

//calendar alarm window struct
typedef struct tagCaleAlarm
{
    CaleAlarmHead AlarmHead;
    HBITMAP     hMeet;
    HBITMAP     hAnni;
}CaleAlarm, *PCaleAlarm;

/**********************************CreateData******************/
typedef struct tagCALE_BrowseCreateData
{
    HWND    HwndCaleMonth;              //month view window
    HWND    HwndCaleWeek;               //week view window
    HWND    HwndCaleDay;                //day view window
    
#ifdef _NOKIA_CALE_
    HWND    HwndCaleSpin;               //the date display window on the top-right//nokia style
#endif
    
    HWND    HwndCaleGoto;
    int     CurView;                     //current view
    int     DefaultView;                 //default view(config)
    int     DelType;                     //delete type
    
    BOOL    bMenuChanged;
    HWND    hFrameWnd;
    HMENU   hMenu;

    HINSTANCE hInstance;
}CALE_BROWSECREATEDATA, *PCALE_BROWSECREATEDATA;

typedef struct tagCALE_SettingCreateData
{
    HWND    hFrameWnd;
    HWND    hParent;

    HWND    hDefView;
    HWND    hWeekStart;
    HWND    hDayStart;
    HWND    hSnooze;
    
    HWND    hFocus;

    int     DefDefView;
    int     DefWeekStart;
    int     DefDayStart;
    int     DefSnooze;

    int     CurSpin;
    int     CurItem;
}CALE_SETTINGCREATEDATA, *PCALE_SETTINGCREATEDATA;

typedef struct tagCALE_SnoozeCreateData
{
    HWND    hFrameWnd;
    HWND    hParent;
    HWND    HwndList;
    
    HWND    hFocus;

    HBITMAP hSelect;
    HBITMAP hUnSelect;
    
    int     CurSpin;
    int     CurItem;
}CALE_SNOOZECREATEDATA, *PCALE_SNOOZECREATEDATA;

typedef struct tagCALE_EntryCreateData
{
    HWND    hFrameWnd;
    HWND    hParent;
    HWND    hFocus;
}CALE_ENTRYCREATEDATA, *PCALE_ENTRYCREATEDATA;

typedef struct tagCALE_ViewCreateData
{
    HWND    hFrameWnd;
    HWND    hParent;
    HWND    hFocus;
}CALE_VIEWCREATEDATA, *PCALE_VIEWCREATEDATA;

typedef struct tagCALE_NewCreateData
{
    HWND    hFrameWnd;
    HWND    hParent;
    HWND    hFocus;
}CALE_NEWCREATEDATA, *PCALE_NEWCREATEDATA;

typedef struct tagCALE_JumpCreateData
{
    HWND    hFrameWnd;
    HWND    hParent;
    HWND    hJumpEdit;
    SYSTEMTIME sDatetime;
}CALE_JUMPCREATEDATA, *PCALE_JUMPCREATEDATA;

//extern function
extern HWND CALE_GetFrame(void);
//extern BOOL GetTimeDisplay (SYSTEMTIME st, char* cTime, char* cDate);
