/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : mmse.h
 *
 * Purpose  : 彩信引擎常量、数据结构定义及函数声明
 *            
\**************************************************************************/

#ifndef __HMMMSE_H
#define __HMMMSE_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _C166
    #define  _HUGE _huge
#else
    #define  _HUGE
#endif
// 以下两个即是函数返回值也做为消息的错误码
#define MMSE_SUCCESS        0   // 成功
#define MMSE_FAILURE        -1  // 失败

/* 定义错误码 */
#define MMSE_NOT_INIT       -6  // 没有调用初始化函数
#define MMSE_NOT_PROXY      -7  // 没有设置代理
#define MMSE_NOT_CONF       -8  // 发送的消息没有得到对方的确认
#define MMSE_NOT_HANDLE     -9  // 不是有效的 HMMMS 名柄
#define MMSE_INPROGRESS     -10 // 操作正在进行中
#define MMSE_INVALID_MSGID  -11 // 无效彩信ID
#define MMSE_BUFFER_ERR     -12 // 缓冲区不够
#define MMSE_NOT_CONNECT    -13 // 没有连接到代理
#define MMSE_SYS_ERROR      -14 // 调用系统函数出错
#define MMSE_NOT_SUPPORT    -15 // 不支持此选项
#define MMSE_NOT_GRANT      -16 // 不允许此项操作
#define MMSE_NOT_END        -17 // 写数据没有结束
#define MMSE_INVALID_PARAM  -18 // 无效参数
#define MMSE_EXPIRY_MM      -19 // 过期的彩信
#define MMSE_NO_SPACE       -20 // 空间不足
#define MMSE_MM_SIZE_ERROR  -21 // 彩信大于设定值
#define MMSE_FILE_RW_ERROR  -22 // 文件读写失败
#define MMSE_FILE_TYPE_ERROR -23 // 无效的文件类型
#define MMSE_FOLDER_EXIST	 -24 // 文件夹已经存在
#define MMSE_FOLDER_NEXIST	 -25 // 文件夹不存在

// 打开彩信引擎的方式 /
#define MMSE_ACCESS_READ        0x01 // 只读
#define MMSE_ACCESS_WRITE       0x02 // 只写

#define MMSE_MAX_FNAME       256 // 文件名的最大长度

#define MMSE_MAX_TO              256

// 消息定义: LOWORD(lParam)
// wParam = nMsgID
// LOWORD(lParam) = Old Type
// HIWORD(lParam0 = New Type
// 彩信类型

#define MMT_NULL                    0  // 空闲无效状态

#define MMT_UNRECEIVE               1  // 未接收成功的彩信
#define MMT_RECEIVING               2  // 正在接受的彩信
#define MMT_AUTORECEIVING           3
#define MMT_RECEIVEDELAY            4  // 延迟接收的彩信
#define MMT_UNREAD                  5  // 未读的彩信
#define MMT_READED                  6  // 已读的彩信

#define MMT_DRAFT                   7  // 彩信草稿
#define MMT_UNSEND                  8  // 未发送成功的彩信
#define MMT_SENDING                 9  // 正在发送的彩信
#define MMT_SENT                    10  // 已经发送成功的彩信

#define MMT_DELIVERYREPORT          11 // 彩信发送报告，已读
#define MMT_UNREADDELIVERYREPORT    12 // 彩信发送报告，未读

#define MMT_TEMPLATE                13 // 彩信模版

#define MMT_EXPIRY                  14 // 过期彩信
#define MMT_LOWSPACE                15 // 存储空间不足
#define MMT_SIZEOVER                16 // 彩信大于设定值

// 彩信头字段
#define MMHF_BCC                            (const char*)0x01
#define MMHF_CC                             (const char*)0x02
#define MMHF_X_MMS_CONTENT_LOCATION         (const char*)0x03
#define MMHF_CONTENT_TYPE                   (const char*)0x04
#define MMHF_DATE                           (const char*)0x05
#define MMHF_X_MMS_DELIVERY_REPORT          (const char*)0x06
#define MMHF_X_MMS_DELIVERY_TIME            (const char*)0x07
#define MMHF_X_MMS_EXPIRY                   (const char*)0x08
#define MMHF_FROM                           (const char*)0x09
#define MMHF_X_MMS_MESSAGE_CLASS            (const char*)0x0A
#define MMHF_MESSAGE_ID                     (const char*)0x0B
#define MMHF_X_MMS_MESSAGE_TYPE             (const char*)0x0C
#define MMHF_X_MMS_MMS_VERSION              (const char*)0x0D
#define MMHF_X_MMS_MESSAGE_SIZE             (const char*)0x0E
#define MMHF_X_MMS_PRIORITY                 (const char*)0x0F
#define MMHF_X_MMS_READ_REPORT              (const char*)0x10
#define MMHF_X_MMS_REPORT_ALLOWED           (const char*)0x11
#define MMHF_X_MMS_RESPONSE_STATUS          (const char*)0x12
#define MMHF_X_MMS_RESPONSE_TEXT            (const char*)0x13
#define MMHF_X_MMS_SENDER_VISIBILITY        (const char*)0x14
#define MMHF_X_MMS_STATUS                   (const char*)0x15
#define MMHF_SUBJECT                        (const char*)0x16
#define MMHF_TO                             (const char*)0x17
#define MMHF_X_MMS_TRANSACTION_ID           (const char*)0x18
#define MMHF_X_MMS_RETRIEVE_STATUS          (const char*)0x19
#define MMHF_X_MMS_RETRIEVE_TEXT            (const char*)0x1A
#define MMHF_X_MMS_READ_STATUS              (const char*)0x1B
#define MMHF_X_MMS_REPLY_CHARGING           (const char*)0x1C
#define MMHF_X_MMS_REPLY_CHARGING_DEADLINE  (const char*)0x1D

// 发送选项
#define MMSE_SEND_DEFAULT   0   // 按全局的设置发送
#define MMSE_SEND_SAVE      1   // 发送成功后，彩信保存到已发送箱
#define MMSE_SEND_NOSAVE    2   // 发送成功后，不保存彩信

// 文件夹
#define MMSE_FOLDER_INBOX    (const char*)1     // 收件箱
#define MMSE_FOLDER_OUTBOX   (const char*)2     // 发件箱
#define MMSE_FOLDER_DRAFT    (const char*)3     // 草稿箱
#define MMSE_FOLDER_SENT     (const char*)4     // 已发送箱
#define MMSE_FOLDER_TEMPLATE (const char*)5     // 模版箱

typedef struct tagMMSETIME 
{
    unsigned short year;     
    unsigned short month; 
    unsigned short day;     
    unsigned short hour;     
    unsigned short minute; 
    unsigned short second;     
} MMSETIME, *PMMSETIME;

#define MAX_MSG_INFO_SIZE   1024

// MMSE_ListMessage, MMSE_GetListInfo 使用的结构
typedef struct tagMMSEMSGINFO
{
    long        MsgID;          // 信息ID号
    int         status;         // 状态
    long        length;         // 彩信文件大小
    char*       pSubject;       // 标题
    char*       pSender;        // 发送者或接收者
    MMSETIME    time;           // 发送/接受时间
    MMSETIME    ExpiryTime;     // 有效时间
} MMSEMSGINFO, *PMMSEMSGINFO;

// MMSE_ListFolder MMSE_GetFolderInfo使用的结构
typedef struct tagMMFolderInfo
{
    char    FolderName[MMSE_MAX_FNAME];    // 文件夹名字
    int     UnreadCount;                   // 未读彩信个数
	int     UnreceivedCount;               // 未接收彩信个数
	int     AutoreceiveCount;              // 正在自动接收彩信个数
	int     TotalCount;                    // 所有彩信个数
    long    size;                           // 文件夹的大小
}MMFOLDERINFO, *PMMFOLDERINFO;

// MIME content 对象

typedef struct tagMMCONTENT
{
    long    reserved[5];
    char*   content_ID;
    char*   content_type;
    char*   content_location;
    long    length;
    void _HUGE * pData;
    long    (*read)(void* param, void _HUGE* pBuf, long buflen);
    void*   param;
} MMCONTENT, *PMMCONTENT;


// Delivery Report
typedef struct tagMMDELIVERYREPORT
{
    char     to[MMSE_MAX_TO];
    MMSETIME date;
    int      status;
	int      bRead;
    long     ReportID;
} MMDELIVERYREPORT, *PMMDELIVERYREPORT;

/**************************************************************
 *              MMSE 提供的公开函数声明
 **************************************************************/

void*   MMSE_Open(int nMode);
void    MMSE_Close(void* pMmse);
void    MMSE_SetNotify(void* pMmse, void* hWnd, unsigned int uMsg);
int     MMSE_ChangeMode(void* pMmse, int nMode);

int     MMSE_ListFolder(void* pMmse, int nIndex, PMMFOLDERINFO pBuf, int nCount);
int     MMSE_GetFolderInfo(void* pMmse, const char* pName, 
                           PMMFOLDERINFO pFolderInfo);
int     MMSE_CreateFolder(void* pMmse, const char* pName, int nType);
int		MMSE_RenameFolder(void* pMmse, const char* pOldName, const char* pNewName);
int     MMSE_RemoveFolder(void* pMmse, const char* pName, int nForce);
int     MMSE_ClearFolder(void* pMmse, const char* pName);
int     MMSE_SelectFolder(void* pMmse, const char* pName);

int     MMSE_ListMessage(void* pMmse, int nIndex, void* pBuf, int BufLen);
long    MMSE_MoveMessage(void* pMmse, long nMsgId, const char* pFolder);
long    MMSE_CopyMessage(void* pMmse, long nMsgId, const char* pFolder);
int     MMSE_DeleteMessage(void* pMmse, long nMsgId);
int     MMSE_GetMessageStatus(void* pMmse, long nMsgId);
int     MMSE_GetMessageInfo(void* pMmse, long nMsgId, void* pBuf, int BufLen);

int     MMSE_SendMessage(void* pMmse, long nMsgId, int nMode, int nFlag);
int     MMSE_ReceiveMessage(void* pMmse, long nMsgId);
int		MMSE_Cancel(void* pMmse);

void*   MMSE_CreateMessage(void* pMmse, long nMsgId);
void*   MMSE_OpenMessage(void* pMmse, long nMsgId);

int     MMSE_FlushMessage(void* pMmse, void* pMessage);
long    MMSE_SaveMessage(void* pMmse, void* pMessage, int bTemplate);
int     MMSE_CloseMessage(void* pMmse, void* pMessage);

const PMMCONTENT MMSE_GetContent(void* pMmse, void* pMessage, int nIndex);
const PMMCONTENT MMSE_GetContentByID(void* pMmse, void* pMessage, const char* id);

int     MMSE_AddContent(void* pMmse, void* pMessage, PMMCONTENT pMMContent);
int     MMSE_DeleteContent(void* pMmse, void* pMessage, int nIndex);
int     MMSE_DeleteContentByID(void* pMmse, void* pMessage, const char* id);

const char* MMSE_GetHeaderField(void* pMmse, void* pMessage, const char* pField);
int     MMSE_SetHeaderField(void* pMmse, void* pMessage, const char* pField, 
                            const char* value);

// X-Mms-Status value Meaning in delivery report

// The MM in question reached the recipient MMS Proxy-Relay but the
// recipient MMS Client did not retrieve it before expiration.
#define X_MMS_STATUS_EXPIRED        0

// The MM in question was successfully retrieved by the recipient MMS
// Client.
#define X_MMS_STATUS_RETRIEVED      1

// The MM in question reached the recipient MMS Proxy-Relay but the
// recipient MMS Client rejected it.
#define X_MMS_STATUS_REJECTED       2

// The MM in question reached the recipient MMS Proxy-Relay but the
// recipient MMS Client decided to retrieve it at a later time.
#define X_MMS_STATUS_DEFERRED       3

// It is not possible to determine if the MM in question reached its
// destination.
#define X_MMS_STATUS_INDETERMINATE  4

// The MM in question reached the recipient MMS Proxy-Relay but the
// recipient MMS Client forwarded it without retrieving it first.
#define X_MMS_STATUS_FORWARDED      5

// The recipient MMS Client is not reachable due to MMS Proxy-Relay
// interworking, routing failure or some other condition that prevents
// the MM to reach it’s final destination.
#define X_MMS_STATUS_UNREACHABLE    6

int     MMSE_ListDeliveryReport(void* pMmse, int nIndex, PMMDELIVERYREPORT pBuf, 
                                int BufLen, int nCount);
int     MMSE_ReadDeliveryReport(void* pMmse, long nReportID ,PMMDELIVERYREPORT pBuf);

// id == -1, delete all delivery reports
int     MMSE_DeleteDeliveryReport(void* pMmse, long nReportID);

// MMSE option mask
#define MMOM_ALL                    0xFFFF
#define MMOM_MMSC                   0x0001
#define MMOM_GATEWAY                0x0002
#define MMOM_PORT                   0x0004
#define MMOM_EXPIRE                 0x0008
#define MMOM_SENDFLAG               0x0010
#define MMOM_RECEIVEFLAG            0x0020
#define MMOM_MAXNUM                 0x0040
#define MMOM_MAXSIZE                0x0080

// 发送选项
#define MMSF_DELIVERYREPORT         0x0001      // 是否要求发送报告：否
#define MMSF_READREPORT             0x0002      // 是否要求与读报告：否
#define MMSF_NORMALPRIORITY         0x0004      // 普通优先级
#define MMSF_LOWPRIORITY            0x0008      // 低优先级
#define MMSF_HIGHPRIORITY           0x0010      // 高优先级

// 接收选项
#define MMRF_RECEIVE                0x0001      // 允许接收：是
#define MMRF_AUTORETRIEVE           0x0002      // 立即提取：是
#define MMRF_DELIVERYNOTIFICATION   0x0004      // 允许发送报告：否
#define MMRF_READNOTIFICATION       0x0008      // 允许发送阅读发送报告：否
#define MMRF_RECEIVEADVERT          0x0010      // 接收广告
#define MMRF_RECEIVEANONYMOUS       0x0020      // 接收匿名信息

#define MMSE_MAX_MMSC               64
#define MMSE_MAX_IP                 16

typedef struct tagMMSEOPTION
{
    unsigned short  mask;
    
    // 连接设置
    char            mmsc[MMSE_MAX_MMSC];
    char            gateway[MMSE_MAX_IP];
    unsigned short  port;
    
    // 发送参数
    unsigned short  expire;             // 有效期，以小时为单位
    unsigned short  SendFlag;           // 发送选项
    
    // 接收参数
    unsigned short  ReceiveFlag;        // 接收选项
    unsigned short  MaxSize;            // 接收彩信大小，以k为单位
    
    // 限制参数，=0表示没有限制
    unsigned short  MaxNum;             // 彩信的最大数目
} MMSEOPTION, *PMMSEOPTION;

int MMSE_GetOption(void* pMmse ,PMMSEOPTION pOption);
int MMSE_SetOption(void* pMmse ,const PMMSEOPTION pOption);

int MMSE_AddFilterAddress(void* pMmse ,const char* pAddr);
//pAddr = NULL ,全部删除.其余，删除匹配地址
int MMSE_DeleteFilterAddress(void* pMmse ,const char* pAddr);
//pBuf = NULL,返回所需缓冲区大小。
//filter address以 ; 分隔 。pBuf结束没有0，函数返回pBuf中有效字节个数
int MMSE_GetFilterAddress(void* pMmse ,char* pBuf ,int nBufLen);

#ifdef __cplusplus
}
#endif

#endif  //HMMMSE_H_
