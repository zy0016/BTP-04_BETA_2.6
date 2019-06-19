#ifndef HCRPMACRO_H
#define HCRPMACRO_H

/*L2CAP PSM for printer and scanner predefinitions*/
#define HCRP_PRT_CCH_PSM	4097
#define HCRP_PRT_DCH_PSM	4099
#define HCRP_SCAN_CCH_PSM	15
#define HCRP_SCAN_DCH_PSM	17
#define HCRP_NOTIF_PSM		4095

/*L2CAP indication definitions*/
#define L2CAP_CONN_IND		1
#define L2CAP_CONFIG_IND	2
#define L2CAP_DISC_IND		3
#define L2CAP_QOSVIA_IND	4
#define L2CAP_DATA_IND		5

/*indication and confirmation definitions*/
#define HCRP_CONNIND			1
#define HCRP_DISCIND			2
#define HCRP_DATAIND			3
#define HCRP_CREDITGRANTIND		4
#define HCRP_CREDITGRANTCFM		5
#define HCRP_CREDITREQIND		6
#define HCRP_CREDITREQCFM		7
#define HCRP_CREDITRETURNIND	8
#define HCRP_CREDITRETURNCFM	9
#define HCRP_CREDITQUERYIND		10
#define HCRP_CREDITQUERYCFM		11
#define HCRP_LPTSTATUSIND		12
#define HCRP_LPTSTATUSCFM		13
#define HCRP_1284IDIND			14
#define HCRP_1284IDCFM			15
#define HCRP_SOFTRESETIND		16
#define HCRP_SOFTRESETCFM		17
#define HCRP_HARDRESETIND		18
#define HCRP_HARDRESETCFM		19
#define HCRP_NOTIFREGIND		20
#define HCRP_NOTIFREGCFM		21
#define HCRP_NOTIFALIVEIND		22
#define HCRP_NOTIFALIVECFM		23
#define HCRP_NOTIFDISCIND		24
#define HCRP_NOTIFIND			25
#define HCRP_NOTIFUNREGIND	26
#define HCRP_SRV_POLL_TIMER	27
//added for win2000 can't print immediately after writefile;
#define HCRP_SESSIONINACTIVEIND 28

#define HCRP_CONNCFM			29
#define HCRP_DISCCFM			30

#define HCRP_DATACFM			31

/*channel id*/
#define HCRP_CCH		0
#define HCRP_DCH		1
#define HCRP_NCH		2
#define HCRP_NONECH		0xFF

/*control fsminst userdata flag value*/
#define HCRP_CSROLE			0   /*bit 0 represent Client or Server role*/
#define HCRP_BYTE_ORDER		1   /*bit 1 determine the byte ordering little or big endian codec rule*/
#define HCRP_CLIENT			0
#define HCRP_SERVER			1
#define HCRP_BIG_ENDIAN		0
#define HCRP_LITTLE_ENDIAN	1

/*control fsminst userdata flag processing*/
#define SETCLIENTROLE(flag)		((flag) = (UCHAR)((flag)&0xFE))
#define SETSERVERROLE(flag)		((flag) = (UCHAR)((flag)|0x01))
#define GETCSROLE(flag)			((UCHAR)((flag)&0x01))
#define SETBIGENDIAN(flag)		((flag) = (UCHAR)((flag)&0xFD))
#define SETLITTLEENDIAN(flag)	((flag) = (UCHAR)((flag)|0x02))
#define GETBYTEORDER(flag)		((UCHAR)((((flag)&0x02)==0)?0:1))

/*session fsminst userdata flag value*/
#define CCH_BIT			    0   /*bit 0 represent cch status*/
#define DCH_BIT			    1   /*bit 1 represent dch status*/
#define TIMER			    2	/*bit 2 represent timer status*/
#define ACTIVE              3   /*bit 3 represent the session active or inactive for data channel*/
#define DATA                4   /*bit 4 represent whether session received data in some time interval*/
#define CCH_READY		    1
#define CCH_INVALID		    0
#define DCH_READY		    1
#define DCH_INVALID		    0
#define TIMER_VALID		    1
#define TIMER_INVALID	    0
#define DCH_ACTIVE			1
#define DCH_INACTIVE		0
#define DATA_RECEIVED		1
#define DATA_IDLE           0

/*session fsminst userdata flag processing*/
#define SETCCHREADY(flag)		((flag) = (UCHAR)((flag)|0x01))
#define GETCCHREADY(flag)		((UCHAR)((flag)&0x01))
#define SETDCHREADY(flag)		((flag) = (UCHAR)((flag)|0x02))
#define GETDCHREADY(flag)		((UCHAR)((((flag)&0x02)==0)?0:1))
#define CLRCDCHREADY(flag)		((flag) = (UCHAR)((flag)&0xFC))
#define CLRTIMERVALID(flag)		((flag) = (UCHAR)((flag)&0xFB))
#define SETTIMERVALID(flag)		((flag) = (UCHAR)((flag)|0x04))
#define GETTIMER(flag)			((UCHAR)((((flag)&0x04)==0)?0:1))
#define SETDCHACTIVE(flag)		((flag) = (UCHAR)((flag)|0x08))
#define CLRDCHACTIVE(flag)		((flag) = (UCHAR)((flag)&0xF7))
#define GETDCHACTIVE(flag)		((UCHAR)((((flag)&0x08)==0)?0:1))
#define SETDATAFLAG(flag)		((flag) = (UCHAR)((flag)|0x10))
#define CLRDATAFLAG(flag)		((flag) = (UCHAR)((flag)&0xEF))
#define GETDATAFLAG(flag)		((UCHAR)((((flag)&0x10)==0)?0:1))

/*PDU id*/
#define PDU_CREDITGRANT		0x0001
#define PDU_CREDITREQ		0x0002
#define PDU_CREDITRETURN	0x0003
#define PDU_CREDITQUERY		0x0004
#define PDU_LPTSTATUS		0x0005
#define PDU_1284ID			0x0006
#define PDU_SOFTRESET		0x0007
#define PDU_HARDRESET		0x0008
#define PDU_NOTIFREG		0x0009
#define PDU_NOTIFALIVE		0x000A
#define PDU_N_NOTIF			0x0001

/*meaning of code field in response PDU*/
#define CODE_UNSUPPORTED	0x0000
#define CODE_SUCCESS		0x0001
#define CODE_SYNCERROR		0x0002
#define CODE_FAILURE		0xFFFF

/*lpt status information mask code*/
#define STATUS_MASK_PAPER_EMPTY		5
#define STATUS_MASK_SELECT			4
#define STATUS_MASK_NOT_ERROR		3

/*pdu length definitions*/
#define HCRP_REQ_PDU_HEADER_LEN		6
#define HCRP_RSP_PDU_HEADER_LEN		8
#define HCRP_BUF_RES_LEN			(HCI_RESERVE + L2CAP_CO_RESERVE)	/* 20 */
#define HCRP_CREDITGRANT_REQ_LEN	10
#define HCRP_CREDIT_RSP_LEN			12
#define HCRP_CREDITRETURN_REQ_LEN	10
#define HCRP_CREDITRETURN_RSP_LEN	12
#define HCRP_CREDITQUERY_REQ_LEN	10
#define HCRP_CREDITQUERY_RSP_LEN	12
#define	HCRP_LPTSTATUS_RSP_LEN		9
#define HCRP_1284ID_REQ_LEN			10
#define HCRP_NOTIFREG_REQ_LEN		15
#define	HCRP_NOTIFREG_RSP_LEN		16
#define	HCRP_NOTIFALIVE_RSP_LEN		12
#define HCRP_NOTIF_REQ_LEN			6

/*HCRP configarable parameters*/
#define HCRP_MAX_CREDIT			        0xFFFFFFFF
#define HCRP_SRV_POLL_TIMEOUT	        60000
#define HCRP_NOTIF_ALIVE_TIMEOUT		5000  
#define HCRP_NOTIF_CONN_TIMEOUT			15000
#define HCRP_NOTIF_REG_TIMEOUT			600000
#define HCRP_NOTIF_REG_TIMERID_BASE		0x00010000
#define HCRP_NOTIF_CONN_MAX_NUMBER		10

/*error codes*/
#define HCRP_ERROR_NO_RESOURCE		(PROT_HCRP_BASE+10)
#define HCRP_ERROR_ROLE				(PROT_HCRP_BASE+11)
#define HCRP_ERROR_NO_CREDIT		(PROT_HCRP_BASE+12)
#define HCRP_ERROR_NO_CONNECTION	(PROT_HCRP_BASE+13)
#define HCRP_ERROR_IN_CONNECTION	(PROT_HCRP_BASE+14)
#define HCRP_ERROR_TIMER_INVALID	(PROT_HCRP_BASE+15)
#define HCRP_ERROR_CREDIT_VALUE		(PROT_HCRP_BASE+16)
#define HCRP_ERROR_TIMEOUT			(PROT_HCRP_BASE+17)
#define HCRP_ERROR_CONNECTION_FAIL	(PROT_HCRP_BASE+18)
#define HCRP_ERROR_PDUSEND_FAIL     (PROT_HCRP_BASE+19)
#define HCRP_ERROR_REPLYPDU_STATUS  (PROT_HCRP_BASE+20)
#define HCRP_ERROR_FEATURE_UNSUPPORTED  (PROT_HCRP_BASE+21)


/*codec mask*/
#define HCRP_CODEC_WORD			0x00000002
#define HCRP_CODEC_DWORD		0x00000003
#define HCRP_CODEC_REQ_HEADER	0x0000002A

/*fsm status flag in connect procedure*/
#define HCRP_START		1
#define HCRP_INIT		2

/*flag to distinguish data or command when write data to l2cap*/
#define HCRP_DATA		0
#define HCRP_CONTROL	1

/*mask for notif reg command*/
#define HCRP_REG_NOTIF		0x01
#define HCRP_UNREG_NOTIF	0x00
#endif

///added
#define PROT_HCRPAPI_BASE 900

#define HCRP_CREDIT_GRANT_HDL (PROT_HCRPAPI_BASE)
#define HCRP_CREDIT_REQ_HDL	(PROT_HCRPAPI_BASE+1)
#define HCRP_CREDIT_RETURN_HDL (PROT_HCRPAPI_BASE+2)
#define HCRP_CREDIT_QUERY_HDL (PROT_HCRPAPI_BASE+3)
#define HCRP_GET_LPT_STATUS_HDL (PROT_HCRPAPI_BASE+4)
#define HCRP_GET_1284ID_HDL (PROT_HCRPAPI_BASE+5)
#define HCRP_SOFT_RESET_HDL (PROT_HCRPAPI_BASE+6)
#define HCRP_HARD_RESET_HDL (PROT_HCRPAPI_BASE+7)
#define HCRP_REG_NOTIF_HDL (PROT_HCRPAPI_BASE+8)
#define HCRP_NOTIF_CONN_ALIVE_HDL (PROT_HCRPAPI_BASE+9)
#define HCRP_CONN_REQ_HDL (PROT_HCRPAPI_BASE+10)
#define HCRP_DISC_REQ_HDL (PROT_HCRPAPI_BASE+11)
#define HCRP_NOTIF_REQ_HDL (PROT_HCRPAPI_BASE+12)
#define HCRP_DATA_REQ_HDL (PROT_HCRPAPI_BASE+13)
