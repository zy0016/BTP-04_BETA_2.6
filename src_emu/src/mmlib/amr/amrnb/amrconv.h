/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : amrconv.h
 *
 * Purpose  : Declare structures of converter and functions.
 *
\**************************************************************************/

#ifndef __AMRCONV_H
#define __AMRCONV_H

#define NONE 255

/***************************************************************************
* For avoiding mulit-definition error due to confliction with the library 
* from the third party
* Added by Wang Wenchao (2004-04-21)
****************************************************************************/

#define init_nibble_reader     HP_init_nibble_reader
#define read_nibble_reader     HP_read_nibble_reader
#define init_nibble_writer     HP_init_nibble_writer
#define write_nibble_writer    HP_write_nibble_writer
#define tostring_nibble_writer HP_tostring_nibble_writer
#define flip_bitorder          HP_flip_bitorder
#define nybflip                HP_nybflip
#define bytflip                HP_bytflip
#define makefliptab            HP_makefliptab

typedef struct _nibble_reader
{
	unsigned char data[32];
	unsigned int count;
	unsigned int i;
} nibble_reader;

typedef struct _nibble_writer
{
	unsigned char out[32];
	unsigned int i;
} nibble_writer;

void init_nibble_reader(nibble_reader *self, unsigned char *in,
						unsigned int count);

unsigned char read_nibble_reader(nibble_reader *self);

void init_nibble_writer(nibble_writer *self);

void write_nibble_writer(nibble_writer *self, unsigned char n);

unsigned char *tostring_nibble_writer(nibble_writer *self);

unsigned char *flip_bitorder(unsigned char *b, unsigned int count);

unsigned char nybflip(unsigned char n);

unsigned char bytflip(unsigned char n);

void makefliptab();

#endif // __AMRCONV_H
