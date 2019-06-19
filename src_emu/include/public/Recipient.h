/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Public	control
 *
 * Purpose  : implement the Recipient control
 *            
\**************************************************************************/

#ifndef	_RECEIVEBOX_
#define	_RECEIVEBOX_



#include "winpda.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "window.h"
#include "window/hpdef.h"
#include "plx_pdaex.h"
#include "mullang.h"
#include "pubapp.h"
#include "imesys.h"
/* Recipient definition start */
#define	MAXNAMLENGTH	50
typedef	struct tagRECEOVEBOXLISTNODE
{
	int		nIndex;
	char	szShowName[110];
	char	szPhoneNum[60];
	BOOL	bExistInAB;
	struct	tagRECEOVEBOXLISTNODE* pNext;
	struct	tagRECEOVEBOXLISTNODE* pPre;
}RECIPIENTLISTNODE, * PRECIPIENTLISTNODE;

typedef	struct	tagRECEOVEBOXLISTBUF
{
	int			nDataNum;
	int			nCurrentIndex;

	PRECIPIENTLISTNODE	pCurrentData;
	PRECIPIENTLISTNODE	pDataHead;
}RECIPIENTLISTBUF, *PRECIPIENTLISTBUF;

void	RECIPIENT_InitListBuf(PRECIPIENTLISTBUF pListBuf);
BOOL	RECIPIENT_InsertData (PRECIPIENTLISTBUF pListBuf,int nindex, const char* szData, const char* szFullData, BOOL bExistInAB);
BOOL	RECIPIENT_AddData (PRECIPIENTLISTBUF pListBuf, const char* szData, const char* szFullData,BOOL bExistInAB);
BOOL	RECIPIENT_GetData (PRECIPIENTLISTBUF pLiSTBuf, int nIndex, char* szGetData, char* szFullData, BOOL bExistInAB);
BOOL	RECIPIENT_DelData (PRECIPIENTLISTBUF pListBuf, int nIndex);
int		RECIPIENT_FindData (PRECIPIENTLISTBUF pListBuf, char* szFindData);
void	RECIPIENT_FreeListBuf (PRECIPIENTLISTBUF pListBuf);
PRECIPIENTLISTNODE	RECIPIENT_GetPDATA (PRECIPIENTLISTBUF pListBuf, int nIndex );
BOOL	RECIPIENT_ModifyData (PRECIPIENTLISTBUF pListBuf, int nIndex,PSTR szShowName, PSTR szFullName, BOOL bAB);
BOOL	RECIPIENT_DivideData (PRECIPIENTLISTBUF pListBuf, int nIndex, int nPrelen);
void	UpdateContextWnd(HWND hWnd, PRECIPIENTLISTBUF pListBuf, WNDPROC RecipientProc);
BOOL	PLXstrchr(PSTR p, int c);
//Recipient message
//add recipient
//lparam:	point to a recipient
#define GHP_ADDREC		(WM_USER+1001)

//the msg is received when the size of windows was changed,
//the param wParam is the bottom position of my recipient. 
#define	REC_SIZE		(WM_USER+1002)
//set the max number of recipients
#define	REC_SETMAXREC	(WM_USER+1003)
//set the button text
#define	REC_BNTEXTCHANGE	(WM_USER+1004)//reserved
#define	GHP_GETREC			(WM_USER+1005)
#define	GHP_GETRECLEN			(WM_USER+1006)
//if multi recipients need to added when opening sms or other,this msg should be used 
#define	REC_FILLREC			(WM_USER+1007)
#define REC_SENDREADY		(WM_USER+1008)
#define	REC_CLEAR			(WM_USER+1009)
#define	RS_EMAIL	0x00001000L




/*
 *	create control receive box;
 *	BOOL bAbc mean whether the abc input method can be used in recipient field,
 *	wID is the control id
 */



#endif	//_RECEIVEBOX_
