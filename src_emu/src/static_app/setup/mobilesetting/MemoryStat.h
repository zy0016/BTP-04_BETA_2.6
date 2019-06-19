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
#include    "winpda.h"
#include    "str_plx.h"
//#include    "fapi.h"
#include	"sys/types.h"
#include	"sys/stat.h"
#include	"sys/vfs.h"
#include	"fcntl.h"
#include	"unistd.h" 
#include	"dirent.h"
#include    "setting.h"
#include    "str_public.h"
#include    "plx_pdaex.h"
#include    "setup.h"
#include    "pmi.h"
#include    "malloc.h"

#define     IDC_BUTTON_QUIT         3//退出按钮
#define     IDC_BUTTON_SET          4//确定按钮
#define     IDC_LIST                5

#define     TITLECAPTION            GetString(STR_SETUP_MEMORYSTAT_CAPTION)//空间查看
#define     BUTTONOK                (LPARAM)GetString(STR_WINDOW_OK)    //确定
#define     BUTTONQUIT              (LPARAM)GetString(STR_WINDOW_EXIT)  //退出
#define     ERROR1                  GetString(STR_SETUP_ERROR1)         //出错提示
#define     WINDOWICO               (LPARAM)""   //窗口图标

#define     MEMORYTXT0              GetString(STR_SETUP_MEMORYSTAT_TXT1)//MMC卡使用情况
#define     MEMORYTXT1              GetString(STR_SETUP_MEMORYSTAT_TXT0)//空间使用情况"

#define     LISTTXT0                GetString(STR_SETUP_MEMORYSTAT_LISTTXT0)//"短信"
#define     LISTTXT1                GetString(STR_SETUP_MEMORYSTAT_LISTTXT1)//"彩信"
#define     LISTTXT2                GetString(STR_SETUP_MEMORYSTAT_LISTTXT2)//"联系人
#define     LISTTXT3                GetString(STR_SETUP_MEMORYSTAT_LISTTXT3)//"记事本
#define     LISTTXT4                GetString(STR_SETUP_MEMORYSTAT_LISTTXT4)//"速记"
#define     LISTTXT5                GetString(STR_SETUP_MEMORYSTAT_LISTTXT5)//"日程"
#define     LISTTXT6                GetString(STR_SETUP_MEMORYSTAT_LISTTXT6)//"图片"
#define     LISTTXT7                GetString(STR_SETUP_MEMORYSTAT_LISTTXT7)//"录音机
#define     LISTTXT8                GetString(STR_SETUP_MEMORYSTAT_LISTTXT8)//"游戏"

typedef struct 
{
    DWORD iUsedSMS;                 //已收短信k数
    DWORD iTotalSMS;                //短信最大k数

    DWORD iUsedMMS;                 //已收彩信k数
    DWORD iTotalMMS;                //彩信最大k数

    DWORD iUsedLinkman;             //联系人以用k数
    DWORD iTotalLinkman;            //联系人最大k数

    DWORD iUsedNodepad;             //记事本
    DWORD iTotalNodepad;            //

    DWORD iUsedSteno;               //速记
    DWORD iTotalSteno;              //

    DWORD iUsedCale;                //日程
    DWORD iTotalCale;               //

    DWORD iUsedPic;                 //图片
    DWORD iTotalPic;                //

    DWORD iUsedRec;                 //录音机
    DWORD iTotalRec;                //

    DWORD iUsedGame;                //游戏
    DWORD iTotalGame;               //
}MEMORYSTAT;

extern  DWORD   GetAllTextSize();       //记事本
extern  DWORD   GetLenthOfStenoFiles(); //速记
extern  int     GetSMSSpace(void);      //短信
extern  int     APP_GetPhoneBookFileSize(void);//获得联系人文件尺寸,文件尺寸(KB). -1 = 失败。
extern  int     APP_GetMMSSize(void);   //彩信
extern  DWORD   GetRecorderFileSize();  //获得录音机所有文件所占空间
extern  DWORD   GetAllImageFileSize();  //获得图片所占空间
extern  DWORD   GetDatebookFileSize( void );//日程内存空间查看
