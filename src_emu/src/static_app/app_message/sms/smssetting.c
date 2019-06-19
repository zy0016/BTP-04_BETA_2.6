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

#define MSG_SCA_REFRESH         WM_USER+100

#define IDC_OK      100
#define IDC_CANCEL  200

#define IDC_SMSSETTINGLIST_LIST     300
#define IDM_SMSSETTINGLIST_EDIT     301
#define IDM_SMSSETTINGLIST_ACTIVATE 302
#define IDM_SMSSETTINGLIST_DELETE   303

#define IDC_SMSSETTING_SCA          301
#define IDC_SMSSETTING_SCAINUSE     302
#define IDC_SMSSETTING_REPORT       303
#define IDC_SMSSETTING_VALIDITY     304
#define IDC_SMSSETTING_PID          305
#define IDC_SMSSETTING_CONNECTION   306
#define IDC_SMSSETTING_REPLYPATH    307  
#define IDC_SMSSETTING_PROFILENAME  308  

#define IDC_SMSSETTING_EDIT         300


#define IDC_SMSSCA_LIST             300  
#define IDM_SMSSCA_EDIT             101
#define IDM_SMSSCA_NEW              102 
#define IDM_SMSSCA_DELETE           103

#define IDC_SCAEDIT_NAME            301
#define IDC_SCAEDIT_NUM             302

#define IDC_SMSSELSCA_LIST          300



#define TIMER_ID                    1
#define ET_REPEAT_FIRST             300
#define ET_REPEAT_LATER             100



static HWND hSettingWnd = NULL;
static int  nWay;
static int  nSCACounter;
static HWND hFocus = NULL;
//static SMS_SCA *pSCA = NULL;
static SMS_SETTING SMSSetting;

static HWND hSCAEditWnd = NULL;
static HWND hSCAEditFocus = NULL;
static BOOL bChangeMenu;

static LRESULT SMSSettingListWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static BOOL SMSSettingList_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct);
static void SMSSettingList_OnActivate(HWND hwnd, UINT state);
static void SMSSettingList_OnPaint(HWND hWnd);
static void SMSSettingList_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags);
static void SMSSettingList_OnCommand(HWND hWnd, int id, UINT codeNotify);
static void SMSSettingList_OnDestroy(HWND hWnd);
static void SMSSettingList_OnClose(HWND hWnd);
static void SMSSettingList_OnSetFocus(HWND hWnd);
static void SMSSettingList_OnRefresh(HWND hWnd,BOOL bRefresh,BOOL bSetActive,SMS_SETTINGCHAIN *pSetting);
static void SMSSettingList_OnSetCenterNR(HWND hWnd,BOOL bSet);
static void SMSSettingList_OnDeleteProfile(HWND hWnd,BOOL bDelete);
static void SMSSettingList_OnSetNRResult(HWND hWnd,BOOL bCanSend,SMS_SETTINGCHAIN *pSetting);
static void SMSSettingList_OnSetCenterNRForSend(HWND hWnd,BOOL bSet);

BOOL SMS_CreateSettingWnd(HWND hFrameWnd,HWND hMsgWnd,UINT uMsgCmd,SMS_SETTINGCHAIN *pSetting,BOOL bFirst,BOOL bNew);
static LRESULT SMSSettingWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static BOOL SMSSetting_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct);
static void SMSSetting_OnActivate(HWND hwnd, UINT state);
static void SMSSetting_OnSetFocus(HWND hWnd);
static void SMSSetting_OnKillFocus(HWND hWnd);
static void SMSSetting_OnPaint(HWND hWnd);
static void SMSSetting_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags);
static void SMSSetting_OnKeyUp(HWND hWnd, UINT vk, int cRepeat, UINT flags);
static void SMSSetting_OnTimer(HWND hWnd,UINT id);
static void SMSSetting_OnCommand(HWND hWnd, int id, UINT codeNotify);
static void SMSSetting_OnDestroy(HWND hWnd);
static void SMSSetting_OnClose(HWND hWnd);
static void SMSSetting_OnVScroll(HWND hWnd, HWND hwndCtl, UINT code, int pos);
static void SMSSetting_InitVScroll(HWND hWnd);
static void SMSSetting_OnEditProfile(HWND hWnd,BOOL bSave,char* pszProfileName);
static void SMSSetting_OnEditCenter(HWND hWnd,BOOL bSave,char* pszSCA);

BOOL SMS_CreateSettingEditWnd(HWND hFrameWnd,HWND hMsgWnd,UINT uMsgCmd,char* pszCaption,char* pDefault,BOOL bText,BOOL bVMN);
static LRESULT SMSSettingEditWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static BOOL SMSSettingEdit_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct);
static void SMSSettingEdit_OnActivate(HWND hwnd, UINT state);
static void SMSSettingEdit_OnSetFocus(HWND hWnd);
static void SMSSettingEdit_OnPaint(HWND hWnd);
static void SMSSettingEdit_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags);
static void SMSSettingEdit_OnCommand(HWND hWnd, int id, UINT codeNotify);
static void SMSSettingEdit_OnDestroy(HWND hWnd);
static void SMSSettingEdit_OnClose(HWND hWnd);
static void SMSSettingEdit_OnRefresh(HWND hWnd, WPARAM wParam, LPARAM lParam);
BOOL SMS_SaveVMN(char *szPhoneNumber,int nLine);

BOOL SMS_IsIDUsed(SMS_SETTINGCHAIN* pHeader,int ID);
int  SMS_FindUseableID(SMS_SETTINGCHAIN* pHeader);
BOOL SMS_IsNameUsed(SMS_SETTINGCHAIN* pHeader,char* pszProfileName);
BOOL SMS_FindUseableName(SMS_SETTINGCHAIN* pHeader,char* pszProfileName);
BOOL SMS_ReadSetting(PSMS_SETTINGCHAIN *ppHeader);
BOOL SMS_WriteSetting(SMS_SETTING *pSetting,DWORD* pdwoffset);
SMS_SETTINGCHAIN* Setting_New(void);
int Setting_Insert(SMS_SETTINGCHAIN** ppHeader,SMS_SETTINGCHAIN* pChainNode);
void Setting_Delete(SMS_SETTINGCHAIN** ppHeader,SMS_SETTINGCHAIN* pChainNode);
void Setting_Erase(SMS_SETTINGCHAIN* pHeader);


//BOOL SMS_AppendSCA(SMS_SCA *pSCA);
//BOOL SMS_ReadSCA(SMS_SCA *pSCA,int *pnSize);
//BOOL SMS_WriteSCA(SMS_SCA *pSCA,int nSize);

typedef struct tagSMS_SettingListCreateData
{
    HMENU hMenu;
    HWND  hFrameWnd;
    HWND  hMsgWnd;
    UINT  uMsgCmd;
    PSMS_SETTINGCHAIN pHeader;
    int   nActivateID;
    BOOL  bExtern;
    HBITMAP hActive;
    HBITMAP hNewSMSProfile;
    HBITMAP hSMSProfile;
}SMS_SETTINGLISTCREATEDATA,*PSMS_SETTINGLISTCREATEDATA;


BOOL SMS_CreateSettingListWnd(HWND hFrameWnd)
{
    return SMS_CreateSetting(hFrameWnd,NULL,0,TRUE);
}
/*********************************************************************\
* Function	   SMS_DetailRegisterClass
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL SMS_SettingListRegisterClass(void)
{
    WNDCLASS wc;
        
    wc.style         = 0;
    wc.lpfnWndProc   = SMSSettingListWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = sizeof(SMS_SETTINGLISTCREATEDATA);
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName  = "SMSSettingListWndClass";
    
    if (!RegisterClass(&wc))
        return FALSE;

    return TRUE;
}
/*********************************************************************\
* Function	   SMS_CreateSettingListWnd
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL SMS_CreateSetting(HWND hFrameWnd,HWND hMsgWnd,UINT uMsgCmd,BOOL bExtern)
{
    HWND     hListWnd = NULL;
    RECT     rc;
    SMS_SETTINGLISTCREATEDATA Data;

    memset(&Data,0,sizeof(SMS_SETTINGLISTCREATEDATA));

    SMS_SettingListRegisterClass();

    Data.hFrameWnd = hFrameWnd;
    Data.bExtern = bExtern;
    Data.hMsgWnd = hMsgWnd;
    Data.uMsgCmd = uMsgCmd;

    GetClientRect(hFrameWnd,&rc);

    Data.hMenu = CreateMenu();
    hListWnd = CreateWindow(
        "SMSSettingListWndClass", 
        "",
        WS_VISIBLE | WS_CHILD,
        rc.left,  
        rc.top,  
        rc.right - rc.left,  
        rc.bottom - rc.top,  
        hFrameWnd,
        NULL,
        NULL, 
        (PVOID)&Data
        );
    
    if (!hListWnd)
    {
        return FALSE;
    }

    SetFocus(hListWnd);

    SetWindowText(hFrameWnd,IDS_SMS);

    AppendMenu(Data.hMenu,MF_ENABLED, IDM_SMSSETTINGLIST_EDIT, IDS_EDIT);
    AppendMenu(Data.hMenu,MF_ENABLED, IDM_SMSSETTINGLIST_ACTIVATE, IDS_ACTIVATE);
    AppendMenu(Data.hMenu,MF_ENABLED, IDM_SMSSETTINGLIST_DELETE, IDS_DELETE);
    PDASetMenu(hFrameWnd,Data.hMenu);
             
    SendMessage(hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_OK,1), (LPARAM)"");
    SendMessage(hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_CANCEL,0), (LPARAM)IDS_BACK);
    SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_SELECT);
    
    return TRUE;
}

/*********************************************************************\
* Function	SMSSettingListWndProc
* Purpose   Main window proc
* Params
*			hWnd: Handle of the window
*			wMsgCmd: Message ID
* Return
*			TRUE: Success
*			FALSE: Fail
* Remarks
**********************************************************************/
static LRESULT SMSSettingListWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    LRESULT lResult = TRUE;

	switch (wMsgCmd)
    {
    case WM_CREATE:
        lResult = SMSSettingList_OnCreate(hWnd,(LPCREATESTRUCT)(lParam));
        break;

    case PWM_SHOWWINDOW:
        SMSSettingList_OnActivate(hWnd,(UINT)LOWORD(wParam));
        break;

    case WM_SETFOCUS:
        SMSSettingList_OnSetFocus(hWnd);
        break;
        
    case WM_PAINT:
        SMSSettingList_OnPaint(hWnd);
        break;

    case WM_KEYDOWN:
        SMSSettingList_OnKey(hWnd,(UINT)(wParam),(int)(short)LOWORD(lParam),(UINT)HIWORD(lParam));
        break;

    case WM_COMMAND:
        SMSSettingList_OnCommand(hWnd,(int)(LOWORD(wParam)),(UINT)HIWORD(wParam));
        break;
        
    case WM_CLOSE:
        SMSSettingList_OnClose(hWnd);
        break;

    case WM_DESTROY:
        SMSSettingList_OnDestroy(hWnd);
        break;

    case WM_FRESHSETTINGLIS:
        SMSSettingList_OnRefresh(hWnd,(BOOL)HIWORD(wParam),(BOOL)LOWORD(wParam),(SMS_SETTINGCHAIN*)lParam);
        break;

    case WM_SETCENTERNR:
        SMSSettingList_OnSetCenterNR(hWnd,(BOOL)lParam);
        break;

    case WM_SETCENTERNRFORSEND:
        SMSSettingList_OnSetCenterNRForSend(hWnd,(BOOL)lParam);
        break;

    case WM_SETNRRESULT:
        SMSSettingList_OnSetNRResult(hWnd,(BOOL)LOWORD(wParam),(SMS_SETTINGCHAIN*)lParam);
        break;

    case WM_DELETEPROFILE:
        SMSSettingList_OnDeleteProfile(hWnd,(BOOL)lParam);
        break;

    default:     
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
	}

    return lResult;

}
/*********************************************************************\
* Function	SMSSettingList_OnCreate
* Purpose   WM_CREATE message handler of the main window
* Params
*			hWnd: Handle of the window
*			lpCreateStruct: Create Structure
* Return
*			TRUE: Success
*			FALSE: Fail
* Remarks
**********************************************************************/
static BOOL SMSSettingList_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
{   
    RECT rect;
    SMS_SETTINGLISTCREATEDATA *pData;
    HWND hLst;
    SMS_SETTINGCHAIN *pTemp;
    int  index;
    SIZE size;

    pData = GetUserData(hWnd);

    memcpy(pData,lpCreateStruct->lpCreateParams,sizeof(SMS_SETTINGLISTCREATEDATA));
    
    GetClientRect(pData->hFrameWnd,&rect);

    hLst = CreateWindow(
        "LISTBOX", 
        "", 
        WS_CHILD | WS_VISIBLE | WS_VSCROLL | LBS_BITMAP,
        0,
        0,
        rect.right-rect.left,
        rect.bottom-rect.top, 
        hWnd, 
        (HMENU)IDC_SMSSETTINGLIST_LIST, 
        NULL, 
        NULL);
    
    if(hLst == NULL)
        return FALSE;
    
    GetImageDimensionFromFile(SMS_BMP_ACTIVE,&size);

	pData->hActive = LoadImage(NULL, SMS_BMP_ACTIVE, IMAGE_BITMAP,
		size.cx, size.cy, LR_LOADFROMFILE);

    GetImageDimensionFromFile(SMS_BMP_NEWSMSPROFILE,&size);

    pData->hNewSMSProfile = LoadImage(NULL, SMS_BMP_NEWSMSPROFILE, IMAGE_BITMAP,
		size.cx, size.cy, LR_LOADFROMFILE);

    GetImageDimensionFromFile(SMS_BMP_SMSPROFILE,&size);

    pData->hSMSProfile = LoadImage(NULL, SMS_BMP_SMSPROFILE, IMAGE_BITMAP,
		size.cx, size.cy, LR_LOADFROMFILE);

	SMS_GetActivateID(&(pData->nActivateID));

    SMS_ReadSetting(&(pData->pHeader));

    SendMessage(hLst,LB_ADDSTRING,-1,(LPARAM)IDS_NEWSMSPROFILE);

    SendMessage(hLst,LB_SETIMAGE,MAKEWPARAM(IMAGE_BITMAP, 0),(LPARAM)pData->hNewSMSProfile);

    pTemp = pData->pHeader;

    while(pTemp)
    {
        index = SendMessage(hLst,LB_ADDSTRING,-1,(LPARAM)pTemp->Setting.szProfileName);

        SendMessage(hLst,LB_SETITEMDATA,index,(LPARAM)pTemp);
        
        SendMessage(hLst,LB_SETIMAGE,MAKEWPARAM(IMAGE_BITMAP, index),(LPARAM)pData->hSMSProfile);
        
        if(pTemp->Setting.nID == pData->nActivateID)
            SendMessage(hLst,LB_SETIMAGE,MAKEWPARAM(MAKEWORD(IMAGE_BITMAP, 1),index),(LPARAM)pData->hActive);

        pTemp = pTemp->pNext;
    }
    SendMessage(hLst,LB_SETCURSEL,0,0);
        
    return TRUE;
    
}

/*********************************************************************\
* Function	SMSSettingList_OnSetFocus
* Purpose   WM_ACTIVATE message handler of the main window
* Params
* Return    None
* Remarks
**********************************************************************/
static void SMSSettingList_OnSetFocus(HWND hWnd)
{    
    HWND hList;

    hList = GetDlgItem(hWnd,IDC_SMSSETTINGLIST_LIST);

    SetFocus(hList);
}
/*********************************************************************\
* Function	SMSSettingList_OnActivate
* Purpose   WM_ACTIVATE message handler of the main window
* Params
* Return    None
* Remarks
**********************************************************************/
static void SMSSettingList_OnActivate(HWND hWnd, UINT state)
{
    SMS_SETTINGLISTCREATEDATA *pData;
    HWND hList;
    int  nCurSel;

    pData = GetUserData(hWnd);

    PDASetMenu(pData->hFrameWnd,pData->hMenu);

    hList = GetDlgItem(hWnd,IDC_SMSSETTINGLIST_LIST);

    SetFocus(hList); 

    SendMessage(pData->hFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_BACK);

    nCurSel = SendMessage(hList,LB_GETCURSEL,0,0);
    
    if(nCurSel == 0)
    {
        SendMessage(pData->hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
        SendMessage(pData->hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_SELECT);
    }
    else
    {
        if(pData->bExtern)
            SendMessage(pData->hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_ACTIVATE);
        else
            SendMessage(pData->hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_SEND);
        SendMessage(pData->hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_OPTIONS);
    }

    SetWindowText(pData->hFrameWnd,IDS_SMS);

    return;
}
/*********************************************************************\
* Function	SMSSettingList_OnPaint
* Purpose   WM_PAINT message handler of the main window
* Params	hWnd: Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void SMSSettingList_OnPaint(HWND hWnd)
{
	HDC hdc = BeginPaint(hWnd, NULL);

    EndPaint(hWnd, NULL);

	return;
}

/*********************************************************************\
* Function	SMSSettingList_OnKey
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
static void SMSSettingList_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags)
{
    SMS_SETTINGLISTCREATEDATA *pData;
    HWND hList;
    int  nCusSel;

    pData = GetUserData(hWnd);
    
    hList = GetDlgItem(hWnd,IDC_SMSSETTINGLIST_LIST);

	switch (vk)
	{
	case VK_F10:
        if(pData->bExtern)
            PostMessage(hWnd,WM_CLOSE,0,0);
        else
        {
            SendMessage(pData->hMsgWnd,pData->uMsgCmd,FALSE,0);

            PostMessage(hWnd,WM_CLOSE,0,0);
        }
		break;

    case VK_F5:
        nCusSel = SendMessage(hList,LB_GETCURSEL,0,0);

        if(nCusSel == 0)
        {
            SMS_SETTINGCHAIN *pNewNode;
            
            pNewNode = Setting_New();

            if(pNewNode == NULL)
                break;

            pNewNode->dwoffset = 0xffff;
            pNewNode->Setting.bReplyPath = FALSE;
            pNewNode->Setting.bReport = FALSE;
            pNewNode->Setting.nConnection = 0;
            pNewNode->Setting.nPID = PID_IMPLICIT;
            pNewNode->Setting.nValidity = SMS_MAXTIME;
            SMS_FindUseableName(pData->pHeader,pNewNode->Setting.szProfileName);
            pNewNode->Setting.nID = SMS_FindUseableID(pData->pHeader);

            SMS_CreateSettingWnd(pData->hFrameWnd,hWnd,WM_FRESHSETTINGLIS,pNewNode,TRUE,TRUE);
        }
        else
            SendMessage(pData->hFrameWnd, WM_KEYDOWN, vk, MAKELPARAM(cRepeat, flags));
        break;

    case VK_RETURN:

        if(pData->bExtern)
            SendMessage(hWnd,WM_COMMAND,IDM_SMSSETTINGLIST_ACTIVATE,NULL);
        else
        {
            SMS_SETTINGCHAIN *pNode;

            nCusSel = SendMessage(hList,LB_GETCURSEL,0,0);

            if(nCusSel == 0 || nCusSel == LB_ERR)
                break;

            pNode = (SMS_SETTINGCHAIN*)SendMessage(hList,LB_GETITEMDATA,nCusSel,0);

            if(strlen(pNode->Setting.szSCA) != 0)
            {
                SendMessage(pData->hMsgWnd,pData->uMsgCmd,(WPARAM)TRUE,(LPARAM)(&(pNode->Setting)));
                
                PostMessage(hWnd,WM_CLOSE,0,0);
            }
            else
            {
                PLXConfirmWinEx(pData->hFrameWnd,hWnd,IDS_SETCENTERNR,Notify_Request,IDS_SMS,IDS_YES,IDS_NO,WM_SETCENTERNRFORSEND);
            }
        }

        break;

	default:
		PDADefWindowProc(hWnd, WM_KEYDOWN, vk, MAKELPARAM(cRepeat, flags));
		break;
	}

	return;
}
/*********************************************************************\
* Function	SMSSettingList_OnCommand
* Purpose   WM_COMMAND message handler of the main window
* Params
*			hWnd:	Handle of the window
*			id:		Id of command message
*			hwndCtl: Handle of the window which sended this message
*			codeNotify:Notify code of the message
* Return	None
* Remarks
**********************************************************************/
static void SMSSettingList_OnCommand(HWND hWnd, int id, UINT codeNotify)
{
    SMS_SETTINGLISTCREATEDATA *pData;
    int  nsel;
    HWND hList;
    SMS_SETTINGCHAIN *p,*pTemp;
    
    pData = GetUserData(hWnd);
    
    hList = GetDlgItem(hWnd,IDC_SMSSETTINGLIST_LIST);
	switch(id)
	{       
    case IDC_SMSSETTINGLIST_LIST:
        if(codeNotify == LBN_KILLFOCUS)
            PDADefWindowProc(hWnd, WM_COMMAND ,MAKEWPARAM(id,codeNotify),NULL);
        else if(codeNotify == LBN_SELCHANGE)
        {
            char szBtn[16],szMenu[16];

            nsel = SendMessage(hList,LB_GETCURSEL,0,0);
            
            SendMessage(pData->hFrameWnd,PWM_GETBUTTONTEXT,1,(LPARAM)szBtn);
            SendMessage(pData->hFrameWnd,PWM_GETBUTTONTEXT,2,(LPARAM)szMenu);

            if(nsel == 0)
            {    
                if(strcmp(szBtn,"") != 0)
                    SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,1,(LPARAM)"");
                
                
                if(strcmp(szMenu,ICON_SELECT) != 0)
                    SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,2,(LPARAM)ICON_SELECT);
            }
            else
            {             
                if(pData->bExtern)
                {
                    if(strcmp(szBtn,IDS_ACTIVATE) != 0)
                        SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,1,(LPARAM)IDS_ACTIVATE);
                }
                else
                {
                    if(strcmp(szBtn,IDS_SEND) != 0)
                        SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,1,(LPARAM)IDS_SEND);                   
                }
                
                if(strcmp(szMenu,ICON_OPTIONS) != 0)
                    SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,2,(LPARAM)ICON_OPTIONS);
            }
        }
        break;

    case IDM_SMSSETTINGLIST_ACTIVATE:
        {
            nsel = SendMessage(hList,LB_GETCURSEL,0,0);

            if(nsel == LB_ERR || nsel == 0)
                break;

            p = (SMS_SETTINGCHAIN*)SendMessage(hList,LB_GETITEMDATA,nsel,0);

            if(strlen(p->Setting.szSCA) != 0)
            {
                char szTemp[50];
                int  nOldActiveIndex;
                BOOL bGet = FALSE;
                
                pTemp = pData->pHeader;
                
                nOldActiveIndex = 1;
                
                while(pTemp)
                {
                    if(pTemp->Setting.nID == pData->nActivateID)
                    {
                        bGet = TRUE;

                        break;
                    }
                    
                    nOldActiveIndex++;
                    
                    pTemp = pTemp->pNext;
                }

                pData->nActivateID = p->Setting.nID;
                    
                if(bGet == TRUE)
                    SendMessage(hList,LB_SETIMAGE,MAKEWPARAM(MAKEWORD(IMAGE_BITMAP, 1),nOldActiveIndex),(LPARAM)NULL);

                SendMessage(hList,LB_SETIMAGE,MAKEWPARAM(MAKEWORD(IMAGE_BITMAP, 1), nsel),(LPARAM)pData->hActive);
                //change image
                //SendMessage(hList,LB_SETIMAGE,nsel,0);
                SMS_SaveActivateID(pData->nActivateID);

                sprintf(szTemp,"%s: %s",p->Setting.szProfileName,IDS_ACTIVATED);

                PLXTipsWin(NULL,NULL,0,szTemp,IDS_SMS,Notify_Success,IDS_OK,NULL,WAITTIMEOUT);
            }
            else
            {
                PLXConfirmWinEx(pData->hFrameWnd,hWnd,IDS_SETCENTERNR,Notify_Request,IDS_SMS,IDS_YES,IDS_NO,WM_SETCENTERNR);
            }
        }
        break;

    case IDM_SMSSETTINGLIST_EDIT:
        {        
            nsel = SendMessage(hList,LB_GETCURSEL,0,0);

            if(nsel == LB_ERR || nsel == 0)
                break;

            p = (SMS_SETTINGCHAIN*)SendMessage(hList,LB_GETITEMDATA,nsel,0);

            SMS_CreateSettingWnd(pData->hFrameWnd,hWnd,WM_FRESHSETTINGLIS,p,TRUE,FALSE);
        }
        break;

    case IDM_SMSSETTINGLIST_DELETE:
        { 
            char szTemp[50];
            int  nCounter;
            
            nsel = SendMessage(hList,LB_GETCURSEL,0,0);
            
            if(nsel == LB_ERR || nsel == 0)
                break;

            nCounter = SendMessage(hList,LB_GETCOUNT,0,0);

            nCounter--;

            if(nCounter == 1)
            {                
                PLXConfirmWinEx(pData->hFrameWnd,hWnd,IDS_LASTDELETE,Notify_Request,IDS_SMS,IDS_YES,IDS_NO,WM_DELETEPROFILE);

                break;
            }

            p = (SMS_SETTINGCHAIN*)SendMessage(hList,LB_GETITEMDATA,nsel,0);
            
            if(pData->nActivateID == p->Setting.nID)
            {
                sprintf(szTemp,"%s: %s?",p->Setting.szProfileName,IDS_DELETEDACTIVE);
                
                PLXConfirmWinEx(pData->hFrameWnd,hWnd,szTemp,Notify_Request,IDS_SMS,IDS_YES,IDS_NO,WM_DELETEPROFILE);

                break;
            }

            sprintf(szTemp,"%s: %s?",p->Setting.szProfileName,IDS_DELETE);
            
            PLXConfirmWinEx(pData->hFrameWnd,hWnd,szTemp,Notify_Request,IDS_SMS,IDS_YES,IDS_NO,WM_DELETEPROFILE);

        }
        break;

    default:
        break;
	}
    

	return;
}

/*********************************************************************\
* Function	SMSSettingList_OnDestroy
* Purpose   WM_DESTROY message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void SMSSettingList_OnDestroy(HWND hWnd)
{
	
    SMS_SETTINGLISTCREATEDATA *pData;
    
    pData = GetUserData(hWnd);

    if(pData->hActive)
        DeleteObject(pData->hActive);

    if(pData->hNewSMSProfile)
        DeleteObject(pData->hNewSMSProfile);

    if(pData->hSMSProfile)
        DeleteObject(pData->hSMSProfile);

    Setting_Erase(pData->pHeader);

    return;

}
/*********************************************************************\
* Function	SMSSettingList_OnClose
* Purpose   WM_CLOSE message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void SMSSettingList_OnClose(HWND hWnd)
{
    SMS_SETTINGLISTCREATEDATA *pData;
    
    pData = GetUserData(hWnd);
    
    SendMessage(pData->hFrameWnd,PWM_CLOSEWINDOW,(WPARAM)hWnd,0);

    DestroyMenu(pData->hMenu);

    DestroyWindow (hWnd);

    return;

}
/*********************************************************************\
* Function	   SMSSettingList_OnRefresh
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void SMSSettingList_OnRefresh(HWND hWnd,BOOL bRefresh,BOOL bSetActive,SMS_SETTINGCHAIN *pSetting)
{
    SMS_SETTINGLISTCREATEDATA *pData;
    int nPos;
    HWND hList;
    SMS_SETTINGCHAIN *pTemp;
    int nIndex;
    BOOL bExist = FALSE;
    int nOldActiveIndex = -1;
    
    pData = GetUserData(hWnd);

    hList = GetDlgItem(hWnd,IDC_SMSSETTINGLIST_LIST);
    
    SMS_WriteSetting(&(pSetting->Setting),&(pSetting->dwoffset));
    
    pTemp = pData->pHeader;
    
    nIndex = 1;
    while(pTemp)
    {
        if(pTemp->Setting.nID == pSetting->Setting.nID)
        {
            bExist = TRUE;
            break;
        }
        
        nIndex++;
        pTemp = pTemp->pNext;
    }

    if(bRefresh == TRUE)
    {            
        if(bExist == TRUE)
        {
            pTemp = (SMS_SETTINGCHAIN*)SendMessage(hList,LB_GETITEMDATA,nIndex,NULL);
            SendMessage(hList,LB_DELETESTRING,nIndex,0);
            Setting_Delete(&(pData->pHeader),pTemp);
        }
        
        nPos = Setting_Insert(&(pData->pHeader),pSetting);    
        SendMessage(hList,LB_INSERTSTRING,nPos+1,(LPARAM)pSetting->Setting.szProfileName);
        SendMessage(hList,LB_SETITEMDATA,nPos+1,(LPARAM)pSetting);
        SendMessage(hList,LB_SETIMAGE,MAKEWPARAM(IMAGE_BITMAP, nPos+1),(LPARAM)pData->hSMSProfile);
        //image
        if(pData->nActivateID == pSetting->Setting.nID)
            SendMessage(hList,LB_SETIMAGE,MAKEWPARAM(MAKEWORD(IMAGE_BITMAP, 1), nPos+1),(LPARAM)pData->hActive);
        SendMessage(hList,LB_SETCURSEL,nPos+1,0);
        
    }
    else
    {      
        if(bExist == TRUE)
        {
            SendMessage(hList,LB_SETTEXT,nIndex,(LPARAM)pSetting->Setting.szProfileName);
            SendMessage(hList,LB_SETCURSEL,nIndex,0);
        }

        if(bSetActive)
        {     
            BOOL bGet = FALSE;

            pTemp = pData->pHeader;

            nOldActiveIndex = 1;

            while(pTemp)
            {
                if(pTemp->Setting.nID == pData->nActivateID)
                {
                    bGet = TRUE;

                    break;
                }
                
                nOldActiveIndex++;

                pTemp = pTemp->pNext;
            }
            
            if(strlen(pSetting->Setting.szSCA) > 0)
            {
                char szTemp[50];
                
                pData->nActivateID = pSetting->Setting.nID;
                
                if(bGet)
                    SendMessage(hList,LB_SETIMAGE,MAKEWPARAM(MAKEWORD(IMAGE_BITMAP, 1),nOldActiveIndex),(LPARAM)NULL);
                
                SendMessage(hList,LB_SETIMAGE,MAKEWPARAM(MAKEWORD(IMAGE_BITMAP, 1),nIndex),(LPARAM)pData->hActive);
                
                SMS_SaveActivateID(pData->nActivateID);
                //change image
                //SendMessage(hList,LB_SETIMAGE,nsel,0);
                sprintf(szTemp,"%s: %s",pSetting->Setting.szProfileName,IDS_ACTIVATED);
                
                PLXTipsWin(NULL,NULL,0,szTemp,IDS_SMS,Notify_Success,IDS_OK,NULL,WAITTIMEOUT);
            }
        }
    }

    SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,1,(LPARAM)IDS_ACTIVATE);
    SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,2,(LPARAM)ICON_OPTIONS);
    

    return;
}
/*********************************************************************\
* Function	   SMSSettingList_OnSetNRResult
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void SMSSettingList_OnSetNRResult(HWND hWnd,BOOL bCanSend,SMS_SETTINGCHAIN *pSetting)
{
    SMS_SETTINGLISTCREATEDATA *pData;
    HWND hList;
    SMS_SETTINGCHAIN *pTemp;
    int nIndex;
    BOOL bExist = FALSE;
    int nOldActiveIndex = -1;
    
    pData = GetUserData(hWnd);

    hList = GetDlgItem(hWnd,IDC_SMSSETTINGLIST_LIST);
    
    SMS_WriteSetting(&(pSetting->Setting),&(pSetting->dwoffset));
    
    pTemp = pData->pHeader;
    
    nIndex = 1;
    while(pTemp)
    {
        if(pTemp->Setting.nID == pSetting->Setting.nID)
        {
            bExist = TRUE;
            break;
        }
        
        nIndex++;
        pTemp = pTemp->pNext;
    }

    if(bExist == TRUE)
    {
        SendMessage(hList,LB_SETTEXT,nIndex,(LPARAM)pSetting->Setting.szProfileName);
        SendMessage(hList,LB_SETCURSEL,nIndex,0);
    }
    
    if(bCanSend)
    {             
        if(strlen(pSetting->Setting.szSCA) != 0)
        {           
            SendMessage(pData->hMsgWnd,pData->uMsgCmd,(WPARAM)TRUE,(LPARAM)(&(pSetting->Setting)));
            
            PostMessage(hWnd,WM_CLOSE,0,0);

            return;
        }
    }
    
    SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,1,(LPARAM)IDS_SEND);
    SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,2,(LPARAM)ICON_OPTIONS);
    
    return;
}
/*********************************************************************\
* Function	   SMSSettingList_OnSetCenterNR
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void SMSSettingList_OnSetCenterNR(HWND hWnd,BOOL bSet)
{
    SMS_SETTINGLISTCREATEDATA *pData;
    int nsel;
    SMS_SETTINGCHAIN* p;
    HWND hList;
    
    pData = GetUserData(hWnd);

    if(bSet == FALSE)
        return;

    hList = GetDlgItem(hWnd,IDC_SMSSETTINGLIST_LIST);

    nsel = SendMessage(hList,LB_GETCURSEL,0,0);

    p = (SMS_SETTINGCHAIN*)SendMessage(hList,LB_GETITEMDATA,nsel,0);

    SMS_CreateSettingWnd(pData->hFrameWnd,hWnd,WM_FRESHSETTINGLIS,p,FALSE,FALSE);
}

/*********************************************************************\
* Function	   SMSSettingList_OnSetCenterNR
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void SMSSettingList_OnSetCenterNRForSend(HWND hWnd,BOOL bSet)
{
    SMS_SETTINGLISTCREATEDATA *pData;
    int nsel;
    SMS_SETTINGCHAIN* p;
    HWND hList;
    
    pData = GetUserData(hWnd);

    if(bSet == FALSE)
        return;

    hList = GetDlgItem(hWnd,IDC_SMSSETTINGLIST_LIST);

    nsel = SendMessage(hList,LB_GETCURSEL,0,0);

    p = (SMS_SETTINGCHAIN*)SendMessage(hList,LB_GETITEMDATA,nsel,0);

    SMS_CreateSettingWnd(pData->hFrameWnd,hWnd,WM_SETNRRESULT,p,FALSE,FALSE);
}
/*********************************************************************\
* Function	   SMSSettingList_OnDeleteProfile
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void SMSSettingList_OnDeleteProfile(HWND hWnd,BOOL bDelete)
{
    SMS_SETTINGLISTCREATEDATA *pData;
    int  nsel,ncount;
    HWND hList;
    SMS_SETTINGCHAIN *p;

    pData = GetUserData(hWnd);

    hList = GetDlgItem(hWnd,IDC_SMSSETTINGLIST_LIST);

    if(bDelete == FALSE)
        return;
    
    nsel = SendMessage(hList,LB_GETCURSEL,0,0);
    
    p = (SMS_SETTINGCHAIN*)SendMessage(hList,LB_GETITEMDATA,nsel,0);
    
    if(pData->nActivateID == p->Setting.nID)
    {
        pData->nActivateID = 0;
        SMS_SaveActivateID(pData->nActivateID);
    }

    p->Setting.nID = 0;
    
    if(SMS_WriteSetting(&(p->Setting),&(p->dwoffset)))
    {
        SendMessage(hList,LB_DELETESTRING,nsel,0);
        
        Setting_Delete(&(pData->pHeader),p);
        
        PLXTipsWin(NULL,NULL,0,IDS_DELETED,IDS_SMS,Notify_Success,IDS_OK,NULL,WAITTIMEOUT);

        ncount = SendMessage(hList,LB_GETCOUNT,0,0);

        if(ncount == 1)
        {
            SendMessage(hList,LB_SETCURSEL,0,0);
            SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,1,(LPARAM)"");
            SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,2,(LPARAM)ICON_SELECT);
        }
        else
        {
            if(nsel == ncount)
                SendMessage(hList,LB_SETCURSEL,nsel-1,0);
            else
                SendMessage(hList,LB_SETCURSEL,nsel,0);

            if(pData->bExtern)
                SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,1,(LPARAM)IDS_ACTIVATE);
            else
                SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,1,(LPARAM)IDS_SEND);
            SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,2,(LPARAM)ICON_OPTIONS);
        }
    }

    return;
}

typedef struct tagSMS_SettingCreateData
{
    HWND  hFrameWnd;
    HWND  hMsgWnd;
    UINT  uMsgCmd;
    SMS_SETTINGCHAIN *pSetting;
    BOOL  bChange;
    HWND  hFocus;
    BOOL  bFirst;
    BOOL  bNew;
    int   nCurFocus;
    WORD  wKeyCode;
    int   nRepeats;
}SMS_SETTINGCREATEDATA,*PSMS_SETTINGCREATEDATA;

/*********************************************************************\
* Function	   SMS_SettingRegisterClass
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL SMS_SettingRegisterClass(void)
{
    WNDCLASS wc;
       
    wc.style         = 0;
    wc.lpfnWndProc   = SMSSettingWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = sizeof(SMS_SETTINGCREATEDATA);
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName  = "SMSSettingWndClass";
    
    if (!RegisterClass(&wc))
        return FALSE;

    return TRUE;
}

/*********************************************************************\
* Function	   SMS_CreateSettingWnd
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL SMS_CreateSettingWnd(HWND hFrameWnd,HWND hMsgWnd,UINT uMsgCmd,SMS_SETTINGCHAIN* pSetting,BOOL bFrist,BOOL bNew)
{
    SMS_SETTINGCREATEDATA Data;
    HWND     hSettingWnd;
    RECT     rc;

    memset(&Data,0,sizeof(SMS_SETTINGCREATEDATA));

    Data.hFrameWnd = hFrameWnd;
    Data.hMsgWnd = hMsgWnd;
    Data.uMsgCmd = uMsgCmd;
    Data.bChange = FALSE;
    Data.pSetting = pSetting;
    Data.bFirst = bFrist;
    Data.bNew = bNew;
        
    SMS_SettingRegisterClass();

    GetClientRect(hFrameWnd,&rc);

    hSettingWnd = CreateWindow(
        "SMSSettingWndClass", 
        "",
        WS_VISIBLE|WS_CHILD|WS_VSCROLL,
        rc.left,
        rc.top,
        rc.right - rc.left,
        rc.bottom - rc.top,
        hFrameWnd,
        NULL,
        NULL, 
        (PVOID)&Data
        );
    
    if (!hSettingWnd)
    {
        return FALSE;
    }

    SetFocus(hSettingWnd);
    
    if(bNew)
        SetWindowText(hFrameWnd,IDS_NEWSMSPROFILE);
    else
        SetWindowText(hFrameWnd,pSetting->Setting.szProfileName);

    SendMessage(hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_OK,1), (LPARAM)"");
    SendMessage(hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_CANCEL,0), (LPARAM)IDS_BACK);
    SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_SELECT);
             
    return TRUE;
}

/*********************************************************************\
* Function	SMSSettingWndProc
* Purpose   Main window proc
* Params
*			hWnd: Handle of the window
*			wMsgCmd: Message ID
* Return
*			TRUE: Success
*			FALSE: Fail
* Remarks
**********************************************************************/
static LRESULT SMSSettingWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    LRESULT lResult = TRUE;

	switch (wMsgCmd)
    {
    case WM_CREATE:
        lResult = SMSSetting_OnCreate(hWnd,(LPCREATESTRUCT)(lParam));
        break;

    case PWM_SHOWWINDOW:
        SMSSetting_OnActivate(hWnd,(UINT)LOWORD(wParam));
        break;

    case WM_SETFOCUS:
        SMSSetting_OnSetFocus(hWnd);
        break;

    case WM_KILLFOCUS:
        SMSSetting_OnKillFocus(hWnd);
        break;
        
    case WM_PAINT:
        SMSSetting_OnPaint(hWnd);
        break;

    case WM_KEYDOWN:
        SMSSetting_OnKey(hWnd,(UINT)(wParam),(int)(short)LOWORD(lParam),(UINT)HIWORD(lParam));
        break;

    case WM_KEYUP:
        SMSSetting_OnKeyUp(hWnd,(UINT)(wParam),(int)(short)LOWORD(lParam),(UINT)HIWORD(lParam));
        break;
        
    case WM_TIMER:
        SMSSetting_OnTimer(hWnd,(WPARAM)(UINT)(wParam));
        break;

    case WM_COMMAND:
        SMSSetting_OnCommand(hWnd,(int)(LOWORD(wParam)),(UINT)HIWORD(wParam));
        break;

	case WM_VSCROLL:		
		SMSSetting_OnVScroll(hWnd, (HWND)(lParam),(UINT)(LOWORD(wParam)), (int)(short)HIWORD(wParam));
		break;
        
    case WM_CLOSE:
        SMSSetting_OnClose(hWnd);
        break;

    case WM_DESTROY:
        SMSSetting_OnDestroy(hWnd);
        break;

    case WM_EDITPROFILENAME:
        SMSSetting_OnEditProfile(hWnd,(BOOL)wParam,(char*)lParam);
        break;

    case WM_EDITCENTER:
        SMSSetting_OnEditCenter(hWnd,(BOOL)wParam,(char*)lParam);
        break;

    default:     
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
	}

    return lResult;

}
/*********************************************************************\
* Function	SMSSetting_OnCreate
* Purpose   WM_CREATE message handler of the main window
* Params
*			hWnd: Handle of the window
*			lpCreateStruct: Create Structure
* Return
*			TRUE: Success
*			FALSE: Fail
* Remarks
**********************************************************************/
static BOOL SMSSetting_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
{   
    int  y,nHeight,nWidth;
    RECT rcClient;
    HWND hBtn;
    SMS_SETTINGCREATEDATA* pData;

    pData = GetUserData(hWnd);

    memcpy(pData,lpCreateStruct->lpCreateParams,sizeof(SMS_SETTINGCREATEDATA));

    GetClientRect(hWnd,&rcClient);

    y = 0;

    nHeight = (rcClient.bottom - rcClient.top)/3;
    
    nWidth = rcClient.right - rcClient.left;
    
    hBtn = CreateWindow(
        "SPINBOXEX", 
        IDS_SMSPROFILENAME, 
        WS_CHILD | WS_VISIBLE | SSBS_LEFT | WS_TABSTOP | SSBS_ARROWRIGHT | CS_NOSYSCTRL,
        0,y,nWidth,nHeight,
        hWnd, 
        (HMENU)IDC_SMSSETTING_PROFILENAME, 
        NULL, 
        NULL);

    if(hBtn == NULL)
        return FALSE;
    
    SendMessage(hBtn, SSBM_ADDSTRING, 0, (LPARAM)pData->pSetting->Setting.szProfileName);

    if(pData->bFirst)
        pData->hFocus = hBtn;
    
    y +=  nHeight;
    
    hBtn = CreateWindow(
        "SPINBOXEX", 
        IDS_SMSCENTERNR, 
        WS_CHILD | WS_VISIBLE | SSBS_LEFT | WS_TABSTOP | SSBS_ARROWRIGHT | CS_NOSYSCTRL,
        0,y,nWidth,nHeight,
        hWnd, 
        (HMENU)IDC_SMSSETTING_SCA, 
        NULL, 
        NULL);
    
    if(hBtn == NULL)
        return FALSE;
    
    if(pData->hFocus == NULL)
        pData->hFocus = hBtn;

    SendMessage(hBtn, SSBM_ADDSTRING, 0, (LPARAM)pData->pSetting->Setting.szSCA);

    y +=  nHeight;
   
    hBtn = CreateWindow(
        "SPINBOXEX", 
        IDS_REPORT, 
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT | CS_NOSYSCTRL,
        0,y,nWidth,nHeight,
        hWnd, 
        (HMENU)IDC_SMSSETTING_REPORT, 
        NULL, 
        NULL);
    
    if(hBtn == NULL)
        return FALSE;

    SendMessage(hBtn, SSBM_ADDSTRING, 0, (LPARAM)IDS_ON);
    SendMessage(hBtn, SSBM_ADDSTRING, 0, (LPARAM)IDS_OFF);
    if(pData->pSetting->Setting.bReport)
        SendMessage(hBtn, SSBM_SETCURSEL, 0, 0);
    else
        SendMessage(hBtn, SSBM_SETCURSEL, 1, 0);
    
    y +=  nHeight;
    
    hBtn = CreateWindow(
        "SPINBOXEX", 
        IDS_VALIDITY, 
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT | CS_NOSYSCTRL,
        0,y,nWidth,nHeight,
        hWnd, 
        (HMENU)IDC_SMSSETTING_VALIDITY, 
        NULL, 
        NULL);

    if(hBtn == NULL)
        return FALSE;

    SendMessage(hBtn, SSBM_ADDSTRING, 0, (LPARAM)IDS_1HOUR);
    SendMessage(hBtn, SSBM_ADDSTRING, 0, (LPARAM)IDS_6HOURS);
    SendMessage(hBtn, SSBM_ADDSTRING, 0, (LPARAM)IDS_1DAY);
    SendMessage(hBtn, SSBM_ADDSTRING, 0, (LPARAM)IDS_3DAYS);
    SendMessage(hBtn, SSBM_ADDSTRING, 0, (LPARAM)IDS_1WEEK);
    SendMessage(hBtn, SSBM_ADDSTRING, 0, (LPARAM)IDS_MAXTIME);

    SendMessage(hBtn, SSBM_SETCURSEL, pData->pSetting->Setting.nValidity, 0);
    
    y +=  nHeight;
    
    hBtn = CreateWindow(
        "SPINBOXEX", 
        IDS_SENTAS, 
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT | CS_NOSYSCTRL,
        0,y,nWidth,nHeight,
        hWnd, 
        (HMENU)IDC_SMSSETTING_PID, 
        NULL, 
        NULL);
   
    if(hBtn == NULL)
        return FALSE;
     
    SendMessage(hBtn, SSBM_ADDSTRING, 0, (LPARAM)IDS_TEXT);
    SendMessage(hBtn, SSBM_ADDSTRING, 0, (LPARAM)IDS_FAX);
    SendMessage(hBtn, SSBM_ADDSTRING, 0, (LPARAM)IDS_SENDASEMAIL);

    switch(pData->pSetting->Setting.nPID)
    {
    case PID_IMPLICIT:
        SendMessage(hBtn, SSBM_SETCURSEL, 0, 0);
    	break;
    case PID_TELEFAX:
        SendMessage(hBtn, SSBM_SETCURSEL, 1, 0);
    	break;
    case PID_EMAIL:
        SendMessage(hBtn, SSBM_SETCURSEL, 2, 0);
        break;
    default:
        break;
    }
    
    y +=  nHeight;

    
    hBtn = CreateWindow(
        "SPINBOXEX", 
        IDS_CONNECTION, 
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT | CS_NOSYSCTRL,
        0,y,nWidth,nHeight,
        hWnd, 
        (HMENU)IDC_SMSSETTING_CONNECTION, 
        NULL, 
        NULL);

    if(hBtn == NULL)
        return FALSE;
    
    SendMessage(hBtn, SSBM_ADDSTRING, 0, (LPARAM)IDS_GSM);
    SendMessage(hBtn, SSBM_ADDSTRING, 0, (LPARAM)IDS_GPRS);
    SendMessage(hBtn, SSBM_SETCURSEL, pData->pSetting->Setting.nConnection, 0);
    
    y +=  nHeight;
        
    hBtn = CreateWindow(
        "SPINBOXEX", 
        IDS_REPLYPATH, 
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT | CS_NOSYSCTRL,
        0,y,nWidth,nHeight,
        hWnd, 
        (HMENU)IDC_SMSSETTING_REPLYPATH, 
        NULL, 
        NULL);

    if(hBtn == NULL)
        return FALSE;

    SendMessage(hBtn, SSBM_ADDSTRING, 0, (LPARAM)IDS_YES);
    SendMessage(hBtn, SSBM_ADDSTRING, 0, (LPARAM)IDS_NO);
    if(pData->pSetting->Setting.bReplyPath)
        SendMessage(hBtn, SSBM_SETCURSEL, 0, 0);
    else
        SendMessage(hBtn, SSBM_SETCURSEL, 1, 0);

    SMSSetting_InitVScroll(hWnd);
    

    return TRUE;
    
}
/*********************************************************************\
* Function	SMSSetting_OnActivate
* Purpose   WM_ACTIVATE message handler of the main window
* Params
* Return    None
* Remarks
**********************************************************************/
static void SMSSetting_OnActivate(HWND hWnd, UINT state)
{
    SMS_SETTINGCREATEDATA* pData;

    pData = GetUserData(hWnd);

    SetFocus(pData->hFocus);

    SendMessage(pData->hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
    SendMessage(pData->hFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_BACK);
    SendMessage(pData->hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_SELECT);

    if(pData->bNew)
        SetWindowText(pData->hFrameWnd,IDS_NEWSMSPROFILE);
    else
        SetWindowText(pData->hFrameWnd,pData->pSetting->Setting.szProfileName);


    return;
}
/*********************************************************************\
* Function	SMSSetting_OnSetFocus
* Purpose   WM_SETFOCUS message handler of the main window
* Params
* Return    None
* Remarks
**********************************************************************/
static void SMSSetting_OnSetFocus(HWND hWnd)
{

    SMS_SETTINGCREATEDATA* pData;

    pData = GetUserData(hWnd);

    SetFocus(pData->hFocus);
}
/*********************************************************************\
* Function	SMSSetting_OnKillFocus
* Purpose   WM_KILLFOCUS message handler of the main window
* Params
* Return    None
* Remarks
**********************************************************************/
static void SMSSetting_OnKillFocus(HWND hWnd)
{
    SMS_SETTINGCREATEDATA* pData;

    pData = GetUserData(hWnd);

	KillTimer(hWnd, TIMER_ID);
	
    pData->nRepeats = 0;
	
    pData->wKeyCode = 0;
}
/*********************************************************************\
* Function	SMSSetting_OnPaint
* Purpose   WM_PAINT message handler of the main window
* Params	hWnd: Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void SMSSetting_OnPaint(HWND hWnd)
{
	HDC hdc = BeginPaint(hWnd, NULL);

    EndPaint(hWnd, NULL);

	return;
}

/*********************************************************************\
* Function	SMSSetting_OnKey
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
static void SMSSetting_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags)
{
    HWND hWndFocus;
    HWND hWndTmp;
    int  index;
    SMS_SETTINGCREATEDATA* pData;
    int  n;

    pData = GetUserData(hWnd);

    if ((pData->nRepeats > 0) && (vk != pData->wKeyCode))
    {
        KillTimer(hWnd, TIMER_ID);
        pData->nRepeats = 0;
    }
    
    pData->wKeyCode = vk;
    pData->nRepeats++;
    
    
	switch (vk)
	{
	case VK_F10:
        
        hWndTmp = GetDlgItem(hWnd,IDC_SMSSETTING_REPORT);
        n = SendMessage(hWndTmp,SSBM_GETCURSEL,0,0);
        if(n == 0)
            pData->pSetting->Setting.bReport = TRUE;
        else
            pData->pSetting->Setting.bReport = FALSE;
        
        hWndTmp = GetDlgItem(hWnd,IDC_SMSSETTING_VALIDITY);
        
        pData->pSetting->Setting.nValidity = SendMessage(hWndTmp,SSBM_GETCURSEL,0,0);
        
        hWndTmp = GetDlgItem(hWnd,IDC_SMSSETTING_PID);
        index = SendMessage(hWndTmp,SSBM_GETCURSEL,0,0);
        switch(index)
        {
        case 0:
            pData->pSetting->Setting.nPID = PID_IMPLICIT;
            break;
            
        case 1:
            pData->pSetting->Setting.nPID = PID_TELEFAX;
            break;
            
        case 2:
            pData->pSetting->Setting.nPID = PID_EMAIL;
            break;
            
        default:
            break;
        }
        
        hWndTmp = GetDlgItem(hWnd,IDC_SMSSETTING_CONNECTION);
        pData->pSetting->Setting.nConnection = SendMessage(hWndTmp,SSBM_GETCURSEL,0,0);
        
        hWndTmp = GetDlgItem(hWnd,IDC_SMSSETTING_REPLYPATH);
        n = SendMessage(hWndTmp,SSBM_GETCURSEL,0,0);
        if(n == 0)
            pData->pSetting->Setting.bReplyPath = TRUE;
        else
            pData->pSetting->Setting.bReplyPath = FALSE;
        
        //Save

        if(pData->bNew == FALSE)
        {
            if(pData->bFirst == FALSE)
                SendMessage(pData->hMsgWnd,pData->uMsgCmd,MAKEWPARAM(TRUE,FALSE),(LPARAM)pData->pSetting);
            else
                SendMessage(pData->hMsgWnd,pData->uMsgCmd,MAKEWPARAM(FALSE,FALSE),(LPARAM)pData->pSetting);
        }
        else
            SendMessage(pData->hMsgWnd,pData->uMsgCmd,MAKEWPARAM(FALSE,TRUE),(LPARAM)pData->pSetting);
                
        PostMessage(hWnd,WM_CLOSE,0,0);
        
		break;

    case VK_F5:
        {
            int nCount;

            hWndFocus = GetFocus();
            
            while(GetParent(hWndFocus) != hWnd)
                hWndFocus = GetParent(hWndFocus);

            nCount = SendMessage(hWndFocus,SSBM_GETCOUNT,0,0);
            
            if(nCount > 1)
                CreateSpinList(pData->hFrameWnd,hWndFocus);
            else
            {
                if(hWndFocus == GetDlgItem(hWnd,IDC_SMSSETTING_PROFILENAME))
                    SMS_CreateSettingEditWnd(pData->hFrameWnd,hWnd,WM_EDITPROFILENAME,
                    pData->pSetting->Setting.szProfileName,pData->pSetting->Setting.szProfileName,TRUE,FALSE);
                else
                    SMS_CreateSettingEditWnd(pData->hFrameWnd,hWnd,WM_EDITCENTER,
                    pData->pSetting->Setting.szProfileName,pData->pSetting->Setting.szSCA,FALSE,FALSE);
            }
        }
        break;

    case VK_DOWN:       
        if (pData->nRepeats == 1)
        {
            SetTimer(hWnd, TIMER_ID, ET_REPEAT_FIRST, NULL);
        }
        hWndFocus = GetFocus();
        while(GetParent(hWndFocus) != hWnd)
            hWndFocus = GetParent(hWndFocus);
        hWndFocus = GetNextDlgTabItem(hWnd, hWndFocus, FALSE);
        SetFocus(hWndFocus);
        pData->hFocus = hWndFocus;
        SendMessage(hWnd, WM_VSCROLL, SB_LINEDOWN, NULL); 
        break;

    case VK_UP:       
        if (pData->nRepeats == 1)
        {
            SetTimer(hWnd, TIMER_ID, ET_REPEAT_FIRST, NULL);
        }
        hWndFocus = GetFocus();
        while(GetParent(hWndFocus) != hWnd)
            hWndFocus = GetParent(hWndFocus);
        hWndFocus = GetNextDlgTabItem(hWnd, hWndFocus, TRUE);
        SetFocus(hWndFocus);
        pData->hFocus = hWndFocus;
        SendMessage(hWnd, WM_VSCROLL, SB_LINEUP, NULL);
        break;

	default:
		PDADefWindowProc(hWnd, WM_KEYDOWN, vk, MAKELPARAM(cRepeat, flags));
		break;
	}

	return;
}

/*********************************************************************\
* Function	SMSSetting_OnKeyUp
* Purpose   WM_KEYUP message handler of the main window
* Params
*			hWnd: Handle of the window
*			vk:	Virtual key code
*			fDown: Is key donw
*			cRepeat: Key repeat rate
*			flags:	flag of key down
* Return	None
* Remarks
**********************************************************************/
static void SMSSetting_OnKeyUp(HWND hWnd, UINT vk, int cRepeat, UINT flags)
{
    SMS_SETTINGCREATEDATA* pData;
    
    pData = GetUserData(hWnd);
    
    pData->nRepeats = 0;
    
    switch (vk)
    {
    case VK_DOWN:
    case VK_UP:
        KillTimer(hWnd, TIMER_ID);
        break;
        
    default:
        break;
    }
}

/*********************************************************************\
* Function	SMSSetting_OnTimer
* Purpose   WM_TIMER message handler of the main window
* Params
*			hWnd: Handle of the window
*			id:	event id
* Return	None
* Remarks
**********************************************************************/
static void SMSSetting_OnTimer(HWND hWnd,UINT id)
{
    SMS_SETTINGCREATEDATA* pData;
    
    pData = GetUserData(hWnd);
    
    switch(id)
    {
    case TIMER_ID:   
        if (pData->nRepeats == 1)
        {
            KillTimer(hWnd, TIMER_ID);   
            SetTimer(hWnd, TIMER_ID, ET_REPEAT_LATER, NULL);
        }
        
        keybd_event(pData->wKeyCode, 0, 0, 0);
        
        break;                
        
    default:
        KillTimer(hWnd, id);
        break;
        
    }
}
/*********************************************************************\
* Function	SMSSetting_OnCommand
* Purpose   WM_COMMAND message handler of the main window
* Params
*			hWnd:	Handle of the window
*			id:		Id of command message
*			hwndCtl: Handle of the window which sended this message
*			codeNotify:Notify code of the message
* Return	None
* Remarks
**********************************************************************/
static void SMSSetting_OnCommand(HWND hWnd, int id, UINT codeNotify)
{

    SMS_SETTINGCREATEDATA* pData;

    pData = GetUserData(hWnd);

	switch(id)
	{
    case IDC_SMSSETTING_PROFILENAME:
        if(codeNotify == SSBN_KILLFOCUS)
            PDADefWindowProc(hWnd, WM_COMMAND ,MAKEWPARAM(id,codeNotify),NULL);
        break;

    case IDC_SMSSETTING_SCA:
        if(codeNotify == SSBN_KILLFOCUS)
            PDADefWindowProc(hWnd, WM_COMMAND ,MAKEWPARAM(id,codeNotify),NULL);
        break;

    case IDC_SMSSETTING_REPORT:
    case IDC_SMSSETTING_VALIDITY:
    case IDC_SMSSETTING_PID:
    case IDC_SMSSETTING_CONNECTION:
    case IDC_SMSSETTING_REPLYPATH :
        if(codeNotify == SSBN_CHANGE)
            pData->bChange = TRUE;
        else if(codeNotify == SSBN_KILLFOCUS)
            PDADefWindowProc(hWnd, WM_COMMAND ,MAKEWPARAM(id,codeNotify),NULL);
        break;
        
    default:
        break;
	}

	return;
}

/*********************************************************************\
* Function	SMSSetting_OnVScroll
* Purpose   WM_VSCROLL
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void SMSSetting_OnVScroll(HWND hWnd, HWND hwndCtl, UINT code, int pos)
{
    int  nY;
    RECT rcClient;
    SCROLLINFO      vsi;
    SMS_SETTINGCREATEDATA* pData;

    pData = GetUserData(hWnd);
        
    switch(code)
    {
    case SB_LINEDOWN:
       
        memset(&vsi, 0, sizeof(SCROLLINFO));
        vsi.cbSize = sizeof(SCROLLINFO);
        vsi.fMask  = SIF_ALL ;
        GetScrollInfo(hWnd, SB_VERT, &vsi);
        
        GetClientRect(hWnd,&rcClient);
        nY = (rcClient.bottom - rcClient.top)/3;
        
        pData->nCurFocus++;
        
        if(pData->nCurFocus > vsi.nMax)
        {
            ScrollWindow(hWnd,0,(vsi.nMax-vsi.nPage+1)*nY,NULL,NULL);
            UpdateWindow(hWnd);
            pData->nCurFocus = 0;
            vsi.nPos = 0;
            vsi.fMask  = SIF_POS ;
            SetScrollInfo(hWnd, SB_VERT, &vsi, TRUE);
            break;
        }
        
        if((int)(vsi.nPos + vsi.nPage - 1) <= pData->nCurFocus && pData->nCurFocus != vsi.nMax)
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
        vsi.cbSize = sizeof(SCROLLINFO);
        vsi.fMask  = SIF_ALL ;
        GetScrollInfo(hWnd, SB_VERT, &vsi);
        
        GetClientRect(hWnd,&rcClient);
        nY = (rcClient.bottom - rcClient.top)/3;
        
        pData->nCurFocus--;

        if(pData->nCurFocus < vsi.nMin)
        {
            ScrollWindow(hWnd,0,(vsi.nPage-vsi.nMax-1)*nY,NULL,NULL);
            UpdateWindow(hWnd);
            pData->nCurFocus = vsi.nMax;
            vsi.nPos = vsi.nMax-vsi.nPage+1;
            vsi.fMask  = SIF_POS ;
            SetScrollInfo(hWnd, SB_VERT, &vsi, TRUE);
            break;
        }
        
        if(vsi.nPos == pData->nCurFocus && pData->nCurFocus != vsi.nMin)
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
* Function	SMSSetting_OnDestroy
* Purpose   WM_DESTROY message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void SMSSetting_OnDestroy(HWND hWnd)
{
	
    return;

}
/*********************************************************************\
* Function	SMSSetting_OnClose
* Purpose   WM_CLOSE message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void SMSSetting_OnClose(HWND hWnd)
{
        
    SMS_SETTINGCREATEDATA* pData;

    pData = GetUserData(hWnd);

    SendMessage(pData->hFrameWnd,PWM_CLOSEWINDOW,(WPARAM)hWnd,0);

    DestroyWindow (hWnd);

    return;

}

/*********************************************************************\
* Function	SMSSetting_OnEditProfile
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
static void SMSSetting_OnEditProfile(HWND hWnd,BOOL bSave,char* pszPrrfileName)
{
    SMS_SETTINGCREATEDATA* pData;
    HWND hBtn;

    if(bSave == FALSE)
        return;

    pData = GetUserData(hWnd);

    strcpy(pData->pSetting->Setting.szProfileName,pszPrrfileName);

    hBtn = GetDlgItem(hWnd,IDC_SMSSETTING_PROFILENAME);

    SendMessage(hBtn,SSBM_SETTEXT,0,(LPARAM)pData->pSetting->Setting.szProfileName);
}
/*********************************************************************\
* Function	SMSSetting_OnEditCenter
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
static void SMSSetting_OnEditCenter(HWND hWnd,BOOL bSave,char* pszSCA)
{
    SMS_SETTINGCREATEDATA* pData;
    HWND hBtn;

    if(bSave == FALSE)
        return;

    pData = GetUserData(hWnd);

    strcpy(pData->pSetting->Setting.szSCA,pszSCA);

    hBtn = GetDlgItem(hWnd,IDC_SMSSETTING_SCA);

    SendMessage(hBtn,SSBM_SETTEXT,0,(LPARAM)pData->pSetting->Setting.szSCA);
}
/*********************************************************************\
* Function	SMS_ReadSetting
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
BOOL SMS_ReadSetting(PSMS_SETTINGCHAIN *ppSMSSetting)
{   
    char szOldPath[PATH_MAXLEN];
    int f;
    struct stat buf;
    LONG    datalen;
    PSMS_SETTINGCHAIN pNewNode = NULL;
    SMS_SETTING       temp;


    szOldPath[0] = 0;
    
    getcwd(szOldPath,PATH_MAXLEN);

    chdir(PATH_DIR_SMS);

    memset(&buf,0,sizeof(struct stat));

    stat(SMS_FILENAME_SETTING,&buf);

    datalen = buf.st_size;

    if(datalen == 0)
    {
        chdir(szOldPath);
        
        return TRUE;
    }
    
    f = open(SMS_FILENAME_SETTING,O_RDONLY);

    if( f == -1 )
    {
        chdir(szOldPath);
        
        return FALSE;
    }
    else
    {
        while(datalen > 0)
        {
            read(f,&temp,sizeof(SMS_SETTING));

            if(temp.nID != 0)
            {   
                pNewNode = Setting_New();
                
                if(pNewNode == NULL)
                {
                    close(f);
                    
                    chdir(szOldPath);

                    return FALSE;
                }

                memcpy(&(pNewNode->Setting),&temp,sizeof(SMS_SETTING));

                pNewNode->dwoffset = buf.st_size - datalen;

                Setting_Insert(ppSMSSetting,pNewNode);
            }

            datalen -= sizeof(SMS_SETTING);
        }
        
        close(f);
        
        chdir(szOldPath);
        
        return TRUE;
    }
    
    return FALSE;
}

/*********************************************************************\
* Function	SMS_SaveActivateID
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
BOOL SMS_SaveActivateID(int nID)
{
    char szOldPath[PATH_MAXLEN];
    int f;

    szOldPath[0] = 0;
    
    getcwd(szOldPath,PATH_MAXLEN);

    chdir(PATH_DIR_SMS);

	f = open(SMS_FILENAME_ACTIVATEID,O_RDWR|O_CREAT,S_IRWXU);

	if( f == -1)
	{
        chdir(szOldPath);
        
        return FALSE;
	}

	write(f,&nID,sizeof(int));

	close(f);

    chdir(szOldPath);

	return TRUE;
}
/*********************************************************************\
* Function	SMS_GetActivateID
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
BOOL SMS_GetActivateID(int* pnID)
{
    char szOldPath[PATH_MAXLEN];
    int f;

    szOldPath[0] = 0;
    
    getcwd(szOldPath,PATH_MAXLEN);

    chdir(PATH_DIR_SMS);

	f = open(SMS_FILENAME_ACTIVATEID,O_RDONLY);

	if( f == -1)
	{
        chdir(szOldPath);
        
        return FALSE;
	}

	read(f,pnID,sizeof(int));

	close(f);

    chdir(szOldPath);

	return TRUE;
}
/*********************************************************************\
* Function	SMS_GetActivateSetting
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
BOOL SMS_GetActivateSetting(SMS_SETTING *pSetting)
{   
    char szOldPath[PATH_MAXLEN];
    int f;
    struct stat buf;
    LONG    datalen;
    PSMS_SETTINGCHAIN pNewNode = NULL;
	int				  nActivateID;

    szOldPath[0] = 0;
    
    getcwd(szOldPath,PATH_MAXLEN);

    chdir(PATH_DIR_SMS);

    memset(&buf,0,sizeof(struct stat));

    stat(SMS_FILENAME_SETTING,&buf);

    datalen = buf.st_size;

    if(datalen == 0)
    {
        chdir(szOldPath);
        
        return FALSE;
    }
	
	if(SMS_GetActivateID(&nActivateID) == FALSE)
    {
        chdir(szOldPath);
		
        return FALSE;
    }
    
    f = open(SMS_FILENAME_SETTING,O_RDONLY);

    if( f == -1 )
    {
        chdir(szOldPath);
        
        return FALSE;
    }
    else
    {
        while(datalen > 0)
        {
            read(f,pSetting,sizeof(SMS_SETTING));

            if(pSetting->nID != 0 && pSetting->nID == nActivateID)
            {   
				
				close(f);
				
				chdir(szOldPath);
				
				return TRUE;
            }

            datalen -= sizeof(SMS_SETTING);
        }
        
        close(f);
        
        chdir(szOldPath);
        
        return FALSE;
    }
    
    return FALSE;
}
///*********************************************************************\
//* Function	SMS_ReadSCA
//* Purpose   
//* Params	
//* Return	
//* Remarks
//**********************************************************************/
//BOOL SMS_ReadSCA(SMS_SCA *pSCA,int *pnSize)
//{
//    char szOldPath[PATH_MAXLEN];
//    int f;
//    struct stat statbuf;
//
//    szOldPath[0] = 0;
//    
//    getcwd(szOldPath,PATH_MAXLEN);
//
//    chdir(PATH_DIR_SMS);
//
//    if(pSCA == NULL)
//    {
//        memset(&statbuf,0,sizeof(struct stat));
//        stat(SMS_FILENAME_SCA,&statbuf);
//        *pnSize = statbuf.st_size;
//    }
//    else
//    {
//        f = open(SMS_FILENAME_SCA,O_RDONLY);
//        
//        if( f == -1 )
//            return FALSE;
//
//        read(f,pSCA,*pnSize);
//
//        close(f);
//    }
//
//    chdir(szOldPath);
//
//    return TRUE;
//}
///*********************************************************************\
//* Function	SMS_WriteSCA
//* Purpose   
//* Params	
//* Return	
//* Remarks
//**********************************************************************/
//BOOL SMS_WriteSCA(SMS_SCA *pSCA,int nSize)
//{
//    char szOldPath[PATH_MAXLEN];
//    int f;
//
//    szOldPath[0] = 0;
//    
//    getcwd(szOldPath,PATH_MAXLEN);
//
//    chdir(PATH_DIR_SMS);
//
//    if(pSCA == NULL)
//    {
//        remove(SMS_FILENAME_SCA);
//    }
//    else
//    {
//        f = open(SMS_FILENAME_SCA,O_RDWR|O_CREAT,S_IRWXU);
//        
//        if( f == -1 )
//            return FALSE;
//
//        write(f,pSCA,nSize);
//
//        close(f);
//
//        truncate(SMS_FILENAME_SCA,nSize);
//    }
//
//    chdir(szOldPath);
//
//    return TRUE;
//}
///*********************************************************************\
//* Function	SMS_AppendSCA
//* Purpose   
//* Params	
//* Return	
//* Remarks
//**********************************************************************/
//BOOL SMS_AppendSCA(SMS_SCA *pSCA)
//{
//    char szOldPath[PATH_MAXLEN];
//    int f;
//
//    if(pSCA == NULL)
//    {
//        return FALSE;
//    }
//    else
//    {
//        szOldPath[0] = 0;
//        
//        getcwd(szOldPath,PATH_MAXLEN);
//        
//        chdir(PATH_DIR_SMS);
//
//        f = open(SMS_FILENAME_SCA,O_RDWR|O_CREAT|O_APPEND,S_IRWXU);
//        
//        if( f == -1 )
//            return FALSE;
//
//        write(f,pSCA,sizeof(SMS_SCA));
//
//        close(f);
//
//        chdir(szOldPath);
//        
//        return TRUE;
//    }
//
//}
/*********************************************************************\
* Function	SMS_WriteSetting
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
BOOL SMS_WriteSetting(SMS_SETTING *pSetting,DWORD* pdwoffset)
{
    char szOldPath[PATH_MAXLEN];
    int f;
    struct stat buf;

    if(pSetting == NULL)
    {
        return FALSE;
    }
    else
    {
        memset(szOldPath,0,PATH_MAXLEN);
        
        getcwd(szOldPath,PATH_MAXLEN);
        
        chdir(PATH_DIR_SMS);

        f = open(SMS_FILENAME_SETTING,O_RDWR|O_CREAT,S_IRWXU);
        
        if( f == -1 )
        {
            chdir(szOldPath);

            return FALSE;
        }
        
        memset(&buf,0,sizeof(struct stat));
        
        stat(SMS_FILENAME_SETTING,&buf);
        
        if(*pdwoffset == 0xffff)
        {    
            lseek(f,0,SEEK_END);

            write(f,pSetting,sizeof(SMS_SETTING));

            *pdwoffset = buf.st_size;
        }
        else
        {
            lseek(f,*pdwoffset,SEEK_SET);
            
            write(f,pSetting,sizeof(SMS_SETTING));
        }

        close(f);

        chdir(szOldPath);
    }

    return TRUE;
}


typedef struct tagSMS_SettingEditCreateData
{
    HWND  hFrameWnd;
    HWND  hMsgWnd;
    UINT  uMsgCmd;
    BOOL  bText;
    BOOL  bVMN;
    char  szString[42];
    char  szCaption[50];
}SMS_SETTINGEDITCREATEDATA,*PSMS_SETTINGEDITCREATEDATA;

/*********************************************************************\
* Function	   SMS_SettingEditRegisterClass
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL SMS_SettingEditRegisterClass(void)
{
    WNDCLASS wc;
       
    wc.style         = 0;
    wc.lpfnWndProc   = SMSSettingEditWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = sizeof(SMS_SETTINGEDITCREATEDATA);
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName  = "SMSSettingEditWndClass";
    
    if (!RegisterClass(&wc))
        return FALSE;

    return TRUE;
}
/*********************************************************************\
* Function	   SMS_CreateSettingEditWnd
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL SMS_CreateSettingEditWnd(HWND hFrameWnd,HWND hMsgWnd,UINT uMsgCmd,char* pszCaption,
                              char* pDefault,BOOL bText,BOOL bVMN)
{    
    HWND     hWnd = NULL;
    RECT     rc;
    SMS_SETTINGEDITCREATEDATA Data;

    memset(&Data,0,sizeof(SMS_SETTINGEDITCREATEDATA));

    Data.hFrameWnd = hFrameWnd;
    Data.hMsgWnd = hMsgWnd;
    Data.uMsgCmd = uMsgCmd;
    Data.bText = bText;
    Data.bVMN = bVMN;
    strcpy(Data.szString,pDefault);
    strncpy(Data.szCaption,pszCaption,50);
    Data.szCaption[49] = 0;

    GetClientRect(hFrameWnd,&rc);

    SMS_SettingEditRegisterClass();

    hWnd = CreateWindow(
        "SMSSettingEditWndClass", 
        "",
        WS_VISIBLE | WS_CHILD,
        rc.left,  
        rc.top,  
        rc.right - rc.left,  
        rc.bottom - rc.top,  
        hFrameWnd,
        NULL,
        NULL, 
        (PVOID)&Data
        );
    
    if (!hWnd)
    {
        return FALSE;
    }

    SetFocus(hWnd);

    SetWindowText(hFrameWnd,pszCaption);

    SendMessage(hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_OK,1), (LPARAM)IDS_SAVE);
    SendMessage(hFrameWnd,PWM_SETBUTTONTEXT,2,(LPARAM)"");

    return TRUE;
        
}

/*********************************************************************\
* Function	SMSSettingEditWndProc
* Purpose   Main window proc
* Params
*			hWnd: Handle of the window
*			wMsgCmd: Message ID
* Return
*			TRUE: Success
*			FALSE: Fail
* Remarks
**********************************************************************/
static LRESULT SMSSettingEditWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    LRESULT lResult = TRUE;

	switch (wMsgCmd)
    {
    case WM_CREATE:
        lResult = SMSSettingEdit_OnCreate(hWnd,(LPCREATESTRUCT)(lParam));
        break;

    case PWM_SHOWWINDOW:
        SMSSettingEdit_OnActivate(hWnd,(UINT)LOWORD(wParam));
        break;

    case WM_SETFOCUS:
        SMSSettingEdit_OnSetFocus(hWnd);
        break;
        
    case WM_PAINT:
        SMSSettingEdit_OnPaint(hWnd);
        break;

    case WM_KEYDOWN:
        SMSSettingEdit_OnKey(hWnd,(UINT)(wParam),(int)(short)LOWORD(lParam),(UINT)HIWORD(lParam));
        break;

    case WM_COMMAND:
        SMSSettingEdit_OnCommand(hWnd,(int)(LOWORD(wParam)),(UINT)HIWORD(wParam));
        break;
        
    case WM_CLOSE:
        SMSSettingEdit_OnClose(hWnd);
        break;

    case WM_DESTROY:
        SMSSettingEdit_OnDestroy(hWnd);
        break;

    default:     
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
	}

    return lResult;

}
/*********************************************************************\
* Function	SMSSettingEdit_OnCreate
* Purpose   WM_CREATE message handler of the main window
* Params
*			hWnd: Handle of the window
*			lpCreateStruct: Create Structure
* Return
*			TRUE: Success
*			FALSE: Fail
* Remarks
**********************************************************************/
static BOOL SMSSettingEdit_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
{    
    HWND hEdit;
    RECT rc;
    SMS_SETTINGEDITCREATEDATA *pData;
    IMEEDIT ie;
    DWORD dwStyle;

    pData = GetUserData(hWnd);

    memcpy(pData,lpCreateStruct->lpCreateParams,sizeof(SMS_SETTINGEDITCREATEDATA));

    GetClientRect(hWnd,&rc);
    
    memset(&ie, 0, sizeof(IMEEDIT));
    
    ie.hwndNotify	= (HWND)hWnd;    
    ie.dwAttrib	    = 0;                
    ie.dwAscTextMax	= 0;
    ie.dwUniTextMax	= 0;
    ie.wPageMax	    = 0;        
    ie.pszCharSet	= NULL;
    ie.pszTitle	    = NULL;
    
    if(pData->bText == TRUE)
    {
        dwStyle = WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL | WS_TABSTOP | ES_TITLE;
        ie.pszImeName	= NULL;
    }
    else
    {
        dwStyle = WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL | WS_TABSTOP | ES_TITLE;
        ie.pszImeName	= "Phone";
    }
    
    hEdit = CreateWindow(
        "IMEEDIT",
        "",
        dwStyle,
        rc.left,
        rc.top,
        rc.right - rc.left,
        (rc.bottom - rc.top) / 3,
        hWnd,
        (HMENU)IDC_SMSSETTING_EDIT,
        NULL,
        (PVOID)&ie);
    
    if(hEdit == NULL)
        return FALSE;
    
    
    if(pData->bText == TRUE)
    {
        SendMessage(hEdit, EM_LIMITTEXT, (WPARAM)SMS_PROFILE_NAMELEN-1, NULL);
        SendMessage(hEdit, EM_SETTITLE, 0, (LPARAM)IDS_SMSPROFILENAME);
    }
    else
    {
        SendMessage(hEdit, EM_LIMITTEXT, (WPARAM)ME_PHONENUM_LEN-1, NULL);
        if(pData->bVMN == TRUE)
            SendMessage(hEdit, EM_SETTITLE, 0, (LPARAM)IDS_VOICEMAILBOXNR);
        else
            SendMessage(hEdit, EM_SETTITLE, 0, (LPARAM)IDS_SMSCENTERNR);
    }

    SetWindowText(hEdit, pData->szString);

    SendMessage(hEdit, EM_SETSEL, -1, -1);
    
    return TRUE;
    
}
/*********************************************************************\
* Function	SMSSettingEdit_OnActivate
* Purpose   WM_ACTIVATE message handler of the main window
* Params
* Return    None
* Remarks
**********************************************************************/
static void SMSSettingEdit_OnActivate(HWND hWnd, UINT state)
{
    SMS_SETTINGEDITCREATEDATA *pData;

    pData = GetUserData(hWnd);

    SetFocus(GetDlgItem(hWnd,IDC_SMSSETTING_EDIT));

    SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,1,(LPARAM)IDS_SAVE);
    SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,2,(LPARAM)"");

    SetWindowText(pData->hFrameWnd,pData->szCaption);

    return;
}
/*********************************************************************\
* Function	SMSSettingEdit_OnSetFocus
* Purpose   WM_INITMENU message handler of the main window
* Params	hWnd: Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void SMSSettingEdit_OnSetFocus(HWND hWnd)
{
    SMS_SETTINGEDITCREATEDATA *pData;

    pData = GetUserData(hWnd);

    SetFocus(GetDlgItem(hWnd,IDC_SMSSETTING_EDIT));

    return;
}
/*********************************************************************\
* Function	SMSSettingEdit_OnPaint
* Purpose   WM_PAINT message handler of the main window
* Params	hWnd: Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void SMSSettingEdit_OnPaint(HWND hWnd)
{
    HDC hdc = BeginPaint(hWnd, NULL);
    
    EndPaint(hWnd, NULL);

	return;
}

/*********************************************************************\
* Function	SMSSettingEdit_OnKey
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
static void SMSSettingEdit_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags)
{
    SMS_SETTINGEDITCREATEDATA *pData;
    HWND hEdit;

    pData = GetUserData(hWnd);

	switch (vk)
	{
	case VK_F10:
        SendMessage(pData->hMsgWnd,pData->uMsgCmd,FALSE,NULL);
		PostMessage(hWnd,WM_CLOSE,NULL,NULL);
		break;

    case VK_RETURN:
        hEdit = GetDlgItem(hWnd,IDC_SMSSETTING_EDIT);
        GetWindowText(hEdit,pData->szString,ME_PHONENUM_LEN);

        if(pData->bVMN == TRUE)
            SMS_SaveVMN(pData->szString,GetCurrentAltLine());
        else if(GetWindowTextLength(hEdit) == 0)
        {
            if(pData->bText == TRUE)
            {
                PLXTipsWin(NULL,NULL,0,IDS_DEFINENAME,pData->szString,Notify_Alert,IDS_OK,NULL,WAITTIMEOUT);
                break;
            }
            else
            {
                PLXTipsWin(NULL,NULL,0,IDS_DEFINENUM,pData->szString,Notify_Alert,IDS_OK,NULL,WAITTIMEOUT);
                break;
            }
        }
        SendMessage(pData->hMsgWnd,pData->uMsgCmd,TRUE,(LPARAM)pData->szString);
        
		PostMessage(hWnd,WM_CLOSE,NULL,NULL);
        break;

	default:
		PDADefWindowProc(hWnd, WM_KEYDOWN, vk, MAKELPARAM(cRepeat, flags));
		break;
	}

	return;
}
/*********************************************************************\
* Function	SMSSettingEdit_OnCommand
* Purpose   WM_COMMAND message handler of the main window
* Params
*			hWnd:	Handle of the window
*			id:		Id of command message
*			hwndCtl: Handle of the window which sended this message
*			codeNotify:Notify code of the message
* Return	None
* Remarks
**********************************************************************/
static void SMSSettingEdit_OnCommand(HWND hWnd, int id, UINT codeNotify)
{
	switch(id)
	{       
    case IDC_SMSSETTING_EDIT:
        if(codeNotify == EN_CHANGE)
        {
            SMS_SETTINGEDITCREATEDATA *pData;
            
            pData = GetUserData(hWnd);
         
            if(pData->bText == TRUE)
            {
                HWND hEdit;

                hEdit = GetDlgItem(hWnd,IDC_SMSSETTING_EDIT);
                GetWindowText(hEdit,pData->szString,ME_PHONENUM_LEN);
                
                strcpy(pData->szCaption,pData->szString);
                SetWindowText(pData->hFrameWnd,pData->szString);
            }
        }
        break;

    default:
        break;
	}

	return;
}
/*********************************************************************\
* Function	SMSSettingEdit_OnDestroy
* Purpose   WM_DESTROY message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void SMSSettingEdit_OnDestroy(HWND hWnd)
{

    return;

}
/*********************************************************************\
* Function	SMSSettingEdit_OnClose
* Purpose   WM_CLOSE message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void SMSSettingEdit_OnClose(HWND hWnd)
{
    SendMessage(GetParent(hWnd),PWM_CLOSEWINDOW,(WPARAM)hWnd,NULL);

    DestroyWindow (hWnd);
	
    return;

}

/*********************************************************************\
* Function	   SMSSetting_InitVScroll
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void SMSSetting_InitVScroll(HWND hWnd)
{
    SCROLLINFO      vsi;
    SMS_SETTINGCREATEDATA* pData;

    pData = GetUserData(hWnd);
    
    memset(&vsi, 0, sizeof(SCROLLINFO));
   
    vsi.cbSize = sizeof(vsi);
    vsi.fMask  = SIF_ALL ;
    vsi.nMin   = 0;
    vsi.nPage  = 3;
    vsi.nMax   = 6;
    vsi.nPos = 0;
    
    SetScrollInfo(hWnd, SB_VERT, &vsi, TRUE);

    if(pData->bFirst)
        pData->nCurFocus = 0;
    else
        pData->nCurFocus = 1;

	return;
}
/*********************************************************************\
* Function	SMS_FindUseableID
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
int SMS_FindUseableID(SMS_SETTINGCHAIN* pHeader)
{
    int nIDTemp = 1;

    while( nIDTemp )
    {
        if(SMS_IsIDUsed(pHeader,nIDTemp))
            nIDTemp++;
        else
            return nIDTemp;
    }

    return nIDTemp;
}
/*********************************************************************\
* Function	SMS_IsIDUsed
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
BOOL SMS_IsIDUsed(SMS_SETTINGCHAIN* pHeader,int ID)
{
    SMS_SETTINGCHAIN* pTemp;

    pTemp = pHeader;

    while(pTemp)
    {
        if(pTemp->Setting.nID == ID)
        {
            return TRUE;
        }
        pTemp = pTemp->pNext;
    }
    return FALSE;
}
/*********************************************************************\
* Function	SMS_IsNameUsed
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
BOOL SMS_FindUseableName(SMS_SETTINGCHAIN* pHeader,char* pszProfileName)
{
    int nLoop;

    nLoop = 1;

    while( nLoop >= 0 )
    {
        sprintf(pszProfileName,"%s %d",IDS_SMSPROFILE,nLoop);

        if(SMS_IsNameUsed(pHeader,pszProfileName))
            nLoop++;
        else
            return TRUE;
    }

    return FALSE;
}
/*********************************************************************\
* Function	SMS_IsNameUsed
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
BOOL SMS_IsNameUsed(SMS_SETTINGCHAIN* pHeader ,char* pszProfileName)
{
    SMS_SETTINGCHAIN* pTemp;

    pTemp = pHeader;

    while(pTemp)
    {
        if(strcmp(pTemp->Setting.szProfileName,pszProfileName) == 0)
        {
            return TRUE;
        }
        pTemp = pTemp->pNext;
    }
    return FALSE;
}

/*********************************************************************\
* Function	   Setting_New
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
SMS_SETTINGCHAIN* Setting_New(void)
{
    SMS_SETTINGCHAIN* p = NULL;

    p = (SMS_SETTINGCHAIN*)malloc(sizeof(SMS_SETTINGCHAIN));

    if(p == NULL)
        return NULL;

    memset(p,0,sizeof(SMS_SETTINGCHAIN));

    return p;
}
/*********************************************************************\
* Function	   Setting_Insert
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
int Setting_Insert(SMS_SETTINGCHAIN** ppHeader,SMS_SETTINGCHAIN* pChainNode)
{
    SMS_SETTINGCHAIN* p;
    int nPos = 0;

    if(*ppHeader)
    {
        p = *ppHeader;

        while( p )
        {
            if(strcmp(pChainNode->Setting.szProfileName,p->Setting.szProfileName) < 0)
            {
                if( p == *ppHeader ) // before the first one node
                {
                    pChainNode->pNext = *ppHeader;
                    pChainNode->pPioneer = NULL;
                    (*ppHeader)->pPioneer = pChainNode;
                    *ppHeader = pChainNode;
                }
                else
                {
                    p->pPioneer->pNext = pChainNode;
                    pChainNode->pPioneer = p->pPioneer;
                    pChainNode->pNext = p;
                    p->pPioneer = pChainNode;
                }
                return nPos;
            }
            
            nPos++;
            
            if( p->pNext )
            {
                p = p->pNext;
            }
            else
                break;
        }

        p->pNext = pChainNode;
        pChainNode->pPioneer = p;
        pChainNode->pNext = NULL;
    }
    else // is empty chain
    {
        *ppHeader = pChainNode;
        pChainNode->pNext = NULL;
        pChainNode->pPioneer = NULL;
    }

    return nPos;
}
/*********************************************************************\
* Function	   Setting_Delete
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
void Setting_Delete(SMS_SETTINGCHAIN** ppHeader,SMS_SETTINGCHAIN* pChainNode)
{
    if( pChainNode == *ppHeader )
    {
        if(pChainNode->pNext)
        {
            *ppHeader = pChainNode->pNext;
            pChainNode->pNext->pPioneer = NULL;
        }
        else
            *ppHeader = NULL;
    }
    else if( pChainNode->pNext == NULL )
    {
        pChainNode->pPioneer->pNext = NULL;
    }
    else
    {
        pChainNode->pPioneer->pNext = pChainNode->pNext;
        pChainNode->pNext->pPioneer = pChainNode->pPioneer;
    }

    free(pChainNode);
    pChainNode = NULL;
}
/*********************************************************************\
* Function	   Setting_Erase
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
void Setting_Erase(SMS_SETTINGCHAIN* pHeader)
{
    SMS_SETTINGCHAIN* p;    
    SMS_SETTINGCHAIN* ptemp;

    p = pHeader;

    while( p )
    {
        ptemp = p->pNext;
        
        free(p);
        
        p = ptemp;
    }
}

/*********************************************************************\
* Function	   SMS_VMNSetting
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL SMS_VMNSetting(HWND hFrameWnd)
{    
    BOOL bRet;
    char szMailbox[ME_PHONENUM_LEN];

    memset(szMailbox,0,ME_PHONENUM_LEN);

    GetVoiceMailBox(szMailbox, ME_PHONENUM_LEN);

    bRet = SMS_CreateSettingEditWnd(hFrameWnd,NULL,0,(char*)IDS_MESSAGING,szMailbox,FALSE,TRUE);

    return bRet;
        
}
/*********************************************************************\
* Function	   SMS_VMNSettingEx
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL SMS_VMNSettingEx(HWND hFrameWnd,HWND hMsgWnd,UINT uMsgCmd)
{    
    BOOL bRet;
    char szMailbox[ME_PHONENUM_LEN];

    memset(szMailbox,0,ME_PHONENUM_LEN);

    GetVoiceMailBox(szMailbox, ME_PHONENUM_LEN);

    bRet = SMS_CreateSettingEditWnd(hFrameWnd,hMsgWnd,uMsgCmd,(char*)IDS_VOICEMAILBOX,szMailbox,FALSE,TRUE);

    return bRet;
        
}
/*********************************************************************\
* Function	   GetVoiceMailBox
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL GetVoiceMailBox(char *PhoneNumber, int iPhoneNumberLen)
{
    char szOldPath[PATH_MAXLEN];
    int f;
    int nline,len;
    struct stat buf;
    char szVMNFile[PATH_MAXLEN];

    memset(PhoneNumber,0,iPhoneNumberLen);

    memset(szOldPath,0,PATH_MAXLEN);
    
    getcwd(szOldPath,PATH_MAXLEN);

    chdir(PATH_DIR_SMS);

    szVMNFile[0] = 0;

    nline = GetCurrentAltLine();

    sprintf(szVMNFile,"%s%d",SMS_FILENAME_VMN,nline);

    f = open(szVMNFile,O_RDWR,S_IRWXU);
    
    if( f == -1 )
    {
        chdir(szOldPath);
                
        SMS_GetSIMVMN(PhoneNumber,nline);

        return TRUE;
    }
    else
    {
        memset(&buf,0,sizeof(struct stat));
        
        stat(szVMNFile,&buf);

        len = min((int)(buf.st_size),iPhoneNumberLen);
        
        read(f,PhoneNumber,len);

        close(f);

        chdir(szOldPath);
        
        PhoneNumber[iPhoneNumberLen-1] = 0;
        
        return TRUE;
    }
}
/*********************************************************************\
* Function	   SMS_SaveVMN
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL SMS_SaveVMN(char *szPhoneNumber,int nLine)
{
    char szOldPath[PATH_MAXLEN];
    char szVMNFile[PATH_MAXLEN];
    int f;

    memset(szOldPath,0,PATH_MAXLEN);
    
    getcwd(szOldPath,PATH_MAXLEN);

    chdir(PATH_DIR_SMS);

    szVMNFile[0] = 0;

    sprintf(szVMNFile,"%s%d",SMS_FILENAME_VMN,nLine);

    remove(szVMNFile);

    f = open(szVMNFile,O_RDWR|O_CREAT,S_IRWXU);
    
    if( f == -1 )
    {
        chdir(szOldPath);
        
        return FALSE;
    }
    else
    {
        write(f,szPhoneNumber,strlen(szPhoneNumber)+1);

        close(f);

        chdir(szOldPath);
        
        return TRUE;
    }
}
