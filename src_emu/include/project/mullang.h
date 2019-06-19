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

#ifndef __MULLANG_H
#define __MULLANG_H

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned short  WCHAR;

#ifdef UNICODE

#define TCHAR   WCHAR

#define _T(s)   L##s
#define ML(s)   Translate(L##s)

#else   // UNICODE

#define TCHAR   char

#define _T(s)   s
#define ML(s)   Translate(s)

#endif  // UNICODE

#define WM_LANGUAGECHANGED  0x03E0
/*********************************************************************\
* Function	SetActiveLanguage
* Purpose   Set active Language 
* Params
*		
* Return    1:succeed
*           0:failed
*           
* Remarks   If the function succeeds, the return value is 1.
*           If the function fails, the return value is 0.
**********************************************************************/
int SetActiveLanguage(const TCHAR* pszLanguage);
/*********************************************************************\
* Function	GetActiveLanguage
* Purpose   Set active Language 
* Params
*		
* Return    
*           
*           
* Remarks  
**********************************************************************/
const TCHAR* GetActiveLanguage(void);
/*********************************************************************\
* Function	GetLanguageInfo
* Purpose   Get Language count and string
* Params
* Return    1:succeed
*           0:failed
*           
* Remarks   If the function succeeds, the return value is 1.
*           If the function fails, the return value is 0.
* for example 
*           TCHAR** ppszLanguage;
*           int     nCount;
*           if(GetLanguageData(NULL,&nCount)) //get count
*               ;
*           if(GetLanguageData(&ppszLanguage,NULL)) //get language string
*               ;
**********************************************************************/
int GetLanguageInfo(TCHAR*** pszLanguage,int* pnCount);
/*********************************************************************\
* Function	ML_Init
* Purpose   Load Multilanguage String 
* Params
*		
* Return    1:succeed
*           0:failed
*           
* Remarks   If the function succeeds, the return value is 1.
*           If the function fails, the return value is 0.		 
**********************************************************************/
int ML_Init(void);
/*********************************************************************\
* Function	ML_Destroy
* Purpose   free the memory 
* Params
*		
* Return    
*           
*           
* Remarks 
**********************************************************************/
void ML_Destroy(void);

const TCHAR* Translate(const TCHAR* pStr);

#ifdef __cplusplus
}
#endif

#endif  // MULLANG_H
