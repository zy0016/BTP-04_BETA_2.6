#ifndef _BT_SDAP_UISTRU_H
#define _BT_SDAP_UISTRU_H

/*++++++++++++++++++++ SDAP UI Structure Definition +++++++++++++++++++*/
struct SDAP_UUIDStru
{
    DWORD Data1;
    WORD Data2;
    WORD Data3;
    UCHAR Data4[8];
};

struct SDAP_UInt64Stru
{
	DWORD	higher_4bytes;
	DWORD	lower_4bytes;
};

struct SDAP_DataEleStru {
	DWORD	size;
	UCHAR	descriptor;
	UCHAR	data[1];
};

struct SDAP_SvcAttrStru {
	WORD	attr_id;
	WORD	align_byte;
	struct SDAP_DataEleStru	attr_val;
};

struct SDAP_BrowseInfoStru {
  WORD				trans_hdl;				/* transaction handle representing the browsing transaction */
  WORD				mask;					/* flag used to identify the type of group_id, */
  struct SDAP_UUIDStru	group_id;			/* any UUID value used to identify the browse group 
   											   that is, the size of the UUID value*/
  WORD				duration;				/* duration of the ServiceSearch transaction */
};

struct SDAP_SvcAttrIDStru {
  UCHAR				mask;
  UCHAR				reserved;
  WORD				id;
  WORD				end_id;
};

struct SDAP_SearchInfoStru {
  WORD				trans_hdl;				
  WORD				mask;
  struct SDAP_UUIDStru	pattern;
  WORD				duration;
  WORD				id_count;
  struct SDAP_SvcAttrIDStru	attr_id_list[1];
};

/*++++++++++++++ Service Information Structure +++++++++++++++*/
struct SDAP_GeneralInfoStru {
	WORD           size;				/* sizeof SDAP_GeneralInfoStru, include additional bytes allocated dynamically */
	WORD			reserved1;			/* reserved for alignment */
	DWORD			svc_hdl;			/* service handle */
	UCHAR			reserved2[1];		/* reserved for alignment */
};

struct SDAP_CTPInfoStru {
	WORD 			size;			    /* sizeof SDAP_CTPInfoStru */
	WORD 			mask;				/* optional attribute mask */
	DWORD 			svc_hdl;			/* service handle */
	UCHAR			ext_network;		/* extern network */
};

struct SDAP_ICPInfoStru {
	WORD 			size;			    /* sizeof SDAP_ICPInfoStru */
	WORD 			reserved;			/* reserved for alignment */
	DWORD 			svc_hdl;			/* service handle */
};

struct SDAP_SPPInfoStru {
	WORD 			size;			    /* sizeof SDAP_SPPInfoStru */
	WORD			reserved;			/* reserved for alignment */
	DWORD			svc_hdl;			/* service handle */
	UCHAR			svr_chnl;			/* service channel */
};

struct SDAP_HEPHSInfoStru {
	WORD 			size;			    /* sizeof SDAP_HEPHSInfoStru */
	WORD			mask;				/* optional attribute mask */
	DWORD			svc_hdl;			/* service handle */
	UCHAR			svr_chnl;			/* service channel */
};

struct SDAP_HEPAGInfoStru {
	WORD 			size;			    /* sizeof SDAP_HEPAGInfoStru */
	WORD			reserved;			/* reserved for alignment */
	DWORD			svc_hdl;			/* service handle */
	UCHAR			svr_chnl;			/* service channel */
};

struct SDAP_HFPHFInfoStru {
	WORD			size;			    /* sizeof SDAP_HFPHFInfoStru */
	WORD			mask;				/* optional attribute mask */
	DWORD			svc_hdl;			/* service handle */
	UCHAR			svr_chnl;			/* service channel */
	UCHAR			reserved;			/* reserved for alignment */
	WORD			features;			/* supported features */
};

struct SDAP_HFPAGInfoStru {
	WORD			size;			    /* sizeof SDAP_HFPAGInfoStru */
	WORD			mask;				/* optional attribute mask */
	DWORD			svc_hdl;			/* service handle */
	UCHAR			svr_chnl;			/* service channel */
	UCHAR			ag_network;			/* network attribute */
	WORD			features;			/* supported features */
};

struct SDAP_DUNInfoStru {
	WORD			size;			    /* sizeof SDAP_DUNInfoStru */
	WORD			mask;				/* optional attribute mask */
	DWORD			svc_hdl;			/* service handle */
	UCHAR			svr_chnl;			/* service channel */
};

struct SDAP_FaxInfoStru {
	WORD 			size;			    /* sizeof SDAP_FaxInfoStru */
	WORD			mask;				/* optional attribute mask */
	DWORD			svc_hdl;			/* service handle */
	UCHAR 			svr_chnl;			/* service channel */
};

struct SDAP_LAPInfoStru {
	WORD			size;			    /* sizeof SDAP_LAPInfoStru */
	WORD			reserved;			/* reserved for alignment */
	DWORD			svc_hdl;			/* service handle */
	UCHAR			svr_chnl;			/* service channel */
};

struct SDAP_OPPInfoStru {
	WORD 			size;			    /* sizeof SDAP_OPPInfoStru, include additional bytes for fmt_list */
	WORD			reserved;			/* reserved for alignment */
	DWORD			svc_hdl;			/* service handle */
	UCHAR			svr_chnl;			/* service channel */
	UCHAR			fmt_num;			/* number of supported formats */
	UCHAR			fmt_list[1];		/* list of supported formats */
};

struct SDAP_FTPInfoStru {
	WORD			size;			    /* sizeof SDAP_FTPInfoStru */
	WORD			reserved;			/* reserved for alignment */
	DWORD			svc_hdl;			/* service handle */
	UCHAR			svr_chnl;			/* server channel */
};

struct SDAP_SyncInfoStru {
	WORD			size;			    /* sizeof SDAP_SyncInfoStru, include additional bytes for stores_list */
	WORD 			reserved;			/* reserved for alignment */
	DWORD			svc_hdl;			/* service handle */
	UCHAR			svr_chnl;			/* server channel */
	UCHAR			stores_num;			/* number of supported data stores */
	UCHAR 			stores_list[1];		/* list of supported data stores */
};

struct SDAP_SyncCmdInfoStru {
	WORD			size;			    /* sizeof SDAP_SyncCmdInfoStru. */
	WORD 			reserved;			/* reserved for alignment */
	DWORD			svc_hdl;			/* service handle */
	UCHAR			svr_chnl;			/* server channel */
};

struct SDAP_PANInfoStru {
	WORD			size;				/* sizeof SDAP_NAPInfoStru, include additional bytes for type_list */
	WORD 			reserved;			/* reserved for alignment */
	DWORD			svc_hdl;			/* service handle */
	WORD			svc_type;			/* type of PAN service in UUID16, it can be GN, NAP or PANU */
	WORD			psm;				/* PSM value in ProtocolDescriptorList attribute */
	WORD 			secu_desc;			/* security description */
	WORD 			net_access_type;	/* net access type, only valid for NAP service */
	DWORD			max_access_rate;	/* maximum possible network access data rate, only valid for NAP service */
	WORD			type_num;			/* number of supported network packet type */
	WORD			type_list[1];		/* list of supported network packet type */
};

struct SDAP_AVRCPInfoStru {
	WORD			size;				/* size of SDAP_AVCTInfoStru */
	WORD			reserved;			/* reserved for alignment */
	DWORD			svc_hdl;			/* service handle */
	WORD			svc_type;			/* type of AVRCP service, it can be CT or TG */
	WORD			sup_cg;				/* Supported Categories */
};

struct SDAP_A2DPInfoStru {
	WORD			size;				/* size of SDAP_A2DPInfoStru */
	WORD			reserved;			/* reserved for alignment */
	DWORD			svc_hdl;			/* service handle */
	WORD			svc_type;			/* type of A2DP service, it can be SRC or SNK */
	WORD			features;			/* supported features */
};

struct SDAP_HCRPInfoStru {
	WORD			size;				/* sizeof SDAP_HCRPInfoStru */
	WORD			mask;				/* optional attribute mask */
	DWORD			svc_hdl;			/* service handle */
	WORD 			svc_type;			/* the UUID16 value of the most specific class in ServiceClassIDList Attribute */
	WORD 			conn_type;			/* connection type, it can be Control Channel or Notification Channel. */
	WORD			psm_main;			/* psm value for ProtocolDescriptorList attribute */
	WORD			psm_data;			/* psm value for AdditionalProtocolDescriptorList attribute, only valid for 
										   when .*/
	DWORD			svc_id;				/* ServiceID attribute, only valid when conn_type is set to Control Channel. */
	WORD 			size_of_1284id;		/* size of the 1284id attribute, only valid when conn_type is set to Control Channel. */
	UCHAR			str_1284id[1];		/* 1284id attribute value, only valid when conn_type is set to Control Channel. */
};

struct SDAP_HIDInfoStru {
	WORD 			size;				/* size of SDAP_HIDInfoStru, include additional bytes for stores_list */
	WORD 			mask;				/* optional or mandatory Bool type attribute mask */
	DWORD 			svc_hdl;			/* service handle */
	WORD			release_num;		/* HID device release number */
	WORD			parser_ver;			/* HID parser version */
	UCHAR			sub_cls;			/* HID device subclass */
	UCHAR			country_code;		/* HID country code */
	WORD			super_to;			/* HID supervision timeout */
	WORD			profile_ver;		/* HID ProfileVersion attribute value*/
	WORD			desc_list_size;		/* total size of the descriptor list. It also marks the start point 
										   of the report list in the successive memory. */
	UCHAR			list[1];			/* list of HID class descriptor. */
};

struct SDAP_DIInfoStru {
	WORD			size;				/* size of SDAP_DIInfoStru, include additional bytes for str_url_list */
	WORD			mask;				/* optional or mandatory Bool type attribute mask */
	DWORD			svc_hdl;			/* service handle */
	WORD			spec_id;			/* value of SpecificationID attribute */
	WORD			vendor_id;			/* value of VendorID attriubte */
	WORD			product_id;			/* value of ProductID attribute */
	WORD			version;			/* value of Version attribute */
	WORD			vendor_id_src;		/* value of VendorIDSource attribute */
	WORD			list_size;			/* size of the text string list */
	UCHAR			str_url_list[1];	/* List of ClientExecutableURL, DocumentationURL and 
										  ServiceDescription attributes. */
};

struct SDAP_BIPImgInfoStru {
	WORD 			size;				/* size of SDAP_BIPImgInfoStru */
	WORD			reserved;			/* reserved for alignment */
	DWORD			svc_hdl;			/* service handle */
	UCHAR			svr_chnl;			/* server channel */
	UCHAR			capabilities;		/* supported capabilities */
	WORD			features;			/* supported features */
	DWORD			functions;			/* supported functions */
	struct SDAP_UInt64Stru total_data_capacity; /* value of the TotalImagingDataCapacity attribute */
};

struct SDAP_BIPObjInfoStru {
	WORD			size;				/* size of SDAP_BIPObjInfoStru */
	WORD			mask;				/* attribute mask. It presents size descriptor of svc_id currently. */
	DWORD			svc_hdl;			/* service handle */
	WORD			obj_type;			/* a UUID16 representing the type of the object, it can be a referenced object 
										  or an automatic archive. */
	UCHAR			svr_chnl;			/* server channel */
	struct SDAP_UUIDStru svc_id;		/* The value of ServiceId attribute */
	DWORD			functions;			/* supported functions */
};

struct SDAP_SAPInfoStru {
	WORD			size;				/* size of SDAP_SAPInfoStru */
	WORD			mask;				/* attribute mask */
	DWORD			svc_hdl;			/* service handle */
	UCHAR			svr_chnl;			/* server channel */
};

#define SDAP_HEPInfoStru			SDAP_HEPHSInfoStru
#define SDAP_HFPInfoStru			SDAP_HFPHFInfoStru

#define SDAP_GNInfoStru				SDAP_PANInfoStru
#define SDAP_NAPInfoStru			SDAP_PANInfoStru
#define SDAP_PANUInfoStru			SDAP_PANInfoStru

#define SDAP_AVCTInfoStru			SDAP_AVRCPInfoStru
#define SDAP_AVTGInfoStru			SDAP_AVRCPInfoStru

#define SDAP_A2DPSrcInfoStru		SDAP_A2DPInfoStru
#define SDAP_A2DPSnkInfoStru		SDAP_A2DPInfoStru

#define SDAP_HCRPCtrlInfoStru		SDAP_HCRPInfoStru
#define SDAP_HCRPNotifyInfoStru		SDAP_HCRPInfoStru

#define SDAP_BIPRefObjInfoStru		SDAP_BIPObjInfoStru
#define SDAP_BIPArchInfoStru		SDAP_BIPObjInfoStru


#endif
