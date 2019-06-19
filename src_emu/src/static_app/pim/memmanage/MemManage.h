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
#include    "stdio.h" 
#include "stdlib.h"
#include    "winpda.h"
#include    "str_plx.h"
//#include    "fapi.h"
#include	"sys/types.h"
#include	"sys/stat.h"
#include	"sys/vfs.h"
#include	"fcntl.h"
#include	"unistd.h" 
//#include	"dirent.h"
#include    "setup/setting.h"
#include    "str_public.h"
#include    "plx_pdaex.h"
#include    "setup/setup.h"
#include    "pmi.h"
#include	"mullang.h"
#include	"callapp/MBPublic.h"
#include "pubapp.h"

#define		ID_BACK					1001
#define		ID_OK					1002
#define     IDC_BUTTON_QUIT         3//退出按钮
#define     IDC_BUTTON_SET          4//确定按钮
#define     IDC_LIST                1005

#ifndef _EMULATE_
extern BOOL		MMC_CheckCardStatus();
#define		PHONEFLASH		"/mnt/flash"
#define		MMCFLASH		"/mnt/fat16"
#define		MMCNAMEPATH		"/mnt/fat16/name.ini"//"/mnt/fat16/mmc/name.ini"
#else
#define		MMC_CheckCardStatus()	1
#define		PHONEFLASH		"/mnt/flash"
#define		MMCFLASH		"/mnt/flash/mmc"
#define		MMCNAMEPATH		"/mnt/flash/mmc/name.ini"//"/mnt/fat16/mmc/name.ini"
#endif
/*
#define		PHONEFLASH		"/mnt/flash"
#define		MMCFLASH		"/mnt/fat16"
#define		MMCNAMEPATH		"/mnt/fat16/name.ini"//"/mnt/fat16/mmc/name.ini"
*/
#define		PHONEICON		"/rom/pim/mgm_phone_22x16.bmp"
#define		MMCICON			"/rom/pim/mgm_memorycard_22x16.bmp"

#define		IDS_PHONE				ML("Phone")
#define		IDS_PHONEMEM			ML("Phone memory")
#define		IDS_CARD				ML("Memory card")
#define     IDS_CAPTION				ML("Memory info")
#define		IDS_RENAMECAPTION		ML("Memory card name")
#define		PHONEVIEWNAME			"PLXPHONEMEMVIEWCLASS"
#define		CARDVIEWNAME			"PLXCARDMEMVIEWCLASS"
#define		RENAMEWNDCLASS			"PLXRENAMEWNDCLASS"
#define		IDS_DEFINENAME			ML("Please define name")
#define		IDS_SAVE			ML("Save")
#define		IDS_CANCEL			ML("Cancel")

#define     IDS_OK                ML("OK")//(LPARAM)GetString(STR_WINDOW_OK)    //确定
#define     IDS_BACK              ML("Exit")//(LPARAM)GetString(STR_WINDOW_EXIT)  //退出
#define     ERROR1                  ML("Failed to show the memory stat,try it later!")//GetString(STR_SETUP_ERROR1)         //出错提示
#define		ERROR2					ML("No memory card")
#define     WINDOWICO               (LPARAM)""   //窗口图标

#define     MEMCARD             ML("Memory card:")//GetString(STR_SETUP_MEMORYSTAT_TXT1)//MMC卡使用情况
//#define     MEMORYTXT1              ML("Total")//GetString(STR_SETUP_MEMORYSTAT_TXT0)//空间使用情况"

#define     LISTTXT0                ML("Total:")	//Total memory of phone in kilobytes.
#define     LISTTXT1                ML("Used:")		//Used memory of phone in kilobytes. 
#define     LISTTXT2                ML("Available:")//Available memory of phone in kilobytes.
#define     LISTTXT3                ML("Contacts:")	//The amount of memory, which Contacts uses.
#define     LISTTXT4                ML("Calendar:")	//The amount of memory, which Calendar uses.
#define     LISTTXT5                ML("Messaging:")//The amount of memory, which Messaging application uses.
#define     LISTTXT6                ML("Navigator:")//The amount of memory, which Navigator uses.
#define     LISTTXT7                ML("Notepad:")	//The amount of memory, which Notepad uses.
#define     LISTTXT8                ML("Images:")	//The amount of memory, which stored Images use.
#define		LISTTXT9				ML("Maps:")
#define		IDS_EDIT				ML("Edit")
enum
{
	MainView =1,
	PhoneMem =2,
	MMCardMem =3
};
typedef struct tagMEMORYSTAT
{
    DWORD iTotal;                 //Total memory
    DWORD iUsed;                //Used memory

    DWORD iAvailable;                 //Available memory
    float iContacts;                //The amount of memory, which Contacts uses.

    float iCalendar;             
    float iMessage;            
	float iNavigator;
    float iNodepad;             
	float iImages;
	float iMaps;

}MemStat,PMemStat;


