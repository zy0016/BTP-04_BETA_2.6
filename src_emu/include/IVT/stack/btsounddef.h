/*++

Copyright (c) 1997-2000  IVT Corporation All Rights Reserved

Module Name:

    btsounddef.h

Abstract:

    

--*/

#ifndef _BTSOUND_H_
#define _BTSOUND_H_

//=============================================================================
// Defines
//=============================================================================

/*define message type*/

#define AUDIOMSGID_BASE					0

/*Audio message ID sent from driver to application*/
#define INVALID_MSG									(AUDIOMSGID_BASE+0) /* invalid message*/
#define DRV2UI_STREAM_DATA							(AUDIOMSGID_BASE+1) /* send render data msg to app*/
#define DRV2UI_STREAM_OPEN							(AUDIOMSGID_BASE+2) /* send open msg to app*/
#define DRV2UI_STREAM_FORMAT						(AUDIOMSGID_BASE+3) /* send stream format to app*/
#define DRV2UI_STREAM_CLOSE							(AUDIOMSGID_BASE+4) /* send close msg to app*/
#define DRV2UI_STREAMSTATE_STOP						(AUDIOMSGID_BASE+5) /* send stream state STOP to app*/
#define DRV2UI_STREAMSTATE_ACQUIRE					(AUDIOMSGID_BASE+6) /* send stream state ACQUIRE to app*/
#define DRV2UI_STREAMSTATE_PAUSE					(AUDIOMSGID_BASE+7) /* send stream state PAUSE to app*/
#define DRV2UI_STREAMSTATE_RUN						(AUDIOMSGID_BASE+8) /* send stream state RUN to app*/
#define DRV2UI_SET_VOLUME							(AUDIOMSGID_BASE+9) /* send stream volume value to app*/
#define DRV2UI_SET_MUTE								(AUDIOMSGID_BASE+10) /* send set mute state to app*/
#define DRV2UI_CLR_MUTE								(AUDIOMSGID_BASE+11) /* send clear mute state to app*/


/*Audio message ID sent from application to driver*/
#define UI2DRV_STREAM_OPEN_COMPLETE					(AUDIOMSGID_BASE+31) /* send stream open processing complete to drv*/
#define UI2DRV_STREAM_CLOSE_COMPLETE				(AUDIOMSGID_BASE+32) /* send stream close processing complete to drv*/
#define UI2DRV_BLUETOOTHSTREAM_PRESENT				(AUDIOMSGID_BASE+33) /* send BT connection state PRESENT to drv*/
#define UI2DRV_BLUETOOTHSTREAM_ABSENT				(AUDIOMSGID_BASE+34) /* send BT connection state ABSENT to drv*/
#define UI2DRV_STREAM_DATA							(AUDIOMSGID_BASE+35) /* send render stream data to drv*/
#define UI2DRV_APP_FINISH_INIT						(AUDIOMSGID_BASE+36) /* send application's finish event to drv when init is done*/
#define UI2DRV_STREAM_SUSPEND						(AUDIOMSGID_BASE+37) /* send stream suspend request to drv*/
#define UI2DRV_STREAM_RESUME						(AUDIOMSGID_BASE+38) /* send stream resume request to drv*/
#define UI2DRV_SET_AV_MODE							(AUDIOMSGID_BASE+39) /* set Blueletaudio AV work mode to drv*/
#define UI2DRV_SET_HEADSET_MODE						(AUDIOMSGID_BASE+40) /* set Blueletaudio Headset work mode to drv*/
#define UI2DRV_SETMODE_COMPLETE					(AUDIOMSGID_BASE+41)/*set AV mode or HS mode complete , inform system open stream again(power on)*/
#define UI2DRV_SET_HCTTEST_MODE					(AUDIOMSGID_BASE+100)/*set test mode to driver, for microsoft driver HCT test*/
#define UI2DRV_SET_HCTNORMAL_MODE					(AUDIOMSGID_BASE+101)/*set test mode to normal, don't test HCT*/


/*Audio message ID sent from driver to application(only for internal use)*/
#define DRV2UI_SET_NOTIFICATIONFREQ					(AUDIOMSGID_BASE+51) /* send notification frequency to app*/


#define AUDIOMSG_LENGTH			(1024*2)
#define AUDIOMSG_NUMBER			(32)


typedef struct tagWAVEFORMAT
{
    WORD    wFormatTag;        /* format type */
    WORD    nChannels;         /* number of channels (i.e. mono, stereo...) */
    DWORD   nSamplesPerSec;    /* sample rate */
    DWORD   nAvgBytesPerSec;   /* for buffer estimation */
    WORD    nBlockAlign;       /* block size of data */
    WORD    wBitsPerSample;    /* Number of bits per sample of mono data */
    WORD    cbSize;            /* The count in bytes of the size of
                                    extra information (after cbSize) */

} WAVEFORMAT_STRUCT;

typedef struct tagPCMDataStruct
{
	ULONG       length; 
	BYTE		data[1]; 
}STREAMDATA_STRUCT;

typedef struct tagVolumeSettingStruct
{
	LONG 		channel; /* Indicate channel number*/
	DWORD		db;	 /*channel's volume value*/
}VOLUMESETTING_STRUCT;

typedef struct tagNotificationFreqStruct
{
	ULONG		Interval; /* Interval value*/
    ULONG		FrameSize; /*PCM samples' size during the time of Interval*/
}NOTIFICATIONFREQ_STRUCT;


typedef struct tagAudioMsgStruct
{
	WORD		AudioMsgID;		/* message type */
	UCHAR		Capture;		/* if it's capture stream, it is TRUE, otherwise, zero.*/
	UCHAR 		Reserved;		/**/
	union
	{
		/*for DRV2UI_STEAM_DATA*/
		STREAMDATA_STRUCT 		streamdata;
		/*for DRV2UI_STREAM_OPEN and DRV2UI_STREAM_FORMAT*/
		WAVEFORMAT_STRUCT		format;		
		/*for DRV2UI_SET_VOLUME*/
		VOLUMESETTING_STRUCT	volumesetting;
		/*for DRV2UI_SET_NOTIFICATIONFREQ*/
		NOTIFICATIONFREQ_STRUCT notificationfreq;
	}content;
}AUDIOMSG_STRUCT, *PAUDIOMSG_STRUCT;

typedef struct tagSharedBufferStruct
{
	AUDIOMSG_STRUCT*    audiomsg_ptr;
	WORD				buffer_len;
	short				buffer_counter;
	UCHAR 				buffer_num;
	UCHAR				writeindex;		
	UCHAR				readindex;	
	UCHAR				oldmsgid;
}SHAREDBUFFER_STRUCT, *PSHAREDBUFFER_STRUCT;

typedef struct tagSharedBufferInfoStruct
{
	DWORD						sharebuffer_len;	
	SHAREDBUFFER_STRUCT* 		sharebuffer_addr;
}SHAREDBUFFERINFO_STRUCT, *PSHAREDBUFFERINFO_STRUCT;

#endif
