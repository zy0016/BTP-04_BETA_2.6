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
#include "window.h"
#include "app.h"
#include "plx_pdaex.h"


/************************ START DEFINE APPCONTROL *************************/
////addressbook app
DWORD ADDRESSBOOK_AppControl(int nCode, void* pInstance, WPARAM wParam, LPARAM lParam);

////message app
DWORD MessageUnibox_AppControl(int nCode, void* pInstance, WPARAM wParam, LPARAM lParam);

//calllogs app
DWORD CallLogs_AppControl(int nCode, void* pInstance, WPARAM wParam, LPARAM lParam);

////Calendar
DWORD Date_AppControl(int nCode, void* pInstance, WPARAM wParam, LPARAM lParam);

////Navigation client
//DWORD Navigation_AppControl(int nCode, void* pInstance, WPARAM wParam, LPARAM lParam);

////Extra app group
DWORD Calc_AppControl(int nCode, void* pInstance, WPARAM wParam, LPARAM lParam);  //Calculator
DWORD Notepad_AppControl(int nCode, void* pInstance, WPARAM wParam, LPARAM lParam);  //NotePad
DWORD Exch_AppControl(int nCode, void* pInstance, WPARAM wParam, LPARAM lParam); //Currency converter
DWORD Clock_AppControl(int nCode, void* pInstance, WPARAM wParam, LPARAM lParam);//clock include alarm
DWORD PDAWorldClock_AppControl(int nCode, void* pInstance, WPARAM wParam, LPARAM lParam);

//wap browser app
DWORD WIE_AppControl(int nCode, void* pInstance, WPARAM wParam, LPARAM lParam);

////Tools app group
DWORD	Settings_AppControl (int nCode, void* pInstance, WPARAM wParam, LPARAM lParam ); //setting
DWORD   SetupDial_AppControl ( int nCode, void* pInstance, WPARAM wParam,LPARAM lParam );
DWORD Network_AppControl(int nCode, void* pInstance, WPARAM wParam, LPARAM lParam);
DWORD MemmoryManagement_AppControl(int nCode, void* pInstance, WPARAM wParam, LPARAM lParam );
DWORD Bluetooth_AppControl(int nCode, void* pInstance, WPARAM wParam, LPARAM lParam);
// GPS Monitor
DWORD GPSMonitor_AppControl(int nCode, void* hInst, WPARAM wParam, LPARAM lParam); 
DWORD SIMmailbox_AppControl(int nCode, void* pInstance, WPARAM wParam, LPARAM lParam);
DWORD SIMinfonumbers_AppControl(int nCode, void* pInstance, WPARAM wParam, LPARAM lParam);

//fun app group
DWORD PICBROWSER_AppControl(int nCode, void* pInstance, WPARAM wParam, LPARAM lParam);//pictures
//DWORD GameRus_AppControl(int nCode, void* pInstance, WPARAM wParam, LPARAM lParam); //game1
DWORD GameDiamond_AppControl(int nCode, void* pInstance, WPARAM wParam,  LPARAM lParam);
DWORD GamePao_AppControl(int nCode, void* pInstance, WPARAM wParam, LPARAM lParam);//game2
DWORD POKER_AppControl(int nCode, void* pInstance, WPARAM wParam,  LPARAM lParam);  //game3
DWORD GoBang_AppControl(int nCode, void* pInstance, WPARAM wParam, LPARAM lParam);//game4//
DWORD GamePushBox_AppControl(int nCode, void* pInstance, WPARAM wParam, LPARAM lParam); //game5
DWORD DepthBomb_AppControl(int nCode, void* pInstance, WPARAM wParam, LPARAM lParam);//game6

//DWORD Dlm_AppCtrl(int nCode, void* pInstance, WPARAM wParam, LPARAM lParam);
DWORD Dlm_AppControl(int nCode,void* pInstance, WPARAM wParam, LPARAM lParam);
DWORD MUSIC_AppControl(int nCode, void* pInstance, WPARAM wParam, LPARAM lParam);
DWORD SNDMANAGE_AppControl(int nCode, void* pInstance, WPARAM wParam, LPARAM lParam);

////Favorites app
DWORD Favor_AppControl(int nCode, void* pInstance, WPARAM wParam, LPARAM lParam);

///Help app
DWORD Help_AppControl(int nCode, void* pInstance, WPARAM wParam, LPARAM lParam);

////SIM toolkit
DWORD Stk_AppControl(int nCode, void* pInstance, WPARAM wParam, LPARAM lParam);

//Test 
DWORD	Test_AppControl(int nCode, void* pInstance, WPARAM wParam, LPARAM lParam);

//fufu usb modem
DWORD	USBmodem_AppControl(int nCode, void* pInstance, WPARAM wParam, LPARAM lParam);
//end usb modem

DWORD   MBDial_AppControl (int nCode, void* pInstance, WPARAM wParam, LPARAM lParam );

DWORD	WriteSMS_AppControl(int nCode, void* pInstance, WPARAM wParam, LPARAM lParam);
DWORD   EditMMS_AppControl (int nCode, void* pInstance, WPARAM wParam, LPARAM lParam );
DWORD   EditMail_AppControl (int nCode, void* pInstance, WPARAM wParam, LPARAM lParam );
DWORD   InstantViewSMS_AppControl(int nCode, void* pInstance, WPARAM wParam, LPARAM lParam );

//Location Module app
DWORD	LocationModule_AppControl( int nCode, void* pInstance, WPARAM wParam, LPARAM lParam );

/********************** DEFINE APPLICATION DETAIL***********************/
/*AddressBook app*/
/**********************************************************************************/

static const APPGROUP  AddBookApp =	
{
	"Contacts",
	"Contacts",
	"/rom/progman/app/mcontacts_43x28.bmp",		
	APP_TYPE,
	0,
	NULL,
	ADDRESSBOOK_AppControl,//Test_AppControl,
};

/*Message app*/
/**********************************************************************************/

static const APPGROUP  MsgApp =	
{
	"Messaging",
	"Messaging",
	"/rom/progman/app/mmessaging_43x28.bmp",
	APP_TYPE,
	0,
	NULL,
	MessageUnibox_AppControl,
};
/**********************************************************************************/

/*Call logs app*/
/**********************************************************************************/

static const APPGROUP  CalllogsApp =	
{
	"Logs",
	"Logs",
	"/rom/progman/app/mlogs_43x28.bmp",//mlogs_43x28.bmp		
	APP_TYPE,
	0,
	NULL,
	CallLogs_AppControl,
};

/*calender app*/
/**********************************************************************************/

static const APPGROUP  CalendarApp =	
{
	"Calendar",
	"Calendar",
	"/rom/progman/app/mcalendar_43x28.bmp",		
	APP_TYPE,
	0,
	NULL,
	Date_AppControl,//Test_AppControl,
};

/*Navigation client app*/
/**********************************************************************************/

static const APPGROUP  NaviClientApp =	
{
	"Navigation",
	"Navigation client",
	"/rom/progman/app/mnavigation_43x28.bmp",		
	APP_TYPE,
	0,
	NULL,
	NULL,//Navigation_AppControl,
};

/*Extra app group*/
/**********************************************************************************/
//Calculator
static const APPLICATION  CalcApp =	
{
	"Calculator",
	"Calculator",
	"/rom/progman/app/mcalculator_43x28.bmp",		
	0,
	Calc_AppControl,
};	
//Notepad
static const APPLICATION  NotepadApp =	
{
	"Notepad",
	"Notepad",
	"/rom/progman/app/mnotepad_43x28.bmp",		
	0,
    Notepad_AppControl,
};

//Currency converter
static const APPLICATION  ExchApp =	
{
	"Currency converter",
	"Currency converter",
	"/rom/progman/app/mcconverter_43x28.bmp",		
	0,
	Exch_AppControl,
};

//clock(includes alarm)
static const APPLICATION  ClockApp =	
{
	"Clock",
	"Clock",
	"/rom/progman/app/mclock_43x28.bmp",		
	0,
	Clock_AppControl,//Test_AppControl,
};

const APPLICATION*  ExtraTable[] =
{
	&CalcApp,
    &NotepadApp,
    &ExchApp,
    &ClockApp,
   	NULL,
};
static const APPGROUP  ExtrasGrp =	
{
	"Extras",
	"Extras",
	"/rom/progman/app/mextras_43x28.bmp",		
	DIR_TYPE,
	0,
	(APPLICATION **)ExtraTable,
	NULL,
};

/*wap browser app*/
/**********************************************************************************/

static const APPGROUP  WapApp =	
{
	"WAP",
	"WAP browser",
	"/rom/progman/app/mwapbrowser_43x28.bmp",		
	APP_TYPE,
	0,
	NULL,
	WIE_AppControl,
};

/*Tools app group*/
/**********************************************************************************/
//setting
static const APPLICATION  SettingApp =	
{
	"Settings",
	"Settings",
	"/rom/progman/app/msettings_43x28.bmp",		
	0,
	Settings_AppControl,//Test_AppControl,//SetupDial_AppControl,
};	
//Network services
static const APPLICATION  NetServApp =	
{
	"Network",
	"Network services",
	"/rom/progman/app/mnetservices_43x28.bmp",		
	0,
	Network_AppControl, //NetWork_AppControl,
};

//Memory management
static const APPLICATION  MemorymanadApp =	
{
	"Memory",
	"Memory management",
	"/rom/progman/app/mmemorymngt_43x28.bmp",		
	0,
	MemmoryManagement_AppControl,
};

//Bluetooth
static const APPLICATION  BluetoothApp =	
{
	"Bluetooth",
	"Bluetooth",
	"/rom/progman/app/mbluetooth_43x28.bmp",		
	0,
	Bluetooth_AppControl,
};

//GPS monitor
static const APPLICATION  GPSmonitorApp =	
{
	"GPS monitor",
	"GPS monitor",
	"/rom/progman/app/mgpsmonitor_43x28.bmp",		
	0,
	GPSMonitor_AppControl,
};

//SIM mailbox numbers
static const APPLICATION  SimmailApp =	
{
	"SIM mailbox",
	"SIM mailbox numbers",
	"/rom/progman/app/msimmailbox_43x28.bmp",		
	0,
	SIMmailbox_AppControl,
};

//SIM infomation numbers
static const APPLICATION  SiminfoApp =	
{
	"SIM info",//SIM infomation numbers
	"SIM infomation numbers",
	"/rom/progman/app/msiminfo_43x28.bmp",		
	0,
	SIMinfonumbers_AppControl,
};

const APPLICATION*  ToolsTable[] =
{
	&SettingApp,
	&NetServApp,
    &MemorymanadApp,
    &BluetoothApp,
	&GPSmonitorApp,
	&SimmailApp,
	&SiminfoApp,
   	NULL,
};

static const APPGROUP  ToolsGrp =	
{
	"Tools",
	"Tools",
	"/rom/progman/app/mtools_43x28.bmp",		
	DIR_TYPE,
	0,
	(APPLICATION **)ToolsTable,
	NULL,
};

/*Fun app group*/
/**********************************************************************************/
//pictures
static const APPLICATION  PicApp =	
{
	"Pictures",
	"Pictures",
	"/rom/progman/app/mpictures_43x28.bmp",		
	0,
	PICBROWSER_AppControl,
};	

//sounds
static const APPLICATION  SoundsApp =	
{
	"Sounds",
	"Sounds",
	"/rom/progman/app/msounds_43x28.bmp",		
	0,
	SNDMANAGE_AppControl,
};	

//game1
static const APPLICATION  GameRusApp =	
{
	"Diamond",//Game1
	"Diamond",
	"/rom/progman/app/mgame1_43x28.bmp",		
	0,
	GameDiamond_AppControl,//NULL,//GameRus_AppControl,
};

//game2
static const APPLICATION  GamePaoApp =	
{
	"Pao",
	"Pao",//Game2
	"/rom/progman/app/mgame2_43x28.bmp",		
	0,
	GamePao_AppControl,
};

//game3
static const APPLICATION  GamePockerApp =	
{
	"Poker",
	"Poker",//Game3
	"/rom/progman/app/mgame3_43x28.bmp",		
	0,
	POKER_AppControl,
};

//game4
static const APPLICATION  GameGoBangApp =	
{
	"GoBang",
	"GoBang",//Game4
	"/rom/progman/app/mgame4_43x28.bmp",		
	0,
	GoBang_AppControl,
};

//game5
static const APPLICATION  GamePushboxApp =	
{
	"Pushbox",
	"Pushbox",//Game5
	"/rom/progman/app/mgame5_43x28.bmp",		
	0,
	GamePushBox_AppControl,
};

//game6
static const APPLICATION  GameDepthBombApp =	
{
	"DepthBomb",
	"DepthBomb",//Game6
	"/rom/progman/app/mgame6_43x28.bmp",		
	0,
	DepthBomb_AppControl,
};

const APPLICATION*  FunTable[] =
{
	&PicApp,
	&SoundsApp,
	&GameRusApp,
    &GamePaoApp,
    &GamePockerApp,
    &GameGoBangApp,
	&GamePushboxApp,
	&GameDepthBombApp,
   	NULL,
};
static const APPGROUP  FunGrp =	
{
	"Fun",
	"Fun",
	"/rom/progman/app/mfun_43x28.bmp",		
	DIR_TYPE,
	0,
	(APPLICATION **)FunTable,
	NULL,
};

/*Favorites app*/
/**********************************************************************************/

static const APPGROUP  FavorApp =	
{
	"Favorites",
	"Favorites",
	"/rom/progman/app/mfavorites_43x28.bmp",	
	APP_TYPE,
	0,
	NULL,
	Favor_AppControl,
};

/*help app*/
/**********************************************************************************/

static const APPGROUP  HelpApp =	
{
	"Help",
	"Help",
	"/rom/progman/app/mhelp_43x28.bmp",		
	APP_TYPE,
	0,
	NULL,
	Help_AppControl,
};
/*SIM toolkit app*/
/**********************************************************************************/

static const APPGROUP  SimToolApp =	
{
	"Toolkit",
	"SIM toolkit",//visible only if sim support
	"/rom/progman/app/mstk_43x28.bmp",		
	APP_TYPE,
	0,
	NULL,
	Stk_AppControl,
};
/*SIM toolkit app*/
/**********************************************************************************/

static const APPGROUP  DlmApp =	
{
	"Manager",
		"Manager",
		"/rom/progman/app/mdlm_43x28.bmp",		
		APP_TYPE,
		0,
		NULL,
		Dlm_AppControl,
};
/*edit sms app*/
/**********************************************************************************/

static const APPGROUP  EdiSMSApp =	
{
	"Edit SMS",
	"Edit SMS",
	"/rom/progman/app/meditsms_43x28.bmp",		
	APP_TYPE,
	SHORTCUT,
	NULL,
	WriteSMS_AppControl,
};
/*edit mms app*/
/**********************************************************************************/

static const APPGROUP  EdiMMSApp =	
{
	"Edit MMS",
	"Edit MMS",
	"/rom/progman/app/meditmms_43x28.bmp",		
	APP_TYPE,
	SHORTCUT,
	NULL,
	EditMMS_AppControl,
};
/*edit email app*/
/**********************************************************************************/

static const APPGROUP  EdiEmailApp =	
{
	"Edit Email",
	"Edit Email",
	"/rom/progman/app/meditemail_43x28.bmp",		
	APP_TYPE,
	SHORTCUT,
	NULL,
	EditMail_AppControl,
};
/*edit class0 msg app*/
/**********************************************************************************/

static const APPGROUP  EditMsgApp =	
{
	"SMS",
	"SMS",
	"/rom/progman/app/meditmsg_43x28.bmp",		
	APP_TYPE,
	SHORTCUT,
	NULL,
	InstantViewSMS_AppControl,
};
/*call app*/
/**********************************************************************************/

static const APPGROUP  CallApp =	
{
	"Call",
	"Call",
	"/rom/progman/app/mcall_43x28.bmp",		
	APP_TYPE,
	HIDE_APP,
	NULL,
	MBDial_AppControl,
};
//fufu usb modem
static const APPGROUP  UsbModemApp =	
{
	"UsbModem",
	"UsbModem",
	"/rom/progman/app/musbmodem_43x28.bmp",		
	APP_TYPE,
	HIDE_APP,
	NULL,
	USBmodem_AppControl,
};	
//end usb modem
/*Location Module app*/
/**********************************************************************************/ 
static const APPGROUP  LocationModuleApp =
{
	"LocationModule",
	"LocationModule",
	"/rom/progman/app/mlocationmodule_43x28.bmp",
	APP_TYPE,
	HIDE_APP,
	NULL,
	LocationModule_AppControl
}; // KAo

/***************** START DEFINE MAIN GROUP TABLE *******************/
const APPGROUP* GrpTable[] =
{
	&AddBookApp,
	&MsgApp,
	&CalllogsApp,
	&CalendarApp,
	&NaviClientApp,
	&ExtrasGrp,		
	&WapApp,
	&ToolsGrp,
	&FunGrp,
	&FavorApp,
	&HelpApp,
	&DlmApp,
	&SimToolApp,
	&EdiSMSApp,
	&EdiMMSApp,
	&EdiEmailApp,
    &EditMsgApp,//class 0 message
	&CallApp,
	&UsbModemApp,
	&LocationModuleApp,
	NULL,				// Indicates table end, mustn't be deleted
};
