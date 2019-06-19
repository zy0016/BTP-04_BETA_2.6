#ifndef _SETTING_H
#define _SETTING_H

#include "window.h"
#include "plxmm.h"
#include "prioman.h"
#include "me_wnd.h"
#ifndef _863BIRD_
#define _863BIRD_       // bird项目宏定义
#endif

short APP_GetSIMRcdByOder ( short iOrder, PPHONEBOOK pRtnRcd, short MaxNum );
BOOL  Setup_InitFunction(void);//提供给程序管理器,用于初始化设置各个参数

#define     SN_GETBARCOLOR    0x01
#define     SN_GETWINCOLOR    0x02
#define     SN_GETFOCUSCOLOR  0x03
COLORREF    SetupGetDeskColor(int iIndex);//获得当前设置颜色值

//*********************************当前风格部分**************************
#define		STYLE_COM					1//商务风格
#define		STYLE_PASTIME				0//娱乐风格

/**********读取拨号设置******************************************/
#define     DIALDEF_WAP      1
#define     DIALDEF_MAIL     2
#define     DIALDEF_MMS      3
/*拨号的UDB设置信息*/
#define     DIALFILE              "/mnt/flash/setup/DialCfg.mal"
#define     SN_CONNSETT           "Sett_ConnInfo"
#define     KN_CONNID             "Sett_ConnID"



#define     UDB_K_DNS_SIZE        31
#define     UDB_K_NAME_SIZE       31
#define     UDB_K_PWD_SIZE        31
#define     UDB_K_TEL_SIZE        41
#define     UDB_K_PORT_SIZE       4
#define     UDB_HOMEPAGE_SIZE     1023
/* UDB数据结构 */
typedef struct
{
    unsigned long       ISPID;                      /*  ISP的ID号           */
    unsigned long       PrimarySelect;              /*  是否首选ISP         */
    unsigned long       ProxyFlag;                  /*  是否使用代理服务器  */
    unsigned long       ProxyID;                    /*  代理服务器ID            */
    unsigned long       DtType;                     /*  传输类型GPRS/CSD    GPRS: 1, CSD: 0*/

    char    ISPName     [ UDB_K_NAME_SIZE + 1 ];    /*  ISP名               */
    char    ISPUserName [ UDB_K_NAME_SIZE + 1 ];    /*  ISP登录用户名       */
    char    ISPPassword [ UDB_K_PWD_SIZE  + 1 ];    /*  ISP登录密码         */
    char    PrimaryDns  [ UDB_K_DNS_SIZE  + 1 ];    /*  主域名服务器            */
    char    SlaveDns    [ UDB_K_DNS_SIZE  + 1 ];    /*  辅助域名服务器      */

    char    ISPPhoneNum1[ UDB_K_TEL_SIZE+1 ];       /*  连接ISP的电话号码   */
    char    ISPPhoneNum2[ UDB_K_TEL_SIZE+1 ];       /*  连接ISP的电话号码   */
    char    ISPPhoneNum3[ UDB_K_TEL_SIZE+1 ];       /*  连接ISP的电话号码   */
    //char    ISPPhoneNum4[ UDB_K_TEL_SIZE+1 ];       /*  连接ISP的电话号码   */
    
    unsigned int        AuthenType; //0: Secure; 1: Normal
    DWORD               ConnectGateway; //IP address: OxFFFFFFFF; 
    char                ConnectHomepage[UDB_HOMEPAGE_SIZE + 1];
    char                GPRSAccPoint[UDB_K_DNS_SIZE + 1];
    unsigned long       DatacallType; //0: Analogue; 1: ISDN v.110; 2: ISDN v.120
    unsigned long       MaxDataSpeed; //0: Automatic; 1: 9600bps; 2: 14400bps;
    char                ConnPort[UDB_K_PORT_SIZE+1];
}UDB_ISPINFO;

typedef struct tag_ConnIndexTbl
{
	char ConnInfoName[UDB_K_NAME_SIZE + 1];
	int iIndex;
	int iListIndex;
	int nType;//GPRS or CSD
}ConnIndexTbl;

BOOL    Setup_ReadDefDial(UDB_ISPINFO  * ,int );//提供给拨号,直接读取默认配置
void    SetUp_SetDialCfg(void);                 //提供给程序管理器,第一次时调用来建立拨号设置文件
int     Setup_GetDialType(int nType);           //得到默认拨号类型
BOOL    SetConnectionUsage(int nType, int nIndex);

/**************************************************************/
BOOL    Setup_ShowPenCalib();//调用笔校准
//**********************************时间格式*************************************************
#define     SN_TIMEMODE                 "timesetting"

#define     KN_TIMEMODE                 "timemodeID"
#define     KN_DATEMODE                 "datemodeID"
#define     KN_TIMESRC                  "timesrcID"


typedef enum
{
    TF_12,  //当前为12小时制
    TF_24   //当前为24小时制
}TIMEFORMAT;
TIMEFORMAT      GetTimeFormt(void);//返回0:当前为24小时制,1:当前为12小时制

typedef enum
{
	DF_DMY_DOT,//dd.mm.yyyy 
	DF_MDY_DOT,//mm.dd.yyyy
	DF_YMD_DOT,//yyyy.mm.dd 
	DF_DMY_SLD,//dd/mm/yyyy 
	DF_MDY_SLD,//mm/dd/yyyy 
	DF_YMD_SLD,//yyyy/mm/dd 
	DF_DMY_DSH,//dd-mm-yyyy
	DF_MDY_DSH,//mm-dd-yyyy 
	DF_YMD_DSH //yyyy-mm-dd
}DATEFORMAT;
DATEFORMAT      GetDateFormt(void);

typedef enum
{
    GPS_BASED,
    USER_DEFINED,
}TIMESOURCE;
TIMESOURCE   GetTimeSource(void);

BOOL GetTimeDisplay(SYSTEMTIME st, char* cTime, char* cDate);

typedef enum
{
    TZ_HOME,
    TZ_VISIT
}TIMEZONE;

TIMEZONE GetUsingTimeZone(void);//取得当前使用的时区


#define SN_TIMEZONE "timezone"
#define KN_TIMEZONE "timezoneID"
#define KN_HZSTRING "homezoneID"
#define KN_VZSTRING "visitzoneID"

//************************************密码设置所需部分**************************************************
typedef enum
{
    PWSV_LOCKING,//设置部分中现在上锁时的手机密码验证
    PWSV_STARTUP //启动手机时的手机密码验证          
}PASSWORD_VALIDATE;
BOOL    CallPwsValidate(HWND hPasswordValidate,PASSWORD_VALIDATE itype);//itype:当前需要验证的密码类型
//本函数显示密码验证的窗口,窗口中显示一个EDIT控件和"确定","取消"按钮,提示用户输入手机密码.
//itype:当前需要验证的密码类型
//例如在开机时要验证手机密码,只需直接调用这个函数就可以了:CallPwsValidate(hwnd,PWSV_STARTUP),
//如果当前开机密码设置为关闭,函数会自动退出,不会提示输入密码,否则会提示输入密码,密码输入正确之后,才会退出函数.

#define     PASSWORD_DEFAULT        "0000"    //Default password of phone

//***********************************Unit设置所需部分************************************************
#define  SN_UNIT            "UnitSetting"
#define  KN_UNIT_DISTANCE   "DistanceUnitSetting"
#define  KN_UNIT_SPEED      "SpeedUnitSetting"
#define  KN_UNIT_ELEVATION  "ElevationUnitSetting"

typedef enum 
{
    DIS_METERS,       //Meters
    DIS_I_MILES,      //Imperial miles
    DIS_I_YARDS,      //Imperial yards
    DIS_N_MILES       //Nautical miles
}UNIT_DISTANCE;

UNIT_DISTANCE GetUnitDistance(void);

typedef enum
{
    KM_PER_HOUR,        //Kilometers/hour
    METER_PER_SEC,      //Meters/second
    MILES_PER_HOUR,     //Miles/hour
    KNOTS               //Knots
}UNIT_SPEED;

UNIT_SPEED GetUnitSpeed(void);

typedef enum
{
    ELE_METERS,             //Meters
    ELE_I_FEET              //imperial feet
}UNIT_ELEVATION;

UNIT_ELEVATION GetUnitElevation(void);

typedef enum
{
    DIS_UNIT_LIST,
    SPID_UNIT_LIST,
    ELE_UNIT_LIST
}UNIT_TYPE;

//***********************************Unit设置所需部分************************************************

//***********************************音乐设置部分***********************************************
void    GetOpenRing(char *);                //获得开机音乐的函数(char *)长度为:RINGNAMEMAXLEN + 1
void    GetCloseRing(char *);               //获得关机音乐的函数(char *)长度为:RINGNAMEMAXLEN + 1
//***********************************铃声选择接口部分***********************************************
#define     RINGNAMEMAXLEN          400     //铃声文件名数组的长度(包括路径)
#define     RINGNAMEONLYLEN         100     //铃声文件名数组的长度(不包括路径)
#define     CAPTIONMAXLEN           20      //窗口标题最大长度

typedef enum
{
    RINGTYPE_CALL   = 0,//来电
    RINGTYPE_SMS    = 1,//短信铃声
    RINGTYPE_MMS    = 2,//彩信铃声
    RINGTYPE_OPEN   = 3,//开机铃声
    RINGTYPE_CLOSE  = 4//关机铃声
}RINGSELECTTYPE;

typedef struct
{
    RINGSELECTTYPE  iRingType;
    HWND            hCallWnd;
    int             hCallMsg;
    char            titlecaption[CAPTIONMAXLEN + 1];
    char            pSelRingname[RINGNAMEMAXLEN + 1];
}FUNCRINGSELECT;
BOOL    CallRingSelect(const FUNCRINGSELECT * ringselect);//显示铃声列表
void    GetReplyData(char * ringname,LPARAM lParam ,unsigned int buflen);
/*
CallRingSelect函数显示一个窗口,窗口中有一个列表框,当前ROM,FLASH2中的铃声文件显示在列表框中。
对于ROM的铃声,显示其对应的中文名称,对于FLASH2中的铃声,只显示其实际文件名.
用户选择窗口的"确定"按钮之后,由回调消息返回选中的铃声的实际文件名包括路径(需要调用GetReplyData函数).
用户选择窗口的"取消"按钮之后,回调消息中的LPARAM=0
函数执行成功WPARAM=1
函数执行失败WPARAM=0
iRingType   :需要显示的铃声的类别,RINGTYPE_CALL:来电,RINGTYPE_SMSMMS:短信/彩信铃声,
                                RINGTYPE_OPEN:开机铃声,RINGTYPE_ALARM:日程表闹铃的铃声
hCallWnd    :回调模块的指定窗口句柄
hCallMsg    :回调模块的指定消息响应
titlecaption:窗口的标题
pSelRingname:需要选中铃声的文件名

返回说明：
接收到返回消息后需要调用GetReplyData(char * ringname,LPARAM lParam ,unsigned int buflen)返回结果。
ringname:返回选中的文件名字符串
LPARAM:回调消息的lParam
buflen:调用方定义的需要返回的字符串的长度.注意:没有边界检查.
*/
//**************************************开机短语/屏保短语************************
#define     DESK_LIMIT      12          //开机短语字符串最大长度(6个汉字/12个英文)
#define     SCREEN_LIMIT    12          //屏保短语字符串最大长度(6个汉字/12个英文)
void    GetDeskPhrase  (char * );       //获得开机短语的函数(char * 的长度应为DESK_LIMIT + 1)
void    GetScreenPhrase(char * );       //获得屏保短语的函数(char * 的长度应为SCREEN_LIMIT + 1)
//*********************************动画设置部分**************************
#define     CARTOONFILENAMELEN      400
typedef struct
{
    char    cOpen[CARTOONFILENAMELEN + 1];      //开机动画文件名
    char    cClose[CARTOONFILENAMELEN + 1];     //关机动画文件名
    char    cScreen[CARTOONFILENAMELEN + 1];    //屏保动画文件名
    char    cCall[CARTOONFILENAMELEN + 1];      //来电动画文件名
}CARTOON;
//BOOL    GetCartoon(CARTOON * cartoon);//获得动画设置结构信息

typedef struct tagMobileConfig
{
    BOOL    bAutoRedial;//自动重拨
    int     iAutoRedial;//自动重拨的次数
    BOOL    bGetClew;   //接通提示
    BOOL    bModeTurn;  //翻盖接听
    BOOL    bModeAnyKey;//按任意键接听
    BOOL    b50Tip;     //50秒提示音
    BOOL    bModeAuto;  //自动接听
    int     ALS;
#define     ALS_LINE1	0
#define     ALS_LINE2	1
#define     MAX_LINENAME 40
	char    Line1Name[MAX_LINENAME];
	char    Line2Name[MAX_LINENAME];
	BOOL    bShowCallDuration;
	BOOL    bShowCallCost;
	BOOL    bCallSummary ;
}MBConfig, *PMBConfig;
BOOL    ReadMobileConfigFile(MBConfig* pConfig);

//********************************当前通讯模式***************************
BOOL    CallCommunicateWindow(HWND hwndCall,int icallmsg);
/*
hwndCall:调用方的指定窗口句柄.
icallmsg:调用方的指定消息响应.
该函数显示一个窗口,窗口中显示两个单选按钮,分别为"通讯模式","飞行模式".用户选择"确定"按钮之后窗口自动关闭
并向调用方发送消息,消息中的wparam = 1,lparam = 1
*/
typedef enum
{
    COMMUNICATION = 0,//通讯模式
    FLAY          = 1 //飞行模式
}MESSAGEMODE;
MESSAGEMODE     GetMessageMode(void);//获得当前通讯模式,0:通讯模式,1:飞行模式
//在没有SIM卡的情况下，用户两种模式均可选择，如果选择了通讯模式则只可以拨打紧急电话
//********************************红外接口设置***************************
BOOL    CallInfraredWindow(HWND);//提供给程序管理器,显示红外接口设置
//***************************************背景灯/液晶屏持续显示的时间设置部分**********************
int     GetBglight(void);                       
//获得背景灯点亮的持续时间,返回0表示"关闭",-1表示不关闭,否则表示持续多少秒
int     GetScreen(void);                    
//获得液晶屏幕持续显示的时间,返回0表示"关闭",-1表示不关闭,否则表示持续多少秒
int     GetSystemWait(void);
//获得系统待机的持续时间,返回0表示"关闭",-1表示不关闭,否则表示持续多少秒
//*****************************提供给其它应用****************************
void    BeginVibration(unsigned int iSecond);//实现振动iSecond秒
void    GetRingShowName(const char * ringname,char * cshowname);
//***************************************声音播放*****************************
#define AUDIOTYPE 6
typedef enum
{
    RING_PHONE      = 150,//来电提示音                                             
    RING_ALARM      = 145,//闹钟                                                   
    RING_CALENDAR   = 144,//日程                                                   
    RING_POWNON     = 140,//开机铃声                                               
    RING_POWNOFF    = 130,//关机音乐                                               
    RING_SMS        = 120,//短信                                                   
    RING_MMS        = 110,//彩信                                                   
    RING_WARNING    = 105,//警告音                                                 
    RING_CAMERA     = 103,//照相提示音                                             
    RING_KEY        = 100,//按键音                                                 
    RING_TOUCH      = 90, //接通提示音                                              
    RING_SCREEN     = 80, //触屏音                                                  
    RING_CLEW50     = 70, //50秒提示音                                              
    RING_OTHER      = 60, //播放其它声音,比如彩信,浏览器中的声音文件,铃声预览等一般应用
    RING_WAP        = 55, //浏览器中的声音文件或者数据
    RING_MMSAPP     = 54, //彩信应用中的文件或者数据  
    RING_PUBLIC     = 53, //铃声预览中的文件          
    RING_VOLUME     = 50, //测试音量用宏                                             
    RING_WITHOUT    = 0   //没有声音播放                                              
}MUSICTYPE;//铃声的类别
typedef enum
{
    AMR_TYPE        = 1,//amr文件
    MIDI_TYPE       = 2,//midi文件
    WAVE_TYPE       = 3,//wave文件
    MMF_TYPE        = 4,//mmf文件
    INVALID_TYPE    = 5,//不支持播放的声音文件(或者数据区)类别
    OPENFAIL        = 6 //文件打开失败
}FILETYPE;//铃声文件的类别
typedef enum
{
    PLAY_OVER  = 0x0001,//停止
    PLAY_BREAK = 0x0002 //中断
}PLAYEND_TYPE;//回调消息的类别

typedef struct              //铃声播放结构
{
    HWND            hWnd;       
    int             iWM_MMWRITE;
    int             iWM_MMSTOP; 
    int             iWM_MMGETDATA;
    int             iVolume;    
    FILETYPE        iType;  
    MUSICTYPE       uiringselect;
    unsigned char * pFileName;
    unsigned char * pBuffer;
    unsigned long   ulBufferSize;
}SetupHS_PlayMusic;
/*
结构说明
hWnd:       调用方的窗口句柄
iWM_MMWRITE:调用方定义的消息,调用方必须在该消息中调用Setup_WriteMusicData函数
            推荐这样调用:Setup_WriteMusicData((LPWAVEHDR)lParam);
iWM_MMSTOP: 调用方定义的消息,用于通知调用方铃声播放完毕.当铃声播放完毕的时候,
            iWM_MMSTOP消息中的wParam=PLAY_OVER表示音乐播放完毕,而且是正常结束
            调用方必须在该消息中调用Setup_EndPlayMusic(int uiringselect)函数关闭声卡等设备
            iWM_MMSTOP消息中的wParam=PLAY_BREAK表示音乐被其它应用打断,
iWM_MMGETDATA:调用方定义的消息,调用方必须在该消息中调用Setup_GetMusicData函数
            推荐这样调用:Setup_GetMusicData((LPWAVEHDR)lParam);
iVolume:    铃声音量
iType:      铃声文件的类型,参见上方的 FILETYPE
uiringselect:铃声的类别,参见上边的MUSICTYPE
pFileName:  铃声文件名
pBuffer:    数据存储区域
ulBufferSize:数据存储区域的长度
*/
typedef enum
{
    PLAYMUSIC_SUCCESS           ,//播放成功
    PLAYMUSIC_ERROR_PRI         ,//优先级不够
    PLAYMUSIC_ERROR_RINGTYPE    ,//非法的铃声类别
    PLAYMUSIC_ERROR_WAVEFORMAT  ,//非法的声音格式
    PLAYMUSIC_ERROR_OPENFILE    ,//文件打开失败
    PLAYMUSIC_ERROR_MALLOC      ,//malloc失败
    PLAYMUSIC_ERROR_PARAMETER   ,//非法的SetupHS_PlayMusic参数
    PLAYMUSIC_ERROR_PLAYFUNCTION,//调用播放函数失败
}PlayMusic_Result;//SETUP_PlayMusic函数返回值

PlayMusic_Result    SETUP_PlayMusic(SetupHS_PlayMusic * hs);//播放音乐,播放成功返回1,失败返回0
/*
该函数提供给其他应用播放音乐,
如果调用方能提供铃声文件名的话,在此为pFileName指针赋值,
ROM中铃声合法的文件名形式为:"/rom/setup/test1.amr"
FLASH中铃声合法的文件名形式为:"FLASH2:/test1/test1.amr"
其中setup,test1为目录,
请将pBuffer指针赋空,ulBufferSize清零
如果调用方提供的是数据存储区域的话,请将pFileName指针赋空
pBuffer指向数据存储区域,
ulBufferSize为数据存储区域的长度

需要注意的是:只有当uiringselect=RING_VOLUME的时候,iVolume才会生效,否则的话播放的音量由当前铃声的音量大小决定,
此外不必填写属性iType,程序会根据声音文件名或者数据存储区自动判断铃声文件类型
但是如果文件扩展名是"tmp",则文件类型由调用方即iType决定

必须在iWM_MMWRITE消息中调用Setup_WriteMusicData函数,
推荐这样调用:Setup_WriteMusicData((LPWAVEHDR)lParam);
必须在iWM_MMGETDATA消息中调用Setup_GetMusicData函数
推荐这样调用:Setup_GetMusicData((LPWAVEHDR)lParam);

iWM_MMSTOP消息
iWM_MMSTOP消息中的wParam=PLAY_OVER表示音乐播放完毕,而且是正常结束
在该消息中必须调用Setup_EndPlayMusic(int uiringselect)函数关闭声卡等设备
iWM_MMSTOP消息中的wParam=PLAY_BREAK表示音乐被其它应用打断.
*/
//int     Setup_WriteMusicData(LPWAVEHDR lpHdr);  
//播放的时候,给BUffer提供数据
//int     Setup_GetMusicData(LPWAVEHDR phdr);
//播放的时候,给BUffer提供数据
typedef enum
{
    STOPMUSIC_SUCCESS       ,//停止成功   
    STOPMUSIC_ERROR_PRI     ,//优先级不够
    STOPMUSIC_ERROR_UNKNOWN ,//未知的失败
}EndPlayMusic_Result;

EndPlayMusic_Result     Setup_EndPlayMusic(MUSICTYPE uiringselect);   
//提供给其他应用的停止播放音乐函数,用于关闭设备,uiringselect:当前铃声的类别,取值为上边的RING_SELECT
/*注意:当音乐正常播放完毕的时候,窗口会收到iWM_MMSTOP消息,只有当wParam=PLAY_OVER的时候才可以调用
Setup_EndPlayMusic函数,当wParam=PLAY_BREAK的时候不要再调用Setup_EndPlayMusic函数.
只有当使用SETUP_PlayMusic函数播放音乐的时候才可以使用Setup_EndPlayMusic函数停止音乐播放
*/
///////////////////////////////////////////////////////////////////////////////////
BOOL    Call_PlayMusic(MUSICTYPE uiringselect,unsigned int icount);
/*声音播放接口函数
uiringselect:播放类别,取值为上边的RING_SELECT
iCount:播放次数,0为循环播放,非零为播放次数
当播放的音乐不是循环播放的时候,音乐自己播放完毕的时候不必调用Call_EndPlayMusic函数或者Setup_EndPlayMusic
如果由Call_PlayMusic函数播放音乐的时候,需要中途停止音乐,则需要调用Call_EndPlayMusic函数
*/
BOOL    Call_EndPlayMusic(MUSICTYPE uiringselect);
/*声音停止接口函数,uiringselect:当前铃声的类别,取值为上边的RING_SELECT
Call_EndPlayMusic中的uiringselect取值要等于调用Call_PlayMusic函数时的uiringselect值
即比如播放来电音乐时为:Call_PlayMusic(RING_PHONE);
停止来电音乐时为:Call_EndPlayMusic(RING_PHONE);
只有当使用Call_PlayMusic函数播放音乐的时候才可以使用Call_EndPlayMusic函数停止音乐播放
*/

BOOL    SetCanPlayMusic_Key(BOOL bCanPlay);//设置当前是否可以播放按键音
BOOL    SetCanPlayMusic(BOOL bCanPlay,DWORD dPriority);//设置当前是否可以播放某个级别的声音
//工程模式部分,调用工程模式界面,参数可以为NULL
BOOL    CallProjectModeWindow(HWND hwndCall);
#endif
