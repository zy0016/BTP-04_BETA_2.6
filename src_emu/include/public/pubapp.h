/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : 
 *
\**************************************************************************/

#ifndef _PUBAPP_H
#define _PUBAPP_H

#include "window.h"

#define			WM_MSGWINOK				(WM_USER + 4321)
#define WNDWIDTH  240//128176
#define WNDHEIGHT 296//160216
#define	PLX_NOTIFICATON_POS	0,46,176,174
#define	SEPARATORS	"_/"
typedef enum
{
    Notify_Info		= 1,	//Info notification
    Notify_Failure	= 2,	//Failure notification
    Notify_Success	= 3,    //Success notification
	Notify_Alert	= 4,	//Alert notification 
	Notify_Request	= 5,	//Request notification
	Notify_Wait		= 6,
	Notify_Batter	= 7
}NOTIFYTYPE;


/*
BOOL PLXVerifyPassword(HWND hParentWnd,PCSTR szCaption, PCSTR szPrompt, PSTR szPwd, DWORD dwSizeRange, //int nSize, 
                      PCSTR szOk, PCSTR szCancel, int nTimes);
*/
BOOL PLXVerifyPassword(HWND hParentHwnd, PCSTR szCaption, PCSTR szPrompt, PSTR szPwd, DWORD dwSizeRange, //int nSize, 
                      PCSTR szOk, PCSTR szCancel,int nTimes, UINT uMsg);

BOOL SSPLXVerifyPassword(HWND hParentHwnd, PCSTR szCaption, PCSTR szPrompt, PSTR szPwd, DWORD dwSizeRange, //int nSize, 
                      PCSTR szOk, PCSTR szCancel, int nTimes);

BOOL PLXVerifyPin(PCSTR szCaption, PSTR szPwd, int nSize, 
                 PCSTR szOk, PCSTR szCancel, PCSTR szIcon);
BOOL PLXModifyPassword(PCSTR szCaption, PSTR szPwdOld, DWORD dwOldSizeRange, PSTR szPwdNew, DWORD dwNewSizeRange, PCSTR szOk, 
                      PCSTR szCancel);
BOOL PLXConfirmWin(HWND hFrame, HWND hParent, PCSTR szPrompt, NOTIFYTYPE TipsPic, PCSTR szCaption, 
                   PCSTR szOk, PCSTR szCancel );
void PLXTipsWin(HWND hFrame, HWND hParent, UINT	rMsg, const char *szPrompt,PCSTR szCaption,NOTIFYTYPE Tipspic, 
				PCSTR szOK, PCSTR szCancel, int nTimeout );
BOOL PLXSelectColor(PCSTR szCaption, COLORREF cr, BYTE *pr, BYTE *pg, BYTE *pb, 
                   PCSTR szOk, PCSTR szCancel);
void WaitWindowStateEx(HWND hParent, BOOL bShow, PCSTR cPrompt, PCSTR szCaption,PCSTR szOk,PCSTR szCancel);
void WaitWindowStateDaemonEx(HWND hParent, BOOL bShow);
void WaitWin(HWND hParent, BOOL bShow, PCSTR cPrompt, PCSTR szCaption,PCSTR szOk,PCSTR szCancel, UINT rMsg);
BOOL	UpdateWaitwinContext(PCSTR buf);
BOOL	UpdateTipswinPrompt(PCSTR pBuf);
BOOL	UpdateConfirmExPrompt(PCSTR pBuf);
void SureIcon(char* CONFGIF,NOTIFYTYPE TipsPic);
DWORD FormatStr(char* sMsgWinPrompt, char* szPrompt);
BOOL	TopWndIsNotification();
BOOL IsNotification();
BOOL PLXConfirmWinEx(HWND hFrame, HWND hWnd, PCSTR szPrompt, NOTIFYTYPE ePic, PCSTR szCaption, 
                  PCSTR szOk, PCSTR szCancel, UINT msg);

int	GetPhoneImageSpace(PSTR pPath);
int	GetMMCImageSpace();
BOOL PLXConfirmWinExOem(HWND hFrame, HWND hWnd, PCSTR szPrompt, NOTIFYTYPE ePic, PCSTR szCaption, 
                  PCSTR szOk, PCSTR szCancel, UINT msg);
void PLXTipsWinForSTK(HWND hFrame, HWND hParent, UINT	rMsg, PCSTR szPrompt,PCSTR szCaption,NOTIFYTYPE Tipspic,PCSTR szOK, PCSTR szCancel, int nTimeout );
void PLXTipsWinOem(HWND hFrame, HWND hParent, UINT	rMsg, PCSTR szPrompt,PCSTR szCaption,NOTIFYTYPE Tipspic,PCSTR szOK, PCSTR szCancel, int nTimeout );
/*
*	public function interface
*/
char *floattoa(float real, char *fstring);
char	* plxstrrev(char * string);
char	* plxstrtok (char * string,  const char * control );
BOOL	findstr(char **src,char *dec);
char	*itoa(int a, char *p, int b);
int		GetAvailFlashSpace();
int		GetAvailMMCSpace();
DWORD		GetFreeMemory();
/***** menulist macro definition and function declaration *****/
// Menulist style
#define MLS_SEPAPIC         0x0001      // item separator is picture

// focus notification
#define ML_FOCUSNOTIFY      1
// select notification
#define ML_SELNOTIFY        10


#define ML_SETBGPIC         WM_USER+100 // set background picture
#define ML_SETITEMSIZE      WM_USER+101 // set item width and height
#define ML_SETITEMSTARTPOS  WM_USER+102 // set position of the first item
#define ML_SETICONOFFSET    WM_USER+103 // set icon offset from the left

#define ML_SETITEMICON      WM_USER+110 // set item icon
#define ML_SETITEMDATA      WM_USER+111 // set item data

#define ML_GETITEMDATA      WM_USER+120 // get item data

#define ML_ADDITEM          WM_USER+130 // add item
#define ML_DELITEM          WM_USER+131 // del item

#define ML_SETITEMTEXT      WM_USER+132
#define ML_GETITEMTEXT      WM_USER+133
#define ML_GETITEMTEXTLEN   WM_USER+134

#define ML_SETCURSEL        WM_USER+135
#define ML_GETCURSEL        WM_USER+136

#define ML_RESETCONTENT     WM_USER+137

#define ML_SETITEMPIC       WM_USER+138
#define ML_GETITEMPIC       WM_USER+139
#define ML_SETBKCOL         WM_USER+140
#define ML_GETBKCOL         WM_USER+141

// item align
#define MLI_LEFT        0x0000
#define MLI_CENTER      0x0001
#define MLI_RIGHT       0x0002
#define MLI_HMASK       0x0003

// item notification flag
#define MLI_NOTIFY      0x0004

// item focus flag
#define MLI_SELECTED    0x0100
#define MLI_CHANGEICON  0x0200

typedef void    *HMENULIST;

typedef struct tagMENULISTITEMTEMPLATE
{
    WORD    wID;                // item ID
    DWORD   dwFlags;            // item flags
    int     nPicAreaWidth;      // icon area width
    PCSTR   lpszIconName;       // icon name
    PCSTR   lpszItemName;       // item string
} MENULISTITEMTEMPLATE, *PMENULISTITEMTEMPLATE;

typedef struct tagMENULISTTEMPLATE
{
    //DWORD                   version;
    //BOOL                    bVScroll;
    DWORD                   dwStyle;        // style of Menulist
    DWORD                   cmlit;          // Count of Menulist items
    PCSTR                   lpszBgPic;      // background bitmap name
    PVOID                   separator;      // item separator
    int                     nItemWidth;     // item width
    int                     nItemHeight;    // item height
    int                     nTopLeftX;      // topleft x of the top item
    int                     nTopLeftY;      // topleft y of the top item
    int                     nLeftGap;       // left gap of each icon
    int                     nUArrowX;       // topleft x of the up arrow
    int                     nUArrowY;       // topleft y of the up arrow
    int                     nUArrowWidth;   // width of the up arrow
    int                     nUArrowHeight;  // height of the up arrow
    PCSTR                   lpszUAPic;      // bitmap name of the up arrow
    int                     nDArrowX;       // topleft x of the down arrow
    int                     nDArrowY;       // topleft y of the down arrow
    int                     nDArrowWidth;   // width of the down arrow
    int                     nDArrowHeight;  // height of the down arrow
    PCSTR                   lpszDAPic;      // bitmap name of the down arrow
    PCSTR                   lpszItemFocus;
    PCSTR                   lpszItemNFocus;
    PMENULISTITEMTEMPLATE   pmlItems;       // Menulist items: mlItems[cdit]
} MENULISTTEMPLATE;

HMENULIST LoadMenuListIndirect(const MENULISTTEMPLATE* pMenuListTemplate);
BOOL DestroyMenuList(HMENULIST hMenuList);
HMENULIST   LoadMenuList(MENULISTTEMPLATE * MenuListElement,BOOL bLostkeyNotify);
void    MenuList_AddString(HWND hMenuList,char ** p,int * id,char ** pic);
void    MenuList_AddStringOne(HWND hMenuList,const char * pItemName,int id,const char * pIconName);
BOOL    ResetMenuList(HWND hMenuList,int * id);
/***** menulist macro definition and function declaration *****/

/***** Window Standard Control Extension *****/

BOOL WSCTRLEX_Init(void);

/*********************************************/
/*             Unit Edit control             */
/*********************************************/

#ifndef NOCTLMGR

/* UNITEDIT control styles */
#ifndef NOWINSTYLES
#define UES_TIME             0x00000006L
#define UES_DATE             0x00000007L
#define UES_TIMEZONE         0x00000008L
#define UES_IPADDR           0x00000009L
#define UES_TIMEPERIOD       0x0000000AL
#define UES_COORDINATE       0x0000000BL
#define UES_TIME_24HR        0x00000000L
#define UES_TIME_12HR        0x00000100L
#define UES_DATE_YMD         0x00000200L
#define UES_DATE_MDY         0x00000400L
#define UES_DATE_DMY         0x00000800L
#define UES_TIME_HM          0x00000000L
#define UES_TIME_HMS         0x00001000L
#define UES_LATITUDE         0x00000000L
#define UES_LONGITUDE        0x00000100L
#define UES_UTM_EASTING      0x00000200L
#define UES_UTM_NORTHING     0x00000400L
#define UES_COORDINATE_DMS   0x00000000L
#define UES_COORDINATE_DM    0x00001000L
#define UES_COORDINATE_D     0x00002000L
#define UES_TITLE            0x00008000L
#endif  /* NOWINSTYLES */

/* UNITEDIT control messages */
#ifndef NOWINMESSAGES

#define UEM_GETDATA          0x00C5
// wParam : unused
// lParam : pointer to the SYSTEMTIME structure variable

#define UEM_SETTITLE         0x00C6
// wParam : unused
// lParam : pointer to a null-terminated string

#define UEM_SETSEPARATOR     0x00C7
// wParam : unused
// lParam : char

#endif  /* NOWINMESSAGES */

// And this is a useful macro for making the IP Address to be passed
// as a LPARAM.

#define MAKEIPADDRESS(b1,b2,b3,b4)  ((LPARAM)(((DWORD)(b1)<<24)+((DWORD)(b2)<<16)+((DWORD)(b3)<<8)+((DWORD)(b4))))

// Get individual number
#define FIRST_IPADDRESS(x)  ((x>>24) & 0xff)
#define SECOND_IPADDRESS(x) ((x>>16) & 0xff)
#define THIRD_IPADDRESS(x)  ((x>>8) & 0xff)
#define FOURTH_IPADDRESS(x) (x & 0xff)

// COORDINATE

typedef struct tagANGLEDEG
{
    BOOL  bSW;
    INT   nDegree;
    INT   nMinute;
    INT   nSecond;
    INT   nMilliseconds;
}
ANGLEDEG, *PANGLEDEG;

typedef struct tagCOORDINATE
{
    PANGLEDEG  adLatitude;
    PANGLEDEG  adLongitude;
}
COORDINATE, *PCOORDINATE;

#define DMSTODEG(pad) ((pad)->nDegree + (pad)->nMinute / 60.0         \
                       + (pad)->nSecond / (60.0 * 60.0)               \
                       + (pad)->nMilliseconds / (1000 * 60.0 * 60.0))

#define DEGTODMS(dDegree, pad)       \
{                                    \
double dReal = 0.0;                  \
dReal = (dDegree);                   \
(pad)->nDegree = (int)dReal;         \
dReal = 60 * (dReal - (int)dReal);   \
(pad)->nMinute = (int)dReal;         \
dReal = 60 * (dReal - (int)dReal);   \
(pad)->nSecond = (int)dReal;         \
dReal = 1000 * (dReal - (int)dReal); \
(pad)->nMilliseconds = (int)dReal;   \
}

#define MSTOMIN(pad)  ((pad)->nMinute + (pad)->nSecond / 60.0  \
                       + (pad)->nMilliseconds / (1000 * 60.0))

typedef struct tagUTMZONE
{
    BYTE   byZoneNum;
    char   chZoneID;
    DWORD  dwDirectPart;
}
UTMZONE, *PUTMZONE;

#endif  /* NOCTLMGR */

/*********************************************/
/*             Time Edit control             */
/*********************************************/

#ifndef NOCTLMGR

/* TIMEEDIT control styles */
#ifndef NOWINSTYLES
#define TES_TIME_24HR        UES_TIME_24HR
#define TES_TIME_12HR        UES_TIME_12HR
#define TES_DATE_YMD         UES_DATE_YMD
#define TES_DATE_MDY         UES_DATE_MDY
#define TES_DATE_DMY         UES_DATE_DMY
#define TES_LEFT             0x00000000L
#define TES_CENTER           0x00000000L
#define TES_RIGHT            0x00000000L
#define TES_READONLY         0x00000000L
#define TES_TITLE            UES_TITLE
#endif  /* NOWINSTYLES */

/* TIMEEDIT control messages */
#ifndef NOWINMESSAGES

#define TEM_GETTIME          UEM_GETDATA
// wParam : unused
// lParam : pointer to the SYSTEMTIME structure variable

#define TEM_SETTITLE         UEM_SETTITLE
// wParam : unused
// lParam : pointer to a null-terminated string

#define TEM_SETSEPARATOR     UEM_SETSEPARATOR
// wParam : unused
// lParam : char

#endif  /* NOWINMESSAGES */

#endif  /* NOCTLMGR */

/***** Window Standard Control Extension *****/

/***** SMS count*****/
#define SMS_MODE_GSM     0
#define SMS_MODE_UNICODE 1

/**********************************************************************
* Function     CountSmsChar
* Purpose      Counts the number of characters in SMS string.
* Params       pszText
*                [in] SMS text, it is a null-terminated string.
*              puCount
*                [in/out] Pointer to an UINT varible that receives the
*                result of counting.
* Return       SMS sending mode. The following values are defined.
*              Value                 Meaning
*              SMS_MODE_GSM          Send by coding in GSM.
*              SMS_MODE_UNICODE      Send by coding in Unicode.
* Remarks      Returns -1 if not successfully.
**********************************************************************/

int CountSmsChar(PCSTR pszText, PUINT puCount);

HWND PlxTextView(HWND hFrame, HWND hParent, PSTR szContent, int nTextLen, BOOL bImage, PSTR szImgPath, PVOID pImgData, int nDataSize);
#define TVM_FINDNHL     500
#define TVM_GETCURHL    501
#define TVM_HIDEHL      502

#define TVS_NUMBER      1
#define TVS_URL         2
#define TVS_EMAIL       3
#define TVS_COORDINATE  4

int GetValidUrlEx(const char * url, int len);
int GetCursel(PSTR pStrSource,int textLength,int curOffset,int curlength,int *offset,int* length,int bType,int bForward);
#endif  // _PUBAPP_H
