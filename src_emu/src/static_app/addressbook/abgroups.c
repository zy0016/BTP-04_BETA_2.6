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
#include "ABGlobal.h"

#define IDC_OPEN                100
#define IDM_ABGROUPS_RENAME     101
#define IDM_ABGROUPS_DELETE     102
#define IDM_ABGROUPS_SETTONE    103

#define IDC_BACK                200
#define IDC_ABGROUPS_LIST       300

// #define AB_MAXLEN_GROUPNAME     51

static DWORD idGroup;

//typedef struct tagGroup_Info
//{
//    int    nDel;
//    int    nGroupID;
//    char   szGroupName[AB_MAXLEN_GROUPNAME];
//}GROUP_INFO,*PGROUP_INFO;

typedef struct tagGroup_Node
{
    GROUP_INFO           Info;
    DWORD                dwOffset;
    struct tagGroup_Node *pPioneer;
    struct tagGroup_Node *pNext;
}GROUP_NODE,*PGROUP_NODE;

typedef struct tagGroupNew_Data
{
    HWND        hFrameWnd;
    GROUP_NODE* pGroup;
    BOOL        bNew;
}GROUPNEW_DATA,*PGROUPNEW_DATA;


typedef struct tagGroupTone_Data
{
    int     idGroup;
    DWORD   dwoffset;
}GROUPTONE_DATA,*PGROUPTONE_DATA;

typedef struct tagGroupView_Data
{
    GROUP_NODE* pGroup;
    PVOID       handleGroup;
    PVOID       handleContact;
    int         nCount;
    DWORD*      pId;
    HWND        hFrameWnd;
    HWND        hWnd;
    HMENU       hMenu;
    HBITMAP     hAddMember;
    HBITMAP     hMember;
    WNDPROC     OldListWndProc;
}GROUPVIEW_DATA,*PGROUPVIEW_DATA;

static GROUP_NODE* pHeader = NULL;
static int nCount;

BOOL AB_CreateGroupsWindow(HWND hWnd);
LRESULT ABGroupsWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);

static BOOL ABGroups_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct);
static void ABGroups_OnActivate(HWND hwnd, UINT state);
static void ABGroups_OnPaint(HWND hWnd);
static void ABGroups_OnSetFocus(HWND hWnd);
static void ABGroups_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags);
static void ABGroups_OnCommand(HWND hWnd, int id, UINT codeNotify);
static void ABGroups_OnDestroy(HWND hWnd);
static void ABGroups_OnClose(HWND hWnd);
static void ABGroups_OnDataChange(HWND hWnd,GROUP_NODE *pNode,int nMode);
static void ABGroups_OnGroupDeleted(HWND hWnd,BOOL bDelete);
static void ABGroups_OnSetTone(HWND hWnd,BOOL bSelect,UINT nLen,char* szTonePath);

static GROUP_NODE* Group_New(void);
static int Group_Insert(GROUP_NODE* pChainNode);
static void Group_Delete(GROUP_NODE* pChainNode);
static void Group_DeleteFromChain(GROUP_NODE* pChainNode);
static void Group_Erase(void);

static int AB_FindUseableID(void);
static BOOL AB_IsGroupNameUsed(char* pszGroupName);
static BOOL AB_IsGroupIDUsed(int ID);
static BOOL AB_FindUseableName(char* szGroupName);

static BOOL AB_ClearUp(void);
static BOOL AB_ReadGroup(GROUP_NODE *pNode,int* pnCount);
static BOOL AB_SaveGroupInfo(GROUP_NODE *pNode,BOOL bNew);
static BOOL AB_DeleteGroup(GROUP_NODE* pNode);


#define IDC_ABGROUPSNEW_EDIT    300
#define IDC_SAVE                100

BOOL AB_CreateGroupsNewWnd(HWND hWnd,GROUP_NODE* pGroup,BOOL bNew);
LRESULT ABGroupsNewWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);

static BOOL ABGroupsNew_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct);
static void ABGroupsNew_OnActivate(HWND hwnd, UINT state);
static void ABGroupsNew_OnSetFocus(HWND hWnd);
static void ABGroupsNew_OnPaint(HWND hWnd);
static void ABGroupsNew_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags);
static void ABGroupsNew_OnCommand(HWND hWnd, int id, UINT codeNotify);
static void ABGroupsNew_OnDestroy(HWND hWnd);
static void ABGroupsNew_OnClose(HWND hWnd);




#define IDC_OPEN                        100
#define IDC_BACK                        200
#define IDC_ABGROUPSVIEW_LIST           300
#define IDM_ABGROUPSVIEW_RENAME         101
#define IDM_ABGROUPSVIEW_SETTONE        102
#define IDM_ABGROUPSVIEW_REMOVEMEMBER   103
#define IDM_AB_REMOVESELECT             1041
#define IDM_AB_REMOVEALL                1042

BOOL AB_CreateGroupsViewWnd(HWND hFrameWnd,HWND hWnd,GROUP_NODE* pGroup);
LRESULT ABGroupsViewWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);

static BOOL ABGroupsView_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct);
static void ABGroupsView_OnActivate(HWND hwnd, UINT state);
static void ABGroupsView_OnPaint(HWND hWnd);
static void ABGroupsView_OnSetFocus(HWND hWnd);
static void ABGroupsView_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags);
static void ABGroupsView_OnCommand(HWND hWnd, int id, UINT codeNotify);
static void ABGroupsView_OnDestroy(HWND hWnd);
static void ABGroupsView_OnClose(HWND hWnd);
static void ABGroupsView_OnDataChange(HWND hWnd,GROUP_NODE* pGroup,int nMode);
static void ABGroupsView_OnDataChangeContact(HWND hWnd,int nPos,int nMode);
static void ABGroupsView_OnSelect(HWND hWnd,BOOL bSelect,int nCount,DWORD* pnId);
static void ABGroupsView_OnSetTone(HWND hWnd,BOOL bSelect,UINT nLen,char* pszTonePath);
static void ABGroupsView_OnSureAddMember(HWND hWnd,BOOL bAdd);
static void ABGroupsView_OnRemoveMember(HWND hWnd,BOOL bRemove);
static void ABGroupsView_OnRemoveAll(HWND hWnd,BOOL bRemoveAll);
static void ABGroupsView_OnRemoveSel(HWND hWnd,BOOL bSelect,int nCount,DWORD* pnId);
static void ABGroupsView_OnSureRemoveSel(HWND hWnd,BOOL bSure);
static void ABGroupsView_OnRefreshList(HWND hWnd,char* pszSearch,int nLen,BOOL bExit);

static LRESULT CallListWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);


extern BOOL AB_CreateSearchPopUpWnd(HWND hFrameWnd,HWND hMsgWnd,UINT wMsgCmd,BOOL bDealArrow);


DWORD AB_GetUnusedGroupID(void);
void AB_SetGroupID(DWORD id);
DWORD AB_GetGroupID(void);
void AB_LoadGroupID(void);
BOOL AB_SaveGroupID(void);
static BOOL AB_GetGroupToneInfo(GROUPTONE_DATA* pGroupTone,int *nToneCount);
static BOOL AB_WriteGroupTone(int ID,int nLen,char* pszTonePath,DWORD* pdwoffset);
static BOOL AB_DelGroupTone(DWORD dwoffset);
void AB_DeleteGroupTone(HWND hWnd,int ID);
void AB_ReWriteGroupTone(HWND hWnd,int ID,int nLen,char* pszTonePath);

typedef struct tagAB_GROUPDATA
{
    PVOID pPointer;
    HWND  hFrameWnd;
    HMENU hMenu;
    int   nSetToneCount;
    HBITMAP hNewGroup;
    HBITMAP hGroupItem;
    GROUPTONE_DATA* pGroupTone;
}AB_GROUPDATA,*PAB_GROUPDATA;

BOOL AB_CreateGroupsWindow(HWND hFrameWnd)
{
    WNDCLASS    wc;
    HWND        hGroupsWnd;
    RECT        rcClient;
    AB_GROUPDATA Data;
    
    memset(&Data,0,sizeof(AB_GROUPDATA));

    Data.hFrameWnd = hFrameWnd;

    wc.style         = 0;
    wc.lpfnWndProc   = ABGroupsWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = sizeof(AB_GROUPDATA);
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName  = "ABGroupsWndClass";
    
    if (!RegisterClass(&wc))
        return FALSE;
         
    GetClientRect(hFrameWnd,&rcClient);

    Data.hMenu = CreateMenu();

    hGroupsWnd = CreateWindow(
        "ABGroupsWndClass",
        "", 
        WS_VISIBLE | WS_CHILD, 
        rcClient.left,
        rcClient.top,
        rcClient.right - rcClient.left,
        rcClient.bottom - rcClient.top,
        hFrameWnd,
        NULL, 
        NULL, 
        (PVOID)&Data
        );
    
    if (!hGroupsWnd)
    {
		DestroyMenu(Data.hMenu);

        UnregisterClass("ABGroupsWndClass",NULL);
        return FALSE;
    }

    AppendMenu(Data.hMenu, MF_ENABLED, IDC_OPEN, IDS_OPEN);
    AppendMenu(Data.hMenu, MF_ENABLED, IDM_ABGROUPS_RENAME, IDS_RENAMEGROUP);
    AppendMenu(Data.hMenu, MF_ENABLED, IDM_ABGROUPS_SETTONE, IDS_SETTONE);
	AppendMenu(Data.hMenu, MF_ENABLED, IDM_ABGROUPS_DELETE, IDS_DELETEGROUP);

    PDASetMenu(hFrameWnd,Data.hMenu);

    SetFocus(hGroupsWnd);

    SetWindowText(hFrameWnd,IDS_GROUPS);

    SendMessage(hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_BACK, 0), (LPARAM)IDS_BACK);
    SendMessage(hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_OPEN, 1), (LPARAM)"");
    SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_SELECT);
    
    SendMessage(hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON),(LPARAM)NULL);
    SendMessage(hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON),(LPARAM)NULL);

    ShowWindow(hGroupsWnd,SW_SHOW);
    UpdateWindow(hGroupsWnd);

    
    return TRUE;
}

LRESULT ABGroupsWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    LRESULT lResult = TRUE;

	switch (wMsgCmd)
    {
    case WM_CREATE:
        lResult = ABGroups_OnCreate(hWnd,(LPCREATESTRUCT)(lParam));
        break;

    case WM_ACTIVATE:
    case PWM_SHOWWINDOW:
        ABGroups_OnActivate(hWnd,(UINT)LOWORD(wParam));
        break;

    case WM_SETFOCUS:
        ABGroups_OnSetFocus(hWnd);
        break;

    case WM_PAINT:
        ABGroups_OnPaint(hWnd);
        break;

    case WM_KEYDOWN:
        ABGroups_OnKey(hWnd,(UINT)(wParam),(int)(short)LOWORD(lParam),(UINT)HIWORD(lParam));
        break;

    case WM_COMMAND:
        ABGroups_OnCommand(hWnd,(int)(LOWORD(wParam)),(UINT)HIWORD(wParam));
        break;
        
    case WM_CLOSE:
        ABGroups_OnClose(hWnd);
        break;

    case WM_DESTROY:
        ABGroups_OnDestroy(hWnd);
        break;

    case WM_DATACHANGE:
        ABGroups_OnDataChange(hWnd,(GROUP_NODE*)wParam,(int)lParam);
        break;

    case WM_GROUPDELETED:
        ABGroups_OnGroupDeleted(hWnd,(BOOL)lParam);
        break;
        
    case WM_SETTONE:
        ABGroups_OnSetTone(hWnd,(BOOL)(HIWORD(wParam)),(UINT)LOWORD(wParam),(char*)lParam);
        break;

    default:     
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
	}

    return lResult;

}
/*********************************************************************\
* Function	ABGroups_OnCreate
* Purpose   WM_CREATE message handler of the main window
* Params
*			hWnd: Handle of the window
*			lpCreateStruct: Create Structure
* Return
*			TRUE: Success
*			FALSE: Fail
* Remarks
**********************************************************************/
static BOOL ABGroups_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
{    
    RECT rect;
    HWND hList;
    GROUP_NODE *pNewNode;
    int  i,index;
    AB_GROUPDATA* pData;

    pData = (AB_GROUPDATA*)GetUserData(hWnd);

    memcpy(pData,lpCreateStruct->lpCreateParams,sizeof(AB_GROUPDATA));
        
    GetClientRect(hWnd,&rect);
    
    hList = CreateWindow(
        "LISTBOX",
        "",
        WS_VISIBLE | WS_CHILD | WS_VSCROLL | LBS_BITMAP,
        rect.left,
        rect.top,
        rect.right - rect.left,
        rect.bottom - rect.top,
        hWnd,
        (HMENU)IDC_ABGROUPS_LIST,
        NULL,
        NULL);
   
    if(hList == NULL)
        return FALSE;
    
    pData->hNewGroup = LoadImage(NULL, AB_BMP_NEWGROUP, IMAGE_BITMAP,
		ICON_WIDTH, ICON_HEIGHT, LR_LOADFROMFILE);
    
    pData->hGroupItem = LoadImage(NULL, AB_BMP_GROUPITEM, IMAGE_BITMAP,
		ICON_WIDTH, ICON_HEIGHT, LR_LOADFROMFILE);

    nCount = 0;

    SendMessage(hList,LB_ADDSTRING,-1,(LPARAM)IDS_NEWGROUP);
    SendMessage(hList,LB_SETIMAGE,MAKEWPARAM(IMAGE_BITMAP, 0),(LPARAM)pData->hNewGroup);

    if(AB_ReadGroup(NULL,&nCount))
    {
        for(i = 0 ; i < nCount ; i++)
        {
            pNewNode = Group_New();
            if(pNewNode == NULL)
            {
                Group_Erase();
                return FALSE;
            }
            
            AB_ReadGroup(pNewNode,&i);
            
            index = Group_Insert(pNewNode);
            
            SendMessage(hList,LB_INSERTSTRING,index+1,(LPARAM)(pNewNode->Info.szGroupName));
            SendMessage(hList,LB_SETITEMDATA,index+1,(LPARAM)pNewNode);
            SendMessage(hList,LB_SETIMAGE,MAKEWPARAM(IMAGE_BITMAP, index+1),(LPARAM)pData->hGroupItem);
        }
        
        SendMessage(hList,LB_SETCURSEL,0,0);        
    }
    else
        return FALSE;

    if(AB_GetGroupToneInfo(NULL,&(pData->nSetToneCount)))
    {
        if(pData->nSetToneCount > 0)
        {
            pData->pGroupTone = (GROUPTONE_DATA*)malloc(sizeof(GROUPTONE_DATA) * pData->nSetToneCount);

            if(pData->pGroupTone == NULL)
                return FALSE;

            AB_GetGroupToneInfo(pData->pGroupTone,&(pData->nSetToneCount));
        }
    }
    else 
        return FALSE;

    pData->pPointer = AB_RegisterNotify(hWnd,WM_DATACHANGE,AB_OBJECT_ALL,AB_MDU_GROUP);
        
    return TRUE;
    
}
/*********************************************************************\
* Function	ABGroups_OnActivate
* Purpose   WM_ACTIVATE message handler of the main window
* Params
* Return    None
* Remarks
**********************************************************************/
static void ABGroups_OnActivate(HWND hwnd, UINT state)
{
    HWND hLst;

    AB_GROUPDATA* pData;

    int nIndex;

    pData = (AB_GROUPDATA*)GetUserData(hwnd);

    hLst = GetDlgItem(hwnd,IDC_ABGROUPS_LIST);

    SetFocus(hLst);

    PDASetMenu(pData->hFrameWnd,pData->hMenu);

    SetWindowText(pData->hFrameWnd,IDS_GROUPS);

    nIndex = SendMessage(hLst,LB_GETCURSEL,0,0);
    
    if(nIndex == 0)
    {
        SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,1, (LPARAM)"");
        SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,2, (LPARAM)ICON_SELECT);
    }
    else      
    {
        SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,1, (LPARAM)IDS_OPEN);
        SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,2, (LPARAM)ICON_OPTIONS);
    }

    SendMessage(pData->hFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_BACK);
    
    SendMessage(pData->hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON),(LPARAM)NULL);
    SendMessage(pData->hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON),(LPARAM)NULL);

    return;
}

/*********************************************************************\
* Function	ABGroups_OnSetFocus
* Purpose   WM_SETFOCUS message handler of the main window
* Params	hWnd: Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void ABGroups_OnSetFocus(HWND hWnd)
{
    
    HWND hLst;

    hLst = GetDlgItem(hWnd,IDC_ABGROUPS_LIST);

    SetFocus(hLst);

}
/*********************************************************************\
* Function	ABGroups_OnPaint
* Purpose   WM_PAINT message handler of the main window
* Params	hWnd: Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void ABGroups_OnPaint(HWND hWnd)
{
	HDC hdc = BeginPaint(hWnd, NULL);

	EndPaint(hWnd, NULL);

	return;
}

/*********************************************************************\
* Function	ABGroups_OnKey
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
static void ABGroups_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags)
{
    HWND hLst;
    int  nIndex;
    GROUP_NODE *pGroup;
    AB_GROUPDATA* pData;

    pData = (AB_GROUPDATA*)GetUserData(hWnd);

    switch (vk)
	{
    case VK_F5:
        hLst = GetDlgItem(hWnd,IDC_ABGROUPS_LIST);
        nIndex = SendMessage(hLst,LB_GETCURSEL,0,0);
        
        switch(nIndex)
        {
        case LB_ERR:
            break;
            
        case 0:// new group
            if(AB_IsFlashEnough() == FALSE)
            {
                PLXTipsWin(NULL,NULL,0,IDS_MEMORYFULL,IDS_CONTACTS,Notify_Failure,IDS_OK,NULL,WAITTIMEOUT);
                break;
            }

            pGroup = Group_New();
            pGroup->Info.nDel = 1;
            pGroup->Info.nGroupID = -1;
            pGroup->Info.szGroupName[0] = 0;
            //AB_FindUseableName(pGroup->Info.szGroupName);
            
            AB_CreateGroupsNewWnd(pData->hFrameWnd,pGroup,TRUE);
            
            free(pGroup);

            break;
            
        default:
            PDADefWindowProc(pData->hFrameWnd, WM_KEYDOWN, vk, MAKELPARAM(cRepeat, flags));
            break;
        }       
        break;

    case VK_RETURN:
        SendMessage(hWnd,WM_COMMAND,IDC_OPEN,NULL);
        break;

	case VK_F10:
		PostMessage(hWnd,WM_CLOSE,NULL,NULL);
		break;

	default:
		PDADefWindowProc(hWnd, WM_KEYDOWN, vk, MAKELPARAM(cRepeat, flags));
		break;
	}

	return;
}
/*********************************************************************\
* Function	ABGroups_OnCommand
* Purpose   WM_COMMAND message handler of the main window
* Params
*			hWnd:	Handle of the window
*			id:		Id of command message
*			hwndCtl: Handle of the window which sended this message
*			codeNotify:Notify code of the message
* Return	None
* Remarks
**********************************************************************/
static void ABGroups_OnCommand(HWND hWnd, int id, UINT codeNotify)
{
    HWND hLst;
    int  nIndex;
    char szButText[20];
    char szMenu[20];
    AB_GROUPDATA* pData;

    pData = (AB_GROUPDATA*)GetUserData(hWnd);

    hLst = GetDlgItem(hWnd,IDC_ABGROUPS_LIST);
	
    switch(id)
	{
    case IDC_BACK:
		PostMessage(hWnd,WM_CLOSE,NULL,NULL);
        break;

    case IDC_ABGROUPS_LIST:
        if(codeNotify == LBN_SELCHANGE)
        {
            nIndex = SendMessage(hLst,LB_GETCURSEL,0,0);
            if(nIndex == LB_ERR)
                break;
            
            memset(szButText,0,20);
            memset(szMenu,0,20);

            SendMessage(pData->hFrameWnd, PWM_GETBUTTONTEXT, 1, (LPARAM)szButText);
            SendMessage(pData->hFrameWnd, PWM_GETBUTTONTEXT, 2, (LPARAM)szMenu);

            if(nIndex == 0)
            {  
                if(strcmp(szButText,IDS_OPEN) == 0)
                    SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,1, (LPARAM)"");
                
                if(strcmp(szMenu,ICON_SELECT) != 0)
                    SendMessage(pData->hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_SELECT);
            }
            else
            {
                if(strcmp(szButText,IDS_OPEN) != 0)
                    SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,1, (LPARAM)IDS_OPEN);
                
                if(strcmp(szMenu,ICON_OPTIONS) != 0)
                    SendMessage(pData->hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_OPTIONS);
            }
        }
        break;

    case IDC_OPEN:
        {   
            GROUP_NODE *pGroup;
            
            nIndex = SendMessage(hLst,LB_GETCURSEL,0,0);
            if(nIndex == LB_ERR || nIndex == 0)
                break;

            pGroup = (GROUP_NODE*)SendMessage(hLst,LB_GETITEMDATA,nIndex,0);
            
            AB_CreateGroupsViewWnd(pData->hFrameWnd,hWnd,pGroup);
        }
        break;

    case IDM_ABGROUPS_RENAME:
        {   
            GROUP_NODE *pGroup;
            
            nIndex = SendMessage(hLst,LB_GETCURSEL,0,0);
            if(nIndex == LB_ERR)
                break;

            pGroup = (GROUP_NODE*)SendMessage(hLst,LB_GETITEMDATA,nIndex,0);
            
            AB_CreateGroupsNewWnd(pData->hFrameWnd,pGroup,FALSE);
        }
        break;
        
    case IDM_ABGROUPS_DELETE:
        {
            GROUP_NODE *pGroup;
            char szPrompt[100];
            char szCaption[AB_MAXLEN_GROUPNAME];
            HDC hdc;
         
            nIndex = SendMessage(hLst,LB_GETCURSEL,0,0);
            
            if(nIndex == LB_ERR)
                break;

            pGroup = (GROUP_NODE*)SendMessage(hLst,LB_GETITEMDATA,nIndex,0);
            
            szCaption[0] = 0;
            
            hdc = GetDC(hWnd);
            
            GetExtentFittedText(hdc,pGroup->Info.szGroupName,-1,szCaption,AB_MAXLEN_GROUPNAME,TEXT_WIDTH,OMIT_SUFFIX,SUFFIX_REPEAT);
            
            ReleaseDC(hWnd,hdc);

            szPrompt[0] = 0;

            sprintf(szPrompt,"%s:\r\n%s?",szCaption,IDS_DELGROUP);
                        
            PLXConfirmWinEx(pData->hFrameWnd,hWnd,szPrompt, Notify_Request, pGroup->Info.szGroupName, IDS_YES, IDS_NO,WM_GROUPDELETED);    
            
        }
        break;

    case IDM_ABGROUPS_SETTONE:
        //APP_PreviewRing(pData->hFrameWnd,hWnd,WM_SETTONE,(PSTR)IDS_GROUPTONE);
		{
			HWND hRing;
			char szRing[AB_MAXLEN_RING];
			HWND hList;
			int nIndex;
			GROUP_NODE* pGroup;
			
			hList = GetDlgItem(hWnd,IDC_ABGROUPS_LIST);
			
			nIndex = SendMessage(hList,LB_GETCURSEL,0,0);
			if(nIndex == LB_ERR)
				break;
			
			pGroup = (GROUP_NODE*)SendMessage(hList,LB_GETITEMDATA,nIndex,0);

			AB_GetGroupToneByID(pGroup->Info.nGroupID, szRing, AB_MAXLEN_RING);
			
			hRing = PreviewSoundEx(pData->hFrameWnd, hWnd, WM_SETTONE, (PSTR)IDS_GROUPTONE, TRUE);
			if(hRing)
			{
				SendMessage(hRing, SPM_SETDEFTONE, 0, 0);
				SendMessage(hRing, SPM_SETCURSEL, 0, (LPARAM)szRing);
			}

		}
        break;

    default:
        break;
    }
}
/*********************************************************************\
* Function	ABGroups_OnDestroy
* Purpose   WM_DESTROY message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void ABGroups_OnDestroy(HWND hWnd)
{
    AB_GROUPDATA* pData;

    pData = (AB_GROUPDATA*)GetUserData(hWnd);

    if(pData->hNewGroup)
        DeleteObject(pData->hNewGroup);

    if(pData->hGroupItem)
        DeleteObject(pData->hGroupItem);

    DestroyMenu(pData->hMenu);

	Group_Erase();    

    AB_FREE(pData->pGroupTone);

    AB_UnRegisterNotify(pData->pPointer);

	UnregisterClass("ABGroupsWndClass",NULL);

    return;

}
/*********************************************************************\
* Function	ABGroups_OnClose
* Purpose   WM_CLOSE message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void ABGroups_OnClose(HWND hWnd)
{

    AB_GROUPDATA* pData;

    pData = (AB_GROUPDATA*)GetUserData(hWnd);

    SendMessage(pData->hFrameWnd,PWM_CLOSEWINDOW,(WPARAM)hWnd,NULL);

    DestroyWindow (hWnd);

    return;

}

/*********************************************************************\
* Function	   ABGroups_OnSetTone
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void ABGroups_OnSetTone(HWND hWnd,BOOL bSelect,UINT nLen,char* szTonePath)
{
    int  nIndex;
    HWND hList;
    GROUP_NODE *pGroup;
    
    if(bSelect == FALSE)
        return;

    hList = GetDlgItem(hWnd,IDC_ABGROUPS_LIST);
    
    nIndex = SendMessage(hList,LB_GETCURSEL,0,0);
    
    if(nIndex == LB_ERR)
        return;
    
    pGroup = (GROUP_NODE*)SendMessage(hList,LB_GETITEMDATA,nIndex,0);

    AB_ReWriteGroupTone(hWnd,pGroup->Info.nGroupID,nLen,szTonePath);

	PLXTipsWin(NULL,NULL,0,IDS_TONECHANGED,NULL,Notify_Success,IDS_OK,NULL,WAITTIMEOUT);
}
/*********************************************************************\
* Function	   Group_New
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void ABGroups_OnGroupDeleted(HWND hWnd,BOOL bDelete)
{
    HWND hLst;
    int  nIndex;
    char szButText[20];
    char szMenu[20];
    GROUP_NODE *pGroup;
    int  nCount;
    AB_GROUPDATA* pData;

    pData = (AB_GROUPDATA*)GetUserData(hWnd);

    if(bDelete == FALSE)
        return;
    
    hLst = GetDlgItem(hWnd,IDC_ABGROUPS_LIST);

    nIndex = SendMessage(hLst,LB_GETCURSEL,0,0);

    if(nIndex == LB_ERR)
        return;
    
    pGroup = (GROUP_NODE*)SendMessage(hLst,LB_GETITEMDATA,nIndex,0);
    
    if(AB_DeleteGroup(pGroup))
    {
        AB_DeleteGroupTone(hWnd,pGroup->Info.nGroupID);

        PLXTipsWin(NULL,NULL,0,(char*)IDS_DELETED,pGroup->Info.szGroupName, Notify_Success,IDS_OK, NULL, WAITTIMEOUT);//(char*)
        
        SendMessage(hLst,LB_DELETESTRING,nIndex,0);
        
        Group_Delete(pGroup);
        
        nCount = SendMessage(hLst,LB_GETCOUNT,0,0);
        
        if(nIndex == nCount)
            SendMessage(hLst,LB_SETCURSEL,nIndex-1,0);
        else
            SendMessage(hLst,LB_SETCURSEL,nIndex,0);
        
        nIndex = SendMessage(hLst,LB_GETCURSEL,0,0);
        
        memset(szButText,0,20);
        memset(szMenu,0,20);
        
        SendMessage(pData->hFrameWnd, PWM_GETBUTTONTEXT, 1, (LPARAM)szButText);
        SendMessage(pData->hFrameWnd, PWM_GETBUTTONTEXT, 2, (LPARAM)szMenu);
        
        if(nIndex == 0)
        {  
            if(strcmp(szButText,IDS_OPEN) == 0)
                SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,1, (LPARAM)"");
            
            if(strcmp(szMenu,ICON_SELECT) != 0)
                SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,2, (LPARAM)ICON_SELECT);
        }
        else
        {
            if(strcmp(szButText,IDS_OPEN) != 0)
                SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,1, (LPARAM)IDS_OPEN);
            
            if(strcmp(szMenu,ICON_OPTIONS) != 0)
                SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,2, (LPARAM)ICON_OPTIONS);
        }
    }
}
/*********************************************************************\
* Function	   Group_New
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static GROUP_NODE* Group_New(void)
{
    GROUP_NODE* p = NULL;

    p = (GROUP_NODE*)malloc(sizeof(GROUP_NODE));

    memset(p,0,sizeof(GROUP_NODE));

    p->dwOffset = -1;
    p->Info.nDel = 1;

    return p;
}
/*********************************************************************\
* Function	   Group_Insert
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static int Group_Insert(GROUP_NODE* pChainNode)
{
    GROUP_NODE* p;
    int     k;
    BOOL    bIsTail;

    k = 0;
    bIsTail = FALSE;

    if(pHeader)
    {
        p = pHeader;

        while( stricmp(pChainNode->Info.szGroupName,p->Info.szGroupName) > 0 )
        {
            k++;
            if( p->pNext )
                p = p->pNext;
            else
            {
                bIsTail = TRUE;
                break;
            }
        }

        if( bIsTail )
        {
            p->pNext = pChainNode;
            pChainNode->pNext = NULL;
            pChainNode->pPioneer = p;
        }
        else
        {
            if( p == pHeader)
            {
                pChainNode->pNext = p;
                pChainNode->pPioneer = NULL;
                p->pPioneer = pChainNode;
                pHeader = pChainNode;
            }
            else
            {
                p->pPioneer->pNext = pChainNode;
                pChainNode->pPioneer = p->pPioneer;
                pChainNode->pNext = p;
                p->pPioneer = pChainNode;
            }
        }
    }
    else
    {
        pHeader = pChainNode;
        pChainNode->pPioneer = NULL;
        pChainNode->pNext = NULL;
    }

    return k;
}
/*********************************************************************\
* Function	   Group_Delete
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void Group_Delete(GROUP_NODE* pChainNode)
{
    Group_DeleteFromChain(pChainNode);

    free(pChainNode);
    pChainNode = NULL;
}
/*********************************************************************\
* Function	   Group_DeleteFromChain
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void Group_DeleteFromChain(GROUP_NODE* pChainNode)
{
    if( pChainNode == pHeader )
    {
        if(pChainNode->pNext)
        {
            pHeader = pChainNode->pNext;
            pChainNode->pNext->pPioneer = NULL;
        }
        else
            pHeader = NULL;
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
}
/*********************************************************************\
* Function	   Group_Erase
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void Group_Erase(void)
{
    GROUP_NODE* p;    
    GROUP_NODE* ptemp;

    p = pHeader;

    while( p )
    {
        ptemp = p->pNext;
        
        free(p);
        p = ptemp;
    }

    pHeader = NULL;
}



BOOL AB_CreateGroupsNewWnd(HWND hFrameWnd,GROUP_NODE* pGroup,BOOL bNew)
{
    WNDCLASS    wc;
    HWND        hGroupsNewWnd;
    GROUPNEW_DATA Data;
    const char    *pszCaption;
    RECT        reClient;

    wc.style         = 0;
    wc.lpfnWndProc   = ABGroupsNewWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = sizeof(GROUPNEW_DATA);
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName  = "ABGroupsNewWndClass";
    
    if (!RegisterClass(&wc))
        return FALSE;

    memset(&Data,0,sizeof(GROUPNEW_DATA));

    Data.hFrameWnd = hFrameWnd;

    Data.pGroup = Group_New();

    memcpy(Data.pGroup,pGroup,sizeof(GROUP_NODE));

    Data.bNew = bNew;
             
    if(bNew)
        pszCaption = IDS_NEWGROUP;
    else
        pszCaption = IDS_RENAME_GROUP;

    GetClientRect(hFrameWnd,&reClient);

    hGroupsNewWnd = CreateWindow(
        "ABGroupsNewWndClass",
        pszCaption, 
        WS_VISIBLE | WS_CHILD, 
        reClient.left,
        reClient.top,
        reClient.right - reClient.left,
        reClient.bottom - reClient.top,
        hFrameWnd,
        NULL, 
        NULL, 
        (PVOID)&Data
        );
    
    if (!hGroupsNewWnd)
    {
        free(Data.pGroup);

        UnregisterClass("ABGroupsNewWndClass",NULL);

        return FALSE;
    }

    SetFocus(hGroupsNewWnd);

    SetWindowText(hFrameWnd,pszCaption);

    //SendMessage(hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_BACK, 0), (LPARAM)"");
    SendMessage(hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_SAVE, 1), (LPARAM)IDS_SAVE);
    SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
    
    SendMessage(hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON),(LPARAM)NULL);
    
    ShowWindow(hGroupsNewWnd,SW_SHOW);
    UpdateWindow(hGroupsNewWnd);
    
    return TRUE;
}

LRESULT ABGroupsNewWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    LRESULT lResult = TRUE;

	switch (wMsgCmd)
    {
    case WM_CREATE:
        lResult = ABGroupsNew_OnCreate(hWnd,(LPCREATESTRUCT)(lParam));
        break;

    case WM_ACTIVATE:
    case PWM_SHOWWINDOW:
        ABGroupsNew_OnActivate(hWnd,(UINT)LOWORD(wParam));
        break;

    case WM_SETFOCUS:
        ABGroupsNew_OnSetFocus(hWnd);
        break;

    case WM_PAINT:
        ABGroupsNew_OnPaint(hWnd);
        break;

    case WM_KEYDOWN:
        ABGroupsNew_OnKey(hWnd,(UINT)(wParam),(int)(short)LOWORD(lParam),(UINT)HIWORD(lParam));
        break;

    case WM_COMMAND:
        ABGroupsNew_OnCommand(hWnd,(int)(LOWORD(wParam)),(UINT)HIWORD(wParam));
        break;
        
    case WM_CLOSE:
        ABGroupsNew_OnClose(hWnd);
        break;

    case WM_DESTROY:
        ABGroupsNew_OnDestroy(hWnd);
        break;

    default:     
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
	}

    return lResult;

}
/*********************************************************************\
* Function	ABGroupsNew_OnCreate
* Purpose   WM_CREATE message handler of the main window
* Params
*			hWnd: Handle of the window
*			lpCreateStruct: Create Structure
* Return
*			TRUE: Success
*			FALSE: Fail
* Remarks
**********************************************************************/
static BOOL ABGroupsNew_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
{    
    RECT rect;
    HWND hEdit;
    IMEEDIT ie;
    GROUPNEW_DATA* pData;

    pData = GetUserData(hWnd);
    
    memcpy(pData,lpCreateStruct->lpCreateParams,sizeof(GROUPNEW_DATA));
        
    GetClientRect(hWnd,&rect);
    
    memset(&ie, 0, sizeof(IMEEDIT));
    
    ie.hwndNotify	= (HWND)hWnd;    
    ie.dwAttrib	    = 0;                
    ie.dwAscTextMax	= 0;
    ie.dwUniTextMax	= 0;
    ie.wPageMax	    = 0;        
    ie.pszCharSet	= NULL;
    ie.pszTitle	    = NULL;
    ie.pszImeName	= NULL;
        
    hEdit = CreateWindow(
        "IMEEDIT",
        "",
        WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL | WS_TABSTOP | ES_TITLE ,       
        rect.left,
        rect.top,
        rect.right - rect.left,
        (rect.bottom - rect.top)/3,
        hWnd,
        (HMENU)IDC_ABGROUPSNEW_EDIT,
        NULL,
        (PVOID)&ie);
    
    if(hEdit == NULL)
        return FALSE;
    
    SendMessage(hEdit, EM_LIMITTEXT, (WPARAM)(AB_MAXLEN_GROUPNAME-1), NULL);
    
    SendMessage(hEdit, EM_SETTITLE, 0, (LPARAM)IDS_GROUPNAME);
    
    SetWindowText(hEdit, pData->pGroup->Info.szGroupName);
    
    SendMessage(hEdit, EM_SETSEL, -1, -1);
        
    return TRUE;
    
}
/*********************************************************************\
* Function	ABGroupsNew_OnActivate
* Purpose   WM_ACTIVATE message handler of the main window
* Params
* Return    None
* Remarks
**********************************************************************/
static void ABGroupsNew_OnActivate(HWND hwnd, UINT state)
{
    HWND hEdt;

    GROUPNEW_DATA* pData;

    char* pszCaption = NULL;

    pData = GetUserData(hwnd);
             
    if(pData->bNew)
        pszCaption = (char*)IDS_NEWGROUP;
    else
        pszCaption = (char*)IDS_RENAMEGROUP;

    hEdt = GetDlgItem(hwnd,IDC_ABGROUPSNEW_EDIT);

    SetFocus(hEdt);

    SetWindowText(pData->hFrameWnd,pszCaption);

    SendMessage(pData->hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_SAVE);
    
    SendMessage(pData->hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON),(LPARAM)NULL);
 
    return;
}

/*********************************************************************\
* Function	ABGroupsNew_OnSetFocus
* Purpose   WM_SETFOCUS message handler of the main window
* Params	hWnd: Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void ABGroupsNew_OnSetFocus(HWND hWnd)
{
    HWND hEdt;

    hEdt = GetDlgItem(hWnd,IDC_ABGROUPSNEW_EDIT);

    SetFocus(hEdt);  
}
/*********************************************************************\
* Function	ABGroupsNew_OnPaint
* Purpose   WM_PAINT message handler of the main window
* Params	hWnd: Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void ABGroupsNew_OnPaint(HWND hWnd)
{
	HDC hdc = BeginPaint(hWnd, NULL);

	EndPaint(hWnd, NULL);

	return;
}

/*********************************************************************\
* Function	ABGroupsNew_OnKey
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
static void ABGroupsNew_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags)
{
    HWND hEdt;
    GROUPNEW_DATA* pData;

    switch (vk)
	{
	case VK_F10:
		PostMessage(hWnd,WM_CLOSE,NULL,NULL);
		break;

    case VK_RETURN:
        pData = (GROUPNEW_DATA*)GetUserData(hWnd);

        hEdt = GetDlgItem(hWnd,IDC_ABGROUPSNEW_EDIT);
        
        if(GetWindowTextLength(hEdt) == 0)
        {
            char szCaption[50];

            szCaption[0] = 0;
            GetWindowText(pData->hFrameWnd,szCaption,49);
            szCaption[49] = 0;
            PLXTipsWin(NULL,NULL,0,IDS_DEFINENAME,szCaption,Notify_Alert,IDS_OK,NULL,WAITTIMEOUT);
            break;
        }
        
        GetWindowText(hEdt,pData->pGroup->Info.szGroupName,AB_MAXLEN_GROUPNAME);
        
        AB_SaveGroupInfo(pData->pGroup,pData->bNew);
        
        if(pData->bNew == FALSE)
            AB_NotifyWnd(pData->pGroup,AB_MODE_DELETE,AB_OBJECT_ALL,AB_MDU_GROUP);

        AB_NotifyWnd(pData->pGroup,AB_MODE_INSERT,AB_OBJECT_ALL,AB_MDU_GROUP);
                
        PostMessage(hWnd,WM_CLOSE,NULL,NULL);
        break;

	default:
		PDADefWindowProc(hWnd, WM_KEYDOWN, vk, MAKELPARAM(cRepeat, flags));
		break;
	}

	return;
}
/*********************************************************************\
* Function	ABGroupsNew_OnCommand
* Purpose   WM_COMMAND message handler of the main window
* Params
*			hWnd:	Handle of the window
*			id:		Id of command message
*			hwndCtl: Handle of the window which sended this message
*			codeNotify:Notify code of the message
* Return	None
* Remarks
**********************************************************************/
static void ABGroupsNew_OnCommand(HWND hWnd, int id, UINT codeNotify)
{

	switch(id)
	{
    default:
        break;
    }
}
/*********************************************************************\
* Function	ABGroupsNew_OnDestroy
* Purpose   WM_DESTROY message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void ABGroupsNew_OnDestroy(HWND hWnd)
{

    GROUPNEW_DATA* pData;

    pData = (GROUPNEW_DATA*)GetUserData(hWnd);

    free(pData->pGroup);

    UnregisterClass("ABGroupsNewWndClass", NULL);
	    
    return;

}
/*********************************************************************\
* Function	ABGroupsNew_OnClose
* Purpose   WM_CLOSE message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void ABGroupsNew_OnClose(HWND hWnd)
{

    GROUPNEW_DATA* pData;

    pData = (GROUPNEW_DATA*)GetUserData(hWnd);

    SendMessage(pData->hFrameWnd,PWM_CLOSEWINDOW,(WPARAM)hWnd,NULL);

    DestroyWindow (hWnd);

    return;

}
/*********************************************************************\
* Function	AB_FindUseableID
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
static int AB_FindUseableID(void)
{
    int nIDTemp = 1;

    while( nIDTemp )
    {
        if(AB_IsGroupIDUsed(nIDTemp))
            nIDTemp++;
        else
            return nIDTemp;
    }

    return nIDTemp;
}
/*********************************************************************\
* Function	AB_IsGroupNameUsed
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
static BOOL AB_IsGroupNameUsed(char* pszGroupName)
{
    GROUP_NODE *pTemp;
    
    pTemp = pHeader;
    
    while( pTemp )
    {
        if(pTemp->Info.nGroupID > 0 && strcmp(pTemp->Info.szGroupName,pszGroupName) == 0)
        {
            return TRUE;
        }
        pTemp = pTemp->pNext;
    }

    return FALSE;
}
/*********************************************************************\
* Function	AB_IsGroupIDUsed
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
static BOOL AB_IsGroupIDUsed(int ID)
{
    GROUP_NODE* pTemp;
    
    pTemp = pHeader;
    
    while( pTemp )
    {
        if(pTemp->Info.nGroupID == ID)
        {
            return TRUE;
        }
        pTemp = pTemp->pNext;
    }

    return FALSE;
}
/*********************************************************************\
* Function	AB_FindUseableName
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
static BOOL AB_FindUseableName(char* szGroupName)
{
    int nLoop = 1;

    while( nLoop )
    {
        sprintf(szGroupName,IDS_GROUPNAMETEMP,nLoop);

        if(AB_IsGroupNameUsed(szGroupName))
            nLoop++;
        else
            return TRUE;
    }
    return FALSE;
}
/*********************************************************************\
* Function	AB_ReadGroup
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
static BOOL AB_ReadGroup(GROUP_NODE *pNode,int* pnCount)
{
    int handle;
    char szOldPath[PATH_MAXLEN];

    szOldPath[0] = 0;

    if(pNode)
    { 
        getcwd(szOldPath,PATH_MAXLEN);  
        chdir(PATH_DIR_AB);      
        handle = open(PATH_FILE_GROUP,O_RDONLY,S_IRUSR);        
        if(handle == -1)
        {
            chdir(szOldPath);    
            return FALSE;
        }
        pNode->dwOffset = *pnCount*sizeof(GROUP_INFO);
        lseek(handle,pNode->dwOffset,SEEK_SET);
        read(handle,&(pNode->Info),sizeof(GROUP_INFO));
        close(handle);
        chdir(szOldPath);    
        return TRUE;
    }
    else
    {
        struct stat buf;

        AB_ClearUp();
        
        getcwd(szOldPath,PATH_MAXLEN);  
        chdir(PATH_DIR_AB);   

        memset(&buf,0,sizeof(struct stat));
        stat(PATH_FILE_GROUP,(struct stat*)&buf);

        chdir(szOldPath);  

        *pnCount = buf.st_size/sizeof(GROUP_INFO);

        return TRUE;
    }
}
/*********************************************************************\
* Function	AB_ClearUp
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
static BOOL AB_ClearUp(void)
{
    struct stat buf;
    int fold,fnew;
    int i;   
    char szOldPath[PATH_MAXLEN];

    szOldPath[0] = 0;

    getcwd(szOldPath,PATH_MAXLEN);
    
    chdir(PATH_DIR_AB);
    
    memset(&buf,0,sizeof(struct stat));
    stat(PATH_FILE_GROUP,(struct stat*)&buf);

    if(buf.st_size == 0)
    {
        chdir(szOldPath);
        return TRUE;
    }
        
    fold = open(PATH_FILE_GROUP,O_RDONLY);
    if(fold == -1)
    {
        fold = open(PATH_FILE_GROUP, O_RDWR | O_CREAT, S_IRWXU);
        if(fold == -1)
        {
            chdir(szOldPath);
            return FALSE;
        }
        else
        {
            close(fold);
            chdir(szOldPath);
            return TRUE;
        }
    }
    
    fnew = open(PATH_FILE_TEMP, O_RDWR | O_CREAT, S_IRWXU);
    if(fnew == -1)
    {
        close(fold);
        chdir(szOldPath);
        return FALSE;
    }
    
    if(buf.st_size <= AB_TEMPSPACE_MAX)
    {
        GROUP_INFO* p;
        p = (GROUP_INFO*)malloc(buf.st_size);
        
        if(p == NULL)
        {
            close(fold);
            close(fnew);
            chdir(szOldPath);
            return FALSE;
        }
        
        read(fold,p,buf.st_size);
        
        for(i = 0 ; i < (int)(buf.st_size / sizeof(GROUP_INFO)) ; i++)
        {
            if(p[i].nDel == AB_DEL_MASK)
                continue;
            else
                write(fnew,&p[i],sizeof(GROUP_INFO));
        }
        
        free(p);
    }
    else
    {
        GROUP_INFO temp;
        
        for(i = 0 ; i < (int)(buf.st_size / sizeof(GROUP_INFO)) ; i++)
        {
            read(fold,&temp,sizeof(GROUP_INFO));
            if(temp.nDel == AB_DEL_MASK)
                continue;
            else
                write(fnew,&temp,sizeof(GROUP_INFO));
        }
    }
    
    close(fold);
    close(fnew);
    remove(PATH_FILE_GROUP);
    rename(PATH_FILE_TEMP,PATH_FILE_GROUP);
    chdir(szOldPath);
    return TRUE;
}
/*********************************************************************\
* Function	   AB_SaveGroupInfo
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL AB_SaveGroupInfo(GROUP_NODE *pNode,BOOL bNew)
{
    int f,DelMask;
    char szOldPath[PATH_MAXLEN];
    struct stat buf;

    szOldPath[0] = 0;

    getcwd(szOldPath,PATH_MAXLEN);
    
    chdir(PATH_DIR_AB);
    
    f = open(PATH_FILE_GROUP, O_RDWR | O_CREAT , S_IRWXU);
    if(f == -1)
    {
        chdir(szOldPath);
        return FALSE;
    }

    if(bNew == FALSE)
    {
        DelMask = AB_DEL_MASK;
        lseek(f,pNode->dwOffset,SEEK_SET);
        write(f,&DelMask,sizeof(int));
    }    

    memset(&buf,0,sizeof(struct stat));
    stat(PATH_FILE_GROUP,(struct stat*)&buf);

    pNode->dwOffset = buf.st_size;

    if(pNode->Info.nGroupID == -1)
        pNode->Info.nGroupID = AB_GetUnusedGroupID();

    lseek(f,pNode->dwOffset,SEEK_SET);
    write(f,&(pNode->Info),sizeof(GROUP_INFO));

    close(f);

    chdir(szOldPath);

    return TRUE;
}
/*********************************************************************\
* Function	   AB_DeleteGroup
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL AB_DeleteGroup(GROUP_NODE* pNode)
{
    int f,DelMask;
    char szOldPath[PATH_MAXLEN];

    szOldPath[0] = 0;

    getcwd(szOldPath,PATH_MAXLEN);
    
    chdir(PATH_DIR_AB);
    
    f = open(PATH_FILE_GROUP, O_RDWR , S_IRWXU);
    if(f == -1)
    {
        chdir(szOldPath);
        return FALSE;
    }
    
    DelMask = AB_DEL_MASK;
    lseek(f,pNode->dwOffset,SEEK_SET);
    write(f,&DelMask,sizeof(int));

    close(f);

    chdir(szOldPath);

    return TRUE;
}
/*********************************************************************\
* Function	   AB_GetGroupToneInfo
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL AB_GetGroupToneInfo(GROUPTONE_DATA* pGroupTone,int *nToneCount)
{
    int handle;
    char szOldPath[PATH_MAXLEN];
    int ID;
    int nToneLen;
    struct stat buf;
    LONG DataLen; 
    int nCount;

    szOldPath[0] = 0;
    
    getcwd(szOldPath,PATH_MAXLEN);  
    
    chdir(PATH_DIR_AB);    
    
    memset(&buf,0,sizeof(struct stat));
    
    stat(PATH_FILE_GROUPTONE,(struct stat*)&buf);

    if(buf.st_size == 0)
    {
        *nToneCount = 0;

        chdir(szOldPath);
        
        return TRUE;
    }

    if(pGroupTone)
    {  
        handle = open(PATH_FILE_GROUPTONE,O_RDONLY,S_IRUSR);        
        
        if(handle == -1)
        {
            chdir(szOldPath);    
            
            return FALSE;
        }

        DataLen = buf.st_size;

        nCount = 0;

        while(DataLen > 0)
        {
            read(handle,&ID,sizeof(int));

            read(handle,&nToneLen,sizeof(int));
            
            if(ID != AB_DEL_MASK)
            {
                pGroupTone->idGroup = ID;

                pGroupTone->dwoffset = buf.st_size - DataLen;

                nCount++;
            }
                
            lseek(handle,nToneLen,SEEK_CUR);

            DataLen -= (sizeof(int)*2 + nToneLen);
        }

        close(handle);

        chdir(szOldPath);    
        
        return TRUE;
    }
    else
    { 
        handle = open(PATH_FILE_GROUPTONE,O_RDONLY,S_IRUSR);        
        
        if(handle == -1)
        {
            chdir(szOldPath);    
            
            return FALSE;
        }

        DataLen = buf.st_size;

        nCount = 0;

        while(DataLen > 0)
        {
            read(handle,&ID,sizeof(int));

            read(handle,&nToneLen,sizeof(int));
            
            if(ID != AB_DEL_MASK)
            {
                (*nToneCount)++;
            }
                
            lseek(handle,nToneLen,SEEK_CUR);

            DataLen -= (sizeof(int)*2 + nToneLen);
        }

        close(handle);

        chdir(szOldPath); 

        return TRUE;
    }
}
/*********************************************************************\
* Function	   AB_WriteGroupTone
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL AB_WriteGroupTone(int ID,int nLen,char* pszTonePath,DWORD* pdwoffset)
{
    int f;
    char szOldPath[PATH_MAXLEN];
    struct stat buf;

    szOldPath[0] = 0;
    
    getcwd(szOldPath,PATH_MAXLEN);  
    
    chdir(PATH_DIR_AB);  
    
    memset(&buf,0,sizeof(struct stat));

    stat(PATH_FILE_GROUPTONE,(struct stat*)&buf);
    
    *pdwoffset = buf.st_size;

    f = open(PATH_FILE_GROUPTONE,O_RDWR|O_CREAT|O_APPEND,S_IRUSR);        
    
    if(f == -1)
    {
        chdir(szOldPath);

        return FALSE;
    }

    write(f,&ID,sizeof(int));

    nLen++;

    write(f,&nLen,sizeof(int));

    write(f,pszTonePath,nLen);

    close(f);

    chdir(szOldPath);

    return TRUE;
}

/*********************************************************************\
* Function	   AB_DeleteGroupTone
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
void AB_DeleteGroupTone(HWND hWnd,int ID)
{

    AB_GROUPDATA* pData;

    int i,j;

    pData = (AB_GROUPDATA*)GetUserData(hWnd);

    for(i = 0 ; i < pData->nSetToneCount ; i++)
    {
        if(ID == pData->pGroupTone[i].idGroup)
        {
            AB_DelGroupTone(pData->pGroupTone[i].dwoffset);

            for(j = i ; j < (pData->nSetToneCount-1) ; j++)
            {
                memcpy(&(pData->pGroupTone[j]),&(pData->pGroupTone[j+1]),sizeof(GROUPTONE_DATA));
            }
            pData->nSetToneCount--;
            
            return;
        }
    }

    return;
}
/*********************************************************************\
* Function	   AB_ReWriteGroupTone
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
void AB_ReWriteGroupTone(HWND hWnd,int ID,int nLen,char* pszTonePath)
{
    AB_GROUPDATA* pData;

    int i;
    
    BOOL bExist = FALSE;

    DWORD dwoffset;

    pData = (AB_GROUPDATA*)GetUserData(hWnd);

    for(i = 0 ; i < pData->nSetToneCount ; i++)
    {
        if(pData->pGroupTone[i].idGroup == ID)
        {
            AB_DelGroupTone(pData->pGroupTone[i].dwoffset);
            
            bExist = TRUE;

            break;
        }
    }

    AB_WriteGroupTone(ID,nLen,pszTonePath,&dwoffset);

    if(bExist)
    {
        pData->pGroupTone[i].dwoffset = dwoffset;
    }
    else
    {
        pData->pGroupTone = realloc(pData->pGroupTone,sizeof(GROUPTONE_DATA)*(pData->nSetToneCount+1));
        
        if(pData->pGroupTone == NULL)
        {
            pData->nSetToneCount = 0;
            
            return;
        }
        
        pData->pGroupTone[pData->nSetToneCount].dwoffset = dwoffset;
        
        pData->pGroupTone[pData->nSetToneCount].idGroup = ID;
        
        pData->nSetToneCount++;
    }
}
/*********************************************************************\
* Function	   AB_DelGroupTone
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL AB_DelGroupTone(DWORD dwoffset)
{
    int f;
    char szOldPath[PATH_MAXLEN];
    int nDel = AB_DEL_MASK;

    szOldPath[0] = 0;
    
    getcwd(szOldPath,PATH_MAXLEN);  
    
    chdir(PATH_DIR_AB);  
    
    f = open(PATH_FILE_GROUPTONE,O_RDWR);        
    
    if(f == -1)
    {
        chdir(szOldPath);

        return FALSE;
    }

    lseek(f,dwoffset,SEEK_SET);

    write(f,&nDel,sizeof(int));

    close(f);

    chdir(szOldPath);

    return TRUE;

}
/*********************************************************************\
* Function	   AB_ReadGroupTone
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL AB_ReadGroupTone(DWORD dwoffset, int ID,char* pszTonePath)
{
//    int f;
//    char szOldPath[PATH_MAXLEN];
//    struct stat buf;
//
//    szOldPath[0] = 0;
//    
//    getcwd(szOldPath,PATH_MAXLEN);  
//    
//    chdir(PATH_DIR_AB);  
//    
//    memset(&buf,0,sizeof(struct stat));
//    
//    stat(PATH_FILE_GROUPTONE,(struct stat*)&buf);
//    
//    f = open(PATH_FILE_GROUPTONE,O_RDWR|O_CREAT|O_APPEND,S_IRUSR);        
//    
//    if(f == -1)
//    {
//        chdir(szOldPath);
//
//        return FALSE;
//    }
//
//    write(f,&ID,sizeof(int));
//
//    write(f,&(nLen+1),sizeof(int));
//
//    write(f,pszTonePath,(nLen+1));
//
//    close(f);
//
//    chdir(szOldPath);

    return TRUE;
}

/*********************************************************************\
* Function	   AB_CreateGroupsViewWnd
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL AB_CreateGroupsViewWnd(HWND hFrameWnd,HWND hWnd,GROUP_NODE* pNode)
{
    WNDCLASS    wc;
    HWND        hGroupsViewWnd;
    GROUPVIEW_DATA Data;
    HMENU       hRemoveMenu;
    RECT        rcClient;
   
    wc.style         = 0;
    wc.lpfnWndProc   = ABGroupsViewWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = sizeof(GROUPVIEW_DATA);
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName  = "ABGroupsViewWndClass";
    
    if (!RegisterClass(&wc))
        return FALSE;

    memset(&Data,0,sizeof(GROUPVIEW_DATA));

    Data.hFrameWnd = hFrameWnd;
    Data.hWnd = hWnd;
    Data.pGroup = Group_New();

    if(Data.pGroup)
        memcpy(Data.pGroup,pNode,sizeof(GROUP_NODE));         

    GetClientRect(hFrameWnd,&rcClient);

    Data.hMenu = CreateMenu();
    
    hGroupsViewWnd = CreateWindow(
        "ABGroupsViewWndClass",
        "", 
        WS_VISIBLE | WS_CHILD, 
        rcClient.left,
        rcClient.top,
        rcClient.right - rcClient.left,
        rcClient.bottom - rcClient.top,
        hFrameWnd,
        NULL, 
        NULL, 
        (PVOID)&Data
        );
    
    if (!hGroupsViewWnd)
    {
		DestroyMenu(Data.hMenu);

        free(Data.pGroup);
        UnregisterClass("ABGroupsViewWndClass",NULL);
        return FALSE;
    }

    SetWindowText(hFrameWnd,pNode->Info.szGroupName);

    AppendMenu(Data.hMenu, MF_ENABLED, IDM_ABGROUPSVIEW_RENAME, IDS_RENAMEGROUP);
    AppendMenu(Data.hMenu, MF_ENABLED, IDM_ABGROUPSVIEW_SETTONE, IDS_SETGROUPTONE);
    AppendMenu(Data.hMenu, MF_ENABLED, IDM_ABGROUPSVIEW_REMOVEMEMBER, IDS_REMOVEMEMBER);
    
    hRemoveMenu = CreateMenu();
    AppendMenu(hRemoveMenu, MF_ENABLED, IDM_AB_REMOVESELECT, IDS_SELECT);
    AppendMenu(hRemoveMenu, MF_ENABLED, IDM_AB_REMOVEALL, IDS_ALL);
    
    AppendMenu(Data.hMenu, MF_POPUP|MF_ENABLED, (DWORD)hRemoveMenu, IDS_REMOVEMANY);

    SetFocus(hGroupsViewWnd);
    
    PDASetMenu(hFrameWnd,Data.hMenu);

    SendMessage(hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_BACK, 0), (LPARAM)IDS_BACK);
    SendMessage(hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_OPEN, 1), (LPARAM)"");
    SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_SELECT);

    SendMessage(hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON),(LPARAM)NULL);
    SendMessage(hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON),(LPARAM)NULL);

    ShowWindow(hGroupsViewWnd,SW_SHOW);
    UpdateWindow(hGroupsViewWnd);

    return TRUE;
}

LRESULT ABGroupsViewWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    LRESULT lResult = TRUE;

	switch (wMsgCmd)
    {
    case WM_CREATE:
        lResult = ABGroupsView_OnCreate(hWnd,(LPCREATESTRUCT)(lParam));
        break;

    case WM_ACTIVATE:
    case PWM_SHOWWINDOW:
        ABGroupsView_OnActivate(hWnd,(UINT)LOWORD(wParam));
        break;

    case WM_SETFOCUS:
        ABGroupsView_OnSetFocus(hWnd);
        break;

    case WM_PAINT:
        ABGroupsView_OnPaint(hWnd);
        break;

    case WM_KEYDOWN:
        ABGroupsView_OnKey(hWnd,(UINT)(wParam),(int)(short)LOWORD(lParam),(UINT)HIWORD(lParam));
        break;

    case WM_COMMAND:
        ABGroupsView_OnCommand(hWnd,(int)(LOWORD(wParam)),(UINT)HIWORD(wParam));
        break;
        
    case WM_CLOSE:
        ABGroupsView_OnClose(hWnd);
        break;

    case WM_DESTROY:
        ABGroupsView_OnDestroy(hWnd);
        break;

    case WM_DATACHANGE:
        ABGroupsView_OnDataChange(hWnd,(GROUP_NODE*)wParam,(int)lParam);
        break;

    case WM_DATACHANGECONTACT:
        ABGroupsView_OnDataChangeContact(hWnd,(int)wParam,(int)lParam);
        break;

	case WM_SELECT:
        ABGroupsView_OnSelect(hWnd,(BOOL)LOWORD(wParam),(int)HIWORD(wParam),(DWORD*)lParam);
		break;

	case WM_SETTONE:
		ABGroupsView_OnSetTone(hWnd,(BOOL)(HIWORD(wParam)),(UINT)LOWORD(wParam),(char*)lParam);
		break;

    case WM_SUREADDMEMBER:
        ABGroupsView_OnSureAddMember(hWnd,(BOOL)lParam);
        break;

    case WM_REMOVEMEMBER:
        ABGroupsView_OnRemoveMember(hWnd,(BOOL)lParam);
        break;

    case WM_REMOVEALL:
        ABGroupsView_OnRemoveAll(hWnd,(BOOL)lParam);
        break;

    case WM_REMOVESEL:
        ABGroupsView_OnRemoveSel(hWnd,(BOOL)LOWORD(wParam),(int)HIWORD(wParam),(DWORD*)lParam);
        break;

    case WM_SUREREMOVESEL:
        ABGroupsView_OnSureRemoveSel(hWnd,(BOOL)lParam);
        break;

    case AB_MSG_REFRESHLIST:
        ABGroupsView_OnRefreshList(hWnd,(char*)wParam,(int)LOWORD(lParam),(BOOL)HIWORD(lParam));
        break;

    default:     
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
	}

    return lResult;

}
/*********************************************************************\
* Function	ABGroupsView_OnCreate
* Purpose   WM_CREATE message handler of the main window
* Params
*			hWnd: Handle of the window
*			lpCreateStruct: Create Structure
* Return
*			TRUE: Success
*			FALSE: Fail
* Remarks
**********************************************************************/
static BOOL ABGroupsView_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
{    
    RECT rect;
    HWND hList;
    GROUPVIEW_DATA* pData;
	int i,insert;

    pData = GetUserData(hWnd);

    memcpy(pData,lpCreateStruct->lpCreateParams,sizeof(GROUPVIEW_DATA));
        
    GetClientRect(hWnd,&rect);
    
    hList = CreateWindow(
        "LISTBOX",
        "",
        WS_VISIBLE | WS_CHILD | WS_VSCROLL | LBS_BITMAP,
        rect.left,
        rect.top,
        rect.right - rect.left,
        rect.bottom - rect.top,
        hWnd,
        (HMENU)IDC_ABGROUPSVIEW_LIST,
        NULL,
        NULL);
    
    if(hList == NULL)
        return FALSE;
    
    pData->OldListWndProc = (WNDPROC)SetWindowLong(hList,GWL_WNDPROC,(LONG)CallListWndProc);
    
    pData->hAddMember = LoadImage(NULL, AB_BMP_ADDMEMBER, IMAGE_BITMAP,
		ICON_WIDTH, ICON_HEIGHT, LR_LOADFROMFILE);
    
    pData->hMember = LoadImage(NULL, AB_BMP_MEMBER, IMAGE_BITMAP,
		ICON_WIDTH, ICON_HEIGHT, LR_LOADFROMFILE);

    SendMessage(hList,LB_ADDSTRING,-1,(LPARAM)IDS_ADDMEMBER);
    SendMessage(hList,LB_SETIMAGE,MAKEWPARAM(IMAGE_BITMAP, 0),(LPARAM)pData->hAddMember);

    //need search data 
	for(i = 1; i <= nName ; i++)
	{
		if(AB_IsUserCancel(hWnd, 0))
		{
			PostMessage(hWnd, WM_CLOSE, 0, 0);
			break;		
		}

		if(pData->pGroup->Info.nGroupID == pIndexName[i]->nGroup)
        {
			insert = SendMessage(hList,LB_ADDSTRING,-1,(LPARAM)AB_GetNameString(pIndexName[i]));
            SendMessage(hList,LB_SETITEMDATA,insert,(LPARAM)pIndexName[i]->id);
            SendMessage(hList,LB_SETIMAGE,MAKEWPARAM(IMAGE_BITMAP, insert),(LPARAM)pData->hMember);
        }
	}
    SendMessage(hList,LB_SETCURSEL,0,0);        
        
    pData->handleGroup = AB_RegisterNotify(hWnd,WM_DATACHANGE,AB_OBJECT_ALL,AB_MDU_GROUP);
    pData->handleContact = AB_RegisterNotify(hWnd,WM_DATACHANGECONTACT,AB_OBJECT_NAME,AB_MDU_CONTRACT);

    return TRUE;
    
}
/*********************************************************************\
* Function	ABGroupsView_OnActivate
* Purpose   WM_ACTIVATE message handler of the main window
* Params
* Return    None
* Remarks
**********************************************************************/
static void ABGroupsView_OnActivate(HWND hwnd, UINT state)
{
    HWND hLst;
    GROUPVIEW_DATA* pData;

    pData = GetUserData(hwnd);

    hLst = GetDlgItem(hwnd,IDC_ABGROUPSVIEW_LIST);

    SetFocus(hLst);
    
    PDASetMenu(pData->hFrameWnd,pData->hMenu);

    SendMessage(pData->hFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_BACK);
    if(SendMessage(hLst,LB_GETCURSEL,0,0) == 0)
    {
        SendMessage(pData->hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
        
        SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,2, (LPARAM)ICON_SELECT);
    }
    else
    {
        SendMessage(pData->hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_OPEN);
        SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,2, (LPARAM)ICON_OPTIONS);
    }

    SendMessage(pData->hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON),(LPARAM)NULL);
    SendMessage(pData->hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON),(LPARAM)NULL);

    SetWindowText(pData->hFrameWnd,pData->pGroup->Info.szGroupName);

    return;
}
/*********************************************************************\
* Function	ABGroupsView_OnSetFocus
* Purpose   WM_ACTIVATE message handler of the main window
* Params
* Return    None
* Remarks
**********************************************************************/
static void ABGroupsView_OnSetFocus(HWND hwnd)
{
    HWND hLst;

    hLst = GetDlgItem(hwnd,IDC_ABGROUPSVIEW_LIST);

    SetFocus(hLst);

    return;
}
/*********************************************************************\
* Function	ABGroupsView_OnPaint
* Purpose   WM_PAINT message handler of the main window
* Params	hWnd: Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void ABGroupsView_OnPaint(HWND hWnd)
{
	HDC hdc = BeginPaint(hWnd, NULL);

	EndPaint(hWnd, NULL);

	return;
}

/*********************************************************************\
* Function	ABGroupsView_OnKey
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
static void ABGroupsView_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags)
{
    HWND hLst;
    int  nIndex;
    GROUPVIEW_DATA* pData;

    pData = GetUserData(hWnd);

    switch (vk)
	{
	case VK_F10:
        PostMessage(hWnd,WM_CLOSE,NULL,NULL);
        break;
        
    case VK_RETURN:
        SendMessage(hWnd,WM_COMMAND,IDC_OPEN,NULL);
        break;
        
    case VK_F5:
        hLst = GetDlgItem(hWnd,IDC_ABGROUPSVIEW_LIST);
        nIndex = SendMessage(hLst,LB_GETCURSEL,0,0);
        
        switch(nIndex)
        {
        case LB_ERR:
            break;
            
        case 0:// Add member
			{
				int i,j;

				pData->pId = (DWORD*)malloc(nName*sizeof(DWORD));

				if(pData->pId == NULL)
					break;

				memset(pData->pId,0,nName*sizeof(DWORD));

                pData->nCount = nName;

				for(i = 1,j = 0; i <= nName ; i++)
				{
					if(pIndexName[i]->nGroup == pData->pGroup->Info.nGroupID)
						pData->pId[j++] = pIndexName[i]->id;
				}
				
				ABCreateMultiPickerWnd(pData->hFrameWnd,hWnd,WM_SELECT,(char*)IDS_SELECTMEMBER,pData->nCount,pData->pId,-1,IDS_SAVE,IDS_CANCEL);
			}
            break;
            
        default:
            PDADefWindowProc(pData->hFrameWnd, WM_KEYDOWN, vk, MAKELPARAM(cRepeat, flags));
            break;
        }     
        break;

    case VK_1:
    case VK_2:
    case VK_3:
    case VK_4:
    case VK_5:
    case VK_6:
    case VK_7:
    case VK_8:
    case VK_9:
    case VK_0:
    case VK_F3:
    case VK_F4:
        AB_CreateSearchPopUpWnd(pData->hFrameWnd,hWnd,AB_MSG_REFRESHLIST,FALSE);
        keybd_event(vk,0,0,0);
        break;


	default:
		PDADefWindowProc(hWnd, WM_KEYDOWN, vk, MAKELPARAM(cRepeat, flags));
		break;
	}

	return;
}

/*********************************************************************\
* Function	ABGroupsView_OnCommand
* Purpose   WM_COMMAND message handler of the main window
* Params
*			hWnd:	Handle of the window
*			id:		Id of command message
*			hwndCtl: Handle of the window which sended this message
*			codeNotify:Notify code of the message
* Return	None
* Remarks
**********************************************************************/
static void ABGroupsView_OnCommand(HWND hWnd, int id, UINT codeNotify)
{
    GROUPVIEW_DATA* pData;
    HWND hLst;

    pData = GetUserData(hWnd);

    hLst = GetDlgItem(hWnd,IDC_ABGROUPSVIEW_LIST);

	switch(id)
	{
    case IDC_OPEN:
        {
            int  nIndex;
            CONTACT_ITEMCHAIN *pItem;
			int  nTelID = 0;
            DWORD id;
            MEMORY_NAME *p;


            nIndex = SendMessage(hLst,LB_GETCURSEL,0,0);
            
            if(nIndex == LB_ERR || nIndex == 0)
                break;
            
            id = (DWORD)SendMessage(hLst,LB_GETITEMDATA,nIndex,0);
            
            p = Memory_Find((PVOID)id,AB_FIND_ID);

            if(p == NULL)
                break;

            pItem = NULL;
            
            AB_ReadRecord(p->dwoffset,&pItem,&nTelID);
            
            AB_CreateViewContactWnd(pData->hFrameWnd,pItem,p->dwoffset,p->id,nTelID);

            Item_Erase(pItem);
        }
        break;

    case IDC_ABGROUPSVIEW_LIST:
        if(codeNotify == LBN_SELCHANGE)
        {
            char    szButText[20];
            char    szMenu[20];
            int     nIndex;

            nIndex = SendMessage(hLst,LB_GETCURSEL,0,0);
            if(nIndex == LB_ERR)
                break;
            
            memset(szButText,0,20);
            SendMessage(pData->hFrameWnd, PWM_GETBUTTONTEXT, 1, (LPARAM)szButText);
            memset(szMenu,0,20);
            SendMessage(pData->hFrameWnd, PWM_GETBUTTONTEXT, 2, (LPARAM)szMenu);
            
            if(nIndex == 0)
            {  
                if(strcmp(szButText,IDS_OPEN) == 0)
                    SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,1, (LPARAM)"");
                
                if(strcmp(szMenu,ICON_SELECT) != 0)
                    SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,2, (LPARAM)ICON_SELECT);
            }
            else
            {
                if(strcmp(szButText,IDS_OPEN) != 0)
                    SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,1, (LPARAM)IDS_OPEN);
                
                if(strcmp(szMenu,ICON_OPTIONS) != 0)
                    SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,2, (LPARAM)ICON_OPTIONS);
            }
        }
        break;

    case IDM_ABGROUPSVIEW_RENAME:
        AB_CreateGroupsNewWnd(pData->hFrameWnd,pData->pGroup,FALSE);
        break;

    case IDM_ABGROUPSVIEW_SETTONE:
        //APP_PreviewRing(pData->hFrameWnd,hWnd,WM_SETTONE,(PSTR)IDS_GROUPTONE);
		{
			HWND hRing;
			char szRing[AB_MAXLEN_RING];
			
			AB_GetGroupToneByID(pData->pGroup->Info.nGroupID, szRing, AB_MAXLEN_RING);

			hRing = PreviewSoundEx(pData->hFrameWnd, hWnd, WM_SETTONE, (PSTR)IDS_GROUPTONE, TRUE);
			
			if(hRing)
			{
				SendMessage(hRing, SPM_SETDEFTONE, 0, 0);
				SendMessage(hRing, SPM_SETCURSEL, 0, (LPARAM)szRing);
			}
		}
		
        break;
        
    case IDM_ABGROUPSVIEW_REMOVEMEMBER:
        {
            int  nIndex;
            char szPrompt[125];
            char szTitle[50];
            DWORD id;
            MEMORY_NAME *p;
            HDC  hdc;
            char szCaption[50];

            nIndex = SendMessage(hLst,LB_GETCURSEL,0,0);
            
            if(nIndex == LB_ERR || nIndex == 0)
                break;

            id = (DWORD)SendMessage(hLst,LB_GETITEMDATA,nIndex,0);            

            p = Memory_Find((PVOID)id,AB_FIND_ID);

            if(p == NULL)
                break;

            szTitle[0] = 0;

            GetWindowText(pData->hFrameWnd,szTitle,49);

            szTitle[49] = 0;
            
            hdc = GetDC(hWnd);
            
            GetExtentFittedText(hdc,AB_GetNameString(p),-1,szCaption,50,TEXT_WIDTH,OMIT_SUFFIX,SUFFIX_REPEAT);
            
            ReleaseDC(hWnd,hdc);

            szPrompt[0] = 0;

            sprintf(szPrompt,"%s:\r\n%s?",szCaption,IDS_REMOVEMEMBER);

            PLXConfirmWinEx(pData->hFrameWnd,hWnd,szPrompt, Notify_Request, szTitle,
                IDS_YES, IDS_NO,WM_REMOVEMEMBER);                        

        }
        break;

    case IDM_AB_REMOVESELECT:
        {
            int  nCount;
            GROUPVIEW_DATA* pData;
            
            pData = GetUserData(hWnd);

            nCount = SendMessage(hLst,LB_GETCOUNT,0,0);

            pData->nCount = nCount-1;

            pData->pId = (DWORD*)malloc(sizeof(DWORD)*pData->nCount);

            ABCreateMultiPickerWnd(pData->hFrameWnd,hWnd,WM_REMOVESEL,(char*)IDS_SELECTCONTACT,pData->nCount,
                pData->pId,pData->pGroup->Info.nGroupID,IDS_REMOVE,IDS_CANCEL);                     

        }
        break;

    case IDM_AB_REMOVEALL:
        {
            char szTitle[50];

            szTitle[0] = 0;

            GetWindowText(pData->hFrameWnd,szTitle,49);

            szTitle[49] = 0;

            PLXConfirmWinEx(pData->hFrameWnd,hWnd,IDS_REMOVEALL, Notify_Request, szTitle, IDS_YES, IDS_NO,WM_REMOVEALL);                        
        }
        break;

    default:
        break;
    }
}
/*********************************************************************\
* Function	ABGroupsView_OnDestroy
* Purpose   WM_DESTROY message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void ABGroupsView_OnDestroy(HWND hWnd)
{

    GROUPVIEW_DATA* pData;

    pData = GetUserData(hWnd);

    if(pData->hAddMember)
        DeleteObject(pData->hAddMember);

    if(pData->hMember)
        DeleteObject(pData->hMember);

    free(pData->pGroup);

    DestroyMenu(pData->hMenu);

    AB_UnRegisterNotify(pData->handleGroup);

    AB_UnRegisterNotify(pData->handleContact);

    UnregisterClass("ABGroupsViewWndClass", NULL);
	    
    return;

}
/*********************************************************************\
* Function	ABGroupsView_OnClose
* Purpose   WM_CLOSE message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void ABGroupsView_OnClose(HWND hWnd)
{
    GROUPVIEW_DATA* pData;

    pData = GetUserData(hWnd);

    SendMessage(pData->hFrameWnd,PWM_CLOSEWINDOW,(WPARAM)hWnd,NULL);

    DestroyWindow (hWnd);

    return;

}

/*********************************************************************\
* Function	CallListWndProc
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
static LRESULT CallListWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    LRESULT lResult = TRUE;
    HWND    hParent;
    GROUPVIEW_DATA *pData;

    hParent = GetParent(hWnd);
    
    pData = GetUserData(hParent);
    
    switch(wMsgCmd)
    {         
    case WM_KEYDOWN:
        switch (LOWORD(wParam))
        { 
        case VK_LEFT:
        case VK_RIGHT:
        case VK_1:
        case VK_2:
        case VK_3:
        case VK_4:
        case VK_5:
        case VK_6:
        case VK_7:
        case VK_8:
        case VK_9:
        case VK_0:
        case VK_F3:
        case VK_F4:
            PostMessage(hParent,wMsgCmd,wParam,lParam);
            return lResult;

        default:
            return CallWindowProc(pData->OldListWndProc, hWnd, wMsgCmd, wParam, lParam);
        }
    default:
        return CallWindowProc(pData->OldListWndProc, hWnd, wMsgCmd, wParam, lParam);
    }
    return lResult;       
}

/*********************************************************************\
* Function	ABGroupsView_OnDataChange
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
static void ABGroupsView_OnDataChange(HWND hWnd,GROUP_NODE* pGroup,int nMode)
{
    GROUPVIEW_DATA* pData;
    
    pData = GetUserData(hWnd);

    memcpy(pData->pGroup,pGroup,sizeof(GROUP_NODE));

    SetWindowText(pData->hFrameWnd,pData->pGroup->Info.szGroupName);
    
}
/*********************************************************************\
* Function	ABGroups_OnDataChange
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
static void ABGroups_OnDataChange(HWND hWnd,GROUP_NODE *pNode,int nMode)
{
    HWND hList;
    char szButText[20],szMenu[20];
    GROUP_NODE *pGroup;
    int n,i,nCount;
    AB_GROUPDATA* pData;

    pData = (AB_GROUPDATA*)GetUserData(hWnd);

    hList = GetDlgItem(hWnd,IDC_ABGROUPS_LIST);
    
    switch(nMode)
    {
    case AB_MODE_INSERT:

        pGroup = Group_New();

        memcpy(pGroup,pNode,sizeof(GROUP_NODE));
        
        n = Group_Insert(pGroup);

        n += 1;
        
        SendMessage(hList,LB_INSERTSTRING,n,(LPARAM)(pGroup->Info.szGroupName));
        
        SendMessage(hList,LB_SETIMAGE,MAKEWPARAM(IMAGE_BITMAP, n),(LPARAM)pData->hGroupItem);
        
        SendMessage(hList,LB_SETITEMDATA,n,(LPARAM)pGroup);
        
        SendMessage(hList,LB_SETCURSEL,n,0);
        
        memset(szButText,0,20);
        
        memset(szMenu,0,20);
        
        SendMessage(pData->hFrameWnd, PWM_GETBUTTONTEXT, 1, (LPARAM)szButText);
        
        SendMessage(pData->hFrameWnd, PWM_GETBUTTONTEXT, 2, (LPARAM)szMenu);
        
        if(n == 0)
        {  
            if(strcmp(szButText,IDS_OPEN) == 0)
                SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,1, (LPARAM)"");
            
            if(strcmp(szMenu,ICON_SELECT) != 0)
                SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,2, (LPARAM)ICON_SELECT);
        }
        else
        {
            if(strcmp(szButText,IDS_OPEN) != 0)
                SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,1, (LPARAM)IDS_OPEN);
            
            if(strcmp(szMenu,ICON_OPTIONS) != 0)
                SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,2, (LPARAM)ICON_OPTIONS);
        }
        break;
        
    case AB_MODE_DELETE:

        nCount = SendMessage(hList,LB_GETCOUNT,NULL,NULL);

        for(i = 1; i < nCount ; i++)
        {
            pGroup = (GROUP_NODE*)SendMessage(hList,LB_GETITEMDATA,i,NULL);

            if(pGroup)
            {
                if(pGroup->Info.nGroupID == pNode->Info.nGroupID)
                {
                    SendMessage(hList,LB_DELETESTRING,i,NULL);

                    Group_Delete(pGroup);

                    break;
                }
            }
        }
        
        n = SendMessage(hList,LB_GETCURSEL,0,0);

        if( n == SendMessage(hList,LB_GETCOUNT,0,0))
            SendMessage(hList,LB_SETCURSEL,n-1,NULL);
        else
            SendMessage(hList,LB_SETCURSEL,n,NULL);
        
        memset(szButText,0,20);
        
        memset(szMenu,0,20);
        
        SendMessage(pData->hFrameWnd, PWM_GETBUTTONTEXT, 1, (LPARAM)szButText);
        
        SendMessage(pData->hFrameWnd, PWM_GETBUTTONTEXT, 2, (LPARAM)szMenu);
        
        n = SendMessage(hList,LB_GETCURSEL,0,0);
                
        if(n == 0)
        {  
            if(strcmp(szButText,IDS_OPEN) == 0)
                SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,1, (LPARAM)"");
            
            if(strcmp(szMenu,ICON_SELECT) != 0)
                SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,2, (LPARAM)ICON_SELECT);
        }
        else
        {
            if(strcmp(szButText,IDS_OPEN) != 0)
                SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,1, (LPARAM)IDS_OPEN);
            
            if(strcmp(szMenu,ICON_OPTIONS) != 0)
                SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,2, (LPARAM)ICON_OPTIONS);
        }
        break;
        
    default:
        break;
    }
}
/*********************************************************************\
* Function	ABGroupsView_OnSelect
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
static void ABGroupsView_OnSelect(HWND hWnd,BOOL bSelect,int nCount,DWORD* pnId)
{
	HWND hLst;
	int i,j;
	CONTACT_ITEMCHAIN *pItem;
    GROUPVIEW_DATA* pData;
    int nTelID = 0;
    
    pData = GetUserData(hWnd);

    pData->nCount = nCount;

	if(bSelect == FALSE)
	{
		AB_FREE(pData->pId);

        pData->nCount = 0;

		return;
	}

    if(nCount == 0)
    {   
		WaitWinWithTimer(NULL, TRUE, ML("Waiting..."), IDS_CONTACTS, NULL, NULL,
            NULL, 10000);		//infinite

        hLst = GetDlgItem(hWnd,IDC_ABGROUPSVIEW_LIST);
        
        SendMessage(hLst, LB_RESETCONTENT,0,0);
        
        SendMessage(hLst,LB_ADDSTRING,-1,(LPARAM)IDS_ADDMEMBER);
        
        SendMessage(hLst,LB_SETIMAGE,MAKEWPARAM(IMAGE_BITMAP, 0),(LPARAM)pData->hAddMember);

        for(i = 1 ; i <= nName ; i++)
        {
            if(pIndexName[i]->nGroup == pData->pGroup->Info.nGroupID)
            {
                pItem = NULL;
                
                AB_ReadRecord(pIndexName[i]->dwoffset,&pItem,&nTelID);
                
                pIndexName[i]->nGroup = -1;
                
				AB_ChangeGroup(&(pIndexName[i]->dwoffset), &(pIndexName[i]->id), &(pIndexName[i]->nGroup));

                //AB_SaveRecord(&(pIndexName[i]->dwoffset),&(pIndexName[i]->id),
                //    &(pIndexName[i]->nGroup),TRUE,nTelID,pItem);
                
                Item_Erase(pItem);
            }
        }
        SendMessage(hLst,LB_SETCURSEL,0,0);
        
		AB_FREE(pData->pId);

        pData->nCount = 0;

		WaitWinWithTimer(NULL, FALSE, ML("Waiting..."), IDS_CONTACTS, NULL, NULL,
            NULL, 10000);		//infinite
    }
    else
    {
        for(i = 1 , j = 0; i <= nName ; i++)
        {
            if(pnId[j] == pIndexName[i]->id)
            {
                j++;
                
                if(pIndexName[i]->nGroup != pData->pGroup->Info.nGroupID && AB_IsGroupIDUsed(pIndexName[i]->nGroup))
                {
                    char szCaption[50];

                    szCaption[0] = 0;

                    GetWindowText(pData->hFrameWnd,szCaption,49);

                    PLXConfirmWinEx(pData->hFrameWnd,hWnd,IDS_BELONGOTHERS, Notify_Request, szCaption, 
                        IDS_YES, IDS_NO,WM_SUREADDMEMBER);

                    return;
                }
            }
        }

        ABGroupsView_OnSureAddMember(hWnd,TRUE);
    }
        
}
/*********************************************************************\
* Function	ABGroupsView_OnSureAddMember
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
static void ABGroupsView_OnSureAddMember(HWND hWnd,BOOL bAdd)
{
    HWND    hLst;
    GROUPVIEW_DATA* pData;
	int i,j,insert;
	CONTACT_ITEMCHAIN *pItem;
    int nTelID = 0;
    
    pData = GetUserData(hWnd);

    if(bAdd == FALSE)
    {
		AB_FREE(pData->pId);

        pData->nCount = 0;

        return;
    }
    
    hLst = GetDlgItem(hWnd,IDC_ABGROUPSVIEW_LIST);
    
    SendMessage(hLst, LB_RESETCONTENT,0,0);
    
    SendMessage(hLst,LB_ADDSTRING,-1,(LPARAM)IDS_ADDMEMBER);

    SendMessage(hLst,LB_SETIMAGE,MAKEWPARAM(IMAGE_BITMAP, 0),(LPARAM)pData->hAddMember);

	WaitWinWithTimer(NULL, TRUE, ML("Waiting..."), IDS_CONTACTS, NULL, NULL,
            NULL, 10000);		//infinite
	
    for(i = 1, j = 0; i <= nName ; i++)
    {
        if(j < pData->nCount && pData->pId[j] == pIndexName[i]->id)
        {
            j++;
            
            if(pIndexName[i]->nGroup != pData->pGroup->Info.nGroupID)
            {
                pItem = NULL;
                
                AB_ReadRecord(pIndexName[i]->dwoffset,&pItem,&nTelID);
                
                pIndexName[i]->nGroup = pData->pGroup->Info.nGroupID;
                
				AB_ChangeGroup(&(pIndexName[i]->dwoffset), &(pIndexName[i]->id), &(pIndexName[i]->nGroup));
                //AB_SaveRecord(&(pIndexName[i]->dwoffset),&(pIndexName[i]->id),
                //    &(pIndexName[i]->nGroup),TRUE,nTelID,pItem);
                
                Item_Erase(pItem);
            }
            
            insert = SendMessage(hLst,LB_ADDSTRING,-1,(LPARAM)AB_GetNameString(pIndexName[i]));
            SendMessage(hLst,LB_SETITEMDATA,insert,(LPARAM)pIndexName[i]->id);
            SendMessage(hLst,LB_SETIMAGE,MAKEWPARAM(IMAGE_BITMAP, insert),(LPARAM)pData->hMember);
        }
        else
        {
            if(pIndexName[i]->nGroup == pData->pGroup->Info.nGroupID)
            {
                pItem = NULL;
                
                AB_ReadRecord(pIndexName[i]->dwoffset,&pItem,&nTelID);
                
                pIndexName[i]->nGroup = -1;
                
				AB_ChangeGroup(&(pIndexName[i]->dwoffset), &(pIndexName[i]->id), &(pIndexName[i]->nGroup));
                //AB_SaveRecord(&(pIndexName[i]->dwoffset),&(pIndexName[i]->id),
                //    &(pIndexName[i]->nGroup),TRUE,nTelID,pItem);
                
                Item_Erase(pItem);
            }
        }
    }

    SendMessage(hLst,LB_SETCURSEL,0,0);

    WaitWinWithTimer(NULL, FALSE, ML("Waiting..."), IDS_CONTACTS, NULL, NULL,
            NULL, 10000);

    AB_FREE(pData->pId);
    
    pData->nCount = 0;
}
/*********************************************************************\
* Function	   ABGroupsView_OnSetTone
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void ABGroupsView_OnSetTone(HWND hWnd,BOOL bSelect,UINT nLen,char* pszTonePath)
{    
	char szCaption[50];
    GROUPVIEW_DATA* pData;
    
    pData = GetUserData(hWnd);

	if(bSelect == FALSE)
		return;

    AB_ReWriteGroupTone(pData->hWnd,pData->pGroup->Info.nGroupID,nLen,pszTonePath);
    
	szCaption[0] = 0;
	GetWindowText(GetParent(hWnd),szCaption,49);
    szCaption[49] = 0;	
	PLXTipsWin(NULL,NULL,0,IDS_TONECHANGED,szCaption,Notify_Success,IDS_OK,NULL,WAITTIMEOUT);
	
}

/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
DWORD AB_GetUnusedGroupID(void)
{
    DWORD id;

    id = AB_GetGroupID();

    id++;

    AB_SetGroupID(id);

    return id;
}
/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
void AB_SetGroupID(DWORD id)
{
    idGroup = id;

    AB_SaveGroupID();
}
/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
DWORD AB_GetGroupID(void)
{
    return idGroup;
}
/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
void AB_LoadGroupID(void)
{
    char szOldPath[PATH_MAXLEN];
    int  f;
    DWORD id;

    szOldPath[0] = 0;

    getcwd(szOldPath,PATH_MAXLEN);  
    
    chdir(PATH_DIR_AB);
    
    f = open(PATH_FILE_GROUPID,O_RDONLY);

    if( f == -1 )
        AB_SetGroupID(0);
    else
    {
        read(f,&id,sizeof(DWORD));

        AB_SetGroupID(id);

        close(f);
    }
    
    chdir(szOldPath);
}
/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL AB_SaveGroupID(void)
{
    char szOldPath[PATH_MAXLEN];
    int  f;
    DWORD id;

    szOldPath[0] = 0;

    getcwd(szOldPath,PATH_MAXLEN);  
    
    chdir(PATH_DIR_AB);
    
    f = open(PATH_FILE_GROUPID, O_RDWR | O_CREAT,S_IRWXU);
 
    if( f == -1 )
    {
        chdir(szOldPath);
        
        return FALSE;
    }
    else
    {
        id = AB_GetGroupID();

        write(f,&id,sizeof(DWORD));

        close(f);
    }
    
    chdir(szOldPath);

    return TRUE;
}
/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void ABGroupsView_OnDataChangeContact(HWND hWnd,int nPos,int nMode)
{
    HWND hList;
//    char szButText[20],szMenu[20];
    GROUPVIEW_DATA* pData;
    int  i,insert,nCurSel;
    
    pData = GetUserData(hWnd);
    
    hList = GetDlgItem(hWnd,IDC_ABGROUPSVIEW_LIST);
    
    switch(nMode)
    {
    case AB_MODE_INSERT:
        if(pIndexName[nPos]->nGroup == pData->pGroup->Info.nGroupID)
        {
            SendMessage(hList,LB_RESETCONTENT,0,0);

            SendMessage(hList,LB_ADDSTRING,-1,(LPARAM)IDS_ADDMEMBER);
            
            SendMessage(hList,LB_SETIMAGE,MAKEWPARAM(IMAGE_BITMAP, 0),(LPARAM)pData->hAddMember);

            for(i = 1; i <= nName ; i++)
            {
                if(pData->pGroup->Info.nGroupID == pIndexName[i]->nGroup)
                {
                    insert = SendMessage(hList,LB_ADDSTRING,-1,(LPARAM)AB_GetNameString(pIndexName[i]));
                    SendMessage(hList,LB_SETITEMDATA,insert,(LPARAM)pIndexName[i]->id);
                    SendMessage(hList,LB_SETIMAGE,MAKEWPARAM(IMAGE_BITMAP, insert),(LPARAM)pData->hMember);
                }

                if(nPos == i)
                    nCurSel = insert;  
            }
                
            SendMessage(hList,LB_SETCURSEL,nCurSel,0);
        }                
        /*
        memset(szButText,0,20);
                
                memset(szMenu,0,20);
                
                SendMessage(pData->hFrameWnd, PWM_GETBUTTONTEXT, 1, (LPARAM)szButText);
                
                SendMessage(pData->hFrameWnd, PWM_GETBUTTONTEXT, 2, (LPARAM)szMenu);
                
                insert = SendMessage(hList,LB_GETCURSEL,0,0);
        
                if(insert == 0)
                {  
                    if(strcmp(szButText,IDS_OPEN) == 0)
                        SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,1, (LPARAM)"");
                    
                    if(strcmp(szMenu,ICON_SELECT) != 0)
                        SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,2, (LPARAM)ICON_SELECT);
                }
                else
                {
                    if(strcmp(szButText,IDS_OPEN) != 0)
                        SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,1, (LPARAM)IDS_OPEN);
                    
                    if(strcmp(szMenu,ICON_OPTIONS) != 0)
                        SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,2, (LPARAM)ICON_OPTIONS);
                }*/
        
        break;
        
    case AB_MODE_DELETE:
		{
			int nCount, i;
			DWORD id;

			nCount = SendMessage(hList,LB_GETCOUNT,0,0);
			for(i=1; i<nCount; i++)
			{
				id = SendMessage(hList,LB_GETITEMDATA,i,0);
				if(pIndexName[nPos]->id == id)
				{
					SendMessage(hList, LB_DELETESTRING, i, 0);

					if(i == nCount - 1)
						SendMessage(hList,LB_SETCURSEL, i-1, 0);
					else
						SendMessage(hList, LB_SETCURSEL, i, 0);
					break;
				}
			}
			
		}
        break;
        
    default:
        break;
    }
}
/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void ABGroupsView_OnRemoveMember(HWND hWnd,BOOL bRemove)
{
    HWND    hLst;
	int     nIndex,nTelID;
    DWORD   id;
    MEMORY_NAME *p;
	CONTACT_ITEMCHAIN *pItem;
    char    szTitle[50];
    GROUPVIEW_DATA *pData;

    pData = GetUserData(hWnd);
    
    if(bRemove == FALSE)
    {
        return;
    }
       
    hLst = GetDlgItem(hWnd,IDC_ABGROUPSVIEW_LIST);
        
    nIndex = SendMessage(hLst,LB_GETCURSEL,0,0);

    if(nIndex == LB_ERR || nIndex == 0)
        return;

    id = (DWORD)SendMessage(hLst,LB_GETITEMDATA,nIndex,0);
            
    p = Memory_Find((PVOID)id,AB_FIND_ID);

    if(p == NULL)
        return;
    
    pItem = NULL;
    
    AB_ReadRecord(p->dwoffset,&pItem,&nTelID);
    
    p->nGroup = -1;
    
    AB_SaveRecord(&(p->dwoffset),&(p->id),&(p->nGroup),TRUE,nTelID,pItem);
    
    Item_Erase(pItem);
    
    SendMessage(hLst,LB_DELETESTRING,nIndex,0);

    if(nIndex == SendMessage(hLst,LB_GETCOUNT,0,0))
        SendMessage(hLst,LB_SETCURSEL,nIndex-1,0);
    else
        SendMessage(hLst,LB_SETCURSEL,nIndex,0);

    szTitle[0] = 0;

    GetWindowText(pData->hFrameWnd,szTitle,49);

    szTitle[49] = 0;

    PLXTipsWin(NULL,NULL,0,IDS_REMOVED,szTitle,Notify_Success,IDS_OK,NULL,WAITTIMEOUT);
    
}
/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void ABGroupsView_OnRemoveAll(HWND hWnd,BOOL bRemoveAll)
{    
    HWND    hLst;
    int     i,nCount;
	int     nTelID;
    DWORD   id;
    MEMORY_NAME *p;
	CONTACT_ITEMCHAIN *pItem;
    char    szTitle[50];
    GROUPVIEW_DATA *pData;

    pData = GetUserData(hWnd);
    
    if(bRemoveAll == FALSE)
    {
        return;
    }
       
    hLst = GetDlgItem(hWnd,IDC_ABGROUPSVIEW_LIST);

    nCount = SendMessage(hLst,LB_GETCOUNT,0,0);
    
	WaitWindowStateEx(NULL, TRUE, IDS_REMOVING, NULL, "", "");
    for(i = nCount-1 ; i >= 1  ; i--)
    {   
        id = (DWORD)SendMessage(hLst,LB_GETITEMDATA,i,0);
        
        p = Memory_Find((PVOID)id,AB_FIND_ID);
        
        if(p == NULL)
		{
			WaitWindowStateEx(NULL, FALSE, IDS_REMOVING, NULL, "", "");
            return;
		}
        
        pItem = NULL;
        
        AB_ReadRecord(p->dwoffset,&pItem,&nTelID);
        
        p->nGroup = -1;
        
        AB_SaveRecord(&(p->dwoffset),&(p->id),&(p->nGroup),TRUE,nTelID,pItem);
        
        Item_Erase(pItem);
        
        SendMessage(hLst,LB_DELETESTRING,i,0);
        
        if(i == SendMessage(hLst,LB_GETCOUNT,0,0))
            SendMessage(hLst,LB_SETCURSEL,i-1,0);
        else
            SendMessage(hLst,LB_SETCURSEL,i,0);
    }

    szTitle[0] = 0;

    GetWindowText(pData->hFrameWnd,szTitle,49);

    szTitle[49] = 0;

	WaitWindowStateEx(NULL, FALSE, IDS_REMOVING, NULL, "", "");
	
    PLXTipsWin(NULL,NULL,0,IDS_REMOVED,szTitle,Notify_Success,IDS_OK,NULL,WAITTIMEOUT);
    
}
/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void ABGroupsView_OnRemoveSel(HWND hWnd,BOOL bSelect,int nCount,DWORD* pnId)
{   
    HWND    hLst;
    int     i,nMax,j;
    DWORD   id;
    MEMORY_NAME *p;
    char    szTitle[50];
    GROUPVIEW_DATA* pData;
    char* pszName;
    char  szPrompt[125];
    HDC   hdc;
    char  szCaption[50];
        
    pData = GetUserData(hWnd);

	if(bSelect == FALSE || (bSelect == TRUE && nCount == 0))
	{
		AB_FREE(pData->pId);

        pData->nCount = 0;

		return;
	}
   
    pData->nCount = nCount;

    hLst = GetDlgItem(hWnd,IDC_ABGROUPSVIEW_LIST);
        
    nMax = SendMessage(hLst,LB_GETCOUNT,0,0);
    
    if(nCount == 1)
    {
        for(i = nMax - 1,j = nCount-1 ; i >= 0 && j >= 0 ; i--)
        {
        
            id = (DWORD)SendMessage(hLst,LB_GETITEMDATA,i,0);

            if(pData->pId[j] == id)
            {
                p = Memory_Find((PVOID)id,AB_FIND_ID);
                
                if(p == NULL)
                    return;

                pszName = AB_GetNameString(p);
                
                szPrompt[0] = 0;
                
                hdc = GetDC(hWnd);
                
                GetExtentFittedText(hdc,pszName,-1,szCaption,50,TEXT_WIDTH,OMIT_SUFFIX,SUFFIX_REPEAT);
                
                ReleaseDC(hWnd,hdc);
                
                sprintf(szPrompt,"%s:\r\n%s?",szCaption,IDS_REMOVE);
                
                PLXConfirmWinEx(pData->hFrameWnd,hWnd,szPrompt, Notify_Request, pszName, IDS_YES, IDS_NO,WM_SUREREMOVESEL);    

                return;
            }
        }
    }
    else
    {
        GetWindowText(pData->hFrameWnd,szTitle,49);
        
        szTitle[49] = 0;
        
        PLXConfirmWinEx(pData->hFrameWnd,hWnd,IDS_REMOVESELECT, Notify_Request, szTitle, IDS_YES, IDS_NO,WM_SUREREMOVESEL);   
    } 
}
/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void ABGroupsView_OnSureRemoveSel(HWND hWnd,BOOL bSure)
{   
    HWND    hLst;
    int     i,nMax,j;
	int     nTelID;
    DWORD   id;
    MEMORY_NAME *p;
	CONTACT_ITEMCHAIN *pItem;
    char    szTitle[50];
    GROUPVIEW_DATA* pData;
        
    pData = GetUserData(hWnd);
    
    if(bSure == FALSE)
    {
		AB_FREE(pData->pId);

        pData->nCount = 0;

        return;
    }
       
    hLst = GetDlgItem(hWnd,IDC_ABGROUPSVIEW_LIST);

    nMax = SendMessage(hLst,LB_GETCOUNT,0,0);
        
    for(i = nMax-1 , j = pData->nCount-1 ; i >= 1  ; i--)
    {   
        id = (DWORD)SendMessage(hLst,LB_GETITEMDATA,i,0);
        
        if(id == pData->pId[j])
        {
            j--;

            p = Memory_Find((PVOID)id,AB_FIND_ID);
            
            if(p == NULL)
                return;
            
            pItem = NULL;
            
            AB_ReadRecord(p->dwoffset,&pItem,&nTelID);
            
            p->nGroup = -1;
            
            AB_SaveRecord(&(p->dwoffset),&(p->id),&(p->nGroup),TRUE,nTelID,pItem);
            
            Item_Erase(pItem);
            
            SendMessage(hLst,LB_DELETESTRING,i,0);
            
            if(i == SendMessage(hLst,LB_GETCOUNT,0,0))
                SendMessage(hLst,LB_SETCURSEL,i-1,0);
            else
                SendMessage(hLst,LB_SETCURSEL,i,0);
        }
    }

    szTitle[0] = 0;

    GetWindowText(pData->hFrameWnd,szTitle,49);

    szTitle[49] = 0;

    PLXTipsWin(NULL,NULL,0,IDS_REMOVED,szTitle,Notify_Success,IDS_OK,NULL,WAITTIMEOUT);
    
    AB_FREE(pData->pId);
    
    pData->nCount = 0;
}
/*********************************************************************\
* Function	ABGroupsView_OnRefreshList
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
static void ABGroupsView_OnRefreshList(HWND hWnd,char* pszSearch,int nLen,BOOL bExit)
{
	int i,nCount;
	char szMenu[20],szLeft[20];
	HWND hList;
	int insert;
	char* pName;
	DWORD id;
    GROUPVIEW_DATA* pData;
        
    pData = GetUserData(hWnd);

    hList = GetDlgItem(hWnd,IDC_ABGROUPSVIEW_LIST);

	if(bExit)
	{
        if(nLen == 0)
        {   
            SendMessage(hList,LB_RESETCONTENT,0,0);    
            
            SendMessage(hList,LB_ADDSTRING,-1,(LPARAM)IDS_ADDMEMBER);
            
            SendMessage(hList,LB_SETIMAGE,MAKEWPARAM(IMAGE_BITMAP, 0),(LPARAM)pData->hAddMember);

            for(i = 1; i <= nName ; i++)
            {
                if(pData->pGroup->Info.nGroupID == pIndexName[i]->nGroup)
                {
                    insert = SendMessage(hList,LB_ADDSTRING,-1,(LPARAM)AB_GetNameString(pIndexName[i]));
                    SendMessage(hList,LB_SETITEMDATA,insert,(LPARAM)pIndexName[i]->id);
                    SendMessage(hList,LB_SETIMAGE,MAKEWPARAM(IMAGE_BITMAP, insert),(LPARAM)pData->hMember);
                }
            }
            SendMessage(hList,LB_SETCURSEL,0,0);  
            
			SendMessage(pData->hFrameWnd,PWM_GETBUTTONTEXT,2,(LPARAM)szMenu);
			if(strcmp(szMenu,ICON_SELECT) != 0)
				SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,2, (LPARAM)ICON_SELECT);
			
			SendMessage(pData->hFrameWnd,PWM_GETBUTTONTEXT,1,(LPARAM)szLeft);
			if(strcmp(szLeft,"") != 0)
				SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,1, (LPARAM)"");
        }
		else
		{
			insert = SendMessage(hList,LB_GETCURSEL,0,0);

            if(insert == LB_ERR)       
            {   
                SendMessage(hList,LB_RESETCONTENT,0,0);    
                
                SendMessage(hList,LB_ADDSTRING,-1,(LPARAM)IDS_ADDMEMBER);
                
                SendMessage(hList,LB_SETIMAGE,MAKEWPARAM(IMAGE_BITMAP, 0),(LPARAM)pData->hAddMember);

                for(i = 1; i <= nName ; i++)
                {
                    if(pData->pGroup->Info.nGroupID == pIndexName[i]->nGroup)
                    {
                        insert = SendMessage(hList,LB_ADDSTRING,-1,(LPARAM)AB_GetNameString(pIndexName[i]));
                        SendMessage(hList,LB_SETITEMDATA,insert,(LPARAM)pIndexName[i]->id);
                        SendMessage(hList,LB_SETIMAGE,MAKEWPARAM(IMAGE_BITMAP, insert),(LPARAM)pData->hMember);
                    }
                }
                
                SendMessage(hList,LB_SETCURSEL,0,0);        
                
                SendMessage(pData->hFrameWnd,PWM_GETBUTTONTEXT,2,(LPARAM)szMenu);
                if(strcmp(szMenu,ICON_SELECT) != 0)
                    SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,2, (LPARAM)ICON_SELECT);
                
                SendMessage(pData->hFrameWnd,PWM_GETBUTTONTEXT,1,(LPARAM)szLeft);
                if(strcmp(szLeft,"") != 0)
                    SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,1, (LPARAM)"");
            }
			else
			{
				id = SendMessage(hList,LB_GETITEMDATA,insert,0);
				
				SendMessage(hList,LB_RESETCONTENT,0,0);
				
                SendMessage(hList,LB_ADDSTRING,-1,(LPARAM)IDS_ADDMEMBER);
				
                SendMessage(hList,LB_SETIMAGE,MAKEWPARAM(IMAGE_BITMAP, 0),(LPARAM)pData->hAddMember);

				for(i=1 ; i <= nName ; i++)
				{
                    if(pData->pGroup->Info.nGroupID == pIndexName[i]->nGroup)
                    {
                        insert = SendMessage(hList,LB_ADDSTRING,-1,(LPARAM)AB_GetNameString(pIndexName[i]));
                        SendMessage(hList,LB_SETITEMDATA,insert,(LPARAM)pIndexName[i]->id);
                        SendMessage(hList,LB_SETIMAGE,MAKEWPARAM(IMAGE_BITMAP, insert),(LPARAM)pData->hMember);
                        if(id == pIndexName[i]->id)
                        {
                            SendMessage(hList,LB_SETCURSEL,i,0);
                            
                            SendMessage(pData->hFrameWnd,PWM_GETBUTTONTEXT,2,(LPARAM)szMenu);
                            if(strcmp(szMenu,ICON_OPTIONS) != 0)
                                SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,2, (LPARAM)ICON_OPTIONS);
                            
                            SendMessage(pData->hFrameWnd,PWM_GETBUTTONTEXT,1,(LPARAM)szLeft);
                            if(strcmp(szLeft,IDS_OPEN) != 0)
                                SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,1, (LPARAM)IDS_OPEN);
                        }
                    }
				}

				if(SendMessage(hList,LB_GETCURSEL,0,0) == LB_ERR)
                {
                    SendMessage(hList,LB_SETCURSEL,0,0);
                    
                    SendMessage(pData->hFrameWnd,PWM_GETBUTTONTEXT,2,(LPARAM)szMenu);
                    if(strcmp(szMenu,ICON_SELECT) != 0)
                        SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,2, (LPARAM)ICON_SELECT);
                    
                    SendMessage(pData->hFrameWnd,PWM_GETBUTTONTEXT,1,(LPARAM)szLeft);
                    if(strcmp(szLeft,"") != 0)
                        SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,1, (LPARAM)"");
                }
			}
		}
	}
	else
	{
		SendMessage(hList,LB_RESETCONTENT,0,0);
	
        SendMessage(hList,LB_ADDSTRING,-1,(LPARAM)IDS_ADDMEMBER);
		
        SendMessage(hList,LB_SETIMAGE,MAKEWPARAM(IMAGE_BITMAP, 0),(LPARAM)pData->hAddMember);

        for(i=1 ; i <= nName ; i++)
		{
            if(pData->pGroup->Info.nGroupID == pIndexName[i]->nGroup)
            {
                pName = AB_GetNameString(pIndexName[i]);
                if(strstr(pName,pszSearch) == pName)
                {
                    insert = SendMessage(hList,LB_ADDSTRING,-1,(LPARAM)AB_GetNameString(pIndexName[i]));
                    SendMessage(hList,LB_SETITEMDATA,insert,(LPARAM)(pIndexName[i]->id));
                    SendMessage(hList,LB_SETIMAGE,MAKEWPARAM(IMAGE_BITMAP, insert),(LPARAM)pData->hMember);
                }
            }
		}
        nCount = SendMessage(hList,LB_GETCOUNT,0,0);
		
		if(nCount > 1)
		{
			SendMessage(hList,LB_SETCURSEL,1,0);
			
//			SendMessage(pData->hFrameWnd,PWM_GETBUTTONTEXT,2,(LPARAM)szMenu);
//			if(strcmp(szMenu,ICON_OPTIONS) != 0)
//				SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,2, (LPARAM)ICON_OPTIONS);
//			
//			SendMessage(pData->hFrameWnd,PWM_GETBUTTONTEXT,1,(LPARAM)szLeft);
//			if(strcmp(szLeft,IDS_OPEN) != 0)
//				SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,1, (LPARAM)IDS_OPEN);
		}
		else
		{
			SendMessage(hList,LB_SETCURSEL,0,0);
			
//			SendMessage(pData->hFrameWnd,PWM_GETBUTTONTEXT,2,(LPARAM)szMenu);
//			if(strcmp(szMenu,ICON_SELECT) != 0)
//				SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,2, (LPARAM)ICON_SELECT);
//			
//			SendMessage(pData->hFrameWnd,PWM_GETBUTTONTEXT,1,(LPARAM)szLeft);
//			if(strcmp(szLeft,"") != 0)
//				SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,1, (LPARAM)"");
		}
	}
}

BOOL AB_GetGroupToneByID(int id,char* pszTone,int nMaxLen)
{
    int handle;
    char szOldPath[PATH_MAXLEN];
    int ID;
    int nToneLen;
    struct stat buf;
    LONG DataLen; 

    szOldPath[0] = 0;
    
    getcwd(szOldPath,PATH_MAXLEN);  
    
    chdir(PATH_DIR_AB);    
    
    memset(&buf,0,sizeof(struct stat));
    
    stat(PATH_FILE_GROUPTONE,(struct stat*)&buf);

    if(buf.st_size == 0)
    {
        chdir(szOldPath);
        
        return FALSE;
    }
    
    handle = open(PATH_FILE_GROUPTONE,O_RDONLY,S_IRUSR);        
    
    if(handle == -1)
    {
        chdir(szOldPath);    
        
        return FALSE;
    }
    
    DataLen = buf.st_size;
       
    while(DataLen > 0)
    {
        read(handle,&ID,sizeof(int));
        
        read(handle,&nToneLen,sizeof(int));
        
        if(ID == id)
        {
            nToneLen = min(nToneLen,nMaxLen);

            read(handle,pszTone,nToneLen);

            pszTone[nMaxLen-1] = 0;

            close(handle);

            chdir(szOldPath);

            return TRUE;
        }
        
        lseek(handle,nToneLen,SEEK_CUR);
        
        DataLen -= (sizeof(int)*2 + nToneLen);
    }
    
    close(handle);
    
    chdir(szOldPath);    
    
    return FALSE;
}
