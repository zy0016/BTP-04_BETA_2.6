/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : 
 *
 * Purpose  : 
 *
\**************************************************************************/
#ifndef _SMILPARSE_H

#define _SMILPARSE_H
#include <hpdef.h>
#include "MmsGlobal.h"

#define CODETABLELEN       5
#define ISBLANK(c)  \
        (c == ' ' || c == '\t' || c == '\n' || c == '\r')

#define ISSPEC(c)   \
        (c == '<' || c == '>' || c == '&' || c == '"' || c == '\'' || c == '=')

#define ISNUMBER(c) \
        (c <= '9' && c >= '0')

#define ISLETTER(c) \
        ((c <= 'z' && c >= 'a') || (c <= 'a' && c >= 'A'))

#define CONVERT(c) \
            (c = c >= 10 ? c - 10 + 'A' : c + '0')

#define MMS_HIBYTE(w)  ((w) >> 8)
#define MMS_LOBYTE(w)  ((w) & 0x00FF)

/*后续字节判断掩码*/
#define MUTF8_FOLLOW_MASK       0xC0
#define MUTF8_FOLLOW_RESULT     0x80
#define MUTF8_ASCII_MASK        0x80
#define MUTF8_ASCII_RESULT      0x00


#define MUTF8_ILLSTART_MASK     0xFE
#define MUTF8_ILLSTART_RESULT   0xFE

#ifdef WB_FOR_MSWIN
#define UNICODE_PATH  ".\\unicode\\"
#else
static int UTF8Process(unsigned char *start);
#ifdef EMULATION
//#define UNICODE_PATH  "RAM:"
#define UNICODE_PATH  "/rom/"
#else
#define UNICODE_PATH  "/rom/"
#endif

#endif

#ifdef _EMULATE_
#define Wml_MsgOut      MsgOut
extern void MsgOut( char * format, ... );//add by linquan 01/08/2002 
#endif

#define TableSize   20
#define NUM_IN_FIELD_ZERO   21

/*定义字符集类型*/
#define CS_GB2312           2025            
#define CS_BIG5             2026            
#define CS_UTF8             106         /*UTF8*/
#define CS_UCS2             1000        /*UNICODE*/
#define CS_DEFAULT          CS_GB2312   

#define HHFN_FROM           "FROM:"
#define HHFN_CONTENTTYPE    "CONTENT-TYPE:" 
#define HHFN_TO             "TO:"
#define HHFN_CC             "CC:"
#define HHFN_BCC            "BCC:"
#define HHFN_SUBJECT        "SUBJECT:"
#define HHFN_BREPORT        "X-MMS-DELIVERY-REPORT:"
#define HHFN_BREPLY         "X-MMS-READ-REPLY:"
#define HHFN_EXPIRY         "X-MMS-EXPIRY:"
#define HHFN_PRIORITY       "X-MMS-PRIORITY:"
#define HHFN_DATE           "DATE:"
#define HHFN_CONTENTID      "CONTENT-ID:"   
#define HHFN_CONTENTLOCATION    "CONTENT-LOCATION:" 

#define HHFN_MSGTYPE        "X-MMS-MESSAGE-TYPE:"
#define HHFN_TRANSID        "X-MMS-TRANSACTION-ID:"
#define HHFN_VERSION        "X-MMS-MMS-VERSION:"
#define HHFN_MSGCLASS       "X-MMS-MESSAGE-CLASS:"
#define HHFN_MSGSIZE        "X-MMS-MESSAGE-SIZE:"
#define HHFN_ACONTLOCA      "X-MMS-CONTENT-LOCATION:"
#define HHFN_CID            "CID:"

#define CFN_BIG5        big5            
#define CFN_UCS2        iso-10646-ucs-2
#define CFN_ISO88591    iso-8859-1
#define CFN_ISO88592    iso-8859-2  
#define CFN_ISO88593    iso-8859-3  
#define CFN_ISO88594    iso-8859-4  
#define CFN_ISO88595    iso-8859-5  
#define CFN_ISO88596    iso-8859-6  
#define CFN_ISO88597    iso-8859-7  
#define CFN_ISO88598    iso-8859-8  
#define CFN_ISO88599    iso-8859-9  
#define CFN_SHIFTJI     shift_jis   //shift_JIS
#define CFN_ASCII       us-ascii    
#define CFN_UTF8        utf-8

#define TT_UNKNOWN      -1
#define TT_NONE         0
#define TT_SMIL         1
#define TT_BODY         2
#define TT_HEAD         3
#define TT_LAYOUT       4
#define TT_META         5
#define TT_REGION       6
#define TT_ROOT_LAYOUT  7
#define TT_PAR          10
#define TT_AUDIO        8
#define TT_IMG          9   
#define TT_REF          11
#define TT_TEXT         12

/*#define META_OTHER          0
#define META_AUDIO          2
#define META_AUDIO_AMR      21
#define META_AUDIO_MIDI     22
#define META_AUDIO_WAV      23
#define META_AUDIO_MMF      24
#define META_AUDIO_MP3      25
#define META_AUDIO_IMY      26//I_Melody
#define META_IMG            3
#define META_IMG_GIF        31
#define META_IMG_JPG        32
#define META_IMG_BMP        33
#define META_IMG_WBMP       34  
#define META_IMG_PNG        35  
#define META_TEXT           5
#define META_TEXT_PLAIN     41
#define META_TEXT_VCALE     42  //"text/x-vCalendar" 0x06,    
#define META_TEXT_VCARD     43  //"text/x-vCard"     0x07,
#define META_REF            5
#define META_SMIL           6
*/
/*
#define AT_ID           "id"
#define AT_TITLE        "title"
#define AT_SKIP         "skip"
*/
/* //\\means already define tag */

#define MT_SMIL         "smil"
//of SMIL
#define MT_BODY         "body"
#define MT_HEAD         "head"

//of HEAD  any "meta" and either a "layout"  or a "switch"  
#define MT_LAYOUT       "layout"     //can in switch
#define MT_META         "meta"      //empty elment

//of LAYOUT
#define MT_REGION       "region"        //empty elment
#define MT_ROOT_LAYOUT  "root-layout"   //empty elment

#define MT_PAR          "par"

//of BODY
#define MT_AUDIO        "audio"
#define MT_IMG          "img"
#define MT_REF          "ref"
#define MT_TEXT         "text"


struct smil_tag;
typedef struct smil_tag SMILTAG;

#define SMILBASE_MEMBER                 \
            short TagType;              \
            BOOL  bEnd;                 \
            SMILTAG *child;             \
            SMILTAG *parent;            \
            SMILTAG *sibling


struct smil_tag
{
    SMILBASE_MEMBER;
};

#define SYS_CAPTION TRUE
#define SYS_OVERDUB FALSE
 
#define SYS_CAPON   TRUE
#define SYS_CAPOFF  FALSE

#define MAX_TAG_NUM  12

#define MAX_NAME_LEN  20

#define  MAX_ATTR_NUM   6 

typedef char* SMILID;
typedef char* CDATA; 
typedef char* NMTOKEN;

#define INTSTR_LEN 33
#define ATTRV_LEN   20

typedef struct tagREGION
{
    char *left;
    char *top;
    char *height;
    char *width;
    char *fit;
    char *id;
    struct tagREGION *pnext;

} TAG_REGION;


typedef struct tagMEDIA1
{
//  unsigned char mType;
    char *src;
    char *region;
    char *alt;
    char *begin;
    char *end;  
    struct tagMEDIA1 *pnext;

} TAG_TEXT,TAG_IMG,TAG_REF;


typedef struct tagMEDIA2
{
    //unsigned char mType;
    char *src;  
    char *alt;
    char *begin;
    char *end;  
    struct tagMEDIA2 *pnext;

} TAG_AUDIO,TAG_MEDIA;

typedef struct tagMETA
{
    //unsigned char mType;
    char *name; 
    char *content;
    struct tagMETA *pnext;
} TAG_META;

typedef struct tagPAR
{
    char *dur;
    TAG_IMG *timg;
    TAG_TEXT *ttxt;
    TAG_AUDIO *taudio;
    TAG_REF *tref;
    struct tagPAR *pnext;

} TAG_PAR;

typedef struct tagROOTLAYOUT
{
    char *height;
    char *width;
    struct  tagROOTLAYOUT *pnext;
} TAG_ROOTLAYOUT;

typedef struct tagSMIL
{
    TAG_META *metah,*metat;
    TAG_REGION *regionh,*regiont;
    TAG_ROOTLAYOUT *rootlayouth,*rootlayoutt;
    unsigned char nParNum;
    TAG_PAR *parh,*part;
} TAG_SMIL;

typedef struct TagPARSERELT
{
    TAG_SMIL *ParseSmil;
    WSPHead  Parsehead;
    mmsMetaNode *hMeta;

} PARSETRELT;

typedef struct tagAttributeList
{
    int count;
    char * name[MAX_ATTR_NUM];
    char * value[MAX_ATTR_NUM];
} AttributeList;

typedef struct tagSlideBody
{
    int     imgbodylen;
    char    *imgbody;
    int     txtbodylen;
    char    *txtbody;
    int     audiobodylen;
    char    *audiobody;
} SlideBody;
/*for UTF8TO GB2312*/
typedef struct tagUTF8MASK
{
    int   judgemask;    //the mask code of starting
    int   judgeresult;   //the result of judge the starting
    
    int    illb1mask;   
    int    illb1result; 

    int    illb2mask;   
    int    illb2result; 

    
} UTF8MASK;

void MMS_ParseAccept(const char *pAcceptBuf, int bufLen, PACCEPTINFO pAcceptInfo);
WSPHead *ParseRecvHead(char *pBuffer, int Count,WSPHead *pNewHead);
MetaNode *ParseRecvContent(char *pBody, int bodylen,PARSETRELT *pMMS);
void ConvertType(PARSETRELT *pMMS, TotalMMS *tmms);
void PARSER_FreePARSETRELT(PARSETRELT *pRelt);
MetaNode *ParseText(char *pBody, int bodylen,PARSETRELT *pMMS);
MetaNode *ParseImage(char *pBody, int bodylen,PARSETRELT *pMMS);
void PARSER_FreeSmil(TAG_SMIL *pSmil);

#endif
