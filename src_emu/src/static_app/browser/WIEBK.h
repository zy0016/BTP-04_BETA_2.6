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

#ifndef WIEBK_C
#define WIEBK_C

#define	WIE_IDM_BM_OK         (WM_USER+501)
#define WIE_IDM_BM_CLOSE      (WM_USER+502)
#define WIE_ID_BM_LIST        (WM_USER+503)   
#define WIE_IDM_BM_ADD        (WM_USER+504)    
#define WIE_IDM_BM_EDIT       (WM_USER+505)
#define WIE_IDM_BM_DEL        (WM_USER+506)    
#define WIE_IDM_BM_GO         (WM_USER+507)
#define WIE_IDM_BM_EXIT       (WM_USER+508)
#define WIE_IDM_BM_CHGNAME    (WM_USER+500)
#define WIE_IDM_BME_CLOSE     (WM_USER+508)
#define WIE_IDM_BME_SAVE      (WM_USER+509)
#define WIE_IDM_BM_EDITPAGE   (WM_USER+510)
#define WIE_IDM_BM_CURPAGE    (WM_USER+511)
#define WIE_IDM_BM_EDITNAME   (WM_USER+512)
#define WM_IMEN_CHANGE        (WM_USER+100)
#define WIE_SETCURSOR		  (WM_USER+513)
#define WIE_IDM_BM_DELCONFIRM (WM_USER+514)
#define WIE_IDM_BM_OVERWRITE  (WM_USER+515)
#define WIE_IDM_BM_JUDGETOSAVE (WM_USER+516)
#define WIE_IDM_BM_NOTSAVE	  (WM_USER+517)

#define WIE_BM_NAMELEN        41
#define WIE_BM_MAXNUM         40

typedef struct tagBookMark
{
    int  nUrlType;
    char szBMName[WIE_BM_NAMELEN];
    char *szBMUrl;    
    BOOL ibChange;
}WIEBookMark;

BOOL On_IDM_BM(HWND hParent, RECT rClient);
int WBM_ADD_FROMURL();
void WBK_Close_Window();
int WIE_BM_Edit(int id, HWND hParent, HWND hList, char * url, RECT rClient, BOOL bEditorNot);
int WIE_LoadBookMark();
void WIE_FreeAllBm();
int WIE_SaveBookMark();

#endif
