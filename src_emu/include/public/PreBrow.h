/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : PreBrow.h
 *
 * Purpose  : 预览多媒体文件界面头文件
 *            
\**************************************************************************/

#ifndef _PREBROW_H_
#define _PREBROW_H_

#include		"window.h"
#include		"string.h"
#include		"setup/setting.h"
//#include		"fapi.h"
#include		"sys/types.h"
#include		"sys/stat.h"
#include		"fcntl.h"
#include		"unistd.h" 
#include		"winpda.h"
#include		"malloc.h"
#include		"stdlib.h"
#include		"stdio.h" 
#include		"plx_pdaex.h"
#include		"str_plx.h"
#include		"str_public.h"
//#include		"hp_icon.h"


#define	RTN_NOSELECT	0		/*没有选中任何文件直接返回*/
#define RTN_IMAGE		4			/*图片文件类型*/
#define RTN_SOUND		1			/*声音文件类型*/
#define RTN_TEXT		2			/*文本文件类型*/
#define RTN_QTEXT		3			/*短语文件类型*/

#define PIC_PREBROW			0x0001
#define RING_PREBROW		0x0002
#define TEXT_PREBROW		0x0004
#define QTEXT_PREBROW		0x0008

#define	PREBROW_MASKGET		0x0001
#define	PREBROW_MASKDEL		0x0002
#define	PREBROW_MASKINSERT	0x0004
#define	PREBROW_MASKEDIT	0x0008

/*
 *	email interface
 */
typedef struct tagListAttNode{
    char *AttPath;
    char *AttName;
    struct tagListAttNode *pNext;
}LISTATTNODE, *PLISTATTNODE;

BOOL MAIL_CreateMultiInterface(HWND hFrameWindow, PLISTATTNODE InsertAtt);
/**********************************************************************\
通用直接预览接口
\**********************************************************************/
HINSTANCE APP_PreviewDirect(HWND hwnd, LPCTSTR lpOperation, LPCTSTR lpFile, 
                LPCTSTR lpParameters, LPCTSTR lpDirectory, INT nShowCmd);

BOOL	APP_PreDirectTxt(HWND hWnd, LPCTSTR pFileName);	//直接预览文本
BOOL	APP_PreDirectRing(HWND hWnd, LPCTSTR pFileName, BYTE bType);//直接预览铃声
BOOL	APP_PreDirectPic(HWND hWnd, LPCTSTR pFileName, int FileType);//直接预览图片
				
/**********************************************************************\
通用选择预览接口
\**********************************************************************/
HWND	APP_PreviewSelect(HWND hWnd, UINT returnmsg, DWORD dwFileStyle, DWORD dwMaskStyle);


/*********************************************************************\
* Function	SmPreBrow   
* Purpose   短语预览外部调用接口   
* Params	父窗口句柄，父窗口的接收消息，要保存的短语，要保存的短语的长度
            注意：如果需要不需要保存短语，则MsLen为0，InsertedMs为""即可
* Return	过向父窗口发送消息returnmsg，传递返回值,详细见Remarks
* Remarks	Wparam为0 ，returnmsg中lParam存放短语字符串指针，
* Remarks	1、当用户选择一个文本后返回：
               wParam的高位为1
			   wParam的地位存放文本的长度
               lParam存放文本串指针，
			2、当用户不进行任何选择“退出”时：
			   wParam的高位为0
			   wParam的低位位0
			   lParam无效（缺省为""） 
**********************************************************************/
//BOOL	APP_PreviewPhrase(HWND hFrame, HWND hWnd, UINT iRetMsg, const char * pInsertedPhrase, 
//						  int len, DWORD dMask);
BOOL	APP_PreviewPhrase(HWND	hFrame, HWND hWnd, UINT returnmsg, PCSTR szCaption);
BOOL	APP_EditPhrase(HWND	hFrame);
//BOOL	APP_PreviewPhrase(HWND hWnd, UINT returnmsg, const char * InsertedMs,  int MsLen, DWORD dwMask);

/*********************************************************************\
* Function	TxPreBrow   
* Purpose   文本预览外部调用接口   
* Params	父窗口句柄，父窗口的接收消息
* Return	过向父窗口发送消息returnmsg，传递返回值,详细见Remarks
* Remarks	1、当用户选择一个文件后返回：
               wParam的高位为1
			   wParam的地位存放文本文件名称的长度
               lParam存放文本文件名称串指针，
			2、当用户不进行任何选择“退出”时：
			   wParam的高位为0
			   wParam的低位位0
			   lParam无效（缺省为""） 
**********************************************************************/

BOOL	APP_PreviewText(HWND hFrame, HWND hWnd, UINT iRetMsg, DWORD dMask);

/*********************************************************************\
* Function	PICPreBrow   
* Purpose   图片预览外部调用接口   
* Params	父窗口句柄，父窗口的接收消息
* Return	过向父窗口发送消息returnmsg，传递返回值,详细见Remarks
* Remarks	1、当用户选择一个图片后返回：
              
			   wParam的地位存放图片文件名称的长度
               lParam存放图片文件名称串指针，
			2、当用户不进行任何选择“退出”时：没有返回消息
			  
**********************************************************************/

BOOL	APP_PreviewPicture(HWND hFrame, HWND hWnd, UINT iRetMsg, PCSTR szCap, PCSTR pFirstItem);
BOOL	APP_PreviewPictureEx(HWND hFrame, HWND hWnd, UINT returnmsg, PCSTR szCapion, PCSTR pFirstItem, PCSTR pFocusPic);

/*********************************************************************\
* Function	RGPreBrow   
* Purpose   铃声预览外部调用接口   
* Params	父窗口句柄，父窗口的接收消息,初始铃声的文件名称，允许删除标记
* Return	过向父窗口发送消息returnmsg，传递返回值,详细见Remarks
* Remarks	1、当用户选择一个铃声后返回：               
			   wParam的地位存放铃声文件名称的长度
               lParam存放铃声文件名称串指针，
			2、当用户不进行任何选择“退出”时：
			   没有返回消息
**********************************************************************/
BOOL	APP_PreviewRing(HWND hFrame, HWND hWnd, UINT iRetMsg, PSTR szCaption);




#endif //_PREBROW_H_
