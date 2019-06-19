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

typedef struct tagABEDITCREATEDATA
{
    HWND        hFrameWnd;
    HWND        hMsgWnd;
    UINT        uMsgCmd;
    HWND        hViewWnd;
    HMENU       hMenu;
    PCONTACT_ITEMCHAIN pItem;
    DWORD       dwoffset;
    DWORD       id;
    HWND        hFocus;
    int         nCurFocus;
    PVOID       pFocusDataPtr;
    PVOID       pPic;
    PVOID       pTone;
    BOOL        bNew;
    BOOL        bSetText;
    BOOL        bKeyDown;
    int         nMaxCtl;
	int		    nTelID;
	BOOL        bChange;
    BOOL        bChangeName;
    int         nStat[AB_EDITCONTACT_CTRNUM];
}ABEDITCREATEDATA,*PABEDITCREATEDATA;

#define IDC_SAVE                        100
#define IDC_EXIT                        200
#define IDC_ABEDITCONTACT_LIST          300
#define IDC_ABEDITCONTACT_EDIT          301
#define IDM_ABEDIT_ADD                  101
#define IDM_ABEDIT_REMOVE               102
#define IDM_ABEDIT_SETPIC               103
#define IDM_ABEDIT_SETTONE              104

LRESULT ABEditContactWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static BOOL ABEditContact_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct);
static void ABEditContact_OnActivate(HWND hwnd, UINT state);
static void ABEditContact_OnSetFocus(HWND hWnd);
static void ABEditContact_OnPaint(HWND hWnd);
static void ABEditContact_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags);
static void ABEditContact_OnKeyUp(HWND hWnd, UINT vk, int cRepeat, UINT flags);
static LRESULT ABEditContact_OnCommand(HWND hWnd, int id, UINT codeNotify,LPARAM lParam);
static void ABEditContact_OnDestroy(HWND hWnd);
static void ABEditContact_OnClose(HWND hWnd);
static void ABEditContact_OnAddField(HWND hWnd,int nSel);
static void ABEditContact_OnRemoveField(HWND hWnd,BOOL bRemove);
static void ABEditContact_OnSetPic(HWND hWnd,BOOL bSelect,UINT nLen,char* szPicPath);
static void ABEditContact_OnSetTone(HWND hWnd,BOOL bSelect,UINT nLen,char* szTonePath);
static void ABEditContact_OnSave(HWND hWnd,BOOL bSave);
static int  AB_UpdateOffset(DWORD dwoffset,DWORD id);
BOOL AB_GetFullName(CONTACT_ITEMCHAIN* pItem,char* pszFullName);

static int  ABEdit_CreateItem(HWND hWnd);
static BOOL ABEdit_CreateEdit(HWND hWnd);
int  GetIndexByMask(DWORD dwMask);

CONTACT_ITEMCHAIN* Item_New(DWORD dwMask);
int Item_Insert(CONTACT_ITEMCHAIN** ppHeader,CONTACT_ITEMCHAIN* pChainNode);
void Item_Delete(CONTACT_ITEMCHAIN** ppHeader,CONTACT_ITEMCHAIN* pChainNode);
void Item_Erase(CONTACT_ITEMCHAIN* pHeader);

int AB_Insert2Index(PVOID* Array, int start, int end, int index,CMPFUNC cmpf);
int AB_Insert2Table(DWORD dwoffset,DWORD id,int nGroup,CONTACT_ITEMCHAIN *pItem);

BOOL AB_CreateAddFieldWindow(HWND hParent,HWND hMsgWnd,UINT uMsgCmd,int nStat[]);

BOOL AB_EditRegisterClass(void);
BOOL CopyItemChain(PCONTACT_ITEMCHAIN* ppHeader,CONTACT_ITEMCHAIN* pItemChain,PVOID *ppFocusPtr,PVOID pFocus);

/*********************************************************************\
* Function	   AB_EditRegisterClass
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL AB_EditRegisterClass(void)
{
    WNDCLASS wc;
        
    wc.style         = 0;
    wc.lpfnWndProc   = ABEditContactWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = sizeof(ABEDITCREATEDATA);
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName  = "ABEditContactWndClass";
    
    if (!RegisterClass(&wc))
        return FALSE;

    return TRUE;
}
/*********************************************************************\
* Function	   AB_CreateEditContactWnd
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL AB_CreateEditContactWnd(HWND hFrameWnd,HWND hMsgWnd,UINT uMsgCmd,CONTACT_ITEMCHAIN* pItemData,
                             PVOID pFocusPtr,BOOL bNew,DWORD dwoffset,DWORD id,int nTelID,HWND hViewWnd)
{
    ABEDITCREATEDATA Data;
    HWND        hEditWnd = NULL;    
    const char* pszCaption;
    RECT        rcCliet;
	char        szFullName[AB_MAXLEN_FIRSTNAME*2];

    memset(&Data,0,sizeof(ABEDITCREATEDATA));

    if(CopyItemChain(&(Data.pItem),pItemData,&(Data.pFocusDataPtr),pFocusPtr) == FALSE)
        return FALSE;

    Data.hFrameWnd = hFrameWnd;
    Data.hMsgWnd = hMsgWnd;
    Data.uMsgCmd = uMsgCmd;
    Data.hViewWnd = hViewWnd;
    Data.nMaxCtl = 0;
    Data.bNew = bNew;
    Data.dwoffset = dwoffset;
    Data.id = id;
    if(hMsgWnd != NULL)
        Data.bChange = TRUE;
    else
        Data.bChange = FALSE;
	Data.nTelID = nTelID;

    if(bNew)
        pszCaption = IDS_NEWCONTACT;
    else
        pszCaption = "";

    GetClientRect(hFrameWnd,&rcCliet);

    Data.hMenu = CreateMenu();    

    hEditWnd = CreateWindow(
        "ABEditContactWndClass", 
        "",
        WS_VISIBLE | WS_CHILD,
        rcCliet.left,
        rcCliet.top,
        rcCliet.right - rcCliet.left,
        rcCliet.bottom - rcCliet.top,
        hFrameWnd,
        NULL,
        NULL, 
        (PVOID)&Data
        );
    
    if (!hEditWnd)
    {
		DestroyMenu(Data.hMenu);

        Item_Erase(Data.pItem);

        return FALSE;
    }

    
    {
        HWND hList;
    
        ABEDITCREATEDATA *pData;

        int nPos;

        CONTACT_ITEMCHAIN *pItem;

        pData = GetUserData(hEditWnd);

        hList = GetDlgItem(hEditWnd,IDC_ABEDITCONTACT_LIST);
        
        pItem = pData->pItem;
            
        nPos = 0;
        
        while(pItem)
        {            
            if(pData->pFocusDataPtr == pItem)
            {            
                SendMessage(hList,LB_SETCURSEL,nPos,NULL);
                
                pData->hFocus = hList;
                
                pData->nCurFocus = nPos;
            }
            
            pItem = pItem->pNext;
            
            nPos++;
            
        }
    }
    

    SetFocus(hEditWnd);
    
    ABEdit_CreateEdit(hEditWnd);

//    SetWindowText(hFrameWnd,pszCaption);
	if(!bNew)
	{
		AB_GetFullName(Data.pItem, szFullName);
        
		SetWindowText(hFrameWnd, szFullName);
	}
	else
		SetWindowText(hFrameWnd, pszCaption);

    AppendMenu(Data.hMenu, MF_ENABLED, IDM_ABEDIT_ADD, IDS_ADDFIELD);
    AppendMenu(Data.hMenu, MF_ENABLED, IDM_ABEDIT_REMOVE, IDS_REMOVEFIELD);
    AppendMenu(Data.hMenu, MF_ENABLED, IDM_ABEDIT_SETPIC, IDS_SETPIC);
    AppendMenu(Data.hMenu, MF_ENABLED, IDM_ABEDIT_SETTONE, IDS_SETTONE);
    
    PDASetMenu(hFrameWnd,Data.hMenu);

//    SendMessage(hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_EXIT,0), (LPARAM)IDS_CANCEL);  
    SendMessage(hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_SAVE,1), (LPARAM)IDS_SAVE);  
    SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_OPTIONS);
    
    SendMessage(hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON),(LPARAM)NULL);

    ShowWindow(hEditWnd,SW_SHOW);
    UpdateWindow(hEditWnd);

    return TRUE;
}

/*********************************************************************\
* Function	ABEditContactWndProc
* Purpose   Main window proc
* Params
*			hWnd: Handle of the window
*			wMsgCmd: Message ID
* Return
*			TRUE: Success
*			FALSE: Fail
* Remarks
**********************************************************************/
static LRESULT ABEditContactWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    LRESULT lResult = TRUE;

	switch (wMsgCmd)
    {
    case WM_CREATE:
        lResult = ABEditContact_OnCreate(hWnd,(LPCREATESTRUCT)(lParam));
        break;

    case WM_ACTIVATE:
    case PWM_SHOWWINDOW:
        ABEditContact_OnActivate(hWnd,(UINT)LOWORD(wParam));
        break;

    case WM_SETFOCUS:
        ABEditContact_OnSetFocus(hWnd);
        break;
        
    case WM_PAINT:
        ABEditContact_OnPaint(hWnd);
        break;

    case WM_KEYDOWN:
        ABEditContact_OnKey(hWnd,(UINT)(wParam),(int)(short)LOWORD(lParam),(UINT)HIWORD(lParam));
        break;

    case WM_KEYUP:
        ABEditContact_OnKeyUp(hWnd,(UINT)(wParam),(int)(short)LOWORD(lParam),(UINT)HIWORD(lParam));
        break;

    case WM_COMMAND:
        lResult = ABEditContact_OnCommand(hWnd,(int)(LOWORD(wParam)),(UINT)HIWORD(wParam),(LPARAM)lParam);
        break;

    case WM_CLOSE:
        ABEditContact_OnClose(hWnd);
        break;

    case WM_DESTROY:
        ABEditContact_OnDestroy(hWnd);
        break;

    case WM_SETPIC:
        ABEditContact_OnSetPic(hWnd,(BOOL)(HIWORD(wParam)),(UINT)LOWORD(wParam),(char*)lParam);
        break;

    case WM_SETTONE:
        ABEditContact_OnSetTone(hWnd,(BOOL)(HIWORD(wParam)),(UINT)LOWORD(wParam),(char*)lParam);
        break;

    case WM_ADDFIELD:
        ABEditContact_OnAddField(hWnd,(int)wParam);
        break;

    case WM_REMOVEFIELD:
        ABEditContact_OnRemoveField(hWnd,(BOOL)lParam);
        break;

	case WM_SAVECONTACT:
        ABEditContact_OnSave(hWnd,(BOOL)lParam);
		break;

    default:     
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
	}

    return lResult;

}
/*********************************************************************\
* Function	ABEditContact_OnCreate
* Purpose   WM_CREATE message handler of the main window
* Params
*			hWnd: Handle of the window
*			lpCreateStruct: Create Structure
* Return
*			TRUE: Success
*			FALSE: Fail
* Remarks
**********************************************************************/
static BOOL ABEditContact_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
{    
    ABEDITCREATEDATA *pCreateData;

    pCreateData = (ABEDITCREATEDATA*)GetUserData(hWnd);

    memcpy(pCreateData,(ABEDITCREATEDATA*)(lpCreateStruct->lpCreateParams),sizeof(ABEDITCREATEDATA));

    if(ABEdit_CreateItem(hWnd) == FALSE)
        return FALSE;
        
    return TRUE;
    
}
/*********************************************************************\
* Function	ABEditContact_OnActivate
* Purpose   WM_ACTIVATE message handler of the main window
* Params
* Return    None
* Remarks
**********************************************************************/
static void ABEditContact_OnActivate(HWND hWnd, UINT state)
{
    char szFullName[AB_MAXLEN_FIRSTNAME*2];
    ABEDITCREATEDATA *pCreateData;

    pCreateData = (ABEDITCREATEDATA*)GetUserData(hWnd);

    PDASetMenu(pCreateData->hFrameWnd,pCreateData->hMenu);

//    SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_CANCEL);  
    SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_SAVE);  
    SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_OPTIONS);
    SendMessage(pCreateData->hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON),(LPARAM)NULL);
    SetFocus(pCreateData->hFocus);

    if( pCreateData->bNew == FALSE )
    {
        szFullName[0] = 0;
        
        AB_GetFullName(pCreateData->pItem,szFullName);
        
        SetWindowText(pCreateData->hFrameWnd,szFullName);
    }
    else
    {
        SetWindowText(pCreateData->hFrameWnd,IDS_NEWCONTACT);
    }

    return;
}
/*********************************************************************\
* Function	ABEditContact_OnSetFocus
* Purpose   WM_SETFOCUS message handler of the main window
* Params
* Return    None
* Remarks
**********************************************************************/
static void ABEditContact_OnSetFocus(HWND hWnd)
{
    ABEDITCREATEDATA *pCreateData;

    pCreateData = (ABEDITCREATEDATA*)GetUserData(hWnd);

    SetFocus(pCreateData->hFocus);
}
/*********************************************************************\
* Function	ABEditContact_OnPaint
* Purpose   WM_PAINT message handler of the main window
* Params	hWnd: Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void ABEditContact_OnPaint(HWND hWnd)
{
    PAINTSTRUCT ps;

	HDC hdc = BeginPaint(hWnd, &ps);

	EndPaint(hWnd, &ps);

	return;
}

/*********************************************************************\
* Function	ABEditContact_OnKey
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
static void ABEditContact_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags)
{
    HWND hLst = NULL;
    ABEDITCREATEDATA *pCreateData;
    int nIndex;
    CONTACT_ITEMCHAIN *pData = NULL;
	char szTitle[50];

    pCreateData = (ABEDITCREATEDATA*)GetUserData(hWnd);

	switch (vk)
	{
	case VK_F10:
        szTitle[0] = 0;

		GetWindowText(pCreateData->hFrameWnd,szTitle,49);

        szTitle[49] = 0;

		if(pCreateData->bNew && pCreateData->bChange)
		{
			PLXConfirmWinEx(pCreateData->hFrameWnd,hWnd,IDS_SAVECONTACT, Notify_Request, szTitle, 
                IDS_YES, IDS_NO,WM_SAVECONTACT);
			break;
		}
        else if(pCreateData->bChange)
		{
			PLXConfirmWinEx(pCreateData->hFrameWnd,hWnd,IDS_SAVECHANGES, Notify_Request, szTitle, 
                IDS_YES, IDS_NO,WM_SAVECONTACT);
			break;
		}
        else
        {
            SendMessage(pCreateData->hMsgWnd,pCreateData->uMsgCmd,FALSE,NULL);

            PostMessage(hWnd,WM_CLOSE,0,0);
        }

        break;

    case VK_F5:
		PDADefWindowProc(pCreateData->hFrameWnd, WM_KEYDOWN, vk, MAKELPARAM(cRepeat, flags));
        break;

    case VK_RETURN:
        ABEditContact_OnSave(hWnd,TRUE);
        break;

    case VK_UP:
        hLst = GetDlgItem(hWnd,IDC_ABEDITCONTACT_LIST);
        
        nIndex = SendMessage(hLst,LB_GETCURSEL,0,0);
        
        if(nIndex == 0)
            break;

        pData = (CONTACT_ITEMCHAIN*)SendMessage(hLst,LB_GETITEMDATA,nIndex,NULL);

        if(pData->dwMask == AB_TYPEMASK_DATE)
        {
            char szTime[50];
            
            char szDate[50];

            szTime[0] = 0;

            szDate[0] = 0;

            GetTimeDisplay(*((SYSTEMTIME*)(pData->pszData)),szTime,szDate);

            SendMessage(hLst, LB_SETAUXTEXT, MAKEWPARAM(nIndex, -1), (LPARAM)szDate);
        }
        else
            SendMessage(hLst, LB_SETAUXTEXT, MAKEWPARAM(nIndex, -1), (LPARAM)(pData->pszData));
     
        pCreateData->pFocusDataPtr = (CONTACT_ITEMCHAIN*)SendMessage(hLst,LB_GETITEMDATA,nIndex-1,NULL);

        pCreateData->bKeyDown = TRUE;
        
        SendMessage(hLst,WM_KEYDOWN,vk,MAKELPARAM(cRepeat, flags));
        break;

    case VK_DOWN:    
        hLst = GetDlgItem(hWnd,IDC_ABEDITCONTACT_LIST);

        nIndex = SendMessage(hLst,LB_GETCURSEL,0,0);
        
        if((nIndex+1) == SendMessage(hLst,LB_GETCOUNT,0,0))
            break;

        pData = (CONTACT_ITEMCHAIN*)SendMessage(hLst,LB_GETITEMDATA,nIndex,NULL);
        
        if(pData->dwMask == AB_TYPEMASK_DATE)
        {
            char szTime[50];
            
            char szDate[50];
            
            szTime[0] = 0;
            
            szDate[0] = 0;
            
            GetTimeDisplay(*((SYSTEMTIME*)(pData->pszData)),szTime,szDate);
            
            SendMessage(hLst, LB_SETAUXTEXT, MAKEWPARAM(nIndex, -1), (LPARAM)szDate);
        }
        else
            SendMessage(hLst, LB_SETAUXTEXT, MAKEWPARAM(nIndex, -1), (LPARAM)(pData->pszData));
        
        pCreateData->pFocusDataPtr = (CONTACT_ITEMCHAIN*)SendMessage(hLst,LB_GETITEMDATA,nIndex+1,NULL);
        
        pCreateData->bKeyDown = TRUE;

        SendMessage(hLst,WM_KEYDOWN,vk,MAKELPARAM(cRepeat, flags));
        break;

	default:
		PDADefWindowProc(hWnd, WM_KEYDOWN, vk, MAKELPARAM(cRepeat, flags));
		break;
	}

	return;
}
/*********************************************************************\
* Function	ABEditContact_OnKeyUp
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
static void ABEditContact_OnKeyUp(HWND hWnd, UINT vk, int cRepeat, UINT flags)
{
    HWND hLst = NULL;
    ABEDITCREATEDATA *pCreateData;

	switch (vk)
	{
    case VK_UP:
    case VK_DOWN:  
        pCreateData = GetUserData(hWnd);

        if(pCreateData->bKeyDown == FALSE)
            break;
        
        pCreateData->bKeyDown = FALSE;

        hLst = GetDlgItem(hWnd,IDC_ABEDITCONTACT_LIST);
        
        SendMessage(hLst,WM_KEYUP,vk,MAKELPARAM(cRepeat, flags));
        break;

	default:
		PDADefWindowProc(hWnd, WM_KEYUP, vk, MAKELPARAM(cRepeat, flags));
		break;
	}

	return;
}
/*********************************************************************\
* Function	ABEditContact_OnCommand
* Purpose   WM_COMMAND message handler of the main window
* Params
*			hWnd:	Handle of the window
*			id:		Id of command message
*			hwndCtl: Handle of the window which sended this message
*			codeNotify:Notify code of the message
* Return	None
* Remarks
**********************************************************************/
static LRESULT ABEditContact_OnCommand(HWND hWnd, int id, UINT codeNotify,LPARAM lParam)
{
    ABEDITCREATEDATA *pCreateData;

    pCreateData = (ABEDITCREATEDATA*)GetUserData(hWnd);

	switch(id)
	{
    case IDC_ABEDITCONTACT_LIST:
        if(codeNotify == LBN_SELCHANGE)
        {
            ABEdit_CreateEdit(hWnd);
        }
        else if(codeNotify == LBN_SETFONT)
        {            
            HFONT hFont = NULL;
            
            GetFontHandle(&hFont, (lParam == 0) ? SMALL_FONT : LARGE_FONT);
            
            return (LRESULT)hFont;
        }
        else if(codeNotify == LBN_SETFOCUS)
        {
            SetFocus(GetDlgItem(hWnd,IDC_ABEDITCONTACT_EDIT));
        }
        break;

    case IDC_ABEDITCONTACT_EDIT:
        if(codeNotify == EN_CHANGE)
        {
            HWND hEdt;
            CONTACT_ITEMCHAIN *pNode;
            char szFullName[AB_MAXLEN_FIRSTNAME*2];

            hEdt = GetDlgItem(hWnd,IDC_ABEDITCONTACT_EDIT);
            
            pNode = (CONTACT_ITEMCHAIN*)GetWindowLong(hEdt,GWL_USERDATA);
            
            if(pNode->dwMask == AB_TYPEMASK_DATE)
                SendMessage(hEdt,TEM_GETTIME,0,(LPARAM)pNode->pszData);
            else
                GetWindowText(hEdt,pNode->pszData,pNode->nMaxLen);


            if(pCreateData->bSetText == TRUE)
                pCreateData->bSetText = FALSE;
            else
            {
                if(pNode->dwMask == AB_TYPEMASK_FIRSTNAME || pNode->dwMask == AB_TYPEMASK_LASTNAME)
                {
                    pCreateData->bChangeName = TRUE;
                }
                pNode->bChange = TRUE;
				if(pCreateData->bChange == FALSE)
					pCreateData->bChange = TRUE;
			}

            if( pCreateData->bChangeName == TRUE 
				&& pCreateData->bNew == FALSE )
            {
                if(pNode->dwMask == AB_TYPEMASK_FIRSTNAME || pNode->dwMask == AB_TYPEMASK_LASTNAME)
                {
                    szFullName[0] = 0;
                    
                    AB_GetFullName(pCreateData->pItem,szFullName);
                    
                    SetWindowText(pCreateData->hFrameWnd,szFullName);
                }
            }
            else if(pCreateData->bChangeName == TRUE)
                SetWindowText(pCreateData->hFrameWnd,IDS_NEWCONTACT);
        }
        else if(codeNotify == EN_KILLFOCUS)
        {
            HWND hLst;
            CONTACT_ITEMCHAIN* pData;
            int  nIndex;
            HWND hEdt;
            CONTACT_ITEMCHAIN *pNode;

            hEdt = GetDlgItem(hWnd,IDC_ABEDITCONTACT_EDIT);
            
            pNode = (CONTACT_ITEMCHAIN*)GetWindowLong(hEdt,GWL_USERDATA);
            
            if(pNode->dwMask == AB_TYPEMASK_DATE)
                SendMessage(hEdt,TEM_GETTIME,0,(LPARAM)pNode->pszData);
            else
                GetWindowText(hEdt,pNode->pszData,pNode->nMaxLen);

            hLst = GetDlgItem(hWnd,IDC_ABEDITCONTACT_LIST);
                                 
            pData = pCreateData->pItem;
            
            nIndex = 0;

            while(pData)
            {
                if(pData != pNode)
                {
                    pData = pData->pNext;
                    
                    nIndex++;

                    continue;
                }

                if(pData->dwMask == AB_TYPEMASK_DATE)
                {
                    char szTime[50];
                    
                    char szDate[50];
                    
                    szTime[0] = 0;
                    
                    szDate[0] = 0;
                    
                    GetTimeDisplay(*((SYSTEMTIME*)(pData->pszData)),szTime,szDate);
                    
                    SendMessage(hLst, LB_SETAUXTEXT, MAKEWPARAM(nIndex, -1), (LPARAM)szDate);
                }
                else
                    SendMessage(hLst, LB_SETAUXTEXT, MAKEWPARAM(nIndex, -1), (LPARAM)(pData->pszData));

                nIndex++;

                pData = pData->pNext;
            }
        }
        break;

    case IDM_ABEDIT_ADD:
        {
            char szCaption[AB_MAXLEN_FIRSTNAME];
            int  i;
            ABEDITCREATEDATA    *pCreateData;
            CONTACT_ITEMCHAIN   *pTemp;
            int  index;

            pCreateData = (ABEDITCREATEDATA*)GetUserData(hWnd);

            szCaption[0] = 0;

            GetWindowText(pCreateData->hFrameWnd,szCaption,AB_MAXLEN_FIRSTNAME);
            
            for(i = 0; i < AB_EDITCONTACT_CTRNUM ; i++)
                pCreateData->nStat[i] = 1;

            pTemp = pCreateData->pItem;

            while(pTemp)
            {
                if(pTemp->dwMask != AB_TYPEMASK_PIC 
                    && pTemp->dwMask != AB_TYPEMASK_TONE)
                {
                    index = GetIndexByMask(pTemp->dwMask);
                    
                    if(Contact_Item[index].bOnly == TRUE)
                        pCreateData->nStat[index] = 0;
                }

                pTemp = pTemp->pNext;
            }

            AB_CreateAddFieldWindow(pCreateData->hFrameWnd,hWnd,WM_ADDFIELD,pCreateData->nStat);
        }
        break;

    case IDM_ABEDIT_REMOVE:
        {
            HWND hLst = NULL;
            int  nPos;
            HDC  hdc;
            CONTACT_ITEMCHAIN   *pData = NULL,*pPioneerData = NULL;
            int                 index;
            char                szTitle[50];
            char                szPrompt[100];
            char                szCaption[50];

            hLst = GetDlgItem(hWnd,IDC_ABEDITCONTACT_LIST);

            nPos = SendMessage(hLst,LB_GETCURSEL,0,0);

            if(nPos == LB_ERR)
                break;

            GetWindowText(pCreateData->hFrameWnd,szTitle,49);

            szTitle[49] = 0;

            pData = (CONTACT_ITEMCHAIN*)SendMessage(hLst,LB_GETITEMDATA,nPos,0);

            if(pData->dwMask == AB_TYPEMASK_FIRSTNAME || pData->dwMask == AB_TYPEMASK_LASTNAME)
            {
                PLXTipsWin(NULL,NULL,0,IDS_NOTALLOWED,szTitle,Notify_Alert,IDS_OK,NULL,WAITTIMEOUT);
                break;
            }

            if(pData->dwMask == AB_TYPEMASK_TEL || pData->dwMask == AB_TYPEMASK_MOBILE)
            {
                pPioneerData = (CONTACT_ITEMCHAIN*)SendMessage(hLst,LB_GETITEMDATA,nPos-1,0);

                if(pPioneerData->dwMask != pData->dwMask)
                {
                    PLXTipsWin(NULL,NULL,0,IDS_NOTALLOWED,szTitle,Notify_Alert,IDS_OK,NULL,WAITTIMEOUT);
                    break;
                }
            }

            index = GetIndexByMask(pData->dwMask);

            if(index == -1)
                break;
            
            hdc = GetDC(hWnd);
            
            GetExtentFittedText(hdc,Contact_Item[index].pszCpation,-1,szCaption,50,TEXT_WIDTH,OMIT_SUFFIX,SUFFIX_REPEAT);
            
            ReleaseDC(hWnd,hdc);

            szPrompt[0] = 0;

            sprintf(szPrompt,"%s:\r\n %s?",szCaption,IDS_REMOVE);
            
            PLXConfirmWinEx(GetParent(hWnd),hWnd,szPrompt, Notify_Request, szTitle, IDS_YES, IDS_NO,WM_REMOVEFIELD);                        
        }
        break;

    case IDM_ABEDIT_SETPIC:
        {        
			CONTACT_ITEMCHAIN* pItem = NULL;
			CONTACT_ITEMCHAIN* pTempItem = NULL;
            char szCaption[AB_MAXLEN_FIRSTNAME];
            
            szCaption[0] = 0;
            
            GetWindowText(pCreateData->hFrameWnd,szCaption,AB_MAXLEN_FIRSTNAME);

			pTempItem = pCreateData->pItem;

			while(pTempItem)
			{
				if(pTempItem->dwMask & AB_TYPEMASK_PIC)
				{
					pItem = pTempItem;
					break;
				}
				pTempItem = pTempItem->pNext;
			}
			
			if(pItem != NULL)
				APP_PreviewPictureEx(pCreateData->hFrameWnd,hWnd,WM_SETPIC,szCaption,IDS_NOPICTURE, pItem->pszData);
			else
				APP_PreviewPictureEx(pCreateData->hFrameWnd,hWnd,WM_SETPIC,szCaption,IDS_NOPICTURE, "");
        }
        break;

    case IDM_ABEDIT_SETTONE:
        //APP_PreviewRing(pCreateData->hFrameWnd,hWnd,WM_SETTONE,(PSTR)IDS_CONTACTTONE);
		{
			HWND hRing;
			CONTACT_ITEMCHAIN* pItem = NULL;
			CONTACT_ITEMCHAIN* pTempItem = NULL;

			pTempItem = pCreateData->pItem;

			while(pTempItem)
			{
				if(pTempItem->dwMask & AB_TYPEMASK_TONE)
				{
					pItem = pTempItem;
					break;
				}
				pTempItem = pTempItem->pNext;
			}

			hRing = PreviewSoundEx(pCreateData->hFrameWnd, hWnd, WM_SETTONE, (PSTR)IDS_CONTACTTONE, TRUE);
			if(hRing)
			{
				SendMessage(hRing, SPM_SETDEFTONE, 0, 0);
				if(pItem != NULL)
					SendMessage(hRing, SPM_SETCURSEL, 0, (LPARAM)pItem->pszData);
			}
		}
		
        break;

    case IDC_ABEDITCONTACT_DATE:
        break;
        
    default:
        break;
	}

	return TRUE;
}
/*********************************************************************\
* Function	ABEditContact_OnDestroy
* Purpose   WM_DESTROY message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void ABEditContact_OnDestroy(HWND hWnd)
{
    ABEDITCREATEDATA    *pCreateData;

    pCreateData = (ABEDITCREATEDATA*)GetUserData(hWnd);

    DestroyMenu(pCreateData->hMenu);

    Item_Erase(pCreateData->pItem);

    return;
}
/*********************************************************************\
* Function	ABEditContact_OnClose
* Purpose   WM_CLOSE message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void ABEditContact_OnClose(HWND hWnd)
{
    ABEDITCREATEDATA    *pCreateData;

    pCreateData = (ABEDITCREATEDATA*)GetUserData(hWnd);

    SendMessage(pCreateData->hFrameWnd,PWM_CLOSEWINDOW,(WPARAM)hWnd,NULL);

    DestroyWindow (hWnd);
	
    return;
}

/*********************************************************************\
* Function	   ABEditContact_OnAddField
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void ABEditContact_OnAddField(HWND hWnd,int nSel)
{              
    CONTACT_ITEMCHAIN   *pNewItem;
    int                 nPos;
    HWND                hLst;
    ABEDITCREATEDATA    *pCreateData;
    char                szCaption[100];
    
    if(nSel < 0)
        return;

    pCreateData = GetUserData(hWnd);

    hLst = GetDlgItem(hWnd,IDC_ABEDITCONTACT_LIST);

    pNewItem = Item_New(Contact_Item[nSel].dwMask);
    
    if(pNewItem == NULL)
        return;

	if(AB_IsTel(pNewItem->dwMask) || AB_IsEmail(pNewItem->dwMask))
	{
		pNewItem->nID = pCreateData->nTelID;

		pCreateData->nTelID += 1;
	}
    
    nPos = Item_Insert(&(pCreateData->pItem),pNewItem);
    
    hLst = GetDlgItem(hWnd,IDC_ABEDITCONTACT_LIST);
    
    szCaption[0] = 0;
    
    sprintf(szCaption,"%s:",Contact_Item[nSel].pszCpation);
    
    SendMessage(hLst,LB_INSERTSTRING,nPos,(LPARAM)szCaption);
    
    if(pNewItem->dwMask == AB_TYPEMASK_DATE)
    {
        char szTime[50];
        
        char szDate[50];
        
        szTime[0] = 0;
        
        szDate[0] = 0;
        
        GetTimeDisplay(*((SYSTEMTIME*)(pNewItem->pszData)),szTime,szDate);
        
        SendMessage(hLst, LB_SETAUXTEXT, MAKEWPARAM(nPos, -1), (LPARAM)szDate);
    }
    else
        SendMessage(hLst,LB_SETAUXTEXT, MAKEWPARAM(nPos, -1), (LPARAM)(pNewItem->pszData));
    
    SendMessage(hLst,LB_SETITEMDATA,nPos,(LPARAM)pNewItem);
    
    SendMessage(hLst,LB_SETCURSEL,nPos,0);
    
    pCreateData->pFocusDataPtr = pNewItem;
    
    pCreateData->hFocus = NULL;
    
    pCreateData->nCurFocus = 0;
    
    if(ABEdit_CreateEdit(hWnd) == FALSE)
        return;                 
}

/*********************************************************************\
* Function	   ABEditContact_OnSetPic
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void ABEditContact_OnSetPic(HWND hWnd,BOOL bSelect,UINT nLen,char* szPicPath)
{
    CONTACT_ITEMCHAIN   *pNewItem;
    ABEDITCREATEDATA    *pCreateData;
    
	pCreateData = GetUserData(hWnd);

	pCreateData->bChange = TRUE;
	
    pNewItem = pCreateData->pItem;

	while(pNewItem)
    {
        if(pNewItem->dwMask & AB_TYPEMASK_PIC)
        {
            Item_Delete(&(pCreateData->pItem),pNewItem);
            
            break;
        }
        pNewItem = pNewItem->pNext;
    }

    if(bSelect == FALSE)
	{
		pCreateData->pPic = NULL;
        return;
	}
	
    if(nLen == 0)
    {
        pCreateData->pPic = NULL;

        return;
    }

    pNewItem = (CONTACT_ITEMCHAIN*)malloc(sizeof(CONTACT_ITEMCHAIN));

    if(pNewItem == NULL)
    {
        pCreateData->pPic = NULL;

        return;
    }

    memset(pNewItem,0,sizeof(CONTACT_ITEMCHAIN));

    pNewItem->pszData = (char*)malloc(nLen+1);

    if(pNewItem->pszData == NULL)
    {
        pCreateData->pPic = NULL;

        free(pNewItem);
        
        return;
    }

    pNewItem->dwMask = AB_TYPEMASK_PIC;

    pNewItem->nMaxLen = nLen + 1;

    pNewItem->bChange = TRUE;
    
    memcpy(pNewItem->pszData,szPicPath,nLen);

    pNewItem->pszData[nLen] = 0;
    
    Item_Insert(&(pCreateData->pItem),pNewItem);
    
    pCreateData->pPic = pNewItem->pszData;
    
}

/*********************************************************************\
* Function	   ABEditContact_OnSetTone
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void ABEditContact_OnSetTone(HWND hWnd,BOOL bSelect,UINT nLen,char* szTonePath)
{
    CONTACT_ITEMCHAIN   *pNewItem;
    ABEDITCREATEDATA    *pCreateData;
    
    if(bSelect == FALSE)
        return;

    pCreateData = GetUserData(hWnd);

	pCreateData->bChange = TRUE;
	
    pNewItem = pCreateData->pItem;

    while(pNewItem)
    {
        if(pNewItem->dwMask & AB_TYPEMASK_TONE)
        {
            Item_Delete(&(pCreateData->pItem),pNewItem);
            
            break;
        }
        pNewItem = pNewItem->pNext;
    }

    if(nLen == 0)
    {
        pCreateData->pTone = NULL;

        return;
    }

    pNewItem = (CONTACT_ITEMCHAIN*)malloc(sizeof(CONTACT_ITEMCHAIN));

    if(pNewItem == NULL)
    {
        pCreateData->pTone = NULL;

        return;
    }

    memset(pNewItem,0,sizeof(CONTACT_ITEMCHAIN));

    pNewItem->pszData = (char*)malloc(nLen+1);

    if(pNewItem->pszData == NULL)
    {
        pCreateData->pTone = NULL;

        free(pNewItem);
        
        return;
    }

    pNewItem->dwMask = AB_TYPEMASK_TONE;

    pNewItem->nMaxLen = nLen + 1;

    pNewItem->bChange = TRUE;
    
    memcpy(pNewItem->pszData,szTonePath,nLen);

    pNewItem->pszData[nLen] = 0;
    
    Item_Insert(&(pCreateData->pItem),pNewItem);
    
    pCreateData->pTone = pNewItem->pszData;

	PLXTipsWin(NULL,NULL,0,IDS_TONECHANGED,NULL,Notify_Success,IDS_OK,NULL,WAITTIMEOUT);
}
/*********************************************************************\
* Function	   ABEditContact_OnSave
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void ABEditContact_OnSave(HWND hWnd,BOOL bSave)
{
    ABEDITCREATEDATA *pCreateData;
    int   nGroup = -1;
    CONTACT_ITEMCHAIN *pData = NULL;
	char szTitle[50];
    BOOL bCanSave;
    int  nCount;

    pCreateData = (ABEDITCREATEDATA*)GetUserData(hWnd);

	if(bSave == TRUE)
	{
        HWND hEdit;
        CONTACT_ITEMCHAIN *pTmp = NULL;
        char szName[AB_MAXLEN_FIRSTNAME*2];
        char* pFirst = NULL;
		BOOL bValidTime = FALSE;
        
        hEdit = GetDlgItem(hWnd,IDC_ABEDITCONTACT_EDIT);
        
        pTmp = (CONTACT_ITEMCHAIN*)GetWindowLong(hEdit,GWL_USERDATA);
        
        if(pTmp->dwMask == AB_TYPEMASK_DATE)
        {
            bValidTime = SendMessage(hEdit,TEM_GETTIME,0,(LPARAM)pTmp->pszData);
			if(!bValidTime)
			{
				PLXTipsWin(NULL,NULL,0,ML("format error."),NULL,Notify_Alert , IDS_OK , NULL, WAITTIMEOUT);	
				return;
			}
        }
        
		pData = pCreateData->pItem;

        memset(szName,0,AB_MAXLEN_FIRSTNAME*2);

		bCanSave = FALSE;

		while(pData)
		{
			switch( pData->dwMask ) 
			{
			case AB_TYPEMASK_FIRSTNAME:
                if(strlen(pData->pszData) > 0)
                {
                    bCanSave = TRUE;
                    pFirst = pData->pszData;
                }
                break;

			case AB_TYPEMASK_LASTNAME:
				if(strlen(pData->pszData) > 0)
                {
					bCanSave = TRUE;
                    strcpy(szName,pData->pszData);
                }
				break;
			}
			pData = pData->pNext;
		}

		GetWindowText(pCreateData->hFrameWnd,szTitle,49);

		if(bCanSave == FALSE)
		{
			HWND hLst;

			hLst = GetDlgItem(hWnd,IDC_ABEDITCONTACT_LIST);
			
			SendMessage(hLst,LB_SETCURSEL,0,0);

			pCreateData->pFocusDataPtr = (CONTACT_ITEMCHAIN*)SendMessage(hLst,LB_GETITEMDATA,0,0);

			ABEdit_CreateEdit(hWnd);
			
			PLXTipsWin(NULL,NULL,0,IDS_DEFINENAME,szTitle,Notify_Alert , IDS_OK , NULL, WAITTIMEOUT);
			
			return;
		}
        else
        {
            if(pFirst != NULL)
                strcat(szName,pFirst);
        }

		if(AB_SaveRecord(&(pCreateData->dwoffset),&(pCreateData->id),&nGroup,FALSE,
			pCreateData->nTelID,pCreateData->pItem) == FALSE)
			return;

		if(!pCreateData->bNew)
		{
			pData = pCreateData->pItem;

            AB_NotifyWnd((PVOID)AB_GetIndexByID(pIndexName,pCreateData->id,nName),
				AB_MODE_DELETE,AB_OBJECT_NAME,AB_MDU_CONTRACT);  
			                
            nCount = AB_DeleteFromIndexByID(pIndexTel,pCreateData->id,nTel,FALSE);
            nTel -= nCount;
            nCount = AB_DeleteFromIndexByID(pIndexEmail,pCreateData->id,nEmail,FALSE);
            nEmail -= nCount;
            nCount = AB_DeleteFromIndexByID(pIndexID,pCreateData->id,nID,FALSE);
            nID -= nCount;
            nCount = AB_DeleteFromIndexByID(pIndexName,pCreateData->id,nName,TRUE);
            nName--;   
          
		}

		AB_Insert2Table(pCreateData->dwoffset,pCreateData->id,nGroup,pCreateData->pItem);
		
		//nPos = AB_UpdateOffset(pCreateData->dwoffset,pCreateData->id);
		
		//AB_NotifyWnd((PVOID)nPos,AB_MODE_INSERT,AB_OBJECT_ALL,AB_MDU_CONTRACT);

		PLXTipsWin(NULL,NULL,0,IDS_SAVED, szTitle,Notify_Success , IDS_OK , NULL, WAITTIMEOUT);

        SendMessage(pCreateData->hMsgWnd,pCreateData->uMsgCmd,TRUE,(LPARAM)szName);
    
	}
	else
        SendMessage(pCreateData->hMsgWnd,pCreateData->uMsgCmd,FALSE,NULL);

	PostMessage(hWnd,WM_CLOSE,0,0);

    if(pCreateData->hViewWnd)
        PostMessage(pCreateData->hViewWnd,WM_CLOSE,0,0);
}
/*********************************************************************\
* Function	   ABEdit_CreateItem
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static int ABEdit_CreateItem(HWND hWnd)
{
    HWND hList = NULL;

    RECT rcClient;
 
    ABEDITCREATEDATA *pCreateData;

    CONTACT_ITEMCHAIN  *pItem;

    CONTACT_ITEMCHAIN   *pData;

    int  index = -1,nPos,i;

    char szCaption[100];
    
    pCreateData = (ABEDITCREATEDATA*)GetUserData(hWnd);

    pItem = pCreateData->pItem;

    nPos = 0;

    GetClientRect(hWnd,&rcClient);

    pData = pItem;
  
    hList = CreateWindow(
        "LISTBOX",
        "",
        WS_VISIBLE | WS_CHILD | WS_VSCROLL | LBS_MULTILINE,
        rcClient.left,
        rcClient.top,
        rcClient.right - rcClient.left,
        rcClient.bottom - rcClient.top,
        hWnd,
        (HMENU)IDC_ABEDITCONTACT_LIST,
        NULL,
        NULL);
    
    if(hList == NULL)
        return FALSE;

    while(pData)
    {
        if(pData->dwMask == AB_TYPEMASK_PIC || pData->dwMask == AB_TYPEMASK_TONE)
        {
            pData = pData->pNext;

            continue;
        }

        index = GetIndexByMask(pData->dwMask);

        if(index == -1)
            return FALSE;

        szCaption[0] = 0;

        sprintf(szCaption,"%s:",Contact_Item[index].pszCpation);

        i = SendMessage(hList,LB_ADDSTRING,-1,(LPARAM)szCaption);

        if(pData->dwMask == AB_TYPEMASK_DATE)
        {
            char szTime[50];
            
            char szDate[50];

            szTime[0] = 0;

            szDate[0] = 0;

            GetTimeDisplay(*((SYSTEMTIME*)(pData->pszData)),szTime,szDate);

            SendMessage(hList, LB_SETAUXTEXT, MAKEWPARAM(i, -1), (LPARAM)szDate);
        }
        else
            SendMessage(hList, LB_SETAUXTEXT, MAKEWPARAM(i, -1), (LPARAM)(pData->pszData));
        
        SendMessage(hList, LB_SETITEMDATA, i, (LPARAM)pData);
       
        if(pCreateData->pFocusDataPtr == pData)
        {            
            SendMessage(hList,LB_SETCURSEL,nPos,NULL);
         
            pCreateData->hFocus = hList;
            
            pCreateData->nCurFocus = nPos;
        }
                
        pData = pData->pNext;

        nPos++;
        
    }

    return TRUE;

}
/*********************************************************************\
* Function	   ABEdit_CreateEdit
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL ABEdit_CreateEdit(HWND hWnd)
{
    int index = -1,nPos = 0;
    HWND hEdit = NULL,hList = NULL;
    RECT rc;
    IMEEDIT ie;
    DWORD dwStyle;
    ABEDITCREATEDATA *pCreateData = NULL;
    CONTACT_ITEMCHAIN *pData = NULL;
    char szCaption[100];

    pCreateData = (ABEDITCREATEDATA*)GetUserData(hWnd);
    pData = pCreateData->pFocusDataPtr;

    hList = GetDlgItem(hWnd,IDC_ABEDITCONTACT_LIST);
    hEdit = GetDlgItem(hWnd,IDC_ABEDITCONTACT_EDIT);

    nPos = SendMessage(hList,LB_GETCURSEL,0,0);
    SendMessage(hList,LB_GETITEMRECT,(WPARAM)nPos,(LPARAM)&rc);

    if(hEdit != NULL)
    {
        DestroyWindow(hEdit);
        hEdit = NULL;
    }

    index = GetIndexByMask(pData->dwMask);
    
    if(index == -1)
        return FALSE;
        
    if(pData->dwMask == AB_TYPEMASK_DATE)
    {
        DATEFORMAT dateformat;
        
        if(((SYSTEMTIME*)(pData->pszData))->wYear == 0)
        {
            SYSTEMTIME sy;

            memset(&sy,0,sizeof(SYSTEMTIME));

            GetLocalTime(&sy);

            memcpy((SYSTEMTIME*)(pData->pszData),&sy,sizeof(SYSTEMTIME));
        }
        
        szCaption[0] = 0;

        sprintf(szCaption,"%s:",Contact_Item[index].pszCpation);

        dateformat = GetDateFormt();
        if( dateformat == DF_DMY_DOT || dateformat == DF_DMY_SLD || dateformat == DF_DMY_DSH )                
            hEdit = CreateWindow(           //create "yyyy mm dd" date editor;
            "TIMEEDIT",
            szCaption, 
            WS_VISIBLE | WS_CHILD | WS_TABSTOP | TES_DATE_DMY | TES_TITLE | CS_NOSYSCTRL,
            rc.left,
            rc.top,
            rc.right - rc.left,
            rc.bottom - rc.top,
            hWnd,
            (HMENU)IDC_ABEDITCONTACT_EDIT,
            NULL,
            (LPVOID)pData->pszData);
        
        if( dateformat == DF_MDY_DOT || dateformat == DF_MDY_SLD || dateformat == DF_MDY_DSH )                
            hEdit = CreateWindow(           //create "yyyy mm dd" date editor;
            "TIMEEDIT",
            szCaption, 
            WS_VISIBLE | WS_CHILD | WS_TABSTOP | TES_DATE_MDY | TES_TITLE | CS_NOSYSCTRL,
            rc.left,
            rc.top,
            rc.right - rc.left,
            rc.bottom - rc.top,
            hWnd,
            (HMENU)IDC_ABEDITCONTACT_EDIT,
            NULL,
            (LPVOID)pData->pszData);
        
        if( dateformat == DF_YMD_DOT || dateformat == DF_YMD_SLD || dateformat == DF_YMD_DSH )
            hEdit = CreateWindow(           //create "yyyy mm dd" date editor;
            "TIMEEDIT",
            szCaption, 
            WS_VISIBLE | WS_CHILD | WS_TABSTOP | TES_DATE_YMD | TES_TITLE | CS_NOSYSCTRL,
            rc.left,
            rc.top,
            rc.right - rc.left,
            rc.bottom - rc.top,
            hWnd,
            (HMENU)IDC_ABEDITCONTACT_EDIT,
            NULL,
            (LPVOID)pData->pszData);
        
        if (hEdit == NULL)
            return FALSE;
        
        SetWindowLong(hEdit, GWL_USERDATA, (LONG)pData);
    }
    else if(pData->dwMask != AB_TYPEMASK_PIC && pData->dwMask != AB_TYPEMASK_TONE)
    {        
        memset(&ie, 0, sizeof(IMEEDIT));
        
        ie.hwndNotify	= (HWND)hWnd;    
        ie.dwAttrib	    = 0;                
        ie.dwAscTextMax	= 0;
        ie.dwUniTextMax	= 0;
        ie.wPageMax	    = 0;        
        ie.pszCharSet	= NULL;
        ie.pszTitle	    = NULL;
        
        if(Contact_Item[index].bNum == TRUE)
        {
            dwStyle = WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL | WS_TABSTOP | ES_TITLE | CS_NOSYSCTRL ;
            ie.pszImeName	= "Phone";
        }
        else
        {
            dwStyle = WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL | WS_TABSTOP | ES_TITLE | CS_NOSYSCTRL;
            ie.pszImeName	= NULL;
        }
        
        hEdit = CreateWindow(
            "IMEEDIT",
            "",
            dwStyle,
            rc.left,
            rc.top,
            rc.right - rc.left,
            rc.bottom - rc.top,
            hWnd,
            (HMENU)IDC_ABEDITCONTACT_EDIT,
            NULL,
            (PVOID)&ie);
        
        if(hEdit == NULL)
            return FALSE;
        
        SetWindowLong(hEdit, GWL_USERDATA, (LONG)pData);
        
        SendMessage(hEdit, EM_LIMITTEXT, (WPARAM)(Contact_Item[index].nMaxLen), NULL);
        
        pCreateData->bSetText = TRUE;

        szCaption[0] = 0;

        sprintf(szCaption,"%s:",Contact_Item[index].pszCpation);
        
        SendMessage(hEdit, EM_SETTITLE, 0, (LPARAM)szCaption);
        
        SetWindowText(hEdit, pData->pszData);

        //SendMessage(pData->hWnd, TEM_GETTIME, 0, (LPARAM)&st);
        
    }

    if(hEdit != NULL)
    {
        pCreateData->hFocus = hEdit;
        SetFocus(hEdit);
    }

	SendMessage(hEdit, EM_SETSEL, -1, -1);
	
    return TRUE;
}
/*********************************************************************\
* Function	   GetIndexByMask
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
int GetIndexByMask(DWORD dwMask)
{
    int i;
    
    for(i = 0 ; i < AB_EDITCONTACT_CTRNUM+2 ; i++)
    {
        if(Contact_Item[i].dwMask & dwMask)
            return i;
    }

    return -1;
}

int GetIndexByName(const char* itemName)
{
    int i;
    
    for(i = 0 ; i < AB_EDITCONTACT_CTRNUM+2 ; i++)
    {
        if(strcmp(Contact_Item[i].pszCpation, itemName) == 0)
            return i;
    }

    return -1;
}
/*********************************************************************\
* Function	   AB_UpdateOffset
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static int AB_UpdateOffset(DWORD dwoffset,DWORD id)
{
    int i;
    
    for( i = 1 ; i <= nID ; i++ )
    {
        if(id == ((MEMORY_NAME*)pIndexName[i])->id)
        {
            ((MEMORY_NAME*)pIndexName[i])->dwoffset = dwoffset;
            
            return i;
        }
    }

    return -1;
}


typedef struct tagABADDFIELDCREATEDATA
{
    HWND        hFrameWnd;
    HWND        hMsgWnd;
    UINT        uMsgCmd;
    int*        pnStat;
	HBITMAP		hButton;
	HBITMAP		hButtonSel;
}ABADDFIELDCREATEDATA,*PABADDFIELDCREATEDATA;

#define IDC_ABADDFIELD_LIST         300

LRESULT ABAddFieldWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);

static BOOL ABAddField_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct);
static void ABAddField_OnActivate(HWND hwnd, UINT state);
static void ABAddField_OnSetFocus(HWND hWnd);
static void ABAddField_OnPaint(HWND hWnd);
static void ABAddField_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags);
static void ABAddField_OnCommand(HWND hWnd, int id, UINT codeNotify);
static void ABAddField_OnDestroy(HWND hWnd);
static void ABAddField_OnClose(HWND hWnd);

BOOL AB_CreateAddFieldWindow(HWND hFrameWnd,HWND hMsgWnd,UINT uMsgCmd,int nStat[])
{
    WNDCLASS    wc;
    HWND        hAddFieldWnd;
    ABADDFIELDCREATEDATA CreateData;
    RECT        rcClient;

    memset(&CreateData,0,sizeof(ABADDFIELDCREATEDATA));

    CreateData.hFrameWnd = hFrameWnd;
    CreateData.hMsgWnd = hMsgWnd;
    CreateData.uMsgCmd = uMsgCmd;
    CreateData.pnStat = nStat;
    
    wc.style         = 0;
    wc.lpfnWndProc   = ABAddFieldWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = sizeof(ABADDFIELDCREATEDATA);
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName  = "ABAddFieldWndClass";
    
	RegisterClass(&wc);

//    if (!RegisterClass(&wc))
        //return FALSE;
     
    GetClientRect(hFrameWnd,&rcClient);

    hAddFieldWnd = CreateWindow(
        "ABAddFieldWndClass",
        "", 
        WS_VISIBLE | WS_CHILD, 
        rcClient.left,
        rcClient.top,
        rcClient.right - rcClient.left,
        rcClient.bottom - rcClient.top,
        hFrameWnd,
        NULL, 
        NULL, 
        (PVOID)&CreateData
        );
    
    if (!hAddFieldWnd)
    {
        UnregisterClass("ABAddFieldWndClass",NULL);
        return FALSE;
    }

    ShowWindow(hAddFieldWnd,SW_SHOW);
    UpdateWindow(hAddFieldWnd);

    SetFocus(hAddFieldWnd);

    SetWindowText(hFrameWnd,IDS_ADDFIELD);

    SendMessage(hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_CANCEL, 0), (LPARAM)IDS_CANCEL);
    SendMessage(hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_OK, 1), (LPARAM)"");
    SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_SELECT);
    SendMessage(hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON),(LPARAM)NULL);
    SendMessage(hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON),(LPARAM)NULL);


    return TRUE;
}

LRESULT ABAddFieldWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    LRESULT lResult = TRUE;

	switch (wMsgCmd)
    {
    case WM_CREATE:
        lResult = ABAddField_OnCreate(hWnd,(LPCREATESTRUCT)(lParam));
        break;

    case WM_ACTIVATE:
    case PWM_SHOWWINDOW:
        ABAddField_OnActivate(hWnd,(UINT)LOWORD(wParam));
        break;

	case WM_SETFOCUS:
        ABAddField_OnSetFocus(hWnd);
		break;

    case WM_PAINT:
        ABAddField_OnPaint(hWnd);
        break;

    case WM_KEYDOWN:
        ABAddField_OnKey(hWnd,(UINT)(wParam),(int)(short)LOWORD(lParam),(UINT)HIWORD(lParam));
        break;

    case WM_COMMAND:
        ABAddField_OnCommand(hWnd,(int)(LOWORD(wParam)),(UINT)HIWORD(wParam));
        break;
        
    case WM_CLOSE:
        ABAddField_OnClose(hWnd);
        break;

	case WM_TIMER:
		{
			ABADDFIELDCREATEDATA *pData;
			HWND hLst;
            int  index;
            int nSel;

			pData = GetUserData(hWnd);
			
            hLst = GetDlgItem(hWnd,IDC_ABADDFIELD_LIST);

            nSel = SendMessage(hLst,LB_GETCURSEL,0,0);

            index = (int)SendMessage(hLst,LB_GETITEMDATA,nSel,0);
			
			if(IsWindow(pData->hMsgWnd))
                SendMessage(pData->hMsgWnd,pData->uMsgCmd,index,0);

            PostMessage(hWnd,WM_CLOSE,0,0);
		}
		break;

    case WM_DESTROY:
        ABAddField_OnDestroy(hWnd);
        break;

    default:     
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
	}

    return lResult;

}
/*********************************************************************\
* Function	ABAddField_OnCreate
* Purpose   WM_CREATE message handler of the main window
* Params
*			hWnd: Handle of the window
*			lpCreateStruct: Create Structure
* Return
*			TRUE: Success
*			FALSE: Fail
* Remarks
**********************************************************************/
static BOOL ABAddField_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
{    
    RECT rect;
    HWND hList;
    int  i,nIndex;
    ABADDFIELDCREATEDATA *pData;
	SIZE size;
    
    pData = GetUserData(hWnd);

    memcpy(pData,(lpCreateStruct->lpCreateParams),sizeof(ABADDFIELDCREATEDATA));
    
    GetClientRect(hWnd,&rect);

	GetImageDimensionFromFile(AB_BMP_NORMAL,&size);

    pData->hButton = LoadImage(NULL, AB_BMP_NORMAL, IMAGE_BITMAP,
		size.cx, size.cy, LR_LOADFROMFILE);
	
	GetImageDimensionFromFile(AB_BMP_SELECT,&size);

	pData->hButtonSel = LoadImage(NULL, AB_BMP_SELECT, IMAGE_BITMAP,
		size.cx, size.cy, LR_LOADFROMFILE);
	
    hList = CreateWindow(
        "LISTBOX",
        "",
        WS_VISIBLE | WS_CHILD | LBS_BITMAP | WS_VSCROLL,
        rect.left,
        rect.top,
        rect.right - rect.left,
        rect.bottom - rect.top,
        hWnd,
        (HMENU)IDC_ABADDFIELD_LIST,
        NULL,
        NULL);
   
    if(hList == NULL)
        return FALSE;

    for(i = 0 ; i < AB_EDITCONTACT_CTRNUM ; i++)
    {
        if(pData->pnStat[i] == 1)
        {
            nIndex = SendMessage(hList, LB_ADDSTRING, -1, (LPARAM)Contact_Item[i].pszCpation);
            SendMessage(hList, LB_SETITEMDATA, nIndex, (LPARAM)i);
			SendMessage(hList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, nIndex), (LPARAM)pData->hButton);
        }
    }
    SendMessage(hList, LB_SETCURSEL, 0, 0);
        
    return TRUE;
    
}
/*********************************************************************\
* Function	ABAddField_OnActivate
* Purpose   WM_ACTIVATE message handler of the main window
* Params
* Return    None
* Remarks
**********************************************************************/
static void ABAddField_OnActivate(HWND hwnd, UINT state)
{
    HWND hLst;

    ABADDFIELDCREATEDATA *pData;
    
    pData = GetUserData(hwnd);

    hLst = GetDlgItem(hwnd,IDC_ABADDFIELD_LIST);

    SetFocus(hLst);
    
    SetWindowText(pData->hFrameWnd,IDS_ADDFIELD);

    SendMessage(pData->hFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_CANCEL);
    SendMessage(pData->hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
    SendMessage(pData->hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_SELECT);
    SendMessage(pData->hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON),(LPARAM)NULL);
    SendMessage(pData->hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON),(LPARAM)NULL);

    return;
}

/*********************************************************************\
* Function	ABAddField_OnSetFocus
* Purpose   WM_PAINT message handler of the main window
* Params	hWnd: Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void ABAddField_OnSetFocus(HWND hWnd)
{
    HWND hLst;

    hLst = GetDlgItem(hWnd,IDC_ABADDFIELD_LIST);

    SetFocus(hLst);

	return;
}

/*********************************************************************\
* Function	ABAddField_OnPaint
* Purpose   WM_PAINT message handler of the main window
* Params	hWnd: Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void ABAddField_OnPaint(HWND hWnd)
{
	HDC hdc = BeginPaint(hWnd, NULL);

	EndPaint(hWnd, NULL);

	return;
}

/*********************************************************************\
* Function	ABAddField_OnKey
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
static void ABAddField_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags)
{
    ABADDFIELDCREATEDATA* pData;
    
    pData = GetUserData(hWnd);

    switch (vk)
	{
	case VK_F10:
		PostMessage(hWnd,WM_CLOSE,NULL,NULL);
		break;

    case VK_F5:
        {
            HWND hLst;
            int  index;
            int nSel;

            hLst = GetDlgItem(hWnd,IDC_ABADDFIELD_LIST);

            nSel = SendMessage(hLst,LB_GETCURSEL,0,0);

            if(nSel == LB_ERR)
                break;

            index = (int)SendMessage(hLst,LB_GETITEMDATA,nSel,0);

			SendMessage(hLst, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, nSel), (LPARAM)pData->hButtonSel);
			
			SetTimer(hWnd, 1, 500, NULL);

        }
        break;

	default:
		PDADefWindowProc(hWnd, WM_KEYDOWN, vk, MAKELPARAM(cRepeat, flags));
		break;
	}

	return;
}
/*********************************************************************\
* Function	ABAddField_OnCommand
* Purpose   WM_COMMAND message handler of the main window
* Params
*			hWnd:	Handle of the window
*			id:		Id of command message
*			hwndCtl: Handle of the window which sended this message
*			codeNotify:Notify code of the message
* Return	None
* Remarks
**********************************************************************/
static void ABAddField_OnCommand(HWND hWnd, int id, UINT codeNotify)
{
	switch(id)
	{
    case IDC_EXIT:
		PostMessage(hWnd,WM_CLOSE,NULL,NULL);
        break;

    default:
        break;
    }
}
/*********************************************************************\
* Function	ABAddField_OnDestroy
* Purpose   WM_DESTROY message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void ABAddField_OnDestroy(HWND hWnd)
{
    ABADDFIELDCREATEDATA* pData;
    
    pData = GetUserData(hWnd);

	if(pData->hButton)
		DeleteObject(pData->hButton);
	
    UnregisterClass("ABAddFieldWndClass", NULL);

    return;

}
/*********************************************************************\
* Function	ABAddField_OnClose
* Purpose   WM_CLOSE message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void ABAddField_OnClose(HWND hWnd)
{
    ABADDFIELDCREATEDATA* pData;
    
    pData = GetUserData(hWnd);

    SendMessage(pData->hFrameWnd,PWM_CLOSEWINDOW,(WPARAM)hWnd,NULL);
 
    DestroyWindow (hWnd);

    return;

}

/*********************************************************************\
* Function	   Item_New
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
CONTACT_ITEMCHAIN* Item_New(DWORD dwMask)
{
    CONTACT_ITEMCHAIN* p = NULL;
    int  index;

    index = GetIndexByMask(dwMask);
    
    if(index == -1)
        return NULL;

    p = (CONTACT_ITEMCHAIN*)malloc(sizeof(CONTACT_ITEMCHAIN));

    if(p == NULL)
        return NULL;

    memset(p,0,sizeof(CONTACT_ITEMCHAIN));

    p->pszData = (char*)malloc(Contact_Item[index].nMaxLen);

    if(p->pszData == NULL)
    {
        free(p);
        return NULL;
    }

    p->dwMask = dwMask;

    p->nMaxLen = Contact_Item[index].nMaxLen;

    p->bChange = FALSE;
    
    memset(p->pszData,0,p->nMaxLen);

    return p;
}
/*********************************************************************\
* Function	   Item_Insert
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
int Item_Insert(CONTACT_ITEMCHAIN** ppHeader,CONTACT_ITEMCHAIN* pChainNode)
{
    CONTACT_ITEMCHAIN* p;
    int nPos = 0;

    if(*ppHeader)
    {
        p = *ppHeader;

        while( p )
        {
            if(pChainNode->dwMask < p->dwMask)
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
* Function	   Item_Delete
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
void Item_Delete(CONTACT_ITEMCHAIN** ppHeader,CONTACT_ITEMCHAIN* pChainNode)
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

    if(pChainNode->pszData)
        free(pChainNode->pszData);
    free(pChainNode);
    pChainNode = NULL;
}
/*********************************************************************\
* Function	   Item_Erase
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
void Item_Erase(CONTACT_ITEMCHAIN* pHeader)
{
    CONTACT_ITEMCHAIN* p;    
    CONTACT_ITEMCHAIN* ptemp;

    p = pHeader;

    while( p )
    {
        ptemp = p->pNext;
        
        if(p->pszData)
            free(p->pszData);

        free(p);
        p = ptemp;
    }
}

/*********************************************************************\
* Function	   AB_Insert2Table
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
int AB_Insert2Table(DWORD dwoffset,DWORD id,int nGroup,CONTACT_ITEMCHAIN *pItem)
{
    CONTACT_ITEMCHAIN* pTmp = NULL;
    char szTemp[AB_MAXLEN_EMAIL];
    char szName[AB_MAXLEN_FIRSTNAME*2];
    int  nPos = 0;

    pTmp = pItem;

    szName[0] = 0;

    while(pTmp)
    {
        szTemp[0] = 0;
    
        switch(pTmp->dwMask) 
        {
        case AB_TYPEMASK_FIRSTNAME:    
            
            if(strlen(pTmp->pszData) == 0)
                break;

            if(strlen(szName) == 0)
                strcpy(szName,pTmp->pszData);
            else
            {
#ifndef LANGUAGE_CHN
				strcat(szName," ");
#endif
                strcat(szName,pTmp->pszData);
            }
            break;
            
        case AB_TYPEMASK_LASTNAME:
            
            if(strlen(pTmp->pszData) == 0)
                break;

            if(strlen(szName) == 0)
                strcpy(szName,pTmp->pszData);
            else
            {
                strcpy(szTemp,szName);
                
                strcpy(szName,pTmp->pszData);

#ifndef LANGUAGE_CHN                
                strcat(szName," ");
#endif
                strcat(szName,szTemp);
            }
            break;
            
        case AB_TYPEMASK_TEL:
        case AB_TYPEMASK_TELHOME:
        case AB_TYPEMASK_TELWORK:
        case AB_TYPEMASK_MOBILE:
        case AB_TYPEMASK_MOBILEHOME:
        case AB_TYPEMASK_MOBILEWORK:
        case AB_TYPEMASK_FAX:
        case AB_TYPEMASK_PAGER:
        case AB_TYPEMASK_COMPANYTEL:
        case AB_TYPEMASK_DTMF:
            
            if(strlen(pTmp->pszData) == 0)
                break;

            AB_InsertTel(id,pTmp->nID,(BYTE)(strlen(pTmp->pszData)+1),pTmp->pszData);

            AB_Insert2Index(pIndexTel,1,(nTel-1),nTel,g_CmpFunc[1]);

            break;
            
        case AB_TYPEMASK_EMAIL:
        case AB_TYPEMASK_EMAILWORK:
        case AB_TYPEMASK_EMAILHOME:
            
            if(strlen(pTmp->pszData) == 0)
                break;

            AB_InsertEmail(id,pTmp->nID,(BYTE)(strlen(pTmp->pszData)+1),pTmp->pszData);
            
            AB_Insert2Index(pIndexEmail,1,(nEmail-1),nEmail,g_CmpFunc[2]);
            
            break;
            
        default:
            break;
        }

        pTmp = pTmp->pNext;
    }

    AB_InsertName(dwoffset,id,nGroup,(BYTE)(strlen(szName)+1),szName);
    
    nPos = AB_Insert2Index(pIndexID,1,(nID-1),nID,g_CmpFunc[3]);

    nPos = AB_Insert2Index(pIndexName,1,(nName-1),nName,g_CmpFunc[0]);

    AB_NotifyWnd((PVOID)nPos,AB_MODE_INSERT,AB_OBJECT_NAME,AB_MDU_CONTRACT);

    return nPos;
    
}
/*********************************************************************\
* Function	   AB_Insert2Index
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
int AB_Insert2Index(PVOID* Array, int start, int end, int index,CMPFUNC cmpf)
{
    int nPos,i;
    PVOID Temp1,Temp2;

    if(end == 0)
        return 1;
    else
        nPos = HalfComp(Array,start,end,index,cmpf);
 
    if(nPos > end)
        return nPos;
    
    Temp1 = Array[nPos];
    Array[nPos] = Array[index];

    for(i= nPos + 1; i<= end; i++)
    {
        Temp2 = Array[i];
        Array[i] = Temp1;
        Temp1 = Temp2;
    }

    //the last one
    Array[end + 1] = Temp1;

    return nPos;
}
/*********************************************************************\
* Function	   CopyItemChain
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL CopyItemChain(PCONTACT_ITEMCHAIN* ppHeader,CONTACT_ITEMCHAIN* pItemChain,PVOID *ppFocusPtr,PVOID pFocus)
{
    CONTACT_ITEMCHAIN * pNewItem = NULL;
    CONTACT_ITEMCHAIN * pTemp = NULL;

    *ppHeader = NULL;

    pTemp = pItemChain;

    while(pTemp)
    {
        if(pTemp->dwMask == AB_TYPEMASK_PIC || pTemp->dwMask == AB_TYPEMASK_TONE)
        {
            pNewItem = (CONTACT_ITEMCHAIN*)malloc(sizeof(CONTACT_ITEMCHAIN));
            
            if(pNewItem == NULL)
            {
                Item_Erase(*ppHeader);
                
                return FALSE;
            }
            
            memset(pNewItem,0,sizeof(CONTACT_ITEMCHAIN));
            
            pNewItem->pszData = (char*)malloc(pTemp->nMaxLen);
            
            if(pNewItem->pszData == NULL)
            {
                Item_Erase(*ppHeader);
                
                free(pNewItem);
                
                return FALSE;
            }
        }
        else
        {
            pNewItem = Item_New(pTemp->dwMask);
            
            if(pNewItem == NULL)
            {
                Item_Erase(*ppHeader);
                
                return FALSE;
            }
        }

        pNewItem->dwMask = pTemp->dwMask;
        pNewItem->bChange = pTemp->bChange;
        pNewItem->nID = pTemp->nID;
        pNewItem->nMaxLen = pTemp->nMaxLen;
        memcpy(pNewItem->pszData,pTemp->pszData,pTemp->nMaxLen);
        
        Item_Insert(ppHeader,pNewItem);

        if(pFocus == pTemp)
            *ppFocusPtr = pNewItem;

        pTemp = pTemp->pNext;
    }

    return TRUE;
}

/*********************************************************************\
* Function	   AB_DeleteFromIndexByID
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
int AB_DeleteFromIndexByID(PVOID* Array, DWORD id,int end,BOOL bPos)
{
    int i,j,nCount,nPos;

    nCount = 0;

    for(i = end ; i >= 1 ; i--)
    {
        if(*(DWORD*)Array[i] == id)
        {
            nPos = i;

            for(j = i +1 ; j <= (end-nCount) ; j++)
                Array[j -1] = Array[j];
            
            nCount++;
        }
    }

    if(bPos)
        return nPos;
    else
        return nCount;
}
/*********************************************************************\
* Function	   AB_GetIndexByID
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
int AB_GetIndexByID(PVOID* Array, DWORD id,int end)
{
    int i;

    for(i = end ; i >= 1 ; i--)
    {
        if(*(DWORD*)Array[i] == id)
        {
			return i;
        }
    }

	return -1;
}
/*********************************************************************\
* Function	   ABEditContact_OnRemoveField
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void ABEditContact_OnRemoveField(HWND hWnd,BOOL bRemove)
{    
    HWND hLst = NULL;
    int  nPos;
    int  index;
    char szTitle[50];
    CONTACT_ITEMCHAIN   *pData = NULL;
    ABEDITCREATEDATA    *pCreateData;

    if(bRemove == FALSE)
        return;
    
    pCreateData = GetUserData(hWnd);
    
    hLst = GetDlgItem(hWnd,IDC_ABEDITCONTACT_LIST);
    
    nPos = SendMessage(hLst,LB_GETCURSEL,0,0);

    pData = (CONTACT_ITEMCHAIN*)SendMessage(hLst,LB_GETITEMDATA,nPos,0);

    pData->bChange = TRUE;
    
    SendMessage(hLst,LB_DELETESTRING,nPos,0);
       
    index = SendMessage(hLst,LB_GETCOUNT,0,0);
    
    if(nPos == index)
        SendMessage(hLst,LB_SETCURSEL,nPos-1,0);
    else
        SendMessage(hLst,LB_SETCURSEL,nPos,0);
    
    nPos = SendMessage(hLst,LB_GETCURSEL,0,0);
    
    pCreateData->pFocusDataPtr = (CONTACT_ITEMCHAIN*)SendMessage(hLst,LB_GETITEMDATA,nPos,0);
    
    pCreateData->nCurFocus = 0;
    
    ABEdit_CreateEdit(hWnd);    

    Item_Delete(&(pCreateData->pItem),pData);
    
    szTitle[0] = 0;
    
    GetWindowText(pCreateData->hFrameWnd,szTitle,49);
            
    szTitle[49] = 0;

    PLXTipsWin(NULL,NULL,0,IDS_REMOVED, szTitle,Notify_Success , IDS_OK , NULL, WAITTIMEOUT);
    
}




/*********************************************************************\
* Function	   AB_CharsetTransfer
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void AB_CharsetTransfer(char* pSrc,int nLen,char* pData,int nMaxLen,int nCharset)
{
    char* ptemp;
    int  templen = 0;
    int  copylen = 0;

    if(nCharset == VCARD_CHARSET_UTF_8 || nCharset == VCARD_CHARSET_USASCII)
    {
        templen = UTF8ToMultiByte(CP_ACP,0,pSrc,nLen,NULL,NULL,NULL,NULL);

        if(templen == 0)
            return;

        ptemp = (char*)malloc(templen+1);

        if(ptemp == NULL)
            return;

        memset(ptemp,0,templen+1);

        UTF8ToMultiByte(CP_ACP,0,pSrc,nLen,ptemp,templen+1,NULL,NULL);
        
        memset(pData,0,nMaxLen);

        copylen = templen > (nMaxLen-1) ? (nMaxLen-1) : templen;

        strncpy(pData,ptemp,copylen);

        pData[copylen] = '\0';
        
    }
    else if(nCharset == VCARD_CHARSET_UNICODE)
    {
        templen = WideCharToMultiByte(CP_ACP,0,(LPWSTR)pSrc,nLen,NULL,NULL,NULL,NULL);

        if(templen == 0)
            return;

        ptemp = (char*)malloc(templen*2+1);

        if(ptemp == NULL)
            return;

        memset(ptemp,0,templen*2+1);

        WideCharToMultiByte(CP_ACP,0,(LPWSTR)pSrc,nLen,ptemp,templen*2+1,NULL,NULL);
        
        memset(pData,0,nMaxLen);

        copylen = templen*2 > (nMaxLen-1) ? (nMaxLen-1) : templen*2;

        strncpy(pData,ptemp,copylen);

        pData[copylen] = '\0';
    }

    return;
}



/*********************************************************************\
* Function	   AB_AnalyseVcard
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL AB_AnalyseVcard(vCard_Obj* pvCardObj,CONTACT_ITEMCHAIN** ppVcardVcal,int *pnTelID)
{
    vCard_Tel* pTel;
    vCard_Name* pName;
    vCard_EMail* pEmail;
    vCard_Addr* pAddr;
    CONTACT_ITEMCHAIN* pNewItem = NULL;
    BOOL        bName = FALSE;
    BOOL        bPostAdd = FALSE,bCoAdd = FALSE;
    
    if(pvCardObj == NULL)
        return FALSE;
    
    if(pvCardObj) 
    {
        pTel   = pvCardObj->pTel;
        pName  = pvCardObj->pName;
        pEmail = pvCardObj->pEmail;
        pAddr  = pvCardObj->pAddr;
        
#ifndef _EMULATE_
        ;//        KickDog();
#endif
        
        //telnumber
        while(pTel)
        {                                       
            if(pTel->number)
            {                
                pNewItem = Item_New(AB_TYPEMASK_TEL);
                
                if(pNewItem == NULL)
                {
                    Item_Erase(*ppVcardVcal);
                    
                    return FALSE;
                }
                
                AB_CharsetTransfer(pTel->number,pTel->len,pNewItem->pszData,AB_MAXLEN_TEL,pTel->Charset);
                
                switch(pTel->Mask)
                {
                case VCARD_TEL_CELL_MASK|VCARD_TEL_VOICE_MASK://p802 cell
                case VCARD_TEL_CELL_MASK://nokia cell
                case VCARD_TEL_PREE_MASK:
                    pNewItem->dwMask = AB_TYPEMASK_MOBILE;
                    break;
                    
                case VCARD_TEL_HOME_MASK|VCARD_TEL_VOICE_MASK:
                case VCARD_TEL_HOME_MASK:
                    pNewItem->dwMask = AB_TYPEMASK_TELHOME;
                    break;
                    
                case VCARD_TEL_WORK_MASK|VCARD_TEL_VOICE_MASK:
                case VCARD_TEL_WORK_MASK:
                    pNewItem->dwMask = AB_TYPEMASK_TELWORK;
                    break;
                    
                case VCARD_TEL_FAX_MASK|VCARD_TEL_VOICE_MASK:
                case VCARD_TEL_FAX_MASK:
                    pNewItem->dwMask = AB_TYPEMASK_FAX;
                    break;
                    
                case VCARD_TEL_PAGER_MASK|VCARD_TEL_VOICE_MASK:
                case VCARD_TEL_PAGER_MASK:
                    pNewItem->dwMask = AB_TYPEMASK_PAGER;
                    break;
                    
                default:
                    break;                           
                }         
                pNewItem->nID = (*pnTelID)++;

                Item_Insert(ppVcardVcal,pNewItem);
            }           
            pTel = pTel->pNext;    
        }
        
        //Name
        if(pName)
        {
            int len = AB_MAXLEN_FIRSTNAME-1;
            
            if(pName->famil != NULL)
            {
                pNewItem = Item_New(AB_TYPEMASK_LASTNAME);
                
                if(pNewItem == NULL)
                {
                    Item_Erase(*ppVcardVcal);
                    
                    return FALSE;
                }
                
                AB_CharsetTransfer(pName->famil,pName->famillen,pNewItem->pszData,AB_MAXLEN_FIRSTNAME,pName->Charset);
                
                Item_Insert(ppVcardVcal,pNewItem);
                
                bName = TRUE;
            }
            
            if(pName->given != NULL)
            {
                pNewItem = Item_New(AB_TYPEMASK_FIRSTNAME);
                
                if(pNewItem == NULL)
                {
                    Item_Erase(*ppVcardVcal);
                    
                    return FALSE;
                }
                
                AB_CharsetTransfer(pName->given,pName->givenlen,pNewItem->pszData,AB_MAXLEN_FIRSTNAME,pName->Charset);
                
                Item_Insert(ppVcardVcal,pNewItem);
                
                bName = TRUE;
            }
            if(pName->Addition != NULL && bName == FALSE)
            {
                pNewItem = Item_New(AB_TYPEMASK_LASTNAME);
                
                if(pNewItem == NULL)
                {
                    Item_Erase(*ppVcardVcal);
                    
                    return FALSE;
                }
                
                AB_CharsetTransfer(pName->Addition,pName->Additionlen,pNewItem->pszData,AB_MAXLEN_FIRSTNAME,pName->Charset);
                
                Item_Insert(ppVcardVcal,pNewItem);
                
                bName = TRUE;
            }
        }
        
        //Email1,Email2
        while(pEmail)
        {             
            if(pEmail->Email)
            {                              
                pNewItem = Item_New(AB_TYPEMASK_EMAIL);
                
                if(pNewItem == NULL)
                {
                    Item_Erase(*ppVcardVcal);
                    
                    return FALSE;
                }
                
                AB_CharsetTransfer(pEmail->Email,pEmail->len,pNewItem->pszData,AB_MAXLEN_EMAIL,pEmail->Charset);
                         
                pNewItem->nID = (*pnTelID)++;

                Item_Insert(ppVcardVcal,pNewItem);                        
            }
            
            pEmail = pEmail->pNext;        
        }
        
        //Address
        while(pAddr)
        {             
            if(bPostAdd == TRUE && bCoAdd == TRUE)
                break;
            
            if(pAddr->postOffice)
            {                              
                pNewItem = Item_New(AB_TYPEMASK_POSTALADDRESS);
                
                if(pNewItem == NULL)
                {
                    Item_Erase(*ppVcardVcal);
                    
                    return FALSE;
                }
                
                AB_CharsetTransfer(pAddr->postOffice,pAddr->postOfficelen,pNewItem->pszData,
                    AB_MAXLEN_POSTALADDRESS,pAddr->Charset);
                
                switch(pAddr->Mask)
                {
                case VCARD_ADR_POSTAL_MASK:
                    pNewItem->dwMask = AB_TYPEMASK_POSTALADDRESS;
                    Item_Insert(ppVcardVcal,pNewItem); 
                    
                    if(bPostAdd != TRUE)
                    {
                        bPostAdd = TRUE; 
                    }
                    else
                    {
                        Item_Delete(ppVcardVcal,pNewItem);
                    }
                    break;
                    
                case VCARD_ADR_WORK_MASK:
                    pNewItem->dwMask = AB_TYPEMASK_COMPANYADDRESS;
                    Item_Insert(ppVcardVcal,pNewItem);  
                    
                    if(bCoAdd != TRUE)
                    {
                        bCoAdd = TRUE; 
                    }
                    else
                    {
                        Item_Delete(ppVcardVcal,pNewItem);
                    }
                    break;
                    
                default:
                    Item_Insert(ppVcardVcal,pNewItem);  
                    
                    Item_Delete(ppVcardVcal,pNewItem);
                    break;
                }
                
            }
            pAddr = pAddr->pNext;        
        }
        
        
        //content
        if(pvCardObj->pComment)
        {                      
            pNewItem = Item_New(AB_TYPEMASK_NOTE);
            
            if(pNewItem == NULL)
            {
                Item_Erase(*ppVcardVcal);
                
                return FALSE;
            }
            
            AB_CharsetTransfer(pvCardObj->pComment->pData,pvCardObj->pComment->len,pNewItem->pszData,
                AB_MAXLEN_NOTE,pvCardObj->pComment->Charset);
            
            Item_Insert(ppVcardVcal,pNewItem);      
        }
        
        //URL
        if(pvCardObj->pURL)
        {                      
            pNewItem = Item_New(AB_TYPEMASK_HTTPURL);
            
            if(pNewItem == NULL)
            {
                Item_Erase(*ppVcardVcal);
                
                return FALSE;
            }
            
            AB_CharsetTransfer(pvCardObj->pURL->pData,pvCardObj->pURL->len,pNewItem->pszData,
                AB_MAXLEN_HTTPURL,pvCardObj->pURL->Charset);
            
            Item_Insert(ppVcardVcal,pNewItem);      
        }
        
        //Title
        if(pvCardObj->pTitle)
        {                      
            pNewItem = Item_New(AB_TYPEMASK_JOBTITLE);
            
            if(pNewItem == NULL)
            {
                Item_Erase(*ppVcardVcal);
                
                return FALSE;
            }
            
            AB_CharsetTransfer(pvCardObj->pTitle->pData,pvCardObj->pTitle->len,pNewItem->pszData,
                AB_MAXLEN_JOBTITLE,pvCardObj->pTitle->Charset);
            
            Item_Insert(ppVcardVcal,pNewItem);      
        }
        
        
        //Company
        if(pvCardObj->pOrg)
        {                      
            pNewItem = Item_New(AB_TYPEMASK_COMPANY);
            
            if(pNewItem == NULL)
            {
                Item_Erase(*ppVcardVcal);
                
                return FALSE;
            }
            
            AB_CharsetTransfer(pvCardObj->pOrg->OrgName,pvCardObj->pOrg->OrgNamelen,pNewItem->pszData,
                AB_MAXLEN_COMPANY,pvCardObj->pOrg->Charset);
            
            Item_Insert(ppVcardVcal,pNewItem);      
        }
        
        //birthday
        if(strlen(pvCardObj->BirthDay) > 0)
        {              
            char    year[5];
            char    month[3];
            char    day[3];
            char*   p;
            int     n;
            
            pNewItem = Item_New(AB_TYPEMASK_DATE);
            
            if(pNewItem == NULL)
            {
                Item_Erase(*ppVcardVcal);
                
                return FALSE;
            }
            
            p = pvCardObj->BirthDay;
            
            strncpy(year,p,4);
            year[4] = 0;
            p += 4;

            ((SYSTEMTIME*)(pNewItem->pszData))->wYear = atoi(year);
            
            n = strlen(p);

            while(*p != 0 && *p < '0' && *p > '9')
            {
                p++;
            }
            
            strncpy(month,p,2);
            month[2] = 0;

            ((SYSTEMTIME*)(pNewItem->pszData))->wMonth = atoi(month);

            if(((SYSTEMTIME*)(pNewItem->pszData))->wMonth <= 12 && 
                ((SYSTEMTIME*)(pNewItem->pszData))->wMonth >= 0)
            {
                p += 2;
            }
            else
            {
                month[1] = 0;
                ((SYSTEMTIME*)(pNewItem->pszData))->wMonth = atoi(month);
                p += 1;
            }
            
            while(*p != 0 && *p < '0' && *p > '9')
            {
                p++;
            }
            
            strncpy(day,p,2);
            day[2] = 0;
            p += 2;
            
            ((SYSTEMTIME*)(pNewItem->pszData))->wDay = atoi(day);
            
            Item_Insert(ppVcardVcal,pNewItem);      
        }
    }
    
    return TRUE;
}
/*********************************************************************\
* Function	   AB_GetFullName
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL AB_GetFullName(CONTACT_ITEMCHAIN* pItem,char* pszFullName)
{
    CONTACT_ITEMCHAIN *pTemp;
    
    char *pszFirstName,*pszLastName;
            
    pTemp = pItem;
    
    pszFirstName = NULL;
    
    pszLastName = NULL;
    
    while(pTemp)
    {
        if(pTemp->dwMask == AB_TYPEMASK_FIRSTNAME)
            pszFirstName = pTemp->pszData;
        else if(pTemp->dwMask == AB_TYPEMASK_LASTNAME)
            pszLastName = pTemp->pszData;
        
        pTemp = pTemp->pNext;
    }
    
    if(pszFirstName == NULL && pszLastName == NULL)
        pszFullName[0] = 0;
    else if(pszFirstName == NULL && pszLastName != NULL)
        strcpy(pszFullName,pszLastName);
    else if(pszFirstName != NULL && pszLastName == NULL)
        strcpy(pszFullName,pszFirstName);
    else
        sprintf(pszFullName,"%s %s",pszLastName,pszFirstName);

    return TRUE;
    
}
