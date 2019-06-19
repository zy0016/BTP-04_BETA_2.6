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
#include    "window.h"
#include    "string.h"
#include    "stdlib.h"
#include    "stdio.h" 
#include    "winpda.h"
#include    "str_plx.h"
//#include    "fapi.h"
#include	"sys/types.h"
#include	"sys/stat.h"
#include	"fcntl.h"
#include	"unistd.h" 

#include    "setting.h"
#include    "str_public.h"
#include    "plx_pdaex.h"
#include    "dialer.h"
#include    "pubapp.h"
#include    "hpimage.h"
#include    "setup.h"

static  BOOL    SetDefaultDialIndex(int nType, int nIndex);
static  BOOL    DialSetDefault(HWND hwnd,HWND hList, 
                               HBITMAP hBmp,HBITMAP hGPRS,HBITMAP hCSD, int nType);
static  int     GetDefaultDialIndex(int nType);
BOOL    DeleteIspInfo(int iNum);
BOOL    IspSaveInfo(UDB_ISPINFO  * uIspInfo ,int iNum);
BOOL    IspReadInfo(UDB_ISPINFO  * uIspInfo ,int iNum);
int     IspIsNum(void);
int     IspGetNum(void);
BOOL    Setup_ReadDefDial(UDB_ISPINFO  *uIspInfo ,int nType);
int     Setup_GetDialType(int nType);
void    SetUp_SetDialCfg(void);

extern  BOOL    CallDialWindow(HWND hwndCall,int imsg,int iIndex);
extern  void    readdata();
//**********************************************************

#define     MAX_DIAL_NUM        5//ISP个数
#define     MAX_DEF             0//默认的ISP个数

#define     DIAL_TXT0       "移动梦网GPRS"
#define     DIAL_TXT1       "互联网GPRS"
#define     DIAL_TXT2       "移动梦网CSD"
#define     DIAL_TXT3       "互联网CSD"
