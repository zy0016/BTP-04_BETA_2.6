 
/**************************************************************************\
 *
 *                      PDAOS Window System
 *
 * Copyright (c) 2000 by CASS Corporation & Software Engineering Center
 *                       All Rights Reserved
 *
 *
 * Filename  : ui.h
 * 
 * Purpose   : about UDB infomation
 *  
 * Author    : jingjing xu
 *
 * Date      : 2000/12/29
 *
 **************************************************************************/

#ifndef _UI_H_
#define _UI_H_

/*���ŵ�UDB������Ϣ*/
#define     DIALFILETEST		"FLASH2:DialCfgTest.mal"

#define     UDB_K_DNS_SIZE        31
#define     UDB_K_NAME_SIZE       31
#define     UDB_K_PWD_SIZE        31
#define     UDB_K_TEL_SIZE        31
#define     UDB_K_URL_SIZE        511

typedef struct{
	char   szISP[ UDB_K_DNS_SIZE+1 ];
	char   szPhoneNum[ UDB_K_TEL_SIZE+1 ];
	char   szUserName[ UDB_K_NAME_SIZE+1 ];
	char   szPassWord[ UDB_K_PWD_SIZE+1 ];
}DIALTYPE;


/* UDB���ݽṹ */
typedef struct
{
	unsigned long		ISPID;						/*	ISP��ID��			*/
	unsigned long		PrimarySelect;				/*	�Ƿ���ѡISP			*/
	unsigned long		ProxyFlag;					/*	�Ƿ�ʹ�ô��������	*/
	unsigned long		ProxyID;					/*	���������ID		*/
    unsigned long		DtType;                    /*  ��������GPRS/CSD    */

	char    ISPName[ UDB_K_NAME_SIZE+1 ];			/*	ISP��				*/
	char	ISPUserName[ UDB_K_NAME_SIZE+1 ];		/*	ISP��¼�û���		*/
	char	ISPPassword[ UDB_K_PWD_SIZE+1 ];		/*	ISP��¼����			*/
	char	PrimaryDns[ UDB_K_DNS_SIZE+1 ];			/*	������������		*/
	char	SlaveDns[ UDB_K_DNS_SIZE+1 ];			/*	��������������		*/
	
	char    WAPGatewayIP[ UDB_K_DNS_SIZE+1 ];		/*	WAP����IP		*/
	char    MMSGatewayIP[UDB_K_DNS_SIZE+1];			/*	WAP����IP		*/
	char    MMSURL[ UDB_K_URL_SIZE+1 ];				/*	MMSURL		*/
	char    WAPPORT[UDB_K_DNS_SIZE+1];         /* WHTTP �� �˿�*/
	
	char	ISPPhoneNum1[ UDB_K_TEL_SIZE+1 ];		/*	����ISP�ĵ绰����	*/
	char	ISPPhoneNum2[ UDB_K_TEL_SIZE+1 ];		/*	����ISP�ĵ绰����	*/
	char	ISPPhoneNum3[ UDB_K_TEL_SIZE+1 ];		/*	����ISP�ĵ绰����	*/
	char	ISPPhoneNum4[ UDB_K_TEL_SIZE+1 ];		/*	����ISP�ĵ绰����	*/

}UDB_ISPINFO;

typedef struct tagSETINFO
{
    char MMSURL[ UDB_K_URL_SIZE+1 ];
	char MMSIP[ UDB_K_DNS_SIZE+1 ];
	char MMSPORT[ UDB_K_DNS_SIZE+1 ];
	char WAPIP[ UDB_K_DNS_SIZE+1 ];
	char  WAPPORT[ UDB_K_DNS_SIZE+1 ];
}SETINFO, *PSETINFO;


int	UDB_IsLogin( void );
int	UDB_GetISPInfo( int, void* );
int	UDB_GetLogUser( void );
int	UDB_CntISP( int );
int	UDB_EnumISP( int, int );
int GetPredefinedISP( UDB_ISPINFO *, int );
int	UDB_GetProfileInt( const int, char*, char*, int );
int	UDB_GetProfileString( const int, char*, char*, char*, char*, int );
int	UDB_WriteProfile(int a,char* b,char* c,char* d);
int UDB_WriteProfileInt( int a, char* b, char* c, int d) ;

#endif/*_UI_H_*/
