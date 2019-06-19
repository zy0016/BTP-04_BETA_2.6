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
#ifndef _SMS_GLOBAL_H
#define _SMS_GLOBAL_H

#include <project/msgunibox.h>
#include <public/pubapp.h>
#include <project/mullang.h>
#include <sys/stat.h>
#include <sys/types.h> //dir
#include <unistd.h>
#include <dirent.h> // dir
#include <errno.h>
#include <me/me_wnd.h>
#include <project/msgpublic.h>
#include <project/imesys.h>
#include <public/PreBrowHead.h>
#include <public/PreBrow.h>
#include <public/Recipient.h>
#include <setup/setting.h>
#include <vcard/vcaglb.h>
#include <ab/PhonebookExt.h>
#include <project/log.h>
#include <project/MmsUi.h>
#include <plxdebug.h>
#include <project/vcardvcal.h>
#include <project/plxmm.h>
#include <project/prioman.h>

#include "smsconst.h"
#include "smsrs.h"
#include "smsmemory.h"

#ifndef _SMS_DEBUG_
#define _SMS_DEBUG_
#endif

/* Callback function when caret position changing */
typedef void (CALLBACK *CARETPROC)(const RECT*);
extern CARETPROC WINAPI SetCaretProc(CARETPROC pNewProc);


typedef struct tagSMS_EditCreateData
{
	HWND	hFrameWnd;
    HWND    hwndmu;
	HMENU	hMenu;
    char*   pszRecipient;
    char*   pszContent;
    HWND    hFocus;
    BOOL    bChange;
    char*   pString;
    LONG    nConLen;
    LONG    nReciLen;
    DWORD   handle;
    int     folderid;
#define MASK_MENUSTATUS_ADDRECIPIENT    0x01
#define MASK_MENUSTATUS_SEND            0x02
#define MASK_MENUSTATUS_SAVETODRAFT     0x04
#define MASK_MENUSTATUS_INSERT          0x08
    BYTE    byMenuStatus;
    int     nContentLine;
    int     nHeight;
    BOOL    bSaved;
    char    szFileName[SMS_FILENAME_MAXLEN];
    HDC     MemoryDCIcon;
    HBITMAP hBitmapIcon;
    MU_MSG_TYPE nMsgType;
    HINSTANCE hWriteSMSInstance;
    BOOL    bForward;
    char*   pszInsertString;
    int     nInsertStringLen;
    CARETPROC OldCaretProc;
}SMS_EDITCREATEDATA,*PSMS_EDITCREATEDATA;

typedef struct tagSMS_Index
{
    BYTE    byStoreType;
    union
    {
        int     index;
        DWORD   dwOffest;
        char    szFileName[SMS_FILENAME_MAXLEN];
    };
    struct tagSMS_Index* pPioneer;
    struct tagSMS_Index* pNext; 
}SMS_INDEX,*PSMS_INDEX;

typedef struct tagSMS_StoreFix
{
	int		Type;
	int		dcs;
    int     MR;
	BOOL	Report;
	BOOL	ReplyPath;
	int		PID;
	int		VPvalue;
	int		Stat;
	int		Status;
    int     Phonelen;
    int     Conlen;
    int     Udhlen;
    DWORD   dwDateTime;
	char	SCA[ME_PHONENUM_LEN];
}SMS_STOREFIX, *PSMS_STOREFIX;

typedef struct tagSMS_Store
{
    SMS_STOREFIX fix;
    char*   pszPhone;
    char*   pszContent;
    char*   pszUDH;
}SMS_STORE, *PSMS_STORE;


typedef struct tagSMS_ViewCreateData
{
	HWND hFrameWnd;
    HWND hMuWnd;
	HMENU hMenu;
    int folderid;
    DWORD handle;
    DWORD dwDateTime;
    SMS_STORE Store;
    BOOL bChangeMenu;
    HWND  hWndTextView;
    BOOL  bPre;
    BOOL  bNext;
    HBITMAP hLeftArrow;
    HBITMAP hRightArrow;
}SMS_VIEWCREATEDATA, *PSMS_VIEWCREATEDATA;

typedef struct tagSMS_Setting
{
    int     nID;
    BOOL    bReport;
    BOOL	bReplyPath;
    int     nValidity;
    int     nPID;
    int     nConnection;
	char	szSCA[ME_PHONENUM_LEN];
#define SMS_PROFILE_NAMELEN     21
	char	szProfileName[SMS_PROFILE_NAMELEN];
}SMS_SETTING, *PSMS_SETTING;


typedef struct tagSMS_SettingChain
{
    SMS_SETTING Setting;
    DWORD       dwoffset;
    struct tagSMS_SettingChain *pPioneer;
    struct tagSMS_SettingChain *pNext;
}SMS_SETTINGCHAIN,*PSMS_SETTINGCHAIN;

//
//
//#define SMS_SCANAME_LEN     30
//
//typedef struct tagSMS_SCA
//{
//    int     nID;
//    char    szSCAName[SMS_SCANAME_LEN+1];
//	char	szSCA[ME_PHONENUM_LEN+1];
//}SMS_SCA, *PSMS_SCA;



//typedef struct tagContact_ItemChain
//{
//    char*  pszData;
//    DWORD  dwMask;
//    int    nMaxLen;
//    BOOL   bChange;
//    int    nID;
//    struct tagContact_ItemChain *pPioneer;
//    struct tagContact_ItemChain *pNext;
//}CONTACT_ITEMCHAIN,*PCONTACT_ITEMCHAIN;

//extern int GetIndexByMask(DWORD dwMask);
//extern CONTACT_ITEMCHAIN* Item_New(DWORD dwMask);
//extern int Item_Insert(CONTACT_ITEMCHAIN** ppHeader,CONTACT_ITEMCHAIN* pChainNode);
//extern void Item_Delete(CONTACT_ITEMCHAIN** ppHeader,CONTACT_ITEMCHAIN* pChainNode);
//extern void Item_Erase(CONTACT_ITEMCHAIN* pHeader);
//
//typedef struct tagContact_Item
//{ 
//    DWORD   dwMask;
//    BOOL    bOnly;
//    int     nMaxLen;
//    BOOL    bNum;
//    const char*   pszCpation;
//}CONTACT_ITEM,*PCONTACT_ITEM;
//
//#define AB_EDITCONTACT_CTRNUM           22   
//
//extern CONTACT_ITEM Contact_Item[AB_EDITCONTACT_CTRNUM+2];  

typedef struct tagSMS_SIMProfile
{ 
    int     nValidity;
    int     PID;
    char    szSCA[ME_PHONENUM_LEN+1];
}SMS_SIMPROFILE,*PSMS_SIMPROFILE;

typedef struct tagSMS_SIMProfileData
{ 
    int nRecordNum;
    int nRecordLen;
    int nCurProfile;
    PSMS_SIMPROFILE pSIMProfile;
}SMS_SIMPROFILEDATA,*PSMS_SIMPROFILEDATA;

typedef struct tagCB_Info
{ 
    int     index;
    short   ID;
}CB_INFO,*PCB_INFO;

typedef struct tagSMS_Init
{
    int nSIMCount;
    int nSIMMaxCount;
    int nMECount;
    int nMEMaxCount;
    SMS_Service nService;
    SMS_SIMPROFILEDATA SIMProfileData;
    SMS_INFO* pMESMSInfo;
    int nSIMCBFileLen;
    char* pszSIMCBFile;
}SMS_INITDATA,*PSMS_INITDATA;

typedef struct tagSMS_InstantChain
{
    SMS_INFO smsinfo;
    struct tagSMS_InstantChain *pPioneer;
    struct tagSMS_InstantChain *pNext;
}SMS_INSTANTCHAIN,*PSMS_INSTANTCHAIN;

typedef struct tagCB_Language
{ 
    DWORD         mask;
    const char*   pszLanguage;
}CB_LANGUANGE,*PCB_LANGUANGE;


typedef struct tagMEStore_Info
{ 
//    int     index;
    int     nfolderid;
}MESTORE_INFO,*PMESTORE_INFO;


typedef struct tagReport_Node
{ 
    int     MR;
    DWORD   dwOffset;
    DWORD   handle;
    struct tagReport_Node *pPioneer;
    struct tagReport_Node *pNext;
}REPORT_NODE,*PREPORT_NODE;


extern void SendNode_Init(void);
extern BOOL SMS_IsInitOK(void);

extern BOOL SMS_SaveFile(int folderid,SMS_STORE *psmsstore,char* szFileName,MU_MSG_TYPE nMsgType);
extern BOOL SMS_SaveRecord(char* szFileName,SMS_STORE *psmsstore,DWORD* pdwOffset);
extern BOOL SMS_CreateSIMWnd(HWND hParent);
extern BOOL CB_CreateWnd(HWND hParent);
extern void SMS_ChangeCount(int nFolderID,SMS_COUNT_TYPE nType,int nChangeNum);
extern BOOL SMS_Send(char* pszFileName,DWORD handle);
extern void smsapi_Delete(DWORD handle);
extern BOOL SMS_CreateDetailWnd(HWND hParent,SMS_STORE* psms_store);
extern BOOL SMS_ReadSetting(PSMS_SETTINGCHAIN *ppHeader);
extern BOOL SMS_WriteSetting(SMS_SETTING *pSetting,DWORD* pdwoffset);
//extern BOOL SMS_WriteSCA(SMS_SCA *pSCA,int nSize);
extern HWND SMS_CreateSendControlWnd(void);
extern BOOL SMS_CounterInit(void);
extern BOOL SIM_Delete(int index,int MemType);
extern DWORD smsapi_NewRecord(DWORD dwOffset,SMS_STORE *psmsstore);
extern void smsapi_Modify(char* pszFileName,DWORD handle,int folderid,SMS_STORE *psmsstore);
extern BOOL SMS_NewFileName(int folderid, char* pszFileName);
extern BOOL SMS_ReadDataFromFile(char* szFileName,SMS_STORE *psms_store,BOOL bOrg);
extern BOOL SMS_CodingContent(SMS_STORE *psmsstore,char** ppszCon,int* pnlen,MU_MSG_TYPE nMsgType);
extern BOOL MYBUTTON_RegisterClass(void);
extern int  DeleteWithOffset(char* pszFileName,DWORD dwOffest);
extern int  App_SMSReceive(int nType, void *pInfo);
extern HWND SMS_CreateControlSIMWnd(void);
extern DWORD String2DWORD(char* pszTime);
extern DWORD smsapi_New(char* szFileName,SMS_STORE *psmsstore,DWORD* phandle,int* pfolderid);
extern BOOL SMS_Acknowledge(void);
extern BOOL SMS_RecombineTimeout(int nTimerId);
extern BOOL SMS_CreateEditWnd(HWND hFrameWnd,HWND hParent,const char* PSZRECIPIENT,const char* PSZCONTENT,int nConlen,
                              DWORD handle,int folderid,MU_MSG_TYPE nMsgType,HINSTANCE hWriteSMSInstance,BOOL bForward);
extern BOOL SMS_CreateViewWnd(HWND hParent,SMS_STORE* psms_store,DWORD handle,int folderid,BOOL bPre,BOOL bNext);
extern LRESULT SMSViewWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
extern void DealReport(HWND hWnd,WPARAM wParam,LPARAM lParam);
extern BOOL SMS_OnRecvPort(WPARAM para);
extern BOOL SMS_VcardVcalPortInit(HWND hMsgWnd,UINT message);
extern BOOL SMS_ParseContent(int dcs , char* pszContent, int nContenLen,SMS_STORE *psmsstore);
//extern BOOL SMS_AppendSCA(SMS_SCA *pSCA);
extern void SMS_NotifyIdle(void);
extern void SMS_SetSIMCounter(int nCount);
extern int  SMS_GetSIMCounter(void);
extern void smsapi_DeleteByFileName(char* szFileName);

extern BOOL SMS_IsIDUsed(SMS_SETTINGCHAIN* pHeader,int ID);
extern int  SMS_FindUseableID(SMS_SETTINGCHAIN* pHeader);
extern BOOL SMS_IsNameUsed(SMS_SETTINGCHAIN* pHeader,char* pszProfileName);
extern BOOL SMS_FindUseableName(SMS_SETTINGCHAIN* pHeader,char* pszProfileName);

extern BOOL IsFlashEnough(void);
extern int  GetSIMState(void);
extern BOOL CanPaste(void);
extern BOOL CanInsertTemplate(void);
extern BOOL CanMoveToFolder(void);
extern void SMS_PleaseToWait(void);

extern BOOL APP_CallPhoneNumber(const char * pPhoneNumber);
extern BOOL GetTimeDisplay (SYSTEMTIME st, char* cTime, char* cDate);
extern void DealMultiPageSMSTimeOut(int Timerid);
extern BOOL SMS_IsVcradOrVcal(SMS_STORE *psmsstore);

extern void MaybeDelete(void);
extern BOOL SMS_CreateViewVcardOrVcalWnd(HWND hFrameWnd,HWND hwndmu,SMS_STORE *psmsstore,int nMsgType,
                                         DWORD handle,int nFolderID,BOOL bPre,BOOL bNext);
extern BOOL SMS_AnalyseVcard(vCard_Obj* pvCardObj,PVOID* ppVcardVcal);
extern BOOL SMS_CopyStore(SMS_STORE *pDec,SMS_STORE* pSrc);

extern BOOL CreateSpinList(HWND hFrameWnd,HWND hWndFocus);

extern void SMS_SetSIMVMN(char* szVMN,int nline);
extern void SMS_GetSIMVMN(char* szVMN,int nline);

extern BOOL GetVoiceMailBox(char *PhoneNumber, int iPhoneNumberLen);
extern BOOL SMS_CreateSetting(HWND hFrameWnd,HWND hMsgWnd,UINT uMsgCmd,BOOL bExtern);
extern BOOL SMS_CreateSettingListWnd(HWND hFrameWnd);
//extern BOOL AB_SaveRecord(DWORD* dwoffset,DWORD* id,int *pnGroup,BOOL bSaveGroup,int nTelID,CONTACT_ITEMCHAIN* pItem);

extern BOOL SMS_GetActivateSetting(SMS_SETTING *pSetting);
extern BOOL SMS_SaveActivateID(int nID);
extern BOOL SMS_GetActivateID(int* pnID);

extern SMS_SETTINGCHAIN* Setting_New(void);
extern int Setting_Insert(SMS_SETTINGCHAIN** ppHeader,SMS_SETTINGCHAIN* pChainNode);
extern void Setting_Delete(SMS_SETTINGCHAIN** ppHeader,SMS_SETTINGCHAIN* pChainNode);
extern void Setting_Erase(SMS_SETTINGCHAIN* pHeader);
extern BOOL APP_EditSMS(HWND hParent,const char* PSZRECIPIENT,const char* PSZCONTENT);
//extern BOOL AB_GenVcard(CONTACT_ITEMCHAIN* pItem,char** ppVcardData,int *nVcardLen);
extern BOOL SMS_CreateSettingWnd(HWND hFrameWnd,HWND hMsgWnd,UINT uMsgCmd,SMS_SETTINGCHAIN* pSetting,BOOL bFrist,BOOL bNew);

extern BOOL SMS_CreateReportWnd(HWND hParent,SMS_STORE* psms_store);

extern BOOL APP_CallPhoneNumber(const char * pPhoneNumber);

extern BOOL CreateMailEditWnd(HWND hParent, const char *PSZRECIPIENT, const char *PSZRECIPIENT2, 
        const char *PSZTITLE, const char *PSZCONTENT, const char *PSZATTENMENT, 
        const SYSTEMTIME *PTIME, DWORD handle, int folderid);
extern void smsapi_ModifyByFileName(char* pszFileName,int nType);

extern BOOL SMS_EditRegisterClass(void);
extern BOOL SMS_DetailRegisterClass(void);
extern BOOL SMS_SettingListRegisterClass(void);

extern BOOL SMS_ReadStatusFromFile(char* pszFileName,SMS_STOREFIX *psms_storefix);
extern BOOL SMS_ModifyFileStatus(char* pszFileName,SMS_STOREFIX *psms_storefix);
extern BOOL SMS_Suspend(char* pszFileName);

extern BOOL MU_FolderSelectionEx(HWND hParent, HWND hWnd, const char* pCaption, UINT wMsgCmd, int nFolderType);
extern int WBM_ADD_FROMURL(char *szinput, HWND hWnd);

typedef void* HBROWSER;
extern HBROWSER App_WapRequestUrl(char *szurl);

//extern BOOL APP_PreviewPhrase (HWND hFrame, HWND hWnd, UINT returnmsg);

extern int SMS_SendPortSMS(HWND hSendCtlWnd, UINT uMsgCmd,int nMsgType, SMS_STORE * psmsstore);

extern int GetCurrentAltLine(void);

extern void SMS_InitData(void);
extern PSMS_INITDATA SMS_GetInitData(void);

extern void SMS_SetService(SMS_Service nService);

extern BOOL SetCBID(HWND hWnd,unsigned char* File,unsigned int nFileSize, int Offset,CB_OPTION Option);
extern BOOL SMS_ParseContentEx(int dcs , char* pszSrc, int nSrcLen,char **ppszDec, int *pDeclen);
extern PSMS_INSTANTCHAIN GetInstant(void);
extern void Instant_Delete(SMS_INSTANTCHAIN* pChainNode);

extern BOOL APP_EntryEditSMS(HWND hParent,const char* PSZRECIPIENT,HINSTANCE hWriteSMSInstance);
extern BOOL SMS_UpdateView(HWND hWnd);
extern BOOL SMS_CreateClass0Wnd(HWND hFrameWnd,HINSTANCE hInstance);
extern void smsapi_NewME(int index,SMS_INFO *psmsinfo);
extern HWND SMS_GetSIMCtrlWnd(void);
extern BOOL CallAppEntryEx (PSTR pAppName, WPARAM wparam, LPARAM lparam);

extern BOOL CALE_DecodeVcalToDisplay(vCal_Obj *pObj, VCARDVCAL **ppChain);
extern BOOL CALE_SaveFromSms(vCal_Obj *pObj);

extern BOOL SMS_UpdateReport(SMS_INFO* pSMSInfo);
extern REPORT_NODE*  Report_FindNode(int MR);
extern BOOL SMS_NewReport(SMS_STORE* psmsstore,int MR);
extern void Report_InsertHandle(int MR,DWORD handle);
extern void Report_Delete(REPORT_NODE* pChainNode);

extern BOOL CB_Write(int *pnCB);
extern BOOL CB_Read(int *pnCB);
extern BOOL SMS_IsFull(void);
extern void SMS_SetFull(BOOL bFull);
extern BOOL SMS_Opening(void);
extern BOOL SMS_ReadOneMEStore(MESTORE_INFO* pMEStoreInfo,int index);
extern BOOL SMS_WriteOneMEStore(MESTORE_INFO* pMEStoreInfo,int index);
extern void SMS_TransferData(void);
extern BOOL CB_Set(CELL_BROADCAST *pCB_Setting);
extern BOOL SMS_AutoSend(void);
extern void SMS_SetMode(BOOL bFTAMode);
extern BOOL SMS_GetMode(void);
extern BOOL CB_ReadSetting(CELL_BROADCAST *pCB_Setting,int* pnTopicDetection);
extern BOOL LOG_UpdateSMSStatus(unsigned long SmsID, SMSSTATUS newstatus);

extern BOOL CB_WriteMessage(CBS_INFO *pCBInfo);
extern int  SMS_GetCBCounter(void);
extern void SMS_SetCBCounter(int nNewCBCounter);
extern void SMS_InitCBCounter(void);
extern BOOL CB_Refresh(CBS_INFO *pCBInfo);
extern BOOL SetMTMemory(void);
extern void SMS_InitMode(void);

extern BOOL SMS_FillTrafficLog(char* pszNum,DRTTYPE direction,unsigned long SmsID,int nFrag,SMSSTATUS status);
extern void WaitWinWithTimer(HWND hParent, BOOL bShow, PCSTR cPrompt, PCSTR szCaption,
                             PCSTR szOk,PCSTR szCancel, UINT rMsg, UINT uTimer);

extern LONG APP_GetSMSSize(void);
extern BOOL SMS_ReadMEStoreInfo(MESTORE_INFO* pMEStoreInfo,int MEMaxCount);
extern BOOL Get_ShowCellInfo(void);

extern void SMS_NewSIMRecord(PSMS_INFO pInfo);
extern BOOL SMS_VViewRegisterClass(void);
extern BOOL SMS_ViewRegisterClass(void);
#endif // _SMS_GLOBAL_H
