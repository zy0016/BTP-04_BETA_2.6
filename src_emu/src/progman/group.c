    /***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : progman
 *
 * Purpose  : implement manage progman group 
 *            
\**************************************************************************/

#include "window.h"
#include "winpda.h"
#include "string.h"
#include "stdio.h"
#include "malloc.h"
#include "stdlib.h"
#include "ctype.h"

#include "plx_pdaex.h"
#include "progman.h"
#include "AppDirs.h"
#include "AppFile.h"
#include "math.h"
#include "hpimage.h"
#include "setting.h"
#include "setup.h"
#include "callpub.h"

#include "sys/types.h"
#include "sys/stat.h"
#include "sys/statfs.h"
#include "fcntl.h"
#include "unistd.h"
#include "MBPublic.h"

#define  ITEMHIG  50
#define  ITEMWIN  57
#define  ICONHIG  28
#define  ICONWIN  43
#define  TEXTHIN  13
#define  TEXTWIN  53

#define IDS_OPEN		"Open"
#define IDM_OPEN		101


#define MAX_KEYNUM  4//deal with arrow key
#define IDS_CANNOTRUN	"Cannot perform.\nToo many applications running."

static PFOLDERLINK pHead = NULL;

static  int     nGroupNum;

static  int     nFocus = 4;
static  int     nSrcFocus = 0;

static int  imove = 0; 
static int  iMax,iPos,iPage,nPage,iWinH,iLine,ioldPos;
static SCROLLINFO scinfo;


BOOL	g_bUseScrSave	= TRUE;


static PAPPNODE  pCurAppNode = NULL, pSrcAppNode =NULL;
static PDIRSNODE pCurDirsNode = NULL, pSrcDirsNode = NULL;
static short     nCurType = DIR_TYPE;
static short     nSrcType = DIR_TYPE;

static HWND  hNewfolderFocus = NULL;
static HWND  hRenamefolderFocus = NULL;
static BOOL  bEnterApp = FALSE;

static int      DirShow(HWND hWnd, HDC hdc);
	   void     DeskGetRect(RECT *rcRect, RECT *rcText, int iCurr);
	   void     DeskGetGifRect(RECT *rcRect, int iCurr);
static void     OnGrpKeyDown(HWND hWnd,UINT wMsgCmd,WPARAM wParam, LPARAM lParam);
static BOOL     OpenAppFunc(DWORD fileID);
static void     InitVscrollInfo(HWND hWnd);
static void		OnPmVscroll(HWND hwnd, WPARAM wParam);
static BOOL     DirsRename(HWND hWnd);
static BOOL     DirsNew(HWND hWnd);
static void     DirsAppMove(HWND hWnd, int nOldFocus);
		BOOL	GetDisplayName(char* pDisplayName, char *str, unsigned int len);
static BOOL     AppRunHideFile(struct appMain * pAdm);
static BOOL     FindItem(struct appMain *pAppMain, int nFocus);
static WORD  	GetIdByIndex(short nIndex);
static short	GetIndexById(WORD mId);
void SetPmhandleTableItem(PCSTR pDirName);
static void		FreeNameItem(PSTR pDirName);
static int GetFocusType( struct appMain * pAppMain, int nFocus);

static LRESULT	ShowGrp_WndProc(HWND, UINT, WPARAM, LPARAM);
extern int      Desk_ShowApp(struct appMain * pAdm, short CurdirId);
extern BOOL     CallSetTimeWindow(HWND hWndCall);
extern PDIRSNODE AppGetDirNodeFromId(struct appMain *pAdm, short dirId, struct DirsLink *pDirLink);
extern BOOL     AppRunStaticFile( PAPPADM  pAdm, PCSTR pPathName , short fileID, char bShow );
BOOL            Desk_ShowGrp( struct appMain * pAdm);
int				PmGetDivideNum(int dividend, int divisor);
static void		PM_MakeHotKey(HWND hWnd, WPARAM wParam, LPARAM lParam);
static void		OnArrowKeyDown(HWND hWnd, int nKeyCode);
static void CALLBACK f_TimerProc(HWND hWnd, UINT uMsg, UINT idEvent,
                                 DWORD dwTime);
extern PKEYEVENTDATA  GetKeyTypeByCode(long vkCode);
extern  calliftype GetCallIf();
extern  BOOL CreateKeylockWnd(HWND hWnd);
extern PDIRSNODE AppGetDirNodeFromId(struct appMain *pAdm, short dirId, struct DirsLink *pDirLink);
int PM_IsFlashAvailable(int nSize);
static  BOOL CreateMoveFolderWnd(HWND hWnd);
extern  BOOL CreateMissedEventsWnd(HWND hWnd);
extern  BOOL InitMissedEventsNum();
extern  BOOL CallAppEntryEx(PSTR pAppName, WPARAM wparam, LPARAM lparam);
extern  BOOL PM_IsPowerOff(void);

static int  v_nRepeats;
static PKEYEVENTDATA pCurKeydata;
static HWND hGrpFocus;
static unsigned int  iRepeatKeyTimerId = 0;

/********************************************************************
* Function   OpenAppFunc  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
static BOOL OpenAppFunc(DWORD fileID)
{
    pAppMain->pVtbl->OpenFile( pAppMain, (short)fileID, TRUE, 0);
    return TRUE;
}
/********************************************************************
* Function   RegisteShowGrpClass  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
BOOL RegisteShowGrpClass(void)
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
    
    wc.lpfnWndProc	 = ShowGrp_WndProc;
    wc.lpszClassName = "ShowGrp_WindowClass";
    
    if (!RegisterClass(&wc))
        return NULL;
    
    return TRUE;
}
/********************************************************************
* Function   GetGroupNodeNum  
* Purpose    get group number
* Params     
* Return     
* Remarks      
**********************************************************************/
static void GetGroupNodeNum()
{
    PDIRSLINK	pDir = NULL;
    PDIRSNODE	pNode = NULL;
	PDIRSAPP    pAppLink = NULL;
	PAPPNODE    pAppNode = NULL;
    int         i = 0;
    
    pDir  = pAppMain->pDirRoot->pDirChild;
    
    while((pDir != NULL) && pDir->dirId < MAX_DIRS_NODE)
    {        
        pNode= pAppMain->pDirsCache+pDir->dirId;
        if (!pNode->flags)
        {
            pDir = pDir->pNext;
            continue;
        }
        
        i ++;
        pDir = pDir->pNext;
    }

	//need to deal with over 100 folder
    pDir  = pAppMain->pDirRoot->pDirChild;
    
    while((pDir != NULL) )
	{
		if(pDir->dirId >= MAX_DIRS_NODE)
		{
			i++;
		}
		pDir = pDir->pNext;
	}

    pAppLink = pAppMain->pDirRoot->pAppLink;

	while(pAppLink)
	{
		pAppNode = pAppMain->pFileCache + pAppLink->appId;
		if(!pAppNode->sFlag || (pAppNode->sFlag & APP_NODE_HIDE))
		{
			pAppLink = pAppLink->pNext;
			continue;
		}
		i++;
		pAppLink = pAppLink->pNext;
	}
    
    nGroupNum = i;
}
/********************************************************************
* Function   Desk_ShowGrp  
* Purpose    create grp window
* Params     
* Return     
* Remarks      
**********************************************************************/
BOOL Desk_ShowGrp( struct appMain * pAdm)
{
    HWND	hWnd;
	PDIRSNODE pDirNode = NULL;
	PDIRSLINK pDir = NULL;
	int      i = 0;

    pAppMain = pAdm;
    
    RegisteShowGrpClass();    
    
    GetGroupNodeNum();

	if(nGroupNum <= nFocus)
		nFocus = nGroupNum -1;


    hWnd = CreateWindow("ShowGrp_WindowClass",
		"",
		WS_VISIBLE | PWS_STATICBAR |WS_VSCROLL|WS_CAPTION,
		0,	
		TITLEBAR_HEIGHT, 
		DESK_WIDTH,	
		DESK_HEIGHT- TITLEBAR_HEIGHT,
		NULL,	
		NULL,
		NULL,	NULL);

    pAdm->hGrpWnd = hWnd;

	AppRunHideFile( pAdm);
    
   // SetFocus(hWnd); 
	
	ShowWindow(hWnd, SW_HIDE);
    return TRUE;
}
/********************************************************************
* Function   ShowGrp_WndProc  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
static LRESULT ShowGrp_WndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam,
                               LPARAM lParam)
{
    LRESULT lResult = FALSE;
    HDC     hdc = NULL;
    
    switch (wMsgCmd)
    {
    case WM_CREATE :		
       
		SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(200, 1), 
           (LPARAM)ML(""));
		
        SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDB_EXIT, 0), 
           (LPARAM)ML("Exit"));

		SendMessage(hWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
		
		InitVscrollInfo(hWnd);
		hGrpFocus = hWnd;
		
        break;
	case WM_PMNODEADD:
		{
			int result = 0;

			nGroupNum ++;

			result = PmGetDivideNum(nGroupNum, 3);
		
			iMax = result*ITEMHIG;

			GetScrollInfo(hWnd,SB_VERT,&scinfo);
			scinfo.fMask = SIF_RANGE;
			scinfo.nMax =(LONG)iMax;
			SetScrollInfo(hWnd,SB_VERT,&scinfo,TRUE);
			EnableScrollBar(hWnd, SB_VERT, ESB_ENABLE_BOTH);
			InvalidateRect(hWnd, NULL ,TRUE);
		}
		 
		break;
	case WM_DLMNODEADD:
		{
			int result = 0;
			
			nGroupNum ++;
			
			result = PmGetDivideNum(nGroupNum, 3);
			
			iMax = result*ITEMHIG;
			
			GetScrollInfo(hWnd,SB_VERT,&scinfo);
			scinfo.fMask = SIF_RANGE;
			scinfo.nMax =(LONG)iMax;
			SetScrollInfo(hWnd,SB_VERT,&scinfo,TRUE);
			EnableScrollBar(hWnd, SB_VERT, ESB_ENABLE_BOTH);
			InvalidateRect(hWnd, NULL ,TRUE);
		}
		break;
	case WM_DLMNODEDEL:
	case WM_PMNODEDEL:
		{
			int result = 0;
			
			nGroupNum --;
			
			result = PmGetDivideNum(nGroupNum, 3);
			
			iMax = result*ITEMHIG;
			
			GetScrollInfo(hWnd,SB_VERT,&scinfo);
			scinfo.fMask = SIF_RANGE;
			scinfo.nMax =(LONG)iMax;
			SetScrollInfo(hWnd,SB_VERT,&scinfo,TRUE);
			EnableScrollBar(hWnd, SB_VERT, ESB_ENABLE_BOTH);
			InvalidateRect(hWnd, NULL ,TRUE);
		}
		break;
	case WM_ACTIVATE:
		if (WA_ACTIVE == LOWORD(wParam))
		{
	
			PLXPrintf("\r\n set focus grp window!\r\n");
			if(hNewfolderFocus || hRenamefolderFocus)
				;
			else
				SetFocus(hGrpFocus); 
			
			SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(200, 1), 
				(LPARAM)ML(""));
			
			SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDB_EXIT, 0), 
				(LPARAM)ML("Exit"));

			if(!PM_IsPowerOff())
			{
				if(!bEnterApp)
				{
					GetGroupNodeNum();
					
					nFocus = 4;
					if(nGroupNum <= nFocus)
						nFocus = nGroupNum -1;
					iPos = 0;
					SendMessage(pAppMain->hGrpWnd, WM_VSCROLL,
					MAKEWPARAM(SB_THUMBPOSITION,iPos), NULL);
				}
				else
					bEnterApp = FALSE;
			}
			
		}
		else
		{
           if(iRepeatKeyTimerId !=0)
		   {
			   KillTimer(NULL,iRepeatKeyTimerId);
			   iRepeatKeyTimerId = 0;
		   }
		 
		}
		break;

    case WM_PAINT :
        {
            PAINTSTRUCT ps;
            HDC         hdcMem;
            RECT rect;
			HFONT hFont, hOldFont;
			
            hdc = BeginPaint(hWnd, &ps);        
            GetClientRect(hWnd, &rect);
			
			GetGroupNodeNum();

			if(nGroupNum <= nFocus)// renew focus
				nFocus = nGroupNum -1;

			hdcMem = CreateMemoryDC(rect.right, rect.bottom);
			
			GetFontHandle(&hFont, SMALL_FONT);
					
			hOldFont = SelectObject ( hdcMem, hFont );
            SetClipRect(hdcMem, &ps.rcPaint);            
            
			ClearRect(hdcMem, &ps.rcPaint, COLOR_TRANSBK);
			
			DirShow(hWnd, hdcMem);

            BitBlt(hdc, ps.rcPaint.left, ps.rcPaint.top,
                ps.rcPaint.right - ps.rcPaint.left,
                ps.rcPaint.bottom - ps.rcPaint.top,                
                hdcMem, 
                ps.rcPaint.left, 
                ps.rcPaint.top,
                ROP_SRC);
     
			SelectObject(hdcMem, hOldFont);
            DeleteDC(hdcMem);
			hdcMem = NULL;
		      
            EndPaint(hWnd, &ps);
            break;
        }

	case WM_VSCROLL:
		
		OnPmVscroll(hWnd, wParam);
		
		break;
        
    case WM_QUERYSYSEXIT:
        return SYSEXIT_CONTINUE;
        
    case WM_SYSEXIT:
	case WM_CLOSE:
        DestroyWindow(hWnd);
        break;
        
    case WM_DESTROY:
        {	
			pAppMain->hGrpWnd = NULL;
            break;
        }
        
    case WM_KEYDOWN:
        
		switch(LOWORD(wParam)) 
		{
		case VK_F10:
			{	
				if(IsCallAPP())
				{
					CallAppEntryEx("Call", NULL, NULL);
				}
				else
				{
					if(!IsWindowVisible(pAppMain->hIdleWnd))
					{
						ShowWindow(pAppMain->hIdleWnd, SW_SHOW);
					}
					else
					{
						BringWindowToTop(pAppMain->hIdleWnd);
					}
				}	
			}
			break;
	
		case VK_LEFT:
		case VK_RIGHT:
		case VK_UP:
		case VK_DOWN:
			{
				PM_MakeHotKey(hWnd, wParam, lParam);
			}
			break;
		default :
		    OnGrpKeyDown(hWnd, wMsgCmd, wParam, lParam);
		}
	        break;
	case WM_KEYUP:
		{	
			if(pCurKeydata != NULL && pCurKeydata->nType & PM_REPETEDKEY)
			{
				if(pCurKeydata->nTimerId !=0)
				{
					PLXPrintf("\r\n kill timer \r\n");
					KillTimer(NULL, pCurKeydata->nTimerId);
					pCurKeydata->nTimerId = 0;
				}
				v_nRepeats = 0;
			}	
		}
		break;
	case WM_ERASEBKGND:
		break;

	    case WM_COMMAND:
		{
			switch(LOWORD(wParam)) 
			{
			case IDB_EXIT:
				//return idle state
				if(pAppMain->hIdleWnd)
				{
					if(!IsWindowVisible(pAppMain->hIdleWnd))
					{
						ShowWindow(pAppMain->hIdleWnd, SW_SHOW);
//						if(InitMissedEventsNum())
//							CreateMissedEventsWnd(pAppMain->hIdleWnd);
					}
					else
					{
						BringWindowToTop(pAppMain->hIdleWnd);
//						if(InitMissedEventsNum())
//							CreateMissedEventsWnd(pAppMain->hIdleWnd);
					}
				}
				break;
			case IDM_OPEN:
				SendMessage(hWnd, WM_KEYDOWN, VK_RETURN, 0);
				break;
			
			default:
				break;
			}
			   
		}

		break;
	default : 
			// Must call DefWindowProc to process other message
	  lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
	  break;
    }
    return lResult;
}
/********************************************************************
* Function   PM_MakeHotKey  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
static void PM_MakeHotKey(HWND hWnd, WPARAM wParam, LPARAM lParam)
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
					PLXPrintf("\r\n first time enter keyevent \r\n");
					OnArrowKeyDown(hWnd, LOWORD(wParam));
				}
			}
			
		}
	}
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
				nFocus = nGroupNum -1;
			else if (nFocus > 0)
				nFocus --;
			
			if((PmGetDivideNum(nFocus + 1, 3)  + iPos/ITEMHIG ) > 3)
			{
				iNewPos = (PmGetDivideNum(nFocus + 1, 3) + iPos/ITEMHIG -3)*ITEMHIG;
				SendMessage(pAppMain->hGrpWnd, WM_VSCROLL,
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
		
			if(nOldFocus != nFocus)
			{
				InvalidateRect(hWnd, NULL, TRUE);
				UpdateWindow(hWnd);
			}
		}
		break;
    case VK_RIGHT:
		{
			if(nFocus == nGroupNum - 1)
				nFocus = 0;
			else if (nFocus < nGroupNum - 1)
				nFocus ++;
			
			PLXPrintf("\r\n nfocus = %d, iPos = %d \r\n", nFocus, iPos);
			
			if((PmGetDivideNum(nFocus + 1, 3) -1 + iPos/ITEMHIG ) < 0)
			{
				iNewPos =  -(PmGetDivideNum(nFocus + 1, 3) -1 + iPos/ITEMHIG )*ITEMHIG;
				SendMessage(pAppMain->hGrpWnd, WM_VSCROLL,
					MAKEWPARAM(SB_THUMBPOSITION,iPos+iNewPos), NULL);
			}

			if((nFocus + iPos/ITEMHIG * 3) > 8)
			{
				PLXPrintf("\r\n stop move \r\n");
				SendMessage(hWnd, WM_VSCROLL, SB_LINEDOWN, NULL);
				//break;
			}
			if (nOldFocus != nFocus)
			{
				RECT rc1, rc2;        
				
				DeskGetGifRect(&rc1, nOldFocus);
				DeskGetGifRect(&rc2, nFocus);	
			}
		
			if(nOldFocus != nFocus )
			{
				InvalidateRect(hWnd, NULL, TRUE);
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
				nFocus += (PmGetDivideNum(nGroupNum, 3) -1)*3;
				if(nFocus> nGroupNum -1)
					nFocus -=3;
			}
			
			if((PmGetDivideNum(nFocus + 1, 3)  + iPos/ITEMHIG ) > 3)
			{
				iNewPos = (PmGetDivideNum(nFocus + 1, 3) + iPos/ITEMHIG -3)*ITEMHIG;
				SendMessage(pAppMain->hGrpWnd, WM_VSCROLL,
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
			if (nFocus + 3 <= nGroupNum - 1)
				nFocus +=3;
			else
			{
				if((nFocus + 2 <= nGroupNum -1) && ((nFocus%3) != 0))
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
				SendMessage(pAppMain->hGrpWnd, WM_VSCROLL,
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
		
			if(nOldFocus != nFocus )
			{
				InvalidateRect(hWnd, NULL, TRUE);
				UpdateWindow(hWnd);
			}
		}
		break;
	}
}
/********************************************************************
* Function	 OnGrpKeyDown  
* Purpose    
* Params	 hWnd:
* Return	 	   
* Remarks	   
**********************************************************************/
static void OnGrpKeyDown(HWND hWnd,UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{

    switch(wParam)
    {
	case VK_F5:
		bEnterApp = TRUE;
		if(nCurType == APP_TYPE)
		{
			if(pCurAppNode)
			{
				BOOL ret = FALSE;

				ret = AppFileOpen(pAppMain,pCurAppNode->appId, TRUE, 0);
				if(!ret && pCurAppNode->sFlag & APP_NODE_DLM)
					PLXTipsWin(NULL, pAppMain->hGrpWnd,NULL,ML(IDS_CANNOTRUN),
					NULL,Notify_Failure, ML("Ok"), NULL, 20);
			}
		}
		else if(nCurType == DIR_TYPE)
		{
			if(pCurDirsNode)
				Desk_ShowApp(pAppMain, pCurDirsNode->dirId);
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
		{
			WORD mId ;
			int  nIndex;

			mId = LOWORD(wParam);
			nIndex = mId - 0x31;
			
			if(nIndex - iPos/ITEMHIG * 3 > nGroupNum  -1)
				break;
			
			nFocus =  nIndex - iPos/ITEMHIG * 3;
			FindItem(pAppMain, nFocus);

			bEnterApp = TRUE;

			if(nCurType == APP_TYPE)
			{
				BOOL ret = FALSE;

				
				ret = AppFileOpen(pAppMain, pCurAppNode->appId, TRUE, 0);
				if(!ret && pCurAppNode->sFlag & APP_NODE_DLM)
					PLXTipsWin(NULL, pAppMain->hGrpWnd,NULL,ML(IDS_CANNOTRUN),
					NULL,Notify_Failure, ML("Ok"), NULL, 20);
			}
			else if(nCurType == DIR_TYPE)
				Desk_ShowApp(pAppMain, pCurDirsNode->dirId);

		}
		break;
	default:
		PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
		break;
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
	PLXPrintf("\r\n timeout and enter key event");
	keybd_event(pCurKeydata->nkeyCode, 0, 0, 0);
	OnArrowKeyDown(pAppMain->hGrpWnd, pCurKeydata->nkeyCode);

}
/********************************************************************
* Function   DirShow  
* Purpose    paint first menu
* Params     
* Return     
* Remarks      
**********************************************************************/
void GetDlmFocusIconName(char *strFocusIconName, char* strIconName);
static int DirShow(HWND hWnd, HDC hdc)
{
    int         nDir = 0, nOldmode;
    PDIRSLINK	pDir = NULL;
    PDIRSNODE   pNode = NULL;
	PDIRSAPP    pAppLink = NULL;
	PAPPNODE    pAppNode = NULL;
    RECT		rcIcon, rcText, rcFocus;
    short       nType;
	char        strDirName[PMNODE_NAMELEN + 1];
	COLORREF    oldTxtColor;
	int         len, nFit;
	BOOL        bFind = FALSE;

	nOldmode = GetBkMode(hdc);
	oldTxtColor = GetTextColor(hdc);

	pDir  = pAppMain->pDirRoot->pDirChild;
	pAppLink = pAppMain->pDirRoot->pAppLink;

	while( pDir || pAppLink )
	{
		bFind = FALSE;
		if(pAppLink)
		{
			pAppNode = pAppMain->pFileCache +pAppLink->appId;
			
			if(! pAppNode->sFlag  || (pAppNode->sFlag & APP_NODE_HIDE))
			{
				pAppLink = pAppLink->pNext;
				continue;
			}
			
			if(pAppNode->iIndex == nDir)
			{
				bFind = TRUE;
				nType = APP_TYPE;
				pAppLink = pAppLink->pNext;
				goto PMLIST;
			}
		
		}

		if(pDir)
		{
			if(pDir->dirId < MAX_DIRS_NODE)
				pNode= pAppMain->pDirsCache+pDir->dirId;
			else
				pNode = pDir->pDirNode;

			if (!pNode->flags)
			{
				pDir = pDir->pNext;
				continue;
			}
			
			if(pNode->iIndex == nDir)
			{
				bFind = TRUE;
				nType = DIR_TYPE;
				pDir = pDir->pNext;
				goto PMLIST;
			}
		}
		if(bFind == FALSE)
		{
			nDir++;
			if(!pDir || !pAppLink)
				break;
			else
				continue;
		}

PMLIST:
		DeskGetRect(&rcIcon,&rcText,nDir);
		
		rcIcon.top = rcIcon.top +iPos;
		rcText.top = rcText.top + iPos;
		rcText.bottom = rcText.bottom + iPos;
		len = nFit = 0;
	
		if( nFocus!= nDir)
		{
			SetTextColor(hdc, oldTxtColor);
			if(nType == DIR_TYPE && pNode->iIndex == nDir)
			{
				SetBkMode(hdc, ALPHATRANSPARENT );
				DrawBitmapFromFile(hdc,rcIcon.left ,rcIcon.top , pNode->aIconName, SRCCOPY);			
				len = strlen(ML(pNode->aDisplayName));
				GetTextExtentExPoint(hdc,ML(pNode->aDisplayName) , len, rcText.right- rcText.left, 
					&nFit, NULL, NULL);
				if(nFit < len)
				{
                   len = nFit;
				}
				memset(strDirName, 0 ,PMNODE_NAMELEN + 1);
				GetDisplayName((char*)ML(pNode->aDisplayName), strDirName, len);
				SetBkMode(hdc, TRANSPARENT);
				DrawText(hdc, ML(strDirName),-1, &rcText, DT_CENTER | DT_VCENTER);
			}
			else if(nType == APP_TYPE && pAppNode->iIndex == nDir)
			{
				BOOL bDlmEnable = FALSE;

				SetBkMode(hdc, ALPHATRANSPARENT );
				bDlmEnable = DrawBitmapFromFile(hdc, rcIcon.left, rcIcon.top, pAppNode->cIconName,SRCCOPY);
				if(!bDlmEnable && pAppNode->sFlag & APP_NODE_DLM)
				{
					DrawBitmapFromFile(hdc, rcIcon.left, rcIcon.top, "/rom/progman/app/mdefaultapp_43x28.bmp",SRCCOPY);
				}
				if(!bDlmEnable && pAppNode->sFlag & APP_NODE_WAP)
				{
					DrawBitmapFromFile(hdc, rcIcon.left, rcIcon.top, "/rom/progman/app/mdefaultshortcut_43x28.bmp",SRCCOPY);
				}
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
					GetTextExtentExPoint(hdc,ML(pAppNode->aDisplayName), len, rcText.right- rcText.left, 
						&nFit, NULL, NULL);
				}
				
				if(nFit < len)
				{
					len = nFit;
				}
				memset(strDirName, 0 ,PMNODE_NAMELEN + 1);
				if(IfStkInitSucc() && stricmp(pAppNode->aDisplayName, "Toolkit") == NULL)
				{
					char strStkTitle[100+1];
					
					strStkTitle[0] = 0;
					STK_GetMainText(strStkTitle);
					if(strStkTitle[0] != 0)
						GetDisplayName(strStkTitle, strDirName, len);
					else
						GetDisplayName((char*)ML(pAppNode->aDisplayName), strDirName, len);
				}
				else
					GetDisplayName((char*)ML(pAppNode->aDisplayName), strDirName, len);

				SetBkMode(hdc, TRANSPARENT);
				DrawText(hdc, ML(strDirName),-1, &rcText, DT_CENTER | DT_VCENTER);
			}
		}
		else
		{
			char strIconName[PMICON_NAMELEN];

			if(nType == DIR_TYPE)
				pCurDirsNode = pNode;
			else
				pCurAppNode = pAppNode;

			nCurType = nType;
	
			SetRect(&rcFocus, rcIcon.left -8, rcIcon.top - 9,
				rcIcon.left -8 + 57, rcText.bottom -9 + 50);

			
			DrawImageFromFile(hdc,"/rom/progman/select.gif", rcFocus.left, rcFocus.top, SRCCOPY);
			
			
			memset(strIconName, 0, PMICON_NAMELEN);
			
			
			oldTxtColor = SetTextColor(hdc, RGB(255,255,255));
			
			if(nType == DIR_TYPE && pNode->iIndex == nDir)
			{
				
			//	sprintf(strIconName, "%s%s%s", PMPIC_PATH, "mg", &pNode->aIconName[PMPICPATHLEN +1]);
				GetDlmFocusIconName(strIconName, pNode->aIconName);
					
				SetBkMode(hdc, ALPHATRANSPARENT );
			    DrawBitmapFromFile(hdc, rcIcon.left-2, rcIcon.top-4, strIconName,SRCCOPY);
			
				len = strlen(ML(pNode->aDisplayName));
				GetTextExtentExPoint(hdc,ML(pNode->aDisplayName) , len, rcText.right- rcText.left, 
					&nFit, NULL, NULL);
				if(nFit < len)
				{
					len = nFit;
				}
				memset(strDirName, 0 ,PMNODE_NAMELEN + 1);
				GetDisplayName((char*)ML(pNode->aDisplayName), strDirName, len);
				SetBkMode(hdc, TRANSPARENT);
				DrawText(hdc, ML(strDirName),-1, &rcText, DT_CENTER | DT_VCENTER);
		
				SetWindowText(hWnd, ML(pNode->aTitleName));
				
			}
			else if(nType == APP_TYPE && pAppNode->iIndex == nDir)
			{
				BOOL bDlmEnable = FALSE;
			//	sprintf(strIconName, "%s%s%s", PMPIC_PATH, "mg",&pAppNode->cIconName[PMPICPATHLEN +1]);
				GetDlmFocusIconName(strIconName, pAppNode->cIconName);
				SetBkMode(hdc, ALPHATRANSPARENT );
				bDlmEnable = DrawBitmapFromFile(hdc, rcIcon.left-2, rcIcon.top-4, strIconName,SRCCOPY);
				if(!bDlmEnable && pAppNode->sFlag & APP_NODE_DLM)
				{
					DrawBitmapFromFile(hdc, rcIcon.left-2, rcIcon.top-4, "/rom/progman/app/mgdefaultapp_43x28.bmp",SRCCOPY);
				}
				if(!bDlmEnable && pAppNode->sFlag & APP_NODE_WAP)
				{
					DrawBitmapFromFile(hdc, rcIcon.left, rcIcon.top, "/rom/progman/app/mgdefaultshortcut_43x28.bmp",SRCCOPY);
				}
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
						GetTextExtentExPoint(hdc,ML(pAppNode->aDisplayName), len, rcText.right- rcText.left, 
							&nFit, NULL, NULL);
					}
					
				}
				else
				{
					len = strlen(ML(pAppNode->aDisplayName));
					GetTextExtentExPoint(hdc,ML(pAppNode->aDisplayName), len, rcText.right- rcText.left, 
						&nFit, NULL, NULL);
				}
               
				if(nFit < len)
				{
					len = nFit;
				}
				memset(strDirName, 0 ,PMNODE_NAMELEN + 1);
				if(IfStkInitSucc() && stricmp(pAppNode->aDisplayName, "Toolkit") == NULL)
				{
					char strStkTitle[100+1];
					
					strStkTitle[0] = 0;
					STK_GetMainText(strStkTitle);
					if(strStkTitle[0] != 0)
						GetDisplayName(strStkTitle, strDirName, len);
					else
						GetDisplayName((char*)ML(pAppNode->aDisplayName), strDirName, len);
				}
				else
				GetDisplayName((char*)ML(pAppNode->aDisplayName), strDirName, len);
				SetBkMode(hdc, TRANSPARENT);
				DrawText(hdc, ML(strDirName),-1, &rcText, DT_CENTER | DT_VCENTER);
				
				if(IfStkInitSucc() && stricmp(pAppNode->aDisplayName, "Toolkit") == NULL)
				{
					char strStkTitle[100+1];
					
					strStkTitle[0] = 0;
					STK_GetMainText(strStkTitle);
					if(strStkTitle[0] != 0)
						SetWindowText(hWnd, ML(strStkTitle));
					else
						SetWindowText(hWnd, ML(pAppNode->aTitleName));
				}
				else
					SetWindowText(hWnd, ML(pAppNode->aTitleName));
				
			}
			
		}
		nDir++;
	} 

	SetTextColor(hdc, oldTxtColor);
    SetBkMode(hdc, nOldmode);
    return 0;
}
/********************************************************************
* Function   DeskGetRect  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
void DeskGetRect(RECT *rcRect, RECT *rcText, int iCurr)
{	
    POINT Pos; 
	int   iOldCur = iCurr;

	iCurr = iCurr%9;

	if (iCurr < 3) //first line
	{
		Pos.x = iCurr * 57 ;
		Pos.y = 0;
	}
	else if(iCurr >=3 && iCurr < 6) //second line
	{
		Pos.x = (iCurr - 3) * 57 ;
		Pos.y = ITEMHIG;
	}
	else//third line
	{
		Pos.x = (iCurr - 6) * 57 ;
		Pos.y = 2*ITEMHIG;
	}

	Pos.y = Pos.y + iOldCur/9*3*ITEMHIG;

	SetRect(rcRect, Pos.x + 8, Pos.y + 9, 
		Pos.x + 8 + ICONWIN, Pos.y + ICONHIG);

	SetRect(rcText,Pos.x + 3, Pos.y+ 35,
		Pos.x + 3 + TEXTWIN, Pos.y +  35 + TEXTHIN);
}
/********************************************************************
* Function   DeskGetGifRect  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
void DeskGetGifRect(RECT *rcRect, int iCurr)
{
	RECT rcText;

	DeskGetRect(rcRect, &rcText, iCurr);

	//adjust for gif
	rcRect->top = rcRect->top -7;
	rcRect->left = rcRect->left -7;
	rcRect->bottom = rcText.bottom + 15;
	rcRect->right = rcRect->right +7;
}
/********************************************************************
* Function   InitVscrollInfo  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
static void InitVscrollInfo(HWND hWnd)
{

	iLine = ITEMHIG;
    iWinH = 3*ITEMHIG;
    

    iMax =  (PmGetDivideNum(nGroupNum, 3))* ITEMHIG;
    iPage = 3*ITEMHIG;
    nPage = (int)iMax/iPage;
    iPos = 0;
    ioldPos = 0;
	
	memset(&scinfo,0,sizeof(SCROLLINFO));

	GetScrollInfo(hWnd,SB_VERT,&scinfo);
        
    scinfo.cbSize = sizeof(SCROLLINFO);
    scinfo.fMask = SIF_ALL;
    scinfo.nMax =(LONG)iMax;
    scinfo.nMin = 0;
    scinfo.nPage = iPage;
    scinfo.nPos = iPos;
    SetScrollInfo(hWnd,SB_VERT,&scinfo,TRUE);
    EnableScrollBar(hWnd, SB_VERT, ESB_ENABLE_BOTH); 

}
/********************************************************************
* Function   OnPmVscroll 
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
static void OnPmVscroll(HWND hWnd, WPARAM wParam)
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
	    PLXPrintf("\r\n scroll lineup !\r\n");
        if(iPos >= 0)
        {
            iPos = ioldPos;
            break;
        }
        iPos += iLine;    
		PLXPrintf("\r\n scroll line ipos= %d", iPos);
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
	GetScrollInfo(hWnd, SB_VERT, &scinfo);
	scinfo.fMask = SIF_POS|SIF_RANGE;
	scinfo.nMax = iMax;
	scinfo.nPos = -iPos;
	SetScrollInfo(hWnd,SB_VERT,&scinfo,TRUE);
	EnableScrollBar(hWnd, SB_VERT, ESB_ENABLE_BOTH);
	InvalidateRect(hWnd, NULL, TRUE);
	ioldPos = iPos;
}

/********************************************************************
* Function   PmGetDivideNum  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
int PmGetDivideNum(int dividend, int divisor)
{
	int result;

	result = dividend/divisor;
	
	if(dividend%divisor != 0)
	{
		return result + 1;
	}
	return result;
}
/********************************************************************
* Function   GetDisplayName  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
BOOL GetDisplayName(char* pDisplayName, char *str, unsigned int len)
{

	if(strlen(pDisplayName) > len)
	{
		strncpy(str, pDisplayName, len - 3);
		str[len-3] = 0;
		strcat(str, "...");
		str[len] = 0;
	}
	else
		strcpy(str, pDisplayName);

	return TRUE;	
}
/********************************************************************
* Function   AppRunHideFile  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
static BOOL AppRunHideFile(struct appMain * pAdm)
{
	PAPPNODE pAppNode =NULL;
	int      i ;

	for(i = 0 ; i < MAX_FILE_NODE; i++)
	{
		pAppNode = pAdm->pFileCache + i;
		if(pAppNode && (pAppNode->sFlag & APP_NODE_HIDE))
		{
			if(pAppNode->aDisplayName[0] == 0)
				continue;
			if( (stricmp(pAppNode->aDisplayName, "Call") == NULL) ||
				(stricmp(pAppNode->aDisplayName, "LocationModule") == NULL))
				AppRunStaticFile( pAdm, pAppNode->aDisplayName, pAppNode->appId, FALSE );
			return TRUE;
		}
	}
	
	return FALSE;
}
/********************************************************************
* Function   FindItem  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
static BOOL FindItem(struct appMain *pAppMain, int nFocus)
{
	PDIRSNODE pDirNode = NULL;
	PAPPNODE pAppNode= NULL;
	PDIRSLINK pDir = NULL;
	PDIRSAPP pAppLink = NULL;

	pDir  = pAppMain->pDirRoot->pDirChild;
	pAppLink = pAppMain->pDirRoot->pAppLink;
	
	while( pDir || pAppLink )
	{
		if(pAppLink)
		{
			pAppNode = pAppMain->pFileCache +pAppLink->appId;
			
			if(! pAppNode->sFlag  || (pAppNode->sFlag & APP_NODE_HIDE))
			{
				pAppLink = pAppLink->pNext;
				continue;
			}
			
			if(pAppNode->iIndex == (short)nFocus)
			{
				nCurType = APP_TYPE;
				pAppLink = pAppLink->pNext;
				pCurAppNode = pAppNode;
				return TRUE;
			}
			pAppLink = pAppLink->pNext;
		}
		
		if(pDir)
		{
			if(pDir->dirId < MAX_DIRS_NODE)
				pDirNode= pAppMain->pDirsCache+pDir->dirId;
			else
				pDirNode = pDir->pDirNode;
			
			if (!pDirNode->flags)
			{
				pDir = pDir->pNext;
				continue;
			}
			
			if(pDirNode->iIndex == (short)nFocus)
			{
				nCurType = DIR_TYPE;
				pDir = pDir->pNext;
				pCurDirsNode = pDirNode;
				return TRUE;
			}
			pDir = pDir->pNext;
		}
	} 
	return FALSE;
}
/********************************************************************
* Function   GetIdByIndex  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
static WORD GetIdByIndex(short nIndex)
{
	PFOLDERLINK pFolderItem =NULL, pTemp = NULL;

	if(!pHead)
		return 0;

	pTemp = pHead;
	while(pTemp)
	{
		if(pTemp->nIndex == nIndex)
			return pTemp->mId;

		pTemp = pTemp->pNext;
	}
	return 0;

}
/********************************************************************
* Function   GetIndexById  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
static short GetIndexById(WORD mId)
{
	PFOLDERLINK pFolderItem = NULL, pTemp = NULL;

	if(!pHead)
		return 0;

	pTemp = pHead;
	while(pTemp)
	{
		if(pTemp->mId == mId)
			return pTemp->nIndex;
		pTemp = pTemp->pNext;
	}
	return 0;
}

/********************************************************************
* Function   GetFocusType  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
static int GetFocusType( struct appMain * pAppMain, int nFocus)
{
	PDIRSLINK	pDir = NULL;
	PDIRSNODE   pNode = NULL;
	PDIRSAPP    pAppLink = NULL;
	PAPPNODE    pAppNode = NULL;
	int         nType ;
	   
	pDir  = pAppMain->pDirRoot->pDirChild;
	pAppLink = pAppMain->pDirRoot->pAppLink;
	
	   while( pDir || pAppLink )
	   {
		   if(pAppLink)
		   {
			   pAppNode = pAppMain->pFileCache +pAppLink->appId;
			   
			   if(! pAppNode->sFlag  || (pAppNode->sFlag & APP_NODE_HIDE))
			   {
				   pAppLink = pAppLink->pNext;
				   continue;
			   }
			   
			   if(pAppNode->iIndex == nFocus)
			   {
				   nType = APP_TYPE;
				   return nType;
			   }
			   else
			   {
				   pAppLink = pAppLink->pNext;
			   }
			   
		   }
		   
		   if(pDir)
		   {
			   if(pDir->dirId < MAX_DIRS_NODE)
				   pNode= pAppMain->pDirsCache+pDir->dirId;
			   else
				   pNode = pDir->pDirNode;
			   
			   if (!pNode->flags)
			   {
				   pDir = pDir->pNext;
				   continue;
			   }
			   
			   if(pNode->iIndex == nFocus)
			   {
				   nType = DIR_TYPE;
				   return nType;
			   }
			   else
			   {
				   pDir = pDir->pNext;
			   }
		   }
	   }
	   return -1;
}

#define SPACE_CHECKERROR     0   
#define SPACE_AVAILABLE      1   
#define SPACE_NEEDCLEAR      2   
#define SPACE_NEEDDELFILE    3  
static const char * pFlash2   = "/mnt/flash/";

/********************************************************************
* Function   PM_IsFlashAvailable  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
int PM_IsFlashAvailable(int nSize)
{
	struct statfs  *spaceinfo = NULL;
	int ret = -1;

	
	spaceinfo = malloc(sizeof(struct statfs));
	if(spaceinfo == NULL)
		return ret;

	memset(spaceinfo, 0 , sizeof(struct statfs) );

	statfs(pFlash2, spaceinfo);

	if(nSize > spaceinfo->f_bfree)
		ret = SPACE_CHECKERROR;
	else if(nSize == spaceinfo->f_bfree)
		ret =  SPACE_NEEDCLEAR;
	else
		ret = SPACE_AVAILABLE;

	free(spaceinfo);
	spaceinfo = NULL;

	return ret;


}
/********************************************************************
* Function   GetDlmFocusIconName  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
void GetDlmFocusIconName(char *strFocusIconName, char* strIconName)
{
	char p[PMICON_NAMELEN + 1];
	int i =0, j = 0;

	p[0] = 0;

	strcpy(p, strIconName);
    
	while(p[i] != 0)
	{
		if(p[i] == '/')
		{
			j = i;
		}
		i ++;
	}

	if(j == 0)
		strFocusIconName = NULL;

	j +=2 ;

	while(i != j-1)
	{
		p[i + 1] = p[i];
		i--;
	}

	p[j] = 'g';
	strcpy(strFocusIconName, p);

}
