/*
 * Automatically generated C config: don't edit
 */
#define AUTOCONF_INCLUDED
/*
 * System dependent Selection
 */
#define CONFIG_GNUMAKE 1
#define CONFIG_CHANGE_VERSION 1
#define CONFIG_MAJOR_VERSION 0x4
#define CONFIG_MINOR_VERSION 0x0
#define CONFIG_PATCH_LEVEL 0x0
#define CONFIG_BUILD_RELEASE 0x0316
#define CONFIG_DEBUG_SYMBOL 1
#define CONFIG_DEBUG 1
#undef  CONFIG_GNU
#undef  CONFIG_MICROSOFT
#undef  CONFIG_BORLAND
#define CONFIG_XSCALE 1
#undef  CONFIG_ARM
#undef  CONFIG_ARMTCC
#undef  CONFIG_DIABDCC
#undef  CONFIG_HITACHI
#undef  CONFIG_OS_WIN32
#undef  CONFIG_OS_VXWORKS
#undef  CONFIG_OS_REX
#undef  CONFIG_OS_PSOS
#undef  CONFIG_OS_WINCE
#undef  CONFIG_OS_LINUX
#undef  CONFIG_OS_NUCLEUS
#undef  CONFIG_OS_UC
#undef  CONFIG_OS_RTK
#undef  CONFIG_OS_OSE
#define CONFIG_OS_HOPEN 1
#undef  CONFIG_OS_GKI
#undef  CONFIG_OS_CFW_ST
#undef  CONFIG_OS_SIW
#undef  CONFIG_OS_ZEEVO
#undef  CONFIG_OS_NONE
#define CONFIG_LITTLE_ENDIAN 1
#undef  CONFIG_BIG_ENDIAN
#undef  CONFIG_ALIGN_ONEBYTE
#undef  CONFIG_ALIGN_TWOBYTES
#define CONFIG_ALIGN_FOURBYTES 1
#undef  CONFIG_TYPE_NO_LIMIT
#define CONFIG_TYPE_BIT32_ONLY 1
#undef  CONFIG_TYPE_BIT16_ONLY
#undef  CONFIG_MEMORY_STATIC
#define CONFIG_USER_POOL_SIZE (65536)
#define CONFIG_CPU_BIT32 1
#undef  CONFIG_CPU_BIT16
#undef  CONFIG_FUNC_LIST
#define CONFIG_QUEUE_MAX (64)
#define CONFIG_GAP 1
/*
 * HCI Configuration
 */
#undef  CONFIG_HCI_10A
#undef  CONFIG_HCI_10B
#define CONFIG_HCI_11 1
#undef  CONFIG_HCI_12
#define CONFIG_HCI_STANDARD 1
#undef  CONFIG_TRANSPORT_DLL
#define CONFIG_HCI_DFLT_ASYN_TM (10000)
#define CONFIG_HCI_DFLT_SYN_TM (6000)
#define CONFIG_HCI_DFLT_LKSUPER_TM (20000)
#define CONFIG_HCI_CONN_ACCEPT_TIMEOUT (16000)
#define CONFIG_HCI_ACL_BUFF_SIZE (102400)
#define CONFIG_HCI_SCO_BUFF_SIZE (5120)
#undef  CONFIG_HC2HOST_FC_ENABLE
#undef  CONFIG_HCI_NO_SCO_DATA
#define CONFIG_HCI_RESET_ENABLE 1
#undef  CONFIG_HCI_PAGESCAN_ENABLE
/*
 * L2cap Configuration
 */
#define CONFIG_BLUELET 1
#undef  CONFIG_L2CAP_REDUCED_VER
#undef  CONFIG_PINGINFO_SUPPORT
#define CONFIG_GROUP 1
#undef  CONFIG_RESEND_SUPPORT
#define CONFIG_L2CAP_RTX_TIMEOUT (3000)
#define CONFIG_L2CAP_ERTX_TIMEOUT (3000)
#define CONFIG_L2CAP_HCI_DISCONN_TO (6000)
#define CONFIG_L2CAP_TIMEOUT (60000)
#define CONFIG_L2CAP_MAX_CHANNEL_NUM (100)
/*
 * SDP Configuration
 */
#undef  CONFIG_SDP_FIXED_DATABASE
/*
 * Rfcomm Configuration
 */
#define CONFIG_RFCOMM 1
#define CONFIG_CREDIT (4)
#define CONFIG_RFCOMM_DEFAULT_MTU (1600)
#define CONFIG_RFCOMM_SYS_N1 (127)
#define CONFIG_RFCOMM_SYS_T1 (10000)
#define CONFIG_RFCOMM_SYS_T2 (10000)
/*
 * TCS Configuration
 */
#undef  CONFIG_TCS
/*
 * AVCTP Configuration
 */
#undef  CONFIG_AVCTP
/*
 * AVRCP Configuration
 */
/*
 * AVDTP Configuration
 */
#undef  CONFIG_GAVDP
#undef  CONFIG_AVDTP
/*
 * A2DP Configuration
 */
#define CONFIG_A2DP_DEFAULT_MTU (672)
#define CONFIG_A2DPAPP_WAVEHEADER_NUM (20)
/*
 * Hcrp Configuration
 */
#undef  CONFIG_HCRP
/*
 * Personal Area Networking
 */
#undef  CONFIG_PAN
/*
 * SDAP and Service Database Management
 */
#define CONFIG_SDAP 1
#define CONFIG_SDAP_SERVER 1
#define CONFIG_SDAP_CLIENT 1
#undef  CONFIG_DI_SERVER
#undef  CONFIG_DI_CLIENT
/*
 * Serial port profile
 */
#define CONFIG_SPP 1
#undef  CONFIG_SPP_VSERIAL
#define CONFIG_SPP_CLIENT 1
#define CONFIG_SPP_SERVER 1
/*
 * Headset profile
 */
#define CONFIG_HEP 1
#define CONFIG_HEP_AG 1
#define CONFIG_HEP_HS 1
/*
 * Hands Free profile
 */
#define CONFIG_HFP 1
#define CONFIG_HFP_AG 1
#define CONFIG_HFP_HF 1
#define CONFIG_HFP_HF_SPP 1
#define CONFIG_HFP_ATCMD_TO (3000)
#define CONFIG_HFP_BINP_TO (60000)
/*
 * LAN Access profile
 */
#undef  CONFIG_LAP
/*
 * Dial up networking profile
 */
#define CONFIG_DUN 1
#undef  CONFIG_DUN_CLIENT
#define CONFIG_DUN_SERVER 1
/*
 * FAX profile
 */
#undef  CONFIG_FAX
/*
 * Generic OBEX
 */
#define CONFIG_GOEP 1
#define CONFIG_OBEX 1
#define CONFIG_OBEX_ADDITIONAL 1
#define CONFIG_GOEP_MAX_RECV_LEN (2048)
#define CONFIG_OBEX_MAX_SEND_LEN (2048)
#define CONFIG_GOEP_CLIENT 1
#define CONFIG_GOEP_SERVER 1
#undef  CONFIG_GOEP_AUTH
/*
 * OBEX Basic Imaging
 */
#undef  CONFIG_BIP
#undef  CONFIG_BIP_XML_PARSER
/*
 * OBEX object push
 */
#define CONFIG_OPP 1
#define CONFIG_OPP_CLIENT 1
#define CONFIG_OPP_SERVER 1
/*
 * OBEX file transfer
 */
#define CONFIG_FTP 1
#define CONFIG_FTP_CLIENT 1
#define CONFIG_FTP_SERVER 1
/*
 * IrMC Configuration
 */
#undef  CONFIG_IRMC
#undef  CONFIG_IRMC_VCARD
#undef  CONFIG_IRMC_VCAL
#undef  CONFIG_IRMC_VMSG
#undef  CONFIG_IRMC_VNOTE
/*
 * Human Interface Device profile
 */
#undef  CONFIG_HID
#undef  CONFIG_HID_CLIENT
#undef  CONFIG_HID_SERVER
#undef  CONFIG_HID_SAR
#undef  CONFIG_HID_QOS
/*
 * Cordless Telephony and intercom profile
 */
#undef  CONFIG_CTP_ICP
#define CONFIG_PHONE_FULL 1
#define CONFIG_PHONE_GM 1
/*
 * SOC Configuration
 */
#undef  CONFIG_SOC
#undef  CONFIG_HCI_AVNET
#undef  CONFIG_HCI_ALCATEL
#undef  CONFIG_HCI_ZEEVO
#undef  CONFIG_HCI_NSC
#undef  CONFIG_HCI_STM
#undef  CONFIG_HCI_SILICONWAVE
/*
 * SOC_BACI Configuration
 */
#undef  CONFIG_BACI
/*
 * NAT and DHCP Function
 */
#undef  CONFIG_NAT
#undef  CONFIG_DHCP
/*
 * Bluetooth SDK APIs
 */
#define CONFIG_BTSDK 1
#define CONFIG_BTSDK_OWNTHREAD 1
#define CONFIG_BTSDK_STORE_INFO 1
#undef  CONFIG_BTSDK_AT_CMD
#undef  CONFIG_BTSDK_FLOW_STATISTIC
#undef  CONFIG_BTSDK_REPORT_PRINT
