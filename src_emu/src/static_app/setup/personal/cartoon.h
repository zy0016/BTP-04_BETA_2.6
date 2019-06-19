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
//#include    "window.h"
//#include    "string.h"
//#include    "stdlib.h"
//#include    "stdio.h" 
//#include    "winpda.h"
//#include    "str_plx.h"
////#include    "fapi.h"
//#include	"sys/types.h"
//#include	"sys/stat.h"
//#include	"fcntl.h"
//#include	"unistd.h" 
//
//#include	"dirent.h"
//
//#include    "setting.h"
//#include    "str_public.h"
//#include    "plx_pdaex.h"
//#include    "hpimage.h"
//#include    "setup.h"
//#include    "MBPublic.h"
//#define     IDC_BUTTON_QUIT         3//退出按钮
//#define     IDC_BUTTON_SET          4//确定按钮
//#define     IDC_BUTTON_START        5//启动
//#define     IDC_RADIO1              6
//#define     IDC_RADIO2              7
//#define     IDC_RADIO3              8
//#define     IDC_RADIO4              9
//#define     IDC_LIST                10
//#define     IDC_INIT                WM_USER + 100
//#define     IDC_PREVIEW             11//全屏预览按钮
//
//#define     TITLECAPTION            GetString(STR_SETUP_CARTOON_CAPTION)//动画设置
//#define     BUTTONOK                (LPARAM)GetString(STR_WINDOW_OK)    //确定
//#define     BUTTONQUIT              (LPARAM)GetString(STR_WINDOW_EXIT)  //退出
//#define     BUTTONSTARTTXT          (LPARAM)GetString(STR_WINDOW_STARTUP)//启动
//#define     ERROR1                  GetString(STR_SETUP_ERROR1)         //出错提示
//#define     WINDOWICO               (LPARAM)""    //窗口图标
//
//#define     RADIOTXT0               GetString(STR_SETUP_CARTOON_RADIOTXT0)//无
//#define     RADIOTXT1               GetString(STR_SETUP_CARTOON_RADIOTXT1)//开机
//#define     RADIOTXT2               GetString(STR_SETUP_CARTOON_RADIOTXT2)//关机
//#define     RADIOTXT3               GetString(STR_SETUP_CARTOON_RADIOTXT3)//屏保
//#define     RADIOTXT4               GetString(STR_SETUP_CARTOON_RADIOTXT4)//来电
//#define     CARTOONTXT              GetString(STR_SETUP_CARTOON_CARTOONTXT)//动画%d
//#define     SAVESMSUCC              GetString(STR_SETUP_SM_SETSUCC)     //设置成功
//#define     SAVESMERROR             GetString(STR_SETUP_SMSAVEFAIL)     //设置保存失败
//#define     BUTTON_PREVIEW          GetString(STR_SETUP_CARTOON_PREVIEW)//全屏预览
//#define     CLEW                    GetString(STR_SETUP_CARTOON_CLEW)//%s动画设置为空,是否继续?
//#define     SIZEINVALID             GetString(STR_SETUP_CARTOON_SIZEINVALID)//"合适的动画大小为240*120
//
//#define     SN_CARTOON              "cartoon"       //动画设置节点
//#define     KN_CARTOON_OPEN         "cartoon_open"  //开机关键字名
//#define     KN_CARTOON_CLOSE        "cartoon_close" //关机关键字名
//#define     KN_CARTOON_SCREEN       "cartoon_screen"//屏保关键字名
//#define     KN_CARTOON_CALL         "cartoon_call"  //来电关键字名
//
//typedef struct 
//{
//    int x1;
//    int y1;
//    int x2;
//    int y2;
//    int nHeight;
//    int nWidth;
//}ROUNDRECT;
//
//#define     DEFGIF0                 "/rom/setup/personal/g1.gif"
//#define     DEFGIF1                 "/rom/setup/personal/g2.gif"
//#define     DEFGIF2                 "/rom/setup/personal/g3.gif"
//#define     DEFGIF3                 "/rom/setup/personal/g7.gif"
//
//#define     CSHOWNAMEMAXLEN         20
//typedef struct cartoon
//{
//    char                cFilename[CARTOONFILENAMELEN + 1];  //动画实际文件名(包括路径)
//    char                cShowname[CSHOWNAMEMAXLEN + 1];     //动画的显示名称
//    struct cartoon  *   next;
//};
//typedef struct cartoon CARTOONS;
