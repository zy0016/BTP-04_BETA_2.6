/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : imc.c
 *
 * Purpose  : implement input method controller
 *            
\**************************************************************************/

#include	"window.h"
#include	"malloc.h"
#include	"string.h"

#include	"imm.h"

/**************************************************************************\
 *	Purpose:	define inline macro const   
 *	Remarks:	   
 *
 **************************************************************************/
#define	KBEVENTF_KEYUP		0xC0000000
#define	KBLONG_DOWN_TIME	1000
#define KBSHORT_DOWN_TIME	800

#define ET_LONG             1000
#define ET_REPEAT_FIRST     300
#define ET_REPEAT_LATER     100
#define ET_MULTITAP         800

#define IsAlphabetInput(w)  (((w) & 0x00F0) == 0x00A0)
#define IsWordInput(w)      (((w) & 0x00F0) == 0x00B0)
#define IsChineseInput(w)   (((w) & 0x00F0) == 0x00C0)
#define IsTextInput(w)      (IsAlphabetInput(w) || IsWordInput(w))

extern ZI8_LANGUAGE_ENTRY Zi8StaticLanguageTable[];

/**************************************************************************\
 *	purpose	:	Program inline function declare
 *	remark	:
 *
 **************************************************************************/
static	BOOL 	f_HandleHotKey	( HIMC hImc, DWORD dwKeyData );
static	BOOL	f_HandleSysActive	( HIMC hImc );
static	BOOL	f_HandleSysSwitch	( HIMC hImc );
static	BOOL	f_ImeShow ( HIMC hImc, HIME hIme, BOOL bOnly );
static	DWORD	f_MakeHotKey	( HIMC hImc, WORD wKeyCode, DWORD dwKeyState );
static	BOOL	f_IsEventHotKey ( WORD nHotKey, WORD * pEvent );
static  BOOL    f_ProcessKeyEvent(HIMC hImc, WORD wKeyCode, DWORD dwKeyData);

static	BOOL	f_KeyMessageProc ( HIMC hImc,  WPARAM nOne, LPARAM nTwo );
static	BOOL	f_PenMessageProc ( HIMC hImc,  WPARAM nOne, LPARAM nTwo );

static	void	f_ImeDestroy ( HIMC hImc );

static void CALLBACK f_TimerProc(HWND hWnd, UINT uMsg, UINT idEvent,
                                 DWORD dwTime);

extern TCHAR* GetWriLangStr(void);

/**************************************************************************\
 *	purpose	:	define global const
 *	remark	:
 *
 **************************************************************************/
typedef struct	
{
	WORD	v_nKey;
	WORD	v_nEvent;

}	UHOTKEY;

static	UHOTKEY g_afKey[] = 
{
	// COMMENT: 输入法内部事件					
	{ VK_1,		IME_EVT_KB1	},
	{ VK_2,		IME_EVT_KB2	},	
	{ VK_3,		IME_EVT_KB3	},	
	{ VK_4,		IME_EVT_KB4	},	
	{ VK_5,		IME_EVT_KB5	},	
	{ VK_6,		IME_EVT_KB6	},	
	{ VK_7,		IME_EVT_KB7	},	
	{ VK_8,		IME_EVT_KB8	},	
	{ VK_9,		IME_EVT_KB9	},	
	{ VK_0,		IME_EVT_KB0	},	
	{ VK_F1,	IME_EVT_F1	},	
	{ VK_F2,	IME_EVT_F2	},	
	{ VK_F3,	IME_EVT_F3	},	
	{ VK_F4,	IME_EVT_F4	},	
	{ VK_F5,	IME_EVT_F5	},	
	{ VK_F6,	IME_EVT_F6	},	
	{ VK_F7,	IME_EVT_F7	},	
	{ VK_F8,	IME_EVT_F8	},	
	{ VK_F9,	IME_EVT_F9	},	
	{ VK_F10,	IME_EVT_CLEAR	},
	{ VK_RETURN,IME_EVT_RETURN	},	
	{ VK_UP,	IME_EVT_PAGEUP	},	
	{ VK_DOWN,	IME_EVT_PAGEDOWN},	
	{ VK_LEFT,	IME_EVT_PAGELEFT},	
	{ VK_RIGHT,	IME_EVT_PAGERIGHT},		
};

/**************************************************************************\
 * Function	ImcCreate ( LPHIMC lphImc );	   
 * Purpose	Create One Input method controller       
 * Params	lphImc	: a point which to receive controller handle 			
 * Return	successed return TRUE 	   
 * Remarks	   
 **************************************************************************/
BOOL	ImcCreate ( LPHIMC lphImc )
{
	HIMC hImc;

	if ( NULL == lphImc )
		return	FALSE;

	hImc = (HIMC)malloc( sizeof(UIMC) );
	if ( NULL == hImc )
		return	FALSE;

	memset ( hImc, 0, sizeof(UIMC) );

    hImc->v_pZiGlobal = (PZI8VOID)malloc(Zi8GetGlobalDataSize());
    Zi8InitializeDynamic(Zi8StaticLanguageTable, hImc->v_pZiGlobal);

    *lphImc = hImc;

	return	(hImc == NULL) ? FALSE : TRUE;
}

/**************************************************************************\
 * Function	ImcRelease ( HIMC hImc );	   
 * Purpose	Release One Input method controller       
 * Params	hImc	:	controller handle 			
 * Return	return just FALSE; 	   
 * Remarks	
 **************************************************************************/
BOOL	ImcRelease ( HIMC hImc )
{
	if ( NULL == hImc )
		return	FALSE;

	f_ImeDestroy ( hImc );

    if (hImc->v_pZiGlobal != NULL)
        free(hImc->v_pZiGlobal);

	free ( hImc );
	
	return	FALSE;
}

/**************************************************************************\
 * Function	ImcGetImeNumber	( HIMC hImc, BOOL bShow, LPWORD pNumber )
 * Purpose	Get system ime number  
 * Params	hImc	:
			bShow	: 
			pNumber	: a point to receive value
 * Return	successed return number 	   
 * Remarks	   
 **************************************************************************/
BOOL	ImcGetImeNumber	( HIMC hImc, BOOL bShow, LPWORD pNumber )
{
	HIMELINK	hFind;
	WORD		nNumber = 0;

	if ( NULL == hImc )
	if ( !ImmGetDefaultImc ( &hImc ) )
		return	FALSE;	
	
	if ( bShow )
		hFind = hImc->v_hShow;
	else
		hFind = hImc->v_hHide;

	while ( NULL != hFind )
	{
		hFind = hFind->v_hNext;
		nNumber ++;
	}
	*pNumber = nNumber;
	
	return	TRUE;
}
/**************************************************************************\
* Function	ImcLockIme	( HIMC hImc, UINT nMode )
* Purpose	   
* Params	hImc	: controller handle
nMode	: if zero hide ime window, or show
* Return	successed return TRUE; 	  	   
* Remarks	
**************************************************************************/
BOOL	ImcLockIme	( HIMC hImc, UINT nMode )
{
	if ( NULL == hImc )
	if ( !ImmGetDefaultImc ( &hImc ) )
		return	FALSE;
	
	switch ( nMode ) 
	{
	case IMC_LOCK_IME	:	
		
		if ( NULL == hImc->v_hShow && NULL == hImc->v_hHide )
			return	FALSE;

		if ( NULL != hImc->v_hShow )
			hImc->v_nLockID = hImc->v_hShow->v_hIme->v_nID;
		else
			hImc->v_nLockID = hImc->v_hHide->v_hIme->v_nID;
		
		if ( !(hImc->v_nAttrib & IMC_ATR_LOCK) )
			hImc->v_nAttrib |= IMC_ATR_LOCK;
		
		return	TRUE;
	case IMC_UNLOCK_IME	:
		
		if ( hImc->v_nAttrib & IMC_ATR_LOCK )
			hImc->v_nAttrib &= ~IMC_ATR_LOCK;
		
		return	TRUE;
	case IMC_LOCK_SWITCH	:		
		
		if ( !(hImc->v_nAttrib & IMC_ATR_NOSWITCH) )
			hImc->v_nAttrib |= IMC_ATR_NOSWITCH;
		
		return	TRUE;
	case IMC_UNLOCK_SWITCH:		
		
		if ( hImc->v_nAttrib & IMC_ATR_NOSWITCH )
			hImc->v_nAttrib &= ~IMC_ATR_NOSWITCH;
		
		return	TRUE;
	}
	return	FALSE;
}

/**********************************************************************
 * Function     ImcToggleTextInput
 * Purpose      
 * Params       hImc, bWordMode
 * Return       
 * Remarks      
 **********************************************************************/

BOOL ImcToggleTextInput(HIMC hImc, BOOL bWordMode)
{
    int nTextIndex = 0, i = 0;
    WORD wTotal = 0;
    HIME hIme = NULL;

    if (hImc == NULL)
    {
        if (!ImmGetDefaultImc(&hImc))
            return FALSE;
    }

    ImmGetImeNumber(&wTotal);

    if (bWordMode)
    {
        if (hImc->v_nAttrib & IMC_ATR_NOWORD)
            return FALSE;
        hImc->v_nAttrib |= IMC_ATR_WORD;

        if (hImc->v_hShow == NULL)
            return TRUE;

        nTextIndex = hImc->v_hShow->v_hIme->v_nExtend & 0x000F;

        if (!IsTextInput(hImc->v_hShow->v_hIme->v_nExtend))
            return TRUE;

        for (i = 0; i < (int)wTotal; i++)
        {
            ImmGetImeHandleByID((WORD)i, &hIme);
            if (IsWordInput(hIme->v_nExtend)
                && ((hIme->v_nExtend & 0x000F) == nTextIndex))
            {
                f_ImeShow(hImc, hIme, TRUE);
                break;
            }
        }
    }
    else
    {
        hImc->v_nAttrib &= ~IMC_ATR_WORD;
        if (hImc->v_hShow == NULL)
            return TRUE;

        if (!IsTextInput(hImc->v_hShow->v_hIme->v_nExtend))
            return TRUE;

        nTextIndex = hImc->v_hShow->v_hIme->v_nExtend & 0x000F;

        for (i = 0; i < (int)wTotal; i++)
        {
            ImmGetImeHandleByID((WORD)i, &hIme);
            if (IsAlphabetInput(hIme->v_nExtend)
                && ((hIme->v_nExtend & 0x000F) == nTextIndex))
            {
                f_ImeShow(hImc, hIme, TRUE);
                break;
            }
        }
    }

    return TRUE;
}

/**********************************************************************
 * Function     ImcGetImeLangState
 * Purpose      
 * Params       hImc, pszIcon
 * Return       
 * Remarks      
 **********************************************************************/

BOOL ImcGetImeLangState(HIMC hImc, PSTR *ppszIcon)
{
    if (ppszIcon == NULL)
        return FALSE;

    if (hImc == NULL)
    {
        if (!ImmGetDefaultImc(&hImc))
            return FALSE;
    }

    if (!(hImc->v_nAttrib & IMC_ATR_WORD)
        || !IsTextInput(hImc->v_hShow->v_hIme->v_nExtend))
        return FALSE;

    *ppszIcon = (char*)hImc->v_ppImeLang[hImc->v_iLang]->szLangIcon;
    return TRUE;
}

/**********************************************************************
 * Function     ImcSetImeLanguage
 * Purpose      
 * Params       HIMC hImc, int iLang
 * Return       
 * Remarks      
 **********************************************************************/

BOOL ImcSetImeLanguage(HIMC hImc)
{
    TCHAR* pszLang = NULL;
    int iLang = 0;

    if (hImc == NULL)
    {
        if (!ImmGetDefaultImc(&hImc))
            return FALSE;
    }

    pszLang = GetWriLangStr();
    for (iLang = 0; iLang < (int)hImc->v_nTotalLang; iLang++)
    {
        if (stricmp(hImc->v_ppImeLang[iLang]->szLanguage, pszLang) == 0)
            break;
    }

    if (iLang >= (int)hImc->v_nTotalLang)
    {
        iLang = 0;
    }
    hImc->v_iLang = iLang;

    return TRUE;
}

/**********************************************************************
 * Function     ImcEnableWordInput
 * Purpose      
 * Params       hImc, bEnable
 * Return       
 * Remarks      
 **********************************************************************/

BOOL ImcEnableWordInput(HIMC hImc, BOOL bEnable)
{
    if (hImc == NULL)
    {
        if (!ImmGetDefaultImc(&hImc))
            return FALSE;
    }

    if (bEnable)
    {
        hImc->v_nAttrib &= ~IMC_ATR_NOWORD;
    }
    else
    {
        hImc->v_nAttrib |= IMC_ATR_NOWORD;
    }

    return TRUE;
}

/**********************************************************************
 * Function     ImcGetZiGlobal
 * Purpose      
 * Params       
 * Return       
 * Remarks      
 **********************************************************************/

PZI8VOID ImcGetZiGlobal(void)
{
    HIMC hImc = NULL;

    if (!ImmGetDefaultImc(&hImc))
        return NULL;

    return hImc->v_pZiGlobal;
}

/**************************************************************************\
 * Function	f_CheckOut ( HIMC hImc, WORD nID, BOOL bShow )
 * Purpose	 
 * Params	hImc	: controller handle
			nID	: 
			bShow	: if zero hide ime window, or show
 * Return	successed return TRUE; 	  	   
 * Remarks	
 **************************************************************************/
static	HIMELINK	f_CheckOut ( HIMC hImc, WORD nID, BOOL bShow )
{
	HIMELINK hFind, hPrev;

	if ( bShow )
		hFind = hImc->v_hShow;
	else
		hFind = hImc->v_hHide;

	if ( NULL == hFind )
		return	NULL;

	if ( hFind->v_hIme->v_nID == nID )
	{
		if ( bShow )
			hImc->v_hShow = hImc->v_hShow->v_hNext;
		else
			hImc->v_hHide = hImc->v_hHide->v_hNext;

		return	hFind;
	}

	hPrev = hFind;
	hFind = hFind->v_hNext;

	while ( NULL != hFind )
	{
		if ( hFind->v_hIme->v_nID == nID )
			break;

		hPrev = hFind;
		hFind = hFind->v_hNext;
	}

	if ( NULL == hFind )
		return	NULL;

	hPrev->v_hNext = hFind->v_hNext;

	return	hFind;
}

/**************************************************************************\
 * Function	f_ImeDestroy ( HIMC hImc, BOOL bShow )
 * Purpose	Destroy the controller's current input method edit       
 * Params	hImc	: controller handle
 * Return	successed return TRUE; 	  	   
 * Remarks	
 **************************************************************************/
static	void	f_ImeDestroy ( HIMC hImc )
{
	HIMELINK	hLink, hTmp;

	hLink = hImc->v_hHide;
	while ( NULL != hLink )
	{
		if ( NULL != hLink->v_hIme )
		{
			hLink->v_hIme->f_Inquire ( hLink->v_hThis, IME_OPA_DESTROY, 0, 0 );
			if ( hImc->v_nLockID == hLink->v_hIme->v_nID )
				hImc->v_nAttrib &= ~IMC_ATR_LOCK;
		}		
		hTmp	= hLink;
		hLink	= hLink->v_hNext;
		free ( hTmp );
	}
	
	hLink = hImc->v_hShow;
	while ( NULL != hLink )
	{
		if ( NULL != hLink->v_hIme )
		{
			hLink->v_hIme->f_Inquire ( hLink->v_hThis, IME_OPA_DESTROY, 0, 0 );
			if ( hImc->v_nLockID == hLink->v_hIme->v_nID )
				hImc->v_nAttrib &= ~IMC_ATR_LOCK;
		}		
		hTmp	= hLink;
		hLink	= hLink->v_hNext;
		free ( hTmp );
	}
	hImc->v_hShow = NULL;
	hImc->v_hHide = NULL;
}

/**************************************************************************\
 * Function	f_ImeHide ( HIMC hImc, WORD imeID )
 * Purpose	Hide the controller's current input method edit       
 * Params	hImc	: controller handle
			imeID	: 
 * Return	successed return TRUE; 	  	   
 * Remarks	
 **************************************************************************/
static	BOOL	f_ImeHide ( HIMC hImc, WORD imeID )
{
	WORD		nPrevID, nID;
	HIMELINK	hFind, hPrev;

	if ( IME_ID_ALL == imeID )
	{
		hFind = hImc->v_hShow;
		if ( NULL == hFind )
			return	FALSE;

		hPrev = hFind;

		while ( NULL != hFind ) 
		{
			hFind->v_hIme->f_Inquire ( hFind->v_hThis, IME_OPA_HIDE, 0, 0 );
			hFind = hFind->v_hNext;
			if ( NULL != hFind )
				hPrev = hFind;
		}		
		hPrev->v_hNext	= hImc->v_hHide;
		hImc->v_hHide	= hImc->v_hShow;
		hImc->v_hShow	= NULL;

		return	TRUE;
	}
	if ( NULL != hImc->v_hShow )
		nPrevID = hImc->v_hShow->v_hIme->v_nID;
	else if ( NULL != hImc->v_hHide )
		nPrevID = hImc->v_hHide->v_hIme->v_nID;
	else
		nPrevID = imeID;

	hFind = f_CheckOut( hImc, imeID, TRUE );
	if ( NULL == hFind )
		return	FALSE;

	hFind->v_hNext	= hImc->v_hHide;
	hImc->v_hHide	= hFind;

	hFind->v_hIme->f_Inquire ( hFind->v_hThis, IME_OPA_HIDE, 0, 0 );
	
	if ( hImc->v_nAttrib & IMC_ATR_LOCK )
		return	TRUE;

	if ( NULL != hImc->v_hShow )
		nID = hImc->v_hShow->v_hIme->v_nID;
	else if ( NULL != hImc->v_hHide )
		nID = hImc->v_hHide->v_hIme->v_nID;
	else
		nID = imeID;
	
	hFind->v_hIme->f_Inquire ( hFind->v_hThis, IME_OPA_HIDE, 0, 0 );

	if ( NULL != hImc->v_hShow && nPrevID != nID )
	{
		SendMessage ( GetFocus (), IME_MSG_CHANGED_IME, 
			MAKELONG( nPrevID, nID ), 0 );

		hImc->v_hShow->v_hIme->f_Inquire( hImc->v_hShow->v_hThis, 
			IME_OPA_SHOW, 0, 0 );
	}

    if (hImc->v_hShow == NULL)
    {
        if (hImc->v_nTimerNotify != 0)
        {
            KillTimer(NULL, hImc->v_nTimerNotify);
            hImc->v_nTimerNotify = 0;
        }
        if (hImc->v_nTimerID != 0)
        {
            KillTimer(NULL, hImc->v_nTimerID);
            hImc->v_nTimerID = 0;
        }
    }
    
	return	TRUE;
}

/**************************************************************************\
 * Function	f_ImeShow ( HIMC hImc, HIME hIme, BOOL bOnly )
 * Purpose	Show the controller's current input method edit       
 * Params	hImc	: controller handle
			hIme	: 
 * Return	successed return TRUE; 	  	   
 * Remarks	
 **************************************************************************/
static	BOOL	f_ImeShow ( HIMC hImc, HIME hIme, BOOL bOnly )
{
	INT32		nCode, nID, nRet;
	HIMELINK	hFind;
    PVOID       pPrev = NULL;
    BOOL        bRet = FALSE;
	
	if ( NULL != hImc->v_hShow )
	if ( hImc->v_hShow->v_hIme == hIme )		
		return	FALSE;	

	if ( hImc->v_nAttrib & IMC_ATR_LOCK )
	if ( hIme->v_nID != hImc->v_nLockID )
		return	FALSE;

	if ( NULL != hImc->v_hShow )
    {
        nID = hImc->v_hShow->v_hIme->v_nID;
        pPrev = hImc->v_hShow->v_hThis;
    }
	else if ( NULL != hImc->v_hHide )
		nID = hImc->v_hHide->v_hIme->v_nID;
	else
		nID = hIme->v_nID;

	hFind = f_CheckOut ( hImc, hIme->v_nID, TRUE );
	if ( NULL != hFind )
		goto RET_SUCCESSED;

	hFind = f_CheckOut ( hImc, hIme->v_nID, FALSE );
	if ( NULL != hFind )
		goto RET_SUCCESSED;	

	hFind = (HIMELINK)malloc ( sizeof(IMELINK) );
	if ( NULL == hFind )
		return	FALSE;
	
	nCode	= IME_OPA_CREATE | ((INT32)hIme->v_nExtend << 16);
	nRet	= hIme->f_Inquire ( (PVOID)&hFind->v_hThis, nCode, nID, 0 );
	if ( 0 == nRet )
	{
		free ( hFind );

		return	FALSE;
	}	
	hFind->v_hIme  = hIme;

RET_SUCCESSED:

	if ( bOnly )
		f_ImeHide ( hImc, IME_ID_ALL );//f_ImeDestroy ( hImc );	

	hFind->v_hNext	= hImc->v_hShow;
	hImc->v_hShow	= hFind;
	
    if (IsTextInput(hIme->v_nExtend))
    {
        if (IsWordInput(hIme->v_nExtend))
        {
            hImc->v_nAttrib |= IMC_ATR_WORD;
        }
        else
        {
            hImc->v_nAttrib &= ~IMC_ATR_WORD;
        }
    }
    
    bRet = hFind->v_hIme->f_Inquire(hFind->v_hThis, IME_OPA_ACTIVATE,
        (WPARAM)nID, (LPARAM)pPrev);

    if (bRet == (BOOL)-1)
    {
        f_ImeHide(hImc, hIme->v_nID);
        return FALSE;
    }

	bRet = hFind->v_hIme->f_Inquire(hFind->v_hThis, IME_OPA_SHOW, 0, 0);

    if ( hIme->v_nID != nID )
    {
        SendMessage(GetFocus(), IME_MSG_CHANGED_IME,
            MAKEWPARAM(nID, hIme->v_nID), 0);
    }

    return bRet;
}

/**************************************************************************\
 * Function	ImcShowImeByID ( HIMC hImc, WORD imeID, BOOL bShow )
 * Purpose	Show or Hide the controller's current input method edit       
 * Params	hImc	: controller handle
			imeID	: 
			bShow	: if zero hide ime window, or show
 * Return	successed return TRUE; 	  	   
 * Remarks	
 **************************************************************************/
BOOL	ImcShowImeByID ( HIMC hImc, WORD imeID, BOOL bShow )
{
	HIME	hIme;
	
	if ( NULL == hImc )	//	使用缺省的控制器
	if ( !ImmGetDefaultImc ( &hImc ) )
		return	FALSE;	
	
	if ( !bShow )		//	要求隐藏
		return	f_ImeHide ( hImc, imeID );	
	
	if ( !ImmGetImeHandleByID( imeID, &hIme ) )
		return	FALSE;	

	if ( hIme->v_wAttrib & IME_ATR_DISABLE )
		return	FALSE;

	return	f_ImeShow ( hImc, hIme, FALSE );	
}

/**************************************************************************\
 * Function	ImcMessageProc ( HIMC hImc, INT32 nType, WPARAM nOne, LPARAM nTwo )
 * Purpose	Check hot key, whether it is a ime active key    
 * Params	hImc	: controller handle
			nOne	: current key down key code
			nTwo	: current keyboard state
 * Return	Processed Message return TRUE 	  	   
 * Remarks	
 **************************************************************************/
BOOL	ImcMessageProc ( HIMC hImc, INT32 nType, WPARAM nOne, LPARAM nTwo )
{		
	if ( NULL == hImc )	//	获取缺省的控制对象 
	if ( !ImmGetDefaultImc ( &hImc ) )
		return	FALSE;
	
	if ( IMC_MSG_KEY == nType )
		return	f_KeyMessageProc ( hImc, nOne, nTwo );
	
	return	f_PenMessageProc ( hImc, nOne, nTwo );
}

/**************************************************************************\
 * Function	f_MakeHotKey ( HIMC hImc, INT32 nKeyCode );
 * Purpose	make hot key       
 * Params	hImc		:
			nKeyCode	: current key down key code
 * Return	hot key; 	  	   
 * Remarks	
 **************************************************************************/
static DWORD f_MakeHotKey(HIMC hImc, WORD wKeyCode, DWORD dwKeyState)
{
	DWORD	dwTickNow = 0, dwKeyData = 0;
    WORD    wKeyType = 0;
    BYTE    byKeyEvent = 0;
    int     nInterval = 0;

    if (hImc->v_nDown)
    {
        hImc->v_nRepeats++;
        
        if (hImc->v_nKeyType & IME_HKEY_MULTITAP)
        {
            dwTickNow = GetTickCount();
            nInterval = dwTickNow - hImc->v_nTicker;
            if (nInterval < 0)
                nInterval = -nInterval;
            
            // 1.Press down during the MULTITAP interval,
            //   but the pressing key changed
            // 2.Although exceeds the MULTITAP time limit,
            //   the timer haven't arrived
            if (((nInterval <= ET_MULTITAP) && (wKeyCode != hImc->v_nHotKey))
                || ((nInterval > ET_MULTITAP) && (hImc->v_nTimerNotify != 0)))
            {
                if (hImc->v_nTimerNotify != 0)
                {
                    KillTimer(NULL, hImc->v_nTimerNotify);
                    hImc->v_nTimerNotify = 0;
                }
                hImc->v_hShow->v_hIme->f_Inquire(
                    hImc->v_hShow->v_hThis, IME_OPA_COMMAND,
                    IME_NT_MULTAPPAUSE, (LPARAM)hImc->v_nHotKey);
            }
        }

        wKeyType = (WORD)hImc->v_hShow->v_hIme->f_Inquire(hImc->v_hShow->v_hThis,
            IME_OPA_COMMAND, IME_FN_GETKEYTYPE, wKeyCode);

        if (wKeyType & IME_HKEY_INSTANT)
        {
            byKeyEvent |= IME_HKEY_INSTANT;
        }

        if (wKeyType & IME_HKEY_REPEATING)
        {
            if ((hImc->v_nRepeats == 1) && (HIWORD(dwKeyState) != 0))
            {
                hImc->v_nTimerID = SetTimer(NULL, 0, ET_REPEAT_FIRST,
                    f_TimerProc);
            }

            byKeyEvent |= IME_HKEY_REPEATING;
        }

        if ((wKeyType & IME_HKEY_SHORT) || (wKeyType & IME_HKEY_MULTITAP))
        {
            if (!(hImc->v_nKeyType & IME_HKEY_MULTITAP))
            {
                dwTickNow = GetTickCount();
                nInterval = dwTickNow - hImc->v_nTicker;
                if (nInterval < 0)
                    nInterval = -nInterval;
            }

            if (wKeyType & IME_HKEY_MULTITAP)
            {
                if ((nInterval <= ET_MULTITAP) && (wKeyCode == hImc->v_nHotKey))
                {
                    byKeyEvent |= IME_HKEY_MULTITAP;
                }

                if (hImc->v_nTimerNotify != 0)
                {
                    KillTimer(NULL, hImc->v_nTimerNotify);
                }
                hImc->v_nTimerNotify = SetTimer(NULL, 0, ET_MULTITAP,
                    f_TimerProc);
            }

            hImc->v_nTicker = dwTickNow;
        }

        if (wKeyType & IME_HKEY_LONG)
        {
            if (hImc->v_nTimerID != 0)
            {
                KillTimer(NULL, hImc->v_nTimerID);
            }
            hImc->v_nTimerID = SetTimer(NULL, 0, ET_LONG, f_TimerProc);
        }

        if ((wKeyType & IME_HKEY_INSTANT) || (wKeyType & IME_HKEY_REPEATING)
            || (wKeyType & IME_HKEY_MULTITAP))
        {
            dwKeyData = MAKEKEYDATA(hImc->v_nRepeats, byKeyEvent, FALSE);
        }
        else if ((wKeyType & IME_HKEY_SHORT) || (wKeyType & IME_HKEY_LONG))
        {
            dwKeyData = (DWORD)-1;
        }

        hImc->v_nHotKey  = wKeyCode;
        hImc->v_nKeyType = wKeyType;
    }
    else
    {
        wKeyType = hImc->v_nKeyType;

        if (wKeyType & IME_HKEY_REPEATING)
        {
            if (hImc->v_nTimerID != 0)
            {
                KillTimer(NULL, hImc->v_nTimerID);
                hImc->v_nTimerID = 0;
            }
        }

        if (wKeyType & IME_HKEY_SHORT)
        {
            dwTickNow = GetTickCount();
            nInterval = dwTickNow - hImc->v_nTicker;
            if (nInterval < 0)
                nInterval = -nInterval;

            if (nInterval <= ET_LONG)
            {
                dwKeyData = MAKEKEYDATA(1, IME_HKEY_SHORT, FALSE);
                f_ProcessKeyEvent(hImc, wKeyCode, dwKeyData);
            }
        }

        if (wKeyType & IME_HKEY_LONG)
        {
            if (hImc->v_nTimerID != 0)
            {
                KillTimer(NULL, hImc->v_nTimerID);
                hImc->v_nTimerID = 0;
            }
        }

        dwKeyData = MAKEKEYDATA(1, 0, TRUE);
        hImc->v_nRepeats = 0;
    }

	return dwKeyData;
}

/**************************************************************************\
 * Function		: f_TimerProc
 * Description	: 
 * Argument		: hWnd
 *				: UINT uMsg
 *				: UINT idEvent
 *				: DWORD dwTime
 * Return type	: CALLBACK 
 **************************************************************************/

static void CALLBACK f_TimerProc(HWND hWnd, UINT uMsg, UINT idEvent,
                                 DWORD dwTime)
{
    HIMC hImc = NULL;

	if (!ImmGetDefaultImc(&hImc))
		return;

    if (idEvent == hImc->v_nTimerID)
    {
        if (hImc->v_nKeyType & IME_HKEY_REPEATING)
        {
            if (hImc->v_nRepeats == 1)
            {
                KillTimer(NULL, hImc->v_nTimerID);
                hImc->v_nTimerID = SetTimer(NULL, 0, ET_REPEAT_LATER, f_TimerProc);
            }
            if (hImc->v_hShow != NULL)
            {
                keybd_event(hImc->v_nVirtKey, 0, 0, 0);
            }
            else
            {
                KillTimer(NULL, hImc->v_nTimerID);
                hImc->v_nRepeats = 0;
            }
        }
        else
        {
            KillTimer(NULL, hImc->v_nTimerID);
            hImc->v_nTimerID = 0;
            f_ProcessKeyEvent(hImc, hImc->v_nHotKey,
                (DWORD)MAKEKEYDATA(1, IME_HKEY_LONG, FALSE));
        }
    }
    else
    {
        KillTimer(NULL, hImc->v_nTimerNotify);
        hImc->v_nTimerNotify = 0;
        hImc->v_hShow->v_hIme->f_Inquire(hImc->v_hShow->v_hThis,
            IME_OPA_COMMAND, IME_NT_MULTAPPAUSE, (LPARAM)hImc->v_nHotKey);
    }
}

/**************************************************************************\
 * Function	f_KeyMessageProc ( HIMC hImc, WPARAM nOne, LPARAM nTwo )
 * Purpose	Check hot key, whether it is a ime active key    
 * Params	hImc	: controller handle
			nOne	: current key down key code
			nTwo	: current keyboard state
 * Return	Processed Message return TRUE 	  	   
 * Remarks	
 **************************************************************************/
static	BOOL	f_KeyMessageProc ( HIMC hImc, WPARAM nOne, LPARAM nTwo )
{
	WORD	nHotKey;
	DWORD	dwKeyData = 0;
    BOOL    bImeHasKeyDown = FALSE;

    if (hImc->v_hShow == NULL)
    {
        hImc->v_nDown = 0;
        hImc->v_nRepeats = 0;

        // If previous press down operating (hImc->v_nDown != 0)
        // results in IME shut off (hImc->v_hShow == NULL),
        // this release up event (nTwo & KBKBEVENTF_KEYUP) is still
        // processed by IME (no return FALSE).
        if ((nTwo & KBEVENTF_KEYUP) && (hImc->v_nDown != 0))
            return TRUE;

        return FALSE;
    }
    else
    {
        // If previous press down operating (hImc->v_nDown != 0)
        // results in IME opened (hImc->v_hShow != NULL),
        // this release up event (nTwo & KBKBEVENTF_KEYUP) is
        // thrown away by IME (return FALSE).
        if ((nTwo & KBEVENTF_KEYUP) && (hImc->v_nDown == 0))
            return FALSE;

//        if ((hImc->v_nDown != 0) && ((WORD)nOne != hImc->v_nVirtKey))
//            return TRUE;
    }

    if ( !f_IsEventHotKey ( (WORD)nOne, &nHotKey ) )
		return	FALSE;

    hImc->v_nVirtKey = (WORD)nOne;
    hImc->v_nDown = (nTwo & KBEVENTF_KEYUP) ? 0 : 1;

 	dwKeyData = f_MakeHotKey(hImc, nHotKey, (DWORD)nTwo);

	if (dwKeyData == (DWORD)-1)
    {
        // This type of event must be fed by IMM to use later
        // or thrown away to not use any more.
        return TRUE;
    }

    return f_ProcessKeyEvent(hImc, nHotKey, dwKeyData);
}

/**************************************************************************\
 * Function	f_PenMessageProc ( HIMC hImc, WPARAM nOne, LPARAM nTwo )
 * Purpose	Check hot key, whether it is a ime active key    
 * Params	hImc	: controller handle
			nOne	: current key down key code
			nTwo	: current keyboard state
 * Return	Processed Message return TRUE 	  	   
 * Remarks	
 **************************************************************************/
static	BOOL	f_PenMessageProc ( HIMC hImc, WPARAM nOne, LPARAM nTwo )
{	
	MOUSEHOOKSTRUCT*	pmhs;

	if ( NULL == hImc->v_hShow )
		return	FALSE;

	pmhs = (MOUSEHOOKSTRUCT*)nTwo;

	switch ( nOne ) 
	{
	case WM_PENDOWN:

		if ( hImc->v_hShow->v_hIme->f_Inquire ( hImc->v_hShow->v_hThis, 
			IME_OPA_EVENT, IME_EVT_PENDOWN, MAKELONG(pmhs->pt.x, pmhs->pt.y) ) )
			hImc->v_nDown = 1;
		else
			hImc->v_nDown = 0;

		return	(BOOL)hImc->v_nDown;
	case WM_PENMOVE:
		
		if ( hImc->v_nDown > 0 )
			hImc->v_hShow->v_hIme->f_Inquire ( hImc->v_hShow->v_hThis,
			IME_OPA_EVENT, IME_EVT_PENMOVE, MAKELONG(pmhs->pt.x, pmhs->pt.y) );

		return	(BOOL)hImc->v_nDown;
	case WM_PENUP:

		if ( hImc->v_nDown <= 0 )
			return	FALSE;
		
		hImc->v_hShow->v_hIme->f_Inquire ( hImc->v_hShow->v_hThis, 
			IME_OPA_EVENT, IME_EVT_PENUP, MAKELONG(pmhs->pt.x, pmhs->pt.y) );

		hImc->v_nDown = 0;

		return	TRUE;
	}	
	return	FALSE;
}	

/**************************************************************************\
* Function	f_IsEventHotKey ( WORD nHotKey, WORD * pEvent )
 * Purpose	Check hot key, whether it is a ime event    
 * Params	nHotKey	: hot key code
			pEvent	: a point to receive Event value
 * Return	successed return TRUE 	  	   
 * Remarks	不考虑主参数的错误,只允许内部调用
 **************************************************************************/
static	BOOL	f_IsEventHotKey ( WORD nHotKey, WORD * pEvent )
{
	register signed short	i, j;

	nHotKey &= 0xff;
	*pEvent = 0;
	j = sizeof (g_afKey)/sizeof(UHOTKEY);
	
	for ( i = 0; i < j ; i++ )
	if  ( g_afKey[i].v_nKey == nHotKey )
    {
        *pEvent = g_afKey[i].v_nEvent;

		return (*pEvent == NULL) ? FALSE : TRUE;
    }
	return	FALSE;
}

/**************************************************************************\
 * Function	f_HandleHotKey ( HIMC hImc, short nHotKey );
 * Purpose	Process Input method edit active hot key   
 * Params	hImc	: controller handle
			nHotKey	: hot key code
 * Return	Processed return TRUE 	  	   
 * Remarks	不考虑主参数的错误,只允许内部调用
 **************************************************************************/
static	BOOL	f_HandleHotKey	( HIMC hImc, DWORD dwKeyData )
{
	HIME	hIme;
    WORD    wAccel = 0, wTemp = 0;
    BOOL    bRet = FALSE;

    if (ISKEYUP(dwKeyData))
        return FALSE;

    wAccel = MAKEACCEL(hImc->v_nHotKey, dwKeyData);

	if ( IME_HKEY_ACTIVE == wAccel )
		return	f_HandleSysActive ( hImc );

	if ( IME_HKEY_SWITCH == wAccel )
    {
        bRet = f_HandleSysSwitch ( hImc );
        if (bRet)
            hImc->v_bCatchUp = TRUE;

        return bRet;
    }

    if ( NULL == hImc->v_hShow )
		return	FALSE;

    if ( !ImmGetImeHandleByHK ( wAccel, &hIme ) )
		return	FALSE;

	if ( hIme->v_wAttrib & IME_ATR_DISABLE )
		return	FALSE;

    bRet = f_ImeShow ( hImc, hIme, FALSE );
    if (bRet)
        hImc->v_bCatchUp = TRUE;

	return bRet;
}

/**************************************************************************\
 * Function	f_HandleSysActive	( HIMC hImc )
 * Purpose	Process system default active hot key   
 * Params	hImc	: controller handle
 * Return	Processed return TRUE 	  	   
 * Remarks	不考虑主参数的错误,只允许内部调用
 **************************************************************************/
static	BOOL f_HandleSysActive	( HIMC hImc )
{
	HIME	hIme;
	WORD	imeID, nIme, imeDF;

	if ( NULL == hImc->v_hShow )
	{				
		if ( NULL != hImc->v_hHide )
			return	f_ImeShow ( hImc, hImc->v_hHide->v_hIme, TRUE );

		ImmGetDefaultIme ( &imeDF );
		ImmGetImeNumber ( &nIme );

		imeID = imeDF;
GET_MATCH:
		if ( !ImmGetImeHandleByID ( imeID, &hIme ) )
			return	FALSE;		
		
		if ( hIme->v_wAttrib & IME_ATR_DISABLE )
		{
			imeID ++;	
			if ( imeID >= nIme )
				imeID = 0;
			
			if ( imeID == imeDF )
				return	FALSE;	

			goto GET_MATCH;
		}
		return	f_ImeShow ( hImc, hIme, TRUE );	
	}	
	return	f_ImeHide ( hImc, IME_ID_ALL );
}

/**************************************************************************\
 * Function	f_HandleSysSwitch	( HIMC hImc );
 * Purpose	Process system default hot key, to change current ime   
 * Params	hImc	: controller handle
 * Return	Processed return TRUE 	  	   
 * Remarks	不考虑主参数的错误,只允许内部调用
 **************************************************************************/
static	BOOL	f_HandleSysSwitch	( HIMC hImc )
{
	HIME	hIme;
	WORD	nIme, imeID;	

	if ( NULL == hImc->v_hShow )
		return	FALSE;//f_HandleSysActive ( hImc );

	if ( hImc->v_nAttrib & IMC_ATR_NOSWITCH )
		return	FALSE;

	ImmGetImeNumber ( &nIme );
	if ( nIme < 2 )
		return	FALSE;

	imeID = hImc->v_hShow->v_hIme->v_nID;

GET_MATCH:

	imeID ++;	
	if ( imeID >= nIme )
		imeID = 0;

	if ( imeID == hImc->v_hShow->v_hIme->v_nID )
		return	FALSE;

	if ( !ImmGetImeHandleByID ( imeID, &hIme ) )
		return	FALSE;

	if ( (hIme->v_wAttrib & IME_ATR_NOSWITCH) || 
		( hIme->v_wAttrib & IME_ATR_DISABLE )
        || (IsWordInput(hIme->v_nExtend) && !(hImc->v_nAttrib & IMC_ATR_WORD))
        || (IsAlphabetInput(hIme->v_nExtend) && (hImc->v_nAttrib
        & IMC_ATR_WORD))
        || (IsChineseInput(hIme->v_nExtend) && (hImc->v_nAttrib
        & IMC_ATR_NOWORD)))
		goto GET_MATCH;

	return	f_ImeShow ( hImc, hIme, TRUE );
}

/**********************************************************************
 * Function     f_ProcessKeyEvent
 * Purpose      
 * Params       hImc, wKeyCode, dwKeyData
 * Return       
 * Remarks      
 **********************************************************************/

static BOOL f_ProcessKeyEvent(HIMC hImc, WORD wKeyCode, DWORD dwKeyData)
{
    int nRet = 0;

    if (hImc->v_bCatchUp)
    {
        hImc->v_bCatchUp = FALSE;
        if (ISKEYUP(dwKeyData))
            return TRUE;
    }

    nRet = hImc->v_hShow->v_hIme->f_Inquire(hImc->v_hShow->v_hThis, 
        IME_OPA_EVENT, (WPARAM)wKeyCode, (LPARAM)dwKeyData);

    if (f_HandleHotKey ( hImc, dwKeyData ))
    {
        return TRUE;
    }

    return nRet;
}
