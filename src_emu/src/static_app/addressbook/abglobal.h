/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : AddressBook
 *
 * Purpose  : ABGlobal.H file
 *            
\**************************************************************************/

#ifndef _ABGLOBAL_H
#define _ABGLOBAL_H

#include <window/window.h>
#include <window/winpda.h> 
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <malloc.h>
#include <stdlib.h>
#include <stdio.h> 
#include <unistd.h>
#include <project/plx_pdaex.h>
#include <public/pubapp.h>
//#include <public/PreBrow.h>
#include <project/mullang.h>
#include <project/imesys.h>
#include <project/MmsUi.h>
#include <image/hpimage.h>
#include <me/me_wnd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <callapp/callpub.h>
#include <setup/setting.h>
#include <public/PreBrowHead.h>
#include <public/Recipient.h>
#include <public/PreBrow.h>
#include <vcard/vcaglb.h>
#include <ab/PhonebookExt.h>
#include <project/MmsUi.h>
#include <project/vcardvcal.h>
#include "plxdebug.h"
#include "abrs.h"
#include "abconst.h"
#include "BtSendData.h"
#include "project/sndmgr.h"
#include "version.h"

typedef int  (*CMPFUNC)( PVOID index1, PVOID index2 );


#define AB_MSG_SIM_WRITE            WM_USER+300
#define AB_MSG_SIM_WRITE_SUCC       WM_USER+301
#define AB_MSG_SIM_WRITE_FAIL       WM_USER+302
#define AB_MSG_SIM_WRITE_FULL       WM_USER+303

#define AB_TYPEMASK_FIRSTNAME       0x00000001
#define AB_TYPEMASK_LASTNAME        0x00000002
#define AB_TYPEMASK_TEL             0x00000004
#define AB_TYPEMASK_TELHOME         0x00000008
#define AB_TYPEMASK_TELWORK         0x00000010
#define AB_TYPEMASK_MOBILE          0x00000020
#define AB_TYPEMASK_MOBILEHOME      0x00000040
#define AB_TYPEMASK_MOBILEWORK      0x00000080
#define AB_TYPEMASK_FAX             0x00000100
#define AB_TYPEMASK_PAGER           0x00000200
#define AB_TYPEMASK_EMAIL           0x00000400
#define AB_TYPEMASK_EMAILWORK       0x00000800
#define AB_TYPEMASK_EMAILHOME       0x00001000
#define AB_TYPEMASK_POSTALADDRESS   0x00002000
#define AB_TYPEMASK_HTTPURL         0x00004000
#define AB_TYPEMASK_JOBTITLE        0x00008000
#define AB_TYPEMASK_COMPANY         0x00010000
#define AB_TYPEMASK_COMPANYTEL      0x00020000
#define AB_TYPEMASK_COMPANYADDRESS  0x00040000
#define AB_TYPEMASK_DTMF            0x00080000
#define AB_TYPEMASK_DATE            0x00100000
#define AB_TYPEMASK_NOTE            0x00200000
#define AB_TYPEMASK_PIC             0x00400000
#define AB_TYPEMASK_TONE            0x00800000


typedef enum{
    IDC_ABEDITCONTACT_FIRSTNAME = 0,
    IDC_ABEDITCONTACT_LASTNAME,
    IDC_ABEDITCONTACT_TEL,
    IDC_ABEDITCONTACT_TELHOME,
    IDC_ABEDITCONTACT_TELWORK,
    IDC_ABEDITCONTACT_MOBILE,
    IDC_ABEDITCONTACT_MOBILEHOME,
    IDC_ABEDITCONTACT_MOBILEWORK,
    IDC_ABEDITCONTACT_FAX,
    IDC_ABEDITCONTACT_PAGER,
    IDC_ABEDITCONTACT_EMAIL,
    IDC_ABEDITCONTACT_EMAILWORK,
    IDC_ABEDITCONTACT_EMAILHOME,
    IDC_ABEDITCONTACT_POSTALADDRESS,
    IDC_ABEDITCONTACT_HTTPURL,
    IDC_ABEDITCONTACT_JOBTITLE,
    IDC_ABEDITCONTACT_COMPANY,
    IDC_ABEDITCONTACT_COMPANYTEL,
    IDC_ABEDITCONTACT_COMPANYADDRESS,
    IDC_ABEDITCONTACT_DTMF,
    IDC_ABEDITCONTACT_DATE,
    IDC_ABEDITCONTACT_NOTE,
};

#define AB_MAXLEN_FIRSTNAME         51
//#define AB_MAXLEN_TEL               42
#define AB_MAXLEN_TELHOME           42
#define AB_MAXLEN_TELWORK           42
#define AB_MAXLEN_MOBILE            42
#define AB_MAXLEN_MOBILEHOME        42
#define AB_MAXLEN_MOBILEWORK        42
#define AB_MAXLEN_FAX               42
#define AB_MAXLEN_PAGER             42
#define AB_MAXLEN_EMAIL             51
#define AB_MAXLEN_EMAILWORK         51
#define AB_MAXLEN_EMAILHOME         51
#define AB_MAXLEN_POSTALADDRESS     51
#define AB_MAXLEN_HTTPURL           51
#define AB_MAXLEN_JOBTITLE          51
#define AB_MAXLEN_COMPANY           51
#define AB_MAXLEN_COMPANYTEL        42
#define AB_MAXLEN_COMPANYADDRESS    51
#define AB_MAXLEN_DTMF              42
#define AB_MAXLEN_DATE              16
#define AB_MAXLEN_NOTE              51

#define AB_MAXLEN_SIMNAME           21
#define AB_MAXLEN_SIMNUM            42

typedef struct tagContact_Item
{ 
    DWORD   dwMask;
    BOOL    bOnly;
    int     nMaxLen;
    BOOL    bNum;
    const char*   pszCpation;
}CONTACT_ITEM,*PCONTACT_ITEM;

#define AB_EDITCONTACT_CTRNUM           22   

extern CONTACT_ITEM Contact_Item[AB_EDITCONTACT_CTRNUM+2];  

typedef struct tagContact_ItemChain
{
    char*  pszData;
    DWORD  dwMask;
    int    nMaxLen;
    BOOL   bChange;
    int    nID;
    struct tagContact_ItemChain *pPioneer;
    struct tagContact_ItemChain *pNext;
}CONTACT_ITEMCHAIN,*PCONTACT_ITEMCHAIN;

typedef struct tagContact_InitData
{ 
    HWND        hCtrlWnd;
    PPHONEBOOK  pSIMPhoneBook;
    int         nTotalNum;
    int         nNumber; 
    int         nInit;
    int         nErrCtrl;
    PBNUMTEXT_INFO  MaxLenInfo;        
}CONTACT_INITDATA,*PCONTACT_INITDATA;

typedef enum{
    AB_UNUSED = 0,
    AB_USED,
}AB_TYPE_USE;

typedef struct tagContact_DBHeader
{ 
    AB_TYPE_USE Used;
    DWORD       id;
    int         nTelID;
    int         nGroup;
    DWORD       dwOffset;
}CONTACT_DBHEADER,*PCONTACT_DBHEADER;

typedef struct tagTable_ID
{ 
    DWORD       id;
    DWORD       dwOffset;
}TABLE_ID,*PTABLE_ID;

typedef struct tagTable_Name
{ 
    DWORD       id;
    int         nGroup;
    char        szName[AB_MAXLEN_FIRSTNAME*2];
}TABLE_NAME,*PTABLE_NAME;

typedef struct tagTable_Tel
{ 
    DWORD       id;
    char        szTel[AB_MAXLEN_TEL];
}TABLE_TEL,*PTABLE_TEL;

typedef struct tagTable_Email
{ 
    DWORD       id;
    char        szEmail[AB_MAXLEN_EMAIL];
}TABLE_EMAIL,*PTABLE_EMAIL;

//memory
typedef struct tagMemory_Name
{ 
    DWORD       id;
    DWORD       dwoffset;
    int         nGroup;
    BYTE        byLen;
    char*       pszName;
}MEMORY_NAME,*PMEMORY_NAME;

typedef struct tagMemory_Tel
{ 
    DWORD       id;
    int         nTelID;
	int			TelCode;
    BYTE        byLen;
    char*       pszTel;
}MEMORY_TEL,*PMEMORY_TEL;

typedef struct tagMemory_Email
{ 
    DWORD       id;
    int         nEmailID;
	int			EmailCode;
    BYTE        byLen;
    char*       pszEmail;
}MEMORY_EMAIL,*PMEMORY_EMAIL;

typedef enum{
    AB_OBJECT_NAME = 0,
    AB_OBJECT_TEL,
    AB_OBJECT_EMAIL,
    AB_OBJECT_ALL,
}AB_NOTIFY_OBJECT;

typedef enum{
    AB_MDU_CONTRACT = 0,
    AB_MDU_SIMCONTRACT,
    AB_MDU_GROUP,
}AB_NOTIFY_MDU;

typedef struct tagNotify_Node
{ 
    HWND        hWnd;
    UINT        uMsg;
    AB_NOTIFY_MDU       nMdu;
    AB_NOTIFY_OBJECT    nObject;
    struct tagNotify_Node *pPioneer;
    struct tagNotify_Node *pNext;
}NOTIFY_NODE,*PNOTIFY_NODE;

typedef enum{
    TIMERID_GET_MEM = 1,
    TIMERID_GET_PHONEBOOK,
    TIMERID_GET_PHONEBOOKLEN,
    SIMINIT_FAILED,
    SIMINIT_SUCCEEDED,
	SIMWRITE_OVERTIMER,
};
//
//typedef enum{
//    PICK_NUMBER = 0,
//    PICK_EMAIL,
//    PICK_NUMBERANDEMAIL,
//}AB_PICK_TYPE;

typedef enum{
    COMPARE_NAME = 0,
    COMPARE_TEL,
    COMPARE_EMAIL,
    COMPARE_ID,
}AB_COMPARE_TYPE;

typedef enum{
    AB_MODE_INSERT = 0,
    AB_MODE_DELETE,
}AB_NOTIFY_MODE;

typedef enum{
    IMG_BMP = 0,
    IMG_GIF,
    IMG_JPEG,
    IMG_WBMP,
    IMG_UNKNOWN,
}IMAGE_TYPE;

typedef enum{
    AB_FIND_NAME = 0,
    AB_FIND_TEL,
    AB_FIND_EMAIL,
    AB_FIND_ID,
}AB_FIND_TYPE;

typedef struct tagAB_QDIAL
{
    int   nNO;
	DWORD id;
	int   nTelID;
}AB_QDIAL,*PAB_QDIAL;

typedef struct tagABVIEWCREATEDATA
{
    CONTACT_ITEMCHAIN *pItem;
    HBITMAP     hLeftArrow;
    HBITMAP     hRightArrow;
    int         nType;
    DWORD       dwoffset;
    DWORD       id;
    PVOID       handle;
    PVOID       pPic;
    PVOID       pTone;
    IMAGE_TYPE  nImageType;
    HGIFANIMATE hGif;
    SIZE        ImageSize;
    BOOL        bStretch;
	int         nTelID;
	HWND		hFrameWnd;
	HMENU		hMenu;
    WNDPROC     OldListWndProc;
}ABVIEWCREATEDATA,*PABVIEWCREATEDATA;

extern BOOL AB_InitSIM(void);
extern BOOL Test1_GenChain(PCONTACT_ITEMCHAIN *ppItem);
extern int  AB_GetContactCount(void);
extern BOOL AB_IsFlashEnough(void);
extern BOOL AB_CreateMainWindow(HWND hFrameWnd,HINSTANCE   hInstance);
extern void AB_InitStirng(void);

extern BOOL AB_PickRegisterClass(void);
extern BOOL AB_CreateEditContactWnd(HWND hFrameWnd,HWND hMsgWnd,UINT uMsgCmd,CONTACT_ITEMCHAIN* pItemData,
                             PVOID pFocusPtr,BOOL bNew,DWORD dwoffset,DWORD id,int nTelID,HWND hViewWnd);
extern BOOL AB_CreateSearchPopUpWnd(HWND hFrameWnd,HWND hMsgWnd,UINT wMsgCmd,BOOL bDealArrow);
extern BOOL AB_CreateQDialWindow(HWND hWnd);
extern BOOL AB_CreateGroupsWindow(HWND hWnd);
extern BOOL AB_CreateSIMWnd(HWND hWnd);
extern BOOL AB_CreateViewContactWnd(HWND hParent,CONTACT_ITEMCHAIN* pItemData,DWORD dwoffset,DWORD id,int nTelID);

extern CONTACT_ITEMCHAIN* Item_New(DWORD dwMask);
extern int Item_Insert(CONTACT_ITEMCHAIN** ppHeader,CONTACT_ITEMCHAIN* pChainNode);
extern void Item_Delete(CONTACT_ITEMCHAIN** ppHeader,CONTACT_ITEMCHAIN* pChainNode);
extern void Item_Erase(CONTACT_ITEMCHAIN* pHeader);


extern PCONTACT_INITDATA AB_GetSIMData(void);
extern BOOL AB_SelectPhoneOrEmail(HWND hWnd,HWND hMsgWnd,UINT uMsgCmd,CONTACT_ITEMCHAIN* pItemData,
                                  AB_PICK_TYPE nType,DWORD id,BOOL bPrompt,BOOL bExtern);
extern int GetIndexByMask(DWORD dwMask);
extern int GetIndexByName(const char* itemName);

extern BOOL AB_InitData(void);
extern BOOL AB_InsertName(DWORD dwoffset,DWORD ID,int nGroup,BYTE byNamelen,char* pszName);
extern BOOL AB_InsertTel(DWORD ID,int nTelID,BYTE byTellen,char* pszTel);
extern BOOL AB_InsertEmail(DWORD ID,int nTelEmail,BYTE byEmaillen,char* pszEmail);

extern BOOL AB_ReadRecord(DWORD dwoffset,PCONTACT_ITEMCHAIN* ppItem,int * pnTelID);
extern BOOL AB_SaveRecord(DWORD* dwoffset,DWORD* id,int *pnGroup,BOOL bSaveGroup,int nTelID,CONTACT_ITEMCHAIN* pItem);
extern BOOL AB_ChangeGroup(DWORD* pdwoffset,DWORD* pid,int *pnGroup);
extern int HalfComp(PVOID* Array, int start, int end, int index, CMPFUNC cmpf);


extern BOOL ABCreateContactPickerWnd(HWND hWnd,HWND hMsgWnd, const char* pCaption, UINT uMsgCmd,AB_PICK_TYPE nType,
                                     BOOL bContact,DWORD id,BOOL bExtern, BOOL bExitString);

extern void WaitWinWithTimer (HWND hParent, BOOL bShow, PCSTR cPrompt, PCSTR szCaption, PCSTR szOk,PCSTR szCancel, UINT rMsg, UINT uTimer);
extern PNOTIFY_NODE pNotifyHeader;

extern PTABLE_ID   pTableID;
extern PTABLE_NAME pTableName;
extern PTABLE_TEL  pTableTel;
extern PTABLE_EMAIL    pTableEmail;

extern int nName,nTel,nEmail,nID;

extern PMEMORY_NAME  *pIndexID;
extern PMEMORY_NAME  *pIndexName;
extern PMEMORY_TEL   *pIndexTel;
extern PMEMORY_EMAIL *pIndexEmail;

extern CMPFUNC g_CmpFunc [];

extern int AB_Compare_Name(PVOID index1, PVOID index2);
extern int AB_Compare_Tel(PVOID index1, PVOID index2);
extern int AB_Compare_Email(PVOID index1, PVOID index2);
extern int AB_Compare_ID(PVOID index1, PVOID index2);

extern BOOL AB_HeapSort(PVOID* Array, CMPFUNC cmpf, int nCount);
extern BOOL AB_SortTop(PVOID* Array, CMPFUNC cmpf, int nStart, int nMax);

extern char* AB_GetNameString(PVOID pMemory);

extern BOOL AB_MallocMemory(unsigned int nSize);


extern BOOL AB_EditRegisterClass(void);
extern void AB_PleaseToWait(void);

extern BOOL AB_UserListSM(void);

extern NOTIFY_NODE* Notify_New(void);
extern void Notify_Insert(NOTIFY_NODE* pChainNode);
extern void Notify_Delete(NOTIFY_NODE* pChainNode);
extern void Notify_Erase(void);
extern PVOID AB_RegisterNotify(HWND hWnd,UINT uMsg,AB_NOTIFY_OBJECT nObject,AB_NOTIFY_MDU nMdu);
extern void AB_UnRegisterNotify(PVOID pNode);

extern void AB_NotifyWnd(PVOID nPos,AB_NOTIFY_MODE nMode,AB_NOTIFY_OBJECT nObject,AB_NOTIFY_MDU nMdu);

extern BOOL AB_DeleteRecord(DWORD dwoffset);
extern BOOL AB_DeleteAllFile(void);
extern void AB_EmptyMemory(void);

extern int  AB_DeleteFromIndexByID(PVOID* Array, DWORD id,int end,BOOL bPos);

extern BOOL AB_MultiPickRegisterClass(void);
extern BOOL ABCreateMultiPickerWnd(HWND hWnd,HWND hMsgWnd,UINT uMsgCmd,char* pszCaption,int nCount,DWORD nId[],int nGroup,const char* szLeft,const char* szRight);
extern BOOL AB_IsTel(DWORD dwMask);
extern BOOL AB_IsEmail(DWORD dwMask);
extern BOOL AB_ContactPickRegisterClass(void);
extern BOOL AB_CopyItem(PCONTACT_ITEMCHAIN* ppHeader,CONTACT_ITEMCHAIN* pItemChain);

extern BOOL AB_WriteQDial(PAB_QDIAL pQdial);
extern BOOL AB_ReadQDial(PAB_QDIAL pQdial);
extern int  AB_GetSubfix(DWORD id);
extern int AB_Insert2Table(DWORD dwoffset,DWORD id,int nGroup,CONTACT_ITEMCHAIN *pItem);

extern int AB_FindUseableIndex(void);
extern PPHONEBOOK AB_FindUseableSIMPB(void);
extern BOOL AB_SIMSaveToSIM(PHONEBOOK pb);

extern int AB_FindByTel(char* pszTel, int* pID, int nMax);

extern PVOID Memory_Find(PVOID pCompare,AB_FIND_TYPE nType);
extern BOOL AB_GetUseableDuplicateName(char* pszName,PCONTACT_ITEMCHAIN *ppItem);

extern void AB_LoadGroupID(void);

extern BOOL APP_EditSMS(HWND hParent,const char* PSZRECIPIENT,const char* PSZCONTENT);

extern BOOL AB_GetGroupToneByID(int id,char* pszTone,int nMaxLen);

extern BOOL AB_MultiNoOrEmailPickRegisterClass(void);

extern BOOL AB_MultiSelectPickRegisterClass(void);
extern BOOL AB_GetFullName(CONTACT_ITEMCHAIN* pItem,char* pszFullName);
extern BOOL GetVoiceMailBox(char* pszTel,int nlen);
extern BOOL ABCreateMultiNoOrEmailPickerWnd (HWND hFrameWnd,HWND hMsgWnd,UINT uMsgCmd,
                                             AB_PICK_TYPE nType, ABNAMEOREMAIL* pArray,int nCount,int nMax);

extern BOOL APP_CallPhoneNumber(const char * pPhoneNumber);

extern BOOL CreateMailEditWnd(HWND hParent, const char *PSZRECIPIENT, const char *PSZRECIPIENT2, 
                              const char *PSZTITLE, const char *PSZCONTENT, const char *PSZATTENMENT,
                              const SYSTEMTIME *PTIME, DWORD handle, int folderid);

extern BOOL AB_SaveContactRegisterClass(void);
extern BOOL AB_SelectFieldRegisterClass(void);

extern BOOL APP_SaveContact(HWND hFrameWnd,HWND hMsgWnd,UINT uMsgCmd,ABNAMEOREMAIL *pABName,BOOL bNew);

extern BOOL AB_SaveGenChain(PCONTACT_ITEMCHAIN *ppItem,int *pnTelID,DWORD dwMask,char* pszString);

extern BOOL AB_IsExistMask(CONTACT_ITEMCHAIN* pItem,DWORD dwMask);

extern BOOL AB_QDialRegisterClass(void);

extern void AB_ExitApp(HWND hFrameWnd);

extern BOOL AB_IsUserCancel(HWND hWnd,UINT uMsgCmd);

extern BOOL AB_SetQDialRegisterClass(void);

extern BOOL SMS_CreateSettingEditWnd(HWND hFrameWnd,HWND hMsgWnd,UINT uMsgCmd,char* pszCaption,char* pDefault,BOOL bText,BOOL bVMN);

extern BOOL SMS_SaveVMN(char *szPhoneNumber);

extern void AB_SetFlag(BOOL bSpec);

extern BOOL AB_GetFlag(void);

extern BOOL CallAppEntryEx(PSTR pAppName, WPARAM wparam, LPARAM lparam);

extern void AB_DeleteGroupTone(HWND hWnd,int ID);
extern void AB_ReWriteGroupTone(HWND hWnd,int ID,int nLen,char* pszTonePath);

extern void AB_MaybeDelete(void);

extern BOOL APP_EditSMSVcardVcal(HWND hParent,const char* PSZCONTENT,int nContentLen);

extern BOOL AB_AnalyseVcard(vCard_Obj* pvCardObj,CONTACT_ITEMCHAIN** ppVcardVcal,int *pnTelID);

extern BOOL AB_GenVcard(CONTACT_ITEMCHAIN* pItem,char** ppVcardData,int *nVcardLen);

extern BOOL BtSendData(HWND hAppFrameWnd,char* szAppPathName,char* szViewFileName,int iFileType);

extern HWND AB_GetFrameWnd(void);

extern int AB_GetIndexByID(PVOID* Array, DWORD id,int end);
extern void SetABInitOK(BOOL b);
extern int GetSIMState ();
extern BOOL AB_HasNumber(PVOID pItem);
extern BOOL MAIL_CreateEditInterface(HWND hFrameWindow, char *PSZRECIPIENT, char *PSZATTENMENT, char *PSZATTNAME, BOOL bAtt);

/*chinese version define*/
#define CNSEGBASE		176
#define CNOFFSETBASE	161

#define CNISEGBASE			{ "啊", "搭", "嘎", "击", "麻", "七", "挖", "西" }
#define ENGISEGBASE			{ "a", "d", "g", "j", "m", "q", "u", "x" }

#define	HROWCODESTART		175			//中文起始
#define	ASCCODEEND			128			//字母终止	

#define ASCNUMSTART			48			//数字起始
#define ASCNUMEND			57

#define ASCCHARCAPSTART		65			//大写字母起始
#define ASCCHARCAPEND		90

#define ASCCHARSTART		97			//小写字母起始
#define ASCCHAREND			122

#define NULLPYINDEX		0xffff

typedef enum
{
	CNTYPE,							//中文
	ENGTYPE,						//英文
	OTHERTYPE						//其它
}
INDEXCHARTYPE;

#define CHARTYPE_CHN		CNTYPE
#define CHARTYPE_LETTER		ENGTYPE
#define CHARTYPE_OTHER		OTHERTYPE

extern int NameCmpHazy( char *pStringKey, char *pStringSrc );
#endif//_ABGLOBAL_H
