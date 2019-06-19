/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : 
 *
 * Purpose  : 
 *
\**************************************************************************/

#ifndef _VCARDVCAL_H_
#define _VCARDVCAL_H_

#include <vcard/vcaglb.h>

typedef struct tagVcardVcal
{ 
    char*   pszTitle;
    char*   pszContent;
    struct tagVcardVcal *pNext;
}VCARDVCAL,*PVCARDVCAL;

BOOL APP_SaveVcard(char* pVcardData,int DataLen);

BOOL APP_AnalyseVcard(vCard_Obj* pvCardObj,PVCARDVCAL* ppChain);

void APP_ABChainEarse(VCARDVCAL *pChain);

void APP_FreeSource(char* pszVcardFileName);

#endif
