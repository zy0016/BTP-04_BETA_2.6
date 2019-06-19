/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : PreBrow.h
 *
 * Purpose  : Ԥ����ý���ļ�����ͷ�ļ�
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


#define	RTN_NOSELECT	0		/*û��ѡ���κ��ļ�ֱ�ӷ���*/
#define RTN_IMAGE		4			/*ͼƬ�ļ�����*/
#define RTN_SOUND		1			/*�����ļ�����*/
#define RTN_TEXT		2			/*�ı��ļ�����*/
#define RTN_QTEXT		3			/*�����ļ�����*/

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
ͨ��ֱ��Ԥ���ӿ�
\**********************************************************************/
HINSTANCE APP_PreviewDirect(HWND hwnd, LPCTSTR lpOperation, LPCTSTR lpFile, 
                LPCTSTR lpParameters, LPCTSTR lpDirectory, INT nShowCmd);

BOOL	APP_PreDirectTxt(HWND hWnd, LPCTSTR pFileName);	//ֱ��Ԥ���ı�
BOOL	APP_PreDirectRing(HWND hWnd, LPCTSTR pFileName, BYTE bType);//ֱ��Ԥ������
BOOL	APP_PreDirectPic(HWND hWnd, LPCTSTR pFileName, int FileType);//ֱ��Ԥ��ͼƬ
				
/**********************************************************************\
ͨ��ѡ��Ԥ���ӿ�
\**********************************************************************/
HWND	APP_PreviewSelect(HWND hWnd, UINT returnmsg, DWORD dwFileStyle, DWORD dwMaskStyle);


/*********************************************************************\
* Function	SmPreBrow   
* Purpose   ����Ԥ���ⲿ���ýӿ�   
* Params	�����ھ���������ڵĽ�����Ϣ��Ҫ����Ķ��Ҫ����Ķ���ĳ���
            ע�⣺�����Ҫ����Ҫ��������MsLenΪ0��InsertedMsΪ""����
* Return	���򸸴��ڷ�����Ϣreturnmsg�����ݷ���ֵ,��ϸ��Remarks
* Remarks	WparamΪ0 ��returnmsg��lParam��Ŷ����ַ���ָ�룬
* Remarks	1�����û�ѡ��һ���ı��󷵻أ�
               wParam�ĸ�λΪ1
			   wParam�ĵ�λ����ı��ĳ���
               lParam����ı���ָ�룬
			2�����û��������κ�ѡ���˳���ʱ��
			   wParam�ĸ�λΪ0
			   wParam�ĵ�λλ0
			   lParam��Ч��ȱʡΪ""�� 
**********************************************************************/
//BOOL	APP_PreviewPhrase(HWND hFrame, HWND hWnd, UINT iRetMsg, const char * pInsertedPhrase, 
//						  int len, DWORD dMask);
BOOL	APP_PreviewPhrase(HWND	hFrame, HWND hWnd, UINT returnmsg, PCSTR szCaption);
BOOL	APP_EditPhrase(HWND	hFrame);
//BOOL	APP_PreviewPhrase(HWND hWnd, UINT returnmsg, const char * InsertedMs,  int MsLen, DWORD dwMask);

/*********************************************************************\
* Function	TxPreBrow   
* Purpose   �ı�Ԥ���ⲿ���ýӿ�   
* Params	�����ھ���������ڵĽ�����Ϣ
* Return	���򸸴��ڷ�����Ϣreturnmsg�����ݷ���ֵ,��ϸ��Remarks
* Remarks	1�����û�ѡ��һ���ļ��󷵻أ�
               wParam�ĸ�λΪ1
			   wParam�ĵ�λ����ı��ļ����Ƶĳ���
               lParam����ı��ļ����ƴ�ָ�룬
			2�����û��������κ�ѡ���˳���ʱ��
			   wParam�ĸ�λΪ0
			   wParam�ĵ�λλ0
			   lParam��Ч��ȱʡΪ""�� 
**********************************************************************/

BOOL	APP_PreviewText(HWND hFrame, HWND hWnd, UINT iRetMsg, DWORD dMask);

/*********************************************************************\
* Function	PICPreBrow   
* Purpose   ͼƬԤ���ⲿ���ýӿ�   
* Params	�����ھ���������ڵĽ�����Ϣ
* Return	���򸸴��ڷ�����Ϣreturnmsg�����ݷ���ֵ,��ϸ��Remarks
* Remarks	1�����û�ѡ��һ��ͼƬ�󷵻أ�
              
			   wParam�ĵ�λ���ͼƬ�ļ����Ƶĳ���
               lParam���ͼƬ�ļ����ƴ�ָ�룬
			2�����û��������κ�ѡ���˳���ʱ��û�з�����Ϣ
			  
**********************************************************************/

BOOL	APP_PreviewPicture(HWND hFrame, HWND hWnd, UINT iRetMsg, PCSTR szCap, PCSTR pFirstItem);
BOOL	APP_PreviewPictureEx(HWND hFrame, HWND hWnd, UINT returnmsg, PCSTR szCapion, PCSTR pFirstItem, PCSTR pFocusPic);

/*********************************************************************\
* Function	RGPreBrow   
* Purpose   ����Ԥ���ⲿ���ýӿ�   
* Params	�����ھ���������ڵĽ�����Ϣ,��ʼ�������ļ����ƣ�����ɾ�����
* Return	���򸸴��ڷ�����Ϣreturnmsg�����ݷ���ֵ,��ϸ��Remarks
* Remarks	1�����û�ѡ��һ�������󷵻أ�               
			   wParam�ĵ�λ��������ļ����Ƶĳ���
               lParam��������ļ����ƴ�ָ�룬
			2�����û��������κ�ѡ���˳���ʱ��
			   û�з�����Ϣ
**********************************************************************/
BOOL	APP_PreviewRing(HWND hFrame, HWND hWnd, UINT iRetMsg, PSTR szCaption);




#endif //_PREBROW_H_
