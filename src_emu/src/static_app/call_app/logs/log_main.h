 
#include    "winpda.h"
#include    "mullang.h"
#include    "plx_pdaex.h"
#include    "pubapp.h"
#include    "RcCommon.h"

#include "MBPublic.h"

#include    "MBRecordList.h"
#include    "mullang.h"

#define     LOG_DIR                "/mnt/flash/calllogs/"
#define     IDC_BUTTON_QUIT         3
#define     IDC_BUTTON_SET          4
#define     IDC_MENULIST            6
#define     IDC_COUNTER_MENULIST    16
#define     IDC_MENUITEM_UNANSWER   7
#define     IDC_MENUITEM_ANSWER     8
#define     IDC_MENUITEM_DIAL       9
#define     IDC_MENUITEM_TIME       10
#define     IDC_MENUITEM_CALLCOST   11
#define     IDC_MENUITEM_COSTTYPE   12
#define     IDC_MENUITEM_COSTLIMIT  13
#define     IDC_MENUITEM_GPRSCOUNTER 14
#define     IDC_MENUITEM_LOG        15

#define     TITLECAPTION            ML("Record") 
#define     CAP_RCTCALL             ML("Recent calls")
#define     CAP_COUNTERS            ML("Counters")
#define     BUTTONQUIT              (LPARAM)ML("Exit") 
#define     ERROR1                  ML("Create window fail") 

#define     LISTTXT_UNANSWER        ML("Missed calls") 
#define     LISTTXT_ANSWER          ML("Received calls") 
#define     LISTTXT_DIAL            ML("Dialled calls") 
#define     LISTTXT_TIME            ML("Call counters") 
#define     LISTTXT_CALLCOST        ML("Call costs") 
#define     LISTTXT_COSTTYPE        ML("Showcost") 
#define     LISTTXT_COSTLIMIT       ML("Costlimit") 
#define     LISTTXT_GPRSCOUNTER     ML("GPRS counters")
#define     LISTTXT_LOG             ML("Log")

#define     DISPLAY_UNANSDWER_S     ML("%s%ditem") 
#define     DISPLAY_UNANSDWER_M     ML("%s%ditem") 

typedef enum
{
	log_recentcalls = 0,
	log_counters,
	log_communications
}LOGIFTYPE;

static  LRESULT AppWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static  BOOL    CreateControl(HWND hWnd);
static  void    SetMenuListText(HWND hMenuList);

extern BOOL    CallMBListWindow(HWND hwndCall);
extern BOOL    CallMBGPRSCounterWindow(HWND hwndCall);
