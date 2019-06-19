/***************************************************************************
Module Name:
    a2dpfunc.h
Abstract:
	This file provides a2dp user interface functions.
Author:
    Lu Dongmei
Revision History:
	2002.11.29	Created

---------------------------------------------------------------------------*/

#ifndef A2DPFUNC_H
#define A2DPFUNC_H


/*---------------SEP Maintaining Procedures----------------------*/

WORD A2DP_SEPRegister(
		UCHAR 		sep_type, 
		UCHAR 		codec_type, 
		WORD		media_chnl_mtu, 
		UCHAR 		*local_seid);

WORD A2DP_SEPSetCodecCaps(
		UCHAR 		local_seid, 
		UCHAR 		*codec_caps);

WORD A2DP_SEPGetCodecCaps(
		UCHAR 		local_seid, 
		UCHAR 		*codec_caps);

WORD A2DP_GetLocalTransportCaps(
		WORD 		*trans_mask);

WORD A2DP_SEPSetLocalCaps(
		UCHAR 		local_seid, 
		WORD 		caps_type, 
		WORD 		caps_mask);

WORD A2DP_SEPGetLocalCaps(
		UCHAR 		local_seid, 
		WORD 		caps_type, 
		WORD 		*caps_mask);

WORD A2DP_SEPDestroy(
		UCHAR 		local_seid);

UCHAR A2DP_RegCbk(
		A2DP_CallBack 	*pfunc);

/*----------------------Signaling Procedures----------------------*/
WORD A2DP_SEPCreate(
		struct A2DP_LocalSEPAttribStru 	*sep_info);

WORD A2DP_SEPGetRemInfo(
		UCHAR 						*rem_bd, 
		struct A2DP_SEPRemInfoStru  	*rem_sep_info,
		UCHAR 						*number);

WORD A2DP_SEPCapsSelect(
		UCHAR 						sep_type, 
		UCHAR 						rem_sep_num, 
		struct A2DP_SEPRemInfoStru  	*rem_sep_info,
		struct A2DP_SetConfigStru 		*sep_config); 

UCHAR * A2DP_StreamCreate(
		struct A2DP_SetConfigStru 		*sep_config) ;

DWORD A2DP_Init (void);
void A2DP_Done (void);

WORD A2DP_SEPDiscovery(
		UCHAR 						*rem_bd, 
		UCHAR 						*number, 
		struct A2DP_SEPRemInfoStru 	*rem_sep);

WORD A2DP_SEPGetRemoteCaps(
		UCHAR 							*rem_bd, 
		UCHAR 							rem_seid, 
		struct A2DP_SEPRemCapsInfoStru 	*rem_sepcaps);

STREAMHANDLE A2DP_SEPSetConfiguration(
		struct A2DP_SetConfigStru 			*sep_config);

WORD A2DP_StreamGetConfiguration(
		STREAMHANDLE 				handle, 
		struct A2DP_GetConfigStru 		*sep_config);

WORD A2DP_StreamOpen(
		STREAMHANDLE 	handle); 

WORD A2DP_StreamStart(
		STREAMHANDLE 	handle); 

WORD A2DP_StreamSuspend(
		STREAMHANDLE 	handle);

WORD A2DP_StreamReconfigure(
		STREAMHANDLE 			handle, 
		struct A2DP_ReconfigStru	*stream_reconfig);

WORD A2DP_StreamAbort(
		STREAMHANDLE 	handle);

WORD A2DP_StreamRelease(
		STREAMHANDLE 	handle);

/*----------------------Audio Streaming Procedure----------------------*/

WORD A2DP_StreamWrite (
		STREAMHANDLE 		handle, 
		struct BuffStru 		*buf, 
		WORD 				len, 
		WORD 				*nwritten);  /* just for SRC */

WORD A2DP_StreamRead(
		STREAMHANDLE 		handle, 
		DWORD	 			timeout, 
		struct A2DP_DataStru 	*data); /* just for SNK */

void A2DP_CallbackDispatch(
		UCHAR 		event, 
		UCHAR 		*param);
#endif
