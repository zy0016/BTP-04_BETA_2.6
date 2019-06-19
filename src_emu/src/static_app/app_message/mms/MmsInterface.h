/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : MMS
 *
 * Purpose  : 封装一层mms模块用到的其他模块的接口函数声明
 *            
\**************************************************************************/

#ifndef _MMS_INTERFACE_H
#define _MMS_INTERFACE_H

void MMS_Setindbar(int nUnread, int nUnreadReport, int nUnconfirmed, 
                   int nUnreceive, int nTotalnum);
/******************************************************************** 
* Function	   mu_newmsg_ring
* Purpose      play ring when receive a new msg
* Params	   
* Return	 	void
**********************************************************************/
void mu_newmsg_ring(char* pFrom);

/*if bRingIsEnd = TURE，the ring stoped normaly，
if bRingIsEnd = FALSE,the ring was interrupted by other things，
the second parameter: no use*/
typedef void (CALLBACK* STOPRINGPROC)( BYTE bRingIsEnd);
/*********************************************************************\
* Function     MMS_PlayAudio
* Purpose      play audio
* Params       audioFileName: the file name of audio file
               audioType:     the audio type
               audioBody:     the data of audio
               audioSize:     the length of the audio data
* Return        void
**********************************************************************/
#if 0
BOOL MMS_PlayAudio (char *audioFileName, int audioType, const char *audioBody, 
					int audioSize, STOPRINGPROC pCallBackFun);
#endif

BOOL MMS_PlayAudio (HWND hWnd, char *audioFileName, int audioType, const char *audioBody, 
					int audioSize, STOPRINGPROC pCallBackFun);

/******************************************************************** 
* Function	   MMS_StopPlay
* Purpose      stop a ring
**********************************************************************/
void MMS_StopPlay(void);
/******************************************************************** 
* Function	   MMS_FindNameInPB
* Purpose      find the name in contacts according to phone number and email
* Params	   PCSTR address: mobil or email
               name: the found name
               nAddrType:
               #define ADDR_UNKNOWN        0
               #define ADDR_MOBIL          1
               #define ADDR_EMAIL          2
* Return	   Bool :true:found false:not found
**********************************************************************/
BOOL MMS_FindNameInPB(PCSTR address, char* name, int nAddType);

/******************************************************************** 
* Function	   MMS_SaveAddrToPB
* Purpose      save the address to pb
* Params	   hwnd, address
* Return	   BOOL
**********************************************************************/
BOOL MMS_SaveAddrToPB(HWND hWnd, PCSTR szAddress);

#endif
