/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : CPHS application / information number
 *
 * Purpose  : main window
 *
 * Autor    : 
 *
 *-----------------------------------------------------------
 *
 *$Archive::														$
 *$Workfile::														$
 *$Revision::		$		$Date::									$
\**************************************************************************/
//#include    "CPHSInfoNum.h"
#include    "window.h"
#include    "string.h"
#include    "stdlib.h"
#include    "stdio.h" 
#include    "winpda.h"
#include    "str_plx.h"
#include    "me_wnd.h"
#include    "plx_pdaex.h"
#include    "str_public.h"
#include    "setting.h"
#include    "setup.h"
#include    "mullang.h"
#include    "pubapp.h"
#include    "plxdebug.h"


#define     pClassName          "CPHSInfoNumClass"


#define     IDC_BUTTON_EXIT     101
#define     IDC_INFOLIST        102
#define     IDC_BUTTON_OK       103
#define     IDC_BUTTON_EDIT     104
#define     CALLBACK_GETINFOSIZE    WM_USER + 100
#define     CALLBACK_ANALIZE        WM_USER + 101
#define     CALLBACK_READFILE       WM_USER + 102
#define     CALLBACK_SAVEDATA       WM_USER + 103
#define     CALLBACK_LOADLIST       WM_USER + 104
#define     CALLBACK_CANCEL_LOAD    WM_USER + 105

static  HWND        hFrameWin=NULL;
static   unsigned int iRecNum = 0, iRecSize, iContentLen;
static   unsigned int iRecIndex = 1;
static   unsigned int iLevelFlag = 0;
static   int callFlag = 0;
static   BYTE *RecData = NULL;
static   BYTE *pData = NULL;
static   BYTE *pTmpData = NULL;
static   BOOL  OnCPHS_GetInfoSize(HWND hWnd, int iMsg);
static   void  CPHS_DecodeRecordData(HWND hWnd, int iMsg);
static   char EntryName[MAX_CPHS_ENTRY_NAME + 1] = ""; //used when decoding

static  CPHS_INFONUM_ENTRY * pInfo_Num_Entry = NULL;

static  HWND        hWndApp=NULL;
static  HINSTANCE   hInstance;
static  LRESULT     AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static  BOOL        CreateControl(HWND hwnd, HWND * hInfoList);
static  BOOL        OnCreate(HWND hWnd, HWND *hInfoList);
static  void        OnSetFocus(HWND hWnd);
static  LRESULT     OnActivate(HWND hWnd, UINT wMsgCmd, WPARAM wParam, WPARAM lParam);
static  void        OnDestroy(hWnd);
static  LRESULT     OnKeyDown(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static  void        OnCommand(HWND hWnd, HWND hInfoList, WPARAM wParam);

static  void        OnCPHS_Read_File(HWND hWnd, int iSaveMsg, int iLoadContentMsg);
static  void        OnCPHS_Save_Data(HWND hWnd, int iMsg);
static  void        CPHS_FreeSpace(void);
static  BOOL        CPHS_InitSpace(void);
static  void        InitInfoNumList(HWND hWnd, HWND * hList);
static  int        OnMiddleButtonPushed(HWND hWnd, HWND hList, int iCurFocus);
static BOOL CPHS_IfLastLevel(int iListIndex, unsigned int iLevel);
static int CPHS_GetDataIndex(int iListIndex, unsigned int iLevel);

extern void GetScreenUsableWH2 (int * pwidth,int * pheight);
extern void ParseInfoNumber(CPHS_INFONUM_ENTRY * DesData, BYTE * SrcData, unsigned int DataLen);
BOOL    APP_CallPhoneNumber(const char * pPhoneNumber);
void IntToString( int nValue, char *buf );

DWORD SIMinfonumbers_AppControl(int nCode, void* pInstance, WPARAM wParam, LPARAM lParam)
{
    WNDCLASS wc;
    DWORD dwRet = TRUE;

    switch (nCode)
    {
    case APP_INIT :
        hInstance = pInstance;
        break;
    case APP_ACTIVE :

#ifdef _CPHS_DEBUG_
  StartObjectDebug();
#endif
        if(IsWindow(hFrameWin))
        {
            ShowWindow(hFrameWin,SW_SHOW);
            UpdateWindow(hFrameWin);
        }
        else
        {
            RECT rClient;
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
                return FALSE;
            }


            hFrameWin = CreateFrameWindow(WS_CAPTION | PWS_STATICBAR);
            GetClientRect(hFrameWin, &rClient);


            hWndApp = CreateWindow(
                pClassName, 
                NULL, 
                WS_VISIBLE | WS_CHILD, 
                rClient.left,
                rClient.top,
                rClient.right-rClient.left,
                rClient.bottom-rClient.top,
                hFrameWin, 
                NULL,
                NULL, 
                NULL
                );
            SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
            SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
            
            SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, IDC_BUTTON_EXIT,(LPARAM)ML("Exit"));
            SetWindowText(hFrameWin,ML("Information numbers"));
            
            ShowWindow(hFrameWin,SW_SHOW);
            UpdateWindow(hFrameWin);
            SetFocus(hWndApp);                
        }    
        break;

    case APP_INACTIVE :
        ShowWindow(hFrameWin,SW_HIDE); 
        break;
    }
    return dwRet;
}


static LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
    static HWND hInfoList;
    LRESULT     lResult;
	int iret;

    lResult = (LRESULT)TRUE;

    switch ( wMsgCmd )
    {
    case WM_CREATE:
		iLevelFlag = 0;
        OnCreate(hWnd, &hInfoList);
        iret = ME_GetCPHSparameters(hWnd, CALLBACK_GETINFOSIZE, 0x6F19); 
    	if (iret != -1)
		{
	        WaitWin(hWnd, TRUE, ML("Communicating with SIM..."), ML("Information numbers"), 0,
		        ML("Cancel"), CALLBACK_CANCEL_LOAD);
		}
		break;
    case WM_SETFOCUS:
        OnSetFocus(hWnd);
    	break;
    case PWM_SHOWWINDOW:
        SetFocus(hWnd);
        SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
        SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
        SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, IDC_BUTTON_EXIT,(LPARAM)ML("Exit"));
        SetWindowText(hFrameWin,ML("Information numbers"));
        break;
    case WM_DESTROY:
        OnDestroy(hWnd);
#ifdef _CPHS_DEBUG_
  EndObjectDebug();
#endif
        break;
    case WM_KEYDOWN:
        lResult = OnKeyDown(hWnd,wMsgCmd,wParam,lParam);
        break;
    case WM_COMMAND:
        OnCommand(hWnd, hInfoList, wParam);
        break;

    case CALLBACK_GETINFOSIZE:
        if (wParam == ME_RS_SUCCESS)
		{
			if (!OnCPHS_GetInfoSize(hWnd, CALLBACK_READFILE))
			{
				PLXTipsWin(NULL, NULL, NULL, ML("No information available"), NULL, Notify_Alert, ML("Ok"), NULL, WAITTIMEOUT);
				SendMessage(hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hWnd,0); 
				SendMessage(hWnd, WM_CLOSE, 0, 0);		
			}
		}
		else
		{
			PLXTipsWin(NULL, NULL, NULL, ML("No information available"), NULL, Notify_Alert, ML("Ok"), NULL, WAITTIMEOUT);
            SendMessage(hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hWnd,0); 
            SendMessage(hWnd, WM_CLOSE, 0, 0);			
		}
        break;

    case CALLBACK_READFILE:
        OnCPHS_Read_File(hWnd, CALLBACK_SAVEDATA, CALLBACK_LOADLIST);
        break;

    case CALLBACK_SAVEDATA:
        if (wParam == ME_RS_SUCCESS)
            OnCPHS_Save_Data(hWnd, CALLBACK_READFILE);
        break;
 
	case CALLBACK_LOADLIST:
        InitInfoNumList(hWnd, &hInfoList);
		WaitWin(hWnd, FALSE, NULL, NULL, NULL,NULL, NULL); 
        break;

	case CALLBACK_CANCEL_LOAD:
		SendMessage(hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hWnd,0); 
		SendMessage(hWnd, WM_CLOSE, 0, 0);
		break;

    default :
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
    return lResult;
}

static BOOL OnCreate(HWND hWnd, HWND *hInfoList)
{
    int iscreenw, iscreenh;

    GetScreenUsableWH2(&iscreenw,&iscreenh);

    * hInfoList = CreateWindow("LISTBOX", 0, 
       WS_VISIBLE | WS_CHILD | WS_TABSTOP |LBS_BITMAP|LBS_NOTIFY|WS_VSCROLL,   
        0,0,iscreenw,iscreenh,
        hWnd, (HMENU)IDC_INFOLIST, NULL, NULL);
    if (* hInfoList == NULL )
        return FALSE; 
    return TRUE;
}

static BOOL   OnCPHS_GetInfoSize(HWND hWnd, int iMsg)
{
    CPHS_struct result;    

    memset(&result, 0, sizeof(CPHS_struct));

    if(ME_GetResult(&result, sizeof(CPHS_struct)) < 0)
        return FALSE;
    
    if(result.Type == RecordContent)
    {
        iRecSize = result.Record_Len;     //size of one record
        iContentLen = result.Content_Len; //size of all content
        iRecNum = iContentLen / iRecSize; //the number of records
    }
	else if (result.Type == FieldNotFound)
	{
		return FALSE;
	}
    
    if ( 0 == iRecNum )
        return FALSE; //no record

    if(CPHS_InitSpace())
        PostMessage(hWnd,CALLBACK_READFILE,ME_RS_SUCCESS,0);   
    
    return TRUE;
}

static BOOL  CPHS_InitSpace(void) //Initialize the memory space 
{
    RecData = (BYTE*)malloc(iContentLen * sizeof(BYTE)); //init the space for all data
     
    if(NULL == RecData)
        return FALSE;
    
    pData = RecData;
	pTmpData = RecData;

    pInfo_Num_Entry = (CPHS_INFONUM_ENTRY*)malloc(iRecNum * sizeof(CPHS_INFONUM_ENTRY));
	//init the space for saving structure
	
    if(NULL == pInfo_Num_Entry)
        return FALSE;

    return TRUE;
}

static void CPHS_FreeSpace(void)
{
    if (RecData)
        free(RecData);
    RecData = NULL;
    if (pInfo_Num_Entry)
        free(pInfo_Num_Entry);
    pInfo_Num_Entry = NULL;
}

static void OnCPHS_Read_File(HWND hWnd, int iSaveMsg, int iLoadContentMsg)
{
    if(iRecIndex <= iRecNum)
    {
        ME_ReadCPHSRecord( hWnd, iSaveMsg, 0x6F19, iRecSize, iRecIndex); //read the record of iRecIndex
    }
    else 
        CPHS_DecodeRecordData(hWnd, iLoadContentMsg);
}

static void OnCPHS_Save_Data(HWND hWnd, int iMsg)
{        
    ME_GetResult(pTmpData, iRecSize);
        
    iRecIndex++;  

    pTmpData += iRecSize;
         
    PostMessage(hWnd, iMsg, 0, 0);
}

static void OnSetFocus(HWND hWnd)
{
    SetFocus(GetDlgItem(hWnd,IDC_INFOLIST));
}
 
static LRESULT OnActivate(HWND hWnd, UINT wMsgCmd, WPARAM wParam, WPARAM lParam)
{
    LRESULT lResult;
    if (WA_INACTIVE != LOWORD(wParam))
        SetFocus(GetDlgItem(hWnd,IDC_INFOLIST));
    lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
    InvalidateRect(hWnd,NULL,TRUE);
    return lResult;
}

static void    OnDestroy(HWND hWnd)
{
	WaitWin(hWnd, FALSE, NULL, NULL, NULL,NULL, NULL); 
    CPHS_FreeSpace();
    hWndApp = NULL;
    UnregisterClass(pClassName,NULL);
	DlmNotify((WPARAM)PES_STCQUIT, (LPARAM)hInstance);
}

static LRESULT    OnKeyDown(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    LRESULT lResult;
    switch(LOWORD(wParam))
        {       
		case VK_F10:
            SendMessage(hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hWnd,0); 
            SendMessage(hWnd, WM_CLOSE, 0, 0);
            break;
        case VK_F5:
            SendMessage(hWnd,WM_COMMAND,IDC_BUTTON_OK,0);
             break;
		case VK_RETURN:
			SendMessage(hWnd, WM_COMMAND, IDC_BUTTON_EDIT, 0);
			break;
        default:
            lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        }
    return lResult;
}
 
static void    OnCommand(HWND hWnd, HWND hInfoList, WPARAM wParam)
{
	int iCurFocus, iIndex, iDataIndex;
	char InfoNum[CPHS_INFO_NUM_LENGTH];
	
	switch (HIWORD(wParam))
	{
	case LBN_SELCHANGE:
		iIndex = SendMessage(hInfoList,LB_GETCURSEL,0,0);
		
		if( CPHS_IfLastLevel(iIndex, iLevelFlag) )// not the last level
		{
			SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
			SendMessage(hFrameWin , PWM_SETBUTTONTEXT, 1,(LPARAM)"") ;
			callFlag = 0;			
		}
		else// the last level
		{
			SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
			SendMessage(hFrameWin , PWM_CREATECAPTIONBUTTON, 
				MAKEWPARAM(IDC_BUTTON_EDIT,1),(LPARAM)ML("Call") ) ;
			callFlag = 1;			
		}
		break;
		
	default:
		break;
	}

    switch (LOWORD(wParam))
    {
    case IDC_BUTTON_OK:
		iCurFocus = SendMessage(hInfoList, LB_GETCURSEL, 0, 0);
        OnMiddleButtonPushed(hWnd, hInfoList, iCurFocus);
        break;
	case IDC_BUTTON_EDIT:
		if (callFlag == 1)
		{
			iCurFocus = SendMessage(hInfoList, LB_GETCURSEL, 0, 0);
			iDataIndex = CPHS_GetDataIndex(iCurFocus, iLevelFlag);
			IntToString(pInfo_Num_Entry[iDataIndex].nEntryNo, InfoNum);
		 	APP_CallPhoneNumber(InfoNum);
		}
		break;
	default:
		break;
    }
}

static void  CPHS_DecodeRecordData(HWND hWnd, int iMsg)
{
    UINT i = 0;
    CPHS_INFONUM_ENTRY * pINEntry_tmp; //used for saving the data that has been decoded

    pINEntry_tmp = pInfo_Num_Entry;
        
    while(i < iRecNum)
    {
        ParseInfoNumber(pINEntry_tmp, pData, iRecSize);

        pData += iRecSize;

        pINEntry_tmp++;
    }

    PostMessage(hWnd, CALLBACK_LOADLIST, 0, 0);
}



static void InitInfoNumList(HWND hWnd, HWND * hList)
{
    unsigned int i = 0, j = 0;
    while(i<iRecNum)
    {
        if(pInfo_Num_Entry[i].iIndexLev == 1)
		{
			SendMessage(* hList, LB_ADDSTRING, 0, (LPARAM)(pInfo_Num_Entry[i].szEntryName));
			pInfo_Num_Entry[i].iListIndex = j;
			j++;
		}
		i++;
    }
	iLevelFlag++;

	if( CPHS_IfLastLevel(0, iLevelFlag) )// not the last level
	{
		SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
		SendMessage(hFrameWin , PWM_SETBUTTONTEXT, 1,(LPARAM)"") ;
		callFlag = 0;
		
	}
	else// the last level
	{
		SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
		SendMessage(hFrameWin , PWM_CREATECAPTIONBUTTON, 
			MAKEWPARAM(IDC_BUTTON_EDIT,1),(LPARAM)ML("Call") ) ;
		callFlag = 1;
	}

	SendMessage(*hList, LB_SETCURSEL, 0, 0);
}

static int OnMiddleButtonPushed(HWND hWnd, HWND hList, int iCurFocus)
{
	unsigned int i = 0, iDataIndex = 0;
	int iNextLev;
	
	if (0 == iRecNum)
		return -1;	//no record available

	iDataIndex = CPHS_GetDataIndex(iCurFocus, iLevelFlag);

	if (iDataIndex+1 >= iRecNum)
		return 0;	//end of record

	iNextLev = pInfo_Num_Entry[iDataIndex+1].iIndexLev;

	if (iNextLev - iLevelFlag != 1) //last level
		return 0;	

	SendMessage(hList, LB_RESETCONTENT, 0, 0);
	
	iLevelFlag++;

	while(pInfo_Num_Entry[iDataIndex].iIndexLev >= iLevelFlag)
	{
		if (pInfo_Num_Entry[iDataIndex].iIndexLev - iLevelFlag == 1)
			SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)pInfo_Num_Entry[iDataIndex].szEntryName);

		if (iDataIndex + 1 >= iRecNum)
			break;

		iDataIndex++;
	}

	if( CPHS_IfLastLevel(0, iLevelFlag) )// not the last level
	{
		SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
		SendMessage(hFrameWin , PWM_SETBUTTONTEXT, 1,(LPARAM)"") ;
		callFlag = 0;
		
	}
	else// the last level
	{
		SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
		SendMessage(hFrameWin , PWM_CREATECAPTIONBUTTON, 
			MAKEWPARAM(IDC_BUTTON_EDIT,1),(LPARAM)ML("Call") ) ;
		callFlag = 1;
	}

	SendMessage(hList, LB_SETCURSEL, 0, 0);
	return 1;
}
static int CPHS_GetDataIndex(int iListIndex, unsigned int iLevel)
{
	unsigned int i=0, iLevIndex = 0;
	
	while(i<iRecNum)
	{
		if (pInfo_Num_Entry[i].iIndexLev == iLevel)
		{
			if (iLevIndex == (unsigned int)iListIndex)
				return i;
			else
				iLevIndex++;
		}
		i++;
	}
	return -1;
}

static BOOL CPHS_IfLastLevel(int iListIndex, unsigned int iLevel)//TRUE: not the last level; FALSE: the last level
{
	int iDataIndex;

	iDataIndex = CPHS_GetDataIndex(iListIndex, iLevel);

	if (iDataIndex != -1)
	{
		if (pInfo_Num_Entry[iDataIndex+1].iIndexLev - iLevel == 1)
			return TRUE;
		else
			return FALSE;
	}
	else
		return FALSE;
}
