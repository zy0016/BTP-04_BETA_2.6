/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : hpime.h
 *
 * Purpose  : Defines IMEEDIT structure to create IME
 *            and declares function.
 *
\**************************************************************************/

#ifndef _HPIME_H
#define _HPIME_H

#define IME_ATTRIB_GENERAL   0x00000001L
#define IME_ATTRIB_GSM       0x00000002L
#define IME_ATTRIB_RECEIVER  0x00000005L
#define IME_ATTRIB_GSMLONG   0x00000012L
#define IME_ATTRIB_BASIC     0x00000008L

#define IMEEDIT_ATTRIB       0x00000100L
#define IMEEDIT_GSM          0x00000200L
#define IMEEDIT_IMENAME      0x00000400L
#define IMEEDIT_TITLE        0x00000800L
#define IMEEDIT_ALL          0x0000FF00L

#define IME_MSG_SETPARAM     (WM_USER + 0x0201)
#define IME_MSG_SPLITPAGE    (WM_USER + 0x0202)
#define IME_MSG_SHOWEDITEX   (WM_USER + 0x0203)

// 当指定IME_ATTRIB_RECEIVER属性时，CreateWindow包含指定以下窗口风格
#define WS_IMERECEIVER  WS_CHILD | ES_MULTILINE | ES_AUTOVSCROLL

typedef struct tagIMEEDIT
{
    HWND   hwndNotify;    // IMEEDIT控件的通知接收窗口句柄
    DWORD  dwAttrib;      // IMEEDIT控件属性，可设置的标识位见上
    DWORD  dwAscTextMax;  // 当IME_ATTRIB_GSM标识位置位时，该参数指定最大纯英文字符数
    DWORD  dwUniTextMax;  // 当IME_ATTRIB_GSM标识位置位时，该参数指定最大含中文字符数
	WORD   wPageMax;      // 当IME_ATTRIB_GSMLONG标识位置位时，该参数指定长短信中所允许的最大页面数
    PSTR   pszImeName;    // 输入法名称："手写"、"拼音"、"字母"、
                          // "数字"、"号码"、"笔画"、"符号"
    PSTR   pszCharSet;    // 允许输入的字符集合，该功能仅适用于“号码”输入法
    PSTR   pszTitle;      // IMEEDIT控件标题名称
    UINT   uMsgSetText;   // 一般指定为：IME_MSG_SETTEXT
                          // 当保存返回时，wParam值为1，lParam为指向字符串的指针
                          // 当取消退出时，wParam值为0，lParam被忽略
}
IMEEDIT, *PIMEEDIT;

/*********************************************************************\
* Function	   ImeCtrlInit
* Purpose      
* Params	   hInst
* Return	 	   
* Remarks	   
**********************************************************************/

BOOL ImeCtrlInit(HINSTANCE hInst);

#endif  // _HPIME_H
