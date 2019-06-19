/*
***************************************************************************
Module Name:
    avctpcode.h
Abstract:
	avctp pdu code and decode function definition
Author:
    Qian Lixin
Date:
    2001.8.2
**************************************************************************** 
*/
#ifndef _AVCTP_CODE_H
#define _AVCTP_CODE_H    


#define AVCT_SINGLE_FRAME_HdrLen			3
#define AVCT_START_FRAME_HdrLen			4
#define AVCT_CONTINUE_FRAME_HdrLen		1
#define AVCT_END_FRAME_HdrLen				1

void AVCTP_Encode(UCHAR *des,UCHAR *src,DWORD mask);
void AVCTP_Decode(UCHAR *des,UCHAR *src,DWORD mask);

/*p is the structural data to be encoded,l is length,m is mask of p*/
#define AVCTP_CallEncode(p,l,m)	{\
		UCHAR* p1;\
		p1 = (UCHAR*)NEW(l);\
		memcpy(p1,p,l);\
		AVCTP_Encode(p,p1,m);\
		FREE(p1);\
}
	
#define AVCTP_CallDecode(p,l,m)  {\
	UCHAR* p1;\
	p1 = (UCHAR*)NEW(l);\
	memcpy(p1,p,l);\
	AVCTP_Decode(p,p1,m);\
	FREE(p1);\
}


/* 
note:
 because msg_len may be zero,so should not use AVCTP_CallEncode,but
 use Encode!
*/
#define AVCTP_SetSinglePackHeader(ps,ip,qcr,qtl,qd) {\
	struct AVCTP_SinglePackHeader* hd;\
	\
	hd = (struct AVCTP_SinglePackHeader*)NEW(sizeof(struct AVCTP_SinglePackHeader));\
	hd->fb = 0;\
    hd->fb = qtl;\
	(hd->fb) <<= 4;\
	if (qcr != 0x00 ) \
	  (hd->fb) |= 0x02;\
	if (ip != 0x00) \
	  (hd->fb) |= 0x01; \
    hd->pid = qd;\
    AVCTP_Encode((UCHAR*)ps,(UCHAR*)hd,AVCTP_DECODE_SIG_HEADER); /*0x09 ---- 0000 1001*/\
	FREE(hd);\
}
    

#define AVCTP_SetStartPackHeader(h,ip,qcr,qpt,qtl,qn,qpd) {\
	h->fb = 0;\
    h->fb = qtl;\
	(h->fb) <<= 4;\
	if (qcr != 0 ) \
	  (h->fb) |= 0x02;\
	if (ip != 0) \
	  (h->fb) |= 0x01; \
	(h->fb) |= (qpt << 2);\
	h->num = qn;\
    h->pid = qpd;\
    AVCTP_CallEncode((UCHAR*)h,sizeof(struct AVCTP_StartPackHeader),AVCTP_DECODE_START_HEADER); /*0x25 ---- 0010 0101*/\
}
  
#define AVCTP_SetConEndPackHeader(h,qpt,qcr,qtl) {\
     (*h) = 0;\
	 (*h) = qtl;\
	 (*h) <<= 4;\
	 (*h) |= (qpt << 2);\
     (*h) |= (qcr << 1);\
}    


#define AVCTP_EncodeSinglePack(qp,ip,qcr,qtl,qpid,pmsg,qmsg_len) \
{\
	AVCTP_SetSinglePackHeader(qp,ip,qcr,qtl,qpid);\
	if (qmsg_len)  \
		memcpy(qp+3, pmsg, qmsg_len);\
}


#define AVCTP_EncodeStartPack(qp,ip,qcr,qtl,qnum,qpid,pmsg,qmsg_len) \
{\
	AVCTP_SetStartPackHeader(((struct AVCTP_StartPackHeader*)qp),ip,qcr,0x01,qtl,qnum,qpid);\
	if (qmsg_len)  \
		memcpy(qp+4, pmsg, qmsg_len);\
}

#define AVCTP_EncodeConEndPack(qp,qpt,qcr,qtl,pmsg,qmsg_len) \
{\
	AVCTP_SetConEndPackHeader((UCHAR*)qp,qpt,qcr,qtl);\
	if (qmsg_len)  \
		memcpy(qp+1, pmsg, qmsg_len);\
}

/*Get field value of first byte*/
#define AVCTP_DecodePacket(qp,ip,qcr,pt,qtl) \
{\
	 ip = (*qp) & 0x01;\
	 qcr = ((*qp) >> 1) & 0x01;\
	 pt = ((*qp) >> 2) & 0x03;\
	 qtl = (*qp) >> 4;\
}


/*Get Con and End packet*/
#define AVCTP_GetPacket(qp,qlen,pmsg,qmsg_len) \
{\
	pmsg = qp + 1;\
	qmsg_len = qlen - 1;\
}

/*
  len is length of avctp pdu
  msg_len is length of avrcp pdu
*/
#define AVCTP_GetSinglePacket(qp,qlen,pmsg,qmsg_len) \
{\
	pmsg = qp + 3;\
	qmsg_len = qlen - 3;\
}

#define AVCTP_GetStartPacket(qp,qlen,pmsg,qmsg_len) \
{\
	pmsg = qp + 4;\
	qmsg_len = qlen - 4;\
}


#define AVCTP_SendSinglePack(qcid,qip,qcr,qtl,qpid,pmsg,qmsg_len,res) \
{ \
	struct BuffStru * buf; \
	UINT16				len; \
	\
	len=1 +		/*single header size*/ \
	    2 +     /*profile id size*/\
		qmsg_len; /* message info size*/ \
	buf = (struct BuffStru *)BuffNew( (WORD)(len+AVCT_BUFFER_RESERVE), AVCT_BUFFER_RESERVE); \
	AVCTP_EncodeSinglePack((buf->data+buf->off), qip, qcr, qtl, qpid, pmsg, qmsg_len);\
	res = AVCTP_WriteDataToL2cap(qcid, buf, len);\
}

#define AVCTP_SendStartPack(qcid,qcr,qtl,qnum,qpid,pmsg,qmsg_len,res) \
{ \
	struct BuffStru * buf; \
	UINT16				len; \
	\
	len=1+		/*first byte size*/ \
		1+      /*number size*/ \
		2+      /*profile id size*/ \
		qmsg_len; /* message info size*/ \
	buf = BuffNew( (WORD)(len+AVCT_BUFFER_RESERVE), AVCT_BUFFER_RESERVE); \
	AVCTP_EncodeStartPack((buf->data+buf->off), 0, qcr, qtl, qnum, qpid, pmsg, qmsg_len);\
	res = AVCTP_WriteDataToL2cap(qcid, buf, len);\
}

/*Con: 10(0x02), End: 11(0x03)*/
#define AVCTP_SendConEndPack(qcid,qpt,qcr,qtl,pmsg,qmsg_len,res) \
{ \
	struct BuffStru * buf; \
	UINT16				len; \
	\
	len=1+		/*first byte size*/ \
		qmsg_len; /* message info size*/ \
	buf = BuffNew( (WORD)(len+AVCT_BUFFER_RESERVE), AVCT_BUFFER_RESERVE); \
	AVCTP_EncodeConEndPack((buf->data+buf->off),qpt,qcr,qtl,pmsg,qmsg_len);\
	res = AVCTP_WriteDataToL2cap(qcid, buf, len);\
}


#endif
