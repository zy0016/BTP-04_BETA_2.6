#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "window.h"
#include "winpda.h"
#include "plx_pdaex.h"
#include "me_wnd.h"
#include "imesys.h"
#include "pubapp.h"
#include "network.h"
#include "me_wnd.h"

#define   IDC_NETWORKSELECTION		0x101
#define   IDC_PREFERREDNETWORKS		0x102

#define   IDC_NETWORKSELECTIONLIST	0x201

#define   IDM_ASSIGN			0x301
#define	  IDM_REMOVE			0x302
#define   IDC_PREFERREDLIST		0x303
#define	  IDC_PREFERREDLIST1	0x304
#define   IDC_PREFERREDLIST2	0x305
#define   IDC_PREFERREDLIST3	0x306
#define   IDC_PREFERREDLIST4	0x307

#define   WM_WRITEPREFLIST		0x308
#define   WM_WRITEPREFLIST2		0x309

//Control ID for Preferred Network selection list
#define   IDC_PREFERREDNETWORKSSELECTIONLIST		0x401

#define   IDS_ASSIGN			ML("Assign")
#define   IDS_REMOVE			ML("Remove")

#define   ME_MSG_SEARCHNETOPERATOR			(WM_USER+100)
#define   ME_MSG_SETOPERATORMODE			(WM_USER+101)
#define   ME_MSG_CHECKSIGNAL				(WM_USER+102)
#define   ME_MSG_REGISTERNETWORK			(WM_USER+103)
#define   ME_MSG_READPREFERREDOPERATOR		(WM_USER+104)
#define   ME_MSG_WRITEPREFERREDOPERATOR		(WM_USER+105)
#define   WM_PREFERREDNETCHANGED			(WM_USER+106)
#define   ME_MSG_REMOVEPREFERREDNET			(WM_USER+107)
#define   ME_MSG_GET_CURRENT_OPERATOR_MODE	(WM_USER+108)
#define   ME_MSG_GETIMSI					(WM_USER+109)

#define   WM_CANCELSEARCH					(WM_USER+200)
#define   WM_NETMODE_CHANGED				(WM_USER+201)

#define MAXOPERATORNUM		50

static LRESULT NetworksProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static BOOL CreateNetworksList(HWND hParentWnd,HWND *hNetworkSelection,HWND *hPreferredNetworks);
static void NetworkSelection(int *iActiveMode);
static LRESULT NetworkSelectionProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static void AvailableNetworksListWin();
static LRESULT AvailableNetworksListProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );

static void PreferredNetworks();
static LRESULT PreferredNetworksProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
//static BOOL CreatePreferredNetworksList(HWND hParentWnd,HWND *hList,int iItemNum);
static BOOL CreatePreferredNetworksList(HWND hParentWnd,HWND *hList, PREFER_OPRATOR* pReadPreferredOperator,int iReadPreferredOperatorNum);
static void InitVScrollBar(HWND hWnd,int iItemNum);
static void DoVScroll(HWND hWnd,WPARAM wParam);
static void PreferredNetworksSelectionList(void *info);
static LRESULT PreferredNetworksSelectionListProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static BOOL IsIndexUsed(int i);

static int nCurFocus;
static NETOPRATOR *pNetOperator;
static int OperateNum;

static int j2;

static int me_handle = 0;

#define MAXPREFERREDNETWORK		8   //In Siemens MC55,the max number of preferred network is 8
static PREFER_OPRATOR *pReadPreferredOperator;
static int iReadPreferredOperatorNum;

typedef struct tagMCC_INFO{
	unsigned short mcc;
	char  country[4];
}MCC_INFO;

static const unsigned char * const Net_oper[]=
{  
	"20201  GR COSMOTE ", 
    "20205  GR PANAFON ", 
    "20210  GR TELESTET ", 
	"20404  NL LIBERTEL ", 
    "20408  NL KPN ", 
    "20416  Ben NL ", 
    "20420  Orange NL ", 
    "20412  NL TELFORT ", 
	"20601  BEL PROXIMUS ", 
    "20610  B mobistar ",
	"20620  Orange ", 
	"20801  Orange F ", 
    "20810  F SFR ", 
    "20820  BOUYGTEL ", 
	"21303  STA-MOBILAND ", 
	"21401  E VODAFONE ", 
    "21402  Movistar ", 
    "21403  E AMENA ", 
    "21407  Movistar ", 
	"21601  H PANNON GSM ", 
    "21630  WESTEL ", 
    "21670  Vodafone HU ", 
    "21803  BA-ERONET ", 
	"21805  GSM MS1 ", 
	"21890  BIH GSMBIH ", 
	"21901  HR-CRONET ", 
    "21910  HR VIP ", 
	"22001  YU MOBTEL ", 
    "22002  ProMonte ", 
    "22003  YUG 03 ", 
    "22004  MONET ", 
    "22201  I TIM ", 
    "22210  I-OMNITEL ", 
	"22288  I WIND ", 
    "22298  IT BLU ",
	"22601  RO CONNEX ", 
    "22603  RO Cosmorom ", 
    "22610  RO dialog ", 
	"22801  SWISS GSM ", 
    "22802  sunrise ", 
    "22803  Orange CH ", 
	"23001  PAEGAS-CZ ", 
    "23002  EUROTEL - CZ ", 
    "23003  OSKAR ", 
	"23101  Globtel ", 
    "23102  EUROTEL-SK ", 
    "23201  A1", 
    "23203  A max.", 
    "23205  one ", 
    "23207  telering ",
	"23410  BT CELLNET ", 
    "23415  UK VODAFONE ", 
    "23430  One 2 One ", 
    "23433  ORANGE ", 
    "23450  JT GSM ", 
    "23455  GUERNSEY TEL UK ", 
    "23458  MANX ", 
	"23801  DK TDK-MOBIL ", 
    "23802  DK SONOFON ", 
    "23820  TELIA DK ", 
    "23830  DK mobilix ", 
	"24001  TELIA S ", 
    "24007  S COMVIQ ", 
    "24008  S EUROPOLITAN ", 
	"24201  N Telenor ", 
    "24202  N NetCom GSM ", 
	"24403  FI TELIA ", 
    "24405  FI RADIOLINJA ", 
    "24409  FI FINNET ", 
    "24491  FI SONERA ", 
    "24601  OMNITEL LT ", 
	"24602  LT BITE GSM ", 
    "24701  LMT GSM ", 
    "24702  LV TELE2 ",
	"24801  EE EMT GSM ", 
    "24802  EE RLE ", 
    "24803  EE Q GSM ", 
	"25001  MTS-RUS ", 
    "25002  NorthWestGSM RUS ", 
    "25005  SCS RUS ", 
    "25007  RUS SMARTS ", 
    "25010  RUS DTC ", 
    "25012  RUS 12 Far East ", 
    "25013  RUS Kuban-GSM ", 
    "25016  RUS16 250 16 ", 
    "25017  RUS 17 ", 
    "25028  EXTEL RUS ", 
    "25039  Uraltel ", 
    "25044  NC-GSM ", 
    "25093  Telecom XXI RUS ", 
    "25099  BEE LINE ", 
	"25501  UA UMC ",
	"25502  FLASH-UKR ", 
    "25503  UA-KYIVSTAR ", 
	"25505  UA-GT ", 
	"25701  BY VELCOM ", 
	"25901  MD VOXTEL ", 
    "25902  MD MOLDCELL ", 
	"26001  Plus GSM ", 
    "26002  Era ", 
    "26003  IDEA ", 
	"26201  T-D1 ", 
    "26202  D2 ", 
    "26203  E-Plus ", 
    "26207  Interkom ", 
	"26601  GIBTEL GSM ",
	"26801  P VODAFONE ", 
    "26803  P OPTIMUS ", 
    "26806  P TMN ", 
	"27001  L LUXGSM ", 
    "27077  L TANGO ",
	"27201  IRL Eircell ", 
    "27202  IRL Digifone ", 
    "27203  IRL-METEOR ", 
    "27401  IS  SIMINN ", 
    "27402  IS TAL ", 
    "27404  Viking ", 
    "27601  AMC-AL ", 
	"27801  VODAFONE MLA ", 
    "27821  go mobile ", 
	"28001  CY CYTAGSM ", 
	"28201  GEO-GEOCELL ", 
    "28202  MAGTI-GSM-GEO ", 
    "28203  GEO 03 ", 
	"28301  RA-ARMGSM ", 
	"28401  M-TEL GSM BG ", 
	"28601  TR TURKCELL ", 
    "28602  TR  TELSIM ", 
	"29001  TELE Greenland ",
	"29340  SI.MOBIL ", 
    "29341  SI MOBITEL GSM ", 
	"29401  MKD-MOBIMAK ", 
	"29501  telecom FL ", 
    "29502  Montel ", 
    "29505  FL1 ", 
    "29577  LI TANGO ", 
	"33850  JM DIGICEL ", 
	"34001  F AMERIS ", 
	"40001  AZERCELL GSM ", 
    "40002  BAKCELL GSM 2000 ", 
	"40101  KZ K-MOBILE ", 
    "40102  KZ KCELL ", 
	"40401  ESSARH ", 
    "40407  TATA CELLULAR ", 
    "40410  INA-AIRTL ", 
    "40411  INA ESSAR ", 
    "40412  INA-ESCOTEL ", 
    "40414  SPICE ", 
    "40415  ESSARU ", 
    "40420  INA MaxTouch ", 
    "40421  BPL MOBILE ", 
    "40427  BPL MOBILE ", 
    "40430  COMMAND ", 
    "40431  SPICE ", 
    "40441  INA RPG ", 
    "40442  INA AIRCEL ", 
    "40443  BPL MOBILE ", 
    "40444  INA AIRTEL ", 
    "40445  INA AIRTEL ", 
    "40449  INA AIRTEL ", 
    "40446  BPL MOBILE ", 
    "40460  ESSARR ", 
	"41302  SRI DIALOG ", 
    "41303  SRI-CELLTEL ", 
	"41501  RL Cellis ", 
    "41503  RL LibanCell ", 
	"41601  Fastlink ", 
    "41677  JOR MobileCom ", 
	"41709  SYR MOBILE SYR ", 
	"41902  KT MTCNet ", 
    "41903  KT WATANIYA ",
	"42001  ALJAWWAL ", 
    "42007  KSA EAE ", 
	"42202  OMAN MOBILE ", 
	"42402  UAE ETISALAT ", 
	"42501  Orange ", 
	"42505  JAWWAL ", 
	"42601  BATELCO ",
	"42701  QAT-QATARNET ", 
	"42899  MN MobiCom ", 
	"43211  IR-TCI ", 
    "43214  IR KiSH ",
	"43401  BUZTEL ", 
    "43404  UZB DAEWOO-GSM ", 
    "43405  UZB COSCOM GSM ", 
	"43701  BITEL ",
	"45201  VN MOBIFONE ", 
    "45202  VN VINAFONE ", 
    "45400  CSL ", 
    "45401  NEW WORLD ", 
    "45404  Orange ", 
    "45406  HK SMC ", 
    "45410  NEW WORLD ", 
    "45412  PEOPLES ", 
    "45416  SUNDAY ", 
	"45501  MAC-CTMGSM ", 
	"45601  MOBITEL-KHM ", 
    "45602  KHM-SAMART-GSM ", 
	"45701  LAO GSM ", 
	"46000  China Mobile ", 
    "46001  China Unicom ", 
	"46601  FarEasTone ", 
    "46606  TUNTEX ", 
    "46688  KGT ", 
	"46692  Chunghwa ", 
    "46693  MobiTai ", 
	"46697  TWN GSM ", 
    "46699  TransAsia ", 
	"47001  GrameenPhone ", 
    "47002  BGD AKTEL ", 
	"47003  BD ShebaWorld ",
	"47201  MV DHIMOBILE ", 
	"50212  MY Maxis Mobile ", 
    "50213  TMTOUCH ", 
    "50216  MY DIGI 1800 ", 
    "50217  TIMECel ", 
    "50219  MY CELCOM ", 
	"50501  Telstra ",
	"50502  YES OPTUS ", 
    "50503  VODAFONE AUS ", 
    "50508  One.Tel ", 
	"51001  IND SATELINDOCEL ", 
    "51010  IND TELKOMSEL ", 
    "51011  IND GSM-XL ", 
	"51501  Islacom ", 
    "51502  Globe ", 
    "51503  SMART ", 
	"52001  AIS GSM ", 
    "52018  DTAC ", 
    "52023  GSM 1800 ", 
	"52501  ST-GSM-SGP ", 
    "52502  ST-GSM1800-SGP ", 
    "52503  M1-GSM-SGP ", 
    "52505  STARHUB-SGP ", 
	"52811  BRU-DSTCom ", 
	"53001  VODAFONE NZ ", 
	"54201  FJ VODAFONE ", 
	"54601  NCL MOBILIS ", 
	"54720  F-VINI ", 
	"60201  EGY MobiNiL ", 
    "60202  EGY CLICK GSM ", 
    "60301  AMN ",
	"60401  MOR IAM ",
	"60502  TUNTEL ", 
	"60801  SN ALIZE ", 
    "60802  SN-SENTEL SG ", 
	"61001  MALITEL ML ", 
	"61102  GN LAGUI ", 
	"61201  CI CORA ", 
    "61203  CI Ivoiris ", 
    "61205  TELECEL-CI ", 
    "61302  BF CELTEL ", 
	"61501  TG-TOGO CELL ", 
    "61603  BJ BENINCELL ", 
	"61701  CELLPLUS-MRU ", 
    "61710  EMTEL-MRU ", 
	"61801  LBR OMEGA ", 
	"62001  GH SPACEFON ", 
    "62002  GH ONEtouch ", 
	"62201  TD CELTEL ", 
    "62202  TD LIBERTIS ", 
	"62402  Mobilis ", 
    "62501  CPV MOVEL ",    
	"62803  CELTEL GA ", 
	"62901  CELTEL RC ", 
    "63002  CELTEL CD ", 
    "63089  CD OASIS ", 
	"63301  SEYCEL ", 
	"63310  SEZ AIRTEL ", 
	"63401  MobiTel SDN ", 
	"63510  R-CELL ", 
	"63601  ETH-MTN ", 
	"63701  SOM BARAKAAT ", 
	"63903  KE-KENCELL ", 
    "63902  Safaricom ",    
    "63903  KE-KENCELL ", 
	"64001  Tritel-TZ ", 
    "64003  ZANTEL-TZ ", 
	"64101  UG CelTel ", 
    "64110  MTN-UGANDA ", 
    "64111  UTL TELECEL ", 
	"64202  SAFARIS ", 
	"64301  MOZ-mCel ", 
	"64501  ZM ZAMCELL ", 
    "64601  MG Madacom ", 
	"64602  MG ANTARIS ", 
	"64710  SFR  REUNION ", 
	"64801  ZW NET*ONE ", 
    "64803  TELECEL ZW ", 
    "64804  ZW ECONET ", 
	"64901  MCT NAMIBIA ", 
	"65001  MW CP 900 ", 
    "65010  CELTEL ", 
	"65101  VCL COMMS ", 
    "65201  BW MASCOM ", 
    "65202  BW VISTA ", 
	"65310  Swazi-MTN ", 
	"65501  VodaCom-SA ", 
    "65510  MTN-SA ", 
	"73401  VZ INFO ", 
    "73402  DIGITEL ", 
	"74601  ICMS SR ", 
	""
};
/*This table got from Benefon, it provide the relation table about MCC and country*/
static const MCC_INFO  MCC_Info[] =
/*  MCC    Country abbrevation   COMMENT                                      */
{
  { 0x0202, "GR"  },  /* Greece */
  { 0x0204, "NL"  },  /* Netherlands */
  { 0x0206, "BEL" },  /* Belgium */
  { 0x0208, "F"   },  /* France */
  { 0x0213, "AND" },  /* Andorra */
  { 0x0214, "E"   },  /* Spain */
  { 0x0216, "H"   },  /* Hungary */
  { 0x0218, "BIH" },  /* Boznia and Herzegovina */
  { 0x0219, "HR"  },  /* Croatia */
  { 0x0220, "YU"  },  /* Serbia */
  { 0x0222, "I"   },  /* Italy */
  { 0x0226, "RO"  },  /* Romania */
  { 0x0228, "CH"  },  /* Switzerland */
  { 0x0230, "CZ"  },  /* Czech Republic */
  { 0x0231, "SK"  },  /* Slovenia */
  { 0x0232, "A"   },  /* Austria */
  { 0x0234, "UK"  },  /* United Kingdom */
  { 0x0238, "DK"  },  /* Denmark */
  { 0x0240, "S"   },  /* Sweden */
  { 0x0242, "N"   },  /* Norway */
  { 0x0244, "FI"  },  /* Finland */
  { 0x0246, "LT"  },  /* Lithuania */
  { 0x0247, "LV"  },  /* Latvia */
  { 0x0248, "EE"  },  /* Estonia */
  { 0x0250, "RUS" },  /* Russian Federation */
  { 0x0255, "UA"  },  /* Ukraine */
  { 0x0260, "PL"  },  /* Poland */
  { 0x0262, "D"   },  /* Germany */
  { 0x0266, "GIB" },  /* Gibraltar */
  { 0x0268, "P"   },  /* Portugal */
  { 0x0270, "L"   },  /* Luxembourg */
  { 0x0272, "IRL" },  /* Ireland */
  { 0x0274, "IS"  },  /* Iceland */
  { 0x0276, "AL"  },  /* Albania */
  { 0x0278, "M"   },  /* Malta */
  { 0x0280, "CY"  },  /* Cyprus */
  { 0x0282, "GEO" },  /* Georgia */
  { 0x0283, "RA"  },  /* Armenia */
  { 0x0284, "BG"  },  /* Bulgaria */
  { 0x0286, "TR"  },  /* Turkey */
  { 0x0288, "FO"  },  /* Faroe Islands */
  { 0x0290, "GL"  },  /* Greenland */
  { 0x0293, "SI"  },  /* Slovenia */
  { 0x0294, "MKD" },  /* Macedonia */
  { 0x0302, "CAN" },  /* Canada */
  { 0x0310, "USA" },  /* USA */
  { 0x0311, "USA" },  /* USA */
  { 0x0316, "USA" },  /* USA */
  { 0x0332, "USA" },  /* United States Virgin Islands */
  { 0x0334, "MX"  },  /* Mexico */
  { 0x0338, "JM"  },  /* Jamaica */
  { 0x0340, "F"   },  /* Martinique, Guadeloupe (French Department of) */
  { 0x0344, "AN"  },  /* Antigua and Barbuda */
  { 0x0350, "BM"  },  /* Bermuda */
  { 0x0362, "ANT" },  /* Netherlands Antilles */
  { 0x0363, "AW"  },  /* Aruba */
  { 0x0368, "CU"  },  /* Cuba */
  { 0x0370, "DO"  },  /* Dominican Republic */
  { 0x0374, "TTO" },  /* Trinidad and Tobago */
  { 0x0400, "AZE" },  /* Azerbaijani Republic*/
  { 0x0401, "KZ"  },  /* Kazakstan */
  { 0x0402, "BTN" },  /* ?? */
  { 0x0404, "INA" },  /* India */
  { 0x0412, "AF"  },  /* Afghanistan */
  { 0x0413, "SRI" },  /* Sri Lanka */
  { 0x0414, "MM"  },  /* Myanmar */
  { 0x0415, "RL"  },  /* Lebanon */
  { 0x0416, "JOR" },  /* Jordan */
  { 0x0417, "SYR" },  /* Syria */
  { 0x0419, "KT"  },  /* Kuwait */
  { 0x0420, "KSA" },  /* Saudi Arabia */
  { 0x0421, "YE"  },  /* Yemen */
  { 0x0422, "OMN" },  /* Oman */
  { 0x0424, "UAE" },  /* United Arab Emirates */
  { 0x0425, "IL"  },  /* Israel */
  { 0x0426, "BHR" },  /* Bahrein */
  { 0x0427, "QAT" },  /* Qatar */
  { 0x0428, "MN"  },  /* Mongolia */
  { 0x0432, "IR"  },  /* Iran */
  { 0x0434, "UZB" },  /* Uzbekistan */
  { 0x0436, "TJK" },  /* Tajikistan */
  { 0x0437, "KGZ" },  /* Kyrgyz Republic */
  { 0x0452, "VN"  },  /* Vietnam */
  { 0x0454, "HK"  },  /* Hong Kong */
  { 0x0455, "MAC" },  /* Macao, China */
  { 0x0456, "KHM" },  /* Cambodia */
  { 0x0460, "CHN" },  /* China */
  { 0x0466, "TWN" },  /* Taiwan, China */
  { 0x0467, "PRK" },  /* Democratic People's Republic of Korea */
  { 0x0470, "BGD" },  /* Bangladesh */
  { 0x0502, "MY"  },  /* Malaysia */
  { 0x0505, "AUS" },  /* Australia */
  { 0x0510, "IND" },  /* Indonesia */
  { 0x0515, "PH"  },  /* Philippines */
  { 0x0520, "TH"  },  /* Thailand */
  { 0x0525, "SGP" },  /* Singapore */
  { 0x0528, "BRU" },  /* Brunei Darussalam */
  { 0x0530, "NZ"  },  /* New Zealand */
  { 0x0539, "TON" },  /* Tonga */
  { 0x0541, "VUT" },  /* Vanuatu */
  { 0x0542, "FIJ" },  /* Fiji */
  { 0x0544, "AS"  },  /* American Samoa */
  { 0x0546, "NCL" },  /* New Caledonia (Territoire français d'outre-mer) */
  { 0x0547, "F"   },  /* French Polynesia (Territoire français d'outre-mer) */
  { 0x0550, "FSM" },  /* Micronesia */
  { 0x0604, "MOR" },  /* Morocco */
  { 0x0607, "GM"  },  /* Gambia */
  { 0x0609, "MR"  },  /* Mauritania */
  { 0x0610, "ML"  },  /* Mali */
  { 0x0611, "GN"  },  /* Guinea */
  { 0x0612, "CI"  },  /* Côte d'Ivoire */
  { 0x0613, "BF"  },  /* Burkina Faso */
  { 0x0614, "NE"  },  /* Niger */
  { 0x0615, "TG"  },  /* Togolese Republic */
  { 0x0616, "BJ"  },  /* Benin */
  { 0x0617, "MRU" },  /* Mauritius */
  { 0x0620, "GH"  },  /* Ghana */
  { 0x0621, "NG"  },  /* Nigeria */
  { 0x0622, "TD"  },  /* Chad */
  { 0x0623, "CF"  },  /* Central African Republic */
  { 0x0624, "CAM" },  /* Cameroon */
  { 0x0625, "CPV" },  /* Cape Verde */
  { 0x0626, "STP" },  /* Sao Tome and Principe */
  { 0x0627, "GNQ" },  /* Equatorial Guinea */
  { 0x0628, "GAB" },  /* Gabonese Republic */
  { 0x0629, "CG"  },  /* Congo */
  { 0x0630, "CD"  },  /* Democratic Republic of the Congo */
  { 0x0631, "AO"  },  /* Angola */
  { 0x0632, "GW"  },  /* Guinea-Bissau */
  { 0x0633, "SEZ" },  /* Seychelles */
  { 0x0634, "SDN" },  /* Sudan */
  { 0x0635, "R"   },  /* Rwandese Republic */
  { 0x0636, "ETH" },  /* Ethiopia */
  { 0x0637, "SOM" },  /* Somali Democratic Republic */
  { 0x0639, "KE"  },  /* Kenya */
  { 0x0640, "TZ"  },  /* Tanzania */
  { 0x0641, "UG"  },  /* Uganda */
  { 0x0642, "BI"  },  /* Burundi */
  { 0x0643, "MOZ" },  /* Mozambique */
  { 0x0645, "ZM"  },  /* Zambia */
  { 0x0646, "MG"  },  /* Madagascar */
  { 0x0647, "F"   },  /* France */
  { 0x0648, "ZW"  },  /* Zimbabwe */
  { 0x0649, "NA"  },  /* Namibia */
  { 0x0650, "MW"  },  /* Malawi */
  { 0x0651, "LSO" },  /* Lesotho */
  { 0x0655, "SA"  },  /* South Africa */
  { 0x0702, "BL"  },  /* Belize */
  { 0x0706, "ESV" },  /* El Salvador */
  { 0x0710, "NI"  },  /* Nicaragua */
  { 0x0712, "CRI" },  /* Costa Rica */
  { 0x0716, "PE"  },  /* Peru */
  { 0x0722, "AR"  },  /* Argentine Republic */
  { 0x0724, "BRA" },  /* Brazil */
  { 0x0730, "CL"  },  /* Chile */
  { 0x0734, "VZ"  },  /* Venezuela */
  { 0x0736, "BO"  },  /* Bolivia */
  { 0x0744, "PGY" },  /* Paraguay */
  { 0x0746, "SR"  },  /* Suriname */

  /* NULL record indicates the end of the list */
  { 0x0000, ""    }
};

static void GetMcc_mnc(char *pOperateNum,char *mcc_mnc)
{
	int i,len;
	char mcc[4];
	for(i=0;i<sizeof(MCC_Info)/sizeof(MCC_Info[0]);i++)
	{
		sprintf(mcc,"%x",MCC_Info[i].mcc);
		if(mcc[0]==pOperateNum[0] && mcc[1]==pOperateNum[1] && mcc[2]==pOperateNum[2])
		{
			len=strlen(MCC_Info[i].country);
			memcpy(mcc_mnc,MCC_Info[i].country,len);
			mcc_mnc[len]  ='-';
			mcc_mnc[len+1]=pOperateNum[3];
			mcc_mnc[len+2]=pOperateNum[4];
			mcc_mnc[len+3]='\0';
			return;
		}

	}
	//Other countries,which we can't find in the table
	mcc_mnc[0]=pOperateNum[0];
	mcc_mnc[1]=pOperateNum[1];
	mcc_mnc[2]=pOperateNum[2];
	mcc_mnc[3]='-';
	mcc_mnc[4]=pOperateNum[3];
	mcc_mnc[5]=pOperateNum[4];
	mcc_mnc[6]='\0';
}
void NetworksWin()
{
	HWND hNetworkFrame,hWnd;
	RECT rClient;
	WNDCLASS wc;
	
	wc.style         = 0; 
	wc.lpfnWndProc   = NetworksProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = NULL;
	wc.hIcon         = NULL;
	wc.hCursor       = NULL;
	wc.hbrBackground = NULL; //GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = "Networks";
	if(!RegisterClass(&wc))
		return ;
	
	hNetworkFrame=GetNetworkFrameWnd();
	
	GetClientRect(hNetworkFrame,&rClient);
	hWnd=CreateWindow("Networks","",WS_VISIBLE | WS_CHILD /*|WS_VSCROLL*/ ,
		rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
        hNetworkFrame,
		NULL,
		NULL,
		NULL);
	SetFocus(hWnd);
	
	ShowWindow(hNetworkFrame,SW_SHOW);
	UpdateWindow(hNetworkFrame);
}

static LRESULT NetworksProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
	LRESULT lResult;
	HWND hNetworkFrame;
	static HWND hNetworkSelection,hPreferredNetworks;
	static HWND hFocus;
	int i;
	
	
	hNetworkFrame=GetNetworkFrameWnd();
    lResult = (LRESULT)TRUE;
	
    switch ( wMsgCmd )
    {
    case WM_CREATE :
		{
 			CreateNetworksList(hWnd,&hNetworkSelection,&hPreferredNetworks);
			
			SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,0,(LPARAM)ML("Back"));
			SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,1,(LPARAM)"");
			SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,2,(LPARAM)ML("Select"));
			SetWindowText(hNetworkFrame,ML("Networks"));

			hFocus=hNetworkSelection;
			if(ME_GetCurOprator(hWnd,ME_MSG_GET_CURRENT_OPERATOR_MODE,0)<0)
				return FALSE;
		}
        break;
	case ME_MSG_GET_CURRENT_OPERATOR_MODE:
		switch(wParam)
		{
		case ME_RS_SUCCESS:
			{
				CUROPRATOR CurrentOperator;
				ME_GetResult(&CurrentOperator,sizeof(CurrentOperator));
				if(CurrentOperator.Mode==MODE_AUTO)
					//SendMessage(hNetworkSelection,SSBM_SETCURSEL,0,0);
					SendMessage(hNetworkSelection,SSBM_SETTEXT,0,(LPARAM)ML("Automatic"));
				else if(CurrentOperator.Mode==MODE_MANUAL)
					//SendMessage(hNetworkSelection,SSBM_SETCURSEL,1,0);
					SendMessage(hNetworkSelection,SSBM_SETTEXT,0,(LPARAM)ML("Manual"));
				printf("Get current network search mode success\r\n");
			}
			break;
		default:
			break;
		}
		break;
		
	case WM_NETMODE_CHANGED:
		if(wParam==0)
			//SendMessage(hNetworkSelection,SSBM_SETCURSEL,wParam,0);
			SendMessage(hNetworkSelection,SSBM_SETTEXT,0,(LPARAM)ML("Automatic"));
		else if(wParam==1)
			SendMessage(hNetworkSelection,SSBM_SETTEXT,0,(LPARAM)ML("Manual"));
		break;
		
	case WM_PAINT:
		{
			BeginPaint(hWnd,NULL);
			EndPaint(hWnd,NULL);
		}
		break;
		
    case WM_SETFOCUS:
        SetFocus(hFocus);
        break;
		
	case PWM_SHOWWINDOW:
		SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,0,(LPARAM)"Back");
		SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,1,(LPARAM)ML(""));
		SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,2,(LPARAM)ML("Select"));
		SetWindowText(hNetworkFrame,ML("Networks"));
		SetFocus(hFocus);
		break;
		
    case WM_CLOSE:
		DestroyWindow(hWnd);   
        break;
		
    case WM_DESTROY :
        UnregisterClass("Networks",NULL);
        break;

	
	case WM_KEYDOWN:
		switch(LOWORD(wParam))
		{
		case VK_F10:
			PostMessage(hWnd,WM_CLOSE,0,0);
			SendMessage(hNetworkFrame,PWM_CLOSEWINDOW,  (WPARAM) hWnd, (LPARAM)0);
			
			break;	
		case VK_F5:
			{
				if(hFocus==hNetworkSelection)
				{
					//i=SendMessage(hNetworkSelection,SSBM_GETCURSEL,0,0);
					char buffer[20];
					SendMessage(hNetworkSelection,SSBM_GETTEXT,0,(LPARAM)buffer);
					if(strcmp(buffer,ML("Automatic"))==0)
						i=0;
					else
						i=1;
					NetworkSelection(&i);
				}
				else if(hFocus==hPreferredNetworks)
					PreferredNetworks();
				
			}
			break;
		case VK_DOWN:
			{
				hFocus= GetFocus();
				while(GetParent(hFocus) != hWnd)
				{
					hFocus = GetParent(hFocus);
				}
				hFocus = GetNextDlgTabItem(hWnd, hFocus, FALSE);
				SetFocus(hFocus);
				
			}
			break;
			
		case VK_UP:
			{
				hFocus = GetFocus();
				while(GetParent(hFocus) != hWnd)
				{
					hFocus = GetParent(hFocus);
				}
				hFocus = GetNextDlgTabItem(hWnd, hFocus, TRUE);
				SetFocus(hFocus);
				
			}
			break;
		}
		break;
		
		default:
			lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
	}
	
    return lResult;
}
static BOOL CreateNetworksList(HWND hParentWnd,HWND *hNetworkSelection,HWND *hPreferredNetworks)
{
	int     xzero=0,yzero=0,iControlH,iControlW;
	RECT    rect;
    GetClientRect(hParentWnd, &rect);
	iControlH = rect.bottom/3;
    iControlW = rect.right;

    * hNetworkSelection = CreateWindow( "SPINBOXEX", ML("Network selection"), 
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST |/* WS_BORDER |*/ WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
        xzero, yzero, iControlW, iControlH, 
        hParentWnd, (HMENU)IDC_NETWORKSELECTION, NULL, NULL);

    if (* hNetworkSelection == NULL)
        return FALSE;
	
	SendMessage(* hNetworkSelection,SSBM_ADDSTRING,0,(LPARAM)ML("Automatic"));
// 	SendMessage(* hNetworkSelection,SSBM_ADDSTRING,0,(LPARAM)ML("Manual"));
	SendMessage(* hNetworkSelection,SSBM_SETCURSEL,0,0);

    * hPreferredNetworks = CreateWindow( "SPINBOXEX", ML("Preferred networks"), 
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | /*WS_BORDER | */WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
        xzero, yzero+iControlH, iControlW, iControlH, 
        hParentWnd, (HMENU)IDC_PREFERREDNETWORKS, NULL, NULL);

    if (* hPreferredNetworks == NULL)
        return FALSE;
	
	return TRUE;
    
}



static void NetworkSelection(int *iActiveMode)
{
	HWND hNetworkFrame,hWnd;
	RECT rClient;
	WNDCLASS wc;
	
	wc.style         = 0; 
	wc.lpfnWndProc   = NetworkSelectionProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = NULL;
	wc.hIcon         = NULL;
	wc.hCursor       = NULL;
	wc.hbrBackground = NULL; //GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = "NetworkSelection";
	if(!RegisterClass(&wc))
		return ;

	hNetworkFrame=GetNetworkFrameWnd();

	GetClientRect(hNetworkFrame,&rClient);
	hWnd=CreateWindow("NetworkSelection","",WS_VISIBLE | WS_CHILD ,
		rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
        hNetworkFrame,
		NULL,
		NULL,
		(PVOID)iActiveMode);
	SetFocus(hWnd);

	ShowWindow(hNetworkFrame,SW_SHOW);
	UpdateWindow(hNetworkFrame);
}

static LRESULT NetworkSelectionProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
	LRESULT lResult;
	HWND hNetworkFrame;
	HDC hdc;
	static HWND hList;
	static HBITMAP hRadioOn,hRadioOff;
	static int iSelected;
	int result;
	static BOOL bCancelSearch;
	int len;
	
	hNetworkFrame=GetNetworkFrameWnd();
    lResult = (LRESULT)TRUE;
	
    switch ( wMsgCmd )
    {
    case WM_CREATE :
		{
			RECT rClient;
			int i;
			int iActiveMode;
			PCREATESTRUCT pCreateStruct=(PCREATESTRUCT)lParam;

			memcpy(&iActiveMode,pCreateStruct->lpCreateParams,sizeof(int));

			GetClientRect(hWnd,&rClient);
			
			hList=CreateWindow("LISTBOX","",
				WS_VISIBLE|WS_CHILD|/*WS_BORDER|*/WS_VSCROLL|LBS_NOTIFY|LBS_BITMAP,
				rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
				hWnd,(HMENU)IDC_NETWORKSELECTIONLIST,NULL,NULL);
			SendMessage(hList,LB_ADDSTRING,0,(LPARAM)ML("Automatic"));
			SendMessage(hList,LB_ADDSTRING,0,(LPARAM)ML("Manual"));
						
			hdc=GetDC(hWnd);
			hRadioOn=CreateBitmapFromImageFile(hdc,"/rom/network/radio_button_on.bmp",NULL,NULL);
			hRadioOff=CreateBitmapFromImageFile(hdc,"/rom/network/radio_button_off.bmp",NULL,NULL);
			ReleaseDC(hWnd,hdc);

			for(i=0;i<2;i++)
			{
				SendMessage(hList,LB_SETIMAGE,(WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)i),(LPARAM)hRadioOff);
			}
			SendMessage(hList,LB_SETIMAGE,(WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)iActiveMode),(LPARAM)hRadioOn);
			SendMessage(hList,LB_SETCURSEL,iActiveMode,0);
			
			SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,0,(LPARAM)ML("Cancel"));
			SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,1,(LPARAM)"");
			SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,2,(LPARAM)ML("Select"));
			SetWindowText(hNetworkFrame,ML("Network selection"));

			iSelected=iActiveMode;  

			pNetOperator=malloc(MAXOPERATORNUM*sizeof(NETOPRATOR));
			if(!pNetOperator)
			{
				PostMessage(hWnd,WM_CLOSE,0,0);
				SendMessage(hNetworkFrame,PWM_CLOSEWINDOW,  (WPARAM) hWnd, (LPARAM)0);
				return FALSE;
			}
		}
        break;
		
	case WM_PAINT:
		{
			BeginPaint(hWnd,NULL);
			EndPaint(hWnd,NULL);
		}
		break;
		
    case WM_SETFOCUS:
        SetFocus(hList);
        break;
		
	case PWM_SHOWWINDOW:
		SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,0,(LPARAM)ML("Cancel"));
		SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,1,(LPARAM)"");
		SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,2,(LPARAM)ML("Select"));
		SetWindowText(hNetworkFrame,ML("Network selection"));
		SetFocus(hList);
		break;
		
    case WM_CLOSE:
		DestroyWindow(hWnd);   
        break;
		
    case WM_DESTROY :
        UnregisterClass("NetworkSelection",NULL);
		free(pNetOperator);
		OperateNum=0;
		DeleteObject(hRadioOn);
		DeleteObject(hRadioOff);
        break;
		
		
	case WM_KEYDOWN:
		switch(LOWORD(wParam))
		{
		case VK_F10:
			PostMessage(hWnd,WM_CLOSE,0,0);
			SendMessage(hNetworkFrame,PWM_CLOSEWINDOW,  (WPARAM) hWnd, (LPARAM)0);
			
			break;	
		case VK_F5:
			{
				int i;
				i=SendMessage(hList,LB_GETCURSEL,0,0); 
//				if(i!=iSelected)
//				{
//					SendMessage(hList,LB_SETIMAGE,(WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)i),(LPARAM)hRadioOn);
//					SendMessage(hList,LB_SETIMAGE,(WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)iSelected),(LPARAM)hRadioOff);
//				}
				
				iSelected=i;
				
				if(iSelected==0)
				{
					me_handle = ME_SetOpratorMode(hWnd,ME_MSG_SETOPERATORMODE,MODE_AUTO,NULL);
					if(me_handle<0)
					{
						printf("Set Operator Mode to Auto failure,ME return error\r\n");
						SendMessage(hWnd,WM_KEYDOWN,MAKEWPARAM(VK_F10,0),(LPARAM)0);
					}
				}
				else
				{
// 					WaitWindowStateEx(hWnd,TRUE,ML("Searching for\n networks..."),ML("Networks"),NULL,ML("Cancel"));
					bCancelSearch=FALSE;
					WaitWin(hWnd,TRUE,ML("Searching for\n networks..."),ML("Networks"),NULL,ML("Cancel"),WM_CANCELSEARCH);

					me_handle = ME_SearchNetOprator(hWnd,ME_MSG_SEARCHNETOPERATOR);
					if(me_handle<0)
					{
						printf("Search Net Operator failure:ME return error\r\n");
						WaitWindowStateEx(hWnd,FALSE,ML("Searching for\n networks..."),ML("Networks"),
							NULL,ML("Cancel"));
						PostMessage(hWnd,WM_CLOSE,0,0);
						SendMessage(hNetworkFrame,PWM_CLOSEWINDOW,  (WPARAM) hWnd, (LPARAM)0);
						PLXTipsWin(NULL,NULL,0,ML("Cannot get\nnetworks now"),ML("Network selection"),
							Notify_Failure,ML("OK"),NULL,20);
// 						SendMessage(hWnd,WM_KEYDOWN,MAKEWPARAM(VK_F10,0),(LPARAM)0);
					
					}
				}
				
			}
			break;
		
		}
		break;

		case WM_CANCELSEARCH:
			bCancelSearch=TRUE;
			break;
		
		case ME_MSG_SEARCHNETOPERATOR:
			{
				if(bCancelSearch)
					return FALSE;
				
				WaitWindowStateEx(hWnd,FALSE,ML("Searching for\n networks..."),ML("Networks"),NULL,ML("Cancel"));
				
				if(wParam==ME_RS_SUCCESS)
				{
					
					len=ME_GetResult(pNetOperator,MAXOPERATORNUM*sizeof(NETOPRATOR));
					OperateNum=len/sizeof(NETOPRATOR);
					AvailableNetworksListWin();
				}			
				else
				{
					if(ME_GetSignalInfo(hWnd,ME_MSG_CHECKSIGNAL)<0)
						PLXTipsWin(NULL,NULL,0,ML("Cannot get\nnetworks now"),ML("Network selection"),Notify_Failure,ML("OK"),NULL,20);
					
				}

			}
			break;
		case ME_MSG_SETOPERATORMODE:
			{
				HWND hFather;
				if(wParam==ME_RS_SUCCESS)
					printf("Set Operator Mode to Automatic success\r\n");
				else
					printf("Set Operator Mode to Automatic failure\r\n");
				hFather=GetWindow((HWND)hWnd,GW_HWNDNEXT);
				SendMessage(hFather,WM_NETMODE_CHANGED,0,0);
				SendMessage(hWnd,WM_KEYDOWN,MAKEWPARAM(VK_F10,0),(LPARAM)0);

			}
			break;
		case ME_MSG_CHECKSIGNAL:
			{
				
				if(wParam==ME_RS_SUCCESS)
				{
					ME_GetResult(&result,sizeof(int));

#define ME_NOSIGNAL  99

					if(result==ME_NOSIGNAL) 
						PLXTipsWin(NULL,NULL,0,ML("No service"),ML("Network selection"),Notify_Failure,ML("OK"),NULL,20);
					else //have signal,but can't get network operator
						PLXTipsWin(NULL,NULL,0,ML("Cannot get\nnetworks now"),ML("Network selection"),Notify_Failure,ML("OK"),NULL,20);

				}
				else
					PLXTipsWin(NULL,NULL,0,ML("Cannot get\nnetworks now"),ML("Network selection"),Notify_Failure,ML("OK"),NULL,20);
			}
			
			break;
		
		default:
			lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
	}
	
    return lResult;
}
static void AvailableNetworksListWin()
{
	HWND hNetworkFrame,hWnd;
	RECT rClient;
	WNDCLASS wc;
	
	wc.style         = 0; 
	wc.lpfnWndProc   = AvailableNetworksListProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = NULL;
	wc.hIcon         = NULL;
	wc.hCursor       = NULL;
	wc.hbrBackground = NULL; //GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = "AvailableNetworksList";
	if(!RegisterClass(&wc))
		return ;

	hNetworkFrame=GetNetworkFrameWnd();

	GetClientRect(hNetworkFrame,&rClient);
	hWnd=CreateWindow("AvailableNetworksList","",WS_VISIBLE | WS_CHILD ,
		rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
        hNetworkFrame,
		NULL,
		NULL,
		NULL);
	SetFocus(hWnd);

	ShowWindow(hNetworkFrame,SW_SHOW);
	UpdateWindow(hNetworkFrame);
}
static LRESULT AvailableNetworksListProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
	LRESULT lResult;
	HWND hNetworkFrame;
	HDC hdc;
	static HWND hList;
	static HBITMAP hHome,hPreferred,hRoaming,hEmergent;
	PREFER_OPRATOR PreferredOperator[MAXPREFERREDNETWORK];
	
	hNetworkFrame=GetNetworkFrameWnd();
    lResult = (LRESULT)TRUE;
	
    switch ( wMsgCmd )
    {
    case WM_CREATE :
		{
			RECT rClient;
			int i;
			BOOL bEmgNetworkExist;

			GetClientRect(hWnd,&rClient);
			
			hList=CreateWindow("LISTBOX","",
				WS_VISIBLE|WS_CHILD|/*WS_BORDER|*/WS_VSCROLL|LBS_NOTIFY|LBS_BITMAP,
				rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
				hWnd,(HMENU)IDC_NETWORKSELECTIONLIST,NULL,NULL);
			
		
			hdc=GetDC(hWnd);
/*
			hHome=CreateBitmapFromImageFile(hdc,"/rom/ime/radioon.bmp",NULL,NULL);  //Bug fixed. Benefon not provide the pic
			hPreferred=CreateBitmapFromImageFile(hdc,"/rom/ime/radiooff.bmp",NULL,NULL);
			hRoaming=CreateBitmapFromImageFile(hdc,"/rom/ime/radiooff.bmp",NULL,NULL);
			hEmergent=CreateBitmapFromImageFile(hdc,"/rom/ime/radiooff.bmp",NULL,NULL);
*/
			//if home network is available Home network .
			hHome=CreateBitmapFromImageFile(hdc,"/rom/network/ns_i_homenetwork_22x16.bmp",NULL,NULL);  //Bug fixed. Benefon not provide the pic
			//if there are preferred networks available.
			hPreferred=CreateBitmapFromImageFile(hdc,"/rom/network/ns_i_preferrednetwork_22x16.bmp",NULL,NULL);
			//if there are networks available where roaming.
			hRoaming=CreateBitmapFromImageFile(hdc,"/rom/network/ns_i_roamingnetwork_22x16.bmp",NULL,NULL);
			//if there are network available where roaming is not possible.
			hEmergent=CreateBitmapFromImageFile(hdc,"/rom/network/ns_i_emernetwork_22x16.bmp",NULL,NULL);
			ReleaseDC(hWnd,hdc);
			
			bEmgNetworkExist=FALSE;
			for(i=0;i<OperateNum;i++)
			{
				if(pNetOperator[i].stat==STAT_UNKNOWN)
				{
					char mcc_mnc[10];
					GetMcc_mnc(pNetOperator[i].num_oper,mcc_mnc);
					SendMessage(hList,LB_ADDSTRING,i,(LPARAM)mcc_mnc);
				}
				else
					SendMessage(hList,LB_ADDSTRING,i,(LPARAM)pNetOperator[i].alpha_oper);
				
// /*hHome*/	SendMessage(hList,LB_SETIMAGE,(WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)i),(LPARAM)hHome);
				//The first fordidden network is emergent network,according to benefon's explanation
				if(pNetOperator[i].stat==STAT_FORBIDDEN && bEmgNetworkExist==FALSE)
				{
/*hEmergent*/		SendMessage(hList,LB_SETIMAGE,(WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)i),(LPARAM)hEmergent);
					bEmgNetworkExist=TRUE;
				}

			}

			SendMessage(hList,LB_SETCURSEL,0,0);
			
			//First get the prefer network from SIM card
			if(ME_ReadPrefOperList(hWnd,ME_MSG_READPREFERREDOPERATOR,1,MAXPREFERREDNETWORK)<0)
				printf("read preferred operator from SIM card failure\r\n");
			
			SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,0,(LPARAM)ML("Cancel"));
			SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,1,(LPARAM)"");
			SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,2,(LPARAM)ML("Select"));
			SetWindowText(hNetworkFrame,ML("Select network"));

		}
        break;

	case ME_MSG_READPREFERREDOPERATOR:
		switch(wParam)
		{
		case ME_RS_SUCCESS:
			{
				int len,num,i,j;
				len=ME_GetResult(PreferredOperator,MAXPREFERREDNETWORK*sizeof(PREFER_OPRATOR));
				if(len==-1)
					return FALSE;
				
				num=len/sizeof(PREFER_OPRATOR);
				for(i=0;i<num;i++)
				{
					for(j=0;j<OperateNum;j++)
					{
						if(strcmp(pNetOperator[j].num_oper,PreferredOperator[i].oper_id)==0)
/*hPreferred*/				SendMessage(hList,LB_SETIMAGE,(WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)j),(LPARAM)hPreferred);
					}
				}
				
			}
			break;
		default:
			break;
			
		}
		if(ME_GetSubscriberId (hWnd, ME_MSG_GETIMSI)<0)  //Second,Get home network saved in SIM card
			printf("Can not get IMSI\r\n");
		break;
		
	case ME_MSG_GETIMSI:
		{
			switch(wParam) 
			{
			case ME_RS_SUCCESS:
				{
					char imsi[50];
					int i;
					if(ME_GetResult(imsi,sizeof(imsi))<0)
						return FALSE;
					imsi[5]='\0';
					for(i=0;i<OperateNum;i++)
					{
						//Set home network icon
						if(strcmp(pNetOperator[i].num_oper,imsi)==0)
							SendMessage(hList,LB_SETIMAGE,(WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)i),(LPARAM)hHome);

						//A network whose state="current network",and operator number!= "home network" is a roaming network
						//fixed :  operator number!= "home network" is a roaming network
						if(/*pNetOperator[i].stat==STAT_CURRENT &&*/ strcmp(pNetOperator[i].num_oper,imsi)!=0)
							SendMessage(hList,LB_SETIMAGE,(WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)i),(LPARAM)hRoaming);					
					}
					
				}
				break;
			default:
				break;
			}
		}
		break;
		
    case WM_SETFOCUS:
        SetFocus(hList);
        break;
		
	case PWM_SHOWWINDOW:
		SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,0,(LPARAM)ML("Cancel"));
		SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,1,(LPARAM)"");
		SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,2,(LPARAM)ML("Select"));
		SetWindowText(hNetworkFrame,ML("Select network"));
		SetFocus(hList);
		break;
		
    case WM_CLOSE:
		DestroyWindow(hWnd);   
        break;
		
    case WM_DESTROY :
        UnregisterClass("AvailableNetworksList",NULL);
		DeleteObject(hHome);
		DeleteObject(hPreferred);
		DeleteObject(hRoaming);
		DeleteObject(hEmergent);
        break;
		
		
	case WM_KEYDOWN:
		switch(LOWORD(wParam))
		{
		case VK_F10:
			PostMessage(hWnd,WM_CLOSE,0,0);
//			SendMessage(hNetworkFrame,PWM_CLOSEWINDOW,  (WPARAM) hWnd, (LPARAM)0);	
			break;	
			
		case VK_F5:
			{
				int i;
				i=SendMessage(hList,LB_GETCURSEL,0,0); 
				{
					WaitWindowStateEx(hWnd,TRUE,ML("Registering to\nnetwork..."),ML("Select network"),NULL,NULL);

					if(pNetOperator[i].stat==STAT_FORBIDDEN)
					{
						WaitWindowStateEx(hWnd,FALSE,ML("Registering to\nnetwork..."),ML("Select network"),NULL,NULL);
						PLXTipsWin(NULL,NULL,0,ML("Network registration\nfailed"),ML("Select network"),Notify_Failure,ML("OK"),NULL,20);
					}
					else if(ME_SetOpratorMode(hWnd,ME_MSG_REGISTERNETWORK,MODE_MANUAL,pNetOperator[i].num_oper)<0)
					{
						WaitWindowStateEx(hWnd,FALSE,ML("Registering to\nnetwork..."),ML("Select network"),NULL,NULL);
						PLXTipsWin(NULL,NULL,0,ML("Network registration\nfailed"),ML("Select network"),Notify_Failure,ML("OK"),NULL,20);
						
					}
				}
			}
			break;
		
		}
		break;
		case ME_MSG_REGISTERNETWORK:
			WaitWindowStateEx(hWnd,FALSE,ML("Registering to\nnetwork..."),ML("Select network"),NULL,NULL);
			switch(wParam) {
			case ME_RS_SUCCESS:
				{
					HWND hFather,hGrandpa;
					DlmNotify(PMM_OPERATOR_CHANGE, TRUE);//
					PLXTipsWin(NULL,NULL,0,ML("Registered to\nnetwork"),ML("Select network"),Notify_Success,ML("OK"),NULL,20);
					//If registered success,return to Networks.So here close Current window(Available network list window),
					//And Brother windows:Network selection window
					hFather = GetWindow((HWND)hWnd, GW_HWNDNEXT);
					hGrandpa=GetWindow((HWND)hFather,GW_HWNDNEXT);
					PostMessage(hFather,WM_CLOSE,0,0);
					PostMessage(hWnd,WM_CLOSE,0,0);
					SendMessage(hGrandpa,WM_NETMODE_CHANGED,1,0);
					SendMessage(hNetworkFrame,PWM_CLOSEWINDOW,  (WPARAM) hFather, (LPARAM)0);
				}
				break;
			default:
				PLXTipsWin(NULL,NULL,0,ML("Network registration\nfailed"),ML("Select network"),Notify_Failure,ML("OK"),NULL,20);
				break;
			}
			break;

		default:
			lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
	}
	
    return lResult;
}

static void PreferredNetworks()
{
	HWND hNetworkFrame,hWnd;
	RECT rClient;
	WNDCLASS wc;
	
	wc.style         = 0; 
	wc.lpfnWndProc   = PreferredNetworksProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = NULL;
	wc.hIcon         = NULL;
	wc.hCursor       = NULL;
	wc.hbrBackground = NULL; //GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = "PreferredNetworks";
	if(!RegisterClass(&wc))
		return ;

	hNetworkFrame=GetNetworkFrameWnd();

	GetClientRect(hNetworkFrame,&rClient);
	hWnd=CreateWindow("PreferredNetworks","",WS_VISIBLE | WS_CHILD |WS_VSCROLL,
		rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
        hNetworkFrame,
		NULL,
		NULL,
		NULL);
	SetFocus(hWnd);

	ShowWindow(hNetworkFrame,SW_SHOW);
	UpdateWindow(hNetworkFrame);
}
static LRESULT PreferredNetworksProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
	LRESULT lResult;
	static HWND *hList;
	static HWND hFocus;
	static HMENU hMenu;
	HWND  hNetworkFrame;
	int i;
	int len;
	static BOOL bPreferredChanged;
	static BOOL bInitFinished;

    lResult = (LRESULT)TRUE;
	hNetworkFrame=GetNetworkFrameWnd();
    switch ( wMsgCmd )
    {
    case WM_CREATE :
		{
			SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,0,(LPARAM)ML("Back"));
			SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,1,(LPARAM)ML("Assign"));
			SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,2,(LPARAM)"Options");
			SetWindowText(hNetworkFrame,ML("Preferred networks"));

			hList=malloc(MAXPREFERREDNETWORK*sizeof(HWND));
			pReadPreferredOperator=malloc(MAXPREFERREDNETWORK*sizeof(PREFER_OPRATOR));
			if(hList==NULL||pReadPreferredOperator==NULL)
			{	
				PostMessage(hWnd,WM_CLOSE,0,0);
				SendMessage(hNetworkFrame,PWM_CLOSEWINDOW,(WPARAM)hWnd,(LPARAM)NULL);
				return FALSE;
			}
			for(i=0;i<MAXPREFERREDNETWORK;i++)
				hList[i]=NULL;
			memset(pReadPreferredOperator,0,MAXPREFERREDNETWORK*sizeof(PREFER_OPRATOR));
			iReadPreferredOperatorNum=0;

			

			if(ME_ReadPrefOperList(hWnd,ME_MSG_READPREFERREDOPERATOR,1,MAXPREFERREDNETWORK)<0)
				printf("read preferred operator from SIM card failure\r\n");
			
			hMenu=CreateMenu();
			AppendMenu(hMenu,MF_ENABLED, IDM_ASSIGN, (LPCSTR)IDS_ASSIGN);
			AppendMenu(hMenu,MF_ENABLED, IDM_REMOVE, (LPCSTR)IDS_REMOVE);
			PDASetMenu(hNetworkFrame,hMenu);
			
 			hFocus = hWnd;
			bPreferredChanged=FALSE;
			bInitFinished=FALSE;
		}
        break;

	case ME_MSG_READPREFERREDOPERATOR:
		switch(wParam)
		{
			
		case ME_RS_SUCCESS:
			{
				len=ME_GetResult(pReadPreferredOperator,MAXPREFERREDNETWORK*sizeof(PREFER_OPRATOR));
				if(len==-1)
				{
					iReadPreferredOperatorNum=0;
					if(CreatePreferredNetworksList(hWnd,hList,pReadPreferredOperator,iReadPreferredOperatorNum))
					{
						if(!bPreferredChanged)
						{
							hFocus=hList[0];
							SetFocus(hFocus);
							InitVScrollBar(hWnd,MAXPREFERREDNETWORK);
							bInitFinished=TRUE;
						}
					}
 					return FALSE;
 				}
				
				iReadPreferredOperatorNum=len/sizeof(PREFER_OPRATOR);
				if(CreatePreferredNetworksList(hWnd,hList,pReadPreferredOperator,iReadPreferredOperatorNum))
				{
					if(!bPreferredChanged)
					{
						hFocus=hList[0];
						SetFocus(hFocus);
						InitVScrollBar(hWnd,MAXPREFERREDNETWORK);
						bInitFinished=TRUE;
					}
				}
			}
			break;

		default:
			{
				//create empty slot in setting list
			}
			break;
		}
		break;

	case WM_PAINT:
		{
			BeginPaint(hWnd,NULL);
			EndPaint(hWnd,NULL);
		}
		break;
		
    case WM_SETFOCUS:
        SetFocus(hFocus);
        break;

	case WM_WRITEPREFLIST2:
		if (lParam == 1)
			ME_WritePrefOperList(hWnd,ME_MSG_REMOVEPREFERREDNET,nCurFocus+1,NULL);
		break;

	case PWM_SHOWWINDOW:
		SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,0,(LPARAM)ML("Back"));
		SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,1,(LPARAM)ML("Assign"));
		SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,2,(LPARAM)"Options");
		SetWindowText(hNetworkFrame,ML("Preferred networks"));
		PDASetMenu(hNetworkFrame,hMenu);

		SetFocus(hFocus);
		break;
		
	case WM_PREFERREDNETCHANGED:
		bPreferredChanged=TRUE;
		if(ME_ReadPrefOperList(hWnd,ME_MSG_READPREFERREDOPERATOR,1,MAXPREFERREDNETWORK)<0)
			printf("read preferred operator from SIM card failure\r\n");
		break;

    case WM_CLOSE:
		DestroyWindow(hWnd);   
        break;

    case WM_DESTROY :
        UnregisterClass("PreferredNetworks",NULL);
		free(pReadPreferredOperator);
		free((void*)hList);
        break;

	case WM_VSCROLL:
// 		if(iReadPreferredOperatorNum>3)
			DoVScroll(hWnd,wParam);
		break;

	case WM_KEYDOWN:
		switch(LOWORD(wParam))
		{
		case VK_F10:
			PostMessage(hWnd,WM_CLOSE,0,0);
			SendMessage(hNetworkFrame,PWM_CLOSEWINDOW,  (WPARAM) hWnd, 0);
			break;	

		case VK_F5:
			{
				if(!bInitFinished)
					return FALSE;

				SendMessage(hNetworkFrame,WM_KEYDOWN,wParam,lParam);
			}
			break;
        case VK_RETURN:
			{
				if(!bInitFinished)
					return FALSE;
			
			SendMessage(hWnd,WM_COMMAND,MAKEWPARAM(IDM_ASSIGN,0),0);
			}
            break;

		case VK_DOWN:
			{
//				hFocus= GetFocus();
//				if(hFocus==hWnd) //No SpinBox now
//					break;
				if(!bInitFinished)
					return FALSE;
				
				while(GetParent(hFocus) != hWnd)
				{
					hFocus = GetParent(hFocus);
				}
				hFocus = GetNextDlgTabItem(hWnd, hFocus, FALSE);
				SetFocus(hFocus);

				SendMessage(hWnd, WM_VSCROLL, SB_LINEDOWN, (LPARAM)NULL);
			}
			break;
			
		case VK_UP:
			{
//				hFocus = GetFocus();
//				if(hFocus==hWnd) //No SpinBOx now
//					break;
				if(!bInitFinished)
					return FALSE;
				
				while(GetParent(hFocus) != hWnd)
				{
					hFocus = GetParent(hFocus);
				}
				hFocus = GetNextDlgTabItem(hWnd, hFocus, TRUE);
				SetFocus(hFocus);
				SendMessage(hWnd, WM_VSCROLL, SB_LINEUP, (LPARAM)NULL);
			}
			break;

		default:
			lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
		}
		break;        

    case WM_COMMAND:

		if(!bInitFinished)
			return FALSE;

        switch( LOWORD( wParam ))
        {
		case IDM_ASSIGN:
			{
				PreferredNetworksSelectionList(&nCurFocus);
			}
			break;

		case IDM_REMOVE:
			{
				char buffer[20];
				memset(buffer,0,sizeof(buffer));
				SendMessage(hList[nCurFocus],SSBM_GETTEXT,0,(LPARAM)buffer);
				if(!strlen(buffer))
					return FALSE;
				
				PLXConfirmWinEx(hNetworkFrame,hWnd,ML("Remove\nassignment?"),Notify_Request,ML("Preferred networks"),ML("Yes"),ML("No"),WM_WRITEPREFLIST2);
/*
				{
					ME_WritePrefOperList(hWnd,ME_MSG_REMOVEPREFERREDNET,nCurFocus+1,NULL);
				}
				else
					NULL;
*/
			}
			break;

        }
        break;
		
	case ME_MSG_REMOVEPREFERREDNET:
		switch(wParam) 
		{
		case ME_RS_SUCCESS:
			PLXTipsWin(NULL,NULL,0,ML("Removed"),ML("Preferred networks"),Notify_Success,ML("OK"),NULL,20);
			PostMessage(hWnd,WM_PREFERREDNETCHANGED,0,0);
			break;

		default:
			break;
		}
		break;

    default :
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
    return lResult;

}
static void GetOperatorNameFromID(char *Operator_id,char *Operator_name)
{
	int i;
	for(i=0;i< ( sizeof(Net_oper)/sizeof(Net_oper[0]) - 1); i++)
		if(strstr(Net_oper[i],Operator_id))
		{
			strcpy(Operator_name,Net_oper[i]+7);
			return;
		}

}

static BOOL CreatePreferredNetworksList(HWND hParentWnd,HWND *hList, PREFER_OPRATOR* pReadPreferredOperator,int iReadPreferredOperatorNum)
{
	int     xzero=0,yzero=0,iControlH,iControlW;
	RECT    rect;
	char   tmpStr[100];
	char   name[100],mcc_mnc[10];
	int i,j;
	HWND hNetworkFrame=GetNetworkFrameWnd();
    GetClientRect(hNetworkFrame, &rect);
	iControlH = rect.bottom/3;
    iControlW = rect.right;
	
	for(i=0;i<MAXPREFERREDNETWORK;i++)
	{
		sprintf(tmpStr,"%d:",i+1);
		strcat(tmpStr,ML("empty"));
		memset(mcc_mnc,0,sizeof(mcc_mnc));
		for(j=0;j<iReadPreferredOperatorNum;j++)
		{
			if(pReadPreferredOperator[j].index==i+1)
			{
				GetOperatorNameFromID(pReadPreferredOperator[j].oper_id,name);
				GetMcc_mnc(pReadPreferredOperator[j].oper_id,mcc_mnc);
				sprintf(tmpStr,"%d:%s",i+1,name);
				break;
			}
		}
		if(!hList[i]) //Create in WM_CREATE
		{
			hList[i]=CreateWindow( "SPINBOXEX", tmpStr, 
				WS_VISIBLE | WS_CHILD |  CBS_DROPDOWNLIST | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
				xzero, yzero+i*iControlH, iControlW, iControlH, hParentWnd, (HMENU)IDC_PREFERREDLIST, NULL, NULL);
			if(!hList[i])
				return FALSE;
			SendMessage(hList[i],SSBM_ADDSTRING,0,(LPARAM)mcc_mnc);
			SendMessage(hList[i],SSBM_SETCURSEL,0,0);
		}
		else   //Create in frush data, no be called in WM_CREATE
		{
			SendMessage(hList[i],SSBM_SETTITLE,0,(LPARAM)tmpStr);
			SendMessage(hList[i],SSBM_SETTEXT,0,(LPARAM)mcc_mnc);
		}

	}//for(i=0;...

	return TRUE;
}

static void InitVScrollBar(HWND hWnd,int iItemNum)
{
    SCROLLINFO   vsi;

    memset(&vsi, 0, sizeof(SCROLLINFO));

    vsi.cbSize = sizeof(vsi);
    vsi.fMask  = SIF_ALL ;
    vsi.nMin   = 0;
    vsi.nPage  = 3;
    vsi.nMax   = (iItemNum-1);
    vsi.nPos   = 0;

    SetScrollInfo(hWnd, SB_VERT, &vsi, TRUE);
	nCurFocus =0;
	return;
}

static void DoVScroll(HWND hWnd,WPARAM wParam)
{
	int  nY;
    RECT rcClient;
    SCROLLINFO      vsi;

    switch(wParam)
    {
    case SB_LINEDOWN:
		memset(&vsi, 0, sizeof(SCROLLINFO));
		
        vsi.fMask  = SIF_ALL ;
        GetScrollInfo(hWnd, SB_VERT, &vsi);
		
        GetClientRect(hWnd,&rcClient);
        nY = (rcClient.bottom - rcClient.top)/3;
		
        nCurFocus++;
		
        if(nCurFocus > vsi.nMax)	
        {
            ScrollWindow(hWnd,0,(vsi.nMax-vsi.nPage+1)*nY,NULL,NULL);	
            UpdateWindow(hWnd);
            nCurFocus = 0;
			vsi.nPos = 0;
            vsi.fMask  = SIF_POS ;
            SetScrollInfo(hWnd, SB_VERT, &vsi, TRUE);
            break;
        }
	
        if(((int)(vsi.nPos + vsi.nPage - 1) <= nCurFocus) && nCurFocus != vsi.nMax)	//modified for UISG
        { 
            ScrollWindow(hWnd,0,-nY,NULL,NULL);
            UpdateWindow(hWnd);
            vsi.nPos++;
            vsi.fMask  = SIF_POS ;
            SetScrollInfo(hWnd, SB_VERT, &vsi, TRUE);
            break;
        }
		break;
		
    case SB_LINEUP:
		
        memset(&vsi, 0, sizeof(SCROLLINFO));
        vsi.fMask  = SIF_ALL ;
        GetScrollInfo(hWnd, SB_VERT, &vsi);
		
        GetClientRect(hWnd,&rcClient);
		
        nY = (rcClient.bottom - rcClient.top)/3;
		
        nCurFocus--;
		
        if(nCurFocus < vsi.nMin)	
        {
            ScrollWindow(hWnd,0,(vsi.nPage-vsi.nMax-1)*nY,NULL,NULL);
			
            UpdateWindow(hWnd);
			nCurFocus = vsi.nMax;
		    vsi.nPos = vsi.nMax-vsi.nPage+1;
            vsi.fMask  = SIF_POS ;
            SetScrollInfo(hWnd, SB_VERT, &vsi, TRUE);
            break;	
        }
		
        if((int)vsi.nPos == nCurFocus && nCurFocus != vsi.nMin)	//modified for UISG
        { 
            ScrollWindow(hWnd,0,nY,NULL,NULL);
            UpdateWindow(hWnd);
            vsi.nPos--;
            vsi.fMask  = SIF_POS ;
            SetScrollInfo(hWnd, SB_VERT, &vsi, TRUE);   
            break;	
        }
		break;

    case SB_PAGEDOWN:
        break;

    case SB_PAGEUP:
        break;
		
    default:
        break;	
    }	

}


static void PreferredNetworksSelectionList(void *info)
{
	HWND hNetworkFrame,hWnd;
	RECT rClient;
	WNDCLASS wc;
	
	wc.style         = 0; 
	wc.lpfnWndProc   = PreferredNetworksSelectionListProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = NULL;
	wc.hIcon         = NULL;
	wc.hCursor       = NULL;
	wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = "PreferredNetworksSelectionList";
	if(!RegisterClass(&wc))
		return ;

	hNetworkFrame=GetNetworkFrameWnd();
	GetClientRect(hNetworkFrame,&rClient);
	hWnd=CreateWindow("PreferredNetworksSelectionList","",WS_VISIBLE | WS_CHILD /*|WS_VSCROLL*/,
		rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
        hNetworkFrame,
		NULL,
		NULL,
		(PVOID)info);
	SetFocus(hWnd);

	ShowWindow(hNetworkFrame,SW_SHOW);
	UpdateWindow(hNetworkFrame);
	
}

static LRESULT PreferredNetworksSelectionListProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
	LRESULT lResult;
	static HWND hList;
	HWND  hNetworkFrame;
	static HBITMAP hRadioOff,hRadioOn;
	int i,j;
	static char strTmp[20];
	static int iFocusItemLastWin;
	static BOOL bIsItemEmpty;
	

    lResult = (LRESULT)TRUE;
	hNetworkFrame=GetNetworkFrameWnd();

    switch ( wMsgCmd )
    {
    case WM_CREATE :
		{
			RECT rClient;
			int iOperatorNum;
			HDC hdc;
			char OperatorName[50],OperatorNum[20];
			char mcc_mnc[10];
			int InitFocusItem;
			PCREATESTRUCT pCreateStruct=(PCREATESTRUCT)lParam;
			
			memcpy(&iFocusItemLastWin,pCreateStruct->lpCreateParams,sizeof(iFocusItemLastWin));
			

			iOperatorNum=sizeof(Net_oper)/sizeof(Net_oper[0]) - 1;

			hdc=GetDC(hWnd);
			hRadioOn=CreateBitmapFromImageFile(hdc,"/rom/network/radio_button_on.bmp",NULL,NULL);
			hRadioOff=CreateBitmapFromImageFile(hdc,"/rom/network/radio_button_off.bmp",NULL,NULL);
			ReleaseDC(hWnd,hdc);
			
			GetClientRect(hNetworkFrame,&rClient);

			hList=CreateWindow("LISTBOX","",
				WS_VISIBLE|WS_CHILD|/*WS_BORDER|*/WS_VSCROLL|LBS_NOTIFY|LBS_BITMAP|LBS_MULTILINE,
				rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
				hWnd,(HMENU)IDC_PREFERREDNETWORKSSELECTIONLIST,
				NULL,NULL);

			//check if the parameter passed from last window is a empty item. If it is a empty item,set focus to first one.
			//otherwise set focus to the proper one.
			bIsItemEmpty=TRUE;
			for(j=0;j<iReadPreferredOperatorNum;j++)
			{
				if(pReadPreferredOperator[j].index==iFocusItemLastWin+1)
				{
					bIsItemEmpty=FALSE;
					//strncpy(OperatorNum,pReadPreferredOperator->oper_id,sizeof(OperatorNum));
					break;
				}
			}

			InitFocusItem=0;
			for(i=0;i<iOperatorNum;i++)
			{
				strncpy(OperatorName,Net_oper[i]+7,50);
				memcpy(OperatorNum,Net_oper[i],5);
				OperatorNum[5]='\0';

				GetMcc_mnc(OperatorNum,mcc_mnc);

				SendMessage(hList,LB_ADDSTRING,i,(LPARAM)OperatorName);
 				SendMessage(hList,LB_SETIMAGE,MAKEWPARAM(MAKEWORD(IMAGE_BITMAP,-1),i),(LPARAM)hRadioOff);
				SendMessage(hList,LB_SETAUXTEXT,MAKEWPARAM(i,6),(LPARAM)mcc_mnc);

				if(!bIsItemEmpty)
					if(strcmp(OperatorNum,pReadPreferredOperator[j].oper_id)==0)
					{
						InitFocusItem=i;
						SendMessage(hList,LB_SETIMAGE,MAKEWPARAM(MAKEWORD(IMAGE_BITMAP,-1),i),(LPARAM)hRadioOn);
					}
			}
			
			SendMessage(hList,LB_SETCURSEL,InitFocusItem,0);

			SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,0,(LPARAM)ML("Cancel"));
			SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,1,(LPARAM)"");
			SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,2,(LPARAM)"Select");
			SetWindowText(hNetworkFrame,ML("Preferred network"));
		}
        break;

	case WM_PAINT:
		{
			BeginPaint(hWnd,NULL);
			EndPaint(hWnd,NULL);
		}
		break;
		
    case WM_SETFOCUS:
        SetFocus(hList);
        break;

	case PWM_SHOWWINDOW:
		SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,0,(LPARAM)ML("Cancel"));
		SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,1,(LPARAM)"");
		SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,2,(LPARAM)"Select");
		SetWindowText(hNetworkFrame,ML("Preferred network"));
		SetFocus(hList);
		break;

    case WM_CLOSE:
		DestroyWindow(hWnd);   
		DestroyWindow(hList);
        break;

    case WM_DESTROY :
        UnregisterClass("PreferredNetworksSelectionList",NULL);
		DeleteObject(hRadioOn);
		DeleteObject(hRadioOff);
        break;

	case WM_WRITEPREFLIST:
		if ( lParam == 1 )
		{
			if ( ME_WritePrefOperList((HWND)NULL,0,pReadPreferredOperator[j2].index,(LPARAM)NULL) != -1 )
			{	
				if(iReadPreferredOperatorNum>MAXPREFERREDNETWORK)
				{
					printf("===In MC55 the max Preferred Network is 8\r\n");
					PostMessage(hWnd,WM_CLOSE,0,0);
					SendMessage(hNetworkFrame,PWM_CLOSEWINDOW,  (WPARAM) hWnd, 0);
					return FALSE;
				}
				
				if(-1==ME_WritePrefOperList(hWnd,ME_MSG_WRITEPREFERREDOPERATOR,iFocusItemLastWin+1,strTmp)) //ME number begin with 1,not 0
				{
					//ignore failure information return to previous window
					printf("wrtie preferred Operator failure\r\n");
					PostMessage(hWnd,WM_CLOSE,0,0);
					SendMessage(hNetworkFrame,PWM_CLOSEWINDOW,  (WPARAM) hWnd, 0);
				}
			}
		}
		break;

	case WM_KEYDOWN:
		switch(LOWORD(wParam))
		{
		case VK_F10:
			PostMessage(hWnd,WM_CLOSE,0,0);
			SendMessage(hNetworkFrame,PWM_CLOSEWINDOW,  (WPARAM) hWnd, 0);
			break;
			
		case VK_F5:
			{
				// SendMessage(hNetworkFrame,WM_KEYDOWN,wParam,lParam);
 				i=SendMessage(hList,LB_GETCURSEL,0,0);
				memcpy(strTmp,Net_oper[i],5);
				strTmp[5]=0;

				for(j=0;j<iReadPreferredOperatorNum;j++)
				{
					if(strcmp(strTmp,pReadPreferredOperator[j].oper_id)==0)
					{
						j2 = j;
						if ( PLXConfirmWin(hNetworkFrame,hWnd,ML("Already assigned.\nMove?"),Notify_Request,ML("Preferred network"),
							ML("Yes"),ML("No")/*,WM_WRITEPREFLIST*/) )
						{
							ME_WritePrefOperList((HWND)NULL,0,pReadPreferredOperator[j].index,(LPARAM)NULL);

						}
						else
							return FALSE;
					}
				}
				
				if(iReadPreferredOperatorNum>MAXPREFERREDNETWORK)
				{
					printf("===In MC55 the max Preferred Network is 8\r\n");
					PostMessage(hWnd,WM_CLOSE,0,0);
					SendMessage(hNetworkFrame,PWM_CLOSEWINDOW,  (WPARAM) hWnd, 0);
					return FALSE;
				}

				if(-1==ME_WritePrefOperList(hWnd,ME_MSG_WRITEPREFERREDOPERATOR,iFocusItemLastWin+1,strTmp)) //ME number begin with 1,not 0
				{
					//ignore failure information return to previous window
					printf("wrtie preferred Operator failure\r\n");
					PostMessage(hWnd,WM_CLOSE,0,0);
					SendMessage(hNetworkFrame,PWM_CLOSEWINDOW,  (WPARAM) hWnd, 0);
				}
				break;
			}
		}
		break; 
			
		case ME_MSG_WRITEPREFERREDOPERATOR:
			switch(wParam) 
			{
			case ME_RS_SUCCESS:  //if success return to previous window
				{
					HWND hPrevWnd;
					printf("write preferred Operator success\r\n");
					PostMessage(hWnd,WM_CLOSE,0,0);
					hPrevWnd=GetWindow(hWnd,GW_HWNDNEXT);
					SendMessage(hPrevWnd,WM_PREFERREDNETCHANGED,0,0);
					SendMessage(hNetworkFrame,PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
				}
				break;

			default:
				printf("Write Preferred Operator failure\r\n");
				PostMessage(hWnd,WM_CLOSE,0,0);
				SendMessage(hNetworkFrame,PWM_CLOSEWINDOW, (WPARAM) hWnd, 0);
				break;
			}
			break;

    case WM_COMMAND:
        switch( LOWORD( wParam ))
        {
		case IDC_PREFERREDNETWORKSSELECTIONLIST:
			{
				switch(HIWORD(wParam))
				{
				case 1:
					break;
				case 2:
					break;
				default:
					NULL;
				}
			}
			break;
        }
        break;
    default :
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
    return lResult;

}
