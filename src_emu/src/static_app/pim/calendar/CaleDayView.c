/***************************************************************************
*
*                      Pollex Mobile Platform
*
* Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
*                       All Rights Reserved 
*
* Module   : CaleDayView.c
*
* Purpose  : 
*
\**************************************************************************/

#include "CaleHeader.h"

static HINSTANCE hInstance;

//static HWND hCaleList;
//static int CurListNo;
CaleDayHead CurDayHead;
CaleDayNode CurDayNode;

static WNDPROC OldListBoxProc;

//new sub window
static HWND HwndCaleNewSub;

//delete select window
static HWND HwndCaleDelSel;

static HWND HwndList = NULL;

static LRESULT CALLBACK CaleDayWndProc(HWND hWnd, UINT message,
                                       WPARAM wParam, LPARAM lParam);
static LRESULT CALE_NewListBoxProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static void CALE_CreateDayView(HWND hWnd, CaleDay *pCaleDay);
static void CALE_CreateListBox(HWND hWnd, CaleDay *pCaleDay);
static void CALE_ReleaseDayListNode(CaleDayHead *pHead);
static void CALE_AddDayListNode(CaleDayHead *pHead, CaleDayNode *pTmpNode);
static void CALE_DelDayListNode(CaleDayHead *pHead, int iTmp);
static BOOL CALE_GetDayNodeByInt(CaleDayHead *pHead, CaleDayNode *pTmpNode, int iTmp);
BOOL CALE_GetDaySchedule(CaleDayHead *pHead, CaleDay *pCaleDay);
static void CALE_SetHour(CaleDayHead *pHead, CaleDay *pCaleDay);
static void CALE_SetDefault(CaleDayHead *pHead, CaleDay *pCaleDay);
BOOL CreateNewSubWnd(HWND hwnd);
static LRESULT CaleNewSubWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
BOOL CreateDelSelWnd(HWND hwnd);
static LRESULT CaleDelSelWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static BOOL CALE_CreateDelList(CaleDelHead *pHead);
static void CALE_ReleaseDelListNode(CaleDelHead *pHead);
static void CALE_AddDelListNode(CaleDelHead *pHead, CaleDelNode *pTmpNode);
static void CALE_DelDelListNode(CaleDelHead *pHead, int iTmp);
static BOOL CALE_GetDelNodeByInt(CaleDelHead *pHead, CaleDelNode **pTmpNode, int iTmp);

extern HWND CALE_GetAppWnd(void);
extern int CALE_GetDayStart(void);
extern int CALE_KillAlarm(int CaleType, int nAlmID);
extern int CALE_CmpSysDate(SYSTEMTIME *pFirstDate, SYSTEMTIME *pSecondDate);
extern void CALE_DateSysToChar(SYSTEMTIME *pSys, char *pChar);
extern void CALE_TimeSysToChar(SYSTEMTIME *pSys, char *pChar);
extern int CALE_OpenApi(int CaleEntry);
extern BOOL CALE_CloseApi(int Cal_OpenFile);
extern int CALE_ReadApi(int Cal_Open, int itemID, BYTE *pRecord, int size);
extern BOOL CALE_DelApi(int CaleEntry, int Cal_Open, int itemID, BYTE *pRecord, int size);
extern int CALE_AlmDaysOfMonth(SYSTEMTIME *pDate);
extern BOOL CALE_MonthIncrease(SYSTEMTIME *pSystime);
extern BOOL CALE_YearIncrease(SYSTEMTIME *pSystime);
extern BOOL CALE_DayIncrease(SYSTEMTIME *psystime, unsigned int day);
extern BOOL CALE_DayReduce(SYSTEMTIME *psystime, unsigned int day);
extern BOOL CreateDisplayMeet(HWND hwnd, int recordpt, SYSTEMTIME *caltime, 
                              CaleMeetingNode *setmeet, BOOL bCreate, BOOL bRead);
extern BOOL CreateDisplayAnni(HWND hwnd, int recordpt, SYSTEMTIME *caltime, 
                              CaleAnniNode *setanni, BOOL bCreate, BOOL bRead);
extern BOOL CreateDisplayEvent(HWND hwnd, int recordpt, SYSTEMTIME *caltime, 
                               CaleEventNode *setevent, BOOL bCreate, BOOL bRead);

extern BOOL CALE_EncodeVcalendar(int itemID, BYTE *pRecord);

extern BOOL MAIL_CreateEditInterface(HWND hFrameWindow, char *PSZRECIPIENT, 
                                     char *PSZATTENMENT, char *PSZATTNAME, BOOL bAtt);
extern BOOL APP_EditSMSVcardVcal(HWND hParent,const char* PSZCONTENT,int nContentLen);

//extern BOOL BtSendData(HWND hAppFrameWnd,char* szAppPathName,char* szViewFileName,int iFileType);

BOOL CALE_RegisterDayClass(void* hInst)
{
    WNDCLASS wc;
    
    hInstance = (HINSTANCE)hInst;
    
    wc.style        = 0;
    wc.lpfnWndProc  = CaleDayWndProc;
    wc.cbClsExtra   = 0;
    wc.cbWndExtra   = sizeof(CaleDay);
    wc.hInstance    = NULL;
    wc.hIcon        = NULL;
    wc.hCursor      = NULL;
    wc.hbrBackground= NULL;//(HBRUSH)GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName = NULL;
    wc.lpszClassName= "CALEDAYCLASS";
    
    if(!RegisterClass(&wc))
        return FALSE;
    
    return TRUE;
}

BOOL CALE_UnRegisterDayClass()
{
    UnregisterClass("CALEDAYCLASS", NULL);
    return TRUE;
}

static LRESULT CALLBACK CaleDayWndProc(HWND hWnd, UINT message,
                                         WPARAM wParam, LPARAM lParam)
{
    LRESULT lResult;
    HDC hDC;
    //HWND hwndFocus;
    PCREATESTRUCT pCreateStruct;

    PCaleDay pCaleDay;
    int nDays;
    int nOpen;
    CaleMeetingNode TmpMeeting;
    CaleAnniNode TmpAnni;
    CaleEventNode TmpEvent;
    SYSTEMTIME sEntryTime;
    
    lResult = (LRESULT)TRUE;
    pCaleDay = GetUserData(hWnd);
    
    switch(message)
    {
    case WM_CREATE:
        {
            memset(pCaleDay, 0x0, sizeof(CaleDay));
            pCreateStruct = (LPCREATESTRUCT)lParam;
            
            pCaleDay->dwStyle = pCreateStruct->style;
            pCaleDay->hwndParent = pCreateStruct->hwndParent;
            pCaleDay->wID = (WORD)(DWORD)pCreateStruct->hMenu;
            pCaleDay->x= pCreateStruct->x;
            pCaleDay->y= pCreateStruct->y;
            pCaleDay->width= pCreateStruct->cx;
            pCaleDay->height= pCreateStruct->cy;
            
            pCaleDay->MaxYear = MAX_YEAR;
            pCaleDay->MinYear = MIN_YEAR;
            
            GetLocalTime(&pCaleDay->sysCurTime);
            pCaleDay->hNewEn = LoadImage(NULL, CALE_PIC_NEWENTRY, IMAGE_BITMAP,0, 0, LR_LOADFROMFILE);
            pCaleDay->hMeet = LoadImage(NULL, CALE_PIC_MEETING, IMAGE_BITMAP,0, 0, LR_LOADFROMFILE);
            pCaleDay->hAnni = LoadImage(NULL, CALE_PIC_ANNI, IMAGE_BITMAP,0, 0, LR_LOADFROMFILE);
            pCaleDay->hEvent = LoadImage(NULL, CALE_PIC_EVENT, IMAGE_BITMAP,0, 0, LR_LOADFROMFILE);
            CALE_CreateDayView(hWnd, pCaleDay);
            UpdateWindow(hWnd);
        }
        break;

    case WM_SIZE:
        {
        }
        break;
        
    case WM_PAINT:
        {
            hDC = BeginPaint(hWnd, NULL);
            EndPaint(hWnd, NULL);
        }
        break;
        
    case WM_DESTROY:
        {
            CALE_ReleaseDayListNode(&CurDayHead);
            if (pCaleDay->hNewEn)
            {
                DeleteObject(pCaleDay->hNewEn);
                pCaleDay->hNewEn = NULL;
            }
            if (pCaleDay->hMeet)
            {
                DeleteObject(pCaleDay->hMeet);
                pCaleDay->hMeet = NULL;
            }
            if (pCaleDay->hAnni)
            {
                DeleteObject(pCaleDay->hAnni);
                pCaleDay->hAnni = NULL;
            }
            if (pCaleDay->hEvent)
            {
                DeleteObject(pCaleDay->hEvent);
                pCaleDay->hEvent = NULL;
            }
        }
        break;
        
    case WM_SETFOCUS:
        {
            //DefWindowProc(hWnd, message, wParam, lParam);
            //hwndFocus = GetFocus();
            SetFocus(pCaleDay->hCaleList);
        }
        break;

    case WM_COMMAND:
        {
            switch(LOWORD(wParam))
            {
            case IDM_BUTTON_DELETE:
                {
                    memset(&CurDayNode, 0x0, sizeof(CaleDayNode));
                    pCaleDay->CurListNo = SendMessage(pCaleDay->hCaleList, LB_GETCURSEL, NULL, NULL);
                    if(CALE_GetDayNodeByInt(&CurDayHead, &CurDayNode, pCaleDay->CurListNo) == FALSE)
                    {
                        break;
                    }
                    switch(CurDayNode.CaleType)
                    {
                    case 0:
                        break;
                        
                    case CALE_MEETING:
                        {
                            if((nOpen = CALE_OpenApi(CALE_MEETING)) == CALE_FAIL)
                            {
                                return FALSE;
                            }
                            if(CALE_ReadApi(nOpen, CurDayNode.itemID, (BYTE*)&TmpMeeting, 
                                sizeof(CaleMeetingNode)) == CALE_SUCCESS)
                            {
                                if(TmpMeeting.MeetAlarmFlag)
                                {
                                    CALE_KillAlarm(CALE_MEETING, CurDayNode.itemID);
                                }
                                TmpMeeting.LocalFlag = CALE_UNLOCAL;
                                if(CALE_DelApi(CALE_MEETING, nOpen, CurDayNode.itemID, (BYTE*)&TmpMeeting,
                                    sizeof(CaleMeetingNode)))
                                {
                                    PLXTipsWin(NULL, NULL, NULL, IDP_STRING_DELSUCC, NULL,
                                        Notify_Success, IDP_IME_OK, NULL, WAITTIMEOUT);
                                }
                                else
                                {
                                    PLXTipsWin(NULL, NULL, NULL, IDP_STRING_DELFAIL, NULL,
                                        Notify_Failure, IDP_IME_OK, NULL, WAITTIMEOUT);
                                }
                            }
                            CALE_CloseApi(nOpen);
                        }
                        break;
                        
                    case CALE_ANNI:
                        {
                            if((nOpen = CALE_OpenApi(CALE_ANNI)) == CALE_FAIL)
                            {
                                return FALSE;
                            }
                            if(CALE_ReadApi(nOpen, CurDayNode.itemID, (BYTE*)&TmpAnni, 
                                sizeof(CaleAnniNode)) == CALE_SUCCESS)
                            {
                                if(TmpAnni.AnniAlarmFlag)
                                {
                                    CALE_KillAlarm(CALE_ANNI, CurDayNode.itemID);
                                }
                                TmpAnni.LocalFlag = CALE_UNLOCAL;
                                if(CALE_DelApi(CALE_ANNI, nOpen, CurDayNode.itemID, (BYTE*)&TmpAnni,
                                    sizeof(CaleAnniNode)))
                                {
                                    PLXTipsWin(NULL, NULL, NULL, IDP_STRING_DELSUCC, NULL,
                                        Notify_Success, IDP_IME_OK, NULL, WAITTIMEOUT);
                                }
                                else
                                {
                                    PLXTipsWin(NULL, NULL, NULL, IDP_STRING_DELFAIL, NULL,
                                        Notify_Failure, IDP_IME_OK, NULL, WAITTIMEOUT);
                                }
                            }
                            CALE_CloseApi(nOpen);
                        }
                        break;
                        
                    case CALE_EVENT:
                        {
                            if((nOpen = CALE_OpenApi(CALE_EVENT)) == CALE_FAIL)
                            {
                                return FALSE;
                            }
                            if(CALE_ReadApi(nOpen, CurDayNode.itemID, (BYTE*)&TmpEvent, 
                                sizeof(CaleEventNode)) == CALE_SUCCESS)
                            {
                                TmpEvent.LocalFlag = CALE_UNLOCAL;
                                if(CALE_DelApi(CALE_EVENT, nOpen, CurDayNode.itemID, (BYTE*)&TmpEvent,
                                    sizeof(CaleEventNode)))
                                {
                                    PLXTipsWin(NULL, NULL, NULL, IDP_STRING_DELSUCC, NULL,
                                        Notify_Success, IDP_IME_OK, NULL, WAITTIMEOUT);
                                }
                                else
                                {
                                    PLXTipsWin(NULL, NULL, NULL, IDP_STRING_DELFAIL, NULL,
                                        Notify_Failure, IDP_IME_OK, NULL, WAITTIMEOUT);
                                }
                            }
                            CALE_CloseApi(nOpen);
                        }
                        break;
                        
                    default:
                        break;
                    }
                    CALE_CreateListBox(hWnd, pCaleDay);
                }
                break;
    
            case IDM_BUTTON_BYSMS:
            case IDM_BUTTON_BYMMS:
            case IDM_BUTTON_BYEMAIL:
            case IDM_BUTTON_BYBLUE:
                {
                    PCALE_BROWSECREATEDATA pBrowseCreate;
                    int hFile;
                    struct stat SourceStat;
                    BYTE *bData;

                    pBrowseCreate = GetUserData(pCaleDay->hwndParent);
                    
                    memset(&CurDayNode, 0x0, sizeof(CaleDayNode));
                    pCaleDay->CurListNo = SendMessage(pCaleDay->hCaleList, LB_GETCURSEL, NULL, NULL);
                    if(CALE_GetDayNodeByInt(&CurDayHead, &CurDayNode, pCaleDay->CurListNo) == FALSE)
                    {
                        break;
                    }
                    switch(CurDayNode.CaleType)
                    {
                    case 0:
                        break;
                        
                    case CALE_MEETING:
                        {
                            if((nOpen = CALE_OpenApi(CALE_MEETING)) == CALE_FAIL)
                            {
                                return FALSE;
                            }
                            if(CALE_ReadApi(nOpen, CurDayNode.itemID, (BYTE*)&TmpMeeting, 
                                sizeof(CaleMeetingNode)) == CALE_SUCCESS)
                            {
                                CALE_EncodeVcalendar(CALE_MEETING, (BYTE *)&TmpMeeting);
                            }
                            CALE_CloseApi(nOpen);
                        }
                        break;
                        
                    case CALE_ANNI:
                        {
                            if((nOpen = CALE_OpenApi(CALE_ANNI)) == CALE_FAIL)
                            {
                                return FALSE;
                            }
                            if(CALE_ReadApi(nOpen, CurDayNode.itemID, (BYTE*)&TmpAnni, 
                                sizeof(CaleAnniNode)) == CALE_SUCCESS)
                            {
                                CALE_EncodeVcalendar(CALE_ANNI, (BYTE *)&TmpAnni);
                            }
                            CALE_CloseApi(nOpen);
                        }
                        break;
                        
                    case CALE_EVENT:
                        {
                            if((nOpen = CALE_OpenApi(CALE_EVENT)) == CALE_FAIL)
                            {
                                return FALSE;
                            }
                            if(CALE_ReadApi(nOpen, CurDayNode.itemID, (BYTE*)&TmpEvent, 
                                sizeof(CaleEventNode)) == CALE_SUCCESS)
                            {
                                CALE_EncodeVcalendar(CALE_EVENT, (BYTE *)&TmpEvent);
                            }
                            CALE_CloseApi(nOpen);
                        }
                        break;
                        
                    default:
                        break;
                    }
                    switch(LOWORD(wParam))
                    {
                    case IDM_BUTTON_BYSMS:
                        {
                            if(stat(CALE_VCALE_SEND_FILE, &SourceStat) == -1)
                            {
                                return FALSE;
                            }
                            bData = NULL;
                            bData = (BYTE *)malloc(SourceStat.st_size + 1);
                            if(bData == NULL)
                            {
                                return FALSE;
                            }

							memset(bData, 0, SourceStat.st_size + 1);
                            hFile = open(CALE_VCALE_SEND_FILE, O_RDONLY);
                            if(hFile < 0)
                            {
                                GARY_FREE(bData);
                                return FALSE;
                            }
                            read(hFile, bData, SourceStat.st_size);
                            close(hFile);

                            APP_EditSMSVcardVcal(pBrowseCreate->hFrameWnd, bData, SourceStat.st_size);
                            
                            GARY_FREE(bData);
                        }
                        break;

                    case IDM_BUTTON_BYMMS:
                        {
							APP_EditMMS(pBrowseCreate->hFrameWnd,hWnd, 0, MMS_CALLEDIT_VCAL, CALE_VCALE_SEND_FILE);	//*********
                        }
                        break;

                    case IDM_BUTTON_BYEMAIL:
                        {
                            MAIL_CreateEditInterface(pBrowseCreate->hFrameWnd, NULL, 
                                CALE_VCALE_SEND_FILE, "calendar.vcs", 1);
                        }
                        break;

                    case IDM_BUTTON_BYBLUE:
                        {
							BtSendData(pBrowseCreate->hFrameWnd, CALE_VCALE_SEND_FILE, NULL, BTCARDCALENDER);	//**********
                        }
                        break;

                    default:
                        break;
                    }
                }
                break;

            case ID_LISTBOX_DAY:
                {
                    if((HIWORD(wParam) == LBN_SELCHANGE) || (HIWORD(wParam) == LBN_SETFOCUS))
                    {
                        int CurList;

                        CurList = SendMessage(pCaleDay->hCaleList, LB_GETCURSEL, 0, 0);
                        if(CurList == 0)
                        {
                            SendMessage(CALE_GetFrame(), PWM_SETBUTTONTEXT, 1, (LPARAM)"");
                            SendMessage(CALE_GetFrame(), PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_SELECT);
                        }
                        else
                        {
                            SendMessage(CALE_GetFrame(), PWM_SETBUTTONTEXT, 1, (LPARAM)IDP_IME_OPEN);
                            SendMessage(CALE_GetFrame(), PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_OPTIONS);
                        }
                    }
                }
                break;

            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;

    case WM_KEYDOWN:
        {
            switch(LOWORD(wParam))
            {
            case VK_LEFT:
                {
#ifdef _NOKIA_CALE_
                    CALE_DayReduce(&pCaleDay->sysCurTime, 1);
                    CALE_CreateListBox(hWnd, pCaleDay);
                    SendMessage(pCaleDay->hwndParent, WM_COMMAND, 
                        MAKEWPARAM(pCaleDay->wID, CALE_CALC_CHANGED), (LPARAM)(&pCaleDay->sysCurTime));
#endif
#ifdef _BENEFON_CALE_
                    return DefWindowProc(hWnd, message, wParam, lParam);
#endif
                }
                break;

            case VK_RIGHT:
                {
#ifdef _NOKIA_CALE_
                    CALE_DayIncrease(&pCaleDay->sysCurTime, 1);
                    CALE_CreateListBox(hWnd, pCaleDay);
                    SendMessage(pCaleDay->hwndParent, WM_COMMAND, 
                        MAKEWPARAM(pCaleDay->wID, CALE_CALC_CHANGED), (LPARAM)(&pCaleDay->sysCurTime));
#endif
#ifdef _BENEFON_CALE_
                    return DefWindowProc(hWnd, message, wParam, lParam);
#endif
                }
                break;

            case VK_F5:
                {
                    memset(&CurDayNode, 0x0, sizeof(CaleDayNode));
                    pCaleDay->CurListNo = SendMessage(pCaleDay->hCaleList, LB_GETCURSEL, NULL, NULL);
                    if(CALE_GetDayNodeByInt(&CurDayHead, &CurDayNode, pCaleDay->CurListNo) == FALSE)
                    {
                        break;
                    }

                    sEntryTime.wYear = pCaleDay->sysCurTime.wYear;
                    sEntryTime.wMonth = pCaleDay->sysCurTime.wMonth;
                    sEntryTime.wDay = pCaleDay->sysCurTime.wDay;
                    sEntryTime.wHour = CALE_GetDayStart();
                    sEntryTime.wMinute = 0;
                    switch(CurDayNode.CaleType)
                    {
                    case 0:
                        {
                            CreateNewSubWnd(pCaleDay->hwndParent);
                        }
                    	break;

                    default:
                        {
                            SendMessage(pCaleDay->hwndParent, WM_KEYDOWN, wParam, lParam);
                        }
                        break;
                    }
                }
                break;

            case VK_RETURN:
                {
                    SYSTEMTIME temp;

                    GetLocalTime(&temp);
                    memset(&CurDayNode, 0x0, sizeof(CaleDayNode));
                    pCaleDay->CurListNo = SendMessage(pCaleDay->hCaleList, LB_GETCURSEL, NULL, NULL);
                    if(CALE_GetDayNodeByInt(&CurDayHead, &CurDayNode, pCaleDay->CurListNo) == FALSE)
                    {
                        break;
                    }
                    
                    sEntryTime.wYear = pCaleDay->sysCurTime.wYear;
                    sEntryTime.wMonth = pCaleDay->sysCurTime.wMonth;
                    sEntryTime.wDay = pCaleDay->sysCurTime.wDay;
                    sEntryTime.wHour = temp.wHour + 1;
                    sEntryTime.wMinute = 0;
                    switch(CurDayNode.CaleType)
                    {
                    case 0:
                        break;
                        
                    case CALE_MEETING:
                        {
                            if((nOpen = CALE_OpenApi(CALE_MEETING)) == CALE_FAIL)
                            {
                                return FALSE;
                            }
                            if(CALE_ReadApi(nOpen, CurDayNode.itemID, (BYTE*)&TmpMeeting, 
                                sizeof(CaleMeetingNode)) == CALE_SUCCESS)
                            {
                                CALE_CloseApi(nOpen);
                                CreateDisplayMeet(hWnd, CurDayNode.itemID, &sEntryTime, &TmpMeeting, 0, HIWORD(wParam));
                                break;
                            }
                            CALE_CloseApi(nOpen);
                        }
                        break;
                        
                    case CALE_ANNI:
                        {
                            if((nOpen = CALE_OpenApi(CALE_ANNI)) == CALE_FAIL)
                            {
                                return FALSE;
                            }
                            if(CALE_ReadApi(nOpen, CurDayNode.itemID, (BYTE*)&TmpAnni, 
                                sizeof(CaleAnniNode)) == CALE_SUCCESS)
                            {
                                CALE_CloseApi(nOpen);
                                CreateDisplayAnni(hWnd, CurDayNode.itemID, &sEntryTime, &TmpAnni, 0, HIWORD(wParam));
                                break;
                            }
                            CALE_CloseApi(nOpen);
                        }
                        break;
                        
                    case CALE_EVENT:
                        {
                            if((nOpen = CALE_OpenApi(CALE_EVENT)) == CALE_FAIL)
                            {
                                return FALSE;
                            }
                            if(CALE_ReadApi(nOpen, CurDayNode.itemID, (BYTE*)&TmpEvent, 
                                sizeof(CaleEventNode)) == CALE_SUCCESS)
                            {
                                CALE_CloseApi(nOpen);
                                CreateDisplayEvent(hWnd, CurDayNode.itemID, &sEntryTime, &TmpEvent, 0, HIWORD(wParam));//CurDayNode.CurTime
                                break;
                            }
                            CALE_CloseApi(nOpen);
                        }
                        break;
                        
                    default:
                        break;
                    }
                }
                break;

            case VK_F10:
                {
                    SendMessage(pCaleDay->hwndParent, WM_KEYDOWN, wParam, lParam);
                }
                break;
                
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;

    case CALE_CALC_SETCUR:
        {
            if((LPSYSTEMTIME)lParam != NULL)
            {
                if (((LPSYSTEMTIME)lParam)->wYear < MIN_YEAR || 
                    ((LPSYSTEMTIME)lParam)->wYear > MAX_YEAR)
                    return FALSE;
                
                if (((LPSYSTEMTIME)lParam)->wMonth < 1 || 
                    ((LPSYSTEMTIME)lParam)->wMonth > 12)
                    return FALSE;
                
                nDays = CALE_AlmDaysOfMonth((LPSYSTEMTIME)lParam);
                if (((LPSYSTEMTIME)lParam)->wDay < 1 || 
                    ((LPSYSTEMTIME)lParam)->wDay > nDays)
                    return FALSE;
                
                pCaleDay->sysCurTime.wYear = ((LPSYSTEMTIME)lParam)->wYear;
                pCaleDay->sysCurTime.wMonth = ((LPSYSTEMTIME)lParam)->wMonth;
                pCaleDay->sysCurTime.wDay = ((LPSYSTEMTIME)lParam)->wDay;
                pCaleDay->sysCurTime.wDayOfWeek = ((LPSYSTEMTIME)lParam)->wDayOfWeek;
                
                CALE_CreateListBox(hWnd, pCaleDay);
                SendMessage(pCaleDay->hwndParent, WM_COMMAND, 
                    MAKEWPARAM(pCaleDay->wID, CALE_CALC_CHANGED), (LPARAM)(&pCaleDay->sysCurTime));
            }//end if((LPSYSTEMTIME)lParam != NULL)
        }
        break;
        
    case CALE_CALC_GETCUR:
        {
            ((LPSYSTEMTIME)lParam)->wYear = pCaleDay->sysCurTime.wYear;
            ((LPSYSTEMTIME)lParam)->wMonth = pCaleDay->sysCurTime.wMonth;
            ((LPSYSTEMTIME)lParam)->wDay = pCaleDay->sysCurTime.wDay;
            ((LPSYSTEMTIME)lParam)->wDayOfWeek = pCaleDay->sysCurTime.wDayOfWeek;
        }
        break;

    case CALE_CALC_GETLISTNO:
        {
            memset(&CurDayNode, 0x0, sizeof(CaleDayNode));
            pCaleDay->CurListNo = SendMessage(pCaleDay->hCaleList, LB_GETCURSEL, NULL, NULL);
            if(CALE_GetDayNodeByInt(&CurDayHead, &CurDayNode, pCaleDay->CurListNo) == FALSE)
            {
                break;
            }
            if(CurDayNode.CaleType == 0)
            {
                return FALSE;
            }
            else
            {
                return TRUE;
            }
        }
        break;

    case CALE_NEW_MEETING:
        {
            SYSTEMTIME temp;

            GetLocalTime(&temp);
            sEntryTime.wYear = pCaleDay->sysCurTime.wYear;
            sEntryTime.wMonth = pCaleDay->sysCurTime.wMonth;
            sEntryTime.wDay = pCaleDay->sysCurTime.wDay;
            sEntryTime.wHour = temp.wHour + 1;
            sEntryTime.wMinute = 0;

            CreateDisplayMeet(hWnd, 0, &sEntryTime, 0, 1, 1);
        }
        break;

    case CALE_NEW_ANNI:
        {
            SYSTEMTIME temp;
            
            GetLocalTime(&temp);
            sEntryTime.wYear = pCaleDay->sysCurTime.wYear;
            sEntryTime.wMonth = pCaleDay->sysCurTime.wMonth;
            sEntryTime.wDay = pCaleDay->sysCurTime.wDay;
            sEntryTime.wHour = temp.wHour + 1;
            sEntryTime.wMinute = 0;

            CreateDisplayAnni(hWnd, 0, &sEntryTime, 0, 1, 1);
        }
        break;

    case CALE_NEW_EVENT:
        {
            SYSTEMTIME temp;
            
            GetLocalTime(&temp);
            sEntryTime.wYear = pCaleDay->sysCurTime.wYear;
            sEntryTime.wMonth = pCaleDay->sysCurTime.wMonth;
            sEntryTime.wDay = pCaleDay->sysCurTime.wDay;
            sEntryTime.wHour = temp.wHour + 1;
            sEntryTime.wMinute = 0;

            CreateDisplayEvent(hWnd, 0, &sEntryTime, 0, 1, 1);
        }
        break;

    case CALE_APP_NEWC:
        {
            switch(LOWORD(wParam))
            {
            case CALE_MEETING:
            case CALE_ANNI:
            case CALE_EVENT:
                {
					int i;
					CaleDayNode DayNode;

                    CALE_CreateListBox(hWnd, pCaleDay);
			//////////////////////////////////////////////////////////////////////////
					
					if(NULL == lParam)
						break;

                    i = SendMessage(pCaleDay->hCaleList, LB_GETCOUNT, NULL, NULL)-1;
					while(i > 0)
					{
						if(CALE_GetDayNodeByInt(&CurDayHead, &DayNode, i) == FALSE)
							break;
						
						if(LOWORD(wParam) == DayNode.CaleType && HIWORD(wParam) == DayNode.itemID)
						{
							SendMessage(pCaleDay->hCaleList, LB_SETCURSEL, i, 0);
							break;
						}
						i --;
					}
			//////////////////////////////////////////////////////////////////////////
                }
                break;

            default:
                lResult = DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;

    default:
        lResult = DefWindowProc(hWnd, message, wParam, lParam);
    }
    return lResult;
}

static void CALE_CreateDayView(HWND hWnd, CaleDay *pCaleDay)
{
    RECT rcClient;

    GetClientRect(hWnd, &rcClient);
    //InflateRect(&rcClient, -2, -2);

    pCaleDay->hCaleList = CreateWindow(
        "LISTBOX",
        "",
        WS_CHILD | WS_VISIBLE | WS_VSCROLL | LBS_BITMAP | LBS_MULTILINE,
        rcClient.left,
        rcClient.top,
        rcClient.right-rcClient.left,
        rcClient.bottom-rcClient.top,
        hWnd,
        (HMENU)ID_LISTBOX_DAY,
        NULL, NULL);
    
#ifdef _NOKIA_CALE_
    OldListBoxProc = (WNDPROC)GetWindowLong(pCaleDay->hCaleList, GWL_WNDPROC);
    SetWindowLong(pCaleDay->hCaleList, GWL_WNDPROC, (long)CALE_NewListBoxProc);
#endif
    CALE_CreateListBox(hWnd, pCaleDay);

    return;
}

static LRESULT CALE_NewListBoxProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT	lResult = TRUE;
    
    switch(message)
    {
    case WM_KEYDOWN:
        {
            switch(wParam)
            {
            case VK_LEFT:
            case VK_RIGHT:
                SendMessage(GetParent(hWnd), WM_KEYDOWN, wParam, lParam);
                break;
            default:
                lResult = OldListBoxProc(hWnd, message, wParam, lParam);
                break;
            }
        }
        break;        
        
    default:
        lResult = OldListBoxProc(hWnd, message, wParam, lParam);
        break;
    }
    
    return lResult;
}

static void CALE_CreateListBox(HWND hWnd, CaleDay *pCaleDay)
{
    CaleDayNode *pTmp;
    //char cHour[7];
    char cDate[11];
    char cDisString[32];
    int i;

    CALE_ReleaseDayListNode(&CurDayHead);
    //CALE_SetHour(&CurDayHead, pCaleDay);
    CALE_SetDefault(&CurDayHead, pCaleDay);
	CALE_GetDaySchedule(&CurDayHead, pCaleDay);
    SendMessage(pCaleDay->hCaleList, LB_RESETCONTENT, 0, 0);
    
    i = 0;
    pTmp = CurDayHead.pNext;
    while(pTmp)
    {
        memset(cDisString, 0x0, 32);
        memset(cDate, 0x0, 11);
        
        if(pTmp->CaleType == 0)
        {
            SendMessage(pCaleDay->hCaleList, LB_ADDSTRING, NULL, (LPARAM)IDP_IME_NEWENTRY);
            SendMessage(pCaleDay->hCaleList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, i), (LPARAM)pCaleDay->hNewEn);
            SendMessage(pCaleDay->hCaleList, LB_SETAUXTEXT, MAKEWPARAM(i, -1), NULL);
        }
        else if(pTmp->CaleType == CALE_MEETING)
        {
		//	CALE_TimeSysToChar(&pTmp->CurStartTime, cDate);
			CALE_GetFormatedTime(cDate, (pTmp->CurStartTime).wHour, (pTmp->CurStartTime).wMinute);
            strcpy(cDisString, cDate);
            strcat(cDisString, " ");
            strcat(cDisString, pTmp->Location);
            SendMessage(pCaleDay->hCaleList, LB_ADDSTRING, NULL, (LPARAM)pTmp->Subject);
            SendMessage(pCaleDay->hCaleList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, i), (LPARAM)pCaleDay->hMeet);
            SendMessage(pCaleDay->hCaleList, LB_SETAUXTEXT, MAKEWPARAM(i, -1), (LPARAM)cDisString);
        }
        else if(pTmp->CaleType == CALE_ANNI)
        {
		//	CALE_DateSysToChar(&pTmp->DisTime, cDate);
			CALE_GetFormatedDate(&(pTmp->DisTime), cDate);
            strcpy(cDisString, cDate);
            SendMessage(pCaleDay->hCaleList, LB_ADDSTRING, NULL, (LPARAM)pTmp->Subject);
            SendMessage(pCaleDay->hCaleList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, i), (LPARAM)pCaleDay->hAnni);
            SendMessage(pCaleDay->hCaleList, LB_SETAUXTEXT, MAKEWPARAM(i, -1), (LPARAM)cDisString);
        }
        else if(pTmp->CaleType == CALE_EVENT)
        {
		//	CALE_DateSysToChar(&pTmp->CurStartTime, cDate);
			CALE_GetFormatedDate(&(pTmp->CurStartTime), cDate);
            strcpy(cDisString, cDate);
		//	CALE_DateSysToChar(&pTmp->CurEndTime, cDate);
			CALE_GetFormatedDate(&(pTmp->CurEndTime), cDate);
            strcat(cDisString, "-");
            strcat(cDisString, cDate);
            SendMessage(pCaleDay->hCaleList, LB_ADDSTRING, NULL, (LPARAM)pTmp->Subject);
            SendMessage(pCaleDay->hCaleList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, i), (LPARAM)pCaleDay->hEvent);
            SendMessage(pCaleDay->hCaleList, LB_SETAUXTEXT, MAKEWPARAM(i, -1), (LPARAM)cDisString);
        }
        i ++;
        pTmp = pTmp->pNext;
    }
    SendMessage(pCaleDay->hCaleList, LB_SETCURSEL, 0, 0);
    
    return;
}

static void CALE_ReleaseDayListNode(CaleDayHead *pHead)
{
    CaleDayNode *pTemp;
    
    pTemp = pHead->pNext;
    while (pTemp)
    {
        pHead->pNext = pTemp->pNext;
        free(pTemp);
        pTemp = NULL;
        pTemp = pHead->pNext;
    }
    pHead->CurNum = 0;
    pHead->pNext = NULL;
    
    return;
}

static void CALE_AddDayListNode(CaleDayHead *pHead, CaleDayNode *pTmpNode)
{
    CaleDayNode *pTemp;
    CaleDayNode *pPre;
    int CmpRes;
    
    pTmpNode->pNext = NULL;

    if(pHead->pNext == NULL)
    {
        pHead->pNext = pTmpNode;
    }
    else
    {
        pTemp = pHead->pNext;
        CmpRes = CALE_CmpSysDate(&pTemp->CurStartTime, &pTmpNode->CurStartTime);
        if(((CmpRes == Late) || (pTmpNode->CaleType > pTemp->CaleType)) && (pTemp->CaleType != 0))
        {
            pTmpNode->pNext = pTemp;
            pHead->pNext = pTmpNode;
            pHead->CurNum ++;
            return;
        }
        else
        {
            pPre = pTemp;
            pTemp = pPre->pNext;
            while (pTemp)
            {
                CmpRes = CALE_CmpSysDate(&pTemp->CurStartTime, &pTmpNode->CurStartTime);
                if(((CmpRes == Late) || (pTmpNode->CaleType > pTemp->CaleType)) && (pTemp->CaleType != 0))
                {
                    pTmpNode->pNext = pTemp;
                    pPre->pNext = pTmpNode;
                    pHead->CurNum ++;
                    return;
                }
                pPre = pTemp;
                pTemp = pPre->pNext;
            }
            pTmpNode->pNext = NULL;
            pPre->pNext = pTmpNode;
            pHead->CurNum ++;
        }
    }
    return;
}

static void CALE_DelDayListNode(CaleDayHead *pHead, int iTmp)
{
    CaleDayNode *pTemp;
    CaleDayNode *pPre;
    int i;

    if(iTmp > (pHead->CurNum - 1))
        return;
    
    if(pHead->pNext == NULL)
        return;

    pTemp = pHead->pNext;
    if(iTmp == 0)
    {
        pHead->pNext = pTemp->pNext;
        free(pTemp);
        pTemp = NULL;
    }
    else
    {
        for(i = 1; i < iTmp; i ++)
        {
            pPre = pTemp;
            pTemp = pPre->pNext;
        }
        pPre->pNext = pTemp->pNext;
        free(pTemp);
        pTemp = NULL;
    }
    pHead->CurNum --;

    return;
}

static BOOL CALE_GetDayNodeByInt(CaleDayHead *pHead, CaleDayNode *pTmpNode, int iTmp)
{
    int i;
    CaleDayNode *pTemp;
    
    if(iTmp > (pHead->CurNum - 1))
        return FALSE;
    
    if(pHead->pNext == NULL)
        return FALSE;
    
    pTemp = pHead->pNext;
    for(i = 0; i < iTmp; i ++)
    {
        pTemp = pTemp->pNext;
    }
    memcpy(pTmpNode, pTemp, sizeof(CaleDayNode));
    
    return TRUE;
}

BOOL CALE_GetDaySchedule(CaleDayHead *pHead, CaleDay *pCaleDay)
{
    SYSTEMTIME StartSys;
    SYSTEMTIME EndSys;
    SYSTEMTIME CurDateStart;
    SYSTEMTIME CurDateEnd;
    
    DWORD nItemID = 0;
    int CmpRes;
    int outday;
    int iOpen;

    CaleMeetingNode CurMeetDate;
    CaleAnniNode CurAnniDate;
    CaleEventNode CurEventDate;
    CaleDayNode *pCurNode;

    StartSys.wYear = pCaleDay->sysCurTime.wYear;
    StartSys.wMonth = pCaleDay->sysCurTime.wMonth;
    StartSys.wDay = pCaleDay->sysCurTime.wDay;
    StartSys.wHour = 0;
    StartSys.wMinute = 0;

    EndSys.wYear = pCaleDay->sysCurTime.wYear;
    EndSys.wMonth = pCaleDay->sysCurTime.wMonth;
    EndSys.wDay = pCaleDay->sysCurTime.wDay;
    EndSys.wHour = 23;
    EndSys.wMinute = 59;

    if((iOpen = CALE_OpenApi(CALE_MEETING)) == CALE_FAIL)
    {
        return FALSE;
    }
    memset(&CurMeetDate, 0x0, sizeof(CaleMeetingNode));
    while((nItemID = CALE_ReadApi(iOpen, -1, (BYTE*)&CurMeetDate, 
        sizeof(CaleMeetingNode))) != CALE_FAIL)
    {
        if(CurMeetDate.LocalFlag == CALE_LOCAL)
        {
            CurDateStart.wYear = CurMeetDate.MeetStartYear;
            CurDateStart.wMonth = CurMeetDate.MeetStartMonth;
            CurDateStart.wDay = CurMeetDate.MeetStartDay;
            CurDateStart.wHour = CurMeetDate.MeetStartHour;
            CurDateStart.wMinute = CurMeetDate.MeetStartMinute;

			CmpRes = CALE_CmpSysDate(&CurDateStart, &EndSys);
			if(CmpRes == Late)
			{
				continue;
			}
			
            CurDateEnd.wYear = CurMeetDate.MeetEndYear;
            CurDateEnd.wMonth = CurMeetDate.MeetEndMonth;
            CurDateEnd.wDay = CurMeetDate.MeetEndDay;
            CurDateEnd.wHour = 23;
            CurDateEnd.wMinute = 59;
            
			CmpRes = CALE_CmpSysDate(&CurDateStart, &CurDateEnd);
			if(CmpRes == Late)
			{
				continue;
			}
			
            if(CurMeetDate.MeetRepeatFlag == 0 || CurMeetDate.MeetRepeatStyle == CALE_REP_NONE)
            {
                CmpRes = CALE_CmpSysDate(&CurDateEnd, &StartSys);
                if(CmpRes == Early)
                {
                    continue;
                }

                pCurNode = (CaleDayNode *)malloc(sizeof(CaleDayNode));
                if(pCurNode == NULL)
                {
                    CALE_CloseApi(iOpen);
                    return FALSE;
                }
                memset(pCurNode, 0x0, sizeof(CaleDayNode));
                pCurNode->CaleType = CALE_MEETING;
                pCurNode->itemID = nItemID;
                sprintf(pCurNode->Subject, "%s", CurMeetDate.MeetSubject);
                sprintf(pCurNode->Location, "%s", CurMeetDate.MeetLocation);
                //pCurNode->CurStartTime.wYear = StartSys.wYear;
                //pCurNode->CurStartTime.wMonth = StartSys.wMonth;
                //pCurNode->CurStartTime.wDay = StartSys.wDay;
                //pCurNode->CurStartTime.wHour = CurMeetDate.MeetStartHour;
                //pCurNode->CurStartTime.wMinute = CurMeetDate.MeetStartMinute;

                pCurNode->CurStartTime.wYear = CurMeetDate.MeetStartYear;
                pCurNode->CurStartTime.wMonth = CurMeetDate.MeetStartMonth;
                pCurNode->CurStartTime.wDay = CurMeetDate.MeetStartDay;
                pCurNode->CurStartTime.wHour = CurMeetDate.MeetStartHour;
                pCurNode->CurStartTime.wMinute = CurMeetDate.MeetStartMinute;
                
                pCurNode->CurEndTime.wYear = CurMeetDate.MeetEndYear;
                pCurNode->CurEndTime.wMonth = CurMeetDate.MeetEndMonth;
                pCurNode->CurEndTime.wDay = CurMeetDate.MeetEndDay;
                pCurNode->CurEndTime.wHour = CurMeetDate.MeetEndHour;
                pCurNode->CurEndTime.wMinute = CurMeetDate.MeetEndMinute;
                CALE_AddDayListNode(pHead, pCurNode);
            }
            else
            {

                switch(CurMeetDate.MeetRepeatStyle)
                {
                case CALE_REP_NONE:
                	break;

                case CALE_REP_DAY:
                    {
                        pCurNode = (CaleDayNode *)malloc(sizeof(CaleDayNode));
                        if(pCurNode == NULL)
                        {
                            CALE_CloseApi(iOpen);
                            CALE_ReleaseDayListNode(pHead);
                            return FALSE;
                        }
                        memset(pCurNode, 0x0, sizeof(CaleDayNode));
                        pCurNode->CaleType = CALE_MEETING;
                        pCurNode->itemID = nItemID;
                        sprintf(pCurNode->Subject, "%s", CurMeetDate.MeetSubject);
                        sprintf(pCurNode->Location, "%s", CurMeetDate.MeetLocation);
                        //pCurNode->CurTime.wYear = StartSys.wYear;
                        //pCurNode->CurTime.wMonth = StartSys.wMonth;
                        //pCurNode->CurTime.wDay = StartSys.wDay;
                        //pCurNode->CurTime.wHour = CurMeetDate.MeetStartHour;
                        //pCurNode->CurTime.wMinute = CurMeetDate.MeetStartMinute;

                        pCurNode->CurStartTime.wYear = CurMeetDate.MeetStartYear;
                        pCurNode->CurStartTime.wMonth = CurMeetDate.MeetStartMonth;
                        pCurNode->CurStartTime.wDay = CurMeetDate.MeetStartDay;
                        pCurNode->CurStartTime.wHour = CurMeetDate.MeetStartHour;
                        pCurNode->CurStartTime.wMinute = CurMeetDate.MeetStartMinute;
                        
                        pCurNode->CurEndTime.wYear = CurMeetDate.MeetEndYear;
                        pCurNode->CurEndTime.wMonth = CurMeetDate.MeetEndMonth;
                        pCurNode->CurEndTime.wDay = CurMeetDate.MeetEndDay;
                        pCurNode->CurEndTime.wHour = CurMeetDate.MeetEndHour;
                        pCurNode->CurEndTime.wMinute = CurMeetDate.MeetEndMinute;
                        CALE_AddDayListNode(pHead, pCurNode);
                    }
                	break;

                case CALE_REP_WEEK:
                    {
						CmpRes = TRUE;
						while(Early == CALE_CmpSysDate(&CurDateEnd, &StartSys))
						{
							CmpRes = CALE_DayIncrease(&CurDateStart, 7);
							CmpRes = CALE_DayIncrease(&CurDateEnd, 7);
							if(CmpRes == FALSE)
							{
								break;
							}
						}
                        if(CmpRes == FALSE)
                        {
                            continue;
                        }
                        CmpRes = CALE_CmpSysDate(&CurDateStart, &EndSys);
                        if(CmpRes == Late)
                        {
                            continue;
                        }

                        pCurNode = (CaleDayNode *)malloc(sizeof(CaleDayNode));
                        if(pCurNode == NULL)
                        {
                            CALE_CloseApi(iOpen);
                            CALE_ReleaseDayListNode(pHead);
                            return FALSE;
                        }
                        memset(pCurNode, 0x0, sizeof(CaleDayNode));
                        pCurNode->CaleType = CALE_MEETING;
                        pCurNode->itemID = nItemID;
                        sprintf(pCurNode->Subject, "%s", CurMeetDate.MeetSubject);
                        sprintf(pCurNode->Location, "%s", CurMeetDate.MeetLocation);

                        pCurNode->CurStartTime.wYear = CurMeetDate.MeetStartYear;
                        pCurNode->CurStartTime.wMonth = CurMeetDate.MeetStartMonth;
                        pCurNode->CurStartTime.wDay = CurMeetDate.MeetStartDay;
                        pCurNode->CurStartTime.wHour = CurMeetDate.MeetStartHour;
                        pCurNode->CurStartTime.wMinute = CurMeetDate.MeetStartMinute;
                        
                        pCurNode->CurEndTime.wYear = CurMeetDate.MeetEndYear;
                        pCurNode->CurEndTime.wMonth = CurMeetDate.MeetEndMonth;
                        pCurNode->CurEndTime.wDay = CurMeetDate.MeetEndDay;
                        pCurNode->CurEndTime.wHour = CurMeetDate.MeetEndHour;
                        pCurNode->CurEndTime.wMinute = CurMeetDate.MeetEndMinute;
                        CALE_AddDayListNode(pHead, pCurNode);
                    }
                    break;

                case CALE_REP_TWOWEEK:
                    {
						CmpRes = TRUE;
                        while(Early == CALE_CmpSysDate(&CurDateEnd, &StartSys))
						{
							CmpRes = CALE_DayIncrease(&CurDateStart, 14);
							CmpRes = CALE_DayIncrease(&CurDateEnd, 14);
							if(CmpRes == FALSE)
							{
								break;
							}
                        }
                        if(CmpRes == FALSE)
                        {
                            continue;
                        }
                        CmpRes = CALE_CmpSysDate(&CurDateStart, &EndSys);
                        if(CmpRes == Late)
                        {
                            continue;
                        }
                        
                        pCurNode = (CaleDayNode *)malloc(sizeof(CaleDayNode));
                        if(pCurNode == NULL)
                        {
                            CALE_CloseApi(iOpen);
                            CALE_ReleaseDayListNode(pHead);
                            return FALSE;
                        }
                        memset(pCurNode, 0x0, sizeof(CaleDayNode));
                        pCurNode->CaleType = CALE_MEETING;
                        pCurNode->itemID = nItemID;
                        sprintf(pCurNode->Subject, "%s", CurMeetDate.MeetSubject);
                        sprintf(pCurNode->Location, "%s", CurMeetDate.MeetLocation);

                        pCurNode->CurStartTime.wYear = CurMeetDate.MeetStartYear;
                        pCurNode->CurStartTime.wMonth = CurMeetDate.MeetStartMonth;
                        pCurNode->CurStartTime.wDay = CurMeetDate.MeetStartDay;
                        pCurNode->CurStartTime.wHour = CurMeetDate.MeetStartHour;
                        pCurNode->CurStartTime.wMinute = CurMeetDate.MeetStartMinute;
                        
                        pCurNode->CurEndTime.wYear = CurMeetDate.MeetEndYear;
                        pCurNode->CurEndTime.wMonth = CurMeetDate.MeetEndMonth;
                        pCurNode->CurEndTime.wDay = CurMeetDate.MeetEndDay;
                        pCurNode->CurEndTime.wHour = CurMeetDate.MeetEndHour;
                        pCurNode->CurEndTime.wMinute = CurMeetDate.MeetEndMinute;
                        CALE_AddDayListNode(pHead, pCurNode);
                    }
                    break;

                case CALE_REP_MONTH:
                    {
						CmpRes = TRUE;
						while(Early == CALE_CmpSysDate(&CurDateEnd, &StartSys))
						{
						   CmpRes = CALE_MonthIncrease(&CurDateStart);
						   CmpRes = CALE_MonthIncrease(&CurDateEnd);
						   if(CmpRes == FALSE)
						   {
							   break;
						   }
                        }
                        if(CmpRes == FALSE)
                        {
                            continue;
                        }

                        CmpRes = CALE_CmpSysDate(&CurDateStart, &EndSys);
                        if(CmpRes == Late)
                        {
                            continue;
                        }
                        
//                        //exceed days of current month
//                        outday = CALE_AlmDaysOfMonth(&CurDateStart);
//                        if(CurDateStart.wDay > outday)
//                        {
//                            CurDateStart.wDay = outday;
//                        }
						
                        pCurNode = (CaleDayNode *)malloc(sizeof(CaleDayNode));
                        if(pCurNode == NULL)
                        {
                            CALE_CloseApi(iOpen);
                            CALE_ReleaseDayListNode(pHead);
                            return FALSE;
                        }
                        memset(pCurNode, 0x0, sizeof(CaleDayNode));
                        pCurNode->CaleType = CALE_MEETING;
                        pCurNode->itemID = nItemID;
                        sprintf(pCurNode->Subject, "%s", CurMeetDate.MeetSubject);
                        sprintf(pCurNode->Location, "%s", CurMeetDate.MeetLocation);

                        pCurNode->CurStartTime.wYear = CurMeetDate.MeetStartYear;
                        pCurNode->CurStartTime.wMonth = CurMeetDate.MeetStartMonth;
                        pCurNode->CurStartTime.wDay = CurMeetDate.MeetStartDay;
                        pCurNode->CurStartTime.wHour = CurMeetDate.MeetStartHour;
                        pCurNode->CurStartTime.wMinute = CurMeetDate.MeetStartMinute;
                        
                        pCurNode->CurEndTime.wYear = CurMeetDate.MeetEndYear;
                        pCurNode->CurEndTime.wMonth = CurMeetDate.MeetEndMonth;
                        pCurNode->CurEndTime.wDay = CurMeetDate.MeetEndDay;
                        pCurNode->CurEndTime.wHour = CurMeetDate.MeetEndHour;
                        pCurNode->CurEndTime.wMinute = CurMeetDate.MeetEndMinute;
                        CALE_AddDayListNode(pHead, pCurNode);
                    }
                    break;

                case CALE_REP_YEAR:
                    {
						CmpRes = TRUE;
						while(Early == CALE_CmpSysDate(&CurDateEnd, &StartSys))
						{
							CmpRes = CALE_YearIncrease(&CurDateStart);
							CmpRes = CALE_YearIncrease(&CurDateEnd);
							if(CmpRes == FALSE)
							{
								break;
							}
						}
                        if(CmpRes == FALSE)
                        {
                            continue;
                        }
                        
//                        //exceed days of current month
//                        outday = CALE_AlmDaysOfMonth(&CurDateStart);
//                        if(CurDateStart.wDay > outday)
//                        {
//                            CurDateStart.wDay = outday;
//                        }
                        CmpRes = CALE_CmpSysDate(&CurDateStart, &EndSys);
                        if(CmpRes == Late)
                        {
                            continue;
                        }
                        
                        pCurNode = (CaleDayNode *)malloc(sizeof(CaleDayNode));
                        if(pCurNode == NULL)
                        {
                            CALE_CloseApi(iOpen);
                            CALE_ReleaseDayListNode(pHead);
                            return FALSE;
                        }
                        memset(pCurNode, 0x0, sizeof(CaleDayNode));
                        pCurNode->CaleType = CALE_MEETING;
                        pCurNode->itemID = nItemID;
                        sprintf(pCurNode->Subject, "%s", CurMeetDate.MeetSubject);
                        sprintf(pCurNode->Location, "%s", CurMeetDate.MeetLocation);
                        //pCurNode->CurTime.wYear = StartSys.wYear;
                        //pCurNode->CurTime.wMonth = StartSys.wMonth;
                        //pCurNode->CurTime.wDay = StartSys.wDay;
                        //pCurNode->CurTime.wHour = CurMeetDate.MeetStartHour;
                        //pCurNode->CurTime.wMinute = CurMeetDate.MeetStartMinute;

                        pCurNode->CurStartTime.wYear = CurMeetDate.MeetStartYear;
                        pCurNode->CurStartTime.wMonth = CurMeetDate.MeetStartMonth;
                        pCurNode->CurStartTime.wDay = CurMeetDate.MeetStartDay;
                        pCurNode->CurStartTime.wHour = CurMeetDate.MeetStartHour;
                        pCurNode->CurStartTime.wMinute = CurMeetDate.MeetStartMinute;
                        
                        pCurNode->CurEndTime.wYear = CurMeetDate.MeetEndYear;
                        pCurNode->CurEndTime.wMonth = CurMeetDate.MeetEndMonth;
                        pCurNode->CurEndTime.wDay = CurMeetDate.MeetEndDay;
                        pCurNode->CurEndTime.wHour = CurMeetDate.MeetEndHour;
                        pCurNode->CurEndTime.wMinute = CurMeetDate.MeetEndMinute;
                        CALE_AddDayListNode(pHead, pCurNode);
                    }
                    break;

                default:
                    break;
                }
            }//end else if(CurMeetDate.MeetRepeatFlag == 0)
        }//end if(CurMeetDate.LocalFlag == CALE_LOCAL)
    }//end while
    CALE_CloseApi(iOpen);

    if((iOpen = CALE_OpenApi(CALE_ANNI)) == CALE_FAIL)
    {
        return FALSE;
    }
    memset(&CurAnniDate, 0x0, sizeof(CaleAnniNode));
    while((nItemID = CALE_ReadApi(iOpen, -1, (BYTE*)&CurAnniDate, 
        sizeof(CaleAnniNode))) != CALE_FAIL)
    {
        if(CurAnniDate.LocalFlag == CALE_LOCAL)
        {
            CurDateStart.wYear = CurAnniDate.AnniDisYear;
            CurDateStart.wMonth = CurAnniDate.AnniDisMonth;
            CurDateStart.wDay = CurAnniDate.AnniDisDay;
            CurDateStart.wHour = CurAnniDate.AnniHour;
            CurDateStart.wMinute = CurAnniDate.AnniMinute;

            CmpRes = CALE_CmpSysDate(&CurDateStart, &EndSys);
            if(CmpRes == Late)
            {
                continue;
            }

            CmpRes = CALE_CmpSysDate(&CurDateStart, &StartSys);
            if(CmpRes == Early)
            {
                while(1)
                {
                    CmpRes = CALE_YearIncrease(&CurDateStart);
                    if(CmpRes == FALSE)
                    {
                        break;
                    }
                    if((CALE_CmpSysDate(&StartSys, &CurDateStart)) != Late)
                    {
                        break;
                    }
                }
            }
            if(CmpRes == FALSE)
            {
                continue;
            }
                        
            //exceed days of current month
            outday = CALE_AlmDaysOfMonth(&CurDateStart);
            if(CurDateStart.wDay > outday)
            {
                CurDateStart.wDay = outday;
            }

            CmpRes = CALE_CmpSysDate(&CurDateStart, &EndSys);
            if(CmpRes == Late)
            {
                continue;
            }
            
            pCurNode = (CaleDayNode *)malloc(sizeof(CaleDayNode));
            if(pCurNode == NULL)
            {
                CALE_CloseApi(iOpen);
                CALE_ReleaseDayListNode(pHead);
                return FALSE;
            }
            memset(pCurNode, 0x0, sizeof(CaleDayNode));
            pCurNode->CaleType = CALE_ANNI;
            pCurNode->itemID = nItemID;
            sprintf(pCurNode->Subject, "%s", CurAnniDate.AnniOccasion);
            pCurNode->DisTime.wYear = CurAnniDate.AnniDisYear;
            pCurNode->DisTime.wMonth = CurAnniDate.AnniDisMonth;
            pCurNode->DisTime.wDay = CurAnniDate.AnniDisDay;
            pCurNode->DisTime.wHour = 8;
            pCurNode->DisTime.wMinute = 0;
            pCurNode->CurStartTime.wYear = CurAnniDate.AnniYear;
            pCurNode->CurStartTime.wMonth = CurAnniDate.AnniMonth;
            pCurNode->CurStartTime.wDay = CurAnniDate.AnniDay;
            pCurNode->CurStartTime.wHour = CurAnniDate.AnniHour;
            pCurNode->CurStartTime.wMinute = CurAnniDate.AnniMinute;

            CALE_AddDayListNode(pHead, pCurNode);
        }//end if(CurMeetDate.LocalFlag == CALE_LOCAL)
    }
    CALE_CloseApi(iOpen);

    if((iOpen = CALE_OpenApi(CALE_EVENT)) == CALE_FAIL)
    {
        return FALSE;
    }
    memset(&CurEventDate, 0x0, sizeof(CaleEventNode));
    while((nItemID = CALE_ReadApi(iOpen, -1, (BYTE*)&CurEventDate, 
        sizeof(CaleEventNode))) != CALE_FAIL)
    {
        if(CurEventDate.LocalFlag == CALE_LOCAL)
        {
            CurDateStart.wYear = CurEventDate.EventStartYear;
            CurDateStart.wMonth = CurEventDate.EventStartMonth;
            CurDateStart.wDay = CurEventDate.EventStartDay;
            CurDateStart.wHour = CurEventDate.EventStartHour;
            CurDateStart.wMinute = CurEventDate.EventStartMinute;

            CmpRes = CALE_CmpSysDate(&CurDateStart, &EndSys);
            if(CmpRes == Late)
            {
                continue;
            }
			
            CurDateEnd.wYear = CurEventDate.EventEndYear;
            CurDateEnd.wMonth = CurEventDate.EventEndMonth;
            CurDateEnd.wDay = CurEventDate.EventEndDay;
            CurDateEnd.wHour = 23;
            CurDateEnd.wMinute = 59;
            
            CmpRes = CALE_CmpSysDate(&CurDateEnd, &StartSys);
            if(CmpRes == Early)
            {
                continue;
            }
            
            pCurNode = (CaleDayNode *)malloc(sizeof(CaleDayNode));
            if(pCurNode == NULL)
            {
                CALE_CloseApi(iOpen);
                CALE_ReleaseDayListNode(pHead);
                return FALSE;
            }
            memset(pCurNode, 0x0, sizeof(CaleDayNode));
            pCurNode->CaleType = CALE_EVENT;
            pCurNode->itemID = nItemID;
            sprintf(pCurNode->Subject, "%s", CurEventDate.EventSubject);
            pCurNode->CurStartTime.wYear = CurEventDate.EventStartYear;
            pCurNode->CurStartTime.wMonth = CurEventDate.EventStartMonth;
            pCurNode->CurStartTime.wDay = CurEventDate.EventStartDay;
            pCurNode->CurStartTime.wHour = CurEventDate.EventStartHour;
            pCurNode->CurStartTime.wMinute = CurEventDate.EventStartMinute;
            
            pCurNode->CurEndTime.wYear = CurEventDate.EventEndYear;
            pCurNode->CurEndTime.wMonth = CurEventDate.EventEndMonth;
            pCurNode->CurEndTime.wDay = CurEventDate.EventEndDay;
            pCurNode->CurEndTime.wHour = CurEventDate.EventEndHour;
            pCurNode->CurEndTime.wMinute = CurEventDate.EventEndMinute;
            CALE_AddDayListNode(pHead, pCurNode);
        }//end if(CurEventDate.LocalFlag == CALE_LOCAL)
    }
    CALE_CloseApi(iOpen);

    return TRUE;
}

static void CALE_SetHour(CaleDayHead *pHead, CaleDay *pCaleDay)
{
    CaleDayNode *pCurNode;
    int i;

    for(i = 0; i < 24; i ++)
    {
        pCurNode = (CaleDayNode *)malloc(sizeof(CaleDayNode));
        if(pCurNode == NULL)
        {
            CALE_ReleaseDayListNode(pHead);
            return;
        }
        memset(pCurNode, 0x0, sizeof(CaleDayNode));
        pCurNode->CaleType = 0;
        pCurNode->itemID = -1;
        pCurNode->CurStartTime.wYear = pCaleDay->sysCurTime.wYear;
        pCurNode->CurStartTime.wMonth = pCaleDay->sysCurTime.wMonth;
        pCurNode->CurStartTime.wDay = pCaleDay->sysCurTime.wDay;
        pCurNode->CurStartTime.wHour = i;
        pCurNode->CurStartTime.wMinute = 0;
        sprintf(pCurNode->Subject, "%d-%d-%d", pCurNode->CurStartTime.wYear, pCurNode->CurStartTime.wMonth, pCurNode->CurStartTime.wDay);
        CALE_AddDayListNode(pHead, pCurNode);
    }
    return;
}

static void CALE_SetDefault(CaleDayHead *pHead, CaleDay *pCaleDay)
{
    CaleDayNode *pCurNode;

    pCurNode = (CaleDayNode *)malloc(sizeof(CaleDayNode));
    if(pCurNode == NULL)
    {
        CALE_ReleaseDayListNode(pHead);
        return;
    }
    memset(pCurNode, 0x0, sizeof(CaleDayNode));
    pCurNode->CaleType = 0;
    pCurNode->itemID = -1;
    pCurNode->CurStartTime.wYear = pCaleDay->sysCurTime.wYear;
    pCurNode->CurStartTime.wMonth = pCaleDay->sysCurTime.wMonth;
    pCurNode->CurStartTime.wDay = pCaleDay->sysCurTime.wDay;
    pCurNode->CurStartTime.wHour = CALE_GetDayStart();
    pCurNode->CurStartTime.wMinute = 0;

    pCurNode->pNext = pHead->pNext;
    pHead->pNext = pCurNode;
    pHead->CurNum ++;

    return;
}

static DWORD RegisterCaleNewSubClass(void)
{
    WNDCLASS wc;
    
    wc.style        = 0;
    wc.lpfnWndProc  = CaleNewSubWndProc;
    wc.cbClsExtra   = 0;
    wc.cbWndExtra   = sizeof(CALE_NEWCREATEDATA);
    wc.hInstance    = NULL;
    wc.hIcon        = NULL;
    wc.hCursor      = NULL;
    wc.hbrBackground= NULL;//GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName = NULL;
    wc.lpszClassName= "CaleNewSubWndClass";
    
    if (!RegisterClass(&wc))
    {
        UnregisterClass("CaleNewSubWndClass", NULL);
        if(!RegisterClass(&wc))
            return FALSE;
    }
    return TRUE;
}

void UnRegisterCaleNewSubClass(void)
{
    UnregisterClass("CaleNewSubWndClass", NULL);
}

BOOL CreateNewSubWnd(HWND hwnd)
{
    if(IsWindow(HwndCaleNewSub))
    {
        ShowWindow(HwndCaleNewSub, SW_SHOW);
        BringWindowToTop(HwndCaleNewSub);
        UpdateWindow(HwndCaleNewSub);
    }
    else
    {
        CALE_NEWCREATEDATA CreateData;
        RECT rClient;
        
        if(!RegisterCaleNewSubClass())
            return FALSE;

        memset(&CreateData, 0x0, sizeof(CALE_NEWCREATEDATA));

        CreateData.hFrameWnd = CALE_GetFrame();
        CreateData.hParent = hwnd;

        GetClientRect(CreateData.hFrameWnd, &rClient);

        HwndCaleNewSub = CreateWindow(
            "CaleNewSubWndClass",
            "",
            WS_VISIBLE | WS_CHILD, 
            rClient.left,
            rClient.top,
            rClient.right-rClient.left,
            rClient.bottom-rClient.top,
            CreateData.hFrameWnd, 
            NULL,
            NULL,
            (PVOID)&CreateData
            );
        
        if(!HwndCaleNewSub)
        {
            UnRegisterCaleNewSubClass();
            return FALSE;
        }
        SendMessage(CreateData.hFrameWnd, PWM_CREATECAPTIONBUTTON, 
            MAKEWPARAM(IDM_BUTTON_COMPLETE, 1), (LPARAM)"");
        SendMessage(CreateData.hFrameWnd, PWM_CREATECAPTIONBUTTON, 
            MAKEWPARAM(IDM_BUTTON_QUIT, 0), (LPARAM)IDP_IME_CANCEL);
        SendMessage(CreateData.hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_SELECT);
        
        SetWindowText(CreateData.hFrameWnd, IDP_IME_NEWENTRY);
        ShowWindow(CreateData.hFrameWnd, SW_SHOW);
        UpdateWindow(CreateData.hFrameWnd);
    }
    
    return TRUE;
}

static LRESULT CaleNewSubWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    PCALE_NEWCREATEDATA pCreateData;

    LRESULT lResult = TRUE;
    HDC hdc;
    RECT rcClient;
    static HBITMAP hMeet;
    static HBITMAP hAnni;
    static HBITMAP hEvent;

    pCreateData = GetUserData(hWnd);

    switch(wMsgCmd)
    {
    case WM_CREATE:
        {
            memcpy(pCreateData, ((LPCREATESTRUCT)lParam)->lpCreateParams, sizeof(CALE_NEWCREATEDATA));
            GetClientRect(hWnd, &rcClient);

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

            SendMessage(HwndList, LB_ADDSTRING, NULL, (LPARAM)(IDP_IME_MEETING));
            SendMessage(HwndList, LB_ADDSTRING, NULL, (LPARAM)(IDP_IME_EVENT));
            SendMessage(HwndList, LB_ADDSTRING, NULL, (LPARAM)(IDP_IME_ANNIVERARY));
            hMeet = LoadImage(NULL, CALE_ICON_MEET, IMAGE_BITMAP,0, 0, LR_LOADFROMFILE);
            hAnni = LoadImage(NULL, CALE_ICON_ANNI, IMAGE_BITMAP,0, 0, LR_LOADFROMFILE);
            hEvent = LoadImage(NULL, CALE_ICON_EVENT, IMAGE_BITMAP,0, 0, LR_LOADFROMFILE);
            SendMessage(HwndList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, 0), (LPARAM)hMeet);
            SendMessage(HwndList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, 1), (LPARAM)hEvent);
            SendMessage(HwndList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, 2), (LPARAM)hAnni);

            SetFocus(HwndList);
            pCreateData->hFocus = HwndList;
            SendMessage(HwndList, LB_SETCURSEL, 0, 0);
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
        {            
            SetFocus(pCreateData->hFocus);
            CALE_SetSystemIcons(pCreateData->hFrameWnd);

            SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_SELECT);
            SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
            SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDP_IME_CANCEL);
            
            SetWindowText(pCreateData->hFrameWnd, IDP_IME_NEWENTRY);
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
                    if((temp >= 0) && (temp <= 2))
                    {
                        HWND hparent;

                        hparent = pCreateData->hParent;
                        PostMessage(hparent, CALE_NEWSUB_RETURN, temp, 0);
                        PostMessage(hWnd, WM_CLOSE, 0, 0);
                        
                    }
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
            DeleteObject(hMeet);
            hMeet = NULL;
            DeleteObject(hAnni);
            hAnni = NULL;
            DeleteObject(hEvent);
            hEvent = NULL;
            
            HwndCaleNewSub = NULL;
            UnRegisterCaleNewSubClass();
        }
        break;
        
    default:
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
    return lResult;
}

static DWORD RegisterCaleDelSelClass(void)
{
    WNDCLASS wc;
    
    wc.style        = 0;
    wc.lpfnWndProc  = CaleDelSelWndProc;
    wc.cbClsExtra   = 0;
    wc.cbWndExtra   = 0;
    wc.hInstance    = NULL;
    wc.hIcon        = NULL;
    wc.hCursor      = NULL;
    wc.hbrBackground= NULL;//GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName = NULL;
    wc.lpszClassName= "CaleDelSelWndClass";
    
    if (!RegisterClass(&wc))
    {
        UnregisterClass("CaleDelSelWndClass", NULL);
        if(!RegisterClass(&wc))
            return FALSE;
    }
    return TRUE;
}

void UnRegisterCaleDelSelClass(void)
{
    UnregisterClass("CaleDelSelWndClass", NULL);
}

BOOL CreateDelSelWnd(HWND hwnd)
{
    if(IsWindow(HwndCaleDelSel))
    {
        ShowWindow(HwndCaleDelSel, SW_SHOW);
        BringWindowToTop(HwndCaleDelSel);
        UpdateWindow(HwndCaleDelSel);
    }
    else
    {
        HWND hFrameWnd;
        RECT rClient;
        
        if(!RegisterCaleDelSelClass())
            return FALSE;
        
        hFrameWnd = CALE_GetFrame();
        GetClientRect(hFrameWnd, &rClient);

        HwndCaleDelSel = CreateWindow(
            "CaleDelSelWndClass",
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
        
        if(!HwndCaleDelSel)
        {
            UnRegisterCaleDelSelClass();
            return FALSE;
        }
        SendMessage(hFrameWnd, PWM_CREATECAPTIONBUTTON, 
            MAKEWPARAM(IDM_BUTTON_COMPLETE, 1), (LPARAM)IDP_IME_DELETE);
        SendMessage(hFrameWnd, PWM_CREATECAPTIONBUTTON, 
            MAKEWPARAM(IDM_BUTTON_QUIT, 0), (LPARAM)IDP_IME_CANCEL);
        SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_SELECT);
        
        SetWindowText(hFrameWnd, IDP_STRING_SELENTRIES);
        ShowWindow(hFrameWnd, SW_SHOW);
        UpdateWindow(hFrameWnd);
    }
    
    return TRUE;
}

static LRESULT CaleDelSelWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    LRESULT lResult = TRUE;
    HDC hdc;
    RECT rcClient;
    static HWND hwndFocus;
    //static HBITMAP hSelect;
    //static HBITMAP hUnSelect;

    static CaleDelHead DelHead;
    //int i;

    switch(wMsgCmd)
    {
    case WM_CREATE:
        {
            GetClientRect(hWnd, &rcClient);

            HwndList = CreateWindow(
                "MULTILISTBOX",
                "",
                WS_VISIBLE | WS_CHILD | LBS_NOTIFY | LBS_BITMAP,
                rcClient.left,
                rcClient.top,
                rcClient.right-rcClient.left,
                rcClient.bottom-rcClient.top,
                hWnd,
                NULL, NULL, NULL);

            //hSelect = LoadImage(NULL, CALE_ICON_SELECT, IMAGE_BITMAP,0, 0, LR_LOADFROMFILE);
            //hUnSelect = LoadImage(NULL, CALE_ICON_UNSELECT, IMAGE_BITMAP,0, 0, LR_LOADFROMFILE);

            CALE_ReleaseDelListNode(&DelHead);
            if(!CALE_CreateDelList(&DelHead))
            {
                PostMessage(hWnd, WM_CLOSE, NULL, NULL);
            }
//            for(i=0; i<DelHead.TotalNum; i++)
//            {
//                SendMessage(HwndList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, i), (LPARAM)hUnSelect);
//            }
			SendMessage(HwndList, LB_ENDINIT, 0, 0);
            SendMessage(HwndList, LB_SETCURSEL, 0, 0);
            SetFocus(HwndList);
            hwndFocus = HwndList;
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

            SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDP_IME_DELETE);
            SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDP_IME_CANCEL);
            SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_SELECT);
            
            SetWindowText(hFrameWnd, IDP_STRING_SELENTRIES);
        }
        break;

    case WM_COMMAND:
        {
            switch(LOWORD(wParam))
            {
            case IDM_BUTTON_COMPLETE:
                {
                    if(DelHead.DelNum != 0)
                    {
                        PLXConfirmWinEx(NULL, hWnd, IDP_STRING_DELSELECT, Notify_Request, 
                            NULL, IDP_IME_YES, IDP_IME_NO, CALE_CONFIRM_MESSAGE);
                    }
                    else
                    {
						PLXTipsWin(NULL, hWnd, 0, IDP_STRING_SELECTERROR, NULL, 
							Notify_Alert, IDP_IME_OK, NULL, WAITTIMEOUT);
                    //	SendMessage(hWnd, WM_CLOSE, 0, 0);
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
					int temp = -1;
					CaleDelNode *SelNode;

					temp = SendMessage(HwndList, LB_GETCOUNT, 0, 0);
					while(temp > 0)
					{
						temp --;
						if (SendMessage(HwndList, LB_GETSEL, temp, 0))
						{
							CALE_GetDelNodeByInt(&DelHead, &SelNode, temp);
							if(SelNode)
							{
								SelNode->DelFlag = TRUE;
								DelHead.DelNum ++;
							}
						}
					}
                    PostMessage(hWnd, WM_COMMAND, MAKEWPARAM(IDM_BUTTON_COMPLETE, 0), 0);
                }
                break;
            /*    
            case VK_F5:
                {
                    int temp;
                    CaleDelNode *SelNode;

                    temp = SendMessage(HwndList, LB_GETCURSEL, 0, 0);
                    if((temp >= 0) && (temp <= DelHead.TotalNum))
                    {
                        CALE_GetDelNodeByInt(&DelHead, &SelNode, temp);
                        if(SelNode->DelFlag)
                        {
                            //SendMessage(HwndList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, temp), (LPARAM)hUnSelect);
							SendMessage(HwndList, LB_SETSEL, FALSE, temp);
                            SelNode->DelFlag = FALSE;
                            DelHead.DelNum --;
                        }
                        else
                        {
                            //SendMessage(HwndList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, temp), (LPARAM)hSelect);
							SendMessage(HwndList, LB_SETSEL, TRUE, temp);
                            SelNode->DelFlag = TRUE;
                            DelHead.DelNum ++;
                        }
                    }
                }
                break;
                //*/
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
            CALE_ReleaseDelListNode(&DelHead);
            //DeleteObject(hSelect);
            //hSelect = NULL;
            //DeleteObject(hUnSelect);
            //hUnSelect = NULL;

            HwndCaleNewSub = NULL;
            UnRegisterCaleNewSubClass();
        }
        break;

    case CALE_CONFIRM_MESSAGE:
        {
            switch(lParam)
            {
            case 0:
                {
                }
                break;
                
            case 1:
                {
                    CaleDelNode *DelNode;
                    int nOpen;
                    CaleMeetingNode TmpMeeting;
                    CaleAnniNode TmpAnni;
                    CaleEventNode TmpEvent;

                    DelNode = DelHead.pNext;
                    while (DelNode)
                    {
                        if(DelNode->DelFlag)
                        {
                            switch(DelNode->CaleType)
                            {
                            case 0:
                                break;
                                
                            case CALE_MEETING:
                                {
                                    if((nOpen = CALE_OpenApi(CALE_MEETING)) == CALE_FAIL)
                                    {
                                        return FALSE;
                                    }
                                    if(CALE_ReadApi(nOpen, DelNode->itemID, (BYTE*)&TmpMeeting, 
                                        sizeof(CaleMeetingNode)) == CALE_SUCCESS)
                                    {
                                        if(TmpMeeting.MeetAlarmFlag)
                                        {
                                            CALE_KillAlarm(CALE_MEETING, DelNode->itemID);
                                        }
                                        TmpMeeting.LocalFlag = CALE_UNLOCAL;
                                        CALE_DelApi(CALE_MEETING, nOpen, DelNode->itemID, (BYTE*)&TmpMeeting,
                                            sizeof(CaleMeetingNode));
                                    }
                                    CALE_CloseApi(nOpen);
                                }
                                break;
                                
                            case CALE_ANNI:
                                {
                                    if((nOpen = CALE_OpenApi(CALE_ANNI)) == CALE_FAIL)
                                    {
                                        return FALSE;
                                    }
                                    if(CALE_ReadApi(nOpen, DelNode->itemID, (BYTE*)&TmpAnni, 
                                        sizeof(CaleAnniNode)) == CALE_SUCCESS)
                                    {
                                        if(TmpAnni.AnniAlarmFlag)
                                        {
                                            CALE_KillAlarm(CALE_ANNI, DelNode->itemID);
                                        }
                                        TmpAnni.LocalFlag = CALE_UNLOCAL;
                                        CALE_DelApi(CALE_ANNI, nOpen, DelNode->itemID, (BYTE*)&TmpAnni,
                                            sizeof(CaleAnniNode));
                                    }
                                    CALE_CloseApi(nOpen);
                                }
                                break;
                                
                            case CALE_EVENT:
                                {
                                    if((nOpen = CALE_OpenApi(CALE_EVENT)) == CALE_FAIL)
                                    {
                                        return FALSE;
                                    }
                                    if(CALE_ReadApi(nOpen, DelNode->itemID, (BYTE*)&TmpEvent, 
                                        sizeof(CaleEventNode)) == CALE_SUCCESS)
                                    {
                                        TmpEvent.LocalFlag = CALE_UNLOCAL;
                                        CALE_DelApi(CALE_EVENT, nOpen, DelNode->itemID, (BYTE*)&TmpEvent,
                                            sizeof(CaleEventNode));
                                    }
                                    CALE_CloseApi(nOpen);
                                }
                                break;
                                
                            default:
                                break;
                            }//end switch
                            DelNode->DelFlag = FALSE;
                            DelHead.DelNum --;
                        }//end if(DelNode->DelFlag)
                        DelNode = DelNode->pNext;
                        DelHead.TotalNum --;
                    }//end while
                    if(DelHead.DelNum)
                    {
                        SendMessage(CALE_GetAppWnd(), CALE_CONFIRM_MESSAGE, NULL, 0);
                    }
                    else
                    {
                        SendMessage(CALE_GetAppWnd(), CALE_CONFIRM_MESSAGE, NULL, 1);
                    }
                    
                    PostMessage(hWnd, WM_CLOSE, NULL, NULL);
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

static BOOL CALE_CreateDelList(CaleDelHead *pHead)
{
    CaleDayNode *pTmp;
    CaleDelNode *pDelNode;

    pTmp = CurDayHead.pNext;
    while(pTmp)
    {
        if(pTmp->CaleType == 0)
        {
            pTmp = pTmp->pNext;
            continue;
        }
        pDelNode = NULL;
        pDelNode = (CaleDelNode *)malloc(sizeof(CaleDelNode));
        if(pDelNode == NULL)
        {
            CALE_ReleaseDelListNode(pHead);
            return FALSE;
        }
        memset(pDelNode, 0x0, sizeof(CaleDelNode));

        pDelNode->CaleType = pTmp->CaleType;
        pDelNode->itemID = pTmp->itemID;
        sprintf(pDelNode->Subject, "%s", pTmp->Subject);
        pDelNode->DelFlag = 0;
        pDelNode->pNext = NULL;
        CALE_AddDelListNode(pHead, pDelNode);
        SendMessage(HwndList, LB_ADDSTRING, NULL, (LPARAM)pDelNode->Subject);
        
        pTmp = pTmp->pNext;
    }

    return TRUE;
}

static void CALE_ReleaseDelListNode(CaleDelHead *pHead)
{
    CaleDelNode *pTemp;
    
    pTemp = pHead->pNext;
    while (pTemp)
    {
        pHead->pNext = pTemp->pNext;
        free(pTemp);
        pTemp = NULL;
        pTemp = pHead->pNext;
    }
    pHead->DelNum = 0;
    pHead->TotalNum = 0;
    pHead->pNext = NULL;
    
    return;
}

static void CALE_AddDelListNode(CaleDelHead *pHead, CaleDelNode *pTmpNode)
{
    CaleDelNode *pTemp;

    pTmpNode->pNext = NULL;
    
    if(pHead->pNext == NULL)
    {
        pHead->pNext = pTmpNode;
    }
    else
    {
        pTemp = pHead->pNext;
        while (pTemp->pNext)
        {
            pTemp = pTemp->pNext;
        }
        pTmpNode->pNext = NULL;
        pTemp->pNext = pTmpNode;
    }
    pHead->TotalNum ++;
    
    return;
}

static void CALE_DelDelListNode(CaleDelHead *pHead, int iTmp)
{
    CaleDelNode *pTemp;
    CaleDelNode *pPre;
    int i;
    
    if(pHead->pNext == NULL)
        return;
    
    pTemp = pHead->pNext;
    if(iTmp == 0)
    {
        pHead->pNext = pTemp->pNext;
        free(pTemp);
        pTemp = NULL;
    }
    else
    {
        for(i = 1; i < iTmp; i ++)
        {
            pPre = pTemp;
            pTemp = pPre->pNext;
        }
        pPre->pNext = pTemp->pNext;
        free(pTemp);
        pTemp = NULL;
    }
    pHead->TotalNum --;
    
    return;
}

static BOOL CALE_GetDelNodeByInt(CaleDelHead *pHead, CaleDelNode **pTmpNode, int iTmp)
{
    int i;
    CaleDelNode *pTemp;
    
    if(iTmp > (pHead->TotalNum - 1))
        return FALSE;
    
    if(pHead->pNext == NULL)
        return FALSE;
    
    pTemp = pHead->pNext;
    for(i = 0; i < iTmp; i ++)
    {
        pTemp = pTemp->pNext;
    }
    *pTmpNode = pTemp;
    
    return TRUE;
}
