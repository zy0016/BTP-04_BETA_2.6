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

#include "plx_pdaex.h"
#include "window.h"
#include "pubapp.h"
#include "mullang.h"
#include "PreBrowHead.h"
#include "setup/setting.h"
#include "setup/setup.h"
#include "log.h"
#include "MBPublic.h"
#include "../../trafficlog/logdb.h"
#include "PhonebookExt.h"
#include "MmsUi.h"

#define	COMWNDCLASS	"MBCOMMUNICATIONWNDCLASS"
#define	LOGDURATIONWC	"LOGDURATIONWNDCLASS"
#define	SETOPTIONLISTWC	"SETDURATIONLISTWNDCLASS"
/*
 *	string macro
 */
#define	IDS_SUREDELLOG	ML("Delete\ncommunications log?")
#define	IDS_OK		ML("Ok")
#define	IDS_COM_CAPTION	ML("Communications")
#define	IDS_OPEN	ML("Open")
#define	IDS_SAVE	ML("Save")
#define	IDS_WRITEMSG	ML("Write message")
#define	IDS_FILTER	ML("Filter")
#define	IDS_SETLOG	ML("Set log duration")
#define	IDS_EXIT	ML("Exit")
#define	IDS_DELLOG	ML("Delete log")
#define	IDS_NOCOMM	ML("No communications")
#define	IDS_CANCEL	ML("Cancel")
#define	IDS_BACK	ML("Back")

#define	IDS_DIR_OUT		ML("Out")
#define	IDS_DIR_IN		ML("In")
#define	IDS_DIR_MISSED	ML("Missed")

#define	IDS_TYPE_VOICE	ML("Voice call")
#define	IDS_TYPE_SMS	ML("SMS")
#define	IDS_TYPE_GPRS	ML("GPRS")
#define	IDS_TYPE_DATACALL	ML("Data call")

#define	IDS_LABLE_DIRECTION	ML("Direction:")
#define	IDS_LABLE_TYPE		ML("Type:")
#define	IDS_LABLE_TIME		ML("Time:")
#define	IDS_LABLE_DURATION	ML("Duration:")
#define	IDS_LABLE_STATUS	ML("Status:")
#define	IDS_LABLE_SIZE		ML("Size:")
#define	IDS_LABLE_SENTDATA	ML("Sent data:")
#define	IDS_LABLE_RECEIVEDATA	ML("Received data:")
#define	IDS_LABLE_NUMBER	ML("Number:")

#define	IDS_STATUS_SENT		ML("Sent")
#define	IDS_STATUS_DELIVERY	ML("Delivered")
#define	IDS_STATUS_PEND		ML("Pending")
#define	IDS_STATUS_FAILED	ML("Failed")
#define	IDS_STATUS_UNKNOWN	ML("Unknown")
#define	IDS_NEWCONTACT		ML("New contact")
#define	IDS_ADDTOCONTACT	ML("Add to contact")
#define	IDS_SMS				ML("SMS")
#define	IDS_MMS				ML("MMS")

#define	IDS_ALL				ML("All")
#define	IDS_OUT				ML("Outgoing")
#define	IDS_IN				ML("Incoming")
#define	IDS_CALL			ML("Calls")
#define	IDS_MSG				ML("Messages")
#define	IDS_GPRS			ML("GPRS traffic")
#define	IDS_DATACALL		ML("Data calls")
#define	IDS_SELNUMBER		ML("Selected number")

#define	IDS_LOGDURATION		ML("Log duration")

#define LOG_LEFT_ICON	"/rom/public/arrow_left.ico"
#define LOG_RIGHT_ICON	"/rom/public/arrow_right.ico"

#define RADIO_OFF   "/rom/network/radio_button_off.bmp"
#define RADIO_ON    "/rom/network/radio_button_on.bmp"
#define COM_RECORD_ANSWER  "/rom/mbrecord/mb_in.bmp"
#define COM_RECORD_UNANSWER  "/rom/mbrecord/mb_unan.bmp"
#define COM_RECORD_DIALED  "/rom/mbrecord/mb_out.bmp"
#define COM_RECORD_RSMS  "/rom/mbrecord/receivedsms.bmp"
#define COM_RECORD_SSMS  "/rom/mbrecord/sentsms.bmp"
//#define COM_RECORD_GPRS  "/rom/mbrecord/gprs.bmp"

#define COM_RECORD_GPRS  "/rom/network/not_available_43x28.bmp"

#define COM_RECORD_ANSWERDATA "/rom/network/not_available_43x28.bmp"
#define COM_RECORD_UNANSWERDATA "/rom/network/not_available_43x28.bmp"
#define COM_RECORD_DIALEDDATA "/rom/network/not_available_43x28.bmp"

/*
 *	id
 */
#define	IDW_COMM	(WM_USER+1000)
#define	IDC_LISTBOX	(WM_USER+1001)
#define	IDW_FORMVIEW	(WM_USER+1002)
#define	IDC_EXIT	(WM_USER+1003)
#define IDC_OPEN	(WM_USER+1004)

#define	IDM_OPEN	(WM_USER+1100)
#define	IDM_SAVE	(WM_USER+1101)
#define	IDM_WRITEMSG	(WM_USER+1102)
#define	IDM_FILTER	(WM_USER+1103)
#define	IDM_SETLOG	(WM_USER+1104)
#define	IDM_DELLOG	(WM_USER+1105)

#define	IDM_SMS		(WM_USER+1106)
#define	IDM_MMS		(WM_USER+1107)

#define	IDM_NEWCONTACT	(WM_USER+1108)
#define	IDM_ADDTOCONTACT	(WM_USER+1109)

#define	IDM_ALL			(WM_USER+1110)
#define	IDM_OUTGO		(WM_USER+1111)
#define	IDM_INCOME		(WM_USER+1112)
#define	IDM_CALLS		(WM_USER+1113)
#define	IDM_MSG			(WM_USER+1114)
#define	IDM_GPRS		(WM_USER+1115)
#define	IDM_DATACALL	(WM_USER+1116)
#define	IDM_SELNUMBER	(WM_USER+1117)

#define	CM_REPROCESS	(WM_USER+1004)

#define ID_TIMER		1
/*
 *	variable declared
 */
typedef enum
{
	FILTERALL = 1,
	FILTEROUT,
	FILTERIN,
	FILTERCALL,
	FILTERMSG,
	FILTERGPRS,
	FILTERDATA,
	FILTERSELNUM,
}SIGNREALTIME;
#define MAX_CAPLEN 50

static	BOOL		bLogChange = FALSE;
static	HINSTANCE	hIns;
static	HWND	hFrame = NULL;
static	HWND	hCommunicationWnd = NULL;
static	HWND	hListbox = NULL;
static	HWND	hLastFocus = NULL;
static	HMENU	hMenu = NULL;
static	HMENU	hDataMenu = NULL;
static	HMENU	hMenuEmptyList = NULL;
static	int		reccounter;
static	HWND	hFormviewer = NULL;
static	HWND	hSetLogDuration = NULL;
static	HWND	hLogList = NULL;
static	HWND	hSettingList = NULL;
static	HWND	hSetDurationOption = NULL;
static	HWND	hSetOptionlist = NULL;
static	SIGNREALTIME	tagRealTimeUpdate = FILTERALL;
static char  capdisp[MAX_CAPLEN]="";
/*
 *	bitmap resource
 */
//static	HANDLE	hLeftBmp = NULL;
static	HBITMAP	hReceivedCallBmp	= NULL;
static	HBITMAP	hDialledCallBmp		= NULL;
static	HBITMAP	hMissedCallBmp		= NULL;
static	HBITMAP	hReceivedSmsBmp		= NULL;
static	HBITMAP	hSentSmsBmp			= NULL;
static	HBITMAP	hGPRSBmp			= NULL;
static	HBITMAP	hDialledDataCallBmp	= NULL;
static	HBITMAP	hReceivedDataCallBmp= NULL;
static	HBITMAP	hMissedDataCallBmp	= NULL;

/*
 *	function declared
 */
static	LRESULT	AppWndProc(HWND hwnd, UINT uMsg, WPARAM wparam, LPARAM lparam);
static	BOOL	RegisterCommunication(VOID);
static	void	LOGCOMM_Create(HWND hwnd, LPARAM lparam);
static	void	LOGCOMM_Paint(HWND hwnd);
static	BOOL	OpenItem(HWND hwnd, unsigned short nIndex, char * dsp, int dsplen);
static	void	InitLogView();
static	BOOL	RegisterLogDuraton(VOID);
static	void	CreateSetLogDuration();
static	LRESULT	SetLogDurationWndProc(HWND hwnd, UINT uMsg, WPARAM wparam, LPARAM lparam);
static	LRESULT	ProcessWMCommand(HWND hwnd, WPARAM wparam, LPARAM lparam);
static	BOOL	UpdateListFromDir(DRTTYPE dir);

static	BOOL	RegisterSetOptionList();
static	void	CreateSetOptionList();
static	LRESULT	SetOptionListWndProc(HWND hwnd, UINT uMsg, WPARAM wparam, LPARAM lparam);
extern BOOL CreateLogCounters(HWND hParent, HINSTANCE hinst);
extern int  APP_EditSMS(HWND hWnd, const char* pszReciever, const char* pszContent);
extern void ByteToSzKiloByte(unsigned long src, char * pdst);
//extern BOOL APP_GetInfoByPhone(const char* pPhone, ABINFO* pInfo);
void	RealTimeUpdateCommunication();
void	LoadLogInfos(HWND hwnd, LPARAM lparam);
/*
 *	menu
 */
static MENUITEMTEMPLATE SaveMenu[] =
{
	{ MF_STRING, IDM_NEWCONTACT, "New Contact", NULL},
	{ MF_STRING, IDM_ADDTOCONTACT, "Add to contact", NULL},
	{ MF_END, 0, NULL, NULL }
};

static MENUITEMTEMPLATE WriteMenu[] =
{
	{ MF_STRING, IDM_SMS, "SMS", NULL},
	{ MF_STRING, IDM_MMS, "MMS", NULL},
	{ MF_END, 0, NULL, NULL }
};

static MENUITEMTEMPLATE FilterMenu[] =
{
	{ MF_STRING, IDM_ALL, "All", NULL},
	{ MF_STRING, IDM_OUTGO, "Outgoing", NULL},
	{ MF_STRING, IDM_INCOME, "Incoming", NULL},
	{ MF_STRING, IDM_CALLS, "Calls", NULL},
	{ MF_STRING, IDM_MSG, "Message", NULL},
	{ MF_STRING, IDM_GPRS, "GPRS traffic", NULL},
	{ MF_STRING, IDM_DATACALL, "Data calls", NULL},
	{ MF_STRING, IDM_SELNUMBER, "Selected number", NULL},
	{ MF_END, 0, NULL, NULL }
};

static MENUITEMTEMPLATE CommMenu[] =
{
	{ MF_STRING, IDM_OPEN, "Open", NULL},
	{ MF_STRING|MF_POPUP, 0, "Save...", SaveMenu },
	{ MF_STRING|MF_POPUP, 0, "Write message...", WriteMenu},
	{ MF_STRING|MF_POPUP, 0, "Filter", FilterMenu},
	{ MF_STRING, IDM_SETLOG, "Set log duration", NULL},
	{ MF_STRING, IDM_DELLOG, "Delete log", NULL},
    { MF_END, 0, NULL, NULL }
};

static MENUITEMTEMPLATE ComDataMenu[] =
{
	{ MF_STRING, IDM_OPEN, "Open", NULL},
	{ MF_STRING|MF_POPUP, 0, "Filter", FilterMenu},
	{ MF_STRING, IDM_SETLOG, "Set log duration", NULL},
	{ MF_STRING, IDM_DELLOG, "Delete log", NULL},
    { MF_END, 0, NULL, NULL }
};
//#endif
static const MENUTEMPLATE CommDataMenuTemplate =
{
    0,
    ComDataMenu
};

static const MENUTEMPLATE CommMenuTemplate =
{
    0,
    CommMenu
};

static MENUITEMTEMPLATE EmptyCommMenu[] =
{
	{ MF_STRING, IDM_SETLOG, "Set log duration", NULL},
    { MF_END, 0, NULL, NULL }
};
static const MENUTEMPLATE EmptyCommMenuTemplate =
{
    0,
    EmptyCommMenu
};
/*
 *	register window class COMWNDCLASS
 */
static	BOOL	RegisterCommunication(VOID)
{
	WNDCLASS	wc;
	wc.style         = 0;
	wc.lpfnWndProc   = AppWndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = NULL;
	wc.hIcon         = NULL;
	wc.hCursor       = NULL;
	wc.hbrBackground = NULL;	//(HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = COMWNDCLASS;	
	if( !RegisterClass(&wc) )
		return FALSE;
	return	TRUE;
}

/*
 *	Interface to call Communication window
 */
BOOL CallMBCommunicationWnd(HWND hParent, HINSTANCE hInstance)
{
	RECT	rcClient;
	int		i;
	LOGRECORD * pRecord = NULL;
		int len;
	int	nIndex,rcIndex;
	HMENU	hSaveMenu,hWriteMsgMenu,hFilterMenu, hDataFilterMenu;
	hFrame = hParent;
	hIns = hInstance;
	GetClientRect(hParent, &rcClient);
	RegisterCommunication();
	hMenu = LoadMenuIndirect(&CommMenuTemplate);
	hMenuEmptyList = LoadMenuIndirect(&EmptyCommMenuTemplate);
	hDataMenu = LoadMenuIndirect(&CommDataMenuTemplate);
	hCommunicationWnd = CreateWindow(COMWNDCLASS,IDS_COM_CAPTION,
		WS_VISIBLE |WS_CHILD,
		rcClient.left,
		rcClient.top,
		rcClient.right - rcClient.left,
		rcClient.bottom - rcClient.top,
		hParent,
		(HMENU)IDW_COMM,NULL,NULL);
	if (!hCommunicationWnd) 
	{
		UnregisterClass(COMWNDCLASS, NULL);
		return FALSE;
	}
	
	hSaveMenu = GetSubMenu(hMenu,1);
	hWriteMsgMenu = GetSubMenu(hMenu, 2);
	hFilterMenu = GetSubMenu(hMenu, 3);
	hDataFilterMenu = GetSubMenu(hDataMenu,1);
	ModifyMenu(hMenu, IDM_OPEN, MF_BYCOMMAND, IDM_OPEN, IDS_OPEN);
	ModifyMenu(hMenu, 1, MF_BYPOSITION|MF_POPUP, (UINT_PTR)hSaveMenu, IDS_SAVE);
	ModifyMenu(hMenu, 2, MF_BYPOSITION|MF_POPUP, (UINT_PTR)hWriteMsgMenu, IDS_WRITEMSG);
	ModifyMenu(hMenu, 3, MF_BYPOSITION|MF_POPUP, (UINT_PTR)hFilterMenu, IDS_FILTER);
	ModifyMenu(hMenu, IDM_SETLOG, MF_BYCOMMAND, IDM_SETLOG, IDS_SETLOG);
	ModifyMenu(hMenu, IDM_DELLOG, MF_BYCOMMAND, IDM_DELLOG, IDS_DELLOG);

	ModifyMenu(hDataMenu, IDM_OPEN, MF_BYCOMMAND, IDM_OPEN, IDS_OPEN);
	ModifyMenu(hDataMenu, 1, MF_BYPOSITION|MF_POPUP, (UINT_PTR)hFilterMenu, IDS_FILTER);
	ModifyMenu(hDataMenu, IDM_SETLOG, MF_BYCOMMAND, IDM_SETLOG, IDS_SETLOG);
	ModifyMenu(hDataMenu, IDM_DELLOG, MF_BYCOMMAND, IDM_DELLOG, IDS_DELLOG);

	//hSaveMenu = GetSubMenu(hMenu,1);
	ModifyMenu(hSaveMenu, IDM_NEWCONTACT, MF_BYCOMMAND, IDM_NEWCONTACT, IDS_NEWCONTACT);
	ModifyMenu(hSaveMenu, IDM_ADDTOCONTACT, MF_BYCOMMAND, IDM_ADDTOCONTACT, IDS_ADDTOCONTACT);

	//hWriteMsgMenu = GetSubMenu(hMenu, 2);
	ModifyMenu(hWriteMsgMenu, IDM_SMS, MF_BYCOMMAND, IDM_SMS, IDS_SMS);
	ModifyMenu(hWriteMsgMenu, IDM_MMS, MF_BYCOMMAND, IDM_MMS, IDS_MMS);

	//hFilterMenu = GetSubMenu(hMenu, 3);
	ModifyMenu(hFilterMenu, IDM_ALL, MF_BYCOMMAND, IDM_ALL, IDS_ALL);
	ModifyMenu(hFilterMenu, IDM_OUTGO, MF_BYCOMMAND, IDM_OUTGO, IDS_OUT);
	ModifyMenu(hFilterMenu, IDM_INCOME, MF_BYCOMMAND, IDM_INCOME, IDS_IN);
	ModifyMenu(hFilterMenu, IDM_CALLS, MF_BYCOMMAND, IDM_CALLS, IDS_CALL);
	ModifyMenu(hFilterMenu, IDM_MSG, MF_BYCOMMAND, IDM_MSG, IDS_MSG);
	ModifyMenu(hFilterMenu, IDM_GPRS, MF_BYCOMMAND, IDM_GPRS, IDS_GPRS);
	ModifyMenu(hFilterMenu, IDM_DATACALL, MF_BYCOMMAND, IDM_DATACALL, IDS_DATACALL);
	ModifyMenu(hFilterMenu, IDM_SELNUMBER, MF_BYCOMMAND, IDM_SELNUMBER, IDS_SELNUMBER);


	ModifyMenu(hDataFilterMenu, IDM_ALL, MF_BYCOMMAND, IDM_ALL, IDS_ALL);
	ModifyMenu(hDataFilterMenu, IDM_OUTGO, MF_BYCOMMAND, IDM_OUTGO, IDS_OUT);
	ModifyMenu(hDataFilterMenu, IDM_INCOME, MF_BYCOMMAND, IDM_INCOME, IDS_IN);
	ModifyMenu(hDataFilterMenu, IDM_CALLS, MF_BYCOMMAND, IDM_CALLS, IDS_CALL);
	ModifyMenu(hDataFilterMenu, IDM_MSG, MF_BYCOMMAND, IDM_MSG, IDS_MSG);
	ModifyMenu(hDataFilterMenu, IDM_GPRS, MF_BYCOMMAND, IDM_GPRS, IDS_GPRS);
	ModifyMenu(hDataFilterMenu, IDM_DATACALL, MF_BYCOMMAND, IDM_DATACALL, IDS_DATACALL);
	ModifyMenu(hDataFilterMenu, IDM_SELNUMBER, MF_BYCOMMAND, IDM_SELNUMBER, IDS_SELNUMBER);

	ModifyMenu(hMenuEmptyList, IDM_SETLOG, MF_BYCOMMAND, IDM_SETLOG, IDS_SETLOG);
	
	i = SendMessage(hListbox, LB_GETCOUNT, 0, 0);
	if (i>0) 
	{
		nIndex = SendMessage(hListbox, LB_GETCURSEL, 0, 0);
		rcIndex = SendMessage(hListbox, LB_GETITEMDATA, nIndex, 0);
		len = Log_Read(NULL, (WORD)rcIndex, 0);
		pRecord = malloc(len);
		if (NULL == pRecord)
			return FALSE;
		if (LOG_DB_SUCCESS == Log_Read(pRecord, (WORD)rcIndex, (unsigned short)len))
		{
			if(pRecord->type == TYPE_GPRS)
				PDASetMenu(hFrame, hDataMenu);
			else
				PDASetMenu(hFrame, hMenu);
			
		}	
		free(pRecord);
		pRecord = NULL;
		//PDASetMenu(hParent, hMenu);
		SendMessage(hParent, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_OPEN);
	}
	else
	{
		SendMessage(hParent, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
		PDASetMenu(hParent, hMenuEmptyList);
	}
	SendMessage(hParent, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_EXIT);
	SendMessage(hParent, PWM_SETBUTTONTEXT, 2, (LPARAM)"Options");

	ShowWindow(hParent,SW_SHOW);
//	UpdateWindow(hParent);
	SetFocus(hCommunicationWnd);
	return TRUE;
}

/*
 *	window process function
 */
static	LRESULT	AppWndProc(HWND hwnd, UINT uMsg, WPARAM wparam, LPARAM lparam)
{
	 SIZE		rsize;
	 LRESULT	lResult;
     int		i;


	switch(uMsg) 
	{
	case WM_CREATE:
		bLogChange = FALSE;

		memset(&rsize, 0, sizeof(SIZE));
		GetImageDimensionFromFile(COM_RECORD_ANSWER, &rsize);
		if (hReceivedCallBmp == NULL)
			hReceivedCallBmp = LoadImage(NULL, COM_RECORD_ANSWER, IMAGE_BITMAP, rsize.cx, rsize.cy, LR_LOADFROMFILE);
		GetImageDimensionFromFile(COM_RECORD_UNANSWERDATA, &rsize);
		if (hMissedCallBmp == NULL)
			hMissedCallBmp	= LoadImage(NULL, COM_RECORD_UNANSWER, IMAGE_BITMAP, rsize.cx, rsize.cy, LR_LOADFROMFILE);
		GetImageDimensionFromFile(COM_RECORD_DIALED, &rsize);
		if (hDialledCallBmp == NULL)
			hDialledCallBmp = LoadImage(NULL, COM_RECORD_DIALED, IMAGE_BITMAP, rsize.cx, rsize.cy, LR_LOADFROMFILE);
		GetImageDimensionFromFile(COM_RECORD_RSMS, &rsize);
		if (hReceivedSmsBmp == NULL)
			hReceivedSmsBmp = LoadImage(NULL, COM_RECORD_RSMS, IMAGE_BITMAP, rsize.cx, rsize.cy, LR_LOADFROMFILE);
		GetImageDimensionFromFile(COM_RECORD_SSMS, &rsize);
		if (hSentSmsBmp == NULL)
			hSentSmsBmp	= LoadImage(NULL, COM_RECORD_SSMS, IMAGE_BITMAP, rsize.cx, rsize.cy, LR_LOADFROMFILE);
		GetImageDimensionFromFile(COM_RECORD_GPRS, &rsize);
		if (hGPRSBmp == NULL)
			hGPRSBmp = LoadImage(NULL, COM_RECORD_GPRS, IMAGE_BITMAP, rsize.cx, rsize.cy, LR_LOADFROMFILE);
		GetImageDimensionFromFile(COM_RECORD_DIALEDDATA, &rsize);
		if (hDialledDataCallBmp == NULL)
			hDialledDataCallBmp = LoadImage(NULL, COM_RECORD_DIALEDDATA, IMAGE_BITMAP, rsize.cx, rsize.cy, LR_LOADFROMFILE);
		GetImageDimensionFromFile(COM_RECORD_ANSWERDATA, &rsize);
		if (hReceivedDataCallBmp == NULL)
			hReceivedDataCallBmp = LoadImage(NULL, COM_RECORD_ANSWERDATA, IMAGE_BITMAP, rsize.cx, rsize.cy, LR_LOADFROMFILE);
		GetImageDimensionFromFile(COM_RECORD_UNANSWERDATA, &rsize);
		if (hMissedDataCallBmp == NULL)
			hMissedDataCallBmp = LoadImage(NULL, COM_RECORD_UNANSWERDATA, IMAGE_BITMAP, rsize.cx, rsize.cy, LR_LOADFROMFILE);
		
		LOGCOMM_Create(hwnd, lparam);
		InitLogView();

		SendMessage(hFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, LEFTICON), (LPARAM)LOG_LEFT_ICON);
		SendMessage(hFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, RIGHTICON), (LPARAM)"");

		SetWindowText(hFrame,IDS_COM_CAPTION);

		SendMessage(hFrame, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_OPEN, 1), (LPARAM)ML("Open") );
		SendMessage(hFrame,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDC_EXIT, 0),(LPARAM)IDS_EXIT);

		i = SendMessage(hListbox, LB_GETCOUNT, 0, 0);
		if (i <= 0) 
		{
			SendMessage(hFrame,PWM_SETBUTTONTEXT,1,(LPARAM)"");
			PDASetMenu(hFrame, hMenuEmptyList);
			ShowWindow(hListbox,SW_HIDE);
		}
		else
		{
			SendMessage(hFrame,PWM_SETBUTTONTEXT,1,(LPARAM)IDS_OPEN);
			PDASetMenu(hFrame, hMenu);
			ShowWindow(hListbox,SW_SHOW);
			SendMessage(hListbox, LB_SETCURSEL, 0,0);
			SetFocus(hListbox);
		}
		break;

	case WM_PAINT:
		LOGCOMM_Paint(hwnd);
		break;

	case WM_SETFOCUS:
		if (IsWindow(hFormviewer)) 
		{
			SetFocus(hFormviewer);
		}
		else
		{
			i = SendMessage(hListbox, LB_GETCOUNT, 0, 0);
			if (i >0) 
				SetFocus(hListbox);
		}
		break;

	case PWM_SHOWWINDOW:	// wrong action ! need create the listbox after set log
		{
			LOGRECORD * pRecord = NULL;
			int	nIndex,rcIndex;
			int len;
			
			if (IsWindow(hFormviewer))	//if the fromviewer exist
			{
				PDASetMenu(hFrame, NULL);
				SendMessage(hFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
				SendMessage(hFrame, PWM_SETBUTTONTEXT, 0, (LPARAM)"");
				SendMessage(hFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_OK);
				SendMessage(hFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, LEFTICON), (LPARAM)"");
				SendMessage(hFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, RIGHTICON), (LPARAM)"");
				SetWindowText(hFrame, capdisp);
			}
			else
			{
				if ( bLogChange )
				{
					LoadLogInfos(hwnd, lparam);

					i = SendMessage(hListbox, LB_GETCOUNT, 0, 0);
					nIndex = SendMessage(hListbox, LB_GETCURSEL, 0, 0);
					rcIndex = SendMessage(hListbox, LB_GETITEMDATA, nIndex, 0);
					
					if (i <= 0) 
					{
						SendMessage(hFrame,PWM_SETBUTTONTEXT,1,(LPARAM)"");
						PDASetMenu(hFrame, hMenuEmptyList);
						ShowWindow(hListbox,SW_HIDE);
						SetFocus(hwnd);
					}
					else
					{
						SendMessage(hFrame,PWM_SETBUTTONTEXT,1,(LPARAM)IDS_OPEN);
						ShowWindow(hListbox,SW_SHOW);
						SetFocus(hListbox);
					}
				}
				else
				{
					i = SendMessage(hListbox, LB_GETCOUNT, 0, 0);
					nIndex = SendMessage(hListbox, LB_GETCURSEL, 0, 0);
					rcIndex = SendMessage(hListbox, LB_GETITEMDATA, nIndex, 0);
					if (i <= 0) 
					{
						SendMessage(hFrame,PWM_SETBUTTONTEXT,1,(LPARAM)"");
						PDASetMenu(hFrame, hMenuEmptyList);
						ShowWindow(hListbox,SW_HIDE);
						//SetFocus(hwnd);
					}
					else
					{
						len = Log_Read(NULL, (WORD)rcIndex, 0);
						pRecord = malloc(len);
						if (NULL == pRecord)
							return FALSE;
						if (LOG_DB_SUCCESS == Log_Read(pRecord, (WORD)rcIndex, (unsigned short)len))
						{
							if(pRecord->type == TYPE_GPRS)
								PDASetMenu(hFrame, hDataMenu);
							else
								PDASetMenu(hFrame, hMenu);
						}	
						free(pRecord);
						pRecord = NULL;
						
						SendMessage(hFrame,PWM_SETBUTTONTEXT,1,(LPARAM)IDS_OPEN);
						
						ShowWindow(hListbox,SW_SHOW);
						//SetFocus(hListbox);
					}
					SendMessage(hFrame, PWM_SETBUTTONTEXT, 0,(LPARAM)IDS_EXIT );
					SendMessage(hFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)"Options");		
					SendMessage(hFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, LEFTICON), (LPARAM)LOG_LEFT_ICON);
					SendMessage(hFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, RIGHTICON), (LPARAM)"");
					SetWindowText(hFrame,IDS_COM_CAPTION);
				}
				SetFocus(hwnd); 
			}
//			SetFocus(hwnd); 
		}
		break;

	case WM_COMMAND:
		ProcessWMCommand(hwnd, wparam, lparam);
		break;

	case CM_REPROCESS:
		if (lparam == 1) 
		{
			Log_Empty();
			SendMessage(hListbox, LB_RESETCONTENT, 0, 0);
			PLXTipsWin(hFrame, hwnd, 0, ML("Deleted"), NULL, Notify_Success, ML("Ok"),NULL, WAITTIMEOUT);
		}
		break;

	case WM_KEYDOWN:
		switch(LOWORD(wparam)) 
		{
		case VK_LEFT:
			if( !IsWindow(hFormviewer) )
			{
				PostMessage(hwnd,WM_CLOSE,0,0);
				CreateLogCounters(hFrame, hIns);
			}
			break;

		case VK_RETURN:
			{
				if (hFormviewer == GetFocus()) 
				{
					DestroyWindow(hFormviewer);
					hFormviewer = NULL;
					SendMessage(hwnd,PWM_SHOWWINDOW,0,0);
					SetFocus(hListbox);
				}
				else
				{
					i = SendMessage(hListbox, LB_GETCOUNT, 0, 0);
					if (i >= 0) 
					{
						SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(IDM_OPEN, 0), 0);
					}
				}
			}		
			break;

		case VK_F10:
			if (hFormviewer == GetFocus()) 
				break;
			else
			{	
				DlmNotify ((WPARAM)PES_STCQUIT, (LPARAM)hIns);
				SendMessage(hFrame,PWM_CLOSEWINDOW,  (WPARAM)hwnd, 0);
				DestroyWindow(hwnd);
			}
			break;

		case VK_F1:
			{
				int i,nIndex,rcIndex,len;	
				LOGRECORD * pRecord = NULL;

				i = SendMessage(hListbox, LB_GETCOUNT, 0, 0);
				nIndex = SendMessage(hListbox, LB_GETCURSEL, 0, 0);
				rcIndex = SendMessage(hListbox, LB_GETITEMDATA, nIndex, 0);

				len = Log_Read(NULL, (WORD)rcIndex, 0);
				pRecord = malloc(len);
				if (NULL == pRecord)
					return FALSE;

				if (LOG_DB_SUCCESS == Log_Read(pRecord, (WORD)rcIndex, (unsigned short)len))
				{
					APP_CallPhoneNumber(pRecord->APN);
				}	
				else
				{
					return FALSE;
				}
				free(pRecord);
				pRecord = NULL;
			}
			break;

		case VK_F5:
			SendMessage(hFrame, WM_KEYDOWN, wparam, lparam);
			break;

		default:
			lResult = PDADefWindowProc(hwnd, uMsg, wparam, lparam);
			break;
		}
		break;

	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;

	case WM_DESTROY:
		DestroyMenu(hMenu);
		DestroyMenu(hDataMenu);
		DestroyMenu(hMenuEmptyList);

		if (hReceivedCallBmp )
		{
			DeleteObject(hReceivedCallBmp);
			hReceivedCallBmp = NULL;
		}
		if (hMissedCallBmp)
		{
			DeleteObject(hMissedCallBmp);
			hMissedCallBmp = NULL;
		}
		if (hDialledCallBmp)
		{
			DeleteObject(hDialledCallBmp);
			hDialledCallBmp = NULL;
		}
		if (hReceivedSmsBmp)
		{
			DeleteObject(hReceivedSmsBmp);
			hReceivedSmsBmp = NULL;
		}
		if (hSentSmsBmp)
		{
			DeleteObject(hSentSmsBmp);
			hSentSmsBmp = NULL;
		}
		if (hGPRSBmp)
		{
			DeleteObject(hGPRSBmp);
			hGPRSBmp = NULL;
		}
		if (hDialledDataCallBmp)
		{
			DeleteObject(hDialledDataCallBmp);
			hDialledDataCallBmp = NULL;
		}
		if (hReceivedDataCallBmp)
		{
			DeleteObject(hReceivedDataCallBmp);
			hReceivedDataCallBmp = NULL;
		}
		if (hMissedDataCallBmp)
		{
			DeleteObject(hMissedDataCallBmp);
			hMissedDataCallBmp = NULL;
		}
		UnregisterClass(COMWNDCLASS,NULL);
//		DestroyWindow(hFrame);
//		hFrame = NULL;	///
//		DlmNotify ((WPARAM)PES_STCQUIT, (LPARAM)hIns);//\\if shortcut we will send msg 
		break;
			
	default:
		lResult = PDADefWindowProc(hwnd, uMsg, wparam, lparam);
		break;
	}
	return	lResult;
}

static void LOGCOMM_Create(HWND hwnd, LPARAM lparam)
{
	RECT	rClient;

	GetClientRect(hwnd, &rClient);
	if ( hListbox != NULL )
	{
		DestroyWindow( hListbox );
		hListbox = NULL;
	}

	hListbox = CreateWindow("LISTBOX", "", 
		WS_VISIBLE | WS_CHILD | WS_TABSTOP | LBS_BITMAP|WS_VSCROLL, 
		rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
		hwnd, (HMENU)IDC_LISTBOX, NULL, NULL);
	if (hListbox == NULL)
		return ;
	
	ShowWindow(hListbox, SW_SHOW);

}

static	void InitLogView()
{
	int	index;
	LOGRECORD * pRecBuf = NULL; 
	LOGRECORD * pRecord = NULL; 
	unsigned short  recindex = 0;
	int len;
//	int	nIndex,rcIndex;
	char *recstr;

	reccounter = Log_Expired();
	if (reccounter > 0) 
	{		
		recindex = reccounter -1;
		SendMessage(hListbox, LB_RESETCONTENT, 0, 0);
		
		while (recindex >= 0) 
		{
			HBITMAP	hIcon;
			len = Log_Read(NULL, recindex, 0);
			pRecBuf = malloc(len);
			if (NULL == pRecBuf)
				return;
			if (LOG_DB_SUCCESS == Log_Read(pRecBuf, recindex, (unsigned short)len)) 
			{
				recstr = malloc(strlen(pRecBuf->APN)+1);
				if (NULL == recstr)
					return;
				strcpy(recstr, pRecBuf->APN);
				switch(pRecBuf->type) 
				{
				case TYPE_VOICE:
					
					switch(pRecBuf->direction) 
					{
					case DRT_SEND:
						hIcon = hDialledCallBmp;
						break;
						
					case DRT_RECIEVE:
						hIcon = hReceivedCallBmp;
						break;
						
					case DRT_UNRECIEVE:
						hIcon = hMissedCallBmp;
						break;						
					}
					
					index = SendMessage(hListbox, LB_ADDSTRING, 0, (LPARAM)recstr);
					SendMessage(hListbox, LB_SETITEMDATA, index, (LPARAM)recindex);
					SendMessage(hListbox, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP,index), (LPARAM)hIcon);
					//sprintf(temp,"%s:%s", ML("VOICE"), recstr);
					break;

				case TYPE_DATA:
					switch(pRecBuf->direction) 
					{
					case DRT_SEND:
						hIcon = hDialledDataCallBmp;
						break;
						
					case DRT_RECIEVE:
						hIcon = hReceivedDataCallBmp;
						break;
						
					case DRT_UNRECIEVE:
						hIcon = hMissedDataCallBmp;
						break;						
					}
					
					index = SendMessage(hListbox, LB_ADDSTRING, 0, (LPARAM)recstr);
					SendMessage(hListbox, LB_SETITEMDATA, index, (LPARAM)recindex);
					SendMessage(hListbox, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP,index), (LPARAM)hIcon);
					//sprintf(temp,"%s:%s", ML("DATA"), recstr);
					break;

				case TYPE_GPRS:					
					index = SendMessage(hListbox, LB_ADDSTRING, 0, (LPARAM)recstr);
					SendMessage(hListbox, LB_SETITEMDATA, index, (LPARAM)recindex);
					SendMessage(hListbox, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP,index), (LPARAM)hGPRSBmp);
					//sprintf(temp,"%s:%s", ML("GPRS"), recstr);
					break;

				case TYPE_SMS:
					switch(pRecBuf->direction) 
					{
					case DRT_SEND:
						hIcon = hSentSmsBmp;
						break;
						
					case DRT_RECIEVE:
						hIcon = hReceivedSmsBmp;
						break;										
					}
					
					index = SendMessage(hListbox, LB_ADDSTRING, 0, (LPARAM)recstr);
					SendMessage(hListbox, LB_SETITEMDATA, index, (LPARAM)recindex);
					SendMessage(hListbox, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP,index), (LPARAM)hIcon);
					break;
					
				default:
					free(pRecBuf);
					if (recstr) 
					{
						free(recstr);
						recstr = NULL;
					}
					return ;
				}
				if (recstr)
				{
					free(recstr);
					recstr = NULL;
				}
				hIcon = NULL;
				
			}
			free(pRecBuf);
			pRecBuf =NULL;
			if (recindex == 0) 
				break;
			else
				recindex--;
			
		}
		SendMessage(hListbox, LB_SETCURSEL, 0, 0);	
	}
	else if ( reccounter == -1) 
	{
//		DestroyWindow(hCommunicationWnd);
//		SendMessage(hFrame, PWM_CLOSEWINDOW, 0,0);
			Log_Empty();
			SendMessage(hListbox, LB_RESETCONTENT, 0, 0);
			ShowWindow(hListbox,SW_HIDE);
//			SetFocus(hwnd);
	}

}

static	void	LOGCOMM_Paint(HWND hwnd)
{
	HDC	hdc;
	RECT	rClient;
	int oldbk;
	GetClientRect(hwnd,&rClient);
	hdc = BeginPaint(hwnd, NULL);
	oldbk = SetBkMode(hdc, BM_TRANSPARENT);
	if (SendMessage(hListbox, LB_GETCOUNT, 0, 0) > 0) 
	{
		ShowWindow(hListbox, SW_SHOW);
	}
	else
	{
		ShowWindow(hListbox,SW_HIDE);
	}
	DrawText(hdc, IDS_NOCOMM, -1, &rClient, DT_CENTER|DT_VCENTER);
	SetBkMode(hdc, oldbk);
	EndPaint(hwnd,NULL);

}


static	LRESULT	ProcessWMCommand(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	LRESULT	lResult;
	int	nIndex,len;//recindex;
	int		indexInDB;

	lResult = TRUE;
	switch(LOWORD(wparam)) 
	{
	case  IDC_LISTBOX:
		switch(HIWORD(wparam))
		{
		case LBN_SELCHANGE:
			{
				LOGRECORD * pRecord = NULL;
				int	len,nIndex,rcIndex;
				
//				i = SendMessage(hListbox, LB_GETCOUNT, 0, 0);
				nIndex = SendMessage(hListbox, LB_GETCURSEL, 0, 0);
				rcIndex = SendMessage(hListbox, LB_GETITEMDATA, nIndex, 0);
				len = Log_Read(NULL, (WORD)rcIndex, 0);
				pRecord = malloc(len);
				if (NULL == pRecord)
					return FALSE;
				if (LOG_DB_SUCCESS == Log_Read(pRecord, (WORD)rcIndex, (unsigned short)len))
				{
					if(pRecord->type == TYPE_GPRS)
						PDASetMenu(hFrame, hDataMenu);
					else
						PDASetMenu(hFrame, hMenu);
					
				}	
				free(pRecord);
				pRecord = NULL;
			}
			break;
		}
		break;
	case IDM_OPEN:
		{	
			char * info;
			int infolen;
			nIndex = SendMessage(hListbox, LB_GETCURSEL, 0, 0);
			infolen = SendMessage(hListbox, LB_GETTEXTLEN, nIndex, 0) + 1;
			info = malloc(102+1);
			if (info == NULL) 
				break;
			SendMessage(hListbox, LB_GETTEXT, nIndex, (LPARAM)info);
			indexInDB = SendMessage(hListbox, LB_GETITEMDATA, nIndex, 0);
			OpenItem(hwnd, (unsigned short)indexInDB, info, infolen);
			
			free(info);
			info = NULL;
		
		}
		break;
		
	case IDM_NEWCONTACT:
		{
			LOGRECORD * pRecord = NULL;
			ABNAMEOREMAIL abnameinfo;
			int	rcIndex;
			
			memset(&abnameinfo, 0, sizeof(ABNAMEOREMAIL));
			nIndex = SendMessage(hListbox,LB_GETCURSEL,0,0);
			rcIndex= SendMessage(hListbox,LB_GETITEMDATA,nIndex,0);
			len = Log_Read(NULL, (WORD)rcIndex, 0);
			pRecord = malloc(len);
			if (NULL == pRecord)
				return FALSE;
			if (LOG_DB_SUCCESS == Log_Read(pRecord, (WORD)rcIndex, (unsigned short)len))
				strcpy(abnameinfo.szTelOrEmail, pRecord->APN);
			else
			{
				free(pRecord);
				break;
			}
			APP_SaveToAddressBook(hFrame, NULL, 0, &abnameinfo, AB_NEW);
			free(pRecord);
				
		}
		break;
		
	case IDM_ADDTOCONTACT:
		{
			LOGRECORD * pRecord = NULL;
			ABNAMEOREMAIL abnameinfo;
			int	rcIndex;
			
			memset(&abnameinfo, 0, sizeof(ABNAMEOREMAIL));
			nIndex = SendMessage(hListbox,LB_GETCURSEL,0,0);
			rcIndex= SendMessage(hListbox,LB_GETITEMDATA,nIndex,0);
			len = Log_Read(NULL, (WORD)rcIndex, 0);
			pRecord = malloc(len);
			if (NULL == pRecord)
				return FALSE;
			if (LOG_DB_SUCCESS == Log_Read(pRecord, (WORD)rcIndex, (unsigned short)len))
				strcpy(abnameinfo.szTelOrEmail, pRecord->APN);
			else
			{
				free(pRecord);
				break;
			}
			APP_SaveToAddressBook(hFrame, NULL, 0, &abnameinfo, AB_UPDATE);	
			free(pRecord);
				
		}
		break;
		
	case IDM_DELLOG:
		PLXConfirmWinEx(hFrame, hwnd, IDS_SUREDELLOG, Notify_Request, NULL, ML("Yes"), ML("No"), CM_REPROCESS);
		break;
		
	case IDM_ALL:
		tagRealTimeUpdate = FILTERALL;
		RealTimeUpdateCommunication();
//		InitLogView();
//		SendMessage(hwnd, PWM_SHOWWINDOW, 0, 0);
		break;
		
	case IDM_OUTGO:
		tagRealTimeUpdate = FILTEROUT;
		RealTimeUpdateCommunication();
//		{
//			UpdateListFromDir(DRT_SEND);
//			SendMessage(hwnd, PWM_SHOWWINDOW, 0, 0);
//		}
		break;
		
	case IDM_INCOME:
		tagRealTimeUpdate = FILTERIN;
		RealTimeUpdateCommunication();
//		{
//			UpdateListFromDir(DRT_RECIEVE);
//			SendMessage(hwnd, PWM_SHOWWINDOW, 0, 0);
//		}
		break;
		
	case IDM_MSG:
		tagRealTimeUpdate = FILTERMSG;
		RealTimeUpdateCommunication();
		/*
		{
			int	len;
			unsigned short recordIndex;
			LOGRECORD * pRecBuf = NULL;
			SendMessage(hListbox, LB_RESETCONTENT, 0, 0);
			reccounter = Log_Expired();
			if (reccounter > 0)
			{	
				recordIndex = reccounter-1;
				while (recordIndex >= 0)
				{				
					len = Log_Read(NULL, recordIndex, 0);
					pRecBuf = malloc(len);
					if (NULL == pRecBuf)
						break;
					if (LOG_DB_SUCCESS == Log_Read(pRecBuf, recordIndex, (unsigned short)len)) 
					{
						if (pRecBuf->type == TYPE_SMS) 
						{
							nIndex = SendMessage(hListbox, LB_ADDSTRING, 0, (LPARAM)pRecBuf->APN);
							SendMessage(hListbox, LB_SETITEMDATA, nIndex, (LPARAM)recordIndex);
							switch(pRecBuf->direction) 
							{
							case DRT_SEND:
								SendMessage(hListbox, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP,nIndex), (LPARAM)hSentSmsBmp);
								break;

							case DRT_RECIEVE:
								SendMessage(hListbox, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP,nIndex), (LPARAM)hReceivedSmsBmp);
								break;
							}
						}
					}
					if (pRecBuf) 
					{						
						free(pRecBuf);
						pRecBuf = NULL;
					}
					if (recordIndex == 0) 
						break;
					else
						recordIndex--;
				}
			}
			SendMessage(hListbox, LB_SETCURSEL, 0, 0);
			SendMessage(hwnd, PWM_SHOWWINDOW, 0, 0);
		}
		*/
		break;
		
	case IDM_GPRS:
		tagRealTimeUpdate = FILTERGPRS;
		RealTimeUpdateCommunication();
		/*
		{
			int	len;
			unsigned short recordIndex;
			LOGRECORD * pRecBuf = NULL;
			SendMessage(hListbox, LB_RESETCONTENT, 0, 0);
			reccounter = Log_Expired();
			if (reccounter > 0)
			{	
				recordIndex = reccounter-1;
				while (recordIndex >= 0)
				{				
					len = Log_Read(NULL, recordIndex, 0);
					pRecBuf = malloc(len);
					if (NULL == pRecBuf)
						break;
					if (LOG_DB_SUCCESS == Log_Read(pRecBuf, recordIndex, (unsigned short)len)) 
					{
						if (pRecBuf->type == TYPE_GPRS) 
						{
							nIndex = SendMessage(hListbox, LB_ADDSTRING, 0, (LPARAM)pRecBuf->APN);
							SendMessage(hListbox, LB_SETITEMDATA, nIndex, (LPARAM)recordIndex);
							SendMessage(hListbox, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP,nIndex), (LPARAM)hGPRSBmp);
						}
					}
					if (pRecBuf) 
					{						
						free(pRecBuf);
						pRecBuf = NULL;
					}
					if (recordIndex == 0) 
						break;
					else
						recordIndex--;
				}
			}
			SendMessage(hListbox, LB_SETCURSEL, 0, 0);
			SendMessage(hwnd, PWM_SHOWWINDOW, 0, 0);
		}
		*/
		break;

	case IDM_CALLS:
		tagRealTimeUpdate = FILTERCALL;
		RealTimeUpdateCommunication();
		/*
		{
			int	len;
			unsigned short recordIndex;
			LOGRECORD * pRecBuf = NULL;
			SendMessage(hListbox, LB_RESETCONTENT, 0, 0);
			reccounter = Log_Expired();
			if (reccounter > 0)
			{	
				recordIndex = reccounter-1;
				while (recordIndex >= 0)
				{				
					len = Log_Read(NULL, recordIndex, 0);
					pRecBuf = malloc(len);
					if (NULL == pRecBuf)
						break;
					if (LOG_DB_SUCCESS == Log_Read(pRecBuf, recordIndex, (unsigned short)len)) 
					{
						if (pRecBuf->type == TYPE_VOICE) 
						{
							nIndex = SendMessage(hListbox, LB_ADDSTRING, 0, (LPARAM)pRecBuf->APN);
							SendMessage(hListbox, LB_SETITEMDATA, nIndex, (LPARAM)recordIndex);
							switch(pRecBuf->direction) 
							{
							case DRT_SEND:
								SendMessage(hListbox, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP,nIndex), (LPARAM)hDialledCallBmp);
								break;

							case DRT_RECIEVE:
								SendMessage(hListbox, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP,nIndex), (LPARAM)hReceivedCallBmp);
								break;

							case DRT_UNRECIEVE:
								SendMessage(hListbox, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP,nIndex), (LPARAM)hMissedCallBmp);
								break;
							}
						}
					}
					if (pRecBuf) 
					{						
						free(pRecBuf);
						pRecBuf = NULL;
					}
					if (recordIndex == 0) 
						break;
					else
						recordIndex--;
				}
			}
			SendMessage(hListbox, LB_SETCURSEL, 0, 0);
			SendMessage(hwnd, PWM_SHOWWINDOW, 0, 0);
		}
		*/
		break;
		
	case IDM_DATACALL:
		tagRealTimeUpdate = FILTERDATA;
		RealTimeUpdateCommunication();
		/*
		{
			int	len;
			unsigned short recordIndex;
			LOGRECORD * pRecBuf = NULL;
			SendMessage(hListbox, LB_RESETCONTENT, 0, 0);
			reccounter = Log_Expired();
			if (reccounter > 0)
			{	
				recordIndex = reccounter-1;
				while (recordIndex >= 0)
				{				
					len = Log_Read(NULL, recordIndex, 0);
					pRecBuf = malloc(len);
					if (NULL == pRecBuf)
						break;
					if (LOG_DB_SUCCESS == Log_Read(pRecBuf, recordIndex, (unsigned short)len)) 
					{
						if (pRecBuf->type == TYPE_DATA) 
						{
							nIndex = SendMessage(hListbox, LB_ADDSTRING, 0, (LPARAM)pRecBuf->APN);
							SendMessage(hListbox, LB_SETITEMDATA, nIndex, (LPARAM)recordIndex);
							switch(pRecBuf->direction) 
							{
							case DRT_SEND:
								SendMessage(hListbox, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP,nIndex), (LPARAM)hDialledDataCallBmp);
								break;

							case DRT_RECIEVE:
								SendMessage(hListbox, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP,nIndex), (LPARAM)hReceivedDataCallBmp);
								break;

							case DRT_UNRECIEVE:
								SendMessage(hListbox, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP,nIndex), (LPARAM)hMissedDataCallBmp);
								break;
							}
						}
					}
					if (pRecBuf) 
					{						
						free(pRecBuf);
						pRecBuf = NULL;
					}
					if (recordIndex == 0) 
						break;
					else
						recordIndex--;
				}
			}
			SendMessage(hListbox, LB_SETCURSEL, 0, 0);
			SendMessage(hwnd, PWM_SHOWWINDOW, 0, 0);
		}
		*/
		break;
		
	case IDM_SELNUMBER:
		tagRealTimeUpdate = FILTERSELNUM;
		RealTimeUpdateCommunication();
		/*
		{
			int	strlen;
			PSTR	itemname;
			int	len;
			unsigned short recordIndex;
			LOGRECORD * pRecBuf = NULL;
			
			nIndex = SendMessage(hListbox, LB_GETCURSEL, 0, 0);
			if (nIndex >= 0) 
			{
				strlen = SendMessage(hListbox, LB_GETTEXTLEN, nIndex, 0);
				itemname = malloc(strlen+1);
				if (NULL == itemname)
					break;
				SendMessage(hListbox, LB_GETTEXT, nIndex, (LPARAM)itemname);				
				SendMessage(hListbox, LB_RESETCONTENT, 0, 0);
				reccounter = Log_Expired();
				if (reccounter > 0)
				{	
					recordIndex = reccounter-1;
					while (recordIndex >= 0)
					{				
						len = Log_Read(NULL, recordIndex, 0);
						pRecBuf = malloc(len);
						if (NULL == pRecBuf)
							break;
						if (LOG_DB_SUCCESS == Log_Read(pRecBuf, recordIndex, (unsigned short)len)) 
						{
							if (strcmp(pRecBuf->APN, itemname) == 0) 
							{
								nIndex = SendMessage(hListbox, LB_ADDSTRING, 0, (LPARAM)pRecBuf->APN);
								SendMessage(hListbox, LB_SETITEMDATA, nIndex, (LPARAM)recordIndex);
								switch(pRecBuf->direction) 
								{
								case DRT_SEND:
									SendMessage(hListbox, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP,nIndex), (LPARAM)hDialledCallBmp);
									break;
									
								case DRT_RECIEVE:
									SendMessage(hListbox, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP,nIndex), (LPARAM)hReceivedCallBmp);
									break;
									
								case DRT_UNRECIEVE:
									SendMessage(hListbox, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP,nIndex), (LPARAM)hMissedCallBmp);
									break;
								}								
							}
						}
						if (pRecBuf) 
						{						
							free(pRecBuf);
							pRecBuf = NULL;
						}
						if (recordIndex == 0) 
							break;
						else
							recordIndex--;
					}
				}
				SendMessage(hListbox, LB_SETCURSEL, 0, 0);
				SendMessage(hwnd, PWM_SHOWWINDOW, 0, 0);
			}
		}
		*/
		break;
		
	case IDM_SETLOG:
		CreateSetLogDuration();
		bLogChange = TRUE;
		break;
		
	case IDM_SMS:
		{
			LOGRECORD * pRecord = NULL;
			ABNAMEOREMAIL abnameinfo;
			int	rcIndex;
			
			memset(&abnameinfo, 0, sizeof(ABNAMEOREMAIL));
			nIndex = SendMessage(hListbox,LB_GETCURSEL,0,0);
			rcIndex = SendMessage(hListbox, LB_GETITEMDATA, nIndex, 0);
			len = Log_Read(NULL, (WORD)rcIndex, 0);
			pRecord = malloc(len);
			if (NULL == pRecord)
				return FALSE;
			if (LOG_DB_SUCCESS == Log_Read(pRecord, (WORD)rcIndex, (unsigned short)len))
			{
				strcpy(abnameinfo.szTelOrEmail, pRecord->APN);
				APP_EditSMS(hFrame, pRecord->APN, NULL);
				if(pRecord)
					free(pRecord);
				break;
			}
			else
			{
				if(pRecord)
					free(pRecord);
			
			}			
			
			
		}
		break;
		
	case IDM_MMS:
		{
			LOGRECORD * pRecord = NULL;
			ABNAMEOREMAIL abnameinfo;
			int	rcIndex;
			
			memset(&abnameinfo, 0, sizeof(ABNAMEOREMAIL));
			nIndex = SendMessage(hListbox,LB_GETCURSEL,0,0);
			rcIndex = SendMessage(hListbox, LB_GETITEMDATA, nIndex, 0);
			len = Log_Read(NULL, (WORD)rcIndex, 0);
			pRecord = malloc(len);
			if (NULL == pRecord)
				return FALSE;
			if (LOG_DB_SUCCESS == Log_Read(pRecord, (WORD)rcIndex, (unsigned short)len))
			{
				strcpy(abnameinfo.szTelOrEmail, pRecord->APN);
				APP_EditMMS(hFrame, hwnd, 0, MMS_CALLEDIT_MOBIL, pRecord->APN);
				if(pRecord)
					free(pRecord);
				break;
			}
			else
			{
				if(pRecord)
					free(pRecord);			
			}			
		}
		
		break;
		
	default:
		lResult = PDADefWindowProc(hFrame, WM_COMMAND, wparam, lparam);
		break;
		
	}
	return lResult;
}

static	BOOL	UpdateListFromDir(DRTTYPE dir)
{
	int	len,nIndex;
	unsigned short recordIndex;
	LOGRECORD * pRecBuf = NULL;
	SendMessage(hListbox, LB_RESETCONTENT, 0, 0);
	reccounter = Log_Expired();
	if (reccounter > 0)
	{	
		recordIndex = reccounter-1;
		while (recordIndex >= 0)
		{				
			len = Log_Read(NULL, recordIndex, 0);
			pRecBuf = malloc(len);
			if (NULL == pRecBuf) 
				break;
			if (LOG_DB_SUCCESS == Log_Read(pRecBuf, recordIndex, (unsigned short)len)) 
			{
				if (pRecBuf->direction == dir) 
				{
					nIndex = SendMessage(hListbox, LB_ADDSTRING, 0, (LPARAM)pRecBuf->APN);
					SendMessage(hListbox, LB_SETITEMDATA, nIndex, (LPARAM)recordIndex);
					switch(pRecBuf->type) 
					{
					case TYPE_VOICE:
						SendMessage(hListbox, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP,nIndex), (LPARAM)hDialledCallBmp);
						break;
						
					case TYPE_DATA:
						SendMessage(hListbox, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP,nIndex), (LPARAM)hDialledDataCallBmp);
						break;
						
					case TYPE_SMS:
						SendMessage(hListbox, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP,nIndex), (LPARAM)hSentSmsBmp);
						break;
						
					case TYPE_GPRS:
						SendMessage(hListbox, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP,nIndex), (LPARAM)hGPRSBmp);
						break;
					}
				}
			}
			if (pRecBuf) 
			{						
				free(pRecBuf);
				pRecBuf = NULL;
			}
			if (recordIndex == 0) 
				break;
			else
				recordIndex--;
		}
	}
	return SendMessage(hListbox, LB_SETCURSEL, 0, 0);
}

//	try to use   APP_GetInfoByPhone function translate char * phone to name 
static	BOOL	OpenItem(HWND hwnd, unsigned short nIndex, char * dsp, int dsplen)
{
	LISTBUF	listbuf;
	LOGRECORD * pRecord = NULL; 
	SYSTEMTIME	time;
	int	len;
	RECT	rClient;
	char	timebuf[25];
	char	datebuf[25];
	char	timeitem[50];
	char	cTimeUseddata[25];
	int nday = 0;
	DWORD dhour;
	ABINFO abinfo;


//	int caplen;
	capdisp[0] = 0;
	PREBROW_InitListBuf(&listbuf);
	len = Log_Read(NULL, nIndex, 0);
	pRecord = malloc(len);
	if (NULL == pRecord)
		return FALSE;
	if (LOG_DB_SUCCESS == Log_Read(pRecord, nIndex, (unsigned short)len))
	{
		memset(&time, 0, sizeof(SYSTEMTIME));
		LOG_FTtoST(&pRecord->begintm, &time);
		GetTimeDisplay(time, timebuf, datebuf);
		sprintf(timeitem, "%s %s", datebuf, timebuf);
		switch(pRecord->type) 
		{
		case TYPE_DATA:
		case TYPE_VOICE:
			{
				switch(pRecord->direction) 
				{
				case DRT_SEND:
					PREBROW_AddData(&listbuf, IDS_LABLE_DIRECTION, IDS_DIR_OUT);
					if ( pRecord->type == TYPE_DATA )
						PREBROW_AddData(&listbuf, IDS_LABLE_TYPE, IDS_TYPE_DATACALL);
					else
						PREBROW_AddData(&listbuf, IDS_LABLE_TYPE, IDS_TYPE_VOICE);

					PREBROW_AddData(&listbuf, IDS_LABLE_TIME, timeitem);
					//there is a problem about duration,the format displayed is wrong
//					memset(datebuf, 0, 25);
//					memset(timebuf, 0, 25);
					memset(cTimeUseddata, 0, sizeof(cTimeUseddata));
					memset(&time, 0, sizeof(SYSTEMTIME));
					LOG_FTtoST(&pRecord->u.voice.duration, &time);
					time.wYear = 0;
					time.wMonth = 0;
					time.wDayOfWeek = 0;
					time.wDay = 0;
					nday = time.wHour / 24;
					dhour = time.wHour % 24;
//					GetTimeDisplay(time, timebuf, datebuf);
					if(nday > 1)
						sprintf(cTimeUseddata,"%02dd %02d:%02d:%02d", nday, dhour, time.wMinute,time.wSecond);
					else
						sprintf(cTimeUseddata,"%02d:%02d:%02d", dhour, time.wMinute,time.wSecond);

					PREBROW_AddData(&listbuf, IDS_LABLE_DURATION, cTimeUseddata);
					//add a judgement,if APN is Unknown,
					if (pRecord->APN[0]) 
					{
						PREBROW_AddData(&listbuf, IDS_LABLE_NUMBER, pRecord->APN);
					}
					else
						PREBROW_AddData(&listbuf, IDS_LABLE_NUMBER, IDS_STATUS_UNKNOWN);

					break;
					
				case DRT_RECIEVE:
					PREBROW_AddData(&listbuf, IDS_LABLE_DIRECTION, IDS_DIR_IN);
					if ( pRecord->type == TYPE_DATA )
						PREBROW_AddData(&listbuf, IDS_LABLE_TYPE, IDS_TYPE_DATACALL);
					else
						PREBROW_AddData(&listbuf, IDS_LABLE_TYPE, IDS_TYPE_VOICE);
//					PREBROW_AddData(&listbuf, IDS_LABLE_TYPE, IDS_TYPE_VOICE);
					PREBROW_AddData(&listbuf, IDS_LABLE_TIME, timeitem);
					memset(cTimeUseddata, 0, sizeof(cTimeUseddata));
					memset(&time, 0, sizeof(SYSTEMTIME));
					LOG_FTtoST(&pRecord->u.gprs.duration, &time);
					time.wYear = 0;
					time.wMonth = 0;
					time.wDayOfWeek = 0;
					time.wDay = 0;
					nday = time.wHour / 24;
					dhour = time.wHour % 24;
//					GetTimeDisplay(time, timebuf, datebuf);
					if(nday > 1)
						sprintf(cTimeUseddata,"%02dd %02d:%02d:%02d", nday, dhour, time.wMinute,time.wSecond);
					else
						sprintf(cTimeUseddata,"%02d:%02d:%02d", dhour, time.wMinute,time.wSecond);

					PREBROW_AddData(&listbuf, IDS_LABLE_DURATION, cTimeUseddata);

//					memset(datebuf, 0, 25);
//					sprintf(datebuf,"%d", pRecord->u.voice.duration);
//					PREBROW_AddData(&listbuf, IDS_LABLE_DURATION, datebuf);
					if (pRecord->APN[0]) 
					{
						PREBROW_AddData(&listbuf, IDS_LABLE_NUMBER, pRecord->APN);
					}
					else
						PREBROW_AddData(&listbuf, IDS_LABLE_NUMBER, IDS_STATUS_UNKNOWN);
					break;

				case DRT_UNRECIEVE:
					PREBROW_AddData(&listbuf, IDS_LABLE_DIRECTION, IDS_DIR_MISSED);
					PREBROW_AddData(&listbuf, IDS_LABLE_TYPE, IDS_TYPE_VOICE);
					PREBROW_AddData(&listbuf, IDS_LABLE_TIME, timeitem);					
					if (pRecord->APN[0]) 
					{
						PREBROW_AddData(&listbuf, IDS_LABLE_NUMBER, pRecord->APN);
					}
					else
						PREBROW_AddData(&listbuf, IDS_LABLE_NUMBER, IDS_STATUS_UNKNOWN);
					break;
				
				}
			}
			break;
		
		case TYPE_GPRS:
			{
				switch(pRecord->direction) 
				{
				case DRT_SEND:
					PREBROW_AddData(&listbuf, IDS_LABLE_DIRECTION, IDS_DIR_OUT);
					break;
				case DRT_RECIEVE:
					PREBROW_AddData(&listbuf, IDS_LABLE_DIRECTION, IDS_DIR_IN);
					break;
				}
				PREBROW_AddData(&listbuf, IDS_LABLE_TYPE, IDS_TYPE_GPRS);
				PREBROW_AddData(&listbuf, IDS_LABLE_TIME, timeitem);
				memset(cTimeUseddata, 0, sizeof(cTimeUseddata));
				memset(&time, 0, sizeof(SYSTEMTIME));
				LOG_FTtoST(&pRecord->u.voice.duration, &time);
				time.wYear = 0;
				time.wMonth = 0;
				time.wDayOfWeek = 0;
				time.wDay = 0;
				nday = time.wHour / 24;
				dhour = time.wHour % 24;
				//					GetTimeDisplay(time, timebuf, datebuf);
				if(nday > 1)
					sprintf(cTimeUseddata,"%02dd %02d:%02d:%02d", nday, dhour, time.wMinute,time.wSecond);
				else
					sprintf(cTimeUseddata,"%02d:%02d:%02d", dhour, time.wMinute,time.wSecond);

					PREBROW_AddData(&listbuf, IDS_LABLE_DURATION, cTimeUseddata);
				
/*
				memset(datebuf, 0, 25);
				sprintf(datebuf,"%d", pRecord->u.gprs.duration);
				PREBROW_AddData(&listbuf, IDS_LABLE_DURATION, datebuf);
*/
				memset(datebuf, 0, sizeof(datebuf));
				ByteToSzKiloByte(pRecord->u.gprs.sendcounter, datebuf);
//				sprintf(datebuf,"%d", pRecord->u.gprs.sendcounter);
				PREBROW_AddData(&listbuf, IDS_LABLE_SENTDATA, datebuf);
				memset(datebuf, 0, sizeof(datebuf));
//				sprintf(datebuf,"%d", pRecord->u.gprs.recievecounter);
				ByteToSzKiloByte(pRecord->u.gprs.recievecounter, datebuf);
				PREBROW_AddData(&listbuf, IDS_LABLE_RECEIVEDATA, datebuf);
			}
			break;
			
		case TYPE_SMS:
			{
				switch(pRecord->direction) 
				{
				case DRT_SEND:
					PREBROW_AddData(&listbuf, IDS_LABLE_DIRECTION, IDS_DIR_OUT);
					PREBROW_AddData(&listbuf, IDS_LABLE_TYPE, IDS_TYPE_SMS);
					PREBROW_AddData(&listbuf, IDS_LABLE_TIME, timeitem);
					switch(pRecord->u.sms.status) 
					{
					case SMS_WAIT_SEND:
						PREBROW_AddData(&listbuf, IDS_LABLE_STATUS, IDS_STATUS_PEND);							
						break;

					case SMS_SEND_CONFIRM:
						PREBROW_AddData(&listbuf, IDS_LABLE_STATUS, IDS_STATUS_SENT);
						break;

					case SMS_SEND_UNCONFIRM:
						PREBROW_AddData(&listbuf, IDS_LABLE_STATUS, IDS_STATUS_UNKNOWN);
						break;

					case SMS_SEND_FAIL:
						PREBROW_AddData(&listbuf, IDS_LABLE_STATUS, IDS_STATUS_FAILED);
						break;

					case SMS_RECEIVE:
						PREBROW_AddData(&listbuf, IDS_LABLE_STATUS, IDS_STATUS_DELIVERY);
						break;					
					}
					memset(datebuf, 0, sizeof(datebuf));
					sprintf(datebuf,"%d",pRecord->u.sms.smscounter);
					PREBROW_AddData(&listbuf, IDS_LABLE_SIZE, datebuf);
					if (pRecord->APN[0]) 
					{
						PREBROW_AddData(&listbuf, IDS_LABLE_NUMBER, pRecord->APN);
					}
					else
						PREBROW_AddData(&listbuf, IDS_LABLE_NUMBER, IDS_STATUS_UNKNOWN);

					break;
				case DRT_RECIEVE:
					PREBROW_AddData(&listbuf, IDS_LABLE_DIRECTION, IDS_DIR_IN);
					PREBROW_AddData(&listbuf, IDS_LABLE_TYPE, IDS_TYPE_SMS);
					PREBROW_AddData(&listbuf, IDS_LABLE_TIME, timeitem);					
					memset(datebuf, 0, sizeof(datebuf));
					sprintf(datebuf,"%d",pRecord->u.sms.smscounter);
					PREBROW_AddData(&listbuf, IDS_LABLE_SIZE, datebuf);
					if (pRecord->APN[0]) 
					{
						PREBROW_AddData(&listbuf, IDS_LABLE_NUMBER, pRecord->APN);
					}
					else
						PREBROW_AddData(&listbuf, IDS_LABLE_NUMBER, IDS_STATUS_UNKNOWN);
					break;
				}
			}
			break;			
		}
		//free memory
		if (pRecord) 
		{
			free(pRecord);
			pRecord = NULL;
		}
		

		GetClientRect(hwnd, &rClient);
		hFormviewer = CreateWindow(FORMVIEWER, NULL, 
			WS_CHILD |WS_VISIBLE |WS_VSCROLL,
			rClient.left,rClient.top,
			rClient.right-rClient.left,
			rClient.bottom - rClient.top,
			hwnd,NULL,NULL,
			(PVOID)&listbuf);
		if (hFormviewer == NULL) 
		{
			return FALSE;
		}
		
		PDASetMenu(hFrame, NULL);
		SendMessage(hFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
		SendMessage(hFrame, PWM_SETBUTTONTEXT, 0, (LPARAM)"");
		SendMessage(hFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_OK);
		SendMessage(hFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON), (LPARAM)NULL);
		SendMessage(hFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON), (LPARAM)NULL);
//		SetWindowText(hFrame,IDS_COM_CAPTION);

		//Get user name form ab
		if ( APP_GetInfoByPhone(dsp,&abinfo) )
		{
			if(dsp != NULL)
			{
				if ( AB_MAXLEN_NAME > MAX_CAPLEN )
				{
					strncpy(dsp,abinfo.szName,MAX_CAPLEN);
					dsp[MAX_CAPLEN - 1] = 0;
				}
				else
				{
					strncpy(dsp,abinfo.szName,AB_MAXLEN_NAME);
					dsp[AB_MAXLEN_NAME - 1] = 0;
				}
			}
		}

		if(dsp != NULL && dsplen >0 )
		{
			if ( dsplen > MAX_CAPLEN) 
			{				
				strncpy(capdisp, dsp, MAX_CAPLEN);
				capdisp[MAX_CAPLEN - 1] = 0;
			}
			else
			{
				strncpy(capdisp, dsp, dsplen);
				capdisp[dsplen - 1] = 0;
			}
		}
		SetWindowText(hFrame,capdisp);
		ShowWindow(hFrame, SW_SHOW);
		UpdateWindow(hFrame);
		SetFocus(hFormviewer);

		return TRUE;
	}
	else
	{
		free(pRecord);
		pRecord = NULL;
		return FALSE;
	}
}

static	BOOL	RegisterLogDuraton(VOID)
{
	WNDCLASS	wc;
	wc.style         = 0;
	wc.lpfnWndProc   = SetLogDurationWndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = NULL;
	wc.hIcon         = NULL;
	wc.hCursor       = NULL;
	wc.hbrBackground = NULL;//(HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = LOGDURATIONWC;	
	if( !RegisterClass(&wc) )
		return FALSE;
	return TRUE;
}

static	void	CreateSetLogDuration()
{
	RECT	rfClient;
	GetClientRect(hFrame, &rfClient);
	RegisterLogDuraton();
	hSetLogDuration = CreateWindow(LOGDURATIONWC, NULL,
		WS_VISIBLE|WS_CHILD,
		rfClient.left, rfClient.top, rfClient.right - rfClient.left, rfClient.bottom -rfClient.top,
		hFrame, NULL,NULL,NULL);
	PDASetMenu(hFrame, NULL);
	SendMessage(hFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
	SendMessage(hFrame, PWM_SETBUTTONTEXT, 1, NULL);
	SendMessage(hFrame, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_BACK);
	SetWindowText(hFrame, IDS_COM_CAPTION);
	SendMessage(hFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON), (LPARAM)NULL);
	SendMessage(hFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON), (LPARAM)NULL);
	ShowWindow(hFrame, SW_SHOW);
	UpdateWindow(hFrame);
	SetFocus(hSetLogDuration);
}

static	LRESULT	SetLogDurationWndProc(HWND hwnd, UINT uMsg, WPARAM wparam, LPARAM lparam)
{
	LRESULT	lResult;
	RECT	rClient;
	LOG_DB * plogdb; 
	unsigned long usefultime;
	int		nIndex;

	switch(uMsg) 
	{
	case WM_CREATE:
		{
			
			GetClientRect(hwnd, &rClient);
			hSettingList = CreateWindow("SPINBOXEX",IDS_LOGDURATION,WS_VISIBLE|WS_CHILD|SSBS_LEFT|SSBS_ARROWRIGHT,
				rClient.left, rClient.top, rClient.right - rClient.left, 50,
				hwnd, NULL, NULL, NULL);

			SendMessage(hFrame, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_OPEN, 1), (LPARAM)ML("") );
			//lack of code;should implement read valid data;
			SendMessage(hSettingList, SSBM_ADDSTRING, 0, (LPARAM)ML("Not logged"));
			SendMessage(hSettingList, SSBM_ADDSTRING, 0, (LPARAM)ML("One day"));
			SendMessage(hSettingList, SSBM_ADDSTRING, 0, (LPARAM)ML("7 days"));
			SendMessage(hSettingList, SSBM_ADDSTRING, 0, (LPARAM)ML("14 days"));
			SendMessage(hSettingList, SSBM_ADDSTRING, 0, (LPARAM)ML("30 days"));
			
			plogdb = LOG_DB_Open();
			if(NULL == plogdb)
				return FALSE;
			
			GetLogTime(plogdb);
			switch(plogdb->savetime) 
			{
			case 0:
				SendMessage(hSettingList, SSBM_SETCURSEL,0 ,0);
				//SendMessage(hSettingList, SSBM_ADDSTRING, 0, (LPARAM)ML("Not logged"));
				break;

			case 1:
				SendMessage(hSettingList, SSBM_SETCURSEL,1 ,0);
				//SendMessage(hSettingList, SSBM_ADDSTRING, 0, (LPARAM)ML("One day"));
				break;

			case 7:
				SendMessage(hSettingList, SSBM_SETCURSEL,2 ,0);
				//SendMessage(hSettingList, SSBM_ADDSTRING, 0, (LPARAM)ML("7 days"));
				break;

			case 14:
				SendMessage(hSettingList, SSBM_SETCURSEL,3 ,0);
				//SendMessage(hSettingList, SSBM_ADDSTRING, 0, (LPARAM)ML("14 days"));
				break;

			case 30:
				SendMessage(hSettingList, SSBM_SETCURSEL,4 ,0);
				//SendMessage(hSettingList, SSBM_ADDSTRING, 0, (LPARAM)ML("30 days"));
				break;

			default:
				SendMessage(hSettingList, SSBM_SETCURSEL,4 ,0);
				//SendMessage(hSettingList, SSBM_ADDSTRING, 0, (LPARAM)ML("30 days"));
				break;
			}			
			LOG_DB_Close(plogdb);
		}
		break;

	case WM_SETFOCUS:			
			SetFocus(hSettingList);		
		break;

	case WM_KEYDOWN:
		switch(LOWORD(wparam)) 
		{
		case VK_F5:
//			ShowWindow(hwnd, SW_HIDE);
			CreateSetOptionList();
			break;

		case VK_F10:
			SendMessage(hFrame, PWM_CLOSEWINDOW, (WPARAM)hwnd, 0);
			DestroyWindow(hwnd);
			
//			SendMessage(hCommunicationWnd, PWM_SHOWWINDOW, 0, 0);
			break;

		default:
			lResult = PDADefWindowProc(hwnd, uMsg, wparam, lparam);
			break;
		}
		break;

	case PWM_SHOWWINDOW:
		PDASetMenu(hFrame, NULL);
		SendMessage(hFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
		SendMessage(hFrame, PWM_SETBUTTONTEXT, 1, NULL);
		
		SendMessage(hFrame, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_BACK);
		SetWindowText(hFrame, IDS_COM_CAPTION);
		SendMessage(hFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON), (LPARAM)NULL);
		SendMessage(hFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON), (LPARAM)NULL);
		//ShowWindow(hFrame, SW_SHOW);
		//ShowWindow(hwnd, SW_SHOW);
		SetFocus(hSettingList);		
		break;

	case WM_COMMAND:
		if (HIWORD(wparam) == SSBN_CHANGE) 
		{
			nIndex = SendMessage(hSettingList, SSBM_GETCURSEL, 0, 0);
			
			switch(nIndex) 
			{
			case 0:
				usefultime = 0;
				break;
				
			case 1:
				usefultime = 1;
				break;
				
			case 2:
				usefultime = 7;
				break;
				
			case 3:
				usefultime = 14;
				break;
				
			case 4:
				usefultime = 30;
				break;
			default:
				usefultime = 30;
				break;
			}
			plogdb = LOG_DB_Open();
			GetLogTime(plogdb);
			WriteLogTime(plogdb, plogdb->fstrcdtime, usefultime);
			LOG_DB_Close(plogdb);
		}
		else
		{
			lResult = PDADefWindowProc(hwnd, uMsg, wparam, lparam);		
		}
		break;

	case WM_DESTROY:
		/*
		nIndex = SendMessage(hSettingList, SSBM_GETCURSEL, 0, 0);

		switch(nIndex) {
		case 0:
			usefultime = 0;
			break;

		case 1:
			usefultime = 1;
			break;

		case 2:
			usefultime = 7;
			break;

		case 3:
			usefultime = 14;
			break;

		case 4:
			usefultime = 30;
			break;
		default:
			usefultime = 30;
			break;
		}
		plogdb = LOG_DB_Open();
		GetLogTime(plogdb);
		WriteLogTime(plogdb, plogdb->fstrcdtime, usefultime);
		LOG_DB_Close(plogdb);
		//SendMessage(hFrame, PWM_CLOSEWINDOW, (WPARAM)hwnd, 0);
		*/
		UnregisterClass(LOGDURATIONWC, NULL);
		hSetLogDuration = NULL;
		break;

	default:
		lResult = PDADefWindowProc(hwnd, uMsg, wparam, lparam);
		break;
	}
	return lResult;
}
static	BOOL	RegisterSetOptionList()
{
	WNDCLASS	wc;
	wc.style         = 0;
	wc.lpfnWndProc   = SetOptionListWndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = NULL;
	wc.hIcon         = NULL;
	wc.hCursor       = NULL;
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = SETOPTIONLISTWC;	
	if( !RegisterClass(&wc) )
		return FALSE;
	return TRUE;
}

static	void	CreateSetOptionList()
{
	RECT	rfClient;
	GetClientRect(hFrame, &rfClient);
	RegisterSetOptionList();
	hSetDurationOption = CreateWindow(SETOPTIONLISTWC, NULL,
		WS_VISIBLE|WS_CHILD,
		rfClient.left, rfClient.top, rfClient.right - rfClient.left, rfClient.bottom -rfClient.top,
		hFrame, NULL,NULL,NULL);
	PDASetMenu(hFrame, NULL);
	SendMessage(hFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
	SendMessage(hFrame, PWM_SETBUTTONTEXT, 1, NULL);
	SendMessage(hFrame, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_CANCEL);
	SetWindowText(hFrame, IDS_LOGDURATION);
	SendMessage(hFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON), (LPARAM)NULL);
	SendMessage(hFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON), (LPARAM)NULL);
	ShowWindow(hFrame, SW_SHOW);
	UpdateWindow(hFrame);
	SetFocus(hSetDurationOption);
}

static	LRESULT	SetOptionListWndProc(HWND hwnd, UINT uMsg, WPARAM wparam, LPARAM lparam)
{
	LRESULT	lResult;
	RECT	rClient;
	static	HBITMAP	hRBbmp[2];
	int		index;
//	char	logvalue[15];
	LOG_DB * plogdb; 	
	unsigned long usefultime;


	switch(uMsg) 
	{
	case WM_CREATE:
		hRBbmp[0]	=	LoadImage(NULL, RADIO_OFF, IMAGE_BITMAP, 16, 16, LR_LOADFROMFILE);
		hRBbmp[1]	=	LoadImage(NULL, RADIO_ON, IMAGE_BITMAP, 16, 16, LR_LOADFROMFILE);
		GetClientRect(hwnd, &rClient);
		hSetOptionlist = CreateWindow("LISTBOX", NULL,
			WS_CHILD| WS_VISIBLE|LBS_BITMAP,
			rClient.left,
			rClient.top,
			rClient.right - rClient.left,
			rClient.bottom - rClient.top,
			hwnd, NULL, NULL, NULL);
		index = SendMessage(hSetOptionlist,LB_ADDSTRING,0,(LPARAM)ML("Not logged"));
		SendMessage(hSetOptionlist, LB_SETITEMDATA, index, 0);
		SendMessage(hSetOptionlist, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, index), (LPARAM)hRBbmp[0]);
		index = SendMessage(hSetOptionlist,LB_ADDSTRING,0,(LPARAM)ML("One day"));
		SendMessage(hSetOptionlist, LB_SETITEMDATA, index, 1);
		SendMessage(hSetOptionlist, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, index), (LPARAM)hRBbmp[0]);
		index = SendMessage(hSetOptionlist,LB_ADDSTRING,0,(LPARAM)ML("7 days"));
		SendMessage(hSetOptionlist, LB_SETITEMDATA, index, 7);
		SendMessage(hSetOptionlist, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, index), (LPARAM)hRBbmp[0]);
		index = SendMessage(hSetOptionlist,LB_ADDSTRING,0,(LPARAM)ML("14 days"));
		SendMessage(hSetOptionlist, LB_SETITEMDATA, index, 14);
		SendMessage(hSetOptionlist, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, index), (LPARAM)hRBbmp[0]);
		index = SendMessage(hSetOptionlist,LB_ADDSTRING,0,(LPARAM)ML("30 days"));				
		SendMessage(hSetOptionlist, LB_SETITEMDATA, index, 30);
		SendMessage(hSetOptionlist, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, index), (LPARAM)hRBbmp[0]);
		plogdb = LOG_DB_Open();
		GetLogTime(plogdb);
		switch(plogdb->savetime) 
			{
			case 0:
				SendMessage(hSetOptionlist, LB_SETCURSEL,0 ,0);
				SendMessage(hSetOptionlist, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, 0), (LPARAM)hRBbmp[1]);
				break;

			case 1:
				SendMessage(hSetOptionlist, LB_SETCURSEL,1 ,0);
				SendMessage(hSetOptionlist, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, 1), (LPARAM)hRBbmp[1]);
				break;

			case 7:
				SendMessage(hSetOptionlist, LB_SETCURSEL,2 ,0);
				SendMessage(hSetOptionlist, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, 2), (LPARAM)hRBbmp[1]);
				break;

			case 14:
				SendMessage(hSetOptionlist, LB_SETCURSEL,3 ,0);	
				SendMessage(hSetOptionlist, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, 3), (LPARAM)hRBbmp[1]);
				break;

			case 30:
				SendMessage(hSetOptionlist, LB_SETCURSEL,4 ,0);	
				SendMessage(hSetOptionlist, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, 4), (LPARAM)hRBbmp[1]);
				break;

			default:
				SendMessage(hSetOptionlist, LB_SETCURSEL,4 ,0);
				SendMessage(hSetOptionlist, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, 4), (LPARAM)hRBbmp[1]);
				break;
			}			
		LOG_DB_Close(plogdb);
			
		break;

	case PWM_SHOWWINDOW:
		PDASetMenu(hFrame, NULL);
		SendMessage(hFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
		SendMessage(hFrame, PWM_SETBUTTONTEXT, 1, NULL);
		SendMessage(hFrame, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_BACK);
		SetWindowText(hFrame, IDS_LOGDURATION);
		SendMessage(hFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON), (LPARAM)NULL);
		SendMessage(hFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON), (LPARAM)NULL);
		SetFocus(hSetOptionlist);		
		break;

	case WM_SETFOCUS:
		SetFocus(hSetOptionlist);
		break;

	case  WM_TIMER:
		if (wparam  == 1) 
		{
			index = SendMessage(hSetOptionlist, LB_GETCURSEL, 0, 0);
			usefultime = SendMessage(hSetOptionlist, LB_GETITEMDATA, index, 0);
			switch(usefultime) 
			{
			case 0:
				SendMessage(hSettingList, SSBM_SETCURSEL,0 ,0);
				//SendMessage(hSettingList, SSBM_ADDSTRING, 0, (LPARAM)ML("Not logged"));
				break;

			case 1:
				SendMessage(hSettingList, SSBM_SETCURSEL,1 ,0);
				//SendMessage(hSettingList, SSBM_ADDSTRING, 0, (LPARAM)ML("One day"));
				break;

			case 7:
				SendMessage(hSettingList, SSBM_SETCURSEL,2 ,0);
				//SendMessage(hSettingList, SSBM_ADDSTRING, 0, (LPARAM)ML("7 days"));
				break;

			case 14:
				SendMessage(hSettingList, SSBM_SETCURSEL,3 ,0);
				//SendMessage(hSettingList, SSBM_ADDSTRING, 0, (LPARAM)ML("14 days"));
				break;

			case 30:
				SendMessage(hSettingList, SSBM_SETCURSEL,4 ,0);
				//SendMessage(hSettingList, SSBM_ADDSTRING, 0, (LPARAM)ML("30 days"));
				break;

			default:
				SendMessage(hSettingList, SSBM_SETCURSEL,4 ,0);
				//SendMessage(hSettingList, SSBM_ADDSTRING, 0, (LPARAM)ML("30 days"));
				break;
			}			
			//lack of code 
			//todo: implement the setting about log expired time;-->OK
			
			plogdb = LOG_DB_Open();
			GetLogTime(plogdb);
			WriteLogTime(plogdb, plogdb->fstrcdtime, usefultime);
			LOG_DB_Close(plogdb);
			SendMessage(hFrame, PWM_CLOSEWINDOW, (WPARAM)hwnd, 0);
		
			DestroyWindow(hwnd);

		}
		else
			lResult = PDADefWindowProc(hwnd, uMsg, wparam, lparam);
		break;

	case WM_KEYDOWN:
		switch(LOWORD(wparam)) 
		{
		case VK_F5:
			index = SendMessage(hSetOptionlist, LB_GETCURSEL, 0, 0);
			//index = SendMessage(hSetOptionlist, LB_GETCURSEL, 0, 0);
			Load_Icon_SetupList(hSetOptionlist, hRBbmp[0], hRBbmp[1], 5, index);
			SetTimer(hwnd, ID_TIMER, 500, NULL);
			//SendMessage(hSetOptionlist, LB_GETTEXT, index, (LPARAM)logvalue);
			//SendMessage(hSettingList, SSBM_SETTEXT, 0, (LPARAM)logvalue);
			/*
			usefultime = SendMessage(hSetOptionlist, LB_GETITEMDATA, index, 0);
			switch(usefultime) 
			{
			case 0:
				SendMessage(hSettingList, SSBM_SETCURSEL,0 ,0);
				//SendMessage(hSettingList, SSBM_ADDSTRING, 0, (LPARAM)ML("Not logged"));
				break;

			case 1:
				SendMessage(hSettingList, SSBM_SETCURSEL,1 ,0);
				//SendMessage(hSettingList, SSBM_ADDSTRING, 0, (LPARAM)ML("One day"));
				break;

			case 7:
				SendMessage(hSettingList, SSBM_SETCURSEL,2 ,0);
				//SendMessage(hSettingList, SSBM_ADDSTRING, 0, (LPARAM)ML("7 days"));
				break;

			case 14:
				SendMessage(hSettingList, SSBM_SETCURSEL,3 ,0);
				//SendMessage(hSettingList, SSBM_ADDSTRING, 0, (LPARAM)ML("14 days"));
				break;

			case 30:
				SendMessage(hSettingList, SSBM_SETCURSEL,4 ,0);
				//SendMessage(hSettingList, SSBM_ADDSTRING, 0, (LPARAM)ML("30 days"));
				break;

			default:
				SendMessage(hSettingList, SSBM_SETCURSEL,4 ,0);
				//SendMessage(hSettingList, SSBM_ADDSTRING, 0, (LPARAM)ML("30 days"));
				break;
			}			
			//lack of code 
			//todo: implement the setting about log expired time;-->OK
			
			plogdb = LOG_DB_Open();
			GetLogTime(plogdb);
			WriteLogTime(plogdb, plogdb->fstrcdtime, usefultime);
			LOG_DB_Close(plogdb);
			SendMessage(hFrame, PWM_CLOSEWINDOW, (WPARAM)hwnd, 0);
		
			DestroyWindow(hwnd);
			*/
			break;

		case VK_F10:
			SendMessage(hFrame, PWM_CLOSEWINDOW, (WPARAM)hwnd, 0);
//			ShowWindow(hSetLogDuration,SW_SHOW);
			DestroyWindow(hwnd);
			break;

		default:
			lResult = PDADefWindowProc(hwnd, uMsg, wparam, lparam);
			break;
		}
		break;
/*
	case WM_COMMAND:
		switch(HIWORD(wparam)) 
		{
		case LBN_SELCHANGE:
			index = SendMessage(hSetOptionlist, LB_GETCURSEL, 0, 0);
			Load_Icon_SetupList(hSetOptionlist, hRBbmp[0], hRBbmp[1], 5, index);
			break;
		default:
			lResult = PDADefWindowProc(hwnd, uMsg, wparam, lparam);
			break;
		}
		break;
*/
	case WM_DESTROY:
		if (hRBbmp[0]) {
			DeleteObject(hRBbmp[0]);
		}
		if (hRBbmp[1]) {
			DeleteObject(hRBbmp[1]);
		}
		UnregisterClass(SETOPTIONLISTWC,NULL);
		break;

	default:
		lResult = PDADefWindowProc(hwnd, uMsg, wparam, lparam);
		break;
	}
	return lResult;
}

void	RealTimeUpdateCommunication()
{
	int	nIndex;
	if ((!IsWindow(hListbox))||(!IsWindow(hCommunicationWnd))) {
		return;
	}
	switch(tagRealTimeUpdate) 
	{
	case FILTERALL:
		InitLogView();
		SendMessage(hCommunicationWnd, PWM_SHOWWINDOW, 0, 0);
		break;

	case FILTERDATA:
		{
			int	len;
			unsigned short recordIndex;
			LOGRECORD * pRecBuf = NULL;
			SendMessage(hListbox, LB_RESETCONTENT, 0, 0);
			reccounter = Log_Expired();
			if (reccounter > 0)
			{	
				recordIndex = reccounter-1;
				while (recordIndex >= 0)
				{				
					len = Log_Read(NULL, recordIndex, 0);
					pRecBuf = malloc(len);
					if (NULL == pRecBuf)
						break;
					if (LOG_DB_SUCCESS == Log_Read(pRecBuf, recordIndex, (unsigned short)len)) 
					{
						if (pRecBuf->type == TYPE_DATA) 
						{
							nIndex = SendMessage(hListbox, LB_ADDSTRING, 0, (LPARAM)pRecBuf->APN);
							SendMessage(hListbox, LB_SETITEMDATA, nIndex, (LPARAM)recordIndex);
							switch(pRecBuf->direction) 
							{
							case DRT_SEND:
								SendMessage(hListbox, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP,nIndex), (LPARAM)hDialledDataCallBmp);
								break;

							case DRT_RECIEVE:
								SendMessage(hListbox, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP,nIndex), (LPARAM)hReceivedDataCallBmp);
								break;

							case DRT_UNRECIEVE:
								SendMessage(hListbox, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP,nIndex), (LPARAM)hMissedDataCallBmp);
								break;
							}
						}
					}
					if (pRecBuf) 
					{						
						free(pRecBuf);
						pRecBuf = NULL;
					}
					if (recordIndex == 0) 
						break;
					else
						recordIndex--;
				}
			}
			SendMessage(hListbox, LB_SETCURSEL, 0, 0);
			SendMessage(hCommunicationWnd, PWM_SHOWWINDOW, 0, 0);
		}
		break;
		
	case FILTERMSG:
		{
			int	len;
			unsigned short recordIndex;
			LOGRECORD * pRecBuf = NULL;
			SendMessage(hListbox, LB_RESETCONTENT, 0, 0);
			reccounter = Log_Expired();
			if (reccounter > 0)
			{	
				recordIndex = reccounter-1;
				while (recordIndex >= 0)
				{				
					len = Log_Read(NULL, recordIndex, 0);
					pRecBuf = malloc(len);
					if (NULL == pRecBuf)
						break;
					if (LOG_DB_SUCCESS == Log_Read(pRecBuf, recordIndex, (unsigned short)len)) 
					{
						if (pRecBuf->type == TYPE_SMS) 
						{
							nIndex = SendMessage(hListbox, LB_ADDSTRING, 0, (LPARAM)pRecBuf->APN);
							SendMessage(hListbox, LB_SETITEMDATA, nIndex, (LPARAM)recordIndex);
							switch(pRecBuf->direction) 
							{
							case DRT_SEND:
								SendMessage(hListbox, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP,nIndex), (LPARAM)hSentSmsBmp);
								break;

							case DRT_RECIEVE:
								SendMessage(hListbox, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP,nIndex), (LPARAM)hReceivedSmsBmp);
								break;
							}
						}
					}
					if (pRecBuf) 
					{						
						free(pRecBuf);
						pRecBuf = NULL;
					}
					if (recordIndex == 0) 
						break;
					else
						recordIndex--;
				}
			}
			SendMessage(hListbox, LB_SETCURSEL, 0, 0);
			SendMessage(hCommunicationWnd, PWM_SHOWWINDOW, 0, 0);
		}
		break;
		
	case FILTERGPRS:
		{
			int	len;
			unsigned short recordIndex;
			LOGRECORD * pRecBuf = NULL;
			SendMessage(hListbox, LB_RESETCONTENT, 0, 0);
			reccounter = Log_Expired();
			if (reccounter > 0)
			{	
				recordIndex = reccounter-1;
				while (recordIndex >= 0)
				{				
					len = Log_Read(NULL, recordIndex, 0);
					pRecBuf = malloc(len);
					if (NULL == pRecBuf)
						break;
					if (LOG_DB_SUCCESS == Log_Read(pRecBuf, recordIndex, (unsigned short)len)) 
					{
						if (pRecBuf->type == TYPE_GPRS) 
						{
							nIndex = SendMessage(hListbox, LB_ADDSTRING, 0, (LPARAM)pRecBuf->APN);
							SendMessage(hListbox, LB_SETITEMDATA, nIndex, (LPARAM)recordIndex);
							SendMessage(hListbox, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP,nIndex), (LPARAM)hGPRSBmp);
						}
					}
					if (pRecBuf) 
					{						
						free(pRecBuf);
						pRecBuf = NULL;
					}
					if (recordIndex == 0) 
						break;
					else
						recordIndex--;
				}
			}
			SendMessage(hListbox, LB_SETCURSEL, 0, 0);
			SendMessage(hCommunicationWnd, PWM_SHOWWINDOW, 0, 0);
		}
		break;
		
	case FILTERSELNUM:
		{
			int	strlen;
			PSTR	itemname;
			int	len;
			unsigned short recordIndex;
			LOGRECORD * pRecBuf = NULL;
			
			nIndex = SendMessage(hListbox, LB_GETCURSEL, 0, 0);
			if (nIndex >= 0) 
			{
				strlen = SendMessage(hListbox, LB_GETTEXTLEN, nIndex, 0);
				itemname = malloc(strlen+1);
				if (NULL == itemname)
					break;
				SendMessage(hListbox, LB_GETTEXT, nIndex, (LPARAM)itemname);				
				SendMessage(hListbox, LB_RESETCONTENT, 0, 0);
				reccounter = Log_Expired();
				if (reccounter > 0)
				{	
					recordIndex = reccounter-1;
					while (recordIndex >= 0)
					{				
						len = Log_Read(NULL, recordIndex, 0);
						pRecBuf = malloc(len);
						if (NULL == pRecBuf)
							break;
						if (LOG_DB_SUCCESS == Log_Read(pRecBuf, recordIndex, (unsigned short)len)) 
						{
							if (strcmp(pRecBuf->APN, itemname) == 0) 
							{
								nIndex = SendMessage(hListbox, LB_ADDSTRING, 0, (LPARAM)pRecBuf->APN);
								SendMessage(hListbox, LB_SETITEMDATA, nIndex, (LPARAM)recordIndex);
								switch(pRecBuf->direction) 
								{
								case DRT_SEND:
									SendMessage(hListbox, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP,nIndex), (LPARAM)hDialledCallBmp);
									break;
									
								case DRT_RECIEVE:
									SendMessage(hListbox, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP,nIndex), (LPARAM)hReceivedCallBmp);
									break;
									
								case DRT_UNRECIEVE:
									SendMessage(hListbox, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP,nIndex), (LPARAM)hMissedCallBmp);
									break;
								}								
							}
						}
						if (pRecBuf) 
						{						
							free(pRecBuf);
							pRecBuf = NULL;
						}
						if (recordIndex == 0) 
							break;
						else
							recordIndex--;
					}
				}
				if (itemname) 
				{
					free(itemname);
					itemname = NULL;
				}
				SendMessage(hListbox, LB_SETCURSEL, 0, 0);
				SendMessage(hCommunicationWnd, PWM_SHOWWINDOW, 0, 0);
			}
		}
		break;

	case FILTERCALL:
		{
			int	len;
			unsigned short recordIndex;
			LOGRECORD * pRecBuf = NULL;
			SendMessage(hListbox, LB_RESETCONTENT, 0, 0);
			reccounter = Log_Expired();
			if (reccounter > 0)
			{	
				recordIndex = reccounter-1;
				while (recordIndex >= 0)
				{				
					len = Log_Read(NULL, recordIndex, 0);
					pRecBuf = malloc(len);
					if (NULL == pRecBuf)
						break;
					if (LOG_DB_SUCCESS == Log_Read(pRecBuf, recordIndex, (unsigned short)len)) 
					{
						if (pRecBuf->type == TYPE_VOICE) 
						{
							nIndex = SendMessage(hListbox, LB_ADDSTRING, 0, (LPARAM)pRecBuf->APN);
							SendMessage(hListbox, LB_SETITEMDATA, nIndex, (LPARAM)recordIndex);
							switch(pRecBuf->direction) 
							{
							case DRT_SEND:
								SendMessage(hListbox, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP,nIndex), (LPARAM)hDialledCallBmp);
								break;
								
							case DRT_RECIEVE:
								SendMessage(hListbox, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP,nIndex), (LPARAM)hReceivedCallBmp);
								break;
								
							case DRT_UNRECIEVE:
								SendMessage(hListbox, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP,nIndex), (LPARAM)hMissedCallBmp);
								break;
							}
						}
					}
					if (pRecBuf) 
					{						
						free(pRecBuf);
						pRecBuf = NULL;
					}
					if (recordIndex == 0) 
						break;
					else
						recordIndex--;
				}
			}
			SendMessage(hListbox, LB_SETCURSEL, 0, 0);
			SendMessage(hCommunicationWnd, PWM_SHOWWINDOW, 0, 0);
		}
		break;
		
	case FILTEROUT:
		{
			UpdateListFromDir(DRT_SEND);
			SendMessage(hCommunicationWnd, PWM_SHOWWINDOW, 0, 0);
		}
		break;
		
	case FILTERIN:
		{
			UpdateListFromDir(DRT_RECIEVE);
			SendMessage(hCommunicationWnd, PWM_SHOWWINDOW, 0, 0);
		}
		break;

	}
}

void LoadLogInfos(HWND hwnd, LPARAM lparam)
{
	SIZE rsize;
	int j;
	
	memset(&rsize, 0, sizeof(SIZE));
	GetImageDimensionFromFile(COM_RECORD_ANSWER, &rsize);
	if (hReceivedCallBmp == NULL)
		hReceivedCallBmp = LoadImage(NULL, COM_RECORD_ANSWER, IMAGE_BITMAP, rsize.cx, rsize.cy, LR_LOADFROMFILE);
	GetImageDimensionFromFile(COM_RECORD_UNANSWERDATA, &rsize);
	if (hMissedCallBmp == NULL)
		hMissedCallBmp	= LoadImage(NULL, COM_RECORD_UNANSWER, IMAGE_BITMAP, rsize.cx, rsize.cy, LR_LOADFROMFILE);
	GetImageDimensionFromFile(COM_RECORD_DIALED, &rsize);
	if (hDialledCallBmp == NULL)
		hDialledCallBmp = LoadImage(NULL, COM_RECORD_DIALED, IMAGE_BITMAP, rsize.cx, rsize.cy, LR_LOADFROMFILE);
	GetImageDimensionFromFile(COM_RECORD_RSMS, &rsize);
	if (hReceivedSmsBmp == NULL)
		hReceivedSmsBmp = LoadImage(NULL, COM_RECORD_RSMS, IMAGE_BITMAP, rsize.cx, rsize.cy, LR_LOADFROMFILE);
	GetImageDimensionFromFile(COM_RECORD_SSMS, &rsize);
	if (hSentSmsBmp == NULL)
		hSentSmsBmp	= LoadImage(NULL, COM_RECORD_SSMS, IMAGE_BITMAP, rsize.cx, rsize.cy, LR_LOADFROMFILE);
	GetImageDimensionFromFile(COM_RECORD_GPRS, &rsize);
	if (hGPRSBmp == NULL)
		hGPRSBmp = LoadImage(NULL, COM_RECORD_GPRS, IMAGE_BITMAP, rsize.cx, rsize.cy, LR_LOADFROMFILE);
	GetImageDimensionFromFile(COM_RECORD_DIALEDDATA, &rsize);
	if (hDialledDataCallBmp == NULL)
		hDialledDataCallBmp = LoadImage(NULL, COM_RECORD_DIALEDDATA, IMAGE_BITMAP, rsize.cx, rsize.cy, LR_LOADFROMFILE);
	GetImageDimensionFromFile(COM_RECORD_ANSWERDATA, &rsize);
	if (hReceivedDataCallBmp == NULL)
		hReceivedDataCallBmp = LoadImage(NULL, COM_RECORD_ANSWERDATA, IMAGE_BITMAP, rsize.cx, rsize.cy, LR_LOADFROMFILE);
	GetImageDimensionFromFile(COM_RECORD_UNANSWERDATA, &rsize);
	if (hMissedDataCallBmp == NULL)
		hMissedDataCallBmp = LoadImage(NULL, COM_RECORD_UNANSWERDATA, IMAGE_BITMAP, rsize.cx, rsize.cy, LR_LOADFROMFILE);
	
	LOGCOMM_Create(hwnd, lparam);
	InitLogView();
	
	SendMessage(hFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, LEFTICON), (LPARAM)LOG_LEFT_ICON);
	SendMessage(hFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, RIGHTICON), (LPARAM)"");
	
	SetWindowText(hFrame,IDS_COM_CAPTION);
	
	SendMessage(hFrame, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_OPEN, 1), (LPARAM)ML("Open") );
	SendMessage(hFrame,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDC_EXIT, 0),(LPARAM)IDS_EXIT);
	
	j = SendMessage(hListbox, LB_GETCOUNT, 0, 0);
	if (j <= 0) 
	{
		SendMessage(hFrame,PWM_SETBUTTONTEXT,1,(LPARAM)"");
		PDASetMenu(hFrame, hMenuEmptyList);
		ShowWindow(hListbox,SW_HIDE);
		SetFocus(hwnd);
	}
	else
	{
		SendMessage(hFrame,PWM_SETBUTTONTEXT,1,(LPARAM)IDS_OPEN);
		PDASetMenu(hFrame, hMenu);
		ShowWindow(hListbox,SW_SHOW);
		SendMessage(hListbox, LB_SETCURSEL, 0,0);
		SetFocus(hListbox);
	}
}
