 
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

/*拨号的UDB设置信息*/
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


/* UDB数据结构 */
typedef struct
{
	unsigned long		ISPID;						/*	ISP的ID号			*/
	unsigned long		PrimarySelect;				/*	是否首选ISP			*/
	unsigned long		ProxyFlag;					/*	是否使用代理服务器	*/
	unsigned long		ProxyID;					/*	代理服务器ID		*/
    unsigned long		DtType;                    /*  传输类型GPRS/CSD    */

	char    ISPName[ UDB_K_NAME_SIZE+1 ];			/*	ISP名				*/
	char	ISPUserName[ UDB_K_NAME_SIZE+1 ];		/*	ISP登录用户名		*/
	char	ISPPassword[ UDB_K_PWD_SIZE+1 ];		/*	ISP登录密码			*/
	char	PrimaryDns[ UDB_K_DNS_SIZE+1 ];			/*	主域名服务器		*/
	char	SlaveDns[ UDB_K_DNS_SIZE+1 ];			/*	辅助域名服务器		*/
	
	char    WAPGatewayIP[ UDB_K_DNS_SIZE+1 ];		/*	WAP网关IP		*/
	char    MMSGatewayIP[UDB_K_DNS_SIZE+1];			/*	WAP网关IP		*/
	char    MMSURL[ UDB_K_URL_SIZE+1 ];				/*	MMSURL		*/
	char    WAPPORT[UDB_K_DNS_SIZE+1];         /* WHTTP 的 端口*/
	
	char	ISPPhoneNum1[ UDB_K_TEL_SIZE+1 ];		/*	连接ISP的电话号码	*/
	char	ISPPhoneNum2[ UDB_K_TEL_SIZE+1 ];		/*	连接ISP的电话号码	*/
	char	ISPPhoneNum3[ UDB_K_TEL_SIZE+1 ];		/*	连接ISP的电话号码	*/
	char	ISPPhoneNum4[ UDB_K_TEL_SIZE+1 ];		/*	连接ISP的电话号码	*/

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
