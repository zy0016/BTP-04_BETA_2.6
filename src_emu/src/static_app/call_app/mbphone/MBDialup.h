
#include    "winpda.h"

#include    "mullang.h"
#include    "setting.h"
#include    "plx_pdaex.h"
#include    "hpimage.h"
#include    "MBPublic.h"
#include    "phonebookext.h"
#include    "RcCommon.h"

#ifndef QDIAL_MAX
#define QDIAL_MAX 12
#endif

#define     DIALUP_WIN_TOPX         0 
#define     DIALUP_WIN_TOPY         134 
#define     DIALUP_WIN_WIDTH        176
#define     DIALUP_WIN_HEIGHT       86
#define     DIALUP_WIN_POSITION     0, 134,176, 86
#define     DIALUP_EDIT_POSITION    0, 0,176, 62
#define     IDC_BUTTON_CLEAR        2
#define     IDC_BUTTON_QUIT         3
#define     IDC_BUTTON_SET          4
#define     IDC_TIMER_BACKSPACE     5
#define     IDC_TIMER_LONGKEY       6
#define     IDC_BUTTON_CALL         7

#define     IDC_TIMER_EXIT          10

#define     IDM_SAVE                8
#define     IDM_CALL                9
#define     IDM_SERVCMD             10
#define     IDM_MESSAGE             11
#define     IDM_NEWCONTACT          12
#define     IDM_ADDTOCONTACT        13
#define     IDM_SMS                 14
#define     IDM_MMS                 15
#define     IDC_CALLEDIT            16

#define     IDC_HOLD                20
#define     IDC_ACTIVE              21
#define     IDC_MUTE                22
#define     IDC_UNMUTE              23
#define     IDC_SENDDTMF            24
#define     IDC_WILDCARD_CALL       25
#define     IDC_WILDCARD_BACK       26

#define     IDC_ME_SUPERADDDIAL     WM_USER + 100
#define     IDC_ME_SHORTCUT         WM_USER + 110
#define     IDC_ADDNEWCONTACT       WM_USER + 130

#define     IDC_SENDSMSINFO         WM_USER + 140
#define     IDC_SENDMMSINFO         WM_USER + 150

#define     TIMER_LONGKEY           1000
#define     TIMER_BACKSPACE         1000

#define     IDC_INIT112             WM_USER + 90
#define     WM_EMERGENCY            WM_USER +120
#define     TITLECAPTION            ML("Call") 
#define     BUTTONOK                (LPARAM)ML("Ok") 
#define     BUTTONQUIT              (LPARAM)ML("Cancel") 
#define     BUTTONSAVE              (LPARAM)ML("Save") 
#define     IDS_MENU_SAVE           ML("Save")
#define     IDS_MENU_NEWCONTACT     ML("New contact")
#define     IDS_MENU_ADDTOCONTACT   ML("Add to contact")
#define     IDS_MENU_CALL           ML("Call")
#define     IDS_MENU_SERVCMD        ML("sendserv.cmd")
#define     IDS_MENU_MESSAGE        ML("Write message")
#define     IDS_MENU_SMS            ML("SMS")
#define     IDS_MENU_MMS            ML("MMS")

#define     BUTTONDELETE            (LPARAM)ML("Clear") 
#define     ERROR1                  ML("Unsuccessful") 
#define     WINDOWICO               (LPARAM)"ROM:note.ico"              
#define     WILDCALLL               "Call"

#define     EMERGENT                ML("Emergency call") 
#define     MULTME_CLEW             ML("Active all") 
#define     MULTME_DIAL             ML("Unsuccessful") 
#define     ME_APPLY_FAIL           ML("Unsuccessful") 
#define     QUICKDIAL_FAIL          ML("Input number") 
#define     INIT_CLEW               ML("Initialize") 

#define     NOTIFY_NOTALLOWED       ML("Not allowed")
#define     NOTIFY_TOOMANYCALLS     ML("Too many calls")
#define     NOTIFY_NOSIMCARD        ML("No SIM Card")
#define     NOTIFY_UNSUCCESSFUL     ML("Unsuccessful") 
#define     NOTIFY_NOMORECALL       ML("No more calls can be joined")//ML("No more calls can be added")
#define     NOTIFY_WILDWARD         ML("Call to:")

#define     NOTIFY_HOLD             ML("Hold")
#define     NOTIFY_ACTIVE           ML("Active")
#define     NOTIFY_MUTE             ML("Mute")
#define     NOTIFY_UNMUTE           ML("Unmute")
#define     NOTIFY_SENDDTMF         ML("Send DTMF")
#define     NOTIFY_WILDCLEW         ML("Please complete number")
#define     NOTIFY_WILDCLEWEMPTY    ML("Please define number")

#define     BTN_NUM                 14

typedef struct tagDialItemBtn
{
    RECT            rBtnPicLarge;   
    RECT            rBtnPicSmall;   
    unsigned int    iVKey;          
    char            ch_short;       
    char            ch_long;        
    int             iState;         
}DIALITEMBTN,PDIALITEMBTN;



typedef enum
{
    RESULT_NOINIT,      
    RESULT_SUCC,        
    RESULT_ERRORNUMBER, 
    RESULT_GSM,         
    RESUTL_USSD,        
    RESULT_PHONENUM,    
    RESULT_QUICKDIAL,   
    RESULT_NONESIM,     
    RESUTL_PROJECTMODE, 
	RESULT_SW,
	RESULT_SETLG,
	RESULT_PM,
	RESULT_ALS,
	RESULT_BTADDR,
	RESULT_RING,
    RESULT_WILDCARD,
    RESULT_END          
}RESULT_BYSEND;


static  LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static  BOOL    CreateControl(HWND hWnd);
static  void    ShowPhoneNumber(HDC hdc);
static  BOOL    DialNumber(HWND hWnd);

static  int     GetDialitemBtnIndex(char ch);
static  int     GetDialitemBtnKey(unsigned int iVKey);
static  int     DealWithNumber(HWND hWnd);
static  BOOL    IsNumberRemoveLast(void);
static  BOOL    GetLastNumberRect(RECT * pLastRect);
static  BOOL    APP_CallPhoneWindow(const char * pPhoneNumber,BOOL bSuperAdd,BOOL bCanSend);
static  RESULT_BYSEND     DealWithNumberBySend(const char * pPhoneNumber);
static int GetWildCard(const char * pPhoneNumber);
HWND Get_DialupWnd();
extern void APP_EditMMS(HWND hWndFrame, HWND hWndPre, UINT msg, UINT nType,
                    PSTR  InsertFileName);
extern void SMS_Register(HWND hMsgWnd, UINT uMsgCmd);// ×¢²á
extern void SMS_Unregister(void);// ·´×¢²á
extern BOOL MMS_EntryRegister(HWND hWnd, int msg);
