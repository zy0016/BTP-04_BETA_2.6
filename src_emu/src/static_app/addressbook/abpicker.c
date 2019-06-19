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

#define IDC_BACK                200
#define IDC_ABPICK_LIST         300

#define WM_PICK                 WM_USER+91
#define WM_INITSELECT			WM_USER+92

typedef struct tagAB_PickData
{
    HWND                  hFrameWnd;
    HWND                  hMsgWnd;
    UINT                  uMsgCmd;
	DWORD				  id;
    CONTACT_ITEMCHAIN*    pItemData;
    HBITMAP               hBmp;
    BOOL                  bExtern;
    ABNAMEOREMAIL         name;
}AB_PICKDATA,*PAB_PICKDATA;

static BOOL ABCreatePicker(HWND hWnd,HWND hMsgWnd,UINT uMsgCmd,char* pszCaption,
                           CONTACT_ITEMCHAIN* pItemData,DWORD id,BOOL bExtern);
LRESULT ABPickWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);

static BOOL ABPick_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct);
static void ABPick_OnActivate(HWND hwnd, UINT state);
static void ABPick_OnPaint(HWND hWnd);
static void ABPick_OnSetFocus(HWND hwnd);
static void ABPick_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags);
static LRESULT ABPick_OnCommand(HWND hWnd, int id, UINT codeNotify,LPARAM lParam);
static void ABPick_OnDestroy(HWND hWnd);
static void ABPick_OnClose(HWND hWnd);


BOOL AB_SelectPhoneOrEmail(HWND hFrameWnd,HWND hMsgWnd,UINT uMsgCmd,CONTACT_ITEMCHAIN* pItemData,
                           AB_PICK_TYPE nType,DWORD id,BOOL bPrompt,BOOL bExtern)
{
    CONTACT_ITEMCHAIN*    pHeader;
    CONTACT_ITEMCHAIN*    pNewItem;
    CONTACT_ITEMCHAIN*    pTmp;
    int                   nCount;
    char                  szCaption[AB_MAXLEN_FIRSTNAME*2];
    char                  *p = NULL,*q = NULL;
	int					  nTelID;
    ABNAMEOREMAIL         name;
    
    memset(&name,0,sizeof(ABNAMEOREMAIL));

    nCount = 0;
    pHeader = NULL;
    pTmp = pItemData;
    szCaption[0] = 0;

    while(pTmp)
    {
        if(pTmp->dwMask == AB_TYPEMASK_FIRSTNAME )
        {
            p = pTmp->pszData;
        }
        if(pTmp->dwMask == AB_TYPEMASK_LASTNAME)
        {
            q = pTmp->pszData;
        }

        switch(nType) 
        {
        case PICK_NUMBER:
            switch(pTmp->dwMask)
            {
            case AB_TYPEMASK_TEL:
            case AB_TYPEMASK_TELWORK:
            case AB_TYPEMASK_TELHOME:
            case AB_TYPEMASK_MOBILE:      
            case AB_TYPEMASK_MOBILEHOME:  
            case AB_TYPEMASK_MOBILEWORK:
            case AB_TYPEMASK_COMPANYTEL:
            case AB_TYPEMASK_FAX:
            case AB_TYPEMASK_PAGER:   
			case AB_TYPEMASK_DTMF:
                pNewItem = Item_New(pTmp->dwMask);
                
                if(pNewItem == NULL)
                {
                    if(pHeader)
                        Item_Erase(pHeader);
                    if(bExtern == TRUE)
                        SendMessage(hMsgWnd,uMsgCmd,FALSE,NULL);
                    else
                        SendMessage(hMsgWnd,uMsgCmd,(WPARAM)-1,(LPARAM)-1);
                    return FALSE;
                }

                pNewItem->dwMask = pTmp->dwMask;
                pNewItem->bChange = FALSE;
                pNewItem->nMaxLen = pTmp->nMaxLen;
                strcpy(pNewItem->pszData,pTmp->pszData);
                pNewItem->pPioneer = NULL;
                pNewItem->pNext = NULL;
				pNewItem->nID = pTmp->nID;
                nCount++;
				if(nCount == 1 && (AB_IsTel(pTmp->dwMask) || AB_IsEmail(pTmp->dwMask)))
                {
					nTelID = pNewItem->nID;

                    strcpy(name.szTelOrEmail,pNewItem->pszData);

                    name.nType = AB_NUMBER;
                }
                Item_Insert(&pHeader,pNewItem);
                break;    
                
            default:
                break;
            }
            break;

        case PICK_EMAIL:            
            switch(pTmp->dwMask)
            {
            case AB_TYPEMASK_EMAIL:
            case AB_TYPEMASK_EMAILWORK:
            case AB_TYPEMASK_EMAILHOME:
                pNewItem = Item_New(pTmp->dwMask);
                
                if(pNewItem == NULL)
                {
                    if(pHeader)
                        Item_Erase(pHeader);
                    
                    if(bExtern == TRUE)
                        SendMessage(hMsgWnd,uMsgCmd,FALSE,NULL);
                    else
                        SendMessage(hMsgWnd,uMsgCmd,(WPARAM)-1,(LPARAM)-1);

                    return FALSE;
                }
                               
                pNewItem->dwMask = pTmp->dwMask;
                pNewItem->bChange = FALSE;
                pNewItem->nMaxLen = pTmp->nMaxLen;
                strcpy(pNewItem->pszData,pTmp->pszData);
                pNewItem->pPioneer = NULL;
                pNewItem->pNext = NULL;
				pNewItem->nID = pTmp->nID;
                nCount++;
                if(nCount == 1)
                {
					nTelID = pNewItem->nID;

                    strcpy(name.szTelOrEmail,pNewItem->pszData);

                    name.nType = AB_EMAIL;
                }
                Item_Insert(&pHeader,pNewItem);
                break;    
                
            default:
                break;
            }
            break;

        case PICK_NUMBERANDEMAIL:     
            switch(pTmp->dwMask)
            {
            case AB_TYPEMASK_TEL:
            case AB_TYPEMASK_TELWORK:
            case AB_TYPEMASK_TELHOME:
            case AB_TYPEMASK_MOBILE:      
            case AB_TYPEMASK_MOBILEHOME:  
            case AB_TYPEMASK_MOBILEWORK:
            case AB_TYPEMASK_COMPANYTEL:
            case AB_TYPEMASK_FAX:
            case AB_TYPEMASK_PAGER:  
            case AB_TYPEMASK_EMAIL:
            case AB_TYPEMASK_EMAILWORK:
            case AB_TYPEMASK_EMAILHOME:     
			case AB_TYPEMASK_DTMF:
                pNewItem = Item_New(pTmp->dwMask);
                
                if(pNewItem == NULL)
                {
                    if(pHeader)
                        Item_Erase(pHeader);
                    
                    if(bExtern == TRUE)
                        SendMessage(hMsgWnd,uMsgCmd,FALSE,NULL);
                    else
                        SendMessage(hMsgWnd,uMsgCmd,(WPARAM)-1,(LPARAM)-1);

                    return FALSE;
                }
                
                pNewItem->dwMask = pTmp->dwMask;
                pNewItem->bChange = FALSE;
                pNewItem->nMaxLen = pTmp->nMaxLen;
                strcpy(pNewItem->pszData,pTmp->pszData);
                pNewItem->pPioneer = NULL;
                pNewItem->pNext = NULL;
				pNewItem->nID = pTmp->nID;
                nCount++;
				if(nCount == 1 && (AB_IsTel(pTmp->dwMask) || AB_IsEmail(pTmp->dwMask)))
                {
					nTelID = pNewItem->nID;
                
                    strcpy(name.szTelOrEmail,pNewItem->pszData);

                    if(AB_IsTel(pTmp->dwMask))
                        name.nType = AB_NUMBER;
                    else
                        name.nType = AB_EMAIL;
                }
                Item_Insert(&pHeader,pNewItem);
                break;    
                
            default:
                break;
            }
            break;

        default:
            if(pHeader)
                Item_Erase(pHeader);
            
            if(bExtern == TRUE)
                SendMessage(hMsgWnd,uMsgCmd,FALSE,NULL);
            else
                SendMessage(hMsgWnd,uMsgCmd,(WPARAM)-1,(LPARAM)-1);
            return FALSE;
        }

        pTmp = pTmp->pNext;
    }

    if(nCount == 0)
    {
		char        szCaption[50];
        char*       pszPrompt;
		
		GetWindowText(hFrameWnd,szCaption,49);
		szCaption[49] = 0;

		switch(nType) 
        {
		case PICK_NUMBER:
            pszPrompt = (char*)IDS_NOPHONENUM;
			break;

		case PICK_EMAIL:
            pszPrompt = (char*)IDS_NOEMAIL;
			break;

        case PICK_NUMBERANDEMAIL:
		default:
            pszPrompt = (char*)IDS_NOPHONENUMOREMAIL;
            break;
		}
        
        if(bPrompt == TRUE)
            PLXTipsWin(NULL,NULL,0,pszPrompt,szCaption,Notify_Alert,IDS_OK,NULL,WAITTIMEOUT);
        
        if(bExtern == TRUE)
            SendMessage(hMsgWnd,uMsgCmd,FALSE,NULL);
        else
            SendMessage(hMsgWnd,uMsgCmd,-1,-1);
        
        Item_Erase(pHeader);

		return TRUE;
	}
    else if(nCount == 1)
    {
        
        if(bExtern == TRUE)
        {
            if(q == NULL)
                strcpy(name.szName,p);
            else if(p == NULL)
                strcpy(name.szName,q);
            else
                sprintf(name.szName,"%s %s",q,p);

            SendMessage(hMsgWnd,uMsgCmd,TRUE,(LPARAM)&name);
        }
        else
            SendMessage(hMsgWnd,uMsgCmd,(WPARAM)nTelID,(LPARAM)id);

        Item_Erase(pHeader);

        return TRUE;
    }
    else
    {
		if(q == NULL)
			strcpy(szCaption,p);
		else if(p == NULL)
			strcpy(szCaption,q);
		else
			sprintf(szCaption,"%s %s",q,p);
        //Format caption
        if(ABCreatePicker(hFrameWnd,hMsgWnd,uMsgCmd,szCaption,pHeader,id,bExtern))
            return TRUE;
        else
        {
            if(bExtern == TRUE)
                SendMessage(hMsgWnd,uMsgCmd,FALSE,NULL);
            else
                SendMessage(hMsgWnd,uMsgCmd,(WPARAM)-1,(LPARAM)-1);
            
            Item_Erase(pHeader);

            return FALSE;
        }
    }     
}

BOOL AB_PickRegisterClass(void)
{
    WNDCLASS    wc;

    wc.style         = 0;
    wc.lpfnWndProc   = ABPickWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = sizeof(AB_PICKDATA);
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName  = "ABPickWndClass";
    
    return(RegisterClass(&wc));
}

static BOOL ABCreatePicker(HWND hFrameWnd,HWND hMsgWnd,UINT uMsgCmd,char* pszCaption,
                           CONTACT_ITEMCHAIN* pItemData,DWORD id,BOOL bExtern)
{
    AB_PICKDATA CreateData;
    HWND        hPickWnd;
    RECT        rcClient;
    
    memset(&CreateData,0,sizeof(AB_PICKDATA));

    CreateData.bExtern = bExtern;
    CreateData.hFrameWnd = hFrameWnd;
    CreateData.hMsgWnd = hMsgWnd;
    CreateData.uMsgCmd = uMsgCmd;
	CreateData.id = id;
	CreateData.pItemData = pItemData;
    strcpy(CreateData.name.szName,pszCaption);

    GetClientRect(hFrameWnd,&rcClient);

    hPickWnd = CreateWindow(
        "ABPickWndClass",
        "",//pszCaption, 
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
    
    if (!hPickWnd)
    {
        Item_Erase(pItemData);
        return FALSE;
    }
 
    SetFocus(hPickWnd);
   
    SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
    SendMessage(hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_BACK, 0), (LPARAM)IDS_BACK);
    SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_SELECT);
    SendMessage(hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON),(LPARAM)NULL);
    SendMessage(hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON),(LPARAM)NULL);

    return TRUE;
}

LRESULT ABPickWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    LRESULT lResult = TRUE;

	switch (wMsgCmd)
    {
    case WM_CREATE:
        lResult = ABPick_OnCreate(hWnd,(LPCREATESTRUCT)(lParam));
        break;

    case WM_ACTIVATE:
    case PWM_SHOWWINDOW:
        ABPick_OnActivate(hWnd,(UINT)LOWORD(wParam));
        break;

    case WM_SETFOCUS:
        ABPick_OnSetFocus(hWnd);
        break;

    case WM_PAINT:
        ABPick_OnPaint(hWnd);
        break;

    case WM_KEYDOWN:
        ABPick_OnKey(hWnd,(UINT)(wParam),(int)(short)LOWORD(lParam),(UINT)HIWORD(lParam));
        break;

    case WM_COMMAND:
        lResult = ABPick_OnCommand(hWnd,(int)(LOWORD(wParam)),(UINT)HIWORD(wParam), lParam);
        break;
        
    case WM_CLOSE:
        ABPick_OnClose(hWnd);
        break;

    case WM_DESTROY:
        ABPick_OnDestroy(hWnd);
        break;

    default:     
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
	}

    return lResult;

}
/*********************************************************************\
* Function	ABPick_OnCreate
* Purpose   WM_CREATE message handler of the main window
* Params
*			hWnd: Handle of the window
*			lpCreateStruct: Create Structure
* Return
*			TRUE: Success
*			FALSE: Fail
* Remarks
**********************************************************************/
static BOOL ABPick_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
{    
    RECT rect;
    AB_PICKDATA *pData;
    HWND hList;
    CONTACT_ITEMCHAIN* pTempData;
    int  nIndex,nOffset;
    char szCaption[32];

    pData = GetUserData(hWnd);

    memcpy(pData,lpCreateStruct->lpCreateParams,sizeof(AB_PICKDATA));
    
    GetClientRect(hWnd,&rect);
    
    hList = CreateWindow(
        "LISTBOX",
        "",
        WS_VISIBLE | WS_CHILD | WS_VSCROLL | LBS_BITMAP | LBS_MULTILINE ,
        rect.left,
        rect.top,
        rect.right - rect.left,
        rect.bottom - rect.top,
        hWnd,
        (HMENU)IDC_ABPICK_LIST,
        NULL,
        NULL);
   
    if(hList == NULL)
        return FALSE;

    pTempData = pData->pItemData;
    
    //pData->hBmp = LoadImage(NULL, "/rom/message/unibox/mail_aff.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

    while(pTempData)
    {
        nOffset = GetIndexByMask(pTempData->dwMask);

        if(nOffset == -1)
            return FALSE;
		
		sprintf(szCaption, "%s:", Contact_Item[nOffset].pszCpation);
        nIndex = SendMessage(hList, LB_ADDSTRING, (WPARAM)-1, (LPARAM) (LPCTSTR)szCaption);
        //SendMessage(hList, LB_SETIMAGE, (WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)nIndex),(LPARAM)pData->hBmp);
        SendMessage(hList, LB_SETAUXTEXT, MAKEWPARAM(nIndex, -1), (LPARAM)pTempData->pszData);
        SendMessage(hList, LB_SETITEMDATA, nIndex, (LPARAM)pTempData);

        pTempData = pTempData->pNext;
    }
    SendMessage(hList, LB_SETCURSEL, 0, 0);
    
    return TRUE;
    
}
/*********************************************************************\
* Function	ABPick_OnActivate
* Purpose   WM_ACTIVATE message handler of the main window
* Params
* Return    None
* Remarks
**********************************************************************/
static void ABPick_OnActivate(HWND hwnd, UINT state)
{
    HWND hLst;
    AB_PICKDATA *pData;
    
    pData = GetUserData(hwnd);

    hLst = GetDlgItem(hwnd,IDC_ABPICK_LIST);

    SetFocus(hLst);

    SendMessage(pData->hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
    SendMessage(pData->hFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_BACK);
    SendMessage(pData->hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_SELECT);
    SendMessage(pData->hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON),(LPARAM)NULL);
    SendMessage(pData->hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON),(LPARAM)NULL);
    
    return;
}
/*********************************************************************\
* Function	ABPick_OnSetFocus
* Purpose   WM_SETFOCUS message handler of the main window
* Params
* Return    None
* Remarks
**********************************************************************/
static void ABPick_OnSetFocus(HWND hwnd)
{
    HWND hLst;
    
    hLst = GetDlgItem(hwnd,IDC_ABPICK_LIST);

    SetFocus(hLst);

    return;
}
/*********************************************************************\
* Function	ABPick_OnPaint
* Purpose   WM_PAINT message handler of the main window
* Params	hWnd: Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void ABPick_OnPaint(HWND hWnd)
{
	HDC hdc = BeginPaint(hWnd, NULL);

	EndPaint(hWnd, NULL);

	return;
}

/*********************************************************************\
* Function	ABPick_OnKey
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
static void ABPick_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags)
{
    PAB_PICKDATA pData;

    pData = (PAB_PICKDATA)GetUserData(hWnd);
    
    switch (vk)
	{
	case VK_F10:
        if(pData->bExtern)
            SendMessage(pData->hMsgWnd,pData->uMsgCmd,(WPARAM)FALSE,NULL);
        else
            SendMessage(pData->hMsgWnd,pData->uMsgCmd,(WPARAM)-1,(LPARAM)-1);
        PostMessage(hWnd,WM_CLOSE,NULL,NULL);
		break;

    case VK_F1://Send dial
    case VK_F5:
        {
            HWND hLst;
            int  nIndex;
            CONTACT_ITEMCHAIN* pTempData;
			
            hLst = GetDlgItem(hWnd,IDC_ABPICK_LIST);
            nIndex = SendMessage(hLst,LB_GETCURSEL,0,0);
            if(nIndex == LB_ERR)
                break;
            
            pTempData = (CONTACT_ITEMCHAIN*)SendMessage(hLst,LB_GETITEMDATA,nIndex,0);
            
            if(pData->bExtern)
            {
                strcpy(pData->name.szTelOrEmail,pTempData->pszData);

                if(AB_IsTel(pTempData->dwMask))
                    pData->name.nType = AB_NUMBER;
                else
                    pData->name.nType = AB_EMAIL;

                SendMessage(pData->hMsgWnd,pData->uMsgCmd,(WPARAM)TRUE,(LPARAM)&(pData->name));
            }
            else
                SendMessage(pData->hMsgWnd,pData->uMsgCmd,(WPARAM)pTempData->nID,(LPARAM)pData->id);

            PostMessage(hWnd,WM_CLOSE,NULL,NULL);
        }
        break;

	default:
		PDADefWindowProc(hWnd, WM_KEYDOWN, vk, MAKELPARAM(cRepeat, flags));
		break;
	}

	return;
}
/*********************************************************************\
* Function	ABPick_OnCommand
* Purpose   WM_COMMAND message handler of the main window
* Params
*			hWnd:	Handle of the window
*			id:		Id of command message
*			hwndCtl: Handle of the window which sended this message
*			codeNotify:Notify code of the message
* Return	None
* Remarks
**********************************************************************/
static LRESULT ABPick_OnCommand(HWND hWnd, int id, UINT codeNotify, LPARAM lParam)
{
	switch(id)
	{
	case IDC_ABPICK_LIST:
        if(codeNotify == LBN_SETFONT)
        {            
            HFONT hFont = NULL;
            
            GetFontHandle(&hFont, (lParam == 0) ? SMALL_FONT : LARGE_FONT);
            
            return (LRESULT)hFont;
        }
        break;

    default:
        break;
    }

	return 0;
}
/*********************************************************************\
* Function	ABPick_OnDestroy
* Purpose   WM_DESTROY message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void ABPick_OnDestroy(HWND hWnd)
{

    PAB_PICKDATA pData;

    pData = (PAB_PICKDATA)GetUserData(hWnd);
    
//    if(pData->hBmp)
//        DeleteObject(pData->hBmp);

    Item_Erase(pData->pItemData);

    return;

}
/*********************************************************************\
* Function	ABPick_OnClose
* Purpose   WM_CLOSE message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void ABPick_OnClose(HWND hWnd)
{

    PAB_PICKDATA pData;

    pData = (PAB_PICKDATA)GetUserData(hWnd);

    SendMessage(pData->hFrameWnd,PWM_CLOSEWINDOW,(WPARAM)hWnd,NULL);

    DestroyWindow (hWnd);

    return;

}












#define IDC_BACK                200
#define IDC_ABCONTACTPICK_LIST  300

typedef struct tagAB_ContactPickData
{
    HWND                  hFrameWnd;
    HWND                  hMsgWnd;
    UINT                  uMsgCmd;
    BOOL				  bContact;
    DWORD                 id;
    BOOL                  bExtern;
    AB_PICK_TYPE          nType;
    HBITMAP               hSelected;
    HBITMAP               hNormal;
	BOOL				  bExitString;
	char				  szCaption[64];
}AB_CONTACTPICKDATA,*PAB_CONTACTPICKDATA;

BOOL ABCreateContactPickerWnd(HWND hWnd,HWND hMsgWnd,const char* pCaption, UINT uMsgCmd,AB_PICK_TYPE nType,BOOL bContact,
                              DWORD id,BOOL bExtern, BOOL bExitString);
LRESULT ABContactPickWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);

static BOOL ABContactPick_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct);
static void ABContactPick_OnActivate(HWND hwnd, UINT state);
static void ABContactPick_OnSetFocus(HWND hWnd);
static void ABContactPick_OnPaint(HWND hWnd);
static void ABContactPick_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags);
static void ABContactPick_OnCommand(HWND hWnd, int id, UINT codeNotify);
static void ABContactPick_OnDestroy(HWND hWnd);
static void ABContactPick_OnClose(HWND hWnd);
static void ABContactPick_OnPick(HWND hWnd,char* szString,int nLen);

BOOL ABCreateContactPickerWnd(HWND hFrameWnd,HWND hMsgWnd,const char* pCaption, UINT uMsgCmd,AB_PICK_TYPE nType,BOOL bContact,
                              DWORD id,BOOL bExtern, BOOL bExitString)
{
    AB_CONTACTPICKDATA CreateData;
    HWND        hContactPickWnd;
    //char        szCaption[50];
    RECT        rcClient;
    
    //GetWindowText(hFrameWnd,szCaption,49);
    //szCaption[49] = 0;
	//strcpy(szCaption, IDS_SELECTCONTACT);
	memset(&CreateData,0,sizeof(AB_CONTACTPICKDATA));
	
	if(pCaption == NULL)
		strcpy(CreateData.szCaption, IDS_SELECTONECONTACT);
	else
	{
		strncpy(CreateData.szCaption, pCaption, 30);
		CreateData.szCaption[30] = 0;
	}
	SetWindowText(hFrameWnd, CreateData.szCaption);

    if(AB_GetContactCount() == 0 )
    {
        PLXTipsWin(NULL,NULL,0,IDS_CONTACTISEMPTY,IDS_SELECTCONTACT,Notify_Alert,IDS_OK,NULL,WAITTIMEOUT);

		 if(bContact == TRUE)
            SendMessage(hMsgWnd,uMsgCmd,(WPARAM)FALSE,NULL);

        if(bExtern == FALSE)
            SendMessage(hMsgWnd,uMsgCmd,(WPARAM)-1,(LPARAM)-1);
		
		return TRUE;
    }

    CreateData.hFrameWnd = hFrameWnd;
    CreateData.nType = nType;
    CreateData.hMsgWnd = hMsgWnd;
    CreateData.uMsgCmd = uMsgCmd;
	CreateData.bContact = bContact;
    CreateData.id = id;
    CreateData.bExtern = bExtern;
	CreateData.bExitString = bExitString;

    GetClientRect(hFrameWnd,&rcClient);

    hContactPickWnd = CreateWindow(
        "ABContactPickWndClass",
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
    
    if (!hContactPickWnd)
    {
        return FALSE;
    }

    SetFocus(hContactPickWnd);
    
	if(!bExitString)
		SendMessage(hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_BACK, 0), (LPARAM)IDS_CANCEL);
	else
		SendMessage(hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_BACK, 0), (LPARAM)IDS_EXIT);

    SendMessage(hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_BACK, 1), (LPARAM)"");
    SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_SELECT);
    SendMessage(hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON),(LPARAM)NULL);
    SendMessage(hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON),(LPARAM)NULL);
    return TRUE;

}

BOOL AB_ContactPickRegisterClass(void)
{
    WNDCLASS    wc;

    wc.style         = 0;
    wc.lpfnWndProc   = ABContactPickWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = sizeof(AB_CONTACTPICKDATA);
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName  = "ABContactPickWndClass";
    
    return(RegisterClass(&wc));
}

LRESULT ABContactPickWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    LRESULT lResult = TRUE;

	switch (wMsgCmd)
    {
    case WM_CREATE:
        lResult = ABContactPick_OnCreate(hWnd,(LPCREATESTRUCT)(lParam));
        break;

    case WM_ACTIVATE:
    case PWM_SHOWWINDOW:
        ABContactPick_OnActivate(hWnd,(UINT)LOWORD(wParam));
        break;

    case WM_SETFOCUS:
        ABContactPick_OnSetFocus(hWnd);
        break;

    case WM_PAINT:
        ABContactPick_OnPaint(hWnd);
        break;

    case WM_KEYDOWN:
        ABContactPick_OnKey(hWnd,(UINT)(wParam),(int)(short)LOWORD(lParam),(UINT)HIWORD(lParam));
        break;

    case WM_COMMAND:
        ABContactPick_OnCommand(hWnd,(int)(LOWORD(wParam)),(UINT)HIWORD(wParam));
        break;
        
    case WM_CLOSE:
        ABContactPick_OnClose(hWnd);
        break;

    case WM_DESTROY:
        ABContactPick_OnDestroy(hWnd);
        break;

    case WM_PICK:
        ABContactPick_OnPick(hWnd,(char*)wParam,(int)lParam);
        break;

    default:     
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
	}

    return lResult;

}
/*********************************************************************\
* Function	ABContactPick_OnCreate
* Purpose   WM_CREATE message handler of the main window
* Params
*			hWnd: Handle of the window
*			lpCreateStruct: Create Structure
* Return
*			TRUE: Success
*			FALSE: Fail
* Remarks
**********************************************************************/
static BOOL ABContactPick_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
{    
    RECT rect;
    AB_CONTACTPICKDATA *pData;
    HWND hList;
	int i,insert;
    SIZE size;
    
    pData = GetUserData(hWnd);

    memcpy(pData,lpCreateStruct->lpCreateParams,sizeof(AB_CONTACTPICKDATA));
    
    GetClientRect(hWnd,&rect);
    
    hList = CreateWindow(
        "LISTBOX",
        "",
        WS_VISIBLE | WS_CHILD | WS_VSCROLL | LBS_BITMAP ,
        rect.left,
        rect.top,
        rect.right - rect.left,
        rect.bottom - rect.top,
        hWnd,
        (HMENU)IDC_ABCONTACTPICK_LIST,
        NULL,
        NULL);
   
    if(hList == NULL)
        return FALSE;
    
    GetImageDimensionFromFile(AB_BMP_SELECT,&size);

    pData->hSelected = LoadImage(NULL, AB_BMP_SELECT, IMAGE_BITMAP,
		size.cx, size.cy, LR_LOADFROMFILE);
    
    GetImageDimensionFromFile(AB_BMP_NORMAL,&size);

    pData->hNormal = LoadImage(NULL, AB_BMP_NORMAL, IMAGE_BITMAP,
		size.cx, size.cy, LR_LOADFROMFILE);

    
    //Init listbox data
	for(i = 1; i <= nName ; i++)
	{
		insert = SendMessage(hList,LB_ADDSTRING,-1,(LPARAM)AB_GetNameString(pIndexName[i]));
        SendMessage(hList,LB_SETITEMDATA,insert,pIndexName[i]->id);
        if(pData->id != 0 && pData->id == pIndexName[i]->id)
        {
            SendMessage(hList,LB_SETCURSEL,insert,0);
            SendMessage(hList,LB_SETIMAGE,MAKEWPARAM(IMAGE_BITMAP,insert),(LPARAM)pData->hSelected);
        }
        else
            SendMessage(hList,LB_SETIMAGE,MAKEWPARAM(IMAGE_BITMAP,insert),(LPARAM)pData->hNormal);

	}
    if(pData->id == 0 || SendMessage(hList,LB_GETCURSEL,0,0) == LB_ERR)
        SendMessage(hList,LB_SETCURSEL,0,0);
    
        
    return TRUE;
    
}
/*********************************************************************\
* Function	ABContactPick_OnActivate
* Purpose   WM_ACTIVATE message handler of the main window
* Params
* Return    None
* Remarks
**********************************************************************/
static void ABContactPick_OnActivate(HWND hwnd, UINT state)
{
    HWND hLst;
    AB_CONTACTPICKDATA *pData;
    
    pData = GetUserData(hwnd);

    hLst = GetDlgItem(hwnd,IDC_ABCONTACTPICK_LIST);

    SetFocus(hLst);
 
	SetWindowText(pData->hFrameWnd, pData->szCaption);

	if(pData->bExitString)
		SendMessage(pData->hFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_EXIT);
	else
		SendMessage(pData->hFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_CANCEL);

    SendMessage(pData->hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
    SendMessage(pData->hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_SELECT);

    SendMessage(pData->hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON),(LPARAM)NULL);
    SendMessage(pData->hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON),(LPARAM)NULL);
    
    return;
}
/*********************************************************************\
* Function	ABContactPick_OnSetFocus
* Purpose   WM_PAINT message handler of the main window
* Params	hWnd: Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void ABContactPick_OnSetFocus(HWND hWnd)
{
    HWND hLst;

    hLst = GetDlgItem(hWnd,IDC_ABCONTACTPICK_LIST);

    SetFocus(hLst);
}
/*********************************************************************\
* Function	ABContactPick_OnPaint
* Purpose   WM_PAINT message handler of the main window
* Params	hWnd: Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void ABContactPick_OnPaint(HWND hWnd)
{
	HDC hdc = BeginPaint(hWnd, NULL);

	EndPaint(hWnd, NULL);

	return;
}

/*********************************************************************\
* Function	ABContactPick_OnKey
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
static void ABContactPick_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags)
{
    PAB_CONTACTPICKDATA pData;

    pData = (PAB_CONTACTPICKDATA)GetUserData(hWnd);
    
    switch (vk)
	{
	case VK_F10:
        if(pData->bContact == TRUE)
            SendMessage(pData->hMsgWnd,pData->uMsgCmd,(WPARAM)FALSE,NULL);

        if(pData->bExtern == FALSE)
            SendMessage(pData->hMsgWnd,pData->uMsgCmd,(WPARAM)-1,(LPARAM)-1);

		PostMessage(hWnd,WM_CLOSE,NULL,NULL);
		break;

    case VK_F5:
        {
			if(pData->bContact == FALSE)
			{
				HWND hList;
				CONTACT_ITEMCHAIN* pItem;
				int nTelID;
				int  nIndex;
				
				hList = GetDlgItem(hWnd,IDC_ABCONTACTPICK_LIST);
				
				nIndex = SendMessage(hList,LB_GETCURSEL,0,0);

				pItem = NULL;

				AB_ReadRecord(pIndexName[nIndex+1]->dwoffset,&pItem,&nTelID);
				
				AB_SelectPhoneOrEmail(pData->hFrameWnd,pData->hMsgWnd,pData->uMsgCmd,pItem,pData->nType,
                    pIndexName[nIndex+1]->id,TRUE,pData->bExtern);
				
				Item_Erase(pItem);

				PostMessage(hWnd,WM_CLOSE,0,0);
			}
			else
			{
				HWND hList;
				int  nIndex;
				
				hList = GetDlgItem(hWnd,IDC_ABCONTACTPICK_LIST);
				
				nIndex = SendMessage(hList,LB_GETCURSEL,0,0);

				SendMessage(pData->hMsgWnd,pData->uMsgCmd,TRUE,(LPARAM)(pIndexName[nIndex+1]->id));

				PostMessage(hWnd,WM_CLOSE,0,0);
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
* Function	ABContactPick_OnCommand
* Purpose   WM_COMMAND message handler of the main window
* Params
*			hWnd:	Handle of the window
*			id:		Id of command message
*			hwndCtl: Handle of the window which sended this message
*			codeNotify:Notify code of the message
* Return	None
* Remarks
**********************************************************************/
static void ABContactPick_OnCommand(HWND hWnd, int id, UINT codeNotify)
{
	switch(id)
	{
    default:
        break;
    }
}
/*********************************************************************\
* Function	ABContactPick_OnDestroy
* Purpose   WM_DESTROY message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void ABContactPick_OnDestroy(HWND hWnd)
{

    PAB_CONTACTPICKDATA pData;

    pData = (PAB_CONTACTPICKDATA)GetUserData(hWnd);
    
    if(pData->hSelected)
        DeleteObject(pData->hSelected);

    if(pData->hNormal)
        DeleteObject(pData->hNormal);

    return;

}
/*********************************************************************\
* Function	ABContactPick_OnClose
* Purpose   WM_CLOSE message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void ABContactPick_OnClose(HWND hWnd)
{
    PAB_CONTACTPICKDATA pData;

    pData = (PAB_CONTACTPICKDATA)GetUserData(hWnd);

    SendMessage(pData->hFrameWnd,PWM_CLOSEWINDOW,(WPARAM)hWnd,NULL);

    DestroyWindow (hWnd);

    return;

}
/*********************************************************************\
* Function	ABContactPick_OnPick
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
static void ABContactPick_OnPick(HWND hWnd,char* szString,int nLen)
{
    return;
}










#define IDC_OK		            100
#define IDC_CANCEL              200
#define IDC_ABMULITPICK_LIST	300

typedef struct tagAB_MultiPickData
{
    HWND                  hFrameWnd;
    HWND                  hMsgWnd;
    UINT                  uMsgCmd;
	int					  nGroup;
	int					  nCount;
	DWORD*			      nId;
}AB_MULTIPICKDATA,*PAB_MULTIPICKDATA;

BOOL ABCreateMultiPickerWnd(HWND hWnd,HWND hMsgWnd,UINT uMsgCmd,char* pszCaption ,int nCount,DWORD nId[],int nGroup,const char* szLeft,const char* szRight);
LRESULT ABMultiPickWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);

static BOOL ABMultiPick_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct);
static void ABMultiPick_OnActivate(HWND hwnd, UINT state);
static void ABMultiPick_OnSetFocus(HWND hWnd);
static void ABMultiPick_OnPaint(HWND hWnd);
static void ABMultiPick_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags);
static void ABMultiPick_OnCommand(HWND hWnd, int id, UINT codeNotify);
static void ABMultiPick_OnDestroy(HWND hWnd);
static void ABMultiPick_OnClose(HWND hWnd);
static void ABMultiPick_OnPick(HWND hWnd,char* szString,int nLen);
static void ABMultiPick_OnInitSelect(HWND hWnd);

//need to refresh according to contact update
BOOL ABCreateMultiPickerWnd(HWND hFrameWnd,HWND hMsgWnd,UINT uMsgCmd,char* pszCaption,int nCount,DWORD nId[],int nGroup,const char* szLeft,const char* szRight)
{
    AB_MULTIPICKDATA CreateData;
    HWND        hMultiPickWnd;
    char        szCaption[50];
    RECT        rcClient;
    
    if(pszCaption == NULL)
    {
        GetWindowText(hFrameWnd,szCaption,49);
        szCaption[49] = 0;
    }
    else
    {
        strncpy(szCaption,pszCaption,49);
        szCaption[49] = 0;
    }

    if(nGroup == -2)
    {
        PCONTACT_INITDATA pPhoneBook;
        
        pPhoneBook = AB_GetSIMData();

        if(pPhoneBook->nNumber == 0)
        {
            PLXTipsWin(NULL,NULL,0,IDS_NOPHONENUMS,szCaption,Notify_Alert,IDS_OK,NULL,WAITTIMEOUT);
            return TRUE;
        }
    }
    else if(AB_GetContactCount() == 0 )
    {
        PLXTipsWin(NULL,NULL,0,IDS_NOPHONENUMS,szCaption,Notify_Alert,IDS_OK,NULL,WAITTIMEOUT);
        return TRUE;
    }
    
    memset(&CreateData,0,sizeof(AB_MULTIPICKDATA));

    CreateData.hFrameWnd = hFrameWnd;
    CreateData.nId = nId;
    CreateData.nCount = nCount;
    CreateData.nGroup = nGroup;
    CreateData.hMsgWnd = hMsgWnd;
    CreateData.uMsgCmd = uMsgCmd;

    GetClientRect(hFrameWnd,&rcClient);

    hMultiPickWnd = CreateWindow(
        "ABMultiPickWndClass",
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
    
    if (!hMultiPickWnd)
    {
        return FALSE;
    }

    SetFocus(hMultiPickWnd);
     
    SetWindowText(hFrameWnd,szCaption);
    SendMessage(hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_OK, 1), (LPARAM)szLeft);
    SendMessage(hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_BACK, 0), (LPARAM)szRight);
    SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_SELECT);
    SendMessage(hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON),(LPARAM)NULL);
    SendMessage(hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON),(LPARAM)NULL);
    
    return TRUE;

}

BOOL AB_MultiPickRegisterClass(void)
{
    WNDCLASS    wc;

    wc.style         = 0;
    wc.lpfnWndProc   = ABMultiPickWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = sizeof(AB_MULTIPICKDATA);
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName  = "ABMultiPickWndClass";
    
    return(RegisterClass(&wc));
}

LRESULT ABMultiPickWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    LRESULT lResult = TRUE;

	switch (wMsgCmd)
    {
    case WM_CREATE:
        lResult = ABMultiPick_OnCreate(hWnd,(LPCREATESTRUCT)(lParam));
        break;

    case WM_ACTIVATE:
    case PWM_SHOWWINDOW:
        ABMultiPick_OnActivate(hWnd,(UINT)LOWORD(wParam));
        break;

    case WM_SETFOCUS:
        ABMultiPick_OnSetFocus(hWnd);
        break;

    case WM_PAINT:
        ABMultiPick_OnPaint(hWnd);
        break;

    case WM_KEYDOWN:
        ABMultiPick_OnKey(hWnd,(UINT)(wParam),(int)(short)LOWORD(lParam),(UINT)HIWORD(lParam));
        break;

    case WM_COMMAND:
        ABMultiPick_OnCommand(hWnd,(int)(LOWORD(wParam)),(UINT)HIWORD(wParam));
        break;
        
    case WM_CLOSE:
        ABMultiPick_OnClose(hWnd);
        break;

    case WM_DESTROY:
        ABMultiPick_OnDestroy(hWnd);
        break;

	case WM_INITSELECT:
        ABMultiPick_OnInitSelect(hWnd);
		break;

    default:     
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
	}

    return lResult;

}
/*********************************************************************\
* Function	ABMultiPick_OnCreate
* Purpose   WM_CREATE message handler of the main window
* Params
*			hWnd: Handle of the window
*			lpCreateStruct: Create Structure
* Return
*			TRUE: Success
*			FALSE: Fail
* Remarks
**********************************************************************/
static BOOL ABMultiPick_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
{    
    RECT rect;
    AB_MULTIPICKDATA *pData;
    HWND hList;
	int i,index;
    
    pData = GetUserData(hWnd);

    memcpy(pData,lpCreateStruct->lpCreateParams,sizeof(AB_MULTIPICKDATA));
    
    GetClientRect(hWnd,&rect);
    
    hList = CreateWindow(
        "MULTILISTBOX",
        "",
        WS_VISIBLE | WS_CHILD | WS_VSCROLL | LBS_BITMAP,
        rect.left,
        rect.top,
        rect.right - rect.left,
        rect.bottom - rect.top,
        hWnd,
        (HMENU)IDC_ABMULITPICK_LIST,
        NULL,
        NULL);
   
    if(hList == NULL)
        return FALSE;
    
    //Init listbox data
    if(pData->nGroup == -2)
    {
        PCONTACT_INITDATA pPhoneBook;
        int i,j,nScan;
        PPHONEBOOK pPB;
        
        pPhoneBook = AB_GetSIMData();
        
        for(i = 0, nScan = 0; i < pPhoneBook->nTotalNum ; i++)
        {
            if(pPhoneBook->pSIMPhoneBook[i].Index == 0)
                continue;
            
            for(j = 0 ; j < nScan ; j++)
            {
                pPB = (PPHONEBOOK)SendMessage(hList, LB_GETITEMDATA,j,0);
                if(stricmp(pPB->Text,pPhoneBook->pSIMPhoneBook[i].Text) > 0)
                    break;
            }
            SendMessage(hList, LB_INSERTSTRING, j, (LPARAM)pPhoneBook->pSIMPhoneBook[i].Text);
            SendMessage(hList, LB_SETITEMDATA, j, (LPARAM)&(pPhoneBook->pSIMPhoneBook[i]));
            
            nScan++;
            
            if(nScan == pPhoneBook->nNumber)
                break;
        }
    }
    else
    {
        for(i = 1 ; i <= nName ; i++)
        {
            if(pData->nGroup != -1)
            {
                if(pIndexName[i]->nGroup != pData->nGroup)
                    continue;
            }
            index = SendMessage(hList,LB_ADDSTRING,-1,(LPARAM)AB_GetNameString(pIndexName[i]));
            SendMessage(hList,LB_SETITEMDATA,index,(LPARAM)pIndexName[i]->id);
        }
    }

	SendMessage(hList, LB_ENDINIT, 0, 0);
	SendMessage(hList,LB_SETCURSEL,0,0);

	PostMessage(hWnd,WM_INITSELECT,0,0);

            
    return TRUE;
    
}
/*********************************************************************\
* Function	ABMultiPick_OnActivate
* Purpose   WM_ACTIVATE message handler of the main window
* Params
* Return    None
* Remarks
**********************************************************************/
static void ABMultiPick_OnActivate(HWND hwnd, UINT state)
{
    HWND hLst;

    hLst = GetDlgItem(hwnd,IDC_ABMULITPICK_LIST);

    SetFocus(hLst);

    return;
}
/*********************************************************************\
* Function	ABMultiPick_OnSetFocus
* Purpose   WM_SETFOCUS message handler of the main window
* Params
* Return    None
* Remarks
**********************************************************************/
static void ABMultiPick_OnSetFocus(HWND hWnd)
{
    HWND hLst;

    hLst = GetDlgItem(hWnd,IDC_ABMULITPICK_LIST);

    SetFocus(hLst);
}
/*********************************************************************\
* Function	ABMultiPick_OnPaint
* Purpose   WM_PAINT message handler of the main window
* Params	hWnd: Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void ABMultiPick_OnPaint(HWND hWnd)
{
	HDC hdc = BeginPaint(hWnd, NULL);

	EndPaint(hWnd, NULL);

	return;
}

/*********************************************************************\
* Function	ABMultiPick_OnKey
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
static void ABMultiPick_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags)
{
    PAB_MULTIPICKDATA pData;

    pData = (PAB_MULTIPICKDATA)GetUserData(hWnd);
    
    switch (vk)
	{
	case VK_F10:
		SendMessage(pData->hMsgWnd,pData->uMsgCmd,MAKEWPARAM(FALSE,0),0);
		
		PostMessage(hWnd,WM_CLOSE,NULL,NULL);
		break;

	case VK_RETURN:
		{
			HWND hLst;
			int  nCount,i,j;

			hLst = GetDlgItem(hWnd,IDC_ABMULITPICK_LIST);

			nCount = SendMessage(hLst,LB_GETCOUNT,0,0);

			for(i = 0,j = 0; i < nCount ; i++)
			{
				if(SendMessage(hLst,LB_GETSEL,i,0))
				{
					pData->nId[j++] = SendMessage(hLst,LB_GETITEMDATA,i,0);
				}
			}

			if(j == 0)
				break;

			SendMessage(pData->hMsgWnd,pData->uMsgCmd,MAKEWPARAM(TRUE,j),(LPARAM)pData->nId);

			PostMessage(hWnd,WM_CLOSE,NULL,NULL);
		}
		break;

	default:
		PDADefWindowProc(hWnd, WM_KEYDOWN, vk, MAKELPARAM(cRepeat, flags));
		break;
	}

	return;
}
/*********************************************************************\
* Function	ABMultiPick_OnCommand
* Purpose   WM_COMMAND message handler of the main window
* Params
*			hWnd:	Handle of the window
*			id:		Id of command message
*			hwndCtl: Handle of the window which sended this message
*			codeNotify:Notify code of the message
* Return	None
* Remarks
**********************************************************************/
static void ABMultiPick_OnCommand(HWND hWnd, int id, UINT codeNotify)
{
	switch(id)
	{
    default:
        break;
    }
}
/*********************************************************************\
* Function	ABMultiPick_OnDestroy
* Purpose   WM_DESTROY message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void ABMultiPick_OnDestroy(HWND hWnd)
{

    return;

}
/*********************************************************************\
* Function	ABMultiPick_OnClose
* Purpose   WM_CLOSE message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void ABMultiPick_OnClose(HWND hWnd)
{
    PAB_MULTIPICKDATA pData;

    pData = (PAB_MULTIPICKDATA)GetUserData(hWnd);

    SendMessage(pData->hFrameWnd,PWM_CLOSEWINDOW,(WPARAM)hWnd,NULL);

    DestroyWindow (hWnd);

    return;

}
/*********************************************************************\
* Function	ABMultiPick_OnInitSelect
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
static void ABMultiPick_OnInitSelect(HWND hWnd)
{
	int i,j;
    PAB_MULTIPICKDATA pData;
	HWND hList;
			
	hList = GetDlgItem(hWnd,IDC_ABMULITPICK_LIST);

    pData = (PAB_MULTIPICKDATA)GetUserData(hWnd);

    if(pData->nGroup == -2)
        return;

	for(i = 1 ,j = 0; i <= pData->nCount ; i++)
	{
		if(pData->nGroup == -1 && pIndexName[i]->id == pData->nId[j])
		{
			SendMessage(hList,LB_SETSEL,(WPARAM)TRUE,(LPARAM)(i-1));
			j++;
		}
	}
	SendMessage(hList,LB_SETCURSEL,0,0);
}

#define IDC_OK		                    100
#define IDC_CANCEL                      200
#define IDC_ABMULITNOOREMALPICK_LIST	300

typedef struct tagAB_NOOrEmailID
{
	DWORD		          Id;
	int					  nTelId;
}AB_NOOREMAILID,*PAB_NOOREMAILID;

typedef struct tagAB_MultiData
{
	AB_NOOREMAILID*		  pId;
    ABNAMEOREMAIL*        pArray;
    int                   nCount;
    int                   nMax;
}AB_MULTIDATA,*PAB_MULTIDATA;

typedef struct tagAB_MultiNoOrEmailPickData
{
    HWND                  hFrameWnd;
    HWND                  hMsgWnd;
    UINT                  uMsgCmd;
	AB_PICK_TYPE		  nType;
    BOOL                  bDefault;
    AB_MULTIDATA          MultiData;
    ABNAMEOREMAIL*        pArray;
	PVOID				  handle;
    WNDPROC               MultiListProc;
}AB_MULTINOOREMAILPICKDATA,*PAB_MULTINOOREMAILPICKDATA;

LRESULT ABMultiNoOrEmailPickWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);

static BOOL ABMultiNoOrEmailPick_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct);
static void ABMultiNoOrEmailPick_OnActivate(HWND hwnd, UINT state);
static void ABMultiNoOrEmailPick_OnPaint(HWND hWnd);
static void ABMultiNoOrEmailPick_OnSetFocus(HWND hWnd);
static void ABMultiNoOrEmailPick_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags);
static void ABMultiNoOrEmailPick_OnCommand(HWND hWnd, int id, UINT codeNotify);
static void ABMultiNoOrEmailPick_OnDestroy(HWND hWnd);
static void ABMultiNoOrEmailPick_OnClose(HWND hWnd);
static void ABMultiNoOrEmailPick_OnPick(HWND hWnd,char* szString,int nLen);
static void ABMultiNoOrEmailPick_OnInitSelect(HWND hWnd);
static LRESULT CallMultiListWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static void ABMultiNoOrEmailPick_OnUpdate(HWND hWnd,BOOL bModify,AB_MULTIDATA* pMultiData);
static void ABMultiNoOrEmailPick_OnDataChange(HWND hWnd,int nPos,int nMode);
static BOOL AB_Conversion(AB_NOOREMAILID* pId,ABNAMEOREMAIL* pArray,int nMax);

static BOOL ABCreateMultiSelectPicker(HWND hFrameWnd,HWND hMsgWnd,UINT uMsgCmd,char* pszCaption,
                                      CONTACT_ITEMCHAIN* pItem,AB_MULTIDATA * pMultiData,DWORD id);
//need to refresh according to contact update
BOOL ABCreateMultiNoOrEmailPickerWnd(HWND hFrameWnd,HWND hMsgWnd,UINT uMsgCmd,AB_PICK_TYPE nType,
                                     ABNAMEOREMAIL* pArray,int nCount,int nMax)
{
    AB_MULTINOOREMAILPICKDATA CreateData;
    HWND        hWnd;
    char        szCaption[50];
    RECT        rcClient;
    
    GetWindowText(hFrameWnd,szCaption,49);
    szCaption[49] = 0;

    if(AB_GetContactCount() == 0 )
    {
        PLXTipsWin(NULL,NULL,0,IDS_CONTACTISEMPTY,szCaption,Notify_Alert,IDS_OK,NULL,WAITTIMEOUT);
        return TRUE;
    }
    
    memset(&CreateData,0,sizeof(AB_MULTINOOREMAILPICKDATA));

    CreateData.hFrameWnd = hFrameWnd;
    CreateData.hMsgWnd = hMsgWnd;
    CreateData.uMsgCmd = uMsgCmd;
    CreateData.MultiData.nCount = nCount;
    CreateData.MultiData.nMax = nMax;
    CreateData.nType = nType;
    CreateData.pArray = pArray;
    CreateData.MultiData.pArray = (ABNAMEOREMAIL*)malloc(sizeof(ABNAMEOREMAIL)*nMax);
    if(CreateData.MultiData.pArray == NULL)
        return FALSE;
    CreateData.MultiData.pId = (AB_NOOREMAILID*)malloc(sizeof(AB_NOOREMAILID)*nMax);
    if(CreateData.MultiData.pId == NULL)
    {
        AB_FREE(CreateData.MultiData.pArray);
        return FALSE;
    }

    memcpy(CreateData.MultiData.pArray,pArray,sizeof(ABNAMEOREMAIL)*nMax);
    memset(CreateData.MultiData.pId,0,sizeof(AB_NOOREMAILID)*nMax);

    AB_Conversion(CreateData.MultiData.pId,pArray,min(nMax,nCount));

    GetClientRect(hFrameWnd,&rcClient);

    hWnd = CreateWindow(
        "ABMultiNoOrEmailPickWndClass",
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
    
    if (!hWnd)
    {
        AB_FREE(CreateData.MultiData.pArray);
        AB_FREE(CreateData.MultiData.pId);
        return FALSE;
    }

    SetFocus(hWnd);
    SetWindowText(hFrameWnd, IDS_SELECTCONTACT);
    SendMessage(hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_OK, 1), (LPARAM)IDS_SAVE);
    SendMessage(hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_BACK, 0), (LPARAM)IDS_CANCEL);
    SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_SELECT);
    SendMessage(hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON),(LPARAM)NULL);
    SendMessage(hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON),(LPARAM)NULL);

    return TRUE;

}

BOOL AB_MultiNoOrEmailPickRegisterClass(void)
{
    WNDCLASS    wc;

    wc.style         = 0;
    wc.lpfnWndProc   = ABMultiNoOrEmailPickWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = sizeof(AB_MULTINOOREMAILPICKDATA);
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName  = "ABMultiNoOrEmailPickWndClass";
    
    return(RegisterClass(&wc));
}

LRESULT ABMultiNoOrEmailPickWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    LRESULT lResult = TRUE;

	switch (wMsgCmd)
    {
    case WM_CREATE:
        lResult = ABMultiNoOrEmailPick_OnCreate(hWnd,(LPCREATESTRUCT)(lParam));
        break;

    case WM_ACTIVATE:
    case PWM_SHOWWINDOW:
        ABMultiNoOrEmailPick_OnActivate(hWnd,(UINT)LOWORD(wParam));
        break;

    case WM_SETFOCUS:
        ABMultiNoOrEmailPick_OnSetFocus(hWnd);
        break;

    case WM_PAINT:
        ABMultiNoOrEmailPick_OnPaint(hWnd);
        break;

    case WM_KEYDOWN:
        ABMultiNoOrEmailPick_OnKey(hWnd,(UINT)(wParam),(int)(short)LOWORD(lParam),(UINT)HIWORD(lParam));
        break;

    case WM_COMMAND:
        ABMultiNoOrEmailPick_OnCommand(hWnd,(int)(LOWORD(wParam)),(UINT)HIWORD(wParam));
        break;
        
    case WM_CLOSE:
        ABMultiNoOrEmailPick_OnClose(hWnd);
        break;

    case WM_DESTROY:
        ABMultiNoOrEmailPick_OnDestroy(hWnd);
        break;

	case WM_INITSELECT:
        ABMultiNoOrEmailPick_OnInitSelect(hWnd);
		break;

    case WM_UPDATE:
        ABMultiNoOrEmailPick_OnUpdate(hWnd,(BOOL)wParam,(AB_MULTIDATA*)lParam);
        break;
		
    case WM_DATACHANGE:
        ABMultiNoOrEmailPick_OnDataChange(hWnd,(int)wParam,(int)lParam);
        break;

    default:     
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
	}

    return lResult;

}
/*********************************************************************\
* Function	ABMultiNoOrEmailPick_OnCreate
* Purpose   WM_CREATE message handler of the main window
* Params
*			hWnd: Handle of the window
*			lpCreateStruct: Create Structure
* Return
*			TRUE: Success
*			FALSE: Fail
* Remarks
**********************************************************************/
static BOOL ABMultiNoOrEmailPick_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
{    
    RECT rect;
    AB_MULTINOOREMAILPICKDATA *pData;
    HWND hList;
	int i,index;
    
    pData = GetUserData(hWnd);

    memcpy(pData,lpCreateStruct->lpCreateParams,sizeof(AB_MULTINOOREMAILPICKDATA));
    
    GetClientRect(hWnd,&rect);
    
    hList = CreateWindow(
        "MULTILISTBOX",
        "",
        WS_VISIBLE | WS_CHILD | WS_VSCROLL | LBS_BITMAP,
        rect.left,
        rect.top,
        rect.right - rect.left,
        rect.bottom - rect.top,
        hWnd,
        (HMENU)IDC_ABMULITPICK_LIST,
        NULL,
        NULL);
   
    if(hList == NULL)
        return FALSE;

    pData->MultiListProc = (WNDPROC)SetWindowLong(hList,GWL_WNDPROC,(LONG)CallMultiListWndProc);
    
    //Init listbox data
    for(i = 1 ; i <= nName ; i++)
    {
        index = SendMessage(hList,LB_ADDSTRING,-1,(LPARAM)AB_GetNameString(pIndexName[i]));
        SendMessage(hList,LB_SETITEMDATA,index,(LPARAM)pIndexName[i]->id);
    }

	SendMessage(hList, LB_ENDINIT, 0, 0);
	SendMessage(hList,LB_SETCURSEL,0,0);

	PostMessage(hWnd,WM_INITSELECT,0,0);
            
	pData->handle = AB_RegisterNotify(hWnd,WM_DATACHANGE,AB_OBJECT_NAME,AB_MDU_CONTRACT);
	
    return TRUE;
    
}
/*********************************************************************\
* Function	ABMultiNoOrEmailPick_OnActivate
* Purpose   WM_ACTIVATE message handler of the main window
* Params
* Return    None
* Remarks
**********************************************************************/
static void ABMultiNoOrEmailPick_OnActivate(HWND hwnd, UINT state)
{
    HWND hLst;
    AB_MULTINOOREMAILPICKDATA *pData;
    
    pData = GetUserData(hwnd);

    hLst = GetDlgItem(hwnd,IDC_ABMULITPICK_LIST);

    SetFocus(hLst);

	SetWindowText(pData->hFrameWnd, IDS_SELECTCONTACT);
    SendMessage(pData->hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_SAVE);
    SendMessage(pData->hFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_CANCEL);
    SendMessage(pData->hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_SELECT);

    return;
}

/*********************************************************************\
* Function	ABMultiNoOrEmailPick_OnPaint
* Purpose   WM_PAINT message handler of the main window
* Params	hWnd: Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void ABMultiNoOrEmailPick_OnSetFocus(HWND hWnd)
{
    HWND hLst;

    hLst = GetDlgItem(hWnd,IDC_ABMULITPICK_LIST);

    SetFocus(hLst);

}
/*********************************************************************\
* Function	ABMultiNoOrEmailPick_OnPaint
* Purpose   WM_PAINT message handler of the main window
* Params	hWnd: Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void ABMultiNoOrEmailPick_OnPaint(HWND hWnd)
{
	HDC hdc = BeginPaint(hWnd, NULL);

	EndPaint(hWnd, NULL);

	return;
}

/*********************************************************************\
* Function	ABMultiNoOrEmailPick_OnKey
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
static void ABMultiNoOrEmailPick_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags)
{
    AB_MULTINOOREMAILPICKDATA* pData;

    pData = (AB_MULTINOOREMAILPICKDATA*)GetUserData(hWnd);
    
    switch (vk)
	{
	case VK_F10:
		SendMessage(pData->hMsgWnd,pData->uMsgCmd,MAKEWPARAM(FALSE,pData->MultiData.nCount),(LPARAM)pData->pArray);
		
		PostMessage(hWnd,WM_CLOSE,NULL,NULL);
		break;

    case VK_RETURN:
        memcpy(pData->pArray,pData->MultiData.pArray,sizeof(ABNAMEOREMAIL)*pData->MultiData.nMax);

		SendMessage(pData->hMsgWnd,pData->uMsgCmd,MAKEWPARAM(TRUE,min(pData->MultiData.nCount,pData->MultiData.nMax))
            ,(LPARAM)pData->pArray);
        
        PostMessage(hWnd,WM_CLOSE,NULL,NULL);
		break;

    case VK_F5:
        {
            HWND hList;
            int  index,nTelID,i,j,nCount;
            BOOL bSelect,bLowMem;         
            DWORD   id;
            CONTACT_ITEMCHAIN*    pItem;
            CONTACT_ITEMCHAIN*    pTmp;
            CONTACT_ITEMCHAIN*    pHeader;
            CONTACT_ITEMCHAIN*    pNewItem;
            MEMORY_NAME*          pName;  
            char                  szCaption[AB_MAXLEN_FIRSTNAME*2];
            char                  *p = NULL,*q = NULL,*pStr = NULL;

            if(pData->bDefault == TRUE)
                break;
            
            nCount = 0;

            pHeader = NULL;
            
            szCaption[0] = 0;

            hList = GetDlgItem(hWnd,IDC_ABMULITPICK_LIST);

            index = SendMessage(hList,LB_GETCURSEL,0,0);

            if(index == LB_ERR)
                break;

            id = SendMessage(hList,LB_GETITEMDATA,index,NULL);

            pName = (MEMORY_NAME*)Memory_Find((PVOID)id,AB_FIND_ID);

            bSelect = SendMessage(hList,LB_GETSEL,index,0);
            
            pItem = NULL;
            
            AB_ReadRecord(pName->dwoffset,&pItem,&nTelID);
            
            pTmp = pItem;

            bLowMem = FALSE;

            while(pTmp)
            {  
                if(bLowMem == TRUE)
                {
                    if(pHeader)
                        Item_Erase(pHeader);

                    break;
                }
                
                if(pTmp->dwMask == AB_TYPEMASK_FIRSTNAME )
                {
                    p = pTmp->pszData;
                }
                if(pTmp->dwMask == AB_TYPEMASK_LASTNAME)
                {
                    q = pTmp->pszData;
                }

                switch(pData->nType) 
                {
                case PICK_NUMBER:
                    switch(pTmp->dwMask)
                    {
                    case AB_TYPEMASK_TEL:
                    case AB_TYPEMASK_TELWORK:
                    case AB_TYPEMASK_TELHOME:
                    case AB_TYPEMASK_MOBILE:      
                    case AB_TYPEMASK_MOBILEHOME:  
                    case AB_TYPEMASK_MOBILEWORK:
                    case AB_TYPEMASK_COMPANYTEL:
                    case AB_TYPEMASK_FAX:
                    case AB_TYPEMASK_PAGER: 
					case AB_TYPEMASK_DTMF:
                        pNewItem = Item_New(pTmp->dwMask);
                        
                        if(pNewItem == NULL)
                        {
                            bLowMem = TRUE;
                            break;
                        }
                        
                        pNewItem->dwMask = pTmp->dwMask;
                        pNewItem->bChange = FALSE;
                        pNewItem->nMaxLen = pTmp->nMaxLen;
                        strcpy(pNewItem->pszData,pTmp->pszData);
                        pNewItem->pPioneer = NULL;
                        pNewItem->pNext = NULL;
                        pNewItem->nID = pTmp->nID;

                        nCount++;
                        
//                        if(nCount == 1 && (AB_IsTel(pTmp->dwMask) || AB_IsEmail(pTmp->dwMask)))
//                            nTelID = pNewItem->nID;
                        Item_Insert(&pHeader,pNewItem);
                        break;    
                        
                    default:
                        break;
                    }
                    break;
                    
                case PICK_EMAIL:            
                    switch(pTmp->dwMask)
                    {
                    case AB_TYPEMASK_EMAIL:
                    case AB_TYPEMASK_EMAILWORK:
                    case AB_TYPEMASK_EMAILHOME:
                        pNewItem = Item_New(pTmp->dwMask);
                        
                        if(pNewItem == NULL)
                        {
                            bLowMem = TRUE;
                            break;
                        }
                        
                        pNewItem->dwMask = pTmp->dwMask;
                        pNewItem->bChange = FALSE;
                        pNewItem->nMaxLen = pTmp->nMaxLen;
                        strcpy(pNewItem->pszData,pTmp->pszData);
                        pNewItem->pPioneer = NULL;
                        pNewItem->pNext = NULL;
                        nCount++;
                        Item_Insert(&pHeader,pNewItem);
                        break;    
                        
                    default:
                        break;
                    }
                    break;
                        
                case PICK_NUMBERANDEMAIL:     
                    switch(pTmp->dwMask)
                    {
                    case AB_TYPEMASK_TEL:
                    case AB_TYPEMASK_TELWORK:
                    case AB_TYPEMASK_TELHOME:
                    case AB_TYPEMASK_MOBILE:      
                    case AB_TYPEMASK_MOBILEHOME:  
                    case AB_TYPEMASK_MOBILEWORK:
                    case AB_TYPEMASK_COMPANYTEL:
                    case AB_TYPEMASK_FAX:
                    case AB_TYPEMASK_PAGER:
					case AB_TYPEMASK_DTMF:
                    case AB_TYPEMASK_EMAIL:
                    case AB_TYPEMASK_EMAILWORK:
                    case AB_TYPEMASK_EMAILHOME:                
                        pNewItem = Item_New(pTmp->dwMask);
                        
                        if(pNewItem == NULL)
                        {
                            bLowMem = TRUE;
                            break;
                        }
                        
                        pNewItem->dwMask = pTmp->dwMask;
                        pNewItem->bChange = FALSE;
                        pNewItem->nMaxLen = pTmp->nMaxLen;
                        strcpy(pNewItem->pszData,pTmp->pszData);
                        pNewItem->pPioneer = NULL;
                        pNewItem->pNext = NULL;
                        pNewItem->nID = pTmp->nID;
                        nCount++;
                        //                            if(nCount == 1 && (AB_IsTel(pTmp->dwMask) || AB_IsEmail(pTmp->dwMask)))
                        //                                nTelID = pNewItem->nID;
                        Item_Insert(&pHeader,pNewItem);
                        break;    
                        
                    default:
                        break;
                    }
                    break;
                        
                default:
                    bLowMem = TRUE;
                    break;
                }
                pTmp = pTmp->pNext;
            }
    
            if(nCount == 0)
            {
                char        szCaption[50];
                char*       pszPrompt;
                
                GetWindowText(pData->hFrameWnd,szCaption,49);
                szCaption[49] = 0;
                
                switch(pData->nType) 
                {
                case PICK_NUMBER:
                    pszPrompt = (char*)IDS_NOPHONENUMS;
                    break;
                    
                case PICK_EMAIL:
                    pszPrompt = (char*)IDS_NOEMAIL;
                    break;
                    
                case PICK_NUMBERANDEMAIL:
                default:
                    pszPrompt = (char*)IDS_NOPHONENUMOREMAIL;
                    break;
                }
                
                PLXTipsWin(NULL,NULL,0,pszPrompt,szCaption,Notify_Alert,IDS_OK,NULL,WAITTIMEOUT);
                
                Item_Erase(pHeader);

                Item_Erase(pItem);
                
                break;
            }
            else if(nCount == 1)
            {
                if(bSelect == TRUE)
                {
                    for(i = 0; i < min(pData->MultiData.nCount,pData->MultiData.nMax) ; i++)
                    {
                        if(pData->MultiData.pId[i].Id == pName->id && pData->MultiData.pId[i].nTelId == pHeader->nID)
                        {
                            for(j = i; j < min(pData->MultiData.nCount,pData->MultiData.nMax)-1 ; j++)
                            {
                                pData->MultiData.pId[j] = pData->MultiData.pId[j+1];
                                pData->MultiData.pArray[j] = pData->MultiData.pArray[j+1];
                            }
                            
                            (pData->MultiData.nCount)--;
                        }
                    }
                }
                else
                {
                    if(pData->MultiData.nCount >= pData->MultiData.nMax)
                    {
                        Item_Erase(pItem);
                        
                        Item_Erase(pHeader);

                        PLXTipsWin(NULL,NULL,0,IDS_TOOMANY,NULL,Notify_Failure,IDS_OK,NULL,WAITTIMEOUT);
                        
                        break;
                    }
                    pData->MultiData.pId[pData->MultiData.nCount].Id = id;
                    pData->MultiData.pId[pData->MultiData.nCount].nTelId = pHeader->nID;

                    if(AB_IsTel(pHeader->dwMask))
                        pData->MultiData.pArray[pData->MultiData.nCount].nType = AB_NUMBER;
                    else
                        pData->MultiData.pArray[pData->MultiData.nCount].nType = AB_EMAIL;
                    
                    pStr = pData->MultiData.pArray[pData->MultiData.nCount].szName;
                    if(q == NULL)
                        strcpy(pStr,p);
                    else if(p == NULL)
                        strcpy(pStr,q);
                    else
                        sprintf(pStr,"%s %s",q,p);                
               
                    strcpy(pData->MultiData.pArray[pData->MultiData.nCount].szTelOrEmail,pHeader->pszData);

                    (pData->MultiData.nCount)++;
                }
                
                pData->bDefault = TRUE;

                SendMessage(hList,WM_KEYDOWN,VK_F5,NULL);
                
                pData->bDefault = FALSE;
                
                Item_Erase(pHeader);

                Item_Erase(pItem);

                break;
            }
            else
            {
                if(q == NULL)
                    strcpy(szCaption,p);
                else if(p == NULL)
                    strcpy(szCaption,q);
                else
                    sprintf(szCaption,"%s %s",q,p);
                //Format caption
                ABCreateMultiSelectPicker(pData->hFrameWnd,hWnd,WM_UPDATE,szCaption,pHeader,&(pData->MultiData),id);

                Item_Erase(pHeader);
                
                Item_Erase(pItem);
                
                break;
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
* Function	ABMultiNoOrEmailPick_OnCommand
* Purpose   WM_COMMAND message handler of the main window
* Params
*			hWnd:	Handle of the window
*			id:		Id of command message
*			hwndCtl: Handle of the window which sended this message
*			codeNotify:Notify code of the message
* Return	None
* Remarks
**********************************************************************/
static void ABMultiNoOrEmailPick_OnCommand(HWND hWnd, int id, UINT codeNotify)
{
	switch(id)
	{
    default:
        break;
    }
}
/*********************************************************************\
* Function	ABMultiNoOrEmailPick_OnDestroy
* Purpose   WM_DESTROY message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void ABMultiNoOrEmailPick_OnDestroy(HWND hWnd)
{
    AB_MULTINOOREMAILPICKDATA* pData;

    pData = (AB_MULTINOOREMAILPICKDATA*)GetUserData(hWnd);

    AB_FREE(pData->MultiData.pId);

    AB_FREE(pData->MultiData.pArray);
	
    AB_UnRegisterNotify(pData->handle);

    return;

}
/*********************************************************************\
* Function	ABMultiNoOrEmailPick_OnClose
* Purpose   WM_CLOSE message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void ABMultiNoOrEmailPick_OnClose(HWND hWnd)
{
    AB_MULTINOOREMAILPICKDATA *pData;
    
    pData = GetUserData(hWnd);

    SendMessage(pData->hFrameWnd,PWM_CLOSEWINDOW,(WPARAM)hWnd,NULL);

    DestroyWindow (hWnd);

    return;

}
/*********************************************************************\
* Function	ABMultiNoOrEmailPick_OnInitSelect
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
static void ABMultiNoOrEmailPick_OnInitSelect(HWND hWnd)
{
	int j,k;
    AB_MULTINOOREMAILPICKDATA* pData;
	HWND hList;

    pData = (AB_MULTINOOREMAILPICKDATA*)GetUserData(hWnd);
			
	hList = GetDlgItem(hWnd,IDC_ABMULITPICK_LIST);

    pData->bDefault = TRUE;

	for(j = 0; j < min(pData->MultiData.nCount,pData->MultiData.nMax) ; j++)
	{
        for(k = 1 ; k <= nName ; k++)
        {
            if(pIndexName[k]->id == pData->MultiData.pId[j].Id)
            {
                SendMessage(hList,LB_SETSEL,(WPARAM)TRUE,(LPARAM)(k-1));
                break;
            }
        }
	}
	SendMessage(hList,LB_SETCURSEL,0,0);

    pData->bDefault = FALSE;

}

/*********************************************************************\
* Function	AB_Conversion
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
static void ABMultiNoOrEmailPick_OnUpdate(HWND hWnd,BOOL bModify,AB_MULTIDATA* pMultiData)
{
    AB_MULTINOOREMAILPICKDATA* pData;

	HWND hList;

    int  index,i;

    DWORD id;

    BOOL bSelect,bExist;

    if(bModify == FALSE)
        return;

    pData = (AB_MULTINOOREMAILPICKDATA*)GetUserData(hWnd);

    pData->MultiData.nCount = pMultiData->nCount;
    pData->MultiData.nMax = pMultiData->nMax;
    memcpy(pData->MultiData.pArray,pMultiData->pArray,sizeof(ABNAMEOREMAIL)*pMultiData->nMax);
    memcpy(pData->MultiData.pId,pMultiData->pId,sizeof(AB_NOOREMAILID)*pMultiData->nMax);

    hList = GetDlgItem(hWnd,IDC_ABMULITPICK_LIST);

    index = SendMessage(hList,LB_GETCURSEL,0,0);

    if(index == LB_ERR)
        return;

    bSelect = SendMessage(hList,LB_GETSEL,index,0);

    id = SendMessage(hList,LB_GETITEMDATA,index,0);

    bExist = FALSE;

    for(i = 0 ; i < min(pData->MultiData.nCount,pData->MultiData.nMax) ; i++)
    {
        if(pData->MultiData.pId[i].Id == id)
        {
            bExist = TRUE;

            break;
        }
    }

    if(bExist == TRUE)
    {
        if(bSelect == FALSE)            
        {
            pData->bDefault = TRUE;
            
            SendMessage(hList,WM_KEYDOWN,VK_F5,NULL);
            
            pData->bDefault = FALSE;
        }
    }
    else
    {
        if(bSelect == TRUE)
        {
            pData->bDefault = TRUE;
            
            SendMessage(hList,WM_KEYDOWN,VK_F5,NULL);
            
            pData->bDefault = FALSE;
        }
    }

}
/*********************************************************************\
* Function	   ABMultiNoOrEmailPick_OnDataChange
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void ABMultiNoOrEmailPick_OnDataChange(HWND hWnd,int nPos,int nMode)
{
    AB_MULTINOOREMAILPICKDATA* pData;
    HWND hList;
	int nCount,i,j;
	BOOL bSelect;
    
    pData = GetUserData(hWnd);
        
	hList = GetDlgItem(hWnd,IDC_ABMULITPICK_LIST);
	
	switch(nMode)
	{
	case AB_MODE_INSERT:
		SendMessage(hList,LB_INSERTSTRING,nPos-1,(LPARAM)AB_GetNameString(pIndexName[nPos]));
		
		SendMessage(hList,LB_SETITEMDATA,nPos-1,(LPARAM)pIndexName[nPos]->id);
		
		SendMessage(hList,LB_SETCURSEL,nPos-1,NULL);

		break;
		
	case AB_MODE_DELETE:
			
		if(SendMessage(hList,LB_GETSEL,nPos-1,0))
			bSelect = TRUE;
		else
			bSelect = FALSE;

		if(bSelect == TRUE)
		{
			for(i = 0; i < min(pData->MultiData.nCount,pData->MultiData.nMax) ; i++)
			{
				if(pData->MultiData.pId[i].Id == pIndexName[nPos]->id)
				{
					for(j = i; j < min(pData->MultiData.nCount,pData->MultiData.nMax)-1 ; j++)
					{
						pData->MultiData.pId[j] = pData->MultiData.pId[j+1];
						pData->MultiData.pArray[j] = pData->MultiData.pArray[j+1];
					}
					
					(pData->MultiData.nCount)--;
				}
			}
		}

		SendMessage(hList,LB_DELETESTRING,nPos-1,NULL);
		
		nCount = SendMessage(hList,LB_GETCOUNT,0,0);

		if(nCount == 0)
			SendMessage(hList,WM_KEYDOWN,VK_F10,NULL);
		else if(nPos-1 == nCount)
			SendMessage(hList,LB_SETCURSEL,nPos-2,NULL);
		else
			SendMessage(hList,LB_SETCURSEL,nPos-1,NULL);	
		break;
		
	default:
		break;
	}
}
/*********************************************************************\
* Function	AB_Conversion
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
static BOOL AB_Conversion(AB_NOOREMAILID* pId,ABNAMEOREMAIL* pArray,int nMax)
{
    int i,j;
    MEMORY_TEL * pTel;
    MEMORY_EMAIL * pEmail;
    
//    assert(pId != NULL && pArray != NULL);

    for(i = 0,j = 0; i < nMax; i++)
    {
        switch(pArray[i].nType)
        {
        case AB_NUMBER:
            if(strlen(pArray[i].szTelOrEmail) > (AB_MAXLEN_TEL-1))
                continue;
            
            pTel = Memory_Find((PVOID)pArray[i].szTelOrEmail,AB_FIND_TEL);
            
            if(pTel == NULL)
                continue;

            pId[j].Id = pTel->id;
            pId[j].nTelId = pTel->nTelID;
            j++;
        	break;

        case AB_EMAIL:
        default:
            if(strlen(pArray[i].szTelOrEmail) > (AB_MAXLEN_EMAIL-1))
                continue;
            
            pEmail = Memory_Find((PVOID)pArray[i].szTelOrEmail,AB_FIND_EMAIL);
            
            if(pEmail == NULL)
                continue;

            pId[j].Id = pEmail->id;
            pId[j].nTelId = pEmail->nEmailID;
            j++;
            break;
        }
    }
    return TRUE;
}

/*********************************************************************\
* Function	CallMultiListWndProc
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
static LRESULT CallMultiListWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    LRESULT lResult = TRUE;
    AB_MULTINOOREMAILPICKDATA* pData;
    HWND hParent;

    hParent = GetParent(hWnd);
    
    pData = (AB_MULTINOOREMAILPICKDATA*)GetUserData(hParent);
    
    switch(wMsgCmd)
    {         
    case WM_KEYDOWN:
        switch (LOWORD(wParam))
        { 
        case VK_F5:
            if(pData->bDefault == FALSE)
            {
                PostMessage(hParent,wMsgCmd,wParam,lParam);
                return lResult;
            }
            return CallWindowProc(pData->MultiListProc, hWnd, wMsgCmd, wParam, lParam);
              
        default:
            return CallWindowProc(pData->MultiListProc, hWnd, wMsgCmd, wParam, lParam);
        }
        break;
        
    default:
        return CallWindowProc(pData->MultiListProc, hWnd, wMsgCmd, wParam, lParam);
    }
    return lResult;       
}



BOOL AB_MultiSelectPickRegisterClass(void);
LRESULT ABMultiSelectPickWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static BOOL ABMultiSelectPick_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct);
static void ABMultiSelectPick_OnActivate(HWND hwnd, UINT state);
static void ABMultiSelectPick_OnPaint(HWND hWnd);
static void ABMultiSelectPick_OnSetFocus(HWND hwnd);
static void ABMultiSelectPick_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags);
static LRESULT ABMultiSelectPick_OnCommand(HWND hWnd, int id, UINT codeNotify, LPARAM lParam);
static void ABMultiSelectPick_OnDestroy(HWND hWnd);
static void ABMultiSelectPick_OnClose(HWND hWnd);



typedef struct tagAB_MultiSelectPickData
{
    HWND                  hFrameWnd;
    HWND                  hMsgWnd;
    UINT                  uMsgCmd;
    AB_MULTIDATA          MultiData;
    DWORD                 id;
    CONTACT_ITEMCHAIN*    pItem;
    HBITMAP               hSelected;
    HBITMAP               hUnselect;
}AB_MULTISELECTPICKDATA,*PAB_MULTISELECTPICKDATA;


BOOL AB_MultiSelectPickRegisterClass(void)
{
    WNDCLASS    wc;

    wc.style         = 0;
    wc.lpfnWndProc   = ABMultiSelectPickWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = sizeof(AB_MULTISELECTPICKDATA);
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName  = "ABMultiSelectPickWndClass";
    
    return(RegisterClass(&wc));
}


static BOOL ABCreateMultiSelectPicker(HWND hFrameWnd,HWND hMsgWnd,UINT uMsgCmd,char* pszCaption,
                                      CONTACT_ITEMCHAIN* pItem,AB_MULTIDATA * pMultiData,DWORD id)
{
    AB_MULTISELECTPICKDATA CreateData;
    HWND        hPickWnd;
    RECT        rcClient;
    
    memset(&CreateData,0,sizeof(AB_MULTISELECTPICKDATA));

    CreateData.hFrameWnd = hFrameWnd;
    CreateData.hMsgWnd = hMsgWnd;
    CreateData.uMsgCmd = uMsgCmd;
    CreateData.id = id;
	CreateData.MultiData.nCount = pMultiData->nCount;
    CreateData.MultiData.nMax = pMultiData->nMax;
    CreateData.MultiData.pId = (AB_NOOREMAILID*)malloc(pMultiData->nMax*sizeof(AB_NOOREMAILID));
    if(CreateData.MultiData.pId == NULL)
        return FALSE;
    CreateData.MultiData.pArray = (ABNAMEOREMAIL*)malloc(pMultiData->nMax*sizeof(ABNAMEOREMAIL));
    if(CreateData.MultiData.pArray == NULL)
    {
        AB_FREE(CreateData.MultiData.pId);
        return FALSE;
    }
    memcpy(CreateData.MultiData.pId,pMultiData->pId,pMultiData->nMax*sizeof(AB_NOOREMAILID));
    memcpy(CreateData.MultiData.pArray,pMultiData->pArray,pMultiData->nMax*sizeof(ABNAMEOREMAIL));
    
    if(AB_CopyItem(&(CreateData.pItem),pItem) == FALSE)
    {
        AB_FREE(CreateData.MultiData.pArray);
        AB_FREE(CreateData.MultiData.pId);
        return FALSE;
    }

    GetClientRect(hFrameWnd,&rcClient);

    hPickWnd = CreateWindow(
        "ABMultiSelectPickWndClass",
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
    
    if (!hPickWnd)
    {
        AB_FREE(CreateData.MultiData.pArray);
        AB_FREE(CreateData.MultiData.pId);
        Item_Erase(CreateData.pItem);
        return FALSE;
    }

    SetFocus(hPickWnd);

    SetWindowText(hFrameWnd,pszCaption);
    
//    SendMessage(hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_OK, 1), (LPARAM)IDS_SELECT);
	SendMessage(hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_OK, 1), (LPARAM)"");
    SendMessage(hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_BACK, 0), (LPARAM)IDS_BACK);
    SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_SELECT);
    SendMessage(hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON),(LPARAM)NULL);
    SendMessage(hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON),(LPARAM)NULL);

    return TRUE;
}

LRESULT ABMultiSelectPickWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    LRESULT lResult = TRUE;

	switch (wMsgCmd)
    {
    case WM_CREATE:
        lResult = ABMultiSelectPick_OnCreate(hWnd,(LPCREATESTRUCT)(lParam));
        break;

    case WM_ACTIVATE:
    case PWM_SHOWWINDOW:
        ABMultiSelectPick_OnActivate(hWnd,(UINT)LOWORD(wParam));
        break;

    case WM_SETFOCUS:
        ABMultiSelectPick_OnSetFocus(hWnd);
        break;

    case WM_PAINT:
        ABMultiSelectPick_OnPaint(hWnd);
        break;

    case WM_KEYDOWN:
        ABMultiSelectPick_OnKey(hWnd,(UINT)(wParam),(int)(short)LOWORD(lParam),(UINT)HIWORD(lParam));
        break;

    case WM_COMMAND:
        lResult = ABMultiSelectPick_OnCommand(hWnd,(int)(LOWORD(wParam)),(UINT)HIWORD(wParam), lParam);
        break;
        
    case WM_CLOSE:
        ABMultiSelectPick_OnClose(hWnd);
        break;

    case WM_DESTROY:
        ABMultiSelectPick_OnDestroy(hWnd);
        break;

    default:     
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
	}

    return lResult;

}
/*********************************************************************\
* Function	ABMultiSelectPick_OnCreate
* Purpose   WM_CREATE message handler of the main window
* Params
*			hWnd: Handle of the window
*			lpCreateStruct: Create Structure
* Return
*			TRUE: Success
*			FALSE: Fail
* Remarks
**********************************************************************/
static BOOL ABMultiSelectPick_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
{    
    RECT rect;
    AB_MULTISELECTPICKDATA *pData;
    HWND hList;
    CONTACT_ITEMCHAIN* pTempData;
    int  nIndex,nOffset,i;
    BOOL bExist;
	BOOL bFocus = FALSE;
    
    pData = GetUserData(hWnd);

    memcpy(pData,lpCreateStruct->lpCreateParams,sizeof(AB_MULTISELECTPICKDATA));
    
    GetClientRect(hWnd,&rect);
    
    hList = CreateWindow(
        "LISTBOX",
        "",
        WS_VISIBLE | WS_CHILD | WS_VSCROLL | LBS_BITMAP | LBS_MULTILINE,
        rect.left,
        rect.top,
        rect.right - rect.left,
        rect.bottom - rect.top,
        hWnd,
        (HMENU)IDC_ABPICK_LIST,
        NULL,
        NULL);
   
    if(hList == NULL)
        return FALSE;

    pTempData = pData->pItem;
    
    pData->hSelected = LoadImage(NULL, AB_BMP_CBSELECT, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    pData->hUnselect = LoadImage(NULL, AB_BMP_CBNORMAL, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    
    while(pTempData)
    {
        bExist = FALSE;
        
        nOffset = GetIndexByMask(pTempData->dwMask);

        if(nOffset == -1)
            return FALSE;

        nIndex = SendMessage(hList, LB_ADDSTRING, (WPARAM)-1, (LPARAM) (LPCTSTR)Contact_Item[nOffset].pszCpation);
        SendMessage(hList, LB_SETAUXTEXT, MAKEWPARAM(nIndex, -1), (LPARAM)pTempData->pszData);
        SendMessage(hList, LB_SETITEMDATA, nIndex, (LPARAM)pTempData);
                
        for(i = 0 ; i < min(pData->MultiData.nCount,pData->MultiData.nMax); i++)
        {
            if(pData->MultiData.pId[i].Id == pData->id && pData->MultiData.pId[i].nTelId == pTempData->nID)
            {
                bExist = TRUE;
                
                break;
            }
        }
        
//        if(bExist)
//            SendMessage(hList, LB_SETIMAGE, (WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)nIndex),(LPARAM)pData->hSelected);
//        else
//            SendMessage(hList, LB_SETIMAGE, (WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)nIndex),(LPARAM)pData->hUnselect);
		if(bExist)
		{
			bFocus = TRUE;
			SendMessage(hList, LB_SETCURSEL, nIndex, 0);
		}
    
        pTempData = pTempData->pNext;
    }

	if(!bFocus)
		SendMessage(hList, LB_SETCURSEL, 0, 0);
        
    return TRUE;
    
}
/*********************************************************************\
* Function	ABMultiSelectPick_OnActivate
* Purpose   WM_ACTIVATE message handler of the main window
* Params
* Return    None
* Remarks
**********************************************************************/
static void ABMultiSelectPick_OnActivate(HWND hwnd, UINT state)
{
    HWND hLst;
    AB_MULTISELECTPICKDATA *pData;
    
    pData = GetUserData(hwnd);

    hLst = GetDlgItem(hwnd,IDC_ABPICK_LIST);

    SetFocus(hLst);

//    SendMessage(pData->hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_SELECT);
	SendMessage(pData->hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
    SendMessage(pData->hFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_BACK);
    SendMessage(pData->hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_SELECT);

    return;
}
/*********************************************************************\
* Function	ABMultiSelectPick_OnSetFocus
* Purpose   WM_ACTIVATE message handler of the main window
* Params
* Return    None
* Remarks
**********************************************************************/
static void ABMultiSelectPick_OnSetFocus(HWND hwnd)
{
    HWND hLst;
    
    hLst = GetDlgItem(hwnd,IDC_ABPICK_LIST);

    SetFocus(hLst);

    return;
}
/*********************************************************************\
* Function	ABMultiSelectPick_OnPaint
* Purpose   WM_PAINT message handler of the main window
* Params	hWnd: Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void ABMultiSelectPick_OnPaint(HWND hWnd)
{
	HDC hdc = BeginPaint(hWnd, NULL);

	EndPaint(hWnd, NULL);

	return;
}

/*********************************************************************\
* Function	ABMultiSelectPick_OnKey
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
static void ABMultiSelectPick_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags)
{
    AB_MULTISELECTPICKDATA* pData;

    pData = (AB_MULTISELECTPICKDATA*)GetUserData(hWnd);
    
    switch (vk)
	{
	case VK_F10:
		SendMessage(pData->hMsgWnd,pData->uMsgCmd,(WPARAM)FALSE,NULL);

		PostMessage(hWnd,WM_CLOSE,NULL,NULL);
		break;

	case VK_F5:
		{
			HWND hList;
			int  index;
			int  i;
			CONTACT_ITEMCHAIN*    pTempData;
			MEMORY_NAME *pMemoryName = NULL;
            char *pName = NULL;

			hList = GetDlgItem(hWnd,IDC_ABPICK_LIST);

			index = SendMessage(hList,LB_GETCURSEL,0,0);
        
			if(index == LB_ERR)
				break;

			pTempData = (CONTACT_ITEMCHAIN* )SendMessage(hList,LB_GETITEMDATA,index,NULL);
			
			for(i = 0; i < min(pData->MultiData.nCount,pData->MultiData.nMax) ; i++)
			{
				if(pData->MultiData.pId[i].Id == pData->id)
				{
					//already choose, only modify
					if(pData->MultiData.pId[i].nTelId != pTempData->nID)
					{
						pData->MultiData.pId[i].nTelId = pTempData->nID;

						if(AB_IsTel(pTempData->dwMask))
							pData->MultiData.pArray[i].nType = AB_NUMBER;
						else
							pData->MultiData.pArray[i].nType = AB_EMAIL;
						
						pName = pData->MultiData.pArray[i].szName;
						
						pMemoryName = Memory_Find((PVOID)pData->id,AB_FIND_ID);
						
						if(pMemoryName != NULL)
							strcpy(pName,AB_GetNameString(pMemoryName));
						
						strcpy(pData->MultiData.pArray[i].szTelOrEmail,pTempData->pszData);
					}	
					
					SendMessage(pData->hMsgWnd,pData->uMsgCmd,(WPARAM)TRUE,(LPARAM)(&(pData->MultiData)));
					PostMessage(hWnd,WM_CLOSE,NULL,NULL);
					return;
				}
			}
			
			//add a number
			if(pData->MultiData.nCount >= pData->MultiData.nMax)
			{
				PLXTipsWin(NULL,NULL,0,IDS_TOOMANY,NULL,Notify_Failure,IDS_OK,NULL,WAITTIMEOUT);
				
				break;
			}

			pData->MultiData.pId[pData->MultiData.nCount].Id = pData->id;
			pData->MultiData.pId[pData->MultiData.nCount].nTelId = pTempData->nID;

			if(AB_IsTel(pTempData->dwMask))
				pData->MultiData.pArray[pData->MultiData.nCount].nType = AB_NUMBER;
			else
				pData->MultiData.pArray[pData->MultiData.nCount].nType = AB_EMAIL;

			pName = pData->MultiData.pArray[pData->MultiData.nCount].szName;
			
			pMemoryName = Memory_Find((PVOID)pData->id,AB_FIND_ID);
			
			if(pMemoryName != NULL)
				strcpy(pName,AB_GetNameString(pMemoryName));

			strcpy(pData->MultiData.pArray[pData->MultiData.nCount].szTelOrEmail,pTempData->pszData);
			
			(pData->MultiData.nCount)++;
			
			SendMessage(pData->hMsgWnd,pData->uMsgCmd,(WPARAM)TRUE,(LPARAM)(&(pData->MultiData)));
			PostMessage(hWnd,WM_CLOSE,NULL,NULL);
		}
		break;

/*
            case VK_RETURN:
                SendMessage(pData->hMsgWnd,pData->uMsgCmd,(WPARAM)TRUE,(LPARAM)(&(pData->MultiData)));
                
                PostMessage(hWnd,WM_CLOSE,NULL,NULL);
                break;*/
        

/*
            case VK_F5:
                {
                    HWND hList;
                    int  index,i,j;
                    BOOL bSelect;    
                    char                  szCaption[AB_MAXLEN_FIRSTNAME*2];
                    char                  *p = NULL,*q = NULL;
                    CONTACT_ITEMCHAIN*    pTempData;
                    HBITMAP hBmp;
                    
                    szCaption[0] = 0;
        
                    hList = GetDlgItem(hWnd,IDC_ABPICK_LIST);
        
                    index = SendMessage(hList,LB_GETCURSEL,0,0);
        
                    if(index == LB_ERR)
                        break;
        
                    pTempData = (CONTACT_ITEMCHAIN* )SendMessage(hList,LB_GETITEMDATA,index,NULL);
        
                    hBmp = (HBITMAP)SendMessage(hList,LB_GETIMAGE,IMAGE_BITMAP,index);
                    
                    if(hBmp == pData->hSelected)
                        bSelect = TRUE;
                    else
                        bSelect = FALSE;
                    
                    if(bSelect == TRUE)
                    {
                        for(i = 0; i < min(pData->MultiData.nCount,pData->MultiData.nMax) ; i++)
                        {
                            if(pData->MultiData.pId[i].Id == pData->id && pData->MultiData.pId[i].nTelId == pTempData->nID)
                            {
                                for(j = i; j < min(pData->MultiData.nCount,pData->MultiData.nMax)-1 ; j++)
                                {
                                    pData->MultiData.pId[j] = pData->MultiData.pId[j+1];
                                    pData->MultiData.pArray[j] = pData->MultiData.pArray[j+1];
                                }
                                
                                (pData->MultiData.nCount)--;
                            }
                        }
                        SendMessage(hList,LB_SETIMAGE,(WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)index),(LPARAM)pData->hUnselect);
                    }
                    else
                    {
                        MEMORY_NAME *pMemoryName = NULL;
                        char *pName = NULL;
                        if(pData->MultiData.nCount >= pData->MultiData.nMax)
                        {
                            PLXTipsWin(NULL,NULL,0,IDS_TOOMANY,NULL,Notify_Failure,IDS_OK,NULL,WAITTIMEOUT);
                            
                            break;
                        }
                        pData->MultiData.pId[pData->MultiData.nCount].Id = pData->id;
                        pData->MultiData.pId[pData->MultiData.nCount].nTelId = pTempData->nID;
                        
                        if(AB_IsTel(pTempData->dwMask))
                            pData->MultiData.pArray[pData->MultiData.nCount].nType = AB_NUMBER;
                        else
                            pData->MultiData.pArray[pData->MultiData.nCount].nType = AB_EMAIL;
                        
                        pName = pData->MultiData.pArray[pData->MultiData.nCount].szName;
                        
                        pMemoryName = Memory_Find((PVOID)pData->id,AB_FIND_ID);
        
                        if(pMemoryName != NULL)
                            strcpy(pName,AB_GetNameString(pMemoryName));
                                                        
                        strcpy(pData->MultiData.pArray[pData->MultiData.nCount].szTelOrEmail,pTempData->pszData);
                        
                        (pData->MultiData.nCount)++;
                        SendMessage(hList,LB_SETIMAGE,(WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)index),(LPARAM)pData->hSelected);
                    }            
                }
                break;*/
        

	default:
		PDADefWindowProc(hWnd, WM_KEYDOWN, vk, MAKELPARAM(cRepeat, flags));
		break;
	}

	return;
}
/*********************************************************************\
* Function	ABMultiSelectPick_OnCommand
* Purpose   WM_COMMAND message handler of the main window
* Params
*			hWnd:	Handle of the window
*			id:		Id of command message
*			hwndCtl: Handle of the window which sended this message
*			codeNotify:Notify code of the message
* Return	None
* Remarks
**********************************************************************/
static LRESULT ABMultiSelectPick_OnCommand(HWND hWnd, int id, UINT codeNotify, LPARAM lParam)
{
	switch(id)
	{
	case IDC_ABPICK_LIST:
        if(codeNotify == LBN_SETFONT)
        {            
            HFONT hFont = NULL;
            
            GetFontHandle(&hFont, (lParam == 0) ? SMALL_FONT : LARGE_FONT);
            
            return (LRESULT)hFont;
        }
        break;

    case IDC_BACK:
		PostMessage(hWnd,WM_CLOSE,NULL,NULL);
        break;

    default:
        break;
    }

	return 0;
}
/*********************************************************************\
* Function	ABMultiSelectPick_OnDestroy
* Purpose   WM_DESTROY message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void ABMultiSelectPick_OnDestroy(HWND hWnd)
{

    AB_MULTISELECTPICKDATA* pData;

    pData = (AB_MULTISELECTPICKDATA*)GetUserData(hWnd);
    
    Item_Erase(pData->pItem);

    if(pData->hSelected)
        DeleteObject(pData->hSelected);

    if(pData->hUnselect)
        DeleteObject(pData->hUnselect);

    AB_FREE(pData->MultiData.pArray);
    
    AB_FREE(pData->MultiData.pId);

    return;

}
/*********************************************************************\
* Function	ABMultiSelectPick_OnClose
* Purpose   WM_CLOSE message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void ABMultiSelectPick_OnClose(HWND hWnd)
{
    AB_MULTISELECTPICKDATA *pData;
    
    pData = GetUserData(hWnd);

    SendMessage(pData->hFrameWnd,PWM_CLOSEWINDOW,(WPARAM)hWnd,NULL);

    DestroyWindow(hWnd);

    return;

}
