/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : MMS
 *
 * Purpose  : implements the function of Edit MMS
 *            
\**************************************************************************/

#include "MmsEdit.h"
#include "stdlib.h"
#include "MmsInsert.h"

extern PSTR     imgfix[MAX_IMGTYPE_NUM];
extern PSTR     audiofix[MAX_AUDIOTYPE_NUM];
extern PSTR     txtfix[MAX_TXTTYPE_NUM];
	
static BOOL MMSEdit_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct);
static void MMSEdit_SetVScroll(HWND hWnd, BOOL bInit);
static void MMSEdit_OnPaint(HWND hWnd, LPCREATESTRUCT lpCreateStruct);
static void MMSEdit_OnInitmenu(HWND hWnd);
static void MMSEdit_OnCommand(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
static void MMSEdit_OnKeydown(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
static void MMSEdit_OnActivate(HWND hWnd, UINT state);
static void MMSEdit_OnSetFocus(HWND hWnd);
static void MMSEdit_OnMoveWindow(HWND hWnd,WPARAM wParam);
static void MMSEdit_OnClose(HWND hWnd);
static void MMSEdit_OnDestory(HWND hWnd);
void MMS_CaretProc(const RECT* rc);
static BOOL IsObjectCanInsert(PCSTR szFileName, int nLimit);
static void OnCloseNotify(HWND hwndPre, UINT msg, UINT nType, PCSTR szFileName, BOOL bSaveOrSend);
static void OnCallMMSInfo(HWND hWnd);
static BOOL OnEditInsert(HWND hWnd, WPARAM wParam, LPARAM lParam);
static BOOL IsMMSNull(const TotalMMS* ptmms);
static BOOL JudgeSupportType(int handle);
static BOOL JudgeSupportTypeFromName(PCSTR filename);
static BOOL Mms_OnRemoveImg(HWND hWnd);
static BOOL Mms_OnRemoveAudio(HWND hWnd);
static BOOL Mms_OnRemoveSlide(HWND hWnd);
static BOOL Mms_OnRemoveText(HWND hWnd);
static BOOL Mms_OnRemoveObj(HWND hWnd);
static void MMS_SetIndicator(HWND hWnd, int curSlide, int nAllSlide, int nMmsSize);
static void MMS_SetSoundInd(HWND hWnd, BOOL bShow);
static void Mms_OnImeSwitch(HWND hWnd, WPARAM wParam, LPARAM lParam);
static int MMS_RecipientGetTextLength(HWND hEdt);
static void MMS_RecipientGetText(HWND hEdt,PSTR pszText, int nMaxCount);
static void MmsEdit_OnAddReturn(HWND hWnd, WPARAM wParam, LPARAM lParam);
static void MMSEdit_OnSetLBtnText(HWND hWnd, int nID, BOOL bEmpty, LPSTR pszText);
static BOOL InsertMultiImage(HWND hWnd, void* pImage);

//void EditStopRing(BYTE bRingIsEnd);
/*********************************************************************\
* Function     MMS_EditRegisterClass
* Purpose      
* Params       
* Return           
* Remarks      
**********************************************************************/
static BOOL MMS_EditRegisterClass(void)
{
    WNDCLASS wc;
        
    wc.style         = 0;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = sizeof(MMS_EDITCREATEDATA);
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.lpszMenuName  = NULL;       
    wc.lpfnWndProc   = MMSWindowClass[5].wndProc;
    wc.lpszClassName = MMSWindowClass[5].szClassName;
    wc.hbrBackground = NULL;
    
    RegisterClass(&wc);

    return TRUE;
}
/******************************************************************** 
            call edit window       
*
*pFileName      edit mms name
*nType          call type
*InsertFileName the file name with path or mobile number
*
**********************************************************************/
BOOL CallMMSEditWnd(PSTR FileName,HWND hWndFrame, HWND hWndPre, UINT msg, UINT nType,
                    PSTR  InsertFileName, HINSTANCE hInst)
{   
    int             menupos = 0;
    MMS_EDITCREATEDATA CreateData;
    HMENU           hMenu  = NULL;
    HWND            hwndMMSEdit = NULL;
	RECT			rClient;

#ifdef MMS_DEBUG
    StartObjectDebug();
#endif

    if (MMS_IsFlashEnough() == FALSE)
        return FALSE;

//    if (MMSC_IsTotalFull())
//        return FALSE;

	if(nType != MMS_CALLEDIT_MULTIIMAGE)
	{
		if (!IsObjectCanInsert(InsertFileName, nType))
			return FALSE;
	}
    
    MMS_EditRegisterClass();

    memset(&CreateData, 0, sizeof(MMS_EDITCREATEDATA));

	CreateData.hInst = hInst;
    CreateData.EditMsgInfo.hWnd = hWndPre;
    CreateData.EditMsgInfo.msg = msg;
    CreateData.EditMsgInfo.nType = nType;

    CreateData.bDraft = FALSE;  
    CreateData.InsertFileName = InsertFileName;

	CreateData.hDCLeftIcon = NULL;
	CreateData.hBmpLeftIcon = NULL;

	CreateData.handle = NULL;
	CreateData.iconorbmp = 0;
	CreateData.szIconName[0] = 0;
    
    if (!InitMms(&CreateData, FileName, nType))
    {
        FreeAllMMSData(&CreateData);
        UnregisterClass(MMSWindowClass[5].szClassName, NULL);
        return FALSE;
    }
    
    hMenu = CreateMenu();

	CreateData.hMenu = hMenu;

	if (NULL == hWndFrame)
		hWndFrame = MuGetFrame();
	
    CreateData.hWndFrame = hWndFrame;
	GetClientRect(hWndFrame, &rClient);
	
    hwndMMSEdit = CreateWindow(
        MMSWindowClass[5].szClassName,
        MMS_EDIT_APP,
        WS_VISIBLE | WS_CHILD | WS_VSCROLL, 
        rClient.left, rClient.top, rClient.right-rClient.left, rClient.bottom-rClient.top,
        hWndFrame, 
        NULL,
        NULL, 
        (PVOID)&CreateData);

	SetWindowText(hWndFrame, MMS_EDIT_APP);

	PDASetMenu(hWndFrame, hMenu);
	SetFocus(hwndMMSEdit);
    ShowWindow(hwndMMSEdit, SW_SHOW);
    UpdateWindow(hwndMMSEdit);
    
    return TRUE;
}
/******************************************************************** 
            the window proc of edit tmms.mms       
**********************************************************************/
LRESULT MMSEditWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    LRESULT         lResult;

    switch(wMsgCmd)
    {
    case WM_CREATE: 
        lResult = MMSEdit_OnCreate(hWnd,(LPCREATESTRUCT)(lParam));
        break;
        
    case WM_PAINT:
        MMSEdit_OnPaint(hWnd,(LPCREATESTRUCT)(lParam));
        break;
        
    case WM_INITMENU:
        MMSEdit_OnInitmenu(hWnd);
        break;
        
    case REC_SIZE:
        MMSEdit_OnMoveWindow(hWnd,wParam);
        break;
		
	case WM_SETLBTNTEXT:
		MMSEdit_OnSetLBtnText(hWnd, (int)LOWORD(wParam), (BOOL)HIWORD(wParam),

            (LPSTR)lParam);

        lResult = 0;

		break;

    case WM_COMMAND:
	{
		PMMS_EDITCREATEDATA		pEditData;

		pEditData = GetUserData(hWnd);

		if(wParam == (WPARAM)GetMenu(pEditData->hWndFrame))
		{
			MMSEdit_OnInitmenu(hWnd);
			break;
		}

        MMSEdit_OnCommand(hWnd, wMsgCmd, wParam , lParam);
        break;
	}
        
    case WM_KEYDOWN:
        MMSEdit_OnKeydown(hWnd, wMsgCmd, wParam, lParam);
        break;
        
    case WM_ACTIVATE:
	case PWM_SHOWWINDOW:
        MMSEdit_OnActivate(hWnd, (UINT)LOWORD(wParam)) ;
        break;

	case WM_SETFOCUS:
		MMSEdit_OnSetFocus(hWnd);
		break;
		
    case MMS_EDIT_INSERT:
        if (HIWORD(wParam) == RTN_SLIDE)
            SendMessage(hWnd, WM_COMMAND, IDM_SLIDEADD, 0);
        else
            OnEditInsert(hWnd, wParam, lParam);
        break;

	case MMS_INSERT_TEMP:
		OnEditInsert(hWnd, MAKEWPARAM(0, RTN_QTEXT), lParam);
		break;

	case MMS_REMOVE_IMG:
		if (lParam == 1)
			Mms_OnRemoveImg(hWnd);
		break;
		
	case MMS_REMOVE_AUDIO:
		if (lParam == 1)
			Mms_OnRemoveAudio(hWnd);
		break;

	case MMS_REMOVE_TEXT:
		if (lParam == 1)
			Mms_OnRemoveText(hWnd);
		break;

	case MMS_REMOVE_OBJ:
		if (lParam == 1)
			Mms_OnRemoveObj(hWnd);
		break;

	case MMS_REMOVE_SLIDE:
		if (lParam == 1)
			Mms_OnRemoveSlide(hWnd);
		break;

	case WM_IMESWITCHED:
		Mms_OnImeSwitch(hWnd, wParam, lParam);
		break;

	case MMS_ADD_RECIPIENT:
		MmsEdit_OnAddReturn(hWnd, wParam, lParam);
		break;

	case MMS_EXIT_SAVE:
		{
			if (lParam == 1)
			{
				PostMessage(hWnd, WM_EXIT_OK, 0, 1);
			}
			else
				DestroyWindow(hWnd);
		}
		break;

    case WM_MMS_EXIT:
        {
            PMMS_EDITCREATEDATA pCreateData;
            
            pCreateData = GetUserData(hWnd);
            if (pCreateData->bModify)
            {
				PLXConfirmWinEx(pCreateData->hWndFrame, hWnd, STR_MMS_SELSAVE,
					Notify_Request, STR_MMS_MMS, STR_MMS_YES, 
					STR_MMS_NO, MMS_EXIT_SAVE);                
            }
            else
                DestroyWindow(hWnd);
        }
        break;

    case WM_EXIT_OK:
        if(LOWORD(lParam) == 1)
        {
            PMMS_EDITCREATEDATA pCreateData;

            pCreateData = GetUserData(hWnd);

            //edit draft box file
            SaveCurSlideTxt(hWnd);
			WaitWindowStateEx(NULL, TRUE, STR_MMS_SAVING, NULL, NULL, STR_MMS_CANCEL);
            if(OnSaveMMS(hWnd, pCreateData->mmsDraftFile) == TRUE)
            {
                pCreateData->bSave = TRUE;
                MsgWin(NULL, NULL, 0, STR_MMS_SAVEOK, STR_MMS_MMS,
					Notify_Success, STR_MMS_OK, NULL, MMS_WAITTIME);
				WaitWindowStateEx(NULL, FALSE, STR_MMS_SAVING, NULL, NULL, STR_MMS_CANCEL);
                DestroyWindow(hWnd);
            }
			WaitWindowStateEx(NULL, FALSE, STR_MMS_SAVING, NULL, NULL, STR_MMS_CANCEL);
        }
        else if(LOWORD(lParam) == 0)
            DestroyWindow(hWnd);
        break;

    case WM_CLOSE:
        MMSEdit_OnClose(hWnd);
        break;
        
    case WM_DESTROY:
        MMSEdit_OnDestory(hWnd);
        break;

    default:
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
    return lResult;
}
/*********************************************************************\
* Function  MMSEdit_OnCreate
* Purpose   WM_CREATE message handler of the main window
* Params
*           hWnd: Handle of the window
*           lpCreateStruct: Create Structure
* Return
*           TRUE: Success
*           FALSE: Fail
* Remarks
**********************************************************************/
static BOOL MMSEdit_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
{
    PMMS_EDITCREATEDATA pCreateData;
    int      cnx, cny, cnw, cnh;
    IMEEDIT  ie_Cont;
    int  x,y,nWidth,nHeight;
    RECT rc;
    SIZE Size;
    HDC  hdc;
    int  nY,nCharPerLine,nLine;
    HWND    hwndPhoneNum = NULL;
    HWND    hwndContent = NULL;
	HWND    hWndFrame = NULL;
    
    pCreateData = GetUserData(hWnd);
	
    memcpy(pCreateData,lpCreateStruct->lpCreateParams,sizeof(MMS_EDITCREATEDATA));

	hWndFrame = pCreateData->hWndFrame;	
    // right button
    SendMessage(hWndFrame, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(MENU_EDIT_EXIT, 0), 
        (LPARAM)(""));

    // left button
    SendMessage(hWndFrame, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDM_EDIT_LEFT, 1), 
        (LPARAM)(""));
    
    SendMessage(hWndFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)ML("Options"));
    
    pCreateData->bModify = FALSE;
    pCreateData->bSave = FALSE;
        
    GetClientRect(hWnd,&rc);
    
    x = 0;
    y = 0;
    nWidth = rc.right - rc.left;
    nHeight = (rc.bottom - rc.top) * 2 / 5;
    
    hwndPhoneNum = CreateWindow(
        WC_RECIPIENT,
        "",
        WS_VISIBLE | WS_CHILD | RS_EMAIL | WS_TABSTOP | CS_NOSYSCTRL,
        x,y,nWidth,nHeight,
        hWnd,
        (HMENU)ID_PHONENUM,
        NULL,
        NULL);
    
    if (NULL == hwndPhoneNum)
        return FALSE;

	SendMessage(hwndPhoneNum, REC_SETMAXREC, 20, 0);
    //SendMessage(hwndPhoneNum, EM_LIMITTEXT, SIZE_1 - 1, NULL);  
    SendMessage(hwndPhoneNum, EM_SETTITLE, 0, (LPARAM)MMS_RECV_STR);

    hdc = GetDC(hWnd);
    GetTextExtent(hdc, "W", 1, &Size);
    ReleaseDC(hWnd,hdc);
    
    y = nHeight;

    nY = Size.cy + MMS_EDIT_SPACE;
    nCharPerLine = (nWidth-2) / Size.cx;
    nLine = MAX_TXT_SIZE/nCharPerLine;
        
    nHeight = (nY + 1)*nLine + 2;

	// because edit`s high is IWORD type
	if (nHeight > 32767)
		nHeight = 32767;
	
    pCreateData->nHeight = nHeight;
    
    if (pCreateData->tmms.mms.slide[pCreateData->curSlide].pImage != NULL)
    {
        cnx = 0;
        cnw = SCREEN_WIDTH;        
        cny = y + pCreateData->tmms.mms.slide[pCreateData->curSlide].imgSize.cy;
//        cnh = nHeight - 
//            pCreateData->tmms.mms.slide[pCreateData->curSlide].imgSize.cy;

		cnh = nHeight;
        pCreateData->tmms.mms.slide[pCreateData->curSlide].imgPoint.x = 
			pCreateData->tmms.mms.slide[pCreateData->curSlide].imgSize.cx > SCREEN_WIDTH ?
			0 : (SCREEN_WIDTH - pCreateData->tmms.mms.slide[pCreateData->curSlide].imgSize.cx)/2;
        pCreateData->tmms.mms.slide[pCreateData->curSlide].imgPoint.y = y;
    }
    else
    {        
        cnx = 0;
        cny = y;
        cnw = SCREEN_WIDTH;
        cnh = nHeight;
    }
    
    memset(&ie_Cont, 0, sizeof(IMEEDIT));            
    ie_Cont.hwndNotify   = hWnd;
    ie_Cont.dwAttrib    = 0;            
    ie_Cont.pszImeName = NULL;
    ie_Cont.pszTitle = NULL;
    ie_Cont.uMsgSetText = MMS_IME_CONT;

//content
    hwndContent = CreateWindow(
        "IMEEDIT",
        "",
        WS_VISIBLE | WS_CHILD | WS_TABSTOP | ES_MULTILINE|CS_NOSYSCTRL|ES_AUTOVSCROLL,
        cnx, cny, cnw, cnh ,
        hWnd,
        (HMENU)ID_CONTENT,
        NULL,
        (PVOID)&ie_Cont);
    if (NULL == hwndContent)
        return FALSE;

    SendMessage(hwndContent, EM_LIMITTEXT, MAX_TXT_SIZE, NULL);
    
	if (pCreateData->tmms.mms.slide[pCreateData->curSlide].pText == NULL)
        SaveCurSlideTxt(hWnd);

    pCreateData->tmms.mms.slide[pCreateData->curSlide].dur = DEFAULT_DUR;

//    MMS_SetPageNum(hWnd, MMS_EDIT_APP, pCreateData->curSlide, 
//        pCreateData->tmms.mms.wspHead.nSlidnum);
    
    pCreateData->hFocus = hwndPhoneNum;
    
    if (pCreateData->InsertFileName != NULL)
    {
        if (pCreateData->EditMsgInfo.nType == MMS_CALLEDIT_MOBIL)
        {
            strcpy(pCreateData->tmms.mms.wspHead.to, pCreateData->InsertFileName);
        }
        else if(pCreateData->EditMsgInfo.nType == MMS_CALLEDIT_MULTIIMAGE)
		{
			InsertMultiImage(hWnd, pCreateData->InsertFileName);
		}
		else
            SendMessage(hWnd, MMS_EDIT_INSERT, pCreateData->EditMsgInfo.nType, 
            (LPARAM)pCreateData->InsertFileName);
    }


    SetMMSContent(hWnd);
	MMS_SetIndicator(hWnd, pCreateData->curSlide,pCreateData->tmms.mms.wspHead.nSlidnum,
		pCreateData->tmms.mms.wspHead.nMmsSize);

	MMSEdit_SetVScroll(hWnd, TRUE);
    return TRUE;
}
/*********************************************************************\
* Function     MMSEdit_InitVScroll
* Purpose      
* Params       
* Return           
* Remarks      
**********************************************************************/
static void MMSEdit_SetVScroll(HWND hWnd, BOOL bInit)
{
    SCROLLINFO      vsi;
    PMMS_EDITCREATEDATA pData;
    HWND            hEdt;
    int             nLine;
	SIZE			Size;
	HDC				hdc;
	int				nY = 0, nAllMax = 0;
    
    pData = GetUserData(hWnd);
    
    hEdt = GetDlgItem(hWnd,ID_CONTENT);

	hdc = GetDC(hWnd);
	GetTextExtent(hdc, "L", 1, &Size);
	ReleaseDC(hWnd,hdc);
	
    nLine = SendMessage(hEdt,EM_GETLINECOUNT,0,0);

	if (pData->curSlide == 0)
	   nLine ++;

	nAllMax = nLine + 
		pData->tmms.mms.slide[pData->curSlide].imgSize.cy/(Size.cy + MMS_EDIT_SPACE + 1);
	
	memset(&vsi, 0, sizeof(SCROLLINFO));
	if (bInit)
	{	
		vsi.cbSize = sizeof(vsi);
		vsi.fMask  = SIF_ALL ;
		vsi.nMin   = 0;
		vsi.nPage  = 5;
		vsi.nMax   = nAllMax;
		vsi.nPos   = 0;
    }
	else
	{
		vsi.fMask  = SIF_ALL;
		GetScrollInfo(hWnd, SB_VERT, &vsi);
		vsi.nMax = nAllMax - 1;
		vsi.fMask  = SIF_ALL;
	}
    SetScrollInfo(hWnd, SB_VERT, &vsi, TRUE);
    
    return;
}
/*********************************************************************\
* Function  Mms_OnImeSwitch
* Purpose   when ime switch,mms should save the icon
* Params    hWnd: Handle of the window
* Return    None
* Remarks   // wParam: Image type IMAGE_BITMAP or IMAGE_ICON
			// lParam: Path of icon file or handle to bitmap
**********************************************************************/
static void Mms_OnImeSwitch(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	PMMS_EDITCREATEDATA    pData;

	pData = GetUserData(hWnd);
	
	if (pData->tmms.mms.slide[pData->curSlide].pAudio)
	{
		pData->iconorbmp = wParam;
		if (pData->iconorbmp == IMAGE_BITMAP)
			pData->handle = lParam;
		else if (pData->iconorbmp == IMAGE_ICON)
		{
			if (strlen((char *)lParam) > 0)
				strncpy(pData->szIconName, (char *)lParam, 127);			
		}
	}
	else
	{
		PDADefWindowProc(hWnd, WM_IMESWITCHED, wParam, lParam);
	}
}
/*********************************************************************\
* Function  MMSEdit_OnPaint
* Purpose   WM_PAINT message handler of the main window
* Params    hWnd: Handle of the window
* Return    None
* Remarks
**********************************************************************/
static void MMSEdit_OnPaint(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
{
    PMMS_EDITCREATEDATA pCreateData;
    HDC      hdc;
    COLORREF OldClr;
    int      sWidth,sHeight;
    RECT     rect;
    
    pCreateData = GetUserData(hWnd);
  
    hdc = BeginPaint(hWnd, NULL);
    GetClientRect(hWnd, &rect);
    ClearRect(hdc, &rect, COLOR_TRANSBK);
    
    OldClr = SetBkColor(hdc, COLOR_TRANSBK);
        
    if (pCreateData->tmms.mms.slide[pCreateData->curSlide].pImage != NULL 
        && pCreateData->tmms.mms.slide[pCreateData->curSlide].pImage->Metalen > 0)
    {
        if (pCreateData->tmms.mms.slide[pCreateData->curSlide].imgSize.cy > 0)
        {
            sWidth = pCreateData->tmms.mms.slide[pCreateData->curSlide].imgSize.cx;
            sHeight = pCreateData->tmms.mms.slide[pCreateData->curSlide].imgSize.cy;
                        
            if (pCreateData->hdcPic[pCreateData->curSlide] == NULL)
            {
                pCreateData->hdcPic[pCreateData->curSlide] = CreateMemoryDC(sWidth, 
                    sHeight);
                rect.top = 0;
				rect.bottom = sHeight;
				rect.left = 0;
				rect.right = sWidth;
				ClearRect(pCreateData->hdcPic[pCreateData->curSlide], &rect, 
					COLOR_TRANSBK);
                DrawImageFromDataEx(pCreateData->hdcPic[pCreateData->curSlide], 
                    pCreateData->tmms.mms.slide[pCreateData->curSlide].pImage->Metadata, 
                    0, 0, sWidth, sHeight,  
                    pCreateData->tmms.mms.slide[pCreateData->curSlide].pImage->Metalen, SRCCOPY);
            }
            BitBlt(hdc, 
                pCreateData->tmms.mms.slide[pCreateData->curSlide].imgPoint.x,
                pCreateData->tmms.mms.slide[pCreateData->curSlide].imgPoint.y,
                sWidth, sHeight, 
                pCreateData->hdcPic[pCreateData->curSlide], 0, 0, SRCCOPY);
        }
    }
    SetBkColor(hdc, OldClr);
    EndPaint(hWnd,NULL);
}
/*********************************************************************\
* Function  MMS_IsHaveObj
* Purpose   judge objects
* Params    mmsmetanode
* Return    true:have False:no
**********************************************************************/
static BOOL MMS_IsHaveObj(mmsMetaNode *pmeta)
{
    mmsMetaNode *p;

    p = pmeta;
    if (NULL == p)
        return FALSE;
    
    while (p)
    {
        if (p->MetaType/10 == META_TEXT)
        {
            if (p->Metalen > 0)
                return TRUE;
        }
        else
            return TRUE;
        p = p->pnext;
    }

    return FALSE;
}
/*********************************************************************\
* Function  MMSEdit_OnInitmenu
* Purpose   WM_INITMENU message handler of the main window
* Params    hWnd: Handle of the window
* Return    None
* Remarks
**********************************************************************/
static void MMSEdit_OnInitmenu(HWND hWnd)
{
    PMMS_EDITCREATEDATA pCreateData;
    int nMenuItem;
    HMENU hMenu = NULL, hSubMenuInsert = NULL, hSubMenuRemove = NULL;
    HWND hwndPhoneNum = NULL;
    HWND    hwndContent = NULL;
	HWND    hWndFrame = NULL;
	int     receivelen = 0;
    
    pCreateData = GetUserData(hWnd);

	hWndFrame = pCreateData->hWndFrame;
	if (hWndFrame == NULL)
		return;
	
    hMenu = pCreateData->hMenu;
	hwndPhoneNum = GetDlgItem(hWnd, ID_PHONENUM);
    hwndContent = GetDlgItem(hWnd, ID_CONTENT);
    
    if(hMenu)
    {
        nMenuItem = GetMenuItemCount(hMenu);
        while(nMenuItem > 0)
        {
            nMenuItem--;
            DeleteMenu(hMenu,nMenuItem,MF_BYPOSITION);
        }
    }
	
    hSubMenuInsert = CreateMenu();
    hSubMenuRemove = CreateMenu();

	receivelen = MMS_RecipientGetTextLength(hwndPhoneNum);
    if(receivelen
		/*SendMessage(hwndPhoneNum,GHP_GETRECLEN,NULL,NULL)*/ > 0)
    {
        AppendMenu(hMenu, MF_ENABLED, IDM_SEND, IDS_SEND);
        AppendMenu(hMenu, MF_ENABLED, IDM_SENDOPTIONS, IDS_SENDOPT);
    }

	if(receivelen > 0 || pCreateData->tmms.mms.wspHead.nMmsSize > 0)
        AppendMenu(hMenu, MF_ENABLED, IDM_SAVEMMS, IDS_SAVEMMS);

    if(pCreateData->hFocus == hwndPhoneNum)
        AppendMenu(hMenu, MF_ENABLED, IDM_ADDRECI, IDS_ADDRECI);
    else if(pCreateData->hFocus == hwndContent)
    {
		if (pCreateData->EditMsgInfo.nType != MMS_CALLEDIT_TRAN)
		{
			AppendMenu(hSubMenuInsert, MF_ENABLED, IDM_INSERT_IMG, IDS_INSERT_IMG);
			AppendMenu(hSubMenuInsert, MF_ENABLED, IDM_INSERT_AMR, IDS_INSERT_AMR);
			AppendMenu(hSubMenuInsert, MF_ENABLED, IDM_INSERT_SLIDE, IDS_SLIDE);
			AppendMenu(hSubMenuInsert, MF_ENABLED, IDM_INSERT_TEPLATE, IDS_INSERT_TXT);
			
			AppendMenu(hMenu,MF_ENABLED|MF_POPUP, (DWORD)hSubMenuInsert,IDS_INSERT);
		}
    }

	if (pCreateData->EditMsgInfo.nType != MMS_CALLEDIT_TRAN)
	{
		if ((pCreateData->tmms.mms.slide[pCreateData->curSlide].pImage != NULL) ||
			(pCreateData->tmms.mms.slide[pCreateData->curSlide].pAudio != NULL) ||
			pCreateData->tmms.mms.wspHead.nSlidnum > 1)
		{
			if(pCreateData->tmms.mms.slide[pCreateData->curSlide].pImage != NULL)
				AppendMenu(hSubMenuRemove, MF_ENABLED, IDM_REMOVE_IMG, IDS_REMOVE_IMG);
			
			if(pCreateData->tmms.mms.slide[pCreateData->curSlide].pAudio != NULL)
				AppendMenu(hSubMenuRemove, MF_ENABLED, IDM_REMOVE_AMR, IDS_REMOVE_AMR);
			
			if(pCreateData->tmms.mms.wspHead.nSlidnum > 1)
				AppendMenu(hSubMenuRemove, MF_ENABLED, IDM_REMOVE_SLIDE, IDS_REMOVE_SLIDE);
			
			AppendMenu(hMenu,MF_ENABLED|MF_POPUP, (DWORD)hSubMenuRemove,IDS_REMOVE);    
		}
    }

    AppendMenu(hMenu, MF_ENABLED, IDM_PREVIEW, IDS_PREVIEW);

	if (MMS_IsHaveObj(pCreateData->tmms.pAttatch)
		&& pCreateData->EditMsgInfo.nType != MMS_CALLEDIT_TRAN)
        AppendMenu(hMenu, MF_ENABLED, IDM_OBJ, IDS_OBJ);

    AppendMenu(hMenu, MF_ENABLED, IDM_MMSINFO, IDS_INFO);

    if(pCreateData->bPlayMusic)
    {
        MMS_StopPlay();
        pCreateData->bPlayMusic = FALSE;
    }
}

// 
static BOOL Mms_OnRemoveImg(HWND hWnd)
{
    HWND hwndContent = NULL;
	PMMS_EDITCREATEDATA pCreateData;
	int nSlide;

    pCreateData = GetUserData(hWnd);
    hwndContent = GetDlgItem(hWnd, ID_CONTENT);
	nSlide = pCreateData->delSlide;
    pCreateData->bModify = TRUE;
    
    DeleteAttatch(pCreateData,pCreateData->tmms.mms.slide[nSlide].pImage);
	MMS_SetIndicator(hWnd, pCreateData->curSlide, 
						pCreateData->tmms.mms.wspHead.nSlidnum, 
						pCreateData->tmms.mms.wspHead.nMmsSize);

    
    pCreateData->tmms.mms.slide[nSlide].pImage = NULL;
    
    pCreateData->tmms.mms.slide[nSlide].imgSize.cx = 0;
    
    pCreateData->tmms.mms.slide[nSlide].imgSize.cy = 0;
    
    if (pCreateData->hdcPic[nSlide])
    {
        DeleteDC(pCreateData->hdcPic[nSlide]);
        pCreateData->hdcPic[nSlide] = NULL;
    }
    
	MMSEdit_SetVScroll(hWnd, FALSE);

    MoveWindow(hwndContent, 0, 
        pCreateData->tmms.mms.slide[nSlide].imgPoint.y, 
        SCREEN_WIDTH, pCreateData->nHeight, FALSE);

	MsgWin(NULL, NULL, 0, ML("Removed"), STR_MMS_MMS,
					Notify_Success, STR_MMS_OK, NULL, MMS_WAITTIME);
    return TRUE;
}

static BOOL Mms_OnRemoveAudio(HWND hWnd)
{        
	PMMS_EDITCREATEDATA pCreateData;
	int nSlide;

	pCreateData = GetUserData(hWnd);
	nSlide = pCreateData->delSlide;

    pCreateData->bModify = TRUE;     
    
    DeleteAttatch(pCreateData, pCreateData->tmms.mms.slide[nSlide].pAudio);
    
    pCreateData->tmms.mms.slide[nSlide].pAudio = NULL;
	MMS_SetIndicator(hWnd, pCreateData->curSlide, 
						pCreateData->tmms.mms.wspHead.nSlidnum, 
						pCreateData->tmms.mms.wspHead.nMmsSize);

	MMS_SetSoundInd(hWnd, FALSE);
	MsgWin(NULL, NULL, 0, ML("Removed"), STR_MMS_MMS,
					Notify_Success, STR_MMS_OK, NULL, MMS_WAITTIME);
    return TRUE;
}

// for vcard and vcalendar
static BOOL Mms_OnRemoveObj(HWND hWnd)
{        
	PMMS_EDITCREATEDATA pCreateData;

	pCreateData = GetUserData(hWnd);

    pCreateData->bModify = TRUE;     
    
    DeleteAttatch(pCreateData, pCreateData->pDel);
    
	MMS_SetIndicator(hWnd, pCreateData->curSlide, 
						pCreateData->tmms.mms.wspHead.nSlidnum, 
						pCreateData->tmms.mms.wspHead.nMmsSize);

	MsgWin(NULL, NULL, 0, ML("Removed"), STR_MMS_MMS,
					Notify_Success, STR_MMS_OK, NULL, MMS_WAITTIME);
    return TRUE;
}

static BOOL Mms_OnRemoveText(HWND hWnd)
{
	HWND		hwndContent;
	PMMS_EDITCREATEDATA    pCreateData;
	int         nSlide;
	mmsMetaNode			   *pNode;

	pCreateData = GetUserData(hWnd);
	nSlide = pCreateData->delSlide;
	pNode = pCreateData->tmms.mms.slide[nSlide].pText;

	pCreateData->bModify = TRUE;

	hwndContent = GetDlgItem(hWnd, ID_CONTENT);
	if (pNode->Metadata)
	{
		MMS_free(pNode->Metadata);
		pNode->Metadata = NULL;
	}
	pNode->Metalen = 0;
	if (pNode->inSlide == pCreateData->curSlide)
		SetWindowText(hwndContent, "");

	MsgWin(NULL, NULL, 0, ML("Removed"), STR_MMS_MMS,
					Notify_Success, STR_MMS_OK, NULL, MMS_WAITTIME);
	return TRUE;
}

static BOOL Mms_OnRemoveSlide(HWND hWnd)
{
	PMMS_EDITCREATEDATA pCreateData;
	HWND                hwndContent;
	HWND                hwndPhoneNum;
	int                 i;

	pCreateData = GetUserData(hWnd);
	hwndContent = GetDlgItem(hWnd, ID_CONTENT);
	hwndPhoneNum = GetDlgItem(hWnd, ID_PHONENUM);

	pCreateData->bModify = TRUE;
	
	if (pCreateData->tmms.mms.slide[pCreateData->curSlide].pImage)
	{                    
		DeleteAttatch(pCreateData, 
			pCreateData->tmms.mms.slide[pCreateData->curSlide].pImage);
		MMS_SetIndicator(hWnd, pCreateData->curSlide, 
						pCreateData->tmms.mms.wspHead.nSlidnum, 
						pCreateData->tmms.mms.wspHead.nMmsSize);

		pCreateData->tmms.mms.slide[pCreateData->curSlide].pImage = NULL;
		
		DeleteDC(pCreateData->hdcPic[pCreateData->curSlide]);
		
		pCreateData->hdcPic[pCreateData->curSlide] = NULL;
	}
	
	SetWindowText(hwndContent, "");
	
	if (pCreateData->tmms.mms.slide[pCreateData->curSlide].pText)
	{
		DeleteAttatch(pCreateData, 
			pCreateData->tmms.mms.slide[pCreateData->curSlide].pText);
		pCreateData->tmms.mms.slide[pCreateData->curSlide].pText = NULL;		
	}
	
	if (pCreateData->tmms.mms.slide[pCreateData->curSlide].pAudio)
	{
		DeleteAttatch(pCreateData, 
			pCreateData->tmms.mms.slide[pCreateData->curSlide].pAudio);
		pCreateData->tmms.mms.slide[pCreateData->curSlide].pAudio = NULL;
	}
	
	memset(&pCreateData->tmms.mms.slide[pCreateData->curSlide], 0, sizeof(SLIDE));
	
	if (pCreateData->curSlide >= 0)
	{
		if (pCreateData->curSlide != pCreateData->tmms.mms.wspHead.nSlidnum - 1)
		{
			for (i = pCreateData->curSlide; 
			i < pCreateData->tmms.mms.wspHead.nSlidnum - 1; i ++)
				MoveBetweenSlides(hWnd,i, i + 1);
			
			memset(&pCreateData->tmms.mms.slide[pCreateData->tmms.mms.wspHead.nSlidnum - 1],
				0, sizeof(SLIDE));
		}
		else
			memset(&pCreateData->tmms.mms.slide[pCreateData->curSlide], 0, sizeof(SLIDE));
		
		pCreateData->tmms.mms.wspHead.nSlidnum --;
		
		if(pCreateData->curSlide > 0)
			pCreateData->curSlide --;
	}
	
	if (pCreateData->curSlide == 0)
		ShowWindow(hwndPhoneNum, SW_SHOW);
	else
		ShowWindow(hwndPhoneNum, SW_HIDE);
	
	if (pCreateData->tmms.mms.slide[pCreateData->curSlide].pText != NULL)
	{
		SetWindowText(hwndContent, 
			pCreateData->tmms.mms.slide[pCreateData->curSlide].pText->Metadata);
	}
//	MMS_SetPageNum(hWnd, MMS_EDIT_APP, pCreateData->curSlide, 
//		pCreateData->tmms.mms.wspHead.nSlidnum);
	MMS_SetIndicator(hWnd, pCreateData->curSlide,pCreateData->tmms.mms.wspHead.nSlidnum,
		pCreateData->tmms.mms.wspHead.nMmsSize);

	if (pCreateData->tmms.mms.slide[pCreateData->curSlide].pAudio)
		MMS_SetSoundInd(hWnd, TRUE);
	else
		MMS_SetSoundInd(hWnd, FALSE);

	if (pCreateData->tmms.mms.slide[pCreateData->curSlide].pImage != NULL)
	{
		MoveWindow(hwndContent, 0, 
			(pCreateData->tmms.mms.slide[pCreateData->curSlide].imgSize.cy + 
			pCreateData->tmms.mms.slide[pCreateData->curSlide].imgPoint.y + 1),
			SCREEN_WIDTH, 
			pCreateData->nHeight ,
			FALSE);
	}
	else
	{
		MoveWindow(hwndContent, 0, 
			pCreateData->tmms.mms.slide[pCreateData->curSlide].imgPoint.y, 
			SCREEN_WIDTH, pCreateData->nHeight, FALSE);
	}
	InvalidateRect(hWnd, NULL, TRUE);
	MMSEdit_SetVScroll(hWnd, TRUE);
	MsgWin(NULL, NULL, 0, ML("Removed"), STR_MMS_MMS,
					Notify_Success, STR_MMS_OK, NULL, MMS_WAITTIME);
	return TRUE;
}

static void MMSEdit_OnAddreci(HWND hWndFrame, HWND hWnd)
{
	HWND            hWndPhone;
	PABNAMEOREMAIL  pArray;
	RECIPIENTLISTBUF Recipient;	
    PRECIPIENTLISTNODE pTemp = NULL;	
    int i = 0, k=0;
	
	hWndPhone = GetDlgItem(hWnd, ID_PHONENUM);
	pArray = MMS_malloc(sizeof(ABNAMEOREMAIL) * MAX_ADDR_NUM);
	memset(pArray, 0, sizeof(ABNAMEOREMAIL) * MAX_ADDR_NUM);

	memset(&Recipient,0,sizeof(RECIPIENTLISTBUF));

    SendMessage(hWndPhone,GHP_GETREC,0,(LPARAM)&Recipient);

    pTemp = Recipient.pDataHead;

    for(i = 0; i < Recipient.nDataNum ; i++)
    {
		if(pTemp->bExistInAB)
		{
			strcpy(pArray[k].szName, pTemp->szShowName);
			strcpy(pArray[k].szTelOrEmail, pTemp->szPhoneNum);
			if(MMS_IsEmail(pArray[k].szTelOrEmail))
				pArray[k].nType = AB_EMAIL;
			else
				pArray[k].nType = AB_NUMBER;
			
			k++;
		}
        pTemp = pTemp->pNext;
    }
	// call phonebook, need to ...
	APP_GetMultiPhoneOrEmail(hWndFrame, hWnd, MMS_ADD_RECIPIENT, 
		PICK_NUMBERANDEMAIL, pArray,k,20 - (Recipient.nDataNum - k));
}
/*
add recipient:	GHP_ADDREC: 
			wparam:  0 or 1;
			lparam: according to wapram; if wapram == 0, lparam is a pointer to 
					PABNAMEOREMAIL
			        if wparam==1, lparam is a string of phonenumer or email
*/
static void MmsEdit_OnAddReturn(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	int		num = 0, i = 0;
	PABNAMEOREMAIL  pArray = NULL;
	HWND            hWndPhone = 0;

	pArray = (PABNAMEOREMAIL)lParam;
	
	if (LOWORD(wParam) == FALSE)
	{
		if (pArray)
			MMS_free(pArray);
		return;
	}

	hWndPhone = GetDlgItem(hWnd, ID_PHONENUM);
	SendMessage(hWndPhone, REC_CLEAR, 0, 0);

	num = HIWORD(wParam);
	
	for (i = 0; i < num; i++)
	{
		SendMessage(hWndPhone, GHP_ADDREC, 0, (LPARAM)&pArray[i]);
	}
	if (pArray)
		MMS_free(pArray);
}
/*********************************************************************\
* Function  MMSEdit_OnCommand
* Purpose   WM_COMMAND message handler of the main window
* Params
*           hWnd:   Handle of the window
*           id:     Id of command message
*           hwndCtl: Handle of the window which sended this message
*           codeNotify:Notify code of the message
* Return    None
* Remarks
**********************************************************************/
static void MMSEdit_OnCommand(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    PMMS_EDITCREATEDATA pCreateData;
    WORD mID;
    HWND hwndPhoneNum = NULL;
    UINT codeNotify = HIWORD(wParam);
    HWND hwndContent = NULL;

    pCreateData = GetUserData(hWnd);

    mID = LOWORD(wParam);

    hwndPhoneNum = GetDlgItem(hWnd, ID_PHONENUM);
    hwndContent = GetDlgItem(hWnd, ID_CONTENT);
    
    if (mID == ID_CONTENT)
    {
        int oldlen;
        int overlen;
        char *tmpTxt = NULL;
		static HWND hPreFocus;
		HWND hParent;		
		int nRecipientLen = 0, nContentLen = 0;
        
        if(codeNotify == EN_CHANGE)
        {
            pCreateData->bModify = TRUE;
            
            oldlen = pCreateData->tmms.mms.slide[pCreateData->curSlide].pText->Metalen;
            
            pCreateData->tmms.mms.slide[pCreateData->curSlide].pText->Metalen 
                = GetWindowTextLength(hwndContent);
            
            if (pCreateData->tmms.mms.slide[pCreateData->curSlide].pText->Metalen > MAX_TXT_SIZE )
                MsgWin(NULL, NULL, 0, STR_MMS_OVERSIZE, 
				STR_MMS_MMS, Notify_Alert, STR_MMS_OK,NULL,MMS_WAITTIME);
            else
            {
                pCreateData->tmms.mms.wspHead.nMmsSize -= oldlen*TXT_UTF8_MUL;
                
                pCreateData->tmms.mms.wspHead.nMmsSize += 
                    pCreateData->tmms.mms.slide[pCreateData->curSlide].pText->Metalen*TXT_UTF8_MUL;
                
                if (pCreateData->tmms.mms.wspHead.nMmsSize > MAX_MMS_LEN)
                {
                    overlen = pCreateData->tmms.mms.wspHead.nMmsSize - MAX_MMS_LEN;
                    
                    pCreateData->tmms.mms.wspHead.nMmsSize -= 
                        pCreateData->tmms.mms.slide[pCreateData->curSlide].pText->Metalen*TXT_UTF8_MUL;
                    
                    pCreateData->tmms.mms.slide[pCreateData->curSlide].pText->Metalen -= (overlen/TXT_UTF8_MUL + 1);
                    
                    tmpTxt = MMS_malloc(pCreateData->tmms.mms.slide[pCreateData->curSlide].pText->Metalen + 1);
                    
                    pCreateData->tmms.mms.slide[pCreateData->curSlide].pText->Metalen = 
                        GetWindowText(hwndContent, tmpTxt, pCreateData->tmms.mms.slide[pCreateData->curSlide].pText->Metalen);
                    
                    tmpTxt[pCreateData->tmms.mms.slide[pCreateData->curSlide].pText->Metalen] = '\0';
                    
                    pCreateData->tmms.mms.wspHead.nMmsSize += 
                        pCreateData->tmms.mms.slide[pCreateData->curSlide].pText->Metalen*TXT_UTF8_MUL;
                    
                    SetWindowText(hwndContent, tmpTxt);
                    
                    MMS_free(tmpTxt);
                    tmpTxt = NULL;
                    
                    MsgWin(NULL, NULL, 0, STR_MMS_OVERSIZE, 
						STR_MMS_MMS,Notify_Alert, STR_MMS_OK,NULL,
                        MMS_WAITTIME);
                }
            }
        
			MMS_SetIndicator(hWnd, pCreateData->curSlide, 
				pCreateData->tmms.mms.wspHead.nSlidnum, 
				pCreateData->tmms.mms.wspHead.nMmsSize);
			
			hwndPhoneNum = GetDlgItem(hWnd, ID_PHONENUM);
//			if(GetFocus() == hwndContent)
//			{
//				nRecipientLen = GetWindowTextLength(hwndPhoneNum);
//				nContentLen = pCreateData->tmms.mms.wspHead.nMmsSize;
//				
//				if(nRecipientLen == 0 && nContentLen == 0)
//					SendMessage(pCreateData->hWndFrame, PWM_SETBUTTONTEXT,
//					1, (LPARAM)"");
//				
//				if(nRecipientLen == 0 && nContentLen != 0)
//					SendMessage(pCreateData->hWndFrame, PWM_SETBUTTONTEXT,
//					1, (LPARAM)STR_MMS_SAVE);
//				
//				if(nRecipientLen != 0)
//					SendMessage(pCreateData->hWndFrame, PWM_SETBUTTONTEXT,
//					1, (LPARAM)STR_MMS_SEND);                    
//			}			
        }
        else if(codeNotify == EN_SETFOCUS)
        {
            pCreateData->hFocus = hwndContent;
			
			SetCaretProc(MMS_CaretProc);
			
			nRecipientLen = GetWindowTextLength(hwndPhoneNum);
			nContentLen = pCreateData->tmms.mms.wspHead.nMmsSize;
			
			if(nRecipientLen == 0 && nContentLen == 0)
				SendMessage(pCreateData->hWndFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
			
			if(nRecipientLen == 0 && nContentLen != 0)
				SendMessage(pCreateData->hWndFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)STR_MMS_SAVE);
			
			if(nRecipientLen != 0)
				SendMessage(pCreateData->hWndFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)STR_MMS_SEND);

			hParent = GetParent(hPreFocus);
			if (hParent == hWnd)
				SendMessage(hwndContent, EM_SETSEL, 0, 0);
        }
		else if (codeNotify == EN_KILLFOCUS)
		{
			hPreFocus = GetFocus();
			SetCaretProc(NULL);
		}
        else
            PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        
    }
    if (mID == ID_PHONENUM)
    {
        if(codeNotify ==  EN_CHANGE )
        {
            pCreateData->bModify = TRUE;
            
//            SendMessage(hwndPhoneNum, ,(WPARAM)SIZE_1,
//                (LPARAM)pCreateData->tmms.mms.wspHead.to);
//            
//            MMS_FormatStringNoRtn(pCreateData->tmms.mms.wspHead.to);
        }
        else if( codeNotify == EN_SETFOCUS)
        {
            pCreateData->hFocus = hwndPhoneNum;
			SetCaretProc(MMS_CaretProc);
            SendMessage(pCreateData->hWndFrame, PWM_SETBUTTONTEXT, 
				1, (LPARAM)STR_MMS_ADD);
        }
		else if (codeNotify == EN_KILLFOCUS)
			SetCaretProc(NULL);
        else 
            PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        
    }
    
    switch(mID)
    {
    case IDM_SEND:
        if(pCreateData->bPlayMusic)
        {
            MMS_StopPlay();
            pCreateData->bPlayMusic = FALSE;
        }
        
        if (GetMEIsSim() != 1)
        {
			int handle;
			
			SaveCurSlideTxt(hWnd);

			if (pCreateData->EditMsgInfo.nType != MMS_CALLEDIT_TRAN 
				&& pCreateData->EditMsgInfo.nType != MMS_CALLEDIT_REPLY
				&& pCreateData->EditMsgInfo.nType != MMS_CALLEDIT_REPLYALL
				&& pCreateData->EditMsgInfo.nType != NOTI_CALLEDIT_REPLY
				&& pCreateData->EditMsgInfo.nType != MMS_CALLEDIT_VCARD
				&& pCreateData->EditMsgInfo.nType != MMS_CALLEDIT_VCAL
				&& pCreateData->tmms.mms.slide[0].pText->Metadata)
			{
				int sublen;
				
				sublen = min(pCreateData->tmms.mms.slide[0].pText->Metalen,
					30);
				
				strncpy(pCreateData->tmms.mms.wspHead.subject, 
					pCreateData->tmms.mms.slide[0].pText->Metadata, sublen);
				pCreateData->tmms.mms.wspHead.subject[sublen] = 0;
			}

			if(!OnSendMms(hWnd, FALSE))
				goto error_handle;
			
			if (PackMmsBody(pCreateData->mmsDraftFile, pCreateData->tmms) != RETURN_OK)
			{
				MMSMSGWIN("MMS:Packmmsbody failure\r\n");
				goto error_handle;
			}

			pCreateData->bSave = TRUE;

			handle = GetHandleByName(pCreateData->mmsDraftFile);

			MMS_ModifyResend(pCreateData->mmsDraftFile, 1);

			if(handle)
				ModifyMsgNode(handle, pCreateData->mmsDraftFile, MMFT_UNSEND);

			if(!MMS_ModifyType(pCreateData->mmsDraftFile, TYPE_UNSEND, MU_OUTBOX))
			{
				MMSMSGWIN("MMS:mms_modifytype failure\r\n");
				goto error_handle;
			}	
			
			if(handle == 0)
				AllocMsgHandle(pCreateData->mmsDraftFile, MMFT_UNSEND, FALSE);

error_handle:
            MsgWin(NULL, NULL, 0, STR_MMS_SEND_NOSIM, STR_MMS_MMS,
				Notify_Alert, STR_MMS_OK, NULL, MMS_WAITTIME);
			
			DestroyWindow(hWnd);

            break;
        }
        
        SaveCurSlideTxt(hWnd);
		
		if (pCreateData->EditMsgInfo.nType != MMS_CALLEDIT_TRAN 
			&& pCreateData->EditMsgInfo.nType != MMS_CALLEDIT_REPLY
			&& pCreateData->EditMsgInfo.nType != MMS_CALLEDIT_REPLYALL
			&& pCreateData->EditMsgInfo.nType != NOTI_CALLEDIT_REPLY
			&& pCreateData->EditMsgInfo.nType != MMS_CALLEDIT_VCARD
			&& pCreateData->EditMsgInfo.nType != MMS_CALLEDIT_VCAL
			&& pCreateData->tmms.mms.slide[0].pText->Metadata)
		{
			int sublen;

			sublen = min(pCreateData->tmms.mms.slide[0].pText->Metalen,
					30);

			strncpy(pCreateData->tmms.mms.wspHead.subject, 
					pCreateData->tmms.mms.slide[0].pText->Metadata, sublen);
				pCreateData->tmms.mms.wspHead.subject[sublen] = 0;
		}

//        if (OnSendMms(hWnd, TRUE))
//            DestroyWindow(hWnd);
//        else
//            pCreateData->hFocus = hwndPhoneNum;

		OnSendMms(hWnd, TRUE);
		DestroyWindow(hWnd);

        break;
        
    case IDM_SENDOPTIONS:
        MMS_CreateSettingWnd(pCreateData->hWndFrame, hWnd);
        break;

    case IDM_SAVEMMS:
        pCreateData->bModify = FALSE;
        
        pCreateData->bDraft = TRUE;
        
        if (!SaveCurSlideTxt(hWnd))
            PostMessage(hWnd, WM_MMS_EXIT, 0, 0);
        
        if (OnSaveMMS(hWnd, pCreateData->mmsDraftFile))
        {   
            pCreateData->bSave = TRUE;
            
            //DestroyWindow(hWnd);
            MsgWin(NULL, NULL, 0, STR_MMS_SAVEOK, STR_MMS_MMS, 
				Notify_Success, STR_MMS_OK, NULL, MMS_WAITTIME);
        }		
        break;

    case IDM_ADDRECI:
		MMSEdit_OnAddreci(pCreateData->hWndFrame, hWnd);
        break;
        
    case IDM_INSERT_IMG:
        {
            //DWORD dwStyle = PREBROW_MASKINSERT|PREBROW_MASKEDIT|PREBROW_MASKDEL;
            APP_PreviewPicture(pCreateData->hWndFrame, hWnd, MMS_EDIT_INSERT, "", "None");
        }   
        break;
    case IDM_INSERT_SLIDE:
        {
            SendMessage(hWnd, MMS_EDIT_INSERT, MAKEWPARAM(0, RTN_SLIDE), NULL);
        }
        break;

    case IDM_INSERT_AMR:
        {
            //DWORD dwStyle = PREBROW_MASKINSERT|PREBROW_MASKEDIT|PREBROW_MASKDEL;
            //APP_PreviewRing(pCreateData->hWndFrame, hWnd, MMS_EDIT_INSERT, "MMS");
			PreviewSoundEx(pCreateData->hWndFrame, hWnd, MMS_EDIT_INSERT, NULL, FALSE);
        }
        break;

    case IDM_INSERT_TEPLATE:
		// need to modify
		APP_PreviewPhrase(pCreateData->hWndFrame, hWnd, MMS_INSERT_TEMP, NULL);
        break;
        
    case IDM_REMOVE_IMG:
		pCreateData->delSlide = pCreateData->curSlide;
		PLXConfirmWinEx(pCreateData->hWndFrame, hWnd, STR_MMS_REMOVECONFIRM, Notify_Request,
			STR_MMS_MMS, STR_MMS_YES, STR_MMS_NO, MMS_REMOVE_IMG);
        break;
        
    case IDM_REMOVE_AMR:
		pCreateData->delSlide = pCreateData->curSlide;
		PLXConfirmWinEx(pCreateData->hWndFrame, hWnd, STR_MMS_REMOVECONFIRM, Notify_Request,
			STR_MMS_MMS, STR_MMS_YES, STR_MMS_NO, MMS_REMOVE_AUDIO);
        break;

    case IDM_REMOVE_SLIDE:
		PLXConfirmWinEx(pCreateData->hWndFrame, hWnd, STR_MMS_REMOVECONFIRM, Notify_Request,
			STR_MMS_MMS, STR_MMS_YES, STR_MMS_NO, MMS_REMOVE_SLIDE);
		break;

    case IDM_PREVIEW:       
        {
            if (!SaveCurSlideTxt(hWnd))
                PostMessage(hWnd, WM_MMS_EXIT, 0, 0);
            
            GetLocalTime(&pCreateData->tmms.mms.wspHead.date);
            //SendMessage(hwndPhoneNum,GHP_GETREC,(WPARAM)SIZE_1,
            //    (LPARAM)pCreateData->tmms.mms.wspHead.to);
			MMS_RecipientGetText(hwndPhoneNum,pCreateData->tmms.mms.wspHead.to, MAX_ADDR_NUM);
            CallDisplayWnd(&pCreateData->tmms, pCreateData->hWndFrame, hWnd, 
				MMS_EDIT_DISPLAY, EDIT_VIEW, 0, FALSE, FALSE);
        }
        break;

    case IDM_OBJ:
		SaveCurSlideTxt(hWnd);
        CallMMSObjList(pCreateData->hWndFrame, hWnd, pCreateData->tmms.pAttatch, EDIT_CALLOBJ);
		printf("CallMMSObjList return!!!!!!!!!!\r\n");
        break;
        
    case IDM_MMSINFO:   //mms information
        {
			int sublen = 0;

            //SendMessage(hwndPhoneNum,GHP_GETREC,(WPARAM)SIZE_1,
            //    (LPARAM)pCreateData->tmms.mms.wspHead.to);

			MMS_RecipientGetText(hwndPhoneNum,pCreateData->tmms.mms.wspHead.to, MAX_ADDR_NUM);
			SaveCurSlideTxt(hWnd);
			if (pCreateData->EditMsgInfo.nType != MMS_CALLEDIT_TRAN 
				&& pCreateData->EditMsgInfo.nType != MMS_CALLEDIT_REPLY
				&& pCreateData->EditMsgInfo.nType != MMS_CALLEDIT_REPLYALL
				&& pCreateData->EditMsgInfo.nType != NOTI_CALLEDIT_REPLY
				&& pCreateData->EditMsgInfo.nType != MMS_CALLEDIT_VCARD
				&& pCreateData->EditMsgInfo.nType != MMS_CALLEDIT_VCAL
				&& pCreateData->tmms.mms.slide[0].pText->Metadata)
			{
				sublen = min(pCreateData->tmms.mms.slide[0].pText->Metalen,
					30);
				strncpy(pCreateData->tmms.mms.wspHead.subject, 
					pCreateData->tmms.mms.slide[0].pText->Metadata, sublen);
				pCreateData->tmms.mms.wspHead.subject[sublen] = 0;
			}

            OnCallMMSInfo(hWnd);
			//pCreateData->tmms.mms.wspHead.subject[0] = 0;
        }
        break;

    case IDM_SLIDEADD:
        {            
            pCreateData->bModify = TRUE;
            
            if (!SaveCurSlideTxt(hWnd))
                PostMessage(hWnd, WM_MMS_EXIT, 0, 0);
            
            if (pCreateData->tmms.mms.wspHead.nSlidnum == MMS_MAXNUM)
			{
				MsgWin(NULL, NULL, 0, "Too many slides", STR_MMS_MMS,
					Notify_Failure, STR_MMS_OK, NULL, MMS_WAITTIME);
                return;
			}
            
            pCreateData->tmms.mms.wspHead.nSlidnum ++;
            
            pCreateData->curSlide = pCreateData->tmms.mms.wspHead.nSlidnum -1;
            if (pCreateData->curSlide == 0)
                ShowWindow(hwndPhoneNum, SW_SHOW);
            else
                ShowWindow(hwndPhoneNum, SW_HIDE);

            SetWindowText(hwndContent, "");
            MMS_SetSoundInd(hWnd, FALSE);
//            MMS_SetPageNum(hWnd,MMS_EDIT_APP, pCreateData->curSlide, 
//                pCreateData->tmms.mms.wspHead.nSlidnum);
            MMS_SetIndicator(hWnd, pCreateData->curSlide,pCreateData->tmms.mms.wspHead.nSlidnum,
				pCreateData->tmms.mms.wspHead.nMmsSize);

            pCreateData->tmms.mms.slide[pCreateData->curSlide].dur = DEFAULT_DUR;
            
            SaveCurSlideTxt(hWnd);
                        
            if (pCreateData->tmms.mms.slide[pCreateData->curSlide].pImage != NULL)
            {
                MoveWindow(hwndContent, 0, 
                    (pCreateData->tmms.mms.slide[pCreateData->curSlide].imgSize.cy +
                    pCreateData->tmms.mms.slide[pCreateData->curSlide].imgPoint.y), 
                    SCREEN_WIDTH, 
                    pCreateData->nHeight,
                    FALSE);
            }
            else
            {
                MoveWindow(hwndContent, 0, 
                    pCreateData->tmms.mms.slide[pCreateData->curSlide].imgPoint.y, 
                    SCREEN_WIDTH,
                    pCreateData->nHeight, 
                    FALSE);
            }
			MMSEdit_SetVScroll(hWnd, TRUE);
            InvalidateRect(hWnd, NULL, TRUE);
            break;
        }

    case IDM_EDIT_LEFT:
        {
            char ButtonText[8];
            
            SendMessage(pCreateData->hWndFrame, PWM_GETBUTTONTEXT, 1, (LPARAM)ButtonText);

            if (0 == strcmp(ButtonText, STR_MMS_SAVE))
                SendMessage(hWnd, WM_COMMAND,IDM_SAVEMMS, 0);
            else if (0 == strcmp(ButtonText, STR_MMS_SEND))
                SendMessage(hWnd, WM_COMMAND, IDM_SEND, 0);
            else if (0 == strcmp(ButtonText, STR_MMS_ADD))
                SendMessage(hWnd, WM_COMMAND, IDM_ADDRECI, 0);
        }
        break;

    case MENU_EDIT_EXIT:    // right key
        {
            char ButtonText[8];
            
            SendMessage(hWnd, PWM_GETBUTTONTEXT, 0, (LPARAM)ButtonText);
            
            if(strcmp(ButtonText, STR_MMS_CLEAR) == 0)
            {
                SendMessage(GetFocus(), WM_KEYDOWN, VK_BACK, 0);
            }
            else
            {
                SendMessage(hWnd,WM_MMS_EXIT,NULL,NULL);
            }
        }
        break;  
    default:
        PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;

}
}
static void MMSEdit_OnKeydown(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    DWORD  dwCaretPos  = 0;
    HWND   hwndPhoneNum = NULL;
    HWND   hwndContent = NULL;
	HDC    hdc;
	SIZE   sz;
	int    nY;
	SCROLLINFO vsi;
    PMMS_EDITCREATEDATA pCreateData;
    
    pCreateData = GetUserData(hWnd);

    hwndPhoneNum = GetDlgItem(hWnd, ID_PHONENUM);
    hwndContent = GetDlgItem(hWnd, ID_CONTENT);

    switch (LOWORD(wParam))
    {
    case VK_F1:
        //if(SendMessage(hwndPhoneNum,GHP_GETRECLEN,NULL,NULL) !=0)
		if (MMS_RecipientGetTextLength(hwndPhoneNum) != 0)
            SendMessage(hWnd, WM_COMMAND, IDM_SEND, 0);
        else
            SendMessage(hWnd, WM_COMMAND ,IDM_ADDRECI, 0);
        break;

	case VK_RETURN:
		{
            char ButtonText[8];
            
            SendMessage(pCreateData->hWndFrame, PWM_GETBUTTONTEXT, 1, (LPARAM)ButtonText);
			
            if (0 == strcmp(ButtonText, STR_MMS_SAVE))
                SendMessage(hWnd, WM_COMMAND,IDM_SAVEMMS, 0);
            else if (0 == strcmp(ButtonText, STR_MMS_SEND))
                SendMessage(hWnd, WM_COMMAND, IDM_SEND, 0);
            else if (0 == strcmp(ButtonText, STR_MMS_ADD))
                SendMessage(hWnd, WM_COMMAND, IDM_ADDRECI, 0);
        }
		break;

    case VK_UP:
        {       
			HWND hWndFocus;

			hWndFocus = GetFocus();

			if (hWndFocus == hwndPhoneNum)
			{
				if (pCreateData->EditMsgInfo.nType != MMS_CALLEDIT_TRAN
					&& pCreateData->EditMsgInfo.nType != MMS_CALLEDIT_VCARD
					&& pCreateData->EditMsgInfo.nType != MMS_CALLEDIT_VCAL)
					SetFocus(hwndContent);
			}
            else if(hWndFocus == hwndContent)
            {
				if (pCreateData->tmms.mms.slide[pCreateData->curSlide].imgPoint.y < 0)
				{
					hdc = GetDC(hWnd);
					GetTextExtent(hdc, "L", 1, &sz);
					ReleaseDC(hWnd,hdc);
					
					nY = sz.cy + MMS_EDIT_SPACE;

					memset(&vsi, 0, sizeof(SCROLLINFO));
					vsi.fMask  = SIF_POS;
					GetScrollInfo(hWnd, SB_VERT, &vsi);
					
					vsi.nPos --;
					vsi.fMask  = SIF_POS;
					SetScrollInfo(hWnd, SB_VERT, &vsi, TRUE);

					ScrollWindow(hWnd,0,nY,NULL,NULL);
					pCreateData->tmms.mms.slide[pCreateData->curSlide].imgPoint.y += nY;
					UpdateWindow(hWnd);
				}
				// go to previous slide
                else if (pCreateData->curSlide > 0)
                {                    
                    if (!SaveCurSlideTxt(hWnd))
                        PostMessage(hWnd, WM_MMS_EXIT, 0, 0);
                    
                    pCreateData->curSlide --;
                    
                    if (pCreateData->tmms.mms.slide[pCreateData->curSlide].pText
						&& pCreateData->tmms.mms.slide[pCreateData->curSlide].pText->Metadata)
                    {
                        SetWindowText(hwndContent, pCreateData->tmms.mms.slide[pCreateData->curSlide].pText->Metadata);
                    }
                    else
                    {
                        SaveCurSlideTxt(hWnd);
                        SetWindowText(hwndContent, "");
                    }
                    
//                    MMS_SetPageNum(hWnd, MMS_EDIT_APP, pCreateData->curSlide, pCreateData->tmms.mms.wspHead.nSlidnum);
					MMS_SetIndicator(hWnd, pCreateData->curSlide,
						pCreateData->tmms.mms.wspHead.nSlidnum,
						pCreateData->tmms.mms.wspHead.nMmsSize);
					
					if (pCreateData->tmms.mms.slide[pCreateData->curSlide].pAudio)
						MMS_SetSoundInd(hWnd, TRUE);
					else
						MMS_SetSoundInd(hWnd, FALSE);

                    if (pCreateData->curSlide == 0)
						ShowWindow(hwndPhoneNum, SW_SHOW);
					else
						ShowWindow(hwndPhoneNum, SW_HIDE);

                    SetFocus(hwndContent);
                    //SendMessage(hwndContent, EM_SETSEL, -1, -1);
					if (pCreateData->tmms.mms.slide[pCreateData->curSlide].pImage != NULL)
					{
						MoveWindow(hwndContent, 0, 
							(pCreateData->tmms.mms.slide[pCreateData->curSlide].imgSize.cy + 
							pCreateData->tmms.mms.slide[pCreateData->curSlide].imgPoint.y + 1),
							SCREEN_WIDTH, 
							pCreateData->nHeight ,
							FALSE);
					}
					else
					{
						MoveWindow(hwndContent, 0, 
							pCreateData->tmms.mms.slide[pCreateData->curSlide].imgPoint.y, 
							SCREEN_WIDTH, pCreateData->nHeight, FALSE);
					}
					SendMessage(hwndContent, EM_SETSEL, -1, -1);
					MMSEdit_SetVScroll(hWnd, TRUE);
					memset(&vsi, 0, sizeof(SCROLLINFO));
					vsi.fMask  = SIF_ALL;
					GetScrollInfo(hWnd, SB_VERT, &vsi);
					
					vsi.nPos   = vsi.nMax - 1;
					vsi.fMask  = SIF_POS;
					SetScrollInfo(hWnd, SB_VERT, &vsi, TRUE);

                    InvalidateRect(hWnd, NULL, TRUE);
                }
                else
                {
					RECT rectClient, rectReceive;

					SetFocus(hwndPhoneNum);

					GetWindowRectEx(hWnd,&rectClient,W_CLIENT,XY_SCREEN);
					GetWindowRectEx(hwndPhoneNum,&rectReceive,W_WINDOW,XY_SCREEN);
					if(rectReceive.top < rectClient.top)
					{
						ScrollWindow(hWnd,0,rectClient.top - rectReceive.top,NULL,NULL);
						pCreateData->tmms.mms.slide[pCreateData->curSlide].imgPoint.y += rectClient.top - rectReceive.top;
					}
					memset(&vsi, 0, sizeof(SCROLLINFO));
					vsi.fMask  = SIF_POS;
					GetScrollInfo(hWnd, SB_VERT, &vsi);
					
					vsi.nPos = 0;
					vsi.fMask  = SIF_POS;
					SetScrollInfo(hWnd, SB_VERT, &vsi, TRUE);
                }
            }// end getfocus
            else
                PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        }
        
        break;
    case VK_DOWN:
        {
			HWND	hWndFocus;

			hdc = GetDC(hWnd);
			GetTextExtent(hdc, "L", 1, &sz);
			ReleaseDC(hWnd,hdc);

			hWndFocus = GetFocus();
			if (hWndFocus == hwndPhoneNum)
			{
				if (pCreateData->EditMsgInfo.nType != MMS_CALLEDIT_TRAN
					&& pCreateData->EditMsgInfo.nType != MMS_CALLEDIT_VCARD
					&& pCreateData->EditMsgInfo.nType != MMS_CALLEDIT_VCAL)
					SetFocus(hwndContent);
			}
            else if(hWndFocus == hwndContent)
            {
				if (pCreateData->tmms.mms.slide[pCreateData->curSlide].imgPoint.y + 
					pCreateData->tmms.mms.slide[pCreateData->curSlide].imgSize.cy >= SCREEN_HEIGHT - sz.cy)
				{
					nY = sz.cy + MMS_EDIT_SPACE;
					
					memset(&vsi, 0, sizeof(SCROLLINFO));
					vsi.fMask  = SIF_POS;
					GetScrollInfo(hWnd, SB_VERT, &vsi);
					
					vsi.nPos ++;
					vsi.fMask  = SIF_POS;
					SetScrollInfo(hWnd, SB_VERT, &vsi, TRUE);

					ScrollWindow(hWnd,0,-nY,NULL,NULL);
					pCreateData->tmms.mms.slide[pCreateData->curSlide].imgPoint.y -= nY;
					UpdateWindow(hWnd);
				}
				// to next slide
                else if (pCreateData->curSlide < pCreateData->tmms.mms.wspHead.nSlidnum -1)
                {
                    if (!SaveCurSlideTxt(hWnd))
                        PostMessage(hWnd, WM_MMS_EXIT, 0, 0);
                    
                    pCreateData->curSlide ++;
                    
                    if (pCreateData->tmms.mms.slide[pCreateData->curSlide].pText != NULL)
                    {
                        SetWindowText(hwndContent, pCreateData->tmms.mms.slide[pCreateData->curSlide].pText->Metadata);
                    }
                    else
                    {
                        SaveCurSlideTxt(hWnd);
                        SetWindowText(hwndContent, "");
                    }
                    
//                    MMS_SetPageNum(hWnd, MMS_EDIT_APP, pCreateData->curSlide, 
//                        pCreateData->tmms.mms.wspHead.nSlidnum);
					MMS_SetIndicator(hWnd, pCreateData->curSlide,
						pCreateData->tmms.mms.wspHead.nSlidnum,
						pCreateData->tmms.mms.wspHead.nMmsSize);

					if (pCreateData->tmms.mms.slide[pCreateData->curSlide].pAudio)
						MMS_SetSoundInd(hWnd, TRUE);
					else
						MMS_SetSoundInd(hWnd, FALSE);

                    if (pCreateData->curSlide == 0)
						ShowWindow(hwndPhoneNum, SW_SHOW);
					else
						ShowWindow(hwndPhoneNum, SW_HIDE);

					if (pCreateData->tmms.mms.slide[pCreateData->curSlide].pImage != NULL)
					{
						MoveWindow(hwndContent, 0, 
							(pCreateData->tmms.mms.slide[pCreateData->curSlide].imgSize.cy + 
							pCreateData->tmms.mms.slide[pCreateData->curSlide].imgPoint.y + 1),
							SCREEN_WIDTH, 
							pCreateData->nHeight ,
							FALSE);
					}
					else
					{
						MoveWindow(hwndContent, 0, 
							pCreateData->tmms.mms.slide[pCreateData->curSlide].imgPoint.y, 
							SCREEN_WIDTH, pCreateData->nHeight, FALSE);
					}
                    //SetFocus(hwndContent);
					SetFocus(hWnd);
					MMSEdit_SetVScroll(hWnd, TRUE);
                    InvalidateRect(hWnd, NULL, TRUE);
                }
				else if (pCreateData->curSlide == 0 && 
					pCreateData->tmms.mms.wspHead.nSlidnum == 1)
				{
					hdc = GetDC(hWnd);
					GetTextExtent(hdc, "L", 1, &sz);
					ReleaseDC(hWnd,hdc);
					nY = sz.cy + MMS_EDIT_SPACE;
					
					

					SetFocus(hwndPhoneNum);
					memset(&vsi, 0, sizeof(SCROLLINFO));
					vsi.fMask  = SIF_POS;
					GetScrollInfo(hWnd, SB_VERT, &vsi);
					
					nY = nY * vsi.nPos;

					vsi.nPos = 0;
					vsi.fMask  = SIF_POS;
					SetScrollInfo(hWnd, SB_VERT, &vsi, TRUE);

					ScrollWindow(hWnd,0,nY,NULL,NULL);
					pCreateData->tmms.mms.slide[pCreateData->curSlide].imgPoint.y += nY;
					
                    //SendMessage(hwndPhoneNum, EM_SETSEL, -1, -1);
				}
            }// end getfocus hwndcontent
			//else if (GetFocus() == hWnd)//??
			//{
			//}
            else
                PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        }
        break;
        
    case VK_F10:
        SendMessage(hWnd,WM_MMS_EXIT,NULL,NULL);
        break;

	case VK_F5:
		PDADefWindowProc(pCreateData->hWndFrame, wMsgCmd, wParam, lParam);
		break;

    default:
        PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
    }
}
/*********************************************************************\
* Function  MMSEdit_OnActivate
* Purpose   WM_ACTIVATE message handler of the main window
* Params
* Return    None
* Remarks
**********************************************************************/
static void MMSEdit_OnActivate(HWND hWnd, UINT state)
{
    PMMS_EDITCREATEDATA pCreateData;
	HWND                hWndFrame = NULL, hWndContent = NULL;
	int                 nRecipientLen = 0, nContentLen = 0;

    pCreateData = GetUserData(hWnd);
	hWndFrame = pCreateData->hWndFrame;

//    if(state == WA_ACTIVE)
//    {
//        pCreateData->OldCaretProc = SetCaretProc(MMS_CaretProc);
//        SetFocus(pCreateData->hFocus);
//      }
//    else if(state == WA_INACTIVE)
//    {
//        SetCaretProc(pCreateData->OldCaretProc);
//        pCreateData->hFocus = GetFocus();
//    }
	
	SetFocus(pCreateData->hFocus);
	SetWindowText(hWndFrame, MMS_EDIT_APP);
	hWndContent = GetDlgItem(hWnd, ID_CONTENT);
	// right button
	//SendMessage(hWndFrame, PWM_SETBUTTONTEXT, 0, (LPARAM)ML("Cancel"));

    // left button
    if(pCreateData->hFocus == GetDlgItem(hWnd, ID_CONTENT))
	{
		nRecipientLen = MMS_RecipientGetTextLength(GetDlgItem(hWnd, ID_PHONENUM));
		nContentLen = pCreateData->tmms.mms.wspHead.nMmsSize;

		if(nRecipientLen == 0 && nContentLen == 0)
			SendMessage(hWndFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)"");

		if(nRecipientLen == 0 && nContentLen != 0)
			SendMessage(hWndFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)STR_MMS_SAVE);

		if(nRecipientLen != 0)
			SendMessage(hWndFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)STR_MMS_SEND);
	}
	else if (pCreateData->hFocus == GetDlgItem(hWnd, ID_PHONENUM))
	{
		SendMessage(hWndFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)STR_MMS_ADD);
	}
    
    SendMessage(hWndFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)ML("Options"));

	PDASetMenu(hWndFrame, pCreateData->hMenu);

	if (pCreateData->tmms.mms.slide[pCreateData->curSlide].pAudio)
		MMS_SetSoundInd(hWnd, TRUE);
	else
		MMS_SetSoundInd(hWnd, FALSE);
	
	MMS_SetIndicator(hWnd, pCreateData->curSlide, 
		pCreateData->tmms.mms.wspHead.nSlidnum, 
		pCreateData->tmms.mms.wspHead.nMmsSize);

    return;
}
/*********************************************************************\
* Function  MMSEdit_OnSetFocus
* Purpose   WM_SETFOCUS message handler of the main window
* Params
* Return    None
* Remarks
**********************************************************************/
static void MMSEdit_OnSetFocus(HWND hWnd)
{
    PMMS_EDITCREATEDATA pCreateData;

    pCreateData = GetUserData(hWnd);
    
	SetFocus(pCreateData->hFocus);
    
    return;
}
/*********************************************************************\
* Function  MMSEdit_OnMoveWindow
* Purpose   
* Params    
* Return    
* Remarks
**********************************************************************/
static void MMSEdit_OnMoveWindow(HWND hWnd,WPARAM wParam)
{
    RECT    rcEdt;
    int     y;
    HWND    hwndContent = NULL;
    PMMS_EDITCREATEDATA  pCreateData;
 
    hwndContent = GetDlgItem(hWnd, ID_CONTENT);
    GetWindowRect(hwndContent,&rcEdt);

    pCreateData = GetUserData(hWnd);
    
    y = wParam + 1;

    pCreateData->tmms.mms.slide[pCreateData->curSlide].imgPoint.y = y;

    if (pCreateData->tmms.mms.slide[pCreateData->curSlide].pImage != NULL)
    {        
        pCreateData->tmms.mms.slide[pCreateData->curSlide].imgPoint.x = 
			pCreateData->tmms.mms.slide[pCreateData->curSlide].imgSize.cx > SCREEN_WIDTH ?
			0 : (SCREEN_WIDTH - pCreateData->tmms.mms.slide[pCreateData->curSlide].imgSize.cx)/2;

        y = y + pCreateData->tmms.mms.slide[pCreateData->curSlide].imgSize.cy;        
    }
    MoveWindow(hwndContent,0,y,rcEdt.right-rcEdt.left,rcEdt.bottom-rcEdt.top,TRUE);
    InvalidateRect(hWnd, NULL, TRUE);
}
/*********************************************************************\
* Function  MMSEdit_OnClose
* Purpose   
* Params    
* Return    
* Remarks
**********************************************************************/
static void MMSEdit_OnClose(HWND hWnd)
{
    PMMS_EDITCREATEDATA pCreateData;
    
    pCreateData = GetUserData(hWnd);
    //this is not used
    SaveCurSlideTxt(hWnd);
    
    OnSaveMMS(hWnd, pCreateData->mmsDraftFile);        
    pCreateData->bSave = TRUE;		
    DestroyWindow(hWnd);
}
/*********************************************************************\
* Function  MMSEdit_OnDestory
* Purpose   
* Params    
* Return    
* Remarks
**********************************************************************/
static void MMSEdit_OnDestory(HWND hWnd)
{
    int i, nFolder;
    PMMS_EDITCREATEDATA pCreateData;
	HWND hWndFrame;
    
    pCreateData = GetUserData(hWnd);
	hWndFrame = pCreateData->hWndFrame;
	DestroyMenu(PDAGetMenu(hWndFrame));
    
    if (!pCreateData->bSave && pCreateData->EditMsgInfo.nType != MMS_CALLEDIT_OPEN) 
    {
        MMSC_ModifyTotal(COUNT_DEC, 0);
        nFolder = MMS_GetFileFolder(pCreateData->mmsDraftFile);
        MMSC_ModifyMsgCount(nFolder, COUNT_DEC, 0);
    }
    
    if(pCreateData->bPlayMusic)
    {
        MMS_StopPlay();
        pCreateData->bPlayMusic = FALSE;
    }
    
    for (i = 0; i < pCreateData->tmms.mms.wspHead.nSlidnum; i++)
    {
        if (pCreateData->hdcPic[i])
        {
            DeleteDC(pCreateData->hdcPic[i]);
            pCreateData->hdcPic[i] = NULL;
        }
    }
    
	DeleteObject(pCreateData->hBmpLeftIcon);
	pCreateData->hBmpLeftIcon = NULL;
	DeleteDC(pCreateData->hDCLeftIcon);
	pCreateData->hDCLeftIcon = NULL;

	SendMessage(pCreateData->hWndFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, RIGHTICON), 
				(LPARAM)"");
	SendMessage(pCreateData->hWndFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, LEFTICON), 
				(LPARAM)"");

    OnCloseNotify(pCreateData->EditMsgInfo.hWnd, pCreateData->EditMsgInfo.msg, 
        pCreateData->EditMsgInfo.nType, pCreateData->mmsDraftFile, pCreateData->bSave);
    
    FreeAllMMSData(pCreateData);

    SendMessage(hWndFrame,PWM_CLOSEWINDOW,(WPARAM)hWnd,0);

	if (pCreateData->hInst)
		DlmNotify((WPARAM)PES_STCQUIT, (LPARAM)(pCreateData->hInst));

    UnregisterClass(MMSWindowClass[5].szClassName, NULL);
    
#ifdef MMS_DEBUG
    EndObjectDebug();
#endif
}
/******************************************************************** 
* Function     OnCloseNotify
* Purpose      notify the call window
* Params       hwndpre:the handle of the window
               msg: the return msg
               nType：call type
               szFileName:file name
* Return        void
**********************************************************************/
static void OnCloseNotify(HWND hwndPre, UINT msg, UINT nType, PCSTR szFileName, BOOL bSaveOrSend)
{
    MU_MsgNode     msgnode;
    char           *pSubject;
    int            msghandle;
    WSPHead        wspHead;
    int            hFileMms;
	int            nMmfttype, nCurFolder;
    
    // draft
    if (nType == MMS_CALLEDIT_OPEN)
    {        
        chdir(MMS_FILEPATH);

        hFileMms = open(szFileName, O_RDONLY, 0);
        if (hFileMms == -1)
            return;
        lseek(hFileMms, MMS_HEADER_SIZE, SEEK_SET);
        read(hFileMms, &wspHead, sizeof(WSPHead));
        close(hFileMms);
        
        pSubject = wspHead.subject;
        
        msghandle = msg;

		nMmfttype = MMS_GetFileMmft(szFileName);
        nCurFolder = MMS_GetFileFolder(szFileName);
        FillMsgNode(&msgnode, MU_MSG_MMS, nMmfttype, wspHead.date, 
            msghandle, pSubject, wspHead.to, MU_STORE_IN_FLASH);

		if ((nCurFolder == MU_DRAFT))//(nMmfttype == MMFT_DRAFT)
			SendMessage(hwndPre, PWM_MSG_MU_MODIFIED, MAKEWPARAM(MU_ERR_SUCC, MU_MDU_MMS), 
				(LPARAM)&msgnode);
		else
			SendMessage(hwndPre, PWM_MSG_MU_DELETE, MAKEWPARAM(MU_ERR_SUCC, MU_MDU_MMS), 
				(LPARAM)msghandle);
    }
    else if (nType == MMS_CALLEDIT_IMAGE || nType == MMS_CALLEDIT_SOUND 
		|| nType == MMS_CALLEDIT_MOBIL || nType == MMS_CALLEDIT_QTEXT
		|| nType == MMS_CALLEDIT_VCARD || nType == MMS_CALLEDIT_VCAL
		|| nType == MMS_CALLEDIT_MULTIIMAGE)
    {
        PostMessage(hwndPre, msg, bSaveOrSend, 0);
    }
    // forward
    /*else if (nType == MMS_CALLEDIT_TRAN)
    {
        curFordid = GetCurFolder();
        if (curFordid != MU_OUTBOX)
            return;

        nFileType = MMS_GetFileMmft(szFileName);

        if (nFileType != MMFT_DRAFT)
        {            
            msghandle = AllocMsgHandle(szFileName, nFileType, FALSE);
            if (msghandle == -1)
            {
                MsgWin(MMS_MSG_5, MMS_MSG, MMS_WAITTIME);
                return;
            }
            FillMsgNode(&msgnode, MU_MSG_MMS, nFileType, tmms.mms.wspHead.date, 
                msghandle, pSubject, tmms.mms.wspHead.to, MU_STORE_IN_FLASH);

            SendMessage(hwndPre, PWM_MSG_MU_NEWMTMSG, 
                MAKEWPARAM(MU_ERR_SUCC, MU_MDU_MMS), (LPARAM)&msgnode);
        }
    }*/
}

/*********************************************************************\
* Function     InitMms
* Purpose      
* Params       
* Return           
* Remarks      
**********************************************************************/
static int InitMms(PMMS_EDITCREATEDATA pCreateData, PSTR FileName, UINT nFlag)
{
    int i = 0;
    int  hMMSFile;

    pCreateData->curSlide = 0;
    memset(&pCreateData->tmms.mms, 0, sizeof(MultiMMS));
    pCreateData->tmms.mms.wspHead.nSlidnum = 1;

//intiliaze filename
    switch(nFlag)
    {
        case MMS_CALLEDIT_NEW:      // On New MMS
        case MMS_CALLEDIT_REPLY:
        case MMS_CALLEDIT_REPLYALL:
        case MMS_CALLEDIT_IMAGE:
		case MMS_CALLEDIT_MULTIIMAGE:
        case MMS_CALLEDIT_SOUND:
        case MMS_CALLEDIT_TEXT:
        case MMS_CALLEDIT_MOBIL:
        case NOTI_CALLEDIT_REPLY:
        case MMS_CALLEDIT_TRAN:
		case MMS_CALLEDIT_QTEXT:
		case MMS_CALLEDIT_VCARD:
		case MMS_CALLEDIT_VCAL:
            MMS_CreateFileName(pCreateData->mmsDraftFile, TYPE_DRAFT, MU_DRAFT);
            break;
        case MMS_CALLEDIT_OPEN:
            strcpy(pCreateData->mmsDraftFile, FileName);
            break;
        default:
            break;
    }   
    
	switch(nFlag) 
	{
	case MMS_CALLEDIT_VCARD:
		strcpy(pCreateData->tmms.mms.wspHead.subject, "Business card");
		break;
		
	case MMS_CALLEDIT_VCAL:
		strcpy(pCreateData->tmms.mms.wspHead.subject, "Calendar entry");
		break;
	
	default:
		break;
	}

    if ( nFlag == MMS_CALLEDIT_TRAN || nFlag == MMS_CALLEDIT_REPLY
            || nFlag == MMS_CALLEDIT_REPLYALL || nFlag == MMS_CALLEDIT_OPEN ||
            nFlag == NOTI_CALLEDIT_REPLY  ||nFlag == MMS_CALLEDIT_TRAN)
    {
        if (strstr(FileName, ROMPATH) != NULL)
            chdir(ROMPATH);
        else 
            chdir(FLASHPATH);
        
        hMMSFile = MMS_CreateFile(FileName, O_RDONLY);
        
        if (-1 == hMMSFile)
            return 0;

        if ( nFlag != NOTI_CALLEDIT_REPLY &&
            !MMS_IsMyfile(hMMSFile))
        {
            MMS_CloseFile(hMMSFile);
            return 0;
        }

        switch(nFlag) 
        {
        // mms can be Opened in draft
        case MMS_CALLEDIT_OPEN:
            {
                int     subLen;
                int     nFit;
                
                MMS_CloseFile(hMMSFile);
                ReadAndParse(FileName, &pCreateData->tmms);
                subLen = strlen(pCreateData->tmms.mms.wspHead.subject);
                if ( subLen > (SIZE_SUBJECT - 3))
                {
                    GetTextExtentExPoint(NULL, pCreateData->tmms.mms.wspHead.subject, subLen, 
                        (SIZE_SUBJECT - 3) * ENG_FONT_WIDTH, &nFit, NULL, NULL);
                    
                    pCreateData->tmms.mms.wspHead.subject[nFit] = '\0';
                }
                i = 0;
        
                if(pCreateData->tmms.pAttatch)
                    GetAttacthEnd(pCreateData);
            }
            break;

        case MMS_CALLEDIT_REPLY:
            {
                char    subject[SIZE_1];
                int     subLen;
                int     nFit;

                
                lseek(hMMSFile, MMS_HEADER_SIZE + mms_offsetof(WSPHead, from), SEEK_SET);
                read(hMMSFile, &pCreateData->tmms.mms.wspHead.to, SIZE_1);
                lseek(hMMSFile, MMS_HEADER_SIZE+mms_offsetof(WSPHead, subject) , SEEK_SET);//SIZE_1 * 3*/, SEEK_CUR);
                read(hMMSFile,&pCreateData->tmms.mms.wspHead.subject, SIZE_1);
                
                subLen = strlen(pCreateData->tmms.mms.wspHead.subject);
                if ( subLen > (SIZE_SUBJECT - 3))
                {
                    GetTextExtentExPoint(NULL, pCreateData->tmms.mms.wspHead.subject, subLen, 
                        (SIZE_SUBJECT - 3) * ENG_FONT_WIDTH, &nFit, NULL, NULL);
                    
                    pCreateData->tmms.mms.wspHead.subject[nFit] = '\0';
                }
                sprintf(subject, "Re:%s", pCreateData->tmms.mms.wspHead.subject);
                strcpy(pCreateData->tmms.mms.wspHead.subject, subject);
            }
            break;

        case NOTI_CALLEDIT_REPLY:
            {
                int len, fieldlen;
                char *buf;
                char *field;
                char szfrom[SIZE_1];

                len = MMS_GetFileSize(FileName);
                buf = MMS_malloc(len);
                read(hMMSFile, buf, len);
                field = FindContentByKey(buf, "From", &fieldlen);
                if (!field)
                {
                    MMS_free(buf);
                    MMS_CloseFile(hMMSFile);
                    WaitWindowState(NULL, FALSE);
                    return 0;
                }
                if (fieldlen > SIZE_1)
                {
                    strncpy(szfrom, field, SIZE_1);
                    szfrom[SIZE_1] = '\0';
                }
                else
                {
                    strncpy(szfrom, field, fieldlen);
                    szfrom[fieldlen] = '\0';
                }

				strcpy(pCreateData->tmms.mms.wspHead.to, szfrom);
                MMS_free(buf);
            }
            break;

        case MMS_CALLEDIT_REPLYALL:
    
            MMS_ReplyAll(pCreateData, hMMSFile);

            break;

        case MMS_CALLEDIT_TRAN:
            {
                char  subject[SIZE_1];
                int     subLen;
                int     nFit;
                
                MMS_CloseFile(hMMSFile);
                ReadAndParse(FileName, &pCreateData->tmms);
                if (pCreateData->tmms.mms.wspHead.nMmsSize > MAX_MMS_LEN)
                {
                    MsgWin(NULL, NULL, 0, STR_MMS_OVERSIZE, 
						STR_MMS_MMS, Notify_Alert,
                        STR_MMS_OK, NULL,WAITTIMEOUT);
                    return 0;
                }
                subLen = strlen(pCreateData->tmms.mms.wspHead.subject);
                if ( subLen > (SIZE_SUBJECT - 3))
                {
                    GetTextExtentExPoint(NULL, pCreateData->tmms.mms.wspHead.subject, subLen, 
                        (SIZE_SUBJECT - 3) * ENG_FONT_WIDTH, &nFit, NULL, NULL);
                    
                    pCreateData->tmms.mms.wspHead.subject[nFit] = '\0';
                }
                sprintf(subject, "FW:%s", pCreateData->tmms.mms.wspHead.subject);
                strcpy(pCreateData->tmms.mms.wspHead.subject, subject);
                pCreateData->tmms.mms.wspHead.to[0] = '\0';
                pCreateData->tmms.mms.wspHead.from[0] = '\0';
				if (pCreateData->tmms.mms.slide[i].pImage)
				{
		            int imgW = 0, imgH = 0;

					//shrink to fit maintaining proportions of the original image if they are 
					//larger than the horizonal dimension
					imgW = pCreateData->tmms.mms.slide[pCreateData->curSlide].imgSize.cx;
					
					pCreateData->tmms.mms.slide[pCreateData->curSlide].imgSize.cx = 
						imgW < SCREEN_WIDTH ? imgW : SCREEN_WIDTH;
					
					pCreateData->tmms.mms.slide[pCreateData->curSlide].imgSize.cy = 
						pCreateData->tmms.mms.slide[pCreateData->curSlide].imgSize.cy *
						pCreateData->tmms.mms.slide[pCreateData->curSlide].imgSize.cx/imgW;
					
					imgH = pCreateData->tmms.mms.slide[pCreateData->curSlide].imgSize.cy;
					
					pCreateData->tmms.mms.slide[pCreateData->curSlide].imgSize.cy = 
						imgH < SCREEN_HEIGHT ? imgH : SCREEN_HEIGHT;
					
					if (pCreateData->tmms.mms.slide[pCreateData->curSlide].imgSize.cy == SCREEN_HEIGHT)
						pCreateData->tmms.mms.slide[pCreateData->curSlide].imgSize.cx = 
						pCreateData->tmms.mms.slide[pCreateData->curSlide].imgSize.cx *
						SCREEN_HEIGHT/imgH;
					
					pCreateData->tmms.mms.slide[pCreateData->curSlide].imgPoint.x = 
						(SCREEN_WIDTH - pCreateData->tmms.mms.slide[pCreateData->curSlide].imgSize.cx)/2;

				}
                if (pCreateData->tmms.mms.slide[i].pImage && 
                    pCreateData->tmms.mms.slide[i].pText &&
                    pCreateData->tmms.mms.slide[i].pText->Metalen > 0)
                {
//                    if (tmms.mms.slide[i].txtPoint.y < tmms.mms.slide[i].imgPoint.y)
//                        bTxtOnTop[i] = TRUE;
                }
                if(pCreateData->tmms.pAttatch)
                    GetAttacthEnd(pCreateData);
                break;
            }
        
        default:
            break;
        }
        MMS_CloseFile(hMMSFile);
    }

    return 1;
}

/******************************************************************** 
* Function  OnCallMMSInfo   
* Purpose   call mmsinfo   
* Params     hWnd  
* Return     void
**********************************************************************/
static void OnCallMMSInfo(HWND hWnd)
{
    ACCEPTINFO  headInfo;
    PMMS_EDITCREATEDATA pCreateData;
    
    pCreateData = GetUserData(hWnd);

    memset(&headInfo, 0, sizeof(ACCEPTINFO));
    
    headInfo.acceptTime = pCreateData->tmms.mms.wspHead.date;               //time
    strcpy(headInfo.From, pCreateData->tmms.mms.wspHead.from);              //from
    strcpy(headInfo.To, pCreateData->tmms.mms.wspHead.to);                  //to
    strcpy(headInfo.Subject, pCreateData->tmms.mms.wspHead.subject);            //subject
    headInfo.MsgSize = pCreateData->tmms.mms.wspHead.nMmsSize;
   // sprintf(headInfo.MsgSize, "%d", pCreateData->tmms.mms.wspHead.nMmsSize);  //size
    headInfo.infoType = INFO_HEAD;                              //mms type                      
    GetLocalTime(&headInfo.acceptTime);                         //get local time
	//show mms information window
    CallDisplayInfo(pCreateData->hWndFrame, hWnd, 0, headInfo, FALSE, FALSE);
}

/*********************************************************************\
* Function    OnSaveMMS 
* Purpose      save mms
* Params       
* Return           
* Remarks      
**********************************************************************/
static BOOL OnSaveMMS(HWND hWnd, const char * mmsSaveFileName)
{
    int i, sublen = 0;
	HWND   hwndPhoneNum = NULL;
	DWORD  handle = 0;
    PMMS_EDITCREATEDATA pCreateData;
	char dupTo [513];
	int len;
    
    pCreateData = GetUserData(hWnd);

/*    if(!IsMMSNull(&pCreateData->tmms))
    {
        MsgWin(pCreateData->hWndFrame, NULL, 0, STR_MMS_CONTENTNULL, 
			STR_MMS_MMS, Notify_Alert, STR_MMS_OK,
            NULL, MMS_WAITTIME);
        return FALSE;
    }
*/
    if (pCreateData->tmms.mms.wspHead.nMmsSize > MAX_MMS_LEN)
    {
        MsgWin(NULL, NULL, 0, STR_MMS_BIG, STR_MMS_MMS, 
			Notify_Alert, STR_MMS_OK, NULL, MMS_WAITTIME);
        return FALSE;
    }
	hwndPhoneNum = GetDlgItem(hWnd, ID_PHONENUM);
	MMS_RecipientGetText(hwndPhoneNum, pCreateData->tmms.mms.wspHead.to,
		MAX_ADDR_NUM);
	
    GetLocalTime(&pCreateData->tmms.mms.wspHead.date);
    
    for (i = 0; i < pCreateData->tmms.mms.wspHead.nSlidnum; i++)
    {
        strcpy(pCreateData->tmms.mms.slide[i].imgAlt, MMS_IMAGE_ALT);        
        AddNameofObject(&pCreateData->tmms.mms.slide[i], i);
    }

    chdir(FLASHPATH);
    strcpy(pCreateData->tmms.mms.wspHead.ConType, "application/vnd.wap.multipart.related"); 
    pCreateData->tmms.mms.wspHead.bReply = MMSPro_IsReply();
	
	if (pCreateData->EditMsgInfo.nType != MMS_CALLEDIT_TRAN 
		&& pCreateData->EditMsgInfo.nType != MMS_CALLEDIT_REPLY
		&& pCreateData->EditMsgInfo.nType != MMS_CALLEDIT_REPLYALL
		&& pCreateData->EditMsgInfo.nType != NOTI_CALLEDIT_REPLY
		&& pCreateData->EditMsgInfo.nType != MMS_CALLEDIT_VCARD
		&& pCreateData->EditMsgInfo.nType != MMS_CALLEDIT_VCAL
		&& pCreateData->tmms.mms.slide[0].pText->Metadata)
	{
		sublen = min(pCreateData->tmms.mms.slide[0].pText->Metalen,
			30);
		strncpy(pCreateData->tmms.mms.wspHead.subject, 
		pCreateData->tmms.mms.slide[0].pText->Metadata, sublen);
	}

	len = MultiByteToUTF8(CP_ACP, 0, pCreateData->tmms.mms.wspHead.to, -1, dupTo, 512);
	dupTo[len] = 0;

	strcpy(pCreateData->tmms.mms.wspHead.to, dupTo);
	
    if (PackMmsBody(mmsSaveFileName, pCreateData->tmms) == RETURN_OK)
    {
        pCreateData->bSave = TRUE;
		handle = GetHandleByName(mmsSaveFileName);
		if (handle == 0)
			AllocMsgHandle(mmsSaveFileName, MMFT_DRAFT, FALSE);
        return TRUE;
    }
	MsgWin(NULL, NULL, 0, ML("Not enough memory.\r\nMessage not saved."), 
				STR_MMS_MMS, Notify_Failure, STR_MMS_OK, NULL, MMS_WAITTIME);
	return FALSE;
}

/*********************************************************************\
* Function     SetMMSContent
* Purpose      when edit mms set its content first
* Params       
* Return           
* Remarks      
**********************************************************************/
void SetMMSContent(HWND hWnd)
{
    PMMS_EDITCREATEDATA pCreateData;
    HWND hwndPhoneNum = NULL;
    HWND hwndContent = NULL;
	char *p = NULL;
	char *addToken = NULL;
    
    pCreateData = GetUserData(hWnd);
    hwndPhoneNum = GetDlgItem(hWnd, ID_PHONENUM);
    hwndContent = GetDlgItem(hWnd, ID_CONTENT);

	UTF8ToMultiByte(CP_ACP, 0, pCreateData->tmms.mms.wspHead.to, -1, pCreateData->tmms.mms.wspHead.to, SIZE_1 - 1, NULL, NULL);
    if (pCreateData->tmms.mms.wspHead.to[0] != 0)
	{
		p = MMS_malloc(strlen(pCreateData->tmms.mms.wspHead.to)+1);
		strcpy(p, pCreateData->tmms.mms.wspHead.to);

		addToken = MMS_chrtok(p, MMS_ASEPCHR, MMS_ASEPCHRF);
		while (addToken) 
		{
			if (*addToken == '\0')
			{
				addToken = MMS_chrtok(NULL,  MMS_ASEPCHR, MMS_ASEPCHRF);
				continue;
			}			
			SendMessage(hwndPhoneNum,GHP_ADDREC,1,(LPARAM)addToken);
			addToken = MMS_chrtok(NULL, MMS_ASEPCHR, MMS_ASEPCHRF);
		}
	}

    if (pCreateData->tmms.mms.slide[0].pText != NULL)
        SetWindowText(hwndContent, pCreateData->tmms.mms.slide[0].pText->Metadata);

	if (pCreateData->tmms.mms.slide[pCreateData->curSlide].pAudio)
		MMS_SetSoundInd(hWnd, TRUE);
}

/*********************************************************************\
* Function     OnSendMms
* Purpose      
* Params       
* Return           
* Remarks      
**********************************************************************/
static BOOL OnSendMms(HWND hWnd, BOOL bSend)
{   
     int     i = 0,iRet = 0;
    char    *token;
    BOOL    bSpace = TRUE;
    char    dupTo[SIZE_6];
    int     nSender = 0;
    HWND    hwndPhoneNum = NULL;
    PMMS_EDITCREATEDATA pCreateData;
    
    pCreateData = GetUserData(hWnd);
    hwndPhoneNum = GetDlgItem(hWnd, ID_PHONENUM);

    //phone number is null
//    if (0 == GetWindowTextLength(hwndPhoneNum))
//    {
//        MsgWin(ML(STR_MMS_PNUM), ML(STR_MMS_MMS), Notify_Alert, NULL, NULL,MMS_WAITTIME);
//        return FALSE;
//    }
//    MMS_UpdateNameInfo(szUserInputNumber);
    //SendMessage(hwndPhoneNum,GHP_GETREC,(WPARAM)SIZE_1,(LPARAM)pCreateData->tmms.mms.wspHead.to);
	MMS_RecipientGetText(hwndPhoneNum,pCreateData->tmms.mms.wspHead.to, MAX_ADDR_NUM);
	MultiByteToUTF8(CP_ACP, 0, pCreateData->tmms.mms.wspHead.to, -1, dupTo, SIZE_6);
	strcpy(pCreateData->tmms.mms.wspHead.to, dupTo);
    //strcpy(dupTo, pCreateData->tmms.mms.wspHead.to);
    token = MMS_chrtok(dupTo, MMS_ASEPCHR, MMS_ASEPCHRF);
    while (token != NULL) 
    {
        if (*token != '\0')
        {
            nSender++;
            bSpace = FALSE;
        }
        token = MMS_chrtok(NULL, MMS_ASEPCHR, MMS_ASEPCHRF);
    }
    if (bSpace)
        return FALSE;
    
    if (nSender > MAX_SENDER_NUM)
    {
        MsgWin(NULL, NULL, 0, STR_MMS_OVERSENDNUM, STR_MMS_MMS, 
			Notify_Alert, STR_MMS_OK, NULL, MMS_WAITTIME);
        return FALSE;
    }
    
//    if (pCreateData->tmms.mms.wspHead.nMmsSize == 0 && pCreateData->tmms.mms.wspHead.subject[0] == 0)
//    {
//        MsgWin(ML(STR_MMS_NULL), ML(STR_MMS_MMS), Notify_Alert, NULL,
//          NULL, MMS_WAITTIME);
//        return FALSE;
//    }
    if (pCreateData->tmms.mms.wspHead.nMmsSize > MAX_MMS_LEN)
    {
        MsgWin(NULL, NULL, 0, STR_MMS_OVERSIZE, STR_MMS_MMS, 
			Notify_Alert, STR_MMS_OK, NULL,MMS_WAITTIME);
        CallMMSObjList(pCreateData->hWndFrame, hWnd, pCreateData->tmms.pAttatch, EDIT_CALLOBJ);
        return FALSE;
    }

    pCreateData->tmms.mms.wspHead.bReply = MMSPro_IsReply();
    strcpy(pCreateData->tmms.mms.wspHead.ConType, "application/vnd.wap.multipart.related");

    GetLocalTime(&pCreateData->tmms.mms.wspHead.date);
    
    for (i = 0; i < pCreateData->tmms.mms.wspHead.nSlidnum; i++)
    {
        strcpy(pCreateData->tmms.mms.slide[i].imgAlt, MMS_IMAGE_ALT);
        pCreateData->tmms.mms.slide[i].imgPoint.y = 0;
        AddNameofObject(&pCreateData->tmms.mms.slide[i], i);
    }
    
	if(bSend)
	{
		MMSMSGWIN("MMS:Before mmspro_packsend\r\n");
		iRet = MMSPro_PackSend(pCreateData->mmsDraftFile, pCreateData->tmms, FALSE);
		MMSMSGWIN("MMS:Mmspro_packsend success\r\n");
		if (iRet < 2)
		{	
			pCreateData->bSave = TRUE;
			
			if (iRet == -1)//无效MMS设置
				return FALSE;
			
			return TRUE;
		}
		else
		{
			MMSMSGWIN("MMS: mmspro_packsend failure\r\n");
			MsgWin(NULL, NULL, 0, STR_MMS_SENDFAI, STR_MMS_MMS, 
				Notify_Alert, STR_MMS_OK ,NULL,MMS_WAITTIME);
			return FALSE;
		}
	}
	else
	{
		return TRUE;
	}

}
/*********************************************************************\
* Function     CalPoition
* Purpose      
* Params       
* Return           
* Remarks      
**********************************************************************/
static int CalPoistion(HWND hwnd, SLIDE * pSlide, int num)
{
    PMMS_EDITCREATEDATA pCreateData;
    
    pCreateData = GetUserData(hwnd);

    CalTxtSize(pCreateData->tmms.mms.slide[num].pText->Metadata, &pSlide->txtSize);

	//pSlide->imgPoint.y = 0;
    pSlide->txtPoint.y = pSlide->imgSize.cy + INTERVAL_BTN_TXT_IMG;

    pSlide->imgPoint.x = pSlide->imgSize.cx > SCREEN_WIDTH ? 
        0 : (SCREEN_WIDTH - pSlide->imgSize.cx) / 2;
    pSlide->txtPoint.x = 0;

    return 1;
}
/*********************************************************************\
* Function     SaveCurSlideTxt
* Purpose      
* Params       
* Return           
* Remarks      
**********************************************************************/
static int SaveCurSlideTxt(HWND hwnd)
{
    char txtName[32];
    mmsMetaNode *p;
    HWND        hwndContent = NULL;
    PMMS_EDITCREATEDATA pCreateData  =NULL;
    
    pCreateData = GetUserData(hwnd);

    hwndContent = GetDlgItem(hwnd, ID_CONTENT);
    if (pCreateData->tmms.mms.slide[pCreateData->curSlide].pText == NULL)
    {
        p = (mmsMetaNode *)MMS_malloc(sizeof(mmsMetaNode));
        p->refCount = 1;
        p->inSlide = pCreateData->curSlide;
        p->txtcharset = CHARSET_UTF8;
        p->Content_Id = NULL;
        p->Content_Type = NULL;
        p->Metalen = GetWindowTextLength(hwndContent);
        
        p->Metadata = MMS_malloc(p->Metalen + 1);
        // MMS_malloc fail
        if (p->Metadata == NULL)
        {
            MMS_free(p);
            return 0;
        }
        GetWindowText(hwndContent, p->Metadata, p->Metalen + 1);
        p->Metadata[p->Metalen] = 0;
        sprintf(txtName, "pollex_att%d.txt", pCreateData->curSlide);
        p->Content_Location = My_StrDup(txtName);
        p->MetaType = META_TEXT_PLAIN;
        
        if(pCreateData->tmms.pAttatch == NULL)
            pCreateData->tmms.pAttatch = p;
        else
            pCreateData->pEnd->pnext = p;    
        pCreateData->pEnd = p;   
        if (pCreateData->pEnd != NULL)
            pCreateData->pEnd->pnext = NULL;
        
        pCreateData->tmms.mms.slide[pCreateData->curSlide].pText = p;
    }
    else
    {
        p = pCreateData->tmms.mms.slide[pCreateData->curSlide].pText;

        MMS_free(p->Metadata);

        p->Metalen = GetWindowTextLength(hwndContent);
        p->Metadata = MMS_malloc(p->Metalen + 1);
        // MMS_malloc fail
        if (p->Metadata == NULL)
        {
            return 0;
        }
        GetWindowText(hwndContent, p->Metadata, p->Metalen + 1);
        p->Metadata[p->Metalen] = 0;
    }
    CalPoistion(hwnd, &pCreateData->tmms.mms.slide[pCreateData->curSlide], pCreateData->curSlide);
    return 1;
}
/*********************************************************************\
* Function     MoveBetweenSlide
* Purpose      
* Params       
* Return           
* Remarks      
**********************************************************************/
static void MoveBetweenSlides(HWND hwnd, int slide1, int slide2)
{
    PMMS_EDITCREATEDATA pCreateData;
    
    pCreateData = GetUserData(hwnd);

    memcpy(&pCreateData->tmms.mms.slide[slide1], &pCreateData->tmms.mms.slide[slide2], sizeof(SLIDE));
    
   // bTxtOnTop[slide1] = bTxtOnTop[slide2];
   // bTxtOnTop[slide2] = FALSE;
}

#ifndef MMS_18030
int CalTxtSize(PCSTR pTxt, SIZE * pSize)
{
    int i, j, len, nLine;

    if (pTxt == NULL)
    {
        nLine = 0;
    }
    else
    {
        len = strlen(pTxt);
        j = 0;
        nLine = 1;
        for (i = 0; i < len; i ++)
        {
            if (pTxt[i] == '\r' || pTxt[i] == '\n')
            {
                if (pTxt[i + 1] == '\n' || pTxt[i + 1] == '\r')
                {
                    i ++;
                    j ++;
                }
                nLine ++;
                j = 0;
                continue;
            }
                        
            if (MMS_IsChinese(pTxt[i]))
            {
                if (j == LMAXLEN - 1)
                {
                    i --;
                    j = 0;
                    nLine ++;
                }
                else if (j == LMAXLEN - 2)
                {
                    i ++;
                    j = 0;
                    nLine ++;
                }
                else
                {
                    i ++;
                    j += 2;
                }
            }
            else
            {
                if (j == LMAXLEN - 1)
                {
                    j = 0;
                    nLine ++;
                }
                else
                    j ++;
            }// end chinese else
        }// end for
    }// end else
    pSize->cx = SCREEN_WIDTH;
    pSize->cy = nLine * TXTSPACE;
    
    return nLine;
}
#else
int CalTxtSize(PCSTR pTxt, SIZE * pSize)
{
    int len, i, nNowLen;
    int nLine;
    int nFit = 0;
    int nMove = 0;
    int nDx[LMAX_18030];

    if (pTxt == NULL || *pTxt == '\0')
    {
        nLine = 0;
    }
    else
    {
        len = strlen(pTxt);
        nLine = 0;

        do 
        { 
            nNowLen = (len > LMAX_18030) ? LMAX_18030 : len;

            for (i = 0; i < nNowLen; i++)
            {
                if (pTxt[i] == '\r' || pTxt[i] == '\n')
                {
                    if (pTxt[i + 1] == '\n' || pTxt[i + 1] == '\r')
                    {
                        GetTextExtentExPoint(NULL, pTxt, i, SCREEN_WIDTH, 
                            &nFit, nDx, NULL);
                        nLine ++;
                        if (nFit < i)
                        {
                            if ((pTxt[nFit] == '\r' || pTxt[nFit] == '\n') 
                                && (pTxt[nFit + 1] == '\n' || pTxt[nFit + 1] == '\r'))
                                nMove = nFit + 2;
                            else
                                nMove = nFit;
                        }
                        else
                            nMove = nFit + 2;
                        
                        i = nFit;
                        break;
                    }
                }
            } 
            if (i < nNowLen)
            {
                pTxt += nMove;
                len -= nMove;
            }
            else
            {
                GetTextExtentExPoint(NULL, pTxt, len, SCREEN_WIDTH, &nFit, nDx, NULL);                
                nLine ++;
                if ((pTxt[nFit] == '\r' || pTxt[nFit] == '\n') 
                    && (pTxt[nFit + 1] == '\n' || pTxt[nFit + 1] == '\r'))
                    nMove = nFit + 2;
                else
                    nMove = nFit;
                pTxt += nMove;
                len -= nMove;
            }
        } while(*pTxt);
    }
    
    pSize->cx = SCREEN_WIDTH;
    pSize->cy = nLine * TXTSPACE;
    
    return nLine;
}
#endif
/******************************************************************** 
* Function  IsObjectCanInsert
* Purpose   judge wether this object can be inserted
* Params    const char* szFileName
            nType: the type of object
* Return    BOOL       
**********************************************************************/
static BOOL IsObjectCanInsert(PCSTR szFileName, int nType)
{
    int    hFile;
    int    nFileLen;
    int    nLimit;

    if (szFileName == NULL || *szFileName == 0 || (nType == MMS_CALLEDIT_MOBIL) 
		|| (nType == MMS_CALLEDIT_QTEXT))
        return TRUE;

    if (nType == MMS_CALLEDIT_IMAGE || nType == MMS_CALLEDIT_SOUND 
		|| nType == MMS_CALLEDIT_VCARD || nType == MMS_CALLEDIT_VCAL)
    {
        //sound, picture as mms
        nLimit = MAX_MMS_LEN;
    }
    else if (nType == MMS_CALLEDIT_TEXT)
    {
        //text as mms
        nLimit = MAX_TXT_SIZE;
    }

    hFile = open(szFileName, O_RDONLY, 0);
    
    if (hFile == -1)
    {
        MMSMSGWIN("this file can not be open", STR_MMS_MMS, MMS_WAITTIME);
        return FALSE;
    }
    
    if (nType != MMS_CALLEDIT_TEXT && FALSE == JudgeSupportType(hFile))
    {
		close(hFile);
        MsgWin(NULL, NULL, 0, STR_MMS_UNSUPPORT, STR_MMS_MMS,
            Notify_Failure, STR_MMS_OK, NULL, MMS_WAITTIME);
		return FALSE;
    }
    close(hFile);

    nFileLen = MMS_GetFileSize(szFileName);
    
    if (nFileLen > nLimit)
    {
        MsgWin(NULL, NULL, 0, STR_MMS_OVERSIZE, STR_MMS_MMS, 
			Notify_Alert, STR_MMS_OK,NULL,MMS_WAITTIME);
        return FALSE;
    }
    return TRUE;
}
/******************************************************************** 
* Function  FreeAllMMSData
* Purpose   MMS_free the malloced data
* Params    void
* Return    void       
**********************************************************************/
static void FreeAllMMSData(PMMS_EDITCREATEDATA pCreateData)
{
    FreeMeta(pCreateData->tmms.pAttatch);
    pCreateData->tmms.pAttatch = NULL;
}

/*********************************************************************\
* Function     GetNumberFromMultiSelect
* Purpose      
* Params       
* Return           
* Remarks      
**********************************************************************/
/*static void GetNumberFromMultiSelect(int num, PBTEL *pbSeltel)
{
    int i;
    int len;

    len = strlen(szUserInputNumber);
    if(len > 0 && ((szUserInputNumber[len-1] != ',') || (szUserInputNumber[len-1] != ';')))
    {
        strcat(szUserInputNumber, ",");
    }
    
    for (i = 0; i < num; i++)
    {
        if(strlen(szUserInputNumber) + strlen(pbSeltel[i].szTel) > SIZE_1)
        {
            break;
        }
        strcat(szUserInputNumber, pbSeltel[i].szTel);
        strcat(szUserInputNumber, ",");
    }

    len = strlen(szUserInputNumber);
    szUserInputNumber[len - 1] = 0x0;

    SetWindowText(hwndPhoneNum, szUserInputNumber);
}*///

/*********************************************************************\
* Function     GetEmailFromMultiSelect
* Purpose      
* Params       
* Return           
* Remarks      
**********************************************************************/
/*
static void GetEmailFromMultiSelect(int num, PBEMAIL *pbSeltel)
{
    int i;
    int len;
    len = strlen(szUserInputNumber);
    if(len > 0 && ((szUserInputNumber[len-1] != ',') || (szUserInputNumber[len-1] != ';')))
    {
        strcat(szUserInputNumber, ",");
    }
    
    for (i = 0; i < num; i++)
    {
        if(strlen(szUserInputNumber) + strlen(pbSeltel[i].szAddr) > SIZE_1)
        {
            break;
        }
        strcat(szUserInputNumber, pbSeltel[i].szAddr);
        strcat(szUserInputNumber, ",");
    }
    len = strlen(szUserInputNumber);
    szUserInputNumber[len - 1] = 0x0;
    SetWindowText(hwndPhoneNum, szUserInputNumber);
}*///


// get the name which saved in phonebook
/*********************************************************************\
* Function     GetNameStrFromAdd
* Purpose      
* Params       
* Return           
* Remarks      
**********************************************************************/
static int GetNameStrFromAdd(char *szExist, const char *szAddress)
{
    char *addToken = NULL;
    char *szDup = NULL;
    char name[AB_MAXLEN_NAME];
    int  num, len;
    BOOL bFind;

    if (szExist == NULL)
        return 0;

    szExist[0] = '\0';
    name[0] = '\0';
    num = 0;

    len = strlen(szAddress);
    szDup = MMS_malloc(len + 1);
    strcpy(szDup, szAddress);

    addToken = MMS_chrtok(szDup,  MMS_ASEPCHR, MMS_ASEPCHRF);
    
    while(addToken != NULL)
    {
        if (addToken[0] == '\0')
        {
            addToken = MMS_chrtok(NULL,  MMS_ASEPCHR, MMS_ASEPCHRF);
            continue;
        }

        bFind = MMS_FindNameInPB(addToken, name, ADDR_UNKNOWN);

        if (bFind)
        {
            num ++;
            strcat(szExist, name);
            strcat(szExist, MMS_ASEPSTR);
        }
        addToken = MMS_chrtok(NULL, MMS_ASEPCHR, MMS_ASEPCHRF);
    }
    MMS_free(szDup);
    szDup = NULL;

    len = strlen(szExist);
    
    if (len > 0)
    {
        if (szExist[len - 1] == MMS_ASEPCHR)
            szExist[len - 1] = '\0';
    }
    return num;
}

/*********************************************************************\
* Function     GetPhoneNum
* Purpose      
* Params       
* Return           
* Remarks      
**********************************************************************/
static int GetPhoneNum(char * szString)
{
    int len, i;
    int nAtcount,nSepCount;

    char sz[SIZE_1 + 1];
    memset(sz, 0x0, SIZE_1 +1);
    strcpy(sz, szString);

    len = strlen(sz);
    if(sz[len -1] != ',')
    {
        sz[len] = ',';
        len ++;
    }
    
    nAtcount = nSepCount = 0;
    
    for( i = 0; i < len; i++ )
    {
        if(sz[i] == ',' || sz[i] == ';')
            nSepCount ++;
        if(sz[i] == '@')
            nAtcount ++;

    }
    return (nSepCount - nAtcount);
}
/*********************************************************************\
* Function     GetMailNum
* Purpose      
* Params       
* Return           
* Remarks      
**********************************************************************/
static int GetMailNum(char *szString)
{
    int len,i;
    int nAtCount;
    char *pStr;

    len = strlen(szString);
    pStr = szString;
    nAtCount = 0;

    for(i = 0; i < len; i++)
    {
        if(*pStr == '@')
        {
            nAtCount ++;
        }
        pStr ++;
    }
    return nAtCount;
}
/*********************************************************************\
* Function     OnEditInsert
* Purpose      
* Params       
* Return           
* Remarks      
**********************************************************************/
static BOOL OnEditInsert(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    char *InsertFileName;
    int  hFile;            
    int  highwparam, lowwparam;
    char pPathName[MAX_PATH_LEN];
    char pFileName[MAX_FILENAME];
    HWND hwndContent = NULL;
    PMMS_EDITCREATEDATA pCreateData;

    if(lParam == 0)
        return FALSE;
    
    //插入文件的名字
    InsertFileName = (char*)lParam;

    if (*InsertFileName == '\0')
        return FALSE;
    
    while (*InsertFileName == 0x20)    // space
    {
        InsertFileName++;
    }

    pCreateData = GetUserData(hWnd);
    hwndContent = GetDlgItem(hWnd, ID_CONTENT);

    highwparam = HIWORD(wParam);
    lowwparam = LOWORD(wParam);
    if (highwparam != RTN_QTEXT && wParam != MMS_CALLEDIT_QTEXT)
    {
		if (highwparam != RTN_TEXT && wParam != MMS_CALLEDIT_TEXT 
			&& !JudgeSupportTypeFromName(InsertFileName))
		{
			MsgWin(NULL, NULL, 0, STR_MMS_UNSUPPORT, STR_MMS_MMS,
				Notify_Failure, STR_MMS_OK, NULL, MMS_WAITTIME);
			
			return FALSE;
		}
        MMS_ParseFileName(InsertFileName, pFileName, pPathName);

        chdir(pPathName);
    }
    
    if (highwparam == RTN_IMAGE || wParam == MMS_CALLEDIT_IMAGE)
    {
		if (pCreateData->tmms.mms.slide[pCreateData->curSlide].pImage)
        {
            // add a slide
			if (pCreateData->tmms.mms.wspHead.nSlidnum == MMS_MAXNUM)
			{
				MsgWin(NULL, NULL, 0, "Too many slides", STR_MMS_MMS,
					Notify_Failure, STR_MMS_OK, NULL, MMS_WAITTIME);
                return FALSE;
			}
            SendMessage(hWnd, WM_COMMAND, IDM_INSERT_SLIDE, NULL);
            SendMessage(hWnd, MMS_EDIT_INSERT,MAKEWPARAM(0, RTN_IMAGE), (LPARAM)InsertFileName);
            return TRUE;
        }

        pCreateData->tmms.mms.slide[pCreateData->curSlide].pImage = 
            OnInsertAttatch(hWnd,RTN_IMAGE, pFileName, TRUE); 
        
        if (pCreateData->tmms.mms.slide[pCreateData->curSlide].pImage)
        {
            int imgW = 0, contentY = 0, imgH = 0;

            GetImageDimensionFromFile(InsertFileName, 
                &pCreateData->tmms.mms.slide[pCreateData->curSlide].imgSize); //if bottom-system is linux, use absolute path      
            
            MMS_InsertWindowState(NULL, TRUE);

            //shrink to fit maintaining proportions of the original image if they are 
            //larger than the horizonal dimension
            imgW = pCreateData->tmms.mms.slide[pCreateData->curSlide].imgSize.cx;

            pCreateData->tmms.mms.slide[pCreateData->curSlide].imgSize.cx = 
                imgW < SCREEN_WIDTH ? imgW : SCREEN_WIDTH;

            pCreateData->tmms.mms.slide[pCreateData->curSlide].imgSize.cy = 
                pCreateData->tmms.mms.slide[pCreateData->curSlide].imgSize.cy *
                pCreateData->tmms.mms.slide[pCreateData->curSlide].imgSize.cx/imgW;

			imgH = pCreateData->tmms.mms.slide[pCreateData->curSlide].imgSize.cy;

			pCreateData->tmms.mms.slide[pCreateData->curSlide].imgSize.cy = 
                imgH < SCREEN_HEIGHT ? imgH : SCREEN_HEIGHT;

			if (pCreateData->tmms.mms.slide[pCreateData->curSlide].imgSize.cy == SCREEN_HEIGHT)
				pCreateData->tmms.mms.slide[pCreateData->curSlide].imgSize.cx = 
                pCreateData->tmms.mms.slide[pCreateData->curSlide].imgSize.cx *
                SCREEN_HEIGHT/imgH;

            pCreateData->tmms.mms.slide[pCreateData->curSlide].imgPoint.x = 
                (SCREEN_WIDTH - pCreateData->tmms.mms.slide[pCreateData->curSlide].imgSize.cx)/2;
            
            contentY = pCreateData->tmms.mms.slide[pCreateData->curSlide].imgSize.cy + 
                pCreateData->tmms.mms.slide[pCreateData->curSlide].imgPoint.y + 1;
         
			MMSEdit_SetVScroll(hWnd, FALSE);

            MoveWindow(hwndContent, 0, contentY, SCREEN_WIDTH, 
                pCreateData->nHeight, FALSE);
            
            InvalidateRect(hWnd, NULL, FALSE);
            MMS_InsertWindowState(NULL, FALSE);                
        }
    }// end image
    else if (highwparam == RTN_SOUND || wParam == MMS_CALLEDIT_SOUND)
    {  
        if (pCreateData->tmms.mms.slide[pCreateData->curSlide].pAudio)
        {
			if (pCreateData->tmms.mms.wspHead.nSlidnum == MMS_MAXNUM)
			{
				MsgWin(NULL, NULL, 0, "Too many slides", STR_MMS_MMS,
					Notify_Failure, STR_MMS_OK, NULL, MMS_WAITTIME);
                return FALSE;
			}
            // add a slide
            SendMessage(hWnd, WM_COMMAND, IDM_INSERT_SLIDE, NULL);
            SendMessage(hWnd, MMS_EDIT_INSERT,MAKEWPARAM(0, RTN_SOUND), (LPARAM)InsertFileName);
            return TRUE;
        }
        pCreateData->tmms.mms.slide[pCreateData->curSlide].pAudio = 
            OnInsertAttatch(hWnd, RTN_SOUND, pFileName, TRUE);

        if (pCreateData->tmms.mms.slide[pCreateData->curSlide].pAudio)
            MMS_SetSoundInd(hWnd, TRUE);
    }// end sound
    else if (highwparam == RTN_QTEXT || wParam == MMS_CALLEDIT_QTEXT)
    {
//        char *szContent;
        int nLen, allLen;
        
        nLen = GetWindowTextLength(hwndContent);
            
        allLen = nLen + strlen((char*)lParam);
        if (allLen > MAX_TXT_SIZE)
        {
            MsgWin(NULL, NULL, 0, STR_MMS_OVERSIZE, STR_MMS_MMS, 
				Notify_Alert, STR_MMS_OK, NULL, MMS_WAITTIME);
            return FALSE;
        }
        
        pCreateData->tmms.mms.wspHead.nMmsSize += strlen((char*)lParam)*TXT_UTF8_MUL;
        if (pCreateData->tmms.mms.wspHead.nMmsSize > MAX_MMS_LEN)
        {
            MsgWin(NULL, NULL, 0, STR_MMS_OVERSIZE, STR_MMS_MMS, 
				Notify_Alert,STR_MMS_OK,NULL, MMS_WAITTIME);
            pCreateData->tmms.mms.wspHead.nMmsSize -= strlen((char*)lParam)*TXT_UTF8_MUL;
			MMS_SetIndicator(hWnd, pCreateData->curSlide, 
						pCreateData->tmms.mms.wspHead.nSlidnum, 
						pCreateData->tmms.mms.wspHead.nMmsSize);
            return FALSE;
        }
        MMS_SetIndicator(hWnd, pCreateData->curSlide, 
						pCreateData->tmms.mms.wspHead.nSlidnum, 
						pCreateData->tmms.mms.wspHead.nMmsSize);
        pCreateData->tmms.mms.slide[pCreateData->curSlide].pText->Metalen = allLen;  
		SendMessage(hwndContent, EM_REPLACESEL, 0, lParam);
#if 0
		{
			szContent = (char *)MMS_malloc(allLen + 1 );
            
			szContent[0] = 0;
			GetWindowText(hwndContent, szContent, nLen + 1 );
			
			if(strcmp(szContent,STR_MMS_CONTENTDEF) == 0)
				SetWindowText(hwndContent,(char*)lParam);
			else
			{
				strcat(szContent, (char*)lParam);
				SetWindowText(hwndContent, szContent);  
			}
        }
		MMS_free(szContent);
#endif
		SaveCurSlideTxt(hWnd);
        
    }// end qtext
    else if (highwparam == RTN_TEXT || wParam == MMS_CALLEDIT_TEXT)
    {
        char *text = NULL;
        int textlen;
        
        hFile = MMS_CreateFile(pFileName, O_RDONLY);
        
        if (hFile == -1)
            return FALSE;
        
        textlen = MMS_GetFileSize(pFileName);
        
        if (textlen >= MAX_TXT_SIZE)
        {
            MsgWin(NULL, NULL, 0, STR_MMS_OVERSIZE, STR_MMS_MMS, Notify_Alert,
                STR_MMS_OK, NULL,MMS_WAITTIME);
            
            MMS_CloseFile(hFile);
            return FALSE;
        }
        
        pCreateData->tmms.mms.wspHead.nMmsSize += textlen*TXT_UTF8_MUL;
        if (pCreateData->tmms.mms.wspHead.nMmsSize > MAX_MMS_LEN)
        {
            MsgWin(NULL, NULL, 0, STR_MMS_OVERSIZE, STR_MMS_MMS, Notify_Alert,
                STR_MMS_OK, NULL,MMS_WAITTIME);
            pCreateData->tmms.mms.wspHead.nMmsSize -= textlen*TXT_UTF8_MUL;
            MMS_CloseFile(hFile);
            return FALSE;
        }
        
        text = (char *)MMS_malloc(textlen + 1);
        //读取文本数据
        read(hFile, text, textlen);
        MMS_CloseFile(hFile);               
        
        text[textlen] = '\0';
        
//      nExistlen = GetWindowTextLength(hwndContent);
        if (textlen >= MAX_TXT_SIZE)
        {
            MsgWin(NULL, NULL, 0, STR_MMS_OVERSIZE, STR_MMS_MMS, Notify_Alert, 
                STR_MMS_OK, NULL,MMS_WAITTIME);
            pCreateData->tmms.mms.wspHead.nMmsSize -= textlen*TXT_UTF8_MUL;
            MMS_free(text);
			MMS_SetIndicator(hWnd, pCreateData->curSlide, 
						pCreateData->tmms.mms.wspHead.nSlidnum, 
						pCreateData->tmms.mms.wspHead.nMmsSize);
            return FALSE;
        }
		MMS_SetIndicator(hWnd, pCreateData->curSlide, 
						pCreateData->tmms.mms.wspHead.nSlidnum, 
						pCreateData->tmms.mms.wspHead.nMmsSize);

        if(textlen == 0)
            SetWindowText(hwndContent,STR_MMS_CONTENTDEF);
        else
            SetWindowText(hwndContent, text);

        MMS_free(text);
        SaveCurSlideTxt(hWnd);
    }// end text
    else if (highwparam == RTN_ATTATCH || wParam == MMS_CALLEDIT_VCARD 
		|| wParam == MMS_CALLEDIT_VCAL)
    {
        OnInsertAttatch(hWnd, wParam, pFileName, FALSE);
    }   
    pCreateData->bModify = TRUE; 
    return TRUE;
}
/*********************************************************************\
* Function     OnInsertAttatch
* Purpose      when insert a object, call this func
**********************************************************************/
static mmsMetaNode* OnInsertAttatch(HWND hwnd, int nAttatchType, PCSTR szFileName, BOOL bSmil)
{
    mmsMetaNode *p = NULL;
    int          hAttatch, len;
    int          nFilesize = 0;
    BOOL         bRepeatname = FALSE;
    static       int  num = 0;
    PMMS_EDITCREATEDATA pCreateData;
	char szTemp [256];
    
    pCreateData = GetUserData(hwnd);

    if(strlen(szFileName) > MAX_FILENAME)
        return NULL;
    
    if(MMS_GetFileSize(szFileName) == 0)
    {
        MsgWin(NULL, NULL, 0, STR_MMS_CONTENTNULL, STR_MMS_MMS, Notify_Alert, 
            STR_MMS_OK, NULL,MMS_WAITTIME);
        return NULL;
    }

    p = pCreateData->tmms.pAttatch;
    while (p)
    {
        if (p->Content_Location &&
            (stricmp(p->Content_Location, szFileName) == 0
            ||(strnicmp(p->Content_Location, "pollex_att", 10) == 0
            &&strnicmp((char*)p->Content_Location+10, (char*)szFileName,strlen(szFileName))==0)))
        {
            if(p->Metalen != MMS_GetFileSize(szFileName))
            {
                p = p->pnext;
                bRepeatname = TRUE;
                continue;
            }
            
            if (bSmil)
            {
//                if (p->inSlide >= 0)
//                {
//                    p->refCount++;
//                    return p;
//                }
//                else
                    bRepeatname = TRUE;
            }                                                                                                    
            else
            {
                if (p->inSlide == -1)
                {
                    MsgWin(NULL, NULL, 0, STR_MMS_ATTEXIST, STR_MMS_MMS, Notify_Info,
                        STR_MMS_OK, NULL,MMS_WAITTIME);
                    return NULL;
                }
                else
                    bRepeatname = TRUE;
            }
        }
        p = p->pnext;
    }

    hAttatch = MMS_CreateFile(szFileName, O_RDONLY);
    if (hAttatch == -1)
        return NULL;

    p = (mmsMetaNode *)MMS_malloc(sizeof(mmsMetaNode));
    if (bSmil)
    {
        p->inSlide = pCreateData->curSlide;
        p->refCount = 1;
    }
    else
    {
        p->inSlide = -1;
        p->refCount = 0;
    }        
    
    p->txtcharset = 0;
    p->Content_Id = NULL;
    p->Content_Type = NULL;
    p->Metalen = MMS_GetFileSize(szFileName);
    
    if (nAttatchType == RTN_TEXT || nAttatchType == MMS_CALLEDIT_VCARD
		|| nAttatchType == MMS_CALLEDIT_VCAL)
        nFilesize = p->Metalen*TXT_UTF8_MUL;
    else
        nFilesize = p->Metalen;

    if ((pCreateData->tmms.mms.wspHead.nMmsSize + nFilesize) > MAX_MMS_LEN)
    {
        MsgWin(NULL, NULL, 0, STR_MMS_OVERSIZE, STR_MMS_MMS, Notify_Alert, 
            STR_MMS_OK, NULL,MMS_WAITTIME);
        MMS_free(p);
        MMS_CloseFile(hAttatch);
		SaveCurSlideTxt(hwnd);
		CallMMSObjList(pCreateData->hWndFrame, hwnd, pCreateData->tmms.pAttatch, EDIT_CALLOBJ);
        return NULL;
    }

    p->Metadata = MMS_malloc(p->Metalen + 1);
    // 分配内存失败
    if (p->Metadata == NULL)
    {
        MMS_free(p);
        MMS_CloseFile(hAttatch);
        return NULL;
    }
    read(hAttatch, p->Metadata, p->Metalen);
    p->Metadata[p->Metalen] = 0;

    p->Content_Location = MMS_malloc(strlen(szFileName) + 16);
    if (bRepeatname)
    {
        sprintf(p->Content_Location, "att%02d%s", num, szFileName);
        num++;
        if (num == 100)
            num = 0;
    }
    else
        strcpy(p->Content_Location, szFileName);

	strcpy(szTemp, p->Content_Location);
	len = MultiByteToUTF8(CP_ACP, 0, szTemp, -1, p->Content_Location, 512);
	p->Content_Location[len] = 0;
	
    switch(nAttatchType) 
    {
    case RTN_IMAGE:        
        pCreateData->tmms.mms.wspHead.nMmsSize += p->Metalen;
        p->MetaType = GetImageType(hAttatch) + 30;
        
        break;
    case RTN_SOUND:
        pCreateData->tmms.mms.wspHead.nMmsSize += p->Metalen;
        p->MetaType = GetSoundType(hAttatch, szFileName) + 20;
        break;
    case RTN_TEXT:
	case MMS_CALLEDIT_VCARD:
	case MMS_CALLEDIT_VCAL:
        pCreateData->tmms.mms.wspHead.nMmsSize += p->Metalen*TXT_UTF8_MUL;
        p->MetaType = GetTextType(szFileName) + 50;
        p->txtcharset = CHARSET_UTF8;
        break;
    default:
        break;
    }

	MMS_SetIndicator(hwnd, pCreateData->curSlide, 
						pCreateData->tmms.mms.wspHead.nSlidnum, 
						pCreateData->tmms.mms.wspHead.nMmsSize);

    MMS_CloseFile(hAttatch);
    
    if(pCreateData->tmms.pAttatch == NULL)
        pCreateData->tmms.pAttatch = p;
    else
        pCreateData->pEnd->pnext = p;    
    pCreateData->pEnd = p;   
    if (pCreateData->pEnd != NULL)
        pCreateData->pEnd->pnext = NULL;

    return p;
}
/*********************************************************************\
* Function     DeleteAttatch
* Purpose      
* Params       
* Return           
* Remarks      
**********************************************************************/
// 删除其中的一个附件
static BOOL DeleteAttatch(PMMS_EDITCREATEDATA pCreateData, mmsMetaNode *pDel)
{
    mmsMetaNode *p = NULL;
    mmsMetaNode *pPrenode = NULL;

    if (pDel == NULL)
        return FALSE;

    pPrenode = pCreateData->tmms.pAttatch;
    p = pPrenode;

    while (p)
    {
        if (p == pDel)
        {
            break;
        }
        pPrenode = p;
        p = p->pnext;
    }
    if (p == NULL)
        return FALSE;

    if (p->inSlide >= 0)
    {
        p->refCount--;
        if (p->refCount > 0)
            return TRUE;
    }
    if (pDel == pCreateData->tmms.pAttatch)  // 删除头指针
        pCreateData->tmms.pAttatch = pDel->pnext;
    else
        pPrenode->pnext = pDel->pnext;

    if (pDel->MetaType/10 == META_TEXT)
        pCreateData->tmms.mms.wspHead.nMmsSize -= pDel->Metalen*TXT_UTF8_MUL;
    else
        pCreateData->tmms.mms.wspHead.nMmsSize -= pDel->Metalen;

    if (pDel->pnext == NULL)
        pCreateData->pEnd = pPrenode;
    MMS_free(pDel->Content_Id);
    MMS_free(pDel->Content_Location);
    MMS_free(pDel->Content_Type);
    MMS_free(pDel->Metadata);       
    MMS_free(pDel);
    pDel = NULL;
	
    return TRUE;
}
/*********************************************************************\
* Function     DeleteAttatchByIndex
* Purpose      
* Params       
* Return           
* Remarks      only call from objlist
**********************************************************************/
/*BOOL DeleteAttatchByIndex(HWND hwnd, int index)
{
    mmsMetaNode *pNode = NULL;
    mmsMetaNode *pPre = NULL;
    int         i = 0;
    HWND        hwndContent = NULL;
	HWND        hWndEdit = NULL;
    PMMS_EDITCREATEDATA pCreateData;
	char classname[32];
		
	classname[0] = 0; 	
	hWndEdit = hwnd;
	do 
	{
		hWndEdit = GetWindow(hWndEdit, GW_HWNDNEXT);
		GetClassName(hWndEdit, classname, 32);
		
		if(strcmp(classname, MMSWindowClass[5].szClassName) == 0)
		{
			break;
		}
		
	} while(hWndEdit);
	
    pCreateData = GetUserData(hWndEdit);
    pPre = pCreateData->tmms.pAttatch;
    pNode  = pPre;
    
    while (pNode)
    {        
        if (i == index)
            break;
        i++;
        pPre = pNode;
        pNode = pNode->pnext;
    }
    if (i != index)
        return FALSE;

    if (pNode->inSlide >= 0)
    {
        if (pNode->MetaType/10 == META_IMG)
		{
			pCreateData->delSlide = pNode->inSlide;
			PLXConfirmWinEx(pCreateData->hWndFrame, hwnd, STR_MMS_REMOVECONFIRM, Notify_Request,
				STR_MMS_MMS, STR_MMS_YES, STR_MMS_NO, MMS_REMOVE_IMG);
		}
        else if (pNode->MetaType/10 == META_AUDIO)
		{
			pCreateData->delSlide = pNode->inSlide;
			PLXConfirmWinEx(pCreateData->hWndFrame, hwnd, STR_MMS_REMOVECONFIRM, Notify_Request,
				STR_MMS_MMS, STR_MMS_YES, STR_MMS_NO, MMS_REMOVE_AUDIO);
		}
        else if (pNode->MetaType/10 == META_TEXT)
        {
			pCreateData->delSlide = pNode->inSlide;
			PLXConfirmWinEx(pCreateData->hWndFrame, hwnd, STR_MMS_REMOVECONFIRM, Notify_Request,
				STR_MMS_MMS, STR_MMS_YES, STR_MMS_NO, MMS_REMOVE_TEXT);
        }

        pNode->refCount--;
    }
    return TRUE;
}*/

BOOL DeleteAttatchByIndex(HWND hwnd, mmsMetaNode *pNode)
{
    int         i = 0;
    HWND        hwndContent = NULL;
	HWND        hWndEdit = NULL;
    PMMS_EDITCREATEDATA pCreateData;
	char classname[32];

	if (pNode == NULL)
		return FALSE;
	classname[0] = 0; 	
	hWndEdit = hwnd;
	do 
	{
		hWndEdit = GetWindow(hWndEdit, GW_HWNDNEXT);
		GetClassName(hWndEdit, classname, 32);
		
		if(strcmp(classname, MMSWindowClass[5].szClassName) == 0)
		{
			break;
		}
		
	} while(hWndEdit);
	
    pCreateData = GetUserData(hWndEdit);
    
    if (pNode->inSlide >= 0)
    {
        if (pNode->MetaType/10 == META_IMG)
		{
			pCreateData->delSlide = pNode->inSlide;
			PLXConfirmWinEx(pCreateData->hWndFrame, hwnd, STR_MMS_REMOVECONFIRM, Notify_Request,
				STR_MMS_MMS, STR_MMS_YES, STR_MMS_NO, MMS_REMOVE_IMG);
		}
        else if (pNode->MetaType/10 == META_AUDIO)
		{
			pCreateData->delSlide = pNode->inSlide;
			PLXConfirmWinEx(pCreateData->hWndFrame, hwnd, STR_MMS_REMOVECONFIRM, Notify_Request,
				STR_MMS_MMS, STR_MMS_YES, STR_MMS_NO, MMS_REMOVE_AUDIO);
		}
        else if (pNode->MetaType/10 == META_TEXT)
        {
			pCreateData->delSlide = pNode->inSlide;
			PLXConfirmWinEx(pCreateData->hWndFrame, hwnd, STR_MMS_REMOVECONFIRM, Notify_Request,
				STR_MMS_MMS, STR_MMS_YES, STR_MMS_NO, MMS_REMOVE_TEXT);
        }

        pNode->refCount--;
    }
	else
	{
		pCreateData->pDel = pNode;
		PLXConfirmWinEx(pCreateData->hWndFrame, hwnd, STR_MMS_REMOVECONFIRM, Notify_Request,
				STR_MMS_MMS, STR_MMS_YES, STR_MMS_NO, MMS_REMOVE_OBJ);
	}
    return TRUE;
}

/*********************************************************************\
* Function     AddNameofObject
* Purpose      
* Params       
* Return           
* Remarks      
**********************************************************************/
static void AddNameofObject(PSLIDE pcurSlide, int i)
{ 
    char filename[MAX_FILENAME];

    filename[0] = 0;

    // image
    if (pcurSlide->pImage &&
        pcurSlide->pImage->Metalen > 0)
    {
        // content_location == NULL
        if (pcurSlide->pImage->Content_Location == NULL)
        {
            sprintf(filename, "pollex_att%d%s", i, 
                imgfix[pcurSlide->pImage->MetaType%10]);
            pcurSlide->pImage->Content_Location = My_StrDup(filename);
        }
        // !NULL
        else
        {    
            // no name
            if (pcurSlide->pImage->Content_Location[0] == 0)
            {
                sprintf(filename, "pollex_att%d%s", i, 
                    imgfix[pcurSlide->pImage->MetaType%10]);
                MMS_free(pcurSlide->pImage->Content_Location);
                pcurSlide->pImage->Content_Location = My_StrDup(filename);
            }//end noname
        }// end !NULL
    }//end image
    
    filename[0] = 0;
    // audio
    if (pcurSlide->pAudio &&
        pcurSlide->pAudio->Metalen > 0)
    {
        // content_location == NULL
        if (pcurSlide->pAudio->Content_Location == NULL)
        {
            sprintf(filename, "pollex_att%d%s", i, 
                audiofix[pcurSlide->pAudio->MetaType%10]);
            pcurSlide->pAudio->Content_Location = My_StrDup(filename);
        }
        // !NULL
        else
        {    
            // no name
            if (pcurSlide->pAudio->Content_Location[0] == 0)
            {
                sprintf(filename, "pollex_att%d%s", i, 
                    audiofix[pcurSlide->pAudio->MetaType%10]);
                MMS_free(pcurSlide->pAudio->Content_Location);
                pcurSlide->pAudio->Content_Location = My_StrDup(filename);
            }//end noname
        }// end !NULL
    }//end audio

    filename[0] = 0;
    // text
    if (pcurSlide->pText &&
        pcurSlide->pText->Metalen > 0)
    {
        // content_location == NULL
        if (pcurSlide->pText->Content_Location == NULL)
        {
            sprintf(filename, "pollex_att%d%s", i, 
                txtfix[pcurSlide->pText->MetaType%10]);
            pcurSlide->pText->Content_Location = My_StrDup(filename);
        }
        // !NULL
        else
        {    
            // no name
            if (pcurSlide->pText->Content_Location[0] == 0)
            {
                sprintf(filename, "pollex_att%d%s", i, 
                    txtfix[pcurSlide->pText->MetaType%10]);
                MMS_free(pcurSlide->pText->Content_Location);
                pcurSlide->pText->Content_Location = My_StrDup(filename);
            }//end noname
        }// end !NULL
    }//end txt

}
/*********************************************************************\
* Function     MMS_ReplyAll
* Purpose      
* Params       
* Return           
* Remarks      
**********************************************************************/
static void MMS_ReplyAll(PMMS_EDITCREATEDATA pCreateData, int hMMSFile)
{
    char    *addTokens;
    char    newTo[SIZE_6];
    char    subject[SIZE_1];
    int     subLen;
    int     nFit;
    int     tolen = 0;
            
    lseek(hMMSFile, MMS_HEADER_SIZE + mms_offsetof(WSPHead, from), SEEK_SET);
    read(hMMSFile, &pCreateData->tmms.mms.wspHead.from, SIZE_1);
    read(hMMSFile, &pCreateData->tmms.mms.wspHead.to, SIZE_1);
    lseek(hMMSFile, MMS_HEADER_SIZE + mms_offsetof(WSPHead, subject) , SEEK_SET);
    read(hMMSFile,&pCreateData->tmms.mms.wspHead.subject, SIZE_1);
    
    subLen = strlen(pCreateData->tmms.mms.wspHead.subject);
    if ( subLen > (SIZE_SUBJECT - 3))
    {
        GetTextExtentExPoint(NULL, pCreateData->tmms.mms.wspHead.subject, subLen, 
            (SIZE_SUBJECT - 3) * ENG_FONT_WIDTH, &nFit, NULL, NULL);
        
        pCreateData->tmms.mms.wspHead.subject[nFit] = '\0';
    }
    sprintf(subject, "Re:%s", pCreateData->tmms.mms.wspHead.subject);
    strcpy(pCreateData->tmms.mms.wspHead.subject, subject);
    
    //去掉 to中与from重复的号码    
    addTokens = MMS_chrtok(pCreateData->tmms.mms.wspHead.to, MMS_ASEPCHR, MMS_ASEPCHRF);
    
    newTo[0] = '\0';
    
    while(addTokens != NULL)
    {
        if (*addTokens == '\0')
        {
            addTokens = MMS_chrtok(NULL, MMS_ASEPCHR, MMS_ASEPCHRF);
            continue;
        }
        if (strcmp(addTokens, pCreateData->tmms.mms.wspHead.from) != 0)
        {
            if(tolen + strlen(addTokens) + 1 <= SIZE_6 - 1) 
            {
                strcat(newTo, MMS_ASEPSTR);
                strcat(newTo, addTokens);
                tolen = tolen +  strlen(addTokens) + 1; 
            }
            else
            {
                MsgWin(NULL, NULL, 0, STR_MMS_OVERSIZE, STR_MMS_MMS, Notify_Alert,
                    STR_MMS_OK,NULL,WAITTIMEOUT);
                break;
            }
            
        }
        addTokens = MMS_chrtok(NULL, MMS_ASEPCHR, MMS_ASEPCHRF);
    }

    pCreateData->tmms.mms.wspHead.to[0] = 0;   
    sprintf(pCreateData->tmms.mms.wspHead.to, "%s%s", pCreateData->tmms.mms.wspHead.from, newTo);
    pCreateData->tmms.mms.wspHead.from[0] = 0;

    return ;
}
/*********************************************************************\
* Function     GetAttatchEnd
* Purpose      
* Params       
* Return           
* Remarks      
**********************************************************************/
static void GetAttacthEnd(PMMS_EDITCREATEDATA pCreateData)
{
    mmsMetaNode* p;

    p = pCreateData->tmms.pAttatch;
    
    while(p->pnext)         
        p = p->pnext;   
    pCreateData->pEnd = p;
}
/*********************************************************************\
* Function     GetTextType
* Purpose      
* Params       
* Return           
* Remarks      
**********************************************************************/
static int GetTextType(PCSTR pFileName)
{
    char sufix[SIZE_3];
    
    GetFileNameSuffix((const PSTR)pFileName, sufix);
    if (stricmp(sufix, ".vcf") == 0)
        return TEXT_VCARD;
    if (stricmp(sufix, ".vcs") == 0)
        return TEXT_VCALE;
    else
        return TEXT_PLAIN;
}
/*********************************************************************\
* Function     GetSoundType
* Purpose      
* Params       
* Return           
* Remarks      
**********************************************************************/
static int GetSoundType(int hFile, PCSTR pFileName)
{
    unsigned char   strAudioType[6];

    if (-1 == hFile)
        return AUDIO_UNKNOWN;
    
    lseek(hFile, 0, SEEK_SET);
    read(hFile, strAudioType, 5);
    
    if ((strAudioType[0] == 0x23) && (strAudioType[1] == 0x21) && 
        (strAudioType[2] == 'A') && (strAudioType[3] == 'M') &&
        (strAudioType[4] == 'R'))
        return AUDIO_AMR;
    else if ((strAudioType[0] == 'M') && (strAudioType[1] == 'T') && 
        (strAudioType[2] == 'h') && (strAudioType[3] == 'd'))
        return AUDIO_MIDI;
    else if ((strAudioType[0] == 'M') && (strAudioType[1] == 'M') &&
        (strAudioType[2] == 'M') && (strAudioType[3] == 'D'))
        return AUDIO_MMF;
    else if ((strAudioType[0] == 'R') && (strAudioType[1] == 'I') && 
        (strAudioType[2] == 'F') && (strAudioType[3] == 'F'))
        return AUDIO_WAV;
    else
        return AUDIO_UNKNOWN;
}
/*********************************************************************\
* Function     GetImageType
* Purpose      
* Params       
* Return           
* Remarks      
**********************************************************************/
static int  GetImageType(int hFile)
{
    unsigned char   strType[4];

   if (-1 == hFile)
       return IMG_UNKNOWN;

    lseek(hFile, 0, SEEK_SET);
    read(hFile, strType, 3);
    
    if ((strType[0] == 'B') && (strType[1] == 'M'))
        return IMG_BMP;
    else if ((strType[0] == 'G') && (strType[1] == 'I') && (strType[2] == 'F'))
        return IMG_GIF;
    else if ((strType[0] == 0xff) && (strType[1] == 0xd8))
        return IMG_JPEG;
    else if ((strType[0] == 0x00) && (strType[1] == 0x00))
        return IMG_WBMP;
	else if ((strType[0] == 0x89) && (strType[1] == 0x50) && 
        (strType[2] == 0x4e) && (strType[3] == 0x47))
		return IMG_PNG;
    else
        return IMG_UNKNOWN;
}
/*********************************************************************\
* Function     IsMMSNull
* Purpose      
* Params       
* Return           
* Remarks      
**********************************************************************/
static BOOL IsMMSNull(const TotalMMS* ptmms)
{

    int tempLen;

//    if(bAutoSign && !bNoSign)
//    {
//        //有签名档
//        tempLen = ptmms->mms.wspHead.nMmsSize - GetSignSize();
//    }
//     else
    tempLen = ptmms->mms.wspHead.nMmsSize;

    if((ptmms->mms.wspHead.to[0] == 0)&&(ptmms->mms.wspHead.subject[0] == 0)
        &&(tempLen == 0))
        return FALSE;
    else
        return TRUE;
}

static void MMS_RecipientGetText(HWND hEdt,PSTR pszText, int nMaxCount)
{
    RECIPIENTLISTBUF Recipient;

    PRECIPIENTLISTNODE pTemp = NULL;

    int i;

    memset(&Recipient,0,sizeof(RECIPIENTLISTBUF));

    SendMessage(hEdt,GHP_GETREC,0,(LPARAM)&Recipient);

    pTemp = Recipient.pDataHead;

    pszText[0] = 0;

    for(i = 0; i < Recipient.nDataNum ; i++)
    {
        strcat(pszText,pTemp->szPhoneNum);

        strcat(pszText,";");

        pTemp = pTemp->pNext;
    }

    return ;
}

static int MMS_RecipientGetTextLength(HWND hEdt)
{
    
    RECIPIENTLISTBUF Recipient;

    PRECIPIENTLISTNODE pTemp = NULL;

    int nLen,i;

    memset(&Recipient,0,sizeof(RECIPIENTLISTBUF));

    SendMessage(hEdt,GHP_GETREC,0,(LPARAM)&Recipient);

    pTemp = Recipient.pDataHead;

    nLen = 0;

    for(i = 0; i < Recipient.nDataNum ; i++)
    {
        nLen += strlen(pTemp->szPhoneNum) + 1;

        pTemp = pTemp->pNext;
    }

    return nLen;
}
/*********************************************************************\
* Function     EditStopRing
* Purpose      
* Params       
* Return           
* Remarks      
**********************************************************************/
/*void EditStopRing(BYTE bRingIsEnd)
{
    PMMS_EDITCREATEDATA pCreateData;
    
    pCreateData = GetUserData(hwndMMSEdit);
    pCreateData->bPlayMusic = FALSE;
}*/

static void  GetMmsTotalSize(TotalMMS *ptmms)
{
    int i = 0;
    mmsMetaNode *p = NULL;

    ptmms->mms.wspHead.nMmsSize = 0;

    for(i = 0; i < ptmms->mms.wspHead.nSlidnum; i++)
    {
        if(ptmms->mms.slide[i].pText && ptmms->mms.slide[i].pText->Metalen > 0)
            ptmms->mms.wspHead.nMmsSize += ptmms->mms.slide[i].pText->Metalen * TXT_UTF8_MUL;
        if(ptmms->mms.slide[i].pAudio && ptmms->mms.slide[i].pAudio->Metalen > 0)
            ptmms->mms.wspHead.nMmsSize += ptmms->mms.slide[i].pAudio->Metalen;
        if(ptmms->mms.slide[i].pImage && ptmms->mms.slide[i].pImage->Metalen >0)
            ptmms->mms.wspHead.nMmsSize += ptmms->mms.slide[i].pImage->Metalen;
    }

    p = ptmms->pAttatch;
    while(p)
    {          
        if(p->inSlide == -1 && p->Metalen >0)
        {
            ptmms->mms.wspHead.nMmsSize += p->Metalen;
        }
        p = p->pnext;
    }
}
/*
static void GetFileSize(TotalMMS *ptmms, float *nSize)
{
    int filesize = 0;
    
    GetMmsTotalSize(ptmms);
    
    if(ptmms->mms.wspHead.nMmsSize > 1)
    {
        filesize = (int)ptmms->mms.wspHead.nMmsSize;//Nkb
        nSize = &(float)filesize;
    }
    else
        nSize = &(float)ptmms->mms.wspHead.nMmsSize;//<0.N>kb
}*/

void MMS_CaretProc(const RECT* rc)
{
#define  CLASS_NAMELEN          100
   HWND					hFocus,hParent,hEdt;
   char					pszClassName[CLASS_NAMELEN];
   PMMS_EDITCREATEDATA	pCreateData;
   RECT					rcClient;
   BOOL					bSpecial;
   int					nLine;
   SCROLLINFO			vsi;
   SIZE					Size;
   HDC					hdc;
   int					nY = 0, nAllMax = 0;

   bSpecial = FALSE;
   pszClassName[0] = 0;
   
   hFocus = GetFocus();
   hParent = hFocus;//GetParent(hFocus);
   GetClassName(hParent,pszClassName,CLASS_NAMELEN);

   if(stricmp(pszClassName,WC_RECIPIENT)==0)
       bSpecial = TRUE;

   while(stricmp(pszClassName,MMSWindowClass[5].szClassName)!=0)
   {
       hFocus = hParent;
       hParent = GetParent(hFocus);
	   if (hParent == NULL)
		   return;
       GetClassName(hParent,pszClassName,CLASS_NAMELEN);
   }

   hEdt = GetDlgItem(hParent,ID_CONTENT);
   pCreateData = GetUserData(hParent);

   hdc = GetDC(hParent);
   GetTextExtent(hdc, "L", 1, &Size);
   ReleaseDC(hParent,hdc);

   GetWindowRectEx(hParent,&rcClient,W_CLIENT,XY_SCREEN);

   if(rcClient.bottom < rc->bottom) //down
   {   
       nY = Size.cy + MMS_EDIT_SPACE;
       
       memset(&vsi, 0, sizeof(SCROLLINFO));
       vsi.fMask  = SIF_POS;
       GetScrollInfo(hParent, SB_VERT, &vsi);
       
       vsi.nPos++;
       vsi.fMask  = SIF_POS ;
       SetScrollInfo(hParent, SB_VERT, &vsi, TRUE);

       ScrollWindow(hParent,0,-nY,NULL,NULL);
	   pCreateData->tmms.mms.slide[pCreateData->curSlide].imgPoint.y -= nY;
       UpdateWindow(hParent);
   }
   else if(rcClient.top >= rc->top) //up
   {
       if(bSpecial == TRUE)
       {      
           RECT rect,rcSys;

           GetWindowRect(hFocus,&rect);
           GetWindowRectEx(hParent,&rcSys,W_CLIENT,XY_SCREEN);
           nY = rcSys.top - rect.top;
           
           memset(&vsi, 0, sizeof(SCROLLINFO));
           vsi.fMask  = SIF_POS ;
           GetScrollInfo(hParent, SB_VERT, &vsi);   
           vsi.nPos--;
           vsi.fMask  = SIF_POS ;
           SetScrollInfo(hParent, SB_VERT, &vsi, TRUE);        
           ScrollWindow(hParent,0,nY,NULL,NULL);
		   pCreateData->tmms.mms.slide[pCreateData->curSlide].imgPoint.y += nY;
           UpdateWindow(hParent);
       }
       else
       {   
           nY = Size.cy + MMS_EDIT_SPACE;
           
           memset(&vsi, 0, sizeof(SCROLLINFO));
           vsi.fMask  = SIF_POS ;
           GetScrollInfo(hParent, SB_VERT, &vsi); 
           vsi.nPos--;
           vsi.fMask  = SIF_POS ;
           SetScrollInfo(hParent, SB_VERT, &vsi, TRUE);          
           ScrollWindow(hParent,0,nY,NULL,NULL);
		   pCreateData->tmms.mms.slide[pCreateData->curSlide].imgPoint.y += nY;
           UpdateWindow(hParent);
       }
   }
   
   if(bSpecial == TRUE)
   {      
       memset(&vsi, 0, sizeof(SCROLLINFO));
       vsi.fMask  = SIF_POS ;
       GetScrollInfo(hParent, SB_VERT, &vsi);
       if(vsi.nPos != 0)
       {
           RECT rect,rcSys;
           
           GetWindowRect(hFocus,&rect);
           GetWindowRectEx(hParent,&rcSys,W_CLIENT,XY_SCREEN);
           nY = rcSys.top - rect.top;           
           vsi.nPos = 0;
           vsi.fMask  = SIF_POS ;
           SetScrollInfo(hParent, SB_VERT, &vsi, TRUE);        
           ScrollWindow(hParent,0,nY,NULL,NULL);
		   pCreateData->tmms.mms.slide[pCreateData->curSlide].imgPoint.y += nY;
           UpdateWindow(hParent);
       }
   }

   memset(&vsi, 0, sizeof(SCROLLINFO));
   vsi.fMask  = SIF_ALL ;
   GetScrollInfo(hParent, SB_VERT, &vsi);
   
   nLine = SendMessage(hEdt, EM_GETLINECOUNT, 0, 0);
   if (pCreateData->curSlide == 0)
	   nLine ++;

   nAllMax = 
	   nLine + pCreateData->tmms.mms.slide[pCreateData->curSlide].imgSize.cy/(Size.cy + MMS_EDIT_SPACE + 1);
   if(nAllMax != vsi.nMax)
   {
       vsi.nMax = nAllMax - 1;
       vsi.fMask  = SIF_RANGE;
       SetScrollInfo(hParent, SB_VERT, &vsi, TRUE); 
   }   
}

// judge wether this object is supported types
static BOOL JudgeSupportTypeFromName(PCSTR filename)
{
    unsigned char   strType[17];
	int             handle;

	handle = open(filename, O_RDONLY, 0);

   if (-1 == handle)
       return FALSE;

    lseek(handle, 0, SEEK_SET);
    read(handle, strType, 16);
    close(handle);

    // gif
    if ((strType[0] == 'G') && (strType[1] == 'I') && (strType[2] == 'F'))
        return TRUE;
    // jpeg
    if ((strType[0] == 0xff) && (strType[1] == 0xd8))
        return TRUE;

	if((strType[0] == 'B') && (strType[1] == 'M'))
		return TRUE;
	
    // wbmp
    if ((strType[0] == 0x00) && (strType[1] == 0x00))
        return TRUE;

    // png
    if ((strType[0] == 0x89) && (strType[1] == 0x50) && 
        (strType[2] == 0x4e) && (strType[3] == 0x47))
        return TRUE;

    // amr
    if ((strType[0] == 0x23) && (strType[1] == 0x21) && 
        (strType[2] == 'A') && (strType[3] == 'M') &&
        (strType[4] == 'R'))
        return TRUE;

	//wav
	if((strType[0] == 'R') && (strType[1] == 'I') 
		&& (strType[2] == 'F') && (strType[3] == 'F'))
		return TRUE;
	
	if (strnicmp(strType, "BEGIN:VCALENDAR", 15) == 0)
		return TRUE;

	if (strnicmp(strType, "BEGIN:VCARD", 11) == 0)
		return TRUE;

    return FALSE;
}

static BOOL JudgeSupportType(int handle)
{
    unsigned char   strType[17];

   if (-1 == handle)
       return FALSE;

    lseek(handle, 0, SEEK_SET);
    read(handle, strType, 16);
    
    // gif
    if ((strType[0] == 'G') && (strType[1] == 'I') && (strType[2] == 'F'))
        return TRUE;
    // jpeg
    if ((strType[0] == 0xff) && (strType[1] == 0xd8))
        return TRUE;

	if((strType[0] == 'B') && (strType[1] == 'M'))
		return TRUE;

    // wbmp
    if ((strType[0] == 0x00) && (strType[1] == 0x00))
        return TRUE;

    // png
    if ((strType[0] == 0x89) && (strType[1] == 0x50) && 
        (strType[2] == 0x4e) && (strType[3] == 0x47))
        return TRUE;

    // amr
    if ((strType[0] == 0x23) && (strType[1] == 0x21) && 
        (strType[2] == 'A') && (strType[3] == 'M') &&
        (strType[4] == 'R'))
        return TRUE;
	//wav
	if((strType[0] == 'R') && (strType[1] == 'I') 
		&& (strType[2] == 'F') && (strType[3] == 'F'))
		return TRUE;
	
	if (strnicmp(strType, "BEGIN:VCALENDAR", 15) == 0)
		return TRUE;

	if (strnicmp(strType, "BEGIN:VCARD", 11) == 0)
		return TRUE;
	
    return FALSE;
}
/*
1、	create a MemoryDC
2、	create a Bitmap
3、	select Bitmap to MermoryDC
4、	DrawText on MemoryDC 
5、	SETAPPICON
*/
static void MMS_SetIndicator(HWND hWnd, int curSlide, int nAllSlide, int nMmsSize)
{
	HDC			hdc = NULL;
	HBITMAP		hBitmap = NULL;
	char        msg[16], size[16];
	PMMS_EDITCREATEDATA  pEditData;
    HFONT       hFont = NULL;
	RECT        rect;
	SIZE        sz1, sz2;
	int         nMode = 0;

	pEditData = GetUserData(hWnd);
	
	hdc = GetDC(hWnd);

	if (pEditData->hDCLeftIcon)
	{
		DeleteDC(pEditData->hDCLeftIcon);
		pEditData->hDCLeftIcon = NULL;
	}

	pEditData->hDCLeftIcon = CreateCompatibleDC(hdc);

	GetFontHandle(&hFont,SMALL_FONT);
    SelectObject(pEditData->hDCLeftIcon,hFont);

    sprintf(msg, "%d/%d", curSlide+1, nAllSlide);
	if (nMmsSize >= 1024)
		sprintf(size, "%dkB", nMmsSize/1024);
	else
		sprintf(size, "0.%dkB", nMmsSize*10/1024);

	GetTextExtentPoint(pEditData->hDCLeftIcon, msg, -1, &sz1);
	GetTextExtentPoint(pEditData->hDCLeftIcon, size, -1, &sz2);

	SetRect(&rect, 0, 0, max(sz1.cx, sz2.cx), sz1.cy + sz2.cy - 4);
	
	if (pEditData->hBmpLeftIcon)
	{
		DeleteObject(pEditData->hBmpLeftIcon);
		pEditData->hBmpLeftIcon = NULL;
	}

    pEditData->hBmpLeftIcon = CreateCompatibleBitmap(pEditData->hDCLeftIcon, 
		rect.right, rect.bottom);

	if (pEditData->hBmpLeftIcon == NULL)
	{
		ReleaseDC(hWnd, hdc);
		return;
	}
    SelectObject(pEditData->hDCLeftIcon, pEditData->hBmpLeftIcon);

	ClearRect(pEditData->hDCLeftIcon, &rect, COLOR_TRANSBK);

	nMode = SetBkMode(pEditData->hDCLeftIcon, TRANSPARENT);
	TextOut(pEditData->hDCLeftIcon, 0, 0, msg, -1);
	TextOut(pEditData->hDCLeftIcon, 0, sz1.cy - 4, size, -1);
	SetBkMode(pEditData->hDCLeftIcon, nMode);

	SendMessage(pEditData->hWndFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON), 
		(LPARAM)pEditData->hBmpLeftIcon);

	ReleaseDC(hWnd, hdc);
}

static void MMSEdit_OnSetLBtnText(HWND hWnd, int nID, BOOL bEmpty, LPSTR pszText)
{
	HWND hwndContent;
	HWND hwndPhoneNum ;
	int nRecipientLen;
	PMMS_EDITCREATEDATA pCreateData;

	pCreateData = GetUserData(hWnd);
	hwndPhoneNum = GetDlgItem(hWnd, ID_PHONENUM);
	hwndContent = GetDlgItem(hWnd, ID_CONTENT);
	
	nRecipientLen = GetWindowTextLength(hwndPhoneNum);

	switch(nID)
	{
	case ID_CONTENT:
		{
			if(GetFocus() == hwndContent)
			{
				if(bEmpty)
				{
					if(nRecipientLen != 0)
						SendMessage(pCreateData->hWndFrame, PWM_SETBUTTONTEXT,
						1, (LPARAM)STR_MMS_SEND);

					if(nRecipientLen == 0)
						SendMessage(pCreateData->hWndFrame, PWM_SETBUTTONTEXT,
						1, (LPARAM)"");
				}
				else
				{
					if(nRecipientLen != 0)
						SendMessage(pCreateData->hWndFrame, PWM_SETBUTTONTEXT,
						1, (LPARAM)STR_MMS_SEND);
					
					if(nRecipientLen == 0)
						SendMessage(pCreateData->hWndFrame, PWM_SETBUTTONTEXT,
						1, (LPARAM)STR_MMS_SAVE);
				}
			}
		}
		break;

	case ID_PHONENUM:
		SendMessage(pCreateData->hWndFrame, PWM_SETBUTTONTEXT,
			1, (LPARAM)STR_MMS_ADD);
		break;

	default:
		SendMessage(pCreateData->hWndFrame, PWM_SETBUTTONTEXT, 1,
            (LPARAM)pszText);
		break;
	}

	return;
}
// set sound object indicator
static void MMS_SetSoundInd(HWND hWnd, BOOL bShow)
{
	HWND		hWndFrame;
	PMMS_EDITCREATEDATA  pData;
	
	pData = GetUserData(hWnd);
	hWndFrame = pData->hWndFrame;

	if (bShow)
		SendMessage(hWndFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, RIGHTICON), 
			(LPARAM)SOUND_NAME);
	else
	{
		if (pData->iconorbmp == IMAGE_BITMAP && pData->handle)
			SendMessage(hWndFrame, PWM_SETAPPICON, MAKEWPARAM(pData->iconorbmp, RIGHTICON), 
				(LPARAM)pData->handle);
		else if (pData->iconorbmp == IMAGE_ICON && pData->szIconName[0])
			SendMessage(hWndFrame, PWM_SETAPPICON, MAKEWPARAM(pData->iconorbmp, RIGHTICON), 
				(LPARAM)pData->szIconName);
	}
}

static BOOL InsertMultiImage(HWND hWnd, void* pImage)
{
	PLISTATTNODE pList;
	PLISTATTNODE pTemp;
	PMMS_EDITCREATEDATA pCreateData;
	HWND    hwndContent = NULL;
	int i = 0;

	pList = pImage;
	pCreateData = GetUserData(hWnd);
	hwndContent = GetDlgItem(hWnd, ID_CONTENT);
	
	if(pList == NULL)
		return FALSE;

	pTemp = pList;

	while(pTemp)
	{
		if(IsObjectCanInsert(pTemp->AttPath, MMS_CALLEDIT_IMAGE))
		{
			if(i >= 1)
			{
				pCreateData->hFocus = hwndContent;
				SetFocus(hwndContent);
			}
			SendMessage(hWnd, MMS_EDIT_INSERT, MMS_CALLEDIT_IMAGE, 
            (LPARAM)pTemp->AttPath);

			i++;
		}

		pTemp = pTemp->pNext;
		
	}

	return TRUE;
}
