#ifndef _WAPSI
#define _WAPSI
#include "window.h"

#define CONTENT_TYPE_WAPSI   1
#define CONTENT_TYPE_WAPSIC  2
#define CONTENT_TYPE_TEXT    3
#define CONTENT_TYPE_UNKNOWN 4

#define CONTENT_CHARSET_GB2312       1
#define CONTENT_CHARSET_UTF8         2
#define CONTENT_CHARSET_UCS2         3
#define CONTENT_CHARSET_BIG5         4
#define CONTENT_CHARSET_UNKNOWN      5

#define ACTION_NONE          1
#define ACTION_LOW           2
#define ACTION_MEDIUM        3
#define ACTION_HIGH          4
#define ACTION_DELETE        5


#define DATETIEMLEN	21
#define READFLAG_YES 0
#define READFLAG_NO  1
typedef struct tagSIINFO
{
	char *pszTitle;
	char *pszUri;
	char *pszId;
	char szCreateTime[DATETIEMLEN];
	char szExTime[DATETIEMLEN];
	WORD nActionType;
	WORD nRead;//0ÎªÒÑ¶Á£¬1ÎªÎ´¶Á
} SIINFO,*PSIINFO;

extern void *WAPB_PushSIParse(unsigned short ContentType,unsigned short  Charset,
					   char *Bodybuf,int Buflen);
extern void SIFreeINFOST(PSIINFO  pInfo);
extern BOOL InitInboxReceive(void);
extern BOOL ConstrutInboxView(void);

#endif

//yyyy-mm-ddThh-mm-ssZ
