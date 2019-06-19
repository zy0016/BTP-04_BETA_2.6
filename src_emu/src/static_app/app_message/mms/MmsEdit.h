/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : MMS	
 *
 * Purpose  : define consts and functions etc.
 *            
\**************************************************************************/

#ifndef   MMSEDIT_
#define	  MMSEDIT_

#include "MmsGlobal.h"
#include "hpimage.h"
#include "MmsInterface.h"
#include "PreBrow.h"
#include "Recipient.h"
/******************************************************************** 
* 	   
*	definination
* 	   
**********************************************************************/
#define MMS_EDIT_INSERT     WM_USER + 21
#define MMS_EDIT_PHRASE     WM_USER + 22
#define MMS_EDIT_BMAIL      WM_USER + 23
#define WM_MMS_EXIT         WM_USER + 24
#define WM_RETURN_EMAIL		WM_USER + 25
#define WM_RETURN_PHONENO	WM_USER + 26
#define EDIT_MSG_STOP       WM_USER + 27
#define EDIT_MSG_WRITE      WM_USER + 28
#define EDIT_MSG_DATA       WM_USER + 29
#define WM_EXIT_OK          WM_USER + 30
#define MMS_IME_CONT        WM_USER + 31
#define MMS_IME_SAVETEMP    WM_USER + 32
#define MMS_EXIT_SAVE       WM_USER + 33

#define MMS_REMOVE_SLIDE    WM_USER + 38
#define MMS_ADD_RECIPIENT   WM_USER + 39

#define MMS_INSERT_TEMP     WM_USER + 40

#define MMS_EDIT_APP		"MMS"
#define MMS_RECV_STR		"To:"	
#define MMS_IMAGE_ALT		 "Unable to show the image"

#define SOUND_NAME			"/rom/message/mms/rightaudio.ico"
#define ATTATCH_NAME        "/rom/message/unibox/mail_aff.bmp"

#define MAX_TXT_SIZE		1024*10
#define TXT_UTF8_MUL        1//(3/2)

#define ID_PHONENUM			221
#define ID_CONTENT			222

#define BMP_WIDTH			118
#define BMP_HEIGHT			28


#define PHONENUM_X			0
#define PHONENUM_Y		    0
#define PHONENUM_HEIGHT		60
#define  SUBJECT_Y 10
#define SUBJECT_HEIGHT 20


#define RECTI_LEFT			56
#define RECTI_RIGHT			120
#define RECTI_TOP		    (SUBJECT_Y + SUBJECT_HEIGHT + 2)
#define RECTI_BOTTOM		(RECTI_TOP + 64)

#define CONTENT_Y			(SUBJECT_Y + SUBJECT_HEIGHT)
#define CONTENT_HEIGHT		(SCREEN_HEIGHT - CONTENT_Y - 39)


#define MENU_EDIT_EXIT		200
#define IDS_EDIT_EXIT		STR_MMS_EXIT

#define IDM_INSERT_IMG		2011
#define IDM_INSERT_AMR		2012
#define IDM_INSERT_SLIDE	2013
#define IDM_INSERT_TEPLATE	2014

#define IDM_SEND			202
#define IDM_PREVIEW			203
#define	IDM_REMOVE			204
#define IDM_REMOVE_IMG		2041
#define IDM_REMOVE_SLIDE	2042
#define IDM_REMOVE_AMR		2043
#define IDM_OBJ				205
#define IDM_SLIDEADD		206

#define MMS_EDIT_DISPLAY    2081

#define IDM_MMSINFO			208
#define IDM_SAVEMMS			209

#define IDM_EDIT_LEFT       210
#define IDM_SELBACK			211
#define IDM_SELOK			212

#define IDC_SELNUM			213
#define IDC_SELMAIL			214

#define IDM_SENDOPTIONS		215
#define IDM_ADDRECI			216

#define IDS_SEND			"Send"
#define IDS_SENDOPT			"Send with options"
#define IDS_SAVEMMS			"Save to Drafts"
#define IDS_ADDRECI			"Add recipient"
#define IDS_INSERT			"Insert"
#define IDS_REMOVE			"Remove"
#define IDS_PREVIEW			"Preview"
#define IDS_OBJ				"Objects"
#define IDS_INFO			"Details"

#define IDS_INSERT_IMG		"Picture"
#define IDS_INSERT_AMR		"Sound"
#define IDS_SLIDE			"Slide"
#define IDS_INSERT_TXT		"Template"

#define IDS_REMOVE_IMG		"Picture"
#define IDS_REMOVE_AMR		"Sound"
#define IDS_REMOVE_SLIDE	"Slide"

#define  VIEW_CALLOBJ		0
#define  EDIT_CALLOBJ		1

#define MMS_EDIT_SPACE      5
/******************************************************************** 
* 
*		data structure definition
* 
**********************************************************************/
typedef struct tagMMS_EditCreateData
{
	HINSTANCE       hInst;
	HWND            hWndFrame;
	BOOL			bPlayMusic;                 // 
	BOOL			bContKeyDown;
	HWND			hFocus;
	int				nHeight;
	HMENU			hMenu;
	
    TotalMMS		tmms;
    char			mmsDraftFile[MAX_FILENAME];
    int				curSlide;
	int             delSlide;
	MMSMSGINFO		EditMsgInfo;
	mmsMetaNode		*pEnd;
	mmsMetaNode     *pDel;
	CARETPROC		OldCaretProc;

	BOOL			bModify;
	BOOL			bDraft;
	BOOL			bSave;
	BOOL            bSend;
	HDC             hdcPic[MMS_MAXNUM];
	HDC             hDCLeftIcon;
	HBITMAP         hBmpLeftIcon;
	PSTR            InsertFileName;	

	WPARAM          iconorbmp;
	LPARAM          handle;
	char            szIconName[128];
}MMS_EDITCREATEDATA,*PMMS_EDITCREATEDATA;
/******************************************************************** 
* 
*		function definition
* 
**********************************************************************/

LRESULT MMSEditWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);

static LRESULT SelWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);

static void CallSelNumOrMail(HWND hwnd, UINT msg);
static int  InitMms(PMMS_EDITCREATEDATA pCreateData, PSTR FileName, UINT nFlag);
static void SetTextInfo(void);
static void SetMMSContent(HWND hWnd);
static BOOL CreateAddCtrl(HWND hWnd);
static BOOL CreateControls(HWND hWnd);
static BOOL OnSaveMMS(HWND hWnd, const char * mmsSaveFileName);
static BOOL OnSendMms(HWND hWnd, BOOL bSend);
static int  CalPoistion(HWND hWnd, SLIDE *, int);
static void DestroyEdit(void);
static int  SaveCurSlideTxt(HWND hWnd);
static void MoveBetweenSlides(HWND , int, int);
static void FreeAllMMSData(PMMS_EDITCREATEDATA pCreateData);

static int GetPhoneNum(char * szString);
static int GetMailNum(char *szString);
static void MMS_ReplyAll(PMMS_EDITCREATEDATA pCreateData, int hMMSFile);

extern int PackMmsBody(const char *pFileName, TotalMMS tmms);
static void GetAttacthEnd(PMMS_EDITCREATEDATA pCreateData);
static int GetTextType(PCSTR pFileName);
static int GetSoundType(int hFile, PCSTR pFileName);
static int  GetImageType(int hFile);

void MMS_Insert(HWND hWnd);
static mmsMetaNode* OnInsertAttatch(HWND hWnd, int nAttatchType, PCSTR szFileName, BOOL bSmil);
static void AddNameofObject(PSLIDE pcurSlide, int i);
static BOOL DeleteAttatch(PMMS_EDITCREATEDATA pCreateData, mmsMetaNode *pDel);
extern BOOL CallMMSObjList(HWND hWndFrame, HWND hWndParent, mmsMetaNode *pObjMeta, int nCallType);
extern void FreeMeta(mmsMetaNode *pMeta);
extern mmsMetaNode* GetAttatchNotInSmil(mmsMetaNode *p);
extern MmsWndClass MMSWindowClass[MAX_CLASS_NUM];

static	LRESULT	MMS_NewConWndProc (HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);

#endif 
