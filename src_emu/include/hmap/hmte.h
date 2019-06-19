/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : PMAP
 *
 * Purpose  : 
 *
\**************************************************************************/

#ifndef _HMAP_TE_H_
#define _HMAP_TE_H_

/* TE error code */
#define TE_E_NOTREADY               1024
#define TE_E_NOTSUPPORT             1025
#define TE_E_NOTALLOW               1026
#define TE_E_INVALIDARG             1027
#define TE_E_BUFLIMIT               1028
#define TE_E_NOMODEM                1029
#define TE_E_NOSIM                  1030
#define TE_E_UNKNOWN                1204

/* Async Event definition */
#define TE_ASYNC_DONE               0
#define TE_ASYNC_FAIL               1
#define TE_ASYNC_CANCEL             2
#define TE_ASYNC_TIMEOUT            3
#define TE_ASYNC_NOTIFY             4
#define TE_ASYNC_REQUEST            5

/* Async Result definition */
#define TE_RESULT_BUSY              1
#define TE_RESULT_NODIALTONE        2
#define TE_RESULT_NOCARRIER         3
#define TE_RESULT_CONNECT           4

/* Notify Event definition */
#define TE_NOTIFY_MTCALL            0x00000001
#define TE_NOTIFY_MOCALL            0x00000002
#define TE_NOTIFY_CALLEND           0x00000004
#define TE_NOTIFY_MTSMS             0x00000008
#define TE_NOTIFY_SMSFULL           0x00000010
#define TE_NOTIFY_BATTERY           0x00000020
#define TE_NOTIFY_GSM               0x00000040
#define TE_NOTIFY_GPRS              0x00000080
#define TE_NOTIFY_ALARM             0x00000100
#define TE_NOTIFY_SYSSTART          0x00000200
#define TE_NOTIFY_STK               0x00000400
#define TE_NOTIFY_DATAIN            0x00010000
#define TE_NOTIFY_DATAOUT           0x00020000
#define TE_NOTIFY_UNLINK            0x00040000
#define TE_NOTIFY_HANGUP            0x00080000

/* SYSSTART notify lParam definition */
#define SYSN_CONNECTFAIL            0x00000001
#define SYSN_MUXSTARTFAIL           0x00000002
#define SYSN_SIMNOTEXIST            0x00000003
#define SYSN_NEEDPIN                0x00000004
#define SYSN_SIMINITFAIL            0x00000005
#define SYSN_MODEMREADY             0x00000007
#define SYSN_STARTREADY             0x00000006

/* STK notify lParam definition */
#define STK_PACCMD                  0x00000001
#define STK_TMNCMD                  0x00000002
#define STK_NOTIFY                  0x00000003
#define SIM_MAINMENU                0x00000004
#define SIM_RESET                   0x00000005
#define STK_NOTIFY_CALLTERM         0x00000006

/* notify mask operator */
enum TE_MASK_OP 
{
    TE_MASK_REPLACE = 1,
    TE_MASK_COMBINE,
    TE_MASK_ERASE
};

// API para definition 
enum TE_SMS_STATE 
{
    TE_SMS_UNREAD = 1,
    TE_SMS_READED,
    TE_SMS_UNSEND,
    TE_SMS_SENDED,
    TE_SMS_ALL
};

enum TE_PBOOK_MEM 
{
    TE_PBMEM_SM = 1,    // SIM card phonebook memory
    TE_PBMEM_ME,        // Mobile equipment phonebook memory
    TE_PBMEM_MT,        // Sum of SM and ME
    TE_PBMEM_LD,        // Last dialed number
    TE_PBMEM_MC,        // Missed call number
    TE_PBMEM_RC,        // Last received call number
    TE_PBMEM_FD,        // Fixed dial number
    TE_PBMEM_ON,        // Own number(MSISDNs)
    TE_PBMEM_EN,        // Emergency calls number
    TE_PBMEM_SN         // Supplementary number
};

enum TE_SMS_MEM
{
    TE_SMSMEM_SM = 1,
    TE_SMSMEM_ME,
    TE_SMSMEM_MT
};

enum TE_SMS_FORMAT
{
    TE_SMSFMT_TEXT = 1,
    TE_SMSFMT_PDU
};

enum TE_SMS_IND_MODE
{
    TE_SMSIND_DEFAULT = 1,
    TE_SMSIND_NONE,
    TE_SMSIND_STORAGE,
    TE_SMSIND_DIRECT
};

enum TE_LOCK_FACILITY
{
    TE_FAC_SC = 1,
    TE_FAC_P2,
    TE_FAC_PS,
    TE_FAC_PF,
    TE_FAC_PN,
    TE_FAC_PU,
    TE_FAC_PP,
    TE_FAC_PC
};

enum TE_PASS_TYPE
{
    TE_SIMPIN1 = 1,
    TE_SIMPUK1,
    TE_SIMPIN2,
    TE_SIMPUK2,
    TE_PHSIMPIN,
    TE_PHSIMPUK,
    TE_PHFSIMPIN,
    TE_PHFSIMPUK,
    TE_PHNETPIN,
    TE_PHNETPUK,
    TE_PHNSPIN,
    TE_PHNSPUK,
    TE_PHSPPIN,
    TE_PHSPPUK,
    TE_PHCPIN,
    TE_PHCPUK
};

enum STK_ICON
{
    SIMICON_COLOR=1,
    SIMICON_BW
};

typedef enum TE_CHAN_CLASS
{
    TE_CHAN_COMMAND = 1,
    TE_CHAN_VOICE,
    TE_CHAN_PBOOK,
    TE_CHAN_SMS,
    TE_CHAN_STK,
    TE_CHAN_ATCMD,
    TE_CHAN_DATA1,              // GSM Data Channel
    TE_CHAN_DATA2,              // GPRS1(cmnet)
    TE_CHAN_DATA3,              // GPRS2(cmwap)
    TE_CHAN_USSD
} TE_CHNL;

/* STK Proactive Command */
enum 
{
    TE_STKCMD_REFRESH = 1,
    TE_STKCMD_SETUPEVENTLIST,
    TE_STKCMD_SETUPCALL,
    TE_STKCMD_SENDSS,
    TE_STKCMD_SENDUSSD,
    TE_STKCMD_SENDSMS,
    TE_STKCMD_SENDDTMF,
    TE_STKCMD_LAUNCHBROWSER,
    TE_STKCMD_PLAYTONE,
    TE_STKCMD_DISPLAYTEXT,
    TE_STKCMD_GETINKEY,
    TE_STKCMD_GETINPUT,
    TE_STKCMD_SELECTITEM,
    TE_STKCMD_SETUPMENU,
    TE_STKCMD_SETUPIDLETEXT
};

/* STK Response Status */
enum stk_status 
{
    TE_STKRSPS_SUCCESS = 1,
    TE_STKRSPS_TERMINATE,
    TE_STKRSPS_DENY,
    TE_STKRSPS_BACKWARD,
    TE_STKRSPS_NORESPONSE,
    TE_STKRSPS_CANOTHANDLE,
    TE_STKRSPS_HELP
};

typedef enum 
{
    OPMODE_REGIST = 1,      // register operation
    OPMODE_ACTIVATE,    // enable operation
    OPMODE_DEACTIVE,    // disable operation
    OPMODE_ERASE,       // erase operation
    OPMODE_INTERROG     
} SSOPMODE;

typedef enum 
{
    CBFAC_AO = 1,   // bar all outgoing calls
    CBFAC_OI,   // bar outgoing international calls
    CBFAC_OX,   // bar outgoing international calls except to home country
    CBFAC_AI,   // bar all incoming calls
    CBFAC_IR,   // bar incoming calls when roam outside the home country
    CBFAC_AB    // all bar service
} CBARFAC;

typedef enum 
{
    SSCLASS_VOICE = 1,
    SSCLASS_FAX,
    SSCLASS_DATA,
    SSCLASS_DEFAULT
} SSCLASS;

typedef struct call_bar 
{
    CBARFAC         fac;
    SSCLASS         bclass;
    SSOPMODE        mode;
    unsigned char   passlen;
    char            password[1];
}CALL_BAR;

typedef enum 
{
    CF_ALL,         // forward all call
    CF_ALLVOICE,    // forward all voice call
    CF_ALLFAX,      // forward all fax call
    CF_ALLDATA,     // forward all data call
    CF_NOREPLY,     // forward when no reply
    CF_BUSY,        // forward when busy
    CF_NOTREACHABLE,     // forward when can not reach destination
    CF_NOTAVAILABLE,    //forward all conditional reason
} CFREASON;

typedef struct call_forward 
{
    CFREASON        reason;
    SSOPMODE        mode;
    unsigned char   time;
    unsigned char   numlen;
    char            number[1];
}CALL_FORWARD;

typedef struct call_wait 
{
    SSOPMODE          mode;
    SSCLASS         wclass;
} CALL_WAIT;

#define TE_PHONENUM_LEN     44

#if 0
typedef enum 
{
    DIAL_INVALID = 0,
    DIAL_IMEI,
    DIAL_SS,
    DIAL_SIM,
    DIAL_USSD,
    DIAL_CALL,
    DIAL_ADN
} DIALTYPE;

typedef enum 
{
    SIM_CHANGEPIN1,
    SIM_CHANGEPIN2,
    SIM_UNBLKPIN1,
    SIM_UNBLKPIN2
} SIMOP;

typedef enum 
{
    SSCLIP,
    SSCLIR,
    SSCOLP,
    SSCOLR,
    SSCFU,
    SSCFB,
    SSCFNRY,
    SSCFNRC,
    SSCFALL,
    SSCFALLCOND,
    SSCW,
    SSBAOC,
    SSBOIC,
    SSBOIC_EXHC,
    SSBIC,
    SSBIC_ROAM,
    SSBARALL,
    SSBARALLOUT,
    SSBARALLIN,
    SSCALL,
    SSREGPASS
} SSCODE;

typedef struct dial_input 
{
    DIALTYPE type;
    union {
        struct {
            int     unused;
        } IMEI;
        struct {
            SIMOP   op;
            int     unused;
            int     paraoff[3];
            char    data[1];
        } SIM;
        struct {
            SSCODE      scode;
            SSOPMODE    op;
            int         paraoff[3];
            char        data[1];
        } SS;
        struct {
            char    data[1];
        } USSD;
        struct {
            int     emergency;
            char    number[1];
        } CALL;
        struct {
            int     index;
        } ADN;
    }u;
} DIAL_INPUT;

typedef struct tagCFInfo {
    SSCODE      reason;
    int         active;
    SSCLASS     bearer;
    int         time;
    char        number[TE_PHONENUM_LEN];
}CFINFO, *PCFINFO;

typedef struct tagCBInfo {
    SSCODE      fac;
    int         active;
    SSCLASS     bearer;
} CBINFO, *PCBINFO;

typedef struct tagCWInfo {
    int         active;
    SSCLASS     bearer; 
} CWINFO, *PCWINFO;

typedef enum {
    CLIPNONE,
    CLIPALLOW,
    CLIPUNKNOWN
} CLIPSTATUS;

typedef struct tagCLIPInfo {
    CLIPSTATUS status;
}CLIPINFO, *PCLIPINFO;

typedef enum {
    CLIRNONE,
    CLIRPERMANENT,
    CLIRUNKNOWN,
    CLIRTEMPRES,
    CLIRTEMPALLOW
} CLIRSTATUS;

typedef struct tagCLIRInfo {
    CLIRSTATUS status;
}CLIRINFO, *PCLIRINFO;

typedef struct tagCOLPInfo {
    int      active;
} COLPINFO, *PCOLPINFO;

typedef struct tagCOLRInfo {
    int      active;
} COLRINFO, *PCOLRINFO;

#endif

typedef struct call_info 
{
    int     callid;             //call index
    int     status;             //call status:RINGING/WAITING/HOLD/ACTIVE etc.
    int     mpty;               //whether this call in an mpty
    int     mode;               //call type:VOICE/DATA/FAX etc.
    int     dir;                //call direction:MT/MO
    char    name[TE_PHONENUM_LEN];
} CALL_INFO;

typedef enum call_status 
{
    TE_CALLSTAT_ACTIVE,
    TE_CALLSTAT_HELD,
    TE_CALLSTAT_DIALING,
    TE_CALLSTAT_ALERTING,
    TE_CALLSTAT_INCOMING,
    TE_CALLSTAT_WAITING
};

typedef enum call_type 
{
    TE_CALLMODE_VOICE,
    TE_CALLMODE_DATA,
    TE_CALLMODE_FAX
};

typedef enum call_dir 
{
    TE_CALLDIR_ORIGINATED,
    TE_CALLDIR_TERMINATED
};

typedef struct tagSTK_Callinfo 
{
    unsigned short confimationText_offset;
    unsigned short number_offset;
    unsigned short setupText_offset;
    unsigned char confirmationIconQualifier;
    unsigned char confirmationIconID;
    unsigned char setupIconID;
    unsigned char setupIconQualifier;
    char data[1];
}STK_CALLINFO, *PSTK_CALLINFO;

typedef struct tagSTK_PromptInfo 
{
    unsigned char iconQualifier;
    unsigned char iconID;
    char textInfo[1];
}STK_PROMPTINFO, *PSTK_PROMPTINFO;

typedef struct tagSTK_Browserinfo 
{
  unsigned char mode;
  unsigned char cmdDetail;
  unsigned char  confirmationQualifier;
  unsigned char  confirmationIconID;
  unsigned char  browserID;
  unsigned short confirmationText_offset;
  unsigned short URL_offset;
  unsigned short bearerList_offset;
  unsigned short FileRefList_offset;
  unsigned short gatewayID_offset;
  char  data[1];
}STK_BROWSERINFO, *PSTK_BROWSERINFO;

typedef struct tagSTK_Playtoneinfo 
{
    unsigned char toneType;
    unsigned char durationUnit;
    unsigned char duration;
    unsigned char iconQualifier;
    unsigned char iconID;
    char  infoText[1];
}STK_PLAYTONEINFO, *PSTK_PLAYTONEINFO;

typedef struct tagSTK_TextInfo 
{
    unsigned char mode;
    unsigned char cmdDetail;
    unsigned char responseMode;
    unsigned char iconQualifier;
    unsigned char iconID;
    char text[1];
}STK_TEXTINFO, *PSTK_TEXTINFO;

typedef struct tagSTK_Inkeyinfo 
{
    unsigned char mode;
    unsigned char cmdDetail;
    unsigned char iconQualifier;
    unsigned char iconID;
    char promptText[1];
}STK_INKEYINFO, *PSTK_INKEYINFO;

typedef struct tagSTK_Inputinfo 
{
    unsigned char mode;
    unsigned char minLen;
    unsigned char maxLen;
    unsigned char iconQualifier;
    unsigned char iconID;
    unsigned short promptText_offset;
    unsigned short defaultText_offset;
    char data[1];
}STK_INPUTINFO, *PSTK_INPUTINFO;

typedef struct tagSTK_MenuInfo 
{
    unsigned char mode;
    unsigned char items;
    unsigned char defaultItem;
    unsigned char itemIcon;
    unsigned char itemIconsQualifier;
    unsigned char titleIconQualifier;
    unsigned char titleIconID;
    char  titleText[1];
}STK_MENUINFO, *PSTK_MENUINFO;

typedef struct tagSTK_MenuItemInfo 
{
    unsigned char itemID;
    unsigned char nextAction;
    unsigned char iconID;
    char itemText[1];
}STK_MENUITEMINFO, *PSTK_MENUITEMINFO;

typedef struct tagSTK_IdleTextInfo 
{
  unsigned char iconQualifier;
  unsigned char iconID;
  char  text[1];
}STK_IDLETEXTINFO, *PSTK_IDLETEXTINFO;

typedef struct tagSTK_Callterm 
{
    unsigned char termQualifier;
    char  termCauseText[1];
}STK_CALLTERM, *PSTK_CALLTERM;

typedef struct tagSIM_Icon 
{
    unsigned char icon_id;
    unsigned char icon_count;
    unsigned char clut_entries;
    unsigned char bits_perpoint;
    unsigned char icon_width;
    unsigned char icon_height;
    unsigned char icon_datalen;
    unsigned short  color_table_offset;
    unsigned short  icon_data_offset;
    char    data[1];
}SIM_ICON, *PSIM_ICON;

typedef struct async_wait 
{
    void *  hWnd;
    unsigned msg;
    char    done;
    char    wait;
    unsigned long event;
    unsigned long result;
} ASYNC_WAIT;

#ifdef __cplusplus
extern "C" {
#endif

typedef int (*TE_AsyncHandle)(void * pClient, unsigned event, void * pdata, int datalen); 

void * TE_Connect   (TE_AsyncHandle asyncHandle, void * pClient);
void TE_Unlink      (void * pConnect);
void * TE_Open      (int mode);
int TE_Close        (void * handle, int mode);
int TE_Init         (void);
int TE_SetNotify    (void * handle, int op, unsigned long mask, const void * hwnd, int msg);
int TE_WaitAsync    (void * handle, ASYNC_WAIT * wait, long timeout);
int TE_CancelAsync  (void * handle, ASYNC_WAIT * wait);

int TE_SendATCommand(void * handle, const char * cmd);
int TE_ReadResponse (void * handle, ASYNC_WAIT * wait, char * buf, int buflen);

int TE_Reinitialize     (void * handle, ASYNC_WAIT * wait, int mode);
int TE_GetMode          (void * handle, ASYNC_WAIT * wait, const char * name, void * value, int datalen);
int TE_SetMode          (void * handle, ASYNC_WAIT * wait, const char * name, void * value, int datalen);
int TE_SearchPLMN       (void * handle, ASYNC_WAIT * wait, int mode);
int TE_SelectPLMN       (void * handle, ASYNC_WAIT* wait, int mode, char* plmn_id);
int TE_FacilityLock     (void * handle, ASYNC_WAIT * wait, int facility, char * password);
int TE_FacilityUnlock   (void * handle, ASYNC_WAIT * wait, int facility, char * password);
int TE_PowerOff         (void * handle, int mode );
int TE_GetWaitingPasswd (void * handle, ASYNC_WAIT * wait);
int TE_GetPINCounter    (void * handle, ASYNC_WAIT * wait, int mode);
int TE_SendPIN          (void * handle, ASYNC_WAIT * wait, int mode, char * pPIN);
int TE_SendPUK          (void * handle, ASYNC_WAIT * wait, int mode, char * pPUK, char* pNewPIN);
int TE_ChangePass       (void * handle, ASYNC_WAIT * wait, int facility, char * old_pass, char * new_pass);
int TE_SetSupService    (void * handle, ASYNC_WAIT * wait, const char * name, void * value, int valuelen);
int TE_GetSupService    (void * handle, ASYNC_WAIT * wait, const char * name, void * value, int valuelen, void * buf, int buflen);

int TE_GetCallNumber    (void * handle, int * pbuf, int buflen);
int TE_GetCallInfo      (void * handle, int call_id, CALL_INFO * pInfo);
int TE_StartRing        (void * handle, int type);
int TE_StopRing         (void * handle);
int TE_GetCallStatus    (void * handle, int call_id);
int TE_Dial             (void * handle, ASYNC_WAIT * wait, int type, const char * number, unsigned long params);
int TE_Answer           (void * handle, ASYNC_WAIT * wait, int call_id);
int TE_HangUp           (void * handle, ASYNC_WAIT * wait, int call_id);
int TE_HangupMPTY       (void * handle, ASYNC_WAIT * wait, int call_id);
int TE_HangupData       (void * handle, ASYNC_WAIT * wait, int channel);
int TE_Hold             (void * handle, ASYNC_WAIT * wait, int call_id);
int TE_Retrieve         (void * handle, ASYNC_WAIT * wait, int call_id);
int TE_SwapCall         (void * handle, ASYNC_WAIT * wait, int active_id, int held_id);
int TE_SendDTMF         (void * handle, int call_id, int code);
int TE_IndicateRing     (void * handle, int call_id);
int TE_JoinMPTY         (void * handle, ASYNC_WAIT * wait, int call_id_1, int call_id_2);
int TE_SplitMPTY        (void * handle, ASYNC_WAIT * wait, int call_id);
int TE_ReplaceCall      (void * handle, ASYNC_WAIT * wait, int oldcall, int newcall);
int TE_MuteOn           (void * handle);
int TE_MuteOff          (void * handle);

int TE_DataDial         (void * handle, ASYNC_WAIT* wait);
int TE_StartData        (void * handle);
int TE_StopData         (void * handle);
int TE_SendData         (void * handle, char * data, int datalen);
int TE_RecvData         (void * handle, char * buf, int buflen);
int TE_Bind             (void * handle, const char * devname);

int TE_PB_Select        (void * handle, ASYNC_WAIT * wait, int memory);
int TE_PB_GetInfo       (void * handle, ASYNC_WAIT * wait, char * buffer, int buflen);
int TE_PB_Read          (void * handle, ASYNC_WAIT * wait, int start, int end, char * buffer, int buflen);
int TE_PB_Write         (void * handle, ASYNC_WAIT * wait, int index, char * buffer, int buflen);
int TE_PB_Delete        (void * handle, ASYNC_WAIT * wait, int index);
int TE_PB_Empty         (void * handle, ASYNC_WAIT * wait, int name);

int TE_SMS_Acknowledgment   (void * handle, int code, char * buf, int buflen);
int TE_SMS_PreferredStorage (void * handle, ASYNC_WAIT * wait, int mem1, int mem2);
int TE_SMS_PreferredFormat  (void * handle, ASYNC_WAIT * wait, int mode);
int TE_SMS_IndicationMode   (void * handle, ASYNC_WAIT * wait, int mode, int mt, int bm, int ds, int bfr);
int TE_SMS_GetMemoryStatus  (void * handle, ASYNC_WAIT * wait, char * buf, int buflen);
int TE_SMS_ListSIM          (void * handle, ASYNC_WAIT * wait, int stat, char * buf, int buflen);
int TE_SMS_ChangeStatus     (void * handle, ASYNC_WAIT * wait, int index, int status);
int TE_SMS_ReadSIM          (void * handle, ASYNC_WAIT * wait, int index, int mode, char * buffer, int buflen);
int TE_SMS_WriteSIM         (void * handle, ASYNC_WAIT * wait, int stat, char * pData, int datalen);
int TE_SMS_Delete           (void * handle, ASYNC_WAIT * wait, int index, int delflag );
int TE_SMS_Send             (void * handle, ASYNC_WAIT * wait, int mode, char * pData, int datalen);
int TE_SMS_Read             (void * handle, int flags, char * buffer, int buflen);

int TE_STK_Activation       (void * handle, int mode);
int TE_STK_GetNotifyDetails (void * handle, int notify, char * buffer, int buflen);
int TE_STK_GetCmdDetails    (void * handle, ASYNC_WAIT * wait, int cmd, char * buffer, int buflen);
int TE_STK_GetMenuItem      (void * handle, char * buffer, int buflen);
int TE_STK_GetIcon          (void * handle, ASYNC_WAIT * wait, int icon_id, int color, char * buffer, int buflen);
int TE_STK_SendResponse     (void * handle, int cmd, int status, char * string, int strlen);
int TE_STK_SelectMenu       (void * handle, int status, int itemID);
int TE_STK_UserActivity     (void * handle);
int TE_STK_IdleScreen       (void * handle);
int TE_STK_Language         (void * handle, char * name);
int TE_STK_BrowserTerm      (void * handle, int termCause);
int TE_STK_TerminateCmd     (void * handle);

#ifdef __cplusplus
};
#endif
#endif //_HMAP_TE_H_

