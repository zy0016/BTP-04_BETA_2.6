/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : sphdec.h
 *
 * Purpose  : Declare functions of speech decoder.
 *
\**************************************************************************/

#ifndef __SPHDEC_H
#define __SPHDEC_H

/***************************************************************************
* For avoiding mulit-definition error due to confliction with the library 
* from the third party
* Added by Wang Wenchao (2004-04-21)
****************************************************************************/

#define Speech_Decode_Frame_init  HP_Speech_Decode_Frame_init
#define Speech_Decode_Frame_exit  HP_Speech_Decode_Frame_exit
#define Speech_Decode_Frame       HP_Speech_Decode_Frame
#define Speech_Decode_Frame_reset HP_Speech_Decode_Frame_reset

// definition of modes for decoder

enum Mode
{
	MR475 = 0,
    MR515,
    MR59,
    MR67,
    MR74,
    MR795,
    MR102,
    MR122,
    MRDTX,
    N_MODES     /* number of (SPC) modes */
};

// Declaration received frame types

enum RXFrameType
{
	RX_SPEECH_GOOD = 0,
	RX_SPEECH_DEGRADED,
	RX_ONSET,
	RX_SPEECH_BAD,
	RX_SID_FIRST,
	RX_SID_UPDATE,
	RX_SID_BAD,
	RX_NO_DATA,
	RX_N_FRAMETYPES     /* number of frame types */
};

// initialize one instance of the speech decoder
void* Speech_Decode_Frame_init();

// free status structure
void Speech_Decode_Frame_exit(void **st);

// Decodes one frame from encoded parameters
void Speech_Decode_Frame(void *st, enum Mode mode, short *serial,
         enum RXFrameType frame_type, short *synth);

// reset speech decoder
int Speech_Decode_Frame_reset(void **st);

#endif // __SPHDEC_H
