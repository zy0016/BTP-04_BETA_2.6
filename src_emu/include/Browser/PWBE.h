/**************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   :  wap browser
 *
 * Purpose  : wap browser API for UI
 *
\**************************************************************************/

#ifndef _PWBE_H
#define _PWBE_H

#ifdef __cplusplus
extern "C"{
#endif


/* if runs on 16-bit platform, need _huge pointer */
#ifndef _WBHUGE_
#ifdef ON_16BIT_PLATFORM
#define _WBHUGE_  _huge
#else
#define _WBHUGE_ 
#endif
#endif

/* macro defination */
#define WB_BOOL int

/* �������ʼ��ʱѡ���Э������ */
#define  WBTRANS_WSP            1      //wspЭ��
#define  WBTRANS_WHTTP          2      //whttpЭ��
#define  WBTRANS_OMAD           3      //OmaDownload WSPЭ��
#define  WBTRANS_OMADHTTP       4      //OmaDownload WHTTPЭ��


/* newpage��ʱ���������Ͷ��� */
#define  WBTRANS_POST             1
#define  WBTRANS_GET              2
#define  WBTRANS_DELETE		      3
#define  WBTRANS_TRACE		      4
#define  WBTRANS_HEAD		      5
#define  WBTRANS_PUT		      6
#define  WBTRANS_CONNECT	      7
#define  WBTRANS_OPTION           8

#define WAPAUTH_WWW   0         //www��֤
#define WAPAUTH_PROXY 1         //Proxy��֤


#define XDRM_NOFORWORD 0    
#define XDRM_NOSAVE    1
#define XDRM_NORMAL    2

/*===================================================*/
#define UM_DIALSTART    0x0021  //��ʼ����
#define UM_DIALOK       0x0022  //���ųɹ�
#define UM_HANGUP       0x0023  //���ſ�ʼ�Ҷ�
#define UM_DIALBREAK    0x0024  //���ŹҶ�
#define UM_STARTEXIT    0x0025  //������˳�
#define UM_EXIT         0x0026  //������˳�

#define UM_CONNECTSTART	0x0011	//��ʼ��������
#define UM_CONNECTTED	0x0012	//�������ӳɹ�
#define UM_CONNECTFAIL	0x0013	//��������ʧ��
#define UM_DISCONNECT	0x0014	//���ضϿ�

#define UM_REQUEST		0x0015	//��ʼ��������
#define UM_DATASTART	0x0017	//���ݿ�ʼ����
#define UM_DATAIN       0x0018  //���ݲ��ֵ���
#define UM_DATAEND	    0x0019  //���ݴ������
#define UM_DATAERROR    0x001a  //���ݴ������
#define UM_STOP	        0x001b  //��ҳֹͣ

#define UM_DOCUMENTEND  0x001c  //��ҳԤ�Ű����(����Ϊ:����ҳ���ý���������)


#define UM_SAVEENABLE   0x001e  //��������߱�����Դ
#define UM_SAVEDISABLE  0x001f  //��ֹ�����߱�����Դ

#define UM_SAVEIMGENABLE   0x0030  //��������߱���image
#define UM_SAVEIMGDISABLE  0x0031  //��ֹ�����߱���image

#define UM_SAVEBGSOUNDENABLE   0x0040  //��������߱���bgsound
#define UM_SAVEBGSOUNDDISABLE  0x0041  //��ֹ�����߱���bgsound

#define UM_SET_PRIMARYKEY		0x0060	//���������
#define UM_SET_SECONDARYKEY		0x0061	//���ô����

#define UM_FOCUSCHANGE			0x0065	//��ҳ�����л���param1��ʾ
//WBFOCUS_NULL:�޽��㣻WBFOCUS_LINK:���ӻ�ý��㣻WBFOCUS_WIDGET:�ؼ���ý���

typedef enum WB_FOCUSSTATUS
{
	WBFOCUS_NULL = 0,
	WBFOCUS_LINK,
	WBFOCUS_WIDGET
}WB_FOCUSSTATUS;
	
#define UM_HISTORYINFO     0x0070   //������ʷ��¼��Ϣ:
									//param1:prev�����ˣ���Ϣ��1:�ɺ��ˣ�0�����ɺ���;
									//param2:next��ǰ������Ϣ��1:��ǰ����0������ǰ����

#define UM_SETTITLE		   0x0071
#define UM_URLCHGN	       0x0072		
#define UM_DIALRETURN      0x0073
#define UM_ERROR           0x0074

#define UM_CLEAREDITENABLE  0x0080
#define UM_CLEAREDITDISABLE 0x0081


#define ERR_FORMATERR        1         //���ݸ�ʽ����
#define ERR_ACCSSDENY        2         //���ʱ��ܾ�
#define ERR_UNKNOWNCHARSET   3         //��֧�ֵ��ַ��� 
#define ERR_UNKNOWNTYPE      4         //��֧�ֵ��ļ�����
#define ERR_SCRIPT           5         //Scriptִ�д���
#define ERR_PAGETOOLARGE     6         //��ҳ����̫��
#define ERR_SCRIPTFAIL       7         //Script��������ʧ��
#define ERR_INVALIDURL       8         //Ŀ��urlΪ��,�򲻺Ϸ�
#define ERR_EMPTYINPUT       9         //û����������:��ҳҪ���������������,����Ϊ��
#define ERR_INVALIDINPUT     10        //�������ݲ�������ҳҪ��ĸ�ʽ


#define UM_JARINSTALL_START    0x0084   //��ʼ��װJAVAӦ��
#define UM_JARINSTALL_ACK      0x0085   //JAVAӦ�ð�װ�ɹ�
#define UM_JARINSTALL_NOACK    0x0086   //JAVAӦ�ð�װδ��������ȷ��


/* ��ҳҪ��������ʽ */

//uppercase letter, symbol, or punctuation character. Numeric characters are excluded.
#define FMT__UPPER_SYMB_PUNC      1     
//lowercase letter, symbol, or punctuation character. Numeric characters are excluded.
#define FMT__LOWER_SYMB_PUNC      2
//numeric character.
#define FMT__NUM                  3
//numeric, symbol, or punctuation character.
#define FMT__NUM_SYMB_PUNC        4
//uppercase letter, numeric character, symbol, or punctuation character.
#define FMT__UPPER_NUM_SYMB_PUNC  5     
//lowercase letter, numeric character, symbol, or punctuation character.
#define FMT__LOWER_NUM_SYMB_PUNC  6     

#define FMT__UNKNOWN              10


#define TRANS_NORMAL       0      //��ͨ����
#define TRANS_SECURE       1      //��ȫ����

#define UI_MSG				WM_USER + 100



/*===================================================*/

/*ý�����ݵ�����*/
enum
{
	MMT_UNKNOWN,				/*���Ͳ�֪��*/
	MMT_WML1,					/*������WML1.3�ı��ĵ�*/
	MMT_WMLC,					/*������WML�����Ʊ����ĵ�*/
	MMT_WMLS,					/*������WMLScript�ı��ĵ�*/
	MMT_WMLSC,					/*������WMLScript�����Ʊ����ĵ�*/
	MMT_HTML,					/*������HTML�ı�*/
	MMT_XHTMLMP,				/*������XHTMLMP�ı��ĵ�*/
	MMT_XHTMLBASIC,	    		/*������XHTMLBASIC�ı��ĵ�*/
	MMT_WML2,				    /*������WML2.0�ı��ĵ�*/
	MMT_WCSS,					/*������WCSS�ĵ�*/
	MMT_TEXT,                   /*������text/plain�ĵ�*/		
	
	/*ͼƬ*/
	MMT_IMGWBMP,				/*������WBMPͼ���ʽ*/
	MMT_IMGGIF,				    /*������GIFͼ���ʽ*/
	MMT_IMGBMP,					/*������BMPͼ���ʽ*/
	MMT_IMGJPG,					/*������JPGͼ���ʽ*/
	MMT_IMGPNG,					/*������PNGͼ���ʽ*/	
		
	/*��Ƶ*/
	MMT_VIDIO3GPP,	            

	/*��ý��*/
	MMT_VIDIOASF,               /* .asf: video/x-ms-asf       */
	MMT_VIDIOQUICK,             /* .mov: video/quicktime      */
	MMT_VIDIORM,                /* .rm : video/x-pn-realaudio */

	/*����*/
	MMT_AUDIOMPEG,              /*������mpeg������ʽ*/
	MMT_AUDIO3GPP,					
	MMT_AUDIORMF,               /*������RMF������ʽ*/
	MMT_AUDIOMMF,				/*������MMF������ʽ*/
	MMT_AUDIOMIDI,				/*������MIDI������ʽ*/
	MMT_AUDIOAMR,				/*������AMR������ʽ*/
	MMT_AUDIOWAV,               /*������WAV������ʽ*/
	
	/*����ͨm-flash*/
	MMT_MFLASH,                  /*����ͨm-flashh*/
	MMT_MFLASHDD,                /*����ͨm-flashh descriptor*/

	
	/*����*/
	MMT_JAVAJAR,				/*������jar*/
	MMT_JAVAJAD,				/*������jad*/
	MMT_JAVAINDEX,				/*������INDEX*/
		
	MMT_VCARD,                  /*vCard    */
	MMT_VCALENDAR,              /*vCalendar*/
		
	MMT_POVECONT,               /*provision��Ϣ*/
	MMT_WBPOVECONT,             /*������provision��Ϣ*/
	MMT_DRMDISCTIPTOR,   
	MMT_DRMRIGHTS,  
	MMT_DRMCONTENT,      
	MMT_DRMMESSAGE,     
	
	MMT_DOCUMENT,    
	MMT_MEDIA,       

	MMT_UNSUPPORTED  			/*���Ͳ�֧��*/
};
#define MMT_DEFAULT			MMT_WMLC	/*Ĭ��������WMLC�ı��ĵ�*/

/*********************************************************
 ��ҳ��������
*********************************************************/
#define  SSR_NODATA         0   //no source data to save
#define  SSR_OK             1   //ok
#define  SSR_TOOLARGE      -1   //page is too large, exceed specified limit size
#define  SSR_DRMNOSAVE     -2   //DRM: no-save
#define  SSR_NOFLASHSPACE  -3   //no available flash space
#define  SSR_FILEOPENFAIL  -4   //file open error


/* widget status */
#define WS_NORMAL   0x00
#define WS_FOCUS    0x01
#define WS_PENDOWN  0x02
#define WS_CHECKED  0x04

#define COLOR_INVALID    0xFFFFFFFF	//

#define  SBSTEP_Y   30		// the vertical ScrollBar step
#define  SBSTEP_X   30		// the horizontal ScrollBar step

/* struct defination */

typedef enum WB_EVENTTYPE
{
	WBET_KEYDOWN = 0x100,
	WBET_KEYUP,
	WBET_CHAR,
	WBET_PENDOWN,
	WBET_PENUP,
	WBET_VERTSCROLL,
	WBET_HORZSCROLL,	
	WBET_KILLFOCUS,
	WBET_SETFOCUS
}WB_EVENTTYPE;

typedef enum WB_DISPLAYOPTION
{
	WBDOP_COLOR,
	WBDOP_IMGDISPLAY,
	WBDOP_BGSOUND,
	WBDOP_FONTSIZE,
	WBDOP_MAXPAGESIZE,
	WBDOP_SCROLLBARSIZE
}WB_DISPLAYOPTION;

typedef enum WB_IMGTYPE
{
	WBIMG_WBMP,
	WBIMG_BMP,
	WBIMG_JPG,
	WBIMG_GIF,
	WBIMG_PNG
}WB_IMGTYPE;

typedef enum WB_AUDIOTYPE
{
	WBAUDIO_RMF,
	WBAUDIO_MMF,
	WBAUDIO_MIDI,
	WBAUDIO_AMR,
	WBAUDIO_WAV  
}WB_AUDIOTYPE;

typedef enum tagWB_REQDATATYPE
{
	WBREQDATA_MFLASHDD
}WB_REQDATATYPE;


typedef struct WB_PAGEINFO
{
    int     nMax; 		//������ҳ�ĸ߶�/���
    int     nPage;  	//һҳ�ĸ߶�/���
    int     nPos;   	//��ǰ��ʾҳ������λ�ã���Ӧ�����������λ�ã�
}WB_PAGEINFO;


typedef enum WB_SBTYPE
{
	WBSBT_HORZ,
    WBSBT_VERT
}WB_SBTYPE;

typedef enum WB_SBSTATE
{
	WBSBS_ENABLE,
	WBSBS_DISABLE
}WB_SBSTATE;

typedef enum WB_SCROLL
{
	WBSCROLL_LINEUP,
	WBSCROLL_LINEDOWN, 
	WBSCROLL_PAGEUP, 
	WBSCROLL_PAGEDOWN, 
	WBSCROLL_LINELEFT, 
	WBSCROLL_LINERIGHT, 
	WBSCROLL_PAGELEFT, 
	WBSCROLL_PAGERIGHT, 
	WBSCROLL_THUMBPOSITION, 
	WBSCROLL_THUMBTRACK
}WB_SCROLL;

typedef enum WB_VKEY
{
	WBKEY_UP,
	WBKEY_DOWN,	
	WBKEY_RIGHT,
	WBKEY_LEFT,	
	WBKEY_PAGEUP,
	WBKEY_PAGEDOWN,
	WBKEY_F5,
	WBKEY_TAB,
	WBKEY_BACK
}WB_VKEY;

typedef enum WB_CACOOPTION
{
	WBCACHE_SIZE,    /* ����Ĵ�С	*/
	WBCACHE_MODE,    /* ����ģʽ		*/	
	WBCOOKIE_SIZE,	  /* Cookie �Ĵ�С*/
	WBCOOKIE_MODE,	  /* �Ƿ�����Cookie*/
	WBCOOKIE_COUNT,	  /* Cookie ������*/

	WBCACHE_FILE,     //cache�ļ���ŵľ���·��
    WBCOOKIE_FILE,     //cookie�ļ���ŵľ���·��
	WBCACHE_CLEAR,     //clear cache
	WBDIAL_CFGID       //dialmanager config id

}WB_CACOOPTION;


#define WBFONT_FACESIZE 32

enum WB_FONTWEIGHT
{
    WBFW_DONTCARE = 0,
    WBFW_THIN,
    WBFW_EXTRALIGHT,
    WBFW_LIGHT,
    WBFW_NORMAL,
    WBFW_MEDIUM,
    WBFW_SEMIBOLD,
    WBFW_BOLD,
    WBFW_EXTRABOLD,
    WBFW_HEAVY
};

enum WB_FONTCHARSET
{
    WBCHARSET_ANSI = 0,
    WBCHARSET_DEFAULT,
    WBCHARSET_SYMBOL,
    WBCHARSET_SHIFTJIS,
    WBCHARSET_HANGEUL,
    WBCHARSET_GB2312,
    WBCHARSET_CHINESEBIG5,
    WBCHARSET_OEM
};

enum WB_FONTPITCH
{
    WBPITCH_DEFAULT = 0,
    WBPITCH_FIXED,
    WBPITCH_VARIABLE
};

enum WB_FONTFAMILY
{
    WBFF_DONTCARE   = (0 << 4),
    WBFF_ROMAN      = (1 << 4),
    WBFF_SWISS      = (2 << 4),
    WBFF_MODERN     = (3 << 4),
    WBFF_SCRIPT     = (4 << 4),
    WBFF_DECORATIVE = (5 << 4)
};

typedef struct WB_Font
{
	long			size;
    long            weight;
    unsigned char   italic;
    unsigned char   charSet;
    unsigned char   pitchAndFamily;
    char            faceName[WBFONT_FACESIZE];

    void            *hFont;
}WB_FONT;


typedef enum WB_TEXTSTYLEOPTION
{
	WBTS_FONT,
	WBTS_COLOR
}WB_TEXTSTYLEOPTION;

typedef enum WB_SELTYPE
{
	WBSEL_SINGLE,
    WBSEL_MULTIPLE
}WB_SELTYPE;

typedef enum WB_ENDSELTYPE
{
	WBSEL_OK,
    WBSEL_CANCEL,
	WBSEL_ONPICK
}WB_ENDSELTYPE;

typedef struct WB_OPTIONDATA
{
   char *caption;    //ѡ����ʾ����
   WB_BOOL  selected; //TRUE: ��ѡ���ѱ�ѡ��; FALSE:û�б�ѡ��
   WB_BOOL  onpick;   //TRUE: ��ѡ����onpick�¼�; FALSE:û��onpick�¼�. 
}WB_OPTIONDATA;


typedef struct WB_POINT 
{ 
  long x; 
  long y; 
}WB_POINT;


typedef enum WB_EDITTYPE
{
	EDT_SINGLELINE,
	EDT_PASSWORD,
	EDT_MULTILINE
}WB_EDITTYPE;


typedef void * HBROWSER;


typedef struct color_scheme
{
	unsigned long text_color;			//��ͨ������ɫ	
	unsigned long link_textcolor;		//����������ɫ
	unsigned long link_bgcolor;			//���ӱ���ɫ
	unsigned long link_Focustextcolor;	//����������������ɫ
	unsigned long link_Focusbgcolor;	//���������ӱ���ɫ
}Color_Scheme;

typedef struct WB_RECT 
{ 
  long left; 
  long top; 
  long right; 
  long bottom; 
} WB_RECT;

enum WB_CACHEMODE
{
    WBCACHE_OK,	            /* ʹ���κη���Ļ���	*/
	WBCACHE_FLUSH_MEM,			/* ������򻺴�����		*/
	WBCACHE_VALIDATE,	        /* ��֤����				*/
	WBCACHE_END_VALIDATE,      /* �˵��˵���֤			*/
	WBCACHE_RANGE_VALIDATE,
	WBCACHE_FLUSH			    /* ǿ������				*/
};

enum WB_COOKIEMODE
{
    WBCOOKIE_ENABLE,	    /* ����ʹ��Cookie*/
	WBCOOKIE_DISABLE       /* ��ֹʹ��Cookie*/
};


/*Browser  ��LIB_PERMIT��LIB_PROMPT, LIB_ASSERTʹ�õı�׼�ַ���*/
#define MSGMACKCALLSTR  "Are you sure make call?"
#define MSGDTMFSTR      "Send DTMF?"
#define MSGADDPBSTR     "Are you sure add a new phonebook entry?"
#define SOFTKEY_OK      "OK"
#define SOFTKEY_CANCEL  "Cancel"
#define SOFTKEY_ACCEPT  "Accept"


enum WB_PENSTYLE
{
    WBPS_SOLID = 0,       /* ʵ�߱�               */
    WBPS_DASH,            /* ���߱�     -------   */
    WBPS_DOT,             /* ���߱�     .......   */
    WBPS_DASHDOT,         /* �㻮�߱�   _._._._   */
    WBPS_DASHDOTDOT,      /* ˫�㻮�߱� _.._.._   */
    WBPS_NULL            /* �ձ�                 */
};


/*LIB_DEALJAD����ֵ*/
typedef enum tagJadCheck
{
	PWBE_JAR_CONTINUE,      //JAD��ȷ����������jar
	PWBE_JAR_CANCEL,        //���ټ�������jar	
	PWBE_JAR_DOWNLOAD       //JAR���ع��̽���UI��ɣ����Ĳ���Ҫ��������
}PWBE_JADCHECK;


/*LIB_DEALJAVAAPP����ֵ*/
#define PWBE_JARINS_OK         900 //�ɹ�
#define PWBE_JARINS_MEM        901 //�ڴ治��
#define PWBE_JARINS_CANCEL     902 //�û�ȡ��
#define PWBE_JARINS_BREAK      903 //�����ж�
#define PWBE_JARINS_JARMATCH   904 //Jar�ļ���ƥ��
#define PWBE_JARINS_ATR        905 //���Բ�ƥ��
#define PWBE_JARINS_JADINV     906 //Jad��Ч
#define PWBE_JARINS_JARINV     907// Jar��Ч
#define PWBE_JARINS_FRMINV     908//��Ч���
#define PWBE_JARINS_AUTH       909 //��Ȩʧ��
#define PWBE_JARINS_APP        910//Ӧ��ȷ��ʧ��
#define PWBE_JARINS_PUSH       911 //PUSHע��ʧ��
#define PWBE_JARINS_INV        913 //��֧��


/* GUI callback list */

typedef int (*LIB_DRAWLINE)(void* pUIData, int x1, int y1, int x2, int y2, int style, 
                        unsigned long color);

typedef int (*LIB_RECTANGLE)(void* pUIData, WB_RECT *lprc, unsigned long color, int bFill);

typedef int (*LIB_ELLIPSE)(void* pUIData, WB_RECT *lprc, unsigned long color, int bFill);

typedef int (*LIB_POLYGON)(void *pUIData, const WB_POINT *lpPoints, int nCount, 
			   unsigned long color, int penStyle, int bFill);

typedef int (*LIB_SETTEXTSTYLE)(void* pUIData, WB_TEXTSTYLEOPTION option, void* pnewStyle, void* poldStyle);

typedef int (*LIB_DRAWTEXT)(void* pUIData, WB_RECT *lprc, char* pText, int len, 
                        unsigned long fgcolor, unsigned long bkcolor);

typedef int (*LIB_GETTEXTINFO)(void* pUIData, WB_FONT* pFont, 
							   char* pStr, int len, int* w, int* h);

typedef int (*LIB_DRAWIMAGE)(void* pUIData, int x, int y, int width, int height,
							 int src_x,  int src_y,  int src_width,  int src_height,	
		 WB_IMGTYPE imgtype, char* pData, long len, void** hImage, void* Param1);

typedef int (*LIB_ENDIMAGE)(void* hImage, WB_IMGTYPE imgType, void* param);

typedef int (*LIB_GETIMAGEINFO)(WB_IMGTYPE imagetype, char* pData, int len, int* w, int* h);

typedef int (*ECHO_FINISHEDITTEXT)(HBROWSER hBrowser, char *pBuffer, WB_BOOL bDataChanged);
typedef int (*LIB_EDITTEXT)(WB_EDITTYPE iEditType , char *pBuffer, int iMaxCharLen, int iBufferLen, 
							char *InputFormat, ECHO_FINISHEDITTEXT pFinishEdit);

typedef int (*ECHO_FINISHSELECT)(HBROWSER hBrowser, int *pSelects, int iSelNum, 
								int iPickedIdx, WB_ENDSELTYPE condition);
typedef int (*LIB_SELECTOPTION)(WB_SELTYPE type, WB_OPTIONDATA options[], int iItemNum, ECHO_FINISHSELECT FinishSelect);

typedef int (*LIB_STARTRING)(WB_AUDIOTYPE soundType, int playmode, unsigned char *data, int datalen);
typedef int (*LIB_STOPRING)(void);

typedef int (*LIB_BEGINDISPLAY)(void* pUIData, WB_RECT *pRect);
typedef int (*LIB_ENDDISPLAY)(void* pUIData);

typedef struct tagLIBLIST_GUI
{
	/* draw lines func */
	LIB_DRAWLINE    DrawLine;
	LIB_RECTANGLE   DrawRect;
	LIB_ELLIPSE		DrawEllipse;
	LIB_POLYGON		DrawPolygon;

	/* text func */
	LIB_SETTEXTSTYLE  SetTextStyle;
	LIB_DRAWTEXT     DrawText;
	LIB_GETTEXTINFO  GetTextInfo;
	LIB_EDITTEXT     EditText;
	
	/* image func */
	LIB_DRAWIMAGE    DrawImage;
	LIB_ENDIMAGE     EndImage;
	LIB_GETIMAGEINFO GetImageInfo;
	
	/* select func */
	LIB_SELECTOPTION SelectOption;

	/* ring func */
	LIB_STARTRING    StartRing;
	LIB_STOPRING     StopRing;

	/* display func */
	LIB_BEGINDISPLAY	 BeginDisplay; //��ʼ������ʾ
	LIB_ENDDISPLAY		 EndDisplay;   //����������ʾ

}LIBLIST_GUI;


/* callback procedure list */

typedef int (* LIB_DOWNLOAD)(const char _WBHUGE_ *data,
							   long datalen,
							   unsigned char ContType, const char *szMIMEName,
							   const char *szURLName, unsigned char xdrm_type);

typedef int (* LIB_DOWNLOADWITHDP)(const char _WBHUGE_ *data,
							   long datalen,
							   const char *dd,
                               long ddlen,
							   unsigned char ContType, const char *szMIMEName,
							   const char *szURLName, unsigned char xdrm_type);

typedef void (* LIB_UINOTIFY)(unsigned int UImessage,
								unsigned long wParam,
								unsigned long lParam);

typedef int  (* ECHO_DEALJADPROC)(void* param, PWBE_JADCHECK res);
typedef void  (*LIB_DEALJAD)(void* param, const char* jadbuf, unsigned long jadlen, ECHO_DEALJADPROC pJadProc);

typedef int (* LIB_DEALJAVAAPP)(const char* jadbuf,
							     unsigned long jadlen,
							     const char *jarbuf,
							     unsigned long jarlen, int *pJavaHandle);

typedef void (* LIB_DEALVCARD)(const char *cReqURL,
							 const char* http_head,
							 unsigned int headlen,
							 const char *http_content,
							 unsigned int contentlen);

typedef void (* LIB_DEALVCAL)(const char *cReqURL,
							 const char* http_head,
							 unsigned int headlen,
							 const char *http_content,
							 unsigned int contentlen);

typedef int (* ECHO_FINISHPERMITPROC)(void* param, WB_BOOL bOK);
typedef WB_BOOL (* LIB_PERMIT)(void* param, const char *showinfor, const char *OKBtnCaption, const char *CancelBtnCaption, 
							  ECHO_FINISHPERMITPROC pFinishPermit);

typedef int(*ECHO_FINISHPROMPTPROC)(void* param, const char *szResult);
typedef WB_BOOL (* LIB_PROMPT)(void* param, const char *pTitle, const char* pDefResult, 
							ECHO_FINISHPROMPTPROC pFinishPromt);

typedef WB_BOOL (* LIB_ASSERT)(const char *pMsg, int nMsgLen);

typedef int (*ECHO_FINISHAUTHINPUTPROC)(void* param, WB_BOOL result, const char *szName, const char *szPassword);
typedef WB_BOOL (* LIB_GETAUTH)(void* param, int nMaxAuthLen, int nMaxPswLen, const char *pRealm, const char *pRoot, int type,  
							   ECHO_FINISHAUTHINPUTPROC pFinishAuthInput);

typedef void (*LIB_DEALFLOWMEDIA)(const char *cReqURL, int nContType);

typedef int  (* ECHO_PROCESSOVER) (void* param, int iProcessID, int iReslut);

typedef int  (* LIB_PUBMAKECALL)(void* param, const char* numbuf, int numlen, ECHO_PROCESSOVER  pMakeCallOverProc);

typedef int  (* LIB_PUBSENDDTMF)(void* param, const char* dtmfbuf, int dtmflen, ECHO_PROCESSOVER pSendDTMFOver);

typedef int  (* LIB_PUBADDPBENTRY)(void* param, const char* NUMBER, int numlen, const char* NAME, int namelen, 
								   ECHO_PROCESSOVER pAddPbentry);

typedef void (*LIB_BRSMAILTO)(void* param, char *szMail, char *szCc, 
							  char *szBcc, char *szSubject,	char *szBody,
							  ECHO_PROCESSOVER pMailToProc);

typedef WB_BOOL (* LIB_PUSHSI)(void *data);
typedef WB_BOOL (* LIB_PUSHTEXT)(char *data, int dataLen);
typedef WB_BOOL (* LIB_PUSHSL)(char *data, WB_BOOL isQuaereUser);

typedef void (*LIB_SETSCROLLPOS)(void *pUIData, WB_SBTYPE iBar, WB_PAGEINFO *pPageInfo, WB_SBSTATE iState);


typedef struct tagLIBLIST_APP
{
	LIB_DOWNLOAD		DownloadProc; 		//���ش�����	
	LIB_UINOTIFY		UINotifyProc;  	//����֪ͨ����
	LIB_PERMIT			PermitProc;		//ȷ�Ϻ���
	LIB_PROMPT  		PromptProc;     //��ʾ�û����뺯��
	LIB_ASSERT			AssertProc;		//�û���ʾ����
	LIB_GETAUTH			GetAuthProc;	//�û�������֤����
	
	LIB_DEALJAD         DealJadProc;        //Jad������
	LIB_DEALJAVAAPP		DealJavaAppProc;	//JavaӦ�ô�����
	
	LIB_DEALVCARD		vCardProc;		//vCard������
	LIB_DEALVCAL		vCalendarProc;		//vCalendar������
	
	LIB_BRSMAILTO		MailToProc;		//����mail��ҵ����õĴ�����

	LIB_DEALFLOWMEDIA   FlowMediaProc;      //��ý�岥��

	/* WTAI Public func */
	LIB_PUBMAKECALL			MakeCallProc;		//��绰�Ĵ�����
	LIB_PUBSENDDTMF			SendDtmfProc;		//����DTMF ������
	LIB_PUBADDPBENTRY		AddPbEntryProc;		//������ϵ�˼�¼�Ĵ�����
	
	/* set scroll bar position */
	LIB_SETSCROLLPOS		SetSrollPosProc;	//�������ػ�֪ͨ����
	LIB_DOWNLOADWITHDP      DownloadWithDP;     //���ش�����(����Descriptor)

}LIBLIST_APP;	

typedef enum tagAPPTYPE
{
	APPTYPE_PERMIT,               //PermitProc
	APPTYPE_PERMIT_MAKECALL,      //PermitProc for mackcall 
	APPTYPE_PERMIT_ADDPB,         //PermitProc for addpb
	APPTYPE_PERMIT_DTMF,          //PermitProc for senddtmf
	APPTYPE_PROMPT,	              //PromptProc
	APPTYPE_GETAUTH,              //GetAuthProc

	APPTYPE_MAKECALL,             //MakeCallProc
	APPTYPE_DTMF,                 //SendDtmfProc
	APPTYPE_ADDPB,                //AddPbEntryProc

	APPTYPE_UNKNOWN
}WBTYPE_APP;

typedef enum tagRETTYPE
{
	APPRET_SYNC,
	APPRET_ASYNC
}WBTYPE_RET;

typedef enum tagWBTYPE_APPVALKEY
{
	APPVAL_MAKECALL_NUM,
	APPVAL_ADDPB_NAME,
	APPVAL_ADDPB_NUM,
	APPVAL_DTMF_DTMF,

	APPVAL_RES_INT,
	APPVAL_AUTH_UID,
	APPVAL_AUTH_PW,
	APPVAL_PROMPT_RES

}WBTYPE_APPVALKEY;

WBTYPE_APP PWBE_GetAppType(void* param);
int        PWBE_GetAppValue(void* param, WBTYPE_APPVALKEY key, void *pBuf, int *buflen);
int        PWBE_SetAppRetType(void* param, WBTYPE_RET rettype);




/* APIs */

/* create & destroy */
HBROWSER PWBE_Create(void* pUIData, WB_RECT *pRect, 
								int nProtocol, 
							  char *pGateWay, int nPort,
							  char *pUser, char *pPassWord,
							  LIBLIST_GUI * pGUIFunc);

void PWBE_Destroy(HBROWSER hBrowser);

/* navigate APIs */
int PWBE_NewPage(HBROWSER hBrowser,char *AbsUrl, int method,
					   char *postfields, char *ReqHeaders);
int PWBE_NewPageByData(HBROWSER hBrowser, int method, unsigned char* pDataBuf, int nDataBufLen, WB_REQDATATYPE nDataType);
int PWBE_PrevPage(HBROWSER hBrowser);
int PWBE_NextPage(HBROWSER hBrowser);
int PWBE_Refresh(HBROWSER hBrowser);
int PWBE_Stop(HBROWSER hBrowser);
int PWBE_Hangup(HBROWSER hBrowser);

/* set configurations */
int PWBE_SetUserAgent(HBROWSER hBrowser, const char* UserAgent);
int PWBE_SetUAProfileUrl(HBROWSER hBrowser, const char* url);

int PWBE_SetDisplayOption(HBROWSER hBrowser, WB_DISPLAYOPTION Option, 
								void *pValue, int Length);

int PWBE_SetCACOOption(HBROWSER hBrowser, WB_CACOOPTION Option, 
							 void *pValue, int Length);

/* save source */
int PWBE_SaveSource(HBROWSER hBrowser, char *fname, unsigned long iMaxDocSize, unsigned long iMaxImgSize);

void PWBE_DeleteSavedSource(HBROWSER hBrowser, char *filename);

int PWBE_SaveFocusedImage(HBROWSER hBrowser, char *defname);

int PWBE_SaveBgsound(HBROWSER hBrowser, char *defname);

/* refresh painting */
int PWBE_RefreshPaint(HBROWSER hBrowser, WB_RECT *rect);

/* register callbacks */
int PWBE_RegisterCallbackProcs(HBROWSER hBrowser, LIBLIST_APP * pProcList);

/* event processor */
int PWBE_ProcessEvent(HBROWSER hBrowser, WB_EVENTTYPE eventType,  
							unsigned long wParam, unsigned long lParam);

/* get infomation APIs */
int PWBE_GetPageInfo(HBROWSER hBrowser, WB_SBTYPE iBar, WB_PAGEINFO *pPageInfo);

const char *PWBE_GetCurURLName(HBROWSER hBrowser);

unsigned short PWBE_GetSourceType(HBROWSER hBrowser);
unsigned long PWBE_GetDataSize(HBROWSER pBrowser);
const _WBHUGE_ char * PWBE_GetData(HBROWSER pBrowser);

/* soft keys */
int PWBE_PrimaryKey(HBROWSER pBrowser);
int PWBE_SecondaryKey(HBROWSER pBrowser, int index, char *name);

/* PUSH APIs */
WB_BOOL PWBE_InitPUSH(LIB_PUSHSI SIProc, LIB_PUSHTEXT TextProc, LIB_PUSHSL SLProc);
WB_BOOL PWBE_EndPUSH(void);
WB_BOOL PWBE_EnablePUSH(WB_BOOL bEnable);

/*Cert APIs*/

/************************************************
* ��־֤�������ʼ�����ڴ��в��������Ϣ
************************************************/
#define BROWSERCERT_GETISSUER          0x0001              
#define BROWSERCERT_GETSUBJECT         0x0002
#define BROWSERCERT_GETVALIDAFTER      0x0004
#define BROWSERCERT_GETVALIDBEFOR      0x0008
#define BROWSERCERT_GETCERTINFOR       0x0010

/****************************************************
* cert
*****************************************************/
typedef struct tagBrowserCert
{
	int    pHandle;                 /*������        */
	char*  pIssuer;                 /*������          */
	char*  pSubject;                /*������          */
	char*  pValidAfter;             /*��Ч����ʼʱ��  */
	char*  pValidBefor;             /*��Ч����ֹʱ��  */
	char*  pCertInfor;              /*����ǩ��ԭʼ����*/
	struct tagBrowserCert  *pNext;  /*��һ��֤��      */
} BROWSERCERT, *PBROWSERCERT;

/****************************************************
* ֤���������
*****************************************************/
typedef struct tagBrowserCertList
{
	int   nNum;                             /*��ǰ֤����Ŀ  */
	unsigned long dwCol;                    /*��ǰ֤����ȡ��*/
	PBROWSERCERT  pHead;                    /*֤������      */
}BROWSERCERTLIST, *PBROWSERCERTLIST;


PBROWSERCERTLIST PWBE_CertInitiate (unsigned long dwGet);
int PWBE_CertFreeList(PBROWSERCERTLIST pList);

/*Java API(for delete java report)*/
typedef enum PWBEJAVAREPORTMSG
{
	JRPT_SUCCESS,
	JRPT_FAILURE
}PWBE_JAVAREPORTMSG;

typedef int (*LIB_JavaCallBack)(PWBE_JAVAREPORTMSG msg, unsigned long javaparam);

int PWBE_JavaAppReport(char* szUrl, int Http_Mothod, int protocol,
					   const char* gateway, int port, const char* szUid, const char* szPw,
				       char* pPostInfo,  LIB_JavaCallBack pCallBack, unsigned long javaparam);

/*The API for m_flash download by zdd*/
int PWBE_NewMFlashDownload(HBROWSER hBrowser, unsigned char* pZddBuf, int nZddBufLen);

#ifdef __cplusplus
}
#endif

#endif
