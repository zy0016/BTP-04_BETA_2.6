
#include    "winpda.h"

#include "mullang.h"
#include    "plx_pdaex.h"
#include    "me_wnd.h"
#include    "MBPublic.h"
#include    "stdio.h"
#include    "stdlib.h"
#include    "RcCommon.h"

#define     IDC_BUTTON_QUIT         3
#define     IDC_BUTTON_SET          4
#define     IDC_USSD_LEFTBUTTON     6
#define     IDC_USSD_RIGHTBUTTON    7
#define     IDC_PHONE_LEFTBUTTON    8
#define     IDC_PHONE_RIGHTBUTTON   9
#define     IDC_GSMEDIT             5
#define     PUKNUMS                  10      //max limit for input puk
#define     IDC_USSD_TIMER          11

#define     IDC_GSMOPERATE          WM_USER + 110
#define     IDC_USSD_NOTIFY         WM_USER + 120
#define     IDC_ME_PHONE_SIM_AMX    WM_USER + 130
#define     CALLBACK_CODE_REMAIN    WM_USER + 140 //callback code remain times

#define     USSD_SHOWWND_MSG        WM_USER + 121
#define     USSD_REPLYWND_MSG       WM_USER + 122
#define     ME_MSG_GETLASTERROR     WM_USER + 150

#define     ME_GETPIN1STATE         WM_USER + 200

#define     CAP_NETWORKSERVICE      ML("Network service")
#define     NOTIFY_REQUEST          ML("Requesting...")
#define     NOTIFY_ERR_OTHERERRORS  ML("System failure")
#define     NOTIFY_ERR_UNKNOWN      ML("Unknown/nnetwork error./nPlease try again.")

#define     NOTIFY_SWITCH_PINCODE_REQUEST  "Please switch PIN \r\ncode request on"//ML("Please switch PIN \r\ncode request on")
#define     NOTIFY_PINCODE_CHANGED         "PIN code \r\nchanged"//ML("PIN code\r\nchanged")
#define     NOTIFY_WRONG_PINCODE           "PIN code \r\nwrong"//ML("PIN code\r\nwrong")
#define     NOTIFY_CODESNOTMATCH           "Codes do \r\nnot match"//ML("Codes do\r\nnot match")
#define     NOTIFY_PIN_BLOCKED             "PIN blocked"
#define     NOTIFY_PIN2CODE_CHANGED        ML("PIN2 code\r\nchanged")
#define     NOTIFY_PIN2_BLOCKED            "PIN2 blocked"//ML("PIN2 blocked")
#define		NOTIFY_WRONG_PIN2			   "PIN2 code \r\nwrong"//ML("PIN2 code\r\nwrong")
#define     NOTIFY_WRONG_PUKCODE           "PUK code \r\nwrong"//ML("PUK code\r\nwrong")
#define     NOTIFY_WRONG_PUK2CODE          "PUK2 code \r\nwrong"//ML("PUK2 code\r\nwrong")
#define     NOTIFY_SIM_BLOCKED             ML("SIM blocked")
#define     NOTIFY_PUK2_BLOCKED            ML("PUK2 blocked")

#define     TITLECAPTION            ML("Network service")
#define     BUTTONOK                (LPARAM)ML("Ok")
#define     BUTTONQUIT              (LPARAM)ML("Exit")
#define     BUTTONTXTLEFT_REPLY     ML("Reply")
#define     BUTTONTXTRIGHT          ML("Back")

#define     GSMOPERATESUCC_1        ML("Opsucceed") 
#define     GSMOPERATEFAIL_1        ML("Opfailure") 
#define     GSMOPERATESEARCH_FAIL   ML("Interrogatefail") 

#define     GSMOPERATESUCC          ML("Success") 
#define     GSMOPERATEFAIL          ML("Fail") 
#define     GSMOPERATEACTIVATE      ML("Activate") 
#define     GSMOPERATEDEACTIVATE    ML("Cancel") 
#define     GSMOPERATE_RA           ML("Registerandactivate") 
#define     GSMOPERATE_UD           ML("Unregisterandcancel") 
#define     GSMOPERATESEARCH        ML("Interrogate") 


#define     IMEIFAIL                ML("Unsuccessful") 

#define     GSMPIN1MODIFY           ML("Changepin1%s") 
#define     GSMPIN2MODIFY           ML("Changepin2%s") 
#define     GSMPUK1MODIFY           ML("Unlockpin1%s") 
#define     GSMPUK2MODIFY           ML("Unlockpin2%s") 
#define     GSMBARRINGMODIFY        ML("Registrationnetpassword %s") 
#define     GSMUNLOCKPS             ML("Unlockps%s") 
#define     GSM_WRONGPASSWORD       ML("Incorrectpassword") 
#define     GSMBARRING_ERROR1       ML("Servicenotsubscribed") 
#define     GSMBARRING_ERROR2       ML("Incorrectpassword") 
#define     GSMBARRING_ERROR3       ML("Invaliddial") 

#define     GSMCALLFORWARDING1      ML("Cfall") 
#define     GSMCALLFORWARDING2      ML("Cfallconditional") 
#define     GSMCALLFORWARDING3      ML("Cfunconditional") 
#define     GSMCALLFORWARDING4      ML("Cfnoanswer") 
#define     GSMCALLFORWARDING5      ML("Cfoutofreach") 
#define     GSMCALLFORWARDING6      ML("Cfbusy") 
#define     GSMCALLFORWARDING_NO    ML("Cfnumber")

#define     GSMCALLBARRING33        ML("Cboutgoing") 
#define     GSMCALLBARRING330       ML("Cancelcball") 
#define     GSMCALLBARRING331       ML("Cbinternational") 
#define     GSMCALLBARRING332       ML("Cbiceh") 
#define     GSMCALLBARRING333       ML("Cancelcboutgoing") 
#define     GSMCALLBARRING35        ML("Cbincoming") 
#define     GSMCALLBARRING351       ML("Cbica") 
#define     GSMCALLBARRING353       ML("Cancelcbincoming") 

#define     GSMCALLWAITING          ML("Callwaiting") 

#define     GSMCLIR                 ML("Clir%s%s") 
#define     GSMCLIR1                ML("Unsupport") 
#define     GSMCLIR2                ML("Clirpermanent") 
#define     GSMCLIR3                ML("Unknow") 
#define     GSMCLIR4                ML("Clirtemprestrict") 
#define     GSMCLIR5                ML("Clirtempsupport") 

#define     GSMCLIP                 ML("Clip%s%s") 
#define     GSMCOLP                 ML("Colp%s%s") 
#define     GSMCOLR                 ML("Colr%s%s") 

#define     GSMCLIRINVOKE           ML("Activateclir%s") 
#define     GSMCLIRSUPPRESS         ML("Suppressclir%s") 

#define     GSMCLIP_NONE            ML("Unsupport") 
#define     GSMCLIP_ALLOW           ML("Support") 
#define     GSMCLIP_UNKNOWN         ML("Unknow") 

#define     USSD_NOSERVICE          ML("Withoutservice") 

#define     WITHOUTDATA             ML("Withoutdata") 
#define     NOTIFY_UNSUCCESSFUL     ML("Unsuccessful")
#define     NOTIFY_PHONELANG        ML("Phone language:")
#define     IDS_SIMNOTREADY         ML("Reading SIM card.Please wait...")
#define     IDS_SIMNOTFIND         ML("Number not found\r\nfrom SIM location:")
#define     NOTIFY_NOSIMCARD        ML("No SIM Card")
typedef struct 
{
    HWND        hWnd;           
    int         iCallMsg;       
    char        * pPhoneNumber; 
    char        ** pGsmPre;     
    int         iStation;       
    DWORD       wParam;         
}GSMSTRUCT;
typedef struct 
{
    const char  **pGSMPre;      
    int         (* pFunction)(GSMSTRUCT * pGsmStruct);
    DWORD       wParam;         
    BOOL        bExecuteAtOnce; 
    
    BOOL        bGSMLengthMatch;
    
    
}GSMOPERATE;
typedef enum                        
{
    GSM_IMEI,                       
    GSM_PIN1,                       
    GSM_PIN2,                       
    GSM_PUK1,                       
    GSM_PUK2,                       
    GSM_CALLBAR,                    
    GSM_UNLOCKPS,                   

    GSM_CALLFORWARDING1,            
    GSM_CALLFORWARDING2,            
    GSM_CALLFORWARDING3,            
    GSM_CALLFORWARDING4,            
    GSM_CALLFORWARDING5,            
    GSM_CALLFORWARDING6,            

    GSM_CALLFORWARDING_CHECKBEGIN,  
    GSM_CALLFORWARDING1_CHECK,      
    GSM_CALLFORWARDING2_CHECK,      
    GSM_CALLFORWARDING3_CHECK,      
    GSM_CALLFORWARDING4_CHECK,      
    GSM_CALLFORWARDING5_CHECK,      
    GSM_CALLFORWARDING6_CHECK,      
    GSM_CALLFORWARDING_CHECKEND,    

    GSM_CALLBARRING33,              
    GSM_CALLBARRING331,             
    GSM_CALLBARRING332,             
    GSM_CALLBARRING35,              
    GSM_CALLBARRING351,             
    GSM_CALLBARRING330,             
    GSM_CALLBARRING333,             
    GSM_CALLBARRING353,             

    GSM_CALLBARRING_CHECKBEGIN,     
    GSM_CALLBARRING33_CHECK,        
    GSM_CALLBARRING331_CHECK,       
    GSM_CALLBARRING332_CHECK,       
    GSM_CALLBARRING35_CHECK,        
    GSM_CALLBARRING351_CHECK,       
    GSM_CALLBARRING330_CHECK,       
    GSM_CALLBARRING333_CHECK,       
    GSM_CALLBARRING353_CHECK,       
    GSM_CALLBARRING_CHECKEND,       

    GSM_CALLWAIT_ACTIVE,            
    GSM_CALLWAIT_CHECK,             
    GSM_CALLWAIT_DEACTIVE,          

    GSM_CLIP_CHECK,                 
    GSM_CLIR_CHECK,                 
    GSM_COLP_CHECK,                 
    GSM_COLR_CHECK,                 

    GSM_CLIR_INVOKE,                
    GSM_CLIR_SUPPRESS,              

	GSM_SW,
	GSM_SETLANG,
	GSM_SETALS1,
	GSM_SETALS2,
	GSM_PM,
	GSM_BTADDR,

	GSM_ACTIVATENSLC,
	GSM_DEACTIVATENSLC,
	GSM_CHECKNSLC,
	GSM_ACTIVATESPSL,
	GSM_DEACTIVATESPSL,
	GSM_CHECKSPSL,
	GSM_PROJECTMODE,
    CALLTRANSFERTEST_ON,
    CALLTRANSFERTEST_OFF,
    GETMENUERROR,
    GSM_END                         
}GSMTYPE;

static  LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static  void    OperateGSMWindow(GSMTYPE iGSM_Type,BOOL bShow,BOOL bAutoClose);
static  BOOL    GSMClewText_Succ(HWND hWnd);
static  BOOL    GSMClewText_Fail(HWND hWnd,WPARAM wParam,WPARAM lParam);

static  int     GsmIMEI(GSMSTRUCT * pGsmStruct);
static  int     GsmSecurity(GSMSTRUCT * pGsmStruct);
static  int     GsmModifyCallBarring(GSMSTRUCT * pGsmStruct);
static  int     GsmUnlockPS(GSMSTRUCT * pGsmStruct);
static  int     GsmForwardingOperate(GSMSTRUCT * pGsmStruct);
static  int     GsmForwardingCheck(GSMSTRUCT * pGsmStruct);
static  int     GsmCallBarringOperate(GSMSTRUCT * pGsmStruct);
static  int     GsmCallBarringCheck(GSMSTRUCT * pGsmStruct);
static  int     GsmCallWaitingOperate(GSMSTRUCT * pGsmStruct);
static  int     GsmPNPCheck(GSMSTRUCT * pGsmStruct);
static  int     GsmCLIRDeal(GSMSTRUCT * pGsmStruct);

static  int     GetGSMParam(const char * pGsmCode,char * param1,
                            char * param2,char * param3);
static  void    GetGSMSC(const char * pGsmCode,char * pSC);
static  void    SetWindowTextExt(HWND hEdit,const char * pString);
static  void    StringCombination(char * pResult,const char * p1,const char * p2,
                                  const char * p3,const char * p4);

static  BOOL    USSDDealRespose(HWND hWnd,WPARAM wParam,LPARAM lParam);
static  BOOL    SetUssdInfo(HWND hWnd, BOOL isre);
static  int     GsmSW(GSMSTRUCT * pGsmStruct);
static  int     GsmSetLang(GSMSTRUCT * pGsmStruct);
static int  GsmALS(GSMSTRUCT * pGsmStruct);
static int GsmPM(GSMSTRUCT * pGsmStruct);
static int GsmBTAddr(GSMSTRUCT * pGsmStruct);

static int GsmActivateNSLC(GSMSTRUCT * pGsmStruct);
static int GsmDeactivateNSLC(GSMSTRUCT * pGsmStruct);
static int GsmCheckNSLC(GSMSTRUCT * pGsmStruct);
static int GsmActivateSPSL(GSMSTRUCT * pGsmStruct);
static int GsmDeactivateSPSL(GSMSTRUCT * pGsmStruct);
static int GsmCheckSPSL(GSMSTRUCT * pGsmStruct);
static	int	GetStarCount(char *src);
static int GetSPCKParam(const char * pGsmSrcCode, PSPSIMLOCKCODE pHead, char * pSPCK);

extern void GetErrorString(ME_EXERROR ME_Error,char *strErrorInfo);
extern BOOL CallAppEntryEx(PSTR pAppName, WPARAM wparam, LPARAM lparam);
extern BOOL ShowNetSimLock(PNSLC  pHead, int counter,const char * pOperName);
extern BOOL ShowSPSimLock(PSPSIMLOCKCODE pHead);

static int GsmProjectMode(GSMSTRUCT * pGsmStruct);
extern void TESTAPP_EngineeringMode(void);
static BOOL GetOprName(const char * pNetNumber,char * strOprName,size_t buflen);
static int CallTransferTest(GSMSTRUCT * pGsmStruct);
extern void SetCallTransferTest(BOOL bTest);
extern BOOL GetErrorMenu(void);
