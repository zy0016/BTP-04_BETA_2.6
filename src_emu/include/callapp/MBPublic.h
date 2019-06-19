  
#ifndef APP_MBPUBLIC_APP
#define APP_MBPUBLIC_APP

#include "setting.h"
#include "window.h"
#include "me_wnd.h"
#include "phonebookext.h"
#include "pubapp.h"
#include "imesys.h"
#include "mullang.h"

#include "string.h"




#define SHOWDEBUGINFO   

#define MB_GetInfoFromPhonebook APP_GetInfoByPhone
#define PHONENUMMAXLEN              41  
#define MAX_COUNT                   5   
#define EXTENSIONCHAR               'P' 
#define EXTENSIONSTAR               'S' 


typedef enum
{
    LISTCURCALLS_NONE         ,
    LISTCURCALLS_HANGUP_ONE   ,
    LISTCURCALLS_HANGUP_ALL   ,
    LISTCURCALLS_HANGUP_HELD  ,
    LISTCURCALLS_HANGUP_ACTIVE,
    LISTCURCALLS_HANGUP_CUT_X ,
    LISTCURCALLS_SWITCH       ,
    LISTCURCALLS_REPEAT       ,
    LISTCURCALLS_TRANSFER     ,
    LISTCURCALLS_ACTIVE_X      
}OPERATETYPE;

///BlueTooth begin
typedef enum
{
    BT_WAITING,             //来电
    BT_AUDIO_CONNECTED,     //通话
    BT_AUDIO_DISCONNECTED,  //通话结束
    END                     //状态结束
}BT_STATUS;
typedef enum
{
    BT_CONNECTED,           //连接
    BT_DISCONNECTED,        //断连
}BT_CONNECTED_STATUS;

//BlueTooth end

typedef struct tagPhoneItem
{
    char        cName[PHONENUMMAXLEN + 1];  
    CALLS_INFO  Calls_Info;
#define ITEMUSED    1                   
#define ITEMNOTUSED 0                   
    int         ItemStat;               
    SYSTEMTIME  TimeUsed;               
    SYSTEMTIME  TimeInDial;             
}PHONEITEM, *PPHONEITEM;

#define WM_VOICE_HANGUPONE          WM_USER + 999
#define WM_CALLING_HANGUP           WM_USER + 998
#define IDC_RING_HANGUP             WM_USER + 997


#define IDC_FORWARDING              WM_USER + 996


#define MESSAGEBOXTITLE             ML("Prompt") 
#define CONFIRM_OK                  ML("Ok") 
#define CONFIRM_CANCEL              ML("Cancel") 


/*
#define MBPIC_VIEWLIST1             "/rom/mbphone/notepad.ico"
#define MBPIC_VIEWLIST2             "/rom/mbphone/dateapp.ico"
#define MBPIC_VIEWLIST3             "/rom/mbphone/contact.ico"
#define MBPIC_VIEWLIST4             "/rom/mbphone/mu_sms.ico"
#define MBPIC_VIEWLIST5             "/rom/mbphone/mu_mms.ico"
#define MBPIC_VIEWLIST6             "/rom/mbphone/mail_mal.ico"

#define MBPIC_DIALLINK              "/rom/mbphone/mb_linking.gif"

#define MBPIC_DIALUP_NUMBER         "/rom/mbphone/dialnums.gif"
#define MBPIC_DIALUP_NUMBAR         "/rom/mbphone/diabar.gif"
#define MBPIC_DIALUP_BSUP           "/rom/mbphone/dialarr.gif"
#define MBPIC_DIALUP_BSDOWN         "/rom/mbphone/backdown.gif"

#define MBPIC_MBLIST_ACTIVE         "/rom/mbphone/mbincall.gif"
#define MBPIC_MBLIST_HOLD           "/rom/mbphone/mbhold.gif"  
#define MBPIC_MBLIST_AM             "/rom/mbphone/mbam.gif"    
#define MBPIC_MBLIST_HM             "/rom/mbphone/mbhm.gif"   */
 
#define PIC_NA                      "/rom/mbphone/not_available.gif"

#define MBPIC_RECORD_UNANSWER       "/rom/mbrecord/mbunan.bmp"
#define MBPIC_RECORD_ANSWER         "/rom/mbrecord/mbansw.bmp"
#define MBPIC_RECORD_DIALED         "/rom/mbrecord/mbdialed.bmp"
#define MBPIC_RECORD_CALLTIME       "/rom/mbrecord/mbtime.bmp"
#define MBPIC_RECORD_CALLCOST       "/rom/mbrecord/mbcost.bmp"
#define MBPIC_RECORD_GPRSCOUNTER    "/rom/mbrecord/mbgprs.bmp"
//#define MBPIC_RECORD_COSTTYPE       "/rom/mbrecord/mbtime.ico"
//#define MBPIC_RECORD_SETCOST        "/rom/mbrecord/mbtime.ico"
//#define MBPIC_RECORD_LOG            "/rom/mbrecord/mbtime.ico"

#define MBPIC_RECORDLIST_IN         "/rom/mbrecord/mb_in.bmp" 
#define MBPIC_RECORDLIST_OUT        "/rom/mbrecord/mb_out.bmp"
#define MBPIC_RECORDLIST_UN         "/rom/mbrecord/mb_unan.bmp"

#define MBPIC_INCOMINGCALL         PIC_NA//"/rom/mbphone/incomingcall.gif"
#define MBPIC_OUTGOINGCALL         PIC_NA//"/rom/mbphone/outgoingcall.gif"
#define MBPIC_ALSICON1             "/rom/mbphone/als1.ico"
#define MBPIC_INCALL               "/rom/mbphone/call_i_active.bmp"
#define MBPIC_INCALLONHOLD         "/rom/mbphone/call_i_held.bmp"
#define MBPIC_INCALLTWO             "/rom/mbphone/incalltwo.gif"
#define MBPIC_CALLEND               PIC_NA
#define RIGHT_ICON                  "/rom/mbrecord/right.bmp"
#define LEFT_ICON                  "/rom/mbrecord/left.bmp"

#define PIC_INCOMINGCALLDIVERT      PIC_NA

#define SUB_CAPTION_RECT                   0, 0, 175, 34
#define SUB_CAPTION_ICON_LEFT_RECT         1, 4, 22, 30
#define SUB_CAPTION_ICON_RIGHT_RECT        154, 4, 175,30
#define SUB_CAPTION_ICON_MIDDLE_RECT       35, 4, 137, 30
#define LINE1_RECT                         0, 35, 175, 60
#define LINE2_RECT                         0, 62, 175, 87
#define LINE3_RECT                         0, 89, 175, 114
#define LINE4_RECT                         0, 116, 176, 141
#define PHOTO_RECT                         0, 35, 176, 87//49, 35, 78, 87

#define MB_WINDOW_SIZE   0,45,176,175
typedef struct initpaint_struct{
	RECT  subcap;
	RECT  subcapleft;
	RECT  subcapright;
	RECT  subcapmid;
	RECT  line1;
	RECT  line2;
	RECT  line3;
	RECT  line4;
	RECT  photorect;
}INITPAINTSTRUCT;

int GetNetworkStatus(void);//获得网络状态,0:无网络,1:有网络
void PostRingMessage(void);
BOOL CreateSendDTMFWindow(char * pszDTMF);
void    CallingMute(BOOL bMute);
BOOL    GetMuteStatus(void);
int     GetItemTypeNum(int iType);
BOOL OperateLinkError(HWND hWnd,WPARAM wParam,LPARAM lParam);
int MB_GetSendLocalNumber(void);
BOOL IsCallingWnd();
BOOL GetAppCallInfo(void);
HINSTANCE ReturnHinstance(void);//返回电话实例
BOOL    EarphoneStatus(void);
void    bInitMBDialup(void);
void    bInitMBLink(void);
void    InitMBCalling(void);
BOOL    APP_CallPhone(const char * pPhoneNumber);
BOOL    APP_CallPhoneNumber(const char * pPhoneNumber);
BOOL    MBCallEndWindow(HWND hpre, const char * pName,const char * pPhoneCode,const char * pDuration,const char * pCost);
BOOL SuperAddDialNumber(HWND hpre, const char * pPhoneNumber,BOOL bSuperAdd,BOOL bCanSend, WORD keycode);
void    SetCostLimit(BOOL bLimit);
void    SetHangupSingalInfo(BOOL bSingalInfo);//当前电话断连是否是因为没有信号
BOOL    MBCallingWindow(HWND hpre, const PHONEITEM * pPhoneItem,const char * pExtension,BOOL bHeadset);
BOOL    IsWindow_MBCallingOne(void);
BOOL    IsWindow_MBCallingMult(void);
BOOL    IsWindow_MBRing(void);
BOOL    IsWindow_DialLink(void);
BOOL    IsWindow_Dialup(void);
BOOL    MBPhoneRing(const CALLS_INFO * pNewItemInfo);
void    GetNewItemInfo(CALLS_INFO * pNewItemInfo);
void    SetnSelItem(int n);
int     GetUsedItemNum(void);
int     GetPhoneGroupNum(void);
int     AfterHangup_MultOne(int iIndex);
void    ResetnUnanswered(void);
BOOL    MBCalling_Shortcut(int iType,const char * pShortPhone);
int     GsmSpecific(const char * pNumber);
BOOL    IsAllType_Specific(int iCallType);
void    SetListCurCalls(BOOL bListCur);
BOOL    GetListCurCalling(void);
BOOL    MBListCurCalls(OPERATETYPE OperateType,WPARAM wParam,LPARAM lParam);
void    MBRingAnswerCall(void);
BOOL    GetInitState(void);
int     IfEmergencyString(const char ** pArray,const char * pNumber);

void    MessageMBRing( UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
void    MessageMBDialUp(UINT wMsgCmd,WPARAM wParam,LPARAM lParam);
void    MessageMBCallingOne(UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
void    MessageMBPostMsg(int iCallMsg,WPARAM wParam, LPARAM lParam);
void    MessageMBPostMsgCallForward(void);
BOOL    APP_CallPhoneNumberSTK(const char * pPhoneNumber);
BOOL    SendAbortCommand(WPARAM wParam,LPARAM lParam);
BOOL    CreateMBGSMWindow(void);
//BOOL    CallMBDialLinkingWindow(const char * pPhoneCode,const SYSTEMTIME * pDialupTime);
BOOL    CallMBDialLinkingWindow(HWND hframe, const char * pPhoneCode,const SYSTEMTIME * pDialupTime,BOOL bSTK);
void    GetCurrentPhoneNum(char * pPhoneNum,unsigned int iBuflen);
BOOL    GetSceneModeCall(void);
BOOL    ReadMobileConfigFile(MBConfig * pConfig);
void    ReadMobileConfig(MBConfig * pConfig);
BOOL    MB_SetHeadphone(HWND hwndCall,int iDir);
BOOL    GSMDealWith(const char * cPhoneNumber,BOOL bExecuteAtOnce);
BOOL    USSDDealWith(const char * pString,BOOL bCalling);
BOOL GSM_PhoneBookDealWith(HWND hEdit, const char * pPhoneNumber, BOOL * IsFind);
BOOL    BTAnswerHangup(int iType);//执行挂断,接听操作
void    BT_AnswerCall(void);//蓝牙接听
void    BT_Hangup(void);//蓝牙挂断
void	BT_DialOut(void);//蓝牙拨出
BOOL    DialLink_Hangup(void);//挂断拨号
void    BT_Terminal(void);//远端忙
void    BT_RemoteAnswer(void);//远端接听

void    MBPlayMusic(int iMult);

void    SetForward_All(BOOL bForward);
void    SetForward_Uncdtional(BOOL bForward);
void    SetForward_Condition(BOOL bForward);
void    SetForward_Busy(BOOL bForward);
void    SetForward_NoReply(BOOL bForward);
void    SetForward_NoReach(BOOL bForward);
BOOL    GetForward_Condition(void);
BOOL    GetForward_Uncdtional(void);

void    EnableState(void);
void    DisableState(void);

int     TimeCmp(const SYSTEMTIME * pTime1,const SYSTEMTIME * pTime2);
char  * ecvt(double value,int ndigit,int * dec,int * sign);
char  * MB_Mtoa(double dNumber,char * pNumber);
double  MB_atof(const char * pString);
void    MB_DealDouble(char * pSour,unsigned int iBit);
BOOL    FileIfExist(const char * pFileName);
BOOL    StrIsNumber(const char * pStr);
BOOL    StrIsDouble(const char * pStr);
BOOL    IfValidPhoneNumber(const char * pPhoneNumber);
void    PlayClewMusic(unsigned int iPlayCount);
BOOL    GetPhoneItem(PHONEITEM * pPhoneItem,const char * pNumber, ABINFO *pabInfo);
int     InStrArray(const char * pSearchStr,const char ** pArray);
int     InIntArray(int iSearchInt,const int * pArray,size_t count);
BOOL    NumberIsEmergent(const char * pNumber);
void    SetLibrateIfSwitch(void);

BOOL    MBWaitWindowState(BOOL bState);
BOOL    CountControslYaxisExt(int * piConY,unsigned int iControlH,size_t count,int iDirect,unsigned int iSpace);
BOOL    CountControlsYaxis(int * piConY,unsigned int iControlH,size_t count);
BOOL    CountControlsAbscissa(int * pConX,unsigned int iControlW,size_t count);
int     StrAutoNewLine(HDC hdc,const char * cSourstr,char * cDesstr,const RECT * pRect,UINT uFormat);
int     GetCharactorHeight(HWND hWnd);
int     GetStringWidth(HDC hdc,const char * pString);
int     GetScreenUsableWH1(int iwh);
void    GetScreenUsableWH2(int * pwidth,int * pheight);
void    AppMessageBox(HWND hwnd,const char * szPrompt,const char * szCaption,int nTimeout);
BOOL    AppConfirmWin(HWND hwnd,const char * szPrompt,const char * szTipsPic,
                      const char * szCaption,const char * szOk,const char * szCancel);
char *  RemoveStringEndSpace(char * pStr);

int     GetEditControlHeight(void);
int     GetButtonControlHeight(void);
int     GetInputSoftKeyboardPlace(void);
void    EndPicBrowser();
BOOL    IsSpareTimeLeft(void);
BOOL    EndRecorderUnvisualable(void);
BOOL    StartRecorderUnvisualable(HWND hWnd, UINT uRetMsg);

void    PhoneSetChannel(BOOL bSet);
void    PhoneSetChannelCalling(BOOL bSet);
int     GetRadioButtonControlWidth(const char * pString);
BOOL IsRingWnd();
BOOL IsDialupWnd();
BOOL IsCallingWnd();
BOOL IsCallEndWnd();
BOOL IsGSMWndShow();
void CloseRingWnd();
void CloseCallEndWnd();
void CloseCallingWnd();
void CloseDialLinkWnd();
void CloseDialupWnd();
void CloseRingWnd();
void HideGSMWnd();
extern int GetSIMState();
void InitPaint(INITPAINTSTRUCT * ppaintif, BOOL bphoto, BOOL bicon);
void InitTxt(HDC hdc, char * srctxt, char * showtxt, int width, BOOL fend);
BOOL GetFontHandle(HFONT *hFont, int nType);
BOOL IsCallAPP(void);
void InitPhoto(INITPAINTSTRUCT * ppaintif, SIZE *psize);

//BlueTooth begin
BT_CONNECTED_STATUS    BT_EquipmentConnected(void);//蓝牙设备是否已经连接
void    SetBlueToothStatus(void);//检测到蓝牙设备,设置右softkey文字
void    SetBlueToothButtonInfo(AudioInterfaceType AudioType);//通道切换成功之后调用用于设置文字
void    BT_SwitchChannel(AudioInterfaceType AudioType);//切换通道类型
void    BT_RemoveCall(void);//来电自行挂断调用函数
void    BT_HangupNetwork(void);//无信号挂断调用
void	SetBlueToothClear(void);//
BOOL	AnswerCallFromBT(void);
BT_STATUS	BT_GetStatus(void);
void	SetBTDialout(BOOL b);
BOOL	GetBTDialout(void);
void	SetAnswerCallFromBT(BOOL b);
void    BT_CopyRing(const char * pNumber);
//BlueTooth end
#endif
