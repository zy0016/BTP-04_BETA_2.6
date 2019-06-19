#ifndef TCSUISTRU_H
#define TCSUISTRU_H

#define TCS_SIGNAL_NONE	0xff
#define TCS_IN_BAND_TONE_AVAILABLE		0x08

/*-------------------------   DISCONNECT CAUSE    ------------------------------*/
#define CAUSE_PHONE_NORMAL_CALL_CLEARING				16
#define CAUSE_PHONE_USER_BUSY							17
#define CAUSE_PHONE_NO_USER_CORRESPONDING				18
#define CAUSE_PHONE_USER_REJECT							21
#define CAUSE_PHONE_NONSELECTED_USER_CLEARING			26
#define CAUSE_PHONE_NO_CHANNEL_AVAILABLE				34
#define CAUSE_PHONE_REQUESTED_CHANNEL_NOT_AVAILABLE	44
#define CAUSE_PHONE_RECOVERY_ON_TIMER_EXPIRY			102
#define CAUSE_PHONE_LINK_LOSS							       255

/*--------------------------FAST ACCESS FAIL CAUSE -----------------------------*/
#define CAUSE_UNALLOCATED_NUMBER			1
#define CAUSE_USER_BUSY					17
#define CAUSE_SUBSCRIBER_ABSENT			20

#define CALLING_NUM(t) ((t).data)
#define CALLED_NUM(t)   (&((t).data[(t).calling_numb_len]))
#define COMPANY(t) (&((t).data[(t).calling_numb_len+(t).called_numb_len]))
#define TCSCALL_EXT_LEN(t) ((t).calling_numb_len+(t).called_numb_len+(t).comp_len)

struct CompanySpecStru {
	UCHAR comp_len;
	UCHAR company[1];
};

struct TcsCallInfoStru {
	UCHAR	call_cls;
	UCHAR    signal;
	UCHAR	code_type;
	UCHAR	packet_type;
	UCHAR	calling_numb_len;
	UCHAR	called_numb_len;
	UCHAR	comp_len;
	UCHAR    data[1];
};

struct SetupReqParamStru{
	UCHAR	bd_addr[BDLENGTH];
	struct TcsCallInfoStru info;
};

struct SetupReqExParamStru{
	UCHAR	bd_addr[BDLENGTH];
	UCHAR    sending_complete;
	struct TcsCallInfoStru info;
};

/* struct SetupIndExParamStru for full TCS */
struct SetupIndExParamStru{
	UCHAR	sending_complete; /* 1:	complete, 0:	incomplete */
	struct TcsCallInfoStru info;
};

struct ProgressStru {
	UCHAR progress; /* TCS_IN_BAND_TONE_AVAILABLE:	In-band information is available */
	struct CompanySpecStru comp;
};

struct ProgressIndExStru {
	UCHAR  progress; /* TCS_IN_BAND_TONE_AVAILABLE:	In-band information is available */
	UCHAR  sco_hdl;
	struct CompanySpecStru comp;
};

struct DisconnReqInStru {
	UCHAR cause;
	struct CompanySpecStru comp;
};

struct DisconnReqExStru {
	UCHAR cause;
	UCHAR progress;
	struct CompanySpecStru comp;
};

struct DisconnIndExStru {
	UCHAR  cause;
	UCHAR  progress;
	UCHAR  sco_hdl;
	struct CompanySpecStru comp;
};

struct InfoReqExStru {
	UCHAR    sending_complete;
	UCHAR	keypad;
	UCHAR	calling_numb_len;
	UCHAR	called_numb_len;
	UCHAR	audio_control_len;
	UCHAR	comp_len;
	UCHAR    data[1];
};

struct InfoIndExStru {
	UCHAR    sending_complete;
	UCHAR	keypad;
	UCHAR	calling_numb_len;
	UCHAR	called_numb_len;
	UCHAR	audio_control_len;
	UCHAR	comp_len;
	UCHAR    data[1];
};

/* the following struct defined for gm unit */
struct GmRightsRspStru{
    UCHAR bd_addr[BDLENGTH];
    UCHAR flag;
	struct CompanySpecStru comp;
};

struct GmFastAccessReqStru{
    UCHAR bd_addr[BDLENGTH];
	struct CompanySpecStru comp;
};

struct GmFastAccessRspStru{
    UCHAR flag;
    UCHAR cause;
    struct CompanySpecStru comp;
};

#endif
