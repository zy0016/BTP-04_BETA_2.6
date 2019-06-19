/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : amrdec.h
 *
 * Purpose  : Declare interfaces to the kernel AMR decoder.
 *
\**************************************************************************/

#ifndef __AMRDEC_H
#define __AMRDEC_H

/****************************************************************************
* For avoiding mulit-definition error due to confliction with the library 
* from the third party
* Added by Wang Wenchao (2004-04-21)
****************************************************************************/

#define Decoder_Interface_init   HP_Decoder_Interface_init
#define Decoder_Interface_exit   HP_Decoder_Interface_exit
#define Decoder_Interface_Decode HP_Decoder_Interface_Decode

void *Decoder_Interface_init(void);

void Decoder_Interface_exit(void *state);

void Decoder_Interface_Decode(void *st,

#ifndef ETSI
      unsigned char *bits,
#else
      short *bits,
#endif

      short *synth, int bfi);

#endif // __AMRDEC_H
