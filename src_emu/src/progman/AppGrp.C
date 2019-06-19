  /***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : progman
 *
 * Purpose  : application file manager
 *            
\**************************************************************************/

#include "progman.h"

#include "string.h"
#include "malloc.h"

#include "plx_pdaex.h"

#include "AppDirs.h"
#include "AppFile.h"
#include "callpub.h"
#include "setup.h"


#define  BACKUPBITMAP

#define  ITEMHIG  50
#define  ITEMWIN  57
#define  ICONHIG  28
#define  ICONWIN  43
#define  TEXTHIN  13
#define  TEXTWIN  53

#define MAX_KEYNUM  4//deal with arrow key

typedef	struct	tagFileListNode{	
    short	  dwStyle;
    short	  nID;           //app ID
    PAPPNODE pNode;
    struct tagFileListNode *pPrev;
    struct tagFileListNode *pNext;	
} FILELISTNODE, *PFILELISTNODE;

static  PFILELISTNODE pFocusNode, pDownNode; //current app

#define IDS_OPEN  "Open"
#define IDM_OPEN  201

static    int  nFocus = 0;    //focus
static    int  nFiles = 0;    // how many app in group


static PFOLDERLINK pGrpHead = NULL;
static    RECT    rcShort; 
static    HDC     hdcMem;
static  int  nSrcFocus;
static short CurappId, CurDirId, SrcappId;
static int  imove = 0; 
static int  iMax,iPos,iPage,nPage,iWinH,iLine,ioldPos;
static SCROLLINFO scinfo;
static int  v_nRepeats;
static PKEYEVENTDATA pCurKeydata;
static unsigned int  iRepeatKeyTimerId = 0;
extern  PAPPADM		pAppMain;
extern  int         nScreenSave;

static	LRESULT	  ShowApp_WndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, 
                                LPARAM lParam);
BOOL              OpenAppFile(int fileID);
static  PAPPNODE  GetAppFile( PDIRSLINK pDir, int id);
static  void      GetCurrDir(int nCurr);
static  int		  GetAppfileID(int iChoose);
static  BOOL	  DrawAppList(HWND hWnd, HDC hdc);
static  BOOL      DrawAppMatrix(HWND hWnd, HDC hdc);
static	BOOL	  DrawApp(HWND hWnd, HDC hdc, int nApp, int iChoose);
static	int		  GetCurrentApp (void);
static  void	  InitVScroll(HWND hWnd);
static  void	  OnPmGrpVscroll(HWND hwnd, WPARAM wParam);
static short	  GetIdByIndex(short nIndex);
static void       PM_MakeHotKey(HWND hWnd, WPARAM wParam, LPARAM lParam);
static void       OnArrowKeyDown(HWND hWnd, int nKeyCode);
static void       CALLBACK f_TimerProc(HWND hWnd, UINT uMsg, UINT idEvent,
                                 DWORD dwTime);
static BOOL           CreateMoveFolderWnd(HWND hWnd);

extern  BOOL          CreateKeylockWnd(HWND hWnd);
extern PKEYEVENTDATA  GetKeyTypeByCode(long vkCode);
extern  int		 Desk_ShowAppList(int nFatherID);
extern  void     DeskGetRect(RECT *rcRect, RECT *rcText, int iCurr);
extern  void     DeskGetGifRect(RECT *rcRect, int iCurr);
extern PDIRSNODE AppGetDirNodeFromId(struct appMain *pAdm, short dirId, struct DirsLink *pDir);
extern PDIRSLINK GetDirFromId(struct appMain * pAdm, short dirId);
extern int       PmGetDivideNum(int dividend, int divisor);
extern BOOL      GetDisplayName(char* pDisplayName, char *str, unsigned int len);

/********************************************************************
* Function   RegisteShowAppClass  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
static BOOL RegisteShowAppClass(void)
{
    WNDCLASS wc;	
    
    wc.style         = 0;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;
    wc.lpszMenuName  = NULL;
    
    wc.lpfnWndProc	 = ShowApp_WndProc;
    wc.lpszClassName = "ShowApp_WindowClass";
    
    if (!RegisterClass(&wc))
        return NULL;
    
    return TRUE;
}
/********************************************************************
* Function   Desk_ShowApp  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
int Desk_ShowApp(struct appMain * pAdm, short dirId)
{		
    PDIRSNODE pDirsNode;
	PDIRSLINK pDir = NULL;

    pAppMain = pAdm;
    RegisteShowAppClass();
    
	CurDirId  = dirId;

    GetCurrDir(CurDirId);	    
    nFocus = 0;

	pDirsNode = AppGetDirNodeFromId(pAdm, CurDirId, NULL);

	if(pDirsNode == NULL)
	    return -1;
	
	 pAppMain->hAppWnd = CreateWindow("ShowApp_WindowClass",ML(pDirsNode->aDisplayName),
		WS_VISIBLE | WS_CAPTION | PWS_STATICBAR | WS_VSCROLL,
		0, 
		TITLEBAR_HEIGHT, 
		DESK_WIDTH, 
		DESK_HEIGHT-TITLEBAR_HEIGHT,
		NULL,
		NULL, NULL, NULL);

    if (pAppMain->hAppWnd == NULL)		
        return -1;	
    
   
	ShowWindow(pAppMain->hAppWnd, SW_SHOW);
	UpdateWindow(pAppMain->hAppWnd);
    
    return 0;
}
/********************************************************************
* Function   GetSMenuDirId  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
short GetSMenuDirId(void)
{
	return CurDirId;
}
/********************************************************************
* Function   ShowApp_WndProc  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
static	LRESULT	ShowApp_WndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, 
                                LPARAM lParam)
{
    LRESULT lResult = FALSE;
    HDC     hdc;	
    int     fileID;	
	
    
    switch (wMsgCmd)
    {
	case WM_CREATE:
	
		if(nFiles != 0)
		{
			SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(200, 1), 
				(LPARAM)ML(""));			
			SendMessage(hWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");

		}
		
		SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDB_EXIT, 0), 
			(LPARAM)ML("Exit"));

		InitVScroll(hWnd);
	
		break;
		
    case WM_QUERYSYSEXIT:
        return SYSEXIT_CONTINUE;  
        
    case WM_SYSEXIT:
       	{		
			DeleteDC(hdcMem);
			SetFocus(pAppMain->hGrpWnd);
	
			nFocus = 4;
			SendMessage(hWnd, WM_COMMAND, IDB_EXIT, 0);
        }
        break;
	
	case WM_ERASEBKGND:
		break;

	case WM_PMNODEADD:
		{
			int result = 0;

			nFiles ++;

			result = PmGetDivideNum(nFiles, 3);
		
			iMax = result*ITEMHIG;

			GetScrollInfo(hWnd,SB_VERT,&scinfo);
			scinfo.fMask = SIF_RANGE;
			scinfo.nMax =(LONG)iMax;
			SetScrollInfo(hWnd,SB_VERT,&scinfo,TRUE);
			EnableScrollBar(hWnd, SB_VERT, ESB_ENABLE_BOTH);
			InvalidateRect(hWnd, NULL ,TRUE);
		}

		break;

	case WM_PMNODEDEL:
		{
			int result = 0;
			
			nFiles --;
			
			result = PmGetDivideNum(nFiles, 3);
			
			iMax = result*ITEMHIG;
			
			GetScrollInfo(hWnd,SB_VERT,&scinfo);
			scinfo.fMask = SIF_RANGE;
			scinfo.nMax =(LONG)iMax;
			SetScrollInfo(hWnd,SB_VERT,&scinfo,TRUE);
			EnableScrollBar(hWnd, SB_VERT, ESB_ENABLE_BOTH);
			InvalidateRect(hWnd, NULL ,TRUE);

		}
		break;

        
    case WM_PAINT :	
        {
            PAINTSTRUCT ps;
            HDC         hdcMem;
            HFONT       hFont = NULL, hOldFont;
			
            hdc= BeginPaint(hWnd, &ps);

            hdcMem = CreateMemoryDC(DESK_WIDTH, DESK_HEIGHT);
            
			GetFontHandle(&hFont, SMALL_FONT);
			
			hOldFont = SelectObject ( hdcMem, hFont );
			SetClipRect(hdcMem, &ps.rcPaint);
            
			ClearRect(hdcMem, &ps.rcPaint, COLOR_TRANSBK );

			DrawAppMatrix(hWnd, hdcMem);
            
            BitBlt(hdc, ps.rcPaint.left, ps.rcPaint.top,
                ps.rcPaint.right - ps.rcPaint.left,
                ps.rcPaint.bottom - ps.rcPaint.top,
                hdcMem, 
                ps.rcPaint.left,
                ps.rcPaint.top,
                ROP_SRC);
            
		
			SelectObject(hdcMem, hOldFont);

            DeleteDC(hdcMem);
			hdcMem  =NULL;
            
            EndPaint(hWnd, &ps);
        }
        break;
	case WM_VSCROLL:
		
		OnPmGrpVscroll(hWnd, wParam);

		break;
		
    case WM_KEYDOWN:
		{		
			switch (wParam)
			{  
			case VK_F10:			
				{
					SendMessage(hWnd, WM_CLOSE, NULL, NULL);
					BringWindowToTop(pAppMain->hGrpWnd);
					PLXPrintf("bringwindow to top!");
					PLXPrintf("\r\nshow grp window");
					nFocus = 4;
				}				
				break;	
			case VK_F5:	
				{	
					PAPPNODE pAppNode = NULL;
					BOOL ret = FALSE;

					fileID = GetAppfileID(GetCurrentApp());
					
					if ( fileID < 0 )
						break;
					
					pAppNode = pAppMain->pFileCache + fileID;
					ret = OpenAppFile(fileID);
					if(!ret && pAppNode->sFlag & APP_NODE_DLM)
						PLXTipsWin(NULL, pAppMain->hAppWnd,NULL,ML("Cannot perform Too many applications running"),
						NULL,Notify_Failure, ML("Ok"), NULL, 20);

				}
				break;
			case VK_UP:
			case VK_DOWN:
			case VK_LEFT:
			case VK_RIGHT:
				{
					PM_MakeHotKey(hWnd, wParam, lParam);
				}
			default:
				PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
				break;
			}
		}
	   break;
	case WM_KEYUP:
		{	
			if(pCurKeydata != NULL && pCurKeydata->nType & PM_REPETEDKEY)
			{
				if(pCurKeydata->nTimerId !=0)
				{
					KillTimer(NULL, pCurKeydata->nTimerId);
					pCurKeydata->nTimerId = 0;
				}
				v_nRepeats = 0;
			}	
		}
		break;
	case WM_ACTIVATE:
		
		if ( LOWORD(wParam ) == WA_INACTIVE )
		{
			if(iRepeatKeyTimerId != 0)
			{
				KillTimer(NULL, iRepeatKeyTimerId);
				iRepeatKeyTimerId = 0;
			}
			ReleaseCapture();
		}
        else
        {
			SetFocus(hWnd); 
			if(nFiles != 0)
			{
				SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(200, 1), 
					(LPARAM)ML(""));			
				SendMessage(hWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
				
			}
			
			SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDB_EXIT, 0), 
				(LPARAM)ML("Exit"));
        }
		break;
	case WM_COMMAND:
		{	
			switch (wParam)
			{
			case IDB_EXIT:
				PostMessage(hWnd, WM_CLOSE, 0, 0);
				break;
			case IDM_OPEN:
				SendMessage(hWnd, WM_KEYDOWN, VK_RETURN, 0);
				break;
						
			default:
				break;
			}
		}
		break;
		
	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;
		
	case WM_DESTROY:
		pAppMain->hAppWnd = NULL;
		UnregisterClass("ShowApp_WindowClass", NULL);
		break;
		
	default :         
		lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
		break;
    }
    return lResult;
}
/********************************************************************
* Function   GetCurrDir  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
static void GetCurrDir(int nCurr)
{
    PDIRSLINK	pDirChild = pAppMain->pDirRoot->pDirChild;	
    PDIRSNODE	pDirsNode = NULL;	

    
	while ( pDirChild )
	{
		pDirsNode = AppGetDirNodeFromId(pAppMain, pDirChild->dirId, pDirChild);
		if (pDirsNode->flags)
		{
			if (pDirChild->dirId == nCurr)
			{
				pAppMain->pDirCurr = pDirChild;
				nFiles = AppDirsGetFileChilds ( pDirChild, DIRS_SHOW );
				return;
			}
		}
		pDirChild = pDirChild->pNext;
	}

}
/********************************************************************
* Function   GetAppfileID  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
static  int GetAppfileID(int iChoose)
{
    struct		appLink * pFileLink = pAppMain->pDirCurr->pAppLink;
    PAPPNODE	pFileNode = NULL;
    int         i = 0;	
    
    while ( pFileLink )
    {
        pFileNode = pAppMain->pFileCache+pFileLink->appId;
        
        if (pFileNode->sFlag && !(pFileNode->sFlag & APP_NODE_HIDE))
        {
            if ( i == iChoose )
            {				
                 return pFileLink->appId;
            };
            i++;
        }
        pFileLink = pFileLink->pNext;
    }	
    
    return -2;
}
/********************************************************************
* Function   OpenAppFile  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
BOOL OpenAppFile(int fileID)
{
    pAppMain->pVtbl->OpenFile( pAppMain, (short)fileID, TRUE, 0 );
    return TRUE;
}
/********************************************************************
* Function   GetAppFile  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
static  PAPPNODE GetAppFile( PDIRSLINK pDir, int id)
{
    struct		appLink * pFileLink = pDir->pAppLink;
    PAPPNODE	pFileNode = NULL;
    int         i = 0;	
    
    while ( pFileLink )
    {
        pFileNode = pAppMain->pFileCache+pFileLink->appId;
        
        if (pFileNode->sFlag && !(pFileNode->sFlag & APP_NODE_HIDE))
        {
            if ( i == id )
            {				
                return pFileNode;
            };
            i++;
        }
        pFileLink = pFileLink->pNext;
    }
    return NULL;
}
/********************************************************************
* Function   InitVScroll  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
void InitVScroll(HWND hWnd)
{
	
	memset(&scinfo,0,sizeof(SCROLLINFO));

	iLine = ITEMHIG;
    iWinH = 3*ITEMHIG;
    
	iMax =  (PmGetDivideNum(nFiles,3))* ITEMHIG;
    iPage = 3*ITEMHIG;
    nPage = (int)iMax/iPage;
    iPos = 0;
    ioldPos = 0;
	GetScrollInfo(hWnd,SB_VERT,&scinfo);
	
	scinfo.cbSize = sizeof(SCROLLINFO);
	scinfo.fMask = SIF_ALL;
	scinfo.nMax =(LONG)iMax;
	scinfo.nMin = 0;
	scinfo.nPage = iPage;
	scinfo.nPos = iPos;
	SetScrollInfo(hWnd,SB_VERT,&scinfo,TRUE);
	EnableScrollBar(hWnd, SB_VERT, ESB_ENABLE_BOTH);
	return;
}
/********************************************************************
* Function   DrawAppMatrix  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
extern void GetDlmFocusIconName(char *strFocusIconName, char* strIconName);
BOOL DrawAppMatrix(HWND hWnd, HDC hdc)
{
	int         nApp = 0, nOldmode;
    PDIRSLINK   pDir = NULL;
	PDIRSAPP    pAppLink = NULL;
	PAPPNODE    pAppNode = NULL;
    RECT		rcIcon, rcText, rcFocus,rect;
	COLORREF    /*bkColor,*/ oldTxtColor;
	char        strTitleName[PMNODE_NAMELEN + 1];
	int         len, nFit;
	HFONT       hFont;

	nOldmode = GetBkMode(hdc);
    SetBkMode(hdc, NEWTRANSPARENT);
	oldTxtColor = GetTextColor(hdc);

	if(nFiles ==0)
	{
		SetBkMode(hdc, TRANSPARENT);
		GetFontHandle(&hFont, LARGE_FONT);
		SelectObject(hdc, hFont);
		GetClientRect(hWnd, &rect);
		DrawText(hdc, ML("No items"),-1, &rect,DT_VCENTER|DT_CENTER|DT_HCENTER);
		SetTextColor(hdc, oldTxtColor);
		SetBkMode(hdc, nOldmode);
		return 0;
	}
	
	GetFontHandle(&hFont, SMALL_FONT);
	SelectObject(hdc, hFont);

	pDir = 	GetDirFromId(pAppMain, CurDirId);

	pAppLink = pDir->pAppLink;

	while( pAppLink )
	{
		if(pAppLink)
		{
			pAppNode = pAppMain->pFileCache +pAppLink->appId;
			
			if(! pAppNode->sFlag ||(pAppNode->sFlag & APP_NODE_HIDE))
			{
				pAppLink = pAppLink->pNext;
				continue;
			}
			
			if(pAppNode->iIndex == nApp)
			{
				pAppLink = pAppLink->pNext;
				goto PMLIST;
			}
		
		}
	
PMLIST:
		DeskGetRect(&rcIcon,&rcText,nApp);
		
		rcIcon.top = rcIcon.top +iPos;
		rcText.top = rcText.top + iPos;
		rcText.bottom = rcText.bottom + iPos;
		len = nFit = 0;
	
		if( nFocus!= nApp)
		{
			BOOL bDlmEnable= FALSE;
			SetTextColor(hdc, oldTxtColor);
			SetBkMode(hdc, ALPHATRANSPARENT);
			bDlmEnable = DrawBitmapFromFile(hdc, rcIcon.left, rcIcon.top, pAppNode->cIconName,SRCCOPY);
			if(!bDlmEnable && pAppNode->sFlag & APP_NODE_DLM)
			{
				DrawBitmapFromFile(hdc, rcIcon.left, rcIcon.top, "/rom/progman/app/mdefaultapp_43x28.bmp",SRCCOPY);
			}
			if(!bDlmEnable && pAppNode->sFlag & APP_NODE_WAP)
			{
				DrawBitmapFromFile(hdc, rcIcon.left, rcIcon.top, "/rom/progman/app/mdefaultshortcut_43x28.bmp",SRCCOPY);
			}
			SetBkMode(hdc, TRANSPARENT);

			if(IfStkInitSucc() && stricmp(pAppNode->aDisplayName, "Toolkit") == NULL)
			{
				char strStkTitle[100+1];

				strStkTitle[0] = 0;
				STK_GetMainText(strStkTitle);
				if(strStkTitle[0] != 0)
				{
					len = strlen(strStkTitle );
					GetTextExtentExPoint(hdc,strStkTitle , len, rcText.right- rcText.left, 
						&nFit, NULL, NULL);
				}
				else
				{
					len = strlen(ML(pAppNode->aDisplayName));
					GetTextExtentExPoint(hdc,ML(pAppNode->aDisplayName) , len, rcText.right- rcText.left, 
						&nFit, NULL, NULL);
				}

			}
			else
			{
				len = strlen(ML(pAppNode->aDisplayName));
				GetTextExtentExPoint(hdc,ML(pAppNode->aDisplayName) , len, rcText.right- rcText.left, 
					&nFit, NULL, NULL);
			}
		
			if(nFit < len)
			{
				len = nFit;
			}	
			memset(strTitleName, 0, PMNODE_NAMELEN+1);
			if(IfStkInitSucc() && stricmp(pAppNode->aDisplayName, "Toolkit") == NULL)
			{
				char strStkTitle[100+1];

				strStkTitle[0] = 0;
				STK_GetMainText(strStkTitle);
				if(strStkTitle[0] != 0)
					GetDisplayName(strStkTitle, strTitleName, len);
				else
					GetDisplayName((char*)ML(pAppNode->aDisplayName), strTitleName, len);
			}
			else
				GetDisplayName((char*)ML(pAppNode->aDisplayName), strTitleName, len);
			DrawText(hdc, ML(strTitleName), -1, &rcText, DT_CENTER | DT_VCENTER);
				
		}
		else
		{
			char strIconName[PMICON_NAMELEN];
			BOOL bDlmEnable = FALSE;
			
			oldTxtColor = SetTextColor(hdc, RGB(255,255,255));
			
			SetRect(&rcFocus, rcIcon.left-8, rcIcon.top - 9,
				rcIcon.left - 8 + 57, rcText.top -9 +50);
			
			DrawImageFromFile(hdc,"/rom/progman/select.gif", rcFocus.left, rcFocus.top, SRCCOPY);
			
			memset(strIconName, 0, PMICON_NAMELEN);
		
		//	sprintf(strIconName, "%s%s%s", PMPIC_PATH, "mg", &pAppNode->cIconName[PMPICPATHLEN +1]);

			GetDlmFocusIconName(strIconName, pAppNode->cIconName);
			SetBkMode(hdc, ALPHATRANSPARENT);
			bDlmEnable = DrawBitmapFromFile(hdc, rcIcon.left, rcIcon.top, strIconName,SRCCOPY);
			if(!bDlmEnable && pAppNode->sFlag & APP_NODE_DLM)
			{
				DrawBitmapFromFile(hdc, rcIcon.left, rcIcon.top, "/rom/progman/app/mgdefaultapp_43x28.bmp",SRCCOPY);
			}
			if(!bDlmEnable && pAppNode->sFlag & APP_NODE_WAP)
			{
				DrawBitmapFromFile(hdc, rcIcon.left, rcIcon.top, "/rom/progman/app/mgdefaultshortcut_43x28.bmp",SRCCOPY);
			}
			SetBkMode(hdc, TRANSPARENT);
			
			if(IfStkInitSucc() && stricmp(pAppNode->aDisplayName, "Toolkit") == NULL)
			{
				char strStkTitle[100+1];
				
				strStkTitle[0] = 0;
				STK_GetMainText(strStkTitle);
				if(strStkTitle[0] != 0)
				{
					len = strlen(strStkTitle );
					GetTextExtentExPoint(hdc,strStkTitle , len, rcText.right- rcText.left, 
						&nFit, NULL, NULL);
				}
				else
				{
					len = strlen(ML(pAppNode->aDisplayName) );
					GetTextExtentExPoint(hdc,ML(pAppNode->aDisplayName ), len, rcText.right- rcText.left, 
						&nFit, NULL, NULL);
				}
				
			}
			else
			{
				len = strlen(ML(pAppNode->aDisplayName) );
				GetTextExtentExPoint(hdc,ML(pAppNode->aDisplayName) , len, rcText.right- rcText.left, 
					&nFit, NULL, NULL);
			}
		
			if(nFit < len)
			{
				len = nFit;
			}	
			memset(strTitleName, 0, PMNODE_NAMELEN+1);
			if(IfStkInitSucc() && stricmp(pAppNode->aDisplayName, "Toolkit") == NULL)
			{
				char strStkTitle[100+1];
				
				strStkTitle[0] = 0;
				STK_GetMainText(strStkTitle);
				if(strStkTitle[0] != 0)
					GetDisplayName(strStkTitle, strTitleName, len);
				else
					GetDisplayName((char*)ML(pAppNode->aDisplayName), strTitleName, len);
			}
			else
				GetDisplayName((char*)ML(pAppNode->aDisplayName), strTitleName, len);


			DrawText(hdc, ML(strTitleName), -1, &rcText, DT_CENTER | DT_VCENTER);
			{
				if(pAppNode != NULL && pAppNode->aTitleName != 0)
				{
					SetWindowText(hWnd, ML(pAppNode->aTitleName));
				}
			}
					
		}
		nApp++;
	} 

	SetTextColor(hdc, oldTxtColor);
    SetBkMode(hdc, nOldmode);
    return 0;
}
/********************************************************************
* Function   DrawAppList  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
BOOL DrawAppList(HWND hWnd, HDC hdc)
{

	int i;
	HPEN hPen; 
	int oldMode;

	hPen = (HPEN)SelectObject(hdc, GetStockObject(LTGRAY_PEN));
	oldMode = SetBkMode(hdc,BM_TRANSPARENT);
#ifdef BACKUPBITMAP
#endif
	for(i = 0; i < 8; i++)
	{
		DrawApp(hWnd, hdc, i, nFocus);
	}
	SelectObject(hdc, hPen);
	SetBkMode(hdc, oldMode);
	return TRUE;
}
/********************************************************************
* Function   DrawApp  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
BOOL DrawApp(HWND hWnd, HDC hdc, int nApp, int iChoose)
{
	RECT rcIcon, rcText;
	PAPPNODE pFileNode;

	pFileNode = GetAppFile(pAppMain->pDirCurr, nApp);
	if(pFileNode == NULL)
		return FALSE;

	SetRect(&rcIcon, 4, nApp * 25 + 1, 22, nApp * 25 + 25 + 1);
	
	rcText.bottom = rcIcon.bottom;
	rcText.top = rcIcon.top;
	rcText.left = rcIcon.right + 5;
	rcText.right = DESK_WIDTH - 2;
	
	DrawIconFromFile(hdc, pFileNode->cIconName, rcIcon.left, rcIcon.top,
		20, 20);
	DrawText(hdc, ML(pFileNode->aDisplayName), -1, &rcText, DT_CENTER | DT_VCENTER);

	if(nApp == nFocus)
	{
		HPEN hPen;            
		
		hPen = (HPEN)SelectObject(hdc, GetStockObject(LTGRAY_PEN));
		DrawLine(hdc, rcIcon.left - 2, rcText.top - 1, rcIcon.left - 2, 
			rcText.bottom - 3);
		DrawLine(hdc, rcIcon.left - 1, rcText.top -1, 
			rcText.right - 1, rcText.top - 1);
		
		SelectObject(hdc, GetStockObject(BLACK_PEN));
		DrawLine(hdc, rcText.right, rcText.top - 1, 
			rcText.right, rcText.bottom - 3);
		DrawLine(hdc, rcIcon.left - 1, rcText.bottom - 3, 
			rcText.right, rcText.bottom - 3);
		SelectObject(hdc, hPen);
	}

	return TRUE;
}
/********************************************************************
* Function   GetCurrentApp  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
int GetCurrentApp(void)
{
	return nFocus;
}
/********************************************************************
* Function   OnPmGrpVscroll  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
static void OnPmGrpVscroll(HWND hwnd, WPARAM wParam)
{
    switch(LOWORD(wParam)) 
    {
        
    case SB_LINEDOWN:
        
        if(iMax - iLine + iPos < iWinH)
        {
            if(iMax + iPos > iWinH)
            {
                int move;
                
                move = iMax + ioldPos -iWinH;
                iPos -= move;
            }
            else
            {
                iPos = ioldPos;
                break;
            }
        }
        else
        {
            iPos -= iLine;
        }       
        break;
    case SB_LINEUP:
        if(iPos >= 0)
        {
            iPos = ioldPos;
            break;
        }
        iPos += iLine;    
        break;
    case SB_PAGEDOWN:
        {
            int move;
            if(iMax + iPos <iWinH)
            {
                iPos = ioldPos;
                break;
            }
            if(iPos == 0)
            {
                move = iPage;
                iPos -= move;
            }
            else
            {    
                move = (iMax-iPage+iPos)>iPage? iPage:(iMax-iPage+iPos);
                iPos -= move;
            }
        }
        
        break;
    case SB_PAGEUP:
        {
            int move;
            if(iPos>= 0)
            {
                iPos = ioldPos;
                break;
            }
            move = (-iPos) > iPage?iPage:(-iPos);
            iPos += move;           
        }        
        break;
    case SB_THUMBPOSITION:
        {
            
            int move;
            
            iPos = -HIWORD(wParam);
            if((iMax + iPos <iWinH)||(iPos>0))
            {
                iPos = ioldPos;
                break;
            }
            move =  ioldPos - iPos;
            imove = -move;
        }
        break;   
    default:
        break; 
    } 
	
	GetScrollInfo(hwnd, SB_VERT, &scinfo);
	scinfo.fMask = SIF_POS|SIF_RANGE;
	scinfo.nMax = iMax;
	scinfo.nPos = -iPos;
	SetScrollInfo(hwnd,SB_VERT,&scinfo,TRUE);
	EnableScrollBar(hwnd, SB_VERT, ESB_ENABLE_BOTH);
	InvalidateRect(hwnd, NULL, TRUE);
	ioldPos = iPos;

}
/********************************************************************
* Function   GetIdByIndex  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
static short GetIdByIndex(short nIndex)
{
	PFOLDERLINK pFolderItem =NULL, pTemp = NULL;

	if(!pGrpHead)
		return 0;

	pTemp = pGrpHead;
	while(pTemp)
	{
		if(pTemp->nIndex == nIndex)
			return pTemp->mId;

		pTemp = pTemp->pNext;
	}
	return 0;

}
/********************************************************************
* Function   PM_MakeHotKey  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
static void PM_MakeHotKey(HWND hWnd,WPARAM wParam, LPARAM lParam)
{
	PKEYEVENTDATA pKeyData = NULL;
	BOOL bKeyAvailable =FALSE;
	int i;
	long vkParam[] = 
    {
        VK_UP,VK_DOWN,VK_LEFT,VK_RIGHT, 0
    };
	
	for(i=0; i <MAX_KEYNUM;i++)
	{
		if(vkParam[i] == LOWORD(wParam))
			bKeyAvailable =TRUE;
	}
	
	if(bKeyAvailable)
	{
		pKeyData = GetKeyTypeByCode(LOWORD(wParam));
	
		{
			v_nRepeats++;
			if(pKeyData != NULL && pKeyData->nType & PM_REPETEDKEY)
			{
				if(v_nRepeats == 1)
				{
					pKeyData->nTimerId = SetTimer(NULL, 0,  ET_REPEAT_FIRST, f_TimerProc);
					iRepeatKeyTimerId = pKeyData->nTimerId;
					pCurKeydata = pKeyData;
					OnArrowKeyDown(hWnd, LOWORD(wParam));
				}
			}
			
		}
	}
}
/********************************************************************
* Function   f_TimerProc  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
static void CALLBACK f_TimerProc(HWND hWnd, UINT uMsg, UINT idEvent,
                                 DWORD dwTime)
{

	if(v_nRepeats == 1)
	{
		KillTimer(NULL, pCurKeydata->nTimerId);
		pCurKeydata->nTimerId = SetTimer(NULL,0,  ET_REPEAT_LATER,f_TimerProc );
		iRepeatKeyTimerId = pCurKeydata->nTimerId;
	}
	keybd_event(pCurKeydata->nkeyCode, 0, 0, 0);
	OnArrowKeyDown(pAppMain->hAppWnd, pCurKeydata->nkeyCode);

}
/********************************************************************
* Function   OnArrowKeyDown  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
static void OnArrowKeyDown(HWND hWnd, int nKeyCode)
{
	int nOldFocus = nFocus;
	int iNewPos = 0;

	switch(nKeyCode)
	{
	case VK_LEFT:
		{
			if(nFocus == 0)
				nFocus = nFiles - 1;
			else if (nFocus > 0)
				nFocus --;

			if((PmGetDivideNum(nFocus + 1, 3)  + iPos/ITEMHIG ) > 3)
			{
				iNewPos = (PmGetDivideNum(nFocus + 1, 3) + iPos/ITEMHIG -3)*ITEMHIG;
				SendMessage(pAppMain->hAppWnd, WM_VSCROLL,
					MAKEWPARAM(SB_THUMBPOSITION,iPos+iNewPos), NULL);
			}
			
			if((nFocus + iPos/ITEMHIG *3) < 0)
			{
				SendMessage(hWnd, WM_VSCROLL, SB_LINEUP, NULL);
			}
			if (nOldFocus != nFocus)
			{
				RECT rc1, rc2;        
				
				DeskGetGifRect(&rc1, nOldFocus);
				DeskGetGifRect(&rc2, nFocus);	
				
			}
		
			if(nOldFocus != nFocus )
			{
				InvalidateRect(hWnd, NULL,TRUE);
				UpdateWindow(hWnd);
			}
		}
		break;
	case VK_RIGHT:
		{
			if(nFocus == nFiles -1)
				nFocus = 0;
			else if (nFocus < nFiles - 1)
				nFocus ++;
			
			if((PmGetDivideNum(nFocus + 1, 3) -1 + iPos/ITEMHIG ) < 0)
			{
				iNewPos =  -(PmGetDivideNum(nFocus + 1, 3) -1 + iPos/ITEMHIG )*ITEMHIG;
				SendMessage(pAppMain->hAppWnd, WM_VSCROLL,
					MAKEWPARAM(SB_THUMBPOSITION,iPos+iNewPos), NULL);
			}

			if((nFocus + iPos/ITEMHIG * 3) > 8)
			{
				SendMessage(hWnd, WM_VSCROLL, SB_LINEDOWN, NULL);
			}

			if (nOldFocus != nFocus)
			{
				RECT rc1, rc2;        
				
				DeskGetGifRect(&rc1, nOldFocus);
				DeskGetGifRect(&rc2, nFocus);	
			}

			if(nOldFocus != nFocus)
			{
				InvalidateRect(hWnd, NULL,TRUE);
				UpdateWindow(hWnd);
			}
		}
		break;
	case VK_UP:
		{
			if (nFocus > 2)
				nFocus -= 3;
			else
			{
				nFocus += (PmGetDivideNum(nFiles, 3) -1)*3;
				if(nFocus> nFiles-1)
					nFocus -=3;
			}
			
			if((PmGetDivideNum(nFocus + 1, 3)  + iPos/ITEMHIG ) > 3)
			{
				iNewPos = (PmGetDivideNum(nFocus + 1, 3) + iPos/ITEMHIG -3)*ITEMHIG;
				SendMessage(pAppMain->hAppWnd, WM_VSCROLL,
					MAKEWPARAM(SB_THUMBPOSITION,iPos+iNewPos), NULL);
			}
			else if((PmGetDivideNum(nFocus + 1, 3) -1 + iPos/ITEMHIG) < 0)
			{	
				SendMessage(hWnd, WM_VSCROLL, SB_LINEUP, NULL);
			}

			if (nOldFocus != nFocus)
			{
				RECT rc1, rc2;        
				
				DeskGetGifRect(&rc1, nOldFocus);
				DeskGetGifRect(&rc2, nFocus);	
			}

			if(nOldFocus != nFocus)
			{
				InvalidateRect(hWnd, NULL, TRUE);
				UpdateWindow(hWnd);
			}
		}
		break;
	case VK_DOWN:
		{
			if (nFocus + 3 <= nFiles - 1)
				nFocus +=3;
			else
			{
				if((nFocus + 2 <= nFiles -1) && ((nFocus%3) != 0))
					nFocus +=2;
				else
				{
					if((nOldFocus + 1)%3 != 0)
						nFocus = (nOldFocus + 1)%3 - 1;
					else
						nFocus = 2;
				}
			}
			
			if((PmGetDivideNum(nFocus + 1, 3) -1 + iPos/ITEMHIG ) < 0)
			{
				iNewPos =  -(PmGetDivideNum(nFocus + 1, 3) -1 + iPos/ITEMHIG )*ITEMHIG;
				SendMessage(pAppMain->hAppWnd, WM_VSCROLL,
					MAKEWPARAM(SB_THUMBPOSITION,iPos+iNewPos), NULL);
			}
			else if((PmGetDivideNum(nFocus + 1, 3) + iPos/ITEMHIG ) > 3)
			{
				SendMessage(hWnd, WM_VSCROLL, SB_LINEDOWN, NULL);
			}
			
			if (nOldFocus != nFocus)
			{
				RECT rc1, rc2;        
				
				DeskGetGifRect(&rc1, nOldFocus);
				DeskGetGifRect(&rc2, nFocus);	
			}
		
			if(nOldFocus != nFocus)
			{
				InvalidateRect(hWnd, NULL, TRUE);
				UpdateWindow(hWnd);
			}
		}
		break;
	}
}
