/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : 
 *
 * Purpose  : ¡¡¡¡
 *            
\**************************************************************************/
#include "smsglobal.h"

#define CB_MSG_MAX              50

#define CB_CHANNELNO_LEN        3
#define CB_CHANNELNAME_LEN      20

#define TIMER_ID                1
#define ET_REPEAT_FIRST         300
#define ET_REPEAT_LATER         100

typedef struct  tagCB_VIEW
{
    HWND     hFrameWnd;
    short    ID;
    HMENU    hMenu;
    BOOL     bPre;
    BOOL     bNext;
    HWND     hWndTextView;
    HWND     hMsgWnd;
    char*    pszText;
    HBITMAP  hLeftArrow;
    HBITMAP  hRightArrow;
}CB_VIEW,*PCB_VIEW;

#define IDC_EXIT            200
#define IDM_CB_OPEN         101
#define IDM_CB_SUBSCRIBE    102
#define IDM_CB_HOTMARK      103
#define IDM_CB_ADD          1041
#define IDM_CB_EDIT         1042 
#define IDM_CB_DELETE       1043
#define IDM_CB_DELALL       1044
#define IDM_CB_SETTING      105
#define IDC_CB_LIST         300

static LRESULT CALLBACK CBWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static BOOL CB_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct);
static void CB_OnActivate(HWND hwnd, UINT state);
static void CB_OnInitMenu(HWND hWnd);
static void CB_OnSetFocus(HWND hWnd);
static void CB_OnPaint(HWND hWnd);
static void CB_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags);
static void CB_OnCommand(HWND hWnd, int id, UINT codeNotify);
static void CB_OnDestroy(HWND hWnd);
static void CB_OnClose(HWND hWnd);
static void CB_OnAddTopic(HWND hWnd,BOOL bNew,char* pszNr);
static void CB_OnModifyTopic(HWND hWnd,BOOL bModify,char* pszNr);
static void CB_OnDeleteTopic(HWND hWnd,BOOL bDel);
static void CB_OnParalleView(HWND hWnd,UINT message);
static void CB_GetInfo(HWND hWnd,int index,BOOL *pPer,BOOL *pNext);
static void CB_ParallelInfo(HWND hWnd);
static int  CB_GetNextText(HWND hWnd,int index,BOOL bDirtection);
static BOOL CB_IsExistMsg(int nCB);
static void CB_SetLeftSoftKey(HWND hWnd);

#define IDC_CHANGE              100
#define IDC_CBSETTING_RECEPTION 301
#define IDC_CBSETTING_LANGUAGE  302
#define IDC_CBSETTING_DETECTION 303

typedef struct  tagCB_SETTING
{
    HWND     hFrameWnd;
    HWND     hFocus;
    CELL_BROADCAST CB_Setting;
    int      nTopicDetection;
    WORD     wKeyCode;
    int      nRepeats;
}CB_SETTING,*PCB_SETTING;

BOOL CB_CreateSettingWnd(HWND hParent);
static LRESULT CALLBACK CBSettingWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static BOOL CBSetting_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct);
static void CBSetting_OnActivate(HWND hwnd, UINT state);
static void CBSetting_OnPaint(HWND hWnd);
static void CBSetting_OnKillFocus(HWND hWnd);
static void CBSetting_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags);
static void CBSetting_OnKeyUp(HWND hWnd, UINT vk, int cRepeat, UINT flags);
static void CBSetting_OnTimer(HWND hWnd,UINT id);
static void CBSetting_OnCommand(HWND hWnd, int id, UINT codeNotify);
static void CBSetting_OnDestroy(HWND hWnd);
static void CBSetting_OnClose(HWND hWnd);
static void CBSetting_OnSetFocus(HWND hWnd);
static void CBSetting_OnSelectLanguage(HWND hWnd,BOOL bSelect,unsigned long code);

BOOL CB_ReadSetting(CELL_BROADCAST *pCB_Setting,int* pnTopicDetection);
BOOL CB_WriteSetting(CELL_BROADCAST *pCB_Setting,int nTopicDetection);
static BOOL CB_IsSubscribe(HWND hWnd,int nCB);
static BOOL CB_DeleteMessage(int nCB);
static int  CB_GetPos(HWND hWnd,int ID);
static BOOL CB_GetSubscribeID(char* pIDs,int * pnID);
static BOOL CB_GetStringFromID(char* pszIDs,int * pnID);
static BOOL CB_Subscribe(HWND hWnd,int nCB,BOOL bSubscribe);

#define IDM_CBVIEW_GOTOURL             101
#define IDM_CBVIEW_ADDTOBM             102
#define IDM_CBVIEW_VIEWONMAP           103
#define IDM_CBVIEW_SETASDESTINATION    104
#define IDM_CBVIEW_SAVEASWAYPOINT      105
#define IDM_CBVIEW_SEND                107//sub option
#define IDM_CBVIEW_SEND_SMS                1071
#define IDM_CBVIEW_SEND_MMS                1072
#define IDM_CBVIEW_SEND_EMAIL              1073
#define IDM_CBVIEW_ADDTOCONTACTS       110//sub option
#define IDM_CBVIEW_ADDTOCONTACTS_UPDATE    1101
#define IDM_CBVIEW_ADDTOCONTACTS_CREATE    1102
#define IDM_CBVIEW_FIND                111//sub option
#define IDM_CBVIEW_FIND_PHONE              1111
#define IDM_CBVIEW_FIND_EMAIL              1112
#define IDM_CBVIEW_FIND_WEB                1113
#define IDM_CBVIEW_FIND_COORDINATE         1114
#define IDM_CBVIEW_HIDEFOUNDITEMS      112

BOOL CB_CreateViewWnd(HWND hFrameWnd,HWND hMsgWnd,short ID,BOOL bPer,BOOL bNext);
static LRESULT CBViewWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static BOOL CBView_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct);
static void CBView_OnActivate(HWND hwnd, UINT state);
static void CBView_OnInitmenu(HWND hWnd);
static void CBView_OnSetFocus(HWND hWnd);
static void CBView_OnPaint(HWND hWnd);
static void CBView_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags);
static void CBView_OnCommand(HWND hWnd, int id, UINT codeNotify);
static void CBView_OnDestroy(HWND hWnd);
static void CBView_OnClose(HWND hWnd);
static BOOL CB_ReadText(short ID, char** ppszText);



typedef struct  tagCB_LanguageSelect
{
    HWND hFrameWnd;
    HWND hMsgWnd;
    UINT uMsgCmd;
    unsigned long code;
    HBITMAP hSelected;
    HBITMAP hNormal;
}CB_LANGUAGESELECT,*PCB_LANGUAGESELECT;

#define IDS_CB_LANGUANGESELECT  400
#define IDC_CBLS_LIST       300

static BOOL CreateLanguageList(HWND hFrameWnd,HWND hMsgWnd,UINT uMsgCmd,unsigned long code);
static LRESULT CBLanguageSelectWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static BOOL CBLS_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct);
static void CBLS_OnActivate(HWND hwnd, UINT state);
static void CBLS_OnPaint(HWND hWnd);
static void CBLS_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags);
static void CBLS_OnCommand(HWND hWnd, int id, UINT codeNotify);
static void CBLS_OnDestroy(HWND hWnd);
static void CBLS_OnClose(HWND hWnd);
static void CBLS_OnSetFocus(HWND hWnd);


#define IDC_CBMLS_LIST       300

static BOOL CreateLanguageMultiList(HWND hFrameWnd,HWND hMsgWnd,UINT uMsgCmd,unsigned long code);
static LRESULT CBLanguageMultiSelectWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static BOOL CBMLS_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct);
static void CBMLS_OnActivate(HWND hwnd, UINT state);
static void CBMLS_OnPaint(HWND hWnd);
static void CBMLS_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags);
static void CBMLS_OnCommand(HWND hWnd, int id, UINT codeNotify);
static void CBMLS_OnDestroy(HWND hWnd);
static void CBMLS_OnClose(HWND hWnd);
static void CBMLS_OnSetFocus(HWND hWnd);
static void CBMLS_OnInitSelect(HWND hWnd);



BOOL CB_CreateEditWnd(HWND hFrameWnd,HWND hMsgWnd,UINT uMsgCmd,char* pDefault,int index);
static LRESULT CBEditWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static BOOL CBEdit_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct);
static void CBEdit_OnActivate(HWND hWnd, UINT state);
static void CBEdit_OnSetFocus(HWND hWnd);
static void CBEdit_OnPaint(HWND hWnd);
static void CBEdit_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags);
static void CBEdit_OnCommand(HWND hWnd, int id, UINT codeNotify);
static void CBEdit_OnDestroy(HWND hWnd);
static void CBEdit_OnClose(HWND hWnd);
static BOOL CB_IsExist(HWND hWnd,char* pszNum,int index);

typedef struct  tagCB_DATA
{
    HWND  hFrameWnd;
    HMENU hMenu;
    int   nIndex;
    CB_INFO NewSIMCBInfo;
    BOOL  bParallelInfo[CB_MAX];
    int   nCBCount;
    int   nCB[CB_MAX];
    int   nSubscribeCB[CB_SUBSCRIBE_MAX];
    HBITMAP hAddTopic;
    HBITMAP hTopic;
}CB_DATA,*PCB_DATA;

CB_LANGUANGE CB_Language[CB_LANGUAGE_MAX]={  
    {CBS_LANG_GERMAN,             NULL},
    {CBS_LANG_ENGLISH,            NULL},
    {CBS_LANG_ITALIAN,            NULL},
    {CBS_LANG_FRENCH,             NULL},
    {CBS_LANG_SPANISH,            NULL},
    {CBS_LANG_DUTCH,              NULL},  
    {CBS_LANG_SWEDISH,            NULL},
    {CBS_LANG_DANISH,             NULL},
    {CBS_LANG_PORTUGUESE,         NULL},
    {CBS_LANG_FINNISH,            NULL},
    {CBS_LANG_NORWEGIAN,          NULL},
    {CBS_LANG_GREEK,              NULL},
    {CBS_LANG_TURKISH,            NULL},
    {CBS_LANG_HANGARIAN,          NULL},       
    {CBS_LANG_POLISH,             NULL},
};     

static int nCBCounter;
/********************************************************************
* Function	   CB_CreateWnd
* Purpose      
* Params	   
* Return	   
* Remarks	   
**********************************************************************/
BOOL CB_CreateWnd(HWND hParent)
{
	WNDCLASS	wc;
    HWND        hCBWnd;
    CB_DATA     Data;
    RECT        rcClient;

    memset(&Data,0,sizeof(CB_DATA));
	
	wc.style         = 0;
    wc.lpfnWndProc   = CBWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = sizeof(CB_DATA);
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = "CBWndClass";
        
    if (!RegisterClass(&wc))
        return FALSE;
    
    Data.hFrameWnd = MuGetFrame();
    Data.hMenu = CreateMenu();

    GetClientRect(Data.hFrameWnd,&rcClient);

    hCBWnd = CreateWindow(
        "CBWndClass", 
        "", 
        WS_VISIBLE|WS_CHILD,
        rcClient.left,
        rcClient.top,
        rcClient.right - rcClient.left,
        rcClient.bottom - rcClient.top,
        Data.hFrameWnd, 
        (HMENU)IDC_CB_MAIN,
        NULL, 
        (PVOID)&Data
        );
    
    if (!hCBWnd)
    {
        UnregisterClass("CBWndClass",NULL);
        return FALSE;
    }
    
    AppendMenu(Data.hMenu,MF_ENABLED, IDM_CB_OPEN, IDS_OPEN);
    AppendMenu(Data.hMenu,MF_ENABLED, IDM_CB_SUBSCRIBE, IDS_SUBSCRIBE);
    AppendMenu(Data.hMenu,MF_ENABLED, IDM_CB_EDIT, IDS_EDITTOPIC);
    AppendMenu(Data.hMenu,MF_ENABLED, IDM_CB_DELETE, IDS_DELETETOPIC);

    PDASetMenu(Data.hFrameWnd,Data.hMenu);

    SetFocus(hCBWnd);

    SetWindowText(Data.hFrameWnd,IDS_CB);

	SendMessage(Data.hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_EXIT,0), (LPARAM)IDS_BACK);
	SendMessage(Data.hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDM_CB_OPEN,1), (LPARAM)"");
    SendMessage(Data.hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_SELECT);

	SendMessage(Data.hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON), (LPARAM)NULL);
	SendMessage(Data.hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON), (LPARAM)NULL);
    
    ShowWindow(hCBWnd, SW_SHOW);
    UpdateWindow(hCBWnd);

	return TRUE;	
}
/********************************************************************
* Function	   CBWndProc
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static LRESULT CALLBACK CBWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult;
	
    lResult = (LRESULT)TRUE;
    
    switch (message)
    {
    case WM_CREATE:
        lResult = (LRESULT)CB_OnCreate(hWnd,(LPCREATESTRUCT)(lParam));
        break;
        
    case WM_ACTIVATE:
    case PWM_SHOWWINDOW:
        CB_OnActivate(hWnd,(UINT)LOWORD(wParam));
        break;

    case WM_INITMENU:
        CB_OnInitMenu(hWnd);
        break;

    case WM_SETFOCUS:
        CB_OnSetFocus(hWnd);
        break;
        
    case WM_PAINT:
        CB_OnPaint(hWnd);
        break;
        
    case WM_KEYDOWN:
        CB_OnKey(hWnd,(UINT)(wParam),(int)(short)LOWORD(lParam),(UINT)HIWORD(lParam));
        break;
        
    case WM_COMMAND:
        CB_OnCommand(hWnd,(int)(LOWORD(wParam)),(UINT)HIWORD(wParam));
        break;
                
    case WM_CLOSE:
        CB_OnClose(hWnd);
        break;
        
    case WM_DESTROY:
        CB_OnDestroy(hWnd);
        break;
        
    case WM_ADDTOPIC:
        CB_OnAddTopic(hWnd,(BOOL)wParam,(char*)lParam);
        break;

    case WM_MODIFYTOPIC:
        CB_OnModifyTopic(hWnd,(BOOL)wParam,(char*)lParam);
        break;

    case WM_CBMSG_PRE:
    case WM_CBMSG_NEXT:
        CB_OnParalleView(hWnd,message);
        break;

    case WM_SUREDELETE:
        CB_OnDeleteTopic(hWnd,(BOOL)lParam);
        break;

    default :
        lResult = PDADefWindowProc(hWnd, message, wParam, lParam);
        break;
    } 
    return lResult;
}
/*********************************************************************\
* Function	CB_OnCreate
* Purpose   WM_CREATE message handler of the main window
* Params
*			hWnd: Handle of the window
*			lpCreateStruct: Create Structure
* Return
*			TRUE: Success
*			FALSE: Fail
* Remarks
**********************************************************************/
static BOOL CB_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
{
    HWND hLst;
    RECT rcClient;
    char szTmp[21];
    int  index,i;
    CB_DATA *pData;
    int  nTopicDetection;
    CELL_BROADCAST CB_Setting;

    pData = GetUserData(hWnd);

    memcpy(pData,lpCreateStruct->lpCreateParams,sizeof(CB_DATA));

    memset(&CB_Setting,0,sizeof(CELL_BROADCAST));

    CB_ReadSetting(&CB_Setting,&nTopicDetection);

    for(i = 0 ; i < CB_SUBSCRIBE_MAX ; i++)
        pData->nSubscribeCB[i] = -1;
    
    CB_GetSubscribeID(CB_Setting.ids,pData->nSubscribeCB);

    GetClientRect(hWnd,&rcClient);

    hLst = CreateWindow(
        "LISTBOX",
        "",
        WS_VISIBLE|WS_CHILD|WS_TABSTOP|LBS_BITMAP|WS_VSCROLL,
        0,
        0,
        rcClient.right - rcClient.left,
        rcClient.bottom - rcClient.top,
        hWnd,
        (HMENU)IDC_CB_LIST,
        NULL,
        NULL);

    if( hLst == NULL )
        return FALSE;
    
	pData->hAddTopic = LoadImage(NULL, SMS_BMP_ADDTOPIC, IMAGE_BITMAP,
		ICON_WIDTH, ICON_HEIGHT, LR_LOADFROMFILE);
    
	pData->hTopic = LoadImage(NULL, SMS_BMP_TOPIC, IMAGE_BITMAP,
		ICON_WIDTH, ICON_HEIGHT, LR_LOADFROMFILE);

    SendMessage(hLst,LB_ADDSTRING,-1,(LPARAM)IDS_ADDTOPIC);
    SendMessage(hLst,LB_SETIMAGE,MAKEWPARAM(IMAGE_BITMAP, 0),(LPARAM)pData->hAddTopic);
            
    CB_Read(pData->nCB);

    for(i = 0,pData->nCBCount = 0 ; i < CB_MAX ; i++)
    {
        if(pData->nCB[i] != -1)
        {
            szTmp[0] = 0;
            sprintf(szTmp,"%d",pData->nCB[i]);
            index = SendMessage(hLst,LB_ADDSTRING,-1,(LPARAM)szTmp);
            SendMessage(hLst,LB_SETITEMDATA,index,(LPARAM)&(pData->nCB[i]));
            SendMessage(hLst,LB_SETIMAGE,MAKEWPARAM(IMAGE_BITMAP, index),(LPARAM)pData->hTopic);

            pData->nCBCount++;
        }
    }

    SendMessage(hLst,LB_SETCURSEL,0,0);
 
    return TRUE;	
}
/*********************************************************************\
* Function	CB_OnActivate
* Purpose   WM_ACTIVATE message handler of the main window
* Params
* Return    None
* Remarks
**********************************************************************/
static void CB_OnActivate(HWND hWnd, UINT state)
{
    HWND hLst;
    CB_DATA *pData;

    pData = GetUserData(hWnd);

    hLst = GetDlgItem(hWnd,IDC_CB_LIST);

    SetFocus(hLst);
    
    PDASetMenu(pData->hFrameWnd,pData->hMenu);

    SetWindowText(pData->hFrameWnd,IDS_CB);

	SendMessage(pData->hFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_BACK);

    CB_SetLeftSoftKey(hWnd);

	SendMessage(pData->hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON), (LPARAM)NULL);
	SendMessage(pData->hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON), (LPARAM)NULL);

    return;
}

/*********************************************************************\
* Function	CB_OnInitMenu
* Purpose   WM_INITMENU message handler of the main window
* Params
* Return    None
* Remarks
**********************************************************************/
static void CB_OnInitMenu(HWND hWnd)
{
    int index;
    HWND hLst;
    int* pnCB;
    PCB_DATA pData;
    int  nMenuItemCount;

    hLst = GetDlgItem(hWnd,IDC_CB_LIST);
    
    index = SendMessage(hLst,LB_GETCURSEL,0,0);
    
    if(index == LB_ERR)
        return;
    
    pData = GetUserData(hWnd);
    
    pnCB = (int*)SendMessage(hLst,LB_GETITEMDATA,index,NULL);
    
    if(CB_IsSubscribe(hWnd,*pnCB))
        ModifyMenu(pData->hMenu,IDM_CB_SUBSCRIBE, MF_BYCOMMAND, IDM_CB_SUBSCRIBE, IDS_UNSUBSCRIBE);
    else
        ModifyMenu(pData->hMenu,IDM_CB_SUBSCRIBE, MF_BYCOMMAND, IDM_CB_SUBSCRIBE, IDS_SUBSCRIBE);

    nMenuItemCount = GetMenuItemCount(pData->hMenu);
    
    if(CB_IsExistMsg(*pnCB))
    {
        if(nMenuItemCount < 4)
            InsertMenu(pData->hMenu,0,MF_BYPOSITION,IDM_CB_OPEN,IDS_OPEN);
    }
    else
    {
        if(nMenuItemCount >= 4)
            DeleteMenu(pData->hMenu,0,MF_BYPOSITION);
    }
}
/*********************************************************************\
* Function	CB_OnSetFocus
* Purpose   WM_SETFOCUS message handler of the main window
* Params
* Return    None
* Remarks
**********************************************************************/
static void CB_OnSetFocus(HWND hWnd)
{
    HWND hLst;

    hLst = GetDlgItem(hWnd,IDC_CB_LIST);

    SetFocus(hLst);

    return;
}
/*********************************************************************\
* Function	CB_OnPaint
* Purpose   WM_PAINT message handler of the main window
* Params	hWnd: Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void CB_OnPaint(HWND hWnd)
{
	HDC hdc = BeginPaint(hWnd, NULL);

    EndPaint(hWnd, NULL);

	return;
}
/*********************************************************************\
* Function	CB_OnKey
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
static void CB_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags)
{    
    HWND hLst;
    CB_DATA *pData;

    pData = GetUserData(hWnd);

    hLst = GetDlgItem(hWnd,IDC_CB_LIST);
	
    switch (vk)
	{
    case VK_RETURN:
        SendMessage(hWnd,WM_COMMAND,IDM_CB_OPEN,NULL);
        break;

	case VK_F10:
        PostMessage(hWnd,WM_CLOSE,0,0);
		break;

    case VK_F5:
        if(SendMessage(hLst,LB_GETCURSEL,0,0) == 0)
        {           
            if(pData->nCBCount >= CB_MAX)
                PLXTipsWin(NULL, NULL, 0, IDS_NOMORETOPICALLOW, IDS_CB,
                Notify_Failure, IDS_OK, NULL, WAITTIMEOUT);
            else
                CB_CreateEditWnd(pData->hFrameWnd,hWnd,WM_ADDTOPIC,"",-1);
        }
        else
            PDADefWindowProc(pData->hFrameWnd, WM_KEYDOWN, vk, MAKELPARAM(cRepeat, flags));
        break;
        
	default:
		PDADefWindowProc(hWnd, WM_KEYDOWN, vk, MAKELPARAM(cRepeat, flags));
		break;
	}

	return;
}
/*********************************************************************\
* Function	CB_OnCommand
* Purpose   WM_COMMAND message handler of the main window
* Params
*			hWnd:	Handle of the window
*			id:		Id of command message
*			hwndCtl: Handle of the window which sended this message
*			codeNotify:Notify code of the message
* Return	None
* Remarks
**********************************************************************/
static void CB_OnCommand(HWND hWnd, int id, UINT codeNotify)
{
    int index;
    HWND hLst;

    hLst = GetDlgItem(hWnd,IDC_CB_LIST);

    switch(id)
	{
    case IDC_CB_LIST:
        if(codeNotify == LBN_SELCHANGE)
        {
            CB_SetLeftSoftKey(hWnd);
        }
        break;

    case IDM_CB_OPEN:

        index = SendMessage(hLst,LB_GETCURSEL,0,0);
        
        if(index == LB_ERR || index == 0)
            break;
        
        {
            PCB_DATA pData;
            BOOL bPer,bNext;
            int* pnCB;
            
            pData = GetUserData(hWnd);
           
            pnCB = (int*)SendMessage(hLst,LB_GETITEMDATA,index,NULL);

            if(CB_IsExistMsg(*pnCB))
            {
                CB_ParallelInfo(hWnd);
                
                CB_GetInfo(hWnd,index-1,&bPer,&bNext);
                
                CB_CreateViewWnd(pData->hFrameWnd,hWnd,(short)(*pnCB),bPer,bNext);
            }
        }

        break;

    case IDM_CB_SUBSCRIBE:
        
        index = SendMessage(hLst,LB_GETCURSEL,0,0);
        
        if(index == LB_ERR)
            break;
        
        {
            int* pnCB;
            PCB_DATA pData;
            
            pData = GetUserData(hWnd);

            pnCB = (int*)SendMessage(hLst,LB_GETITEMDATA,index,NULL);
            
            if(CB_IsSubscribe(hWnd,*pnCB))
            {
                CB_Subscribe(hWnd,*pnCB,FALSE);
                //change icon
            }
            else
            {
                CB_Subscribe(hWnd,*pnCB,TRUE);
                //change icon
            }
        }
        break;
                   
    case IDM_CB_EDIT:

        index = SendMessage(hLst,LB_GETCURSEL,0,0);
        
        if(index == LB_ERR)
            break;

        {
            PCB_DATA pData;
            char sztemp[4];
            int* pnCB;
            
            pData = GetUserData(hWnd);
           
            pnCB = (int*)SendMessage(hLst,LB_GETITEMDATA,index,NULL);
            
            sztemp[0] = 0;

            itoa(*pnCB,sztemp,10);

            CB_CreateEditWnd(pData->hFrameWnd,hWnd,WM_MODIFYTOPIC,sztemp,*pnCB);
        }
        
        break;
        
    case IDM_CB_DELETE:
        
        index = SendMessage(hLst,LB_GETCURSEL,0,0);
        
        if(index == LB_ERR)
            break;

        {
            PCB_DATA pData;
            int* pnCB;
            char szPrompt[100];
            
            pData = GetUserData(hWnd);
        
            szPrompt[0] = 0;

            pnCB = (int*)SendMessage(hLst,LB_GETITEMDATA,index,NULL);
            
            sprintf(szPrompt,"%d: %s?",*pnCB,IDS_DELETE);

            PLXConfirmWinEx(pData->hFrameWnd,hWnd,szPrompt,Notify_Request,IDS_CB,IDS_YES,IDS_NO,WM_SUREDELETE);
            
        }
        break;
        
    default:
        break;
	}

	return;
}
/*********************************************************************\
* Function	CB_OnDestroy
* Purpose   WM_DESTROY message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void CB_OnDestroy(HWND hWnd)
{
    CB_DATA * pData;

    pData = GetUserData(hWnd);

    if(pData->hAddTopic)
        DeleteObject(pData->hAddTopic);

    if(pData->hTopic)
        DeleteObject(pData->hTopic);

    DestroyMenu(pData->hMenu);

	UnregisterClass("CBWndClass", NULL);
 
    return;

}
/*********************************************************************\
* Function	CB_OnClose
* Purpose   WM_CLOSE message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void CB_OnClose(HWND hWnd)
{
    DestroyWindow (hWnd);
	
    return;
}

/*********************************************************************\
* Function	CB_OnParalleView
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
static void CB_OnParalleView(HWND hWnd,UINT message)
{
    int index,curindex;
    PCB_DATA pData;
    BOOL bPer,bNext;
    HWND hLst;
    int *pnCB;

    hLst = GetDlgItem(hWnd,IDC_CB_LIST);
    
    index = SendMessage(hLst,LB_GETCURSEL,0,0);
    
    if(index == LB_ERR || index == 0)
        return;

    pnCB = (int*)SendMessage(hLst,LB_GETITEMDATA,index,NULL);

    if(message == WM_CBMSG_PRE)
        curindex = CB_GetNextText(hWnd,index-1,FALSE);
    else if(message == WM_CBMSG_NEXT)
        curindex = CB_GetNextText(hWnd,index-1,TRUE);
    else
        curindex = index;
    
    if(curindex == -1)
        return;

    SendMessage(hLst,LB_SETCURSEL,curindex+1,0);
    
    CB_SetLeftSoftKey(hWnd);

    pData = GetUserData(hWnd);
        
    CB_GetInfo(hWnd,curindex,&bPer,&bNext);
        
    CB_CreateViewWnd(pData->hFrameWnd,hWnd,(short)(*pnCB),bPer,bNext);
    
}
/*********************************************************************\
* Function	CB_OnAddTopic
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
static void CB_OnAddTopic(HWND hWnd,BOOL bNew,char* pszNr)
{
    int  ID,nPos;
    PCB_DATA pData;
    HWND hList;

    if( bNew == FALSE)
        return;

    pData = GetUserData(hWnd);

    ID = atoi(pszNr);

    pData->nCB[ID] = ID;

    pData->nCBCount++;
    
    SMS_SetCBCounter(pData->nCBCount);
    
    hList = GetDlgItem(hWnd,IDC_CB_LIST);
       
    nPos = CB_GetPos(hWnd,ID);
        
    SendMessage(hList,LB_INSERTSTRING,nPos+1,(LPARAM)pszNr);
    
    SendMessage(hList,LB_SETITEMDATA,nPos+1,(LPARAM)&(pData->nCB[ID]));
    
    SendMessage(hList,LB_SETIMAGE,MAKEWPARAM(IMAGE_BITMAP, nPos+1),(LPARAM)pData->hTopic);
    
    SendMessage(hList,LB_SETCURSEL,nPos+1,0);

    CB_SetLeftSoftKey(hWnd);

    CB_Write(pData->nCB);
}
/*********************************************************************\
* Function	CB_OnModifyTopic
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
static void CB_OnModifyTopic(HWND hWnd,BOOL bModify,char* pszNr)
{
    int  ID,nPos;
    PCB_DATA pData;
    HWND hList;
    int  index;
    int  *pnCB;

    if( bModify == FALSE)
        return;

    pData = GetUserData(hWnd);

    hList = GetDlgItem(hWnd,IDC_CB_LIST);

    index = SendMessage(hList,LB_GETCURSEL,0,0);

    if(index == 0 || index == LB_ERR)
        return;

    //judge the status of the topic (subscribe or unsubsrcibe)
    //if subscribe need to unsubsrcibe and delete the relatived message 

    pnCB = (int*)SendMessage(hList,LB_GETITEMDATA,index,NULL);

    if(CB_IsSubscribe(hWnd,*pnCB))
    {
        CELL_BROADCAST CB_Setting;
        int nTopicDetection = 0;
        
        memset(&CB_Setting,0,sizeof(CELL_BROADCAST));
        
        CB_ReadSetting(&CB_Setting,&nTopicDetection);
        
        CB_Subscribe(hWnd,*pnCB,FALSE);
        
        CB_GetStringFromID(CB_Setting.ids,pData->nSubscribeCB);
        
        if(nTopicDetection == 0)
            CB_Setting.ids[0] = 0;
        
        CB_Set(&CB_Setting);
    }
    
    CB_DeleteMessage(*pnCB);
    
    *pnCB = -1;
    
    SendMessage(hList,LB_DELETESTRING,index,0);
    
    ID = atoi(pszNr);

    pData->nCB[ID] = ID;

    nPos = CB_GetPos(hWnd,ID);
        
    SendMessage(hList,LB_INSERTSTRING,nPos+1,(LPARAM)pszNr);
    
    SendMessage(hList,LB_SETITEMDATA,nPos+1,(LPARAM)&(pData->nCB[ID]));
    
    SendMessage(hList,LB_SETIMAGE,MAKEWPARAM(IMAGE_BITMAP, nPos+1),(LPARAM)pData->hTopic);
    
    SendMessage(hList,LB_SETCURSEL,nPos+1,0);

    CB_SetLeftSoftKey(hWnd);
    
    CB_Write(pData->nCB);
}

/*********************************************************************\
* Function	CB_OnDeleteTopic
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
static void CB_OnDeleteTopic(HWND hWnd,BOOL bDel)
{
    PCB_DATA pData;
    int* pnCB;
    int index;
    HWND hLst;

    if(bDel == FALSE)
        return;
    
    hLst = GetDlgItem(hWnd,IDC_CB_LIST);
    
    index = SendMessage(hLst,LB_GETCURSEL,0,0);
    
    if(index == LB_ERR)
        return;

    pData = GetUserData(hWnd);
    
    pnCB = (int*)SendMessage(hLst,LB_GETITEMDATA,index,NULL);
    
    if(CB_IsSubscribe(hWnd,*pnCB))
    {
        CELL_BROADCAST CB_Setting;
        int nTopicDetection = 0;
        
        memset(&CB_Setting,0,sizeof(CELL_BROADCAST));
        
        CB_ReadSetting(&CB_Setting,&nTopicDetection);
        
        CB_Subscribe(hWnd,*pnCB,FALSE);
        
        CB_GetStringFromID(CB_Setting.ids,pData->nSubscribeCB);
        
        if(nTopicDetection == 0)
            CB_Setting.ids[0] = 0;
        
        CB_Set(&CB_Setting);
    }
    
    CB_DeleteMessage(*pnCB);
    
    *pnCB = -1;
    
    CB_Write(pData->nCB);
    
    pData->nCBCount--;        
    
    SMS_SetCBCounter(pData->nCBCount);
    
    SendMessage(hLst,LB_DELETESTRING,index,0);
    
    if(index == SendMessage(hLst,LB_GETCOUNT,0,0))
        SendMessage(hLst,LB_SETCURSEL,index-1,0);
    else
        SendMessage(hLst,LB_SETCURSEL,index,0);   
    
    CB_SetLeftSoftKey(hWnd);

    PLXTipsWin(NULL, NULL, 0, (char*)IDS_DELETED, (char*)IDS_CB, Notify_Success, (char*)IDS_OK, "", WAITTIMEOUT);
}
/*********************************************************************\
* Function	CB_GetPos
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
static int CB_GetPos(HWND hWnd,int ID)
{
    PCB_DATA pData;
    int nPos,i;

    pData = GetUserData(hWnd);

    for(i = 0 ,nPos = 0; i <CB_MAX ; i++)
    {
        if(pData->nCB[i] != -1)
        {
            if(pData->nCB[i] == ID)
                return nPos;

            nPos++;
        }
    }

    return -1;
}
/*********************************************************************\
* Function	CB_Read
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
BOOL CB_Read(int *pnCB)
{
    char szOldPath[PATH_MAXLEN];
    int f;
    int i;

    szOldPath[0] = 0;
    
    getcwd(szOldPath,PATH_MAXLEN);

    chdir(PATH_DIR_SMS);

    f = open(SMS_FILENAME_CBTOPIC,O_RDONLY);
    
    if( f == -1 )
    {
        f = open(SMS_FILENAME_CBTOPIC, O_RDWR | O_CREAT , S_IRWXU);

        if(f == -1)
        {
            chdir(szOldPath);
            
            return FALSE;
        }
        
#ifdef _SMS_DEBUG_
        printf("\r\n*****SMS Debug Info*****  CB_Read file handle = %d \r\n",f);
#endif

        for(i = 0 ; i < CB_MAX ; i++)
            pnCB[i] = -1;

        write(f,pnCB,sizeof(int)*CB_MAX);

        close(f);
        
        chdir(szOldPath);

        return TRUE;
    }
    
#ifdef _SMS_DEBUG_
    printf("\r\n*****SMS Debug Info*****  CB_Read file handle = %d \r\n",f);
#endif
    
    read(f,pnCB,sizeof(int)*CB_MAX);
    
    close(f);

    chdir(szOldPath);

    return TRUE;
}
/*********************************************************************\
* Function	CB_Write
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
BOOL CB_Write(int *pnCB)
{
    char szOldPath[PATH_MAXLEN];
    int f;

    szOldPath[0] = 0;
    
    getcwd(szOldPath,PATH_MAXLEN);

    chdir(PATH_DIR_SMS);

    f = open(SMS_FILENAME_CBTOPIC,O_RDWR);
    
    if( f == -1 )
    {
        chdir(szOldPath);

        return FALSE;
    }
    
#ifdef _SMS_DEBUG_
    printf("\r\n*****SMS Debug Info*****  CB_Write file handle = %d \r\n",f);
#endif

    write(f,pnCB,sizeof(int)*CB_MAX);
    
    close(f);
    
    chdir(szOldPath);

    return TRUE;
}


/********************************************************************
* Function	   CB_CreateSettingWnd
* Purpose      
* Params	   
* Return	   
* Remarks	   
**********************************************************************/
BOOL CB_CreateSettingWnd(HWND hFrameWnd)
{
	WNDCLASS	wc;
    HWND        hCBSettingWnd;
    CB_SETTING  Data;
    RECT        rc;

    memset(&Data,0,sizeof(CB_SETTING));

    Data.hFrameWnd = hFrameWnd;
	
	wc.style         = 0;
    wc.lpfnWndProc   = CBSettingWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = sizeof(CB_SETTING);
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = "CBSettingWndClass";
        
    if (!RegisterClass(&wc))
        return FALSE;
    
    GetClientRect(hFrameWnd,&rc);

    CB_ReadSetting(&(Data.CB_Setting),&(Data.nTopicDetection));
    
    hCBSettingWnd = CreateWindow(
        "CBSettingWndClass", 
        "", 
        WS_CHILD | WS_VISIBLE,
        rc.left,  
        rc.top,  
        rc.right - rc.left,  
        rc.bottom - rc.top,  
        hFrameWnd, 
        NULL,
        NULL, 
        (PVOID)&Data
        );
    
    if (!hCBSettingWnd)
    {
        UnregisterClass("CBSettingWndClass",NULL);
        return FALSE;
    }
    SetFocus(hCBSettingWnd);

    SetWindowText(hFrameWnd,IDS_CB);

	SendMessage(hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_EXIT,0), (LPARAM)IDS_BACK);
	SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_SELECT);    
    
    SendMessage(hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON),(LPARAM)NULL);
    SendMessage(hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON),(LPARAM)NULL);
        
    ShowWindow(hCBSettingWnd, SW_SHOW);
    UpdateWindow(hCBSettingWnd);

	return TRUE;	
}
/********************************************************************
* Function	   CBSettingWndProc
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static LRESULT CALLBACK CBSettingWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult;
	
    lResult = (LRESULT)TRUE;
    
    switch (message)
    {
    case WM_CREATE:
        lResult = (LRESULT)CBSetting_OnCreate(hWnd,(LPCREATESTRUCT)(lParam));
        break;
        
    case PWM_SHOWWINDOW:
        CBSetting_OnActivate(hWnd,(UINT)LOWORD(wParam));
        break;

    case WM_SETFOCUS:
        CBSetting_OnSetFocus(hWnd);
        break;

    case WM_KILLFOCUS:
        CBSetting_OnKillFocus(hWnd);
        break;
        
    case WM_PAINT:
        CBSetting_OnPaint(hWnd);
        break;
        
    case WM_KEYDOWN:
        CBSetting_OnKey(hWnd,(UINT)(wParam),(int)(short)LOWORD(lParam),(UINT)HIWORD(lParam));
        break;

    case WM_KEYUP:
        CBSetting_OnKeyUp(hWnd,(UINT)(wParam),(int)(short)LOWORD(lParam),(UINT)HIWORD(lParam));
        break;

    case WM_TIMER:
        CBSetting_OnTimer(hWnd,(WPARAM)(UINT)(wParam));
        break;
                
    case WM_COMMAND:
        CBSetting_OnCommand(hWnd,(int)(LOWORD(wParam)),(UINT)HIWORD(wParam));
        break;
                
    case WM_CLOSE:
        CBSetting_OnClose(hWnd);
        break;
        
    case WM_DESTROY:
        CBSetting_OnDestroy(hWnd);
        break;

    case WM_SELECTLANGUAGE:
        CBSetting_OnSelectLanguage(hWnd,(BOOL)wParam,lParam);
        break;
        
    default :
        lResult = PDADefWindowProc(hWnd, message, wParam, lParam);
        break;
    } 
    return lResult;
}
/*********************************************************************\
* Function	CBSetting_OnCreate
* Purpose   WM_CREATE message handler of the main window
* Params
*			hWnd: Handle of the window
*			lpCreateStruct: Create Structure
* Return
*			TRUE: Success
*			FALSE: Fail
* Remarks
**********************************************************************/
static BOOL CBSetting_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
{
    HWND hWndTmp;
    RECT rcClient;
    int  y,nWidth,nHeight;
    CB_SETTING *pData;

    pData = GetUserData(hWnd);

    memcpy(pData,lpCreateStruct->lpCreateParams,sizeof(CB_SETTING));

    GetClientRect(hWnd,&rcClient);
    
    y = 0;
    
    nWidth = rcClient.right - rcClient.left;

    nHeight = (rcClient.bottom - rcClient.top)/3;
    
    hWndTmp = CreateWindow(
        "SPINBOXEX", 
        IDS_RECEPTION, 
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT | CS_NOSYSCTRL,
        0,y,nWidth,nHeight,
        hWnd, 
        (HMENU)IDC_CBSETTING_RECEPTION, 
        NULL, 
        NULL);

    if(hWndTmp == NULL)
        return FALSE;

    pData->hFocus = hWndTmp;

    SendMessage(hWndTmp, SSBM_ADDSTRING, 0, (LPARAM)IDS_ON);
    SendMessage(hWndTmp, SSBM_ADDSTRING, 0, (LPARAM)IDS_OFF);

    SendMessage(hWndTmp, SSBM_SETCURSEL, pData->CB_Setting.mode, 0);

    y += nHeight;

    hWndTmp = CreateWindow(
        "SPINBOXEX", 
        IDS_LANGUAGES, 
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT | CS_NOSYSCTRL,
        0,y,nWidth,nHeight,
        hWnd, 
        (HMENU)IDC_CBSETTING_LANGUAGE, 
        NULL, 
        NULL);

    if(hWndTmp == NULL)
        return FALSE;
    
    if(pData->CB_Setting.code == 0)
        SendMessage(hWndTmp, SSBM_ADDSTRING, 0, (LPARAM)IDS_ALL);
    else
        SendMessage(hWndTmp, SSBM_ADDSTRING, 0, (LPARAM)IDS_SELECTED);

    y += nHeight;

    hWndTmp = CreateWindow(
        "SPINBOXEX", 
        IDS_TOPICDETECTION, 
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT | CS_NOSYSCTRL,
        0,y,nWidth,nHeight,
        hWnd, 
        (HMENU)IDC_CBSETTING_DETECTION, 
        NULL, 
        NULL);

    if(hWndTmp == NULL)
        return FALSE;

    SendMessage(hWndTmp, SSBM_ADDSTRING, 0, (LPARAM)IDS_ON);
    SendMessage(hWndTmp, SSBM_ADDSTRING, 0, (LPARAM)IDS_OFF);

    SendMessage(hWndTmp, SSBM_SETCURSEL, pData->nTopicDetection, 0);

    return TRUE;	
}
/*********************************************************************\
* Function	CBSetting_OnActivate
* Purpose   WM_ACTIVATE message handler of the main window
* Params
* Return    None
* Remarks
**********************************************************************/
static void CBSetting_OnActivate(HWND hWnd, UINT state)
{
    CB_SETTING *pData;

    pData = GetUserData(hWnd);

    SetFocus(pData->hFocus);

	SendMessage(pData->hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
	SendMessage(pData->hFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_BACK);
	SendMessage(pData->hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_SELECT);

    
    SendMessage(pData->hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON),(LPARAM)NULL);
    SendMessage(pData->hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON),(LPARAM)NULL);

    SetWindowText(pData->hFrameWnd,IDS_CB);


    return;
}
/*********************************************************************\
* Function	CBSetting_OnSetFocus
* Purpose   WM_SETFOCUS
* Params	hWnd: Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void CBSetting_OnSetFocus(HWND hWnd)
{
    CB_SETTING *pData;

    pData = GetUserData(hWnd);

    SetFocus(pData->hFocus);
}
/*********************************************************************\
* Function	CBSetting_OnPaint
* Purpose   WM_PAINT message handler of the main window
* Params	hWnd: Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void CBSetting_OnPaint(HWND hWnd)
{
	HDC hdc = BeginPaint(hWnd, NULL);

    EndPaint(hWnd, NULL);

	return;
}
/*********************************************************************\
* Function	CBSetting_OnKey
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
static void CBSetting_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags)
{    
    HWND hWndTmp;
    CELL_BROADCAST Temp;
    int  index;
    CB_SETTING *pData;
    int  nTopicDetection;
    HWND hWndFocus;

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

        memset(&Temp,0,sizeof(CELL_BROADCAST));
    
        hWndTmp = GetDlgItem(hWnd,IDC_CBSETTING_RECEPTION);
        index = SendMessage(hWndTmp,SSBM_GETCURSEL,0,0);
        if(index != LB_ERR)
        {
            Temp.mode = index;
        }

        Temp.code = pData->CB_Setting.code;

        hWndTmp = GetDlgItem(hWnd,IDC_CBSETTING_DETECTION);
        index = SendMessage(hWndTmp,SSBM_GETCURSEL,0,0);
        if(index != LB_ERR)
        {
            nTopicDetection = index;
        }

        memcpy(Temp.ids,pData->CB_Setting.ids,40);

//        if(Temp.code != pData->CB_Setting.code || Temp.mode != pData->CB_Setting.mode 
//            || strcmp(Temp.ids,pData->CB_Setting.ids) != 0 )
        {
            CB_WriteSetting(&Temp,nTopicDetection);

            if(nTopicDetection == 0)
                Temp.ids[0] = 0;

            CB_Set(&Temp);
        }

        SendMessage(pData->hFrameWnd,PWM_CLOSEWINDOW,(WPARAM)hWnd,0);
        PostMessage(hWnd,WM_CLOSE,0,0);
		break;

    case VK_F5:
        {
            HWND hWndFocus;

            hWndFocus = GetFocus();
            
            while(GetParent(hWndFocus) != hWnd)
                hWndFocus = GetParent(hWndFocus);

            if(hWndFocus == GetDlgItem(hWnd,IDC_CBSETTING_LANGUAGE))
                CreateLanguageList(pData->hFrameWnd,hWnd,WM_SELECTLANGUAGE,pData->CB_Setting.code);
            else
                CreateSpinList(pData->hFrameWnd,hWndFocus);
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
        break;

	default:
		PDADefWindowProc(hWnd, WM_KEYDOWN, vk, MAKELPARAM(cRepeat, flags));
		break;
	}

	return;
}
/*********************************************************************\
* Function	CBSetting_OnKeyUp
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
static void CBSetting_OnKeyUp(HWND hWnd, UINT vk, int cRepeat, UINT flags)
{
    CB_SETTING* pData;
    
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
* Function	CBSetting_OnTimer
* Purpose   WM_TIMER message handler of the main window
* Params
*			hWnd: Handle of the window
*			id:	event id
* Return	None
* Remarks
**********************************************************************/
static void CBSetting_OnTimer(HWND hWnd,UINT id)
{
    
    CB_SETTING* pData;
    
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
* Function	CBSetting_OnKillFocus
* Purpose   WM_KILLFOCUS message handler of the main window
* Params
* Return    None
* Remarks
**********************************************************************/
static void CBSetting_OnKillFocus(HWND hWnd)
{
    CB_SETTING* pData;

    pData = GetUserData(hWnd);

	KillTimer(hWnd, TIMER_ID);
	
    pData->nRepeats = 0;
	
    pData->wKeyCode = 0;
}
/*********************************************************************\
* Function	CBSetting_OnCommand
* Purpose   WM_COMMAND message handler of the main window
* Params
*			hWnd:	Handle of the window
*			id:		Id of command message
*			hwndCtl: Handle of the window which sended this message
*			codeNotify:Notify code of the message
* Return	None
* Remarks
**********************************************************************/
static void CBSetting_OnCommand(HWND hWnd, int id, UINT codeNotify)
{
    CB_SETTING *pData;

    pData = GetUserData(hWnd);

	switch(id)
	{       
    case IDC_CBSETTING_RECEPTION:
    case IDC_CBSETTING_LANGUAGE:
    case IDC_CBSETTING_DETECTION:
        if(codeNotify == SSBN_SETFOCUS)
            pData->hFocus = GetDlgItem(hWnd,id);
        break;

    default:
        break;
	}

	return;
}
/*********************************************************************\
* Function	CBSetting_OnDestroy
* Purpose   WM_DESTROY message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void CBSetting_OnDestroy(HWND hWnd)
{

	UnregisterClass("CBSettingWndClass", NULL);

    return;

}
/*********************************************************************\
* Function	CBSetting_OnClose
* Purpose   WM_CLOSE message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void CBSetting_OnClose(HWND hWnd)
{
    DestroyWindow (hWnd);
	
    return;

}
/*********************************************************************\
* Function	CBSetting_OnSelectLanguage
* Purpose   
* Params	
* Return	None
* Remarks
**********************************************************************/
static void CBSetting_OnSelectLanguage(HWND hWnd,BOOL bSelect,unsigned long code)
{
    HWND hCBList,hLangList;
    CB_SETTING *pData;
    HWND hFrameWnd = GetParent(hWnd);
    
    hCBList = GetDlgItem(hFrameWnd,IDS_CB_LANGUANGESELECT);
    
    if(IsWindow(hCBList))
    {
        SendMessage(hFrameWnd,PWM_CLOSEWINDOW,(WPARAM)hCBList,NULL);
        SendMessage(hCBList,WM_CLOSE,NULL,NULL);
    }
    
    if(bSelect == FALSE)
        return;
    
    pData = GetUserData(hWnd);
    
    pData->CB_Setting.code = code;
    
    hLangList = GetDlgItem(hWnd,IDC_CBSETTING_LANGUAGE);
    
    if(code == 0)
        SendMessage(hLangList,SSBM_SETTEXT,0,(LPARAM)IDS_ALL);
    else
        SendMessage(hLangList,SSBM_SETTEXT,0,(LPARAM)IDS_SELECTED);
}
/*********************************************************************\
* Function	CB_ReadSetting
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
BOOL CB_ReadSetting(CELL_BROADCAST *pCB_Setting,int* pnTopicDetection)
{
    char szOldPath[PATH_MAXLEN];
    int f;

    if(pCB_Setting == NULL)
    {        
        return FALSE;
    }
    else
    {
        szOldPath[0] = 0;
        
        getcwd(szOldPath,PATH_MAXLEN);
        
        chdir(PATH_DIR_SMS);

        f = open(SMS_FILENAME_CBSETTING,O_RDONLY);
        
        if( f == -1 )
        {
            f = open(SMS_FILENAME_CBSETTING,O_RDWR|O_CREAT,S_IRWXU);

            if(f == -1)
            {
                chdir(szOldPath);

                return FALSE;
            }

#ifdef _SMS_DEBUG_
            printf("\r\n*****SMS Debug Info*****  CB_ReadSetting file handle = %d \r\n",f);
#endif
            
            pCB_Setting->code = 0;
            
            pCB_Setting->mode = 1;
            
            write(f,pCB_Setting,sizeof(CELL_BROADCAST));

            *pnTopicDetection = 1;
            
            write(f,pnTopicDetection,sizeof(int));
            
            close(f);

        }
        else
        {
#ifdef _SMS_DEBUG_
            printf("\r\n*****SMS Debug Info*****  CB_ReadSetting file handle = %d \r\n",f);
#endif
            read(f,pCB_Setting,sizeof(CELL_BROADCAST));
            
            read(f,pnTopicDetection,sizeof(int));
            
            close(f);
        }
    }

    chdir(szOldPath);

    return TRUE;
}
/*********************************************************************\
* Function	CB_WriteSetting
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
BOOL CB_WriteSetting(CELL_BROADCAST *pCB_Setting,int nTopicDetection)
{
    char szOldPath[PATH_MAXLEN];
    int f;

    szOldPath[0] = 0;
    
    getcwd(szOldPath,PATH_MAXLEN);

    chdir(PATH_DIR_SMS);

    if(pCB_Setting == NULL)
    {
        remove(SMS_FILENAME_CBTOPIC);
    }
    else
    {
        f = open(SMS_FILENAME_CBSETTING,O_RDWR|O_CREAT,S_IRWXU);
        
        if( f == -1 )
        {
            chdir(szOldPath);

            return FALSE;
        }
        
#ifdef _SMS_DEBUG_
        printf("\r\n*****SMS Debug Info*****  CB_WriteSetting file handle = %d \r\n",f);
#endif

        write(f,pCB_Setting,sizeof(CELL_BROADCAST));

        if(nTopicDetection != -1)
            write(f,&nTopicDetection,sizeof(int));

        close(f);

    }

    chdir(szOldPath);

    return TRUE;
}
/********************************************************************
* Function	   CB_CreateViewWnd
* Purpose      
* Params	   
* Return	   
* Remarks	   
**********************************************************************/
BOOL CB_CreateViewWnd(HWND hFrameWnd,HWND hMsgWnd,short ID,BOOL bPre,BOOL bNext)
{
	WNDCLASS	wc;
    HWND        hCBViewWnd;
    CB_VIEW     CBView;
    CB_VIEW     *pData;
    RECT        rect;
    char        sztemp[4];

    memset(&CBView,0,sizeof(CB_VIEW));

    CBView.hFrameWnd = hFrameWnd;
    CBView.ID = ID;
    CBView.bPre = bPre;
    CBView.bNext = bNext;
    CBView.hMsgWnd = hMsgWnd;
	
	wc.style         = 0;
    wc.lpfnWndProc   = CBViewWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = sizeof(CB_VIEW);
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = "CBViewWndClass";
        
    if (!RegisterClass(&wc))
        return FALSE;
    
    CBView.hMenu = CreateMenu();

    GetClientRect(hFrameWnd,&rect);

    hCBViewWnd = CreateWindow(
        "CBViewWndClass", 
        "",
        WS_VISIBLE|WS_CHILD,
        rect.left,
        rect.top,
        rect.right - rect.left,
        rect.bottom - rect.top,
        hFrameWnd, 
        (HMENU)IDC_CB_VIEW,
        NULL, 
        (PVOID)&CBView
        );
    
    if (!hCBViewWnd)
    {
        DestroyMenu(CBView.hMenu);

        UnregisterClass("CBViewWndClass",NULL);

        return FALSE;
    }
    
	PDASetMenu(hFrameWnd,CBView.hMenu);

    sztemp[0] = 0;

    itoa(CBView.ID,sztemp,10);

    SetWindowText(hFrameWnd,sztemp);

	SetFocus(hCBViewWnd);

    pData = GetUserData(hCBViewWnd);

    if(pData->bPre)
        SendMessage(hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, LEFTICON),(LPARAM)SMS_ICO_ARROWLEFT);
    else
        SendMessage(hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, LEFTICON),(LPARAM)"");

    if(pData->bNext)
        SendMessage(hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, RIGHTICON),(LPARAM)SMS_ICO_ARROWRIGHT);
    else
        SendMessage(hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, RIGHTICON),(LPARAM)"");

	SendMessage(hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_EXIT,0), (LPARAM)IDS_BACK);
    SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_OPTIONS);
    
    ShowWindow(hCBViewWnd, SW_SHOW);
    UpdateWindow(hCBViewWnd);

	return TRUE;	
}
/********************************************************************
* Function	   CBViewWndProc
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static LRESULT CALLBACK CBViewWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult;
	
    lResult = (LRESULT)TRUE;
    
    switch (message)
    {
    case WM_CREATE:
        lResult = (LRESULT)CBView_OnCreate(hWnd,(LPCREATESTRUCT)(lParam));
        break;
        
    case WM_ACTIVATE:
    case PWM_SHOWWINDOW:
        CBView_OnActivate(hWnd,(UINT)LOWORD(wParam));
        break;

    case WM_INITMENU:
        CBView_OnInitmenu(hWnd);
        break;

	case WM_SETFOCUS:
		CBView_OnSetFocus(hWnd);
		break;
        
    case WM_PAINT:
        CBView_OnPaint(hWnd);
        break;
        
    case WM_KEYDOWN:
        CBView_OnKey(hWnd,(UINT)(wParam),(int)(short)LOWORD(lParam),(UINT)HIWORD(lParam));
        break;
        
    case WM_COMMAND:
        CBView_OnCommand(hWnd,(int)(LOWORD(wParam)),(UINT)HIWORD(wParam));
        break;
                
    case WM_CLOSE:
        CBView_OnClose(hWnd);
        break;
        
    case WM_DESTROY:
        CBView_OnDestroy(hWnd);
        break;
        
    default :
        lResult = PDADefWindowProc(hWnd, message, wParam, lParam);
        break;
    } 
    return lResult;
}
/*********************************************************************\
* Function	CBView_OnCreate
* Purpose   WM_CREATE message handler of the main window
* Params
*			hWnd: Handle of the window
*			lpCreateStruct: Create Structure
* Return
*			TRUE: Success
*			FALSE: Fail
* Remarks
**********************************************************************/
static BOOL CBView_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
{
    CB_VIEW *pData;
    int nTextLen;
        
    pData = (CB_VIEW*)GetUserData(hWnd);

    memcpy(pData,lpCreateStruct->lpCreateParams,sizeof(CB_VIEW));
    
    CB_ReadText(pData->ID, &(pData->pszText));

    if(pData->pszText)
        nTextLen = strlen(pData->pszText);
    else
        nTextLen = 0;

	pData->hWndTextView = PlxTextView(pData->hFrameWnd, hWnd, pData->pszText,
        nTextLen,FALSE, NULL, NULL, 0);
     
    return TRUE;	
}
/*********************************************************************\
* Function	CBView_OnActivate
* Purpose   WM_ACTIVATE message handler of the main window
* Params
* Return    None
* Remarks
**********************************************************************/
static void CBView_OnActivate(HWND hWnd, UINT state)
{
    CB_VIEW *pData;
    char  sztemp[4];
        
    pData = (CB_VIEW*)GetUserData(hWnd);

    SetFocus(pData->hWndTextView);
    
	PDASetMenu(pData->hFrameWnd,pData->hMenu);

    sztemp[0] = 0;

    itoa(pData->ID,sztemp,10);

    SetWindowText(pData->hFrameWnd,sztemp);

    SendMessage(pData->hFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_BACK);   
    SendMessage(pData->hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_OPTIONS);

    if(pData->bPre)
        SendMessage(MuGetFrame(), PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, LEFTICON),(LPARAM)SMS_ICO_ARROWLEFT);
    else
        SendMessage(MuGetFrame(), PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, LEFTICON),(LPARAM)"");

    if(pData->bNext)
        SendMessage(MuGetFrame(), PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, RIGHTICON),(LPARAM)SMS_ICO_ARROWRIGHT);
    else
        SendMessage(MuGetFrame(), PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, RIGHTICON),(LPARAM)"");


    return;
}
/*********************************************************************\
* Function	CBView_OnInitmenu
* Purpose   WM_INITMENU message handler of the main window
* Params	hWnd: Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void CBView_OnInitmenu(HWND hWnd)
{
    HMENU hMenu,hSendMenu,hAddMenu,hFindMenu;
    PCB_VIEW pCreateData;
    int   nMenuItem = 0;
	DWORD  high, low;
	int    ret;
        
    pCreateData = GetUserData(hWnd);

    hMenu = pCreateData->hMenu;

    nMenuItem = GetMenuItemCount(hMenu);

    while(nMenuItem > 0)
    {
        nMenuItem--;
        DeleteMenu(hMenu,nMenuItem,MF_BYPOSITION);
    }

	ret = SendMessage(pCreateData->hWndTextView, TVM_GETCURHL, (WPARAM)&high, (LPARAM)&low);

    if(ret != -1 && low == TVS_URL)
    {
        AppendMenu(hMenu,MF_ENABLED, IDM_CBVIEW_GOTOURL, IDS_GOTOURL);
        AppendMenu(hMenu,MF_ENABLED, IDM_CBVIEW_ADDTOBM, IDS_ADDTOBOOKMARK);
    }
    
    if(ret != -1 && low == TVS_COORDINATE)
    {
        AppendMenu(hMenu,MF_ENABLED, IDM_CBVIEW_VIEWONMAP, IDS_VIEWONMAP);
        AppendMenu(hMenu,MF_ENABLED, IDM_CBVIEW_SETASDESTINATION, IDS_SETASDESTINATION);
        AppendMenu(hMenu,MF_ENABLED, IDM_CBVIEW_SAVEASWAYPOINT, IDS_SAVEASWAYPOINT);
    }
    
    if(ret != -1 && low == TVS_NUMBER)
    {
        hSendMenu = CreateMenu();   
        AppendMenu(hSendMenu,MF_ENABLED, IDM_CBVIEW_SEND_SMS, IDS_SMS);
        AppendMenu(hSendMenu,MF_ENABLED, IDM_CBVIEW_SEND_MMS, IDS_MMS);
        AppendMenu(hMenu,MF_POPUP|MF_ENABLED, (DWORD)hSendMenu, IDS_WRITEMEASSAGE);
    }
    else if(ret != -1 && low == TVS_EMAIL)
    {
        hSendMenu = CreateMenu();   
        AppendMenu(hSendMenu,MF_ENABLED, IDM_CBVIEW_SEND_MMS, IDS_MMS);
        AppendMenu(hSendMenu,MF_ENABLED, IDM_CBVIEW_SEND_EMAIL, IDS_EMAIL);
        AppendMenu(hMenu,MF_POPUP|MF_ENABLED, (DWORD)hSendMenu, IDS_WRITEMEASSAGE);
    }

    if(ret != -1 && low != TVS_COORDINATE)
    {
        hAddMenu = CreateMenu();   
        AppendMenu(hAddMenu,MF_ENABLED, IDM_CBVIEW_ADDTOCONTACTS_CREATE, IDS_CREATENEW);
        AppendMenu(hAddMenu,MF_ENABLED, IDM_CBVIEW_ADDTOCONTACTS_UPDATE, IDS_UPDATEEXISTING);
        AppendMenu(hMenu,MF_POPUP|MF_ENABLED, (DWORD)hAddMenu, IDS_SAVETOCONTACTS);
    }
    
    if( ret != -1 )
        AppendMenu(hMenu,MF_ENABLED, IDM_CBVIEW_HIDEFOUNDITEMS, IDS_HIDEFOUNDITEMS);
    else
    {
        hFindMenu = CreateMenu();   
        AppendMenu(hFindMenu,MF_ENABLED, IDM_CBVIEW_FIND_PHONE, IDS_PHONENO);
        AppendMenu(hFindMenu,MF_ENABLED, IDM_CBVIEW_FIND_EMAIL, IDS_EMAILADDRESS);
        AppendMenu(hFindMenu,MF_ENABLED, IDM_CBVIEW_FIND_WEB, IDS_WEB);
        AppendMenu(hFindMenu,MF_ENABLED, IDM_CBVIEW_FIND_COORDINATE, IDS_COORDINATE);
        AppendMenu(hMenu,MF_POPUP|MF_ENABLED, (DWORD)hFindMenu, IDS_FIND);
    }

    return;

}
/*********************************************************************\
* Function	CBView_OnSetFocus
* Purpose   WM_SETFOCUS message handler of the main window
* Params
* Return    None
* Remarks
**********************************************************************/
static void CBView_OnSetFocus(HWND hWnd)
{
    CB_VIEW *pData;
        
    pData = (CB_VIEW*)GetUserData(hWnd);

    SetFocus(pData->hWndTextView);

    return;
}
/*********************************************************************\
* Function	CBView_OnPaint
* Purpose   WM_PAINT message handler of the main window
* Params	hWnd: Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void CBView_OnPaint(HWND hWnd)
{
	HDC hdc = BeginPaint(hWnd, NULL);

    EndPaint(hWnd, NULL);

	return;
}
/*********************************************************************\
* Function	CBView_OnKey
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
static void CBView_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags)
{    
    CB_VIEW *pData;
        
    pData = (CB_VIEW*)GetUserData(hWnd);

	switch (vk)
	{
	case VK_F10:
        PostMessage(hWnd,WM_CLOSE,0,0);
		break;
        
    case VK_F5:
		PDADefWindowProc(pData->hFrameWnd, WM_KEYDOWN, vk, MAKELPARAM(cRepeat, flags));
        break;

    case VK_F1:
        {
            char              *phonenum;
            int               ret, offset, len;
            DWORD             wparam, lparam;
            
            // if there is high lighted
            ret = SendMessage(pData->hWndTextView, TVM_GETCURHL, (WPARAM)&wparam, (LPARAM)&lparam);
            if (ret != -1 && lparam == TVS_NUMBER)
            {		
                offset = LOWORD(wparam);
                len = HIWORD(wparam);
                
                phonenum = malloc(len + 1);
                strncpy(phonenum, pData->pszText + offset,len);
                phonenum[len] = 0;	
                APP_CallPhoneNumber(phonenum);
                free(phonenum);
            }
        }
        break;
        
        
    case VK_LEFT:
        if(pData->bPre)
        {
            PostMessage(hWnd,WM_CLOSE,0,0);
            PostMessage(pData->hMsgWnd,WM_CBMSG_PRE,NULL,NULL); 
        }
        break;

    case VK_RIGHT:
        if(pData->bNext)
        {
            PostMessage(hWnd,WM_CLOSE,0,0);
            PostMessage(pData->hMsgWnd,WM_CBMSG_NEXT,NULL,NULL);
        }
        break;


	default:
		PDADefWindowProc(hWnd, WM_KEYDOWN, vk, MAKELPARAM(cRepeat, flags));
		break;
	}

	return;
}
/*********************************************************************\
* Function	CBView_OnCommand
* Purpose   WM_COMMAND message handler of the main window
* Params
*			hWnd:	Handle of the window
*			id:		Id of command message
*			hwndCtl: Handle of the window which sended this message
*			codeNotify:Notify code of the message
* Return	None
* Remarks
**********************************************************************/
static void CBView_OnCommand(HWND hWnd, int id, UINT codeNotify)
{
    CB_VIEW *pData;
        
    pData = (CB_VIEW*)GetUserData(hWnd);

    switch(id)
	{   
    case IDM_CBVIEW_GOTOURL:
    case IDM_CBVIEW_ADDTOBM:
        {
            int ret, offset, len;
            DWORD high, low;
            char  *pUrl = NULL;
            
            // get the high lighted items
            ret = SendMessage(pData->hWndTextView, TVM_GETCURHL, (WPARAM)&high, (LPARAM)&low);
            if (ret != -1 && low == TVS_URL)	// highlighted
            {
                offset = LOWORD(high);
                len = HIWORD(high);
                pUrl = malloc(len + 1);
                strncpy(pUrl, pData->pszText + offset,len);
                pUrl[len] = 0;
                // open browser and go to high lighted web address...(goto url)
                // add highlighted web address to browser bookmark...(add to bk)
                if(id == IDM_CBVIEW_GOTOURL)
                    App_WapRequestUrl(pUrl);
                else
                    WBM_ADD_FROMURL(pUrl,pData->hFrameWnd);
                
                free(pUrl);
            }
        }
        break;

    case IDM_CBVIEW_VIEWONMAP:
        break;

    case IDM_CBVIEW_SETASDESTINATION:
        break;

    case IDM_CBVIEW_SAVEASWAYPOINT:
        break;

    case IDM_CBVIEW_SEND_SMS:
    case IDM_CBVIEW_SEND_MMS:
    case IDM_CBVIEW_SEND_EMAIL:
        {		
            // get the high lighted items...
            // select highlighted phone number or email address to recipient
            // of the msg and open mail editor....
            SYSTEMTIME time;
            int		ret = 0, offset = 0,len = 0;
            DWORD   high = 0, low = 0;
            char    *pAddr = NULL;
            
            ret = SendMessage(pData->hWndTextView, TVM_GETCURHL, (WPARAM)&high, (LPARAM)&low);
            if (ret == -1)
                break;
            // get highlighted items
            offset = LOWORD(high);
            len = HIWORD(high);
            pAddr = malloc(len + 1);
            strncpy(pAddr, pData->pszText + offset, len);

            pAddr[len] = 0;
            
            if (low == TVS_NUMBER)	// highlighted
            {
                if (id == IDM_CBVIEW_SEND_SMS)
                    APP_EditSMS(pData->hFrameWnd, pAddr, NULL);
                else if (id == IDM_CBVIEW_SEND_MMS)
                    APP_EditMMS(pData->hFrameWnd, hWnd, 0, MMS_CALLEDIT_MOBIL,pAddr);		
            }
            else if (low == TVS_EMAIL)
            {
                if (id == IDM_CBVIEW_SEND_MMS)
                    APP_EditMMS(pData->hFrameWnd, hWnd, 0, MMS_CALLEDIT_MOBIL,pAddr);
                else if (id == IDM_CBVIEW_SEND_EMAIL)
                {
                    GetLocalTime(&time);
                    CreateMailEditWnd(pData->hFrameWnd, pAddr, NULL, NULL, NULL, NULL, 
                        &time, -1, -1);
                }
            }
            free(pAddr);
        }
        break;
       
    case IDM_CBVIEW_ADDTOCONTACTS_UPDATE:
    case IDM_CBVIEW_ADDTOCONTACTS_CREATE:
        {
            int		ret = 0, offset = 0,len = 0;
            DWORD   high = 0, low = 0;
            ABNAMEOREMAIL ABName;
            int     nMode;

            if(id == IDM_CBVIEW_ADDTOCONTACTS_UPDATE)
                nMode = AB_UPDATE;
            else
                nMode = AB_NEW;

            memset(&ABName,0,sizeof(ABNAMEOREMAIL));
            
            ret = SendMessage(pData->hWndTextView, TVM_GETCURHL, (WPARAM)&high, (LPARAM)&low);
            
            if(ret == -1 )
            {
                break;
            }
            // get highlighted items
            offset = LOWORD(high);
            len = HIWORD(high);

            len = min(len,AB_MAXLEN_EMAILADDR-1);

            strncpy(ABName.szTelOrEmail,pData->pszText+offset,len);

            ABName.szTelOrEmail[len] = 0;
            
            switch(low)
            {
            case TVS_NUMBER:
                ABName.nType = AB_NUMBER;
            	break;

            case TVS_EMAIL:
                ABName.nType = AB_EMAIL;
            	break;

            case TVS_URL:
                ABName.nType = AB_URL;
                break;

            default:
                return;
            }
            
            APP_SaveToAddressBook(pData->hFrameWnd,NULL,0,&ABName,nMode);
        }
        break;
   
    case IDM_CBVIEW_FIND_PHONE:   
        {
            int ret = 0;
            
            ret = SendMessage(pData->hWndTextView, TVM_FINDNHL, 0, TVS_NUMBER);
            if (ret == -1)		// search found no results
                PLXTipsWin(NULL, NULL, 0, IDS_NOMATCHESFOUND, IDS_SMS,
                Notify_Info, IDS_OK, NULL, WAITTIMEOUT);
        }
        break;

    case IDM_CBVIEW_FIND_EMAIL: 
        {
            int ret = 0;
            
            ret = SendMessage(pData->hWndTextView, TVM_FINDNHL, 0, TVS_EMAIL);
            if (ret == -1)		// search found no results
                PLXTipsWin(NULL, NULL, 0, IDS_NOMATCHESFOUND, IDS_SMS,
                Notify_Info, IDS_OK, NULL, WAITTIMEOUT);
        }
        break;

    case IDM_CBVIEW_FIND_WEB: 
        {
            int ret = 0;
            
            ret = SendMessage(pData->hWndTextView, TVM_FINDNHL, 0, TVS_URL);
            if (ret == -1)		// search found no results
                PLXTipsWin(NULL, NULL, 0, IDS_NOMATCHESFOUND, IDS_SMS,
                Notify_Info, IDS_OK, NULL, WAITTIMEOUT);
        }
        break;

    case IDM_CBVIEW_FIND_COORDINATE: 
        {
            int ret = 0;
            
            ret = SendMessage(pData->hWndTextView, TVM_FINDNHL, 0, TVS_COORDINATE);
            if (ret == -1)		// search found no results
                PLXTipsWin(NULL, NULL, 0, IDS_NOMATCHESFOUND, IDS_SMS,
                Notify_Info, IDS_OK, NULL, WAITTIMEOUT);
        }
        break;

    case IDM_CBVIEW_HIDEFOUNDITEMS:
		// remove highlights from the msg body
		SendMessage(pData->hWndTextView, TVM_HIDEHL, 0, 0);
        break;
      
    default:
        break;
	}

	return;
}
/*********************************************************************\
* Function	CBView_OnDestroy
* Purpose   WM_DESTROY message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void CBView_OnDestroy(HWND hWnd)
{

    CB_VIEW *pData;

    pData = GetUserData(hWnd);

    DestroyMenu(pData->hMenu);

    SMS_FREE(pData->pszText);

	UnregisterClass("CBViewWndClass", NULL);

    return;

}
/*********************************************************************\
* Function	CBView_OnClose
* Purpose   WM_CLOSE message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void CBView_OnClose(HWND hWnd)
{
    CB_VIEW *pData;

    pData = GetUserData(hWnd);

    SendMessage(pData->hFrameWnd,PWM_CLOSEWINDOW,(WPARAM)hWnd,NULL);

    DestroyWindow (hWnd);
	
    return;

}
/*********************************************************************\
* Function	CB_ReadText
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
static BOOL CB_ReadText(short ID, char** ppszText)
{
    char szOldPath[PATH_MAXLEN];
    int f;
    CBS_INFO cb_info;
    char szFileName[7];
    int nLen = 0;
    BOOL bRead = TRUE;

#if 0
    *ppszText = (char*)malloc(94);

    memset(*ppszText,0,94);

    strcpy(*ppszText,"1234567890 http://www.hotmail.com (10,23) kandongtianbeidexueyuanlaiyuanyue a@b.c");

    return TRUE;
#endif

    szOldPath[0] = 0;
    
    getcwd(szOldPath,PATH_MAXLEN);

    chdir(PATH_DIR_SMS);

    szFileName[0] = 0;

    sprintf(szFileName,"%d.cb",ID);
    
    f = open(szFileName,O_RDWR);
    
    if( f == -1 )
    {
        chdir(szOldPath);

        return FALSE;
    }

#ifdef _SMS_DEBUG_
        printf("\r\n*****SMS Debug Info*****  CB_ReadText file handle = %d \r\n",f);
#endif
    
    write(f,&bRead,sizeof(int));

    read(f,&cb_info,sizeof(CBS_INFO));
    
    close(f);
    
    chdir(szOldPath);

    return SMS_ParseContentEx(cb_info.Code,cb_info.Data,cb_info.DataLen,ppszText,&nLen);

}

/********************************************************************
* Function	   CreateLanguageList
* Purpose      
* Params	   
* Return	   
* Remarks	   
**********************************************************************/
static BOOL CreateLanguageList(HWND hFrameWnd,HWND hMsgWnd,UINT uMsgCmd,unsigned long code)
{
	WNDCLASS	wc;
    HWND        hWnd;
    CB_LANGUAGESELECT     Data;
    RECT        rc;

    memset(&Data,0,sizeof(CB_LANGUAGESELECT));

    Data.hFrameWnd = hFrameWnd;
    Data.hMsgWnd = hMsgWnd;
    Data.uMsgCmd = uMsgCmd;
    Data.code = code;
	
	wc.style         = 0;
    wc.lpfnWndProc   = CBLanguageSelectWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = sizeof(CB_LANGUAGESELECT);
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = "CBLanguageSelectWndClass";

    if (!RegisterClass(&wc))
        return FALSE;

    GetClientRect(hFrameWnd,&rc);

    hWnd = CreateWindow(
        "CBLanguageSelectWndClass", 
        "", 
        WS_VISIBLE|WS_CHILD,
        rc.left,
        rc.top,
        rc.right - rc.left,
        rc.bottom - rc.top,
        hFrameWnd, 
        (HMENU)IDS_CB_LANGUANGESELECT,
        NULL, 
        (PVOID)&Data
        );
    
    if (!hWnd)
    {
        UnregisterClass("CBLanguageSelectWndClass",NULL);
        return FALSE;
    }

    SetFocus(hWnd);
        
	SendMessage(hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_EXIT,0), (LPARAM)IDS_CANCEL);
	SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_SELECT);

    SetWindowText(hFrameWnd,IDS_LANGUAGES);

    ShowWindow(hWnd, SW_SHOW);
    UpdateWindow(hWnd);

	return TRUE;	
}
/********************************************************************
* Function	   CBLanguageSelectWndProc
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static LRESULT CALLBACK CBLanguageSelectWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult;
	
    lResult = (LRESULT)TRUE;
    
    switch (message)
    {
    case WM_CREATE:
        lResult = (LRESULT)CBLS_OnCreate(hWnd,(LPCREATESTRUCT)(lParam));
        break;
        
    case PWM_SHOWWINDOW:
        CBLS_OnActivate(hWnd,(UINT)LOWORD(wParam));
        break;

    case WM_SETFOCUS:
        CBLS_OnSetFocus(hWnd);
        break;
        
    case WM_PAINT:
        CBLS_OnPaint(hWnd);
        break;
        
    case WM_KEYDOWN:
        CBLS_OnKey(hWnd,(UINT)(wParam),(int)(short)LOWORD(lParam),(UINT)HIWORD(lParam));
        break;
        
    case WM_COMMAND:
        CBLS_OnCommand(hWnd,(int)(LOWORD(wParam)),(UINT)HIWORD(wParam));
        break;
                
    case WM_CLOSE:
        CBLS_OnClose(hWnd);
        break;
        
    case WM_DESTROY:
        CBLS_OnDestroy(hWnd);
        break;
        
    default :
        lResult = PDADefWindowProc(hWnd, message, wParam, lParam);
        break;
    } 
    return lResult;
}
/*********************************************************************\
* Function	CBLS_OnCreate
* Purpose   WM_CREATE message handler of the main window
* Params
*			hWnd: Handle of the window
*			lpCreateStruct: Create Structure
* Return
*			TRUE: Success
*			FALSE: Fail
* Remarks
**********************************************************************/
static BOOL CBLS_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
{
    HWND hLst;
    RECT rcClient;
    int  index;    
    CB_LANGUAGESELECT     *pData;
    SIZE size;
        
    pData = (CB_LANGUAGESELECT*)GetUserData(hWnd);

    memcpy(pData,lpCreateStruct->lpCreateParams,sizeof(CB_LANGUAGESELECT));
    
    GetClientRect(hWnd,&rcClient);

    hLst = CreateWindow(
        "LISTBOX",
        "",
        WS_VISIBLE|WS_CHILD|LBS_BITMAP|WS_VSCROLL,
        rcClient.left,
        rcClient.top,
        rcClient.right - rcClient.left,
        rcClient.bottom - rcClient.top,
        hWnd,
        (HMENU)IDC_CBLS_LIST,
        NULL,
        NULL);

    if( hLst == NULL )
        return FALSE;
        
    index = SendMessage(hLst,LB_ADDSTRING,-1,(LPARAM)IDS_ALL);
    index = SendMessage(hLst,LB_ADDSTRING,-1,(LPARAM)IDS_SELECTED);

    GetImageDimensionFromFile(SMS_BMP_SELECT,&size);
    
    pData->hSelected = LoadImage(NULL, SMS_BMP_SELECT, IMAGE_BITMAP,
        size.cx, size.cy, LR_LOADFROMFILE);
    
    GetImageDimensionFromFile(SMS_BMP_NORMAL,&size);
    
    pData->hNormal = LoadImage(NULL, SMS_BMP_NORMAL, IMAGE_BITMAP,
        size.cx, size.cy, LR_LOADFROMFILE);
    
    if(pData->code == 0)
    {
        SendMessage(hLst,LB_SETCURSEL,0,0);
        SendMessage(hLst,LB_SETIMAGE,MAKEWPARAM(IMAGE_BITMAP,0),(LPARAM)pData->hSelected);
        SendMessage(hLst,LB_SETIMAGE,MAKEWPARAM(IMAGE_BITMAP,1),(LPARAM)pData->hNormal);
    }
    else
    {
        SendMessage(hLst,LB_SETCURSEL,1,0);
        SendMessage(hLst,LB_SETIMAGE,MAKEWPARAM(IMAGE_BITMAP,0),(LPARAM)pData->hNormal);
        SendMessage(hLst,LB_SETIMAGE,MAKEWPARAM(IMAGE_BITMAP,1),(LPARAM)pData->hSelected);
    }
 
    return TRUE;	
}
/*********************************************************************\
* Function	CBLS_OnActivate
* Purpose   WM_ACTIVATE message handler of the main window
* Params
* Return    None
* Remarks
**********************************************************************/
static void CBLS_OnActivate(HWND hWnd, UINT state)
{
    CB_LANGUAGESELECT     *pData;

    HWND hLst;
        
    pData = (CB_LANGUAGESELECT*)GetUserData(hWnd);

    hLst = GetDlgItem(hWnd,IDC_CBLS_LIST);

    SetFocus(hLst);

	SendMessage(pData->hFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_CANCEL);
	SendMessage(pData->hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_SELECT);

    SetWindowText(pData->hFrameWnd,IDS_LANGUAGES);

    return;
}
/*********************************************************************\
* Function	CBLS_OnSetFocus
* Purpose   WM_SETFOCUS message handler of the main window
* Params	hWnd: Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void CBLS_OnSetFocus(HWND hWnd)
{
    HWND hLst;
        
    hLst = GetDlgItem(hWnd,IDC_CBLS_LIST);

    SetFocus(hLst);
}
/*********************************************************************\
* Function	CBLS_OnPaint
* Purpose   WM_PAINT message handler of the main window
* Params	hWnd: Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void CBLS_OnPaint(HWND hWnd)
{
	HDC hdc = BeginPaint(hWnd, NULL);

    EndPaint(hWnd, NULL);

	return;
}
/*********************************************************************\
* Function	CBLS_OnKey
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
static void CBLS_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags)
{    
    CB_LANGUAGESELECT     *pData;
    HWND hLst;
    int  index;
        
    pData = (CB_LANGUAGESELECT*)GetUserData(hWnd);

    hLst = GetDlgItem(hWnd,IDC_CBLS_LIST);

    switch (vk)
    {
    case VK_F10:
        SendMessage(pData->hMsgWnd,pData->uMsgCmd,FALSE,0);
        SendMessage(pData->hFrameWnd,PWM_CLOSEWINDOW,(WPARAM)hWnd,0);
        PostMessage(hWnd,WM_CLOSE,0,0);
		break;

    case VK_F5:
        index = SendMessage(hLst,LB_GETCURSEL,0,0);

        switch(index) 
        {
        case 0:
            SendMessage(pData->hMsgWnd,pData->uMsgCmd,TRUE,0);
            SendMessage(pData->hFrameWnd,PWM_CLOSEWINDOW,(WPARAM)hWnd,0);
            PostMessage(hWnd,WM_CLOSE,0,0);
        	break;

        case 1:
            CreateLanguageMultiList(pData->hFrameWnd,pData->hMsgWnd,pData->uMsgCmd,pData->code);
            break;

        case LB_ERR:
        default:
            break;
        }
        break;

	default:
		PDADefWindowProc(hWnd, WM_KEYDOWN, vk, MAKELPARAM(cRepeat, flags));
		break;
	}

	return;
}
/*********************************************************************\
* Function	CBLS_OnCommand
* Purpose   WM_COMMAND message handler of the main window
* Params
*			hWnd:	Handle of the window
*			id:		Id of command message
*			hwndCtl: Handle of the window which sended this message
*			codeNotify:Notify code of the message
* Return	None
* Remarks
**********************************************************************/
static void CBLS_OnCommand(HWND hWnd, int id, UINT codeNotify)
{
    switch(id)
	{
        
    default:
        break;
	}

	return;
}
/*********************************************************************\
* Function	CBLS_OnDestroy
* Purpose   WM_DESTROY message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void CBLS_OnDestroy(HWND hWnd)
{
    CB_LANGUAGESELECT     *pData;
        
    pData = (CB_LANGUAGESELECT*)GetUserData(hWnd);

    if(pData->hSelected)
        DeleteObject(pData->hSelected);

    if(pData->hNormal)
        DeleteObject(pData->hNormal);

	UnregisterClass("CBLanguageSelectWndClass", NULL);

    return;

}
/*********************************************************************\
* Function	CBLS_OnClose
* Purpose   WM_CLOSE message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void CBLS_OnClose(HWND hWnd)
{
    DestroyWindow (hWnd);
	
    return;

}



/********************************************************************
* Function	   CreateLanguageMultiList
* Purpose      
* Params	   
* Return	   
* Remarks	   
**********************************************************************/
static BOOL CreateLanguageMultiList(HWND hFrameWnd,HWND hMsgWnd,UINT uMsgCmd,unsigned long code)
{
	WNDCLASS	wc;
    HWND        hWnd;
    CB_LANGUAGESELECT     Data;
    RECT        rc;

    memset(&Data,0,sizeof(CB_LANGUAGESELECT));

    Data.hFrameWnd = hFrameWnd;
    Data.hMsgWnd = hMsgWnd;
    Data.uMsgCmd = uMsgCmd;
    Data.code = code;
	
	wc.style         = 0;
    wc.lpfnWndProc   = CBLanguageMultiSelectWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = sizeof(CB_LANGUAGESELECT);
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = "CBLanguageMultiSelectWndClass";

    if (!RegisterClass(&wc))
        return FALSE;

    GetClientRect(hFrameWnd,&rc);

    hWnd = CreateWindow(
        "CBLanguageMultiSelectWndClass", 
        "", 
        WS_VISIBLE|WS_CHILD,
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
        UnregisterClass("CBLanguageMultiSelectWndClass",NULL);
        return FALSE;
    }
   
    SetFocus(hWnd); 

	SendMessage(hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_OK,1), (LPARAM)IDS_SAVE);
	SendMessage(hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_EXIT,0), (LPARAM)IDS_CANCEL);
	SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_SELECT);

    SetWindowText(hFrameWnd,IDS_SELECTLANGUAGES);
    
    ShowWindow(hWnd, SW_SHOW);
    UpdateWindow(hWnd);

	return TRUE;	
}
/********************************************************************
* Function	   CBLanguageMultiSelectWndProc
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static LRESULT CALLBACK CBLanguageMultiSelectWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult;
	
    lResult = (LRESULT)TRUE;
    
    switch (message)
    {
    case WM_CREATE:
        lResult = (LRESULT)CBMLS_OnCreate(hWnd,(LPCREATESTRUCT)(lParam));
        break;
        
    case PWM_SHOWWINDOW:
        CBMLS_OnActivate(hWnd,(UINT)LOWORD(wParam));
        break;

    case WM_SETFOCUS:
        CBMLS_OnSetFocus(hWnd);
        break;
        
    case WM_PAINT:
        CBMLS_OnPaint(hWnd);
        break;
        
    case WM_KEYDOWN:
        CBMLS_OnKey(hWnd,(UINT)(wParam),(int)(short)LOWORD(lParam),(UINT)HIWORD(lParam));
        break;
        
    case WM_COMMAND:
        CBMLS_OnCommand(hWnd,(int)(LOWORD(wParam)),(UINT)HIWORD(wParam));
        break;
                
    case WM_CLOSE:
        CBMLS_OnClose(hWnd);
        break;
        
    case WM_DESTROY:
        CBMLS_OnDestroy(hWnd);
        break;

    case WM_INITSELECT:
        CBMLS_OnInitSelect(hWnd);
        break;
        
    default :
        lResult = PDADefWindowProc(hWnd, message, wParam, lParam);
        break;
    } 
    return lResult;
}
/*********************************************************************\
* Function	CBMLS_OnCreate
* Purpose   WM_CREATE message handler of the main window
* Params
*			hWnd: Handle of the window
*			lpCreateStruct: Create Structure
* Return
*			TRUE: Success
*			FALSE: Fail
* Remarks
**********************************************************************/
static BOOL CBMLS_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
{
    HWND hLst;
    RECT rcClient;
    CB_LANGUAGESELECT     *pData;
    int i,index;
        
    pData = (CB_LANGUAGESELECT*)GetUserData(hWnd);

    memcpy(pData,lpCreateStruct->lpCreateParams,sizeof(CB_LANGUAGESELECT));
    
    GetClientRect(hWnd,&rcClient);

    hLst = CreateWindow(
        "MULTILISTBOX",
        "",
        WS_VISIBLE|WS_CHILD|LBS_BITMAP|WS_VSCROLL,
        rcClient.left,
        rcClient.top,
        rcClient.right - rcClient.left,
        rcClient.bottom - rcClient.top,
        hWnd,
        (HMENU)IDC_CBMLS_LIST,
        NULL,
        NULL);
    
    if( hLst == NULL )
        return FALSE;
    
    for(i = 0 ; i < CB_LANGUAGE_MAX ; i++)
    {
        index = SendMessage(hLst,LB_ADDSTRING,-1,(LPARAM)CB_Language[i].pszLanguage);
        SendMessage(hLst,LB_SETITEMDATA,index,(LPARAM)CB_Language[i].mask);
        if(pData->code & CB_Language[i].mask)
            SendMessage(hLst,LB_SETSEL,(WPARAM)TRUE,(LPARAM)index);
    }

	SendMessage(hLst, LB_ENDINIT, 0, 0);
    SendMessage(hLst,LB_SETCURSEL,0,0);

    SendMessage(hWnd,WM_INITSELECT,0,0);
 
    return TRUE;	
}
/*********************************************************************\
* Function	CBMLS_OnActivate
* Purpose   WM_ACTIVATE message handler of the main window
* Params
* Return    None
* Remarks
**********************************************************************/
static void CBMLS_OnActivate(HWND hWnd, UINT state)
{
    CB_LANGUAGESELECT     *pData;

    HWND hLst;
        
    pData = (CB_LANGUAGESELECT*)GetUserData(hWnd);

    hLst = GetDlgItem(hWnd,IDC_CBMLS_LIST);

    SetFocus(hLst);

	SendMessage(pData->hFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_CANCEL);
	SendMessage(pData->hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_SAVE);
	SendMessage(pData->hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_SELECT);

    SetWindowText(pData->hFrameWnd,IDS_SELECTLANGUAGES);

    return;
}
/*********************************************************************\
* Function	CBLS_OnSetFocus
* Purpose   WM_SETFOCUS message handler of the main window
* Params	hWnd: Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void CBMLS_OnSetFocus(HWND hWnd)
{
    HWND hLst;
        
    hLst = GetDlgItem(hWnd,IDC_CBMLS_LIST);

    SetFocus(hLst);
}
/*********************************************************************\
* Function	CBMLS_OnPaint
* Purpose   WM_PAINT message handler of the main window
* Params	hWnd: Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void CBMLS_OnPaint(HWND hWnd)
{
	HDC hdc = BeginPaint(hWnd, NULL);

    EndPaint(hWnd, NULL);

	return;
}
/*********************************************************************\
* Function	CBMLS_OnKey
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
static void CBMLS_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags)
{    
    CB_LANGUAGESELECT     *pData;
    HWND hLst;
        
    pData = (CB_LANGUAGESELECT*)GetUserData(hWnd);

    hLst = GetDlgItem(hWnd,IDC_CBMLS_LIST);

    switch (vk)
    {
    case VK_F10:
        SendMessage(pData->hMsgWnd,pData->uMsgCmd,FALSE,0);
        SendMessage(pData->hFrameWnd,PWM_CLOSEWINDOW,(WPARAM)hWnd,0);
        PostMessage(hWnd,WM_CLOSE,0,0);
		break;

    case VK_RETURN:
        {
            int nCount,i;
            DWORD dwMask;

            nCount = SendMessage(hLst,LB_GETCOUNT,0,0);
            
            pData->code = 0;
            
            for(i = 0 ; i < nCount ; i++)
            {
                if(SendMessage(hLst,LB_GETSEL,i,0))
                {
                    dwMask = SendMessage(hLst,LB_GETITEMDATA,i,NULL);
                    pData->code |= dwMask;
                }
            }

            if(pData->code == 0)
            {
                PLXTipsWin(NULL, NULL, 0, IDS_PLSDEFINELANG, IDS_SELECTLANGUAGES,Notify_Alert, IDS_OK, NULL, WAITTIMEOUT);
                break;
            }

            SendMessage(pData->hMsgWnd,pData->uMsgCmd,TRUE,(LPARAM)pData->code);
            SendMessage(pData->hFrameWnd,PWM_CLOSEWINDOW,(WPARAM)hWnd,0);
            PostMessage(hWnd,WM_CLOSE,0,0);
        }
        break;

	default:
		PDADefWindowProc(hWnd, WM_KEYDOWN, vk, MAKELPARAM(cRepeat, flags));
		break;
	}

	return;
}
/*********************************************************************\
* Function	CBMLS_OnCommand
* Purpose   WM_COMMAND message handler of the main window
* Params
*			hWnd:	Handle of the window
*			id:		Id of command message
*			hwndCtl: Handle of the window which sended this message
*			codeNotify:Notify code of the message
* Return	None
* Remarks
**********************************************************************/
static void CBMLS_OnCommand(HWND hWnd, int id, UINT codeNotify)
{
    switch(id)
	{
        
    default:
        break;
	}

	return;
}
/*********************************************************************\
* Function	CBMLS_OnDestroy
* Purpose   WM_DESTROY message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void CBMLS_OnDestroy(HWND hWnd)
{

	UnregisterClass("CBLanguageMultiSelectWndClass", NULL);

    return;

}
/*********************************************************************\
* Function	CBMLS_OnClose
* Purpose   WM_CLOSE message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void CBMLS_OnClose(HWND hWnd)
{
    DestroyWindow (hWnd);
	
    return;

}
/*********************************************************************\
* Function	CBMLS_OnInitSelect
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
static void CBMLS_OnInitSelect(HWND hWnd)
{
    HWND hLst;
    CB_LANGUAGESELECT     *pData;
    int i;
    DWORD dwMask;
        
    pData = (CB_LANGUAGESELECT*)GetUserData(hWnd);

	hLst = GetDlgItem(hWnd,IDC_CBMLS_LIST);

    for(i = 0 ; i < CB_LANGUAGE_MAX ; i++)
    {
        dwMask = SendMessage(hLst,LB_GETITEMDATA,i,NULL);
        if(pData->code & dwMask)
            SendMessage(hLst,LB_SETSEL,(WPARAM)TRUE,(LPARAM)i);
    }
}

#define IDC_CBEDIT_EDIT     300
typedef struct tagCB_EditCreateData
{
    HWND  hFrameWnd;
    HWND  hMsgWnd;
    UINT  uMsgCmd;
    int   index;
    char  szNum[4];
}CB_EDITCREATEDATA,*PCB_EDITCREATEDATA;
/*********************************************************************\
* Function	   CB_CreateEditWnd
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL CB_CreateEditWnd(HWND hFrameWnd,HWND hMsgWnd,UINT uMsgCmd,char* pDefault,int index)
{    
    WNDCLASS wc;
    HWND     hWnd = NULL;
    RECT     rc;
    CB_EDITCREATEDATA Data;

    memset(&Data,0,sizeof(CB_EDITCREATEDATA));

    Data.hFrameWnd = hFrameWnd;
    Data.hMsgWnd = hMsgWnd;
    Data.uMsgCmd = uMsgCmd;
    Data.index = index;
    strcpy(Data.szNum,pDefault);

    GetClientRect(hFrameWnd,&rc);
        
    wc.style         = 0;
    wc.lpfnWndProc   = CBEditWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = sizeof(CB_EDITCREATEDATA);
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName  = "CBEditWndClass";
    
    if (!RegisterClass(&wc))
        return FALSE;

    hWnd = CreateWindow(
        "CBEditWndClass", 
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
        UnregisterClass("CBEditWndClass", NULL);
        return FALSE;
    }

    SetWindowText(hFrameWnd,IDS_ADDTOPIC);

    SendMessage(hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_OK,1), (LPARAM)IDS_SAVE);
    SendMessage(hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_CANCEL,0), (LPARAM)IDS_CANCEL);
    SendMessage(hFrameWnd,PWM_SETBUTTONTEXT,2,(LPARAM)"");

    SetFocus(hWnd);

    return TRUE;
        
}

/*********************************************************************\
* Function	CBEditWndProc
* Purpose   Main window proc
* Params
*			hWnd: Handle of the window
*			wMsgCmd: Message ID
* Return
*			TRUE: Success
*			FALSE: Fail
* Remarks
**********************************************************************/
static LRESULT CBEditWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    LRESULT lResult = TRUE;

	switch (wMsgCmd)
    {
    case WM_CREATE:
        lResult = CBEdit_OnCreate(hWnd,(LPCREATESTRUCT)(lParam));
        break;

    case PWM_SHOWWINDOW:
        CBEdit_OnActivate(hWnd,(UINT)LOWORD(wParam));
        break;

    case WM_SETFOCUS:
        CBEdit_OnSetFocus(hWnd);
        break;
        
    case WM_PAINT:
        CBEdit_OnPaint(hWnd);
        break;

    case WM_KEYDOWN:
        CBEdit_OnKey(hWnd,(UINT)(wParam),(int)(short)LOWORD(lParam),(UINT)HIWORD(lParam));
        break;

    case WM_COMMAND:
        CBEdit_OnCommand(hWnd,(int)(LOWORD(wParam)),(UINT)HIWORD(wParam));
        break;
        
    case WM_CLOSE:
        CBEdit_OnClose(hWnd);
        break;

    case WM_DESTROY:
        CBEdit_OnDestroy(hWnd);
        break;

    default:     
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
	}

    return lResult;

}
/*********************************************************************\
* Function	CBEdit_OnCreate
* Purpose   WM_CREATE message handler of the main window
* Params
*			hWnd: Handle of the window
*			lpCreateStruct: Create Structure
* Return
*			TRUE: Success
*			FALSE: Fail
* Remarks
**********************************************************************/
static BOOL CBEdit_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
{    
    HWND hEdit;
    RECT rc;
    CB_EDITCREATEDATA *pData;
    IMEEDIT ie;
    DWORD dwStyle;

    pData = GetUserData(hWnd);

    memcpy(pData,lpCreateStruct->lpCreateParams,sizeof(CB_EDITCREATEDATA));

    GetClientRect(hWnd,&rc);
    
    memset(&ie, 0, sizeof(IMEEDIT));
    
    ie.hwndNotify	= (HWND)hWnd;    
    ie.dwAttrib	    = 0;                
    ie.dwAscTextMax	= 0;
    ie.dwUniTextMax	= 0;
    ie.wPageMax	    = 0;        
    ie.pszCharSet	= NULL;
    ie.pszTitle	    = NULL;
    
    dwStyle = WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL | WS_TABSTOP | ES_TITLE | ES_NUMBER;
    ie.pszImeName	= "Digit";
    
    hEdit = CreateWindow(
        "IMEEDIT",
        "",
        dwStyle,
        rc.left,
        rc.top,
        rc.right - rc.left,
        (rc.bottom - rc.top) / 3,
        hWnd,
        (HMENU)IDC_CBEDIT_EDIT,
        NULL,
        (PVOID)&ie);
    
    if(hEdit == NULL)
        return FALSE;
    
    SendMessage(hEdit, EM_LIMITTEXT, (WPARAM)3, NULL);
    SendMessage(hEdit, EM_SETTITLE, 0, (LPARAM)IDS_TOPICNR);
    
    SetWindowText(hEdit, pData->szNum);

    SendMessage(hEdit, EM_SETSEL, -1, -1);
        
    return TRUE;
    
}
/*********************************************************************\
* Function	CBEdit_OnActivate
* Purpose   WM_ACTIVATE message handler of the main window
* Params
* Return    None
* Remarks
**********************************************************************/
static void CBEdit_OnActivate(HWND hWnd, UINT state)
{
    CB_EDITCREATEDATA *pData;

    pData = GetUserData(hWnd);

    SetFocus(GetDlgItem(hWnd,IDC_CBEDIT_EDIT));

    //SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,0,(LPARAM)IDS_CANCEL);
    SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,1,(LPARAM)IDS_SAVE);
    SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,2,(LPARAM)"");

    SetWindowText(pData->hFrameWnd,IDS_ADDTOPIC);

    return;
}
/*********************************************************************\
* Function	CBEdit_OnSetFocus
* Purpose   WM_INITMENU message handler of the main window
* Params	hWnd: Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void CBEdit_OnSetFocus(HWND hWnd)
{
    SetFocus(GetDlgItem(hWnd,IDC_CBEDIT_EDIT));

    return;
}
/*********************************************************************\
* Function	CBEdit_OnPaint
* Purpose   WM_PAINT message handler of the main window
* Params	hWnd: Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void CBEdit_OnPaint(HWND hWnd)
{
    HDC hdc = BeginPaint(hWnd, NULL);
    
    EndPaint(hWnd, NULL);

	return;
}

/*********************************************************************\
* Function	CBEdit_OnKey
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
static void CBEdit_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags)
{
    CB_EDITCREATEDATA *pData;
    
    HWND hEdit;

    pData = GetUserData(hWnd);

	switch (vk)
	{
	case VK_F10:
        SendMessage(pData->hMsgWnd,pData->uMsgCmd,FALSE,NULL);
        
        SendMessage(pData->hFrameWnd,PWM_CLOSEWINDOW,(WPARAM)hWnd,NULL);
		
        PostMessage(hWnd,WM_CLOSE,NULL,NULL);
		break;

    case VK_RETURN:
        hEdit = GetDlgItem(hWnd,IDC_CBEDIT_EDIT);
        
        if(GetWindowTextLength(hEdit) == 0)
        {
            SendMessage(pData->hMsgWnd,pData->uMsgCmd,FALSE,NULL);
            
            SendMessage(pData->hFrameWnd,PWM_CLOSEWINDOW,(WPARAM)hWnd,NULL);
            
            PostMessage(hWnd,WM_CLOSE,NULL,NULL);

            break;
        }

        GetWindowText(hEdit,pData->szNum,4);

        if(CB_IsExist(hWnd,pData->szNum,pData->index))
        {
            PLXTipsWin(NULL, NULL, 0, IDS_TOPICEXISTS, IDS_ADDTOPIC,Notify_Failure, IDS_OK, NULL, WAITTIMEOUT);
            break;
        }

        SendMessage(pData->hMsgWnd,pData->uMsgCmd,TRUE,(LPARAM)pData->szNum);
        
        SendMessage(pData->hFrameWnd,PWM_CLOSEWINDOW,(WPARAM)hWnd,NULL);

		PostMessage(hWnd,WM_CLOSE,NULL,NULL);
        break;

	default:
		PDADefWindowProc(hWnd, WM_KEYDOWN, vk, MAKELPARAM(cRepeat, flags));
		break;
	}

	return;
}
/*********************************************************************\
* Function	CBEdit_OnCommand
* Purpose   WM_COMMAND message handler of the main window
* Params
*			hWnd:	Handle of the window
*			id:		Id of command message
*			hwndCtl: Handle of the window which sended this message
*			codeNotify:Notify code of the message
* Return	None
* Remarks
**********************************************************************/
static void CBEdit_OnCommand(HWND hWnd, int id, UINT codeNotify)
{
	switch(id)
	{        
    default:
        break;
	}

	return;
}
/*********************************************************************\
* Function	CBEdit_OnDestroy
* Purpose   WM_DESTROY message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void CBEdit_OnDestroy(HWND hWnd)
{

	UnregisterClass("CBEditWndClass", NULL);
    
    return;

}
/*********************************************************************\
* Function	CBEdit_OnClose
* Purpose   WM_CLOSE message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void CBEdit_OnClose(HWND hWnd)
{
    SendMessage(GetParent(hWnd),PWM_CLOSEWINDOW,(WPARAM)hWnd,NULL);

    DestroyWindow (hWnd);
	
    return;

}
/*********************************************************************\
* Function	CB_ParallelInfo
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
static void CB_ParallelInfo(HWND hWnd)
{    
    CB_DATA *pData;
    char szFileName[7];
    int f;
    char szOldPath[PATH_MAXLEN];
    int i;

    pData = GetUserData(hWnd);

    getcwd(szOldPath,PATH_MAXLEN);  

    chdir(PATH_DIR_SMS);  

    for(i = 0 , pData->nCBCount = 0; i < CB_MAX ; i++)
    {
        if(pData->nCB[i] != -1)
        {
            szFileName[0] = 0;
            
            sprintf(szFileName,"%d.cb",pData->nCB[i]);
            
            f = open(szFileName,O_RDONLY);
            
            if(f == -1)
                pData->bParallelInfo[pData->nCBCount] = FALSE;
            else
            {
                close(f);
                
                pData->bParallelInfo[pData->nCBCount] = TRUE;
            }

            pData->nCBCount++;
        }
    }
    
    chdir(szOldPath);  
}

/*********************************************************************\
* Function	CB_GetInfo
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
static void CB_GetInfo(HWND hWnd,int index,BOOL *pPer,BOOL *pNext)
{    
    int i;
    
    CB_DATA *pData;
    
    pData = GetUserData(hWnd);

    *pPer = FALSE;

    *pNext = FALSE;

    for(i = index+1 ; i < pData->nCBCount ; i++)
    {
        if(pData->bParallelInfo[i] == TRUE)
        {
            *pNext = TRUE;

            break;
        }
    }

    for(i = index-1 ; i >= 0 ; i--)
    {
        if(pData->bParallelInfo[i] == TRUE)
        {
            *pPer = TRUE;

            break;
        }
    }
}
/*********************************************************************\
* Function	CB_GetNextText
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
static int CB_GetNextText(HWND hWnd,int index,BOOL bDirtection)
{
    int i;

    CB_DATA *pData;
    
    pData = GetUserData(hWnd);

    if(bDirtection == TRUE)
    {        
        for(i = index+1 ; i < pData->nCBCount ; i++)
        {
            if(pData->bParallelInfo[i] == TRUE)
            {
                return i;
            }
        }
    }
    else
    {
        for(i = index-1 ; i >= 0 ; i--)
        {
            if(pData->bParallelInfo[i] == TRUE)
            {
                return i;
            }
        }
    }

    return -1;
}
/*********************************************************************\
* Function	CB_IsExist
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
static BOOL CB_IsExist(HWND hWnd,char* pszNum,int index)
{
    short ID;

    HWND hCBListWnd;

    CB_DATA *pData;
    
    hCBListWnd = GetDlgItem(GetParent(hWnd),IDC_CB_MAIN);

    if(hCBListWnd == NULL)
        return FALSE;

    pData = GetUserData(hCBListWnd);

    ID = atoi(pszNum);
    
    if(pData->nCB[ID] == ID && ID != index)
    {
        return TRUE;
    }

    return FALSE;
}
/*********************************************************************\
* Function	CB_IsSubscribe
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
static BOOL CB_IsSubscribe(HWND hWnd,int nCB)
{
    int  i;
    CB_DATA *pData;
    
    pData = GetUserData(hWnd);

    for(i = 0 ; i < CB_SUBSCRIBE_MAX ; i++)
    {
        if(pData->nSubscribeCB[i] != -1)
        {
            if(pData->nSubscribeCB[i] == nCB)
                return TRUE;
        }
    }

    return FALSE;
}
/*********************************************************************\
* Function	CB_DeleteMessage
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
static BOOL CB_DeleteMessage(int nCB)
{
    char szOldPath[PATH_MAXLEN];
    char szFileName[7];
    
    szOldPath[0] = 0;
    
    getcwd(szOldPath,PATH_MAXLEN);

    chdir(PATH_DIR_SMS);

    szFileName[0] = 0;

    sprintf(szFileName,"%d.cb",nCB);
    
    remove(szFileName);
    
    chdir(szOldPath);

    return TRUE;
}
/*********************************************************************\
* Function	CB_Subscribe
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
static BOOL CB_Subscribe(HWND hWnd,int nCB,BOOL bSubscribe)
{
    int  i;
    CELL_BROADCAST CB_Setting;
    char szSub[4];
    CB_DATA *pData;
    int nTopicDetection = 0;
    BOOL bGet = FALSE;
    
    pData = GetUserData(hWnd);

    memset(&CB_Setting,0,sizeof(CELL_BROADCAST));

    CB_ReadSetting(&CB_Setting,&nTopicDetection);

    szSub[0] = 0;

    itoa(nCB,szSub,10);

    if(bSubscribe == FALSE)
    {
        for(i = 0 ; i < CB_SUBSCRIBE_MAX ; i++)
        {
            if(pData->nSubscribeCB[i] == nCB)
            {
                pData->nSubscribeCB[i] = -1;

                break;
            }
        }
    }
    else
    {
        for(i = 0 ; i < CB_SUBSCRIBE_MAX ; i++)
        {
            if(pData->nSubscribeCB[i] == -1)
            {
                pData->nSubscribeCB[i] = nCB;

                bGet = TRUE;

                break;
            }
        }

        if(bGet == FALSE)
        {
            PLXTipsWin(NULL, NULL, 0, IDS_NOMORETOPICALLOW, IDS_CB,
                Notify_Alert, IDS_OK, NULL, WAITTIMEOUT);

            return FALSE; 
        }
    }

    CB_GetStringFromID(CB_Setting.ids,pData->nSubscribeCB);
    
    if(nTopicDetection == 0)
        CB_Setting.ids[0] = 0;

    CB_WriteSetting(&CB_Setting,-1);//?????

    CB_Set(&CB_Setting);
    
    return TRUE;
}
/*********************************************************************\
* Function	CB_GetSubscribeID
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
static BOOL CB_GetSubscribeID(char* pIDs,int * pnID)
{
    char* p;
    char  szTemp[4];
    int  nLen;
    int  *pCurID;

    p = pIDs;

    pCurID = pnID;

    nLen = 0;

    szTemp[0] = 0;

    while(*p != '\0')
    {
        if(*p != ',')
        {
            if(nLen >= 3)
                return FALSE;
            
            szTemp[nLen] = *p;

            nLen++;
        }
        else
        {
            szTemp[nLen] = '\0';

            *pCurID = atoi(szTemp);

            pCurID++;

            szTemp[0] = 0;

            nLen = 0;
        }
        p++;
    }

    return TRUE;
}
/*********************************************************************\
* Function	CB_GetStringFromID
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
static BOOL CB_GetStringFromID(char* pszIDs,int * pnID)
{
    int i;
    char szTemp[4];

    pszIDs[0] = 0;

    for(i = 0 ; i < CB_SUBSCRIBE_MAX ; i++)
    {
        if(pnID[i] != -1)
        {
            szTemp[0] = 0;

            itoa(pnID[i],szTemp,10);
            
            strcat(pszIDs,szTemp);

            if(i < CB_SUBSCRIBE_MAX-1)
                strcat(pszIDs,",");
        }
    }

    return TRUE;
}
/*********************************************************************\
* Function	CB_WriteMessage
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
BOOL CB_WriteMessage(CBS_INFO *pCBInfo)
{
    char szOldPath[PATH_MAXLEN];
    char szFileName[7];
    int  f;
    BOOL bRead = FALSE;
    CELL_BROADCAST CB_Setting;
    int  nTopicDetection = 0;
    int  i;
    BOOL bGet = FALSE;
    int nSubscribeCB[CB_SUBSCRIBE_MAX];

    memset(&CB_Setting,0,sizeof(CELL_BROADCAST));
    
    memset(nSubscribeCB,0,sizeof(int)*CB_SUBSCRIBE_MAX);

    CB_ReadSetting(&CB_Setting,&nTopicDetection);
    
    CB_GetSubscribeID(CB_Setting.ids,nSubscribeCB);
    
    for(i = 0 ; i < CB_SUBSCRIBE_MAX ; i++)
    {
        if(nSubscribeCB[i] == pCBInfo->MId)
        {
            bGet = TRUE;
            break;
        }
    }

    if(bGet == FALSE)
        return FALSE;

    szOldPath[0] = 0;
    
    getcwd(szOldPath,PATH_MAXLEN);

    chdir(PATH_DIR_SMS);

    szFileName[0] = 0;

    sprintf(szFileName,"%d.cb",pCBInfo->MId);
    
    f = open(szFileName,O_RDWR | O_CREAT, S_IRWXU);
    
    if( f == -1 )
    {
        chdir(szOldPath);

        return FALSE;
    }

#ifdef _SMS_DEBUG_
    printf("\r\n*****SMS Debug Info*****  CB_WriteMessage file handle = %d \r\n",f);
#endif

    write(f,&bRead,sizeof(int));
        
    write(f,pCBInfo,sizeof(CBS_INFO));
    
    close(f);
        
    chdir(szOldPath);

    return TRUE;
}
/*********************************************************************\
* Function	SMS_GetCBCounter
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
int SMS_GetCBCounter(void)
{
    return nCBCounter;
}
/*********************************************************************\
* Function	SMS_SetCBCounter
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
void SMS_SetCBCounter(int nNewCBCounter)
{
    nCBCounter = nNewCBCounter;
}
/*********************************************************************\
* Function	SMS_InitCBCounter
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
void SMS_InitCBCounter(void)
{
    int nCBCounter;
    int i;
    int nCB[CB_MAX];
    
    memset(nCB,0,sizeof(int)*CB_MAX);

    CB_Read(nCB);

    for(i = 0,nCBCounter = 0 ; i < CB_MAX ; i++)
    {
        if(nCB[i] != -1)
        {
            nCBCounter++;
        }
    }

    SMS_SetCBCounter(nCBCounter);
}
/*********************************************************************\
* Function	CB_Refresh
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
BOOL CB_Refresh(CBS_INFO *pCBInfo)
{
    HWND hMessagingFrameWnd = NULL;
    HWND hCBListWnd = NULL;
    HWND hCBViewWnd = NULL;

    hMessagingFrameWnd = MuGetFrame();

    if(IsWindow(hMessagingFrameWnd))
    {    
        hCBListWnd = GetDlgItem(MuGetFrame(),IDC_CB_MAIN);
        
        if(IsWindow(hCBListWnd))
        {   
            HWND hLst;
            CB_DATA *pData;
            int i,nCount,nCurLine;
            int *pnCB;
            
            pData = GetUserData(hCBListWnd);
            
            hLst = GetDlgItem(hCBListWnd,IDC_CB_LIST);

            nCount = SendMessage(hLst,LB_GETCOUNT,0,0);

            nCurLine = SendMessage(hLst,LB_GETCURSEL,0,0);

            for(i = 0 ; i < nCount ; i++)
            {
                pnCB = (int*)SendMessage(hLst,LB_GETITEMDATA,i,NULL);

                if(*pnCB == pCBInfo->MId)
                {
                    SendMessage(hLst,LB_SETIMAGE,MAKEWPARAM(IMAGE_BITMAP,i),(LPARAM)pData->hTopic);
                    
                    CB_SetLeftSoftKey(hCBListWnd);

                    break;
                }
            }
        }

        hCBViewWnd = GetDlgItem(MuGetFrame(),IDC_CB_VIEW);

        if(IsWindow(hCBViewWnd))
        {
            CB_VIEW *pData;
           
            pData = (CB_VIEW*)GetUserData(hCBViewWnd);
            
            if(pData->ID == pCBInfo->MId)
            {
                SendMessage(hCBViewWnd,WM_CLOSE,0,0);
                CB_OnParalleView(hCBListWnd,WM_CBMSG_REFRESH);
            }
        }
    }

    return TRUE;
}
/*********************************************************************\
* Function	CB_IsExistMsg
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
static BOOL CB_IsExistMsg(int nCB)
{
    char szOldPath[PATH_MAXLEN];
    char szFileName[7];
    int  f;
    
    szOldPath[0] = 0;
    
    getcwd(szOldPath,PATH_MAXLEN);

    chdir(PATH_DIR_SMS);

    szFileName[0] = 0;

    sprintf(szFileName,"%d.cb",nCB);
    
    f = open(szFileName,O_RDONLY);

    if(f == -1)
    {
        chdir(szOldPath);
        
        return FALSE;
    }
    else
    {
        close(f);
        
        chdir(szOldPath);
        
        return TRUE;
    }
}
/*********************************************************************\
* Function	APP_ShowCellInfo
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
BOOL APP_ShowCellInfo(BOOL bShow)
{
    char szOldPath[PATH_MAXLEN];
    int  f;
    
    szOldPath[0] = 0;
    
    getcwd(szOldPath,PATH_MAXLEN);

    chdir(PATH_DIR_SMS);
    
    f = open(SMS_FILENAME_CELLINFO,O_RDONLY | O_CREAT,S_IRWXU);

    if(f == -1)
    {
        chdir(szOldPath);
        
        return FALSE;
    }
    else
    {
        write(f,&bShow,sizeof(BOOL));

        close(f);
        
        chdir(szOldPath);
        
        return TRUE;
    }

    return FALSE;
}

/*********************************************************************\
* Function	Get_ShowCellInfo
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
BOOL Get_ShowCellInfo(void)
{
    char szOldPath[PATH_MAXLEN];
    int  f;
    BOOL bShow = FALSE;
    
    szOldPath[0] = 0;
    
    getcwd(szOldPath,PATH_MAXLEN);

    chdir(PATH_DIR_SMS);
    
    f = open(SMS_FILENAME_CELLINFO,O_RDONLY);

    if(f == -1)
    {
        chdir(szOldPath);
        
        return bShow;
    }
    else
    {
        write(f,&bShow,sizeof(BOOL));

        close(f);
        
        chdir(szOldPath);
        
        return bShow;
    }
}
/*********************************************************************\
* Function	CB_SetLeftSoftKey
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
static void CB_SetLeftSoftKey(HWND hWnd)
{
    HWND hLst;
    int  *pnCB;
    int  nCur;
    CB_DATA *pData;

    pData = GetUserData(hWnd);

    hLst = GetDlgItem(hWnd,IDC_CB_LIST);

    nCur = SendMessage(hLst,LB_GETCURSEL,0,0);

    if(nCur != 0)
    {
        pnCB = (int*)SendMessage(hLst,LB_GETITEMDATA,nCur,0);
     
        if(CB_IsExistMsg(*pnCB))
            SendMessage(pData->hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_OPEN);
        else
            SendMessage(pData->hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
        SendMessage(pData->hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_OPTIONS);
    }
    else
    {
        SendMessage(pData->hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
        SendMessage(pData->hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_SELECT);
    }
}
