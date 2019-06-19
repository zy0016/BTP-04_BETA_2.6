#ifndef _SETTING_H
#define _SETTING_H

#include "window.h"
#include "plxmm.h"
#include "prioman.h"
#include "me_wnd.h"
#ifndef _863BIRD_
#define _863BIRD_       // bird��Ŀ�궨��
#endif

short APP_GetSIMRcdByOder ( short iOrder, PPHONEBOOK pRtnRcd, short MaxNum );
BOOL  Setup_InitFunction(void);//�ṩ�����������,���ڳ�ʼ�����ø�������

#define     SN_GETBARCOLOR    0x01
#define     SN_GETWINCOLOR    0x02
#define     SN_GETFOCUSCOLOR  0x03
COLORREF    SetupGetDeskColor(int iIndex);//��õ�ǰ������ɫֵ

//*********************************��ǰ��񲿷�**************************
#define		STYLE_COM					1//������
#define		STYLE_PASTIME				0//���ַ��

/**********��ȡ��������******************************************/
#define     DIALDEF_WAP      1
#define     DIALDEF_MAIL     2
#define     DIALDEF_MMS      3
/*���ŵ�UDB������Ϣ*/
#define     DIALFILE              "/mnt/flash/setup/DialCfg.mal"
#define     SN_CONNSETT           "Sett_ConnInfo"
#define     KN_CONNID             "Sett_ConnID"



#define     UDB_K_DNS_SIZE        31
#define     UDB_K_NAME_SIZE       31
#define     UDB_K_PWD_SIZE        31
#define     UDB_K_TEL_SIZE        41
#define     UDB_K_PORT_SIZE       4
#define     UDB_HOMEPAGE_SIZE     1023
/* UDB���ݽṹ */
typedef struct
{
    unsigned long       ISPID;                      /*  ISP��ID��           */
    unsigned long       PrimarySelect;              /*  �Ƿ���ѡISP         */
    unsigned long       ProxyFlag;                  /*  �Ƿ�ʹ�ô��������  */
    unsigned long       ProxyID;                    /*  ���������ID            */
    unsigned long       DtType;                     /*  ��������GPRS/CSD    GPRS: 1, CSD: 0*/

    char    ISPName     [ UDB_K_NAME_SIZE + 1 ];    /*  ISP��               */
    char    ISPUserName [ UDB_K_NAME_SIZE + 1 ];    /*  ISP��¼�û���       */
    char    ISPPassword [ UDB_K_PWD_SIZE  + 1 ];    /*  ISP��¼����         */
    char    PrimaryDns  [ UDB_K_DNS_SIZE  + 1 ];    /*  ������������            */
    char    SlaveDns    [ UDB_K_DNS_SIZE  + 1 ];    /*  ��������������      */

    char    ISPPhoneNum1[ UDB_K_TEL_SIZE+1 ];       /*  ����ISP�ĵ绰����   */
    char    ISPPhoneNum2[ UDB_K_TEL_SIZE+1 ];       /*  ����ISP�ĵ绰����   */
    char    ISPPhoneNum3[ UDB_K_TEL_SIZE+1 ];       /*  ����ISP�ĵ绰����   */
    //char    ISPPhoneNum4[ UDB_K_TEL_SIZE+1 ];       /*  ����ISP�ĵ绰����   */
    
    unsigned int        AuthenType; //0: Secure; 1: Normal
    DWORD               ConnectGateway; //IP address: OxFFFFFFFF; 
    char                ConnectHomepage[UDB_HOMEPAGE_SIZE + 1];
    char                GPRSAccPoint[UDB_K_DNS_SIZE + 1];
    unsigned long       DatacallType; //0: Analogue; 1: ISDN v.110; 2: ISDN v.120
    unsigned long       MaxDataSpeed; //0: Automatic; 1: 9600bps; 2: 14400bps;
    char                ConnPort[UDB_K_PORT_SIZE+1];
}UDB_ISPINFO;

typedef struct tag_ConnIndexTbl
{
	char ConnInfoName[UDB_K_NAME_SIZE + 1];
	int iIndex;
	int iListIndex;
	int nType;//GPRS or CSD
}ConnIndexTbl;

BOOL    Setup_ReadDefDial(UDB_ISPINFO  * ,int );//�ṩ������,ֱ�Ӷ�ȡĬ������
void    SetUp_SetDialCfg(void);                 //�ṩ�����������,��һ��ʱ�������������������ļ�
int     Setup_GetDialType(int nType);           //�õ�Ĭ�ϲ�������
BOOL    SetConnectionUsage(int nType, int nIndex);

/**************************************************************/
BOOL    Setup_ShowPenCalib();//���ñ�У׼
//**********************************ʱ���ʽ*************************************************
#define     SN_TIMEMODE                 "timesetting"

#define     KN_TIMEMODE                 "timemodeID"
#define     KN_DATEMODE                 "datemodeID"
#define     KN_TIMESRC                  "timesrcID"


typedef enum
{
    TF_12,  //��ǰΪ12Сʱ��
    TF_24   //��ǰΪ24Сʱ��
}TIMEFORMAT;
TIMEFORMAT      GetTimeFormt(void);//����0:��ǰΪ24Сʱ��,1:��ǰΪ12Сʱ��

typedef enum
{
	DF_DMY_DOT,//dd.mm.yyyy 
	DF_MDY_DOT,//mm.dd.yyyy
	DF_YMD_DOT,//yyyy.mm.dd 
	DF_DMY_SLD,//dd/mm/yyyy 
	DF_MDY_SLD,//mm/dd/yyyy 
	DF_YMD_SLD,//yyyy/mm/dd 
	DF_DMY_DSH,//dd-mm-yyyy
	DF_MDY_DSH,//mm-dd-yyyy 
	DF_YMD_DSH //yyyy-mm-dd
}DATEFORMAT;
DATEFORMAT      GetDateFormt(void);

typedef enum
{
    GPS_BASED,
    USER_DEFINED,
}TIMESOURCE;
TIMESOURCE   GetTimeSource(void);

BOOL GetTimeDisplay(SYSTEMTIME st, char* cTime, char* cDate);

typedef enum
{
    TZ_HOME,
    TZ_VISIT
}TIMEZONE;

TIMEZONE GetUsingTimeZone(void);//ȡ�õ�ǰʹ�õ�ʱ��


#define SN_TIMEZONE "timezone"
#define KN_TIMEZONE "timezoneID"
#define KN_HZSTRING "homezoneID"
#define KN_VZSTRING "visitzoneID"

//************************************�����������貿��**************************************************
typedef enum
{
    PWSV_LOCKING,//���ò�������������ʱ���ֻ�������֤
    PWSV_STARTUP //�����ֻ�ʱ���ֻ�������֤          
}PASSWORD_VALIDATE;
BOOL    CallPwsValidate(HWND hPasswordValidate,PASSWORD_VALIDATE itype);//itype:��ǰ��Ҫ��֤����������
//��������ʾ������֤�Ĵ���,��������ʾһ��EDIT�ؼ���"ȷ��","ȡ��"��ť,��ʾ�û������ֻ�����.
//itype:��ǰ��Ҫ��֤����������
//�����ڿ���ʱҪ��֤�ֻ�����,ֻ��ֱ�ӵ�����������Ϳ�����:CallPwsValidate(hwnd,PWSV_STARTUP),
//�����ǰ������������Ϊ�ر�,�������Զ��˳�,������ʾ��������,�������ʾ��������,����������ȷ֮��,�Ż��˳�����.

#define     PASSWORD_DEFAULT        "0000"    //Default password of phone

//***********************************Unit�������貿��************************************************
#define  SN_UNIT            "UnitSetting"
#define  KN_UNIT_DISTANCE   "DistanceUnitSetting"
#define  KN_UNIT_SPEED      "SpeedUnitSetting"
#define  KN_UNIT_ELEVATION  "ElevationUnitSetting"

typedef enum 
{
    DIS_METERS,       //Meters
    DIS_I_MILES,      //Imperial miles
    DIS_I_YARDS,      //Imperial yards
    DIS_N_MILES       //Nautical miles
}UNIT_DISTANCE;

UNIT_DISTANCE GetUnitDistance(void);

typedef enum
{
    KM_PER_HOUR,        //Kilometers/hour
    METER_PER_SEC,      //Meters/second
    MILES_PER_HOUR,     //Miles/hour
    KNOTS               //Knots
}UNIT_SPEED;

UNIT_SPEED GetUnitSpeed(void);

typedef enum
{
    ELE_METERS,             //Meters
    ELE_I_FEET              //imperial feet
}UNIT_ELEVATION;

UNIT_ELEVATION GetUnitElevation(void);

typedef enum
{
    DIS_UNIT_LIST,
    SPID_UNIT_LIST,
    ELE_UNIT_LIST
}UNIT_TYPE;

//***********************************Unit�������貿��************************************************

//***********************************�������ò���***********************************************
void    GetOpenRing(char *);                //��ÿ������ֵĺ���(char *)����Ϊ:RINGNAMEMAXLEN + 1
void    GetCloseRing(char *);               //��ùػ����ֵĺ���(char *)����Ϊ:RINGNAMEMAXLEN + 1
//***********************************����ѡ��ӿڲ���***********************************************
#define     RINGNAMEMAXLEN          400     //�����ļ�������ĳ���(����·��)
#define     RINGNAMEONLYLEN         100     //�����ļ�������ĳ���(������·��)
#define     CAPTIONMAXLEN           20      //���ڱ�����󳤶�

typedef enum
{
    RINGTYPE_CALL   = 0,//����
    RINGTYPE_SMS    = 1,//��������
    RINGTYPE_MMS    = 2,//��������
    RINGTYPE_OPEN   = 3,//��������
    RINGTYPE_CLOSE  = 4//�ػ�����
}RINGSELECTTYPE;

typedef struct
{
    RINGSELECTTYPE  iRingType;
    HWND            hCallWnd;
    int             hCallMsg;
    char            titlecaption[CAPTIONMAXLEN + 1];
    char            pSelRingname[RINGNAMEMAXLEN + 1];
}FUNCRINGSELECT;
BOOL    CallRingSelect(const FUNCRINGSELECT * ringselect);//��ʾ�����б�
void    GetReplyData(char * ringname,LPARAM lParam ,unsigned int buflen);
/*
CallRingSelect������ʾһ������,��������һ���б��,��ǰROM,FLASH2�е������ļ���ʾ���б���С�
����ROM������,��ʾ���Ӧ����������,����FLASH2�е�����,ֻ��ʾ��ʵ���ļ���.
�û�ѡ�񴰿ڵ�"ȷ��"��ť֮��,�ɻص���Ϣ����ѡ�е�������ʵ���ļ�������·��(��Ҫ����GetReplyData����).
�û�ѡ�񴰿ڵ�"ȡ��"��ť֮��,�ص���Ϣ�е�LPARAM=0
����ִ�гɹ�WPARAM=1
����ִ��ʧ��WPARAM=0
iRingType   :��Ҫ��ʾ�����������,RINGTYPE_CALL:����,RINGTYPE_SMSMMS:����/��������,
                                RINGTYPE_OPEN:��������,RINGTYPE_ALARM:�ճ̱����������
hCallWnd    :�ص�ģ���ָ�����ھ��
hCallMsg    :�ص�ģ���ָ����Ϣ��Ӧ
titlecaption:���ڵı���
pSelRingname:��Ҫѡ���������ļ���

����˵����
���յ�������Ϣ����Ҫ����GetReplyData(char * ringname,LPARAM lParam ,unsigned int buflen)���ؽ����
ringname:����ѡ�е��ļ����ַ���
LPARAM:�ص���Ϣ��lParam
buflen:���÷��������Ҫ���ص��ַ����ĳ���.ע��:û�б߽���.
*/
//**************************************��������/��������************************
#define     DESK_LIMIT      12          //���������ַ�����󳤶�(6������/12��Ӣ��)
#define     SCREEN_LIMIT    12          //���������ַ�����󳤶�(6������/12��Ӣ��)
void    GetDeskPhrase  (char * );       //��ÿ�������ĺ���(char * �ĳ���ӦΪDESK_LIMIT + 1)
void    GetScreenPhrase(char * );       //�����������ĺ���(char * �ĳ���ӦΪSCREEN_LIMIT + 1)
//*********************************�������ò���**************************
#define     CARTOONFILENAMELEN      400
typedef struct
{
    char    cOpen[CARTOONFILENAMELEN + 1];      //���������ļ���
    char    cClose[CARTOONFILENAMELEN + 1];     //�ػ������ļ���
    char    cScreen[CARTOONFILENAMELEN + 1];    //���������ļ���
    char    cCall[CARTOONFILENAMELEN + 1];      //���綯���ļ���
}CARTOON;
//BOOL    GetCartoon(CARTOON * cartoon);//��ö������ýṹ��Ϣ

typedef struct tagMobileConfig
{
    BOOL    bAutoRedial;//�Զ��ز�
    int     iAutoRedial;//�Զ��ز��Ĵ���
    BOOL    bGetClew;   //��ͨ��ʾ
    BOOL    bModeTurn;  //���ǽ���
    BOOL    bModeAnyKey;//�����������
    BOOL    b50Tip;     //50����ʾ��
    BOOL    bModeAuto;  //�Զ�����
    int     ALS;
#define     ALS_LINE1	0
#define     ALS_LINE2	1
#define     MAX_LINENAME 40
	char    Line1Name[MAX_LINENAME];
	char    Line2Name[MAX_LINENAME];
	BOOL    bShowCallDuration;
	BOOL    bShowCallCost;
	BOOL    bCallSummary ;
}MBConfig, *PMBConfig;
BOOL    ReadMobileConfigFile(MBConfig* pConfig);

//********************************��ǰͨѶģʽ***************************
BOOL    CallCommunicateWindow(HWND hwndCall,int icallmsg);
/*
hwndCall:���÷���ָ�����ھ��.
icallmsg:���÷���ָ����Ϣ��Ӧ.
�ú�����ʾһ������,��������ʾ������ѡ��ť,�ֱ�Ϊ"ͨѶģʽ","����ģʽ".�û�ѡ��"ȷ��"��ť֮�󴰿��Զ��ر�
������÷�������Ϣ,��Ϣ�е�wparam = 1,lparam = 1
*/
typedef enum
{
    COMMUNICATION = 0,//ͨѶģʽ
    FLAY          = 1 //����ģʽ
}MESSAGEMODE;
MESSAGEMODE     GetMessageMode(void);//��õ�ǰͨѶģʽ,0:ͨѶģʽ,1:����ģʽ
//��û��SIM��������£��û�����ģʽ����ѡ�����ѡ����ͨѶģʽ��ֻ���Բ�������绰
//********************************����ӿ�����***************************
BOOL    CallInfraredWindow(HWND);//�ṩ�����������,��ʾ����ӿ�����
//***************************************������/Һ����������ʾ��ʱ�����ò���**********************
int     GetBglight(void);                       
//��ñ����Ƶ����ĳ���ʱ��,����0��ʾ"�ر�",-1��ʾ���ر�,�����ʾ����������
int     GetScreen(void);                    
//���Һ����Ļ������ʾ��ʱ��,����0��ʾ"�ر�",-1��ʾ���ر�,�����ʾ����������
int     GetSystemWait(void);
//���ϵͳ�����ĳ���ʱ��,����0��ʾ"�ر�",-1��ʾ���ر�,�����ʾ����������
//*****************************�ṩ������Ӧ��****************************
void    BeginVibration(unsigned int iSecond);//ʵ����iSecond��
void    GetRingShowName(const char * ringname,char * cshowname);
//***************************************��������*****************************
#define AUDIOTYPE 6
typedef enum
{
    RING_PHONE      = 150,//������ʾ��                                             
    RING_ALARM      = 145,//����                                                   
    RING_CALENDAR   = 144,//�ճ�                                                   
    RING_POWNON     = 140,//��������                                               
    RING_POWNOFF    = 130,//�ػ�����                                               
    RING_SMS        = 120,//����                                                   
    RING_MMS        = 110,//����                                                   
    RING_WARNING    = 105,//������                                                 
    RING_CAMERA     = 103,//������ʾ��                                             
    RING_KEY        = 100,//������                                                 
    RING_TOUCH      = 90, //��ͨ��ʾ��                                              
    RING_SCREEN     = 80, //������                                                  
    RING_CLEW50     = 70, //50����ʾ��                                              
    RING_OTHER      = 60, //������������,�������,������е������ļ�,����Ԥ����һ��Ӧ��
    RING_WAP        = 55, //������е������ļ���������
    RING_MMSAPP     = 54, //����Ӧ���е��ļ���������  
    RING_PUBLIC     = 53, //����Ԥ���е��ļ�          
    RING_VOLUME     = 50, //���������ú�                                             
    RING_WITHOUT    = 0   //û����������                                              
}MUSICTYPE;//���������
typedef enum
{
    AMR_TYPE        = 1,//amr�ļ�
    MIDI_TYPE       = 2,//midi�ļ�
    WAVE_TYPE       = 3,//wave�ļ�
    MMF_TYPE        = 4,//mmf�ļ�
    INVALID_TYPE    = 5,//��֧�ֲ��ŵ������ļ�(����������)���
    OPENFAIL        = 6 //�ļ���ʧ��
}FILETYPE;//�����ļ������
typedef enum
{
    PLAY_OVER  = 0x0001,//ֹͣ
    PLAY_BREAK = 0x0002 //�ж�
}PLAYEND_TYPE;//�ص���Ϣ�����

typedef struct              //�������Žṹ
{
    HWND            hWnd;       
    int             iWM_MMWRITE;
    int             iWM_MMSTOP; 
    int             iWM_MMGETDATA;
    int             iVolume;    
    FILETYPE        iType;  
    MUSICTYPE       uiringselect;
    unsigned char * pFileName;
    unsigned char * pBuffer;
    unsigned long   ulBufferSize;
}SetupHS_PlayMusic;
/*
�ṹ˵��
hWnd:       ���÷��Ĵ��ھ��
iWM_MMWRITE:���÷��������Ϣ,���÷������ڸ���Ϣ�е���Setup_WriteMusicData����
            �Ƽ���������:Setup_WriteMusicData((LPWAVEHDR)lParam);
iWM_MMSTOP: ���÷��������Ϣ,����֪ͨ���÷������������.������������ϵ�ʱ��,
            iWM_MMSTOP��Ϣ�е�wParam=PLAY_OVER��ʾ���ֲ������,��������������
            ���÷������ڸ���Ϣ�е���Setup_EndPlayMusic(int uiringselect)�����ر��������豸
            iWM_MMSTOP��Ϣ�е�wParam=PLAY_BREAK��ʾ���ֱ�����Ӧ�ô��,
iWM_MMGETDATA:���÷��������Ϣ,���÷������ڸ���Ϣ�е���Setup_GetMusicData����
            �Ƽ���������:Setup_GetMusicData((LPWAVEHDR)lParam);
iVolume:    ��������
iType:      �����ļ�������,�μ��Ϸ��� FILETYPE
uiringselect:���������,�μ��ϱߵ�MUSICTYPE
pFileName:  �����ļ���
pBuffer:    ���ݴ洢����
ulBufferSize:���ݴ洢����ĳ���
*/
typedef enum
{
    PLAYMUSIC_SUCCESS           ,//���ųɹ�
    PLAYMUSIC_ERROR_PRI         ,//���ȼ�����
    PLAYMUSIC_ERROR_RINGTYPE    ,//�Ƿ����������
    PLAYMUSIC_ERROR_WAVEFORMAT  ,//�Ƿ���������ʽ
    PLAYMUSIC_ERROR_OPENFILE    ,//�ļ���ʧ��
    PLAYMUSIC_ERROR_MALLOC      ,//mallocʧ��
    PLAYMUSIC_ERROR_PARAMETER   ,//�Ƿ���SetupHS_PlayMusic����
    PLAYMUSIC_ERROR_PLAYFUNCTION,//���ò��ź���ʧ��
}PlayMusic_Result;//SETUP_PlayMusic��������ֵ

PlayMusic_Result    SETUP_PlayMusic(SetupHS_PlayMusic * hs);//��������,���ųɹ�����1,ʧ�ܷ���0
/*
�ú����ṩ������Ӧ�ò�������,
������÷����ṩ�����ļ����Ļ�,�ڴ�ΪpFileNameָ�븳ֵ,
ROM�������Ϸ����ļ�����ʽΪ:"/rom/setup/test1.amr"
FLASH�������Ϸ����ļ�����ʽΪ:"FLASH2:/test1/test1.amr"
����setup,test1ΪĿ¼,
�뽫pBufferָ�븳��,ulBufferSize����
������÷��ṩ�������ݴ洢����Ļ�,�뽫pFileNameָ�븳��
pBufferָ�����ݴ洢����,
ulBufferSizeΪ���ݴ洢����ĳ���

��Ҫע�����:ֻ�е�uiringselect=RING_VOLUME��ʱ��,iVolume�Ż���Ч,����Ļ����ŵ������ɵ�ǰ������������С����,
���ⲻ����д����iType,�������������ļ����������ݴ洢���Զ��ж������ļ�����
��������ļ���չ����"tmp",���ļ������ɵ��÷���iType����

������iWM_MMWRITE��Ϣ�е���Setup_WriteMusicData����,
�Ƽ���������:Setup_WriteMusicData((LPWAVEHDR)lParam);
������iWM_MMGETDATA��Ϣ�е���Setup_GetMusicData����
�Ƽ���������:Setup_GetMusicData((LPWAVEHDR)lParam);

iWM_MMSTOP��Ϣ
iWM_MMSTOP��Ϣ�е�wParam=PLAY_OVER��ʾ���ֲ������,��������������
�ڸ���Ϣ�б������Setup_EndPlayMusic(int uiringselect)�����ر��������豸
iWM_MMSTOP��Ϣ�е�wParam=PLAY_BREAK��ʾ���ֱ�����Ӧ�ô��.
*/
//int     Setup_WriteMusicData(LPWAVEHDR lpHdr);  
//���ŵ�ʱ��,��BUffer�ṩ����
//int     Setup_GetMusicData(LPWAVEHDR phdr);
//���ŵ�ʱ��,��BUffer�ṩ����
typedef enum
{
    STOPMUSIC_SUCCESS       ,//ֹͣ�ɹ�   
    STOPMUSIC_ERROR_PRI     ,//���ȼ�����
    STOPMUSIC_ERROR_UNKNOWN ,//δ֪��ʧ��
}EndPlayMusic_Result;

EndPlayMusic_Result     Setup_EndPlayMusic(MUSICTYPE uiringselect);   
//�ṩ������Ӧ�õ�ֹͣ�������ֺ���,���ڹر��豸,uiringselect:��ǰ���������,ȡֵΪ�ϱߵ�RING_SELECT
/*ע��:����������������ϵ�ʱ��,���ڻ��յ�iWM_MMSTOP��Ϣ,ֻ�е�wParam=PLAY_OVER��ʱ��ſ��Ե���
Setup_EndPlayMusic����,��wParam=PLAY_BREAK��ʱ��Ҫ�ٵ���Setup_EndPlayMusic����.
ֻ�е�ʹ��SETUP_PlayMusic�����������ֵ�ʱ��ſ���ʹ��Setup_EndPlayMusic����ֹͣ���ֲ���
*/
///////////////////////////////////////////////////////////////////////////////////
BOOL    Call_PlayMusic(MUSICTYPE uiringselect,unsigned int icount);
/*�������Žӿں���
uiringselect:�������,ȡֵΪ�ϱߵ�RING_SELECT
iCount:���Ŵ���,0Ϊѭ������,����Ϊ���Ŵ���
�����ŵ����ֲ���ѭ�����ŵ�ʱ��,�����Լ�������ϵ�ʱ�򲻱ص���Call_EndPlayMusic��������Setup_EndPlayMusic
�����Call_PlayMusic�����������ֵ�ʱ��,��Ҫ��;ֹͣ����,����Ҫ����Call_EndPlayMusic����
*/
BOOL    Call_EndPlayMusic(MUSICTYPE uiringselect);
/*����ֹͣ�ӿں���,uiringselect:��ǰ���������,ȡֵΪ�ϱߵ�RING_SELECT
Call_EndPlayMusic�е�uiringselectȡֵҪ���ڵ���Call_PlayMusic����ʱ��uiringselectֵ
�����粥����������ʱΪ:Call_PlayMusic(RING_PHONE);
ֹͣ��������ʱΪ:Call_EndPlayMusic(RING_PHONE);
ֻ�е�ʹ��Call_PlayMusic�����������ֵ�ʱ��ſ���ʹ��Call_EndPlayMusic����ֹͣ���ֲ���
*/

BOOL    SetCanPlayMusic_Key(BOOL bCanPlay);//���õ�ǰ�Ƿ���Բ��Ű�����
BOOL    SetCanPlayMusic(BOOL bCanPlay,DWORD dPriority);//���õ�ǰ�Ƿ���Բ���ĳ�����������
//����ģʽ����,���ù���ģʽ����,��������ΪNULL
BOOL    CallProjectModeWindow(HWND hwndCall);
#endif
