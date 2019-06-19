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

#include "FMenu.h"

/********************************************************************
* Function   RegisteShowGrpClass  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
static BOOL RegisteShowGrpClass(void)
{
    WNDCLASS wc;	
    
    wc.style         = 0;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;//LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = NULL;
    wc.lpszMenuName  = NULL;
    
    wc.lpfnWndProc	 = ShowGrp_WndProc;
    wc.lpszClassName = "MenuShowGrp_WindowClass";
    
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
* Function   MenuManagerCtrl  
* Purpose    create grp window
* Params     
* Return     
* Remarks      
**********************************************************************/
BOOL MenuManagerCtrl( HWND hParentWnd)
{
    HWND	hWnd;
	PDIRSNODE pDirNode = NULL;
	PDIRSLINK pDir = NULL;
	int      i = 0;
	RECT    rect;

	hFrameWin = hParentWnd;

	nFocus = 4;

    pAppMain = GetAppMain();
    
    RegisteShowGrpClass();    
    
    GetGroupNodeNum();

	if(nGroupNum < nFocus)
		nFocus = nGroupNum -1;

	hMenu = CreateMenu();
	hSubMenu = CreateMenu();
	
	pDir = pAppMain->pDirRoot->pDirChild;

	while(pDir)
	{
		if(!pDir->pDirNode)
			pDirNode = pAppMain->pDirsCache + pDir->dirId;
		else
			pDirNode = pDir->pDirNode;

		InsertMenu(hSubMenu, i, MF_BYPOSITION, IDM_FOLDER + pDirNode->dirId, pDirNode->aDisplayName);		
		InsertFolderItem((WORD)(IDM_FOLDER + pDirNode->dirId), (short)i);
			
		pDir = pDir->pNext;
		i++;
	}
	ItemNum = i;

	GetClientRect(hFrameWin, &rect);
	
    hWnd = CreateWindow("MenuShowGrp_WindowClass",
		"",
		WS_VISIBLE |WS_VSCROLL|WS_CHILD,
		rect.left,rect.top,rect.right-rect.left,rect.bottom-rect.top,
		hParentWnd,	
		NULL,
		NULL,	NULL);

    hGrpWnd = hWnd;
  
    SetFocus(hWnd);    
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
       
		if(GetFocusType(pAppMain, nFocus) == DIR_TYPE)
			SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDM_OPENMOVE, 1), 
           (LPARAM)ML("Open"));
		else
			SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDM_OPENMOVE, 1), 
           (LPARAM)ML("Move"));
		
		
        SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDB_EXIT, 0), 
           (LPARAM)ML("Back"));

		SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Options");

		SetWindowText(hFrameWin,ML("Menu manager"));
	
		InitVscrollInfo(hWnd);

		PDASetMenu(hFrameWin, hMenu);
		
		hGrpFocus = hWnd;
		
        break;
	case PWM_SHOWWINDOW:
		{
			if(bMoveReady)
			{
				SendMessage(hFrameWin, PWM_SETBUTTONTEXT, (WPARAM)1, (LPARAM)ML(""));
				SendMessage(hFrameWin, PWM_SETBUTTONTEXT, (WPARAM)0, (LPARAM)ML("Cancel"));
				SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
				
				SetWindowText(hFrameWin,ML("Move"));
				
				PDASetMenu(hFrameWin,NULL);
			}
			else
			{
				if(GetFocusType(pAppMain, nFocus) == DIR_TYPE)
					SendMessage(hFrameWin, PWM_SETBUTTONTEXT, (WPARAM)1, (LPARAM)ML("Open"));
				else
					SendMessage(hFrameWin, PWM_SETBUTTONTEXT, (WPARAM)1, (LPARAM)ML("Move"));
				
				SendMessage(hFrameWin, PWM_SETBUTTONTEXT, (WPARAM)0, (LPARAM)ML("Back"));
				
				SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Options");
				
				SetWindowText(hFrameWin,ML("Menu manager"));
				
				PDASetMenu(hFrameWin,hMenu);
			}
			SetFocus(hGrpFocus);
			
		}
		break;
	case WM_PMNODEADD:
		{
			int result = 0;

			nGroupNum ++;

			result = Menu_PmGetDivideNum(nGroupNum, 3);
		
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
			
			result = Menu_PmGetDivideNum(nGroupNum, 3);
			
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
		{
			int result = 0;
			
			nGroupNum --;

			if(nFocus == nGroupNum)
				nFocus--;
			
			result = Menu_PmGetDivideNum(nGroupNum, 3);
			
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
	case WM_CLOSE:
		SendMessage(hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hWnd,0); 
		DestroyWindow(hWnd);
		break;
  
    case WM_DESTROY:
        {
			FreeFolderLink();
			hGrpWnd = NULL;
			UnregisterClass("MenuShowGrp_WindowClass",NULL);
            break;
        }
        
    case WM_KEYDOWN:
        
		switch(LOWORD(wParam)) 
		{
		case VK_F10:
			{
				if(bMoveReady)
				{
					int iNewPos = 0, nLine = 0;
					
					PDASetMenu(hFrameWin, hMenu);
					SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 1, (LPARAM)ML("Open"));
					SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 0, (LPARAM)ML("Back"));
					SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Options");
					SetWindowText(hFrameWin, ML("Manu manager"));
					bMoveReady = FALSE;
					
					if(nSrcType == DIR_TYPE)
					{
						AppDirsMove( pAppMain, pSrcDirsNode->dirId, (short)nFocus, (short)nSrcFocus,
							GetFocusType(pAppMain, nSrcFocus));
					}
					else
					{
						AppFileMove(pAppMain, pSrcAppNode->appId, (short)nFocus, (short)nSrcFocus, 0 );
					}
					
					nLine = Menu_PmGetDivideNum(nSrcFocus + 1, 3) + iPos/ITEMHIG - 1;
					
					if(nLine < 0 )
						iNewPos = -nLine * ITEMHIG;
					else if(nLine >2)
						iNewPos = (nLine - 2) *ITEMHIG;
					
					if(iNewPos != 0)
					{
						SendMessage(hGrpWnd, WM_VSCROLL, MAKEWPARAM(SB_THUMBPOSITION,iPos+iNewPos), NULL);
					}
					nFocus = nSrcFocus;
					if(!bMoveReady)
					{
						if(GetFocusType(pAppMain, nFocus) == DIR_TYPE)
							SendMessage(hFrameWin, PWM_SETBUTTONTEXT, (WPARAM)1, (LPARAM)ML("Open"));
						else
							SendMessage(hFrameWin, PWM_SETBUTTONTEXT, (WPARAM)1, (LPARAM)ML("Move"));
					}
					InvalidateRect(hWnd, NULL, TRUE);
					
					break;
				}
				else
				{
					PostMessage(hGrpWnd, WM_CLOSE, 0, 0);
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
	case WM_INITMENU:
		{
			int nMenuItem = 0;
			PFOLDERLINK p =NULL;
			PDIRSNODE pDirNode = NULL;
		
			
			hMenu = PDAGetMenu(hFrameWin);
			if(NULL == hMenu)
				break;
			nMenuItem = GetMenuItemCount(hMenu);
			while(nMenuItem > 0)
			{
				nMenuItem--;
				DeleteMenu(hMenu,nMenuItem,MF_BYPOSITION);
			}
					
			if(nCurType == DIR_TYPE)
				AppendMenu(hMenu,MF_ENABLED, IDM_OPEN, IDS_OPEN);
			AppendMenu(hMenu,MF_ENABLED, IDM_MOVE, IDS_MOVE );
			
			if(nCurType == DIR_TYPE)
			{
				PDIRSLINK pDir = NULL;
				
				DeleteMenu(hMenu,IDM_MOVEFOLDER, MF_BYCOMMAND);
				AppendMenu(hMenu,MF_ENABLED, IDM_NEWFOLDER, IDS_NEWFOLDER);
				AppendMenu(hMenu,MF_ENABLED, IDM_RENAME, IDS_RENAME);
				
				pDir = AppDirsFind(pAppMain->pDirRoot,pCurDirsNode->dirId);
				
				if(!pCurDirsNode->dwstyle  || (pDir && pDir->pAppLink))
				{
					DeleteMenu(hMenu, IDM_DELETE, MF_BYCOMMAND);
				}
				else
				{
					AppendMenu(hMenu,MF_ENABLED, IDM_DELETE,IDS_DELETE );
				}
			}
			else
			{
				if(nCurType == APP_TYPE)
				{
					AppendMenu(hMenu,MF_ENABLED, IDM_MOVEFOLDER,IDS_MOVEFOLDER);
				}

				AppendMenu(hMenu,MF_ENABLED, IDM_NEWFOLDER, IDS_NEWFOLDER);
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
	case WM_ERASEBKGND:
		break;

	case PMRENAME_IME_MSG:
		{
			char  strDirNewName[PMNODE_NAMELEN + 1];
			char  strDirOldName[PMNODE_NAMELEN + 1];

			memset(strDirNewName, 0, PMNODE_NAMELEN + 1);
			memset(strDirOldName, 0, PMNODE_NAMELEN + 1);

			strcpy(strDirNewName, (char*)lParam);
			strcpy(strDirOldName, pCurDirsNode->aDisplayName);

			if(wParam == 0)
				AppFileRename(pAppMain, strDirNewName, pCurAppNode->appId);
			else if(wParam == 1)
			{
				AppDirsRename(pAppMain, strDirNewName, pCurDirsNode->dirId);
			}
		}
		break;
	case PMNEW_IME_MSG:
		{
			char  strNewDirName[PMNODE_NAMELEN +1];
			short  dirId =0;
			PAPPADM pappMain = NULL;

			if(wParam == 0)
				return FALSE;
			
			memset(strNewDirName, 0, PMNODE_NAMELEN + 1);

			strcpy(strNewDirName, (char*)lParam);

			dirId =AppDirsCreate(pAppMain, 0, (short)0, DIRS_NODE_VALID,strNewDirName ,
				DEFAULT_DIR_ICON,strNewDirName,TRUE);

			if(dirId > 0)
			{
				int  iNewPos = 0;
				PDIRSNODE pDirNode = NULL;

				nGroupNum ++;
				
				pappMain = GetAppMain();
				PostMessage(pappMain->hGrpWnd, WM_DLMNODEADD, 0, 0);

				nFocus = nGroupNum - 1;

				iMax = Menu_PmGetDivideNum(nGroupNum, 3) *ITEMHIG;

				iNewPos = (Menu_PmGetDivideNum(nGroupNum, 3) + (iPos-150)/ITEMHIG)*ITEMHIG;
			
				if(iNewPos != 0)
				{
					SendMessage(hWnd, WM_VSCROLL, MAKEWPARAM(SB_THUMBPOSITION,iNewPos - iPos), NULL);
				}

				pDirNode = AppGetDirNodeFromId(pAppMain, dirId, NULL);
				
				InsertFolderItem((WORD)(IDM_FOLDER+pDirNode->dirId), (short)ItemNum);
				ItemNum ++;
			}
		}
		break;
	case PM_DELETEDIR:
		{
			short dirId = 0;
			char  strDirName[PMNODE_NAMELEN +1];
			PAPPADM pappMain = NULL;
			
			if(LOWORD(lParam) == 0)
				break;
			printf("\r\n lparam != 0");

		//	WaitWin(hFrameWin, TRUE, ML("Deleting..."),NULL, ML("Ok"), NULL, NULL);
			
			dirId = pCurDirsNode->dirId;
			memset(strDirName, 0, PMNODE_NAMELEN);
			strcpy(strDirName, pCurDirsNode->aDisplayName);
			
			printf("\r\nenter delete folder first!");
						
			nCurType = GetFocusType(pAppMain, nFocus);
			
			if(nCurType == DIR_TYPE)
			{
				if(AppDirsDelete(pAppMain, pCurDirsNode->dirId))
				{
					int iNewPos = 0, iFocus = 0;
					BOOL iMove =TRUE;
					PAPPADM pappMain = NULL;
					
					if(nFocus == nGroupNum - 1)
						nFocus--;
					
					nGroupNum --;
					
					pappMain = GetAppMain();
					PostMessage(pappMain->hGrpWnd, WM_DLMNODEDEL, 0, 0);
					
					printf("\r\ndelete floder successful!");

					//WaitWin(hFrameWin, FALSE, ML("Deleting..."),NULL, ML("Ok"), NULL, NULL);
					
					PLXTipsWin(NULL, hWnd, NULL, ML("Deleted"), NULL, Notify_Success, ML("Ok"), NULL, WAITTIMEOUT);
					
					printf("\r\n delete folder prompt over");
					
					iMax = (Menu_PmGetDivideNum(nGroupNum, 3))*ITEMHIG;
					
					iFocus = Menu_PmGetDivideNum(nFocus+1, 3) + iPos/ITEMHIG;
					
					if((nGroupNum + 1)%3 != 1)
						iMove = FALSE;
					
					iNewPos = (Menu_PmGetDivideNum(nGroupNum, 3)+ (iPos-150)/ITEMHIG)*ITEMHIG;
					
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
					DeleteFolderItem((WORD)(IDM_FOLDER+dirId));
				}
			
			}
			else
			{
				if(( pCurAppNode->nType & WAP_SHORTCUT)
					&& AppFileDelete(hWnd, pAppMain, pCurAppNode->appId))
				{
					int iNewPos = 0, iFocus = 0;
					BOOL iMove =TRUE;
					
					if(nFocus == nGroupNum - 1)
						nFocus--;
					
				//	nGroupNum --;
					
					printf("\r\ndelete floder successful!");
						
					PLXTipsWin(NULL, hWnd, NULL, ML("Deleted"), NULL, Notify_Success, ML("Ok"), NULL, WAITTIMEOUT);
					
					printf("\r\n delete folder prompt over");
					
					iMax = (Menu_PmGetDivideNum(nGroupNum -1, 3))*ITEMHIG;
					
					iFocus = Menu_PmGetDivideNum(nFocus+1, 3) + iPos/ITEMHIG;
					
					if((nGroupNum -1 + 1)%3 != 1)
						iMove = FALSE;
					
					iNewPos = (Menu_PmGetDivideNum(nGroupNum -1, 3)+ (iPos-150)/ITEMHIG)*ITEMHIG;
					
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
			
			}
			InvalidateRect(hWnd, NULL ,TRUE);
		}
		break;
	    case WM_COMMAND:
		{
			switch(LOWORD(wParam)) 
			{
			case IDB_EXIT:
				//return idle state
			    PostMessage(hWnd, WM_CLOSE, 0, 0);
				break;
			case IDM_OPENMOVE:
				{
					if(nCurType == APP_TYPE)
						SendMessage(hWnd, WM_COMMAND, IDM_MOVE, 0);
					else if(nCurType == DIR_TYPE)
						DeskMenu_ShowApp(pAppMain, pCurDirsNode->dirId,hGrpWnd);	
				}
				break;
			case IDM_OPEN:
				if(nCurType == DIR_TYPE)
					DeskMenu_ShowApp(pAppMain, pCurDirsNode->dirId,hGrpWnd);
				break;
			case IDM_MOVEFOLDER:
				CreateMoveFolderWnd(hWnd);
				break;
			case IDM_MOVE:
				nSrcFocus = nFocus;
				nSrcType = nCurType;
				pSrcDirsNode = pCurDirsNode;
				pSrcAppNode = pCurAppNode;
				bMoveReady = TRUE;
				PDASetMenu(hFrameWin, NULL);
				SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
				SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 0, (LPARAM)ML("Cancel"));
				SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
				SetWindowText(hFrameWin, ML("Move"));
				InvalidateRect(hWnd, NULL, TRUE);
				break;
			case IDM_NEWFOLDER:
				if(SPACE_AVAILABLE !=MenuPM_IsFlashAvailable(MIN_DIR_NODE))
				{
					if(nCurType == APP_TYPE)
						PLXTipsWin(NULL, hWnd, NULL,ML("Not enough\r\n memory"), pCurAppNode->aDisplayName,
						Notify_Failure,	ML("Ok"), NULL, WAITTIMEOUT);
					else if(nCurType == DIR_TYPE)
						PLXTipsWin(NULL, hWnd, NULL,ML("Not enough\r\n memory"), pCurDirsNode->aDisplayName,
						Notify_Failure,	ML("Ok"), NULL, WAITTIMEOUT);
				}
				else
					DirsNew(hWnd);
				break;
				
			case IDM_RENAME:
				{
					nCurType = GetFocusType(pAppMain, nFocus);

					if(nCurType == DIR_TYPE)
					{
						if(!MenuGetCurFolderRunApp(pAppMain, pCurDirsNode->dirId))
							DirsAppRename(hWnd, DIR_TYPE);
						else
							PLXTipsWin(hFrameWin, hGrpWnd, NULL, ML("Cannot rename. Folder in use."),
							NULL,Notify_Failure,ML("Ok"), NULL,DEFAULT_TIMEOUT);
					}
					else
					{
						if((pCurAppNode->nType & WAP_SHORTCUT) || (pCurAppNode->nType & DLM_APP))
						{
							if(MenuIsRunApp(pAppMain, pCurAppNode->appId))
								PLXTipsWin(hFrameWin, hGrpWnd, NULL, ML("Cannot rename. Application in use."),
								NULL, Notify_Failure, ML("Ok"), NULL, DEFAULT_TIMEOUT);
							else
								DirsAppRename(hGrpWnd, APP_TYPE);

						}

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

					if(nCurType == DIR_TYPE)
					{
						if(MenuGetCurFolderRunApp(pAppMain, pCurDirsNode->dirId))
						{
							PLXTipsWin(hFrameWin, hGrpWnd, NULL, ML("Cannot delete. Folder in use."),
							NULL,Notify_Failure,ML("Ok"), NULL,DEFAULT_TIMEOUT);
							break;
						}
						strcpy(strDirName, pCurDirsNode->aDisplayName);

						if(pCurDirsNode->aDisplayName[0] !=0)
							sprintf(strPromt, ML("%s: Delete?"), strDirName);
						else
							strcpy(strPromt, "Deleted?");
						
						PLXConfirmWinEx(NULL, hWnd, strPromt,Notify_Request, NULL,
							ML("Yes"), ML("No"), PM_DELETEDIR);
					}
					else
					{
						if(MenuIsRunApp(pAppMain, pCurAppNode->appId))
						{
							PLXTipsWin(hFrameWin, hGrpWnd, NULL, ML("Cannot delete. Application in use."),
								NULL, Notify_Failure, ML("Ok"), NULL, DEFAULT_TIMEOUT);
							break;
						}

						strcpy(strDirName, pCurAppNode->aDisplayName);

						if(pCurAppNode->aDisplayName[0] !=0)
							sprintf(strPromt, ML("%s: Delete?"), strDirName);
						else
							strcpy(strPromt, "Deleted?");
						
						PLXConfirmWinEx(NULL, hWnd, strPromt,Notify_Request, NULL,
							ML("Yes"), ML("No"), PM_DELETEDIR);
						
					}
				}
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
			
			if((Menu_PmGetDivideNum(nFocus + 1, 3)  + iPos/ITEMHIG ) > 3)
			{
				iNewPos = (Menu_PmGetDivideNum(nFocus + 1, 3) + iPos/ITEMHIG -3)*ITEMHIG;
				SendMessage(hGrpWnd, WM_VSCROLL,
					MAKEWPARAM(SB_THUMBPOSITION,iPos+iNewPos), NULL);
			}
			
			if((nFocus + iPos/ITEMHIG *3) < 0)
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
				if(nSrcType == DIR_TYPE)
				{
					nCurType = GetFocusType(pAppMain, nFocus);
					AppDirsMove( pAppMain, pSrcDirsNode->dirId, (short)nOldFocus, (short)nFocus, nCurType);
					nCurType = DIR_TYPE;
				}
				else
				{
					AppFileMove(pAppMain, pSrcAppNode->appId, (short)nOldFocus, (short)nFocus, 0 );
					nCurType = APP_TYPE;
				}
			}
			if(nOldFocus != nFocus ||bMoveReady)
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
			
			if((Menu_PmGetDivideNum(nFocus + 1, 3) -1 + iPos/ITEMHIG ) < 0)
			{
				iNewPos =  -(Menu_PmGetDivideNum(nFocus + 1, 3) -1 + iPos/ITEMHIG )*ITEMHIG;
				SendMessage(hGrpWnd, WM_VSCROLL,
					MAKEWPARAM(SB_THUMBPOSITION,iPos+iNewPos), NULL);
			}

			if((nFocus + iPos/ITEMHIG * 3) > 8)
			{
				PLXPrintf("\r\n stop move \r\n");
				SendMessage(hGrpWnd, WM_VSCROLL, SB_LINEDOWN, NULL);
				//break;
			}
			if (nOldFocus != nFocus)
			{
				RECT rc1, rc2;        
				
				Menu_DeskGetGifRect(&rc1, nOldFocus);
				Menu_DeskGetGifRect(&rc2, nFocus);	
			}
			if(bMoveReady)
			{
				if(nSrcType == DIR_TYPE)
				{
					nCurType = GetFocusType(pAppMain, nFocus);
					AppDirsMove( pAppMain, pSrcDirsNode->dirId, (short)nOldFocus, (short)nFocus, nCurType);
					nCurType = DIR_TYPE;
				}
				else
				{
					AppFileMove(pAppMain, pSrcAppNode->appId, (short)nOldFocus, (short)nFocus, 0 );
					nCurType = APP_TYPE;
				}
			}
			if(nOldFocus != nFocus ||bMoveReady)
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
				nFocus += (Menu_PmGetDivideNum(nGroupNum, 3) -1)*3;
				if(nFocus> nGroupNum -1)
					nFocus -=3;
			}
			
			if((Menu_PmGetDivideNum(nFocus + 1, 3)  + iPos/ITEMHIG ) > 3)
			{
				iNewPos = (Menu_PmGetDivideNum(nFocus + 1, 3) + iPos/ITEMHIG -3)*ITEMHIG;
				SendMessage(hGrpWnd, WM_VSCROLL,
					MAKEWPARAM(SB_THUMBPOSITION,iPos+iNewPos), NULL);
			}
			else if((Menu_PmGetDivideNum(nFocus + 1, 3) -1 + iPos/ITEMHIG) < 0)
			{	
				SendMessage(hGrpWnd, WM_VSCROLL, SB_LINEUP, NULL);
			}
			if (nOldFocus != nFocus)
			{
				RECT rc1, rc2;        
				
				Menu_DeskGetGifRect(&rc1, nOldFocus);
				Menu_DeskGetGifRect(&rc2, nFocus);	
			}
			if(bMoveReady)
			{
				if(nSrcType == DIR_TYPE)
				{
					nCurType = GetFocusType(pAppMain, nFocus);
					AppDirsMove( pAppMain, pSrcDirsNode->dirId, (short)nOldFocus, (short)nFocus, nCurType);
					nCurType = DIR_TYPE;
				}
				else
				{
					AppFileMove(pAppMain, pSrcAppNode->appId, (short)nOldFocus, (short)nFocus, 0 );
					nCurType = APP_TYPE;
				}
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
			if((Menu_PmGetDivideNum(nFocus + 1, 3) -1 + iPos/ITEMHIG ) < 0)
			{
				iNewPos =  -(Menu_PmGetDivideNum(nFocus + 1, 3) -1 + iPos/ITEMHIG )*ITEMHIG;
				SendMessage(hGrpWnd, WM_VSCROLL,
					MAKEWPARAM(SB_THUMBPOSITION,iPos+iNewPos), NULL);
			}
			else if((Menu_PmGetDivideNum(nFocus + 1, 3) + iPos/ITEMHIG ) > 3)
			{
				SendMessage(hGrpWnd, WM_VSCROLL, SB_LINEDOWN, NULL);
			}
			if (nOldFocus != nFocus)
			{
				RECT rc1, rc2;        
				
				Menu_DeskGetGifRect(&rc1, nOldFocus);
				Menu_DeskGetGifRect(&rc2, nFocus);	
			}
			if(bMoveReady)
			{
				if(nSrcType == DIR_TYPE)
				{
					nCurType = GetFocusType(pAppMain, nFocus);
					AppDirsMove( pAppMain, pSrcDirsNode->dirId, (short)nOldFocus, (short)nFocus, nCurType);
					nCurType = DIR_TYPE;
				}
				else
				{
					AppFileMove(pAppMain, pSrcAppNode->appId, (short)nOldFocus, (short)nFocus, 0 );
					nCurType = APP_TYPE;
				}
			}
			if(nOldFocus != nFocus ||bMoveReady)
			{
				InvalidateRect(hWnd, NULL, TRUE);
				UpdateWindow(hWnd);
			}
		}
		break;
	}
	if(!bMoveReady)
	{
		if(GetFocusType(pAppMain, nFocus) == DIR_TYPE)
			SendMessage(hFrameWin, PWM_SETBUTTONTEXT, (WPARAM)1, (LPARAM)ML("Open"));
		else
			SendMessage(hFrameWin, PWM_SETBUTTONTEXT, (WPARAM)1, (LPARAM)ML("Move"));
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
		if(bMoveReady)
		{
			bMoveReady = FALSE;
			PDASetMenu(hFrameWin, hMenu);
			if(GetFocusType(pAppMain, nFocus) == DIR_TYPE)
				SendMessage(hFrameWin, PWM_SETBUTTONTEXT, (WPARAM)1, (LPARAM)ML("Open"));
			else
				SendMessage(hFrameWin, PWM_SETBUTTONTEXT, (WPARAM)1, (LPARAM)ML("Move"));
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
		if(nCurType == APP_TYPE)
			SendMessage(hWnd, WM_COMMAND, IDM_MOVE, 0);//AppFileOpen(pAppMain,pCurAppNode->appId, TRUE, 0);
		else if(nCurType == DIR_TYPE)
			DeskMenu_ShowApp(pAppMain, pCurDirsNode->dirId,hGrpWnd);
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

			if(bMoveReady)
				break;
			mId = LOWORD(wParam);
			nIndex = mId - 0x31;
			nFocus =  nIndex - iPos/ITEMHIG * 3;
			if(nFocus > nGroupNum  -1)
				break;

			FindItem(pAppMain, nFocus);

			if(nCurType == APP_TYPE)
				AppFileOpen(pAppMain, pCurAppNode->appId, TRUE, 0);
			else if(nCurType == DIR_TYPE)
				DeskMenu_ShowApp(pAppMain, pCurDirsNode->dirId,hGrpWnd);

		}
		break;
	default:
		PDADefWindowProc(hFrameWin, wMsgCmd, wParam, lParam);
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
	OnArrowKeyDown(hGrpWnd, pCurKeydata->nkeyCode);

}
/********************************************************************
* Function   GetFMenuWnd  
* Purpose    paint first menu
* Params     
* Return     
* Remarks      
**********************************************************************/
HWND GetFMenuWnd(void)
{
	return hGrpWnd;
}
/********************************************************************
* Function   DirShow  
* Purpose    paint first menu
* Params     
* Return     
* Remarks      
**********************************************************************/
void MenuGetDlmFocusIconName(char *strFocusIconName, char* strIconName);
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

	nOldmode = GetBkMode(hdc);
	oldTxtColor = GetTextColor(hdc);

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
			
			if(pAppNode->iIndex == nDir)
			{
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
				nType = DIR_TYPE;
				pDir = pDir->pNext;
				goto PMLIST;
			}
		}

PMLIST:
		Menu_DeskGetRect(&rcIcon,&rcText,nDir);
		
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
				Menu_GetDisplayName((char*)ML(pNode->aDisplayName), strDirName, len);
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
					GetTextExtentExPoint(hdc,ML(pAppNode->aDisplayName) , len, rcText.right- rcText.left, 
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
						Menu_GetDisplayName(strStkTitle, strDirName, len);
					else
						Menu_GetDisplayName((char*)ML(pAppNode->aDisplayName), strDirName, len);
				}
				else
					Menu_GetDisplayName((char*)ML(pAppNode->aDisplayName), strDirName, len);
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

			if(!bMoveReady)
				DrawImageFromFile(hdc,"/rom/progman/select.gif", rcFocus.left, rcFocus.top, SRCCOPY);
			else
				DrawImageFromFile(hdc,"/rom/progman/app/movebg.gif", rcFocus.left, rcFocus.top, SRCCOPY);

			memset(strIconName, 0, PMICON_NAMELEN);

			if(!bMoveReady)
				oldTxtColor = SetTextColor(hdc, RGB(255,255,255));

			if(nType == DIR_TYPE && pNode->iIndex == nDir)
			{
				
			//	sprintf(strIconName, "%s%s%s", PMPIC_PATH, "mg", &pNode->aIconName[PMPICPATHLEN +1]);
				MenuGetDlmFocusIconName(strIconName, pNode->aIconName);
					
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
				Menu_GetDisplayName((char*)ML(pNode->aDisplayName), strDirName, len);
				SetBkMode(hdc, TRANSPARENT);
				DrawText(hdc, ML(strDirName),-1, &rcText, DT_CENTER | DT_VCENTER);
		//		if(!bMoveReady)
//				{
//				   SetWindowText(hWnd, ML("Menu manager"));
//				}
//				else
	//				SetWindowText(hWnd, ML("Move"));
			}
			else if(nType == APP_TYPE && pAppNode->iIndex == nDir)
			{
				BOOL bDlmEnable = FALSE;
			//	sprintf(strIconName, "%s%s%s", PMPIC_PATH, "mg",&pAppNode->cIconName[PMPICPATHLEN +1]);
				MenuGetDlmFocusIconName(strIconName, pAppNode->cIconName);
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
						len = strlen(ML(pAppNode->aDisplayName) );
						GetTextExtentExPoint(hdc,ML(pAppNode->aDisplayName) , len, rcText.right- rcText.left, 
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
				memset(strDirName, 0 ,PMNODE_NAMELEN + 1);
				if(IfStkInitSucc() && stricmp(pAppNode->aDisplayName, "Toolkit") == NULL)
				{
					char strStkTitle[100+1];
					
					strStkTitle[0] = 0;
					STK_GetMainText(strStkTitle);
					if(strStkTitle[0] != 0)
						Menu_GetDisplayName(strStkTitle, strDirName, len);
					else
						Menu_GetDisplayName((char*)ML(pAppNode->aDisplayName), strDirName, len);
				}
				else
					Menu_GetDisplayName((char*)ML(pAppNode->aDisplayName), strDirName, len);
				SetBkMode(hdc, TRANSPARENT);
				DrawText(hdc, ML(strDirName),-1, &rcText, DT_CENTER | DT_VCENTER);
//				if(!bMoveReady)
//				{
//					SetWindowText(hWnd, ML("Menu manager"));
//				}
//				else
//					SetWindowText(hWnd, ML("Move"));
			}
			
		}
		nDir++;
	} 

	SetTextColor(hdc, oldTxtColor);
    SetBkMode(hdc, nOldmode);
    return 0;
}


/********************************************************************
* Function   Menu_DeskGetRect  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
 void Menu_DeskGetRect(RECT *rcRect, RECT *rcText, int iCurr)
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
* Function   Menu_DeskGetGifRect  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
void Menu_DeskGetGifRect(RECT *rcRect, int iCurr)
{
	RECT rcText;

	Menu_DeskGetRect(rcRect, &rcText, iCurr);

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
    

    iMax =  (Menu_PmGetDivideNum(nGroupNum, 3))* ITEMHIG;
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
               // ScrollWindow(hWnd, 0, -move, NULL, NULL);
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
         //   ScrollWindow(hWnd, 0, -iLine , NULL, NULL);
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
      //  ScrollWindow(hWnd, 0, iLine, NULL, NULL); 
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
            
         //   ScrollWindow(hWnd,0,-move,NULL,NULL);
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
         //   ScrollWindow(hWnd,0,move,NULL,NULL);
            
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
            
            //ScrollWindow(hWnd, 0, -move, NULL, NULL);
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

	if(nType == DIR_TYPE)
		strcpy(strTitle, "Folder");
	else
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
				if(nRenameType == DIR_TYPE)
					SendMessage(hRenameFolderParentWnd,PMRENAME_IME_MSG, 1, (LPARAM)pText);
				else
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
	char strName[PMNODE_NAMELEN + 1];

	int num = 0;

	strName[0] = 0 ;

	if(stricmp(strTitle, "Folder") == NULL)
		strcpy(strName, pCurDirsNode->aDisplayName);
	if(stricmp(strTitle, "Shortcut") == NULL)
		strcpy(strName, pCurAppNode->aDisplayName);
	
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
		ML(strName),
		WS_VISIBLE|WS_CHILD|WS_TABSTOP/*|WS_BORDER*/|ES_TITLE|ES_AUTOHSCROLL,
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
#define  IDC_NEWFOLDER_SAVE  401
#define  IDC_NEWFOLDER_QUIT  402
#define  IDC_NEWFOLDER       403

static BOOL CreateNewFolderControl(HWND hWnd);
static LRESULT AppNewFolderWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );

static HWND  hNewFolderParentWnd = NULL, hNewFolderWnd = NULL;
/********************************************************************
* Function   DirsNew  
* Purpose    create new folder
* Params     
* Return     
* Remarks      
**********************************************************************/
static BOOL  DirsNew(HWND hParentWnd)
{
	WNDCLASS wc;
	RECT rect;
	
	hNewFolderParentWnd = hParentWnd;

	wc.style         = CS_OWNDC;
	wc.lpfnWndProc   = AppNewFolderWndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = NULL;
	wc.hIcon         = NULL;
	wc.hCursor       = NULL;
	wc.hbrBackground = NULL;
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = "NewFolderWndClass";
	
	if(IsWindow(hNewFolderWnd))
	{
		ShowWindow(hNewFolderWnd, SW_SHOW);
		UpdateWindow(hNewFolderWnd);
		return TRUE;

	}
	if (!RegisterClass(&wc))
	{
		return FALSE;
	}
	
	GetClientRect(hFrameWin, &rect);
	hNewFolderWnd = CreateWindow(
		"NewFolderWndClass", 
		"",
		WS_CHILD|WS_VISIBLE, 
		rect.left,rect.top,rect.right-rect.left,rect.bottom-rect.top,
		hFrameWin, 
		NULL,
		NULL, 
		NULL);
	
	
	if (NULL == hNewFolderWnd)
	{
		UnregisterClass("NewFolderWndClass",NULL);
		return FALSE;
	}
	
	ShowWindow(hNewFolderWnd, SW_SHOW);
	UpdateWindow(hNewFolderWnd);

	return TRUE;

}
/********************************************************************
* Function   AppNewFolderWndProc  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
static LRESULT AppNewFolderWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
	LRESULT lResult;
	HDC hdc;

    lResult = (LRESULT)TRUE;
    switch ( wMsgCmd )
    {
    case WM_CREATE :
        CreateNewFolderControl(hWnd);
		PDASetMenu(hFrameWin, NULL);
		SetWindowText(hFrameWin, ML("New folder"));
        break;

	case PWM_SHOWWINDOW:
		{
			SendMessage(hFrameWin, PWM_SETBUTTONTEXT, (WPARAM)1, (LPARAM)ML("Save"));
			
			SendMessage(hFrameWin, PWM_SETBUTTONTEXT, (WPARAM)0, (LPARAM)ML("Cancel"));

			SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"");

			SetWindowText(hFrameWin, ML("New folder"));

			SetFocus(hNewfolderFocus);
		}
		break;

    case WM_ACTIVATE:
        if (WA_INACTIVE != LOWORD(wParam))
		{
			SetFocus(hNewfolderFocus);
		}
	
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
		hNewfolderFocus = NULL;
		hNewFolderWnd = NULL;
        UnregisterClass("NewFolderWndClass",NULL);

        break;
    case WM_KEYDOWN:
        switch(LOWORD(wParam))
        {
		case VK_RETURN:
			SendMessage(hWnd, WM_COMMAND, IDC_NEWFOLDER_SAVE, 0);
			break;
        case VK_F10:
			SendMessage(hWnd, WM_COMMAND, IDC_NEWFOLDER_QUIT, 0);
            break;
        default:
            lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        }
        break;

    case WM_COMMAND:
        switch( LOWORD( wParam ))
        {	
		case IDC_NEWFOLDER_SAVE:
			{
				char pText[PMNODE_NAMELEN+1];
				int  i = 0 ;

				pText[0] = 0;
				
				GetWindowText(GetDlgItem(hWnd, IDC_NEWFOLDER), pText,
					GetWindowTextLength(GetDlgItem(hWnd, IDC_NEWFOLDER)) +1);
				
				while(pText[i] && pText[i]==0x20)
				{
					i++;
				}
				if(pText[i] == 0)
					pText[0] = 0;
				
				if(pText[0] == 0)
				{
					PLXTipsWin(NULL, hWnd, NULL, ML("Please define name"), NULL, Notify_Alert,ML("Ok"),
						NULL, WAITTIMEOUT);
					break;
				}
				
				SendMessage(hNewFolderParentWnd,PMNEW_IME_MSG, 1, (LPARAM)pText);
				SendMessage(hWnd, WM_CLOSE, 0, 0);	
			}
			break;
			
		case IDC_NEWFOLDER_QUIT:	
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
* Function   CreateNewFolderControl  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
static BOOL CreateNewFolderControl(HWND hWnd)
{
	
	HWND hNewFolder= NULL;
	IMEEDIT ie_Name;
	int num = 0;
	
	SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, 
		MAKEWPARAM(IDC_NEWFOLDER_SAVE,1),(LPARAM)ML("Save") );
	
	SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, 
		(WPARAM)IDC_NEWFOLDER_QUIT,(LPARAM)ML("Cancel") );

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
		ML("New folder"),
		WS_VISIBLE|WS_CHILD|WS_TABSTOP/*|WS_BORDER*/|ES_TITLE|ES_AUTOHSCROLL,
		0,0, PLX_WIN_WIDTH-2, 70,                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        
		hWnd,
		(HMENU)IDC_NEWFOLDER,
		NULL,
		(PVOID)&ie_Name);
	if (NULL == hNewFolder)
		return FALSE;

	SendMessage(hNewFolder, EM_SETTITLE, 0, (LPARAM)ML("Folder name:"));
	SendMessage(hNewFolder, EM_LIMITTEXT, PMNODE_NAMELEN, 0);
	SendMessage(hNewFolder, EM_SETSEL, -1, -1);

	hNewfolderFocus = hNewFolder ;
	SetFocus(hNewfolderFocus);
	
	return TRUE;
}
/********************************************************************
* Function   Menu_GetNewFolderWnd  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
HWND Menu_GetNewFolderWnd(void)
{
	return hNewFolderWnd;
}
/********************************************************************
* Function   Menu_PmGetDivideNum  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
int Menu_PmGetDivideNum(int dividend, int divisor)
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
* Function   Menu_GetDisplayName  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
BOOL Menu_GetDisplayName(char* pDisplayName, char *str, unsigned int len)
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
	if(!pHead)
	{
		pHead = pFolderItem;
		pFolderItem->pNext = NULL;
		return TRUE;
	}

	pTemp = pHead;

	while(pTemp->pNext)
	{
		pTemp = pTemp->pNext;
	}
	pTemp->pNext = pFolderItem;
	pFolderItem->pNext =NULL;

	return TRUE;

}
/********************************************************************
* Function   DeleteFolderItem  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
static BOOL DeleteFolderItem(WORD mId)
{
	PFOLDERLINK pFolderLink = NULL, p = NULL;

	if(pHead == NULL)
		return FALSE;

	pFolderLink = pHead;
	while(pFolderLink)
	{
		if(pFolderLink->mId == mId)
		{
			if(p)
				p->pNext = pFolderLink->pNext;
			else
				pHead = pFolderLink->pNext;

			free(pFolderLink);
			return TRUE;
		}

		p = pFolderLink;
		pFolderLink= pFolderLink->pNext;
	}
	return FALSE;
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
	PFOLDERLINK pTemp =NULL,p =NULL;

	if(!pHead)
		return TRUE;

	pTemp = pHead;

	while(pTemp)
	{
		p= pTemp;
		pTemp = pTemp->pNext;
		free(pTemp);
		pTemp = NULL;
	}

	pHead = NULL;
	return TRUE;
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
* Function   MenuPM_IsFlashAvailable  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
static int MenuPM_IsFlashAvailable(int nSize)
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
#define IDB_MOVEFOLDER  201
#define IDC_MOVEFOLDER_LIST 202
static HWND hListMoveFolder, hFocusMoveFolder, hMoveFolderWnd = NULL;
HBITMAP  hIconNormal, hIconSel;
static int nFolderNum = 0 ;
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
    wc.lpszClassName = "MoveFolderWindowClass";
	
	if(IsWindow(hMoveFolderWnd))
	{
		ShowWindow(hMoveFolderWnd, SW_SHOW);
		UpdateWindow(hMoveFolderWnd);
		
		return TRUE;
	}
    if (!RegisterClass(&wc))
        return FALSE;
	
	GetClientRect(hFrameWin, &rect);
    hMoveFolderWnd = CreateWindow("MoveFolderWindowClass",
        "",
        WS_VISIBLE|WS_CHILD,
		rect.left,rect.top,rect.right-rect.left,rect.bottom-rect.top, 
        hFrameWin,NULL,	NULL,	NULL);
	
    if(hMoveFolderWnd == NULL)
    {
        UnregisterClass("MoveFolderWindowClass", NULL);
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
		Load_Icon_SetupList(hListMoveFolder,hIconNormal,hIconSel, nFolderNum,0);
		hFocusMoveFolder = hListMoveFolder;
		SetWindowText(hFrameWin, ML("Move to folder"));
		SetFocus(hFocusMoveFolder);
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
			short dirid = 0;
			PDIRSNODE pDirNode = NULL;
			PAPPADM pappMain = NULL;
			int iNewPos =0;
			
			
			nFocusItem = SendMessage(hListMoveFolder, LB_GETCURSEL, 0, 0);

			dirid = (short)SendMessage(hListMoveFolder, LB_GETITEMDATA, nFocusItem, 0);
			
			if(dirid == 0)
				break;

			pDirNode = AppGetDirNodeFromId(pAppMain, dirid, NULL);
			
			if(pDirNode == NULL)
				break;
			
			AppFileMove(pAppMain, pCurAppNode->appId, (short)nFocus, 0, dirid );
			
			nGroupNum --;
			if(nFocus> nGroupNum - 1)
				nFocus--;
			
			pappMain = GetAppMain();
			PostMessage(pappMain->hGrpWnd, WM_DLMNODEDEL, 0, 0);

			if(pappMain->hAppWnd && dirid == GetSMenuDirId())
				PostMessage(pappMain->hAppWnd, WM_PMNODEADD, 0, 0);
			
			iMax = (Menu_PmGetDivideNum(nGroupNum, 3))*ITEMHIG;
			
			if(-iPos+150 > iMax)
				iNewPos = 150 -iPos -iMax ;
			
			if(iNewPos != 0)
			{
				SendMessage(hGrpWnd, WM_VSCROLL, MAKEWPARAM(SB_THUMBPOSITION,-iNewPos- iPos), NULL);
			}
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
			if(iIndex == 0)
				break;
			Load_Icon_SetupList(hListMoveFolder,hIconNormal,hIconSel, nFolderNum,iIndex);
			
			SetTimer(hWnd, 1, 500, NULL);
		
			break;
		}
		else
			PDADefWindowProc(hFrameWin, wMsgCmd, wParam, lParam);
		
        break;
	case WM_COMMAND:
		if(HIWORD(wParam) == LBN_SELCHANGE)
		{
			int iIndex = 0;

			iIndex = SendMessage(hListMoveFolder,LB_GETCURSEL,0,0);
			Load_Icon_SetupList(hListMoveFolder,hIconNormal,hIconSel, nFolderNum,0);
		}
		else
			PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
		break;
		
    case WM_DESTROY:
		hMoveFolderWnd = NULL;
		hFocusMoveFolder = NULL;
		if(hIconNormal != NULL)
			DeleteObject(hIconNormal);
		if(hIconSel != NULL)
			DeleteObject(hIconSel);
		hIconSel = NULL;
		hIconNormal = NULL;
        UnregisterClass("MoveFolderWindowClass", NULL);

		break;
		
    default:
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
    }
	
    return lResult;
}
/********************************************************************
* Function   CreateMoveFolerControl  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
static BOOL CreateMoveFolderControl(HWND hWnd, HWND *hList)
{
	PFOLDERLINK pLink =  NULL;
	PDIRSNODE pDirNode = NULL;
	int i = 0;
	RECT rect;
	
	GetClientRect(hFrameWin, &rect);

	*hList = CreateWindow("LISTBOX","", 
        WS_VISIBLE | WS_CHILD |/* WS_TABSTOP |*/ LBS_BITMAP |WS_VSCROLL,
        0, 0 , rect.right - rect.left, rect.bottom - rect.top,
        hWnd, (HMENU)IDC_MOVEFOLDER_LIST, NULL, NULL );
	
	if(*hList == NULL)
		return FALSE;
	
	if(pHead != NULL)
		pLink = pHead;

	SendMessage(*hList,LB_ADDSTRING,NULL,(LPARAM)ML("Main level"));
	SendMessage(*hList, LB_SETITEMDATA, 0, (LPARAM)0);
	i++;

	while(pLink != NULL)
	{
		pDirNode = NULL;
		pDirNode = AppGetDirNodeFromId(pAppMain,(short)(pLink->mId -IDM_FOLDER), NULL);

		SendMessage(*hList,LB_ADDSTRING,NULL,(LPARAM)pDirNode->aDisplayName);
		SendMessage(*hList, LB_SETITEMDATA, i, (LPARAM)pDirNode->dirId);
		pLink = pLink->pNext;
		i++;
	}
	nFolderNum = i +1;
	
	SendMessage(*hList,LB_SETCURSEL,0,0);
	return TRUE;
}
/********************************************************************
* Function   MenuGetMoveFolderWnd  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
HWND MenuGetMoveFolderWnd(void)
{
	return hMoveFolderWnd;
}
/********************************************************************
* Function   MenuGetDlmFocusIconName  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
void MenuGetDlmFocusIconName(char *strFocusIconName, char* strIconName)
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
* Function   MenuGetCurFolderRunApp  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
static BOOL MenuGetCurFolderRunApp(struct appMain *pAppMain, short dirID)
{
	PCURAPPLIST *pHead,	pFile = NULL;
	PAPPNODE pAppNode = NULL;

	pHead = AppFileGetListHead( pAppMain);

	if(!pHead)
		return FALSE;

	pFile = *pHead;

	while (pFile)
	{
		pAppNode = pAppMain->pFileCache + pFile->appId;
		if(pAppNode && pAppNode->fatherId == dirID)
		{
			return TRUE;
		}
		pFile = pFile->pNext;
	}

	return FALSE;	
}
