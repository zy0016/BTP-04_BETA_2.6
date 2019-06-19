/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Mobile Engine
 *
 * Purpose  : 
 *
\**************************************************************************/

#ifndef _ME_WIN_H_
#define _ME_WIN_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "hp_modcfg.h"
#define MEINIT_ATRET_COPSLEN	16

typedef struct tag_ME_Init_GetValue{
	BYTE	CREG;
	BYTE	CGREG;
	char	COPS_Numeric[MEINIT_ATRET_COPSLEN];

}ME_INIT_GETVALUE, *PME_INIT_GETVALUE;


// Define GSM Registration Status
#define ME_CREG_ABANDON			0
#define ME_CREG_HOMENET			1
#define ME_CREG_SEARCHING		2
#define ME_CREG_DENIED			3
#define ME_CREG_UNKNOWN			4
#define ME_CREG_ROAMING			5
#define ME_CREG_INVALID			-1

// Define GPRS Registration Status
#define ME_CGREG_ABANDON		0
#define ME_CGREG_HOMENET		1
#define ME_CGREG_SEARCHING		2
#define ME_CGREG_DENIED			3
#define ME_CGREG_UNKNOWN		4
#define ME_CGREG_ROAMING		5
#define ME_CGREG_INVALID		-1


//fufu 0619

/*CPHS service*/
#define MailboxNumbers		0x6F17	
#define InforNumbers		0x6F19
//end 0619


typedef struct tagSTK_PARAM
{
    char  cmd[1024];	//AT命令
	char* data;			//附加参数
	int	  datalen;		//参数长度
    int   timeout;		//超时
    int   line_count;   //行数
    int	  line_len;     //行长度
    void* revbuf;       //缓冲区指针
}STK_PARAM, *PSTK_PARAM;


enum me_result_code
{
#if defined SIEMENS_TC35
	ME_RS_SUCCESS,			//"OK"
	ME_RS_FAILURE,			//"ERROR","CME ERROR","CMS ERROR"
	ME_RS_BUSY,				//"BUSY"
	ME_RS_NODIALTONE,
	ME_RS_NOCARRIER,		//"NO CARRIER"
	ME_RS_CONNECT,			//"CONNECT"
	ME_RS_USERSTOP,			
	ME_RS_TIMEOUT,			//TIMEOUT
	ME_RS_ERRORSTATUS,
	ME_RS_NOSIMCARD,
	ME_RS_NOCONNECTION,
	ME_RS_LIMITSERVICE,
	ME_RS_RESELECTION,
	ME_RS_SEARCHING
#endif
#if defined WAVECOM_2C
	ME_RS_SUCCESS,			//"OK"
	ME_RS_FAILURE,			//"ERROR","CME ERROR","CMS ERROR"
	ME_RS_BUSY,				//"BUSY"
	ME_RS_NOANSWER,			//"NO ANSWER"
	ME_RS_NOCARRIER,		//"NO CARRIER"
	ME_RS_CONNECT,			//"CONNECT"
	ME_RS_USERSTOP,			
	ME_RS_TIMEOUT,			//TIMEOUT
	ME_RS_ERRORSTATUS,
	ME_RS_NOSIMCARD,
	ME_RS_NOTREADY
#endif
#if defined TI_MODULE
	ME_RS_SUCCESS,			//"OK"
	ME_RS_FAILURE,			//"ERROR","CME ERROR","CMS ERROR"
	ME_RS_BUSY,				//"BUSY"
	ME_RS_NODIALTONE,	
	ME_RS_NOANSWER,
	ME_RS_NOCARRIER,		//"NO CARRIER"
	ME_RS_CONNECT,			//"CONNECT"
	ME_RS_USERSTOP,			
	ME_RS_TIMEOUT,			//TIMEOUT
	ME_RS_ERRORSTATUS,
	ME_RS_NOSIMCARD
#endif
#if defined INFINION_MODULE
	ME_RS_SUCCESS,			//"OK"
	ME_RS_FAILURE,			//"ERROR","CME ERROR","CMS ERROR"
	ME_RS_BUSY,				//"BUSY"
	ME_RS_NOANSWER,			//"NO ANSWER"
	ME_RS_NOCARRIER,		//"NO CARRIER"
	ME_RS_CONNECT,			//"CONNECT"
	ME_RS_USERSTOP,			
	ME_RS_TIMEOUT,			//TIMEOUT
	ME_RS_ERRORSTATUS,
	ME_RS_NOSIMCARD,
	ME_RS_NOCONNECTION,
	ME_RS_LIMITSERVICE,
	ME_RS_RESELECTION,
	ME_RS_SEARCHING
#endif
#if defined ADI_MODULE
	ME_RS_SUCCESS,			//"OK"
	ME_RS_FAILURE,			//"ERROR","CME ERROR","CMS ERROR"
	ME_RS_BUSY,				//"BUSY"
	ME_RS_NODIALTONE,	
	ME_RS_NOANSWER,
	ME_RS_NOCARRIER,		//"NO CARRIER"
	ME_RS_CONNECT,			//"CONNECT"
	ME_RS_USERSTOP,			
	ME_RS_TIMEOUT,			//TIMEOUT
	ME_RS_ERRORSTATUS,
	ME_RS_NOSIMCARD
#endif

};

enum me_ur_code
{
#if defined WAVECOM_2C
	ME_URC_CALA,			//"+CALA: <timestring>,<index>"
	ME_URC_CBM,				//"+CBM: <length><pdu>	
	ME_URC_CBMI,			//"+CBMI: "BM",<index>"
	ME_URC_CCCM,			//"+CCCM: <ccm>"
	ME_URC_CCED,			//"+CCED: <values>"
	ME_URC_CCWA,			//"+CCWA: <number>,<type>,<class>[,<alpha>]"
	ME_URC_CDS,				//"+CDS: <length><pdu>"
	ME_URC_CDSI,			//"+CDSI: <mem>,<index>"
	ME_URC_CKEV,			//"+CKEV: <keynb>"
	ME_URC_CLIP,			//"+CLIP: <number>,<type>[,,,<alpha>]"
	ME_URC_CMT,				//"+CMT: <length><pdu>"
	ME_URC_CMTI,			//"+CMTI: <mem>,<index>
	ME_URC_CREG,			//"+CREG: <stat>[,<lac>,<ci>]"
	ME_URC_CGREG,
	ME_URC_CRING,			//"+CRING: <type>"
	ME_URC_CSQ,				//"+CSQ: <RxLev>,99"
	ME_URC_CSSU,			//"+CSSU: <code2>[,<number>,<type>]"
	ME_URC_STIN,			//"+STIN: <ind>"
	ME_URC_WIND,			//"+WIND: <IndicationNb>[,<CallId>]"
	ME_URC_WVMI,			//"+WVMI: <LineId>,<Status>
	ME_URC_RUI,				//"+RUI: <UserInfo>"
	ME_URC_RING,			//"RING"
	ME_URC_NOCARRIER		//"NO CARRIER"
#endif
#if defined SIEMENS_TC35
	ME_URC_CALA,			//"+CALA: <timestring>,<index>"
	ME_URC_CBM,				//"+CBM: <length><pdu>	
	ME_URC_CBMI,			//"+CBMI: "BM",<index>"
	ME_URC_CCCM,			//"+CCCM: <ccm>"
	ME_URC_CCED,			//"+CCED: <values>"
	ME_URC_CCWA,			//"+CCWA: <number>,<type>,<class>[,<alpha>]"
	ME_URC_CDS,				//"+CDS: <length><pdu>"
	ME_URC_CDSI,			//"+CDSI: <mem>,<index>"
	ME_URC_CKEV,			//"+CKEV: <keynb>"
	ME_URC_CLIP,			//"+CLIP: <number>,<type>[,,,<alpha>]"
	ME_URC_CMT,				//"+CMT: <length><pdu>"
	ME_URC_CMTI,			//"+CMTI: <mem>,<index>
	ME_URC_CREG,			//"+CREG: <stat>[,<lac>,<ci>]"
	ME_URC_CGREG,
	ME_URC_CRING,			//"+CRING: <type>"
	ME_URC_CSQ,				//"+CSQ: <RxLev>,99"
    ME_URC_CSSI,            //"+CSSI:<code>[,<index>]"
	ME_URC_CSSU,			//"+CSSU: <code2>[,<index>,<number>,<type>]"
	ME_URC_SSTN,
	ME_URC_SMGO,
	ME_URC_RUI,				//"+RUI: <UserInfo>"
	ME_URC_RING,			//"RING"
	ME_URC_NOCARRIER,		//"NO CARRIER"
	ME_URC_SYSSTART,
	ME_URC_UNDERVOLTAGE,
	ME_URC_CIEV 			//fufu 0601
#endif
#if defined TI_MODULE
	ME_URC_CBM,
	ME_URC_CBMI,
	ME_URC_CDS,
	ME_URC_CDSI,
	ME_URC_CMT,
	ME_URC_CMTI,
	ME_URC_CCCM,
	ME_URC_CCWA,
	ME_URC_CCWV,
	ME_URC_CDEV,
	ME_URC_CIEV,
	ME_URC_CKEV,
	ME_URC_CLAV,
	ME_URC_CLIP,
	ME_URC_CREG,
	ME_URC_CGREG,
	ME_URC_CRING,
	ME_URC_CSSU,
	ME_URC_CUSD,
	ME_URC_SATI,
	ME_URC_RING,
	ME_URC_NOCARRIER,
	ME_URC_SATN
#endif
#if defined INFINION_MODULE
	ME_URC_CALL,			//"+XCALLSTAT:"
	ME_URC_CALA,			//"+CALA: <timestring>,<index>"
	ME_URC_CBM,				//"+CBM: <length><pdu>	
	ME_URC_CBMI,			//"+CBMI: "BM",<index>"
	ME_URC_CCCM,			//"+CCCM: <ccm>"
	ME_URC_CCWA,			//"+CCWA: <number>,<type>,<class>[,<alpha>]"
	ME_URC_CDS,				//"+CDS: <length><pdu>"
	ME_URC_CDSI,			//"+CDSI: <mem>,<index>"
	ME_URC_CLIP,			//"+CLIP: <number>,<type>[,,,<alpha>]"
	ME_URC_CMT,				//"+CMT: <length><pdu>"
	ME_URC_CMTI,			//"+CMTI: <mem>,<index>
	ME_URC_CREG,			//"+CREG: <stat>[,<lac>,<ci>]"
	ME_URC_CGREG,
	ME_URC_CRING,			//"+CRING: <type>"
	ME_URC_CSSU,			//"+CSSU: <code2>[,<number>,<type>]"
	ME_URC_STK,				//"+STKPRO:"
	ME_URC_RING,			//"RING"
	ME_URC_NOCARRIER		//"NO CARRIER"
#endif
#if defined ADI_MODULE
	ME_URC_CALA,			//"+CALA: <timestring>,<index>"
	ME_URC_CBM,				//"+CBM: <length><pdu>	
	ME_URC_CBMI,			//"+CBMI: "BM",<index>"
	ME_URC_CCCM,			//"+CCCM: <ccm>"
	ME_URC_CCED,			//"+CCED: <values>"
	ME_URC_CCWA,			//"+CCWA: <number>,<type>,<class>[,<alpha>]"
	ME_URC_CDS,				//"+CDS: <length><pdu>"
	ME_URC_CDSI,			//"+CDSI: <mem>,<index>"
	ME_URC_CKEV,			//"+CKEV: <keynb>"
	ME_URC_CLIP,			//"+CLIP: <number>,<type>[,,,<alpha>]"
	ME_URC_CMT,				//"+CMT: <length><pdu>"
	ME_URC_CMTI,			//"+CMTI: <mem>,<index>
	ME_URC_CREG,			//"+CREG: <stat>[,<lac>,<ci>]"
	ME_URC_CGREG,
	ME_URC_CRING,			//"+CRING: <type>"
	ME_URC_CSQ,				//"+CSQ: <RxLev>,99"
	ME_URC_CSSU,			//"+CSSU: <code2>[,<number>,<type>]"
	ME_URC_SSTN,
	ME_URC_SMGO,
	ME_URC_RUI,				//"+RUI: <UserInfo>"
	ME_URC_RING,			//"RING"
	ME_URC_NOCARRIER,		//"NO CARRIER"
	ME_URC_SYSSTART,
	ME_URC_UNDERVOLTAGE
#endif
};

enum ind_mod {IND_EXCLUSIVE, IND_PRIORITY, IND_ORDINAL};
enum line_switch { 
	CUT_HELD,			// 呼叫等待时忙音或者挂断保持的通话
	CUT_ACTIVED,		// 挂断激活的通话
	ACTIVE_SWITCH,		// 在保持和激活的通话状态间切换
	ADD_TO_ACTIVE,		// 将保持的通话加入激活通话
	CUT_ALL,			// 挂断所有保持和激活的通话
	ACTIVE_SWITCH_X,	// 将除指定序号外的所有通话变为保持
	CUT_ACTIVED_X,		// 挂断指定序号的激活通话
    CALL_TRANSFER
};
enum call_class 
{ 
	CLASS_VOICE,
	CLASS_DATA,
#if !defined TI_MODULE
	CLASS_FAX,
	CLASS_SMS,
#endif
	CLASS_ALL
};

enum callfwd_style { 
	FWD_UNCDTIONAL, 
	FWD_BUSY,
	FWD_NOREPLY,
	FWD_NOREACH,
	FWD_ALLCDTIONAL,
	FWD_ALL
};

enum lock_fac {
	CALLBAR_BAOC,			//限制所有拨出电话
	CALLBAR_BOIC,			//限制所有拨出国际长途
	CALLBAR_BOIC_EXHC,		//限制所有拨出国际长途，除了本国
	CALLBAR_BAIC,			//限制所有拨入电话
	CALLBAR_BIC_ROAM,		//限制国际漫游时来电
	CALLBAR_UAB,			//解除所有通话限制
	CALLBAR_UAG,			//解除所有拨出限制
	CALLBAR_UAC,			//解除所有拨入限制
	LOCK_PS,
	LOCK_SC,
	LOCK_PN,
	LOCK_PU,
	LOCK_PP,
	LOCK_PC,
	LOCK_FD,
	LOCK_P2
};

enum sms_memtype {
	SMS_MEM_SM,
#if defined TI_MODULE
	SMS_MEM_ME
#endif
#if defined WAVECOM_2C
	SMS_MEM_SR,
	SMS_MEM_BM
#endif
#if defined SIEMENS_TC35
	SMS_MEM_ME,
	SMS_MEM_MT
#endif
#if defined INFINION_MODULE
	SMS_MEM_BM
#endif
};

#if defined SIEMENS_TC35
enum sms_bufstat
{
	SMS_BUF_FREE,
	SMS_BUF_OVERFLOW,
	SMS_BUF_WAITING
};
#endif

enum sms_stat {
	SMS_UNREAD,
	SMS_READED,
	SMS_UNSEND,
	SMS_SENDED,
	SMS_ALL
};

enum sms_storestat{
	SMS_STO_RECUNREAD,
	SMS_STO_RECREAD,
	SMS_STO_STOUNSENT,
	SMS_STO_STOSENT,
	SMS_STO_DEFAULT
};
enum pin_type 
{
	NOPIN,			//"+CPIN READY"
	PIN1,			//"+CPIN SIM PIN"
	PUK1,			//"+CPIN SIM PUK"
	PIN2,			//"+CPIN SIM PIN2"
	PUK2,			//"+CPIN SIM PUK2"
	SIMPHLOCK,		//"+CPIN PH-SIM PIN"
	SIMNTLOCK,		//"+CPIN PH-NET PIN"
    PHSIMPIN,
    PHSIMPUK,
    PHNETPUK
};

typedef enum
{
    SUPPRESSURC = 0,//禁止URC码
    OUTPUTURC   = 1//支持URC码
}SimCardURC;//关于URC码的状态
typedef enum
{
    NOSIMCARD = 0,//无卡或者sim卡损坏
    SIMINSERT = 1 //有卡
}SimCardInfo;//sim卡是否存在
typedef struct 
{
    SimCardURC SimCardUrc;
    SimCardInfo SimCard;
}SimCardStatus;//sim卡状态信息

//fufu 0619
typedef enum
{
	Line1_mailbox = 1,
	Line2_mailbox = 2,
	Data_mailbox = 3,
	Fax_mailbox = 4
}MailBox_Type;
//end 0619

//fufu 0630
typedef struct
{
	unsigned char	FileChar;			//file characteristics
	unsigned char 	CHV1Status;			//CHV1 status
	unsigned char	UnblockCHV1;		//Unblock CHV1 status
	unsigned char 	CHV2Status;			//CHV2 status
	unsigned char	UnblockCHV2;		//Unblock CHV2 status
}CPHSstatus_t;
//end 0630

//fufu 0619
typedef enum
{
	FieldNotFound = 0,
	FileContent,
	RecordContent
}Content_Type;

typedef struct
{
	Content_Type	Type;
	unsigned int 	Content_Len;
	unsigned int 	Record_Len;
}CPHS_struct;
//end 0619


//fufu 0614
typedef enum
{
	SMS_GPRS = 0,		//GPRS service
	SMS_GSM = 1,		//circuit switch service
	SMS_GPRSpre = 2,	//GPRS prefered (use circuit switched if mobile is not GPRS attached)
	SMS_GSMpre = 3		//circuit switched preferred (use GPRS if circuit switched is not available)
}SMS_Service;
//end 0614

//fufu 0602
typedef enum 
{
	Vmwait1,		//Voice Message Waiting Indication for line 1
	Vmwait2,		//Voice Message Waiting Indication for line 2
	Ciphcall,		//Ciphering Status Change Indication
	Rssi,			//signal strength
    Service,
    Battchg
}Ex_Indicator;
//end 0602

#if defined INFINION_MODULE
#define ME_PHONENUM_LEN			44
#else
#define ME_PHONENUM_LEN			42//24		//fufu 0718 for gexin
#endif

//fufu 0602
#define ME_RESPONSE_INT			4
#define ME_SERVICEOPT_LEN		60
//end 0602

#define ME_CPHSINFOMAXLEN       60

#define ME_PHONETXT_LEN			21//16   //fufu 0718 for gexin
#define ME_SMS_TIME_LEN			40
#define ME_SMS_CONTEXT_LEN		164
#define ME_OPRATOR_LEN			20
#define ME_MBID_LEN				40
#if defined INFINION_MODULE
#define ME_STKIND_LEN			200
#endif

enum cring_type
{
	ME_CRING_VOICE,
	ME_CRING_FAX,
	ME_CRING_ASYNC,
	ME_CRING_REL_ASYNC
};

typedef enum
{
    ME_AUDIO_DIGITAL = 1,//数字
    ME_AUDIO_ANALOG  = 2//模拟
}AudioInterfaceType;//通道类型

//fufu 0602
typedef struct
{
	Ex_Indicator	Indicator;
	int				Value;
} ME_EXINFO_ITEM, *PME_EXINFO_ITEM;
//end 0602

typedef struct 
{
    char cCNL[10];
}CPHSparasExt;

typedef struct tagME_PHONE_ITEM
{
	char	PhoneNum[ME_PHONENUM_LEN];
	char	PhoneStr[ME_PHONETXT_LEN];

} ME_PHONE_ITEM, *PME_PHONE_ITEM;

typedef struct tagME_CSSI_ITEM
{
	int		Action;
    int     Index;
} ME_CSSI_ITEM, *PME_CSSI_ITEM;

typedef struct tagME_CSSU_ITEM
{
	int		Action;
    int     Index;
	char	PhoneNum[ME_PHONENUM_LEN];
}ME_CSSU_ITEM, *PME_CSSU_ITEM;

typedef struct tagME_CMTI_ITEM{
	int		DevType;
	int		Index;
} ME_CMTI_ITEM, *PME_CMTI_ITEM;

typedef struct tagME_CREG_URC
{
	int		Stat;		//same to result
#define		CREG_NOREG			0
#define		CREG_HOMEREG		1
#define		CREG_SEARCHING		2
#define		CREG_DENIED			3
#define		CREG_UNKNOWN		4
#define		CREG_ROAMING		5
	char	lac[6];		//location area code
	char	ci[6];		//cell id
}ME_CREG_URC, ME_CGREG_URC, *PME_CREG_URC, *PME_CGREG_URC;

#if defined WAVECOM_2C
typedef struct tagME_WIND_ITEM{
	int		Event;

#define ME_WIND_INIT_COMPLETE	4
#define	ME_WIND_CALL_CREATED	5
#define	ME_WIND_CALL_RELEASED	6
	int		CallID;
}ME_WIND_ITEM, *PME_WIND_ITEM;
#endif

#if defined INFINION_MODULE

typedef struct tagME_CALL_STAT{
	int		id;
	int		stat;
#define CALL_STAT_DISCONNECT		6

}ME_CALL_STAT, *PME_CALL_STAT;

#endif

typedef struct tagCALL_FWD
{
	int		Reason;
	BOOL	bEnable;
	char	PhoneNum[ME_PHONENUM_LEN];
	int		Class;
	int		Time;
}CALL_FWD, *PCALL_FWD;

typedef struct tagCALLS_INFO
{
	int     index;								// call id
#define CALL_STAT_ACTIVE		0
#define CALL_STAT_HELD			1
#define CALL_STAT_DIALING		2
#define CALL_STAT_ALERTING		3
#define CALL_STAT_INCOMING		4
#define CALL_STAT_WAITING		5
	int		Stat;								// call state

#define CALL_MODE_VOICE			0
#define CALL_MODE_DATA			1
#define CALL_MODE_FAX			2
#define CALL_MODE_UNKNOWN		9
	int		Mode;								// call mode

#define	CALL_DIR_ORIGINATED		0
#define CALL_DIR_TERMINATED		1
	int		Dir;								// call direction

	int		Mpty;								// whether one of multiparty
	char	PhoneNum[ME_PHONENUM_LEN];			// responding call
	char	Text[ME_PHONETXT_LEN];				// content of call, maybe ""

}CALLS_INFO, *PCALLS_INFO;

typedef struct tagCallFwd_Info
{
	BOOL	Enable;		// FALSE:Disable, TRUE:Enable
	char	PhoneNum[ME_PHONENUM_LEN];
	int		Time;
	int		Class;

} CALLFWD_INFO, *PCALLFWD_INFO;

typedef struct tagCALLWAIT_INFO
{
	BOOL Enable;
	int  Class;
} CALLWAIT_INFO, *PCALLWAIT_INFO;

typedef struct tagCALLBAR_INFO 
{
	BOOL Enable;
	int  Class;
} CALLBAR_INFO, *PCALLBAR_INFO;

// ME_SMS_SEND
enum sms_type { SMS_SUBMIT, SUBMIT_REPORT, STATUS_REPORT, SMS_DELIVER };
typedef struct tagSMS_Send
{
	char	SCA[ME_PHONENUM_LEN];
	int		Type;
	int		udhl;
	int		dcs;
#define DCS_GSM         0
#define DCS_8BIT        1
#define DCS_UCS2        2
	int		ConLen;
	BOOL	Report;
	BOOL	ReplyPath;
	int		PID;
#define		PID_IMPLICIT	0
#define		PID_TELEX		1
#define		PID_TELEFAX		2
#define		PID_VOICE		3
#define		PID_ERMES		4
#define		PID_NPAGING		5
#define		PID_X400		6
#define		PID_EMAIL		7

	int		VPvalue;
#define SMS_1HOUR	0	//一小时
#define SMS_6HOUR	1	//六小时
#define SMS_24HOUR	2	//二十四小时
#define SMS_72HOUR	3	//七十二小时
#define SMS_1WEEK	4	//一星期
#define SMS_MAXTIME	5	//最长时间

	char	PhoneNum[ME_PHONENUM_LEN];
	char	UDH[ME_SMS_CONTEXT_LEN];
	char	Context[ME_SMS_CONTEXT_LEN];

}SMS_SEND, *PSMS_SEND;

typedef struct tagSMS_INFO
{
	int		Index;
	int		Stat;
	int		Type;
	int		Status;
	int		dcs;
	int		udhl;
	int		ConLen;
    int     MR;
	int		Class;
#define SMS_CLASS_NONE	0
#define SMS_CLASS_0		1
#define SMS_CLASS_1		2
#define SMS_CLASS_2		3
#define SMS_CLASS_3		4
	char	SCA[ME_PHONENUM_LEN];
	char	SenderNum[ME_PHONENUM_LEN];
	char	SendTime[ME_SMS_TIME_LEN];
	char	UDH[ME_SMS_CONTEXT_LEN];
	char	Context[ME_SMS_CONTEXT_LEN];

}SMS_INFO, *PSMS_INFO;

typedef struct 
{
    int ParameterIndicators;//Parameter Indicators
    char DestinationAddress[ME_PHONENUM_LEN];//TP-Destination Address
    char SCA[ME_PHONENUM_LEN];//TS-Service Centre Address
    int ProtocolIdentifier;//TP-Protocol Identifier
    int DataCodingScheme;//TP-Data Coding Scheme
    int ValidityPeriod;//TP-Validity Period
}SMS_CPHS_INFO;

enum cbs_code {
    CBS_ALPHA_GSM,
    CBS_ALPHA_DATA,
    CBS_ALPHA_UCS2
};

#define CBS_LANG_GERMAN     0x00000001
#define CBS_LANG_ENGLISH    0x00000002
#define CBS_LANG_ITALIAN    0x00000004
#define CBS_LANG_FRENCH     0x00000008
#define CBS_LANG_SPANISH    0x00000010
#define CBS_LANG_DUTCH      0x00000020
#define CBS_LANG_SWEDISH    0x00000040
#define CBS_LANG_DANISH     0x00000080
#define CBS_LANG_PORTUGUESE 0x00000100
#define CBS_LANG_FINNISH    0x00000200
#define CBS_LANG_NORWEGIAN  0x00000400
#define CBS_LANG_GREEK      0x00000800
#define CBS_LANG_TURKISH    0x00001000
#define CBS_LANG_HANGARIAN  0x00002000
#define CBS_LANG_POLISH     0x00004000
#define CBS_LANG_CZECH      0x00008000
#define CBS_LANG_RESERVE    0x00010000

enum cbs_class {
    CBS_CLASS0,
    CBS_CLASS1,
    CBS_CLASS2,
    CBS_CLASS3,
    CBS_NOCLASS
};
// Cell Broadcasting Message Info
typedef struct tagCBSInfo {
    unsigned short  SNum;       // Serial Number
    unsigned short  MId;        // Message Identifier
    int             Code;       // Alphabet
    int             Class;      // Msg Class
    unsigned long   Lang;       // Language
    int             PageNum;    // Total number of page
    int             PageRef;    // page reference
    int             DataLen;
    char            Data[94];   // Message Content
} CBS_INFO, *PCBS_INFO;

enum ussdresponse 
{
    ME_USSD_ABORT,
    ME_USSD_REFUSE,
    ME_USSD_NOTIFY,
    ME_USSD_REQUEST,
    ME_USSD_DISCONNECT
};

typedef struct tagUSSDInfo {
    int             Flag;
    int             Code;
    unsigned long   Lang;
    int             DataLen;
    char            Data[160];
} USSD_INFO, *PUSSD_INFO;

typedef struct tagSMSMem_Info
{
	int		nMem1Used;
	int		nMem1Total;
	int		nMem2Used;
	int		nMem2Total;
#if defined SIEMENS_TC35 || defined TI_MODULE || defined INFINION_MODULE || defined ADI_MODULE
	int		nMem3Used;
	int		nMem3Total;
#endif

}SMSMEM_INFO, *PSMSMEM_INFO;

typedef struct tagIND_STATUS
{
	int		Mt;			//短信通知方式
#define IND_NONE		0	//新短信到来时不通知
#define IND_BUFFER		1	//新短信到来时先存储后通知
#define IND_DIRECT		2	//新短信到来时直接通知
	int		Bm;			//小区广播通知方式
	int		Ds;			//状态报告通知方式
}IND_STATUS, *PIND_STATUS;

typedef struct tagCELL_BROADCAST
{
	BOOL mode;
	char ids[40];
	unsigned long code;

}CELL_BROADCAST, *PCELL_BROADCAST;

typedef struct tagPHONEBOOK
{
	int		Index;
	char	PhoneNum[ME_PHONENUM_LEN];
	char	Text[ME_PHONETXT_LEN];

}PHONEBOOK, *PPHONEBOOK;

enum pbmem_type { PBOOK_SM, PBOOK_ME, PBOOK_FD, PBOOK_LD, PBOOK_MC, PBOOK_RC, PBOOK_ON };
typedef struct tagPBMEM_INFO
{
	int		Type;
	int		Used;
	int 	Total;

}PBMEM_INFO, *PPBMEM_INFO;

typedef struct tagPBNUMTEXT_INFO
{
    unsigned int iNumberMaxLen;
    unsigned int iTextMaxLen;
}PBNUMTEXT_INFO;

#define MODE_AUTO     0
#define MODE_MANUAL   1
#define MODE_UNREGIST 2


#define	FORMAT_LONGALPHA	0
#if defined WAVECOM_2C || defined TI_MODULE || defined INFINION_MODULE || defined ADI_MODULE
#define FORMAT_SHORTALPHA	1
#endif
#define	FORMAT_NUMERIC		2

typedef struct tagCurOprator
{
	int Mode;
	int Format;
	char Name[ME_OPRATOR_LEN];

}CUROPRATOR, *PCUROPRATOR;

#define STAT_UNKNOWN	0
#define STAT_AVAILABLE	1
#define STAT_CURRENT	2
#define STAT_FORBIDDEN	3

typedef struct tagNETOPREATOR
{
	int		stat;
	char	alpha_oper[ME_OPRATOR_LEN];
	char	num_oper[ME_OPRATOR_LEN];

}NETOPRATOR, *PNETOPRATOR;

typedef struct tagPREFER_OPRATOR
{
	int		index;
	char	oper_id[ME_OPRATOR_LEN];

}PREFER_OPRATOR, *PPREFER_OPRATOR;

#if defined WAVECOM_2C

enum charge_mode
{
	MIN_LEVEL,		//最小电量，需要充电
	MAX_LEVEL,		//最大电量，充电满
	CHG_IN,			//充电过程中
	CHG_OUT			//充电结束
};

#elif defined SIEMENS_TC35

typedef enum 
{
	CHG_OFF,		//未连接充电器
	CHG_CONNECT,	//连接充电器
	CHG_INPROG,		//充电进行中
	CHG_FINISH,		//充电结束
	CHG_ERROR,		//充电错误
	CHG_HIGHTMP		//电池温度过高
}charge_mode;

//fufu 0704
typedef struct
{
	charge_mode		ChargeStatus;
	int				BatteryCapacity;
	int				PowerConsumption;
}ChargeCtl_t;
//end 0704


#else

enum
{
	BATTERY_INUSE,
	BATTERY_NOUSE,
	BATTERY_NOCON,
	POWER_FAULT
};

#endif

#define ME_BATTERY_NORMAL		0
#define ME_BATTERY_WARNING		1
#define ME_BATTERY_SHUTDOWN		2

typedef struct tagBATTERY_INFO
{
	int BCMode;
	int BCValue;
	int BCLever;
	int BCNotify;
	int Consuming;

}BATTERY_INFO, *PBATTERY_INFO;

typedef enum { CLIR_DEFAULT, CLIR_ENABLE, CLIR_DISABLE } CLIR_MODE;
typedef enum { CLIR_NPROVIS, CLIR_PERMANENT, CLIR_UNKNOWN, CLIR_TEMP_RES, CLIR_TEMP_ALLOW } CLIR_STATUS;

typedef struct {
	CLIR_STATUS		CLIR_Status;
	CLIR_MODE		CLIR_Setting;
} CLIR_SERVICE;


#if defined SIEMENS_TC35
typedef struct tagSAT_Param
{
	int state;
#define SAT_RESET		0
#define SAT_OFF			1
#define SAT_IDLE		2
#define SAT_PAC			3
#define SAT_WAIT		4
	
	int alphabet;
#define ALPHA_GSM		0
#define ALPHA_UCS2		1
	int instance;
#define INS_OTHER		0
#define INS_THIS		1
	char profile[40];
}SAT_PARAM, *PSAT_PARAM;
#endif

#if defined INFINION_MODULE
typedef struct tagSAT_Param
{
	int state;
#define SAT_RESET		0
#define SAT_OFF			1
#define SAT_IDLE		2
#define SAT_PAC			3
#define SAT_WAIT		4
	
	int alphabet;
#define ALPHA_GSM		0
#define ALPHA_UCS2		1
	int instance;
#define INS_OTHER		0
#define INS_THIS		1
	char profile[40];
}SAT_PARAM, *PSAT_PARAM;
#endif

#if defined SIEMENS_TC35
typedef struct tagSERV_CELL
{
	char BCH[8];		//Broadcast Channel
	char BRXL[8];		//Receiving level of BCCH carrier
	char BCC[8];		//Base station Color Code
	char RXLev[8];		//minimal level to allow registration
	char MCC[8];		//Mobile Country Code
	char MNC[8];		//Mobile Network Code
	char BSIC[8];		//Base Station Identity Code
	char NCC[8];		//PLMN color code
	char RSSI[8];		//Received Signal Strength Indication
	char LAC[8];		//Location Area Code
	char CELLID[8];		//Cell ID
	char C1[8];
	char C2[8];
	char PWR[8];		//maximal power level used on RACH level
	char TA[8];			//Time Advance
	char TCH[8];		//Traffic Channel
	char TRXL[8];		//Receiving level of TCH carrier
	char TS[8];			//Time Slot
	char PWRLev[8];		//Current Power Level
	char Q[8];			//Receiving Quality
	char Chmod[8];		//Channel Mode
}SCELL_INFO, *PSCELL_INFO;

typedef struct tagNEIB_CELL
{
	char BCH[8];		//Broadcast Channel
	char BRXL[8];		//Receiving level of BCCH carrier
	char C1[8];
	char C2[8];
}NCELL_INFO, *PNCELL_INFO;

#endif

int ME_Initialize( int Mode );
enum me_init_mode
{
	MOD_COM,
	MOD_MUX
};

enum SS {
    SSCD,       /* Call Deflection */
    SSCLIP,     /* Call Line Identification Presentation */
    SSCLIR,     /* Call Line Identification Restriction */
    SSCOLP,     /* Connected Line Identification Presentation */
    SSCOLR,     /* Connected Line Identification Restriction */
    SSCF,         /* Call Forwarding */
    SSCNAP,       /* Calling Name Presentation */
    SSCW,         /* Call Waiting */
    SSHOLD,       /* Call Hold */
    SSMPTY,       /* Multi Party Service */
    SSCUG,        /* Closed User Group */
    SSAOCI,       /* Advice of Charge Information */
    SSAOCC,       /* Advice of Charge Charging */
    SSUUS,        /* User to User Signalling */
    SSCB,         /* Call Barring */
    SSECT,        /* Explicit Call Transfer */
    SSeMLPP,      /* enhanced Multi-Level Precedence and Pre-emption */
    SSSPNP,       /* Support of Private Numbering Plan */
    SSCCBS,       /* Completion of calls to busy Subscribers */
    SSUNKNOWN      
};

typedef enum {
    SSReg,
    SSAct,
    SSInt,
    SSDeact,
    SSErase
} SSOp;

typedef enum {
    BSVOICE,    // Voice
    BSDATA,     // Data
    BSFAX,      // FAX
    BSSMS,      // Short Message
    BSDCS,      // Data circuit sync
    BSDCA,      // Data circuit async
    BSDPAC,     // Dedicated packet access
    BSDPAD      // Dedicated PAD access
} SSBS;

typedef enum {
    CFU,
    CFB,
    CFNRy,
    CFNRc
} CFTYPE;

typedef struct tagCFInfo {
    CFTYPE  Reason;
    BOOL    Active;
    SSBS    BS;
    int     Time;
    char    DN[ME_PHONENUM_LEN];
}CFINFO, *PCFINFO;

typedef enum {
    BAOC,
    BOIC,
    BOIC_exHC,
    BAIC,
    BIC_Roam
} CBTYPE;

typedef struct tagCBInfo {
    CBTYPE Fac;
    BOOL   Active;
    SSBS    BS;
} CBINFO, *PCBINFO;

typedef struct tagCWInfo {
    BOOL    Active;
    SSBS    BS; 
} CWINFO, *PCWINFO;

typedef enum {
    CLIPNONE,
    CLIPALLOW,
    CLIPUNKNOWN
} CLIPSTATUS;

typedef struct tagCLIPInfo {
    CLIPSTATUS Status;
}CLIPINFO, *PCLIPINFO;

typedef enum {
    CLIRNONE,
    CLIRPERMANENT,
    CLIRUNKNOWN,
    CLIRTEMPRES,
    CLIRTEMPALLOW
} CLIRSTATUS;

typedef struct tagCLIRInfo {
    CLIRSTATUS Status;
}CLIRINFO, *PCLIRINFO;


typedef struct tagCOLPInfo {
    BOOL Active;
} COLPINFO, *PCOLPINFO;

typedef struct tagCOLRInfo {
    BOOL Active;
} COLRINFO, *PCOLRINFO;

typedef struct tagME_PUC {
    char currency[4];
    char ppu[21];
    char unused1;
    char unused2;
    char unused3;
} ME_PUCINFO, *PME_PUCINFO;

typedef struct tagME_ALARMTIME
{
	unsigned char year;
	unsigned char month;
	unsigned char day;
	unsigned char hour;
	unsigned char minute;
	unsigned char second;
	unsigned char unused1;
	unsigned char unused2;
}ME_ALARMTIME, *PME_ALARMTIME;

typedef struct tagEXTENDEDERROR
{
	int LocationID;
	int Reason;
	int SSrelease;
}ME_EXERROR, *PME_EXERROR;


typedef enum{
	Unknow,
	Allocated,
	NotAllocated
}SupportSta;

typedef enum{
	Inactivated,
	Activated
}ServiceSta;

typedef struct{
	SupportSta SupportStatus;
	ServiceSta CurrentStatus;
}SimService;

typedef struct{
	SimService	FDN;
	SimService	AOC;
	SimService	CHV1;
}SimFunctions;

typedef struct 
{
    char BCCH[10];
    char G[5];
    char PBCCH[5];
    char PAT[5];
    char MCC[10];
    char MNC[10];
    char NOM[10];
    char TA[10];
    char TAC[10];
}MONITORINFO;

typedef struct tagACMMode
{
	int nNum;
	int mode[8];
}ACMMODE, *PACMMODE;

typedef struct tagACMInfo
{
	int mode;
	int acm;
	int acmMax;
}ACMINFO, *PACMINFO;

typedef struct tagCONETWORKIFNO
{
	int mcc;
	int mnc;
	int network;	// network subset
	int provider;	// service provider ID
	int coporate;	// coporate ID of co-operative networks
}CONETWORKINFO, *PCONETWORKINFO;

typedef struct tagCONETWORKLIST
{
	int amount;
	CONETWORKINFO conetwork[8];
}CONETWORKLIST, *PCONETWORKLIST;

#define MONITORSEARCHING    "Searching"
int ME_GPRS_Monitor( HWND hWnd, int Msg );
BOOL GetSupportSTK(void);//获得是否支持stk
int ME_SetAlarm             (HWND hwnd, int msg, ME_ALARMTIME* time, int index);
int ME_GetAlarm             (HWND hwnd, int msg, int index);
int ME_SetSpeakerVolumeSNFV( HWND hwnd, int msg, int level );
int ME_GetSpeakerVolumeSNFV( HWND hwnd, int msg );
int ME_SetSpeakerVolume     (HWND hwnd, int msg, int level);
int ME_GetSpeakerVolume     (HWND hwnd, int msg);
int ME_GSMInit_Module       (HWND hWnd, UINT Msg);
int ME_GSMInit_Module_2     (HWND hWnd, UINT Msg);
int ME_GSMInit_Module_3     (HWND hWnd, UINT Msg);
int ME_InitSTK				(HWND hWnd, UINT Msg );
int ME_GetCoNetworkList		(HWND hWnd, UINT Msg);
int ME_SelectAudioModeEx	(HWND hwnd, int msg, int mode);
int ME_GetModuleInfo		(HWND hWnd, UINT Msg);

#if defined WAVECOM_2C
int ME_GSMInit_SIM          (HWND hWnd, UINT Msg);
#endif
int ME_InitRate(HWND hWnd, UINT Msg );
int ME_RegisterIndication   (int MsgClass, int Mode, HWND hwnd, UINT msg);
BOOL ME_UnRegisterIndication (int Handle);

int ME_CancelFunction       (int Handle);
int ME_GetResult            (void* result, int result_len);

int ME_SwitchAudioInterface(HWND hWnd,int Msg,AudioInterfaceType AudioType);
int ME_SpecialDial          (HWND hWnd, int nMsg, char * ssCode, BOOL callControl);
int ME_VoiceDial            (HWND hWnd, int Msg, char* PhoneNum, char* Extention);
int ME_DataDial (HWND hWnd, int Msg, char* PhoneNum, char* Extention);
int ME_GetSALSstatus( HWND hWnd, int Msg);
int ME_SelectOutLine( HWND hWnd, int Msg, int LineNum, int Indication );
int ME_VoiceHangup          (HWND hWnd, int Msg);
int ME_CallAnswer           (HWND hWnd, int Msg);
int ME_ListCurCalls         (HWND hWnd, int Msg);
int ME_SwitchCallStatus     (HWND hWnd, int Msg, int Style, int Index);
int ME_SetAutoAnswerStatus  (HWND hWnd, int Msg, int Times);
int ME_GetAutoAnswerStatus  (HWND hWnd, int Msg);
int ME_SetCallWaitingStatus (HWND hWnd, int Msg, BOOL bEnable, int Class);
int ME_GetCallWaitingStatus (HWND hWnd, int Msg, int Class);
int ME_SetCallForwardStatus (HWND hWnd, int Msg, PCALL_FWD pCallFwd);
int ME_GetCallForwardStatus (HWND hWnd, int Msg, int Style, int Class);
int ME_SetCallBarringStatus (HWND hWnd, int Msg, int Style,BOOL bEnable, char* Password, int Class);
int ME_GetCallBarringStatus (HWND hWnd, int Msg, int Style, int Class, char* Password);

int ME_SMS_Send             (HWND hWnd, int Msg, PSMS_SEND pSend);
int ME_SMS_Read             (HWND hWnd, int Msg, BOOL bPreRead, int MemType, int Index);
int ME_SMS_Write		( HWND hWnd, int Msg, PSMS_SEND pSend, int Location, int StoreStat);	//fufu 0726
int ME_SMS_List             (HWND hWnd, int Msg, int Type, BOOL bPreRead, int MemType);
int ME_SMS_Delete           (HWND hWnd, int Msg, int MemType, int Index);
int ME_SMS_SetSCA           (HWND hWnd, int Msg, char* SCA);
int ME_SMS_GetSCA           (HWND hWnd, int Msg);
int ME_SMS_SetIndStatus     (HWND hWnd, int Msg, PIND_STATUS pStatus);
int ME_SMS_GetIndStatus     (HWND hWnd, int Msg);
int ME_SMS_SetCellBroadcast (HWND hWnd, int Msg, PCELL_BROADCAST pStatus);
int ME_SMS_GetCellBroadcast (HWND hWnd, int Msg);
int ME_SMS_GetMemStatus     (HWND hWnd, int Msg, int MemType);
int ME_SMS_Acknowledge      (HWND hwnd, int msg);

int ME_ReadPhonebook        (HWND hWnd, int Msg, int Type, int IndexBegin, int IndexEnd);
int ME_WritePhonebook       (HWND hWnd, int Msg, int Type, PPHONEBOOK Record);
int ME_GetPhonebookMemStatus(HWND hWnd, int Msg, int Type);
int ME_GetPhonebookNumberTextMaxLen(HWND hWnd,int Msg);

int ME_SetPhoneLockStatus   (HWND hWnd, int Msg, int Type, char* Password, BOOL bEnable);
int ME_GetPhoneLockStatus   (HWND hWnd, int Msg, int Type, char* Password);
int ME_ChangeLockPassword   (HWND hWnd, int Msg, int Type, char* OldPass, char* NewPass);
int ME_PasswordValidation   (HWND hWnd, int Msg, int Type, char* Password, char* NewPass);
int ME_GetCurWaitingPassword(HWND hWnd, int Msg);
int ME_GetPassInputLimit    (HWND hWnd, int Msg, int Type);
int ME_GetCurPasswordStatus( HWND hWnd, int Msg);//获得当前sim卡锁状态,主要是pin2码的状态
int ME_GetSIMCardStatus(HWND hWnd,int Msg);//获得sim卡状态

int ME_SetOpratorMode       (HWND hWnd, int Msg, int Mode, char* OperId);
int ME_GetCurOprator        (HWND hWnd, int Msg, int Format);
int ME_SearchNetOprator     (HWND hWnd, int Msg);

#if defined SIEMENS_TC35
int ME_ReadPrefOperList     (HWND hWnd, int Msg, int IndexBegin, int IndexEnd);
#else
int ME_ReadPrefOperList     (HWND hWnd, int Msg);
#endif

int ME_WritePrefOperList    (HWND hWnd, int Msg, int index, char* oper_id);
int ME_GetEquipmentId       (HWND hWnd, int Msg);
int ME_GetSuscriberId       (HWND hWnd, int Msg);
int ME_SetClockTime         (HWND hWnd, int Msg, char* DateTime);
int ME_GetClockTime         (HWND hWnd, int Msg);
int ME_MuteControl          (HWND hWnd, int Msg, BOOL bEnable);
int ME_SendDTMF             (HWND hWnd, int Msg, char SendChar);
int ME_FactoryDefaultSetting(HWND hWnd, int Msg);
int ME_ResetModule          (HWND hWnd, int Msg);
int ME_ShutdownModule       (HWND hWnd, int Msg);
int ME_GetBatteryInfo       (HWND hWnd, int Msg);
int ME_GetSignalInfo        (HWND hWnd, int Msg);
int ME_GetSubscriberId ( HWND hWnd, int Msg );
int ME_GetChargeStatus( HWND hWnd, int Msg );

int ME_ClearSMSindication( HWND hWnd, int Msg);


int ME_GetCPHSstatus( HWND hWnd, int Msg);
int ME_GetCPHSInfo(HWND hWnd,int Msg);

int ME_SetBatteryChargeControl(HWND hWnd,int Msg,int iCurrent);
int ME_GetCPHSparameters( HWND hWnd, int Msg, unsigned int CPHS_Field );
int ME_GetCPHSparametersExt( HWND hWnd, int Msg, unsigned int CPHS_Field );
int ME_ReadCPHSFile( HWND hWnd, int Msg, unsigned int CPHS_Field, unsigned int FileSize );

int ME_ReadCPHSRecord( HWND hWnd, int Msg, unsigned int CPHS_Field, unsigned int RecSize, unsigned int RecNo );
int ME_ReadCPHSRecordSMS( HWND hWnd, int Msg, unsigned int CPHS_Field, unsigned int RecSize, unsigned int RecNo );
void ParseMailboxNumber(unsigned char *SrcData, unsigned int ResultLen, unsigned char *DesData);

int ME_GetSMSservice( HWND hWnd, int Msg);

int ME_SetSMSservice( HWND hWnd, int Msg, SMS_Service Service);

int ME_UpdateCPHSFile( HWND hWnd, int Msg, unsigned int CPHS_Field, unsigned char *FileContent, unsigned int FileSize, int Offset );
int ME_UpdateCPHSRecord( HWND hWnd, int Msg, unsigned int CPHS_Field, unsigned char *RecContent, unsigned int RecSize, unsigned int RecNo );
int ME_GetExIndicator( HWND hWnd, int Msg, Ex_Indicator Indi);

int ME_GetHomeZone( HWND hWnd, int Msg);

int ME_GetServierProvider( HWND hWnd, int Msg);

int PW_SetAlarm(ME_ALARMTIME *AlarmTime);
int PW_ReadClock(char *P_Time);
int PW_GetCharge(void *ChargeCtl);


int ME_GetCCM               (HWND hWnd, int nMsg);
int ME_SetCCM				(HWND hWnd, int nMsg, unsigned long mode);
int ME_GetACM               (HWND hWnd, int nMsg);
int ME_TestSACM				(HWND hWnd, int nMsg);
int ME_GetSACM				(HWND hWnd, int nMsg);
int ME_SetSACM				(HWND hWnd, int nMsg, unsigned long mode);
int ME_TestACMmax			(HWND hWnd, int nMsg);
int ME_GetACMmax            (HWND hWnd, int nMsg);
int ME_ResetACM             (HWND hWnd, int nMsg, char * Password);
int ME_SetACMmax            (HWND hWnd, int nMsg, unsigned long ACMmax, char * Password);
int ME_GetPricePerUnit      (HWND hWnd, int nMsg);
int ME_GetGPRSAttachSta (HWND hWnd, int nMsg);
int ME_SetGPRSAttach (HWND hWnd, int nMsg, int AttachSta);
int ME_SetPricePerUnit      (HWND hWnd, int nMsg, PME_PUCINFO Puc, char * Password);

int ME_GetUSSDInfo          (PUSSD_INFO info);
int ME_USSD_SetNotify       (HWND hWnd, int nMsg);
void * ME_USSD_Request      (char * str);
int ME_USSD_Response        (void * Obj, char * str);
int ME_USSD_Abort           (void * Obj);

#if defined WAVECOM_2C
int ME_STK_Send             (HWND hwnd, int msg, BOOL direct, STK_PARAM* param);
#else
int ME_STK_Send             (HWND hwnd, int msg, STK_PARAM* param);
#endif

#if defined SIEMENS_TC35
int ME_SleepModule          (HWND hwnd, int msg);
int ME_STK_GetRemoteSAT     (HWND hwnd, int msg);
int ME_STK_ReturnIdle( HWND hwnd, int msg);
int ME_GetServCellInfo      (HWND hwnd, int msg);
int ME_GetNeibCellInfo      (HWND hwnd, int msg);
#define     ME_AUDIO_HEADSET    1
#define     ME_AUDIO_HANDSET    2
#define		ME_AUDIO_BLUETOOTH	3
int ME_SelectAudioMode      (HWND hwnd, int msg, int mode);
int ME_SetCallLineIndRestriction(HWND hwnd, int msg, CLIR_MODE para);
int ME_GetCallLineIndRestriction(HWND hwnd, int msg);
int ME_SendATCmd            (HWND hwnd, int msg, char* cmd, int timeout);
int ME_SMS_SetFormat        (HWND hwnd, int msg, BOOL bPDU);
int ME_SMS_SetProtocol      (HWND hwnd, int msg, BOOL bPhasePlus);

#endif
#if defined INFINION_MODULE
int ME_STK_GetRemoteSAT     (HWND hwnd, int msg);
int ME_GetServCellInfo      (HWND hwnd, int msg);
int ME_GetSoftwareVersion   (HWND hwnd, int msg);
int ME_DataDial             (HWND hwnd, int msg, char* PhoneNum);
int ME_DataHangup           (HWND hwnd, int msg);
int ME_GPRSDial             (HWND hwnd, int msg, char* PhoneNum);
int ME_GPRSHangup           (HWND hwnd, int msg);
#endif

#if defined WAVECOM_2C
int ME_SMS_SetFormat        (HWND hwnd, int msg, BOOL bPDU);
int ME_SMS_SetProtocol      (HWND hwnd, int msg, BOOL bPhasePlus);
int ME_DataDial             (HWND hwnd, int msg, char* PhoneNum);
int ME_DataHangup           (HWND hwnd, int msg);
int ME_GPRSDial             (HWND hwnd, int msg, char* PhoneNum);
int ME_GPRSHangup           (HWND hwnd, int msg);
int ME_SendATCmd            (HWND hwnd, int msg, char* cmd, int timeout);
int ME_SetCallLineIndRestriction(HWND hwnd, int msg, CLIR_MODE para);
int ME_GetCallLineIndRestriction(HWND hwnd, int msg);

#endif
int ME_GetLastError         (HWND hWnd, int Msg);
int GetCMEErrorInfo(int iErrorID,char * pErrorInfo,unsigned int Bufflen);
#define ME_E_MAXACM         (0x00080044L)
#define ME_E_CALLBARRED      (0x0000012CL)

#if defined TI_MODULE
int ME_SendATCmd            (HWND hwnd, int msg, char* cmd, int timeout);
int ME_SMS_SetFormat        (HWND hwnd, int msg, BOOL bPDU);
int ME_SMS_SetProtocol      (HWND hwnd, int msg, BOOL bPhasePlus);
int ME_GetSTKMainMenu       (char* buf, int buflen);

int ME_SetCallLineIndRestriction(HWND hwnd, int msg, CLIR_MODE para);
int ME_GetCallLineIndRestriction(HWND hwnd, int msg);
int ME_DisableSIMCard       (HWND hwnd, int msg);
int ME_SetVBDR              (HWND hwnd, int msg);
int ME_SetAudioMode         (HWND hwnd, int msg, BOOL bEarPhone);
int ME_PhonebookReady       (HWND hwnd, int msg);

#endif
int ME_MuxPowerUp();

#ifdef __cplusplus
};
#endif

#endif
