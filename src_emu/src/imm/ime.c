/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : ime.c
 *
 * Purpose  : define input method edit entry and implement output function
 *            
\**************************************************************************/

#include	"window.h"
#include	"imm.h"

/**************************************************************************\
 *	Purpose:	declare ime entry function
 *	Remarks:	   
 *
 **************************************************************************/

DWORD AbcImeWinMain(PVOID hThis, INT32 nCode, WPARAM nOne, LPARAM nTwo);
DWORD NumberImeWinMain(PVOID hThis, INT32 nCode, WPARAM nOne, LPARAM nTwo);
DWORD SymImeWinMain(PVOID hThis, INT32 nCode, WPARAM nOne, LPARAM nTwo);
DWORD ZiAbcImeWinMain(PVOID hThis, INT32 nCode, WPARAM nOne, LPARAM nTwo);
DWORD ZiChnImeWinMain(PVOID hThis, INT32 nCode, WPARAM nOne, LPARAM nTwo);

/**************************************************************************\
 *	Purpose:	define single ime entry 
 *	Remarks:	   
 *
 **************************************************************************/

static    UIME    abcIme =
{
	"abc",
#ifdef IME_ICON_USE
    "imeabcl.ico",
#else
	"ab ",
#endif
	0, 0x00A0, 0, 
	IME_HKEY_ACTIVE,
	&AbcImeWinMain
};

static    UIME    AbcIme =
{
	"Abc",
#ifdef IME_ICON_USE
    "imeabct.ico",
#else
	"At ",
#endif
	0, 0x00A1, 0, 
	IME_HKEY_ACTIVE,
	&AbcImeWinMain
};

static    UIME    ABCIme =
{
	"ABC",
#ifdef IME_ICON_USE
    "imeabcu.ico",
#else
	"AB ",
#endif
	0, 0x00A2, 0, 
	IME_HKEY_ACTIVE,
	&AbcImeWinMain
};

static    UIME    Ziabc =
{
	"Ziabc",
#ifdef IME_ICON_USE
    "imeabcl.ico",
#else
	"ab ",
#endif
	0, 0x00B0, 0, 
	IME_HKEY_ACTIVE,
	&ZiAbcImeWinMain
};

static    UIME    ZiAbc =
{
	"ZiAbc",
#ifdef IME_ICON_USE
    "imeabct.ico",
#else
	"Ab ",
#endif
	0, 0x00B1, 0, 
	IME_HKEY_ACTIVE,
	&ZiAbcImeWinMain
};

static    UIME    ZiABC =
{
	"ZiABC",
#ifdef IME_ICON_USE
    "imeabcu.ico",
#else
	"AB ",
#endif
	0, 0x00B2, 0, 
	IME_HKEY_ACTIVE,
	&ZiAbcImeWinMain
};

static    UIME    ZiPhn =
{
    "Pinyin",
#ifdef IME_ICON_USE
    "imephn.ico",
#else
    "Py ",
#endif
    0, 0x00C0, 0,
    IME_HKEY_ACTIVE,
    &ZiChnImeWinMain
};

static    UIME    ZiBpmf =
{
    "Bpmf",
#ifdef IME_ICON_USE
    "imebpmf.ico",
#else
    "Py ",
#endif
    0, 0x00C1, 0,
    IME_HKEY_ACTIVE,
    &ZiChnImeWinMain
};

static    UIME    ZiStks =
{
    "Stroke",
#ifdef IME_ICON_USE
    "imestks.ico",
#else
    "Bh ",
#endif
    0, 0x00C2, 0,
    IME_HKEY_ACTIVE,
    &ZiChnImeWinMain
};

static    UIME    ZiStkt =
{
    "Stroket",
#ifdef IME_ICON_USE
    "imestkt.ico",
#else
    "Bh ",
#endif
    0, 0x00C3, 0,
    IME_HKEY_ACTIVE,
    &ZiChnImeWinMain
};

static    UIME    numIme =
{
	"Num",
#ifdef IME_ICON_USE
    "imenum.ico",
#else
	"12 ",
#endif
	0, 0x0000, 0, 
	IME_HKEY_ACTIVE,
	&NumberImeWinMain
};

static    UIME    phoneIme =
{
	"Phone",
#ifdef IME_ICON_USE
    "imenum.ico",
#else
	"12 ",
#endif
	0, 0x0001, IME_ATR_NOSWITCH,
	IME_HKEY_ACTIVE,
	&NumberImeWinMain
};

static    UIME    dgtIme =
{
	"Digit",
#ifdef IME_ICON_USE
    "imenum.ico",
#else
	"12 ",
#endif
	0, 0x0002, IME_ATR_NOSWITCH, 
	IME_HKEY_ACTIVE,
	&NumberImeWinMain
};

static    UIME    symIme =
{
    "Sym",
    "",
    0, 0x0010, IME_ATR_NOSWITCH,
    MAKEWORD(IME_EVT_F3, IME_HKEY_LONG),
    &SymImeWinMain
};

/**************************************************************************\
 *	Purpose:	define main ime entry 
 *	Remarks:	   
 *
 **************************************************************************/

static	HIME	hImeEntry[] =
{
    &abcIme,
	&AbcIme,
    &ABCIme,
    &Ziabc,
    &ZiAbc,
    &ZiABC,
#ifndef IME_NO_CHINESE
    &ZiPhn,
    &ZiStks,
//    &ZiStkt,
//    &ZiBpmf,
#endif
  	&numIme,
    &phoneIme,
    &dgtIme,
    &symIme,
	NULL
};

/**************************************************************************\
 * Function	ImeGetEntry ( PLPHIME plphIme );	   
 * Purpose	return main ime entry       
 * Params	plphIme	: a point which to receive main entry 			
 * Return	successed return TRUE 	   
 * Remarks	   
 **************************************************************************/

BOOL	ImeGetEntry ( PLPHIME plphIme )
{
	if ( !plphIme )
		return	FALSE;

    *plphIme = hImeEntry;
    if (*plphIme == NULL)
    {
        return FALSE;
    }
	return	TRUE;
}

/**********************************************************************
 * 
 * Define IME Languages
 * 
 **********************************************************************/

static IMELANG LangEn =
{
    "English",
    "imelanen.bmp"
};

static IMELANG LangSu =
{
    "Suomi",
    "imelansu.bmp"
};

static IMELANG LangSv =
{
    "Svenska",
    "imelansv.bmp"
};

static IMELANG LangNs =
{
    "Norsk",
    "imelanns.bmp"
};

static PIMELANG aImeLang[] =
{
    &LangEn,
    &LangSu,
    &LangSv,
    &LangNs,
    NULL
};

/**************************************************************************\
 * Function	ImeGetLangEntry ( PPIMELANG ppImeLang );	   
 * Purpose	return main ime entry       
 * Params	ppImeLang	: a point which to receive main entry 			
 * Return	successed return TRUE 	   
 * Remarks	   
 **************************************************************************/

BOOL ImeGetLangEntry(PPPIMELANG pppImeLang)
{
	if (pppImeLang == NULL)
		return	FALSE;

    *pppImeLang = aImeLang;
    if (*pppImeLang == NULL)
    {
        return FALSE;
    }
	return	TRUE;
}
