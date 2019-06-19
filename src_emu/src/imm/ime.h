/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : ime.h
 *
 * Purpose  : define input method edit struct and output function
 *            
\**************************************************************************/

#ifndef	HAVE_IME_H
#define	HAVE_IME_H

#ifdef __cplusplus
extern "C" {
#endif
/**************************************************************************\
*	Purpose:	define data struct  
*	Remarks:	   
*
**************************************************************************/
typedef struct ime_s
{
    char	v_aImeName[16];	//	���뷨����
	char	v_aImeIcon[16];	//	���뷨ͼ��
    
	WORD	v_nID;			//	���뷨������ı�ʾ
	WORD	v_nExtend;		//	���뷨��չ����	
	WORD	v_wAttrib;		//	���뷨����	
    WORD	v_wHotKey;		//	���뷨�л��Ƚ�

    DWORD	(*f_Inquire )( PVOID hThis, INT32 nCode, WPARAM nOne, LPARAM nTwo );
	
}	UIME, * HIME, ** LPHIME, ***PLPHIME;

typedef struct tagIMELANG
{
    char    szLanguage[16];
    char    szLangIcon[16];
}
IMELANG, *PIMELANG, **PPIMELANG, ***PPPIMELANG;

/**************************************************************************\
*	Purpose:	define macro const   
*	Remarks:	
*
**************************************************************************/
#define	IME_OPA_INIT		0x0001
#define	IME_OPA_SHOW		0x0002
#define	IME_OPA_CREATE		0x0003	//	��λΪ���뷨��չ����
#define	IME_OPA_DESTROY		0x0004
#define	IME_OPA_HIDE		0x0005
#define	IME_OPA_CHAR		0x0006
#define	IME_OPA_EVENT		0x0007
#define IME_OPA_COMMAND     0x0008
#define IME_OPA_ACTIVATE    0x0009

#define IME_FN_GETKEYTYPE   0x0001  // IME_OPA_COMMAND��Ϣ�Ĳ���
#define IME_NT_MULTAPPAUSE  0x0002  // IME_OPA_COMMAND��Ϣ�Ĳ���
#define IME_FN_GETCHARTAB   0x0003  // IME_OPA_COMMAND��Ϣ�Ĳ���
//	���뷨����
#define	IME_ID_ALL			0xffff
//	���뷨����
#define	IME_ATR_DISABLE		0x1000
#define	IME_ATR_NOSWITCH	0x0002

//	���뷨�¼�
#define	IME_EVT_F1			0x0011	
#define	IME_EVT_F2			0x0012	
#define	IME_EVT_F3			0x0013	
#define	IME_EVT_F4			0x0014	
#define	IME_EVT_F5			0x0015	
#define	IME_EVT_F6			0x0016
#define	IME_EVT_F7			0x0017
#define	IME_EVT_F8			0x0018
#define	IME_EVT_F9			0x0019
#define	IME_EVT_F10			0x001a

#define	IME_EVT_KB1			0x0001	
#define	IME_EVT_KB2			0x0002	
#define	IME_EVT_KB3			0x0003	
#define	IME_EVT_KB4			0x0004	
#define	IME_EVT_KB5			0x0005	
#define	IME_EVT_KB6			0x0006	
#define	IME_EVT_KB7			0x0007	
#define	IME_EVT_KB8			0x0008
#define	IME_EVT_KB9			0x0009	
#define	IME_EVT_KB0			0x000a
#define IME_EVT_CLEAR		0x0020	
#define	IME_EVT_RETURN		0x0021
#define	IME_EVT_PAGEUP		0x0022
#define	IME_EVT_PAGEDOWN	0x0023
#define	IME_EVT_PAGELEFT	0x0024
#define	IME_EVT_PAGERIGHT	0x0025

#define	IME_EVT_PENDOWN		0x0050
#define	IME_EVT_PENMOVE		0x0051
#define IME_EVT_PENUP		0x0052
//	���뷨��ݼ�����
//	������
#define IME_HKEY_INSTANT    0x0001    // ����
#define IME_HKEY_SHORT      0x0002    // �̰�
#define IME_HKEY_LONG       0x0004    // ����
#define IME_HKEY_REPEATING  0x0008    // ����
#define IME_HKEY_MULTITAP   0x0010    // �찴

#define	IME_HKEY_CTRL		0x2000
#define	IME_HKEY_ALT		0x4000
#define	IME_HKEY_SHIFT		0x8000

//	��ϼ�
//	˫��IME_HKEY_DOUBLE ��������������Ӧkeydown �������can't use with IME_HKEY_FASTER
#define	IME_HKEY_SWITCH		MAKEWORD(IME_EVT_F4, IME_HKEY_SHORT)
#define	IME_HKEY_ACTIVE		(IME_HKEY_CTRL|IME_EVT_F3)

//	���뷨�ı���Ϣ
#define IME_MSG_CHANGED_IME		(WM_USER + 0x0601)
#define	IME_MSG_CHANGED_SEL     (WM_USER + 0x0602)
#define IME_MSG_CHANGING_SEL    (WM_USER + 0x0603)
#define IME_MSG_GETLIMITSTR     (WM_USER + 0x0605)

#define IME_FOCUS_ORIGINAL		0x0001
#define IME_FOCUS_INLINE		0x0002
#define IME_FOCUS_CANDIDATE		0x0003

#define ISKEYUP(kd)             ((kd) & 0x80000000)
#define GETEVTSTATE(kd)         (((kd) & 0x00FF0000) >> 16)
#define GETREPEATS(kd)          ((kd) & 0x0000FFFF)
#define MAKEACCEL(kc, kd)       (MAKEWORD((kc), GETEVTSTATE(kd)))
#define MAKEKEYDATA(wRepeats, byEvent, bUp) ((LONG)(((WORD)(wRepeats)) \
    | (((DWORD)((BYTE)(byEvent))) << 16) | ((DWORD)((bUp) ? (1 << 31) : 0))))

typedef struct tagKEYTYPE
{
    WORD wKeyEvtMin;
    WORD wKeyEvtMax;
    WORD wKeyType;
    BOOL (*pfnOnKeyEvent)(PVOID, INT32, DWORD);
}
KEYTYPE, *PKEYTYPE;

//	OUTPUT FUNCTION DECLARE
BOOL	ImeGetEntry	( PLPHIME plphIme );
BOOL    ImeGetLangEntry(PPPIMELANG pppImeLang);
DWORD   DefImeProc(PVOID hThis, INT32 nCode, WPARAM nOne, LPARAM nTwo);
int     ImeLangSelWinMain(void);
int     ImeZiSpellWinMain(char *pszSpell);

#ifdef __cplusplus
}
#endif

#endif
