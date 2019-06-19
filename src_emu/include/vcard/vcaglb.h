/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Public
 *
 * Purpose  : vCard and vCalendar global data and function declaration
 *            
\**************************************************************************/

#ifndef _VCARD_VCAL_GLB_H_
#define _VCARD_VCAL_GLB_H_

#ifdef  __cplusplus
extern "C" {
#endif

/***********************************************************\
				vCard 数据集函数声明
\***********************************************************/

//定义返回值
#define VCARD_FAILURE			0
#define VCARD_SUCCESS			1

//定义调用类型，在vCard_Clear函数中使用
#define VCARD_OPT_WRITER		0
#define VCARD_OPT_READER		1

//define VCARD version
#define VCARD_VERSION			"2.1"

//define TEL type
#define VCARD_TEL_PREE			0	//Indicates preferred number 
#define VCARD_TEL_WORK			1	//Indicates a work number
#define VCARD_TEL_HOME			2	//Indicates a home number
#define VCARD_TEL_VOICE			3	//Indicates a voice number (Default)
#define VCARD_TEL_FAX			4	//Indicates a facsimile number
#define VCARD_TEL_MSG			5	//Indicates a messaging service on the number
#define VCARD_TEL_CELL			6	//Indicates a cellular number
#define VCARD_TEL_PAGER			7	//Indicates a pager number
#define VCARD_TEL_BBS			8	//Indicates a bulletin board service number
#define VCARD_TEL_MODEM			9	//Indicates a MODEM number
#define VCARD_TEL_CAR			10	//Indicates a car-phone number
#define VCARD_TEL_ISDN			11	//Indicates an ISDN number
#define VCARD_TEL_VIDEO			12	//Indicates a video-phone number
//define ADDR type
#define VCARD_ADR_DOM			13	//Indicates a domestic address			
#define VCARD_ADR_INTL			14	//Indicates an international address (Default)
#define VCARD_ADR_POSTAL		15	//Indicates a postal delivery address  (Default)
#define VCARD_ADR_PARCEL		16	//Indicates a parcel delivery address  (Default)
#define VCARD_ADR_HOME			17	//Indicates a home delivery address
#define VCARD_ADR_WORK			18	//Indicates a work delivery address  (Default)
//define PHOTO Format type
#define VCARD_PHOTO_GIF			19	//Indicates Graphics Interchange Format
#define VCARD_PHOTO_CGM			20	//Indicates ISO Computer Graphics Metafile
#define VCARD_PHOTO_WMF			21	//Indicates MS Windows Metafile
#define VCARD_PHOTO_BMP			22	//Indicates MS Windows Bitmap
#define VCARD_PHOTO_MET			23	//Indicates IBM PM Metafile
#define VCARD_PHOTO_PMB			24	//Indicates IBM PM Bitmap
#define VCARD_PHOTO_DIB			25	//Indicates MS Windows DIB
#define VCARD_PHOTO_PICT		26	//Indicates an Apple Picture format
#define VCARD_PHOTO_TIFF		27	//Indicates a Tagged Image File Format
#define VCARD_PHOTO_PS			28	//Indicates Adobe PostScript format
#define VCARD_PHOTO_PDF			29	//Indicates Adobe Page Description Format
#define VCARD_PHOTO_JPEG		30	//Indicates ISO JPEG format
#define VCARD_PHOTO_MPEG		31	//Indicates ISO MPEG format
#define VCARD_PHOTO_MPEG2		32	//Indicates ISO MPEG version 2 format
#define VCARD_PHOTO_AVI			33	//Indicates Intel AVI format
#define VCARD_PHOTO_QTIME		34	//Indicates Apple QuickTime format
//define SOUND type
#define VCARD_SOUND_WAVE		35	//Indicates Wave format
#define VCARD_SOUND_PCM			36	//Indicates MIME basic audio type
#define VCARD_SOUND_AIFF		37	//Indicates AIFF format
//define EMAIL type
#define VCARD_EMAIL_AOL			38	//Indicates America On-Line
#define VCARD_EMAIL_APPLELINK	39	//Indicates AppleLink
#define VCARD_EMAIL_ATTMAIL		40	//Indicates AT&T Mail
#define VCARD_EMAIL_CIS			41	//Indicates CompuServe Information Service
#define VCARD_EMAIL_EWORLD		42	//Indicates eWorld
#define VCARD_EMAIL_INTERNET	43	//Indicates Internet SMTP (default)
#define VCARD_EMAIL_IBMMAIL		44	//Indicates IBM Mail
#define VCARD_EMAIL_MCIMAIL		45	//Indicates MCI Mail
#define VCARD_EMAIL_POWERSHARE	46	//Indicates PowerShare
#define VCARD_EMAIL_PRODIGY		47	//Indicates Prodigy information service
#define VCARD_EMAIL_TLX			48	//Indicates Telex number
#define VCARD_EMAIL_X400		49	//Indicates X.400 service
//define KEY type
#define VCARD_KEY_X509			50	//Indicates a X.509 public key certificate type of key
#define VCARD_KEY_PGP			51	//Indicates an IETF PGP type of key
//define VALUE type
#define VCARD_VALUE_INLINE		52
#define VCARD_VALUE_URL			53
#define VCARD_VALUE_CONTENTID	54
#define VCARD_VALUE_CID			55
#define VCARD_VALUE_X			56
//define ENCODING type
#define VCARD_ENCODING_7BIT		57
#define VCARD_ENCODING_8BIT		58
#define VCARD_ENCODING_QPRINT	59
#define VCARD_ENCODING_BASE64	60
#define VCARD_ENCODING_X		61
//define CHARSET type
#define VCARD_CHARSET_USASCII	62
#define VCARD_CHARSET_ISO8859_1	63
#define VCARD_CHARSET_ISO8859_2	64
#define VCARD_CHARSET_ISO8859_3	65
#define VCARD_CHARSET_ISO8859_4	66
#define VCARD_CHARSET_ISO8859_5	67
#define VCARD_CHARSET_ISO8859_6	68
#define VCARD_CHARSET_ISO8859_7	69
#define VCARD_CHARSET_ISO8859_8	70
#define VCARD_CHARSET_ISO8859_9	71
#define VCARD_CHARSET_UNICODE	72
#define VCARD_CHARSET_UTF_8		73		
#define VCARD_CHARSET_UTF_7		74		
#define VCARD_CHARSET_GB2312	75		
		

//define tel type mask
#define VCARD_TEL_PREE_MASK			0x0001		
#define VCARD_TEL_WORK_MASK			0x0002		
#define VCARD_TEL_HOME_MASK			0x0004		
#define VCARD_TEL_VOICE_MASK		0x0008		
#define VCARD_TEL_FAX_MASK			0x0010		
#define VCARD_TEL_MSG_MASK			0x0020		
#define VCARD_TEL_CELL_MASK			0x0040		
#define VCARD_TEL_PAGER_MASK		0x0080		
#define VCARD_TEL_BBS_MASK			0x0100		
#define VCARD_TEL_MODEM_MASK		0x0200
#define VCARD_TEL_CAR_MASK			0x0400
#define VCARD_TEL_ISDN_MASK			0x0800
#define VCARD_TEL_VIDEO_MASK		0x1000
//VCARD电话结构定义
typedef struct tagvCard_Tel
{
	struct tagvCard_Tel* pNext;
	int Charset;
	unsigned short Mask;		//电话类型掩码
	unsigned char* number;		//电话号码
	unsigned short len;	
}vCard_Tel;

//define addr type mask
#define VCARD_ADR_DOM_MASK			0x0001		
#define VCARD_ADR_INTL_MASK			0x0002		
#define VCARD_ADR_POSTAL_MASK		0x0004		
#define VCARD_ADR_PARCEL_MASK		0x0008
#define VCARD_ADR_HOME_MASK			0x0010		
#define VCARD_ADR_WORK_MASK			0x0020		

//VCARD地址结构定义
typedef struct tagvCard_Addr
{
	struct tagvCard_Addr* pNext;	
	int Charset;				//Charset
	unsigned short Mask;		//地址类型掩码
	unsigned short unused;
	unsigned char* postOffice;	//邮局
	unsigned short postOfficelen;
	unsigned char* extented;	
	unsigned short extentedlen;
	unsigned char* street;		//街道
	unsigned short streetlen;
	unsigned char* locality;	
	unsigned short localitylen;
	unsigned char* region;		
	unsigned short regionlen;
	unsigned char* postalCode;	//邮编
	unsigned short postalCodelen;
	unsigned char* country;		//国家
	unsigned short countrylen;
}vCard_Addr;

//VCARD的Name结构定义
typedef struct tagvCard_Name
{
	int Charset;				//Charset
	unsigned char* famil;		//family name
	unsigned short famillen;
	unsigned char* given;		//given name
	unsigned short givenlen;
	unsigned char* Addition;	//additional names
	unsigned short Additionlen;
	unsigned char* Prefix;		//name prefix
	unsigned short Prefixlen;
	unsigned char* Suffix;		//name suffix
	unsigned short Suffixlen;
	unsigned short unused;
}vCard_Name;

#define VCARD_PHOTO			0
#define VCARD_LOGO			1
//VCARD的phote grahp结构定义
typedef struct tagvCard_Photo
{
	struct tagvCard_Photo* pNext;//@@@
	int formatType;				 //图片格式类型
	int valueType;			     //值类型，可以为图片数据或者URL
	unsigned char* value;
	int len;
}vCard_Photo;

//VCARD的Sound结构定义
typedef struct tagvCard_Sound
{
	struct tagvCard_Sound* pNext;		
	int formatType;					//声音格式类型
	int valueType;					//值类型，可以为声音数据或者URL
	unsigned char* value;
	int len;
}vCard_Sound;

//VCARD的electronic Mail结构定义
typedef struct tagvCard_EMail
{
	struct tagvCard_EMail * pNext;
	int Charset;					//Charset
	int type;						//mail类型
	unsigned char* Email;
	int	len;
}vCard_EMail;

//VCARD的Organization Name and Unit结构定义
typedef struct tagvCard_Org
{
	int Charset;					//Charset
	unsigned char* OrgName;			//Organization Name
	unsigned short OrgNamelen;
	unsigned char* OrgUnit;			//Organization Uint
	unsigned short OrgUnitlen;
	unsigned char* AdditionOrgUnit;	//Additional Organization Uint
	unsigned short AddiOrgUnitlen;
	unsigned short unused;
}vCard_Org;

//VCARD的Comment结构定义, 也用于FN, Title
typedef struct tagvCard_Comment
{
	int Charset;				//Charset
	unsigned char* pData;
	int len;
}vCard_Comment;

//VCARD Pbulic Key结构定义
typedef struct tagvCard_PublicKey
{
	int type;
	unsigned char* pKey;
	int len;
}vCard_PublicKey;

#define VCARD_BIRTH_LEN			12
#define VCARD_MAILER_LEN		100	
#define VCARD_TIMEZONE_LEN		12
#define VCARD_GEOPST_LEN		32
#define VCARD_LASTTEC_LEN		32
#define VCARD_VERSION_LEN		12
#define VCARD_ROLE_LEN			64
//VCARD对象结构定义
typedef struct tagvCard_Obj
{
	struct tagvCard_Obj* pNext;		//下一个VCARD对象， 一个VCARD数据流可能有多个VCADR对象。	
	struct tagvCard_Obj* pAgent;	//AGENT属性				
	vCard_Org* pOrg;				//organization
	vCard_Addr* pAddr;				//地址列表
	vCard_Tel* pTel;				//电话列表					
	vCard_Photo* pLogo;				//logo photo
	vCard_Photo* pPhoto;			//photo
	vCard_Sound* pSound;			//sound
	vCard_Sound* pFNPronounce;		//pronunciation of the Formatted Name.page20
	vCard_PublicKey* pKey;			//publick key
	void* pData;					//memory buffer control, 调用者，不传入也不使用这个域。
	vCard_Name* pName;				//name
	vCard_EMail* pEmail;			//email addr
	vCard_Comment* pComment;		//comment
	vCard_Comment* pFN;				//formatted name,it include honorific prefix,suffix,title
	vCard_Comment* pURL;			//url
	vCard_Comment* pTitle;			//title
	unsigned char* pUID;						//unique identifier.
	unsigned char Role[VCARD_ROLE_LEN];			//Business Category X.520
	unsigned char BirthDay[VCARD_BIRTH_LEN];	//birthday
	unsigned char Mailer[VCARD_MAILER_LEN];		//mailer
	unsigned char TimeZone[VCARD_TIMEZONE_LEN];	//time zone
	unsigned char GeoPst[VCARD_GEOPST_LEN];		//geographic position
	unsigned char LastRev[VCARD_LASTTEC_LEN];	//last revision	
	unsigned char version[VCARD_VERSION_LEN];	//version 2.1.		
}vCard_Obj;

//函数定义
int vCard_Writer( vCard_Obj* pObj, unsigned char** ppbuf, int *buflen );
int vCard_Reader( unsigned char* pData, int Datalen, vCard_Obj** ppObj );
int vCard_Clear( int type, void* pbuf );


/***********************************************************\
			vCalendar 数据集函数声明
\***********************************************************/

//定义返回值
#define VCAL_FAILURE				0
#define VCAL_SUCCESS				1

//Entity type
#define VCAL_VEVENT					1
#define VCAL_VTODO					2

//used by vCalendar object clear
#define VCAL_OPT_WRITER				1
#define VCAL_OPT_READER				2

//vCalendar version
#define VCAL_VERSION				"1.0"

#define VCAL_UNKNOWN				-1
#define VCAL_SEQUENCE_INIT			VCAL_UNKNOWN	//sequencenum设值初始值

//Role value
#define VCAL_ROLE_ATTENDEE			1	//Indicates an attendee at the event or todo
#define VCAL_ROLE_ORGANIZER			2	//Indicates organizer of the event, but ont owner
#define VCAL_ROLE_OWNER				3	//Indicates owner of the event or todo
#define VCAL_ROLE_DELEGATE			4	//Indicates a delegate of another attendee
//Status value
#define VCAL_STATUS_ACCEPTED		5	//Indicates todo was accepted by attendee
#define VCAL_STATUS_NEEDSACTIONS	6	//Indicates event or todo requires action by attendee
#define VCAL_STATUS_SENT			7	//Indicates event or todo was sent out to attendee
#define VCAL_STATUS_TENTATIVE		8	//Indicates event is tentatively accepted by attendee
#define VCAL_STATUS_CONFIRMED		9	//Indicates attendee has confirmed their attendee
#define VCAL_STATUS_DECLINED		10	//Indicates event or todo has been rejected by attendee
#define VCAL_STATUS_COMPLETED		11	//Indicates todo has been completed by attendee
#define VCAL_STATUS_DELEGATED		12	//Indicates event or todo has been delegated by attendee to another
//RSVP value
#define VCAL_RSVP_YES				13	//Indicates a reply is requested
#define VCAL_RSVP_NO				14	//Indicates a reply is ont requeested  
//Expect value
#define VCAL_EXPECT_FYI				15	//Indicates request is for your information
#define VCAL_EXPECT_REQUIRE			16	//Indicates presence is definitely required
#define VCAL_EXPECT_REQUEST			17	//Indicates presence is being requested
#define VCAL_EXPECT_IMMEDIATE		18	//Indicates an immediate reponse needed
//Audio value
#define VCAL_AUDIO_PCM				19	//Indicates MIME basic audio type
#define VCAL_AUDIO_WAVE				20	//Indicates Wave format
#define VCAL_AUDIO_AIFF				21	//Indicates AIFF format
//type value include the audio type and vcard and X-
#define VCAL_TYPE_VCARD				22	//VCARD
//Categories value
#define VCAL_CAT_APPOINT			23	//Appointment
#define VCAL_CAT_BUSINESS			24	//Business
#define VCAL_CAT_EDUCATION			25	//Education
#define VCAL_CAT_HOLIDAY			26	//Holiday
#define VCAL_CAT_MEETING			27	//Meeting
#define VCAL_CAT_MISCELL			28	//miscellaneous 杂项的
#define VCAL_CAT_PERSONAL			29	//Personal
#define VCAL_CAT_PHONECALL			30	//Phone call
#define VCAL_CAT_SICKDAY			31	//Sick day
#define VCAL_CAT_SPECIALOCA			32	//Special Occation
#define VCAL_CAT_TRALVEL			33	//Travel
#define VCAL_CAT_VACATION			34	//Vacation
#define VCAL_CAT_OTHER				VCAL_UNKNOWN
//Class value						
#define VCAL_CLASS_PUBLIC			35	//Indicates general, public access
#define VCAL_CLASS_PRIVATE			36	//Indicates restricted, private access
#define VCAL_CLASS_CONFIDENTIAL		37	//Indicates very restricted, confidential(机密的) access
//Resources value 
#define VCAL_RES_CATERING			38
#define VCAL_RES_CHAIRS				39
#define VCAL_RES_COMPUTER			40
#define VCAL_RES_PROJECTOR			41
#define VCAL_RES_BASEL				42
#define VCAL_RES_VCR				43
#define VCAL_RES_VEHICLE			44
//Value type
#define VCAL_VALUE_INLINE			55
#define VCAL_VALUE_URL				56
#define VCAL_VALUE_CONTENTID		57
#define VCAL_VALUE_CID				58
#define VCAL_VALUE_X				59
//Encoding value
#define VCAL_ENCODING_7BIT			60
#define VCAL_ENCODING_8BIT			61
#define VCAL_ENCODING_QPRINT		62
#define VCAL_ENCODING_BASE64		63
#define VCAL_ENCODING_X				64
//Charset type
#define VCAL_CHARSET_USASCII		65
#define VCAL_CHARSET_ISO8859_1		66
#define VCAL_CHARSET_ISO8859_2		67
#define VCAL_CHARSET_ISO8859_3		68
#define VCAL_CHARSET_ISO8859_4		69
#define VCAL_CHARSET_ISO8859_5		70
#define VCAL_CHARSET_ISO8859_6		71
#define VCAL_CHARSET_ISO8859_7		72
#define VCAL_CHARSET_ISO8859_8		73
#define VCAL_CHARSET_ISO8859_9		74
#define VCAL_CHARSET_UNICODE		75
#define VCAL_CHARSET_UTF_8			76
#define VCAL_CHARSET_UTF_7			77		
#define VCAL_CHARSET_GB2312			78		
	
//Reminder type
#define VCAL_REMINDER_AUDIO			79
#define VCAL_REMINDER_DISPLAY		80
#define VCAL_REMINDER_MAIL			81
#define VCAL_REMINDER_PROCEDURE		82

//Recurrence rule related
#define VCAL_RECU_DAILY				83
#define VCAL_RECU_WEEKLY			84
#define VCAL_RECU_MONTHLY_POS		85
#define VCAL_RECU_MONTHLY_DAY		86
#define VCAL_RECU_YEARLY_MONTH		87
#define VCAL_RECU_YEARLY_DAY		88
#define VCAL_RECU_LASTDAY			999
#define VCAL_RECU_FOREVER			-1

#define VCAL_FIRST				0X80000000	//'+' MP: 1+ ,..., 5+, MD: 1+, ... , 31+
#define VCAL_LAST				0X40000000	//'-' MP: 1- ,..., 5-, MD: 1-, ... , 31-

//Week definition
#define VCAL_WEEK_MON				1
#define VCAL_WEEK_TUES				2
#define VCAL_WEEK_WED				3
#define VCAL_WEEK_THUR				4
#define VCAL_WEEK_FRI				5
#define VCAL_WEEK_STA				6
#define VCAL_WEEK_SUN				7

//structure's component length
#define VCAL_DT_LEN					8
#define VCAL_DTDATE_LEN				8
#define VCAL_DTTIME_LEN				6
#define VCAL_GEO_LEN				16
#define VCAL_TIMEZONE_LEN			8
#define VCAL_VERSION_LEN			8


//Date and Time
typedef struct tagDT
{
	unsigned char Date[VCAL_DT_LEN + 4];	//19980806	=> 1998/8/6
	unsigned char Time[VCAL_DT_LEN];		//083000	=> 8:30:00
}DT;

//Used by Exception Date/Times and Recurrence Date/Times
typedef struct tagMultiDT
{
	int DTNum;								//The number of DT
	DT	D_T[1];								//The DT array, which can be extend
}MultiDT;

//Time Duration
typedef struct tagTDuration
{
	int Weekly;			//TRUE or FALSE
	int Year;
	int Month;
	int Week;
	int Day;
	int Hour;
	int Minute;
	int Second;
}TDuration;

//Daylight Savings 
typedef struct tagDaylight
{
	int Flag;			//daylight saving time flag, TRUE or FALSE
	int	OffsetValue;	//time(hour) offset value positive or negative
	DT	* pDTStart;		//daylight saving time begin
	DT	* pDTEnd;		//daylight saving time end
}Daylight;

//Attachment
typedef struct tagAttachment
{
	struct tagAttachment * pNext;
	int	ValueType;				//Value type which mabye Inline, URL, Content-ID, CID, x-
	unsigned char * pValue;
}Attachment;

//Attendee
typedef struct tagAttendee
{
	struct tagAttendee * pNext;
	int	Role;					//Role of the attendee
	int Status;					//Status of attendee participation in event or todo
	int ReplyReq;				//Whether reply is requested
	int Expect;					//Expection of attendee participation by origianator
	int type;					//for example vcard
	int ValueType;
	unsigned char * pValue;
}Attendee;

//Categories
typedef struct tagCategories
{
	struct tagCategories * pNext;
	int	Category;
	unsigned char * pCategory;	//while Category == VCAL_UNKNOWN
}Categories;

typedef struct tagLocation
{
	int type;					//for example VCARD
	int	ValueType;				//Value type which mabye Inline, URL, Content-ID, CID, x-
	int Charset;				//Charset
	int Valuelen;				//The inline value length
	unsigned char * pValue;
}Location;

//Reminder include Audio reminder, Display reminder, Mail remindar and Procedure reminder
typedef struct tagRemindar
{
	struct tagRemindar * pNext;
	int Remindertype;			//distinguish Display, Mail or Procedure remindar
	DT  * pDTRun;				//Run time
	TDuration * pSnooze;		//Snooze time
	int Repeat;					//Repeat Count
	int AudioType;
	int ValueType;				
	unsigned char * pValue;		//Display string or Email address or Procedure name
	unsigned char * pNote;		//Only for Mail Remindar
}Remindar;

//Recurrence rule
typedef struct tagRecuRule
{
	int	FreqType;				//Frequency such as D, W, MP, MD, YM and YD
	int Interval;				//Interval such as daily, every third day, etc.
	DT	* pEnddate;				//End date
	int	Duration;				//Duration such #4
	int ModifiTotal;			//Modify part total number
	int  * Modifier;			//used when FreqType = MP or FreqType == MD
}RecuRule;

//Define Description or Summary
typedef struct tagContent
{
	int Charset;				//Charset
	int Valuelen;				//The content length
	unsigned char *pValue;		//The content
}Content;

//X- Property Extension
typedef struct tagvCal_X
{
	unsigned char * pName;
	unsigned char * pParam;
	unsigned char * pValue;
}vCal_X;

//vEvent or vTodo
typedef struct tagvCal_entity
{
	struct tagvCal_entity * pNext;
	int EntityType;					//Entity type, vEvent or vTodo			
	DT	  *	pDTCreated;				//Date/Time Created
	DT	  *	pDTCompleted;			//Date/Time	Completed
	DT	  *	pDTDue;					//Due Date/Time
	DT	  *	pDTEnd;					//End Date/Time
	DT	  *	pDTLast_Modified;		//Date/Time Last Modified
	DT	  *	pDTStart;				//Date/Time Start
	MultiDT * pDTExcept;			//Execption Date/Times
	MultiDT * pDTRecurrence;		//Recurrenct Date/Times
	Attachment * pAttach;			//Attachment
	Attendee * pAttendee;			//Attendee
	Categories * pCategories;		//Categories
	Remindar * pRemindar;			//Display, Mail or Procedure remindar
	RecuRule * pExceptRule;			//Exception Rule
	RecuRule * pRecuRule;			//Recurrence Rule
	Location * pLocation;			//Location
	Content  * pDescription;		//Description
	int		Class;					//Classification
	int		RecurrenceNum;			//Number Recurrences
	int		Priority;				//Priority
	int		SequenceNum;			//Sequence Number
	unsigned char  * pRelatedTo;	//Related to
	int		Status;					//Status
	Content  * pSummary;			//Summary
	unsigned char  * pURL;			//URL
	unsigned char  * pUID;			//Unique Identifier
	void * pData;					//used for output cCalendar data stream
}vCal_entity;

//vCalendar Object
typedef struct tagvCal_Obj
{
	struct tagvCal_Obj * pNextObj;						//Next vCalendar object
	vCal_entity		   * pEntity;						//vEvent or vTodo entity
	Daylight		   * pDaylight;						//Daylight
	unsigned char		 GeoPst[VCAL_GEO_LEN];			//Geographic Position
	unsigned char	   * pProcID;						//Product Identifier
	unsigned char		 TimeZone[VCAL_TIMEZONE_LEN];	//Time Zone
	unsigned char		 Version[VCAL_VERSION_LEN];		//Version : 1.0
	void * pData;										//used for output vCalendar data stream
}vCal_Obj;

//Public function
int vCal_Writer( vCal_Obj * pObj, unsigned char ** ppbuf, int * buflen );
int vCal_Reader( unsigned char * pData, int Datalen, vCal_Obj ** ppObj );
int vCal_Clear( int type, void * pbuf );

#ifdef  __cplusplus
}
#endif
#endif //_VCARD_VCAL_GLB_H_
