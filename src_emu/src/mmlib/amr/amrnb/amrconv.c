/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : amrconv.c
 *
 * Purpose  : Define functions of convertor.
 *
\**************************************************************************/

#include "amrconv.h"

int fliptab[256];

int Framebits[] = {95,103,118,134,148,159,204,244,39,0,0,0,0,0,0,0};

/*********************************************************************\
* Function	   void init_nibble_reader(nibble_reader *self,
*						unsigned char *in, unsigned int count)
* Purpose      Initializes nibble reader.
* Params	   self, in, count
* Return	   
* Remarks	   
**********************************************************************/

void init_nibble_reader(nibble_reader *self,
                        unsigned char *in, unsigned int count)
{
    unsigned int i;
    
    self->i = 0;
    self->count = count;
    for (i = 0; i < count; i ++)
        self->data[i] = in[i];
}

/*********************************************************************\
* Function	   unsigned char read_nibble_reader(nibble_reader *self)
* Purpose      Reads nibble reader.
* Params	   self
* Return	   
* Remarks	   
**********************************************************************/

unsigned char read_nibble_reader(nibble_reader *self)
{
    unsigned char c;
    
    if ((self->i) / 2 > self->count)
        return NONE;
    
    c = self->data[(self->i) / 2];
    if ((self->i) % 2)
        c >>= 4;
    c &= 0xF;
    (self->i) += 1;
    
    return c;
}

/*********************************************************************\
* Function	   void init_nibble_writer(nibble_writer *self)
* Purpose      Initializes nibble writer.
* Params	   self
* Return	   
* Remarks	   
**********************************************************************/

void init_nibble_writer(nibble_writer *self)
{
    int i;
    
    self->i = 0;
    for (i = 0; i < 32; i ++)
        self->out[i] = 0;
}

/*********************************************************************\
* Function	   void write_nibble_writer(nibble_writer *self,
*						unsigned char n)
* Purpose      Write nibble writer.
* Params	   self, writer
* Return	   
* Remarks	   
**********************************************************************/

void write_nibble_writer(nibble_writer *self, unsigned char n)
{
    if((self->i) % 2)
        self->out[(self->i) / 2]
        = (self->out[(self->i) / 2]) | (n << 4);
    else
        self->out[(self->i) / 2] = n;
    
    (self->i) += 1;
}

/*********************************************************************\
* Function	   unsigned char *tostring_nibble_writer(nibble_writer *self)
* Purpose      Output string in the nibble writer.
* Params	   self
* Return	   
* Remarks	   
**********************************************************************/

unsigned char *tostring_nibble_writer(nibble_writer *self)
{
    return self->out;
}

/*********************************************************************\
* Function	   unsigned char *flip_bitorder(unsigned char *b,
*					unsigned int count)
* Purpose      Flip bit order in each byte.
* Params	   b, count
* Return	   
* Remarks	   
**********************************************************************/

unsigned char *flip_bitorder(unsigned char *b, unsigned int count)
{
    unsigned int i;
    
    for (i = 0; i < count; i ++)
        b[i] = fliptab[b[i]];
    
    return b;
}

/*********************************************************************\
* Function	   unsigned char nybflip(unsigned char n)
* Purpose      
* Params	   n
* Return	   
* Remarks	   
**********************************************************************/

unsigned char nybflip(unsigned char n)
{
    return ((n & 1) << 3) + ((n & 2) << 1)
        + ((n & 4) >> 1) + ((n & 8) >> 3);
}

/*********************************************************************\
* Function	   unsigned char bytflip(unsigned char n)
* Purpose      
* Params	   n
* Return	   
* Remarks	   
**********************************************************************/

unsigned char bytflip(unsigned char n)
{
    return (nybflip((unsigned char)(n >> 4))
        + (nybflip((unsigned char)(n & 0xf)) << 4));
}

/*********************************************************************\
* Function	   void makefliptab()
* Purpose      
* Params	   void
* Return	   
* Remarks	   
**********************************************************************/

void makefliptab()
{
    unsigned int i;
    
    for(i = 0; i < 256; i ++)
        fliptab[i] = bytflip((unsigned char)i);
}
