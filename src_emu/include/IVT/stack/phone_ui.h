/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*	File name:  ctp_ui.h
*
*	Description:  This file defines all API components provided by CTP&ICP.
*	Created by:  He gang
*	Date:  2003.2
*
*---------------------------------------------------------------------------*/

#ifndef _PHONE_UI_H_
#define _PHONE_UI_H_

/*---------------------------------COMMON MACRO---------------------------------*/
#define PHONE_MAX_NAME_LEN						64
#define PHONE_BD_CLASS_LEN						3
#define	MAX_CALLING_NUM_LEN						24
#define	MAX_CALLED_NUM_LEN						24

/*-------------------------   DISCONNECT CAUSE    ------------------------------*/
#define CAUSE_PHONE_NORMAL_CALL_CLEARING				16
#define CAUSE_PHONE_USER_BUSY							17
#define CAUSE_PHONE_NO_USER_CORRESPONDING				18
#define CAUSE_PHONE_USER_REJECT							21
#define CAUSE_PHONE_NONSELECTED_USER_CLEARING			26
#define CAUSE_PHONE_NO_CHANNEL_AVAILABLE				34
#define CAUSE_PHONE_REQUESTED_CHANNEL_NOT_AVAILABLE	44
#define CAUSE_PHONE_RECOVERY_ON_TIMER_EXPIRY			102

#define CAUSE_PHONE_CALL_TRANSFER						200
#define CAUSE_PHONE_REGISTER_CALLER_ID_ACCEPT			201
#define CAUSE_PHONE_REGISTER_CALLER_ID_REJECT			202
#define CAUSE_PHONE_USER_CANCEL							203
#define CAUSE_PHONE_CALL_TRANSFER_TIMEOUT				204

#define CAUSE_PHONE_LINK_LOSS							       255

/*--------------------------FAST ACCESS FAIL CAUSE -----------------------------*/
#define CAUSE_USER_BUSY					17

/*---------------------------------MMI EVENT---------------------------------*/
#define EV_MMI_SetupInd									10
#define EV_MMI_SetupCompleteInd						11
#define EV_MMI_DisconnInd								12
#define EV_MMI_LinklossInd								13
#define EV_MMI_LinkConnectInd							14
#define EV_MMI_DigitInd									15
#define EV_MMI_CcidInd                   				             16 //Added by Wangbin, 2004_07_22
#define EV_MMI_AccessRightsInd							30
#define EV_MMI_AccessRightsCfm							31
#define EV_MMI_FastAccessInd							32
#define EV_MMI_FastAccessCfm							33
#define EV_MMI_InfoSuggestInd							34
#define EV_MMI_ExitPageScanInd							37
#define EV_MMI_InfoInd									38

/*----------------------------------HW EVENT---------------------------------*/
#define EV_HW_Local_HookOn								100
#define EV_HW_Local_HookOff								101
#define EV_HW_Local_Flash								102
#define EV_HW_Local_Digit								103
#define EV_HW_Local_GetIncomingExCallInfo				104

#define EV_HW_ConnSCO2Local							130
#define EV_HW_ConnLine2Local							131
#define EV_HW_ConnLine2SCO								132
#define EV_HW_DisconnSCO								134
#define EV_HW_DisconnLine								135


/*--------------------Hook sate---------------------------------------------*/
#define PHONE_ACCEPT		1
#define PHONE_BUSY			2

/*------------------ CALL CLASS --------------------------------------------*/
#define CCLS_EXCALL				0
#define CCLS_INTERCOM			1
#define CCLS_EXCALL_INTERCOM	6
#define CCLS_OUT_EXCALL		7 
#define CCLS_REG_CALLER_ID 		8 /* for call forward */
#define INVALID_CCLS			0xff

/*---------------------------------MMI STRUCTURE---------------------------------*/
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 Description of Partial Elements of Data Structures:
	call_cls:         Call Class
					0: External Call (Default)
					1: Intercom Call
					2: Service Call
					3: Emergency Call
					Others: External Call
	called_numb_len:   Length of the Callee-party Number
	called_numb:      Phone Number of the Callee Party
	calling_numb_len:  Length of the Caller-party Number
	calling_numb:     Phone Number of the Caller Party
	calling_off:	 Offset of the Caller-party Number in the Data Buffer
	comp_len:         Length of the Company-specific Info
	company:		Company-specific Info
	data:			Phone Numbers of Both Parties
	flag:			Indicator of Acceptance or Rejection
					0: Reject
					1: Accept
	inter_numb:		Inter-member Number of a WUG Member
	keypad:			ASCII Code of a Key
	progress:		Progress Indicator
	sco_hdl:		Handle of a SCO link
	sending_complete: Indicator of Completion of Sending the Callee-party Number
					0: No
					1: Yes
------------------------------------------------------------------------------------------*/
/* data order: 1.calling number, 2.called number, 3.company data */
struct MMI_SetupReqStru {
		UCHAR bd_addr[BDLENGTH];
		UCHAR call_cls;
		UCHAR calling_numb_len;
		UCHAR called_numb_len;
		UCHAR comp_len; /* used by tl to register call forward */
		UCHAR data[1];
};

/* call info save order: 1.calling number, 2. called number, 3. company data */
struct MMI_SetupIndStru {
		UCHAR call_cls;
		UCHAR result; /* used by GW, imply whether GW accept the outgoing external call */
		UCHAR is_registered; /* used by TL, imply whether the incoming external call is registered to the TL */
		UCHAR calling_numb_len;
		UCHAR called_numb_len;
		UCHAR comp_len; 
		UCHAR data[1];
};

struct MMI_AccessRightsRspStru{
		UCHAR bd_addr[BDLENGTH];
		UCHAR flag;
		UCHAR comp_len;
		UCHAR company[1];
};

struct MMI_FastAccessReqStru {
		UCHAR bd_addr[BDLENGTH];
		UCHAR comp_len;
		UCHAR company[1];
};

struct MMI_FastAccessRspStru{
		UCHAR flag;
		UCHAR cause;
		UCHAR comp_len;
		UCHAR company[1];
};

struct MMI_GetInfoElementStru{
		UCHAR inter_numb[2];
		UCHAR bd_addr[BDLENGTH];
};

struct MMI_CompanySpecStru {
		UCHAR comp_len;
		UCHAR company[1];
};

struct MMI_TransferDataStru {
		UCHAR		org_bd_addr[BDLENGTH];	/* address of the device which initiates the call-transfer request */
		UCHAR		des_bd_addr[BDLENGTH]; /* address of the device which is the destination of the call-transfer request */
};

/*---------------------------------MMI COMMAND---------------------------------*/
void PHONE_CordlessEstablishReq(DWORD handle, UCHAR* bd_addr, WORD* result);
void PHONE_CordlessReleaseReq(UCHAR* bd_addr, WORD* result);
void PHONE_MMISetupReq(DWORD* handle, struct MMI_SetupReqStru* setup_req, WORD* result);
void PHONE_MMIDisconnReq(DWORD handle, WORD reason, WORD* result);
void PHONE_MMIDigitReq(DWORD handle, UCHAR key, WORD* result);
void PHONE_MMIStopDTMFReq(DWORD handle, WORD* result);
void PHONE_MMIInfoReq(DWORD handle, struct MMI_CompanySpecStru* info, WORD* result);
void PHONE_MMIRegisterRecallReq(DWORD handle, WORD* result);
void PHONE_MMICallTransfer(DWORD handle, struct MMI_TransferDataStru* transfer_req, WORD* result);
void PHONE_MMIAccessRightsReq(DWORD handle, struct MMI_CompanySpecStru*	comp, WORD* result);
void PHONE_MMIFastAccessReq(DWORD handle, struct MMI_FastAccessReqStru* fastreq, WORD* result);
void PHONE_MMISetupRsp(DWORD handle, WORD* result);
void PHONE_MMIUpdateMasterKey(DWORD handle, WORD* result);
void PHONE_MMIAccessRightsRsp(DWORD handle, struct MMI_AccessRightsRspStru* accessrsp, WORD* result);
void PHONE_MMIFastAccessRsp(DWORD handle, struct MMI_FastAccessRspStru* fastrsp, WORD* result);
void PHONE_SLRing(UCHAR * bd_addr); /* bd_addr used for registered incoming call */
void PHONE_SLRingStop(void);
UCHAR PHONE_GetTlBd(UCHAR *bd);

/*---------------------------------PROFILE MANIPULATOR---------------------------------*/
/* Launch CTP/ICP */
int Phone_Init(void);
/* Dump CTP/ICP */
int Phone_Done(void);
/* Get CTP/ICP Version */
const CHAR* PhoneGetVersion(void);

/* Register MMI Event Callback */
typedef void (* PHONECBK) (UCHAR event, UCHAR * handle, WORD wparam, DWORD lparam);
void PhoneAPP_RegCbk(DWORD handle, PHONECBK callback);

/*--------------------------------Register SDAP services-------------------------------*/
UCHAR* CTP_SvrStart(UCHAR* local_attrib);
DWORD CTP_SvrStop(UCHAR* handle);
UCHAR* CTP_ClntStart(UCHAR* bd_addr, UCHAR* sdp_attrib, UCHAR* local_attrib);
DWORD CTP_ClntStop(UCHAR* handle);
UCHAR* ICP_SvrStart(UCHAR* local_attrib);
DWORD ICP_SvrStop(UCHAR* handle);
UCHAR* ICP_ClntStart(UCHAR* bd_addr, UCHAR* sdp_attrib, UCHAR* local_attrib);
DWORD ICP_ClntStop(UCHAR* handle);

/*---------------------------------------------------------------------------------------*/

#endif	/* _PHONE_UI_H_  */
