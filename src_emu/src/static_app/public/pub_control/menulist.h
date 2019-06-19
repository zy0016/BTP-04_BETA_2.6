/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : app controls model for mobile product
 *
 * Purpose  : implement MenuList control
 *            
\**************************************************************************/


#ifndef _MENULIST_H
#define _MENULIST_H

#include "window.h"
#include "hpimage.h"

#include "pubapp.h"

#define ML_CLASSNAME    "MENULIST"

#define MINITEMWIDTH    6

typedef struct tagMLINODE
{
    WORD                wID;
    WORD                wSerial;
    DWORD               dwFlags;
    PSTR                pNodeString;
    int                 nStrLen;
    void                *pNodeData;
    //HICON               hIcon;
	HIMAGE				hIcon;
	int					nFileType;
    PSTR                pSelIconName;
    int                 nPicAreaWidth;
    SIZE                IconSize;
    struct tagMLINODE   *pPrev;
    struct tagMLINODE   *pNext;
} MLINODE, *PMLINODE;

typedef struct tagMENULISTDATA
{
    HWND    hWnd;
    MLINODE *pHead;
    MLINODE *pTopNode;
    MLINODE *pCurNode;
    DWORD   dwStyle;
    MLINODE *pTail;
    WORD    wID;
    WORD    wPageItemCount;
    WORD    wTotalItemCount;
    WORD    wReserved;
    int     nCliWidth;
    int     nCliHeight;
    int     nItemWidth;
    int     nItemHeight;
    int     nTopLeftX;
    int     nTopLeftY;
    int     nLeftGap;
    //HBITMAP hBgBmp;
    HGIFANIMATE hGifBg;
    //HBITMAP hSeparator;
    HGIFANIMATE hGifSeparator;
    int     nBmpWidth;
    int     nBmpHeight;
    int     nUArrowX;
    int     nUArrowY;
    int     nUArrowWidth;
    int     nUArrowHeight;
    //HBITMAP hUArrowBmp;
    HGIFANIMATE hGifUp;
    int     nUABmpWidth;
    int     nUABmpHeight;
    int     nDArrowX;
    int     nDArrowY;
    int     nDArrowWidth;
    int     nDArrowHeight;
    //HBITMAP hDArrowBmp;
    HGIFANIMATE hGifDown;
    int     nDABmpWidth;
    int     nDABmpHeight;
    BOOL    bVScroll;
    HGIFANIMATE hGifItemF;
    HGIFANIMATE hGifItemNF;
    COLORREF    BkColor;
} MENULISTDATA, *PMENULISTDATA;

#endif  // _MENULIST_H
