#ifndef __BTHANDFREEHEADSET_H
#define __BTHANDFREEHEADSET_H

//Callback message type to Call application
#define BT_ANSWER_CALL					0x0
#define BT_CANCEL_CALL					0x1
#define BT_VOL_UP						0x2
#define BT_VOL_DOWN						0x3
#define BT_HFHS_CONNECTED				0x4
#define BT_HFHS_DISCONNECTED			0x5
#define BT_HFHS_AUDIO_CONNECTED			0x6
#define BT_HFHS_AUDIO_DISCONNECTED		0x7
#define BT_HF_DAIL_CALL					0x8

BOOL BT_RegisterNotifyMsg(int type,HWND hWnd,UINT message);

void BT_CopyRingToBtDevice(char *IncomingCallNumber);
void  BT_MobileAnswerCall();
void BT_MobileRejectCall();
void BT_RemoteTerminateCall();
void BT_NetworkUnavailable();

void BT_RemoteAnswerCall();		//for outgoing
void BT_RemoteTerminateCall();	//for incoming,ongoing,outgoing call
void BT_RemoteAlert();		 //for outgoing

//for outgoing call and incoming call. 
//对呼出的几种情况:号码不存在，无拨号音，远程拒绝,都可调用此接口
void BT_LinkNotEstablished(); 

//---------------interface for dial a call -------------------//
void BT_DialCall();		//outgoing call
void BT_RemoteBusy();	//outgoing

//---------------interface for Vol+/Vol-  in stanby mode and ongoing mode
void BT_VolUp();
void BT_VolDown();

//-------------interface for ongoing call -------------------//
void BT_SwitchVoiceChannel();
void BT_MobileHangUpCall();

#endif

