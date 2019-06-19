/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : mmse.h
 *
 * Purpose  : �������泣�������ݽṹ���弰��������
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
// �����������Ǻ�������ֵҲ��Ϊ��Ϣ�Ĵ�����
#define MMSE_SUCCESS        0   // �ɹ�
#define MMSE_FAILURE        -1  // ʧ��

/* ��������� */
#define MMSE_NOT_INIT       -6  // û�е��ó�ʼ������
#define MMSE_NOT_PROXY      -7  // û�����ô���
#define MMSE_NOT_CONF       -8  // ���͵���Ϣû�еõ��Է���ȷ��
#define MMSE_NOT_HANDLE     -9  // ������Ч�� HMMMS ����
#define MMSE_INPROGRESS     -10 // �������ڽ�����
#define MMSE_INVALID_MSGID  -11 // ��Ч����ID
#define MMSE_BUFFER_ERR     -12 // ����������
#define MMSE_NOT_CONNECT    -13 // û�����ӵ�����
#define MMSE_SYS_ERROR      -14 // ����ϵͳ��������
#define MMSE_NOT_SUPPORT    -15 // ��֧�ִ�ѡ��
#define MMSE_NOT_GRANT      -16 // ������������
#define MMSE_NOT_END        -17 // д����û�н���
#define MMSE_INVALID_PARAM  -18 // ��Ч����
#define MMSE_EXPIRY_MM      -19 // ���ڵĲ���
#define MMSE_NO_SPACE       -20 // �ռ䲻��
#define MMSE_MM_SIZE_ERROR  -21 // ���Ŵ����趨ֵ
#define MMSE_FILE_RW_ERROR  -22 // �ļ���дʧ��
#define MMSE_FILE_TYPE_ERROR -23 // ��Ч���ļ�����
#define MMSE_FOLDER_EXIST	 -24 // �ļ����Ѿ�����
#define MMSE_FOLDER_NEXIST	 -25 // �ļ��в�����

// �򿪲�������ķ�ʽ /
#define MMSE_ACCESS_READ        0x01 // ֻ��
#define MMSE_ACCESS_WRITE       0x02 // ֻд

#define MMSE_MAX_FNAME       256 // �ļ�������󳤶�

#define MMSE_MAX_TO              256

// ��Ϣ����: LOWORD(lParam)
// wParam = nMsgID
// LOWORD(lParam) = Old Type
// HIWORD(lParam0 = New Type
// ��������

#define MMT_NULL                    0  // ������Ч״̬

#define MMT_UNRECEIVE               1  // δ���ճɹ��Ĳ���
#define MMT_RECEIVING               2  // ���ڽ��ܵĲ���
#define MMT_AUTORECEIVING           3
#define MMT_RECEIVEDELAY            4  // �ӳٽ��յĲ���
#define MMT_UNREAD                  5  // δ���Ĳ���
#define MMT_READED                  6  // �Ѷ��Ĳ���

#define MMT_DRAFT                   7  // ���Ųݸ�
#define MMT_UNSEND                  8  // δ���ͳɹ��Ĳ���
#define MMT_SENDING                 9  // ���ڷ��͵Ĳ���
#define MMT_SENT                    10  // �Ѿ����ͳɹ��Ĳ���

#define MMT_DELIVERYREPORT          11 // ���ŷ��ͱ��棬�Ѷ�
#define MMT_UNREADDELIVERYREPORT    12 // ���ŷ��ͱ��棬δ��

#define MMT_TEMPLATE                13 // ����ģ��

#define MMT_EXPIRY                  14 // ���ڲ���
#define MMT_LOWSPACE                15 // �洢�ռ䲻��
#define MMT_SIZEOVER                16 // ���Ŵ����趨ֵ

// ����ͷ�ֶ�
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

// ����ѡ��
#define MMSE_SEND_DEFAULT   0   // ��ȫ�ֵ����÷���
#define MMSE_SEND_SAVE      1   // ���ͳɹ��󣬲��ű��浽�ѷ�����
#define MMSE_SEND_NOSAVE    2   // ���ͳɹ��󣬲��������

// �ļ���
#define MMSE_FOLDER_INBOX    (const char*)1     // �ռ���
#define MMSE_FOLDER_OUTBOX   (const char*)2     // ������
#define MMSE_FOLDER_DRAFT    (const char*)3     // �ݸ���
#define MMSE_FOLDER_SENT     (const char*)4     // �ѷ�����
#define MMSE_FOLDER_TEMPLATE (const char*)5     // ģ����

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

// MMSE_ListMessage, MMSE_GetListInfo ʹ�õĽṹ
typedef struct tagMMSEMSGINFO
{
    long        MsgID;          // ��ϢID��
    int         status;         // ״̬
    long        length;         // �����ļ���С
    char*       pSubject;       // ����
    char*       pSender;        // �����߻������
    MMSETIME    time;           // ����/����ʱ��
    MMSETIME    ExpiryTime;     // ��Чʱ��
} MMSEMSGINFO, *PMMSEMSGINFO;

// MMSE_ListFolder MMSE_GetFolderInfoʹ�õĽṹ
typedef struct tagMMFolderInfo
{
    char    FolderName[MMSE_MAX_FNAME];    // �ļ�������
    int     UnreadCount;                   // δ�����Ÿ���
	int     UnreceivedCount;               // δ���ղ��Ÿ���
	int     AutoreceiveCount;              // �����Զ����ղ��Ÿ���
	int     TotalCount;                    // ���в��Ÿ���
    long    size;                           // �ļ��еĴ�С
}MMFOLDERINFO, *PMMFOLDERINFO;

// MIME content ����

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
 *              MMSE �ṩ�Ĺ�����������
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
// the MM to reach it��s final destination.
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

// ����ѡ��
#define MMSF_DELIVERYREPORT         0x0001      // �Ƿ�Ҫ���ͱ��棺��
#define MMSF_READREPORT             0x0002      // �Ƿ�Ҫ��������棺��
#define MMSF_NORMALPRIORITY         0x0004      // ��ͨ���ȼ�
#define MMSF_LOWPRIORITY            0x0008      // �����ȼ�
#define MMSF_HIGHPRIORITY           0x0010      // �����ȼ�

// ����ѡ��
#define MMRF_RECEIVE                0x0001      // ������գ���
#define MMRF_AUTORETRIEVE           0x0002      // ������ȡ����
#define MMRF_DELIVERYNOTIFICATION   0x0004      // �����ͱ��棺��
#define MMRF_READNOTIFICATION       0x0008      // �������Ķ����ͱ��棺��
#define MMRF_RECEIVEADVERT          0x0010      // ���չ��
#define MMRF_RECEIVEANONYMOUS       0x0020      // ����������Ϣ

#define MMSE_MAX_MMSC               64
#define MMSE_MAX_IP                 16

typedef struct tagMMSEOPTION
{
    unsigned short  mask;
    
    // ��������
    char            mmsc[MMSE_MAX_MMSC];
    char            gateway[MMSE_MAX_IP];
    unsigned short  port;
    
    // ���Ͳ���
    unsigned short  expire;             // ��Ч�ڣ���СʱΪ��λ
    unsigned short  SendFlag;           // ����ѡ��
    
    // ���ղ���
    unsigned short  ReceiveFlag;        // ����ѡ��
    unsigned short  MaxSize;            // ���ղ��Ŵ�С����kΪ��λ
    
    // ���Ʋ�����=0��ʾû������
    unsigned short  MaxNum;             // ���ŵ������Ŀ
} MMSEOPTION, *PMMSEOPTION;

int MMSE_GetOption(void* pMmse ,PMMSEOPTION pOption);
int MMSE_SetOption(void* pMmse ,const PMMSEOPTION pOption);

int MMSE_AddFilterAddress(void* pMmse ,const char* pAddr);
//pAddr = NULL ,ȫ��ɾ��.���࣬ɾ��ƥ���ַ
int MMSE_DeleteFilterAddress(void* pMmse ,const char* pAddr);
//pBuf = NULL,�������軺������С��
//filter address�� ; �ָ� ��pBuf����û��0����������pBuf����Ч�ֽڸ���
int MMSE_GetFilterAddress(void* pMmse ,char* pBuf ,int nBufLen);

#ifdef __cplusplus
}
#endif

#endif  //HMMMSE_H_
