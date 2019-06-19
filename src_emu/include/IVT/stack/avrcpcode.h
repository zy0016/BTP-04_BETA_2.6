#ifndef RCP_CODE_H
#define RCP_CODE_H


/* transmit parameter to Cfm and Ind */


#ifndef AVRCP_MACRO_CODE
/* AVC encoding in CT side */
void AvcEncUnitInfoReq(UCHAR * pdu);
void AvcEncSubunitInfoReq (UCHAR page,UCHAR * pdu);
void AvcEncPassThrReq(struct EncPassStru* arg,UCHAR * pdu);
void AvcEncVendorReq(struct EncVendorStru* arg,UCHAR * pdu);

/* AVC encoding in TG side */
void AvcEncUnitInfoRsp(struct UnitInfoStru *info,UCHAR* pdu);
void AvcEncSubunitInfoRsp (struct SubUnitInfoStru *info,UCHAR* pdu);
void AvcEncPassThrRsp (struct PassThrStru * rsp,UCHAR* pdu);
void AvcEncVendorRsp (struct VendorDepStru *rsp,UCHAR* pdu);
void AvcEncInterimRsp(UCHAR* pdu);

UCHAR AVRC_DecUnitInfo(void);
UCHAR AVRC_DecSubunitInfo(struct SubUnitInfoStru *smg,UCHAR *bd_addr,UCHAR *pdu);
UCHAR AVRC_DecPassThr(struct PassThrStru *pmsg,UCHAR *bd_addr,UCHAR* pdu);
UCHAR AVRC_DecVedDep(struct VendorDepStru *vmsg,UCHAR *bd_addr,UCHAR* pdu,UINT16 len);
#else


#define AvcEncUnitInfoReq(pdu) \
{\
	*pdu=AVC_CTYPE_STATUS;\
	*(pdu+1)=(AVC_SUBTYPE_UNIT<<3)|AVC_SUBID_IGNORE;\
	*(pdu+2)=AVC_COM_UNIT_INFO;\
	*(pdu+3)=0xff;\
	*(pdu+4)=0xff;\
	*(pdu+5)=0xff;\
	*(pdu+6)=0xff;\
	*(pdu+7)=0xff;\
}


#define AvcEncSubunitInfoReq(page,pdu) \
{\
	*pdu=AVC_CTYPE_STATUS;\
	*(pdu+1)=(AVC_SUBTYPE_UNIT<<3)|AVC_SUBID_IGNORE;\
	*(pdu+2)=AVC_COM_SUBUNIT_INFO;\
	*(pdu+3)=(page<<4)|0x00;\
	*(pdu+4)=0xff;\
	*(pdu+5)=0xff;\
	*(pdu+6)=0xff;\
	*(pdu+7)=0xff;\
}


#define AvcEncPassThrReq(arg,pdu) \
{\
	UCHAR i;\
	*pdu=AVC_CTYPE_CONTROL;\
	*(pdu+1)=(AVC_SUBTYPE_PANEL<<3)|arg->subunit_id ;\
	*(pdu+2)=AVC_COM_PASS_THROUGH;\
	*(pdu+3)=(arg->state_flag<<7)|(arg->op_id);\
	*(pdu+4)=arg->length;\
	if(arg->length!=0)\
	{\
		memcpy(pdu+5,arg->op_data,arg->length);\
	}\
	if(((arg->length+5)%4)!=0)\
	{\
		for(i=0;i<(4-(arg->length+5)%4);i++)\
		{\
			*(pdu+(arg->length+5)+i)=0x00;\
		}\
	}\
}


#define AvcEncVendorReq(arg,pdu) \
{\
	UCHAR i;\
	*pdu=arg->ctype;\
	*(pdu+1)=(AVC_SUBTYPE_VENDOR_UNIQUE<<3)|AVC_SUBID_0;\
	*(pdu+2)=AVC_COM_VENDOR_DEPENDENT;\
	memcpy(pdu+3,arg->company_id,3);\
	memcpy(pdu+6,arg->vendor_data,arg->len);\
	if(((arg->len + 6)%4)!=0)\
	{\
		for(i=0;i<(4-(arg->len +6)%4);i++)\
		{\
			*(pdu + 6 + arg->len + i)=0x00;\
		}\
	}\
}


#define AvcEncUnitInfoRsp(info,pdu) \
{\
	*pdu=info->response;\
	*(pdu+1)=(AVC_SUBTYPE_UNIT<<3)|AVC_SUBID_0;\
	*(pdu+2)=AVC_COM_UNIT_INFO;\
	*(pdu+3)=0x07;\
	*(pdu+4)=(info->unit_type<<3)|0x00;\
	memcpy(pdu+5,info->company_id,3);\
}


#define AvcEncSubunitInfoRsp(info,pdu) \
{\
	*pdu=info->response;\
	*(pdu+1)=(AVC_SUBTYPE_UNIT<<3)|AVC_SUBID_0;\
	*(pdu+2)=AVC_COM_SUBUNIT_INFO;\
	*(pdu+3)=(info->page<<4)|0x00;\
	memcpy(pdu+4,info->page_data,4);\
}


#define AvcEncPassThrRsp(rsp,pdu) \
{\
	UCHAR i;\
	*pdu=rsp->response;\
	*(pdu+1)=(AVC_SUBTYPE_PANEL<<3)|rsp->subunit_id;\
	*(pdu+2)=AVC_COM_PASS_THROUGH;\
	*(pdu+3)=(rsp->state_flag<<7)|(rsp->op_id);\
	*(pdu+4)=rsp->length;\
	if(rsp->length!=0)\
	{\
		memcpy(pdu+5,rsp->op_data,rsp->length);\
	}\
	if(((rsp->length+5)%4)!=0)\
	{\
		for(i=0;i<(4-(rsp->length+5)%4);i++)\
		{\
			*(pdu+(rsp->length+5)+i)=0x00;\
		}\
	}\
}



#define AvcEncVendorRsp(rsp,pdu) \
{\
	UCHAR i;\
	*pdu=rsp->response;\
	*(pdu+1)=(AVC_SUBTYPE_VENDOR_UNIQUE<<3)|AVC_SUBID_0;\
	*(pdu+2)=AVC_COM_VENDOR_DEPENDENT;\
	memcpy(pdu+3,rsp->company_id,3);\
	memcpy(pdu+6,rsp->vendor_data,rsp->length );\
	if(((rsp->length+6)%4)!=0)\
	{\
		for(i=0;i<(4-(rsp->length+6)%4);i++)\
		{\
			*(pdu+(rsp->length+6)+i)=0x00;\
		}\
	}\
}


#define AvcEncInterimRsp(pdu) \
{\
	*pdu=AVC_RSP_INTERIM;\
}
#endif /*#ifndef AVRCP_MACRO_CODE*/



void AVRC_RspParser(UINT16 len,UCHAR* bd_addr,UCHAR * pdu);

void AVRC_CmdParser(UINT16 len,UCHAR* bd_addr,UCHAR* arc);


#endif
