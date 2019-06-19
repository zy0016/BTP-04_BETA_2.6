/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : wap.h
 *
 * Purpose  : implement wap program interface
 *            
\**************************************************************************/

#ifndef _WAPI_H_
#define _WAPI_H_

/*************************************************************************\
*
*	constant definition
*
\*************************************************************************/

/* API return value */
#define WAPI_SUCCESS		0		/* success */
#define WAPI_FAILURE		1		/* failure */
#define WAPI_BLOCKED		2		/* block */
#define WAPI_PARAMETER		3
#define WAPI_RESOURCE		4
#define WAPI_INVALID		5
#define WAPI_DATA			6
#define WAPI_NOTCONNECTED	7
#define WAPI_NOTSUSPENDED	8
#define WAPI_NOTINIT		9
#define WAPI_CONNECTED		10

/* message definition-LOWORD(lParam) */
#define MSG_CONNECTED		1		/* connect */
#define MSG_DISCONNECT		2		/* disconnect */
#define MSG_REDIRECT		3		/* redirect */
#define MSG_SUSPEND			4		/* suspend */
#define MSG_RESUME			5		/* resume */
#define MSG_PUSHDATA		6		/* PUSH */
#define MSG_RECV_URL		7		/* receive URL data */
#define MSG_CLOSE_URL		8		/* Application may close URL */

/* error reason */
#define WAPI_Continue				0x10
#define WAPESwitching				0x11
#define WAPEOK						0x20
#define WAPECreated					0x21
#define WAPEAccepted				0x22
#define WAPENonAuthoritative		0x23
#define WAPENoContent				0x24
#define WAPEReset					0x25
#define WAPEPartialContent			0x26
#define WAPEMultipleChoices			0x30
#define WAPEMovedPermanently		0x31
#define WAPEMovedtemporarily		0x32
#define WAPESeeOther				0x33
#define WAPENotmodified				0x34
#define WAPEUseProxy				0x35
#define WAPEBadRequest				0x40
#define WAPEUnauthorized			0x41
#define WAPEPaymentrequired			0x42
#define WAPEForbidden				0x43
#define WAPENotFound				0x44
#define WAPEMethodnotallowed		0x45
#define WAPENotAcceptable			0x46
#define WAPEProxyAuthenticationrequired	0x47
#define WAPERequestTimeout			0x48
#define WAPEConflict				0x49
#define WAPEGone					0x4A
#define WAPELengthRequired			0x4B
#define WAPEPreconditionfailed		0x4C
#define WAPERequestedentitytoolarge	0x4D
#define WAPERequestURItoolarge		0x4E
#define WAPEUnsupportedmediatype	0x4F
#define WAPEInternalServerError		0x60
#define WAPENotImplemented			0x61
#define WAPEBadGateway				0x62
#define WAPEServiceUnavailable		0x63
#define WAPEGatewayTimeout			0x64
#define WAPEHTTPversionnotsupported	0x65

#define WAPEPROTOERR			0xe0	//remote protocol error
#define WAPEDISCONNECT			0xe1	//disconnecting error
#define WAPESUSPEND				0xe2	//suspending error
#define WAPERESUME				0xe3	//resuming error
#define WAPECONGESTION			0xe4	//remote resource lack
#define WAPECONNECTERR			0xe5	//session creation error
#define WAPEMRUEXCEEDED			0xe6	//data packet size error
#define WAPEMOREXCEEDED			0xe7	//negotiation method  or push number error
#define WAPEPEERREQ				0xe8	//remote error
#define WAPENETERR				0xe9	//network error
#define WAPEUSERREQ				0xea	//user error
#define WAPEREDIRECT			0xeb	//redirect
#define WAPEREPLY				0xec

/* definition on bearer type and address length */
#define WAP_IPv4					0x00
#define WAP_IPv6					0x01
#define WAP_GSM_USSD				0x02
#define WAP_GSM_SMS					0x03
#define WAP_IS_136_R_Data			0x04
#define WAP_IS_637_CDMA_SMS			0x05

/* WAP default port */
#define WSP_Datagram_PORT				9200	//WAP connectionless session service 
#define WSP_WTLS_Datagram_PORT			9202	//WAP secure connectionless session service
#define WSP_WTP_Datagram_PORT			9201	//WAP session service
#define WSP_WTP_WTLS_Datagram_PORT		9203	//WAP secure session service 
#define vCard_Datagram_PORT				9204	//WAP vCard 
#define vCard_WTLS_Datagram_PORT		9206	//WAP vCard Secure 
#define vCalendar_Datagram_PORT			9205	//WAP vCal 
#define vCalendar_WTLS_Datagram_PORT	9207	//WAP vCal Secure 

/* WAP Method define */
#define WAP_METHOD_GET        1
#define WAP_METHOD_POST       2

/*************************************************************************\
*
*	definitions on data struction and data type 
*
\*************************************************************************/
/* maximal length of redirect address */
#define RDADDR_MAXADDRLEN	32			
/* redirect reason */
#define WAP_PERREDIRECT			0x80	//permanence redirect
#define WAP_REUSESECURITY		0x40	//reuse security session

/* data struction */
typedef struct tagWapData
{
	char* pData;
	int len;
}WapData;

/* redirect struction */
typedef struct tagWapReDirect
{
	int rdReason;		//redirect reason
	int ifBearNet;		//bearer network validity
	int ifRDPort;		//need to redirect port
	char addrlen;		//address length
	char BearNet;		//bearer type
	unsigned short Port;	//redirect port  
	char RDAddr[RDADDR_MAXADDRLEN];//redirect address	
}WapReDirect;

/* note£ºdefinition about WSP capability */
#define MAX_EXMTNUM			10
#define MAX_CODEPAGENUM		10
#define WSP_DEFCSDU			2 * 1024		//byte	
#define WSP_DEFMAXMNUM		10
#define WSP_DEFMAXPNUM		10
#define WSP_DEFSSDU			2 * 1024		//byte
#define MAX_PROOPTIONNUM	10

#define WSP_MAXCAPALEN		1024			//byte

#define WSP_CAPACSUD		0x00			//capability Client SDU Size
#define WSP_CAPASSUD		0x01			//capability Server SDU Size
#define WSP_CAPAPOPTION		0x02			//capability Protocol Option
#define WSP_CAPAMMOR		0x03			//capability Method MOR
#define WSP_CAPAPMOR		0x04			//capability Push MOR
#define WSP_CAPAEMETHOD		0x05			//capability Extended Method
#define WSP_CAPAHCODEPAGE	0x06			//capability Header Code Page
#define WSP_CAPAALIASES		0x07			//capability Aliases

/* WSP capability negotiation struction */
typedef struct tagWspCapability
{
	void** Aliases;						//alias(address struction)¡£
	int CLientSDU;						//maximal data length of client
	int ExMethod[MAX_EXMTNUM];			//extend method
	int HCPage[MAX_CODEPAGENUM];		//header code page
	int Momr;							//maximal number of method
	int Mopr;							//maximal number of push
	int ProtoOption[MAX_PROOPTIONNUM];	//protocol option
	int ServerSDU;						//maximal data length of server.
}WspCapability;

#define WAPI_PUSHIDANYAPP	"x-wap-application:*"			//Any Application 
#define WAPI_PUSHIDSIAAPP	"x-wap-application:push.sia"	//WAP Push SIA 
#define WAPI_PUSHIDWMLAPP	"x-wap-application:wml.ua"		//WML User Agent 
#define WAPI_PUSHIDWTAAPP	"x-wap-application:wta.ua"		//WTA User Agent 
//This ID will used for application dispatching to MMS User 
//Agent in the handling of MMS notfication using WAP Push. 
//See WAP-206-MMSCTR for more detail. 
#define WAPI_PUSHIDMMSAPP	"x-wap-application:mms.ua"		
//SyncML PUSH Application ID: used to push a SyncML Alert 
//from a SyncML server side. The SyncML Alert is an indication 
//for starting a SyncML session e.g., for data synchronization. 
//Requested by the WAP WAG Synchronisation Drafting Committee. 
#define WAPI_PUSHIDSYNCMLAPP	"x-wap-application:push.syncml"		
//This ID is used for application dispatching to Location User 
//Agent in the handling of Location Invocation document. 
//See WAP-257-LOCPROT for details. Requested by the WAP WAG Location 
//Drafting Committee.  
#define WAPI_PUSHIDLOCAPP	"x-wap-application:loc.ua"
//This ID is used for SyncML Device Management. Requested by the SyncML 
//Device Management Expert Group.  
#define WAPI_PUSHIDSYNCDMAPP "x-wap-application:syncml.dm"
//This ID is used for DRM User Agent. Requested by the WAP WAG Download DC.  
#define WAPI_PUSHIDDRMAPP	"x-wap-application:drm.ua"
//This ID is used for Email Notification (EMN) User Agent. 
//Requested by the WAP WAG PUSH DC.  
#define WAPI_PUSHIDEMNAPP	"x-wap-application:emn.ua"
//This ID is used for Wireless Village (EMN) User Agent. 
//Requested by Wireless Village.  
#define WAPI_PUSHIDWVAPP	"x-wap-application:wv.ua"

/* WAPI option setting */
#define WAPI_FIONBIO	1

/* priority when connecting  */
#define CONNECT_LOW		1
#define CONNECT_NORMAL	2
#define CONNECT_HIGHT	3

/*************************************************************************\
*
*	function definition
*
\*************************************************************************/
/* WAP API */
int WAPI_Instance(HWND hwnd, int mesg);
int WAPI_Connect(char* gateway, unsigned short port, int SDUSize);
int WAPI_Connect_Open(char* gateway, unsigned short port, WapData* pHeader, WapData* pCapa, int ifSec );
int WAPI_Disconnect(void);
int WAPI_Suspend(void);
int WAPI_Resume(void);

int WAPI_GetPUSHHeader(int push_handle, char* buf, int buflen);
int WAPI_GetPUSHHeaderSize(int push_handle);
int WAPI_GetPUSHBody(int push_handle, char* buf, int buflen);
int WAPI_GetPUSHBodySize(int push_handle);
int WAPI_ClosePush(int Push_handle);
/* WSP/B API */
// connectionless URL request
int WAPI_RequestURL(char* URL, char method, 
					char* head, int headlen,
					char* body, int bodylen);	/* return value:non-zero handle*/

//connectionless URL request
int WAPI_RequestURLto(char* gateway, unsigned short port, 
					char* URL, char method, 
					char* head, int headlen,
					char* body, int bodylen);	/* return value:non-zero handle*/

int WAPI_GetURLHeader(int URL_handle, char* buf, int buflen);
int WAPI_GetURLHeaderSize(int URL_handle);
int WAPI_GetURLBody(int URL_handle, char* buf, int buflen);
int WAPI_GetURLBodySize(int URL_handle);
int WAPI_CloseURLRequest(int URL_handle);
int WAPI_PushResister( HWND hWnd, int Msg, char* AppID );
int WAPI_PushUnResister( HWND hWnd, char* AppID );

/* priority when connecting  */
#define CONNECT_LOW		1
#define CONNECT_NORMAL	2
#define CONNECT_HIGHT	3

/* public function statement */
int WAPA_Startup(void);
int WAPA_Connect(char *Gateway, unsigned short Port, int SDUSize, 
				  HWND hWnd, int Msg, int Priority);
int WAPA_Connect_Open(char *Gateway, unsigned short Port, WapData *pHeader, 
					  WapData *pCapa, int ifSec, HWND hWnd, int Msg, int Priority);
int WAPA_Disconnect(HWND hWnd, int Msg);
int WAPA_Suspend(void);
int WAPA_Resume(char *gateway, unsigned short port);

int WAPA_RequestURL(char *URL, char method, char *head, int headlen,
					char *body, int bodylen, HWND hWnd, int Msg);
int WAPA_RequestURLto(char *gateway, unsigned short port, char *URL,
					 char method, char *head, int headlen, char *body,
					 int bodylen, HWND hWnd, int Msg);
int WAPA_GetURLHeader(int URL_handle, char* buf, int buflen);
int WAPA_GetURLHeaderSize(int URL_handle);
int WAPA_GetURLBody(int URL_handle, char* buf, int buflen);
int WAPA_GetURLBodySize(int URL_handle);
int WAPA_CloseURLRequest(int URL_handle);

int WAPA_PushResister( HWND hWnd, int Msg, char* AppID );
int WAPA_PushUnResister( HWND hWnd, char* AppID );
int WAPA_GetPUSHHeader(int push_handle, char* buf, int buflen);
int WAPA_GetPUSHHeaderSize(int push_handle);
int WAPA_GetPUSHBody(int push_handle, char* buf, int buflen);
int WAPA_GetPUSHBodySize(int push_handle);
int WAPA_ClosePush(int Push_handle);
/************* End Of Head File ******************************************/
#endif	/* _WAPI_H_ */
