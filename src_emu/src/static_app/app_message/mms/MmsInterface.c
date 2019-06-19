/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : MMS
 *            
\**************************************************************************/

#include "MmsGlobal.h"
#include "PreBrow.h"
#include "setting.h"

/*the interface to play music
uiringselect:
iCount:play times,0:play round,!0:play times
*/
extern BOOL	Call_PlayMusic(MUSICTYPE uiringselect,unsigned int icount);
extern void StopRingCallBackFun( BYTE bRingIsEnd);
extern void StopOBJRingCallBackFun( BYTE bRingIsEnd );

/******************************************************************** 
* Function	   mu_newmsg_ring
* Purpose      play ring when receive a new msg
* Params	   
* Return	 	void
**********************************************************************/
void mu_newmsg_ring(char* pFrom)
{
	printf("mu_newmsg_ring   type = %d", PRIOMAN_PRIORITY_MMSAPP);

	PrioMan_CallMusicEx(PRIOMAN_PRIORITY_MMSAPP, 3000);
}
/*if bRingIsEnd = TURE£¬the ring stoped normaly£¬
if bRingIsEnd = FALSE,the ring was interrupted by other things£¬
the second parameter: no use*/
typedef void (CALLBACK* STOPRINGPROC)( BOOL bRingIsEnd, DWORD);
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
					int audioSize, STOPRINGPROC pCallBackFun)
{
    BOOL bRet = FALSE;

    switch(audioType) 
    {
    case AUDIO_AMR:        
		Call_PlayMusic(RING_OTHER, 1);
        break;
    case AUDIO_WAV:
    case AUDIO_MMF:
    case AUDIO_MIDI:
//    case AUDIO_MP3:
#ifndef _EMULATE_
//        bRet = SEF_PlayRingFromData(audioType, 1, (BYTE*)audioBody, audioSize, 
//            pCallBackFun);
#endif
        break;
    default:
        break;
    }

    return bRet;
}
#endif
BOOL MMS_PlayAudio (HWND hWnd, char *audioFileName, int audioType, const char *audioBody, 
					int audioSize, STOPRINGPROC pCallBackFun)
{
	PM_PlayMusic pm;

	switch(audioType) 
    {
    case AUDIO_AMR: 
		pm.nRingType = PRIOMAN_RINGTYPE_AMR;
		break;

	case AUDIO_MIDI:
		pm.nRingType = PRIOMAN_RINGTYPE_MIDI;
		break;

	case AUDIO_WAV:
		pm.nRingType = PRIOMAN_RINGTYPE_WAVE;
		break;

	case AUDIO_MMF:
		pm.nRingType = PRIOMAN_RINGTYPE_MMF;
		break;
	}

	pm.hCallWnd = hWnd;
	pm.nPriority = PRIOMAN_PRIORITY_MMSAPP;
	pm.nVolume = PRIOMAN_VOLUME_DEFAULT;
	pm.nRepeat = 1;
	pm.pMusicName = NULL;
	pm.pDataBuf = (unsigned char*)audioBody;
	pm.nDataLen = audioSize;

	if (PrioMan_PlayMusic(&pm) != PRIOMAN_ERROR_SUCCESS)
		return FALSE;
	
	return TRUE;
}

/******************************************************************** 
* Function	   MMS_StopPlay
* Purpose      stop a ring
**********************************************************************/

#if 0
void MMS_StopPlay(void)
{
    Setup_EndPlayMusic(RING_OTHER);
}
#endif

void MMS_StopPlay(void)
{
    PrioMan_EndPlayMusic(PRIOMAN_PRIORITY_MMSAPP);
}

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
BOOL MMS_FindNameInPB(PCSTR address, char* name, int nAddType)
{
    BOOL        bRet = FALSE;
    BOOL        bEmail = FALSE;
    ABNAMEOREMAIL  pbname;
	char		sztemp[513];

    memset(&pbname, 0, sizeof(ABNAMEOREMAIL));

    if (nAddType == ADDR_UNKNOWN)
    {
        if (strchr(address, '@') != NULL)
            bEmail = TRUE;
        else
            bEmail = FALSE;
    }
    else if (nAddType == ADDR_MOBIL)
        bEmail = FALSE;
    else if (nAddType == ADDR_EMAIL)
        bEmail = TRUE;

	UTF8ToMultiByte(CP_ACP, 0, address, -1, sztemp, 512, NULL, NULL);

    if (!bEmail)
        bRet = APP_GetNameByPhone(sztemp, &pbname);
	else
		bRet = APP_GetNameByEmail(sztemp, &pbname);

	strcpy(name, pbname.szName);

    return bRet;
}

/******************************************************************** 
* Function	   MMS_SaveAddrToPB
* Purpose      save the address to pb
* Params	   hwnd, address
* Return	   BOOL
**********************************************************************/
BOOL MMS_SaveAddrToPB(HWND hWndFrame, HWND hWnd, PCSTR szAddress)
{
	ABNAMEOREMAIL  ab;

	ab.nType = AB_NUMBER;
	if (strchr(szAddress, '@') != NULL)
		ab.nType = AB_EMAIL;
	ab.szName[0] = 0;
	strcpy(ab.szTelOrEmail, szAddress);
	APP_SaveToAddressBook(hWndFrame, hWnd,0, &ab, AB_NEW);
	return FALSE;
}

//?
void MMS_Setindbar(int nUnread, int nUnreadReport, int nUnconfirmed, 
                   int nUnreceive, int nTotalnum)
{
   // MMSNotify(nUnread, (nTotalnum >= MAX_MSG_NUM));
}
