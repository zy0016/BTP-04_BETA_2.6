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

#ifndef _HMSMSE_H_
#define _HMSMSE_H_

/* Function Call Return Value */
#define SMSE_OK         0
#define SMSE_E_FAILURE  -1

/* SMSE Read_Write Method */
#define SMSE_RW_RDONLY  1   // Read Only
#define SMSE_RW_RDWR    2   // Read and Write

/* Short Message Type */
#define SMSE_TP_SEND        1   //Short Message which will be sent
#define SMSE_TP_RECEIVE     2   //Short Message which has been received
#define SMSE_TP_REPORT      3   //Status Report

/* SMS Coding Scheme */
#define SMSE_CD_GSM         1   //GSM coding
#define SMSE_CD_OCTETS      2   //8 bit data coding
#define SMSE_CD_UCS2        3   //UCS2 coding

/* Short Message Flag */
#define SMSE_HF_UDH         0x0001  //Has UDH Head
#define SMSE_HF_UDHSIG      0x0002  //UDH only be added to the first Fragment

/* Short Message Head Info Struct Define */
typedef struct tagSMS_HeadInfo
{
    char type;
    char coding;
    unsigned short unused;
    unsigned long flags;    
    unsigned long time;
    unsigned char ctnum[16];
    unsigned char number[4];
}SMS_HeadInfo;

/* Send Option Struct Define */
typedef struct tagSMS_SendOptionInfo
{
    unsigned char ctnum[16];
    unsigned char ifreport;
    unsigned char ifreplypath;
    unsigned char ifrd;
    unsigned char pid;
    unsigned long valid;
}SMS_SendOptionInfo;

/* Folder Name Define */
#define SMSE_NM_INBOX            ((unsigned char* )"INBOX")
#define SMSE_NM_OUTBOX           ((unsigned char* )"OUTBOX")
#define SMSE_NM_SIMBOX           ((unsigned char* )"SIMBOX")
#define SMSE_NM_ESPBOX           ((unsigned char* )"ESPBOX")
#define SMSE_NM_SENDBOX          ((unsigned char* )"SENDBOX")
#define SMSE_NM_DRAFTBOX         ((unsigned char* )"DRAFTBOX")
#define SMSE_NM_REPORTBOX        ((unsigned char* )"REPORTBOX")
#define SMSE_NM_UDFBOX           ((unsigned char* )"UDFBOX")

/* Folder Type Define */
#define SMSE_FT_INVALID          0   //Invalid
#define SMSE_FT_INBOX            1   //INBOX
#define SMSE_FT_OUTBOX           2   //OUTBOX
#define SMSE_FT_SIMBOX           3   //SIMBOX
#define SMSE_FT_ESPBOX           4   //ESPBOX
#define SMSE_FT_SENDBOX          5   //SENDBOX
#define SMSE_FT_DRAFTBOX         6   //DRAFTBOX
#define SMSE_FT_REPORTBOX        7   //REPORTBOX
#define SMSE_FT_UDFBOX           8   //User Define Folder 
#define SMSE_FT_MAXNUM           9   //Max

#define SMS_FOLDNAMELEN     64      //byte

/* Folder Sum Info Struct Define (ListFolder) */
typedef struct tagSMS_FolderSum
{
    unsigned char foldername[SMS_FOLDNAMELEN];       
    unsigned long smsnum;                            
    unsigned long types;                             
}SMS_FolderSum;

/* Folder Info Struct Define (GetFolderInfo) */
typedef struct tagSMS_FolderInfo
{
    unsigned short foldID;          //folder ID
    unsigned short smsnum;          //sms number  
    unsigned long flags;            //folder flag 
    int memlen;                     //memory length of this Folder relevant
    unsigned short unreadsmsnum;    //unread sms number
    unsigned short smsmax;          //max sms number allowed in this folder
}SMS_FolderInfo;


#define SMSE_MAXREADLEN         32      //byte
#define SMSE_MAXFRAMENT         5       //piece (the max piece which the SMSE support)
#define SMSE_DF_STORAGE       0x00000001  
#define SMSE_DF_STATUS        0x00000002 

/* Short Message Info Struct Define (List&GetListInfo) */
typedef struct tagSMS_DataInfo
{
    unsigned long types;        //Msg flag Info(include storage location, read/unread status etc.)
	                            //the high two bytes are unused, the higher byte of the two low bytes express the msg type
                                //the lower byte of the two low bytes express the read/unread status and storage location
    unsigned long times;        //time info relevant to this short message
	unsigned char ctnum[18];    //sms center number
    unsigned short MsgID;       //Msg ID to Indentify a Short Message uniquely
    unsigned short Datalen;     //msg data length
    unsigned short readlen;     //the summary data length saved in the pData field in this struct
    unsigned char pData[SMSE_MAXREADLEN];     //the summary data of the msg
	unsigned long numlength;    //the length of the sender's or receiver's number
	unsigned char number[4];    //the number of the sender or receiver
}SMS_DataInfo;

/* No-Store Short Message Info Struct Define */
typedef struct tagSMS_NoStoreInfo
{
    unsigned long times;        //receive time
    unsigned char ctnum[18];    //sms center number
    unsigned short Datalen;     //Msg data length in bytes
    unsigned char pData[164];   //Msg Data 
    unsigned char number[20];   //sender's number
}SMS_NoStoreInfo;

/* Registry Info Struct Define */
typedef struct tagSMS_RegInfo
{
    unsigned char ctnum[20];
	unsigned long valid;
	unsigned char ifreport;
	unsigned char ifinsend;
	unsigned char ifautoresend;
    unsigned char ifgprs;
    unsigned char ifreplypath;
    unsigned char ifrd;
    unsigned char pid;
	unsigned char unused;
}SMS_RegInfo;

/* PID Type Define */
#define SMSE_PID_IMPLICIT   0
#define SMSE_PID_TELEX      1
#define SMSE_PID_TELEFAX    2
#define SMSE_PID_VOICE      3
#define SMSE_PID_ERMES      4
#define SMSE_PID_NPAGING    5
#define SMSE_PID_X400       6
#define SMSE_PID_EMAIL      7

/* Message Define */
/* Once User call the SetNotify, when there is Avaliable and then the relevant Msg 
   will be transmit to the call entity, the wParam of the Msg is the Handle to the
   SMSE, the lowest byte of lParam express the event, other bytes express the info
   of that event in detail*/
#define SMSE_MG_READ           0x0001  // New SM(Short Message) Arrive Msg                                      

#define SMSE_MG_CHANGE         0x0002  // SM status change Msg

#define SMSE_MG_DELETE         0x0004  // SM delete Msg

#define SMSE_MG_FULL           0x0008  // Folder Full Msg

#define SMSE_FOLDER_CHANGE     0x0010  // Folder Status Change Msg

#define SMSE_NOSTORE_READ      0x0020  // No-Store SM Arrive Msg         

#define SMSE_MG_QUIT           0x0040  // SMSE exit Msg

/* Function Return Error Code */
#define SMSE_E_MEMLOW       -100
#define SMSE_E_NETWORK      -101
#define SMSE_E_USERREQ      -102
#define SMSE_E_BUFFER       -104
#define SMSE_E_UNSUPPORT    -105
#define SMSE_E_BOXFULL      -106
#define SMSE_E_FILE         -107
#define SMSE_E_NOTFOUND     -108
#define SMSE_E_CONFLICT     -109
#define SMSE_E_INVALID      -110
#define SMSE_E_PARA         -111
#define SMSE_E_INVALIDCB    -112
#define SMSE_E_NOSIM        -113

/* Public Interface Declaration of the SMSE */
int SMSE_Init();
int SMSE_Dest();
void* SMSE_Open( int rw_mode );
int SMSE_Close( void* pSMSE );
void SMSE_SetNotify( void* pSMSE, long mask, void* hWnd, unsigned long msg );
int SMSE_ListFolder( void* pSMSE, int index, void* pbuf, int buflen );
int SMSE_GetFolderInfo( void* pSMSE, const char* name, void* pBuf, int buflen );
int SMSE_CreateFolder( void* pSMSE, const char* name, int type );
int SMSE_RemoveFolder( void* pSMSE, const char* pname, int force );
int SMSE_ClearFolder( void* pSMSE, const char* name );
int SMSE_SelectFolder( void* pSMSE, const char* name );
int SMSE_Delete( void* pSMSE, int msg_id );
int SMSE_Save(void* pSMSE, unsigned long msg_id );
int SMSE_List( void* pSMSE, int index, void* buf, int buflen );
int SMSE_Move( void* pSMSE, int msg_id, const char* folder );
int SMSE_Copy( void* pSMSE, int msg_id, const char* folder );
int SMSE_GetListInfo( void* pSMSE, int msg_id, void* buf, int buflen );
int SMSE_Create( void* pSMSE, const char* name, const char* class );
int SMSE_GetHead( void* pSMSE, int msg_id, char* pBuf, int buflen );
int SMSE_ReadData( void* pSMSE, int msg_id, int offset, char* pBuf, int buflen );
int SMSE_ReadStatus( void* pSMSE, int msg_id, char* pBuf, int buflen );
int SMSE_SetHead( void* pSMSE, int msg_id, char* pBuf, int datalen );
int SMSE_WriteData( void* pSMSE, int msg_id, int offset, char* pBuf, int datalen );
int SMSE_Send( void* pSMSE, unsigned long msg_id, int mode, SMS_SendOptionInfo* pSendOption );
int SMSE_GetReg( void* pSMSE, SMS_RegInfo* pRegInfo);
int SMSE_SetReg( void* pSMSE, SMS_RegInfo* pRegInfo);
int SMSE_SetFilter( void* pSMSE, unsigned short lport );

#endif

