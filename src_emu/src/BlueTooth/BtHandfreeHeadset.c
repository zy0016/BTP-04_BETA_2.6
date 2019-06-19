#include "BtLowCall.h"
#include "BtMain.h"
#include "BtDeviceNode.h"
#include "ag_hfap.h"
#include "sdk_loc.h"
#include "me/me_wnd.h"
#include "BtThread.h"

#include "mullang.h"
#include "pubapp.h"
#include "window.h"
#include "BtHandfreeHeadset.h"
#include "sdk_def.h"

#define   MAX_VOL		12 //12   //15


static UCHAR g_spk_vol;
#define SETSPKVOL(f)        {if (f) INCVOL(g_spk_vol) else DECVOL(g_spk_vol)}
#define INCVOL(v)           {if (v < MAX_VOL) v++;}
#define DECVOL(v)           {if (v > 0) v--;}

typedef struct tagRegisterCallBackMsg{
	HWND hWnd;
	UINT Msg;
}REGISTER_CALLBACK_MSG;


static REGISTER_CALLBACK_MSG BtAnswerCall,BtCancelCall,BtVolUp,BtVolDown,
							BtHFHSConnected,BtHFHSDisconnect,BtHFHSAudioConnect,BtHFHSAudioDisconnect,
							BtHFDialCall;
static char HF_Dial_Phone_number[50];

static void BtStartPCM(void);

/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
void AGAPP_Event_Callback(BTUINT16 msgid, BTUINT8 *pArg, BTUINT32 dwArg)
{
//	UCHAR buf[31];
	UCHAR extend_cmd[257];

	//printf("AGAPP Event **** %04x ****\r\n", msgid);
	switch(msgid) {
		/*state changed indication*/
	case BTSDK_APP_EV_AGAP_STATE_CHANGED_IND:
		switch (*pArg) {
			/*before service level connection is established*/
		case AGAP_ST_IDLE:
			// do something here
			break;

			/*service level connection is established*/
		case AGAP_ST_STANDBY:
			// do somtheing here
			break;

			/*ringing*/
		case AGAP_ST_RINGING:
			//Actually,when we receive this event,the SCO link is connect between HFHS and mobilephone --axli
			//PostMessage(BtHFHSConnected.hWnd,BtHFHSConnected.Msg,0,0);
			break;

			/*outgoing call*/
		case AGAP_ST_OUTGOINGCALL:
			// do something here
			break;

			/*ongoing call*/
		case AGAP_ST_ONGOINGCALL:
			// do something here
			break;

			/*voice recognition is ongoing*/
		case AGAP_ST_BVRA:
			// do something here
			break;

			/*Audio Connection Transfer to HS*/
		case AGAP_ST_VOVG:
			// do something here
			break;

		default:
			break;

		}
		break;

		/*HF device is available (connection is established)*/
		//**********LIAOCHENG SDK**********//
		case BTSDK_APP_EV_AGAP_HF_AVAILABLE_IND:
			{
				struct Btsdk_AGAP_ConnInfo *sdk_info = (struct Btsdk_AGAP_ConnInfo *)pArg;
				BTDEVHDL hfp_hdl;
				BTUINT8 is_hsp;
				hfp_hdl = sdk_info->hdl;
				is_hsp = sdk_info->is_hsp;
				printf("===============蓝牙设备已经连接connected\r\n");
				printf("========Available service in remote device is %s\n", is_hsp ? "Headset" : "Hands-Free");
				PostMessage(BtHFHSConnected.hWnd,BtHFHSConnected.Msg,is_hsp,0); 
			}
			break;

		/*HF device is unavailable (connection is released or link lost)*/
		//**********LIAOCHENG SDK**********//
	case BTSDK_APP_EV_AGAP_HF_UNAVAILABLE_IND:
		/*
		-----may be link lost! so, need to dispose link lost event 
		accroding to the AG current state(ongoingcall etc.). (refer to CCAP 6.3.1)
		*/
		{
			PostMessage(BtHFHSDisconnect.hWnd,BtHFHSDisconnect.Msg,0,0);
		}
		break;

		/*the ongoing call is released by remote party*/
	case BTSDK_APP_EV_AGAP_RMT_RELEASE_CALL_IND:
		printf("========Remote release the call\r\n");
		/*remote party is busy during the outgong call procedure*/
		break;
	case BTSDK_APP_EV_AGAP_RMT_IS_BUSY_IND:
		/*-----HF should be able to hear busy tone if AG enables to	transfer busy tone over audio connection*/
		// doing something here
		printf("========remote busy\r\n");
		break;

		/*HF request to start voice recognition procedure*/
	case BTSDK_APP_EV_AGAP_VOICE_RECOG_ACTIVATE_REQ:
		/*-----need to activate voice recognition*/

		break;

		/*requests AG to deactivate the voice recognition procedure*/
	case BTSDK_APP_EV_AGAP_VOICE_RECOG_DEACTIVATE_REQ:
		/*-----need to deactivate voice recognition*/
		break;

		/*the incoming call is answered (by HF/HS or AG/VG)*/
	case BTSDK_APP_EV_AGAP_INCOMINGCALL_ANSWERED_IND:
		/*-----need to answer the call if this event is initiated by HF*/
		if (*pArg == AGAP_AG_ANSWERED_INCOMINGCALL)
			printf("========Mobile answered the incoming call!\n");
		else if (*pArg == AGAP_HF_ANSWERED_INCOMINGCALL)
		{
			//printf("\n>>HF answered the incoming call: need to establish the call with remote party here!\n");
			PostMessage(BtAnswerCall.hWnd,BtAnswerCall.Msg,0,0);
			printf("\r\n=======HandFree or Headset answer the call\r\n");
			printf("\r\n=======Begin Send BT_MSG_ANSWERCALL=%x to hwnd=%x\r\n",BtAnswerCall.Msg,BtAnswerCall.hWnd);
		}
        break;

		/*the incoming/outgoing/ongoing call is canceled (by HF/HS or AG/VG or network)*/
	case BTSDK_APP_EV_AGAP_CALL_CANCELED_IND:
		/*-----need to cancel the call if this event is initiated by HF*/
		if (*pArg == AGAP_AG_CANCELED_CALL)
		{
			//printf("\n>>Call Canceled Reason: AG cancel the call!\n");
			printf("============Mobile cancel the call\r\n");
		}
		else if (*pArg == AGAP_HF_CANCELED_CALL)
		{
			//printf("\n>>Call Canceled Reason: HF cancel the call! Need to do something here...\n");
			printf("\r\n===========Handfree or headset cancel the call\r\n");
			printf("\r\n===========Begin Send BT_MSG_CANCELCALL=%x to hwnd=%x\r\n",BtCancelCall.Msg,BtCancelCall.hWnd);
			PostMessage(BtCancelCall.hWnd,BtCancelCall.Msg,0,0);
		}
		else if (*pArg == AGAP_NETWORK_CANCELED_CALL)
		{
			printf("============Call Canceled Reason: Network event received!\n");
		}
		else if (*pArg == AGAP_TBUSY_CANCELED_CALL)
		{
			printf("=============Call Canceled Reason: Tbusy timeout!\n");
		}
		break;
		
		/*audio connection with HF is established*/
	case BTSDK_APP_EV_AGAP_AUDIO_CONN_ESTABLISHED_IND:
		/*-----need to do something here.*/
        //printf("\n>>AG_DBG: AGAP_EV_AUDIO_CONN_ESTABLISHED_IND\nHFP_AG>");
		printf("\r\n=========Audio connection with HF is established\r\n"); //SCO
		printf("PostMessage: hWnd=%x,Msg=%x\r\n",BtHFHSAudioConnect.hWnd,BtHFHSAudioConnect.Msg);
		//Call application can switch audio channel after receiving this message
		PostMessage(BtHFHSAudioConnect.hWnd,BtHFHSAudioConnect.Msg,0,0);
		break;

		/*audio connection with HF is released*/
	case BTSDK_APP_EV_AGAP_AUDIO_CONN_RELEASED_IND:
		/*-----need to do something here.*/
		//printf("\n>>AG_DBG: AGAP_EV_AUDIO_CONN_RELEASED_IND\nHFP_AG>");
		printf("\r\n=========audio connection with HF is released\r\n");
		printf("PostMessage hWnd=%x,Msg=%x\r\n",BtHFHSAudioDisconnect.hWnd,BtHFHSAudioDisconnect.Msg);
		PostMessage(BtHFHSAudioDisconnect.hWnd,BtHFHSAudioDisconnect.Msg,0,0);
		break;

		/*instructs AG to dial the specific phone number*/
	case BTSDK_APP_EV_AGAP_HF_DIAL_IND:
		/*-----need to place a call with the phone number supplied by the HF device here.*/
		/*-----if pArg == NULL then this event means HS originate outgoing call (may be last number redial or voice recognition.)*/
		if (!pArg) {
			//printf("\n>>AG_DBG: Headset Originate outgoing call\n");
			printf("Error: headset want to dial a call\r\n");
		}
	    else {
		    //printf("\n>>AG_DBG: Hands-Free device Originate phone number dial\n");
			printf("============Handfree dial a call\r\n");
		    if (dwArg < sizeof(HF_Dial_Phone_number)) {
			    memcpy(HF_Dial_Phone_number, pArg, dwArg);
			    HF_Dial_Phone_number[dwArg] = 0;
				printf("==========The outgoing phone number=%s\n",HF_Dial_Phone_number);
				PostMessage(BtHFDialCall.hWnd,BtHFDialCall.Msg,MAKEWPARAM(dwArg,0),(LPARAM)HF_Dial_Phone_number);
			}
			else {
                printf("============ the phone number is too long when handfree dial a call");
			}
		}
		break;

		/*instructs AG to dial the phone number indexed by the specific memory location of SIM card*/
	case BTSDK_APP_EV_AGAP_HF_MEM_DIAL_IND:
		/*-----need to memory dialing here.*/
		//printf("\n>>AG_DBG: AGAP_EV_HF_MEM_DIAL_IND\n");
		if (dwArg <sizeof(HF_Dial_Phone_number)) {
			int iNum;
			memcpy(HF_Dial_Phone_number, pArg, dwArg);
			HF_Dial_Phone_number[dwArg] = 0;
			iNum=atoi(HF_Dial_Phone_number);
			printf("================Dial NO.%d phone number in SIM card\r\n",iNum);
			PostMessage(BtHFDialCall.hWnd,BtHFDialCall.Msg,MAKEWPARAM(iNum,1),(LPARAM)NULL);
			//printf(">>AG_DBG: the memory location is %s\nHFP_AG>", buf);
		}
		else 
            printf("\n>>AG_DBG: the memory location is too long\nHFP_AG>");
		break;

		/*instructs AG to redial the last dialed phone number*/
	case BTSDK_APP_EV_AGAP_HF_LASTNUM_REDIAL_IND:
		/*-----need to redial the last dialed phone number here.*/
		//printf("\n>>AG_DBG: AGAP_EV_HF_LASTNUM_REDIAL_IND\nHFP_AG>");
		printf("========Redail the last call from Handfree\r\n");
		PostMessage(BtHFDialCall.hWnd,BtHFDialCall.Msg,MAKEWPARAM(NULL,2),(LPARAM)NULL);
		break;

		/*set the current indicator during the service level connection initialization procedure*/
	case BTSDK_APP_EV_AGAP_CUR_INDICATOR_VAL_REQ:
        /*
		-----need to call this function depending on the current state of AG (standby, 
		ongoing call, outgoing call or incoming call; service available or service unavailable).
		*/ 
        Btsdk_AGAP_SetCurIndicatorVal(AGAP_INDICATOR_SVC_AVAILABLE);  /*call: 0; callsetup: 0; service: 1*/
		break;

		/*instructs AG to transmit the specific DTMF code*/
	case BTSDK_APP_EV_AGAP_DTMF_IND:
		/*-----need to transmit the received DTMF code.*/
		//printf("\n>>AG_DBG: The received DTMF is %c\nHFP_AG>", *pArg);
		printf("=================DTMF from Handfree\r\n");
		PostMessage(BtHFDialCall.hWnd,BtHFDialCall.Msg,MAKEWPARAM(*pArg,3),(LPARAM)NULL);
		break;

		/*speaker volume of HF device has changed*/
	case BTSDK_APP_EV_AGAP_SPKVOL_CHANGED_IND:
		{
			/*-----need to record the speaker volume level of HF device, maybe it is needed to change speaker volume of the AG*/
			
			int diff;
			printf("==============The speaker volume has changed from %d to %d\nHFP_HF>", g_spk_vol, *pArg);
			
			if( *pArg >MAX_VOL)
			{
				*pArg=MAX_VOL;
				printf("=============Set the VOL to 12\r\n");
				Btsdk_AGAP_SetSpkVol(MAX_VOL);
			}
			else if( *pArg<0)
				*pArg=0;

			diff=*pArg-g_spk_vol;
			g_spk_vol=*pArg;

			if(diff>0)
				PostMessage(BtVolUp.hWnd,BtVolUp.Msg,diff,0);
			else
				PostMessage(BtVolDown.hWnd,BtVolDown.Msg,-diff,0);
		}
		// why there is no interface to get vol of handfree
		break; 

//		/*requests AG to input a phone number for the voice-tag at the HF side*/
//	case BTSDK_APP_EV_AGAP_VOICETAG_PHONE_NUM_REQ:
//        /*-----need to indicate user to input phone number for the voice-tag at the HF device side, and it is allowed to input 32 digits at most.*/
//        printf("\nHFP_AG>Input Phone number:123456789\nHFP_AG>\n");     /*demo!*/
//		Btsdk_AGAP_VoiceTagPhoneNumRsp("\"123456789\"", 11);
//		break;

//		/*requests AG to disenable NREC function*/
//	case BTSDK_APP_EV_AGAP_NREC_DISENABLE_IND:
//        /*-----need to disable NREC here.*/
//		break;

		/*generate the in-band ring tone*/
	case BTSDK_APP_EV_AGAP_GENERATE_INBAND_RINGTONE_IND:
		printf("============Generate in-band ring tone!\nHFP_AG>\r\n");
		/*-----need to generate in-band ring tone here.*/
		break;

		/*terminate the in-band ring tone*/
	case BTSDK_APP_EV_AGAP_TERMINATE_INBAND_RINGTONE_IND:
		printf("============Terminate in-band ring tone!\nHFP_AG>\r\n");
   	    /*-----need to terminate its in-band ring tone here!*/
		break;

		/*cellular network is available*/
	case BTSDK_APP_EV_AGAP_NETWORK_AVAILABLE_IND:
		printf("============Cellular network service is available!\nHFP_AG>");
		break;

		/*cellular network is unavailable*/
	case BTSDK_APP_EV_AGAP_NETWORK_UNAVAILABLE_IND:
	    /*-----HF should be able to hear busy tone if AG enables to	transfer busy tone over audio connection
		when in OngoingCall or OutgoingCall state. (refer to CCAP 6.3.2; 6.3.3; 6.3.4)
		*/
		printf("===============Cellular network service is unavailable!\nHFP_AG>");
		break;
		
		/*extend command is received*/
	case BTSDK_APP_EV_AGAP_EXTEND_CMD_IND:
		/*-----need to deal with the extended AT Command here.*/
	    memcpy(extend_cmd, pArg, dwArg);
	    extend_cmd[dwArg] = 0;
	    printf("\n>>Extended ATCommand: %s\nHFP_AG>", extend_cmd);
	    Btsdk_AGAP_ExtendCmd("\r\nERROR\r\n");
		break;

//		/*get paired device's baseband address*/
//		//**********LIAOCHENG SDK**********//
//	case AGAP_EV_PAIRED_DEV_BDADDR_REQ:
//		//Btsdk_GetRemoteDeviceBDAddr(g_hfp_hdl, pArg);
//		break;

	default:
		break;
	}
}
/*********************************************************************\
* Function   HFAG_Init	   
* Purpose    initialize bt stack for handfree and headset,set the handfree
*			 ,handset callback function.   
* Params	   
* Return	 	   
* Remarks	 HF -- short for handfree  AG -- Audio gateway,i.e. Mobile phone 
**********************************************************************/
int HFAG_Init()
{
	UINT ret = BTSDK_OK;

	Btsdk_AGAP_APPRegCbk((Btsdk_AGAP_Event_Ind_Func *)AGAPP_Event_Callback);

	/*Initializing AG Device (including starting AG server)*/
	ret = Btsdk_AGAP_Init(/*BTSDK_AGAP_FEA_NREC*/ /*| BTSDK_AGAP_FEA_VOICE_RECOG */
		/*|*/ /*BTSDK_AGAP_FEA_INBAND_RING*/ /*|*/ BTSDK_AGAP_FEA_VOICETAG_PHONE_NUM 
		| BTSDK_AGAP_FEA_REJ_CALL, 	BTSDK_AGAP_SCO_PKT_HV1 | AGAP_SCO_PKT_HV2 
		| AGAP_SCO_PKT_HV3	, NULL);

	if (ret == BTSDK_OK)
	{
		BtStartPCM();
		printf("Audio Gateway initialization succeed!\n");
	}
	else
	{
		printf("Audio Gateway initialization failed!\n");
		return 0;
	}

	return ret;
}
/*********************************************************************\
* Function	   BtStartPCM
* Purpose      Send HCI command to bt chip to set BT PCM as slave
*			   and pin DA for out, DB for in
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void BtStartPCM(void)
{
		BtSdkVendorCmdStru Cmd;
		BtSdkEventParamStru *out_ev=malloc(sizeof(BtSdkEventParamStru)+10);
		int ret;

		//printf("====Set PCM slave,DA out,DB in\r\n");
		out_ev->param_len = 10;
		Cmd.ocf=0x07;
		Cmd.param_len=1;
		Cmd.param[0]=0x01/*0x03*/;
		ret=Btsdk_VendorCommand(0,&Cmd,out_ev);

		out_ev->param_len = 10;
		Cmd.ocf=0x28;
		Cmd.param_len=1;
		Cmd.param[0]=0x0;
		ret=Btsdk_VendorCommand(0,&Cmd,out_ev);

		free(out_ev);
}


/***************************************************************************
*
*			Extern interface for Call Application
*
****************************************************************************/
BOOL BT_RegisterNotifyMsg(int type,HWND hWnd,UINT message)
{
	switch(type) {
	case BT_ANSWER_CALL:
		BtAnswerCall.hWnd=hWnd;
		BtAnswerCall.Msg=message;
		break;
	case BT_CANCEL_CALL:
		BtCancelCall.hWnd=hWnd;
		BtCancelCall.Msg=message;
		break;
	case BT_VOL_UP:
		BtVolUp.hWnd=hWnd;
		BtVolUp.Msg=message;
		break;
	case BT_VOL_DOWN:
		BtVolDown.hWnd=hWnd;
		BtVolDown.Msg=message;
		break;
	case BT_HFHS_CONNECTED:
		BtHFHSConnected.hWnd=hWnd;
		BtHFHSConnected.Msg=message;
		break;
	case BT_HFHS_DISCONNECTED:
		BtHFHSDisconnect.hWnd=hWnd;
		BtHFHSDisconnect.Msg=message;
		break;
	case BT_HFHS_AUDIO_CONNECTED:
		BtHFHSAudioConnect.hWnd=hWnd;
		BtHFHSAudioConnect.Msg=message;
		break;
	case BT_HFHS_AUDIO_DISCONNECTED:
		BtHFHSAudioDisconnect.hWnd=hWnd;
		BtHFHSAudioDisconnect.Msg=message;
		break;
	case BT_HF_DAIL_CALL:
		BtHFDialCall.hWnd=hWnd;
		BtHFDialCall.Msg=message;
	default:
		break;
	}
	return TRUE;
}

//----------------Interface related to incoming  event ----------//
void BT_CopyRingToBtDevice(char *IncomingCallNumber)
{
	BT_SendRequest(NULL,0,RT_BT_COPYRING,IncomingCallNumber,NULL,0);
}
void  BT_MobileAnswerCall()
{
	Btsdk_AGAP_AnswerCall();
}
void BT_MobileRejectCall()
{
	Btsdk_AGAP_CancelCall();
}

//--------------------------Network event---------------------//
void BT_NetworkUnavailable()    //for incoming,ongoing,outgoing
{
	Btsdk_AGAP_NetworkEvent(AGAP_NETWORK_SVC_UNAVAILABLE,NULL);
}
void BT_RemoteAnswerCall()		//for outgoing
{
	Btsdk_AGAP_NetworkEvent(AGAP_NETWORK_RMT_ANSWER_CALL,NULL);
}
void BT_RemoteTerminateCall()  //for incoming,ongoing,outgoing call
{
	Btsdk_AGAP_NetworkEvent(AGAP_NETWORK_LINK_NOT_ESTABLISHED,NULL);
}
void BT_RemoteAlert()		 //for outgoing
{
	Btsdk_AGAP_NetworkEvent(AGAP_NETWORK_ALERTING_RMT,NULL);
}
//for outgoing call and incoming call. 
//对呼出的几种情况:号码不存在，无拨号音，远程拒绝,都可调用此接口
void BT_LinkNotEstablished() 
{
	Btsdk_AGAP_NetworkEvent(AGAP_NETWORK_LINK_NOT_ESTABLISHED,NULL);
}
//---------------interface for dial a call -------------------//
void BT_DialCall()	  //outgoing call
{
	Btsdk_AGAP_OriginateCall();
}
void BT_RemoteBusy() //outgoing call
{
	Btsdk_AGAP_NetworkEvent(AGAP_NETWORK_RMT_IS_BUSY,NULL);
}

//---------------interface for Vol+/Vol-  in stanby mode and ongoing mode
void BT_VolUp()
{
	SETSPKVOL(1);
	Btsdk_AGAP_SetSpkVol(g_spk_vol);
}
void BT_VolDown()
{
	SETSPKVOL(0);
	Btsdk_AGAP_SetSpkVol(g_spk_vol);
}

//-------------interface for ongoing call -------------------//
void BT_SwitchVoiceChannel()
{
	//Btsdk_AGAP_AnswerCall();
	Btsdk_AGAP_AudioConnTrans();  //This interface will connect/disconnect SCO link alternately
}

void BT_MobileHangUpCall()		  //for incoming call,onging call,outgoing call
{
	Btsdk_AGAP_CancelCall(); 
}



