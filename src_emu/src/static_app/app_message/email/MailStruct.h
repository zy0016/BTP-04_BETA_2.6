/***************************************************************************
*
*                      Pollex Mobile Platform
*
* Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
*                       All Rights Reserved 
*
* Module   : MailStruct.h
*
* Purpose  : 
*
\**************************************************************************/

//mail account setting
typedef struct tagMailConfig 
{
    //const value 16 is for extension
    char    GLMailBoxName[MAX_MAIL_BOXNAME_LEN + 1];
    int     GLConnect;
    char    GLMailAddr[MAX_MAIL_MAILADDR_LEN + 1];
    char    GLOutServer[MAX_MAIL_OUTGOSERVER_LEN + 1];
    int     GLSendMsg;
    int     GLCC;
    char    GLSignatrue[MAX_MAIL_SIGNATURE_LEN + 1];
    char    GLLogin[MAX_MAIL_LOGIN_LEN + 1];
    char    GLPassword[MAX_MAIL_PASSWORD_LEN + 1];
    char    GLInServer[MAX_MAIL_INCOMSERVER_LEN + 1];
    int     GLBoxType;
    int     GLAutoRetrieval;
    int     GLSecurity;
    int     GLSecureLogin;
    int     GLRetrAtt;
    int     GLRetrHead;

    char    GLConfigSaveName[MAX_MAIL_SAVE_FILENAME_LEN + 1];  //get list save file/*MLgetlist*/
    int     GLDefSelect;

    char    GLFillByte[2];    //not used for byte trim
}MailConfig;

//mail config list node
typedef struct tagMailConfigNode
{
    char    GLMailBoxName[MAX_MAIL_BOXNAME_LEN + 1];
    int     GLMailBoxID;
    char    GLMailBoxSaveName[MAX_MAIL_SAVE_FILENAME_LEN + 1];     //config save file/*mail060000*/
    char    GLMailConfigSaveName[MAX_MAIL_SAVE_FILENAME_LEN + 1];  //get list save file/*MLgetlist*/
    int     GLDefSelect;
    int     GLMailUnread;
    int     GLMailNum;

    int     GLConnect;
    char    GLMailAddr[MAX_MAIL_MAILADDR_LEN + 1];
    char    GLOutServer[MAX_MAIL_OUTGOSERVER_LEN + 1];
    int     GLSendMsg;
    int     GLCC;
    char    GLSignatrue[MAX_MAIL_SIGNATURE_LEN + 1];
    char    GLLogin[MAX_MAIL_LOGIN_LEN + 1];
    char    GLPassword[MAX_MAIL_PASSWORD_LEN + 1];
    char    GLInServer[MAX_MAIL_INCOMSERVER_LEN + 1];
    int     GLBoxType;
    int     GLAutoRetrieval;
    int     GLSecurity;
    int     GLSecureLogin;
    int     GLRetrAtt;
    int     GLRetrHead;

    int     GLConStatus;
    BOOL    GLTimer;
    BYTE    GLConType;

    struct tagMailConfigNode *pNext;
}MailConfigNode;

//mail connect waiting node
typedef struct tagMailConWaitNode
{
    char GLMailBoxName[MAX_MAIL_BOXNAME_LEN + 1];
    int  GLMailBoxID;
    BYTE GLConType;
    int  GLConnect;
    struct tagMailConWaitNode *pNext;
}MailConWaitNode;

//mail config list header
typedef struct tagMailConfigHeader
{
    int             len;
    int             DefSelInt;
    unsigned long   StartAdd;
    BOOL            bUsed[10];

    BOOL            GLNetUsed;                                  //connet is used
    char            GLMailBoxName[MAX_MAIL_BOXNAME_LEN + 1];    //used mailbox name
    int             GLConStatus;                                //connect status
    int             GLConnect;                                  //connect type
    BYTE            GLNetFlag;                                  //send,hide get or show get

    int             GLSIMState;
    int             GLInboxHandle;           //inbox handle

    struct tagMailConfigNode      *CurConnectConfig;

    struct tagMailConfigNode    *pNext;
    struct tagMailConWaitNode   *pWaitNext;
}MailConfigHeader;

//the InBox,DraftBox,OutBox and SentBox files
typedef struct tagMailFile
{
    //Send: MessageId generated from sys datetime
    //Get:  Mail Server's MessageId
    char    MsgId[MAX_MAIL_MESSAGEID_LEN + 1];
    char    From[MAX_MAIL_FROM_LEN + 1];
    char    To[MAX_MAIL_TO_LEN + 1];
    char    Cc[MAX_MAIL_TO_LEN + 1];
    //char    Date[MAX_MAIL_DATE_LEN + 1];
    SYSTEMTIME Date;
    char    Subject[MAX_MAIL_SUBJECE_LEN + 1];
    //char    LocalDate[MAX_MAIL_LOCALDATE_LEN + 1];
    SYSTEMTIME LocalDate;
    char    address[MAX_MAIL_ADDRESS_SIZE + 1];         //send:to,get:from
    char    OwnAddress[MAX_MAIL_ADDRESS_SIZE + 1];

    BOOL	ReadFlag;
    BYTE    SendStatus;
    unsigned int    AttNum;
    unsigned int    AttFileNameLen;
    unsigned int    TextSize;
    unsigned int    MailSize;

    char    GLFillBype[2];
}MailFile;

//attachment file structure for return
typedef struct tagMailAttString
{
    char    AttName[MAX_MAIL_BODY_SIZE + 1];
    int     AttSize;
    int     AttNum;
    int     AttLen;
}MailAttString;

//attachment file structure for display
typedef struct tagMailAttachmentNode
{
    char    AttShowName[MAX_MAIL_SHOW_FILENAME_LEN + 1];
    char    AttSaveName[MAX_MAIL_SAVE_FILENAME_LEN + 1];
    int     AttSize;
    int     AttType;
    char    GLFillByte[2];

    struct  tagMailAttachmentNode   *pNext;
}MailAttachmentNode;

//attachment file head for display
typedef struct tagMailAttachmentHead
{
    int Num;
    struct  tagMailAttachmentNode   *pNext;
}MailAttachmentHead;

//mail identification
//to verify whether one mail has been downloaded or not
typedef struct tagMailIdNode
{
    int     ID;
}MailIdNode;

//the node of sending mails chain
typedef struct tagMailSendNode
{
    BOOL    SendFlag;
    BYTE    SendStatus;
    char    SendMailFileName[MAX_MAIL_SAVE_FILENAME_LEN + 1];
    char    SendHandle[MAX_MAIL_MESSAGEID_LEN + 1];
    char    GLFillByte[2];
    
    struct  tagMailSendNode *pNext;
}MailSendNode;

//the header node of sending mails chain
typedef struct tagMailSendHead
{
    unsigned int    len;
    BOOL    OpLock;
    BOOL    SendingFlag;

    struct  tagMailSendNode *pNext;
}MailSendHead;

//the node of geting mails list chain
typedef struct tagMailGetListNode
{
    unsigned int     GetId;
    unsigned long    GetSize;

    char    GetFrom[MAX_MAIL_FROM_LEN + 1];
    char    GetTo[MAX_MAIL_TO_LEN + 1];
    char    GetSubject[MAX_MAIL_SUBJECE_LEN + 1];
    char    GetDate[MAX_MAIL_DATE_LEN + 1];
    char    GetUidl[MAX_MAIL_UID_LEN + 1];
    char    GetMsgId[MAX_MAIL_MSGID_LEN + 1];
    char    GetMailFileName[MAX_MAIL_SAVE_FILENAME_LEN + 1];

    BYTE    GetStatus;              //the mail status
    BYTE    GetAttFlag;             //the mail attachment flag
    BYTE    GetServerFlag;          //mail delete from server flag
    BYTE    GetReadFlag;
    
    BOOL    bSelect;

    struct  tagMailGetListNode  *pNext;
    struct  tagMailGetListNode  *pDelNext;
    struct  tagMailGetListNode  *pSelNext;
}MailGetListNode;

//the header node of geting mails list chain
typedef struct tagMailGetListHead
{
    unsigned int    len;            //all local mail header
    unsigned int    down;           //down and in server mail
    unsigned int    downlocal;      //down and not in server mail
    unsigned int    dellocal;       //delete local mail num
    unsigned int    delserver;      //delete server mail num

    struct  tagMailGetListNode  *pNext;
}MailGetListHead;

//the struct of geting mails saved
typedef struct tagMailGetSaveFile
{
    //unsigned int     GetId;
    unsigned long    GetSize;

    char    GetFrom[MAX_MAIL_FROM_LEN + 1];
    char    GetTo[MAX_MAIL_TO_LEN + 1];
    char    GetSubject[MAX_MAIL_SUBJECE_LEN + 1];
    char    GetDate[MAX_MAIL_DATE_LEN + 1];
    char    GetUidl[MAX_MAIL_UID_LEN + 1];
    char    GetMsgId[MAX_MAIL_MSGID_LEN + 1];
    char    GetMailFileName[MAX_MAIL_SAVE_FILENAME_LEN + 1];
    
    BYTE    GetStatus;              //the mail status
    BYTE    GetServerFlag;
    BYTE    GetLocalFlag;
    BYTE    GetReadFlag;
}MailGetSaveFile;

//the node of geting mails list chain
typedef struct tagMailGetServerNode
{
    unsigned int     GetState;
    unsigned int     GetId;
    unsigned long    GetSize;
    
    char    GetFrom[MAX_MAIL_FROM_LEN + 1];
    char    GetTo[MAX_MAIL_TO_LEN + 1];
    char    GetSubject[MAX_MAIL_SUBJECE_LEN + 1];
    char    GetDate[MAX_MAIL_DATE_LEN + 1];
    char    GetUidl[MAX_MAIL_UID_LEN + 1];
    char    GetMsgId[MAX_MAIL_MSGID_LEN + 1];
    
    struct  tagMailGetServerNode  *pNext;
}MailGetServerNode;

//the header node of geting mails list chain
typedef struct tagMailGetServerHead
{
    unsigned int     len;
    BOOL    DownLoadFlag;
    
    struct  tagMailGetServerNode  *pNext;
}MailGetServerHead;

//the node of list mails chain
typedef struct tagMailListNode
{
    BYTE    ListAttFlag;
    BYTE    ListReadFlag;
    BYTE    ListSendFlag;
    unsigned int   ListSize;
    char    ListMailFileName[MAX_MAIL_SAVE_FILENAME_LEN + 1];
    char    ListAddress[MAX_MAIL_ADDRESS_SIZE + 1];
    DWORD   ListMaskdate;
    char    ListSubject[MAX_MAIL_SUBJECE_LEN + 1];
    char    ListHandle[MAX_MAIL_MESSAGEID_LEN + 1];

    struct  tagMailListNode *pNext;
}MailListNode;

//the header node of list mails chain
typedef struct tagMailListHead
{
    unsigned int   TotalNum;
    unsigned int   UnReadNum;
    
    struct  tagMailListNode *pNext;
}MailListHead;

//the node of count mails list chain
typedef struct tagCountMailListNode
{
    int     GLMailType;
    int     GLMailFolder;
    int     GLMailNum;
    int     GLMailUnread;
    
    struct  tagCountMailListNode *pNext;
}CountMailListNode;

//the header node of count mails list chain
typedef struct tagCountMailListHead
{
    unsigned int   TotalNum;
    unsigned long  StartAdd;
    
    struct  tagCountMailListNode *pNext;
}CountMailListHead;

typedef struct tagNumberMail
{
    unsigned int    TotalNum;
    unsigned int    TotalUnread;
}NumberMail;

/**************************createdata*****************/
typedef struct tagMAIL_EditCreateData
{
    HWND    hFrameWnd;
    HMENU   hMenu;

    char    *pszRecipient;
    char    *pszRecipient2;
    char    *pszTitle;
    char    *pszContent;
    char    *pszAttachment;
    char    *pszOldAtt;
    char    *pszInsertTem;

    SYSTEMTIME date;

    HDC     MemoryDCIcon;
    HDC     AttDCIcon;
    HBITMAP hBitmapIcon;
    HBITMAP hAttIcon;

    HWND    hFocus;
    BOOL    bChange;
    int     bReturn;

    LONG    nConLen;                    //content length
    LONG    nReciLen;                   //recipient length
    LONG    nRec2Len;                   //recipient2 length
    LONG    nTitlLen;                   //title length
    LONG    nAttLen;                    //attachment name length
    LONG    nInsertTemLen;              //insert template length
    int     nAttNum;                    //attachment number
    int     nAttSize;                   //attachment size

    DWORD   handle;
    int     folderid;

    HMENU   hInsertMenu;
    HWND    hOldFocus;
    HWND    hOldRecipient;
    //    int     nContentLine;
    int     nHeight;
    BOOL    bSaved;
    char    szFileName[MAX_MAIL_SAVE_FILENAME_LEN + 1];
    CARETPROC OldCaretProc;
    ABNAMEOREMAIL ABAddr[20];
}MAIL_EDITCREATEDATA,*PMAIL_EDITCREATEDATA;

typedef struct tagMAIL_ViewCreateData
{
    HWND    hFrameWnd;
    HWND    hWndTextView;
    HWND    hParent;
    HMENU   hMenu;
    int     folderid;
    DWORD   handle;

    MailFile ViewFile;
    char    *pAtt;
    char    *pText;
    char    szFileName[MAX_MAIL_SAVE_FILENAME_LEN + 1];

    HDC     AttDCIcon;
    HBITMAP hAttIcon;

    HBITMAP hLeftIcon;
    HBITMAP hRightIcon;

    BOOL    bPre;
    BOOL    bNext;
}MAIL_VIEWCREATEDATA, *PMAIL_VIEWCREATEDATA;

typedef struct tagMAIL_SettingCreateData
{
    HWND    hFrameWnd;
    HMENU   hMenu;
    MailConfigHeader ConfigHead;
    HBITMAP hMailNew;
    HBITMAP hMailIcon;
    HBITMAP hMailActi;
    int     CurConfig;
    BOOL    bActiBox;
}MAIL_SETTINGCREATEDATA, *PMAIL_SETTINGCREATEDATA;

typedef struct tagMAIL_BoxCreateData
{
    int         bNew;
    int         iScrollLine;
    BOOL        bActi;
    BOOL        bFirstEnt;
    BOOL        bChange;
    BOOL        bCreate;
    char        szSaveFile[MAX_MAIL_SAVE_FILENAME_LEN + 1];
    MailConfig  BoxConfig;
    HWND        hFocus;
    HWND        hOldFocus;
    HWND        hFrameWnd;
    HWND        hParent;
    HMENU       hMenu;

    WORD        wKeyCode;
    int         nRepeats;
}MAIL_BOXCREATEDATA, *PMAIL_BOXCREATEDATA;

typedef struct tagMAIL_BoxListCreateData
{
    int         bNew;
    int         BoxID;
    MailConfig  SelConfig;
    HWND        hFrameWnd;
    HWND        hParent;
    BOOL        bCreate;
    HWND        hSelect;
    HWND        hUnSelect;
}MAIL_BOXLISTCREATEDATA, *PMAIL_BOXLISTCREATEDATA;

typedef struct tagMAIL_InBoxCreateData
{
    HWND        hFrameWnd;
    HMENU       hMenu;
    BOOL        bCreate;
    
    MailGetListHead GetListHead;
    MailGetListHead DelListHead;
    MailGetListHead SelListHead;
    MailConfig      GetConfig;
    MailGetListNode *ReadNode;
    MailGetListNode *DelNode;
    MailGetListNode *CurNode;

    char        GLMailBoxName[MAX_MAIL_BOXNAME_LEN + 1];
    char        GLMailBoxSaveName[MAX_MAIL_SAVE_FILENAME_LEN + 1];     //config save file/*mail060000*/
    char        GLMailConfigSaveName[MAX_MAIL_SAVE_FILENAME_LEN + 1];  //get list save file/*MLgetlist*/
    int         GLFileNum;

    BOOL        bConnect;           //whether to connet network(1) or disconnet(0) 
    BOOL        bReadAlreadyDel;    //open or retrieve mail already delete and notification
    BOOL        bConStatus;         //connet status
    BOOL        bChange;            //change menu
    BOOL        bRetrDigest;        //whether to retrieve digest and I will decode the message protocol returned
    int         bConfirm;

    HWND        hRead;
    HWND        hReadAtt;
    HWND        hUnread;
    HWND        hUnreadAtt;
    HWND        hUnretrRead;
    HWND        hUnretr;
    HWND        hUnretrAtt;
    HWND        hDelete;

    HMENU       hSubReply;
    HMENU       hSubMove;
    HMENU       hSubDelete;
}MAIL_INBOXCREATEDATA, *PMAIL_INBOXCREATEDATA;

typedef struct tagMAIL_OpreListCreateData
{
    HWND        hFrameWnd;
    HWND        hParent;
    int         SelType;
}MAIL_OPERLISTCREATEDATA, *PMAIL_OPERLISTCREATEDATA;

typedef struct tagMAIL_DisplayCreateData
{
    HWND        hFrameWnd;
    HWND        hParent;
    HWND        hTextView;
    HMENU       hMenu;
    BOOL        bEdit;
    BOOL        bPlaySound;
    int         AttNum;
    int         nAttSize;
    int         bReturn;

    char        *pNode;

    HBITMAP     hBmpObj[6];

    MailAttachmentHead AttHead;
}MAIL_DISPLAYCREATEDATA, *PMAIL_DISPLAYCREATEDATA;

//global value
CountMailListHead GlobalMailCountHead;
MailConfigHeader  GlobalMailConfigHead;       //config list head
MailConfig        GlobalMailConfigActi;
NumberMail        GlobalMailNumber;

MailListHead      MailListCurHead;
MailGetListHead   CurHideGetListHead;

MailGetListHead   CurHideRetrListHead;
BOOL    RetrDigest;

HWND    UniGlobalHwnd;
int     UniGlobalMsg;

HWND HwndMailDial;
HWND HwndMailSend;
HWND HwndMailGet;

int GlobalMailOpen;
int GlobalMailOpendir;

//extern function
extern void MAIL_GetLocalDateString(char * szLocalDate);
extern int MAIL_DateCmpChar(char *FirstChar, char *SecondChar);
extern void MAIL_CharToSystem(char *CharDate, SYSTEMTIME *SysDate);
extern void MAIL_SystemToChar(SYSTEMTIME *SysDate, char *CharDate);
extern void MAIL_GetNewFileName(char *szNewFilePath, int len);
extern BOOL MAIL_IsFileExist(char *szFileName);
extern void MAIL_JudgeCPUType(void);
extern int MAIL_IsFlashEnough();
extern BOOL MAIL_IsSpaceEnough(void);
extern int MAIL_GetMailTotalFile(void);
extern int MAIL_StringCut(char *pStr, unsigned int nCutLen);
extern void MAIL_UpperCase(char *pDest);
extern void MAIL_LowerCase(char *pDest);
extern int MAIL_GetFileSuffix(void);
extern void MAIL_TrimString(char * pstr);
extern int MAIL_GetMailAttachmentNum(char *AttName);
extern BOOL MAIL_GetMailAttachmentSaveName(char *AttAllName, char *AttName, int num);
extern BOOL MAIL_GetMailAttachmentShowName(char *AttAllName, char *AttName, int num);
extern int MAIL_GetAllMailAttSize(char *AttAll, int Num);
extern int MAIL_GetMailAttachmentSize(char *AttName);
extern char*  MAIL_GetFileType(char *FileName);
extern BOOL MAIL_IsAttSupported(char *szAffixName, char *tmpType, int *type);
extern void MAIL_GetFileNameFromPath(char *szFullName, char*szFileName);
extern void MAIL_GetNewAffixFileName(char *pNewName);
extern BOOL MAIL_DeleteAttachment(char *DelAtt);
extern BOOL MAIL_DeleteFile(char *DeleteFile);
extern BOOL MAIL_CopyMail(char *szDes, char *szSour);
extern BOOL MAIL_CopyFile(char *pSour, char *pDes);
extern BOOL MAIL_CopyAttachment(char *szDes, char *szSour, int Num);
extern int MAIL_AnalyseMailAddr(char * strAddr);
extern BOOL MAIL_GetMailAddress(char *strAddr, char *MailAddr, int number);
extern int MAIL_JudgeEdit();
extern BOOL MAIL_ConvertPath(char *Des, char *Sour, int type);
extern BOOL MAIL_CombineReplyRec(char **ReturnName, char *WholeName, char *IndiName);
extern void MAIL_CommaToSemicolon(char *WholeName);
extern void MAIL_InitConfigNode(MailConfigHeader *pHead, MailConfigNode *pNode, MailConfig *pFile);
extern BOOL MAIL_AddDialWaitNode(MailConfigHeader *pHead, MailConfigNode *pNode, BYTE WaitType);
extern void MAIL_DelDialWaitNode(MailConfigHeader *pHead, MailConWaitNode *pNode);
extern void MAIL_DealNoReturnConnect();
extern BOOL MAIL_DealReturnConnect();
extern void MAIL_DealNextConnect();
extern void MAIL_TruncateRec(char *chDes, char *chSour);
extern BOOL MAIL_PlayAudio(HWND hWnd, char *audioFileName, int audioType,
                           const char *audioBody, int audioSize);
extern void MAIL_StopPlay(void);
extern int GARY_open(const char *filename, int FirstArg, int SecondArg);
extern int GARY_close(int hFile);
extern DIR* GARY_opendir(const char* szPath);
extern int GARY_closedir(DIR* dir);
extern BOOL MAIL_DayIncrease(SYSTEMTIME *psystime, unsigned int day);
//extern count list
extern void MAIL_AddCountNode(CountMailListHead *pHead, int MailNum, BOOL UnRead);
extern void MAIL_DelCountNode(CountMailListHead *pHead, int MailNum, BOOL UnRead);
extern void MAIL_ModifyCountNode(CountMailListHead *pHead, int MailNum, int NewFol, BOOL Unread);
extern void MAIL_AddUnreadNode(CountMailListHead *pHead, int MailNum);
extern void MAIL_DelUnreadNode(CountMailListHead *pHead, int MailNum);

extern void MAIL_ModifyMailBoxCount(char *MailboxName, int Unread, int boxnum);
extern void MAIL_ReinstallMailBoxCount(char *MailboxName, int Unread, int boxnum);
extern void MAIL_InstallMailBoxCount(char *MailboxName);
//extern current list
extern BOOL MAIL_FileTypeMatch(char* szBoxSavePath, char* szBoxFindPath);
extern void MAIL_AddBoxFileNode(MailListHead *pHead, MailListNode *pTempNode);
extern void MAIL_DelBoxFileNode(MailListHead *pHead, int nTempNode);
extern int MAIL_GetNodeByHandle(MailListHead *pHead, MailListNode **pDelListNode, char *msghandle);
//extern function
extern BOOL GetTimeDisplay(SYSTEMTIME st, char* cTime, char* cDate);
extern BOOL IspReadInfoByID(UDB_ISPINFO * uIspInfo, int iIDnum);
extern BOOL MU_CanMoveToFolder(void);
extern DWORD BrowserNote(HWND hFrameWnd, HWND hReturnWnd, UINT uiReturnMsg, PSTR szCaption);
extern int GetFileTypeByName(LPCTSTR pFileName, BOOL bType);
extern int IspGetNum(void);
extern BOOL IspReadInfo(UDB_ISPINFO *uIspInfo, int iNum);
extern int IsFlashAvailable(int nSize);
extern int GetSIMState();
