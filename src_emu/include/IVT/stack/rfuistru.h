/*
*********************************************************************************************************
*                                                Abstract
*                                          
*		This file declare the user interface structure provided for the upper layer
*
* File : rfuistru.h
* By   : Zhang Tao
*********************************************************************************************************
*/

#ifndef __RFUISTRU_H
#define __RFUISTRU_H

/*
*********************************************************************************************************
*                                              Frame and Message
*********************************************************************************************************
*/

#define RF_FRMH_LEN			2										/* frame address,control	*/
#define RF_FRML_LEN			1										/* frame length	1			*/
#define RF_FCS_LEN			1										/* frame fcs				*/
#define RF_MSGH_LEN			2										/* message type,length		*/
#define RF_RFH_LEN			(RF_FRMH_LEN+RF_FRML_LEN+RF_MSGH_LEN)	/* 5						*/
#define RF_RSV_LEN			(HCI_RESERVE + L2CAP_CO_RESERVE)		/* 20						*/

/*
*********************************************************************************************************
*                                              RPN parameters
*********************************************************************************************************
*/

#define RPNM_BITRATE				0x01				/* pm1 ~ pm8	*/
#define RPNM_DATABITS				0x02
#define RPNM_STOPBITS				0x04
#define RPNM_PARITY					0x08
#define RPNM_PRTYPE					0x10
#define RPNM_XON					0x20
#define RPNM_XOFF					0x40

#define RPNM_XIN					0x01				/* pm9 ~ pm16	*/
#define RPNM_XOUT					0x02
#define RPNM_RTRIN					0x04
#define RPNM_RTROUT					0x08
#define RPNM_RTCIN					0x10
#define RPNM_RTCOUT					0x20

#define RPN_2400BPS					0x00				/* baudrate		*/
#define RPN_4800BPS					0x01
#define RPN_7200BPS					0x02
#define RPN_9600BPS					0x03
#define RPN_19200BPS				0x04
#define RPN_38400BPS				0x05
#define RPN_57600BPS				0x06
#define RPN_115200BPS				0x07
#define RPN_230400BPS				0x08

#define RPN_5BITS					0x00				/* num bits		*/
#define RPN_6BITS					0x01
#define RPN_7BITS					0x02
#define	RPN_8BITS					0x03

#define RPN_1STOPBITS				0x00				/* stop bits	*/
#define RPN_15STOPBITS				0x01

#define RPN_NOPARITY				0x00				/* parity		*/
#define RPN_PARITY					0x01

#define RPN_ODDPARITY				0x00				/* parity type */
#define RPN_EVENPARITY				0x01
#define RPN_MARKPARITY				0x02
#define RPN_SPACEPARITY				0x03

#define RPN_NO_FC					0x00				/* fc type		*/
#define RPN_FC_XONXOFF_ON_INPUT		0x01
#define RPN_FC_XONXOFF_ON_OUTPUT	0x02
#define RPN_FC_RTR_ON_INPUT			0x04
#define RPN_FC_RTR_ON_OUTPUT		0x08
#define RPN_FC_RTC_ON_INPUT			0x10
#define RPN_FC_RTC_ON_OUTPUT		0x20

#define RPN_XON_CHAR				0x11				/* xon char		*/

#define RPN_XOFF_CHAR				0x13				/* xoff char	*/

#define RPN_L_MASK					0x7F
#define RPN_H_MASK					0x0C		/* RTR input and output	*/

/*
*********************************************************************************************************
*                                              Event
*********************************************************************************************************
*/

enum
{
	RF_DLCESTABIND,
	RF_DLCRELIND,	
	RF_LINKLOSTIND,
	RF_PNIND,
	RF_RPNIND,
	RF_CTRLIND,
	RF_RLSIND,
	RF_CREDITIND,
	RF_DATAIND,		

	RF_DLCESTABCFM,
	RF_DLCRELCFM,
	RF_PNCFM,
	RF_RPNCFM,
	RF_CTRLCFM,
	RF_RLSCFM,
	RF_TESTCFM
};

/*
*********************************************************************************************************
*                                              ERROR CODES
*********************************************************************************************************
*/

#define RFCOMM_SUCCESS							BT_SUCCESS
#define RFCOMM_ERROR_REJ_L2CONNECT				BT_SUCCESS + 1
#define RFCOMM_ERROR_REJ_SESSION				BT_SUCCESS + 2
#define RFCOMM_ERROR_REJ_DLC					BT_SUCCESS + 3
#define RFCOMM_ERROR_BAD_SVRCHNL				BT_SUCCESS + 4
#define RFCOMM_ERROR_TIMER_EXPIRED				BT_SUCCESS + 5
#define RFCOMM_ERROR_SECURITY					BT_SUCCESS + 6
#define RFCOMM_ERROR_BAD_RMTPNCFM				BT_SUCCESS + 7
#define RFCOMM_ERROR_LINKLOST					BT_SUCCESS + 8

/*
*********************************************************************************************************
*                                              Structure
*********************************************************************************************************
*/

struct struPnPar
{
	UCHAR	pri;
	WORD	mfs;
	UCHAR	credit;								/* the number of credit */
};

struct struRpnPar
{
	UCHAR	baudrate;
	UCHAR	numbits:2;
	UCHAR	stopbits:1;
	UCHAR	parity:1;
	UCHAR	ptype:2;
	UCHAR	fctrl:6;
	UCHAR	xon;
	UCHAR	xoff;
	UCHAR	pm1;
	UCHAR	pm2;
};

struct struRlsPar
{
	UCHAR	L:4;
	UCHAR   rsv:4;
	UCHAR	rls;
};

struct struV24
{
	UCHAR	EA:1;
	UCHAR	FC:1;
	UCHAR	RTC:1;
	UCHAR	RTR:1;
	UCHAR   rsv:2;
	UCHAR	IC:1;
	UCHAR	DV:1;
};

struct struBrSig
{
	UCHAR	EA:1;
	UCHAR	B1:1;							/* B1==0, no break signal	*/
	UCHAR	B2:1;
	UCHAR	B3:1;
	UCHAR	L:4;
};


struct RF_PnStru
{
	WORD   rfhandle;
	UCHAR  bd[BDLENGTH];					/* before dlc established	*/
	struct struPnPar pn;
	UCHAR  result;
};

struct RF_RpnStru
{
	WORD	rfhandle;
	UCHAR   bd[BDLENGTH];					/* before dlc established	*/
	struct  struRpnPar rpn;
	UCHAR	result;
};

struct RF_CtrlStru
{
	WORD	rfhandle;
	UCHAR	v24;							/* struct  strV24 v24		*/
	UCHAR	brsig;							/* struct  strBrSig brsig	*/
	UCHAR	result;
};

struct RF_RlsStru
{
	WORD	rfhandle;
	UCHAR	rls;							/* struct	strRlsPar rls	*/
	UCHAR   result;
};

struct RF_TestStru
{
	UCHAR	bd[BDLENGTH];
	UCHAR	result;
	UCHAR	length;
	UCHAR	*data;
};

struct RF_CreditStru
{
	WORD	rfhandle;
	UCHAR	credit;
};

struct RF_DataReqStru
{
	WORD	rfhandle;
	UCHAR	credit;
	struct	BuffStru *buf;
};

struct RF_DataIndStru
{
	WORD	rfhandle;
	WORD	length;
	struct 	BuffStru *buf;
};

struct RF_DlcEstabReqStru
{
	UCHAR	bd[BDLENGTH];
	UCHAR	svrchnl;
	struct	struPnPar pn;
};

struct RF_DlcEstabRspStru
{
	WORD	rfhandle;
	UCHAR	accept;
};

struct RF_DlcEstabIndStru
{
	WORD	rfhandle;
	UCHAR	bd[BDLENGTH];
};

struct RF_DlcEstabCfmStru
{
	WORD	rfhandle;
	UCHAR	bd[BDLENGTH];
	UCHAR	result;
};

struct RF_DlcRelStru {
	WORD	rfhandle;
	UCHAR	result;
};

#endif
