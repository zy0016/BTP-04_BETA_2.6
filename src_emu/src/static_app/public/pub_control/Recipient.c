/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : control
 *
 * Purpose  : implement Receive Box control
 *            
\**************************************************************************/


#include "Recipient.h"
#include "PhonebookExt.h"

/*
#ifdef _CHINESEINPUT_
#define	EAMILSIGN	'@'
#else
*/
#define	EMAILSIGN	0xfd
//#endif

/*Date about Recipient Control*/
typedef struct tagSTRU_RECIPIENTEDIT
{
    DWORD    wID;
    HWND    hParentWnd;
	DWORD	style;
//	HWND	hWnd_MyEdit;
	WNDPROC	ImeEditProc;
	WNDPROC	ComEditProc;
	LPSTR	Firstman;
	LPSTR	AllMan;				//All recipients，a semicolon and a white space character 
								//are automatically displayed between each recipient.
	int		nCurLen;			//the line number of recipients edit
	int		nLastLineNum;
	int		nMaxRecipient;		//the max number of recipients,the default value is five,
								//if you want to evaluate it yourself,please evaluate it after
								//the recipient component is created immediately.
	int		nCursonPos;
	int		nCursonLine;
	int		nCursonLastLine;
	BOOL	bEnableSet;
	int		x_mainWnd;
	int		y_mainWnd;
	int		width_mainwnd;
	int		height_mainwnd;
	BOOL	bEmail;
	BOOL	bCallAB;
	BOOL	bLostFocusByUporDown;
	RECIPIENTLISTBUF	RecList;
}STRU_RECIPIENT, *PSTRU_RECIPIENT;





#define EDITWND_INTERVAL	25

#define	NEWLINE			0x0a0d

#define MAXVISIBLERECIPIENT		3


//Control.h中的相关定义
#define	NMAXCHAR	1024
	
static LRESULT CALLBACK Recipient_WndProc(HWND hWnd, UINT message,
										 WPARAM wParam, LPARAM lParam);
static	int	GetCurIndexInRecEdit(HWND hWnd);
static	int	GetCharPreCurson(HWND	hWnd, WNDPROC EditProc);
static	void	ClearContactInAB(PRECIPIENTLISTBUF pListBuffer);
/*********************************************************************
* Function	   GAMEHELP_RegisterClass
* Purpose      创建窗口类
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL Recipient_RegisterClass()
{
    WNDCLASS wc;
    if(!GetClassInfo(NULL,"IMEEDIT",&wc))
		return FALSE;
	
    wc.style            = NULL;
    wc.lpfnWndProc      = Recipient_WndProc;
    wc.cbClsExtra       = 0;
    wc.cbWndExtra       = sizeof(STRU_RECIPIENT)+wc.cbWndExtra;
    wc.hInstance        = NULL;
    wc.hIcon            = NULL;
    wc.hCursor          = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground    = NULL;
    wc.lpszMenuName     = NULL;
    wc.lpszClassName    = WC_RECIPIENT;
    
    if (!RegisterClass(&wc))
        return FALSE;

    return TRUE;
}


/*********************************************************************
* Function	   NewWnd_WndProc
* Purpose      替换原EDIT过程
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static LRESULT CALLBACK Recipient_WndProc(HWND hWnd, UINT message,
										 WPARAM wParam, LPARAM lParam)
{
	LRESULT		lResult;
	RECT		editClient;
//	char		*ptr = NULL;
	int			linelength;

	HWND		hWndFocus;
	char		ptredit[NMAXCHAR] = "";
	char		chPreCurson;
	int			curline;// nLineLen;
	int			tmpSelIndex;
	char		buf[30] = "";
//	char		ContactName[100];
	PSTRU_RECIPIENT	pRecipient;
	PRECIPIENTLISTNODE	pContactNode;
	ABNAMEOREMAIL	ABinfo;
	LPCREATESTRUCT		pCreateStruct;
	WNDCLASS			wcimeedit,wcedit;

	IMEEDIT				ie;

    lResult = TRUE;
	
	
	GetClassInfo(NULL,"IMEEDIT",&wcimeedit);

	pRecipient = (PSTRU_RECIPIENT)((BYTE	*)GetUserData(hWnd)+wcimeedit.cbWndExtra);

	GetClassInfo(NULL, "EDIT", &wcedit);
	pRecipient->ImeEditProc	= wcimeedit.lpfnWndProc; //wcimeedit.lpfnWndProc;
	pRecipient->ComEditProc = wcedit.lpfnWndProc;

	
	switch(message)
	{
	case WM_CREATE:
        pCreateStruct			= (PCREATESTRUCT)lParam;
		pRecipient->wID			= (DWORD)pCreateStruct->hMenu;
		pRecipient->hParentWnd	= pCreateStruct->hwndParent;
//		pRecipient->hParentWnd	= GetParent(hWnd);
		pRecipient->AllMan	= NULL;
		pRecipient->Firstman = NULL;
		pRecipient->x_mainWnd = pCreateStruct->x;
		pRecipient->y_mainWnd = pCreateStruct->y;
		pRecipient->width_mainwnd = pCreateStruct->cx;
		pRecipient->height_mainwnd = pCreateStruct->cy;
		pRecipient->nMaxRecipient	=	5;
		pRecipient->nCurLen	 = 1;
		pRecipient->nLastLineNum = 1;
		pRecipient->bEnableSet = TRUE;
		pRecipient->style = pCreateStruct->style;
		pRecipient->nCursonLine = 0;
		pRecipient->nCursonLastLine = 0;
		pRecipient->bCallAB = FALSE;
		pRecipient->bLostFocusByUporDown = FALSE;
		RECIPIENT_InitListBuf(&pRecipient->RecList);

		if (pCreateStruct->style&RS_EMAIL) 
		{
			if (pCreateStruct->style&ES_UNDERLINE)
			{
				memset(&ie, 0, sizeof(IMEEDIT));
				
				ie.hwndNotify	= (HWND)GetParent(hWnd) ;    
				ie.dwAttrib	    = 0;                
				ie.dwAscTextMax	= 0;
				ie.dwUniTextMax	= 0;
				ie.wPageMax	    = 0;        
				ie.pszCharSet	= NULL;
				ie.pszTitle	    = NULL;
				ie.pszImeName	= NULL;
				pRecipient->bEmail	=	TRUE;
			}
			else
			{
				memset(&ie, 0, sizeof(IMEEDIT));
				
				ie.hwndNotify	= (HWND)GetParent(hWnd) ;    
				ie.dwAttrib	    = 0;                
				ie.dwAscTextMax	= 0;
				ie.dwUniTextMax	= 0;
				ie.wPageMax	    = 0;        
				ie.pszCharSet	= NULL;
				ie.pszTitle	    = NULL;
				ie.pszImeName	= "Num";
				pRecipient->bEmail	=	TRUE;
			}
			
		}
		else
		{
			memset(&ie, 0, sizeof(IMEEDIT));
    
			ie.hwndNotify	= (HWND)GetParent(hWnd);    
			ie.dwAttrib	    = 0;                
			ie.dwAscTextMax	= 0;
			ie.dwUniTextMax	= 0;
			ie.wPageMax	    = 0;        
			ie.pszCharSet	= "0123456789+";
			ie.pszTitle	    = NULL;
			ie.pszImeName	= "Phone";

			pRecipient->bEmail	=	FALSE;
		}
		pCreateStruct->style = ((~ES_UNDERLINE)&(pCreateStruct->style|WS_CHILD|WS_VISIBLE|ES_MULTILINE|ES_LEFT|ES_AUTOVSCROLL|WS_BORDER|ES_TITLE|WS_TABSTOP|ES_AUTOHSCROLL));
		pCreateStruct->lpCreateParams = &ie;
		pCreateStruct->lpszClass	=	"IMEEDIT";
		pRecipient->ImeEditProc(hWnd,WM_CREATE,wParam,(LPARAM)pCreateStruct);
//		NMAXCHAR = pRecipient->ImeEditProc(hWnd,EM_GETLIMITTEXT,0,0);
		pRecipient->AllMan = (PSTR)malloc(NMAXCHAR+1);
		*(pRecipient->AllMan) = '\0';
		pRecipient->Firstman = (PSTR)malloc(NMAXCHAR+1);
		*(pRecipient->Firstman) = '\0';

//		PREBROW_InitListBuf(pRecListBuf);
        break;
		
//	case EM_LIMITTEXT:
//		pRecipient->ImeEditProc(hWnd,EM_LIMITTEXT, wParam, lParam);
//		NMAXCHAR = pRecipient->ImeEditProc(hWnd,EM_GETLIMITTEXT,0,0);
//		pRecipient->AllMan = realloc(pRecipient->AllMan, NMAXCHAR+1);
//		pRecipient->Firstman = realloc(pRecipient->Firstman, NMAXCHAR +1);
//		break;

	case WM_SETFOCUS:
		//translate window to abs position
		GetWindowRectEx(hWnd,&editClient, W_WINDOW, XY_SCREEN);
		//pRecipient->x_mainWnd = editClient.left;
		//pRecipient->y_mainWnd = editClient.top - GetSystemMetrics(SM_CYCAPTION)-14;
		
		if (pRecipient->bCallAB) 
		{
			pRecipient->bCallAB = FALSE;
			pRecipient->nCurLen = pRecipient->RecList.nDataNum;
			SendMessage(pRecipient->hParentWnd,REC_SIZE,editClient.top - GetSystemMetrics(SM_CYCAPTION)-15 + pRecipient->height_mainwnd+((pRecipient->nCurLen>3)?2:pRecipient->nCurLen-1)*EDITWND_INTERVAL,0);		
			//MoveWindow(hWnd,pRecipient->x_mainWnd,pRecipient->y_mainWnd,pRecipient->width_mainwnd,pRecipient->height_mainwnd+((pRecipient->nCurLen>3)?2:pRecipient->nCurLen-1)*EDITWND_INTERVAL,TRUE);
			MoveWindow(hWnd,pRecipient->x_mainWnd, editClient.top - GetSystemMetrics(SM_CYCAPTION)-15, pRecipient->width_mainwnd,pRecipient->height_mainwnd+((pRecipient->nCurLen>3)?2:pRecipient->nCurLen-1)*EDITWND_INTERVAL,TRUE);
			UpdateContextWnd(hWnd, &pRecipient->RecList, pRecipient->ComEditProc);
			if(pRecipient->ComEditProc(hWnd, EM_LINELENGTH, (WORD)pRecipient->ComEditProc(hWnd, EM_GETSEL, 0, 0), 0) <=0)
			{
				pRecipient->ComEditProc(hWnd,WM_KEYDOWN,VK_LEFT,0);
				pRecipient->ComEditProc(hWnd,WM_KEYDOWN,VK_DELETE,0);
			}
		}
		else
		{
			
			SendMessage(pRecipient->hParentWnd,REC_SIZE,editClient.top - GetSystemMetrics(SM_CYCAPTION)-15 + pRecipient->height_mainwnd+((pRecipient->nCurLen>3)?2:pRecipient->nCurLen-1)*EDITWND_INTERVAL,0);		
			//MoveWindow(hWnd,pRecipient->x_mainWnd,pRecipient->y_mainWnd,pRecipient->width_mainwnd,pRecipient->height_mainwnd+((pRecipient->nCurLen>3)?2:pRecipient->nCurLen-1)*EDITWND_INTERVAL,TRUE);
			MoveWindow(hWnd,pRecipient->x_mainWnd, editClient.top - GetSystemMetrics(SM_CYCAPTION)-15,pRecipient->width_mainwnd,pRecipient->height_mainwnd+((pRecipient->nCurLen>3)?2:pRecipient->nCurLen-1)*EDITWND_INTERVAL,TRUE);
			if((strlen(pRecipient->AllMan) > 0)&&pRecipient->bEnableSet)
			{
				//pRecipient->ImeEditProc(hWnd,EM_SETSEL, -1, -1);
				pRecipient->ComEditProc(hWnd,WM_SETTEXT,0,(LPARAM)pRecipient->AllMan);
				pRecipient->ComEditProc(hWnd, EM_SETSEL, LOWORD(pRecipient->nCursonPos), LOWORD(pRecipient->nCursonPos));
			}		
			//SendMessage(hWnd,EM_SETSEL,-1,-1);	
			//GetWindowText(hWnd,ptredit,NMAXCHAR);
		}
		tmpSelIndex= (WORD)pRecipient->ComEditProc(hWnd, EM_GETSEL, 0, 0);
		if (tmpSelIndex > 0) {
			SendMessage(GetParent(GetParent(hWnd)), PWM_SETBUTTONTEXT, 0, (LPARAM)ML("Cancel"));
		}
		lResult = pRecipient->ImeEditProc(hWnd,message,wParam,lParam);
		break;
	

	case WM_KILLFOCUS:
//		if(pRecipient->AllMan)
//		{
//			pRecipient->ImeEditProc(hWnd,WM_GETTEXT,NMAXCHAR,(LPARAM)pRecipient->AllMan);
//		}
		// 判断是否char table窗口弹出 [9/5/2005]
//		hWndFocus = GetFocus();
//		if (hWndFocus != hWnd) {
//			GetClassName(hWndFocus, ptredit, NMAXCHAR);
//			if ((stricmp(ptredit, "SYMIME") == 0)||((stricmp(ptredit, "#MENU") == 0)) ){
//				break;
//			}
//		}
		pRecipient->bEnableSet = TRUE;
		pRecipient->nCursonPos =LOWORD(pRecipient->ImeEditProc(hWnd, EM_GETSEL, 0, 0));

		if(pRecipient->AllMan)
		{
			pRecipient->ImeEditProc(hWnd,WM_GETTEXT,NMAXCHAR,(LPARAM)pRecipient->AllMan);
		}
		
//		if(pRecipient->AllMan)
//		{
//			pRecipient->ImeEditProc(hWnd,WM_GETTEXT,NMAXCHAR,(LPARAM)pRecipient->AllMan);
//		}
		pRecipient->nLastLineNum = pRecipient->nCurLen = pRecipient->ComEditProc(hWnd,EM_GETLINECOUNT,0,0);
		if (pRecipient->bLostFocusByUporDown) 
		{			
			//pRecipient->nCurLen = pRecipient->ComEditProc(hWnd,EM_GETLINECOUNT,0,0);
			pRecipient->bLostFocusByUporDown = FALSE;
			pRecipient->bEnableSet = TRUE;
			if(pRecipient->Firstman)
			{
				*(WORD *)pRecipient->Firstman = NMAXCHAR;
				linelength = pRecipient->ImeEditProc(hWnd,EM_GETLINE,0, (LPARAM)pRecipient->Firstman);
				
				*(pRecipient->Firstman+linelength) = NULL;
				
				memset(buf, 0, 30);
				GetExtentFittedText(NULL,pRecipient->Firstman, -1, buf, 30, pRecipient->width_mainwnd-15, '.', 3);
				
				if (pRecipient->nCurLen>1) 
				{					
					if (strcmp(buf, pRecipient->Firstman) == 0) 
					{
						strcat(buf,"...");
						pRecipient->ComEditProc(hWnd,WM_SETTEXT,0,(LPARAM)buf);
					}
					else
						pRecipient->ComEditProc(hWnd,WM_SETTEXT,0,(LPARAM)buf);					
				}			
				
				MoveWindow(hWnd,pRecipient->x_mainWnd,pRecipient->y_mainWnd,pRecipient->width_mainwnd,pRecipient->height_mainwnd,TRUE);
				SendMessage(pRecipient->hParentWnd,REC_SIZE, pRecipient->y_mainWnd +pRecipient->height_mainwnd,0);
				
			}
		}
		//pRecipient->ImeEditProc(hWnd, EM_SETSEL, LOWORD(pRecipient->nCursonPos), HIWORD(pRecipient->nCursonPos));
		pRecipient->ImeEditProc(hWnd,message,wParam,lParam);
		break;

	case REC_SETMAXREC:
		pRecipient->nMaxRecipient = (int)wParam;
		break;

	

	case GHP_GETRECLEN:
		if (hWnd == GetFocus()) {
			GetWindowText(hWnd,pRecipient->AllMan,NMAXCHAR);
			lResult = strlen(pRecipient->AllMan);
		}
		else
		{
			lResult = strlen(pRecipient->AllMan);
		}
		
		break;
		
	case WM_CHAR:
		pRecipient->nCursonLine = GetCurIndexInRecEdit(hWnd);
		pContactNode = RECIPIENT_GetPDATA(&pRecipient->RecList, pRecipient->nCursonLine);
		if (pContactNode) {
			if (pContactNode->bExistInAB) {
				pRecipient->ComEditProc(hWnd, EM_SETSEL, -1, -1);
			}
		}
		if (wParam == 13) {		//if the msg is translated from WM_KEYDOWN VK_RETURN
			break;
		}
		if (wParam == '*')
		{
			break;
		}
//		memset(ptredit,0,sizeof(ptredit));
		
		pRecipient->bEnableSet = FALSE;
		linelength = pRecipient->ComEditProc(hWnd,EM_LINELENGTH, LOWORD(SendMessage(hWnd, EM_GETSEL, 0, 0)), 0);
		chPreCurson = GetCharPreCurson(hWnd, pRecipient->ImeEditProc);
		if((wParam != ';')&&(chPreCurson!=';'))
		{
			if (pRecipient->style&RS_EMAIL) 
			{
				if (linelength >= 50) {
					break;
				}
			}
			else
			{
				if (linelength >= 40) {
					break;
				}
			}
		}
		pRecipient->nCurLen = pRecipient->ComEditProc(hWnd,EM_GETLINECOUNT,0,0);
		tmpSelIndex =LOWORD(pRecipient->ComEditProc(hWnd, EM_GETSEL, 0, 0));
//		pRecipient->ComEditProc(hWnd,EM_SETSEL,tmpSelIndex-1,tmpSelIndex-1);
//		pRecipient->ComEditProc(hWnd,EM_GETSELTEXT,1,(LPARAM)ptredit);
//		pRecipient->ComEditProc(hWnd,EM_SETSEL,tmpSelIndex,tmpSelIndex);
		ptredit[0]  = GetCharPreCurson(hWnd, pRecipient->ImeEditProc);

		
		//在行尾的分号后面输入
		if (*ptredit==';') 
		{
//			pRecipient->ComEditProc(hWnd,WM_KEYDOWN,VK_END,0);
			if (wParam == ';') {
				break;
			}
			if(pRecipient->nCurLen >= pRecipient->nMaxRecipient)
				break;
			//在中间行的分号回输入,应该将光标移到最后位置,新建一个收件人.
			pRecipient->ComEditProc(hWnd,EM_SETSEL, -1, -1);
			pRecipient->ComEditProc(hWnd,WM_CHAR,NEWLINE,0);
			pRecipient->nCurLen = pRecipient->ComEditProc(hWnd,EM_GETLINECOUNT,0,0);
			if ((pRecipient->nCurLen<=MAXVISIBLERECIPIENT)&&(pRecipient->nCurLen != pRecipient->nLastLineNum)) 
			{
				pRecipient->nLastLineNum = pRecipient->nCurLen ;
				SendMessage(pRecipient->hParentWnd,REC_SIZE,pRecipient->y_mainWnd+pRecipient->height_mainwnd+(pRecipient->nCurLen-1)*EDITWND_INTERVAL,0);
				MoveWindow(hWnd,pRecipient->x_mainWnd,pRecipient->y_mainWnd,pRecipient->width_mainwnd,pRecipient->height_mainwnd+(pRecipient->nCurLen-1)*EDITWND_INTERVAL,TRUE);
				//MoveWindow(hWnd,EDITWND_LEFT,EDITWND_TOP,EDITWND_WIDTH,EDITWND_HEIGHT+(pRecipient->nCurLen-1)*EDITWND_INTERVAL,TRUE);
			}
			pRecipient->ImeEditProc(hWnd,message,wParam,lParam);
			break;
		}

		switch(wParam) 
		{
		case ' ':
		case ';':
			//memset(ptredit,0,sizeof(ptredit));
			tmpSelIndex = (WORD)pRecipient->ComEditProc(hWnd, EM_GETSEL, 0, 0);
			pRecipient->ComEditProc(hWnd,WM_KEYDOWN,VK_END,0);
			pRecipient->ComEditProc(hWnd,WM_KEYDOWN,VK_HOME,CK_SHIFT);
			pRecipient->ComEditProc(hWnd,EM_GETSELTEXT,0,(LPARAM)ptredit);
			linelength = strlen(ptredit);
			if((linelength > 0 )&&(ptredit[linelength - 1] == ';'))
			{
				pRecipient->ComEditProc(hWnd, EM_SETSEL, tmpSelIndex, tmpSelIndex);
				break;
			}
//			memset(ptredit,0,sizeof(ptredit));
			
			if (linelength<1) 
			{
				if (wParam == ';')
				{
					pRecipient->ComEditProc(hWnd,WM_KEYDOWN,VK_END,0);
					break;
				}
				else
				{
					pRecipient->ComEditProc(hWnd, WM_CHAR, wParam, lParam);
					break;
				}
				
			}
			pRecipient->nCursonLine = GetCurIndexInRecEdit(hWnd);
			pContactNode = RECIPIENT_GetPDATA(&pRecipient->RecList, pRecipient->nCursonLine);
			//get more info from addressbook
			if (pRecipient->style&RS_EMAIL) //email or mms
			{
				if (PLXstrchr(ptredit, EMAILSIGN)) 
				{
					if(APP_GetNameByEmail(ptredit, &ABinfo))
					{
						
						pRecipient->ComEditProc(hWnd,EM_REPLACESEL, 0, (LPARAM)ABinfo.szName);
						if (pContactNode) {
							RECIPIENT_ModifyData(&pRecipient->RecList, pRecipient->nCursonLine, ABinfo.szName, ABinfo.szTelOrEmail, TRUE);
						}
						else
						{
							RECIPIENT_AddData(&pRecipient->RecList, ABinfo.szName, ptredit, TRUE);
						}
					}
					else
					{
						if (pContactNode)
						{
							RECIPIENT_ModifyData(&pRecipient->RecList, pRecipient->nCursonLine, ptredit, ptredit, FALSE);
						}
						else
							RECIPIENT_AddData(&pRecipient->RecList, ptredit, ptredit, FALSE);
					}
				}
				else
				{
					if(APP_GetNameByPhone(ptredit, &ABinfo))
					{
						
						pRecipient->ComEditProc(hWnd,EM_REPLACESEL, 0, (LPARAM)ABinfo.szName);
						if (pContactNode) {
							RECIPIENT_ModifyData(&pRecipient->RecList, pRecipient->nCursonLine, ABinfo.szName, ABinfo.szTelOrEmail, TRUE);
						}
						else
						{
							RECIPIENT_AddData(&pRecipient->RecList, ABinfo.szName, ptredit, TRUE);
						}
					}
					else
					{
						if (pContactNode)
						{
							RECIPIENT_ModifyData(&pRecipient->RecList, pRecipient->nCursonLine, ptredit, ptredit, FALSE);
						}
						else
							RECIPIENT_AddData(&pRecipient->RecList, ptredit, ptredit, FALSE);
					}
				}
			}
			else
			{
				if(APP_GetNameByPhone(ptredit, &ABinfo))
				{					
					pRecipient->ComEditProc(hWnd,EM_REPLACESEL, 0, (LPARAM)ABinfo.szName);
					if (pContactNode) {
						RECIPIENT_ModifyData(&pRecipient->RecList, pRecipient->nCursonLine, ABinfo.szName, ABinfo.szTelOrEmail, TRUE);
					}
					else
						RECIPIENT_AddData(&pRecipient->RecList, ABinfo.szName, ptredit, TRUE);
				}
				else
				{
					if (pContactNode)
					{
						RECIPIENT_ModifyData(&pRecipient->RecList, pRecipient->nCursonLine, ptredit, ptredit, FALSE);
					}
					else
						RECIPIENT_AddData(&pRecipient->RecList, ptredit, ptredit, FALSE);
				}
			}
			//end
			pRecipient->ComEditProc(hWnd,WM_KEYDOWN,VK_END,0);
			pRecipient->ComEditProc(hWnd,message,wParam,lParam);

			break;

		default:
			pRecipient->ImeEditProc(hWnd,message,wParam,lParam);
			break;
		}
		pRecipient->nCurLen = pRecipient->ComEditProc(hWnd,EM_GETLINECOUNT,0,0);
		if ((pRecipient->nCurLen<=MAXVISIBLERECIPIENT)&&(pRecipient->nCurLen != pRecipient->nLastLineNum)) 
		{
			pRecipient->nLastLineNum = pRecipient->nCurLen ;
			SendMessage(pRecipient->hParentWnd,REC_SIZE,pRecipient->y_mainWnd+pRecipient->height_mainwnd+(pRecipient->nCurLen-1)*EDITWND_INTERVAL,0);
			MoveWindow(hWnd,pRecipient->x_mainWnd,pRecipient->y_mainWnd,pRecipient->width_mainwnd,pRecipient->height_mainwnd+(pRecipient->nCurLen-1)*EDITWND_INTERVAL,TRUE);
			//MoveWindow(hWnd,EDITWND_LEFT,EDITWND_TOP,EDITWND_WIDTH,EDITWND_HEIGHT+(pRecipient->nCurLen-1)*EDITWND_INTERVAL,TRUE);
		}
		GetClientRect(hWnd,&editClient);
		InvalidateRect(hWnd,&editClient,TRUE);
		break;

	case WM_KEYUP:
		switch(LOWORD(wParam)) 
		{
		case VK_0:
				KillTimer(hWnd, 1);
			break;
		default:
			lResult	=	pRecipient->ImeEditProc(hWnd,message,wParam,lParam);
			break;
		}

		break;

	case WM_TIMER:
		if ((UINT)wParam == 1) {
			KillTimer(hWnd, 1);
			pRecipient->ImeEditProc(hWnd,WM_KEYDOWN,VK_BACK,0);
			SendMessage(hWnd,WM_CHAR,';',0);
			if(SendMessage(hWnd, EM_LINELENGTH, (WORD)SendMessage(hWnd, EM_GETSEL, 0, 0), 0) <=0)
			{
				pRecipient->ComEditProc(hWnd,WM_KEYDOWN,VK_LEFT,0);
				pRecipient->ComEditProc(hWnd,WM_KEYDOWN,VK_DELETE,0);
				pRecipient->ComEditProc(hWnd,WM_KEYDOWN,VK_END,0);
				pRecipient->nCurLen = pRecipient->ComEditProc(hWnd,EM_GETLINECOUNT,0,0);
				if ((pRecipient->nCurLen<=MAXVISIBLERECIPIENT) &&(pRecipient->nLastLineNum != pRecipient->nCurLen) )
				{
					pRecipient->nLastLineNum = pRecipient->nCurLen;
					SendMessage(pRecipient->hParentWnd,REC_SIZE,pRecipient->y_mainWnd+pRecipient->height_mainwnd+(pRecipient->nCurLen-1)*EDITWND_INTERVAL,0);
					MoveWindow(hWnd,pRecipient->x_mainWnd,pRecipient->y_mainWnd,pRecipient->width_mainwnd,pRecipient->height_mainwnd+(pRecipient->nCurLen-1)*EDITWND_INTERVAL,TRUE);
					//MoveWindow(hWnd,EDITWND_LEFT,EDITWND_TOP,EDITWND_WIDTH,EDITWND_HEIGHT+(pRecipient->nCurLen-1)*EDITWND_INTERVAL,TRUE);
					
				}
			}
		}
		else
			lResult	=	pRecipient->ImeEditProc(hWnd,message,wParam,lParam);					
		break;

	case WM_KEYDOWN:
		switch(LOWORD(wParam))
		{
		case VK_0:
			if (pRecipient->bEmail) {
				lResult = pRecipient->ComEditProc(hWnd,message,wParam,lParam);
			}
			else
			{
				linelength = pRecipient->ComEditProc(hWnd,EM_LINELENGTH, LOWORD(SendMessage(hWnd, EM_GETSEL, 0, 0)), 0);
				if (pRecipient->style&RS_EMAIL) 
				{
					if (linelength >= 50) {
						break;
					}
				}
				else
				{
					if (linelength >= 40) {
						break;
					}
				}
				//nkeytimerid = SetTimer(NULL, 0,  600, Key_TimerProc);
				SetTimer( hWnd, 1, 600, NULL );
			
			}

			break;

		case VK_RETURN:
			SendMessage(pRecipient->hParentWnd,message,wParam,lParam);
			break;
		
		case VK_F10://map the key backspace on F10
		
			pRecipient->nCursonLine = GetCurIndexInRecEdit(hWnd);
			pContactNode = RECIPIENT_GetPDATA(&pRecipient->RecList, pRecipient->nCursonLine);
			
			chPreCurson = GetCharPreCurson(hWnd, pRecipient->ImeEditProc);
			if (chPreCurson == 0)
			{
				SendMessage(pRecipient->hParentWnd,WM_KEYDOWN,VK_F10,lParam);
				break;
			}
			GetWindowText(hWnd,ptredit,NMAXCHAR);
			if(ptredit[0]=='\0')
				SendMessage(pRecipient->hParentWnd,WM_KEYDOWN,VK_F10,lParam);
			else
			{
				//tmpSelIndex = LOWORD(pRecipient->ImeEditProc(hWnd, EM_GETSEL, 0, 0));
				//pRecipient->ImeEditProc(hWnd, EM_SETSEL, tmpSelIndex-1, tmpSelIndex-1);
				//pRecipient->ComEditProc(hWnd,WM_KEYDOWN,VK_LEFT,CK_SHIFT);
				//pRecipient->ComEditProc(hWnd,EM_GETSELTEXT,1,(LPARAM)ptredit);
				ptredit[0] = GetCharPreCurson(hWnd, pRecipient->ImeEditProc);

				//if (((strchr(ptredit,';'))||((ptredit[0]>'a'&&ptredit[0]<'z')||(ptredit[0]>'A'&&ptredit[0]<'Z')))&&(!pRecipient->bEmail) )
				if(pContactNode&&pContactNode->bExistInAB)
				{
					pRecipient->ComEditProc(hWnd,WM_KEYDOWN,VK_END,0);
					pRecipient->ComEditProc(hWnd,WM_KEYDOWN,VK_HOME,CK_SHIFT);
					pRecipient->ComEditProc(hWnd,WM_CLEAR,0,0);
					pRecipient->ComEditProc(hWnd,WM_KEYDOWN,VK_LEFT,0);
					pRecipient->ComEditProc(hWnd,WM_KEYDOWN,VK_DELETE,0);
					pRecipient->ComEditProc(hWnd,WM_KEYDOWN,VK_END,0);
					RECIPIENT_DelData(&pRecipient->RecList, pRecipient->nCursonLine);
					pRecipient->nCurLen = pRecipient->ComEditProc(hWnd,EM_GETLINECOUNT,0,0);
					if ((pRecipient->nCurLen<=MAXVISIBLERECIPIENT) &&(pRecipient->nLastLineNum != pRecipient->nCurLen))
					{
						pRecipient->nLastLineNum = pRecipient->nCurLen;
						SendMessage(pRecipient->hParentWnd,REC_SIZE,pRecipient->y_mainWnd+pRecipient->height_mainwnd+(pRecipient->nCurLen-1)*EDITWND_INTERVAL,0);
						MoveWindow(hWnd,pRecipient->x_mainWnd,pRecipient->y_mainWnd,pRecipient->width_mainwnd,pRecipient->height_mainwnd+(pRecipient->nCurLen-1)*EDITWND_INTERVAL,TRUE);
					}
//					lResult = pRecipient->ComEditProc(hWnd,WM_KEYDOWN,VK_BACK,0);
				}				
				else
				{
					if (ptredit[0]==0x0a)
					{
						//pRecipient->ComEditProc(hWnd,WM_KEYDOWN,VK_RIGHT,0);
//						pRecipient->ComEditProc(hWnd,WM_KEYDOWN,VK_DELETE,0);
						
						//pRecipient->ComEditProc(hWnd,WM_KEYDOWN,VK_RIGHT,CK_SHIFT);
						pRecipient->ComEditProc(hWnd,EM_GETSELTEXT,1,(LPARAM)ptredit);
						if (ptredit[0]=='\0') 
						{
//							pRecipient->ComEditProc(hWnd,WM_KEYDOWN,VK_BACK,0);
							//pRecipient->ComEditProc(hWnd,WM_KEYDOWN,VK_RIGHT,0);
							//tmpSelIndex = LOWORD(pRecipient->ImeEditProc(hWnd, EM_GETSEL, 0, 0));
							//pRecipient->ImeEditProc(hWnd, EM_SETSEL, tmpSelIndex-1, tmpSelIndex-1);
//							pRecipient->ComEditProc(hWnd,WM_KEYDOWN,VK_LEFT,CK_SHIFT);
//							pRecipient->ComEditProc(hWnd,EM_GETSELTEXT,0,(LPARAM)ptredit);
							pRecipient->ComEditProc(hWnd,WM_KEYDOWN, VK_LEFT, 0);
							pRecipient->ComEditProc(hWnd,WM_KEYDOWN, VK_DELETE, 0);
							//pRecipient->ComEditProc(hWnd,WM_KEYDOWN,VK_BACK,0);
//							GetWindowText(hWnd,ptredit,NMAXCHAR);
							pRecipient->nCurLen = pRecipient->ComEditProc(hWnd,EM_GETLINECOUNT,0,0);
							if ((pRecipient->nCurLen<=MAXVISIBLERECIPIENT) &&(pRecipient->nLastLineNum != pRecipient->nCurLen) )
							{
								pRecipient->nLastLineNum = pRecipient->nCurLen;
								SendMessage(pRecipient->hParentWnd,REC_SIZE,pRecipient->y_mainWnd+pRecipient->height_mainwnd+(pRecipient->nCurLen-1)*EDITWND_INTERVAL,0);
								MoveWindow(hWnd,pRecipient->x_mainWnd,pRecipient->y_mainWnd,pRecipient->width_mainwnd,pRecipient->height_mainwnd+(pRecipient->nCurLen-1)*EDITWND_INTERVAL,TRUE);
								//MoveWindow(hWnd,EDITWND_LEFT,EDITWND_TOP,EDITWND_WIDTH,EDITWND_HEIGHT+(pRecipient->nCurLen-1)*EDITWND_INTERVAL,TRUE);
								
							}
						}
						else
						{
							//当光标在一行的最前面时，应该从上一行的尾部开始删除。
							pRecipient->ComEditProc(hWnd,WM_KEYDOWN, VK_LEFT, 0);
							SendMessage(hWnd, WM_KEYDOWN, VK_F10, lParam);
							//pRecipient->ComEditProc(hWnd,WM_KEYDOWN,VK_DELETE,0);
						}
					}
				
					else if (ptredit[0]!='\0') 
					{
						pRecipient->ComEditProc(hWnd,WM_KEYDOWN, VK_BACK, 0);
						pRecipient->nCurLen = pRecipient->ComEditProc(hWnd,EM_GETLINECOUNT,0,0);
						if ((pRecipient->nCurLen<=MAXVISIBLERECIPIENT) &&(pRecipient->nLastLineNum != pRecipient->nCurLen) )
						{
							pRecipient->nLastLineNum = pRecipient->nCurLen;
							SendMessage(pRecipient->hParentWnd,REC_SIZE,pRecipient->y_mainWnd+pRecipient->height_mainwnd+(pRecipient->nCurLen-1)*EDITWND_INTERVAL,0);
							MoveWindow(hWnd,pRecipient->x_mainWnd,pRecipient->y_mainWnd,pRecipient->width_mainwnd,pRecipient->height_mainwnd+(pRecipient->nCurLen-1)*EDITWND_INTERVAL,TRUE);
							//MoveWindow(hWnd,EDITWND_LEFT,EDITWND_TOP,EDITWND_WIDTH,EDITWND_HEIGHT+(pRecipient->nCurLen-1)*EDITWND_INTERVAL,TRUE);
							
						}
					}
					else
					{
						SendMessage(pRecipient->hParentWnd, WM_KEYDOWN, VK_F10, lParam);
					}
						//当一行的最后一个字符被删除时,此行被清掉.
					if(SendMessage(hWnd, EM_LINELENGTH, (WORD)SendMessage(hWnd, EM_GETSEL, 0, 0), 0) <=0)
					{
						pRecipient->ComEditProc(hWnd,WM_KEYDOWN,VK_LEFT,0);
						pRecipient->ComEditProc(hWnd,WM_KEYDOWN,VK_DELETE,0);
						pRecipient->ComEditProc(hWnd,WM_KEYDOWN,VK_END,0);
						if (RECIPIENT_GetPDATA(&pRecipient->RecList, pRecipient->nCursonLine)) {
							RECIPIENT_DelData(&pRecipient->RecList, pRecipient->nCursonLine);
						}
						pRecipient->nCurLen = pRecipient->ComEditProc(hWnd,EM_GETLINECOUNT,0,0);
						if ((pRecipient->nCurLen<=MAXVISIBLERECIPIENT) &&(pRecipient->nLastLineNum != pRecipient->nCurLen) )
						{
							pRecipient->nLastLineNum = pRecipient->nCurLen;
							SendMessage(pRecipient->hParentWnd,REC_SIZE,pRecipient->y_mainWnd+pRecipient->height_mainwnd+(pRecipient->nCurLen-1)*EDITWND_INTERVAL,0);
							MoveWindow(hWnd,pRecipient->x_mainWnd,pRecipient->y_mainWnd,pRecipient->width_mainwnd,pRecipient->height_mainwnd+(pRecipient->nCurLen-1)*EDITWND_INTERVAL,TRUE);
							//MoveWindow(hWnd,EDITWND_LEFT,EDITWND_TOP,EDITWND_WIDTH,EDITWND_HEIGHT+(pRecipient->nCurLen-1)*EDITWND_INTERVAL,TRUE);
							
						}
						
					}
					UpdateWindow(hWnd);
					GetClientRect(hWnd,&editClient);
					InvalidateRect(hWnd,&editClient,TRUE);
				}
				
			}
			break;

		case VK_UP:
			pRecipient->nCursonLastLine = pRecipient->nCursonLine = GetCurIndexInRecEdit(hWnd);
		
						
			pContactNode = RECIPIENT_GetPDATA(&pRecipient->RecList, pRecipient->nCursonLine);
			//如果光标所在行为空,向上移动等价于向前删除一个字符,而且应该判断数据结构中是否存在改行,然后
			//进行资源得释放.
			if(SendMessage(hWnd, EM_LINELENGTH, (WORD)SendMessage(hWnd, EM_GETSEL, 0, 0), 0) <=0)
			{
				pRecipient->ComEditProc(hWnd,WM_KEYDOWN,VK_LEFT,0);
				pRecipient->ComEditProc(hWnd,WM_KEYDOWN,VK_DELETE,0);
				pRecipient->ComEditProc(hWnd,WM_KEYDOWN,VK_END,0);
				if(pContactNode)
					RECIPIENT_DelData(&pRecipient->RecList, pRecipient->nCursonLine);
				pRecipient->nCurLen = pRecipient->ComEditProc(hWnd,EM_GETLINECOUNT,0,0);
				if ((pRecipient->nCurLen<=MAXVISIBLERECIPIENT) &&(pRecipient->nLastLineNum != pRecipient->nCurLen) )
				{
					pRecipient->nLastLineNum = pRecipient->nCurLen;
					SendMessage(pRecipient->hParentWnd,REC_SIZE,pRecipient->y_mainWnd+pRecipient->height_mainwnd+(pRecipient->nCurLen-1)*EDITWND_INTERVAL,0);
					MoveWindow(hWnd,pRecipient->x_mainWnd,pRecipient->y_mainWnd,pRecipient->width_mainwnd,pRecipient->height_mainwnd+(pRecipient->nCurLen-1)*EDITWND_INTERVAL,TRUE);
					
				}
				
				if (pRecipient->nCursonLine == 0) 
				{
					pRecipient->bLostFocusByUporDown = TRUE;
					pRecipient->ComEditProc(hWnd,WM_KEYDOWN,VK_UP,lParam);
					pRecipient->ComEditProc(hWnd,WM_KEYDOWN,VK_END,0);
					/*
					pRecipient->nCurLen = pRecipient->ComEditProc(hWnd,EM_GETLINECOUNT,0,0);
					if(pRecipient->Firstman)
					{
						*(WORD *)pRecipient->Firstman = NMAXCHAR;
						linelength = pRecipient->ImeEditProc(hWnd,EM_GETLINE,0, (LPARAM)pRecipient->Firstman);
						
						*(pRecipient->Firstman+linelength) = NULL;
						
						memset(buf, 0, 30);
						GetExtentFittedText(NULL,pRecipient->Firstman, -1, buf, 30, pRecipient->width_mainwnd, '.', 3);
						
						if (pRecipient->nCurLen>1) {
							
							if (strcmp(buf, pRecipient->Firstman) == 0) {
								strcat(buf,"...");
								pRecipient->ComEditProc(hWnd,WM_SETTEXT,0,(LPARAM)buf);
							}
							else
								pRecipient->ComEditProc(hWnd,WM_SETTEXT,0,(LPARAM)buf);
							
						}			
						
						MoveWindow(hWnd,pRecipient->x_mainWnd,pRecipient->y_mainWnd,pRecipient->width_mainwnd,pRecipient->height_mainwnd,TRUE);
						SendMessage(pRecipient->hParentWnd,REC_SIZE, pRecipient->y_mainWnd + pRecipient->y_mainWnd+pRecipient->height_mainwnd,0);
						
					}
					*/
				}
				//pRecipient->ComEditProc(hWnd,WM_KEYDOWN,VK_UP,lParam);
				//pRecipient->ComEditProc(hWnd,WM_KEYDOWN,VK_END,0);
				InvalidateRect(hWnd, NULL, TRUE);
				break;
			}

			//判断本行的最后一个字符是否是分号,应该保证光标不在行的结尾都有分号.

			pRecipient->ComEditProc(hWnd,WM_KEYDOWN,VK_END,0);
			pRecipient->ComEditProc(hWnd, WM_KEYDOWN, VK_HOME, CK_SHIFT);
			/*
			nLineLen = pRecipient->ComEditProc(hWnd, EM_LINELENGTH, pRecipient->nCursonLastLine, 0);
			tmpSelIndex =LOWORD(pRecipient->ComEditProc(hWnd, EM_GETSEL, 0, 0));
			pRecipient->ComEditProc(hWnd,EM_SETSEL,tmpSelIndex-nLineLen,tmpSelIndex);
			*/
			pRecipient->ComEditProc(hWnd,EM_GETSELTEXT,0,(LPARAM)ptredit);
			pRecipient->ComEditProc(hWnd,WM_KEYDOWN,VK_END,0);
			if (ptredit[strlen(ptredit) -1 ] != ';') 
			{
				SendMessage(hWnd, WM_CHAR, ';', 0);
				pRecipient->nCursonPos = (WORD)pRecipient->ComEditProc(hWnd, EM_GETSEL, 0, 0);
			}
			//pContactNode = RECIPIENT_GetPDATA(&pRecipient->RecList, pRecipient->nCursonLine);
			//如果有分号,只是改变了中间的输入号码,此中情况下,该号码不可能存在于电话本.
			else
			{
				
				//该行不是新行,已经输入过recipient,而且可能对改行进行了操作;
				//pRecipient->ComEditProc(hWnd,WM_KEYDOWN,VK_HOME,0);
				//pRecipient->ComEditProc(hWnd,WM_KEYDOWN,VK_END,VK_SHIFT);
				//pRecipient->ComEditProc(hWnd, EM_GETSELTEXT, 0, (LPARAM)ptredit);
				ptredit[strlen(ptredit) -1 ] = '\0';

				//nLineLen = pRecipient->ComEditProc(hWnd, EM_LINELENGTH, pRecipient->nCursonLastLine, 0);
				
				if(pContactNode)//必定存在,否则意味着数据结构发生了错误
				{				
					if (!pContactNode->bExistInAB) 
					{						
						if (PLXstrchr(ptredit, EMAILSIGN)) 
						{
							if(APP_GetNameByEmail(ptredit, &ABinfo))
							{
								
								pRecipient->ComEditProc(hWnd,EM_REPLACESEL, 0, (LPARAM)ABinfo.szName);
								if (pContactNode) {
									//should pass here
									RECIPIENT_ModifyData(&pRecipient->RecList, pRecipient->nCursonLine, ABinfo.szName, ABinfo.szTelOrEmail, TRUE);
								}
								else
								{
									RECIPIENT_AddData(&pRecipient->RecList, ABinfo.szName, ptredit, TRUE);
								}
							}
							else
							{
								if (pContactNode)
								{
									RECIPIENT_ModifyData(&pRecipient->RecList, pRecipient->nCursonLine, ptredit, ptredit, FALSE);
								}
								else
									RECIPIENT_AddData(&pRecipient->RecList, ptredit, ptredit, FALSE);
							}
						}
						else
						{
							if(APP_GetNameByPhone(ptredit, &ABinfo))
							{
								
								pRecipient->ComEditProc(hWnd,EM_REPLACESEL, 0, (LPARAM)ABinfo.szName);
								if (pContactNode) {
									RECIPIENT_ModifyData(&pRecipient->RecList, pRecipient->nCursonLine, ABinfo.szName, ABinfo.szTelOrEmail, TRUE);
								}
								else
								{
									RECIPIENT_AddData(&pRecipient->RecList, ABinfo.szName, ptredit, TRUE);
								}
							}
							else
							{
								if (pContactNode)
								{
									RECIPIENT_ModifyData(&pRecipient->RecList, pRecipient->nCursonLine, ptredit, ptredit, FALSE);
								}
								else
									RECIPIENT_AddData(&pRecipient->RecList, ptredit, ptredit, FALSE);
							}
						}						
						
					}
				}
				else	//数据结构发生了错误,修复数据
				{
					//记录刚才的输入数据
					if (PLXstrchr(ptredit, EMAILSIGN)) 
					{
						if(APP_GetNameByEmail(ptredit, &ABinfo))
						{
								RECIPIENT_InsertData(&pRecipient->RecList, pRecipient->nCursonLine, ABinfo.szName, ptredit, TRUE);
							
						}
						else
						{
								RECIPIENT_InsertData(&pRecipient->RecList, pRecipient->nCursonLine, ptredit, ptredit, FALSE);
						}
					}
					else
					{
						if(APP_GetNameByPhone(ptredit, &ABinfo))
						{							
							RECIPIENT_InsertData(&pRecipient->RecList, pRecipient->nCursonLine, ABinfo.szName, ABinfo.szTelOrEmail, TRUE);							
						}
						else
						{
							RECIPIENT_InsertData(&pRecipient->RecList, pRecipient->nCursonLine, ptredit, ptredit, FALSE);						
						}
					}
					//整理数据
					UpdateContextWnd(hWnd, &pRecipient->RecList, pRecipient->ComEditProc);
				}
			}
			pRecipient->nCurLen = pRecipient->ComEditProc(hWnd,EM_GETLINECOUNT,0,0);
			if ((pRecipient->nCurLen<=MAXVISIBLERECIPIENT) &&(pRecipient->nLastLineNum != pRecipient->nCurLen) )
			{
				pRecipient->nLastLineNum = pRecipient->nCurLen;
				SendMessage(pRecipient->hParentWnd,REC_SIZE,pRecipient->y_mainWnd+pRecipient->height_mainwnd+(pRecipient->nCurLen-1)*EDITWND_INTERVAL,0);
				MoveWindow(hWnd,pRecipient->x_mainWnd,pRecipient->y_mainWnd,pRecipient->width_mainwnd,pRecipient->height_mainwnd+(pRecipient->nCurLen-1)*EDITWND_INTERVAL,TRUE);
				
			}			
			if (pRecipient->nCursonLine == 0) 
			{
				pRecipient->bLostFocusByUporDown = TRUE;
			}
			pRecipient->ComEditProc(hWnd,WM_KEYDOWN,VK_UP,lParam);
			pRecipient->ComEditProc(hWnd,WM_KEYDOWN,VK_END,0);
			InvalidateRect(hWnd, NULL, TRUE);
			break;

		case VK_DOWN:			
			pRecipient->nCurLen = pRecipient->ComEditProc(hWnd,EM_GETLINECOUNT,0,0);
			pRecipient->nCursonLine = curline	=	GetCurIndexInRecEdit(hWnd);
			pRecipient->nCursonPos = (WORD)pRecipient->ComEditProc(hWnd, EM_GETSEL, 0, 0);
			pContactNode = RECIPIENT_GetPDATA(&pRecipient->RecList, pRecipient->nCursonLine);

			if(SendMessage(hWnd, EM_LINELENGTH, (WORD)SendMessage(hWnd, EM_GETSEL, 0, 0), 0) <=0)
			{
				pRecipient->ComEditProc(hWnd,WM_KEYDOWN,VK_LEFT,0);
				pRecipient->ComEditProc(hWnd,WM_KEYDOWN,VK_DELETE,0);
				pRecipient->ComEditProc(hWnd,WM_KEYDOWN,VK_END,0);
				if(pContactNode)
					RECIPIENT_DelData(&pRecipient->RecList, pRecipient->nCursonLine);
				pRecipient->nCurLen = pRecipient->ComEditProc(hWnd,EM_GETLINECOUNT,0,0);
				if ((pRecipient->nCurLen<=MAXVISIBLERECIPIENT) &&(pRecipient->nLastLineNum != pRecipient->nCurLen) )
				{
					pRecipient->nLastLineNum = pRecipient->nCurLen;
					SendMessage(pRecipient->hParentWnd,REC_SIZE,pRecipient->y_mainWnd+pRecipient->height_mainwnd+(pRecipient->nCurLen-1)*EDITWND_INTERVAL,0);
					MoveWindow(hWnd,pRecipient->x_mainWnd,pRecipient->y_mainWnd,pRecipient->width_mainwnd,pRecipient->height_mainwnd+(pRecipient->nCurLen-1)*EDITWND_INTERVAL,TRUE);
					//MoveWindow(hWnd,EDITWND_LEFT,EDITWND_TOP,EDITWND_WIDTH,EDITWND_HEIGHT+(pRecipient->nCurLen-1)*EDITWND_INTERVAL,TRUE);
					
				}
				
			}
			else
			{
				
				//判断本行的最后一个字符是否是分号,应该保证光标不在行的结尾都有分号.
				pRecipient->ComEditProc(hWnd,WM_KEYDOWN,VK_END,0);
				pRecipient->ComEditProc(hWnd,WM_KEYDOWN,VK_HOME,CK_SHIFT);
				/*
				nLineLen = pRecipient->ComEditProc(hWnd, EM_LINELENGTH, pRecipient->nCursonLastLine, 0);
				tmpSelIndex =LOWORD(pRecipient->ComEditProc(hWnd, EM_GETSEL, 0, 0));
				pRecipient->ComEditProc(hWnd,EM_SETSEL,tmpSelIndex-nLineLen,tmpSelIndex);
				*/
				pRecipient->ComEditProc(hWnd,EM_GETSELTEXT,0,(LPARAM)ptredit);
				pRecipient->ComEditProc(hWnd,WM_KEYDOWN,VK_END,0);
				if (ptredit[strlen(ptredit) -1 ] != ';')				
				{
					SendMessage(hWnd, WM_CHAR, ';', 0);
					pRecipient->nCursonPos = (WORD)pRecipient->ComEditProc(hWnd, EM_GETSEL, 0, 0);
				}
				else
				{
					//pRecipient->ComEditProc(hWnd,WM_KEYDOWN,VK_HOME,0);
					//pRecipient->ComEditProc(hWnd,WM_KEYDOWN,VK_END,VK_SHIFT);
					//pRecipient->ComEditProc(hWnd, EM_GETSELTEXT, 0, (LPARAM)ptredit);
					ptredit[strlen(ptredit) -1 ] = '\0';
					if(pContactNode)//必定存在,否则意味着数据结构发生了错误
					{				
						if (!pContactNode->bExistInAB) 
						{							
							if (PLXstrchr(ptredit, EMAILSIGN)) 
							{
								if(APP_GetNameByEmail(ptredit, &ABinfo))
								{
									
									pRecipient->ComEditProc(hWnd,EM_REPLACESEL, 0, (LPARAM)ABinfo.szName);
									if (pContactNode) {
										//should pass here
										RECIPIENT_ModifyData(&pRecipient->RecList, pRecipient->nCursonLine, ABinfo.szName, ABinfo.szTelOrEmail, TRUE);
									}
									else
									{
										RECIPIENT_AddData(&pRecipient->RecList, ABinfo.szName, ptredit, TRUE);
									}
								}
								else
								{
									if (pContactNode)
									{
										RECIPIENT_ModifyData(&pRecipient->RecList, pRecipient->nCursonLine, ptredit, ptredit, FALSE);
									}
									else
										RECIPIENT_AddData(&pRecipient->RecList, ptredit, ptredit, FALSE);
								}
							}
							else
							{
								if(APP_GetNameByPhone(ptredit, &ABinfo))
								{
									
									pRecipient->ComEditProc(hWnd,EM_REPLACESEL, 0, (LPARAM)ABinfo.szName);
									if (pContactNode) {
										RECIPIENT_ModifyData(&pRecipient->RecList, pRecipient->nCursonLine, ABinfo.szName, ABinfo.szTelOrEmail, TRUE);
									}
									else
									{
										RECIPIENT_AddData(&pRecipient->RecList, ABinfo.szName, ptredit, TRUE);
									}
								}
								else
								{
									if (pContactNode)
									{
										RECIPIENT_ModifyData(&pRecipient->RecList, pRecipient->nCursonLine, ptredit, ptredit, FALSE);
									}
									else
										RECIPIENT_AddData(&pRecipient->RecList, ptredit, ptredit, FALSE);
								}
							}						
							
						}
					}
					else	//数据结构发生了错误,修复数据
					{
						//记录刚才的输入数据
						if (PLXstrchr(ptredit, EMAILSIGN)) 
						{
							if(APP_GetNameByEmail(ptredit, &ABinfo))
							{
								RECIPIENT_InsertData(&pRecipient->RecList, pRecipient->nCursonLine, ABinfo.szName, ptredit, TRUE);
								
							}
							else
							{
								RECIPIENT_InsertData(&pRecipient->RecList, pRecipient->nCursonLine, ptredit, ptredit, FALSE);
							}
						}
						else
						{
							if(APP_GetNameByPhone(ptredit, &ABinfo))
							{							
								RECIPIENT_InsertData(&pRecipient->RecList, pRecipient->nCursonLine, ABinfo.szName, ABinfo.szTelOrEmail, TRUE);							
							}
							else
							{
								RECIPIENT_InsertData(&pRecipient->RecList, pRecipient->nCursonLine, ptredit, ptredit, FALSE);						
							}
						}
						//整理数据
						UpdateContextWnd(hWnd, &pRecipient->RecList, pRecipient->ComEditProc);
					}
					
				}
			}
			pRecipient->ComEditProc(hWnd, EM_SETSEL, pRecipient->nCursonPos, pRecipient->nCursonPos);
			curline = GetCurIndexInRecEdit(hWnd);
			pRecipient->nCurLen = pRecipient->ComEditProc(hWnd, EM_GETLINECOUNT, 0, 0);
			if (curline == (pRecipient->nCurLen-1))
			{
				if (pRecipient->style&CS_NOSYSCTRL) 
				{
					pRecipient->bLostFocusByUporDown =TRUE;
					/*
					pRecipient->nCurLen = pRecipient->ComEditProc(hWnd,EM_GETLINECOUNT,0,0);
					if(pRecipient->Firstman)
					{
						*(WORD *)pRecipient->Firstman = NMAXCHAR;
						linelength = pRecipient->ImeEditProc(hWnd,EM_GETLINE,0, (LPARAM)pRecipient->Firstman);
						
						*(pRecipient->Firstman+linelength) = NULL;
						
						memset(buf, 0, 30);
						GetExtentFittedText(NULL,pRecipient->Firstman, -1, buf, 30, pRecipient->width_mainwnd, '.', 3);
						
						if (pRecipient->nCurLen>1) {
							
							if (strcmp(buf, pRecipient->Firstman) == 0) {
								strcat(buf,"...");
								pRecipient->ComEditProc(hWnd,WM_SETTEXT,0,(LPARAM)buf);
							}
							else
								pRecipient->ComEditProc(hWnd,WM_SETTEXT,0,(LPARAM)buf);
							
						}			
						
//						MoveWindow(hWnd,pRecipient->x_mainWnd,pRecipient->y_mainWnd,pRecipient->width_mainwnd,pRecipient->height_mainwnd,TRUE);
//						SendMessage(pRecipient->hParentWnd,REC_SIZE, pRecipient->y_mainWnd + pRecipient->y_mainWnd+pRecipient->height_mainwnd,0);
						
					}
					*/
					PostMessage(GetParent(hWnd), message,wParam,lParam);
				}
				else
				{
					hWndFocus = GetFocus();
					while(GetParent(hWndFocus) != pRecipient->hParentWnd)
						hWndFocus = GetParent(hWndFocus);
					hWndFocus = GetNextDlgTabItem(pRecipient->hParentWnd, hWndFocus, FALSE);
					pRecipient->bLostFocusByUporDown = TRUE;
					/*
					pRecipient->nCurLen = pRecipient->ComEditProc(hWnd,EM_GETLINECOUNT,0,0);
					if(pRecipient->Firstman)
					{
						*(WORD *)pRecipient->Firstman = NMAXCHAR;
						linelength = pRecipient->ImeEditProc(hWnd,EM_GETLINE,0, (LPARAM)pRecipient->Firstman);
						
						*(pRecipient->Firstman+linelength) = NULL;
						
						memset(buf, 0, 30);
						GetExtentFittedText(NULL,pRecipient->Firstman, -1, buf, 30, pRecipient->width_mainwnd, '.', 3);
						
						if (pRecipient->nCurLen>1) {
							
							if (strcmp(buf, pRecipient->Firstman) == 0) {
								strcat(buf,"...");
								pRecipient->ComEditProc(hWnd,WM_SETTEXT,0,(LPARAM)buf);
							}
							else
								pRecipient->ComEditProc(hWnd,WM_SETTEXT,0,(LPARAM)buf);
							
						}			
						
//						MoveWindow(hWnd,pRecipient->x_mainWnd,pRecipient->y_mainWnd,pRecipient->width_mainwnd,pRecipient->height_mainwnd,TRUE);
//						SendMessage(pRecipient->hParentWnd,REC_SIZE, pRecipient->y_mainWnd + pRecipient->y_mainWnd+pRecipient->height_mainwnd,0);
						
					}
					*/
					SetFocus(hWndFocus);
				}
			}
			else
			{
				pRecipient->ComEditProc(hWnd,message,wParam,lParam);
				lResult = pRecipient->ComEditProc(hWnd,WM_KEYDOWN,VK_END,0);
				pRecipient->nCursonLine++;
			}
			InvalidateRect(hWnd, NULL, TRUE);
			break;

		case VK_LEFT:
			pRecipient->nCursonLine =	GetCurIndexInRecEdit(hWnd);
			pContactNode = RECIPIENT_GetPDATA(&pRecipient->RecList, pRecipient->nCursonLine);
			if (!pContactNode) {
				chPreCurson = GetCharPreCurson(hWnd, pRecipient->ImeEditProc);
				if (chPreCurson == 0x0a) {
					SendMessage(hWnd, WM_KEYDOWN, VK_UP, 0);
				}
				else
					pRecipient->ImeEditProc(hWnd,message,wParam,lParam);
				break;
			}
			if (pContactNode->bExistInAB) 
			{
				pRecipient->ImeEditProc(hWnd,WM_KEYDOWN,VK_HOME,0);
			}
			else
			{
				chPreCurson = GetCharPreCurson(hWnd, pRecipient->ImeEditProc);
				if (chPreCurson == 0x0a) {
					SendMessage(hWnd, WM_KEYDOWN, VK_UP, 0);
				}
				else
				pRecipient->ImeEditProc(hWnd,message,wParam,lParam);
			}
			break;

		case VK_RIGHT:
			pRecipient->nCursonLine =	GetCurIndexInRecEdit(hWnd);
			pContactNode = RECIPIENT_GetPDATA(&pRecipient->RecList, pRecipient->nCursonLine);
			if (!pContactNode) {
				chPreCurson = GetCharPreCurson(hWnd, pRecipient->ImeEditProc);
				if (chPreCurson == 0x0a) {
					SendMessage(hWnd, WM_KEYDOWN, VK_DOWN, 0);
				}
				else
					pRecipient->ImeEditProc(hWnd,message,wParam,lParam);
				break;
			}
			if (pContactNode->bExistInAB) 
			{
				pRecipient->ImeEditProc(hWnd,WM_KEYDOWN,VK_END,0);
			}
			else
			{
				pRecipient->ImeEditProc(hWnd, EM_GETSELTEXT, 1, (LPARAM)ptredit);
				//chPreCurson = GetCharPreCurson(hWnd, pRecipient->ImeEditProc);
				if (ptredit[0] == 0x0a) {
					SendMessage(hWnd, WM_KEYDOWN, VK_DOWN, 0);
				}
				else
					pRecipient->ImeEditProc(hWnd,message,wParam,lParam);
			}
			break;

		default:
			lResult = pRecipient->ImeEditProc(hWnd,message,wParam,lParam);
			break;
		}
		break;

	case GHP_GETREC:
		{
			tmpSelIndex = (WORD)SendMessage(hWnd, EM_GETSEL, 0, 0);
			curline = SendMessage(hWnd, EM_GETLINECOUNT, 0, 0);
//			//光标位置改变||当前行不是最后一行||
//			if ((tmpSelIndex != (WORD)pRecipient->nCursonPos) ||(pRecipient->nLastLineNum != pRecipient->RecList.nDataNum) || (pRecipient->nLastLineNum != curline))
//			{
				
				pRecipient->nCursonLastLine = pRecipient->nCursonLine = GetCurIndexInRecEdit(hWnd);
				
				
				pContactNode = RECIPIENT_GetPDATA(&pRecipient->RecList, pRecipient->nCursonLine);
				//如果光标所在行为空,向上移动等价于向前删除一个字符,而且应该判断数据结构中是否存在改行,然后
				//进行资源得释放.
				if(SendMessage(hWnd, EM_LINELENGTH, (WORD)SendMessage(hWnd, EM_GETSEL, 0, 0), 0) <=0)
				{
					pRecipient->ComEditProc(hWnd,WM_KEYDOWN,VK_LEFT,0);
					pRecipient->ComEditProc(hWnd,WM_KEYDOWN,VK_DELETE,0);
					pRecipient->ComEditProc(hWnd,WM_KEYDOWN,VK_END,0);
					if(pContactNode)
						RECIPIENT_DelData(&pRecipient->RecList, pRecipient->nCursonLine);
					pRecipient->nCurLen = pRecipient->ComEditProc(hWnd,EM_GETLINECOUNT,0,0);
					if ((pRecipient->nCurLen<=MAXVISIBLERECIPIENT) &&(pRecipient->nLastLineNum != pRecipient->nCurLen) )
					{
						pRecipient->nLastLineNum = pRecipient->nCurLen;
						SendMessage(pRecipient->hParentWnd,REC_SIZE,pRecipient->y_mainWnd+pRecipient->height_mainwnd+(pRecipient->nCurLen-1)*EDITWND_INTERVAL,0);
						MoveWindow(hWnd,pRecipient->x_mainWnd,pRecipient->y_mainWnd,pRecipient->width_mainwnd,pRecipient->height_mainwnd+(pRecipient->nCurLen-1)*EDITWND_INTERVAL,TRUE);
						
					}
					break;
				}
				
				//判断本行的最后一个字符是否是分号,应该保证光标不在行的结尾都有分号.
				
				pRecipient->ComEditProc(hWnd,WM_KEYDOWN,VK_END,0);
				pRecipient->ComEditProc(hWnd, WM_KEYDOWN, VK_HOME, CK_SHIFT);
				/*
				nLineLen = pRecipient->ComEditProc(hWnd, EM_LINELENGTH, pRecipient->nCursonLastLine, 0);
				tmpSelIndex =LOWORD(pRecipient->ComEditProc(hWnd, EM_GETSEL, 0, 0));
				pRecipient->ComEditProc(hWnd,EM_SETSEL,tmpSelIndex-nLineLen,tmpSelIndex);
				*/
				pRecipient->ComEditProc(hWnd,EM_GETSELTEXT,0,(LPARAM)ptredit);
				pRecipient->ComEditProc(hWnd,WM_KEYDOWN,VK_END,0);
				if (ptredit[strlen(ptredit) -1 ] != ';') 
				{
					if (ptredit[strlen(ptredit) -4 ] != ';') {
						SendMessage(hWnd, WM_CHAR, ';', 0);
					}
					//SendMessage(hWnd, WM_CHAR, ';', 0);
				}
				//pContactNode = RECIPIENT_GetPDATA(&pRecipient->RecList, pRecipient->nCursonLine);
				//如果有分号,只是改变了中间的输入号码,此中情况下,该号码不可能存在于电话本.
				else
				{
					
					//该行不是新行,已经输入过recipient,而且可能对改行进行了操作;
					//pRecipient->ComEditProc(hWnd,WM_KEYDOWN,VK_HOME,0);
					//pRecipient->ComEditProc(hWnd,WM_KEYDOWN,VK_END,VK_SHIFT);
					//pRecipient->ComEditProc(hWnd, EM_GETSELTEXT, 0, (LPARAM)ptredit);
					ptredit[strlen(ptredit) -1 ] = '\0';
					
					//nLineLen = pRecipient->ComEditProc(hWnd, EM_LINELENGTH, pRecipient->nCursonLastLine, 0);
					
					if(pContactNode)//必定存在,否则意味着数据结构发生了错误
					{				
						if (!pContactNode->bExistInAB) 
						{						
							if (PLXstrchr(ptredit, EMAILSIGN)) 
							{
								if(APP_GetNameByEmail(ptredit, &ABinfo))
								{
									
									pRecipient->ComEditProc(hWnd,EM_REPLACESEL, 0, (LPARAM)ABinfo.szName);
									if (pContactNode) {
										//should pass here
										RECIPIENT_ModifyData(&pRecipient->RecList, pRecipient->nCursonLine, ABinfo.szName, ABinfo.szTelOrEmail, TRUE);
									}
									else
									{
										RECIPIENT_AddData(&pRecipient->RecList, ABinfo.szName, ptredit, TRUE);
									}
								}
								else
								{
									if (pContactNode)
									{
										RECIPIENT_ModifyData(&pRecipient->RecList, pRecipient->nCursonLine, ptredit, ptredit, FALSE);
									}
									else
										RECIPIENT_AddData(&pRecipient->RecList, ptredit, ptredit, FALSE);
								}
							}
							else
							{
								if(APP_GetNameByPhone(ptredit, &ABinfo))
								{
									
									pRecipient->ComEditProc(hWnd,EM_REPLACESEL, 0, (LPARAM)ABinfo.szName);
									if (pContactNode) {
										RECIPIENT_ModifyData(&pRecipient->RecList, pRecipient->nCursonLine, ABinfo.szName, ABinfo.szTelOrEmail, TRUE);
									}
									else
									{
										RECIPIENT_AddData(&pRecipient->RecList, ABinfo.szName, ptredit, TRUE);
									}
								}
								else
								{
									if (pContactNode)
									{
										RECIPIENT_ModifyData(&pRecipient->RecList, pRecipient->nCursonLine, ptredit, ptredit, FALSE);
									}
									else
										RECIPIENT_AddData(&pRecipient->RecList, ptredit, ptredit, FALSE);
								}
							}						
							
						}
					}
					else	//数据结构发生了错误,修复数据
					{
						//记录刚才的输入数据
						if (PLXstrchr(ptredit, EMAILSIGN)) 
						{
							if(APP_GetNameByEmail(ptredit, &ABinfo))
							{
								RECIPIENT_InsertData(&pRecipient->RecList, pRecipient->nCursonLine, ABinfo.szName, ptredit, TRUE);
								
							}
							else
							{
								RECIPIENT_InsertData(&pRecipient->RecList, pRecipient->nCursonLine, ptredit, ptredit, FALSE);
							}
						}
						else
						{
							if(APP_GetNameByPhone(ptredit, &ABinfo))
							{							
								RECIPIENT_InsertData(&pRecipient->RecList, pRecipient->nCursonLine, ABinfo.szName, ABinfo.szTelOrEmail, TRUE);							
							}
							else
							{
								RECIPIENT_InsertData(&pRecipient->RecList, pRecipient->nCursonLine, ptredit, ptredit, FALSE);						
							}
						}
						//整理数据
						UpdateContextWnd(hWnd, &pRecipient->RecList, pRecipient->ComEditProc);
					}
				}
			//}
			InvalidateRect(hWnd, NULL, TRUE);
			
			memcpy((PRECIPIENTLISTBUF)lParam, &pRecipient->RecList, sizeof(RECIPIENTLISTBUF));
			
		}
		break;

	case REC_CLEAR:
		{
			ClearContactInAB(&pRecipient->RecList);
			UpdateContextWnd(hWnd, &pRecipient->RecList, pRecipient->ComEditProc);			
		}
		break;

	case GHP_ADDREC:
		if (pRecipient->RecList.nDataNum >= pRecipient->nMaxRecipient) 
		{
			break;
		}
		if (lParam == NULL) {
			break;
		}
		pRecipient->nCursonLine = GetCurIndexInRecEdit(hWnd);
		if (pRecipient->nCursonLine == pRecipient->nMaxRecipient)
		{
			pRecipient->ComEditProc(hWnd,WM_KEYDOWN,VK_END,0);
			pRecipient->ComEditProc(hWnd,WM_KEYDOWN,VK_HOME, CK_SHIFT);
			pRecipient->ComEditProc(hWnd,WM_CLEAR, 0, 0);
		}
		pRecipient->bCallAB = TRUE;
		if(wParam == 0)
		{
			memcpy(&ABinfo , (PABNAMEOREMAIL)lParam, sizeof(ABNAMEOREMAIL));
			//pRecipient->ImeEditProc(hWnd,EM_SETSEL, -1, -1);		
			pRecipient->ComEditProc(hWnd,WM_KEYDOWN,VK_END,0);
			linelength = SendMessage(hWnd, EM_LINELENGTH, (WORD)SendMessage(hWnd, EM_GETSEL, 0, 0), 0);		
			if (linelength <1) 
			{
				pRecipient->ComEditProc(hWnd,EM_REPLACESEL,0,(LPARAM)ABinfo.szName);
				RECIPIENT_AddData(&pRecipient->RecList, ABinfo.szName, ABinfo.szTelOrEmail, TRUE);
				pRecipient->ComEditProc(hWnd,WM_CHAR,';',0);
			}
			else
			{
				pRecipient->ComEditProc(hWnd,WM_KEYDOWN,VK_LEFT,CK_SHIFT);
				pRecipient->ComEditProc(hWnd,EM_GETSELTEXT,0,(LPARAM)ptredit);
				if(ptredit[0] != ';')
				{
					pRecipient->ImeEditProc(hWnd,WM_KEYDOWN,VK_END,0);
					SendMessage(hWnd,WM_CHAR,';',0);
				}
				pRecipient->ImeEditProc(hWnd,EM_SETSEL, -1, -1);
				//pRecipient->ComEditProc(hWnd,WM_KEYDOWN,VK_END,0);
				pRecipient->ComEditProc(hWnd,WM_CHAR,NEWLINE,0);
				
				pRecipient->nCurLen = pRecipient->ComEditProc(hWnd,EM_GETLINECOUNT,0,0);
				if ((pRecipient->nCurLen<=MAXVISIBLERECIPIENT)&&(pRecipient->nCurLen != pRecipient->nLastLineNum)) 
				{
					pRecipient->nLastLineNum = pRecipient->nCurLen;
					SendMessage(pRecipient->hParentWnd,REC_SIZE,pRecipient->y_mainWnd+pRecipient->height_mainwnd+(pRecipient->nCurLen-1)*EDITWND_INTERVAL,0);
					MoveWindow(hWnd,pRecipient->x_mainWnd,pRecipient->y_mainWnd,pRecipient->width_mainwnd,pRecipient->height_mainwnd+(pRecipient->nCurLen-1)*EDITWND_INTERVAL,TRUE);
				}
				pRecipient->ComEditProc(hWnd,EM_REPLACESEL,0,(LPARAM)ABinfo.szName);
				pRecipient->ComEditProc(hWnd,WM_CHAR,';',0);
				RECIPIENT_AddData(&pRecipient->RecList, ABinfo.szName, ABinfo.szTelOrEmail, TRUE);
			}
		}
		if (wParam == 1) 
		{
			char	tmp[5];
			strcpy(ptredit, (PSTR)lParam);
			pRecipient->ImeEditProc(hWnd,EM_SETSEL, -1, -1);		
			linelength = SendMessage(hWnd, EM_LINELENGTH, (WORD)SendMessage(hWnd, EM_GETSEL, 0, 0), 0);	
			
			if (linelength <1) 
			{
				if (PLXstrchr(ptredit, EMAILSIGN)) 
				{
					if(APP_GetNameByEmail(ptredit, &ABinfo))
					{
						pRecipient->ComEditProc(hWnd,EM_REPLACESEL,0,(LPARAM)ABinfo.szName);
						pRecipient->ComEditProc(hWnd,WM_CHAR,';',0);
						RECIPIENT_AddData(&pRecipient->RecList, ABinfo.szName, ABinfo.szTelOrEmail, TRUE);
					}
					else
					{
						pRecipient->ComEditProc(hWnd,EM_REPLACESEL,0,(LPARAM)ptredit);
						pRecipient->ComEditProc(hWnd,WM_CHAR,';',0);
						RECIPIENT_AddData(&pRecipient->RecList, ptredit, ptredit, FALSE);
					}
				}
				else
				{
					if(APP_GetNameByPhone(ptredit, &ABinfo))
					{
						pRecipient->ComEditProc(hWnd,EM_REPLACESEL,0,(LPARAM)ABinfo.szName);
						pRecipient->ComEditProc(hWnd,WM_CHAR,';',0);
						RECIPIENT_AddData(&pRecipient->RecList, ABinfo.szName, ABinfo.szTelOrEmail, TRUE);
					}
					else
					{
						pRecipient->ComEditProc(hWnd,EM_REPLACESEL,0,(LPARAM)ptredit);
						pRecipient->ComEditProc(hWnd,WM_CHAR,';',0);
						RECIPIENT_AddData(&pRecipient->RecList, ptredit, ptredit, FALSE);
					}
				}
			}
			else
			{
				pRecipient->ComEditProc(hWnd,WM_KEYDOWN,VK_LEFT,CK_SHIFT);
				pRecipient->ComEditProc(hWnd,EM_GETSELTEXT,0,(LPARAM)tmp);
				if(tmp[0] != ';')
				{
					pRecipient->ImeEditProc(hWnd,WM_KEYDOWN,VK_END,0);
					pRecipient->ComEditProc(hWnd,WM_CHAR,';',0);
				}
				pRecipient->ComEditProc(hWnd,WM_KEYDOWN,VK_END,0);
				pRecipient->ComEditProc(hWnd,WM_CHAR,NEWLINE,0);
				
				pRecipient->nCurLen = pRecipient->ComEditProc(hWnd,EM_GETLINECOUNT,0,0);
				if ((pRecipient->nCurLen<=MAXVISIBLERECIPIENT)&&(pRecipient->nCurLen != pRecipient->nLastLineNum)) 
				{
					pRecipient->nLastLineNum = pRecipient->nCurLen;
					SendMessage(pRecipient->hParentWnd,REC_SIZE,pRecipient->y_mainWnd+pRecipient->height_mainwnd+(pRecipient->nCurLen-1)*EDITWND_INTERVAL,0);
					MoveWindow(hWnd,pRecipient->x_mainWnd,pRecipient->y_mainWnd,pRecipient->width_mainwnd,pRecipient->height_mainwnd+(pRecipient->nCurLen-1)*EDITWND_INTERVAL,TRUE);
				}
				if (PLXstrchr(ptredit, EMAILSIGN)) 
				{
					if(APP_GetNameByEmail(ptredit, &ABinfo))
					{
						pRecipient->ComEditProc(hWnd,EM_REPLACESEL,0,(LPARAM)ABinfo.szName);
						pRecipient->ComEditProc(hWnd,WM_CHAR,';',0);
						RECIPIENT_AddData(&pRecipient->RecList, ABinfo.szName, ABinfo.szTelOrEmail, TRUE);
					}
					else
					{
						pRecipient->ComEditProc(hWnd,EM_REPLACESEL,0,(LPARAM)ptredit);
						pRecipient->ComEditProc(hWnd,WM_CHAR,';',0);
						RECIPIENT_AddData(&pRecipient->RecList, ptredit, ptredit, FALSE);
					}
				}
				else
				{
					if(APP_GetNameByPhone(ptredit, &ABinfo))
					{
						pRecipient->ComEditProc(hWnd,EM_REPLACESEL,0,(LPARAM)ABinfo.szName);
						pRecipient->ComEditProc(hWnd,WM_CHAR,';',0);
						RECIPIENT_AddData(&pRecipient->RecList, ABinfo.szName, ABinfo.szTelOrEmail, TRUE);
					}
					else
					{
						pRecipient->ComEditProc(hWnd,EM_REPLACESEL,0,(LPARAM)ptredit);
						pRecipient->ComEditProc(hWnd,WM_CHAR,';',0);
						RECIPIENT_AddData(&pRecipient->RecList, ptredit, ptredit, FALSE);
					}
				}
			}
		}
		break;

	case WM_DESTROY:

		if (pRecipient->AllMan != NULL) {
			free(pRecipient->AllMan);
			pRecipient->AllMan = NULL;
		}
		if (pRecipient->Firstman!=NULL) {
			free(pRecipient->Firstman);
			pRecipient->Firstman = NULL;
		}
		RECIPIENT_FreeListBuf(&pRecipient->RecList);
		lResult = pRecipient->ImeEditProc(hWnd,message,wParam,lParam);
		break;
	default:
		lResult = pRecipient->ImeEditProc(hWnd,message,wParam,lParam);
		break;
	}

	return lResult;
}

static	int	GetCurIndexInRecEdit(HWND hWnd)
{
	int	curindex = 0;
	int	i;
	WORD	chindex;
	char	*seltext;
	chindex	= (WORD)SendMessage(hWnd,EM_GETSEL,0,0);
	i	=	GetWindowTextLength(hWnd);
	seltext	=(PSTR)malloc(i+1);
	if (!seltext) {
		return	-1;
	}
	GetWindowText(hWnd,seltext,i);
	for(i=0; i<chindex;i++)
	{
		if (*(seltext+i) == 0x0d) {
			curindex++;
		}
	}
	free(seltext);
	seltext = NULL;
	return	curindex;
}

static	int	GetCharPreCurson(HWND	hWnd, WNDPROC EditProc)
{
//	int	nIndex;
	char	ch;
	char	buf[3];
//	nIndex = EditProc(hWnd, EM_GETSEL, 0, 0);
//	EditProc(hWnd, EM_SETSEL, LOWORD(nIndex)-1, LOWORD(nIndex)-1);
	EditProc(hWnd, EM_GETSELTEXT, -1, (LPARAM)buf);
//	EditProc(hWnd, EM_SETSEL, LOWORD(nIndex), LOWORD(nIndex));
	ch = buf[0];
	return ch;
}

static	void	ClearContactInAB(PRECIPIENTLISTBUF pListBuffer)
{
	PRECIPIENTLISTNODE	pNode1;
	int			i, nCount, k=0;
	nCount = pListBuffer->nDataNum;
	for (i=0; i< nCount; i++) 
	{
		pNode1 = RECIPIENT_GetPDATA(pListBuffer, i-k);
		if (!pNode1) {
			continue;
		}
		if (pNode1->bExistInAB) 
		{
			RECIPIENT_DelData(pListBuffer, i-k);
			k++;
		}
	}
	
}
