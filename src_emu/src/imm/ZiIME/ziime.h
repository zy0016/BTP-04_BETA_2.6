/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 *    Copyright (c) 2005-2005 by Pollex Mobile Software Co., Ltd.
 *                       All Rights Reserved
 *
 * Module   :  ziime.h
 *
 * Purpose  :  
 *
\**************************************************************************/

#ifndef _ZIIME_H
#define _ZIIME_H

#include "zi8api.h"

#define ZI_ELEM_BUF_SIZE         0x100
#define ZI_CAND_BUF_SIZE         0x200
#define ZI_SCRA_BUF_SIZE         0x100
#define ZI_MAX_CANDIDATES        8
#define ZI_ASSOC_BUF_SIZE        0x40

#define IMEP_ENTERASSOCMODE    0x00000001
#define IMEP_ENTERCANDIMODE    0x00000002
#define IMEP_INVALIDINPUT      0x00000003
#define IMEP_INPUTELEMENT      0x00000004
#define IMEP_INITZIGETPARAM    0x00000005
#define IMEP_GETASSOCIATES     0x00000006

typedef LRESULT (CALLBACK *PFNIMEPROC)(UINT, WPARAM, LPARAM);

typedef struct tagZICHNPARAM
{
    PZI8VOID       pGlobal;
	ZI8GETPARAM	   GetParam;
    ZI8WCHAR       wcElements[ZI_ELEM_BUF_SIZE];
    ZI8WCHAR       wcCandidates[ZI_CAND_BUF_SIZE];
    ZI8UCHAR       ucScratch[ZI_SCRA_BUF_SIZE];
    ZI8WCHAR       wcCandiCode[ZI_CAND_BUF_SIZE];

    ZI8LONG        lTotalAssoc;
    ZI8LONG        lTotalChinese;
    ZI8UCHAR       iAssocOffset;
    ZI8UCHAR       iChineseOffset;
    ZI8UCHAR       nElement;
    ZI8WCHAR       wcChinese[ZI_MAX_CANDIDATES];
    ZI8UCHAR       szAssoc[ZI_ASSOC_BUF_SIZE];
}
ZICHNPARAM, *PZICHNPARAM;

typedef struct tagZICHNIME
{
    DWORD          fdwStatus;
    int            nRepeat;
    char           *pszCharTable;
    PZICHNPARAM    pZiParam;
    PFNIMEPROC     pfnImeProc;
    HWND           hwndEdit;
    HWND           hWnd;
}
ZICHNIME, *PZICHNIME, **PPZICHNIME;

BOOL ImeLoadProc(int nType, PFNIMEPROC *ppfnImeProc);

#endif  // _ZIIME_H
