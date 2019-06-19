/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : SMIL
 *
 * Purpose  : 
 *
\**************************************************************************/

#ifndef __HMSMIL_H
#define __HMSMIL_H

#ifdef __cplusplus
extern "C"
#endif


#define SLID_SEQ    1
#define SLID_PAR    2

#define MMS_IMAGE   1
#define MMS_TEXT    2
#define MMS_AUDIO   3

#define SMIL_DECODE_FAIL       0
#define SMIL_DECODE_SUCCESS    1
#define SMIL_DECODE_MEMORY     2

struct MMS_Param 
{
	struct MMS_Param * pNext;
	char *	name;
	char *	value;
};

struct MMS_SlideElement 
{
    struct MMS_SlideElement * pNext;
    struct MMS_Param * param_list;
    char    type;
    char    flags;
    int 	begin;
    int 	end;
    char * 	name;
    char * 	alt;
    char * 	region;
};

struct MMS_Slide 
{
    struct MMS_Slide * pNext;
    struct MMS_SlideElement * pLink;
    int	type;
    int	dur;
};

struct MMS_Seq 
{
    struct MMS_Slide * pNext;
    struct MMS_Slide * pLink;
    int	type;
    int	repeat;
};

struct MMS_SmilBody 
{
    struct MMS_Slide * pSlide;
};

struct MMS_Meta 
{
    struct MMS_Meta * pNext;
    char * name;
    char * content;
};

struct MMS_RootLayout 
{
    int	width;
    int	height;
    char * bk_color;
};

struct MMS_Region 
{
    struct MMS_Region * pNext;
    char * name;
    int	left;
    int	top;
    int	width;
    int	height;
};

struct MMS_Layout 
{
    struct MMS_RootLayout	root_layout;
    struct MMS_Region * pRegion;
};

struct MMS_SmilHead 
{
    struct MMS_Meta *  pMeta;
    struct MMS_Layout  Layout;
};

struct MMS_Smil 
{
    struct MMS_SmilHead  Head;
    struct MMS_SmilBody  Body;
};

int SMIL_Decode (unsigned char * pSmilData, int datalen, unsigned char * buffer, int buflen);
int SMIL_Encode (struct MMS_Smil * pSmil, unsigned char * buffer, int buflen);

#ifdef __cplusplus
}
#endif

#endif  //_SMIL_H_
