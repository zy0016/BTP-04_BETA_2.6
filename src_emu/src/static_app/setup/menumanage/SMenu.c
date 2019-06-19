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

#include "SMenu.h"
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
    wc.lpszClassName = "MenuShowApp_WindowClass";
    
    if (!RegisterClass(&wc))
        return NULL;
    
    return TRUE;
}
/********************************************************************
* Function   DeskMenu_ShowApp  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
int DeskMenu_ShowApp(struct appMain * pAdm, short dirId,HWND hGrpMenuWnd)
{		
    PDIRSNODE pDirsNode;
	PDIRSLINK pDir = NULL;
	RECT rect;

    pAppMain = pAdm;
	hParantWnd = hGrpMenuWnd;
    RegisteShowAppClass();
    
	CurDirId  = dirId;

    GetCurrDir(CurDirId);	    
    nFocus = 0;

	pDirsNode = AppGetDirNodeFromId(pAdm, CurDirId, NULL);

	if(nFiles != 0)
		LoadGrpMenu(pAdm);
	else
		hMenu = NULL;

	GetClientRect(hFrameWin, &rect);
	
	 hAppWnd = CreateWindow("MenuShowApp_WindowClass","",
		WS_VISIBLE |WS_CHILD| WS_VSCROLL,
		rect.left,rect.top,rect.right-rect.left,rect.bottom-rect.top,
		hFrameWin,
		hMenu, NULL, NULL);

    if (hAppWnd == NULL)		
        return -1;	
 
	ShowWindow(hAppWnd, SW_SHOW);
	UpdateWindow(hAppWnd);
    
    return 0;
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
   // int     fileID;	
	
    
    switch (wMsgCmd)
    {
	case WM_CREATE:
	
		if(nFiles != 0)
		{
			if(nFiles == 1)
				SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(200, 1), 
				(LPARAM)ML(""));
			else
				SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDM_MOVE, 1), 
				(LPARAM)ML("Move"));
		
			SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Options");

		}
		else
		{
			SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(200, 1), 
				(LPARAM)ML(""));

			SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
		}
		
		SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDB_EXIT, 0), 
			(LPARAM)ML("Back"));

		SetWindowText(hFrameWin, ML("Menu manager"));

		PDASetMenu(hFrameWin, hMenu);

		InitVScroll(hWnd);

		SetFocus(hWnd);
	
		break;
	case PWM_SHOWWINDOW:
		{
			
			if(nFiles != 0)
			{
				if(bMoveReady)
				{
					SendMessage(hFrameWin, PWM_SETBUTTONTEXT, (WPARAM)1, (LPARAM)ML(""));
					SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
				}
				else
				{
					if(nFiles == 1)
						SendMessage(hFrameWin, PWM_SETBUTTONTEXT, (WPARAM)1, (LPARAM)ML(""));
					else
						SendMessage(hFrameWin, PWM_SETBUTTONTEXT, (WPARAM)1, (LPARAM)ML("Move"));
					SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Options");
				}
			}
			else
			{
				SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(200, 1), 
					(LPARAM)ML(""));
				SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
			}

			if(bMoveReady)
				SendMessage(hFrameWin, PWM_SETBUTTONTEXT, (WPARAM)0, (LPARAM)ML("Cancel"));
			else
				SendMessage(hFrameWin, PWM_SETBUTTONTEXT, (WPARAM)0, (LPARAM)ML("Back"));

			if(bMoveReady)
				SetWindowText(hFrameWin, ML("Move"));
			else
				SetWindowText(hFrameWin, ML("Menu manager"));

			if(bMoveReady)
				PDASetMenu(hFrameWin, NULL);
			else
				PDASetMenu(hFrameWin, hMenu);
			
			SetFocus(hAppWnd);
		}
		break;
		
    
	case WM_ERASEBKGND:
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
				
				if(bMoveReady)
				{
					int iNewPos =0, nLine =0;
					
					AppFileMove(pAppMain, SrcappId, (short)nFocus, (short)nSrcFocus, CurDirId);
					bMoveReady = FALSE;
					nLine = Menu_PmGetDivideNum(nSrcFocus + 1, 3) + iPos/ITEMHIG - 1;
					
					if(nLine < 0 )
						iNewPos = -nLine * ITEMHIG;
					else if(nLine >2)
						iNewPos = (nLine - 2) *ITEMHIG;
					
					if(iNewPos != 0)
					{
						SendMessage(hAppWnd, WM_VSCROLL, MAKEWPARAM(SB_THUMBPOSITION,iPos+iNewPos), NULL);
					}
					nFocus = nSrcFocus;
					PDASetMenu(hFrameWin, hMenu);
					SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 1, (LPARAM)ML("Move"));
					SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 0, (LPARAM)ML("Back"));
					SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Options");
					SetWindowText(hFrameWin, ML("Menu manager"));
					InvalidateRect(hFrameWin, NULL, TRUE);
				}
				else
				{
					SendMessage(hWnd, WM_CLOSE, NULL, NULL);
					nFocus = 4;
					
				}				
				break;	
			case VK_F5:
				
				if(bMoveReady)//move
				{
					bMoveReady = FALSE;
					PDASetMenu(hFrameWin, hMenu);
					SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 1,	(LPARAM)ML("Move"));
					SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 0, (LPARAM)ML("Back"));
					SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Options");
					SetWindowText(hFrameWin, ML("Menu manager"));
					InvalidateRect(hWnd, NULL, TRUE);
					break;
				}
				else
					PDADefWindowProc(hFrameWin, wMsgCmd, wParam, lParam);
				break;
			case VK_RETURN:
				{
					if(nFiles != 0)
						SendMessage(hWnd, WM_COMMAND , IDM_MOVE, 0);
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
        }
		break;

	case WM_INITMENU:
		{
			int nMenuItem = 0;
			PDIRSNODE pDirNode = NULL;
			PFOLDERLINK p = NULL;
			HMENU  hMainMenu;

			hMainMenu = PDAGetMenu(hFrameWin);
			if(NULL == hMainMenu)
				PDASetMenu(hFrameWin, hMenu);
			else
				hMenu = hMainMenu;
			
			nMenuItem = GetMenuItemCount(hMenu);
			while(nMenuItem > 0)
			{
				nMenuItem--;
				DeleteMenu(hMenu,nMenuItem,MF_BYPOSITION);
			}
			
			if(nFiles ==0)
			{
				PDASetMenu(hFrameWin, NULL);
			}
			else if(nFiles == 1)
			{
				AppendMenu(hMenu,MF_ENABLED, IDM_MOVEFOLDER, IDS_MOVEFOLDER);
				if(pCurAppNode->nType & WAP_SHORTCUT)
				{
					AppendMenu(hMenu,MF_ENABLED, IDM_RENAME, IDS_RENAME);
					AppendMenu(hMenu,MF_ENABLED, IDM_DELETE,IDS_DELETE );
				}
				else
				{
					DeleteMenu(hMenu, IDM_RENAME, MF_BYCOMMAND);
					DeleteMenu(hMenu, IDM_DELETE, MF_BYCOMMAND);
				}
			}
			else
			{
				AppendMenu(hMenu,MF_ENABLED, IDM_MOVE, IDS_MOVE);
				AppendMenu(hMenu,MF_ENABLED, IDM_MOVEFOLDER, IDS_MOVEFOLDER);
				if(pCurAppNode->nType & WAP_SHORTCUT)
				{
					AppendMenu(hMenu,MF_ENABLED, IDM_RENAME, IDS_RENAME);
					AppendMenu(hMenu,MF_ENABLED, IDM_DELETE,IDS_DELETE );
				}
				else
				{
					DeleteMenu(hMenu, IDM_RENAME, MF_BYCOMMAND);
					DeleteMenu(hMenu, IDM_DELETE, MF_BYCOMMAND);
				}
			}
			
		}
		
		break;
	case PMRENAME_IME_MSG:
		{
			char  strDirNewName[PMNODE_NAMELEN + 1];
			char  strDirOldName[PMNODE_NAMELEN + 1];
			
			memset(strDirNewName, 0, PMNODE_NAMELEN + 1);
			memset(strDirOldName, 0, PMNODE_NAMELEN + 1);
			
			strcpy(strDirNewName, (char*)lParam);
			strcpy(strDirOldName, pCurAppNode->aDisplayName);
			
			AppFileRename(pAppMain, strDirNewName, pCurAppNode->appId);
		}
		break;
	case WM_DLMNODEDEL:
		{
			int result = 0;
			
			nFiles --;
			
			if(nFocus == nFiles)
				nFocus--;
			
			result = Menu_PmGetDivideNum(nFiles, 3);
			
			iMax = result*ITEMHIG;
			
			GetScrollInfo(hWnd,SB_VERT,&scinfo);
			scinfo.fMask = SIF_RANGE;
			scinfo.nMax =(LONG)iMax;
			SetScrollInfo(hWnd,SB_VERT,&scinfo,TRUE);
			EnableScrollBar(hWnd, SB_VERT, ESB_ENABLE_BOTH);
			InvalidateRect(hWnd, NULL ,TRUE);
		}
		break;
	case PM_DELETEDIR:
		{
			short dirId = 0;
			PAPPADM pappMain = NULL;
			
			if(LOWORD(lParam) == 0)
				break;
			printf("\r\n lparam != 0");
	
	
			if(( pCurAppNode->nType & WAP_SHORTCUT)
				&& AppFileDelete(hWnd, pAppMain, pCurAppNode->appId))
			{
				int iNewPos = 0, iFocus = 0;
				BOOL iMove =TRUE;
				
				if(nFocus == nFiles - 1)
					nFocus--;
				
				printf("\r\ndelete floder successful!");
				
				PLXTipsWin(NULL, hWnd, NULL, ML("Deleted"), NULL, Notify_Success, ML("Ok"), NULL, WAITTIMEOUT);
				
				printf("\r\n delete folder prompt over");
				
				iMax = (Menu_PmGetDivideNum(nFiles -1, 3))*ITEMHIG;
				
				iFocus = Menu_PmGetDivideNum(nFiles+1, 3) + iPos/ITEMHIG;
				
				if((nFiles -1 + 1)%3 != 1)
					iMove = FALSE;
				
				iNewPos = (Menu_PmGetDivideNum(nFiles -1, 3)+ (iPos-150)/ITEMHIG)*ITEMHIG;
				
				if(iMove && iNewPos < 0)
				{
					SendMessage(hWnd, WM_VSCROLL, MAKEWPARAM(SB_THUMBPOSITION,iNewPos - iPos), NULL);
				}
				else
				{
					GetScrollInfo(hWnd, SB_VERT, &scinfo);
					scinfo.fMask = SIF_RANGE;
					scinfo.nMax = iMax;
					SetScrollInfo(hWnd,SB_VERT,&scinfo,TRUE);
					EnableScrollBar(hWnd, SB_VERT, ESB_ENABLE_BOTH);
				}
			}

			InvalidateRect(hWnd, NULL ,TRUE);
		}
		break;
		
		
	case WM_COMMAND:
		{
			
			switch (wParam)
			{
			case IDB_EXIT:
				PostMessage(hWnd, WM_CLOSE, 0, 0);
				break;
			case IDM_RENAME:
				{
									
					if((pCurAppNode->nType & WAP_SHORTCUT))
					{
						if(MenuIsRunApp(pAppMain, pCurAppNode->appId))
							PLXTipsWin(hFrameWin, hWnd, NULL, ML("Cannot rename. Application in use."),
							NULL, Notify_Failure, ML("Ok"), NULL, DEFAULT_TIMEOUT);
						else
							DirsAppRename(hWnd, APP_TYPE);		
					}
				}
				break;
			case IDM_DELETE:
				{
					char  strDirName[PMNODE_NAMELEN +1];
					char  strPromt[PMNODE_NAMELEN + 10];
					
					memset(strDirName, 0, PMNODE_NAMELEN);
					//need add notification "delete folder?"
					memset(strPromt, 0, PMNODE_NAMELEN +10);

					if(MenuIsRunApp(pAppMain, pCurAppNode->appId))
					{
						PLXTipsWin(hFrameWin, hWnd, NULL, ML("Cannot delete. Application in use."),
							NULL, Notify_Failure, ML("Ok"), NULL, DEFAULT_TIMEOUT);
						break;
					}
					
					strcpy(strDirName, pCurAppNode->aDisplayName);
					
					if(pCurAppNode->aDisplayName[0] !=0)
						sprintf(strPromt, "%s: Delete?", strDirName);
					else
						strcpy(strPromt, "Deleted?");
					
					PLXConfirmWinEx(NULL, hWnd, strPromt,Notify_Request, NULL,
						ML("Yes"), ML("No"), PM_DELETEDIR);
					
				}
				break;

			case IDM_MOVEFOLDER:
				CreateMoveFolderWnd(hWnd);
				break;
			case IDM_MOVE:
				nSrcFocus = nFocus;
				bMoveReady  =TRUE;
                SrcappId = GetAppfileID(GetCurrentApp());
				PDASetMenu(hFrameWin, NULL);
				SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
				SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 0, (LPARAM)ML("Cancel"));
				SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
				SetWindowText(hFrameWin, ML("Move"));
				InvalidateRect(hWnd, NULL, TRUE);
				break;
				
			default:
				break;
			}
		}
		break;
		
	case WM_CLOSE:
		PLXPrintf("\r\nfree folder link !");
		FreeFolderLink();
		PLXPrintf("\r\n free folder over!");
		DestroyWindow(hWnd);
		break;
		
	case WM_DESTROY:
		hAppWnd = NULL;
	    PLXPrintf("\r\n unregister showapp class!");
		UnregisterClass("MenuShowApp_WindowClass", NULL);
		PLXPrintf("\r\n unregister showapp class over!");
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
    
	iMax =  (Menu_PmGetDivideNum(nFiles,3))* ITEMHIG;
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
	COLORREF    oldTxtColor;
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
		Menu_DeskGetRect(&rcIcon,&rcText,nApp);
		
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
					len = strlen(ML(pAppNode->aDisplayName ));
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
					Menu_GetDisplayName(strStkTitle, strTitleName, len);
				else
					Menu_GetDisplayName((char*)ML(pAppNode->aDisplayName), strTitleName, len);
			}
			else
				Menu_GetDisplayName((char*)ML(pAppNode->aDisplayName), strTitleName, len);
			
			DrawText(hdc, ML(strTitleName), -1, &rcText, DT_CENTER | DT_VCENTER);
				
		}
		else
		{
			char strIconName[PMICON_NAMELEN];
			BOOL bDlmEnable = FALSE;
			
			pCurAppNode = pAppNode;
			
			if(!bMoveReady)
				oldTxtColor = SetTextColor(hdc, RGB(255,255,255));
			
			SetRect(&rcFocus, rcIcon.left-8, rcIcon.top - 9,
				rcIcon.left - 8 + 57, rcText.top -9 +50);
			
			if(!bMoveReady)
				DrawImageFromFile(hdc,"/rom/progman/select.gif", rcFocus.left, rcFocus.top, SRCCOPY);
			else
				DrawImageFromFile(hdc,"/rom/progman/app/movebg.gif", rcFocus.left, rcFocus.top, SRCCOPY);

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
					Menu_GetDisplayName(strStkTitle, strTitleName, len);
				else
					Menu_GetDisplayName((char*)ML(pAppNode->aDisplayName), strTitleName, len);
			}
			else
				Menu_GetDisplayName((char*)ML(pAppNode->aDisplayName), strTitleName, len);
			DrawText(hdc, ML(strTitleName), -1, &rcText, DT_CENTER | DT_VCENTER);
		
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
* Function   LoadGrpMenu  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
static void LoadGrpMenu(struct appMain * pAdm)
{
	PDIRSLINK pDir = NULL;
	PDIRSNODE pDirsNode = NULL;
	int i = 1;
	
	hMenu = CreateMenu();
	hSubMenu = CreateMenu();

	FreeFolderLink();
	
	pDir = pAppMain->pDirRoot->pDirChild;
	
	pDirsNode = pAppMain->pDirsCache;
	
	InsertMenu(hSubMenu, 0, MF_BYPOSITION, IDM_FOLDER, pDirsNode->aDisplayName);
	InsertFolderItem((WORD)(IDM_FOLDER + pDirsNode->dirId), (short)0);
	
	while(pDir)
	{
		if(!pDir->pDirNode)
			pDirsNode = pAppMain->pDirsCache + pDir->dirId;
		else
			pDirsNode = pDir->pDirNode;
			
		InsertMenu(hSubMenu, i, MF_BYPOSITION, IDM_FOLDER + pDirsNode->dirId, pDirsNode->aDisplayName);
		InsertFolderItem((WORD)(IDM_FOLDER + pDirsNode->dirId), (short)i);
		
		pDir = pDir->pNext;
		i++;
	}
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
              //  ScrollWindow(hwnd, 0, -move, NULL, NULL);
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
          //  ScrollWindow(hwnd, 0, -iLine , NULL, NULL);
        }       
        break;
    case SB_LINEUP:
        if(iPos >= 0)
        {
            iPos = ioldPos;
            break;
        }
        iPos += iLine;    
       // ScrollWindow(hwnd, 0, iLine, NULL, NULL); 
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
            
          //  ScrollWindow(hwnd,0,-move,NULL,NULL);
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
         //   ScrollWindow(hwnd,0,move,NULL,NULL);
            
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
            
          //  ScrollWindow(hwnd, 0, -move, NULL, NULL);
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
* Function   InsertFolderItem  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
static BOOL InsertFolderItem(WORD mId, short nIndex)
{
	PFOLDERLINK pFolderItem =NULL, pTemp =NULL;

	pFolderItem = malloc(sizeof(FOLDERLINK));

	if(!pFolderItem)
		return FALSE;

	pFolderItem->mId = mId;
	pFolderItem->nIndex = nIndex;
	if(!pGrpHead)
	{
		pGrpHead = pFolderItem;
		pFolderItem->pNext = NULL;
		return TRUE;
	}

	pTemp = pGrpHead;

	while(pTemp->pNext)
	{
		pTemp = pTemp->pNext;
	}
	pTemp->pNext = pFolderItem;
	pFolderItem->pNext =NULL;

	return TRUE;

}
/********************************************************************
* Function   FreeFolderLink  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
static BOOL FreeFolderLink(void)
{
	PFOLDERLINK pTemp =NULL, p =NULL;

	if(!pGrpHead)
	{
		PLXPrintf("\r\n folder head is null!");
		return TRUE;
	}

	pTemp = pGrpHead;

	while(pTemp)
	{
		p = pTemp;
		pTemp = pTemp->pNext;
		PLXPrintf("\r\n free folder data %x !", p);
		free(p);
		p = NULL;
	}

	pGrpHead = NULL;

	return TRUE;
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
	OnArrowKeyDown(hAppWnd, pCurKeydata->nkeyCode);

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

			if((Menu_PmGetDivideNum(nFocus + 1, 3)  + iPos/ITEMHIG ) > 3)
			{
				iNewPos = (Menu_PmGetDivideNum(nFocus + 1, 3) + iPos/ITEMHIG -3)*ITEMHIG;
				SendMessage(hWnd, WM_VSCROLL,
					MAKEWPARAM(SB_THUMBPOSITION,iPos+iNewPos), NULL);
			}
			
			if((nFocus + iPos/ITEMHIG *3) < 0)
			{
				SendMessage(hWnd, WM_VSCROLL, SB_LINEUP, NULL);
			//	break;
			}
			if (nOldFocus != nFocus)
			{
				RECT rc1, rc2;        
				
				Menu_DeskGetGifRect(&rc1, nOldFocus);
				Menu_DeskGetGifRect(&rc2, nFocus);	
				
			}
			if(bMoveReady)
			{
				AppFileMove(pAppMain, SrcappId, (short)nOldFocus, (short)nFocus, CurDirId );
			}
			if(nOldFocus != nFocus || bMoveReady)
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
			
			if((Menu_PmGetDivideNum(nFocus + 1, 3) -1 + iPos/ITEMHIG ) < 0)
			{
				iNewPos =  -(Menu_PmGetDivideNum(nFocus + 1, 3) -1 + iPos/ITEMHIG )*ITEMHIG;
				SendMessage(hWnd, WM_VSCROLL,
					MAKEWPARAM(SB_THUMBPOSITION,iPos+iNewPos), NULL);
			}

			if((nFocus + iPos/ITEMHIG * 3) > 8)
			{
				SendMessage(hWnd, WM_VSCROLL, SB_LINEDOWN, NULL);
			//	break;
			}

			if (nOldFocus != nFocus)
			{
				RECT rc1, rc2;        
				
				Menu_DeskGetGifRect(&rc1, nOldFocus);
				Menu_DeskGetGifRect(&rc2, nFocus);	
			}

			if(bMoveReady)
			{
				AppFileMove(pAppMain, SrcappId, (short)nOldFocus, (short)nFocus, CurDirId );
			}

			if(nOldFocus != nFocus || bMoveReady)
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
				nFocus += (Menu_PmGetDivideNum(nFiles, 3) -1)*3;
				if(nFocus> nFiles-1)
					nFocus -=3;
			}
			
			if((Menu_PmGetDivideNum(nFocus + 1, 3)  + iPos/ITEMHIG ) > 3)
			{
				iNewPos = (Menu_PmGetDivideNum(nFocus + 1, 3) + iPos/ITEMHIG -3)*ITEMHIG;
				SendMessage(hAppWnd, WM_VSCROLL,
					MAKEWPARAM(SB_THUMBPOSITION,iPos+iNewPos), NULL);
			}
			else if((Menu_PmGetDivideNum(nFocus + 1, 3) -1 + iPos/ITEMHIG) < 0)
			{	
				SendMessage(hWnd, WM_VSCROLL, SB_LINEUP, NULL);
			}

			if (nOldFocus != nFocus)
			{
				RECT rc1, rc2;        
				
				Menu_DeskGetGifRect(&rc1, nOldFocus);
				Menu_DeskGetGifRect(&rc2, nFocus);	
			}

			if(bMoveReady)
			{
				AppFileMove(pAppMain, SrcappId, (short)nOldFocus, (short)nFocus, CurDirId );				
			}
			
			if(nOldFocus != nFocus ||bMoveReady)
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
			
			if((Menu_PmGetDivideNum(nFocus + 1, 3) -1 + iPos/ITEMHIG ) < 0)
			{
				iNewPos =  -(Menu_PmGetDivideNum(nFocus + 1, 3) -1 + iPos/ITEMHIG )*ITEMHIG;
				SendMessage(hAppWnd, WM_VSCROLL,
					MAKEWPARAM(SB_THUMBPOSITION,iPos+iNewPos), NULL);
			}
			else if((Menu_PmGetDivideNum(nFocus + 1, 3) + iPos/ITEMHIG ) > 3)
			{
				SendMessage(hWnd, WM_VSCROLL, SB_LINEDOWN, NULL);
			}
			
			if (nOldFocus != nFocus)
			{
				RECT rc1, rc2;        
				
				Menu_DeskGetGifRect(&rc1, nOldFocus);
				Menu_DeskGetGifRect(&rc2, nFocus);	
			}
			
			if(bMoveReady)
			{
				AppFileMove(pAppMain, SrcappId, (short)nOldFocus, (short)nFocus, CurDirId );				
			}
			
			if(nOldFocus != nFocus ||bMoveReady)
			{
				InvalidateRect(hWnd, NULL, TRUE);
				UpdateWindow(hWnd);
			}
		}
		break;
	}
}
#define IDB_MOVEFOLDER  201
#define IDC_MOVEFOLDER_LIST 202
static HWND hListMoveFolder, hFocusMoveFolder, hMoveFolderWnd = NULL;
HBITMAP  hIconNormal, hIconSel;
static int nFolderNum = 0, nFocusDir = 0;
static LRESULT MoveFolderWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam,
                             LPARAM lParam);
static BOOL CreateMoveFolderControl(HWND hWnd, HWND *hList);
/********************************************************************
* Function   CreateMoveFolderWnd  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
static BOOL CreateMoveFolderWnd(HWND hWnd)
{
		
	WNDCLASS wc;
	RECT rect;
	
    wc.style         = 0;
    wc.lpfnWndProc   = MoveFolderWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = "MoveFolderGrpWindowClass";
	
	if(IsWindow(hMoveFolderWnd))
	{
		ShowWindow(hMoveFolderWnd, SW_SHOW);
		UpdateWindow(hMoveFolderWnd);
		
		return TRUE;
	}
    if (!RegisterClass(&wc))
        return FALSE;
	
	GetClientRect(hFrameWin, &rect);
    hMoveFolderWnd = CreateWindow("MoveFolderGrpWindowClass",
        "",
        WS_VISIBLE|WS_CHILD,
		rect.left,rect.top,rect.right-rect.left,rect.bottom-rect.top, 
        hFrameWin,NULL,	NULL,	NULL);
	
    if(hMoveFolderWnd == NULL)
    {
        UnregisterClass("MoveFolderGrpWindowClass", NULL);
        return FALSE;
    }
	
    ShowWindow(hMoveFolderWnd, SW_SHOW);
    UpdateWindow(hMoveFolderWnd);

	return TRUE;
}
/********************************************************************
* Function   MoveFolderWndProc  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
static LRESULT MoveFolderWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam,
                             LPARAM lParam)
{
    LRESULT      lResult  = TRUE;
	HDC          hdc;


    switch(wMsgCmd) 
    {
    case WM_CREATE:
		
		SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(200, 1), 
			(LPARAM)ML(""));
		SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDB_MOVEFOLDER, 0), 
			(LPARAM)ML("Cancel"));
		SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
		CreateMoveFolderControl(hWnd, &hListMoveFolder);
		hdc         = GetDC(hWnd);
        hIconNormal = LoadImageEx(hdc,NULL,ICONORMAL,IMAGE_BITMAP,17,16,LR_LOADFROMFILE);
        hIconSel = LoadImageEx(hdc,NULL,ICONSEL,IMAGE_BITMAP,17,16,LR_LOADFROMFILE);
		ReleaseDC(hWnd,hdc);
		Load_Icon_SetupList(hListMoveFolder,hIconNormal,hIconSel, nFolderNum,nFocusDir);
		hFocusMoveFolder = hListMoveFolder;
		SetFocus(hFocusMoveFolder);
		SetWindowText(hFrameWin, ML("Move to folder"));
    	break;
	case PWM_SHOWWINDOW:
		{
			SendMessage(hFrameWin, PWM_SETBUTTONTEXT, (WPARAM)1, (LPARAM)ML(""));
			SendMessage(hFrameWin, PWM_SETBUTTONTEXT, (WPARAM)0, (LPARAM)ML("Cancel"));
			SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
			SetWindowText(hFrameWin, ML("Move to folder"));
			SetFocus(hFocusMoveFolder);
		}
		break;
  
    case WM_ACTIVATE:
        if (WA_INACTIVE == LOWORD(wParam))
        {
         }
        else
        {
            SetFocus(hFocusMoveFolder);
         }
        break;
    case WM_PAINT:
        {
			PAINTSTRUCT ps;
			hdc = BeginPaint(hWnd, &ps);
			EndPaint(hWnd, NULL);
        }
        break;
	case WM_CLOSE:
		SendMessage(hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hWnd,0); 
		DestroyWindow(hWnd);
		break;

	case WM_TIMER:
		{
			int nFocusItem = 0;
			PAPPNODE pAppNode = NULL;
			int iNewPos =0;
			short dirid = 0, appId = 0, fatherId = 0;
			PAPPADM pappMain = NULL;
			
			nFocusItem = SendMessage(hListMoveFolder, LB_GETCURSEL, 0, 0);
			dirid = (short)SendMessage(hListMoveFolder, LB_GETITEMDATA, nFocusItem, 0);
			
			appId  = GetAppfileID(GetCurrentApp());
			
			pAppNode = pAppMain->pFileCache + appId;
			if(pAppNode && pAppNode->fatherId == dirid)
				break;

			fatherId = pAppNode->fatherId;
			
			AppFileMove(pAppMain, appId, (short)nFocus, 0, dirid);

			pappMain = GetAppMain();
			
			if(dirid==0)
			{
				PostMessage(pappMain->hGrpWnd, WM_PMNODEADD, 0, 0);
				SendMessage(hParantWnd, WM_PMNODEADD, NULL, NULL);
			}
			else
			{
				if(pappMain->hAppWnd && dirid == GetSMenuDirId())
					PostMessage(pappMain->hAppWnd, WM_PMNODEADD, 0, 0);
			}
			
			nFiles --;

			if(pappMain->hAppWnd && fatherId == GetSMenuDirId())
				PostMessage(pappMain->hAppWnd, WM_PMNODEDEL, 0, 0);

			if(nFiles == 0)
			{
				PDASetMenu(hFrameWin, NULL);
				SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
				SendMessage(hFrameWin	, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
			}
			if(nFocus > nFiles - 1)
				nFocus --;
			
			iMax = (Menu_PmGetDivideNum(nFiles,3))*ITEMHIG;
			
			if(-iPos + 150 > iMax)
				iNewPos =150 -iPos -iMax;
			
			if(iNewPos != 0)
				SendMessage(hAppWnd, WM_VSCROLL, MAKEWPARAM(SB_THUMBPOSITION,-iNewPos - iPos), NULL);
			SendMessage(hWnd, WM_CLOSE, NULL, NULL);

		}
		break;
		
    case WM_KEYDOWN:
		
		if(LOWORD(wParam) == VK_F10)
		{
			SendMessage(hWnd, WM_CLOSE, NULL, NULL);
            break;
		}
		else if(LOWORD(wParam) == VK_F5)
		{
			int iIndex = 0;
			
			iIndex = SendMessage(hListMoveFolder,LB_GETCURSEL,0,0);
			if(iIndex == nFocusDir)
				break;
			Load_Icon_SetupList(hListMoveFolder,hIconNormal,hIconSel, nFolderNum,iIndex);

			SetTimer(hWnd, 1, 500, NULL);
			break;
		}
		else
			PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
		
        break;
		
	case WM_COMMAND:
//		if(HIWORD(wParam) == LBN_SELCHANGE)
//		{
//			int iIndex = 0;
//			
//			iIndex = SendMessage(hListMoveFolder,LB_GETCURSEL,0,0);
//			Load_Icon_SetupList(hListMoveFolder,hIconNormal,hIconSel, nFolderNum,nFocusDir);
//		}
//		else
			PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
		break;
		
    case WM_DESTROY:
		if(hIconNormal != NULL)
		{
			DeleteObject(hIconNormal);
			hIconNormal = NULL;
		}

		if(hIconSel != NULL)
		{
			DeleteObject(hIconSel);
			hIconSel = NULL;
		}

		hMoveFolderWnd =NULL;
		hFocusMoveFolder = NULL;
        UnregisterClass("MoveFolderGrpWindowClass", NULL);

		break;
		
    default:
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
    }
	
    return lResult;
}
/********************************************************************
* Function   CreateMoveFolderControl
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
static BOOL CreateMoveFolderControl(HWND hWnd, HWND *hList)
{
	PFOLDERLINK pLink =  NULL;
	PDIRSNODE pDirNode = NULL;
	int i = 0, appId;
	PAPPNODE  pAppNode = NULL;
	RECT     rect;
	
	GetClientRect(hFrameWin, &rect);

	*hList = CreateWindow("LISTBOX","", 
        WS_VISIBLE | WS_CHILD |/* WS_TABSTOP |*/ LBS_BITMAP |WS_VSCROLL,
        0, 0 , rect.right - rect.left, rect.bottom - rect.top,
        hWnd, (HMENU)IDC_MOVEFOLDER_LIST, NULL, NULL );
	
	if(*hList == NULL)
		return FALSE;
	
	if(pGrpHead != NULL)
		pLink = pGrpHead;


	while(pLink != NULL)
	{
		pDirNode = NULL;
		pDirNode = AppGetDirNodeFromId(pAppMain,(short)(pLink->mId -IDM_FOLDER), NULL);
		SendMessage(*hList,LB_ADDSTRING,NULL,(LPARAM)pDirNode->aDisplayName);
		appId  = GetAppfileID(GetCurrentApp());
		
		pAppNode = pAppMain->pFileCache + appId;
		if(pAppNode->fatherId ==pDirNode->dirId )
			nFocusDir = i;
		SendMessage(*hList, LB_SETITEMDATA, i, (LPARAM)pDirNode->dirId);
		pLink = pLink->pNext;
		i++;
	}
	nFolderNum = i;
	SendMessage(*hList,LB_SETCURSEL,nFocusDir,0);
	return TRUE;
}
/********************************************************************
* Function   MenuGetMoveFolderWnd  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
HWND MenuGetMoveFolderGrpWnd(void)
{
	return hMoveFolderWnd;
}
#define  IDC_RENAMEFOLDER_SAVE  401
#define  IDC_RENAMEFOLDER_QUIT  402
#define  IDC_RENAMEFOLDER       403


static BOOL CreateRenameFolderControl(HWND hWnd);
static LRESULT AppRenameFolderWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );

static HWND  hRenameFolderParentWnd = NULL,hRenameFolderWnd = NULL;
static char  strTitle[PMNODE_NAMELEN + 1];
static short nRenameType = DIR_TYPE;

/********************************************************************
* Function   DirsRename  
* Purpose    Rename folder
* Params     
* Return     
* Remarks      
**********************************************************************/
static BOOL  DirsAppRename(HWND hParentWnd, short nType)
{
	WNDCLASS wc;
	RECT rect;

	memset(strTitle, 0, PMNODE_NAMELEN + 1);

	strcpy(strTitle, "Shortcut");

	nRenameType = nType;
	
		
	hRenameFolderParentWnd = hParentWnd;

	wc.style         = CS_OWNDC;
	wc.lpfnWndProc   = AppRenameFolderWndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = NULL;
	wc.hIcon         = NULL;
	wc.hCursor       = NULL;
	wc.hbrBackground = NULL;
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = "RenameFolderWndClass";
	
	if(IsWindow(hRenameFolderWnd))
	{
		ShowWindow(hRenameFolderWnd, SW_SHOW);
		UpdateWindow(hRenameFolderWnd);	
		return TRUE;
	}
	if (!RegisterClass(&wc))
	{
		return FALSE;
	}
    
	GetClientRect(hFrameWin, &rect);
	hRenameFolderWnd = CreateWindow(
		"RenameFolderWndClass", 
		"",
		WS_CHILD |WS_VISIBLE, 
		rect.left,rect.top,rect.right-rect.left,rect.bottom-rect.top,
		hFrameWin, 
		NULL,
		NULL, 
		NULL);
	
	
	if (NULL == hRenameFolderWnd)
	{
		UnregisterClass("RenameFolderWndClass",NULL);
		return FALSE;
	}
	
	ShowWindow(hRenameFolderWnd, SW_SHOW);
	UpdateWindow(hRenameFolderWnd);

	return TRUE;

}
/********************************************************************
* Function   AppRenameFolderWndProc 
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
static LRESULT AppRenameFolderWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
	LRESULT lResult;
	HDC hdc;

    lResult = (LRESULT)TRUE;
    switch ( wMsgCmd )
    {
    case WM_CREATE :
        CreateRenameFolderControl(hWnd);
		SetWindowText(hFrameWin, ML(strTitle));
		PDASetMenu(hFrameWin, NULL);
        break;

	case PWM_SHOWWINDOW:
		{

			SendMessage(hFrameWin, PWM_SETBUTTONTEXT, (WPARAM)1, (LPARAM)ML("Save"));
			
			SendMessage(hFrameWin, PWM_SETBUTTONTEXT, (WPARAM)0, (LPARAM)ML("Cancel"));
			
			SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"");

			if(bMoveReady)
				SetWindowText(hFrameWin, ML("Move"));
			else
				SetWindowText(hFrameWin, ML(strTitle));

			SetFocus(hRenamefolderFocus);
		}
		break;
    case WM_ACTIVATE:
        if (WA_INACTIVE != LOWORD(wParam))
			SetFocus(hRenamefolderFocus);
        break;

    case WM_CLOSE:
		SendMessage(hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hWnd,0); 
        DestroyWindow(hWnd);
        break;
   	case WM_PAINT:
		hdc = BeginPaint( hWnd, NULL);
        EndPaint(hWnd, NULL);
		break;
    case WM_DESTROY :
		hRenamefolderFocus = NULL;
		hRenameFolderWnd = NULL;
        UnregisterClass("RenameFolderWndClass",NULL);
        break;
    case WM_KEYDOWN:
        switch(LOWORD(wParam))
        {
		case VK_RETURN:
			SendMessage(hWnd, WM_COMMAND, IDC_RENAMEFOLDER_SAVE, 0);
			break;
        case VK_F10:
			SendMessage(hWnd, WM_COMMAND, IDC_RENAMEFOLDER_QUIT, 0);
            break;
        default:
            lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        }
        break;

    case WM_COMMAND:
        switch( LOWORD( wParam ))
        {	
		case IDC_RENAMEFOLDER_SAVE:
			{
				char pText[PMNODE_NAMELEN+1];
				int  i = 0;

				pText[0] = 0;
				
				GetWindowText(GetDlgItem(hWnd, IDC_RENAMEFOLDER), pText,
					GetWindowTextLength(GetDlgItem(hWnd, IDC_RENAMEFOLDER)) +1);

				while(pText[i] && pText[i] ==0x20)
				{
					i++;
				}

				if(pText[i] == 0)
					pText[0] = 0;

				if(pText[0] == 0)
				{
					PLXTipsWin(NULL, hWnd, NULL,ML("Please define name"), NULL, Notify_Alert,ML("Ok"),
					NULL, WAITTIMEOUT);
					break;
				}
				
				SendMessage(hRenameFolderParentWnd,PMRENAME_IME_MSG, 0, (LPARAM)pText);

				SendMessage(hWnd, WM_CLOSE, 0, 0);	
			}
			break;
			
		case IDC_RENAMEFOLDER_QUIT:	
			SendMessage(hWnd, WM_CLOSE, 0, 0);
			break;
		}
		break;
		
		default :
			lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
			break;
	}
	return lResult;
}
/********************************************************************
* Function   CreateRenameFolderControl  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
static BOOL CreateRenameFolderControl(HWND hWnd)
{
	
	HWND hNewFolder= NULL;
	IMEEDIT ie_Name;

	int num = 0;
	
	SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, 
		MAKEWPARAM(IDC_RENAMEFOLDER_SAVE,1),(LPARAM)ML("Save") );
	
	SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, 
		(WPARAM)IDC_RENAMEFOLDER_QUIT,(LPARAM)ML("Cancel") );

	SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
	
	memset(&ie_Name, 0, sizeof(IMEEDIT));
    
    ie_Name.hwndNotify   = hWnd;
	ie_Name.dwAttrib |= 0;

    ie_Name.dwAscTextMax = 0;
    ie_Name.dwUniTextMax = 0;
    
    ie_Name.pszImeName = NULL;
    ie_Name.pszTitle = NULL;

	
	hNewFolder = CreateWindow(
		"IMEEDIT",
		ML(pCurAppNode->aDisplayName),
		WS_VISIBLE|WS_CHILD|WS_TABSTOP|ES_TITLE|ES_AUTOHSCROLL,
		0,0, PLX_WIN_WIDTH -2, 70,                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        
		hWnd,
		(HMENU)IDC_RENAMEFOLDER,
		NULL,
		(PVOID)&ie_Name);
	if (NULL == hNewFolder)
		return FALSE;

	SendMessage(hNewFolder, EM_SETTITLE, 0, (LPARAM)ML("New name:"));
	SendMessage(hNewFolder, EM_LIMITTEXT, PMNODE_NAMELEN, 0);
	SendMessage(hNewFolder, EM_SETSEL, -1, -1);

	hRenamefolderFocus = hNewFolder ;
	SetFocus(hRenamefolderFocus);
	
	return TRUE;
}
/********************************************************************
* Function   Menu_GetRenameFolderWnd  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
static HWND Menu_GetRenameFolderWnd(void)
{
	return hRenameFolderWnd;
}
/********************************************************************
* Function   MenuGetCurFolderRunApp  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
static BOOL MenuIsRunApp(struct appMain *pAppMain, short fileID)
{
	PCURAPPLIST *pHead,	pFile = NULL;
	
	pHead = AppFileGetListHead( pAppMain);

	if(!pHead)
		return FALSE;

	pFile = *pHead;

	while (pFile)
	{
		if(pFile->appId == fileID)
		{
			return TRUE;
		}
		pFile = pFile->pNext;
	}

	return FALSE;	
}
/********************************************************************
* Function   SetGrpGif  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
static int SetGrpGif(HWND hWnd)
{
    int         nDir = 0;
    PDIRSLINK	pDir = NULL;
    PDIRSNODE	pNode = NULL;	
    RECT		rcIcon;
    
    
    pDir  = pAppMain->pDirRoot->pDirChild;
    nDir = 0;

    while (pDir != NULL)
    {   	
        pNode= AppGetDirNodeFromId(pAppMain, pDir->dirId, pDir);
        
        if (!pNode->flags)
        {
            pDir = pDir->pNext;
            continue;
        }
        
        if (nDir == nFocus)
        {            	
            Menu_DeskGetGifRect(&rcIcon, nDir);
           
            return 1;
        }
        
        nDir++;
        pDir = pDir->pNext;
    }	  	
    return 0;
}
