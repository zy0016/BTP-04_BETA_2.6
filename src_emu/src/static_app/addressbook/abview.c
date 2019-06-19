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

typedef enum
{
    AB_VIEW_LIST = 0,
    AB_VIEW_PIC,
}AB_VIEW_TYPE;

#define IDC_ABVIEW_LIST                 300
#define IDC_EDIT                        100
#define IDC_EXIT                        200

#define IDM_ABVIEW_WRITE_SMS        1031 
#define IDM_ABVIEW_WRITE_MMS        1032
#define IDM_ABVIEW_WRITE_EMAIL      1033

#define IDM_ABVIEW_SEND_SMS         1041 
#define IDM_ABVIEW_SEND_BULETOOTH   1042
#define IDM_ABVIEW_SEND_EMAIL       1043
#define IDM_ABVIEW_SEND_MMS			1044

#define IDM_ABVIEW_DELETE           102
#define IDM_ABVIEW_WRITE            103
#define IDM_ABVIEW_SEND             104
#define IDM_ABVIEW_DUPLICATE        105
#define IDM_ABVIEW_SETPIC           106
#define IDM_ABVIEW_SETTONE          107
#define IDM_ABVIEW_COPYTOSIM        108

LRESULT ABViewContactWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static BOOL ABViewContact_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct);
static void ABViewContact_OnActivate(HWND hwnd, UINT state);
static void ABViewContact_OnSetFocus(HWND hWnd);
static void ABViewContact_OnPaint(HWND hWnd);
static void ABViewContact_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags);
static LRESULT ABViewContact_OnCommand(HWND hWnd, int id, UINT codeNotify,LPARAM lParam);
static void ABViewContact_OnDestroy(HWND hWnd);
static void ABViewContact_OnClose(HWND hWnd);
static void ABViewContact_OnDataChange(HWND hWnd,DWORD id,int nMode);
static void ABViewContact_OnDeleteContact(HWND hWnd,BOOL bDelete);
static void ABViewContact_OnDealSelect(HWND hWnd,UINT wMsgCmd,int nTelID,DWORD id);

static LRESULT CallListWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
BOOL AB_CopyItem(PCONTACT_ITEMCHAIN* ppHeader,CONTACT_ITEMCHAIN* pItemChain);
static int  AB_GetImageType(char* pszPathFileName);
void PaintBkFunc(HGIFANIMATE hGIFAnimate, BOOL bEnd ,int x, int y, HDC hdcMem);

BOOL AB_IsTel(DWORD dwMask);
BOOL AB_IsEmail(DWORD dwMask);

extern BOOL AB_CreateTmp(char* filename, char* pData, int len);
extern BOOL AB_ClearTmp(char* filename);

static HBITMAP hBmp = NULL;
/*********************************************************************\
* Function	   AB_CreateViewContactWnd
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL AB_CreateViewContactWnd(HWND hFrameWnd,CONTACT_ITEMCHAIN* pItemData,DWORD dwoffset,DWORD id,int nTelID)
{
    WNDCLASS    wc;
    ABVIEWCREATEDATA Data;
    HWND        hViewWnd = NULL;  
    HMENU       hWriteMenu,hSendMenu;
    char        szCaption[AB_MAXLEN_FIRSTNAME*2];
    char        *p=NULL,*q=NULL;
	RECT		rcClient;

    wc.style         = 0;
    wc.lpfnWndProc   = ABViewContactWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = sizeof(ABVIEWCREATEDATA);
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName  = "ABViewContactWndClass";
    
    if (!RegisterClass(&wc))
        return FALSE;

    memset(&Data,0,sizeof(ABVIEWCREATEDATA));

	Data.hFrameWnd = hFrameWnd;

    if(AB_CopyItem(&(Data.pItem),pItemData) == FALSE)
		return FALSE;

    Data.nType = AB_VIEW_LIST;
    Data.dwoffset = dwoffset;
    Data.id = id; 
    Data.bStretch = FALSE;
    Data.hGif = NULL;
    Data.nImageType = IMG_UNKNOWN;
    Data.pPic = NULL;
    Data.pTone = NULL;
	Data.nTelID = nTelID;
    
    szCaption[0] = 0;

	AB_GetFullName(pItemData,szCaption);

	if(szCaption[0] == 0)
		strcpy(szCaption, "Unnamed");

	GetClientRect(hFrameWnd,&rcClient);

    Data.hMenu = CreateMenu();

    hViewWnd = CreateWindow(
        "ABViewContactWndClass", 
        "",
        WS_CHILD | WS_VISIBLE,
        rcClient.left,
		rcClient.top,
		rcClient.right - rcClient.left,
		rcClient.bottom - rcClient.top,
        hFrameWnd,
        (HMENU)IDC_AB_VIEW,
        NULL, 
        (PVOID)&Data
        );
    
    if (!hViewWnd)
    {
		DestroyMenu(Data.hMenu);

        Item_Erase(Data.pItem);
        UnregisterClass("ABViewContactWndClass", NULL);
        return FALSE;
    }

	SetFocus(hViewWnd);
    
    SetWindowText(hFrameWnd,szCaption);

    AppendMenu(Data.hMenu, MF_ENABLED, IDC_EDIT, IDS_EDIT);
    
    hWriteMenu = CreateMenu();
    AppendMenu(hWriteMenu, MF_ENABLED, IDM_ABVIEW_WRITE_SMS, IDS_SMS);
    AppendMenu(hWriteMenu, MF_ENABLED, IDM_ABVIEW_WRITE_MMS, IDS_MMS);
    AppendMenu(hWriteMenu, MF_ENABLED, IDM_ABVIEW_WRITE_EMAIL, IDS_EMAIL);
    
    AppendMenu(Data.hMenu, MF_POPUP|MF_ENABLED, (DWORD)hWriteMenu, IDS_WRITE);
    
    hSendMenu = CreateMenu();
    AppendMenu(hSendMenu, MF_ENABLED, IDM_ABVIEW_SEND_SMS, IDS_VIASMS);
	AppendMenu(hSendMenu, MF_ENABLED, IDM_ABVIEW_SEND_MMS, IDS_VIAMMS);
	AppendMenu(hSendMenu, MF_ENABLED, IDM_ABVIEW_SEND_EMAIL, IDS_VIAEMAIL);
    AppendMenu(hSendMenu, MF_ENABLED, IDM_ABVIEW_SEND_BULETOOTH, IDS_VIABLUETOOTH);
    
    
    AppendMenu(Data.hMenu, MF_POPUP|MF_ENABLED, (DWORD)hSendMenu, IDS_SEND);
    
    AppendMenu(Data.hMenu, MF_ENABLED, IDM_ABVIEW_DUPLICATE, IDS_DUPLICATECONTACT);
//    AppendMenu(Data.hMenu, MF_ENABLED, IDM_ABVIEW_SETPIC, IDS_SETPIC);
//    AppendMenu(Data.hMenu, MF_ENABLED, IDM_ABVIEW_SETTONE, IDS_SETTONE);
    AppendMenu(Data.hMenu, MF_ENABLED, IDM_ABVIEW_COPYTOSIM, IDS_COPYTOSIM);

	AppendMenu(Data.hMenu, MF_ENABLED, IDM_ABVIEW_DELETE, IDS_DELETE);

	PDASetMenu(hFrameWnd,Data.hMenu);

    SendMessage(hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_EXIT,0), (LPARAM)IDS_BACK);
    SendMessage(hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_EDIT,1), (LPARAM)IDS_EDIT);
    SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_OPTIONS);

    ShowWindow(hViewWnd,SW_SHOW);
    UpdateWindow(hViewWnd);

    return FALSE;
}

/*********************************************************************\
* Function	ABViewContactWndProc
* Purpose   Main window proc
* Params
*			hWnd: Handle of the window
*			wMsgCmd: Message ID
* Return
*			TRUE: Success
*			FALSE: Fail
* Remarks
**********************************************************************/
static LRESULT ABViewContactWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    LRESULT lResult = TRUE;

	switch (wMsgCmd)
    {
    case WM_CREATE:
        lResult = ABViewContact_OnCreate(hWnd,(LPCREATESTRUCT)(lParam));
        break;

    case WM_ACTIVATE:
	case PWM_SHOWWINDOW:
        ABViewContact_OnActivate(hWnd,(UINT)LOWORD(wParam));
        break;
        
    case WM_SETFOCUS:
        ABViewContact_OnSetFocus(hWnd);
        break;

    case WM_PAINT:
        ABViewContact_OnPaint(hWnd);
        break;

    case WM_KEYDOWN:
        ABViewContact_OnKey(hWnd,(UINT)(wParam),(int)(short)LOWORD(lParam),(UINT)HIWORD(lParam));
        break;

    case WM_COMMAND:
        lResult = ABViewContact_OnCommand(hWnd,(int)(LOWORD(wParam)),(UINT)HIWORD(wParam),lParam);
        break;
    
	case WM_WRITESMS:
    case WM_WRITEMMS:
    case WM_WRITEEMAIL:
		ABViewContact_OnDealSelect(hWnd,wMsgCmd,(int)wParam,(DWORD)lParam);
		break;

    case WM_CLOSE:
        ABViewContact_OnClose(hWnd);
        break;

    case WM_DESTROY:
        ABViewContact_OnDestroy(hWnd);
        break;

    case WM_DATACHANGE:
        ABViewContact_OnDataChange(hWnd,(DWORD)wParam,(int)lParam);
        break;

    case WM_DELETECONTACT:
        ABViewContact_OnDeleteContact(hWnd,(BOOL)lParam);
        break;

    default:     
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
	}

    return lResult;

}
/*********************************************************************\
* Function	ABViewContact_OnCreate
* Purpose   WM_CREATE message handler of the main window
* Params
*			hWnd: Handle of the window
*			lpCreateStruct: Create Structure
* Return
*			TRUE: Success
*			FALSE: Fail
* Remarks
**********************************************************************/
static BOOL ABViewContact_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
{
    RECT rect; 
    HWND hList;   
    ABVIEWCREATEDATA *pCreateData;
    PCONTACT_ITEMCHAIN pTmp;
    int  nIndex,i;
    RECT rcClient;
    int  nWidth = 0,nHeight = 0;
    char szCaption[100];
	BOOL bExistNumber = FALSE;

    pCreateData = (ABVIEWCREATEDATA*)GetUserData(hWnd);

    memcpy(pCreateData,(ABVIEWCREATEDATA*)(lpCreateStruct->lpCreateParams),sizeof(ABVIEWCREATEDATA));

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
        (HMENU)IDC_ABVIEW_LIST,
        NULL,
        NULL);
   
    if(hList == NULL)
        return FALSE;

    pCreateData->OldListWndProc = (WNDPROC)SetWindowLong(hList,GWL_WNDPROC,(LONG)CallListWndProc);

	pCreateData->hLeftArrow = LoadImage(NULL, AB_BMP_ARROWLEFT, IMAGE_BITMAP,
		ICON_WIDTH, ICON_HEIGHT, LR_LOADFROMFILE);

    pCreateData->hRightArrow = LoadImage(NULL, AB_BMP_ARROWRIGHT, IMAGE_BITMAP,
		ICON_WIDTH, ICON_HEIGHT, LR_LOADFROMFILE);

    SendMessage(pCreateData->hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON),(LPARAM)NULL);
    SendMessage(pCreateData->hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, RIGHTICON),(LPARAM)AB_ICON_ARROWRIGHT);
    
//    hBmp = LoadImage(NULL, "/rom/message/unibox/mail_aff.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

    pTmp = pCreateData->pItem;

    while(pTmp)
    {
		if(AB_IsTel(pTmp->dwMask) || AB_IsEmail(pTmp->dwMask))
			bExistNumber = TRUE;

        if(pTmp->dwMask == AB_TYPEMASK_PIC)
        {
            pCreateData->pPic = pTmp->pszData;
			pCreateData->nImageType = AB_GetImageType(pTmp->pszData);
            GetImageDimensionFromFile(pCreateData->pPic,&(pCreateData->ImageSize));
            GetClientRect(hWnd,&rcClient);
            nWidth = rcClient.right - rcClient.left;
            nHeight = rcClient.bottom - rcClient.top;
            if(pCreateData->ImageSize.cx > nWidth || pCreateData->ImageSize.cy > nHeight)
                pCreateData->bStretch = TRUE;
        }
        else if(pTmp->dwMask == AB_TYPEMASK_TONE)
            pCreateData->pTone = pTmp->pszData;
        else if(pTmp->dwMask != AB_TYPEMASK_FIRSTNAME && pTmp->dwMask != AB_TYPEMASK_LASTNAME)
        {
            nIndex = GetIndexByMask(pTmp->dwMask);
            
            szCaption[0] = 0;
            
            sprintf(szCaption,"%s:",Contact_Item[nIndex].pszCpation);
            
            i = SendMessage(hList, LB_ADDSTRING, -1, (LPARAM)szCaption);
            //SendMessage(hList, LB_SETIMAGE, (WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)i),(LPARAM)hBmp);
            
            if(pTmp->dwMask == AB_TYPEMASK_DATE)
            {
                char szTime[50];
                
                char szDate[50];
                
                szTime[0] = 0;
                
                szDate[0] = 0;
                
                GetTimeDisplay(*((SYSTEMTIME*)(pTmp->pszData)),szTime,szDate);
                
                SendMessage(hList, LB_SETAUXTEXT, MAKEWPARAM(i, -1), (LPARAM)szDate);
            }
            else
                SendMessage(hList, LB_SETAUXTEXT, MAKEWPARAM(i, -1), (LPARAM)(pTmp->pszData));
            SendMessage(hList, LB_SETITEMDATA, i, (LPARAM)pTmp);
        }
        
        pTmp = pTmp->pNext;
    }
    
	if(!bExistNumber)
		PLXTipsWin(NULL,NULL,0,IDS_NOPHONENUMOREMAIL,"",Notify_Alert,IDS_OK,NULL,WAITTIMEOUT);
	
    SendMessage(hList, LB_SETCURSEL, 0, 0);

//    pCreateData->handle = AB_RegisterNotify(hWnd,WM_DATACHANGE,AB_OBJECT_ALL,AB_MDU_CONTRACT);

    return TRUE;
    
}
/*********************************************************************\
* Function	ABViewContact_OnActivate
* Purpose   WM_ACTIVATE message handler of the main window
* Params
* Return    None
* Remarks
**********************************************************************/
static void ABViewContact_OnActivate(HWND hWnd, UINT state)
{
    HWND hLst;
    ABVIEWCREATEDATA *pCreateData;
    char        szCaption[AB_MAXLEN_FIRSTNAME*2];

    pCreateData = (ABVIEWCREATEDATA*)GetUserData(hWnd);

    hLst = GetDlgItem(hWnd,IDC_ABVIEW_LIST);
    
    if(pCreateData->nType == AB_VIEW_LIST)
    {
        SetFocus(hLst);
        SendMessage(pCreateData->hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON),(LPARAM)NULL);
        SendMessage(pCreateData->hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, RIGHTICON),(LPARAM)AB_ICON_ARROWRIGHT);
    }
    else
    {
        SetFocus(hWnd);
        SendMessage(pCreateData->hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, LEFTICON),(LPARAM)AB_ICON_ARROWLEFT);
        SendMessage(pCreateData->hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON),(LPARAM)NULL);
    }

	PDASetMenu(pCreateData->hFrameWnd,pCreateData->hMenu);

    SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_BACK);
    SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_EDIT);
    SendMessage(pCreateData->hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_OPTIONS);
    
    szCaption[0] = 0;

	AB_GetFullName(pCreateData->pItem,szCaption);
	
	if(szCaption[0] == 0)
		strcpy(szCaption, "Unnamed");

	SetWindowText(pCreateData->hFrameWnd,szCaption);
	
    return;
}
/*********************************************************************\
* Function	ABViewContact_OnSetFocus
* Purpose   WM_SETFOCUS message handler of the main window
* Params
* Return    None
* Remarks
**********************************************************************/
static void ABViewContact_OnSetFocus(HWND hWnd)
{
    HWND hLst;
    ABVIEWCREATEDATA *pCreateData;

    pCreateData = (ABVIEWCREATEDATA*)GetUserData(hWnd);


    hLst = GetDlgItem(hWnd,IDC_ABVIEW_LIST);
	
	if(pCreateData->nType == AB_VIEW_LIST)
		SetFocus(hLst);
	else
		SetFocus(hWnd);

    return;
}
/*********************************************************************\
* Function	ABViewContact_OnPaint
* Purpose   WM_PAINT message handler of the main window
* Params	hWnd: Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void ABViewContact_OnPaint(HWND hWnd)
{
    ABVIEWCREATEDATA *pCreateData;
    
    RECT rcClient;

    int  nWidth = 0,nHeight = 0;

    int  drawwidth = 0,drawheight = 0;

    int  x = 0,y = 0;

    int  nMode;

    COLORREF OldClr;
	
    HDC hdc = BeginPaint(hWnd, NULL);
    
    pCreateData = (ABVIEWCREATEDATA*)GetUserData(hWnd);
    
    GetClientRect(hWnd,&rcClient);

    nWidth = rcClient.right-rcClient.left;
    
    nHeight = rcClient.bottom - rcClient.top;

    if(pCreateData->nType == AB_VIEW_PIC)
    {
        if(pCreateData->pPic == NULL)
        {
            OldClr = SetBkColor(hdc, COLOR_TRANSBK);
            nMode = SetBkMode(hdc,BM_TRANSPARENT);
            
            //Draw image
            DrawText(hdc,IDS_NOPICTURE,-1,&rcClient,DT_CENTER | DT_VCENTER | DT_CLEAR);
            SetBkMode(hdc,nMode);
            
            OldClr = SetBkColor(hdc, COLOR_TRANSBK);
        }
        else
        {
            if(pCreateData->bStretch)
            {
                if( (nWidth*pCreateData->ImageSize.cy - nHeight*pCreateData->ImageSize.cx) < 0)
                {
                    x = 0;

                    drawwidth = nWidth;
                    
                    drawheight = pCreateData->ImageSize.cy * nWidth / pCreateData->ImageSize.cx;

                    y = (nHeight - drawheight) / 2;
                }
                else
                {
                    y = 0;

                    drawheight = nHeight;
                    
                    drawwidth = pCreateData->ImageSize.cx * nHeight / pCreateData->ImageSize.cy;

                    x = (nWidth - drawwidth) / 2;
                }
                switch(pCreateData->nImageType)
                {
                case IMG_BMP:
                case IMG_WBMP:
                    StretchWBMP(hdc,pCreateData->pPic,x,y,drawwidth,drawheight,SRCCOPY);
                    break;

                case IMG_GIF:
                    StretchGif(hdc,pCreateData->pPic,x,y,drawwidth,drawheight,SRCCOPY);
                    break;

                case IMG_JPEG:
                    StretchJpegFromFile(hdc,pCreateData->pPic,x,y,drawwidth,drawheight,SRCCOPY);
                    break;
                    
                case IMG_UNKNOWN:
                default:
                    break;
                }
            }
            else
            {
                if(pCreateData->hGif)
                {
                    PaintAnimatedGIF(hdc, pCreateData->hGif);
                }
                else
                {
                    x = (nWidth - pCreateData->ImageSize.cx) / 2;
                    
                    y = (nHeight - pCreateData->ImageSize.cy) / 2;

                    DrawImageFromFile(hdc,pCreateData->pPic,x,y,SRCCOPY);
                }
            }
        }
    }
	
    EndPaint(hWnd, NULL);

	return;
}

/*********************************************************************\
* Function	ABViewContact_OnKey
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
static void ABViewContact_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags)
{
    ABVIEWCREATEDATA *pCreateData;
    CONTACT_ITEMCHAIN* pFocus;
    HWND hLst;
    int  nSel;

	switch (vk)
	{
	case VK_F10:
		PostMessage(hWnd,WM_CLOSE,NULL,NULL);
        break;

	case VK_RETURN:
		SendMessage(hWnd,WM_COMMAND,IDC_EDIT,NULL);
		break;

	case VK_F5:
        pCreateData = (ABVIEWCREATEDATA*)GetUserData(hWnd);
		PDADefWindowProc(pCreateData->hFrameWnd, WM_KEYDOWN, vk, MAKELPARAM(cRepeat, flags));
		break;
        
    case VK_LEFT:
        pCreateData = (ABVIEWCREATEDATA*)GetUserData(hWnd);
        if(pCreateData->nType == AB_VIEW_PIC)
        {
            hLst = GetDlgItem(hWnd,IDC_ABVIEW_LIST);
            ShowWindow(hLst,SW_SHOW);
            SendMessage(pCreateData->hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON),(LPARAM)NULL);
            SendMessage(pCreateData->hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, RIGHTICON),(LPARAM)AB_ICON_ARROWRIGHT);            
            SetFocus(hLst);
            pCreateData->nType = AB_VIEW_LIST;

			if(pCreateData->hGif != NULL)
			{
				EndAnimatedGIF(pCreateData->hGif);
				pCreateData->hGif = NULL;
			}
        }
        break;
        
    case VK_RIGHT:
        pCreateData = (ABVIEWCREATEDATA*)GetUserData(hWnd);
        if(pCreateData->nType == AB_VIEW_LIST)
        {
            hLst = GetDlgItem(hWnd,IDC_ABVIEW_LIST);
            SetFocus(pCreateData->hFrameWnd);
            ShowWindow(hLst,SW_HIDE);
            SendMessage(pCreateData->hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, LEFTICON),(LPARAM)AB_ICON_ARROWLEFT);
            SendMessage(pCreateData->hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON),(LPARAM)NULL);
            SetFocus(hWnd);
            pCreateData->nType = AB_VIEW_PIC;

			if(pCreateData->pPic != NULL)
			{
				if(pCreateData->nImageType == IMG_GIF && pCreateData->bStretch == FALSE)
				{
					HDC hdc;
					int x = 0,y = 0;
					int nWidth = 0, nHeight = 0;
					RECT rcClient;
					
					GetClientRect(hWnd,&rcClient);
					nWidth = rcClient.right - rcClient.left;
					nHeight = rcClient.bottom - rcClient.top;
					x = (nWidth - pCreateData->ImageSize.cx)/2;
					y = (nHeight - pCreateData->ImageSize.cy)/2;
					pCreateData->hGif = StartAnimatedGIFFromFile(hWnd,pCreateData->pPic,x,y,DM_NONE);
					SetPaintBkFunc(pCreateData->hGif, PaintBkFunc);
					
					hdc = GetDC(hWnd);
					PaintAnimatedGIF(hdc, pCreateData->hGif);
					ReleaseDC(hWnd,hdc);
				}
			}
        }
        break;

    case VK_F1:
        pCreateData = (ABVIEWCREATEDATA*)GetUserData(hWnd);
        hLst = GetDlgItem(hWnd,IDC_ABVIEW_LIST);
        nSel = SendMessage(hLst,LB_GETCURSEL,0,0);
        if(nSel == LB_ERR)
            break;
        
        pFocus = (CONTACT_ITEMCHAIN*)SendMessage(hLst,LB_GETITEMDATA,nSel,0);
        switch(pFocus->dwMask)
        {
        case AB_TYPEMASK_TEL:
        case AB_TYPEMASK_TELHOME:
        case AB_TYPEMASK_TELWORK:
        case AB_TYPEMASK_MOBILE:
        case AB_TYPEMASK_MOBILEHOME:
        case AB_TYPEMASK_MOBILEWORK:
        case AB_TYPEMASK_COMPANYTEL:
        case AB_TYPEMASK_FAX:
        case AB_TYPEMASK_PAGER:
        case AB_TYPEMASK_DTMF:
            if(strlen(pFocus->pszData) != 0)
                APP_CallPhoneNumber(pFocus->pszData);
        	break;

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
* Function	ABViewContact_OnCommand
* Purpose   WM_COMMAND message handler of the main window
* Params
*			hWnd:	Handle of the window
*			id:		Id of command message
*			hwndCtl: Handle of the window which sended this message
*			codeNotify:Notify code of the message
* Return	None
* Remarks
**********************************************************************/
static LRESULT ABViewContact_OnCommand(HWND hWnd, int id, UINT codeNotify,LPARAM lParam)
{
    ABVIEWCREATEDATA *pCreateData;
    CONTACT_ITEMCHAIN* pFocus;
    HWND hLst;
    int  nSel;

    pCreateData = (ABVIEWCREATEDATA*)GetUserData(hWnd);
    hLst = GetDlgItem(hWnd,IDC_ABVIEW_LIST);

	switch(id)
	{
    case IDC_EXIT:
        PostMessage(hWnd,WM_CLOSE,0,0);
        break;

    case IDC_ABVIEW_LIST:
        if(codeNotify == LBN_SETFONT)
        {            
            HFONT hFont = NULL;
            
            GetFontHandle(&hFont, (lParam == 0) ? SMALL_FONT : LARGE_FONT);
            
            return (LRESULT)hFont;
        }
        break;

    case IDC_EDIT:
        nSel = SendMessage(hLst,LB_GETCURSEL,0,0);

        if(nSel == LB_ERR)
            pFocus = pCreateData->pItem;
        else
            pFocus = (CONTACT_ITEMCHAIN*)SendMessage(hLst,LB_GETITEMDATA,nSel,0);
        
        AB_CreateEditContactWnd(pCreateData->hFrameWnd,NULL,0,pCreateData->pItem,pFocus,
            FALSE,pCreateData->dwoffset,pCreateData->id,pCreateData->nTelID,hWnd);
        break;

    case IDM_ABVIEW_WRITE_SMS:
        //nSel = SendMessage(hLst,LB_GETCURSEL,0,0);
        //if(nSel == LB_ERR)
        //    break;
        //pFocus = (CONTACT_ITEMCHAIN*)SendMessage(hLst,LB_GETITEMDATA,nSel,0);
		AB_SelectPhoneOrEmail(pCreateData->hFrameWnd, hWnd, WM_WRITESMS,pCreateData->pItem,
								PICK_NUMBER, pCreateData->id, TRUE,FALSE);
//        if(AB_IsTel(pFocus->dwMask) == TRUE)
//            APP_EditSMS(pCreateData->hFrameWnd,pFocus->pszData,NULL);
        break;

    case IDM_ABVIEW_WRITE_MMS:
//        nSel = SendMessage(hLst,LB_GETCURSEL,0,0);

//        if(nSel == LB_ERR)
//            break;

//        pFocus = (CONTACT_ITEMCHAIN*)SendMessage(hLst,LB_GETITEMDATA,nSel,0);
        		

		AB_SelectPhoneOrEmail(pCreateData->hFrameWnd,hWnd,WM_WRITEMMS,
								pCreateData->pItem,PICK_NUMBERANDEMAIL, pCreateData->id,TRUE,FALSE);
		
//        if(AB_IsTel(pFocus->dwMask) == TRUE || AB_IsEmail(pFocus->dwMask) == TRUE)
//            APP_EditMMS(pCreateData->hFrameWnd,hWnd,0,MMS_CALLEDIT_MOBIL,pFocus->pszData);
        break;

    case IDM_ABVIEW_WRITE_EMAIL:
        /*
        nSel = SendMessage(hLst,LB_GETCURSEL,0,0);
        
                if(nSel == LB_ERR)
                    break;
        
                pFocus = (CONTACT_ITEMCHAIN*)SendMessage(hLst,LB_GETITEMDATA,nSel,0);
                
                if(AB_IsEmail(pFocus->dwMask) == TRUE)
                {
                    SYSTEMTIME time;
                    
                    memset(&time,0,sizeof(SYSTEMTIME));
                    
                    GetLocalTime(&time);
                    
                    CreateMailEditWnd(pCreateData->hFrameWnd, pFocus->pszData, NULL, NULL, NULL, NULL, 
                        &time, -1, -1);
                }*/
        
		 AB_SelectPhoneOrEmail(pCreateData->hFrameWnd,hWnd,WM_WRITEEMAIL,
								pCreateData->pItem,PICK_EMAIL,pCreateData->id,TRUE,FALSE);
        break;

    case IDM_ABVIEW_SEND_SMS:
        {        
            char* pVcardData;
            int   nVcardLen;
           
            pVcardData = NULL;

            nVcardLen = 0;

            AB_GenVcard(pCreateData->pItem,&pVcardData,&nVcardLen);

            APP_EditSMSVcardVcal(pCreateData->hFrameWnd,pVcardData,nVcardLen);

            vCard_Clear(VCARD_OPT_WRITER, pVcardData);
        }
        break;

	case IDM_ABVIEW_SEND_MMS:
		{        
            char* pVcardData;
            int   nVcardLen;
            char szViewFileName [256];
			MEMORY_NAME * p;

            pVcardData = NULL;

            nVcardLen = 0;

            AB_GenVcard(pCreateData->pItem,&pVcardData,&nVcardLen);

			p = Memory_Find((PVOID)(pCreateData->id),AB_FIND_ID);

			strncpy(szViewFileName,AB_GetNameString(p),127);
			szViewFileName[127] = 0;
			if(szViewFileName[0] == 0)
				strcpy(szViewFileName, "Unnamed");

            strcat(szViewFileName,AB_VCARD_PORTFIX);

			if(!AB_CreateTmp(szViewFileName, pVcardData, nVcardLen))
			{
				strcpy(szViewFileName, "Cantact.vcf");
				AB_CreateTmp(szViewFileName, pVcardData, nVcardLen);
			}

            APP_EditMMS(pCreateData->hFrameWnd, NULL, 0, MMS_CALLEDIT_VCARD, szViewFileName);
			AB_ClearTmp(szViewFileName);

            vCard_Clear(VCARD_OPT_WRITER, pVcardData);
        }
        break;

    case IDM_ABVIEW_SEND_BULETOOTH:
		{
			char* pVcardData;
            int   nVcardLen;
			char  szViewFileName[256];
			char  szFileName [256];
			MEMORY_NAME * p;
			
            pVcardData = NULL;

            nVcardLen = 0;

            AB_GenVcard(pCreateData->pItem,&pVcardData,&nVcardLen);
			
			strcpy(szFileName, PATH_TEMP_FILE);
			
			AB_CreateTmp(szFileName, pVcardData, nVcardLen);
			
			p = Memory_Find((PVOID)(pCreateData->id),AB_FIND_ID);

			strncpy(szViewFileName,AB_GetNameString(p),127);
			szViewFileName[127] = 0;
			
			if(szViewFileName[0] == 0)
				strcpy(szViewFileName, "Unnamed.vcf");

			BtSendData(pCreateData->hFrameWnd,szFileName,szViewFileName, BTCARDCALENDER);
            
			AB_ClearTmp(szFileName);

            vCard_Clear(VCARD_OPT_WRITER, pVcardData);
		}
        //AB_PleaseToWait();
        break;

    case IDM_ABVIEW_SEND_EMAIL:
        {
			char* pVcardData;
            int   nVcardLen;
			char  szViewFileName[256];
			char  szFileName [256];
			MEMORY_NAME * p;
			
            pVcardData = NULL;

            nVcardLen = 0;

            AB_GenVcard(pCreateData->pItem,&pVcardData,&nVcardLen);
			
			strcpy(szFileName, PATH_TEMP_FILE);

			AB_CreateTmp(szFileName, pVcardData, nVcardLen);
			
			p = Memory_Find((PVOID)(pCreateData->id),AB_FIND_ID);

			strncpy(szViewFileName,AB_GetNameString(p),127);
			szViewFileName[127] = 0;
			
			if(szViewFileName[0] != 0)
				strcat(szViewFileName,AB_VCARD_PORTFIX);
			else
				strcpy(szViewFileName, "Unnamed.vcf");

			MAIL_CreateEditInterface(pCreateData->hFrameWnd, NULL, szFileName, szViewFileName, TRUE);
            
			AB_ClearTmp(szFileName);

            vCard_Clear(VCARD_OPT_WRITER, pVcardData);
		}
        break;

    case IDM_ABVIEW_DELETE:
        {
            char szPrompt[125];
            char* pszName;
            MEMORY_NAME * p;
            char szCaption[50];
            HDC hdc;
            
            p = Memory_Find((PVOID)(pCreateData->id),AB_FIND_ID);

            if(p == NULL)
                break;

            pszName = AB_GetNameString(p);
            
            szPrompt[0] = 0;
            
            hdc = GetDC(hWnd);
            
            GetExtentFittedText(hdc,pszName,-1,szCaption,50,TEXT_WIDTH,OMIT_SUFFIX,SUFFIX_REPEAT);
            
            ReleaseDC(hWnd,hdc);
            
            sprintf(szPrompt,"%s:\r\n%s?",szCaption,IDS_DELETE);
            
            PLXConfirmWinEx(pCreateData->hFrameWnd,hWnd,szPrompt, Notify_Request, pszName, IDS_YES, IDS_NO,WM_DELETECONTACT);                        
        }
        break;

    case IDM_ABVIEW_DUPLICATE:
        {
            CONTACT_ITEMCHAIN *pItem;
			int  nTelID;
            int  nGroup;
            DWORD dwoffset,id;
            MEMORY_NAME * p;
            
            p = Memory_Find((PVOID)(pCreateData->id),AB_FIND_ID);

            if(p == NULL)
                break;

            pItem = NULL;
            
            AB_ReadRecord(pCreateData->dwoffset,&pItem,&nTelID);

            nGroup = p->nGroup;

            dwoffset = -1;
            
            id = -1;

            AB_GetUseableDuplicateName(AB_GetNameString(p),&pItem);           

            AB_SaveRecord(&dwoffset,&id,&nGroup,TRUE,nTelID,pItem);
            
            AB_Insert2Table(dwoffset,id,nGroup,pItem);
                       
            Item_Erase(pItem);

        }
        break;

    case IDM_ABVIEW_SETPIC:
        AB_PleaseToWait();
        break;

    case IDM_ABVIEW_SETTONE:
        AB_PleaseToWait();
        break;

    case IDM_ABVIEW_COPYTOSIM:
        {
            CONTACT_ITEMCHAIN *ptemp;
            char* pNum,*pName;
            DWORD Mask;
            int  nNameLen;
            PPHONEBOOK pPB;   
            PHONEBOOK  Temp;
            MEMORY_NAME * p;
            PCONTACT_INITDATA pInit;  
            
            pInit = AB_GetSIMData();
            
            WaitWin(hWnd,TRUE,IDS_COPYING,IDS_CONTACTS,NULL,IDS_CANCEL,WM_CANCELCOPYING);
                        
            if(AB_IsUserCancel(hWnd,WM_CANCELCOPYING))
            {
                WaitWin(hWnd,FALSE,IDS_COPYING,IDS_CONTACTS,NULL,IDS_CANCEL,WM_CANCELCOPYING);
                
                return FALSE;
            }

            pPB = AB_FindUseableSIMPB();
            
            if(pPB == NULL)
            {
                WaitWin(hWnd,FALSE,IDS_COPYING,IDS_CONTACTS,NULL,IDS_CANCEL,WM_CANCELCOPYING);
                
                PLXTipsWin(NULL,NULL,0,IDS_MEMORYFULL,IDS_CONTACTS,Notify_Failure,IDS_OK,NULL,WAITTIMEOUT);
                
                return FALSE;
            }
            
            memset(&Temp,0,sizeof(PHONEBOOK));
            
            Temp.Index = AB_FindUseableIndex();
            
            pNum = NULL;
            
            ptemp = pCreateData->pItem;
            
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
                                    
            p = Memory_Find((PVOID)(pCreateData->id),AB_FIND_ID);
            
            if(p == NULL)
                break;

            pName = AB_GetNameString(p);
            
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
                
                break;
            }

            WaitWin(hWnd,FALSE,IDS_COPYING,IDS_CONTACTS,NULL,IDS_CANCEL,WM_CANCELCOPYING); 
            
            PLXTipsWin(NULL,NULL,0,IDS_COPIED,IDS_CONTACTS,Notify_Success,IDS_OK,NULL,WAITTIMEOUT);
            
        }
        break;

    default:
        break;
    }

    return TRUE;
}

static void ABViewContact_OnDealSelect(HWND hWnd,UINT wMsgCmd,int nTelID,DWORD id)
{   
    ABVIEWCREATEDATA* pCreateData;
	CONTACT_ITEMCHAIN* ptemp;
	int  nsubfix = 0;

    pCreateData = (ABVIEWCREATEDATA*)GetUserData(hWnd);

	if(nTelID == -1 || id == -1)
		return;
	
	ptemp = pCreateData->pItem;

    switch(wMsgCmd) 
    {
    case WM_WRITESMS:
              
		while(ptemp)
		{
			if(AB_IsTel(ptemp->dwMask))
			{
				if(ptemp->nID == nTelID)
				{
					APP_EditSMS(pCreateData->hFrameWnd,ptemp->pszData,NULL);
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
                    APP_EditMMS(pCreateData->hFrameWnd,hWnd,0,MMS_CALLEDIT_MOBIL,ptemp->pszData);
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
                    
                    CreateMailEditWnd(pCreateData->hFrameWnd, ptemp->pszData, NULL, NULL, NULL, NULL, 
                        &time, -1, -1);
                    
					break;
				}
			}
			ptemp = ptemp->pNext;
		}
        
        break;
    }    

}

/*********************************************************************\
* Function	ABViewContact_OnDestroy
* Purpose   WM_DESTROY message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void ABViewContact_OnDestroy(HWND hWnd)
{
    PABVIEWCREATEDATA pData;
    
    pData = GetUserData(hWnd);

	DestroyMenu(pData->hMenu);

    if(pData->hLeftArrow != NULL)
        DeleteObject(pData->hLeftArrow);

    if(pData->hRightArrow != NULL)
        DeleteObject(pData->hRightArrow);

    if(pData->hGif)
    {
        EndAnimatedGIF(pData->hGif);
        pData->hGif = NULL;
    }

    Item_Erase(pData->pItem);

    //AB_UnRegisterNotify(pData->handle);

//    if(hBmp)
//        DeleteObject(hBmp);

    UnregisterClass("ABViewContactWndClass", NULL);

    return;

}
/*********************************************************************\
* Function	ABViewContact_OnClose
* Purpose   WM_CLOSE message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void ABViewContact_OnClose(HWND hWnd)
{

    PABVIEWCREATEDATA pData;
    
    pData = GetUserData(hWnd);

	SendMessage(pData->hFrameWnd,PWM_CLOSEWINDOW,(WPARAM)hWnd,NULL);

    DestroyWindow (hWnd);
	
    return;

}

/*********************************************************************\
* Function	ABViewContact_OnRefresh
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
static void ABViewContact_OnDataChange(HWND hWnd,DWORD id,int nMode)
{    
    int nIndex,i;
    ABVIEWCREATEDATA *pCreateData;
    CONTACT_ITEMCHAIN *pTmp;
    HWND hLst;
    RECT rcClient;
    int nWidth = 0,nHeight = 0;
	int nTelID = 0;

    pCreateData = (ABVIEWCREATEDATA*)GetUserData(hWnd);

    if(pCreateData->id != pIndexName[id]->id)//need
        return;
    
    hLst = GetDlgItem(hWnd,IDC_ABVIEW_LIST);
    
    SendMessage(hLst,LB_RESETCONTENT,0,0);
    
    Item_Erase(pCreateData->pItem);

    pCreateData->pItem = NULL;

    AB_ReadRecord(((MEMORY_NAME*)pIndexName[id])->dwoffset,&(pCreateData->pItem),&nTelID);

    pCreateData->dwoffset = ((MEMORY_NAME*)pIndexName[id])->dwoffset;

    pTmp = pCreateData->pItem;

    while(pTmp)
    {
        if(pTmp->dwMask == AB_TYPEMASK_PIC)
        {
            pCreateData->pPic = pTmp->pszData;

            pCreateData->nImageType = AB_GetImageType(pTmp->pszData);
            
            GetImageDimensionFromFile(pCreateData->pPic,&(pCreateData->ImageSize));
            
            GetClientRect(hWnd,&rcClient);
            
            nWidth = rcClient.right - rcClient.left;
            
            nHeight = rcClient.bottom - rcClient.top;
            
            if(pCreateData->ImageSize.cx > nWidth || pCreateData->ImageSize.cy > nHeight)
                pCreateData->bStretch = TRUE;
            
            if(pCreateData->nImageType == IMG_GIF && pCreateData->bStretch == FALSE)
            {
                int x = 0,y = 0;
            
                x = (nWidth - pCreateData->ImageSize.cx)/2;
                
                y = (nHeight - pCreateData->ImageSize.cy)/2;
                
                if(pCreateData->hGif)
                {
                    EndAnimatedGIF(pCreateData->hGif);
                    pCreateData->hGif = NULL;
                }

                pCreateData->hGif = StartAnimatedGIFFromFile(hWnd,pCreateData->pPic,x,y,DM_NONE);

                SetPaintBkFunc(pCreateData->hGif, PaintBkFunc);
            }
        }
        else if(pTmp->dwMask == AB_TYPEMASK_TONE)
        {
            pCreateData->pTone = pTmp->pszData;
        }
        else if(pTmp->dwMask == AB_TYPEMASK_DATE)
        {
            char szTime[50];
            
            char szDate[50];

            nIndex = GetIndexByMask(pTmp->dwMask);
            
            i = SendMessage(hLst, LB_ADDSTRING, -1, (LPARAM)Contact_Item[nIndex].pszCpation);
            
            szTime[0] = 0;
            
            szDate[0] = 0;
            
            GetTimeDisplay(*((SYSTEMTIME*)(pTmp->pszData)),szTime,szDate);
            
            SendMessage(hLst, LB_SETAUXTEXT, MAKEWPARAM(nIndex, -1), (LPARAM)szDate);

            SendMessage(hLst, LB_SETITEMDATA, i, (LPARAM)pTmp);                        
        }
        else
        {    
            nIndex = GetIndexByMask(pTmp->dwMask);
            
            i = SendMessage(hLst, LB_ADDSTRING, -1, (LPARAM)Contact_Item[nIndex].pszCpation);
            
            SendMessage(hLst, LB_SETAUXTEXT, MAKEWPARAM(i, -1), (LPARAM)(pTmp->pszData));
            
            SendMessage(hLst, LB_SETITEMDATA, i, (LPARAM)pTmp);            
        }
        pTmp = pTmp->pNext;     
    }
    SendMessage(hLst, LB_SETCURSEL, 0, 0);
}
/*********************************************************************\
* Function	AB_GetImageType
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
static int AB_GetImageType(char* pszPathFileName)
{
    int f;
    unsigned char   strType[4];

    f = open(pszPathFileName,O_RDONLY);

    if(f == -1)
        return IMG_UNKNOWN;

    read(f, strType, 3);

    close(f);
    
    if ((strType[0] == 'B') && (strType[1] == 'M'))
        return IMG_BMP;
    else if ((strType[0] == 'G') && (strType[1] == 'I') && (strType[2] == 'F'))
        return IMG_GIF;
    else if ((strType[0] == 0xff) && (strType[1] == 0xd8))
        return IMG_JPEG;
    else if ((strType[0] == 0x00) && (strType[1] == 0x00))
        return IMG_WBMP;
    else
        return IMG_UNKNOWN;
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
    PABVIEWCREATEDATA pData;

    hParent = GetParent(hWnd);
    
    pData = GetUserData(hParent);
    
    switch(wMsgCmd)
    {         
    case WM_KEYDOWN:
        switch (LOWORD(wParam))
        { 
        case VK_RIGHT:
        case VK_LEFT:
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
* Function	   AB_CopyItem
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL AB_CopyItem(PCONTACT_ITEMCHAIN* ppHeader,CONTACT_ITEMCHAIN* pItemChain)
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

        pTemp = pTemp->pNext;
    }

    return TRUE;
}
/*********************************************************************\
* Function	   AB_IsTel
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL AB_IsTel(DWORD dwMask)
{
    switch(dwMask)
    {
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
        return TRUE;

    default:
        return FALSE;
    }
}
/*********************************************************************\
* Function	   AB_IsEmail
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL AB_IsEmail(DWORD dwMask) 
{
    switch(dwMask)
    {
    case AB_TYPEMASK_EMAIL:   
    case AB_TYPEMASK_EMAILWORK:
    case AB_TYPEMASK_EMAILHOME:
        return TRUE;

    default:
        return FALSE;
    }
} 
/*********************************************************************\
* Function	   AB_IsEmail
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
void PaintBkFunc(HGIFANIMATE hGIFAnimate, BOOL bEnd ,int x, int y, HDC hdcMem)
{
    int             width, height;
    RECT            rect;

    width = GetDeviceCaps(hdcMem, HORZRES);
    height = GetDeviceCaps(hdcMem, VERTRES);

    rect.bottom = height;
    rect.left = 0;
    rect.top = 0;
    rect.right = width;

    ClearRect(hdcMem, &rect, COLOR_TRANSBK);//????
}
/*********************************************************************\
* Function	   ABViewContact__OnDeleteContact
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void ABViewContact_OnDeleteContact(HWND hWnd,BOOL bDelete)
{
    int  nCount;
    char* pszName;
    ABVIEWCREATEDATA *pCreateData;

    if(bDelete == FALSE)
        return;

    pCreateData = (ABVIEWCREATEDATA*)GetUserData(hWnd);
    
    pszName = AB_GetNameString(Memory_Find((PVOID)(pCreateData->id),AB_FIND_ID));

    if(AB_DeleteRecord(pCreateData->dwoffset))
    {    
        PLXTipsWin(NULL,NULL,0,IDS_DELETED,pszName,Notify_Success,IDS_OK,NULL,WAITTIMEOUT);

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

        PostMessage(hWnd,WM_CLOSE,0,0);
    }
}
