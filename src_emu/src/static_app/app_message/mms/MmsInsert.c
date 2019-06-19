/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : MMS
 *
 * Purpose  : implements the insert function
 *            
\**************************************************************************/

#include "MmsGlobal.h"
#include "PreBrow.h"
#include "MmsInsert.h"
#include "MmsInterface.h"
/*********************************************************************\
*
*           macro definition
*      
**********************************************************************/
#define  WM_MMS_INSERT   WM_USER + 501

#define  IDC_INSERT_LST  501
#define  IDM_INSERT_OK   502
#define  IDB_EXIT        503
#define  LIST_ICON_NAME  "/rom/message/unibox/mms/menu_draft.gif"
#define  IDS_INSERT_EXIT "Back"
#define  MAX_INSERT_NUM  4//6
#define  IDS_MMS_ATTATCH STR_MMS_ATTATCH
#define  IDS_MMS_CAMERO  "Active camera"
/*********************************************************************\
*
*           structure
*      
**********************************************************************/
typedef struct tagINSERTINDEX 
{
    int indexImg;
    int indexSound;
    int indexTxt;
 //   int indexAtt;
    int indexSlide;
 //   int indexCamero;
} INSERTINDEX;
/*********************************************************************\
*
*           global variables
*      
**********************************************************************/
static HWND         hWndInsertLst = NULL;
static HBITMAP      hBitmapLst = NULL;
static MMSMSGINFO   InsertMsgInfo;
static INSERTINFO   infoEdit;
static INSERTINDEX  inIndex;        
/*********************************************************************\
*
*           function declares
*      
**********************************************************************/
static LRESULT InsertBrowWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static BOOL OnCreateInsert(HWND hWnd);
static void OnDblclkLst(HWND hWnd);
// insert object window
void CallInsertBrow(HWND hWndParent, UINT returnmsg, INSERTINFO info)
{
    WNDCLASS    wc;
    HWND        hWndInsert;
	HWND        hWndFrame;
	RECT        rClient;

    InsertMsgInfo.hWnd = hWndParent;
    InsertMsgInfo.msg = returnmsg;
    memcpy(&infoEdit, &info, sizeof(INSERTINFO));
    memset(&inIndex, MAX_INSERT_NUM + 1, sizeof(INSERTINDEX));

    wc.style         = 0;
    wc.lpfnWndProc   = InsertBrowWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = "InsertWndClass";    
    wc.hbrBackground = NULL;

    RegisterClass(&wc);

	hWndFrame = MuGetFrame();
	GetClientRect(hWndFrame, &rClient);

    hWndInsert = CreateWindow(
        "InsertWndClass", 
        STR_MMS_SELECT,
        WS_VISIBLE | WS_CHILD, 
        rClient.left, rClient.top, rClient.right-rClient.left, rClient.bottom-rClient.top,
        hWndFrame, NULL, NULL, NULL);

    if( hWndInsert != NULL )
    {
        ShowWindow( hWndInsert, SW_SHOW );
        UpdateWindow( hWndInsert );
    }
}
/*********************************************************************\
* Function     InsertBrowWndProc
* Purpose      wndproc
**********************************************************************/
static LRESULT InsertBrowWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    LRESULT         lResult;
    WORD            mID;
    HDC             hdc;
    static HWND     hFocus;
    
    switch(wMsgCmd)
    {
    case WM_CREATE:
//        SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDM_INSERT_OK, 1), 
//            (LPARAM)ML(STR_MMS_OK));
        SendMessage(MuGetFrame(), PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDB_EXIT, 0), 
            (LPARAM)IDS_INSERT_EXIT);

        OnCreateInsert(hWnd);       
        hFocus = hWndInsertLst;
        break;

    case WM_PAINT:
        hdc = BeginPaint(hWnd, NULL);       
        EndPaint(hWnd, NULL);
        break;

    case WM_COMMAND:
        mID = LOWORD(wParam);
        switch(mID)
        {       
        case IDB_EXIT:
            PostMessage(hWnd, WM_CLOSE, 0, 0);
            break;

        case IDC_INSERT_LST:
            if((HIWORD(wParam) == LBN_DBLCLK))//||(HIWORD(wParam) == LBN_CLICKED))
            {
                OnDblclkLst(hWnd);
            }           
            break;
            
        case IDM_INSERT_OK:
            OnDblclkLst(hWnd);
            break;
        default:
            break;
        }
        break;

    case WM_MMS_INSERT:
        wParam = wParam>>16;
//        if (SendMessage(hWndInsertLst, LB_GETCURSEL, 0, 0) == inIndex.indexAtt)
//            SendMessage(InsertMsgInfo.hWnd, InsertMsgInfo.msg, 
//                MAKEWPARAM(wParam, RTN_ATTATCH), lParam);
//        else
            SendMessage(InsertMsgInfo.hWnd, InsertMsgInfo.msg, 
                MAKEWPARAM(0, wParam), lParam);
        PostMessage(hWnd, WM_CLOSE, 0, 0);
        break;

    case WM_KEYDOWN:
        switch (LOWORD(wParam))
        {
        case VK_RETURN:
            SendMessage(GetFocus(), WM_KEYDOWN, VK_F5, 0);
            break;
            
        case VK_F5:
            SendMessage(hWnd,WM_COMMAND,MAKEWPARAM(IDC_INSERT_LST,LBN_DBLCLK),0);
            break;
            
        case VK_F10://ESC
            SendMessage(hWnd, WM_CLOSE, 0, 0);
            break;

        default:
            return PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        }
        break;

    case WM_ACTIVATE:
	case PWM_SHOWWINDOW:
        if (WA_INACTIVE == LOWORD(wParam))
            hFocus = GetFocus();    
        else
            SetFocus(hFocus);       
		SendMessage(MuGetFrame(), PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDB_EXIT, 0), 
            (LPARAM)IDS_INSERT_EXIT);
		SetWindowText(MuGetFrame(), STR_MMS_SELECT);
        break;

    case WM_CLOSE:      
        DestroyWindow(hWnd);        
        break;
        
    case WM_DESTROY:
        DeleteObject(hBitmapLst);
        hBitmapLst = NULL;
        hWnd = NULL;
		SendMessage(MuGetFrame(),PWM_CLOSEWINDOW,(WPARAM)hWnd,0);
        UnregisterClass("InsertWndClass", NULL);
        break;

    default:
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
    return lResult;
}

static BOOL OnCreateInsert(HWND hWnd)
{
    COLORREF        Color;
    BOOL            bTran;
    HDC             hdc;

    hdc = GetDC(hWnd);
    if (hBitmapLst == NULL)
        hBitmapLst = CreateBitmapFromImageFile(hdc, LIST_ICON_NAME, &Color, &bTran);
    ReleaseDC(hWnd, hdc);

    hWndInsertLst = CreateWindow(
        "LISTBOX",
        "", 
        WS_CHILD |WS_VISIBLE|WS_TABSTOP|LBS_HASSTRINGS|LBS_BITMAP,
        LIST_X ,
        LIST_Y ,
        LIST_WIDTH ,
        LIST_HEIGHT + 4,
        hWnd, 
        (HMENU)IDC_INSERT_LST,
        NULL, 
        NULL ); 
    if (NULL == hWndInsertLst)
        return FALSE;

    SendMessage(hWndInsertLst, LB_RESETCONTENT, 0, 0);
	
    if (!infoEdit.bHasImg)
    {
        inIndex.indexImg = SendMessage(hWndInsertLst, LB_ADDSTRING, 0, 
            (LPARAM)STR_MMS_IMAGE);
        SendMessage(hWndInsertLst, LB_SETIMAGE, MAKEWPARAM((WORD)IMAGE_BITMAP, 
            (WORD)inIndex.indexImg), (LPARAM)hBitmapLst);   
    }

    if (!infoEdit.bHasSnd)
    {
        inIndex.indexSound = SendMessage(hWndInsertLst, LB_ADDSTRING, 0, 
            (LPARAM)STR_MMS_SOUND);
        SendMessage(hWndInsertLst, LB_SETIMAGE, MAKEWPARAM((WORD)IMAGE_BITMAP, 
            (WORD)inIndex.indexSound), (LPARAM)hBitmapLst); 
    }

    if (!infoEdit.bTxtFull)
    {
        inIndex.indexTxt = SendMessage(hWndInsertLst, LB_ADDSTRING, 0, 
            (LPARAM)STR_MMS_TEXT);
        SendMessage(hWndInsertLst, LB_SETIMAGE, MAKEWPARAM((WORD)IMAGE_BITMAP, 
            (WORD)inIndex.indexTxt), (LPARAM)hBitmapLst);   
    }

  //  inIndex.indexAtt = SendMessage(hWndInsertLst, LB_ADDSTRING, 0, (LPARAM)IDS_MMS_ATTATCH);
//    SendMessage(hWndInsertLst, LB_SETIMAGE, MAKEWPARAM((WORD)IMAGE_BITMAP, 
//        (WORD)inIndex.indexAtt), (LPARAM)hBitmapLst);

	if (!infoEdit.bSlideFull)
	{
		inIndex.indexSlide = SendMessage(hWndInsertLst, LB_ADDSTRING, 0, 
			(LPARAM)STR_MMS_SLIDE);
		SendMessage(hWndInsertLst, LB_SETIMAGE, MAKEWPARAM((WORD)IMAGE_BITMAP, 
			(WORD)inIndex.indexSlide), (LPARAM)hBitmapLst);
	}
    
    
    SendMessage(hWndInsertLst, LB_SETCURSEL, 0, 0);
    MMS_ProcessListBoxScroll(hWnd, hWndInsertLst, MAX_INSERT_NUM);

    return TRUE;}
// double click listbox
static void OnDblclkLst(HWND hWnd)
{
    int index;
    DWORD   dwStyle = PREBROW_MASKINSERT|PREBROW_MASKEDIT|PREBROW_MASKDEL;

    index = SendMessage(hWndInsertLst, LB_GETCURSEL, 0, 0);
    
    if (index == inIndex.indexImg)      // image
    {
        APP_PreviewPicture(MuGetFrame(), hWnd, WM_MMS_INSERT, NULL, NULL);
    }
    else if (index == inIndex.indexSound)       // sound
    {
        APP_PreviewRing(MuGetFrame(), hWnd, WM_MMS_INSERT, "");
    }
    else if (index == inIndex.indexTxt)     // qtext
    {
        APP_PreviewPhrase(MuGetFrame(), hWnd, WM_MMS_INSERT, NULL);
    }
//    else if (index == inIndex.indexAtt)     // attatch
//    {
//        dwStyle = PIC_PREBROW|RING_PREBROW;//|QTEXT_PREBROW;
//         MMS_PreviewSelect(hWnd, WM_MMS_INSERT, dwStyle);
//    }
    else if (index == inIndex.indexSlide)       // slide
    {
        SendMessage(InsertMsgInfo.hWnd, InsertMsgInfo.msg, MAKEWPARAM(0, RTN_SLIDE), NULL);
        PostMessage(hWnd, WM_CLOSE, 0, 0);
    }

}
