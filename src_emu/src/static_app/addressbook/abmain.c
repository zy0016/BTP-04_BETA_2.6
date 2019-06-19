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

#define IDM_AB_DELSELECT        1041
#define IDM_AB_DELALL           1042

#define IDM_AB_WRITE_SMS        1051 
#define IDM_AB_WRITE_MMS        1052
#define IDM_AB_WRITE_EMAIL      1053

#define IDM_AB_SEND_SMS         1061 
#define IDM_AB_SEND_BULETOOTH   1062
#define IDM_AB_SEND_EMAIL       1063
#define IDM_ABVIEW_SEND_MMS		1064

#define IDM_AB_COPYTOSIMSELECT  1081
#define IDM_AB_COPYTOSIMALL     1082


#define IDM_AB_OPEN             101
#define IDM_AB_EDIT             102
#define IDM_AB_DELETE           103
#define IDM_AB_DELETEMANY       104
#define IDM_AB_WRITE            105
#define IDM_AB_SEND             106
#define IDM_AB_DUPLICATE        107
#define IDM_AB_COPYTOSIM        108

#define IDC_EXIT                200
#define IDC_ABMAIN_LIST         300

/**********************AB Main Menu End***************************/

typedef enum
{
    AB_LIST_CONTACT = 0,
    AB_LIST_EXTENDED,
}AB_LIST_TYPE;

typedef struct tagABCREATEDATA
{
    AB_LIST_TYPE nType;
    HINSTANCE   hInstance;
    HBITMAP     hLeftArrow;
    HBITMAP     hRightArrow;
    HBITMAP     hNewContact;
    HBITMAP     hContact;
    HBITMAP     hQuickdialling;
    HBITMAP     hGroups;
    HBITMAP     hSIMPhonebook;
    PVOID       handle;
    DWORD*      pId;   
    int         nCount;
    HMENU       hMenu;
    HWND        hFrameWnd;
    WNDPROC     OldListWndProc;
}ABCREATEDATA,*PABCREATEDATA;

CONTACT_ITEM Contact_Item[AB_EDITCONTACT_CTRNUM+2]={  
    {AB_TYPEMASK_FIRSTNAME,         TRUE,       AB_MAXLEN_FIRSTNAME,        FALSE,  NULL},
    {AB_TYPEMASK_LASTNAME,          TRUE,       AB_MAXLEN_FIRSTNAME,        FALSE,  NULL},
    {AB_TYPEMASK_TEL,               FALSE,      AB_MAXLEN_TEL,              TRUE,   NULL},
    {AB_TYPEMASK_TELHOME,           FALSE,      AB_MAXLEN_TEL,              TRUE,   NULL},
    {AB_TYPEMASK_TELWORK,           FALSE,      AB_MAXLEN_TEL,              TRUE,   NULL},
    {AB_TYPEMASK_MOBILE,            FALSE,      AB_MAXLEN_TEL,              TRUE,   NULL},  
    {AB_TYPEMASK_MOBILEHOME,        FALSE,      AB_MAXLEN_TEL,              TRUE,   NULL},
    {AB_TYPEMASK_MOBILEWORK,        FALSE,      AB_MAXLEN_TEL,              TRUE,   NULL},
    {AB_TYPEMASK_FAX,               FALSE,      AB_MAXLEN_FAX,              TRUE,   NULL},
    {AB_TYPEMASK_PAGER,             TRUE,       AB_MAXLEN_PAGER,            TRUE,   NULL},
    {AB_TYPEMASK_EMAIL,             TRUE,       AB_MAXLEN_EMAIL,            FALSE,  NULL},
    {AB_TYPEMASK_EMAILWORK,         TRUE,       AB_MAXLEN_EMAIL,            FALSE,  NULL},
    {AB_TYPEMASK_EMAILHOME,         TRUE,       AB_MAXLEN_EMAIL,            FALSE,  NULL},
    {AB_TYPEMASK_POSTALADDRESS,     TRUE,       AB_MAXLEN_POSTALADDRESS,    FALSE,  NULL},       
    {AB_TYPEMASK_HTTPURL,           TRUE,       AB_MAXLEN_HTTPURL,          FALSE,  NULL},
    {AB_TYPEMASK_JOBTITLE,          TRUE,       AB_MAXLEN_JOBTITLE,         FALSE,  NULL},
    {AB_TYPEMASK_COMPANY,           TRUE,       AB_MAXLEN_COMPANY,          FALSE,  NULL},
    {AB_TYPEMASK_COMPANYTEL,        TRUE,       AB_MAXLEN_TEL,              TRUE,   NULL},
    {AB_TYPEMASK_COMPANYADDRESS,    TRUE,       AB_MAXLEN_COMPANYADDRESS,   FALSE,  NULL},          
    {AB_TYPEMASK_DTMF,              TRUE,       AB_MAXLEN_DTMF,             TRUE,   NULL},          
    {AB_TYPEMASK_DATE,              TRUE,       AB_MAXLEN_DATE,             TRUE,   NULL},
    {AB_TYPEMASK_NOTE,              TRUE,       AB_MAXLEN_NOTE,             FALSE,  NULL},         
    {AB_TYPEMASK_PIC,               TRUE,       0,                          FALSE,  NULL},
    {AB_TYPEMASK_TONE,              TRUE,       0,                          FALSE,  NULL},
};


PNOTIFY_NODE pNotifyHeader;

//function
BOOL AB_CreateMainWindow(HWND hFrameWnd,HINSTANCE   hInstance);
LRESULT ABMainWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);

static BOOL ABMain_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct);
static void ABMain_OnActivate(HWND hwnd, UINT state);
static void ABMain_OnSetFocus(HWND hWnd);
static void ABMain_OnPaint(HWND hWnd);
static void ABMain_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags);
static void ABMain_OnCommand(HWND hWnd, int id, UINT codeNotify);
static void ABMain_OnDestroy(HWND hWnd);
static void ABMain_OnClose(HWND hWnd);
static void ABMain_OnDataChange(HWND hWnd,int nPos,int nMode);
static void ABMain_OnDealSelect(HWND hWnd,UINT wMsgCmd,int nTelID,DWORD id);
static void ABMain_OnDeleteContact(HWND hWnd,BOOL bDelete);
static void ABMain_OnDeleteAll(HWND hWnd,BOOL bDeleteAll);
static void ABMain_OnDeleteSelect(HWND hWnd,BOOL bDel,int nCount,DWORD* pnId);
static void ABMain_OnSureDeleteSel(HWND hWnd,BOOL bDeleteSel);
static void ABMain_OnCopySelect(HWND hWnd,BOOL bCopy,int nCount,DWORD* pnId);
static void ABMain_OnCopyAll(HWND hWnd,BOOL bCopy);
static BOOL AB_RefreshMain(HWND hWnd,AB_LIST_TYPE Type);
static LRESULT CallListWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static BOOL AB_GenChain(PCONTACT_ITEMCHAIN *ppItem,int *pnTelID);
static BOOL Test_GenChain(PCONTACT_ITEMCHAIN *ppItem,int index);
static void ABMain_OnRefreshList(HWND hWnd,char* pszSearch,int nLen,BOOL bExit);
BOOL AB_GetUseableDuplicateName(char* pszName,PCONTACT_ITEMCHAIN *ppItem);
int AB_Code2UTF8(char* pSrc,char** ppDes);
MEMORY_NAME* AB_FindByName(char* szName);
static void EarseVcardChain(vCard_Obj *ppPointer,int nType);
static void InsertVcardChain(PVOID *ppPointer,PVOID pNew,int nType);
static void EarseVcard(vCard_Obj *pVcardObj);
BOOL AB_GenVcard(CONTACT_ITEMCHAIN* pItem,char** ppVcardData,int *nVcardLen);
static BOOL AB_SaveVcardFile(char* pszVcardFileName,char* pVcardData,int nVcardLen);
static BOOL AB_AllConfirmNum(void);
BOOL AB_CreateTmp(char* filename, char* pData, int len);
BOOL AB_ClearTmp(char* filename);
BOOL AB_SelectConfirmNum (DWORD* pID, int count);

static BOOL cancel = FALSE;

BOOL AB_CreateMainWindow(HWND hFrameWnd, HINSTANCE hInstance)
{
    WNDCLASS    wc;
    HMENU       hWriteMenu,hSendMenu,hDelMenu,hCopyToSIMMenu;
    ABCREATEDATA CreateData;
    int         menupos = 0;
    HWND        hABMainWnd = NULL;
    RECT        rcClient;
    
    wc.style         = 0;
    wc.lpfnWndProc   = ABMainWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = sizeof(ABCREATEDATA);
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName  = "ABMainWndClass";
    
    if (!RegisterClass(&wc))
        return FALSE;
     
	if(GetSIMState() == 0)
		PLXTipsWin(NULL, NULL, 0, (char*)IDS_INSERT_SIM, IDS_CONTACTS, 
				Notify_Alert, (char*)IDS_OK, "", WAITTIMEOUT);

    memset(&CreateData,0,sizeof(ABCREATEDATA));

    CreateData.hFrameWnd = hFrameWnd;
    CreateData.hInstance = hInstance;
    CreateData.nType = AB_LIST_CONTACT;
    
    GetClientRect(hFrameWnd,&rcClient);

    CreateData.hMenu = CreateMenu();

    hABMainWnd = CreateWindow(
        "ABMainWndClass",
        "", 
        WS_VISIBLE | WS_CHILD, 
        rcClient.left,
        rcClient.top,
        rcClient.right - rcClient.left,
        rcClient.bottom - rcClient.top,
        hFrameWnd,
        (HMENU)IDC_AB_MAIN, 
        NULL, 
        (PVOID)&CreateData
        );
    
    if (!hABMainWnd)
    {
		DestroyMenu(CreateData.hMenu);

        UnregisterClass("ABMainWndClass",NULL);
        return FALSE;
    }

    AppendMenu(CreateData.hMenu, MF_ENABLED, IDM_AB_OPEN, IDS_OPEN);
    menupos++;        
    AppendMenu(CreateData.hMenu, MF_ENABLED, IDM_AB_EDIT, IDS_EDIT);
    menupos++;
    
    hWriteMenu = CreateMenu();
    AppendMenu(hWriteMenu, MF_ENABLED, IDM_AB_WRITE_SMS, IDS_SMS);
    AppendMenu(hWriteMenu, MF_ENABLED, IDM_AB_WRITE_MMS, IDS_MMS);
    AppendMenu(hWriteMenu, MF_ENABLED, IDM_AB_WRITE_EMAIL, IDS_EMAIL);
    
    AppendMenu(CreateData.hMenu, MF_POPUP|MF_ENABLED, (DWORD)hWriteMenu, IDS_WRITE);
    menupos++;
    
    hSendMenu = CreateMenu();
    AppendMenu(hSendMenu, MF_ENABLED, IDM_AB_SEND_SMS, IDS_VIASMS);
	AppendMenu(hSendMenu, MF_ENABLED, IDM_ABVIEW_SEND_MMS, IDS_VIAMMS);
	AppendMenu(hSendMenu, MF_ENABLED, IDM_AB_SEND_EMAIL, IDS_VIAEMAIL);
    AppendMenu(hSendMenu, MF_ENABLED, IDM_AB_SEND_BULETOOTH, IDS_VIABLUETOOTH);
    
    
    AppendMenu(CreateData.hMenu, MF_POPUP|MF_ENABLED, (DWORD)hSendMenu, IDS_SEND);
    menupos++;
    
    AppendMenu(CreateData.hMenu, MF_ENABLED, IDM_AB_DUPLICATE, IDS_DUPLICATE);
    menupos++;
    
    hCopyToSIMMenu = CreateMenu();
    AppendMenu(hCopyToSIMMenu, MF_ENABLED, IDM_AB_COPYTOSIMSELECT, IDS_SELECT);
    AppendMenu(hCopyToSIMMenu, MF_ENABLED, IDM_AB_COPYTOSIMALL, IDS_ALL);

    AppendMenu(CreateData.hMenu, MF_POPUP|MF_ENABLED, (DWORD)hCopyToSIMMenu, IDS_COPYTOSIM);
    menupos++;

	AppendMenu(CreateData.hMenu, MF_ENABLED, IDM_AB_DELETE, IDS_DELETE);
    menupos++;

    hDelMenu = CreateMenu();
    AppendMenu(hDelMenu, MF_ENABLED, IDM_AB_DELSELECT, IDS_SELECT);
    AppendMenu(hDelMenu, MF_ENABLED, IDM_AB_DELALL, IDS_ALL);
    
    AppendMenu(CreateData.hMenu, MF_POPUP|MF_ENABLED, (DWORD)hDelMenu, IDS_DELETEMANY);
    menupos++;
	
    SetFocus(hABMainWnd);
    
    PDASetMenu(hFrameWnd,CreateData.hMenu);

    SetWindowText(hFrameWnd,IDS_CONTACTS);

    SendMessage(hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_EXIT, 0), (LPARAM)IDS_EXIT);
    SendMessage(hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDM_AB_OPEN, 1), (LPARAM)"");
    SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_SELECT);

    if(AB_GetFlag())
    {
        PCONTACT_INITDATA pData;

        SendMessage(hABMainWnd,WM_KEYDOWN,VK_RIGHT,NULL);
        SendMessage(hABMainWnd,WM_KEYUP,VK_RIGHT,NULL);
        
		if(GetSIMState() == 0)
			return TRUE;

        pData = AB_GetSIMData();
        
        if(pData->nInit == SIMINIT_SUCCEEDED)
            AB_CreateSIMWnd(hFrameWnd);
        else
        {
            if(pData->nErrCtrl > ERR_RETRY)
                AB_UserListSM();
            else
                PLXTipsWin(NULL,NULL,0,IDS_SIMNOTREADY,IDS_CONTACTS, Notify_Info, IDS_OK, NULL, WAITTIMEOUT);
        }
    }

    return TRUE;
}

LRESULT ABMainWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    LRESULT lResult = TRUE;

	switch (wMsgCmd)
    {
    case WM_CREATE:
        lResult = ABMain_OnCreate(hWnd,(LPCREATESTRUCT)(lParam));
        break;

    case WM_ACTIVATE:
    case PWM_SHOWWINDOW:
        ABMain_OnActivate(hWnd,(UINT)LOWORD(wParam));
        break;

    case WM_SETFOCUS:
        ABMain_OnSetFocus(hWnd);
        break;

    case WM_PAINT:
        ABMain_OnPaint(hWnd);
        break;

    case WM_KEYDOWN:
        ABMain_OnKey(hWnd,(UINT)(wParam),(int)(short)LOWORD(lParam),(UINT)HIWORD(lParam));
        break;

    case WM_COMMAND:
        ABMain_OnCommand(hWnd,(int)(LOWORD(wParam)),(UINT)HIWORD(wParam));
        break;
        
    case WM_CLOSE:
        ABMain_OnClose(hWnd);
        break;

    case WM_DESTROY:
        ABMain_OnDestroy(hWnd);
        break;
      
    case WM_DATACHANGE:
        ABMain_OnDataChange(hWnd,(int)wParam,(int)lParam);
        break;

    case WM_MAKECALL:
    case WM_WRITESMS:
    case WM_WRITEMMS:
    case WM_WRITEEMAIL:
        ABMain_OnDealSelect(hWnd,wMsgCmd,(int)wParam,(DWORD)lParam);
        break;

    case WM_DELETECONTACT:
        ABMain_OnDeleteContact(hWnd,(BOOL)lParam);
        break;

    case WM_DELETEALL:
        ABMain_OnDeleteAll(hWnd,(BOOL)lParam);
        break;

	case WM_DELSELECT:
        ABMain_OnDeleteSelect(hWnd,(BOOL)LOWORD(wParam),(int)HIWORD(wParam),(DWORD*)lParam);
		break;

	case AB_MSG_REFRESHLIST:
        ABMain_OnRefreshList(hWnd,(char*)wParam,(int)LOWORD(lParam),(BOOL)HIWORD(lParam));
		break;

    case WM_SUREELETESEL:
        ABMain_OnSureDeleteSel(hWnd,(BOOL)lParam);
        break;

    case WM_COPYSELECT:
        ABMain_OnCopySelect(hWnd,(BOOL)LOWORD(wParam),(int)HIWORD(wParam),(DWORD*)lParam);
        break;

    case WM_COPYALL:
        ABMain_OnCopyAll(hWnd,(BOOL)lParam);
        break;

	case WM_CANCELCOPYING:
		cancel = TRUE;
		break;

    default:     
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
	}

    return lResult;

}
/*********************************************************************\
* Function	ABMain_OnCreate
* Purpose   WM_CREATE message handler of the main window
* Params
*			hWnd: Handle of the window
*			lpCreateStruct: Create Structure
* Return
*			TRUE: Success
*			FALSE: Fail
* Remarks
**********************************************************************/
static BOOL ABMain_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
{    
    RECT rect;
    PABCREATEDATA pData;
    HWND hList;
//    SIZE size;
    
    pData = GetUserData(hWnd);

    memcpy(pData,lpCreateStruct->lpCreateParams,sizeof(ABCREATEDATA));
    
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
        (HMENU)IDC_ABMAIN_LIST,
        NULL,
        NULL);
   
    if(hList == NULL)
        return FALSE;

    pData->OldListWndProc = (WNDPROC)SetWindowLong(hList,GWL_WNDPROC,(LONG)CallListWndProc);
    
//    GetImageDimensionFromFile(AB_BMP_ARROWLEFT,&size);
//
//	pData->hLeftArrow = LoadImage(NULL, AB_BMP_ARROWLEFT, IMAGE_BITMAP,
//		size.cx, size.cy, LR_LOADFROMFILE);
//
//    GetImageDimensionFromFile(AB_BMP_ARROWRIGHT,&size);

//    pData->hRightArrow = LoadImage(NULL, AB_BMP_ARROWRIGHT, IMAGE_BITMAP,
//		size.cx, size.cy, LR_LOADFROMFILE);

    pData->hNewContact = LoadImage(NULL, AB_BMP_NEWCONTACT, IMAGE_BITMAP,
		ICON_WIDTH, ICON_HEIGHT, LR_LOADFROMFILE);
    
    pData->hContact = LoadImage(NULL, AB_BMP_CONTACT, IMAGE_BITMAP,
		ICON_WIDTH, ICON_HEIGHT, LR_LOADFROMFILE);
    
    pData->hGroups = LoadImage(NULL, AB_BMP_GROUPS, IMAGE_BITMAP,
		ICON_WIDTH, ICON_HEIGHT, LR_LOADFROMFILE);
    
    pData->hQuickdialling = LoadImage(NULL, AB_BMP_QUICKDIALLING, IMAGE_BITMAP,
		ICON_WIDTH, ICON_HEIGHT, LR_LOADFROMFILE);
    
    pData->hSIMPhonebook = LoadImage(NULL, AB_BMP_SIMPHONEBOOK, IMAGE_BITMAP,
		ICON_WIDTH, ICON_HEIGHT, LR_LOADFROMFILE);

    AB_RefreshMain(hWnd,pData->nType);
    
    pData->handle = AB_RegisterNotify(hWnd,WM_DATACHANGE,AB_OBJECT_NAME,AB_MDU_CONTRACT);
        
    return TRUE;
    
}
/*********************************************************************\
* Function	ABMain_OnActivate
* Purpose   WM_ACTIVATE message handler of the main window
* Params
* Return    None
* Remarks
**********************************************************************/
static void ABMain_OnActivate(HWND hwnd, UINT state)
{
    HWND hLst;
    PABCREATEDATA pData;
    int  nIndex;
    
    pData = GetUserData(hwnd);

    hLst = GetDlgItem(hwnd,IDC_ABMAIN_LIST);

    SetFocus(hLst);

    PDASetMenu(pData->hFrameWnd,pData->hMenu);

    SetWindowText(pData->hFrameWnd,IDS_CONTACTS);
    
    SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,0, (LPARAM)IDS_EXIT);

    if(pData->nType == AB_LIST_CONTACT)
    {
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
        
        SendMessage(pData->hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON),(LPARAM)NULL);
        //SendMessage(pData->hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON),(LPARAM)pData->hRightArrow);
		SendMessage(pData->hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, RIGHTICON),(LPARAM)AB_ICON_ARROWRIGHT);
    }
    else if(pData->nType == AB_LIST_EXTENDED)
    {
        SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,1, (LPARAM)"");
        SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,2, (LPARAM)ICON_SELECT);
        
        //SendMessage(pData->hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON),(LPARAM)pData->hLeftArrow);
		SendMessage(pData->hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, LEFTICON),(LPARAM)AB_ICON_ARROWLEFT);
        SendMessage(pData->hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON),(LPARAM)NULL);
    }

    return;
}

/*********************************************************************\
* Function	ABMain_OnPaint
* Purpose   WM_PAINT message handler of the main window
* Params	hWnd: Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void ABMain_OnSetFocus(HWND hWnd)
{
    
    HWND hLst;

    PABCREATEDATA pData;
    
    pData = GetUserData(hWnd);

    hLst = GetDlgItem(hWnd,IDC_ABMAIN_LIST);

    SetFocus(hLst);

}
/*********************************************************************\
* Function	ABMain_OnPaint
* Purpose   WM_PAINT message handler of the main window
* Params	hWnd: Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void ABMain_OnPaint(HWND hWnd)
{
	HDC hdc = BeginPaint(hWnd, NULL);

	EndPaint(hWnd, NULL);

	return;
}

/*********************************************************************\
* Function	ABMain_OnKey
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
static void ABMain_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags)
{
    PABCREATEDATA pData;
    HWND hLst;
    int  nIndex;
    CONTACT_ITEMCHAIN *pItem;
	int  nTelID = 0;

    pData = GetUserData(hWnd);

    switch (vk)
	{
    case VK_F5:
        hLst = GetDlgItem(hWnd,IDC_ABMAIN_LIST);
        nIndex = SendMessage(hLst,LB_GETCURSEL,0,0);

        if(pData->nType == AB_LIST_CONTACT)
        {
            switch(nIndex)
            {
            case LB_ERR:
            	break;

            case 0:
                //if space is enough?
                if(AB_IsFlashEnough() == FALSE)
                {
                    PLXTipsWin(NULL,NULL,0,IDS_MEMORYFULL,IDS_CONTACTS,Notify_Failure,IDS_OK,NULL,WAITTIMEOUT);
                    break;
                }
                pItem = NULL;

				nTelID = 1;

                AB_GenChain(&pItem,&nTelID);
                
                AB_CreateEditContactWnd(pData->hFrameWnd,NULL,0,pItem,pItem,TRUE,-1,-1,nTelID,NULL);

                Item_Erase(pItem);
            	break;

            default:
                PDADefWindowProc(pData->hFrameWnd, WM_KEYDOWN, vk, MAKELPARAM(cRepeat, flags));
                break;
            }
        }
        else
        {
            switch(nIndex)
            {
            case 0:
                AB_CreateGroupsWindow(pData->hFrameWnd);
            	break;

            case 1:
                AB_CreateQDialWindow(pData->hFrameWnd);
                break;

            case 2:
                {
                    PCONTACT_INITDATA pData1;
                    
					if(GetSIMState() == 0)
					{
						PLXTipsWin(NULL,NULL, 0, IDS_INSERT_SIM,IDS_CONTACTS, Notify_Alert,IDS_OK, NULL, WAITTIMEOUT);//
						break;
					}
					
                    pData1 = AB_GetSIMData();
                        
                    if(pData1->nInit == SIMINIT_SUCCEEDED)
                        AB_CreateSIMWnd(GetParent(hWnd));
                    else
                    {
                        if(pData1->nErrCtrl > ERR_RETRY)
						{
							AB_UserListSM();
						}
						
						PLXTipsWin(NULL,NULL,0,IDS_SIMNOTREADY,IDS_CONTACTS, Notify_Failure,IDS_OK, NULL, WAITTIMEOUT);//
						
					}
                            
                }
                break;

            case LB_ERR:
            default:
                break;
            }
        }
        break;

	case VK_F10:
		PostMessage(hWnd,WM_CLOSE,NULL,NULL);
		break;

    case VK_RETURN:
        if(pData->nType == AB_LIST_CONTACT)
            PostMessage(hWnd,WM_COMMAND,IDM_AB_OPEN,NULL);
        break;

    case VK_LEFT:
        if(pData->nType == AB_LIST_EXTENDED)
        {
            if(AB_RefreshMain(hWnd,AB_LIST_CONTACT))
                pData->nType = AB_LIST_CONTACT;
        }
        break;

    case VK_RIGHT:
        if(pData->nType == AB_LIST_CONTACT)
        {
            if(AB_RefreshMain(hWnd,AB_LIST_EXTENDED))
                pData->nType = AB_LIST_EXTENDED;
        }
        break;

    case VK_F1://Send dial
        if(pData->nType == AB_LIST_CONTACT)
        {
			DWORD id;
			int nTelID = 0,nsubfix = 0;

            hLst = GetDlgItem(hWnd,IDC_ABMAIN_LIST);
            nIndex = SendMessage(hLst,LB_GETCURSEL,0,0);
            if(nIndex == LB_ERR || nIndex == 0)
                break;

			id = SendMessage(hLst,LB_GETITEMDATA,nIndex,NULL);

			nsubfix = AB_GetSubfix(id);

            pItem = NULL;

            AB_ReadRecord(pIndexName[nsubfix]->dwoffset,&pItem,&nTelID);
            
            AB_SelectPhoneOrEmail(pData->hFrameWnd,hWnd,WM_MAKECALL,pItem,PICK_NUMBER,id,TRUE,FALSE);

            Item_Erase(pItem);
        }
        break;
        
#if 0
    case VK_1:
        { 
//            ABNAMEOREMAIL ABName;
//
//            memset(&ABName,0,sizeof(ABNAMEOREMAIL));
//
//            ABName.nType = AB_URL;
//            strcpy(ABName.szTelOrEmail,"http://1234567890");
//
//            APP_SaveToAddressBook(pData->hFrameWnd,hWnd,WM_MAKECALL,&ABName,AB_NEW);
//            WORD w;
//
//            w = 1;
//
//            APP_SetQuickDial(hWnd,0,w);
            
//            int index;
//            
//            PHONEBOOK PB;
//
//            memset(&PB,0,sizeof(PHONEBOOK));
//
//            index = 2;
//
//            APP_GetSIMRecByOrder(index,&PB);

//            ABINFO abinfo;
//
//            memset(&abinfo, 0 ,sizeof(ABINFO));
//
//             APP_GetInfoByPhone("12345",&abinfo);

            DWORD id = 0;
            BYTE pData[100];
            int Datalen;

            APP_AddApi(&id,pData,Datalen);
        }
        break;
#else
    //case VK_1: 
#endif

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
//    case VK_F4:
        if(pData->nType == AB_LIST_CONTACT)
        {
			AB_CreateSearchPopUpWnd(pData->hFrameWnd,hWnd,AB_MSG_REFRESHLIST,TRUE);
            keybd_event(vk,0,0,0);
		}
        break;

	default:
		PDADefWindowProc(hWnd, WM_KEYDOWN, vk, MAKELPARAM(cRepeat, flags));
		break;
	}

	return;
}
/*********************************************************************\
* Function	ABMain_OnCommand
* Purpose   WM_COMMAND message handler of the main window
* Params
*			hWnd:	Handle of the window
*			id:		Id of command message
*			hwndCtl: Handle of the window which sended this message
*			codeNotify:Notify code of the message
* Return	None
* Remarks
**********************************************************************/
static void ABMain_OnCommand(HWND hWnd, int id, UINT codeNotify)
{
    PABCREATEDATA pData;
    HWND hLst;
    hLst = GetDlgItem(hWnd,IDC_ABMAIN_LIST);
    
    pData = GetUserData(hWnd);

	switch(id)
	{
    case IDC_ABMAIN_LIST:
        if(codeNotify == LBN_SELCHANGE)
        {
            if(pData->nType == AB_LIST_CONTACT)
            {
                int  nIndex;
                char szButText[20];
                char szMenu[20];
                
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
        break;

    case IDM_AB_OPEN:
        {
            int  nIndex;
            CONTACT_ITEMCHAIN *pItem;
			int  nTelID = 0;

            nIndex = SendMessage(hLst,LB_GETCURSEL,0,0);
            
            if(nIndex == LB_ERR || nIndex == 0)
                break;
            
            pItem = NULL;
            
            AB_ReadRecord(pIndexName[nIndex]->dwoffset,&pItem,&nTelID);
            
            AB_CreateViewContactWnd(pData->hFrameWnd,pItem,pIndexName[nIndex]->dwoffset,pIndexName[nIndex]->id,nTelID);

            Item_Erase(pItem);
        }
        break;

    case IDM_AB_WRITE_SMS:
        {
            int  nIndex;
            CONTACT_ITEMCHAIN *pItem;
			DWORD id;
			int nTelID = 0,nsubfix = 0;

            hLst = GetDlgItem(hWnd,IDC_ABMAIN_LIST);
            nIndex = SendMessage(hLst,LB_GETCURSEL,0,0);
            if(nIndex == LB_ERR || nIndex == 0)
                break;

			id = SendMessage(hLst,LB_GETITEMDATA,nIndex,NULL);

			nsubfix = AB_GetSubfix(id);

            pItem = NULL;

            AB_ReadRecord(pIndexName[nsubfix]->dwoffset,&pItem,&nTelID);

            AB_SelectPhoneOrEmail(pData->hFrameWnd,hWnd,WM_WRITESMS,pItem,PICK_NUMBER,id,TRUE,FALSE);
                
            Item_Erase(pItem);
        }
        break;

    case IDM_AB_WRITE_MMS:
        {
            int  nIndex;
            CONTACT_ITEMCHAIN *pItem;
          	DWORD id;
			int nTelID = 0,nsubfix = 0;

            hLst = GetDlgItem(hWnd,IDC_ABMAIN_LIST);
            nIndex = SendMessage(hLst,LB_GETCURSEL,0,0);
            if(nIndex == LB_ERR || nIndex == 0)
                break;

			id = SendMessage(hLst,LB_GETITEMDATA,nIndex,NULL);

			nsubfix = AB_GetSubfix(id);

            pItem = NULL;

            AB_ReadRecord(pIndexName[nsubfix]->dwoffset,&pItem,&nTelID);
            
            AB_SelectPhoneOrEmail(pData->hFrameWnd,hWnd,WM_WRITEMMS,pItem,PICK_NUMBERANDEMAIL,id,TRUE,FALSE);
                
            Item_Erase(pItem);
        }
        break;
         
    case IDM_AB_WRITE_EMAIL:
        {
            int  nIndex;
            CONTACT_ITEMCHAIN *pItem;
			DWORD id;
			int nTelID = 0,nsubfix = 0;

            hLst = GetDlgItem(hWnd,IDC_ABMAIN_LIST);
            nIndex = SendMessage(hLst,LB_GETCURSEL,0,0);
            if(nIndex == LB_ERR || nIndex == 0)
                break;

			id = SendMessage(hLst,LB_GETITEMDATA,nIndex,NULL);

			nsubfix = AB_GetSubfix(id);

            pItem = NULL;

            AB_ReadRecord(pIndexName[nsubfix]->dwoffset,&pItem,&nTelID);
            
            AB_SelectPhoneOrEmail(pData->hFrameWnd,hWnd,WM_WRITEEMAIL,pItem,PICK_EMAIL,id,TRUE,FALSE);
            
            Item_Erase(pItem);
        }
        break;

    case IDM_AB_EDIT:
        {
            int  nIndex;
            CONTACT_ITEMCHAIN *pItem;
			int  nTelID;

            nIndex = SendMessage(hLst,LB_GETCURSEL,0,0);
            
            if(nIndex == LB_ERR || nIndex == 0)
                break;
            
            pItem = NULL;
            
            AB_ReadRecord(pIndexName[nIndex]->dwoffset,&pItem,&nTelID);
            
            AB_CreateEditContactWnd(pData->hFrameWnd,NULL,0,pItem,pItem,FALSE,
                pIndexName[nIndex]->dwoffset,pIndexName[nIndex]->id,nTelID,NULL);
            
            Item_Erase(pItem);
        }
        break;

    case IDM_AB_DELETE:
        {
            int  nIndex;
            char szPrompt[125];
            char* pszName;
            char szCaption[50];
            HDC hdc;

            nIndex = SendMessage(hLst,LB_GETCURSEL,0,0);
            
            if(nIndex == LB_ERR || nIndex == 0)
                break;

            pszName = AB_GetNameString(pIndexName[nIndex]);

            szPrompt[0] = 0;

            hdc = GetDC(hWnd);
            
            GetExtentFittedText(hdc,pszName,-1,szCaption,50,TEXT_WIDTH,OMIT_SUFFIX,SUFFIX_REPEAT);
            
            ReleaseDC(hWnd,hdc);
            
            sprintf(szPrompt,"%s:\r\n%s?",szCaption,IDS_DELETE);

            PLXConfirmWinEx(pData->hFrameWnd,hWnd,szPrompt, Notify_Request, NULL/*pszName*/, IDS_YES, IDS_NO,WM_DELETECONTACT);                        

        }
        break;

    case IDM_AB_DUPLICATE:
        {
            int  nIndex;
            CONTACT_ITEMCHAIN *pItem;
			int  nTelID;
            int  nGroup;
            DWORD dwoffset,id;

            nIndex = SendMessage(hLst,LB_GETCURSEL,0,0);
            
            if(nIndex == LB_ERR || nIndex == 0)
                break;

            pItem = NULL;
            
            AB_ReadRecord(pIndexName[nIndex]->dwoffset,&pItem,&nTelID);

            nGroup = pIndexName[nIndex]->nGroup;

            dwoffset = -1;
            
            id = -1;

            AB_GetUseableDuplicateName(AB_GetNameString(pIndexName[nIndex]),&pItem);           

            AB_SaveRecord(&dwoffset,&id,&nGroup,TRUE,nTelID,pItem);
            
            AB_Insert2Table(dwoffset,id,nGroup,pItem);
                       
            Item_Erase(pItem);

        }
        break;

    case IDM_AB_SEND_SMS:
        {
            int  nIndex;
            CONTACT_ITEMCHAIN *pItem;
			int  nTelID;
            char* pVcardData;
            int   nVcardLen;

            nIndex = SendMessage(hLst,LB_GETCURSEL,0,0);
            
            if(nIndex == LB_ERR || nIndex == 0)
                break;

            pItem = NULL;
            
            AB_ReadRecord(pIndexName[nIndex]->dwoffset,&pItem,&nTelID);
                       
            pVcardData = NULL;

            nVcardLen = 0;

            AB_GenVcard(pItem,&pVcardData,&nVcardLen);

            APP_EditSMSVcardVcal(pData->hFrameWnd,pVcardData,nVcardLen);

            vCard_Clear(VCARD_OPT_WRITER, pVcardData);
            
            Item_Erase(pItem);

        }
        break;

	case IDM_ABVIEW_SEND_MMS:
		{
			int  nIndex;
            CONTACT_ITEMCHAIN *pItem;
			int  nTelID;
			int  nLen;
            char* pVcardData;
            int   nVcardLen;
			char szViewFileName [256];

            nIndex = SendMessage(hLst,LB_GETCURSEL,0,0);
            
            if(nIndex == LB_ERR || nIndex == 0)
                break;

            pItem = NULL;
            
            AB_ReadRecord(pIndexName[nIndex]->dwoffset,&pItem,&nTelID);
                       
            pVcardData = NULL;

            nVcardLen = 0;

            AB_GenVcard(pItem,&pVcardData,&nVcardLen);
			
			nLen = min(AB_VCARD_FILENAMELEN-5,strlen(AB_GetNameString(pIndexName[nIndex])));
			strncpy(szViewFileName,AB_GetNameString(pIndexName[nIndex]),nLen);
			szViewFileName[nLen] = 0;
			if(szViewFileName[0] == 0)
				strcpy(szViewFileName, "Unnamed");
            strcat(szViewFileName,AB_VCARD_PORTFIX);

			if(!AB_CreateTmp(szViewFileName, pVcardData, nVcardLen))
			{
				strcpy(szViewFileName, "Cantact.vcf");
				AB_CreateTmp(szViewFileName, pVcardData, nVcardLen);
			}

			APP_EditMMS(pData->hFrameWnd, NULL, 0, MMS_CALLEDIT_VCARD, szViewFileName);
            //APP_EditSMSVcardVcal(pData->hFrameWnd, pVcardData, nVcardLen);

			AB_ClearTmp(szViewFileName);
				
            vCard_Clear(VCARD_OPT_WRITER, pVcardData);
            
            Item_Erase(pItem);
		}
		break;

    case IDM_AB_SEND_EMAIL:
        {
            int  nIndex;
            CONTACT_ITEMCHAIN *pItem;
			int  nTelID;
            char* pVcardData;
            int   nVcardLen;
            char  szVcardFileName[PATH_MAXLEN];
            char  szViewFileName[AB_VCARD_FILENAMELEN];
            int   nLen;

            nIndex = SendMessage(hLst,LB_GETCURSEL,0,0);
            
            if(nIndex == LB_ERR || nIndex == 0)
                break;

            pItem = NULL;
            
            AB_ReadRecord(pIndexName[nIndex]->dwoffset,&pItem,&nTelID);
                       
            pVcardData = NULL;

            nVcardLen = 0;

            AB_GenVcard(pItem,&pVcardData,&nVcardLen);

			szVcardFileName[0] = 0;

            if(AB_SaveVcardFile(szVcardFileName,pVcardData,nVcardLen) == FALSE)
                break;
			
            szViewFileName[0] = 0;

            nLen = min(AB_VCARD_FILENAMELEN-5,strlen(AB_GetNameString(pIndexName[nIndex])));
            
            strncpy(szViewFileName,AB_GetNameString(pIndexName[nIndex]),nLen);

            szViewFileName[nLen] = 0;

            if(szViewFileName[0] != 0)
				strcat(szViewFileName,AB_VCARD_PORTFIX);
			else
				strcpy(szViewFileName, "Unnamed.vcf");

            MAIL_CreateEditInterface(pData->hFrameWnd, NULL, szVcardFileName, szViewFileName, TRUE);

            vCard_Clear(VCARD_OPT_WRITER, pVcardData);
            
            Item_Erase(pItem);

    //        AB_PleaseToWait();

        }
        break;

    case IDM_AB_SEND_BULETOOTH:
        {
            int  nIndex;
            CONTACT_ITEMCHAIN *pItem;
			int  nTelID;
            char* pVcardData;
            int   nVcardLen;
            char  szVcardFileName[PATH_MAXLEN];
            char  szViewFileName[AB_VCARD_FILENAMELEN];
            int   nLen;

            nIndex = SendMessage(hLst,LB_GETCURSEL,0,0);
            
            if(nIndex == LB_ERR || nIndex == 0)
                break;

            pItem = NULL;
            
            AB_ReadRecord(pIndexName[nIndex]->dwoffset,&pItem,&nTelID);
                       
            pVcardData = NULL;

            nVcardLen = 0;

            AB_GenVcard(pItem,&pVcardData,&nVcardLen);

            szVcardFileName[0] = 0;

            if(AB_SaveVcardFile(szVcardFileName,pVcardData,nVcardLen) == FALSE)
                break;

            szViewFileName[0] = 0;

            nLen = min(AB_VCARD_FILENAMELEN-5,strlen(AB_GetNameString(pIndexName[nIndex])));
            
            strncpy(szViewFileName,AB_GetNameString(pIndexName[nIndex]),nLen);

            szViewFileName[nLen] = 0;

            strcat(szViewFileName,AB_VCARD_PORTFIX);

            BtSendData(pData->hFrameWnd,szVcardFileName,szViewFileName, BTCARDCALENDER);

            vCard_Clear(VCARD_OPT_WRITER, pVcardData);
            
            Item_Erase(pItem);

        }
        break;

    case IDM_AB_COPYTOSIMSELECT:
        {
            pData->nCount = nName;

			pData->pId = (DWORD*)malloc(pData->nCount*sizeof(DWORD));

			if(pData->pId == NULL)
				break;
			
			memset(pData->pId,0,pData->nCount*sizeof(DWORD));
			
			ABCreateMultiPickerWnd(pData->hFrameWnd,hWnd,WM_COPYSELECT,(char*)IDS_SELECTCONTACT/*IDS_COPYTOSIM*/,pData->nCount,pData->pId,-1,IDS_COPY,IDS_CANCEL);
        }
        break;

    case IDM_AB_COPYTOSIMALL:
        {

			if(!AB_AllConfirmNum())
			{
				PLXTipsWin(NULL,NULL,0,IDS_NOPHONENUMS,IDS_CONTACTS,Notify_Failure,IDS_OK,NULL,WAITTIMEOUT);
				break;
			}

            PLXConfirmWinEx(pData->hFrameWnd,hWnd,IDS_COPYALL, Notify_Request, IDS_CONTACTS, IDS_YES, IDS_NO,WM_COPYALL);

        }
        break;

    case IDM_AB_DELSELECT:
		{		
            pData->nCount = nName;

			pData->pId = (DWORD*)malloc(pData->nCount*sizeof(DWORD));

			if(pData->pId == NULL)
				break;
			
			memset(pData->pId,0,pData->nCount*sizeof(DWORD));
			
			ABCreateMultiPickerWnd(pData->hFrameWnd,hWnd,WM_DELSELECT,(char*)IDS_SELECTCONTACT,pData->nCount,pData->pId,-1,IDS_DELETE,IDS_CANCEL);
		}
        break;

    case IDM_AB_DELALL:
        {
            char szTitle[50];

            GetWindowText(pData->hFrameWnd,szTitle,49);

            szTitle[49] = 0;

            PLXConfirmWinEx(pData->hFrameWnd,hWnd,IDS_DELETEALL, Notify_Request, szTitle, IDS_YES, IDS_NO,WM_DELETEALL);
        }
        break;

    default:
        break;
    }
}
/*********************************************************************\
* Function	ABMain_OnDestroy
* Purpose   WM_DESTROY message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void ABMain_OnDestroy(HWND hWnd)
{
	
    PABCREATEDATA pData;
    
    pData = GetUserData(hWnd);

    if(pData->hLeftArrow != NULL)
        DeleteObject(pData->hLeftArrow);

    if(pData->hRightArrow != NULL)
        DeleteObject(pData->hRightArrow);

    if(pData->hNewContact != NULL)
        DeleteObject(pData->hNewContact);
    
    if(pData->hContact != NULL)
        DeleteObject(pData->hContact);
    
    if(pData->hGroups != NULL)
        DeleteObject(pData->hGroups);
    
    if(pData->hQuickdialling != NULL)
        DeleteObject(pData->hQuickdialling);
    
    if(pData->hSIMPhonebook != NULL)
        DeleteObject(pData->hSIMPhonebook);
    
    DestroyMenu(pData->hMenu);
    
    AB_UnRegisterNotify(pData->handle);
	
    UnregisterClass("ABMainWndClass",NULL);

    //EndObjectDebug();

    return;

}
/*********************************************************************\
* Function	ABMain_OnClose
* Purpose   WM_CLOSE message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void ABMain_OnClose(HWND hWnd)
{
    PABCREATEDATA pData;
    
    pData = GetUserData(hWnd);

    SendMessage(pData->hFrameWnd,PWM_CLOSEWINDOW,(WPARAM)hWnd,NULL);

    DlmNotify((WPARAM)PES_STCQUIT, (LPARAM)(pData->hInstance));

    DestroyWindow(hWnd);

    return;

}
/*********************************************************************\
* Function	ABMain_OnRefreshList
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
static void ABMain_OnRefreshList(HWND hWnd,char* pszSearch,int nLen,BOOL bExit)
{
	int i,nCount;
	char szMenu[20],szLeft[20];
	HWND hList;
	int insert;
	char* pName;
	DWORD id;
    PABCREATEDATA pData;
    
    pData = GetUserData(hWnd);

	if(bExit)
	{
		if(nLen == 0)
		{
			AB_RefreshMain(hWnd,AB_LIST_CONTACT);
		}
		else
		{
			hList = GetDlgItem(hWnd,IDC_ABMAIN_LIST);
			
			insert = SendMessage(hList,LB_GETCURSEL,0,0);

			if(insert == LB_ERR)
				AB_RefreshMain(hWnd,AB_LIST_CONTACT);
			else
			{
				id = SendMessage(hList,LB_GETITEMDATA,insert,0);
				
				SendMessage(hList,LB_RESETCONTENT,0,0);
				SendMessage(hList,LB_ADDSTRING,-1,(LPARAM)IDS_NEW);
                SendMessage(hList,LB_SETIMAGE,MAKEWPARAM(IMAGE_BITMAP, 0),(LPARAM)pData->hNewContact);
				
				for(i=1 ; i <= nName ; i++)
				{
					insert = SendMessage(hList,LB_ADDSTRING,-1,(LPARAM)AB_GetNameString(pIndexName[i]));
					SendMessage(hList,LB_SETITEMDATA,insert,(LPARAM)(pIndexName[i]->id));
                    SendMessage(hList,LB_SETIMAGE,MAKEWPARAM(IMAGE_BITMAP, insert),(LPARAM)pData->hContact);
					if(id == pIndexName[i]->id)
                    {
                        SendMessage(hList,LB_SETCURSEL,i,0);
                        
                        SendMessage(pData->hFrameWnd,PWM_GETBUTTONTEXT,2,(LPARAM)szMenu);
                        if(strcmp(szMenu,ICON_SELECT) != 0)
                            SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,2, (LPARAM)ICON_SELECT);
                        
                        SendMessage(pData->hFrameWnd,PWM_GETBUTTONTEXT,1,(LPARAM)szLeft);
                        if(strcmp(szLeft,"") != 0)
                            SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,1, (LPARAM)"");
                    }
				}
				if(SendMessage(hList,LB_GETCURSEL,0,0) == LB_ERR)
                {
					SendMessage(hList,LB_SETCURSEL,0,0);
                    
                    SendMessage(pData->hFrameWnd,PWM_GETBUTTONTEXT,2,(LPARAM)szMenu);
                    if(strcmp(szMenu,ICON_OPTIONS) != 0)
                        SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,2, (LPARAM)ICON_OPTIONS);
                    
                    SendMessage(pData->hFrameWnd,PWM_GETBUTTONTEXT,1,(LPARAM)szLeft);
                    if(strcmp(szLeft,IDS_OPEN) != 0)
                        SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,1, (LPARAM)IDS_OPEN);
                }
			}
		}
	}
	else
	{
		hList = GetDlgItem(hWnd,IDC_ABMAIN_LIST);

		SendMessage(hList,LB_RESETCONTENT,0,0);
		SendMessage(hList,LB_ADDSTRING,-1,(LPARAM)IDS_NEWCONTACT);
        SendMessage(hList,LB_SETIMAGE,MAKEWPARAM(IMAGE_BITMAP, 0),(LPARAM)pData->hNewContact);
		for(i=1 ; i <= nName ; i++)
		{

			pName = AB_GetNameString(pIndexName[i]);

#ifndef LANGUAGE_CHN
			if(strstr(pName,pszSearch) == pName)
#else
			if(NameCmpHazy(pszSearch, pName) == 0)
#endif
			{
				insert = SendMessage(hList,LB_ADDSTRING,-1,(LPARAM)AB_GetNameString(pIndexName[i]));
				SendMessage(hList,LB_SETITEMDATA,insert,(LPARAM)(pIndexName[i]->id));
                SendMessage(hList,LB_SETIMAGE,MAKEWPARAM(IMAGE_BITMAP, insert),(LPARAM)pData->hContact);
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
/*********************************************************************\
* Function	AB_RefreshMain
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
static BOOL AB_RefreshMain(HWND hWnd,AB_LIST_TYPE Type)
{
    PABCREATEDATA pData;
    HWND hList;
    int  i,insert;
    char szMenu[20];
	static BOOL bLock = FALSE;
    
	if(bLock)
		return TRUE;

	bLock = TRUE;

    pData = GetUserData(hWnd);
    hList = GetDlgItem(hWnd,IDC_ABMAIN_LIST);

    szMenu[0] = 0;

    SendMessage(pData->hFrameWnd,PWM_GETBUTTONTEXT,2,(LPARAM)szMenu);

    if(Type == AB_LIST_CONTACT)
    {
		SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,1, (LPARAM)"");
        
        SendMessage(pData->hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON),(LPARAM)NULL);
//        SendMessage(pData->hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON),(LPARAM)pData->hRightArrow);
		SendMessage(pData->hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, RIGHTICON),(LPARAM)AB_ICON_ARROWRIGHT);
		
		if(strcmp(szMenu,ICON_SELECT) != 0)
            SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,2, (LPARAM)ICON_SELECT);

        SendMessage(hList,LB_RESETCONTENT,0,0);
        SendMessage(hList,LB_ADDSTRING,-1,(LPARAM)IDS_NEW);
        SendMessage(hList,LB_SETIMAGE,MAKEWPARAM(IMAGE_BITMAP, 0),(LPARAM)pData->hNewContact);
        //Load records
        for(i=1 ; i <= nName ; i++)
		{
			if(AB_IsUserCancel(hWnd, 0))
			{
				PostMessage(hWnd, WM_CLOSE, 0, 0);
				break;		
			}
			
            insert = SendMessage(hList,LB_ADDSTRING,-1,(LPARAM)AB_GetNameString(pIndexName[i]));
			SendMessage(hList,LB_SETITEMDATA,insert,(LPARAM)(pIndexName[i]->id));
            SendMessage(hList,LB_SETIMAGE,MAKEWPARAM(IMAGE_BITMAP, insert),(LPARAM)pData->hContact);
		}
        
        SendMessage(hList,LB_SETCURSEL,0,0);
		bLock = FALSE;
        return TRUE;
    }
    else if(Type == AB_LIST_EXTENDED)
    {
        SendMessage(hList,LB_RESETCONTENT,0,0);
        SendMessage(hList,LB_ADDSTRING,-1,(LPARAM)IDS_GROUPS);
        SendMessage(hList,LB_SETIMAGE,MAKEWPARAM(IMAGE_BITMAP, 0),(LPARAM)pData->hGroups);
        
        SendMessage(hList,LB_ADDSTRING,-1,(LPARAM)IDS_QUICKDIAL);
        SendMessage(hList,LB_SETIMAGE,MAKEWPARAM(IMAGE_BITMAP, 1),(LPARAM)pData->hQuickdialling);
        
        SendMessage(hList,LB_ADDSTRING,-1,(LPARAM)IDS_SIM);
        SendMessage(hList,LB_SETIMAGE,MAKEWPARAM(IMAGE_BITMAP, 2),(LPARAM)pData->hSIMPhonebook);
                
        SendMessage(hList,LB_SETCURSEL,0,0);

        SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,1, (LPARAM)"");
        
        SendMessage(pData->hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON),(LPARAM)NULL);
//        SendMessage(pData->hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON),(LPARAM)pData->hLeftArrow);
		SendMessage(pData->hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, LEFTICON),(LPARAM)AB_ICON_ARROWLEFT);

        if(strcmp(szMenu,ICON_SELECT) != 0)
            SendMessage(pData->hFrameWnd,PWM_SETBUTTONTEXT,2, (LPARAM)ICON_SELECT);
		
		bLock = FALSE;
        return TRUE;
    }
    else
	{
		bLock = FALSE;
        return FALSE;
	}
	
	bLock = FALSE;
    return FALSE;
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
    PABCREATEDATA pData;

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
        case VK_F1:
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
* Function	AB_GenChain
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
static BOOL AB_GenChain(PCONTACT_ITEMCHAIN *ppItem,int *pnTelID)
{
#define AB_NEW_ITEMNUM      4
    CONTACT_ITEMCHAIN *pNewItem;
    int i,nIndex;
    DWORD dwMask[AB_NEW_ITEMNUM];

    dwMask[0] = AB_TYPEMASK_FIRSTNAME;
    dwMask[1] = AB_TYPEMASK_LASTNAME;
    dwMask[2] = AB_TYPEMASK_TEL;
    dwMask[3] = AB_TYPEMASK_MOBILE;

    for(i = 0 ; i < AB_NEW_ITEMNUM ; i++)
    {
        nIndex = GetIndexByMask(dwMask[i]);
        pNewItem = Item_New(Contact_Item[nIndex].dwMask);
        
        if(pNewItem == NULL)
            return FALSE;
        
		if(AB_IsTel(dwMask[i]) || AB_IsEmail(dwMask[i]))
		{
			pNewItem->nID = *pnTelID;
			*pnTelID += 1;
		}

        Item_Insert(ppItem,pNewItem);

    }

    return TRUE; 
}
/*********************************************************************\
* Function	Test_GenChain
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
BOOL AB_SaveGenChain(PCONTACT_ITEMCHAIN *ppItem,int *pnTelID,DWORD dwMask,char* pszString)
{
    CONTACT_ITEMCHAIN *pNewItem;
    CONTACT_ITEMCHAIN *pTemp;
    int nIndex;
    
    *pnTelID = 0;
    
    AB_GenChain(ppItem,pnTelID);

    if(dwMask == AB_TYPEMASK_TEL || dwMask == AB_TYPEMASK_MOBILE)
    {
        pTemp = *ppItem;

        while(pTemp)
        {
            if(pTemp->dwMask == dwMask)
                strcpy(pTemp->pszData,pszString);

            pTemp = pTemp->pNext;
        }
    }
    else
    {
        nIndex = GetIndexByMask(dwMask);

        pNewItem = Item_New(Contact_Item[nIndex].dwMask);
        
        if(pNewItem == NULL)
            return FALSE;
        
		if(AB_IsTel(dwMask) || AB_IsEmail(dwMask))
		{
			pNewItem->nID = *pnTelID;
			*pnTelID += 1;
		}

        strcpy(pNewItem->pszData,pszString);

        Item_Insert(ppItem,pNewItem);
    }
    
    return TRUE; 
}

void AB_InitStirng(void)
{
    Contact_Item[0].pszCpation = IDS_FIRSTNAME;      
    Contact_Item[1].pszCpation = IDS_LASTNAME;       
    Contact_Item[2].pszCpation = IDS_TEL;            
    Contact_Item[3].pszCpation = IDS_TELHOME;        
    Contact_Item[4].pszCpation = IDS_TELWORK;        
    Contact_Item[5].pszCpation = IDS_MOBILE;         
    Contact_Item[6].pszCpation = IDS_MOBILEHOME;     
    Contact_Item[7].pszCpation = IDS_MOBILEWORK;     
    Contact_Item[8].pszCpation = IDS_FAX;            
    Contact_Item[9].pszCpation = IDS_PAGER;         
    Contact_Item[10].pszCpation = IDS_EMAIL;         
    Contact_Item[11].pszCpation = IDS_EMAILWORK;     
    Contact_Item[12].pszCpation = IDS_EMAILHOME;     
    Contact_Item[13].pszCpation = IDS_POSTALADDRESS; 
    Contact_Item[14].pszCpation = IDS_HTTPURL;       
    Contact_Item[15].pszCpation = IDS_JOBTITLE;      
    Contact_Item[16].pszCpation = IDS_COMPANY;       
    Contact_Item[17].pszCpation = IDS_COMPANYTEL;    
    Contact_Item[18].pszCpation = IDS_COMPANYADDRESS;
    Contact_Item[19].pszCpation = IDS_DTMF;          
    Contact_Item[20].pszCpation = IDS_DATE;          
    Contact_Item[21].pszCpation = IDS_NOTE;  
}

/*********************************************************************\
* Function	   Notify_New
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
NOTIFY_NODE* Notify_New(void)
{
    NOTIFY_NODE* p = NULL;

    p = (NOTIFY_NODE*)malloc(sizeof(NOTIFY_NODE));

    if(p == NULL)
        return NULL;

    memset(p,0,sizeof(NOTIFY_NODE));

    return p;
}
/*********************************************************************\
* Function	   Notify_Insert
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
void Notify_Insert(NOTIFY_NODE* pChainNode)
{
    NOTIFY_NODE* p;

    if(pNotifyHeader)
    {
        p = pNotifyHeader;

        while( p )
        {
            if( p->pNext )
                p = p->pNext;
            else
                break;
        }

        p->pNext = pChainNode;
        pChainNode->pPioneer = p;
        pChainNode->pNext = NULL;
    }
    else
    {
        pNotifyHeader = pChainNode;
        pChainNode->pNext = NULL;
        pChainNode->pPioneer = NULL;
    }
}
/*********************************************************************\
* Function	   Notify_Delete
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
void Notify_Delete(NOTIFY_NODE* pChainNode)
{
    if( pChainNode == pNotifyHeader )
    {
        if(pChainNode->pNext)
        {
            pNotifyHeader = pChainNode->pNext;
            pChainNode->pNext->pPioneer = NULL;
        }
        else
            pNotifyHeader = NULL;
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
* Function	   Notify_Erase
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
void Notify_Erase(void)
{
    NOTIFY_NODE* p;    
    NOTIFY_NODE* ptemp;

    p = pNotifyHeader;

    while( p )
    {
        ptemp = p->pNext;
        
        free(p);
        
        p = ptemp;
    }
}

/*********************************************************************\
* Function	   AB_RegisterNotify
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
PVOID AB_RegisterNotify(HWND hWnd,UINT uMsg,AB_NOTIFY_OBJECT nObject,AB_NOTIFY_MDU nMdu)
{
    NOTIFY_NODE* pNew = NULL;
    
    pNew = Notify_New();

    if(pNew == NULL)
        return NULL;

    pNew->hWnd = hWnd;
    pNew->uMsg = uMsg;
    pNew->nObject = nObject;
    pNew->nMdu = nMdu;

    Notify_Insert(pNew);

    return (PVOID)pNew;
}
/*********************************************************************\
* Function	   AB_UnRegisterNotify
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
void AB_UnRegisterNotify(PVOID pNode)
{
    if(pNode == NULL)
        return;

    Notify_Delete((NOTIFY_NODE*)pNode);
}
/*********************************************************************\
* Function	   AB_NotifyWnd
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
void AB_NotifyWnd(PVOID ID,AB_NOTIFY_MODE nMode,AB_NOTIFY_OBJECT nObject,AB_NOTIFY_MDU nMdu)
{
    PNOTIFY_NODE pTmp = NULL;

    pTmp = pNotifyHeader;

    while(pTmp)
    {
        if((pTmp->nObject == AB_OBJECT_ALL || pTmp->nObject == nObject )
            && pTmp->nMdu == nMdu)
        {
            if(IsWindow(pTmp->hWnd))
            {
                SendMessage(pTmp->hWnd,pTmp->uMsg,(WPARAM)ID,(LPARAM)nMode);
            }
        }

        pTmp = pTmp->pNext;
    }
}
/*********************************************************************\
* Function	   ABMain_OnDataChange
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void ABMain_OnDataChange(HWND hWnd,int nPos,int nMode)
{
    PABCREATEDATA pData;
    HWND hList;
    char szButText[20],szMenu[20];
    HWND hChild;

    pData = GetUserData(hWnd);

    if(pData->nType == AB_LIST_EXTENDED)
        return;

    if(nPos == -1)
        AB_RefreshMain(hWnd,pData->nType);
    else
    {
        hList = GetDlgItem(hWnd,IDC_ABMAIN_LIST);

        switch(nMode)
        {
        case AB_MODE_INSERT:
            SendMessage(hList,LB_INSERTSTRING,nPos,(LPARAM)AB_GetNameString(pIndexName[nPos]));

            SendMessage(hList,LB_SETIMAGE,MAKEWPARAM(IMAGE_BITMAP, nPos),(LPARAM)pData->hContact);

            SendMessage(hList,LB_SETITEMDATA,nPos,(LPARAM)pIndexName[nPos]->id);
            
            SendMessage(hList,LB_SETCURSEL,nPos,NULL);
            
            memset(szButText,0,20);
            
            memset(szMenu,0,20);
            
            SendMessage(pData->hFrameWnd, PWM_GETBUTTONTEXT, 1, (LPARAM)szButText);
            
            SendMessage(pData->hFrameWnd, PWM_GETBUTTONTEXT, 2, (LPARAM)szMenu);
            
			hChild = GetWindow(pData->hFrameWnd, GW_CHILD);
			if(hChild == hWnd)
			{
				if(nPos == 0)
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

        case AB_MODE_DELETE:
            SendMessage(hList,LB_DELETESTRING,nPos,NULL);
            
            if(nPos == SendMessage(hList,LB_GETCOUNT,0,0))
                SendMessage(hList,LB_SETCURSEL,nPos-1,NULL);
            else
                SendMessage(hList,LB_SETCURSEL,nPos,NULL);
            
            memset(szButText,0,20);
            
            memset(szMenu,0,20);
            
            SendMessage(pData->hFrameWnd, PWM_GETBUTTONTEXT, 1, (LPARAM)szButText);
            
            SendMessage(pData->hFrameWnd, PWM_GETBUTTONTEXT, 2, (LPARAM)szMenu);
            
            nPos = SendMessage(hList,LB_GETCURSEL,0,0);

			hChild = GetWindow(pData->hFrameWnd, GW_CHILD);
			if(hChild == hWnd)
			{
				if(nPos == 0)
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

        default:
            break;
        }
    }
}
/*********************************************************************\
* Function	   ABMain_OnDataChange
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void ABMain_OnDealSelect(HWND hWnd,UINT wMsgCmd,int nTelID,DWORD id)
{   
    CONTACT_ITEMCHAIN *pItem,*ptemp;

	int  nsubfix = 0;

	int  tempid = 0;

    PABCREATEDATA pData;
    
    pData = GetUserData(hWnd);

	if(nTelID == -1 || id == -1)
		return;
	
	nsubfix = AB_GetSubfix(id);

	if(nsubfix == -1)
		return;

	pItem = NULL;
	
	AB_ReadRecord(pIndexName[nsubfix]->dwoffset,&pItem,&tempid);
	
	ptemp = pItem;

    switch(wMsgCmd) 
    {
    case WM_MAKECALL:
		
		while(ptemp)
		{
			if(AB_IsTel(ptemp->dwMask))
			{
				if(ptemp->nID == nTelID)
				{
					APP_CallPhoneNumber(ptemp->pszData);
					break;
				}
			}
			ptemp = ptemp->pNext;
		}

        break;

    case WM_WRITESMS:
              
		while(ptemp)
		{
			if(AB_IsTel(ptemp->dwMask))
			{
				if(ptemp->nID == nTelID)
				{
					APP_EditSMS(pData->hFrameWnd,ptemp->pszData,NULL);
					break;
				}
			}
			ptemp = ptemp->pNext;
		}
                    
        break;
        
    case WM_WRITEMMS:
        
		while(ptemp)
		{
			if(AB_IsTel(ptemp->dwMask) || AB_IsEmail(ptemp->dwMask))
			{
				if(ptemp->nID == nTelID)
				{
                    APP_EditMMS(pData->hFrameWnd,hWnd,0,MMS_CALLEDIT_MOBIL,ptemp->pszData);
					break;
				}
			}
			ptemp = ptemp->pNext;
		}
        
        break;
        
    case WM_WRITEEMAIL:
        
		while(ptemp)
		{
			if(AB_IsEmail(ptemp->dwMask))
			{
				if(ptemp->nID == nTelID)
				{
                    SYSTEMTIME time;

                    memset(&time,0,sizeof(SYSTEMTIME));

                    GetLocalTime(&time);
                    
                    CreateMailEditWnd(pData->hFrameWnd, ptemp->pszData, NULL, NULL, NULL, NULL, 
                        &time, -1, -1);
                    
					break;
				}
			}
			ptemp = ptemp->pNext;
		}
        
        break;
    }    

	Item_Erase(pItem);
}
/*********************************************************************\
* Function	   ABMain_OnDeleteContact
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void ABMain_OnDeleteContact(HWND hWnd,BOOL bDelete)
{
    int  nIndex;
    HWND hLst;
    int  nCount;
    PABCREATEDATA pData;
    
    pData = GetUserData(hWnd);

    if(bDelete == FALSE)
        return;

    hLst = GetDlgItem(hWnd,IDC_ABMAIN_LIST);
        
    nIndex = SendMessage(hLst,LB_GETCURSEL,0,0);
    
    if(nIndex == LB_ERR || nIndex == 0)
        return;    
    
    if(AB_DeleteRecord(pIndexName[nIndex]->dwoffset))
    {    
        PLXTipsWin(NULL,NULL,0,IDS_DELETED,IDS_CONTACTS,Notify_Success,IDS_OK,NULL,WAITTIMEOUT);
		
		AB_NotifyWnd((PVOID)nIndex,AB_MODE_DELETE,AB_OBJECT_NAME,AB_MDU_CONTRACT);

        nCount = AB_DeleteFromIndexByID(pIndexTel,pIndexName[nIndex]->id,nTel,FALSE);
        nTel -= nCount;
        nCount = AB_DeleteFromIndexByID(pIndexEmail,pIndexName[nIndex]->id,nEmail,FALSE);
        nEmail -= nCount;
        nCount = AB_DeleteFromIndexByID(pIndexID,pIndexName[nIndex]->id,nID,FALSE);
        nID -= nCount;
        nCount = AB_DeleteFromIndexByID(pIndexName,pIndexName[nIndex]->id,nName,TRUE);
        nName--;
     
//        SendMessage(hLst,LB_DELETESTRING,nIndex,0);
//
//        if(nIndex == SendMessage(hLst,LB_GETCOUNT,0,0))
//            SendMessage(hLst,LB_SETCURSEL,nIndex-1,0);
//        else
//             SendMessage(hLst,LB_SETCURSEL,nIndex,0);

    }
}
/*********************************************************************\
* Function	   ABMain_OnDeleteAll
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void ABMain_OnDeleteAll(HWND hWnd,BOOL bDeleteAll)
{
    HWND hLst;

	int nCount,i;

    PABCREATEDATA pData;
    
    pData = GetUserData(hWnd);

    if(bDeleteAll == FALSE)
        return;

    if(AB_DeleteAllFile())
    {
        hLst = GetDlgItem(hWnd,IDC_ABMAIN_LIST);
        
		nCount = SendMessage(hLst,LB_GETCOUNT,0,0);

		for(i = nCount; i > 1 ; i--)
		{
			AB_NotifyWnd((PVOID)(i-1),AB_MODE_DELETE,AB_OBJECT_NAME,AB_MDU_CONTRACT);
		}

        AB_EmptyMemory();

        PLXTipsWin(NULL,NULL,0,IDS_DELETED,IDS_CONTACTS,Notify_Success,IDS_OK,NULL,WAITTIMEOUT);        
    }   
}
/*********************************************************************\
* Function	   ABMain_OnDeleteSelect
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void ABMain_OnDeleteSelect(HWND hWnd,BOOL bDel,int nCount,DWORD* pnId)
{
    char* pszName;
    char szPrompt[125];
    int  i,j;
    char szCaption[50];
    HDC hdc;

    PABCREATEDATA pData;

    pData = GetUserData(hWnd);

	if(bDel == FALSE || nCount == 0)
	{
		AB_FREE(pData->pId);

        pData->nCount = 0;

		return;
	}

    pData->nCount = nCount;

    if(nCount == 1)
    {
        for(i = nName,j = nCount-1 ; i >= 1 && j >= 0 ; i--)
        {
            if(pData->pId[j] == pIndexName[i]->id)
            {
                pszName = AB_GetNameString(pIndexName[i]);
                
                szPrompt[0] = 0;
                
                hdc = GetDC(hWnd);
                
                GetExtentFittedText(hdc,pszName,-1,szCaption,50,TEXT_WIDTH,OMIT_SUFFIX,SUFFIX_REPEAT);
                
                ReleaseDC(hWnd,hdc);
                
                sprintf(szPrompt,"%s:\r\n%s?",szCaption,IDS_DELETE);
                
                PLXConfirmWinEx(GetParent(hWnd),hWnd,szPrompt, Notify_Request, IDS_CONTACTS, IDS_YES, IDS_NO,WM_SUREELETESEL);    

                return;
            }
        }
    }
    else
    {
        
        PLXConfirmWinEx(GetParent(hWnd),hWnd,IDS_DELETESELECT, Notify_Request, IDS_CONTACTS, IDS_YES, IDS_NO,WM_SUREELETESEL);   
    } 
}
/*********************************************************************\
* Function	   ABMain_OnCopySelect
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void ABMain_OnCopySelect(HWND hWnd,BOOL bCopy,int nCount,DWORD* pnId)
{
    int  i,j;
    HWND hLst;
    PABCREATEDATA pData;
    CONTACT_ITEMCHAIN *pItem,*ptemp;
    char* pNum,*pName;
    DWORD Mask;
    int  nNameLen;
    int  tempid;
    PPHONEBOOK pPB;
    PHONEBOOK  Temp;
    PCONTACT_INITDATA pInit;          

    pData = GetUserData(hWnd);

	if(bCopy == FALSE || nCount == 0)
	{
		AB_FREE(pData->pId);

        pData->nCount = 0;

		return;
	}
    
	if(!AB_SelectConfirmNum(pnId, nCount))
	{
		PLXTipsWin(NULL,NULL,0,IDS_NOPHONENUMS,IDS_CONTACTS,Notify_Failure,IDS_OK,NULL,WAITTIMEOUT);
		return;
	}
	
    pInit = AB_GetSIMData();

    WaitWin(hWnd,TRUE,IDS_COPYING,IDS_CONTACTS,NULL,IDS_CANCEL,WM_CANCELCOPYING);

    pData->nCount = nCount;

    hLst = GetDlgItem(hWnd,IDC_ABMAIN_LIST);

	for(i = nName,j = pData->nCount-1 ; i >= 1 && j >= 0; i--)
	{
        if(AB_IsUserCancel(hWnd,WM_CANCELCOPYING) == TRUE)
        {
            WaitWin(hWnd,FALSE,IDS_COPYING,IDS_CONTACTS,NULL,IDS_CANCEL,WM_CANCELCOPYING);            
            
            AB_FREE(pData->pId);
            
            pData->nCount = 0;

            return;
        }

		if(pData->pId[j] == pIndexName[i]->id)
        {
            pPB = AB_FindUseableSIMPB();
            
            if(pPB == NULL)
            {
                WaitWin(hWnd,FALSE,IDS_COPYING,IDS_CONTACTS,NULL,IDS_CANCEL,WM_CANCELCOPYING);
                
                PLXTipsWin(NULL,NULL,0,IDS_MEMORYFULL,IDS_CONTACTS,Notify_Failure,IDS_OK,NULL,WAITTIMEOUT);
                
                AB_FREE(pData->pId);
            
                pData->nCount = 0;
        
                return;
            }
            
            memset(&Temp,0,sizeof(PHONEBOOK));
            
            Temp.Index = AB_FindUseableIndex();

            pItem = NULL;
            
            AB_ReadRecord(pIndexName[i]->dwoffset,&pItem,&tempid);

            pNum = NULL;

            ptemp = pItem;

            while( ptemp )
            {
                switch(ptemp->dwMask) 
                {
                case AB_TYPEMASK_TEL:
                    if(pNum == NULL)
                    {
                        Mask = AB_TYPEMASK_TEL;
                        pNum = ptemp->pszData;
                    }
                	break;

                case AB_TYPEMASK_TELHOME:
                    if(pNum == NULL)
                    {
                        Mask = AB_TYPEMASK_TELHOME;
                        pNum = ptemp->pszData;
                    }
                	break;

                case AB_TYPEMASK_TELWORK:
                    if(pNum == NULL)
                    {
                        Mask = AB_TYPEMASK_TELWORK;
                        pNum = ptemp->pszData;
                    }
                    break;

                case AB_TYPEMASK_MOBILE:
                    Mask = AB_TYPEMASK_MOBILE;
                    pNum = ptemp->pszData;
                	break;

                case AB_TYPEMASK_MOBILEHOME:
                    if(pNum == NULL || (pNum != NULL && Mask != AB_TYPEMASK_MOBILE))
                    {
                        Mask = AB_TYPEMASK_MOBILEHOME;
                        pNum = ptemp->pszData;
                    }
                	break;
                case AB_TYPEMASK_MOBILEWORK:
                    if(pNum == NULL || (pNum != NULL && Mask != AB_TYPEMASK_MOBILE && Mask != AB_TYPEMASK_MOBILEHOME))
                    {
                        Mask = AB_TYPEMASK_MOBILEWORK;
                        pNum = ptemp->pszData;
                    }
                    break;

                default:
                    break;
                }
                
                if(Mask == AB_TYPEMASK_MOBILE && pNum)
                    break;

                ptemp = ptemp->pNext;
            }

            if(pNum != NULL)
            {
                int nNumLen;
                
                nNumLen = min(pInit->MaxLenInfo.iNumberMaxLen,strlen(pNum));
                
                strncpy(Temp.PhoneNum,pNum,nNumLen);
            }
            
            Item_Erase(pItem);

            pName = AB_GetNameString(pIndexName[i]);
            
            nNameLen = strlen(pName);
            
            nNameLen = min(nNameLen,(int)(pInit->MaxLenInfo.iTextMaxLen));
            
            memcpy(Temp.Text,pName,nNameLen);
            
            Temp.Text[nNameLen] = 0;
            
            if(AB_SIMSaveToSIM(Temp))
            {
                memcpy(pPB,&Temp,sizeof(PHONEBOOK));
            }
            else
            {
                WaitWin(hWnd,FALSE,IDS_COPYING,IDS_CONTACTS,NULL,IDS_CANCEL,WM_CANCELCOPYING);
                
                AB_FREE(pData->pId);
                
                pData->nCount = 0;
                
                return;
            }
            
			j--;

		}
	}
                

    WaitWin(hWnd,FALSE,IDS_COPYING,IDS_CONTACTS,NULL,IDS_CANCEL,WM_CANCELCOPYING);            
    
    PLXTipsWin(NULL,NULL,0,IDS_COPIED,IDS_CONTACTS,Notify_Success,IDS_OK,NULL,WAITTIMEOUT);
    
    AB_FREE(pData->pId);
    
    pData->nCount = 0;
}

/*********************************************************************\
* Function	   ABMain_OnCopyAll
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void ABMain_OnCopyAll(HWND hWnd,BOOL bCopy)
{
    int  i;
    HWND hLst;
    CONTACT_ITEMCHAIN *pItem,*ptemp;
    char* pNum,*pName;
    DWORD Mask;
    int  nNameLen;
    int  tempid;
    PPHONEBOOK pPB;   
    PHONEBOOK  Temp;
    PCONTACT_INITDATA pInit;

	if(bCopy == FALSE)
	{
		return;
	}

    pInit = AB_GetSIMData();

    WaitWin(hWnd,TRUE,IDS_COPYING,IDS_CONTACTS,NULL,IDS_CANCEL,WM_CANCELCOPYING);

    hLst = GetDlgItem(hWnd,IDC_ABMAIN_LIST);

    for(i = nName ; i >= 1 ; i--)
    {
        if(AB_IsUserCancel(hWnd,WM_CANCELCOPYING))
        {
            WaitWin(hWnd,FALSE,IDS_COPYING,IDS_CONTACTS,NULL,IDS_CANCEL,WM_CANCELCOPYING);
            
            PLXTipsWin(NULL,NULL,0,IDS_MEMORYFULL,IDS_CONTACTS,Notify_Failure,IDS_OK,NULL,WAITTIMEOUT);
            
            return;
        }

        pPB = AB_FindUseableSIMPB();
        
        if(pPB == NULL)
        {
            WaitWin(hWnd,FALSE,IDS_COPYING,IDS_CONTACTS,NULL,IDS_CANCEL,WM_CANCELCOPYING);
            
            PLXTipsWin(NULL,NULL,0,IDS_MEMORYFULL,IDS_CONTACTS,Notify_Failure,IDS_OK,NULL,WAITTIMEOUT);
            
            return;
        }
        
        memset(&Temp,0,sizeof(PHONEBOOK));
        
        Temp.Index = AB_FindUseableIndex();
        
        pItem = NULL;
        
        AB_ReadRecord(pIndexName[i]->dwoffset,&pItem,&tempid);
        
        pNum = NULL;
        
        ptemp = pItem;
        
        while( ptemp )
        {
            switch(ptemp->dwMask) 
            {
            case AB_TYPEMASK_TEL:
                if(pNum == NULL || (pNum != NULL && (Mask == AB_TYPEMASK_TELWORK || Mask == AB_TYPEMASK_TELHOME)))
                {
                    Mask = AB_TYPEMASK_TEL;
                    pNum = ptemp->pszData;
                }
                break;
                
            case AB_TYPEMASK_TELHOME:
                if(pNum == NULL || (pNum != NULL && Mask == AB_TYPEMASK_TELWORK))
                {
                    Mask = AB_TYPEMASK_TELHOME;
                    pNum = ptemp->pszData;
                }
                break;
                
            case AB_TYPEMASK_TELWORK:
                if(pNum == NULL)
                {
                    Mask = AB_TYPEMASK_TELWORK;
                    pNum = ptemp->pszData;
                }
                break;
                
            case AB_TYPEMASK_MOBILE:
                Mask = AB_TYPEMASK_MOBILE;
                pNum = ptemp->pszData;
                break;
                
            case AB_TYPEMASK_MOBILEHOME:
                if(pNum == NULL || (pNum != NULL && Mask != AB_TYPEMASK_MOBILE))
                {
                    Mask = AB_TYPEMASK_MOBILEHOME;
                    pNum = ptemp->pszData;
                }
                break;
            case AB_TYPEMASK_MOBILEWORK:
                if(pNum == NULL || (pNum != NULL && Mask != AB_TYPEMASK_MOBILE && Mask != AB_TYPEMASK_MOBILEHOME))
                {
                    Mask = AB_TYPEMASK_MOBILEWORK;
                    pNum = ptemp->pszData;
                }
                break;
                
            default:
                break;
            }
            
            if(Mask == AB_TYPEMASK_MOBILE && pNum)
                break;
            
            ptemp = ptemp->pNext;
        }
        
        if(pNum != NULL)
        {
            int nNumLen;

            nNumLen = min(pInit->MaxLenInfo.iNumberMaxLen,strlen(pNum));
            
            strncpy(Temp.PhoneNum,pNum,nNumLen);
        }
            
        Item_Erase(pItem);
        
        pName = AB_GetNameString(pIndexName[i]);
        
        nNameLen = strlen(pName);
        
        nNameLen = min(nNameLen,(int)(pInit->MaxLenInfo.iTextMaxLen));
        
        memcpy(Temp.Text,pName,nNameLen);

        Temp.Text[nNameLen] = 0;
        
        if(AB_SIMSaveToSIM(Temp))
        {
            memcpy(pPB,&Temp,sizeof(PHONEBOOK));
        }
        else
        {
            WaitWin(hWnd,FALSE,IDS_COPYING,IDS_CONTACTS,NULL,IDS_CANCEL,WM_CANCELCOPYING);
            
            return;
        }
    }
                

    WaitWin(hWnd,FALSE,IDS_COPYING,IDS_CONTACTS,NULL,IDS_CANCEL,WM_CANCELCOPYING);            
    
    PLXTipsWin(NULL,NULL,0,IDS_COPIED,IDS_CONTACTS,Notify_Success,IDS_OK,NULL,WAITTIMEOUT);
}
/*********************************************************************\
* Function	   ABMain_OnSureDeleteSel
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void ABMain_OnSureDeleteSel(HWND hWnd,BOOL bDel)
{
	int i,j,index,nCount;
	HWND hLst;
    PABCREATEDATA pData;

    pData = GetUserData(hWnd);

    if(bDel == FALSE)
    {
        AB_FREE(pData->pId);

        pData->nCount = 0;

        return;
    }

    hLst = GetDlgItem(hWnd,IDC_ABMAIN_LIST);

	for(i = nName,j = pData->nCount-1 ; i >= 1 ; i--)
	{
		if(pData->pId[j] == pIndexName[i]->id)
		{			
            if(AB_DeleteRecord(pIndexName[i]->dwoffset))
			{    
                nCount = AB_DeleteFromIndexByID(pIndexTel,pIndexName[i]->id,nTel,FALSE);
                nTel -= nCount;
                nCount = AB_DeleteFromIndexByID(pIndexEmail,pIndexName[i]->id,nEmail,FALSE);
                nEmail -= nCount;
                nCount = AB_DeleteFromIndexByID(pIndexID,pIndexName[i]->id,nID,FALSE);
                nID -= nCount;
				nCount = AB_DeleteFromIndexByID(pIndexName,pIndexName[i]->id,nName,TRUE);
				nName--;
				
				SendMessage(hLst,LB_DELETESTRING,i,0);			
			}
			j--;
			index = i;
		}
	}

	if(index == SendMessage(hLst,LB_GETCOUNT,0,0))
		SendMessage(hLst,LB_SETCURSEL,index-1,0);
	else
		SendMessage(hLst,LB_SETCURSEL,index,0);
    
    if(pData->nCount == 1)
        PLXTipsWin(NULL,NULL,0,IDS_DELETED,IDS_CONTACTS,Notify_Success,IDS_OK,NULL,WAITTIMEOUT);
    else
        PLXTipsWin(NULL,NULL,0,IDS_DELETED,IDS_CONTACTS,Notify_Success,IDS_OK,NULL,WAITTIMEOUT);
    
    AB_FREE(pData->pId);
    
    pData->nCount = 0;
}

/*********************************************************************\
* Function	   AB_GenVcard
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL AB_GenVcard(CONTACT_ITEMCHAIN* pItem,char** ppVcardData,int *nVcardLen)
{
    CONTACT_ITEMCHAIN* pTemp; 
    vCard_Obj VcardObj;							
	vCard_Name Name;		
    vCard_Addr* pAddr = NULL,*pNewAddr = NULL;
    vCard_Tel* pTel = NULL,*pNewTel = NULL;			
	vCard_EMail* pEmail = NULL,*pNewEmail = NULL;			
	vCard_Comment* pComment = NULL;		
	vCard_Comment* pURL = NULL;			
	vCard_Comment* pTitle = NULL;				
	vCard_Org* pOrg = NULL;

    memset(&VcardObj,0,sizeof(vCard_Obj));
    memset(&Name,0,sizeof(vCard_Name));
    
    VcardObj.pName = &Name;

    pTemp = pItem;

    while(pTemp)
    {
        switch(pTemp->dwMask)
        {
        case AB_TYPEMASK_FIRSTNAME:
            Name.Charset = VCARD_CHARSET_UTF_8;
            
            Name.givenlen = AB_Code2UTF8(pTemp->pszData,&(Name.given));              
            break;

        case AB_TYPEMASK_LASTNAME:
            Name.Charset = VCARD_CHARSET_UTF_8;
            
            Name.famillen = AB_Code2UTF8(pTemp->pszData,&(Name.famil));  
            break;
            
        case AB_TYPEMASK_TEL://VCARD_TEL_HOME_MASK
        case AB_TYPEMASK_TELHOME://VCARD_TEL_HOME_MASK
        case AB_TYPEMASK_TELWORK://VCARD_TEL_WORK_MASK
        case AB_TYPEMASK_MOBILE://VCARD_TEL_CELL_MASK
        case AB_TYPEMASK_MOBILEHOME://VCARD_TEL_HOME_MASK
        case AB_TYPEMASK_MOBILEWORK://VCARD_TEL_WORK_MASK
        case AB_TYPEMASK_FAX://VCARD_TEL_FAX_MASK
        case AB_TYPEMASK_PAGER://VCARD_TEL_PAGER_MASK
        case AB_TYPEMASK_COMPANYTEL://VCARD_TEL_WORK_MASK
        case AB_TYPEMASK_DTMF://VCARD_TEL_HOME_MASK
			pNewTel = (vCard_Tel*)malloc(sizeof(vCard_Tel));
			
			if(pNewTel == NULL)
			{
				EarseVcard(&VcardObj);

				return FALSE;
			}
			
			memset(pNewTel,0,sizeof(vCard_Tel));

            pNewTel->Charset = VCARD_CHARSET_USASCII;

			switch(pTemp->dwMask)
			{
			case AB_TYPEMASK_TEL://VCARD_TEL_HOME_MASK
			case AB_TYPEMASK_TELHOME://VCARD_TEL_HOME_MASK
			case AB_TYPEMASK_MOBILEHOME://VCARD_TEL_HOME_MASK
			case AB_TYPEMASK_DTMF://VCARD_TEL_HOME_MASK
				pNewTel->Mask = VCARD_TEL_HOME_MASK;
				break;
				
			case AB_TYPEMASK_TELWORK://VCARD_TEL_WORK_MASK
			case AB_TYPEMASK_MOBILEWORK://VCARD_TEL_WORK_MASK
			case AB_TYPEMASK_COMPANYTEL://VCARD_TEL_WORK_MASK
				pNewTel->Mask = VCARD_TEL_WORK_MASK;
				break;
				
			case AB_TYPEMASK_FAX://VCARD_TEL_FAX_MASK
				pNewTel->Mask = VCARD_TEL_FAX_MASK;
				break;
				
			case AB_TYPEMASK_PAGER://VCARD_TEL_PAGER_MASK 
				pNewTel->Mask = VCARD_TEL_PAGER_MASK;
				break;
				
			case AB_TYPEMASK_MOBILE://VCARD_TEL_CELL_MASK
				pNewTel->Mask = VCARD_TEL_CELL_MASK;
				break;
			}

//             pNewTel->len = strlen(pTemp->pszData);
//            
//            pNewTel->number = malloc(pNewTel->len+1);
//
//            if(pNewTel->number == NULL)
//            {
//				EarseVcard(&VcardObj);
//
//				return FALSE;
//            }
//             strcpy(pNewTel->number,pTemp->pszData);
            
            pNewTel->Charset = VCARD_CHARSET_UTF_8;

            pNewTel->len = AB_Code2UTF8(pTemp->pszData,&(pNewTel->number));  
			            
            InsertVcardChain(&pTel,pNewTel,AB_TYPEMASK_TEL);

            break;
            
        case AB_TYPEMASK_EMAIL:
        case AB_TYPEMASK_EMAILWORK:
        case AB_TYPEMASK_EMAILHOME:
			pNewEmail = (vCard_EMail*)malloc(sizeof(vCard_EMail));
			
			if(pNewEmail == NULL)
			{
				EarseVcard(&VcardObj);

				return FALSE;
			}
			
			memset(pNewEmail,0,sizeof(vCard_EMail));

            pNewEmail->Charset = VCARD_CHARSET_UTF_8;
			pNewEmail->type = VCARD_EMAIL_INTERNET;
            pNewEmail->len = AB_Code2UTF8(pTemp->pszData,&(pNewEmail->Email));  
			
			InsertVcardChain(&pEmail,pNewEmail,AB_TYPEMASK_EMAIL);
            break;

        case AB_TYPEMASK_POSTALADDRESS:
        case AB_TYPEMASK_COMPANYADDRESS:
			pNewAddr = (vCard_Addr*)malloc(sizeof(vCard_Addr));
			
			if(pNewAddr == NULL)
			{
				EarseVcard(&VcardObj);

				return FALSE;
			}
			
			memset(pNewAddr,0,sizeof(vCard_Addr));

            pNewAddr->Charset = VCARD_CHARSET_UTF_8;

            pNewAddr->postOfficelen = AB_Code2UTF8(pTemp->pszData,&(pNewAddr->postOffice));  
			
			if(pTemp->dwMask == AB_TYPEMASK_POSTALADDRESS)
			{
				pNewAddr->Mask = VCARD_ADR_POSTAL_MASK;
			}
			else
			{
				pNewAddr->Mask = VCARD_ADR_WORK_MASK;
			}

			InsertVcardChain(&pAddr,pNewAddr,AB_TYPEMASK_POSTALADDRESS);
            break;

        case AB_TYPEMASK_HTTPURL:
			pURL = (vCard_Comment*)malloc(sizeof(vCard_Comment));
			
			if(pURL == NULL)
			{
				EarseVcard(&VcardObj);

				return FALSE;
			}
			
			memset(pURL,0,sizeof(vCard_Comment));

            pURL->Charset = VCARD_CHARSET_UTF_8;

            pURL->len = AB_Code2UTF8(pTemp->pszData,&(pURL->pData));  
			
            break;

        case AB_TYPEMASK_JOBTITLE:
			pTitle = (vCard_Comment*)malloc(sizeof(vCard_Comment));
			
			if(pTitle == NULL)
			{
				EarseVcard(&VcardObj);

				return FALSE;
			}
			
			memset(pTitle,0,sizeof(vCard_Comment));

            pTitle->Charset = VCARD_CHARSET_UTF_8;

            pTitle->len = AB_Code2UTF8(pTemp->pszData,&(pTitle->pData));  
			
            break;

        case AB_TYPEMASK_COMPANY:
			pOrg = (vCard_Org*)malloc(sizeof(vCard_Org));
			
			if(pOrg == NULL)
			{
				EarseVcard(&VcardObj);

				return FALSE;
			}
			
			memset(pOrg,0,sizeof(vCard_Org));

            pOrg->Charset = VCARD_CHARSET_UTF_8;

            pOrg->OrgNamelen = AB_Code2UTF8(pTemp->pszData,&(pOrg->OrgName));  
			
            break;

        case AB_TYPEMASK_DATE:
			sprintf(VcardObj.BirthDay,"%4d%2d%2d",((SYSTEMTIME*)(pTemp->pszData))->wYear,
				((SYSTEMTIME*)(pTemp->pszData))->wMonth,
				((SYSTEMTIME*)(pTemp->pszData))->wDay);
            break;

        case AB_TYPEMASK_NOTE:
			pComment = (vCard_Comment*)malloc(sizeof(vCard_Comment));
			
			if(pComment == NULL)
			{
				EarseVcard(&VcardObj);

				return FALSE;
			}
			
			memset(pComment,0,sizeof(vCard_Comment));

            pComment->Charset = VCARD_CHARSET_UTF_8;

            pComment->len = AB_Code2UTF8(pTemp->pszData,&(pComment->pData));  
			
            break;

        default:
            break;
        }
        pTemp = pTemp->pNext;
    }

    VcardObj.pAddr = pAddr;
    VcardObj.pComment = pComment;
    VcardObj.pEmail = pEmail;
    VcardObj.pTel = pTel;
    VcardObj.pURL = pURL;
    VcardObj.pOrg = pOrg;
    VcardObj.pTitle = pTitle;

	vCard_Writer(&VcardObj,ppVcardData,nVcardLen);
	
//	vCard_Clear(VCARD_OPT_WRITER, *ppVcardData);

	EarseVcard(&VcardObj);

    return TRUE;
}
/*********************************************************************\
* Function	   AB_Code2UTF8
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
int AB_Code2UTF8(char* pSrc,char** ppDes)
{
    int len;

    len = MultiByteToUTF8(CP_ACP, 0, (LPCSTR)pSrc, -1, NULL, 0);

    *ppDes = (char*)malloc(len);

    if(*ppDes == NULL)
        return 0;
    
    MultiByteToUTF8(CP_ACP, 0, (LPCSTR)pSrc, -1, (LPSTR)(*ppDes), len);
    
    return len - 1;
}

/*********************************************************************\
* Function	   EarseVcardChain
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void EarseVcardChain(vCard_Obj *pVcardObj,int nType)
{
	switch(nType)
	{
	case AB_TYPEMASK_EMAIL:
		{
			vCard_EMail *pTemp,*pTemp1;

			pTemp = pVcardObj->pEmail;

			while(pTemp)
			{
				pTemp1 = pTemp->pNext;

				AB_FREE(pTemp->Email);

				AB_FREE(pTemp);
				
				pTemp = pTemp1;
			}
		}
		break;
		
	case AB_TYPEMASK_TEL:
		{
			vCard_Tel *pTemp,*pTemp1;

			pTemp = pVcardObj->pTel;

			while(pTemp)
			{
				pTemp1 = pTemp->pNext;

				AB_FREE(pTemp->number);

				AB_FREE(pTemp);

				pTemp = pTemp1;
			}
		}
		break;

	case AB_TYPEMASK_POSTALADDRESS:
		{
			vCard_Addr *pTemp,*pTemp1;

			pTemp = pVcardObj->pAddr;

			while(pTemp)
			{
				pTemp1 = pTemp->pNext;

				AB_FREE(pTemp->postOffice);

				AB_FREE(pTemp);

				pTemp = pTemp1;
			}
		}
		break;

	default:
		break;
	}
}

/*********************************************************************\
* Function	   InsertVcardChain
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void InsertVcardChain(PVOID *ppPointer,PVOID pNew,int nType)
{	
	switch(nType)
	{
	case AB_TYPEMASK_EMAIL:
		{
			vCard_EMail *pTemp;

			pTemp = (vCard_EMail*)(*ppPointer);

			if(pTemp == NULL)
			{
				*ppPointer = pNew;
				
				return;
			}

			while(pTemp)
			{
				if(pTemp->pNext)
					pTemp = pTemp->pNext;
				else
					break;
			}

			pTemp->pNext = pNew;
		}
		break;
		
	case AB_TYPEMASK_TEL:
		{
			vCard_Tel *pTemp;

			pTemp = (vCard_Tel*)(*ppPointer);

			if(pTemp == NULL)
			{
				*ppPointer = pNew;
				
				return;
			}

			while(pTemp)
			{
				if(pTemp->pNext)
					pTemp = pTemp->pNext;
				else
					break;
			}

			pTemp->pNext = pNew;
		}
		break;

	case AB_TYPEMASK_POSTALADDRESS:
		{
			vCard_Addr *pTemp;

			pTemp = (vCard_Addr*)(*ppPointer);

			if(pTemp == NULL)
			{
				*ppPointer = pNew;
				
				return;
			}

			while(pTemp)
			{
				if(pTemp->pNext)
					pTemp = pTemp->pNext;
				else
					break;
			}

			pTemp->pNext = pNew;
		}
		break;

	default:
		break;
	}
}

/*********************************************************************\
* Function	   EarseVcard
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void EarseVcard(vCard_Obj *pVcardObj)
{
	if(pVcardObj->pName)
	{
		AB_FREE(pVcardObj->pName->famil);
		AB_FREE(pVcardObj->pName->given);
	}

	if(pVcardObj->pURL)
	{
		AB_FREE(pVcardObj->pURL->pData);
		AB_FREE(pVcardObj->pURL);
	}
	
	if(pVcardObj->pTitle)
	{
		AB_FREE(pVcardObj->pTitle->pData);
		AB_FREE(pVcardObj->pTitle);
	}
	
	if(pVcardObj->pComment)
	{
		AB_FREE(pVcardObj->pComment->pData);
		AB_FREE(pVcardObj->pComment);
	}
	
	if(pVcardObj->pOrg)
	{
		AB_FREE(pVcardObj->pOrg->OrgName);
		AB_FREE(pVcardObj->pOrg);
	}

	EarseVcardChain(pVcardObj,AB_TYPEMASK_POSTALADDRESS);
	EarseVcardChain(pVcardObj,AB_TYPEMASK_EMAIL);
	EarseVcardChain(pVcardObj,AB_TYPEMASK_TEL);
}
/*********************************************************************\
* Function	   AB_GetUseableDuplicateName
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL AB_GetUseableDuplicateName(char* pszName,PCONTACT_ITEMCHAIN *ppItem)
{
    char szName[256];
    int ID = 1;
    CONTACT_ITEMCHAIN *pTemp,*pNewItem;
    int nLen;

    while( TRUE )
    {
        szName[0] = 0;

        sprintf(szName,"%s %d",pszName,ID);

        if(Memory_Find((PVOID)szName,AB_FIND_NAME) == NULL)
        {
            pTemp = *ppItem;

            while(pTemp)
            {
                switch(pTemp->dwMask)
                {
                case AB_TYPEMASK_FIRSTNAME:
                    
                    szName[0] = 0;

                    sprintf(szName,"%s %d",pTemp->pszData,ID);

                    nLen = strlen(szName);

                    if(nLen >= AB_MAXLEN_FIRSTNAME)
                        szName[AB_MAXLEN_FIRSTNAME-1] = 0;

                    strcpy(pTemp->pszData,szName);
                    
                    return TRUE;

                default:
                    break;
                }
                pTemp = pTemp->pNext;
            }
            
            pNewItem = Item_New(AB_TYPEMASK_FIRSTNAME);
            
            if(pNewItem == NULL)                
                return FALSE;
            
            pNewItem->dwMask = AB_TYPEMASK_FIRSTNAME;
            pNewItem->bChange = TRUE;
            pNewItem->nMaxLen = AB_MAXLEN_FIRSTNAME;
            sprintf(pNewItem->pszData,"%d",ID);
            
            Item_Insert(ppItem,pNewItem);
            
            return TRUE;
        }

        ID++;
    }
}
/*********************************************************************\
* Function	   AB_FindByName
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
MEMORY_NAME* AB_FindByName(char* szName)
{
    int nmiddle,nstart,nend;
    int nCmpRet;
    char* pszName;
     
    nstart = 1;
    nend = nName;

    while(nend >= nstart)
    {
        nmiddle = (nend+nstart)/2;
        pszName = AB_GetNameString(pIndexName[nmiddle]);
        nCmpRet = strcmp(szName,pszName);
        if(nCmpRet == 0)
        {
            return pIndexName[nmiddle];
        }
        else if(nCmpRet > 0)
            nstart = nmiddle+1;
        else
            nend = nmiddle-1;
    }

    return NULL;
}
/*********************************************************************\
* Function	   AB_IsExistMask
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL AB_IsExistMask(CONTACT_ITEMCHAIN* pItem,DWORD dwMask)
{
    CONTACT_ITEMCHAIN* pTemp;
    
    pTemp = pItem;

    while(pTemp)
    {
        if(pTemp->dwMask == dwMask)
            return TRUE;

        pTemp = pTemp->pNext;
    }

    return FALSE;
}
/*********************************************************************\
* Function	   AB_IsUserCancel
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL AB_IsUserCancel(HWND hWnd,UINT uMsgCmd)
{
	MSG msg;

	while (PeekMessage(&msg, NULL, 0, 0, 1))
    {	
        TranslateMessage(&msg);
        DispatchMessage(&msg);

		if((msg.hwnd == hWnd && msg.message == uMsgCmd)
			||(msg.message == WM_KEYDOWN && msg.wParam == VK_F10))
		{
			cancel = FALSE;
			return TRUE;
		}
    }

	if(cancel)
	{
		cancel = FALSE;
		return TRUE;
	}
	
	return FALSE;
}

void AB_ExitApp(HWND hFrameWnd)
{
    HWND hMainWnd = NULL;

    PABCREATEDATA pData;
    
    hMainWnd = GetDlgItem(hFrameWnd,IDC_AB_MAIN);

    pData = GetUserData(hMainWnd);

    DlmNotify((WPARAM)PES_STCQUIT, (LPARAM)(pData->hInstance));

    DestroyWindow(hFrameWnd);
}

/*********************************************************************\
* Function	   AB_SaveVcardFile
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL AB_SaveVcardFile(char* pszVcardFileName,char* pVcardData,int nVcardLen)
{
    int  f;
    int  n;

    mkdir(PATH_DIR_AB_BT,S_IRUSR);
    
    n = 0;

    while(TRUE)
    {
        sprintf(pszVcardFileName,"%s%d%s",PATH_DIR_AB_BT,n,AB_VCARD_PORTFIX);

        f = open(pszVcardFileName,O_RDONLY);

        if(f == -1)
            break;

        close(f);

        n++;
    }

    f = open(pszVcardFileName,O_RDWR | O_CREAT,S_IRWXU);

    if(f == -1)
    {
        pszVcardFileName[0] = 0;

        return FALSE;
    }

    write(f,pVcardData,nVcardLen);

    close(f);

    return TRUE;
}


BOOL AB_CreateTmp(char* filename, char* pData, int len)
{
	int f;
	char sztmp[256];

	sprintf(sztmp,"%s%s",PATH_DIR_AB_BT, filename);
	strcpy(filename, sztmp);

	f = open(filename, O_RDWR | O_CREAT,S_IRWXU);

	if(f == -1)
		return FALSE;

	write(f, pData, len);

	close(f);

	return TRUE;
}


BOOL AB_ClearTmp(char* filename)
{
	unlink(filename);
	return TRUE;
}

BOOL AB_AllConfirmNum(void)
{
	int i;
	CONTACT_ITEMCHAIN* pItem = NULL;
	int nTelID;

	for(i=1 ; i <= nName ; i++)
	{	
		AB_ReadRecord(pIndexName[i]->dwoffset,&pItem,&nTelID);

		if(AB_HasNumber(pItem))
		{
			Item_Erase(pItem);
			return TRUE;
		}

		Item_Erase(pItem);
		pItem = NULL;
	}

	return FALSE;
}

BOOL AB_SelectConfirmNum(DWORD* pID, int count)
{
	int i;
	MEMORY_NAME *p;
	CONTACT_ITEMCHAIN* pItem = NULL;
	int nTelID;

	for(i=0; i<count; i++)
	{
		p = Memory_Find((PVOID)(pID[i]),AB_FIND_ID);

		if(p == NULL)
			return FALSE;

		AB_ReadRecord(p->dwoffset,&pItem,&nTelID);

		if(AB_HasNumber(pItem))
		{
			Item_Erase(pItem);
			return TRUE;
		}

		Item_Erase(pItem);
	}

	return FALSE;
}
