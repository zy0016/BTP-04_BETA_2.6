/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Bluetooth module
 *
 * Purpose  : 
 *
\**************************************************************************/

#include "window.h"
#include "string.h"
#include "mullang.h"
#include "winpda.h"
#include "prebrowhead.h"
#include "pubapp.h"
#include "imesys.h"
#include "unistd.h"
#include "sys/types.h"
#include "sys/stat.h"
#include "sys/statfs.h"
#include "fcntl.h"

#include "msgunibox.h"
#include "vcardvcal.h"
#include "vcard/vcaglb.h"

#include "BtMsg.h"
#include "BtString.h"

#define ID_BTSAVEENTRY         (WM_USER+0x300)
#define ID_BTMOVETOFOILDER     (WM_USER+0x301)
#define ID_BTMSGINFO           (WM_USER+0x302)
#define ID_BTMSGDELETE         (WM_USER+0x303)

#define BT_MOVE_FOLDER         (WM_USER+0x304)
#define CONFIRM_DELETEBTMSG    (WM_USER+0x305)

#define TIPS_RENAMENOTEPAD     (WM_USER+0x306)
#define TIPS_RENAMEPICTURE     (WM_USER+0x307)
#define TIPS_SAVERENAME        (WM_USER+0x308)

typedef struct tagBTVIEWMSGDATA
{	
	HWND       hWndFrame;
	HWND       hWndmu;
	HWND       hBtViewMsgMain;
	HMENU      hMainMenu;
	BOOL       bPre;
	BOOL       bNext;
 	PVOID      FileContent;      //指向文件内容的一块缓冲区
    int        nContentSize;
	PBTMSGHANDLENAME pBtMsghandle;
}BTVIEWMSGDATA,*PBTVIEWMSGDATA;

typedef struct tagBTRENAMEMSGDATA
{	
	HWND       hWndFrame;
	HWND       hBtRenameMsgEdit;
	PBTMSGHANDLENAME pBtMsghandle;
}BTRENAMEMSGDATA,*PBTRENAMEMSGDATA;

static LRESULT BtViewMsgWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static BOOL BtViewMsg_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct);
static void BtViewMsg_OnKeyDown(HWND hWnd, UINT nKey, UINT nFlags);
static void BtViewMsgChangeMenu(HWND hWnd);
static void BtViewMsg_OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
static HMENU CreateBtViewMsgMenu();
static BOOL BtDeleteMsg(HWND hWnd, PBTMSGHANDLENAME  pBtMsghandle);
static BOOL BtMoveMsg(PBTMSGHANDLENAME  pBtMsghandle,int nFolder);

static LRESULT BtRenameMsgWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

extern BOOL BtDeleteMsgFile(char* filename);
extern void FreeBtMsgHandle(PBTMSGHANDLENAME pBtMsgHandle);
extern BOOL BtMsgInfoWindow(HWND hWndFrame, HWND hWnd, PBTMSGHANDLENAME pBtMsghandle);
extern int  GetBtFileFolder(char *BtRealFileName);
extern PBTFOLDERINFO GetBtFolderByID(int nFolder);

extern BOOL CALE_DecodeVcalToDisplay(vCal_Obj *pObj, VCARDVCAL **ppChain);
extern void CALE_VcaleChainEarse(VCARDVCAL *pChain);
extern BOOL CALE_SaveFromSms(vCal_Obj *pObj);


extern BOOL MMS_SelectDestination(HWND hWndFrame, HWND hWnd, int msg, PCSTR title, 
					PCSTR pName, PCSTR pdata, int datalen, int metaType);

extern BOOL BtViewMsgWindow(HWND hWndFrame, HWND hWnd, PBTMSGHANDLENAME pBtMsghandle, 
					                 BOOL bPre, BOOL bNext)
{
   WNDCLASS	wc;
   BTVIEWMSGDATA  BtViewMsgData;
   HWND  hBtViewMsgWnd;
   RECT  rClient;

   memset(&BtViewMsgData, 0, sizeof(BTVIEWMSGDATA));
   
   BtViewMsgData.hWndFrame=hWndFrame;
   BtViewMsgData.hWndmu=hWnd;    //Unibox的窗口句柄
   BtViewMsgData.bPre=bPre;
   BtViewMsgData.bNext=bNext;
   BtViewMsgData.pBtMsghandle=pBtMsghandle;
   
   wc.style         = 0;
   wc.lpfnWndProc   = BtViewMsgWndProc;
   wc.cbClsExtra    = 0;
   wc.cbWndExtra    = sizeof(BTVIEWMSGDATA);
   wc.hInstance     = NULL;
   wc.hIcon         = NULL;
   wc.hCursor       = NULL;
   wc.lpszMenuName  = NULL;
   wc.lpszClassName = "BtViewMsgWndClass";    
   wc.hbrBackground = NULL;

   if (!RegisterClass(&wc))
   {
	   return FALSE;
   }

   GetClientRect(hWndFrame, &rClient);
   
   hBtViewMsgWnd = CreateWindow(
	   "BtViewMsgWndClass", 
	   "",
	   WS_VISIBLE | WS_CHILD | WS_VSCROLL,
	   rClient.left, rClient.top, rClient.right-rClient.left, rClient.bottom-rClient.top,
	   hWndFrame, 
	   NULL, 
	   NULL, 
	   (PVOID)&BtViewMsgData);
    
   if (NULL ==  hBtViewMsgWnd)
   {
	   UnregisterClass("BtViewMsgWndClass",NULL);
	   return FALSE;
   }
   
   SetFocus(hBtViewMsgWnd);
   
   ShowWindow(hBtViewMsgWnd,SW_SHOW);
   UpdateWindow(hBtViewMsgWnd);
   
   return TRUE;
}


static LRESULT BtViewMsgWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) 
{
    LRESULT lResult=TRUE;
	PBTVIEWMSGDATA pBtViewMsgData = NULL;

    pBtViewMsgData=GetUserData(hWnd);
 	
	switch (message)
    {
    case WM_CREATE :
		lResult = BtViewMsg_OnCreate(hWnd,(LPCREATESTRUCT)(lParam));
        break;
		
	case WM_SETFOCUS:
          SetFocus(pBtViewMsgData->hBtViewMsgMain);
        break;

    case PWM_SHOWWINDOW:
		{
		   int nCurBtMsgType;
		   PBTVIEWMSGDATA pBtViewMsgData = NULL;
		   pBtViewMsgData=GetUserData(hWnd);

		  SendMessage(pBtViewMsgData->hWndFrame,PWM_SETBUTTONTEXT,0,(LPARAM)IDP_BT_BUTTON_BACK);
		  SendMessage(pBtViewMsgData->hWndFrame,PWM_SETBUTTONTEXT,1,(LPARAM)"");
		  SendMessage(pBtViewMsgData->hWndFrame,PWM_SETBUTTONTEXT,2,(LPARAM)"Options");

		  nCurBtMsgType=pBtViewMsgData->pBtMsghandle->nBtMsgType;
		  
		  switch(nCurBtMsgType)
		  {
		  case MU_MSG_BT_VCARD:
			   SetWindowText(pBtViewMsgData->hWndFrame,IDP_BT_TITLE_BUSINESSCARD);
			  break;
		  case MU_MSG_BT_VCAL:
			    SetWindowText(pBtViewMsgData->hWndFrame,IDP_BT_TITLE_CALENDARENTRY);
			  break;
		  case MU_MSG_BT_NOTEPAD:
				SetWindowText(pBtViewMsgData->hWndFrame,IDP_BT_TITLE_NOTE);
			  break;
		  case MU_MSG_BT_PICTURE:
                SetWindowText(pBtViewMsgData->hWndFrame,pBtViewMsgData->pBtMsghandle->szBtMsgViewName);
			  break;
		  case MU_MSG_BT:
			    SetWindowText(pBtViewMsgData->hWndFrame,pBtViewMsgData->pBtMsghandle->szBtMsgViewName);
			  break;
		  default:
			  break;
		  }
          
		  PDASetMenu(pBtViewMsgData->hWndFrame,pBtViewMsgData->hMainMenu);

		  SetFocus(pBtViewMsgData->hBtViewMsgMain);
		}
        break;

	case BT_MOVE_FOLDER:
		{
			int BtMsgDestFold;
			
			if (wParam == 0)
				break;
			
			BtMsgDestFold = (int)lParam;
			
			BtMoveMsg(pBtViewMsgData->pBtMsghandle,BtMsgDestFold);			
	
			SendMessage(pBtViewMsgData->hWndmu, PWM_MSG_MU_DELETE, 
				MAKEWPARAM(MU_ERR_SUCC, MU_MDU_BT), 
				(LPARAM)(DWORD)pBtViewMsgData->pBtMsghandle);

			PLXTipsWin(pBtViewMsgData->hWndFrame, hWnd, 0,IDP_BT_STRING_MESSAGEMOVED, 
				IDP_BT_STRING_SELECTFOLDER,Notify_Success, 
				IDP_BT_BUTTON_OK, NULL,20);

			PostMessage(hWnd, WM_CLOSE, 0, 0);
		}
		break;
    case CONFIRM_DELETEBTMSG:
		if (lParam)
		{
			char BtDeleteMsgTitle[64];

			switch(pBtViewMsgData->pBtMsghandle->nBtMsgType)
			{
			case MU_MSG_BT_VCARD:
				strcpy(BtDeleteMsgTitle,IDP_BT_TITLE_BUSINESSCARD);
				break;
			case MU_MSG_BT_VCAL:
                strcpy(BtDeleteMsgTitle,IDP_BT_TITLE_CALENDARENTRY);
				break;
			case MU_MSG_BT_NOTEPAD:
				strcpy(BtDeleteMsgTitle,IDP_BT_TITLE_NOTE);
				break;
			case MU_MSG_BT_PICTURE:
                strcpy(BtDeleteMsgTitle,pBtViewMsgData->pBtMsghandle->szBtMsgViewName);
				break;
			default:
				break;
			}

			PostMessage(hWnd,WM_CLOSE,0,0);
			SendMessage(pBtViewMsgData->hWndFrame,PWM_CLOSEWINDOW,  (WPARAM) hWnd, (LPARAM)0);

            BtDeleteMsg(hWnd,pBtViewMsgData->pBtMsghandle);

			PLXTipsWin(pBtViewMsgData->hWndFrame, hWnd, 0,IDP_BT_STRING_DELETEDNOTIF, BtDeleteMsgTitle,
				Notify_Success, IDP_BT_BUTTON_OK, NULL,20);
		}
		break;

	case TIPS_RENAMENOTEPAD:
    case TIPS_RENAMEPICTURE:
        {
			HWND hBtRenameMsgWnd;
            WNDCLASS    wc;
			RECT rClient;
			BTRENAMEMSGDATA BtRenameMsgData;

			PBTRENAMEMSGDATA pBtRenameMsgData = NULL;

			memset(&BtRenameMsgData, 0, sizeof(BTRENAMEMSGDATA));

			BtRenameMsgData.hWndFrame=pBtViewMsgData->hWndFrame;
            BtRenameMsgData.pBtMsghandle=pBtViewMsgData->pBtMsghandle;
			
			wc.style         = 0;
			wc.lpfnWndProc   = BtRenameMsgWndProc;
			wc.cbClsExtra    = 0;
			wc.cbWndExtra    = 0;
			wc.hInstance     = NULL;
			wc.hIcon         = NULL;
			wc.hCursor       = NULL;
			wc.hbrBackground = NULL;
			wc.lpszMenuName  = NULL;
			wc.lpszClassName = "BtRenameMsgWndClass";
			
			if (!RegisterClass(&wc))
			{
				return FALSE;
			}
			
			GetClientRect(pBtViewMsgData->hWndFrame,&rClient);
			
			hBtRenameMsgWnd = CreateWindow(
				"BtRenameMsgWndClass",
				"", 
				WS_VISIBLE | WS_CHILD,
				rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
				pBtViewMsgData->hWndFrame, 
				NULL, 
				NULL, 
				(PVOID)&BtRenameMsgData);
			
			if (NULL ==  hBtRenameMsgWnd)
			{
				UnregisterClass("BtRenameMsgWndClass",NULL);
				return FALSE;
			}
			
			SetFocus(hBtRenameMsgWnd);
			
			ShowWindow(hBtRenameMsgWnd,SW_SHOW);
			UpdateWindow(hBtRenameMsgWnd);
			
			pBtRenameMsgData=GetUserData(hBtRenameMsgWnd);

			SendMessage(pBtRenameMsgData->hBtRenameMsgEdit, EM_SETSEL, -1, -1);			
        }
		break;

	case WM_KEYDOWN:
		BtViewMsg_OnKeyDown(hWnd, (UINT)wParam, (UINT)lParam);
		break;

	case WM_COMMAND:
		BtViewMsg_OnCommand(hWnd, wParam, lParam);	
		break;
		
	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;
		
    case WM_DESTROY :
		if(pBtViewMsgData->FileContent!=NULL)
			free(pBtViewMsgData->FileContent);
		
        UnregisterClass("BtViewMsgWndClass", NULL);
        break;

    default:    
        lResult = PDADefWindowProc(hWnd, message, wParam, lParam);
        break;
    }
    
    return lResult;
}

static BOOL BtViewMsg_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
{
	int nCurBtMsgType;
	HMENU hBtViewMsgMainMenu;
    HWND hBtViewMsgMain=NULL;
    LISTBUF		Listtmp;
	RECT rClient;
	int fd;

	UINT BtMsgSize;

	PBTVIEWMSGDATA pBtViewMsgData = NULL;
	HWND hBtMsgWndFrame = NULL;

	pBtViewMsgData=GetUserData(hWnd);
	memcpy(pBtViewMsgData,lpCreateStruct->lpCreateParams,sizeof(BTVIEWMSGDATA));

	GetClientRect(hWnd,&rClient);
    hBtMsgWndFrame = pBtViewMsgData->hWndFrame;
	
	hBtViewMsgMainMenu=CreateBtViewMsgMenu();  //创建蓝牙Viewer的菜单
	PDASetMenu(hBtMsgWndFrame,hBtViewMsgMainMenu);
	
	pBtViewMsgData->hMainMenu=hBtViewMsgMainMenu;

	nCurBtMsgType=pBtViewMsgData->pBtMsghandle->nBtMsgType;
	
	switch(nCurBtMsgType)
	{
	case MU_MSG_BT_VCARD:
		{
			PVCARDVCAL   pVcardVcal;
			vCard_Obj *pvCardObj = NULL;

			PREBROW_InitListBuf (&Listtmp);

			chdir(BTRECV_FILEPATH);

			BtMsgSize=BtGetMsgSize(pBtViewMsgData->pBtMsghandle->szBtMsgRealName);
			
            pBtViewMsgData->nContentSize = BtMsgSize;

            if(BtMsgSize == 0)
                return FALSE;

			pBtViewMsgData->FileContent=(char *)malloc(BtMsgSize);
			
            if(pBtViewMsgData->FileContent == NULL)
                return FALSE;

			memset(pBtViewMsgData->FileContent,0,BtMsgSize);
			
			fd = open(pBtViewMsgData->pBtMsghandle->szBtMsgRealName, O_RDONLY);
            
			if(fd==-1)
				return FALSE;

			read(fd,pBtViewMsgData->FileContent,BtMsgSize);
			
			close(fd);
			
			vCard_Reader(pBtViewMsgData->FileContent,strlen(pBtViewMsgData->FileContent),&pvCardObj);
			
			if(pvCardObj == NULL)
				return FALSE;
			
			APP_AnalyseVcard(pvCardObj,&pVcardVcal);
            
			while(pVcardVcal!=NULL)
			{
				PREBROW_AddData(&Listtmp, pVcardVcal->pszTitle, 
					pVcardVcal->pszContent);
				
                pVcardVcal=pVcardVcal->pNext;
			}
			
			hBtViewMsgMain=CreateWindow(
				FORMVIEWER,
				"",
				WS_VISIBLE  |WS_CHILD | WS_VSCROLL,
				rClient.left,
				rClient.top,
				rClient.right - rClient.left,
				rClient.bottom - rClient.top,
				hWnd,
				NULL,
				NULL, 
				(PVOID)&Listtmp);
			
			vCard_Clear(VCARD_OPT_READER,(PVOID)pvCardObj);
             
		    SetWindowText(hBtMsgWndFrame,IDP_BT_TITLE_BUSINESSCARD);
		}
		break;
	case MU_MSG_BT_VCAL:
		{
			PVCARDVCAL   pVcardVcal;
			vCal_Obj *pvCalObj = NULL;

			PREBROW_InitListBuf (&Listtmp);

			chdir(BTRECV_FILEPATH);
			
			BtMsgSize=BtGetMsgSize(pBtViewMsgData->pBtMsghandle->szBtMsgRealName);

            pBtViewMsgData->nContentSize = BtMsgSize;

            if(BtMsgSize == 0)
                return FALSE;

			pBtViewMsgData->FileContent=(char *)malloc(BtMsgSize);

            if(pBtViewMsgData->FileContent == NULL)
                return FALSE;
			
			memset(pBtViewMsgData->FileContent,0,BtMsgSize);
			
			fd = open(pBtViewMsgData->pBtMsghandle->szBtMsgRealName, O_RDONLY);

			if(fd==-1)
				return FALSE;
			
			read(fd,pBtViewMsgData->FileContent,BtMsgSize);
			
			close(fd);

			vCal_Reader(pBtViewMsgData->FileContent,strlen(pBtViewMsgData->FileContent),&pvCalObj);

			if(pvCalObj == NULL)
				return FALSE;
            
            CALE_DecodeVcalToDisplay(pvCalObj,&pVcardVcal);

			while(pVcardVcal!=NULL)
			{
				PREBROW_AddData(&Listtmp, pVcardVcal->pszTitle, 
					pVcardVcal->pszContent);
				
                pVcardVcal=pVcardVcal->pNext;
			}

		    hBtViewMsgMain=CreateWindow(
				FORMVIEWER,
				"",
				WS_VISIBLE  |WS_CHILD | WS_VSCROLL,
				rClient.left,
				rClient.top,
				rClient.right - rClient.left,
				rClient.bottom - rClient.top,
				hWnd,
				NULL,
				NULL, 
			    (PVOID)&Listtmp);

		  CALE_VcaleChainEarse(pVcardVcal);
			
// 		  vCal_Clear(VCAL_OPT_READER, (PVOID)pvCalObj);		

          SetWindowText(hBtMsgWndFrame,IDP_BT_TITLE_CALENDARENTRY);

		}
		break;
  
	case MU_MSG_BT_PICTURE:
		{  
//        chdir(BTRECV_FILEPATH);
            
            chdir(BTRECV_FILEPATH);
            
            BtMsgSize=BtGetMsgSize(pBtViewMsgData->pBtMsghandle->szBtMsgRealName);
            
            pBtViewMsgData->nContentSize = BtMsgSize;

            if(BtMsgSize == 0)
                return FALSE;

            pBtViewMsgData->FileContent=(char *)malloc(BtMsgSize);

            if(pBtViewMsgData->FileContent == NULL)
                return FALSE;
            
            memset(pBtViewMsgData->FileContent,0,BtMsgSize);
            
            fd = open(pBtViewMsgData->pBtMsghandle->szBtMsgRealName, O_RDONLY);
            
            if(fd==-1)
                return FALSE;
            
            read(fd,pBtViewMsgData->FileContent,BtMsgSize);
            
            close(fd);
            
            hBtViewMsgMain=PlxTextView(hBtMsgWndFrame, hWnd, NULL, 0, 
                TRUE, NULL, pBtViewMsgData->FileContent, BtMsgSize);
            
            SetWindowText(hBtMsgWndFrame,pBtViewMsgData->pBtMsghandle->szBtMsgViewName);
		}
		break;
	case MU_MSG_BT_NOTEPAD:
		{
           chdir(BTRECV_FILEPATH);

		   BtMsgSize=BtGetMsgSize(pBtViewMsgData->pBtMsghandle->szBtMsgRealName);

           pBtViewMsgData->nContentSize = BtMsgSize;
           
           if(BtMsgSize == 0)
               return FALSE;
           
		   pBtViewMsgData->FileContent=(char *)malloc(BtMsgSize);
           
           if(pBtViewMsgData->FileContent == NULL)
               return FALSE;

           memset(pBtViewMsgData->FileContent,0,BtMsgSize);
		   
		   fd = open(pBtViewMsgData->pBtMsghandle->szBtMsgRealName, O_RDONLY);
           
		   if(fd==-1)
			   return FALSE;

		   read(fd,pBtViewMsgData->FileContent,BtMsgSize);

		   close(fd);
	   
		   hBtViewMsgMain=PlxTextView(hBtMsgWndFrame, hWnd, pBtViewMsgData->FileContent, 
			    BtMsgSize, FALSE, NULL, NULL, 0);
		   
		   SetWindowText(hBtMsgWndFrame,IDP_BT_TITLE_NOTE);
		}
	    break;

	default:
		{
			PLXTipsWin(hBtMsgWndFrame, hWnd, 0,IDP_BT_STRING_CANNOTOPENFORMAT, 
				pBtViewMsgData->pBtMsghandle->szBtMsgRealName,Notify_Failure, 
				IDP_BT_BUTTON_OK, NULL,20);
            
			PostMessage(hWnd,WM_CLOSE,0,0);
			SendMessage(hBtMsgWndFrame,PWM_CLOSEWINDOW,  (WPARAM) hWnd, (LPARAM)0);
			
		}
		break;
	}
    
    pBtViewMsgData->hBtViewMsgMain=hBtViewMsgMain;

	SendMessage(hBtMsgWndFrame,PWM_SETBUTTONTEXT,0,(LPARAM)IDP_BT_BUTTON_BACK);
 	SendMessage(hBtMsgWndFrame,PWM_SETBUTTONTEXT,1,(LPARAM)"");
	SendMessage(hBtMsgWndFrame,PWM_SETBUTTONTEXT,2,(LPARAM)"Options");
	
	return TRUE;	
}

static void BtViewMsg_OnKeyDown(HWND hWnd, UINT nKey, UINT nFlags)
{	
	PBTVIEWMSGDATA pBtViewMsgData = NULL;
    pBtViewMsgData=GetUserData(hWnd);
	
	switch (nKey)
	{	
	case VK_F5:	
		BtViewMsgChangeMenu(hWnd);
		SendMessage(GetParent(hWnd), WM_KEYDOWN, (WPARAM)nKey, (LPARAM)nFlags);
        break;
			
    case VK_F10:
		PostMessage(hWnd,WM_CLOSE,0,0);
		SendMessage(pBtViewMsgData->hWndFrame,PWM_CLOSEWINDOW,  (WPARAM) hWnd, (LPARAM)0);
		break;
		
	default:
		PDADefWindowProc(hWnd, WM_KEYDOWN, (WPARAM)nKey, (LPARAM)nFlags);
		break;
	}
}

static void BtViewMsg_OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	int fd;
	
	char *pBtMsgContent;
	UINT BtMsgSize;
	
	PBTVIEWMSGDATA pBtViewMsgData = NULL;
    pBtViewMsgData=GetUserData(hWnd);

	switch(LOWORD(wParam))
	{
	case ID_BTSAVEENTRY:
        {
			switch(pBtViewMsgData->pBtMsghandle->nBtMsgType)
			{
			case MU_MSG_BT_VCARD:
				{
                    BtMsgSize=BtGetMsgSize(pBtViewMsgData->pBtMsghandle->szBtMsgRealName);
					pBtMsgContent=(char *)malloc(BtMsgSize);
					
					memset(pBtMsgContent,0,BtMsgSize);
					
					chdir(BTRECV_FILEPATH);
					
					fd = open(pBtViewMsgData->pBtMsghandle->szBtMsgRealName, O_RDONLY,0);

					if(fd==-1)
						return;

					read(fd,pBtMsgContent,BtMsgSize);
					
					close(fd);

					APP_SaveVcard(pBtMsgContent,BtMsgSize);

					free(pBtMsgContent);

					PLXTipsWin(pBtViewMsgData->hWndFrame, hWnd, 0,IDP_BT_STRING_SAVETOCONTACTS, 
					    IDP_BT_TITLE_BUSINESSCARD,Notify_Success, 
					    IDP_BT_BUTTON_OK, NULL,20);
				}
				break;
			case MU_MSG_BT_VCAL:
				{
                vCal_Obj *pvCalObj = NULL;

				BtMsgSize=BtGetMsgSize(pBtViewMsgData->pBtMsghandle->szBtMsgRealName);
				pBtMsgContent=(char *)malloc(BtMsgSize);
				
				memset(pBtMsgContent,0,BtMsgSize);
				
				chdir(BTRECV_FILEPATH);
				
				fd = open(pBtViewMsgData->pBtMsghandle->szBtMsgRealName, O_RDONLY,0);
                
				if(fd==-1)
					return;

				read(fd,pBtMsgContent,BtMsgSize);
				
				close(fd);

				vCal_Reader(pBtMsgContent, BtMsgSize, &pvCalObj);
				CALE_SaveFromSms(pvCalObj);
				vCal_Clear(VCAL_OPT_READER,(PVOID)pvCalObj);
				free(pBtMsgContent);

                PLXTipsWin(pBtViewMsgData->hWndFrame, hWnd, 0,IDP_BT_STRING_SAVETOCALENDAR, 
					IDP_BT_TITLE_CALENDARENTRY,Notify_Success, 
					IDP_BT_BUTTON_OK, NULL,20);
				}
				break;
			case MU_MSG_BT_NOTEPAD:
				{
                    
                    MMS_SelectDestination(pBtViewMsgData->hWndFrame, NULL, 0, NULL, 
                        pBtViewMsgData->pBtMsghandle->szBtMsgViewName, pBtViewMsgData->FileContent, 
                        pBtViewMsgData->nContentSize, 0);

//                    char SaveFileName[256];                   
//					memset(SaveFileName,0,256);
//					
//					BtMsgSize=BtGetMsgSize(pBtViewMsgData->pBtMsghandle->szBtMsgRealName);
//					pBtMsgContent=(char *)malloc(BtMsgSize);
//					
//					memset(pBtMsgContent,0,BtMsgSize);
//					
//					chdir(BTRECV_FILEPATH);
//					
//					fd = open(pBtViewMsgData->pBtMsghandle->szBtMsgRealName, O_RDONLY,0);
//                    
//					if(fd==-1)
//						return;
//
//					read(fd,pBtMsgContent,BtMsgSize);
//					
//					close(fd);
//                    
//					strcpy(SaveFileName,"/mnt/flash/notepad/");
//                    strcat(SaveFileName,pBtViewMsgData->pBtMsghandle->szBtMsgViewName);
//
//                    chdir("/mnt/flash/notepad");
//
//					fd = open(SaveFileName, O_WRONLY | O_CREAT | O_EXCL,0);
//
//					if(fd==-1)
//					{
//						PLXTipsWin(pBtViewMsgData->hWndFrame, hWnd, TIPS_RENAMENOTEPAD,IDP_BT_STRING_DEFINEANOTHERNAME, 
//						IDP_BT_TITLE_NOTE,Notify_Alert, 
//						IDP_BT_BUTTON_OK, NULL,20);
//						
//					}
//					else
//                    {
//						write(fd,pBtMsgContent,BtMsgSize);
//						
//						close(fd);
//						
//						free(pBtMsgContent);
//						
//						PLXTipsWin(pBtViewMsgData->hWndFrame, hWnd, 0,IDP_BT_STRING_SAVETONOTEPAD, 
//							IDP_BT_TITLE_NOTE,Notify_Success, 
//							IDP_BT_BUTTON_OK, NULL,20);
//					}
				}
				break;
			case MU_MSG_BT_PICTURE:
				{              
                    MMS_SelectDestination(pBtViewMsgData->hWndFrame, NULL, 0, NULL, 
                        pBtViewMsgData->pBtMsghandle->szBtMsgViewName, pBtViewMsgData->FileContent, 
                        pBtViewMsgData->nContentSize, 3);

//					char SaveFileName[256];                   
//					memset(SaveFileName,0,256);
//					
//					BtMsgSize=BtGetMsgSize(pBtViewMsgData->pBtMsghandle->szBtMsgRealName);
//					pBtMsgContent=(char *)malloc(BtMsgSize);
//					
//					memset(pBtMsgContent,0,BtMsgSize);
//					
//					chdir(BTRECV_FILEPATH);
//					
//					fd = open(pBtViewMsgData->pBtMsghandle->szBtMsgRealName, O_RDONLY,0);
//
//                    if(fd==-1)
//						return;
//
//					read(fd,pBtMsgContent,BtMsgSize);
//					
//					close(fd);
//                    
//					strcpy(SaveFileName,"/mnt/flash/pictures/");
//                    strcat(SaveFileName,pBtViewMsgData->pBtMsghandle->szBtMsgViewName);
//					
//                    chdir("/mnt/flash/pictures");
//					
//					fd = open(SaveFileName, O_WRONLY | O_CREAT | O_EXCL,0);
//                    
//					if(fd==-1)
//					{
//						PLXTipsWin(pBtViewMsgData->hWndFrame, hWnd, TIPS_RENAMEPICTURE,IDP_BT_STRING_DEFINEANOTHERNAME, 
//						pBtViewMsgData->pBtMsghandle->szBtMsgViewName,Notify_Alert, 
//						IDP_BT_BUTTON_OK, NULL,20);
//						
//					}
//					else
//					{
//						write(fd,pBtMsgContent,BtMsgSize);
//						
//						close(fd);
//						
//						free(pBtMsgContent);
//						
//						PLXTipsWin(pBtViewMsgData->hWndFrame, hWnd, 0,IDP_BT_STRING_SAVEPICTURE, 
//							pBtViewMsgData->pBtMsghandle->szBtMsgViewName,Notify_Success, 
//							IDP_BT_BUTTON_OK, NULL,20);
//					}
				}
				break;
			default:
				break;
			}
        }
		break;
	case ID_BTMOVETOFOILDER:
//      MU_FolderSelectionEx()
        MU_FolderSelection(NULL, hWnd, BT_MOVE_FOLDER,
			GetBtFileFolder(pBtViewMsgData->pBtMsghandle->szBtMsgRealName));
		break;
	case ID_BTMSGINFO:
        BtMsgInfoWindow(pBtViewMsgData->hWndFrame, hWnd, pBtViewMsgData->pBtMsghandle);
		break;
	case ID_BTMSGDELETE:
        {
			char BtDeleteMsgTitle[64];
            
            switch(pBtViewMsgData->pBtMsghandle->nBtMsgType)
			{
			case MU_MSG_BT_VCARD:
				strcpy(BtDeleteMsgTitle,IDP_BT_TITLE_BUSINESSCARD);
				break;
			case MU_MSG_BT_VCAL:
                strcpy(BtDeleteMsgTitle,IDP_BT_TITLE_CALENDARENTRY);
				break;
			case MU_MSG_BT_NOTEPAD:
				strcpy(BtDeleteMsgTitle,IDP_BT_TITLE_NOTE);
				break;
			case MU_MSG_BT_PICTURE:
                strcpy(BtDeleteMsgTitle,pBtViewMsgData->pBtMsghandle->szBtMsgViewName);
				break;
			default:
				break;
			}
			
            PLXConfirmWinEx(pBtViewMsgData->hWndFrame,hWnd,IDP_BT_STRING_DELMSGPROMPT, Notify_Request, 
			     BtDeleteMsgTitle, IDP_BT_BUTTON_YES, IDP_BT_BUTTON_NO, CONFIRM_DELETEBTMSG);	
        }
	default:        
		break;
	}
	
}

static HMENU CreateBtViewMsgMenu()
{
	HMENU hBtMsgViewMenu;
	
	hBtMsgViewMenu=CreateMenu();
	
	AppendMenu(hBtMsgViewMenu, MF_STRING, (UINT_PTR)ID_BTSAVEENTRY, IDP_BT_STRING_SAVEPICTURE);
	AppendMenu(hBtMsgViewMenu, MF_STRING, (UINT_PTR)ID_BTMOVETOFOILDER, IDP_BT_STRING_MOVETOFOILDER);	
	AppendMenu(hBtMsgViewMenu, MF_STRING, (UINT_PTR)ID_BTMSGINFO, IDP_BT_STRING_MSGINFO);
	AppendMenu(hBtMsgViewMenu, MF_STRING, (UINT_PTR)ID_BTMSGDELETE, IDP_BT_BUTTON_DELETE); 
	
	return hBtMsgViewMenu;
}

static void BtViewMsgChangeMenu(HWND hWnd)
{
	int nCurBtMsgType;
	HMENU hBtViewMsgMainMenu;
	PBTVIEWMSGDATA pBtViewMsgData = NULL;

    pBtViewMsgData=GetUserData(hWnd);
    
	hBtViewMsgMainMenu=PDAGetMenu(pBtViewMsgData->hWndFrame);
    
	nCurBtMsgType=pBtViewMsgData->pBtMsghandle->nBtMsgType;
    
    switch(nCurBtMsgType)
	{
	case MU_MSG_BT_VCARD:
		ModifyMenu(pBtViewMsgData->hMainMenu,0,MF_BYPOSITION,(UINT_PTR)ID_BTSAVEENTRY, IDP_BT_STRING_SAVEBUSINESSCARD);
		break;
	case MU_MSG_BT_VCAL:
		ModifyMenu(pBtViewMsgData->hMainMenu,0,MF_BYPOSITION,(UINT_PTR)ID_BTSAVEENTRY, IDP_BT_STRING_SAVECALENDARENTRY);
		break;
	case MU_MSG_BT_NOTEPAD:
		ModifyMenu(pBtViewMsgData->hMainMenu,0,MF_BYPOSITION,(UINT_PTR)ID_BTSAVEENTRY, IDP_BT_STRING_SAVENOTE);
		break;
	default:
		break;
	}
		  
}

static BOOL BtDeleteMsg(HWND hWnd, PBTMSGHANDLENAME  pBtMsghandle)
{
	BOOL bRet;

	PBTVIEWMSGDATA pBtViewMsgData = NULL;
    pBtViewMsgData=GetUserData(hWnd);	
	
	if(pBtMsghandle == NULL)
		return FALSE;
	
	chdir(BTRECV_FILEPATH);
	
	bRet=BtDeleteMsgFile(pBtMsghandle->szBtMsgRealName);
	if (!bRet)
	{
		SendMessage(pBtViewMsgData->hWndmu, PWM_MSG_MU_DELETE, MAKEWPARAM(MU_ERR_FAILED, MU_MDU_BT),
			(LPARAM)(DWORD)pBtViewMsgData->pBtMsghandle);
		return FALSE;
	}
	else
	{
		FreeBtMsgHandle(pBtMsghandle);  //释放链表中的相应节点
		
		SendMessage(pBtViewMsgData->hWndmu, PWM_MSG_MU_DELETE, MAKEWPARAM(MU_ERR_SUCC, MU_MDU_BT),
			(LPARAM)(DWORD)pBtViewMsgData->pBtMsghandle);
		return TRUE;
	}	
}

static BOOL BtMoveMsg(PBTMSGHANDLENAME  pBtMsghandle,int nFolder)
{
	FILE *fp;
    BTMSGFILEINFO MsgInfoTemp;
	PBTFOLDERINFO pBtFolderInfo;

	pBtFolderInfo=GetBtFolderByID(pBtMsghandle->nCurFolder);
	
	//这个蓝牙信息原来所在的箱子的数目减少一个
	if(pBtMsghandle->nBtMsgStatus)  //未读   
	{
		pBtFolderInfo->nBtUnreadNum--;
		pBtFolderInfo->nBtTotalNum--;
	}
	else
		pBtFolderInfo->nBtTotalNum--;


	pBtMsghandle->nCurFolder=nFolder;  //蓝牙信息链表的箱子信息更新
	
	fp=fopen(BTMSGINFOFILENAME,"rb+"); //Log文件的信息更新

	if(fp==NULL)
		return FALSE;
	
	while(!feof(fp))
	{
		fread(&MsgInfoTemp,sizeof(BTMSGFILEINFO),1,fp);
        
		if(!strcmp(MsgInfoTemp.szBtMsgRealName,pBtMsghandle->szBtMsgRealName))
            break;
	}
    
	fseek(fp,-4,SEEK_CUR);
	
	fwrite(&nFolder,sizeof(int),1,fp);     //写Log文件
	
	fclose(fp);
	
	pBtFolderInfo=GetBtFolderByID(nFolder);
    
	if(pBtMsghandle->nBtMsgStatus)  //未读
	{
		pBtFolderInfo->nBtUnreadNum++;
		pBtFolderInfo->nBtTotalNum++;
	}
	else
		pBtFolderInfo->nBtTotalNum++;
    
    return TRUE;
}

static LRESULT BtRenameMsgWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) 
{
    LRESULT lRet = TRUE;
    
	PBTRENAMEMSGDATA pBtRenameMsgData = NULL;

	pBtRenameMsgData=GetUserData(hWnd);
	
	switch(message){		
	case WM_CREATE:
		{
			IMEEDIT ie;
			RECT rClient;
            HWND hBtRenameMsgEdit;

			memcpy(pBtRenameMsgData,(PBTRENAMEMSGDATA)(((LPCREATESTRUCT)lParam)->lpCreateParams),
				sizeof(BTRENAMEMSGDATA));

			memset((void*)&ie, 0, sizeof(IMEEDIT));
			
			ie.hwndNotify   = hWnd;
			GetClientRect(hWnd, &rClient);
			
			hBtRenameMsgEdit = CreateWindow(
				"IMEEDIT",
				"",
				WS_VISIBLE | WS_CHILD | ES_AUTOVSCROLL | ES_TITLE,
				rClient.left, rClient.top,
				rClient.right - rClient.left, rClient.bottom - rClient.top - 47*2,
				hWnd,
				NULL,
				NULL,
				&ie);

			pBtRenameMsgData->hBtRenameMsgEdit=hBtRenameMsgEdit;

			SendMessage(hBtRenameMsgEdit, EM_LIMITTEXT, 40, 0);
			
			if(pBtRenameMsgData->pBtMsghandle->nBtMsgType==MU_MSG_BT_PICTURE)
			    SendMessage(hBtRenameMsgEdit, EM_SETTITLE, 0, (LPARAM)IDP_BT_STRING_PICTURENAME);

            if(pBtRenameMsgData->pBtMsghandle->nBtMsgType==MU_MSG_BT_NOTEPAD)
				SendMessage(hBtRenameMsgEdit, EM_SETTITLE, 0, (LPARAM)IDP_BT_STRING_NOTENAME);
			
			SetWindowText(hBtRenameMsgEdit, pBtRenameMsgData->pBtMsghandle->szBtMsgViewName);

			SendMessage(pBtRenameMsgData->hWndFrame,PWM_SETBUTTONTEXT,0,(LPARAM)IDP_BT_BUTTON_CANCEL);
			SendMessage(pBtRenameMsgData->hWndFrame,PWM_SETBUTTONTEXT,1,(LPARAM)IDP_BT_BUTTON_SAVE);
			SendMessage(pBtRenameMsgData->hWndFrame,PWM_SETBUTTONTEXT,2,(LPARAM)"");

			if(pBtRenameMsgData->pBtMsghandle->nBtMsgType==MU_MSG_BT_PICTURE)
				SetWindowText(pBtRenameMsgData->hWndFrame,IDP_BT_STRING_RENAMEPICTURE);
			
			if(pBtRenameMsgData->pBtMsghandle->nBtMsgType==MU_MSG_BT_NOTEPAD)
				SetWindowText(pBtRenameMsgData->hWndFrame,IDP_BT_STRING_RENAMENOTE);

			SetFocus(hBtRenameMsgEdit);
		}
		break;
		
	case WM_SETFOCUS:
		SetFocus(pBtRenameMsgData->hBtRenameMsgEdit);
		break;
		
	case PWM_SHOWWINDOW:
		SendMessage(pBtRenameMsgData->hWndFrame,PWM_SETBUTTONTEXT,0,(LPARAM)IDP_BT_BUTTON_CANCEL);
		SendMessage(pBtRenameMsgData->hWndFrame,PWM_SETBUTTONTEXT,1,(LPARAM)IDP_BT_BUTTON_SAVE);
		SendMessage(pBtRenameMsgData->hWndFrame,PWM_SETBUTTONTEXT,2,(LPARAM)"");
		
		if(pBtRenameMsgData->pBtMsghandle->nBtMsgType==MU_MSG_BT_PICTURE)
		    SetWindowText(pBtRenameMsgData->hWndFrame,IDP_BT_STRING_RENAMEPICTURE);
		
		if(pBtRenameMsgData->pBtMsghandle->nBtMsgType==MU_MSG_BT_NOTEPAD)
            SetWindowText(pBtRenameMsgData->hWndFrame,IDP_BT_STRING_RENAMENOTE);

		SetFocus(pBtRenameMsgData->hBtRenameMsgEdit);
		break;
			  
    case WM_KEYDOWN:
		{
			switch (LOWORD(wParam))
			{				
			case VK_RETURN:
				{
					int fd;
                    
					char *pBtMsgContent;
					UINT BtMsgSize;
					char tmpBuff[30];
                    char SaveAsFileName[256];

					memset(tmpBuff,0,30);
					memset(SaveAsFileName,0,256);

					GetWindowText(pBtRenameMsgData->hBtRenameMsgEdit,tmpBuff,30);

					if(strlen(tmpBuff)==0 || 
						!strcmp(tmpBuff,pBtRenameMsgData->pBtMsghandle->szBtMsgViewName))
					{
						if(pBtRenameMsgData->pBtMsghandle->nBtMsgType==MU_MSG_BT_PICTURE)
						  PLXTipsWin(pBtRenameMsgData->hWndFrame, hWnd, 0,
						    IDP_BT_STRING_PLEASEDEFINENAME, IDP_BT_STRING_RENAMEPICTURE,
						    Notify_Alert, IDP_BT_BUTTON_OK, NULL,20);
						if(pBtRenameMsgData->pBtMsghandle->nBtMsgType==MU_MSG_BT_NOTEPAD)
							PLXTipsWin(pBtRenameMsgData->hWndFrame, hWnd, 0,
							IDP_BT_STRING_PLEASEDEFINENAME, IDP_BT_STRING_RENAMENOTE,
							Notify_Alert, IDP_BT_BUTTON_OK, NULL,20);
					}
					else
					{
						if(pBtRenameMsgData->pBtMsghandle->nBtMsgType==MU_MSG_BT_NOTEPAD)
						{
							chdir(BTRECV_FILEPATH);
							
							BtMsgSize=BtGetMsgSize(pBtRenameMsgData->pBtMsghandle->szBtMsgRealName);
							pBtMsgContent=(char *)malloc(BtMsgSize);
							
							memset(pBtMsgContent,0,BtMsgSize);
							
							fd = open(pBtRenameMsgData->pBtMsghandle->szBtMsgRealName, O_RDONLY,0);
							
							if(fd==-1)
								return FALSE;
							
							read(fd,pBtMsgContent,BtMsgSize);
							
							close(fd);

							strcpy(SaveAsFileName,"/mnt/flash/notepad/");
							strcat(SaveAsFileName,tmpBuff);
							
							chdir("/mnt/flash/notepad");
							
							fd = open(SaveAsFileName, O_WRONLY | O_CREAT | O_EXCL,0);

                            if(fd==-1)
								return FALSE;
							
							write(fd,pBtMsgContent,BtMsgSize);
							
							close(fd);
							
							free(pBtMsgContent);

							PLXTipsWin(pBtRenameMsgData->hWndFrame, hWnd, TIPS_SAVERENAME,IDP_BT_STRING_SAVETONOTEPAD, 
								IDP_BT_TITLE_NOTE,Notify_Success, 
								IDP_BT_BUTTON_OK, NULL,20);
						}

						if(pBtRenameMsgData->pBtMsghandle->nBtMsgType==MU_MSG_BT_PICTURE)
						{
							chdir(BTRECV_FILEPATH);

							BtMsgSize=BtGetMsgSize(pBtRenameMsgData->pBtMsghandle->szBtMsgRealName);
							pBtMsgContent=(char *)malloc(BtMsgSize);
							
							memset(pBtMsgContent,0,BtMsgSize);
							
							fd = open(pBtRenameMsgData->pBtMsghandle->szBtMsgRealName, O_RDONLY,0);
							
							if(fd==-1)
								return FALSE;
							
							read(fd,pBtMsgContent,BtMsgSize);
							
							close(fd);
							
							strcpy(SaveAsFileName,"/mnt/flash/pictures/");
							strcat(SaveAsFileName,tmpBuff);
							
							chdir("/mnt/flash/pictures");
							
							fd = open(SaveAsFileName, O_WRONLY | O_CREAT | O_EXCL,0);
							
                            if(fd==-1)
								return FALSE;
							
							write(fd,pBtMsgContent,BtMsgSize);
							
							close(fd);
							
							free(pBtMsgContent);

							PLXTipsWin(pBtRenameMsgData->hWndFrame, hWnd, TIPS_SAVERENAME,IDP_BT_STRING_SAVEPICTURE, 
								IDP_BT_STRING_PICTURE,Notify_Success, 
								IDP_BT_BUTTON_OK, NULL,20);
						}						
					}
				}
				break;

			case VK_F10:
				PostMessage(hWnd,WM_CLOSE,0,0);
				SendMessage(pBtRenameMsgData->hWndFrame,PWM_CLOSEWINDOW,  (WPARAM) hWnd, (LPARAM)0);
				break;
			default:
				PDADefWindowProc(hWnd, WM_KEYDOWN, wParam, lParam);
				break;
			}
		}
		break;

	case TIPS_SAVERENAME:
		PostMessage(hWnd,WM_CLOSE,0,0);
		SendMessage(pBtRenameMsgData->hWndFrame,PWM_CLOSEWINDOW,  (WPARAM) hWnd, (LPARAM)0);
		break;
			  
	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;
	case WM_DESTROY:
		UnregisterClass("BtRenameMsgWndClass", NULL);
		break;
	default:
		lRet = PDADefWindowProc(hWnd, message, wParam, lParam);
		break;
	}
	
	return lRet;
}

