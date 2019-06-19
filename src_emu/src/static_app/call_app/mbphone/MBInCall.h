  
#include    "winpda.h"
#include    "mullang.h"

#include    "plx_pdaex.h"
#include    "MBPublic.h"
#include    "hpimage.h"

//#include    "MBList.h"
#include    "RcCommon.h"

#define MBLIST_IPIC_UNCHANGE    -1
#define MBLIST_IPICNONE         0
#define MBLIST_IPICSTATE_A      1
#define MBLIST_IPICSTATE_H      2
#define MBLIST_IPICSTATE_AM     4
#define MBLIST_IPICSTATE_HM     8

#define     TIMER_CALLING               100
#define     TIMER_CALLING_COST          2000
#define     TIMER_LONGKEY               1000
#define     TIMER_INIT                  2000
#define     TIMER_ERROR                 11000

#define     IDC_BUTTON_MIDDLE           20
#define     IDC_ROBUTTON                30
#define     IDC_LBUTTON_NONE            40
#define     IDC_RBUTTON_BACK            50
#define     IDC_RBUTTON_CANCEL          51

#define     IDC_LBUTTON_MENU            54

#define     IDC_HF                      56
#define     IDC_BTN_ENDACTIVECALL       41
#define     IDC_BTN_SWAP                42
#define     IDC_PICK                    43
#define     IDC_PICKCONFERENCE          44
#define     MB_ACTIVE                   WM_USER + 55
#define     IDC_MBLIST                  60
#define     IDC_MEMBER_LIST             61

#define     TIMER_ID_FRESH              3
#define     IDC_TIMER_CALLING           70
#define     IDC_TIMER_COST              80
#define     IDC_TIMER_INIT              90
#define     IDC_TIMER_LONGKEY           100
#define     IDC_TIMER_ERROR             110
#define     IDC_END_TIMER               120
#define     IDC_TIMER_TRANSFER          130
#define     IDC_END_OK                  121
#define     END_TIMER_DURATION          2000
#define     IDC_EXTRACTMEMBER           WM_USER+52
#define     IDC_DROPMEMBER              WM_USER+53
#define     IDC_ME_SHORTCUTSWITCH       WM_USER + 99
#define     IDC_ONE_SWITCH              WM_USER + 100
#define     IDC_MULTME_SWITCH1          WM_USER + 120
#define     IDC_MULTME_SWITCH2          WM_USER + 121
#define     IDC_MULTME_ALLACTIVE        WM_USER + 130
#define     IDC_MULTME_ALLHANGUP        WM_USER + 140
#define     IDC_ME_LISTCURCALLS         WM_USER + 150

#define     IDC_ME_SUPERADDDIAL         WM_USER + 170
#define     IDC_ME_MUTE                 WM_USER + 180

#define     IDC_ME_SHORTOTHER           WM_USER + 200
#define     IDC_ME_CUT_HELD             WM_USER + 210
#define     IDC_ME_CUT_ACTIVED          WM_USER + 220
#define     IDC_ME_CUT_ACTIVED_X        WM_USER + 230
#define     IDC_ME_ACTIVE_SWITCH_X      WM_USER + 250
#define     IDC_ME_CALL_TRANSFER        WM_USER + 270
#define     IDC_ME_GETPRICEUNIT         WM_USER + 280
#define     IDC_ME_GETCCM               WM_USER + 290
#define     IDC_ME_REPEAT               WM_USER + 300
#define     IDC_RECORD_OVER             WM_USER + 310
#define     IDC_ME_MAXPRICE             WM_USER + 320
#define     WM_ME_EXTRACTSWITCH         WM_USER + 330
#define     IDC_ERROR_MESSAGE           WM_USER + 500
#define     IDC_GETLINESTATUS           WM_USER + 510
#define     IDC_SETVOLUME               WM_USER + 520

#define     TITLECAPTION                ML("Connected")
#define     TXT_MUTE                    ML("Mute")
#define     TXT_UNMUTE                  ML("Unmute")
#define     TXT_BACK                    ML("Exit")
#define     TXT_CONNECT                 ML("Connected")
#define     TXT_HANGUP                  ML("Hangup")
#define     TXT_HANGUPMENU              ML("Hangup")
#define     TXT_CANCEL                  ML("Cancel")
#define     TXT_BACK2                   ML("Back")
#define     TXT_MENU                    ML("Menu")
#define     TXT_DIALNUMBER              ML("Dialup")
#define     IDS_HANDSFREE               ML("HF")
#define     IDS_CALLTIME                ML("Time")
#define     SWITCHFAIL                  ML("Unsuccessful")
#define     ACTIVEFAIL                  ML("Unsuccessful")
#define     HANGUPFAIL                  ML("Unsuccessful")
#define     HANGUPALLCLEW               ML("Hangup all")
#define     MULTME_CLEW                 ML("Active all")
#define     CALLEND                     ML("Call ended")
#define     MUTEFAIL                    ML("Unsuccessful") 
#define     UNMUTEFAIL                  ML("Unsuccessful")
#define     ME_APPLY_FAIL               ML("Unsuccessful") 
#define     DIALLING_FARE               ML("Call cost limit exceeded")

#define     NOTIFY_UNSUCCESSFUL         ML("Unsuccessful")

#define     TXT_HOLD                    ML("Hold")
#define     TXT_RESTORE                 ML("Pick")
#define     TXT_SWITCH                  ML("Swap")
#define     TXT_ALLACTIVE               ML("Join calls")
#define     TXT_PICKUPMEMBER            ML("Pick up member")
#define     TXT_ALLHANGUP               ML("End all calls") 
#define     TXT_HANGUPHOLD              ML("End hold call")
#define     TXT_SHIFT                   ML("Transfer")
#define     TXT_MEETING                 ML("Meeting")
#define     TXT_PICKUPCALL              ML("Pick up member")
#define     TXT_SINGLECALL              ML("Extract member")
#define     TXT_SINGLEHANGUP            ML("Drop member")
#define     TXT_HANGUP_SINGLE           ML("Hangup single")
#define     TXT_HANGUP_MEETING          ML("Hangup meeting")
#define     TXT_HANGUPACTIVE            ML("End actived call")
#define     TXT_SENDDTMF                ML("Send DTMF")
#define     TXT_HANDSET                 ML("Handset")
#define     TXT_HANDFREE                ML("Activate HF")
#define     TXT_HOLDCONFERENCE          ML("Hold conference")
#define     TXT_PICKCONFERENCE          ML("Pick conference")
#define     TXT_MEMBERS                 ML("Members")
#define     STATUS_INFO_ONHOLD          "on hold"//ML("on hold")
#define     STATUS_INFO_ACTIVE          "active"//ML("active")
#define     UNKNOWNUMBER             ML("Unknow number")
#define     TXT_CONFERENCE           ML("Conference call")
#define     TXT_DISCONNECTED         ML("Disconnected")
#define     TXT_ENDCALL              ML("End call")
#define     TXT_TRANSFER            ML("Calls transferred")
#define     PLEASEWAITING           ML("Please wait...")
#define     STATUS_INFO_EMERGENTCALL           ML("Emergency call") 
//BlueTooth begin
#define     BT_SWITCHTODIGITAL      ML("BT")//ML("BlueTooth")
#define     BT_SWITCHTOANALOG       ML("Mobile")//ML("Analog")
#define     IDC_BLUETOOTH           45//À¶ÑÀ°´Å¥
//BlueTooth end
#define     MEMBER_ACITVE_ICON    "/rom/mbphone/call_i_active.bmp"
#define     MEMBER_HELD_ICON      "/rom/mbphone/call_i_held.bmp"
#define     IN_CALL_ICON          "/rom/mbphone/incall.ico"
#define     CALL_MUTED_ICON      "/rom/mbphone/callmuted.bmp"

#define     LINE1BMP            "/rom/mbphone/call_i_line1.bmp"
#define     LINE2BMP            "/rom/mbphone/call_i_line2.bmp"
#define     INCALLING_ONE       PIC_NA



#define     IDM_ONE_HOLD                300
#define     IDM_SWITCH2                 310

#define     IDM_SWITCH1                 350
#define     IDM_ALLACTIVE               360
#define     IDM_ALLHANGUP               370
#define     IDM_HANGUPHOLD              380
#define     IDM_HANGUPACTIVE            450
#define     IDM_SHIFT                   400
#define     IDM_SINGLECALL              410
#define     IDM_SINGLEHANGUP            420
#define     IDM_PICKUPCALL              430
#define     IDM_MUTE                    480
#define     IDM_RECORD                  460
#define     IDM_VIEW                    470
#define     IDM_UNMUTE                  481
#define     IDM_SENDDTMF                490
#define     IDM_PICK                    301
#define     IDM_HANDSET                 500
#define     IDM_HANDFREE                501
static MENUITEMTEMPLATE MBCallingMenu[] = 
{
    {MF_STRING,IDM_MUTE  ,"",NULL},
    

    {MF_END   ,0          , NULL,NULL}
};
static const MENUTEMPLATE MBCallingMenuTemplate =
{
    0,
    (PMENUITEMTEMPLATE)MBCallingMenu
};
typedef enum
{
    MBCALLINNONE    = 0,
    MBCALLINGONE    = 1,
    MBCALLINGMULT   = 2 
}MBCALLTYPE;
typedef enum
{
    INTERFACE_NONE   = 0,
    INTERFACE_ONE    = 1,
    INTERFACE_MULT1  = 2,
    INTERFACE_MULT21 = 3,
    INTERFACE_MULT22 = 4,
    INTERFACE_MULT31 = 5,
    INTERFACE_MULT32 = 6,
    INTERFACE_MULT5  = 7,
    INTERFACE_MULT6  = 8 
}INTERFACE;

static  LRESULT AppWndProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static  void    ChangeTimeOnTimer(void);

static  BOOL    SetAllType(int nType);
static  int     GetTheNUsedItem(int nTheN);
static  BOOL    SetMenuSoftkey(HWND hWnd);
static  BOOL    MBTalkingWindow(HWND hpre);
static  void    CallingDisplay(HDC hdc);
static  BOOL    AddMultCallInfo(void);
static  void    InvertItemStateMult(void);
static  BOOL    InvertItemStateOne(int index);
static  void    RemTime(MBRecordCostTime * Cost,int nCall);
static  void    AfterHangup_Single(void);
static  int     GetFirstNotUsedItem(void);
static  void    AfterHangup_Mult_All(void);
static  BOOL    OperateListCurCalls(void);
int     GetItemTypeNum(int iType);
static  void    SetMBCallingMenu(HWND hWnd, INTERFACE iMenuType);
static  void    SetChangelessMenu(HMENU hMenu);
static  void    ExecuteHangup(void);
static  void    SetPhoneGroupNum(void);
static  void    GetPriceUnit(void);
static  void    MB_RemoveAllMenu(HMENU hMenu);
extern BOOL IsTimeChanged (void);
extern int GetHeadSetVolum (void);
HWND Get_CallingWnd();
