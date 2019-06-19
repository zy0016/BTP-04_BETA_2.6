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
#ifndef WAPPUSH_H
#define WAPPUSH_H

#include "window.h"

#define CONTENT_TYPE_WAPSI          1
#define CONTENT_TYPE_WAPSIC         2
#define CONTENT_TYPE_TEXT           3
#define CONTENT_TYPE_PUSHCO		    4		//文本格式CO
#define CONTENT_TYPE_PUSHCOC	    5		//二进制格式CO
#define CONTENT_TYPE_PUSHSL		    6		//文本格式SL
#define CONTENT_TYPE_PUSHSLC	    7		//二进制格式SL
#define CONTENT_TYPE_UNKNOWN	    8

#define CONTENT_CHARSET_GB2312      1
#define CONTENT_CHARSET_UTF8        2
#define CONTENT_CHARSET_UCS2        3
#define CONTENT_CHARSET_BIG5        4
#define CONTENT_CHARSET_UNKNOWN     5

#define ACTION_NONE                 1
#define ACTION_LOW                  2
#define ACTION_MEDIUM               3
#define ACTION_HIGH                 4
#define ACTION_DELETE               5
#define ACTION_CACHE		        6

#define DATETIEMLEN	                21
#define READFLAG_YES                0
#define READFLAG_NO                 1

typedef struct tagSIINFO
{
	char *pszTitle;
	char *pszUri;
	char *pszId;
	char szCreateTime[DATETIEMLEN];
	char szExTime[DATETIEMLEN];
	WORD nActionType;
	WORD nRead;		//0为已读，1为未读
} SIINFO, *PSIINFO;

//URL列表指针
typedef struct tag_str_listnode 
{
	char *url;				//url：URL字符串
	unsigned char action;	//action：SL执行方式
	struct tag_str_listnode *next;	//next：下一个节点
} STR_LISTNODE, URLLIST;

extern void *WAP_PushSIParse(unsigned short ContentType, unsigned short Charset,
					   char *Bodybuf, int Buflen);
extern void *WAP_PushCOParse(unsigned short contentType, unsigned short charset,
					   char *dataBuf, int bufLen);
extern void *WAP_PushSLParse(unsigned short contentType, unsigned short charset,
					   char *dataBuf, int bufLen);
extern void freeUrlList(URLLIST *pUrlList);
extern BOOL Push_AutoLoading(char *pUrl);
extern void SIFreeINFOST(PSIINFO pInfo);
extern BOOL InitInboxReceive(void);
extern BOOL ConstrutInboxView(void);

#endif
