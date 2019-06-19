      

#include    "MBRecordView.h"
#include    "MBRecordList.h"
#include    "winpda.h"
#include    "window.h"
#include    "mullang.h"
#include    "setting.h"
#include    "plx_pdaex.h"
#include    "RcCommon.h"
#include    "phonebookext.h"
#include    "hpimage.h"

#include	"MBPublic.h"

//#define     IDC_BUTTON_RIGHT        3
#define     IDC_BUTTON_SET          4
#define     IDC_LIST                5

#define		IDRM_ISREMOVE			(WM_USER+200)
#define		IDRM_ISRMALL			(WM_USER+201)
#define		IDRM_RETURN				(WM_USER+202)
#define     WM_CONTACTUPDATE        (WM_USER+203)

#define     TITLECAPTION            ML("Calllogs")
#define     TITLECAPTION1           ML("Missed calls")
#define     TITLECAPTION2           ML("Received calls")
#define     TITLECAPTION3           ML("Dialled calls")

#define     BUTTONRIGHT             (LPARAM)ML("Back") 
#define     IDS_EXIT                (LPARAM)ML("Exit")
#define     ERROR1                  ML("Create window fail") 

#define     IDS_MBRECORD_VIEW       ML("View") 
#define     IDS_MBRECORD_SHIKE      ML("Time") 
#define     IDS_MBRECORD_NUM        ML("Number") 
#define     IDS_MBRECORD_NAME       ML("Name") 
#define     IDS_MBRECORD_UNKNOWN    ML("Unkow") 
#define     IDS_MBRECORD_DURATION   ML("Duration") 

#define     MBRECORDDELETEONE       ML("Deleteone?") 
#define     MBRECORDDELETEALL       ML("Deleteall?") 
#define     MBRECORDLISTDELFAIL     ML("Remove fail") 

#define     MBRECORDMENUVIEW        ML("View") 
#define     MBRECORDMENUSORTTIME    ML("Timeorder")
#define     MBRECORDMENUSORTNAME    ML("Nameorder") 
#define     MBRECORDMENUPICKUP      ML("Pickup") 
#define     MBRECORDMENUPHONE       ML("Call") 
#define     MBRECORDMENULINKMAN     ML("Phonebook") 
#define     MBRECORDMENUSMS         ML("SMS") 
#define     MBRECORDMENUMMS         ML("MMS") 

#define     MBRECORDMENUDELETE      ML("Delete") 
#define     MBRECORDMENUDELETEONE   ML("Deleteone") 
#define     MBRECORDMENUDELETEALL   ML("Deleteall")
 
#define     NOTIFY_REMOVED         ML("Removed")
#define     NOTIFY_REMOVEMISSCALL   ML("Remove missed\ncall?")
#define     NOTIFY_REMOVEMISSCALLS ML("Remove missed\ncalls?")      
#define     NOTIFY_REMOVERECIEVEDCALL   ML("Remove received\ncall?")
#define     NOTIFY_REMOVERECIEVEDCALLS ML("Remove received\ncalls?")      
#define     NOTIFY_REMOVEDIALEDCALL   ML("Remove dialled\ncall?")
#define     NOTIFY_REMOVEDIALEDCALLS ML("Remove dialled\ncalls?")      
#define     NOTIFY_EMPTYRECEIVEDCALLS  ML("No received calls")
#define     NOTIFY_EMPTYMISSEDCALLS  ML("No missed calls")
#define     NOTIFY_EMPTYDIALEDCALLS  ML("No dialled calls")
#define     NOTIFY_NOCALLSSELECTED  ML("No calls selected")
#define     NOTIFY_UNSUCCESSFUL     ML("Unsuccessful") 

#define     IDS_MENU_SAVE           ML("Save")
#define     IDS_MENU_NEWCONTACT     ML("New contact")
#define     IDS_MENU_ADDTOCONTACT   ML("Add to contact")
#define     IDS_MENU_MESSAGE        ML("Write message")
#define     IDS_MENU_SMS            ML("SMS")
#define     IDS_MENU_MMS            ML("MMS")
#define     IDS_MENU_DETAILS        ML("Details")
#define     IDS_REMOVE              ML("Remove")
#define     IDS_REMOVEMANY          ML("Remove many")
#define     IDS_SELECTED            ML("Selected")
#define     IDS_ALL                 ML("All")

#define     TITLECAPTIONMAXLEN      40
#define     IDC_REFRESHRECORDLIST   WM_USER + 21


#define     MENU_POPUP_PICKUP       1
#define     MENU_POPUP_DELETE       2

#define     IDM_NEWCONTACT           90
#define     IDM_ADDTOCONTACT         95
#define     IDC_EXIT				 97
#define		IDC_SAVE				 98
#define     IDM_SMS                 100
#define     IDM_MMS                 105
#define     IDM_DETAILS             110
#define     IDM_REMOVE              120
#define     IDM_REMOVESELECTED      130
#define     IDM_REMOVEALL           135
#define     IDM_DIALRECORD          140

static const int iMBRecords[] = 
{
    UNANSWER_REC,
    ANSWERED_REC,
    DIALED_REC
};
typedef enum
{
    iSORT_TIME = 1,
    iSORT_NAME = 2 
}SORTTYPE;
static const char * pMISSDATA_LOG  = "/mnt/flash/calllogs/MissDataLog.dat";
typedef struct
{
	int flag;
	int count;
}MISSDATA;

static MISSDATA mdata;

typedef struct 
{
	char PhoneNo[PHONENUMMAXLEN + 1];
	int  count;
}RECENTMISSRECORD;
static RECENTMISSRECORD * pRecentMissCall;

static const int    iRecordTypeCount = 3;
static const int    iViewMaxLen      = 12; //20;
static const char * pClassName       = "MBRecordLogListExt";
static const char * pSetTitle        = "%s(%d/%d)";
static const char * pCHOMIT          = "...";

static const char * pIconInName      = MBPIC_RECORDLIST_IN; 
static const char * pIconOutName     = MBPIC_RECORDLIST_OUT;
static const char * pIconUnName      = MBPIC_RECORDLIST_UN;   
static BOOL bInit = FALSE;

typedef struct tagMBRecordListExt
{
    int                         iMBRecordType;               
    int                         iMBRecordIndex;              
    char                        cDisplay[PHONENUMMAXLEN + 1];
    char                        cNumber[PHONENUMMAXLEN + 1]; 
    BOOL                        bInPhonebook;                
    SYSTEMTIME                  Time;                        
    struct tagMBRecordListExt * pNext;
};
typedef struct tagMBRecordListExt MBRECORDLISTEXT;
typedef struct tagWndInfo{
	HWND hInfoWnd;
	UINT wMsg;
}WNDINFO;
static WNDINFO gwndinfo;

static MBRECORDLISTEXT    * pMBRecordListExt;


static HINSTANCE            hInstance;
static int                  iRecordListType;
static HWND   hRecordListWnd;
static HWND   hPWnd;
static HWND   hMWnd;
static HWND   hFocus;
static HWND   hRecordList;
static      HWND            hRecordList;
static HBITMAP              hIconUnans;     
static HBITMAP              hIconIn;        
static HBITMAP              hIconOut;       
static char                 cTitle[TITLECAPTIONMAXLEN + 1] = "";
static WNDPROC              OldListProc;
static int    vmissedcount =0; 


static MBRECORDLIST_STYLE   MBL_Style;
static MBRECORDLIST_RS      BackValue;
static BOOL  Dflag = FALSE;

static  LRESULT CALLBACK NewListProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static  LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static  BOOL    CreateControl(HWND hWnd);
static  int     GetRecordListCount(void);
static  void    DestroyRecordListExt(void);
static  int     AddNodeMBRecordListExt(MBRECORDTYPE iMBRecordType,int iMBRecordIndex,
                                       BOOL bInPhonebook,const SYSTEMTIME * pTime,
                                       const char * pDisplay,const char * pNumber,
                                       SORTTYPE SortType);
static  BOOL    LoadMBRecordToLink(SORTTYPE SortType);
static  void    ReloadRecordList(HWND hWnd,HWND hList,SORTTYPE SortType);
static  BOOL    GetMBRecordListExtNode(MBRECORDLISTEXT * pNode,int iIndex);
static  void    SetTitle(HWND hWnd,HWND hList);
static  void    InitMBRecorListExt(void);

static  void    SetLIstBoxIcon(HWND hList,int index,MBRECORDTYPE type);
static  BOOL    RefreshMBRecordList_One(HWND hList,const MBRECORDLISTEXT * pcMBRecordListExt);
static  void    RefreshMBRecordList_All(HWND hList);
static  void    SrotMBRecordList(SORTTYPE SortType);
static  void    MBRecordList_Sort(HWND hWnd,HWND hList,SORTTYPE SortType);
static void		ShowDispItem(  HWND hWnd, char * number, char * dispdata, char * szdisp, BOOL finab, BOOL fidx);

extern  BOOL    ReadRecordFromData(MBRECORDTYPE CallRecType);
extern  BOOL    GetRecordFromData(CallRecord * pCallRecNode,int iDataIndex);
extern BOOL APP_SaveToPhoneBook(HWND hParent, const char* pPhoneNo);
extern void CloseMainWnd();
void SetMissCallIcon();
void CancelMissCallIcon();
BOOL GetDFlag();
int ClearRecentMissedCall();
int SaveRecentMissedCall(const char * number);
int FindNumberInRecentMissCall(const char * number);
BOOL RemoveSelectedWin();
BOOL IsSelected(int i);
BOOL IsMissedRecordWnd();
extern BOOL FindNumberInUnAnswerList(const char * number);
extern HINSTANCE GetAPPInstance();
extern void APP_EditMMS(HWND hWndFrame, HWND hWndPre, UINT msg, UINT nType,
                    PSTR  InsertFileName);
extern void ShowMainWnd();
extern void CloseDetailWnd(void);

BOOL GetDFlag()
{
	return Dflag;
}

HWND GetFrameHandle()
{
	return hPWnd;
}
static HWND hPrevWnd;
MBRECORDLIST_RS     CallMBLogListExtWindow(HWND hwndCall, HWND hMainWnd, int itype,MBRECORDLIST_STYLE MBRecordList_Style, BOOL flag)
{
//    HWND        hWnd;
    WNDCLASS    wc;
//    HMENU       hSubMenu,hMenu;
	HMENU hsubmenu, hmenu, hsubmenu2, hsubmenu3;
	RECT rClient;

	hPrevWnd = hMainWnd;
	hPWnd = hwndCall;
	Dflag = flag;
	if (Dflag == TRUE && (itype == DIALED_REC ||itype == UNANSWER_REC) && IsWindow(hRecordListWnd))
	{
		CloseDetailWnd();
		SendMessage(hRecordListWnd, WM_CLOSE, 0, 0);
	}
/*
	if (hPWnd == NULL)
	{
		hPWnd=CreateFrameWindow( WS_CAPTION |PWS_STATICBAR |WS_BORDER);
		if (hPWnd == NULL)
			return FALSE;
		else
			Dflag = TRUE;
		//		GetClientRect(hPWnd,&rClient);
	}
	else
		Dflag = FALSE;
*/
    wc.style         = CS_OWNDC;
    wc.lpfnWndProc   = AppWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = pClassName;

    if (!RegisterClass(&wc))
    {
        return FALSE;
    }

    MBL_Style       = MBRecordList_Style;
    iRecordListType = itype;
    BackValue       = MBRECORDLIST_RS_NONE;
    switch (itype)
    {
    case UNANSWER_REC:
        strcpy(cTitle,TITLECAPTION1);
		CancelMissCallIcon();
		ClearRecentMissedCall();
        break;
    case ANSWERED_REC:
        strcpy(cTitle,TITLECAPTION2);
        break;
    case DIALED_REC:
        strcpy(cTitle,TITLECAPTION3);
        break;
    case ANSWERED_REC | DIALED_REC:
        strcpy(cTitle,TITLECAPTION);
        break;
    case ANSWERED_REC | UNANSWER_REC:
        strcpy(cTitle,TITLECAPTION);
        break;
    case DIALED_REC | UNANSWER_REC:
        strcpy(cTitle,TITLECAPTION);
        break;
    case ANSWERED_REC | DIALED_REC | UNANSWER_REC:
        strcpy(cTitle,TITLECAPTION);
        break;
    default:
        return FALSE;
    }

	SendMessage(hPWnd,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDC_SAVE, 1),(LPARAM)ML("Save"));
	hmenu = CreateMenu();
	if (FALSE == PDASetMenu(hPWnd, hmenu))
	{
		DestroyMenu(hmenu);
		hmenu = NULL;
	}
	hsubmenu = CreateMenu();
	InsertMenu(hmenu   ,0 ,MF_BYPOSITION|MF_POPUP, (DWORD)hsubmenu ,(LPCSTR)IDS_MENU_SAVE);   
	InsertMenu(hsubmenu,0       ,MF_BYPOSITION        ,IDM_NEWCONTACT,(LPCSTR)IDS_MENU_NEWCONTACT); 
	InsertMenu(hsubmenu,1       ,MF_BYPOSITION        ,IDM_ADDTOCONTACT  ,(LPCSTR)IDS_MENU_ADDTOCONTACT); 
	hsubmenu2 = CreateMenu();
	InsertMenu(hmenu   ,1 ,MF_BYPOSITION|MF_POPUP, (DWORD)hsubmenu2 ,(LPCSTR)IDS_MENU_MESSAGE);   
	InsertMenu(hsubmenu2,0       ,MF_BYPOSITION        ,IDM_SMS,(LPCSTR)IDS_MENU_SMS); 
	InsertMenu(hsubmenu2,1       ,MF_BYPOSITION        ,IDM_MMS  ,(LPCSTR)IDS_MENU_MMS); 				
	InsertMenu(hmenu   ,2 ,MF_BYPOSITION, IDM_DETAILS ,(LPCSTR)IDS_MENU_DETAILS);   
	InsertMenu(hmenu, 3, MF_BYPOSITION, IDM_REMOVE, (LPSTR)IDS_REMOVE);
	hsubmenu3 = CreateMenu();
	InsertMenu(hmenu, 4, MF_BYPOSITION|MF_POPUP, (DWORD)hsubmenu3, (LPSTR)IDS_REMOVEMANY);
	InsertMenu(hsubmenu3, 0, MF_BYPOSITION, IDM_REMOVESELECTED, (LPSTR)IDS_SELECTED);
	InsertMenu(hsubmenu3, 1, MF_BYPOSITION, IDM_REMOVEALL, (LPSTR)IDS_ALL);

	SendMessage(hPWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Options"); 
	SendMessage(hPWnd, PWM_SETBUTTONTEXT, 0, IDS_EXIT); 
	GetClientRect(hPWnd, &rClient);
    hRecordListWnd = CreateWindow(pClassName,"", 
                WS_VISIBLE | WS_CHILD,
				rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
                hPWnd, NULL, NULL, NULL);
    if (NULL == hRecordListWnd)
    {
        UnregisterClass(pClassName,NULL);
        return FALSE;
    }

	ShowWindow(hPWnd, SW_SHOW); 
	UpdateWindow(hPWnd);  
//	if (hRecordList != NULL)
//		SetFocus(hRecordList);
//	else
		SetFocus(hRecordListWnd); 				
	return (BackValue);
}

static LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
    static      SORTTYPE        SortType;
    static      BOOL            bRefreshList;
                int             icur,i;
                LRESULT         lResult;
//                HMENU           hSubMenuGet,hSubMenuDel,hMenu;
                CallRecord      MBRecord;
                MBRECORDLISTEXT MBRecordListExt;
	HMENU hsubmenu, hmenu, hsubmenu2, hsubmenu3;


    lResult = (LRESULT)TRUE;
    switch (wMsgCmd)
    {
    case WM_CREATE :
		if (GetRecorderNumber(iRecordListType)>0)
		{
			InitMBRecorListExt();
			if (!CreateControl(hWnd))
			{
				AppMessageBox(NULL,ERROR1, cTitle, WAITTIMEOUT);
				return -1;
			}
//			SetFocus(hRecordList);
			//		hFocus = hRecordList;
			SortType     = iSORT_TIME;
			bRefreshList = FALSE;
			ReloadRecordList(hWnd,hRecordList,SortType);
			OldListProc = (WNDPROC)GetWindowLong(hRecordList, GWL_WNDPROC);
			SetWindowLong(hRecordList, GWL_WNDPROC, (long)NewListProc);

			if (Dflag)
				SendMessage(hPWnd,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDC_EXIT, 0),IDS_EXIT);
			//				SendMessage(hPWnd,PWM_SETBUTTONTEXT, 0, IDS_EXIT);
			else
				SendMessage(hPWnd,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDC_EXIT, 0),BUTTONRIGHT);

		}
		else
		{
			RECT rClient;
			InitMBRecorListExt();
			GetClientRect(hWnd, &rClient);
			hRecordList = CreateWindow(
				"LISTBOX",
				"",
				WS_VISIBLE | WS_CHILD|WS_VSCROLL | LBS_NOTIFY | LBS_BITMAP | LBS_MULTILINE,
				rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
				hWnd,
				(HMENU)IDC_LIST,
				NULL,
				NULL);
			if (hRecordList == NULL)
				return FALSE;
			ShowWindow(hRecordList, SW_HIDE);
			SetWindowText(hPWnd, cTitle);
			PDASetMenu(hPWnd, NULL);

			SendMessage(hPWnd,PWM_SETBUTTONTEXT,1,(LPARAM)"");
			SendMessage(hPWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)""); 
			//lanlan fix
			if (Dflag)
				SendMessage(hPWnd,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDC_EXIT, 0),IDS_EXIT);
//				SendMessage(hPWnd,PWM_SETBUTTONTEXT, 0, IDS_EXIT);
			else
				SendMessage(hPWnd,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDC_EXIT, 0),BUTTONRIGHT);
//				SendMessage(hPWnd,PWM_SETBUTTONTEXT, 0, BUTTONRIGHT);
			//end
			SendMessage(hPWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON), (LPARAM)NULL);
			SendMessage(hPWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON), (LPARAM)NULL);
		}
        break;

	case  WM_SETFOCUS:
		if (GetRecorderNumber(iRecordListType)>0)
		{
			if (NULL != hRecordList)
				SetFocus(hRecordList);
		}
		else
			SetFocus(hWnd);
		break;

/*
	case WM_INITMENU:
		icur        = SendMessage(hRecordList,LB_GETCURSEL,0,0);
		hMenu = PDAGetMenu(hPWnd);
		hSubMenuGet = GetSubMenu(hMenu,MENU_POPUP_PICKUP);
		hSubMenuDel = GetSubMenu(hMenu,MENU_POPUP_DELETE);
		if (-1 == icur)
		{
			EnableMenuItem(hMenu,IDM_VIEW,MF_GRAYED);
			EnableMenuItem(hMenu,IDM_SORT,MF_GRAYED);
			ModifyMenu(hMenu,MENU_POPUP_PICKUP,MF_BYPOSITION|MF_POPUP|MF_GRAYED,(DWORD)hSubMenuGet,MBRECORDMENUPICKUP);
			ModifyMenu(hMenu,MENU_POPUP_DELETE,MF_BYPOSITION|MF_POPUP|MF_GRAYED,(DWORD)hSubMenuDel,MBRECORDMENUDELETE);
		}
		else
		{
			EnableMenuItem(hMenu,IDM_VIEW,MF_ENABLED);
			EnableMenuItem(hMenu,IDM_SORT,MF_ENABLED);
			ModifyMenu(hMenu,MENU_POPUP_PICKUP,MF_BYPOSITION|MF_POPUP|MF_ENABLED,(DWORD)hSubMenuGet,MBRECORDMENUPICKUP);
			ModifyMenu(hMenu,MENU_POPUP_DELETE,MF_BYPOSITION|MF_POPUP|MF_ENABLED,(DWORD)hSubMenuDel,MBRECORDMENUDELETE);
		}
		break;
*/

	case PWM_SHOWWINDOW:
		if (GetRecorderNumber(iRecordListType)>0)
		{
//			Dflag = FALSE;
			//lanlan fix  [8/11/2005] 
			//SendMessage(hPWnd,PWM_SETBUTTONTEXT,1,(LPARAM)"");
			SendMessage(hPWnd,PWM_SETBUTTONTEXT,1,(LPARAM)IDS_MENU_SAVE);
//			SendMessage(hPWnd,PWM_CREATECAPTIONBUTTON,(WPARAM)IDC_BUTTON_RIGHT,BUTTONRIGHT);
			
			if (Dflag)
				SendMessage(hPWnd,PWM_SETBUTTONTEXT, 0, IDS_EXIT);
			else
				SendMessage(hPWnd,PWM_SETBUTTONTEXT, 0, BUTTONRIGHT);
			//end
			SendMessage(hPWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON), (LPARAM)NULL);
			SendMessage(hPWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON), (LPARAM)NULL);
			SetWindowText(hPWnd, cTitle);	
			hmenu = CreateMenu();
			if (FALSE == PDASetMenu(hPWnd, hmenu))
			{
				DestroyMenu(hmenu);
				hmenu = NULL;
			}
			hsubmenu = CreateMenu();
			InsertMenu(hmenu   ,0 ,MF_BYPOSITION|MF_POPUP, (DWORD)hsubmenu ,(LPCSTR)IDS_MENU_SAVE);   
			InsertMenu(hsubmenu,0       ,MF_BYPOSITION        ,IDM_NEWCONTACT,(LPCSTR)IDS_MENU_NEWCONTACT); 
			InsertMenu(hsubmenu,1       ,MF_BYPOSITION        ,IDM_ADDTOCONTACT  ,(LPCSTR)IDS_MENU_ADDTOCONTACT); 
			hsubmenu2 = CreateMenu();
			InsertMenu(hmenu   ,1 ,MF_BYPOSITION|MF_POPUP, (DWORD)hsubmenu2 ,(LPCSTR)IDS_MENU_MESSAGE);   
			InsertMenu(hsubmenu2,0       ,MF_BYPOSITION        ,IDM_SMS,(LPCSTR)IDS_MENU_SMS); 
			InsertMenu(hsubmenu2,1       ,MF_BYPOSITION        ,IDM_MMS  ,(LPCSTR)IDS_MENU_MMS); 				
			InsertMenu(hmenu   ,2 ,MF_BYPOSITION, IDM_DETAILS ,(LPCSTR)IDS_MENU_DETAILS);   
			InsertMenu(hmenu, 3, MF_BYPOSITION, IDM_REMOVE, (LPSTR)IDS_REMOVE);
			hsubmenu3 = CreateMenu();
			InsertMenu(hmenu, 4, MF_BYPOSITION|MF_POPUP, (DWORD)hsubmenu3, (LPSTR)IDS_REMOVEMANY);
			InsertMenu(hsubmenu3, 0, MF_BYPOSITION, IDM_REMOVESELECTED, (LPSTR)IDS_SELECTED);
			InsertMenu(hsubmenu3, 1, MF_BYPOSITION, IDM_REMOVEALL, (LPSTR)IDS_ALL);
			SendMessage(hPWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Options"); 
			//		SetFocus(hWnd); 
			//lanlan fix
			if (hRecordList != NULL)
			{
				ShowWindow(hRecordList, SW_SHOW);
				SetFocus(hRecordList);
			}
			else
				SetFocus(hRecordListWnd);
			// end [8/11/2005]
		}
		else
		{
//			Dflag = FALSE;
			SetWindowText(hPWnd, cTitle);
			PDASetMenu(hPWnd, NULL);
			SendMessage(hPWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)""); 
			SendMessage(hPWnd,PWM_SETBUTTONTEXT,1,(LPARAM)"");
			//lanlan fix
			if (Dflag)
				SendMessage(hPWnd,PWM_SETBUTTONTEXT, 0, IDS_EXIT);
			else
				SendMessage(hPWnd,PWM_SETBUTTONTEXT, 0, BUTTONRIGHT);
			//end
			SendMessage(hPWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON), (LPARAM)NULL);
			SendMessage(hPWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON), (LPARAM)NULL);
			if (hRecordList != NULL)
			{
				HWND hTWnd;
				ShowWindow(hRecordList, SW_HIDE);
				hTWnd = GetFocus();
				SetFocus(hRecordListWnd);
//				ShowWindow(hRecordListWnd, SW_SHOW);
//				ShowWindow(hPrevWnd, SW_HIDE);
//				InvalidateRect(hRecordListWnd, NULL, 1);
			}
			//lanlan add
			else
			{
				SetFocus(hRecordListWnd);
			}
			
		}
		break;

//    case WM_ACTIVATE:
//        if (WA_INACTIVE != LOWORD(wParam))
//        {
//            if (UNANSWER_REC & iRecordListType)
//            {
//                ResetnUnanswered();
//            }
//            if (bRefreshList)
//            {
//                PostMessage(hWnd,IDC_REFRESHRECORDLIST,0,0);
//            }
//            else
//            {
//				SetFocus(hFocus);
//				if (hFocus == hRecordList) 
//					RefreshMBRecordList_All(hFocus);
//            }
//        }
//		else
//			hFocus = GetFocus();
//        break;

	case WM_PAINT:
		{
			HDC hdc;
			RECT rClient;
			int oldbk;
			hdc = BeginPaint( hWnd, NULL);
			if(IsWindowVisible(hWnd) == TRUE)
			{
				
				if (GetRecorderNumber(iRecordListType) == 0) 
				{
					char szinfo[TITLECAPTIONMAXLEN];
					szinfo[0] = 0;
					switch (iRecordListType)
					{
					case UNANSWER_REC:
						strcpy(szinfo,NOTIFY_EMPTYMISSEDCALLS);
						break;
					case ANSWERED_REC:
						strcpy(szinfo,NOTIFY_EMPTYRECEIVEDCALLS);
						break;
					case DIALED_REC:
						strcpy(szinfo,NOTIFY_EMPTYDIALEDCALLS);
						break;
					}
					GetClientRect(hWnd, &rClient);
					oldbk = SetBkMode(hdc, BM_TRANSPARENT);
					DrawText(hdc, szinfo, strlen(szinfo), &rClient, DT_HCENTER|DT_VCENTER);
					SetBkMode(hdc, oldbk);
					//				TextOut(hdc, 40, 100, ML("No Record"), -1);
				}
			}
			EndPaint(hWnd, NULL);
		}
		break;
    case WM_CLOSE:
		DestroyWindow(hWnd);
        break;

    case WM_DESTROY :		
 		bInit = FALSE;
		if(iRecordListType == UNANSWER_REC)
		{
			CancelMissCallIcon();
			ClearRecentMissedCall();
			PostMessage(gwndinfo.hInfoWnd, gwndinfo.wMsg, 0,0);
		}
		DestroyMenu(PDAGetMenu(hPWnd));
/*
				if (Dflag == TRUE)
				{
					hPWnd =NULL;
					Dflag = FALSE;
				}
				else*/
		
//	    Dflag = FALSE;
		hRecordList = NULL;
        hRecordListWnd = NULL;
		hFocus = NULL;
        DestroyRecordListExt();
		DeleteObject(hIconIn);
		DeleteObject(hIconOut);
		DeleteObject(hIconUnans);
        UnregisterClass(pClassName,NULL);

		if (Dflag)
		{
			hInstance = GetAPPInstance();
			DlmNotify ((WPARAM)PES_STCQUIT, (LPARAM)hInstance );
        }

		break;

    case WM_TODESKTOP:
        DestroyWindow(hWnd);
        return TRUE;

	case WM_CONTACTUPDATE:
		{

			if (TRUE == wParam)
			{
				icur = SendMessage(hRecordList,LB_GETCURSEL,0,0);
				if (-1 == icur)
					break;
				if (GetMBRecordListExtNode(&MBRecordListExt,icur))
					RefreshMBRecordList_One(hRecordList,&MBRecordListExt);
			}
			
		}
		break;

	case WM_KEYDOWN:
		switch(LOWORD(wParam))
		{
		case VK_F10:
			if (Dflag)
			{	
				SendMessage(hPWnd, WM_CLOSE, 0, 0);
			}
			else
			{
				SendMessage(hPWnd,PWM_CLOSEWINDOW,  (WPARAM) hWnd, 0);			
				SendMessage(hWnd, WM_CLOSE, 0,0);
				ShowMainWnd();
			}
			break;

		case VK_F5:
			SendMessage(hPWnd, WM_KEYDOWN, wParam, lParam);
			break;

		//lanlan fix
		case VK_RETURN:
			{
				char	ptr[20];
				ptr[0] = NULL;
				SendMessage(hPWnd, PWM_GETBUTTONTEXT, 1, (LPARAM)ptr);
				if (strlen(ptr) > 1 ) {
					PostMessage(hWnd, WM_COMMAND, IDM_NEWCONTACT, 0);
				}
			}
			break;
		//end

		default:
			lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
			break;
		}
		break;    

    case WM_COMMAND:
        switch( LOWORD( wParam ))
        {
        case IDC_LIST:
            switch (HIWORD(wParam))
            {
            case LBN_DBLCLK:
                SendMessage(hWnd,WM_COMMAND,IDM_DETAILS,0);
                break;
            case LBN_SELCHANGE:
                SetTitle(hWnd,hRecordList);
                break;
            }
            break;
        case IDM_DETAILS:
            icur = SendMessage(hRecordList,LB_GETCURSEL,0,0);
            if (-1 == icur)
                break;
            if (GetMBRecordListExtNode(&MBRecordListExt,icur))
            {
                MBRECORD_VIEW MBRecord_View;
                MBRECORD_RS MBRecord_RS;
                int iSour;
				char szTitleDisp[40];

				szTitleDisp[0] = 0;
                MBRecord_View.hWnd          = hPWnd;
                MBRecord_View.index         = MBRecordListExt.iMBRecordIndex;
                MBRecord_View.MBRecordType  = MBRecordListExt.iMBRecordType;

//				iSour = SendMessage(hRecordList, LB_GETTEXTLEN, icur, 0);
				iSour = SendMessage(hRecordList, LB_GETTEXT, icur, (LPARAM)szTitleDisp);
               strncpy(MBRecord_View.TitleInfo.cTitle, szTitleDisp,
                    sizeof(MBRecord_View.TitleInfo.cTitle) > iSour ? iSour + 1 : sizeof(MBRecord_View.TitleInfo.cTitle) - 1);

                iSour = strlen(IDS_MBRECORD_NUM);
                strncpy(MBRecord_View.TitleInfo.cNumber,IDS_MBRECORD_NUM,
                    sizeof(MBRecord_View.TitleInfo.cNumber) > iSour ? iSour + 1 : sizeof(MBRecord_View.TitleInfo.cNumber) - 1);

                iSour = strlen(IDS_MBRECORD_NAME);
                strncpy(MBRecord_View.TitleInfo.cName,IDS_MBRECORD_NAME,
                    sizeof(MBRecord_View.TitleInfo.cName) > iSour ? iSour + 1 : sizeof(MBRecord_View.TitleInfo.cName) - 1);

                iSour = strlen(IDS_MBRECORD_SHIKE);
                strncpy(MBRecord_View.TitleInfo.cTime,IDS_MBRECORD_SHIKE,
                    sizeof(MBRecord_View.TitleInfo.cTime) > iSour ? iSour + 1 : sizeof(MBRecord_View.TitleInfo.cTime) - 1);

                iSour = strlen(IDS_MBRECORD_DURATION);
                strncpy(MBRecord_View.TitleInfo.cDuration,IDS_MBRECORD_DURATION,
                    sizeof(MBRecord_View.TitleInfo.cDuration) > iSour ? iSour + 1 : sizeof(MBRecord_View.TitleInfo.cDuration) - 1);

                MBRecord_View.ItemStyle = MBV_WS_NUMBER | MBV_WS_NAME | MBV_WS_TIME | MBV_WS_DURATION;
                MBRecord_View.TimeStyle = TS_NORMAL;

                iSour = strlen(IDS_MBRECORD_UNKNOWN);
                strncpy(MBRecord_View.cUnknow,IDS_MBRECORD_UNKNOWN,
                    sizeof(MBRecord_View.cUnknow) > iSour ? iSour + 1 : sizeof(MBRecord_View.cUnknow) - 1);

                bRefreshList = FALSE;
                
                MBRecord_RS = CallMBLogViewWindow(&MBRecord_View);

/*
                switch (MBRecord_RS)
                {
                case MBRECORD_RS_MODIFY:
                    RefreshMBRecordList_One(hRecordList,&MBRecordListExt);
                    break;
                case MBRECORD_RS_DIALUP:
                    if (iRecordListType & DIALED_REC)
                        PostMessage(hWnd,IDC_REFRESHRECORDLIST,0,0);
                    break;
                case MBRECORD_RS_DELETE:
                    PostMessage(hWnd,IDC_REFRESHRECORDLIST,0,0);
                    break;
                }
*/
            }
            break;
        case IDM_DIALRECORD:
            icur = SendMessage(hRecordList,LB_GETCURSEL,0,0);
            if (-1 == icur)
                break;
            if (GetMBRecordListExtNode(&MBRecordListExt,icur))
            {
                GetMBRecord(&MBRecord,sizeof(CallRecord),
                    MBRecordListExt.iMBRecordType,MBRecordListExt.iMBRecordIndex);

                APP_CallPhoneNumber(MBRecord.PhoneNo);

                if (iRecordListType & DIALED_REC)
                    bRefreshList = TRUE;
            }
            break;
        case IDM_NEWCONTACT:
			{
				ABNAMEOREMAIL abnameinfo;

				memset(&abnameinfo, 0, sizeof(ABNAMEOREMAIL));
				icur = SendMessage(hRecordList,LB_GETCURSEL,0,0);
				if (-1 == icur)
					break;
				if (GetMBRecordListExtNode(&MBRecordListExt,icur))
				{
					GetMBRecord(&MBRecord,sizeof(CallRecord),
						MBRecordListExt.iMBRecordType,MBRecordListExt.iMBRecordIndex);
					abnameinfo.nType = AB_NUMBER;
					strcpy(abnameinfo.szTelOrEmail, MBRecord.PhoneNo);
					if (!APP_SaveToAddressBook(hPWnd, hWnd, WM_CONTACTUPDATE, &abnameinfo, AB_NEW))
					{   
						PLXTipsWin(hPWnd, hWnd, 0, NOTIFY_UNSUCCESSFUL, NULL, Notify_Failure, ML("Ok"), NULL, WAITTIMEOUT);
//						RefreshMBRecordList_One(hRecordList,&MBRecordListExt);
					}
				}
			}		
            break;

		case IDM_ADDTOCONTACT:
			{
				ABNAMEOREMAIL abnameinfo;

				memset(&abnameinfo, 0, sizeof(ABNAMEOREMAIL));
				icur = SendMessage(hRecordList,LB_GETCURSEL,0,0);
				if (-1 == icur)
					break;
				if (GetMBRecordListExtNode(&MBRecordListExt,icur))
				{
					GetMBRecord(&MBRecord,sizeof(CallRecord),
						MBRecordListExt.iMBRecordType,MBRecordListExt.iMBRecordIndex);
					abnameinfo.nType = AB_NUMBER;
					strcpy(abnameinfo.szTelOrEmail, MBRecord.PhoneNo);
					if (!APP_SaveToAddressBook(hPWnd, hWnd, WM_CONTACTUPDATE, &abnameinfo, AB_UPDATE))
					{   
						PLXTipsWin(hPWnd, hWnd, 0, NOTIFY_UNSUCCESSFUL, NULL, Notify_Failure, ML("Ok"), NULL, WAITTIMEOUT);
//						RefreshMBRecordList_One(hRecordList,&MBRecordListExt);
					}
				}
			}
			break;

        case IDM_SMS:
            icur = SendMessage(hRecordList,LB_GETCURSEL,0,0);
            if (-1 == icur)
                break;
            if (GetMBRecordListExtNode(&MBRecordListExt,icur))
            {
                GetMBRecord(&MBRecord,sizeof(CallRecord),
                    MBRecordListExt.iMBRecordType,MBRecordListExt.iMBRecordIndex);

                APP_EditSMS(hPWnd, MBRecord.PhoneNo, NULL);
            }
            break;

        case IDM_MMS:
            icur = SendMessage(hRecordList,LB_GETCURSEL,0,0);
            if (-1 == icur)
                break;
            if (GetMBRecordListExtNode(&MBRecordListExt,icur))
            {
                GetMBRecord(&MBRecord,sizeof(CallRecord),
                    MBRecordListExt.iMBRecordType,MBRecordListExt.iMBRecordIndex);

                APP_EditMMS(hPWnd,NULL,0, MMS_CALLEDIT_MOBIL, MBRecord.PhoneNo);
            }
            break;

        case IDM_REMOVE:
			{
				char sznotify[40];
				sznotify[0] = 0;
				/*
				icur = SendMessage(hRecordList,LB_GETCURSEL,0,0);
				if (-1 == icur)
					break;
					*/
				bRefreshList = FALSE;
				switch(iRecordListType)
				{
				case UNANSWER_REC:
					strcpy(sznotify, NOTIFY_REMOVEMISSCALL);
					break;
				case ANSWERED_REC:
					strcpy(sznotify, NOTIFY_REMOVERECIEVEDCALL);
					break;
				case DIALED_REC:
					strcpy(sznotify, NOTIFY_REMOVEDIALEDCALL);
					break;
				}
//				if (AppConfirmWin(hWnd,sznotify,"",cTitle, CONFIRM_OK,CONFIRM_CANCEL))
				PLXConfirmWinEx(hPWnd, hWnd, sznotify, Notify_Request, NULL, ML("Yes"),ML("No"), IDRM_ISREMOVE);
				
			}
            break;
        case IDM_REMOVEALL:
			{
				char sznotify[40];
				sznotify[0] = 0;
				switch(iRecordListType)
				{
				case UNANSWER_REC:
					strcpy(sznotify, NOTIFY_REMOVEMISSCALLS);
					break;
				case ANSWERED_REC:
					strcpy(sznotify, NOTIFY_REMOVERECIEVEDCALLS);
					break;
				case DIALED_REC:
					strcpy(sznotify, NOTIFY_REMOVEDIALEDCALLS);
					break;
				}
				bRefreshList = FALSE;            
//				if (AppConfirmWin(hWnd,sznotify,"",cTitle, CONFIRM_OK,CONFIRM_CANCEL))
				PLXConfirmWinEx(hPWnd, hWnd,sznotify, Notify_Request, NULL, ML("Yes"),ML("No"), IDRM_ISRMALL);
				
			}
            break;
            
		case  IDM_REMOVESELECTED:
			RemoveSelectedWin();
			break;
/*
        case IDM_SORT:
            hMenu = GetMenu(hWnd);
            if (iSORT_TIME == SortType)
            {
                
                MBRecordList_Sort(hWnd,hRecordList,iSORT_NAME);
                ModifyMenu(hMenu,IDM_SORT,MF_BYCOMMAND|MF_ENABLED,
                    IDM_SORT,MBRECORDMENUSORTTIME);

                SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDM_SORT,2),(LPARAM)MBRECORDMENUSORTTIME);
                SortType = iSORT_NAME;
            }
            else
            {
                
                MBRecordList_Sort(hWnd,hRecordList,iSORT_TIME);
                ModifyMenu(hMenu,IDM_SORT,MF_BYCOMMAND|MF_ENABLED,
                    IDM_SORT,MBRECORDMENUSORTNAME);

                SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDM_SORT,2),(LPARAM)MBRECORDMENUSORTNAME);
                SortType = iSORT_TIME;
            }
            SetFocus(hRecordList);
            break;*/
			
        }
        break;

    case IDC_REFRESHRECORDLIST:
        ReloadRecordList(hWnd,hRecordList,SortType);
        BackValue = MBRECORDLIST_RS_REFRESH;
        break;

	case IDRM_ISREMOVE:
		if (lParam ==1) 
		{
			icur = SendMessage(hRecordList,LB_GETCURSEL,0,0);
			if (-1 == icur)
				break;
			if (GetMBRecordListExtNode(&MBRecordListExt,icur))
			{
				if (!DeleteMBRecord(MBRecordListExt.iMBRecordType,
					MBRecordListExt.iMBRecordIndex))
					//							AppMessageBox(hWnd,MBRECORDLISTDELFAIL,cTitle, WAITTIMEOUT);
					PLXTipsWin(hPWnd, hWnd, 0, MBRECORDLISTDELFAIL, NULL, Notify_Failure, ML("Ok"), NULL, WAITTIMEOUT);
				else
				{
					PLXTipsWin(hPWnd, hWnd, 0, NOTIFY_REMOVED, NULL, Notify_Success, ML("Ok"), NULL, WAITTIMEOUT);
					//							AppMessageBox(hWnd,NOTIFY_REMOVED,cTitle, WAITTIMEOUT);
					PostMessage(hWnd,IDC_REFRESHRECORDLIST,1,0);
				}
			}
			else
			{
				//						AppMessageBox(hWnd,MBRECORDLISTDELFAIL,cTitle, WAITTIMEOUT);
				PLXTipsWin(hPWnd, hWnd, 0, MBRECORDLISTDELFAIL, NULL, Notify_Failure, ML("Ok"), NULL, WAITTIMEOUT);
			}
		}
		break;

	case IDRM_ISRMALL:
		if (lParam ==1) {
			
			for (i = 0;i < iRecordTypeCount;i++)
			{
				if (iRecordListType & iMBRecords[i])
				{
					if (!DeleteMBRecord(iMBRecords[i],-1))
					{
						//								AppMessageBox(hWnd,MBRECORDLISTDELFAIL,cTitle, WAITTIMEOUT);
						PLXTipsWin(hPWnd, hWnd, 0, MBRECORDLISTDELFAIL, NULL, Notify_Failure, ML("Ok"), NULL, WAITTIMEOUT);
						break;
					}
				}
			}
			//					AppMessageBox(hWnd,NOTIFY_REMOVED,cTitle, WAITTIMEOUT);
			PLXTipsWin(hPWnd, hWnd, 0, NOTIFY_REMOVED, NULL, Notify_Success, ML("Ok"), NULL, WAITTIMEOUT);
			PostMessage(hWnd,IDC_REFRESHRECORDLIST,1,0);
		}
		break;

    default :
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
    return lResult;
}

void    MBRecrodList_Refresh(MBRECORDTYPE MBRecord_Type)
{
    if (IsWindow(hRecordListWnd))
    {
        if (iRecordListType & MBRecord_Type)
            PostMessage(hRecordListWnd,IDC_REFRESHRECORDLIST,0,0);
    }
	PostMessage(gwndinfo.hInfoWnd, gwndinfo.wMsg, 0,0);
}

static  void    InitMBRecorListExt(void)
{
    SIZE        size;
    COLORREF    Color;
    BOOL        bGif;
    HDC         hdc;

    if (bInit)
        return;

    GetImageDimensionFromFile(pIconInName,&size);
    hdc         = CreateMemoryDC(size.cx,size.cy);
    hIconIn     = CreateBitmapFromImageFile(hdc,pIconInName ,&Color,&bGif);
    hIconOut    = CreateBitmapFromImageFile(hdc,pIconOutName,&Color,&bGif);
    hIconUnans  = CreateBitmapFromImageFile(hdc,pIconUnName ,&Color,&bGif);
    DeleteObject(hdc);

    bInit = TRUE;
}

static  void    SetTitle(HWND hWnd,HWND hList)
{
    if (MBL_Style & MBL_WS_COUNT)
    {
        int icur,iCount;
        char title[TITLECAPTIONMAXLEN + 1] = "";

        icur   = SendMessage(hList,LB_GETCURSEL,0,0);
        iCount = SendMessage(hList,LB_GETCOUNT ,0,0);

        sprintf(title,pSetTitle,cTitle,icur + 1,iCount);
        SetWindowText(hWnd,title);
    }
}

static  BOOL    GetMBRecordListExtNode(MBRECORDLISTEXT * pNode,int iIndex)
{
    int i = 0,iCount;
    MBRECORDLISTEXT * p = pMBRecordListExt;

    if (NULL == pMBRecordListExt)
        return FALSE;
    iCount = GetRecordListCount();

    if ((0 == iCount) || (iIndex < 0) || (iIndex > iCount - 1))
        return FALSE;

    while (p != NULL)
    {
        if (i < iIndex)
        {
            p = p->pNext;
            i++;
        }
        else
        {
            memcpy(pNode,p,sizeof(MBRECORDLISTEXT));
            return TRUE;
        }
    }
    return FALSE;
}

extern BOOL GetTimeDisplay(SYSTEMTIME st, char* cTime, char* cDate);
#define MAX_ITEMLEN  30
static  void    ReloadRecordList(HWND hWnd,HWND hList,SORTTYPE SortType)
{
    int   index;
    char  cDisplayData[PHONENUMMAXLEN + 1] = "";
	char szDisp[PHONENUMMAXLEN + 1] = "";
    MBRECORDLISTEXT * p;
	char  cAuxdspdata[MAX_ITEMLEN] = "";
	char  cTime[15] = "";
	char  cData[15] = "";
	int len, len1;

    LoadMBRecordToLink(SortType);
    SendMessage(hRecordList,LB_RESETCONTENT,0,0);

    p = pMBRecordListExt;
    while (p != NULL)
    {
		strcpy(cDisplayData,p->cDisplay);
/*
        if (p->bInPhonebook)
        {
            strcpy(cDisplayData,p->cDisplay);
        }
        else
        {
            if ((int)strlen(p->cDisplay) > iViewMaxLen)
            {
                strncpy(cDisplayData,p->cDisplay,iViewMaxLen);
                strcat(cDisplayData,pCHOMIT);
            }
            else
            {
                strcpy(cDisplayData,p->cDisplay);
            }
        }*/

/*
		rindex = FindNumberInRecentMissCall(p->cNumber);
		if ( rindex != -1)
		{
			sprintf(szDisp,"%s (%d)", cDisplayData, pRecentMissCall[rindex].count);
		}
		else
			strcpy(szDisp, cDisplayData);*/
		ShowDispItem(hWnd, p->cNumber, cDisplayData, szDisp, p->bInPhonebook, TRUE);

//        index = SendMessage(hRecordList,LB_ADDSTRING,0,(LPARAM)cDisplayData);
        index = SendMessage(hRecordList,LB_ADDSTRING,0,(LPARAM)szDisp);
		GetTimeDisplay(p->Time, cTime, cData);
		len = strlen(cData);
		len1 = strlen(cTime);
		if (len+len1+1> MAX_ITEMLEN) 
			return ;
		sprintf(cAuxdspdata,"%s %s", cData, cTime);
/*
		strncpy(cAuxdspdata, cData, len);
		strcat(cAuxdspdata, cTime);
*/
		cAuxdspdata[len+len1+1] = 0;
		SendMessage(hRecordList, LB_SETAUXTEXT, MAKEWPARAM(index, MAX_ITEMLEN),  (LPARAM)cAuxdspdata);
        SetLIstBoxIcon(hRecordList,index,p->iMBRecordType);
        p = p->pNext;
    }

    SendMessage(hRecordList,LB_SETCURSEL,0,0);

/*
        if (SendMessage(hList,LB_GETCOUNT,0,0) == 0)
            SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDM_SORT,2),(LPARAM)"");
        else
            SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDM_SORT,2),(LPARAM)MBRECORDMENUSORTNAME);
    
        SetTitle(hWnd,hList);*/
    
}

#define  MAX_RECENTCOUNTER_LEN 5
#define  MAX_RECENTCOUNTER   100
#define  ITEMICON_LEN  90  // will  get from listbox in next version
#define  LINE_WIDTH    176
#define  LINE_TXT_WIDTH  136

static void ShowDispItem(  HWND hWnd, char * number, char * dispdata, char * szdisp, BOOL finab, BOOL fidx)
{
	int rindex;
	char szcounter[MAX_RECENTCOUNTER_LEN+1];
	HDC hdc;
	HFONT hfont, holdfont;
	SIZE size;
	int fitwidth;
	BOOL fend;

	fend = !(finab);
	szcounter[0] = 0;
	if (fidx == TRUE)
		rindex = FindNumberInRecentMissCall(number);
	else
		rindex = -1;
	if (rindex != -1)
	{
		if (pRecentMissCall[rindex].count > 1 && pRecentMissCall[rindex].count< MAX_RECENTCOUNTER)
		{
			sprintf(szcounter, " (%d)", pRecentMissCall[rindex].count);
			hdc = GetDC(hWnd);
			if (GetFontHandle(&hfont, 1))
			{
				holdfont = SelectObject(hdc, hfont);
			}
			GetTextExtentPoint32(hdc, szcounter, strlen(szcounter), &size);
			SelectObject(hdc, holdfont);
			fitwidth = LINE_TXT_WIDTH -size.cx;
			InitTxt(hdc, dispdata, szdisp, fitwidth, fend);			
			ReleaseDC(hWnd, hdc);    
			strcat(szdisp,szcounter);
		}
		else
		{
			fitwidth = LINE_TXT_WIDTH;
			hdc = GetDC(hWnd);
			InitTxt(hdc, dispdata, szdisp, fitwidth, fend);
			ReleaseDC(hWnd, hdc);
		}
	}
	else
	{
		fitwidth = LINE_TXT_WIDTH;
		hdc = GetDC(hWnd);
		InitTxt(hdc, dispdata, szdisp, fitwidth, fend);
		ReleaseDC(hWnd, hdc);
	}
	return;
}

static  BOOL    LoadMBRecordToLink(SORTTYPE SortType)
{
    int         i,j,index;
    CallRecord  MBRecords;
    ABINFO      pbNameIcon;
    char        cDisplayData[PHONENUMMAXLEN + 1] = "";
    BOOL        bInAddress;

    DestroyRecordListExt();
    for (j = 0;j < iRecordTypeCount;j++)
    {
        if (iRecordListType & iMBRecords[j])
        {
            
            if (!ReadRecordFromData(iMBRecords[j]))
                continue;

            for (i = 0;i < MAX_PHONE_RECORD;i++)
            {
                memset(&MBRecords,0x00,sizeof(MBRecords));

                if (!GetRecordFromData(&MBRecords,i))
                    continue;
                if (strlen(MBRecords.PhoneNo) == 0)
                    break;

                memset(&pbNameIcon,0x00,sizeof(ABINFO));
                if (MB_GetInfoFromPhonebook(MBRecords.PhoneNo,&pbNameIcon))
                {
                    strcpy(cDisplayData,pbNameIcon.szName);
                    bInAddress = TRUE;
                }
                else
                {
                    strcpy(cDisplayData,MBRecords.PhoneNo);
                    bInAddress = FALSE;
                }
                index = AddNodeMBRecordListExt(iMBRecords[j],i,bInAddress,
                                                &MBRecords.Time,cDisplayData,
                                                MBRecords.PhoneNo,SortType);
            }
        }
    }
    return TRUE;
}

static  int     AddNodeMBRecordListExt(MBRECORDTYPE iMBRecordType,int iMBRecordIndex,
                                       BOOL bInPhonebook,const SYSTEMTIME * pTime,
                                       const char * pDisplay,const char * pNumber,
                                       SORTTYPE SortType)
{
    MBRECORDLISTEXT * p = NULL,* pCur = NULL,* pCurPre = NULL;
    int iSour,iCount = 0,index;

    if ((pDisplay == NULL) || (pTime == NULL))
        return -1;

    p = (MBRECORDLISTEXT *)malloc(sizeof(MBRECORDLISTEXT));
    if (p == NULL)
        return -1;

    memset(p,0x00,sizeof(MBRECORDLISTEXT));

    iSour = strlen(pDisplay);
    strncpy(p->cDisplay,pDisplay,sizeof(p->cDisplay) > iSour ? iSour + 1: sizeof(p->cDisplay) - 1);

    iSour = strlen(pNumber);
    strncpy(p->cNumber,pNumber,sizeof(p->cNumber) > iSour ? iSour + 1 : sizeof(p->cNumber) - 1);

    p->iMBRecordType    = iMBRecordType;        
    p->iMBRecordIndex   = iMBRecordIndex;       
    p->bInPhonebook     = bInPhonebook;         
    memcpy(&p->Time,pTime,sizeof(SYSTEMTIME));  

    iCount = GetRecordListCount();
    index  = 0;
    if (iCount == 0)
    {
        pMBRecordListExt = p;
        p->pNext         = NULL;
        return (index);
    }
    if (iCount == 1)
    {
        if (iSORT_TIME == SortType)
        {
            if (TimeCmp(&p->Time,&pMBRecordListExt->Time) < 0)
            {
                
                pMBRecordListExt->pNext = p;
                p->pNext                = NULL;
                index++;
            }
            else
            {
                p->pNext         = pMBRecordListExt;
                pMBRecordListExt = p;
            }
        }
        else if (iSORT_NAME == SortType)
        {
            if (strcmp(p->cDisplay,pMBRecordListExt->cDisplay) > 0)
            {
                
                pMBRecordListExt->pNext = p;
                p->pNext                = NULL;
                index++;
            }
            else
            {
                p->pNext         = pMBRecordListExt;
                pMBRecordListExt = p;
            }
        }
        return (index);
    }    
    pCur    = pMBRecordListExt;
    pCurPre = NULL;
    while (pCur != NULL)
    {
        if (iSORT_TIME == SortType)
        {
            if (TimeCmp(&p->Time,&pCur->Time) < 0)
            {
                
                pCurPre = pCur;
                pCur    = pCur->pNext;
                index++;
            }
            else
            {
                if (NULL == pCurPre)
                {
                    p->pNext         = pMBRecordListExt;
                    pMBRecordListExt = p;
                }
                else
                {
                    p->pNext        = pCur;
                    pCurPre->pNext  = p;
                }
                return (index);
            }
        }
        else if (iSORT_NAME == SortType)
        {
            if (strcmp(p->cDisplay,pCur->cDisplay) > 0)
            {
                
                pCurPre = pCur;
                pCur    = pCur->pNext;
                index++;
            }
            else
            {
                if (NULL == pCurPre)
                {
                    p->pNext         = pMBRecordListExt;
                    pMBRecordListExt = p;
                }
                else
                {
                    p->pNext        = pCur;
                    pCurPre->pNext  = p;
                }
                return (index);
            }
        }
    }    
    pCurPre->pNext = p;
    p->pNext       = NULL;
    return (index);
}

static  void    DestroyRecordListExt(void)
{
    MBRECORDLISTEXT * p = NULL;

    while (pMBRecordListExt != NULL)
    {
        p                = pMBRecordListExt;
        pMBRecordListExt = pMBRecordListExt->pNext;
        free(p);
    }
}

static  int     GetRecordListCount(void)
{
    int iCount = 0;
    MBRECORDLISTEXT * p = pMBRecordListExt;

    while (p != NULL)
    {
        p = p->pNext;
        iCount++;
    }
    return (iCount);
}

static  void    MBRecordList_Sort(HWND hWnd,HWND hList,SORTTYPE SortType)
{
    MBRECORDLISTEXT * p;
    char cDisplayData[PHONENUMMAXLEN + 1] = "";
    int index;

    SrotMBRecordList(SortType);
    SendMessage(hList,LB_RESETCONTENT,0,0);

    p = pMBRecordListExt;
    while (p != NULL)
    {
        if (p->bInPhonebook)
        {
            strcpy(cDisplayData,p->cDisplay);
        }
        else
        {
            if ((int)strlen(p->cDisplay) > iViewMaxLen)
            {
                strncpy(cDisplayData,p->cDisplay,iViewMaxLen);
                strcat(cDisplayData,pCHOMIT);
            }
            else
            {
                strcpy(cDisplayData,p->cDisplay);
            }
        }
        index = SendMessage(hList,LB_ADDSTRING,0,(LPARAM)cDisplayData);
        SetLIstBoxIcon(hList,index,p->iMBRecordType);
        p = p->pNext;
    }
    SendMessage(hList,LB_SETCURSEL,0,0);
    SetTitle(hWnd,hList);
}

static  int     iGetNewMBRecrodListCount(MBRECORDLISTEXT * pNew)
{
    int iCount = 0;
    MBRECORDLISTEXT * p = pNew;
    while (p != NULL)
    {
        p = p->pNext;
        iCount++;
    }
    return (iCount);
}

static  void    SrotMBRecordList(SORTTYPE SortType)
{
    BOOL bInsert;
    int iCount;
    MBRECORDLISTEXT * pNew = NULL;
    MBRECORDLISTEXT * pOld = pMBRecordListExt;
    MBRECORDLISTEXT * pCur = NULL,* pCurPre = NULL,* p = NULL;

    while (pOld != NULL)
    {
        iCount = iGetNewMBRecrodListCount(pNew);
        p      = pOld;
        pOld   = pOld->pNext;

        if (0 == iCount)
        {
            pNew = p;
            pNew->pNext = NULL;
            continue;
        }
        if (1 == iCount)
        {
            switch (SortType)
            {
            case iSORT_TIME:
                if (TimeCmp(&p->Time,&pNew->Time) < 0)
                {   
                    pNew->pNext = p;
                    p->pNext = NULL;
                }
                else
                {
                    p->pNext = pNew;
                    pNew     = p;
                }
                break;
            case iSORT_NAME:
                if (strcmp(p->cDisplay,pNew->cDisplay) > 0)
                {   
                    pNew->pNext = p;
                    p->pNext = NULL;
                }
                else
                {
                    p->pNext = pNew;
                    pNew     = p;
                }
                break;
            }
            continue;
        }
        
        bInsert = FALSE;
        pCur    = pNew;
        pCurPre = NULL;
        while (pCur != NULL)
        {
            if (iSORT_TIME == SortType)
            {
                if (TimeCmp(&p->Time,&pCur->Time) < 0)
                {
                    
                    pCurPre = pCur;
                    pCur    = pCur->pNext;
                    continue;
                }
                else
                {
                    if (NULL == pCurPre)
                    {
                        p->pNext = pNew;
                        pNew     = p;
                    }
                    else
                    {
                        p->pNext       = pCur;
                        pCurPre->pNext = p;
                    }
                    bInsert = TRUE;
                    break;
                }
            }
            else if (iSORT_NAME == SortType)
            {
                if (strcmp(p->cDisplay,pCur->cDisplay) > 0)
                {
                    
                    pCurPre = pCur;
                    pCur    = pCur->pNext;                    
                    continue;
                }
                else
                {
                    if (NULL == pCurPre)
                    {
                        p->pNext = pNew;
                        pNew     = p;
                    }
                    else
                    {
                        p->pNext       = pCur;
                        pCurPre->pNext = p;
                    }
                    bInsert = TRUE;
                    break;
                }
            }
        }
        if (!bInsert)
        {
            
            pCurPre->pNext = p;
            p->pNext       = NULL;
        }
    }
    pMBRecordListExt = pNew;
}

static  void    RefreshMBRecordList_All(HWND hList)
{
    MBRECORDLISTEXT * p;
    ABINFO pbNameIcon;
    int    iSour,i = 0,index,iOldCur;
    BOOL   bModify;
    char   cDisplayData[PHONENUMMAXLEN + 1] = "";
	char  cAuxdspdata[MAX_ITEMLEN] = "";
	char  cTime[15] = "";
	char  cData[15] = "";
	int len, len1;

    iOldCur = SendMessage(hList,LB_GETCURSEL,0,0);
    p       = pMBRecordListExt;

    while (p != NULL)
    {
        memset(&pbNameIcon,0x00,sizeof(ABINFO));
        if (MB_GetInfoFromPhonebook(p->cNumber,&pbNameIcon))
        {
            if (p->bInPhonebook)
            {
                if (strcmp(p->cDisplay,pbNameIcon.szName) != 0)
                {
                    iSour = strlen(pbNameIcon.szName);
                    strncpy(p->cDisplay,pbNameIcon.szName,
                        sizeof(p->cDisplay) > iSour ? iSour + 1 : sizeof(p->cDisplay) - 1);
                    bModify = TRUE;

                    strcpy(cDisplayData,p->cDisplay);
                }
                else
                {
                    bModify = FALSE;
                }
            }
            else
            {
                iSour = strlen(pbNameIcon.szName);
                strncpy(p->cDisplay,pbNameIcon.szName,
                    sizeof(p->cDisplay) > iSour ? iSour + 1 : sizeof(p->cDisplay) - 1);
                p->bInPhonebook = TRUE;
                bModify = TRUE;

                strcpy(cDisplayData,p->cDisplay);
            }
        }
        else
        {
            if (p->bInPhonebook)
            {
                iSour = strlen(p->cNumber);
                strncpy(p->cDisplay,p->cNumber,
                    sizeof(p->cDisplay) > iSour ? iSour + 1 : sizeof(p->cDisplay) - 1);
                p->bInPhonebook = FALSE;
                bModify = TRUE;

                if ((int)strlen(p->cDisplay) > iViewMaxLen)
                {
                    strncpy(cDisplayData,p->cDisplay,iViewMaxLen);
                    strcat(cDisplayData,pCHOMIT);
                }
                else
                {
                    strcpy(cDisplayData,p->cDisplay);
                }
            }
            else
            {
                bModify = FALSE;
            }
        }
        if (bModify)
        {
            SendMessage(hList,LB_DELETESTRING,i,0);
            index = SendMessage(hList,LB_INSERTSTRING,i,(LPARAM)cDisplayData);
			
			GetTimeDisplay(p->Time, cTime, cData);
			len = strlen(cData);
			len1 = strlen(cTime);
			if (len+len1+1> MAX_ITEMLEN) 
				return ;
			sprintf(cAuxdspdata,"%s %s", cData, cTime);
			/*
			strncpy(cAuxdspdata, cData, len);
			strcat(cAuxdspdata, cTime);
			*/
			cAuxdspdata[len+len1+1] = 0;
			SendMessage(hRecordList, LB_SETAUXTEXT, MAKEWPARAM(index, MAX_ITEMLEN),  (LPARAM)cAuxdspdata);

            SetLIstBoxIcon(hList,index,p->iMBRecordType);
        }
        p = p->pNext;
        i++;
    }
    SendMessage(hList,LB_SETCURSEL,iOldCur,0);
}

static  BOOL    RefreshMBRecordList_One(HWND hList,const MBRECORDLISTEXT * pcMBRecordListExt)
{
    MBRECORDLISTEXT * p = (MBRECORDLISTEXT *)pMBRecordListExt;
    int    i = 0,iSour,index;
    ABINFO pbNameIcon;
    char   cDisplayData[PHONENUMMAXLEN + 1] = "";
	char  cAuxdspdata[MAX_ITEMLEN] = "";
	char  cTime[15] = "";
	char  cData[15] = "";
	int len, len1;

    while (p != NULL)
    {
        if ((pcMBRecordListExt->iMBRecordType == p->iMBRecordType) &&
            strcmp(pcMBRecordListExt->cDisplay,p->cDisplay) == 0)
        {   
            if (MB_GetInfoFromPhonebook(pcMBRecordListExt->cNumber,&pbNameIcon))
            {
                iSour = strlen(pbNameIcon.szName);
                strncpy(p->cDisplay,pbNameIcon.szName,
                    sizeof(p->cDisplay) > iSour ? iSour + 1 : sizeof(p->cDisplay) - 1);
                p->bInPhonebook = TRUE;

                strcpy(cDisplayData,p->cDisplay);
            }
            else
            {
                iSour = strlen(pcMBRecordListExt->cNumber);
                strncpy(p->cDisplay,pcMBRecordListExt->cNumber,
                    sizeof(p->cDisplay) > iSour ? iSour + 1 : sizeof(p->cDisplay) - 1);
                p->bInPhonebook = FALSE;

                if ((int)strlen(p->cDisplay) > iViewMaxLen)
                {
                    strncpy(cDisplayData,p->cDisplay,iViewMaxLen);
                    strcat(cDisplayData,pCHOMIT);
                }
                else
                {
                    strcpy(cDisplayData,p->cDisplay);
                }
            }
            SendMessage(hList,LB_DELETESTRING,i,0);
            index = SendMessage(hList,LB_INSERTSTRING,i,(LPARAM)cDisplayData);
			//  [9/14/2005]
			GetTimeDisplay(p->Time, cTime, cData);
			len = strlen(cData);
			len1 = strlen(cTime);
			if (len+len1+1> MAX_ITEMLEN) 
				return FALSE;
			sprintf(cAuxdspdata,"%s %s", cData, cTime);
			/*
			strncpy(cAuxdspdata, cData, len);
			strcat(cAuxdspdata, cTime);
			*/
			cAuxdspdata[len+len1+1] = 0;
			SendMessage(hRecordList, LB_SETAUXTEXT, MAKEWPARAM(index, MAX_ITEMLEN),  (LPARAM)cAuxdspdata);
			
            SetLIstBoxIcon(hList,index,p->iMBRecordType);
            SendMessage(hList,LB_SETCURSEL,index,0);
        }
        p = p->pNext;
        i++;
    }
    return TRUE;
}

static  void    SetLIstBoxIcon(HWND hList,int index,MBRECORDTYPE type)
{
    DWORD dWord;
    dWord = MAKEWPARAM(IMAGE_BITMAP, (WORD)index);
    switch (type)
    {
    case UNANSWER_REC:
        SendMessage(hList,LB_SETIMAGE,(WPARAM)dWord,(LPARAM)hIconUnans);
        break;
    case ANSWERED_REC:
        SendMessage(hList,LB_SETIMAGE,(WPARAM)dWord,(LPARAM)hIconIn);
        break;
    case DIALED_REC:
        SendMessage(hList,LB_SETIMAGE,(WPARAM)dWord,(LPARAM)hIconOut);
        break;
    }
}

static  BOOL    CreateControl(HWND hWnd)
{
//    int     iscreenw,iscreenh;
//    DWORD   dStyle;
	RECT rClient;
	GetClientRect(hWnd, &rClient);
	//lanlan fix
	SendMessage(hPWnd,PWM_SETBUTTONTEXT,1,(LPARAM)IDS_MENU_SAVE);
	SendMessage(hPWnd,PWM_SETBUTTONTEXT,2,(LPARAM)"Options");
	//end
	//lanlan fix
	/*
	if (Dflag)
		SendMessage(hPWnd,PWM_CREATECAPTIONBUTTON,(WPARAM)IDC_BUTTON_RIGHT,IDS_EXIT);
	else
		SendMessage(hPWnd,PWM_CREATECAPTIONBUTTON,(WPARAM)IDC_BUTTON_RIGHT,BUTTONRIGHT);
	*/
	if (Dflag)
		SendMessage(hPWnd,PWM_SETBUTTONTEXT, 0, IDS_EXIT);
	else
		SendMessage(hPWnd,PWM_SETBUTTONTEXT, 0, BUTTONRIGHT);
	//end
	SendMessage(hPWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON), (LPARAM)NULL);
	SendMessage(hPWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON), (LPARAM)NULL);
	SetWindowText(hPWnd, cTitle);
	hRecordList = CreateWindow(
        "LISTBOX",
        "",
        WS_VISIBLE | WS_CHILD|WS_VSCROLL | LBS_NOTIFY | LBS_BITMAP | LBS_MULTILINE,
		rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
        hWnd,
        (HMENU)IDC_LIST,
        NULL,
        NULL);
    if (hRecordList == NULL)
        return FALSE;
    return TRUE;
}
static  LRESULT CALLBACK NewListProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT lResult;
    lResult = TRUE;
    switch(message)
    {
    case WM_KEYUP:
        switch (LOWORD(wParam))
        {
        default:
            lResult = OldListProc(hWnd,message,wParam,lParam);
        }
        break;

    case WM_KEYDOWN:
        switch(LOWORD(wParam))
        {
        case VK_F1:
			SendMessage(hRecordListWnd,WM_COMMAND,IDM_DIALRECORD,0);
            
            break;
        default:
            lResult = OldListProc(hWnd,message,wParam,lParam);
            break;
        }
        break;
    default:
        lResult = OldListProc(hWnd,message,wParam,lParam);
        break;
    }
    return lResult;
}

static int gMissedCalls = 0;

void RegisterInfoWnd(HWND hWnd, UINT wMsg)
{
	gwndinfo.hInfoWnd = hWnd;
	gwndinfo.wMsg = wMsg;
}
/*
void SetMissCallIcon()
{
	gMissedCalls++;
	vmissedcount= 1;
	DlmNotify(PS_SETMISSEDCALL, ICON_SET);
	SendMessage(gwndinfo.hInfoWnd, gwndinfo.wMsg, (WPARAM)gMissedCalls, 0);
}

void CancelMissCallIcon()
{
	gMissedCalls = 0;
	if (vmissedcount != 0)
		DlmNotify(PS_SETMISSEDCALL, ICON_CANCEL);
	vmissedcount = 0;
	SendMessage(gwndinfo.hInfoWnd, gwndinfo.wMsg, (WPARAM)gMissedCalls, 0);
}*/

void GetRecentMissedCall(int * pcount)
{
	memcpy(pcount, &mdata.count, sizeof(int));
}

int SaveRecentMissedCall(const char * number)
{
	int len;
	FILE * file;
	int dfsize;
	int i;
	BOOL inflag = FALSE;  // This flag mark if the number is in the Recent missed call list

	if (NULL == number) 
		return -1;
	len = strlen(number);
	if (0 == len ) 
		return -1;
	if (FALSE == FindNumberInUnAnswerList(number)) 
		return -1;
	inflag = FALSE;
	for (i = 0;i < MAX_PHONE_RECORD;i++)
	{
		if (strcmp(pRecentMissCall[i].PhoneNo,number) == 0)
		{
			pRecentMissCall[i].count++;
			inflag = TRUE;
			break;
		}
		else
			continue;
	}
	if (inflag == FALSE)
	{
		for (i = 0;i < MAX_PHONE_RECORD;i++)
		{
			if (strlen(pRecentMissCall[i].PhoneNo) == 0)
			{
				strncpy(pRecentMissCall[i].PhoneNo,number,sizeof(pRecentMissCall[i].PhoneNo) > len ? len + 1:sizeof(pRecentMissCall[i].PhoneNo) - 1);
				pRecentMissCall[i].count = 1;
				inflag = TRUE;
				break;
			}
			else
				continue;
		}
	}
	file = fopen(pMISSDATA_LOG, "r+");
	if (file == NULL) 
		return -1;
	else
	{
		if (fseek(file, sizeof(MISSDATA), SEEK_SET) == -1) 
			return -1;
		dfsize = fwrite(pRecentMissCall, sizeof(RECENTMISSRECORD), MAX_PHONE_RECORD, file);
		fclose(file);
		return 0;
	}
}

int FindNumberInRecentMissCall(const char * number)
{
	int i;
    for (i = 0;i < MAX_PHONE_RECORD;i++)
    {
        if (strcmp(pRecentMissCall[i].PhoneNo, number) == 0)
            return i;
//        else
//		    continue;
    }
	return -1;
}
int ClearRecentMissedCall()
{
	FILE * file;
	int dfsize;

	file = fopen(pMISSDATA_LOG, "w+");
	if (file == NULL) 
		return -1;
	else
	{
		if (fseek(file, sizeof(MISSDATA), SEEK_SET) == -1) 
			return -1;
		memset(pRecentMissCall, 0, sizeof(RECENTMISSRECORD) * MAX_PHONE_RECORD);
		dfsize = fwrite(pRecentMissCall, sizeof(RECENTMISSRECORD), MAX_PHONE_RECORD, file);
		fclose(file);
		return 0;
	}	
}

void SetMissCallIcon()
{
	FILE * file;
    DWORD   dfsize = 0;
	vmissedcount= 1;
	DlmNotify(PS_SETMISSEDCALL, ICON_SET);
	mdata.flag = vmissedcount;
	mdata.count++;
	file = fopen(pMISSDATA_LOG, "w+");
	if (file == NULL) 
		return;
	else
	{
		dfsize = fwrite(&mdata, sizeof(MISSDATA), 1, file);
		fclose(file);
		return;
	}
}

void CancelMissCallIcon()
{
	FILE * file;
    DWORD   dfsize = 0;
	
	file = fopen(pMISSDATA_LOG, "r+");
	if (file == NULL) 
		return;
	else
	{
		fseek(file, 0, SEEK_SET);
		dfsize = fread(&mdata, sizeof(MISSDATA), 1, file);
		//		fclose(file);
		vmissedcount = mdata.flag;
	}
	if (vmissedcount != 0)
		DlmNotify(PS_SETMISSEDCALL, ICON_CANCEL);
	vmissedcount = 0;
	mdata.flag = vmissedcount;
	mdata.count = 0;
	/*
	file = fopen(pMISSDATA_LOG, "w+");
	if (file == NULL) 
	return;
	else
	*/
	//	{
	fseek(file, 0, SEEK_SET);
	dfsize = fwrite(&mdata, sizeof(MISSDATA), 1, file);
	fclose(file);
	return;
	//	}

}

int InitMissData()
{
	FILE * file;
    DWORD   dfsize = 0;

	pRecentMissCall = (RECENTMISSRECORD *)malloc(sizeof(RECENTMISSRECORD) * MAX_PHONE_RECORD);
	memset(pRecentMissCall   , 0, sizeof(RECENTMISSRECORD) * MAX_PHONE_RECORD);
    if (NULL == pRecentMissCall)
		return -1;
 	file = fopen(pMISSDATA_LOG, "r");
	if (file == NULL) 
	{
        file = fopen(pMISSDATA_LOG, "w+");
		if (file == NULL)
			return -1;
		else
		{
			if (fseek(file, 0, SEEK_SET) == -1)
				return -1;
			memset(&mdata, 0 ,sizeof(MISSDATA));
			dfsize = fwrite(&mdata, sizeof(MISSDATA), 1, file);
			if (fseek(file, sizeof(MISSDATA), SEEK_SET) == -1) 
				return -1;
			dfsize = fwrite(pRecentMissCall, sizeof(RECENTMISSRECORD), MAX_PHONE_RECORD, file);
			fclose(file);
			DlmNotify(PS_SETMISSEDCALL, ICON_CANCEL);
			return 0;
		}
	}
	else
	{
		if (fseek(file, 0, SEEK_SET) == -1)
			return -1;
		dfsize = fread(&mdata, sizeof(MISSDATA), 1, file);
		if (dfsize != 1)
			return -1;
		if (fseek(file, sizeof(MISSDATA), SEEK_SET) == -1) 
			return -1;
		dfsize = fread(pRecentMissCall, sizeof(RECENTMISSRECORD), MAX_PHONE_RECORD, file);
		fclose(file);
		if (mdata.flag != 0)
			DlmNotify(PS_SETMISSEDCALL, ICON_SET);
		else
			DlmNotify(PS_SETMISSEDCALL, ICON_CANCEL);
		return 0;
	}
}

BOOL IsDailedRecordWnd()
{
	return (IsWindow(hRecordListWnd) && iRecordListType == DIALED_REC) ;
}

BOOL IsMissedRecordWnd()
{
	if (IsWindow(hRecordListWnd) && iRecordListType == UNANSWER_REC) 
		return TRUE;
	else
		return FALSE;
}
void HideRecordWnd()
{
	ShowWindow(hRecordListWnd, SW_HIDE);
}
void ShowRecordWnd()
{
	ShowWindow(hRecordListWnd, SW_SHOW);
}

#define  IDC_SELECTLIST    7
#define  IDC_CANCEL        8
#define  IDC_REMOVE        9
#define  IDS_SELECTCALLS  ML("Select calls")
#define  IDS_CANECEL      ML("Cancel")
static const char* pSelectedClassName = "RemoveSelectedWinClass";
static HWND hSelectWin;
static HWND hSelectList;
LRESULT CALLBACK RemoveSelectedWinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

BOOL RemoveSelectedWin()
{
	WNDCLASS wc;
	RECT rClient;

	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hbrBackground = GetStockObject(WHITE_BRUSH);
	wc.hCursor = NULL;
	wc.hIcon = NULL;
	wc.hInstance = NULL;
	wc.lpfnWndProc = RemoveSelectedWinProc;
	wc.lpszClassName = pSelectedClassName;
	wc.lpszMenuName = NULL;
	wc.style = 0;

	if(!RegisterClass(&wc))
		return FALSE;

	GetClientRect(hPWnd, &rClient);
	hSelectWin = CreateWindow(
		pSelectedClassName,
		"",
		WS_CHILD | WS_VISIBLE ,
		rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
		hPWnd, NULL, NULL, NULL);
    if (NULL == hSelectWin)
    {
        UnregisterClass(pSelectedClassName,NULL);
        return FALSE;
    }	
	ShowWindow(hPWnd, SW_SHOW); 
	UpdateWindow(hPWnd);  
	if (hSelectList != NULL)
		SetFocus(hSelectList);
	else
		SetFocus(hSelectWin); 
	return TRUE;
}

extern BOOL    DeleteSelectedMBRecord(MBRECORDTYPE CallRecType,int counter);
LRESULT CALLBACK RemoveSelectedWinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRet = TRUE;
	switch(message)
	{
	case WM_CREATE:
		{
			char  cDisplayData[PHONENUMMAXLEN + 1] = "";
			char szDisp[PHONENUMMAXLEN + 1] = "";
			MBRECORDLISTEXT * p;
			RECT rClient;
			int index;
			GetClientRect(hWnd, &rClient);
			//lanlan replace
			/*
			SendMessage(hPWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_CANCEL, 0), (LPARAM)IDS_CANECEL);
			SendMessage(hPWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_REMOVE, 1), (LPARAM)IDS_REMOVE);
			*/
			SendMessage(hPWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_CANECEL); 
			SendMessage(hPWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_REMOVE); 
			//end
			SendMessage(hPWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select"); 
			SendMessage(hPWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON), (LPARAM)NULL);
			SendMessage(hPWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON), (LPARAM)NULL);
			SetWindowText(hPWnd, IDS_SELECTCALLS);
			PDASetMenu(hPWnd, NULL);
			hSelectList = CreateWindow("MULTILISTBOX",
				"",
				WS_VISIBLE | WS_CHILD | WS_VSCROLL | LBS_BITMAP,
				rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
				hWnd,
				(HMENU)IDC_SELECTLIST,
				NULL,
				NULL);
			if (hSelectList == NULL)
				return FALSE;
			LoadMBRecordToLink(iSORT_TIME);
			SendMessage(hSelectList, LB_RESETCONTENT, 0, 0);
			
			p = pMBRecordListExt;
			while (p != NULL)
			{
				strcpy(cDisplayData, p->cDisplay);
				ShowDispItem(hWnd, p->cNumber, cDisplayData, szDisp, p->bInPhonebook, FALSE);
				index = SendMessage(hSelectList, LB_ADDSTRING, 0, (LPARAM)szDisp);
				p = p->pNext;
			}			
			SendMessage(hSelectList, LB_ENDINIT, 0, 0);
			SendMessage(hSelectList, LB_SETCURSEL, 0, 0);
		}
		break;

//	case  WM_SETFOCUS:
//		SetFocus(hSelectList);
//		break;;

	case PWM_SHOWWINDOW:
		//lanlan replace
		/*
		SendMessage(hPWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_CANCEL, 0), (LPARAM)IDS_CANECEL);
		SendMessage(hPWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_REMOVE, 1), (LPARAM)IDS_REMOVE);
		*/
		SendMessage(hPWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_CANECEL); 
		SendMessage(hPWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_REMOVE); 
		//end
		SendMessage(hPWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select"); 
		SendMessage(hPWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON), (LPARAM)NULL);
		SendMessage(hPWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON), (LPARAM)NULL);
		SetWindowText(hPWnd, IDS_SELECTCALLS);
		PDASetMenu(hPWnd, NULL);; 
		SetFocus(hSelectList);
		break;

    case WM_CLOSE:
		DestroyWindow(hWnd);
        break;

    case WM_DESTROY :
		hSelectList = NULL;
        hWnd = NULL;
		hFocus = NULL;
//        DestroyRecordListExt();
        UnregisterClass(pSelectedClassName,NULL);
        break;

	case WM_KEYDOWN:
		switch(LOWORD(wParam))
		{
		case VK_F10:
			SendMessage(hPWnd,PWM_CLOSEWINDOW,  (WPARAM) hWnd, 0);			
			SendMessage(hWnd, WM_CLOSE, 0,0);
			break;

		case VK_RETURN:
			{
//				MBRECORDLISTEXT MBRecordListExt;
				char sznotify[40];
				int nCount, j;
				BOOL  NoSflag = TRUE;

				nCount = SendMessage(hSelectList, LB_GETCOUNT, 0, 0);
				for (j = 0; j <nCount; j++)
				{
					if (IsSelected(j))
					{
						NoSflag = FALSE;
							break;
					}
				}
				if (NoSflag)
				{
					PLXTipsWin(hPWnd, hWnd, 0, NOTIFY_NOCALLSSELECTED,NULL,Notify_Alert, ML("Ok"),NULL, WAITTIMEOUT);
					break;
				}
				sznotify[0] = 0;
				switch(iRecordListType)
				{
				case UNANSWER_REC:
					strcpy(sznotify, NOTIFY_REMOVEMISSCALLS);
					break;
				case ANSWERED_REC:
					strcpy(sznotify, NOTIFY_REMOVERECIEVEDCALLS);
					break;
				case DIALED_REC:
					strcpy(sznotify, NOTIFY_REMOVEDIALEDCALLS);
					break;
				}
				PLXConfirmWinEx(hPWnd, hWnd, sznotify, Notify_Request, NULL, ML("Yes"),ML("No"), IDRM_RETURN);
				
			}
			break;
			
		default:
			lRet = PDADefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	
	case IDRM_RETURN:
		if (lParam == 1 )
		{
			int	nCount;
			nCount = SendMessage(hSelectList, LB_GETCOUNT, 0, 0);
			if(nCount > 0)
			{	
				if (!DeleteSelectedMBRecord(iRecordListType, nCount)) 
				{
//							AppMessageBox(hWnd,MBRECORDLISTDELFAIL,cTitle, WAITTIMEOUT);
					PLXTipsWin(hPWnd, hWnd, 0, MBRECORDLISTDELFAIL, NULL, Notify_Failure, ML("Ok"), NULL, WAITTIMEOUT);
					PostMessage(hPWnd,PWM_CLOSEWINDOW,  (WPARAM) hWnd, 0);			
					PostMessage(hWnd, WM_CLOSE, 0,0);
				}
				else
				{							
//							AppMessageBox(hWnd,NOTIFY_REMOVED,cTitle, WAITTIMEOUT);
                    PLXTipsWin(hPWnd, hWnd, 0, NOTIFY_REMOVED, NULL, Notify_Success, ML("Ok"), NULL, WAITTIMEOUT);
					PostMessage(hRecordListWnd,IDC_REFRESHRECORDLIST,1,0);
					PostMessage(hPWnd,PWM_CLOSEWINDOW,  (WPARAM) hWnd, 0);			
					PostMessage(hWnd, WM_CLOSE, 0,0);
				}
			}
		}
		break;

	default:
		lRet = PDADefWindowProc(hWnd, message, wParam, lParam);
		break;
	}
	return lRet;
}

BOOL IsSelected(int i)
{
	return (SendMessage(hSelectList, LB_GETSEL, (WPARAM)i, 0)) ;
}
