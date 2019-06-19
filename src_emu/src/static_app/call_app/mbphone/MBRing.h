
#include "plx_pdaex.h"
#include "me_wnd.h"
#include "mullang.h"

#include "hpimage.h"
#include "winpda.h"
#include "MBPublic.h"
#include "RcCommon.h"
#include "setting.h"
#include "prioman.h"

#define     IDC_BUTTON_SET          201
#define     IDC_BUTTON_QUIT         202
#define     IDC_REPLACE_CALL        203
#define     IDC_HANGUP              204
#define     IDC_BUTTON_MIDDLE       206
#define     IDC_SILENCE             207
#define     WM_VOICER_ANSWER        WM_USER + 103
#define     WM_SWITCH_CALLLINE      WM_USER + 106
#define     WM_VOICER_HANGUP        WM_USER + 105
#define     WM_SWITCH_ACTIVECALLLINE WM_USER + 107
#define     WM_UPDATE                WM_USER +108
#define     IDC_TIMER_CF            205
#define     IDC_TIMER_BRINGTOTOP    210

#define     IDM_FORWARD             211
#define     IDM_ANSWER              212
#define     IDM_REJECT              213
#define     IDM_REPLACE             214

#define     BUTTONOK                ML("Ok") 
#define     BUTTONQUIT              ML("Exit") 
#define     IDS_HANGUP              ML("Hangup") 
#define     IDS_DROP                ML("Drop")
#define     TITLECAPTION            ML("Incoming") 
#define     IDS_VOICE_ANSWER        ML("Get") 
#define     IDS_REPLACE             "Replace"
#define     IDS_PL_CONFIRM          ML("Incoming") 
#define     IDS_CLOSECURCALL        ML("Hangup the call?") 
#define     IDS_PROMPT              ML("Prompt") 
#define     IDS_HANGUP_FAIL         ML("Fail to hangup") 
#define     IDS_ANSWER_FAIL         ML("Answer fail") 
#define     IDS_SILENCE             ML("Silence")
#define     SWITCHFAIL              ML("Switch fail") 
#define     CALLFORWARD          ML("Call forward") 
#define     BUTTON_MIDDLE_TXT       ML("Dialup") 
#define     STATUS_INFO_CALLING             "calling"
#define     STATUS_INFO_WAITING             "waiting"//ML("waiting")
#define     STATUS_INFO_DATAWAITING ML("Data waiting")
#define     STATUS_INFO_DATACALL    ML("data call")
#define     IDS_MENU_ANSWER          ML("Answer")
#define     IDS_MENU_REJECT          ML("Drop")
#define     IDS_MENU_REPLACE         "Replace"
#define     UNKNOWNUMBER             ML("Unknow number")
#define     NOTIFY_TOOMANYCALLS     ML("Too many calls")
#define     UNSUCCESSFUL            ML("unsuccessful")

static  LRESULT CALLBACK MBRingAppWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam,LPARAM lParam);
static  LRESULT CALLBACK ConfirmWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam,LPARAM lParam);
static  void    OnCommandMsg(HWND hWnd, WPARAM wParam, LPARAM lParam);
static  BOOL    GotoConfirmWindow(HWND hWnd);
static  void    MBRingHangup(void);
static  void    MBRingInit(void);
static  void    DestroyConfirm(void);
static  void    MBHangupOperate(void);
