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

// ��ָ��IME_ATTRIB_RECEIVER����ʱ��CreateWindow����ָ�����´��ڷ��
#define WS_IMERECEIVER  WS_CHILD | ES_MULTILINE | ES_AUTOVSCROLL

typedef struct tagIMEEDIT
{
    HWND   hwndNotify;    // IMEEDIT�ؼ���֪ͨ���մ��ھ��
    DWORD  dwAttrib;      // IMEEDIT�ؼ����ԣ������õı�ʶλ����
    DWORD  dwAscTextMax;  // ��IME_ATTRIB_GSM��ʶλ��λʱ���ò���ָ�����Ӣ���ַ���
    DWORD  dwUniTextMax;  // ��IME_ATTRIB_GSM��ʶλ��λʱ���ò���ָ����������ַ���
	WORD   wPageMax;      // ��IME_ATTRIB_GSMLONG��ʶλ��λʱ���ò���ָ��������������������ҳ����
    PSTR   pszImeName;    // ���뷨���ƣ�"��д"��"ƴ��"��"��ĸ"��
                          // "����"��"����"��"�ʻ�"��"����"
    PSTR   pszCharSet;    // ����������ַ����ϣ��ù��ܽ������ڡ����롱���뷨
    PSTR   pszTitle;      // IMEEDIT�ؼ���������
    UINT   uMsgSetText;   // һ��ָ��Ϊ��IME_MSG_SETTEXT
                          // �����淵��ʱ��wParamֵΪ1��lParamΪָ���ַ�����ָ��
                          // ��ȡ���˳�ʱ��wParamֵΪ0��lParam������
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
