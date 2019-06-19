      
#include "MBInCall.h"
#include "pmi.h"
#include "MB_control.h"
#include    "MBCallEnd.h"
#include "pubapp.h"

#define DEBUG_MBCALLING

static const int    iPortraitX  = 3;
static const int    iPortraitY  = 3;
static const int    iPortraitW  = 80;
static const int    iPortraitH  = 80;

static const int    iSXSpace    = 10;
static const int    iSYSpace    = 5;

static const double dZero       = 0.0;
static const int    iSecond60   = 60;
static const char * pClassName  = "MBCallingWndClass";
static const char * pNum0       = "0";
static const char * pPlus       = "+";
static const char * pAsterisk   = "*";
static const char * pWell       = "#";
static const char * pTime       = "%02d:%02d:%02d";
static const char * pTimeEnd    = "%s:%02d:%02d:%02d";
static const char * pCostInfo1  = "%s:%f";
static const char * pCostInfo2  = "%ld";
static const char * pAbortAT    = "AT+WAC\r";
static const char * pSendDTMFNum = "%s\n%s";
static const int    iAbortValue = 515;
static const int    iTimeOut    = 4000;

static MBCALLTYPE   CallingType;                    
static INTERFACE    InterfaceType = INTERFACE_NONE; 

#define   DTMFNUM_STRLN         50
static char         cPhoneNumberExtension[PHONENUMMAXLEN + 1]="";
static char         cSendDTMFNum[DTMFNUM_STRLN]="";
static int          iExtension;             
static BOOL         bExtension;             

static HWND         hCalling;               
static HWND    hFocus;
static PHONEITEM    PhoneItem[MAX_COUNT];   
static int          PhoneGroupNum = 0;      
static HINSTANCE    hInstance;

static BOOL         bListCurCalling;        

static OPERATETYPE  ListCurCallsType;       
static WPARAM       ListCurCallswParam;     
static WPARAM       ListCurCallslParam;     

static BOOL         bMute = FALSE;          
static BOOL         bHolding = FALSE;       
static BOOL         bRecord = FALSE;        
static BOOL         bHF= FALSE;
static BOOL         bToEndCall = FALSE;  

static RECT         rName;                  
static RECT         rPhoneCode;             
static RECT         rTime;                  
static RECT         rPhoto;                 
static RECT         rCostInfo;              
static int          iCallingOneNum;         
static HBITMAP      hBitmapCalling;              
static SIZE         sPortrait;              
static MBConfig     MBCongig_Calling;       
static int          iTime_Second;           
static int          iInitSecond;            

static char         cName[PHONENUMMAXLEN + 1];
static char         cPhone[PHONENUMMAXLEN + 1];

static BOOL         bCostLimit = FALSE;     
static BOOL         bSingal = FALSE;//当前电话断连是否是因为没有信号
static BOOL         bAOCInfo;               
static BOOL         bCanGetCurrentCCM;      
static DWORD        dCostInfo;              
static ME_PUCINFO   me_pucinfo;             
static  BOOL            bSetPortrait;   
static  INITPAINTSTRUCT  incallif_initpaint;
static  HGIFANIMATE     hIncallGif;
static BOOL finab;
static ABINFO abinfo;
static  SIZE            sCartoon;     
static SIZE Incallsize;  
static  HBITMAP   hBitPortrait;
static 	char  endshowinfo[ME_PHONENUM_LEN];
static 	char  endstatusinfo[20];
static  char  endtimeinfo[20];
static  char  endcostinfo[20];
static BOOL bIcon;
static HWND hPreWnd;
static BOOL bMemberIcon;  // TRUE: active  FALSE: held
static HBITMAP hActiveBmp = NULL;
static HBITMAP hHeldBmp = NULL;
static BOOL SFinEmergency = FALSE;
static int iLineStatus = 1;//线路
static HBITMAP hLineBmp = NULL;
static SIZE LinePicSize = {0,0};
static BOOL bTransfer = FALSE;
static DWORD dAcmMax = 0;
static BOOL dShowAOCClew = FALSE;
typedef enum                                
{
    COSTTYPE_UNIT,                          
		COSTTYPE_PRICE                          
}COSTTYPE;
static COSTTYPE     CostType;               
static double       dCost;                  
static HWND hMemberWnd;
typedef enum {
	EXTRACTMEMBER = 0,
		DROPMEMBER,
} MEMBERWND;
static AudioInterfaceType   BTAudioInterfaceType;//当前通道类型,用于蓝牙
static BOOL bCallTransferTest = FALSE;

static BOOL bSetVolume = FALSE;//是否设置音量
static int iCurrentVolume;//当前音量
static const int iMaxVolume = 4;
static const int iMinVolume = 1;
static RECT VolumeRect;//音量显示区域

static BOOL bAutoHeadset = FALSE;//接通之后是否是自动免提功能
static BOOL bErrorMenu = FALSE;

BOOL CreateMemberWnd(HWND hwnd, MEMBERWND type);
static void MBCallEndInfo(HWND hWnd, const char * pName,const char * pPhoneNumber,const char * pDuration,const char * pCost, BOOL IsShow);
extern CallheadphoneWindow(HWND hFrame,int i);
void closeMemberWnd();

extern void CloseSendDTMFWnd();
extern BOOL GetChangeLockStatus();
extern void SetChangeLockStatus();
extern void NotifyEmergencyCall();
extern void NotifyRingWndUpdate();
extern void SetShowSummaryFlag(BOOL f);
extern BOOL GetShowSummaryFlag();
extern BOOL GetFlagStar(void);
extern BOOL AdjustVolumeForHandFree(BOOL);
BOOL    MBCallingWindow(HWND hpre, const PHONEITEM * pPhoneItem,
                        const char * pExtension,BOOL bHeadset)
{
    int iFirstNotUsedItem;  
    int nUsedItemNum;       
    
    if ((1 == PhoneGroupNum) || (0 == PhoneGroupNum))
    {
        iFirstNotUsedItem = GetFirstNotUsedItem();
        if (-1 == iFirstNotUsedItem)
            return FALSE;
		
        SetAllType(CALL_STAT_HELD); 
		
        memcpy(&PhoneItem[iFirstNotUsedItem],pPhoneItem,sizeof(PHONEITEM));
        PhoneItem[iFirstNotUsedItem].ItemStat = ITEMUSED;
		
        SetPhoneGroupNum();
		
        nUsedItemNum = GetUsedItemNum();
		
        if (1 == nUsedItemNum)
            CallingType = MBCALLINGONE;
        else if ((1 < nUsedItemNum) && (nUsedItemNum <= MAX_COUNT))
            CallingType = MBCALLINGMULT;
        else
            return FALSE;
		
        bAutoHeadset = bHeadset;
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
        printf("\nMB MBCallingWindow,CallingType:%d,号码:%s\n",
            CallingType,PhoneItem[iFirstNotUsedItem].Calls_Info.PhoneNum);
#endif
        if (pExtension != NULL)
        {
            if (strlen(pExtension) == 0)
            {
                bExtension = FALSE;                         
            }
            else
            {
                bExtension = TRUE;                          
                iExtension = 0;                             
                strcpy(cPhoneNumberExtension,pExtension);   
            }
        }
        else
        {
            bExtension = FALSE;                             
        }
		hPreWnd = hpre;
        return (MBTalkingWindow(hPreWnd));
    }
    return FALSE;
}

void    MessageMBCallingOne(UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    if ((hCalling != NULL) && (IsWindow(hCalling)))
        PostMessage(hCalling,wMsgCmd,wParam,lParam);
}
BOOL    IsWindow_MBCallingOne(void)
{
    return (MBCALLINGONE == CallingType);
}
BOOL    IsWindow_MBCallingMult(void)
{
    return (MBCALLINGMULT == CallingType);
}

void    SetListCurCalls(BOOL bListCur)
{
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
    printf("mb MBCalling 设置正在列当前通话\r\n");
#endif
    bListCurCalling = bListCur;
}

BOOL    GetListCurCalling(void)
{
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
    if (bListCurCalling)
        printf("mb MBCalling 正在列当前通话\r\n");
    else
        printf("mb MBCalling 没有列当前通话\r\n");
#endif
    return bListCurCalling;
}

static  int     GetFirstNotUsedItem(void)
{
    int i;
    for (i = 0;i < MAX_COUNT;i++)
    {
        if (ITEMNOTUSED == PhoneItem[i].ItemStat)
            return i;
    }
    return -1;
}

int     GetUsedItemNum(void)
{
    int nRet = 0,i;
    for(i = 0;i < MAX_COUNT;i++)
    {
        if (ITEMUSED == PhoneItem[i].ItemStat)
            nRet++;
    }
    return nRet;
}

int     GetItemTypeNum(int iType)
{
    int nRet = 0,i;
    if ((CALL_STAT_ACTIVE != iType) && (CALL_STAT_HELD != iType))
        return -1;
	
    for (i = 0;i < MAX_COUNT;i++)
    {
        if ((ITEMUSED == PhoneItem[i].ItemStat) &&
            (iType == PhoneItem[i].Calls_Info.Stat))
            nRet++;
    }
    return nRet;
}

static  int     GetTheNUsedItem(int nTheN)
{
    int i;
    if (nTheN <= 0)
        return -1;
	
    for (i = 0 ;i < MAX_COUNT;i++)
    {
        if (ITEMUSED == PhoneItem[i].ItemStat)
        {
            nTheN--;
            if (0 == nTheN)
                return i;
        }
    }
    return -1;
}

static  void    SetPhoneGroupNum(void)
{
    int iHeldNum,iActiveNum;
	
    iHeldNum   = GetItemTypeNum(CALL_STAT_HELD);
    iActiveNum = GetItemTypeNum(CALL_STAT_ACTIVE);
	
    if ((iHeldNum > 0) && (iActiveNum > 0))
        PhoneGroupNum = 2;
    else if ((iHeldNum == 0) && (iActiveNum > 0) || (iHeldNum > 0) && (iActiveNum == 0))
        PhoneGroupNum = 1;
    else 
        PhoneGroupNum = 0;
}

int     GetPhoneGroupNum(void)
{
    SetPhoneGroupNum();
    return (PhoneGroupNum);
}

static  void    ExecuteHangup(void)
{
    int iHeldNum,iActiveNum;
	
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
    printf("\nMB MBCalling,挂断键,CallingType:%d\n",CallingType);
#endif
    switch (CallingType)
    {
    case MBCALLINNONE:
		SendMessage(GetCapture(), WM_CANCELMODE, 0, 0);
		SendMessage(hCalling, WM_CLOSE, 0,0);
//        DestroyWindow(hCalling);
        break;
		
    case MBCALLINGONE:
        if (strcmp(PhoneItem[iCallingOneNum].cName,STATUS_INFO_EMERGENTCALL) == 0)
        {
            ME_VoiceHangup(hCalling,WM_VOICE_HANGUPONE);
        }
        else
        {
            if (-1 == ME_SwitchCallStatus(hCalling,WM_VOICE_HANGUPONE,CUT_ALL,0))
                AppMessageBox(hCalling,HANGUPFAIL,TITLECAPTION,WAITTIMEOUT);
        }
        break;
		
    case MBCALLINGMULT:
        SetPhoneGroupNum();
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
        printf("MB MBCalling,挂断键 PhoneGroupNum:%d\r\n",PhoneGroupNum);
#endif
        if (PhoneGroupNum == 2)
        {
//			if (GetShowSummaryFlag())
//			{
//				if (-1 == ME_SwitchCallStatus(hCalling,IDC_MULTME_ALLHANGUP,CUT_ALL,0))
//					AppMessageBox(hCalling,HANGUPFAIL,TITLECAPTION,WAITTIMEOUT);
//			}
//			else
			{
            if (-1 == ME_SwitchCallStatus(hCalling,IDC_ME_CUT_ACTIVED,CUT_ACTIVED,0))
                AppMessageBox(hCalling,HANGUPFAIL,TITLECAPTION,WAITTIMEOUT);
			}
        }
        else if (PhoneGroupNum == 1)
        {
            iHeldNum   = GetItemTypeNum(CALL_STAT_HELD);
            iActiveNum = GetItemTypeNum(CALL_STAT_ACTIVE);
            if ((iHeldNum == 0) && (iActiveNum > 0))
            {
                if (-1 == ME_SwitchCallStatus(hCalling,IDC_ME_CUT_ACTIVED,CUT_ACTIVED,0))
                    AppMessageBox(hCalling,HANGUPFAIL,TITLECAPTION,WAITTIMEOUT);
            }
            else
            {
                if (-1 == ME_SwitchCallStatus(hCalling,IDC_ME_CUT_HELD,CUT_HELD,0))
                    AppMessageBox(hCalling,HANGUPFAIL,TITLECAPTION,WAITTIMEOUT);
            }
        }
        else
        {
            MBListCurCalls(LISTCURCALLS_REPEAT,-1,-1);
        }
        break;
    }
}

static  void    AfterHangup_Single(void)
{
    char cLastTime[DURATIONLEN] = "";
    char cCostInfo[COSTLEN] = "";
    MBRecordCostTime CurCostPara;
	
    BT_Hangup();//蓝牙挂断
    if (PhoneItem[iCallingOneNum].ItemStat != ITEMUSED)
    {
		
		MBCallEndInfo(hCalling,NULL,NULL,NULL,NULL, FALSE);
		SendMessage(GetCapture(), WM_CANCELMODE, 0, 0);
		
		SetTimer(hCalling,IDC_END_TIMER,END_TIMER_DURATION,0);
        return;
    }
	
    ReadRecord_Cost(&CurCostPara);          
    RemTime(&CurCostPara,iCallingOneNum);   
    SaveRecord_Cost(&CurCostPara);
	MessageUpdateCost();
	if (MBCongig_Calling.bShowCallDuration) 
	{
		if (PhoneItem[iCallingOneNum].Calls_Info.Dir == CALL_DIR_ORIGINATED)
		{   
			SaveMBRecordUsedTime(DIALED_REC,
				PhoneItem[iCallingOneNum].Calls_Info.PhoneNum,
				&PhoneItem[iCallingOneNum].TimeInDial,&PhoneItem[iCallingOneNum].TimeUsed);
			MBRecrodList_Refresh(DIALED_REC);
			SaveMBRecordLog(DIALED_REC,
				PhoneItem[iCallingOneNum].Calls_Info.PhoneNum,
				&PhoneItem[iCallingOneNum].TimeInDial,&PhoneItem[iCallingOneNum].TimeUsed);
		}
		else if (PhoneItem[iCallingOneNum].Calls_Info.Dir == CALL_DIR_TERMINATED)
		{
			SaveMBRecordUsedTime(ANSWERED_REC,
				PhoneItem[iCallingOneNum].Calls_Info.PhoneNum,
				&PhoneItem[iCallingOneNum].TimeInDial,&PhoneItem[iCallingOneNum].TimeUsed);
			MBRecrodList_Refresh(ANSWERED_REC);
			SaveMBRecordLog(ANSWERED_REC,
				PhoneItem[iCallingOneNum].Calls_Info.PhoneNum,
				&PhoneItem[iCallingOneNum].TimeInDial,&PhoneItem[iCallingOneNum].TimeUsed);
		}
		
		sprintf(cLastTime,pTime,
			PhoneItem[iCallingOneNum].TimeUsed.wHour,
			PhoneItem[iCallingOneNum].TimeUsed.wMinute,
			PhoneItem[iCallingOneNum].TimeUsed.wSecond);
	}
	else
    {
        if (MBCongig_Calling.bCallSummary)
        {
            sprintf(cLastTime,pTime,
                PhoneItem[iCallingOneNum].TimeUsed.wHour,
                PhoneItem[iCallingOneNum].TimeUsed.wMinute,
                PhoneItem[iCallingOneNum].TimeUsed.wSecond);
        }
        else
        {
            strcpy(cLastTime, "");
        }
    }
	//if (MBCongig_Calling.bShowCallCost)
	{
		if (bAOCInfo)
		{
			if (bCostLimit)
			{
				strcpy(cCostInfo,DIALLING_FARE);
			}
			else
			{
				switch (CostType)
				{
				case COSTTYPE_UNIT:
					sprintf(cCostInfo,pCostInfo2,dCostInfo);
					break;
				case COSTTYPE_PRICE:
					//sprintf(cCostInfo,pCostInfo1,me_pucinfo.currency,dCostInfo * dCost);
                    MB_Mtoa(dCostInfo * dCost,cCostInfo);
					MB_DealDouble(cCostInfo,3);
					break;
				default:
					strcpy(cCostInfo, "");
					break;
				}
			}
		}
		else
			strcpy(cCostInfo, "");
	}

	PhoneItem[iCallingOneNum].ItemStat = ITEMNOTUSED;
	SetPhoneGroupNum();
	
	SendMessage(hCalling, MB_ACTIVE, 0, 0);

    if (!bSingal)
    {
        if (!bToEndCall) 
        {
            MBInsertMBEndNode(PhoneItem[iCallingOneNum].Calls_Info.PhoneNum, 
                PhoneItem[iCallingOneNum].cName, cLastTime,cCostInfo);
            MBCallEndNodeWindow();
        }
        else if (bToEndCall && MBCongig_Calling.bCallSummary)
        {
            MBInsertMBEndNode(PhoneItem[iCallingOneNum].Calls_Info.PhoneNum, 
                PhoneItem[iCallingOneNum].cName, cLastTime,cCostInfo);
            MBCallEndNodeWindow();
        }
        else
        {
            if (MBCongig_Calling.bCallSummary)
            {
                MBInsertMBEndNode(PhoneItem[iCallingOneNum].Calls_Info.PhoneNum, 
                    PhoneItem[iCallingOneNum].cName, cLastTime,cCostInfo);
            }
            else
            {
                MBInsertMBEndNode(PhoneItem[iCallingOneNum].Calls_Info.PhoneNum, 
                    PhoneItem[iCallingOneNum].cName, "",cCostInfo);
            }
            MBCallEndNodeWindow();
        }
    }
	else
    {
        PLXTipsWin(NULL, NULL, NULL, ML("no network"), "", Notify_Failure, ML("Ok"), NULL, WAITTIMEOUT);
    }
	SendMessage(GetCapture(), WM_CANCELMODE, 0, 0);
	SetTimer(hCalling,IDC_END_TIMER,END_TIMER_DURATION,0);

    ShowWindow(hCalling,SW_HIDE);
    UpdateWindow(hCalling);
}

int  AfterHangup_MultOne(int iIndex)
{
	char cTimer[DURATIONLEN] = "";
	char cCostInfo[COSTLEN] = "";
    MBRecordCostTime MultCostPara;   
	
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
    printf("mb MBCalling AfterHangup_MultOne,iIndex:%d\r\n",iIndex);
#endif
    if ((iIndex < 0) || (iIndex > MAX_COUNT - 1) || 
        (PhoneItem[iIndex].ItemStat != ITEMUSED))
        return -1;
	
    ReadRecord_Cost(&MultCostPara); 
    RemTime(&MultCostPara,iIndex);  
    SaveRecord_Cost(&MultCostPara); 
	MessageUpdateCost();
    PhoneItem[iIndex].ItemStat = ITEMNOTUSED;
	
    if (PhoneItem[iIndex].Calls_Info.Dir == CALL_DIR_ORIGINATED)
    {
        SaveMBRecordUsedTime(DIALED_REC,
            PhoneItem[iIndex].Calls_Info.PhoneNum,
            &PhoneItem[iIndex].TimeInDial,&PhoneItem[iIndex].TimeUsed);
		MBRecrodList_Refresh(DIALED_REC);
		SaveMBRecordLog(DIALED_REC,
            PhoneItem[iIndex].Calls_Info.PhoneNum,
            &PhoneItem[iIndex].TimeInDial,&PhoneItem[iIndex].TimeUsed);
    }
    else if (PhoneItem[iIndex].Calls_Info.Dir == CALL_DIR_TERMINATED)
    {
        SaveMBRecordUsedTime(ANSWERED_REC,
            PhoneItem[iIndex].Calls_Info.PhoneNum,
            &PhoneItem[iIndex].TimeInDial,&PhoneItem[iIndex].TimeUsed);
		MBRecrodList_Refresh(ANSWERED_REC);
		SaveMBRecordLog(ANSWERED_REC,
            PhoneItem[iIndex].Calls_Info.PhoneNum,
            &PhoneItem[iIndex].TimeInDial,&PhoneItem[iIndex].TimeUsed);
    }
	if (MBCongig_Calling.bShowCallDuration) 
		sprintf(cTimer,pTime,
		PhoneItem[iIndex].TimeUsed.wHour,
		PhoneItem[iIndex].TimeUsed.wMinute,
		PhoneItem[iIndex].TimeUsed.wSecond);
	else
		strcpy(cTimer, "");
	//if (MBCongig_Calling.bShowCallCost)
	{
		if (bAOCInfo)
		{
			if (bCostLimit)
			{
				strcpy(cCostInfo,DIALLING_FARE);
			}
			else
			{
				switch (CostType)
				{
				case COSTTYPE_UNIT:
					sprintf(cCostInfo,pCostInfo2,dCostInfo);
					break;
				case COSTTYPE_PRICE:
					//sprintf(cCostInfo,pCostInfo1,me_pucinfo.currency,dCostInfo * dCost);
                    MB_Mtoa(dCostInfo * dCost,cCostInfo);
					MB_DealDouble(cCostInfo,3);
					break;
				default:
					strcpy(cCostInfo, "");
					break;
				}
			}
		}
		else
			strcpy(cCostInfo, "");
	}
	if (!bToEndCall) 
	{
		MBInsertMBEndNode(PhoneItem[iIndex].Calls_Info.PhoneNum, PhoneItem[iIndex].cName, cTimer,cCostInfo);
	}
	else if (bToEndCall && MBCongig_Calling.bCallSummary)
	{
		MBInsertMBEndNode(PhoneItem[iIndex].Calls_Info.PhoneNum, PhoneItem[iIndex].cName, cTimer,cCostInfo);
	}
	else
	{
		MBInsertMBEndNode(PhoneItem[iIndex].Calls_Info.PhoneNum, PhoneItem[iIndex].cName, cTimer,cCostInfo);
	}
    return 1;
}

static  void    AfterHangup_Mult_All(void)
{
    int i;
	int contimeidx = 0;
    char cTimer[DURATIONLEN] = "";
	char cCostInfo[COSTLEN] = "";
    MBRecordCostTime MultCostPara;   
	BOOL IsShowEnd;
	
    ReadRecord_Cost(&MultCostPara);
	
	for (i = 0; i< MAX_COUNT; i++)
	{
		if (PhoneItem[i].ItemStat == ITEMUSED) 
		{
			contimeidx = i;
			break;
		}
	}
	IsShowEnd = FALSE;
    for (i = 0;i < MAX_COUNT;i++)
    {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
        printf("MB 多方通话全部挂断,PhoneItem[%d].ItemStat:%d,号码:%s\r\n",
            i,PhoneItem[i].ItemStat,PhoneItem[i].Calls_Info.PhoneNum);
#endif
        if (PhoneItem[i].ItemStat == ITEMUSED)
        {						
            RemTime(&MultCostPara,i);
            if (PhoneItem[i].Calls_Info.Dir == CALL_DIR_ORIGINATED)
            {
                SaveMBRecordUsedTime(DIALED_REC,
                    PhoneItem[i].Calls_Info.PhoneNum,
                    &PhoneItem[i].TimeInDial,&PhoneItem[i].TimeUsed);
				MBRecrodList_Refresh(DIALED_REC);
				SaveMBRecordLog(DIALED_REC,
					PhoneItem[i].Calls_Info.PhoneNum,
					&PhoneItem[i].TimeInDial,&PhoneItem[i].TimeUsed);				
            }
            else if (PhoneItem[i].Calls_Info.Dir == CALL_DIR_TERMINATED)
            {
                SaveMBRecordUsedTime(ANSWERED_REC,
                    PhoneItem[i].Calls_Info.PhoneNum,
                    &PhoneItem[i].TimeInDial,&PhoneItem[i].TimeUsed);
				MBRecrodList_Refresh(ANSWERED_REC);
				SaveMBRecordLog(ANSWERED_REC,
					PhoneItem[i].Calls_Info.PhoneNum,
					&PhoneItem[i].TimeInDial,&PhoneItem[i].TimeUsed);
            }
            sprintf(cTimer,pTime,
                PhoneItem[i].TimeUsed.wHour,
                PhoneItem[i].TimeUsed.wMinute,
                PhoneItem[i].TimeUsed.wSecond);

			if (MBCongig_Calling.bShowCallDuration) 
				sprintf(cTimer,pTime,
                PhoneItem[i].TimeUsed.wHour,
                PhoneItem[i].TimeUsed.wMinute,
                PhoneItem[i].TimeUsed.wSecond);
			else
				strcpy(cTimer, "");
			//if (MBCongig_Calling.bShowCallCost)
			{
				if (bAOCInfo)
				{
					if (bCostLimit)
					{
						strcpy(cCostInfo,DIALLING_FARE);
					}
					else
					{
						switch (CostType)
						{
						case COSTTYPE_UNIT:
							sprintf(cCostInfo,pCostInfo2,dCostInfo);
							break;
						case COSTTYPE_PRICE:
							//sprintf(cCostInfo,pCostInfo1,me_pucinfo.currency,dCostInfo * dCost);
                            MB_Mtoa(dCostInfo * dCost,cCostInfo);
							MB_DealDouble(cCostInfo,3);
							break;
						default:
							strcpy(cCostInfo, "");
							break;
						}
					}
				}
				else
					strcpy(cCostInfo, "");
			}
			if(!IsShowEnd)
			{				
				if (!bToEndCall) 
				{
					MBInsertMBEndNode(PhoneItem[i].Calls_Info.PhoneNum, PhoneItem[i].cName, cTimer,cCostInfo);
				}
				else if (bToEndCall && MBCongig_Calling.bCallSummary)
				{
					MBInsertMBEndNode(PhoneItem[i].Calls_Info.PhoneNum, PhoneItem[i].cName, cTimer,cCostInfo);
				}
				else 
					MBInsertMBEndNode(PhoneItem[i].Calls_Info.PhoneNum, PhoneItem[i].cName, cTimer,cCostInfo);
			}
        }       	
		PhoneItem[i].ItemStat = ITEMNOTUSED;	
    }
    MBCallEndNodeWindow();
	IsShowEnd = TRUE;
	SetPhoneGroupNum();	
	SendMessage(hCalling, MB_ACTIVE, 0, 0);		
    SaveRecord_Cost(&MultCostPara);	
	MessageUpdateCost();

	SendMessage(GetCapture(), WM_CANCELMODE, 0, 0);
	SetTimer(hCalling,IDC_END_TIMER,END_TIMER_DURATION,0);    
}

BOOL    MBCalling_Shortcut(int iType,const char * pShortPhone)
{
    switch (iType)
    {
    case CUT_HELD:
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
        printf("mb MBCalling_Shortcut挂断保持的通话\r\n");
#endif
        if (IsWindow_MBRing())
        {   
            if (-1 == ME_SwitchCallStatus(hCalling,IDC_ME_REPEAT,CUT_HELD,0))
                return FALSE;
        }
        else
        {
            if (-1 == ME_SwitchCallStatus(hCalling,IDC_ME_CUT_HELD,CUT_HELD,0))
                return FALSE;
        }
        break;
		
    case CUT_ACTIVED:
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
        printf("mb MBCalling_Shortcut挂断激活的通话\r\n");
#endif
        if (-1 == ME_SwitchCallStatus(hCalling,IDC_ME_CUT_ACTIVED,CUT_ACTIVED,0))
            return FALSE;
		
        break;
		
    case ACTIVE_SWITCH:
        switch (CallingType)
        {
        case MBCALLINNONE:
            break;
        case MBCALLINGONE:
        case MBCALLINGMULT:
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
            printf("mb MBCalling_Shortcut 通话切换:%d\r\n",CallingType);
#endif
            if (-1 == ME_SwitchCallStatus(hCalling,IDC_ME_SHORTCUTSWITCH,ACTIVE_SWITCH,0))
                return FALSE;
			
            break;
        }
        break;
		
		case ADD_TO_ACTIVE:
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
			printf("mb MBCalling_Shortcut 将保持的通话加入激活通话\r\n");
#endif
			if (-1 == ME_SwitchCallStatus(hCalling,IDC_MULTME_ALLACTIVE,ADD_TO_ACTIVE,0))
				return FALSE;
			
			break;
			
		case ACTIVE_SWITCH_X:
			if (NULL == pShortPhone)
				return FALSE;
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
			printf("mb MBCalling_Shortcut 将除指定序号外的所有通话变为保持,%s\r\n",pShortPhone);
#endif
			if (-1 == ME_VoiceDial(hCalling,IDC_ME_ACTIVE_SWITCH_X,(char *)pShortPhone,NULL))
				return FALSE;
			
			break;
			
		case CUT_ACTIVED_X:
			if (NULL == pShortPhone)
				return FALSE;
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
			printf("mb MBCalling_Shortcut 挂断指定序号的激活通话,%s\r\n",pShortPhone);
#endif
			if (-1 == ME_VoiceDial(hCalling,IDC_ME_CUT_ACTIVED_X,(char *)pShortPhone,NULL))
				return FALSE;
			
			break;
			
		case CALL_TRANSFER:
			if (NULL == pShortPhone)
				return FALSE;
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
			printf("mb MBCalling_Shortcut 转换,PhoneGroupNum:%d,%s\r\n",PhoneGroupNum,pShortPhone);
#endif
            if (bCallTransferTest)
            {
                ME_SwitchCallStatus(hCalling,IDC_ME_CALL_TRANSFER,CUT_ALL,0);
                return TRUE;
            }
			if (-1 == ME_VoiceDial(hCalling,IDC_ME_CALL_TRANSFER,(char *)pShortPhone,NULL))
                return FALSE;

            break;

        case CUT_ALL:
            ME_SwitchCallStatus(hCalling,IDC_MULTME_ALLHANGUP,CUT_ALL,0);
            break;

		default:
			if (NULL == pShortPhone)
				return FALSE;
			if (-1 == ME_VoiceDial(hCalling,IDC_ME_SHORTOTHER,(char *)pShortPhone,NULL))
				return FALSE;
			
			break;
    }
    return TRUE;
}

static  BOOL    MBTalkingWindow(HWND hpre)
{
    WNDCLASS    wc;
	
    if (IsWindow(hCalling))
    {
        if (!PM_GetPhoneLockStatus())
        {
            ShowWindow(hCalling,SW_SHOW);
            UpdateWindow(hCalling);
            SetFocus(hCalling);
            MoveWindowToTop(hCalling);
        }
		CALLWND_InsertDataByHandle(pGlobalCallListBuf, hCalling);
        PostMessage(hCalling,MB_ACTIVE,0,0);
        return TRUE;
    }
    wc.style         = CS_OWNDC;
    wc.lpfnWndProc   = AppWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = pClassName;
	
    if (!RegisterClass(&wc))
        return FALSE;
	
    hCalling = CreateWindow(pClassName,"", 
        WS_CAPTION|PWS_STATICBAR|PWS_NOTSHOWPI|WS_VISIBLE, 
        PLX_WIN_POSITION,NULL,NULL,NULL,NULL);

	CALLWND_InsertDataByHandle(pGlobalCallListBuf, hCalling);

//	Set_CallWndOrder(hpre, hCalling);
    if (NULL == hCalling)
    {
        UnregisterClass(pClassName,NULL);
        return FALSE;
    }
	if (GetChangeLockStatus()) 
	{
		SetChangeLockStatus();
		DlmNotify(PS_LOCK_ENABLE, TRUE);
		hFocus = hCalling;
		return  TRUE;
	}
	else
	{
		hFocus = hCalling;
        if (PM_GetPhoneLockStatus())
        {
            SendMessage(hCalling, PWM_SETAPPICON, 
                MAKEWPARAM(IMAGE_ICON, CALLICON),(LPARAM)IN_CALL_ICON);
        }
        else
        {
            ShowWindow(hCalling, SW_SHOW);
            UpdateWindow(hCalling); 
        }
		if (GetFlagStar()) 
		{
			bExtension = FALSE;
			CreateSendDTMFWindow(cPhoneNumberExtension);			
		}
		return (TRUE);
	}
}

static  LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
    static BOOL bKeyDown;
	static int iItemData;
    LRESULT     lResult;
    HDC         hdc;
    int         nUsedItemNum;
	COLORREF Color;
    BOOL     bTran;
    HMENU       hMenu;
    SYSTEMTIME  sCurtime;
	RECT listsize;
    
	MEMBERWND tp;
	BOOL  isLock = FALSE;
    lResult = (LRESULT)TRUE;
	
    switch ( wMsgCmd )
    {
    case WM_CREATE :
		{
			SYSTEMTIME	SystemTime;
			char aTimeBuf[25], aDateBuf[25];
			
			GetLocalTime(&SystemTime);	
			GetTimeDisplay(SystemTime, aTimeBuf, aDateBuf);
			SetWindowText(hWnd, aTimeBuf);
			SendMessage(hWnd, PWM_SETAPPICON,MAKEWPARAM(IMAGE_BITMAP, POWERICON), 0);
			SendMessage(hWnd, PWM_SETAPPICON,MAKEWPARAM(IMAGE_BITMAP, SIGNALICON), 0);
			SetTimer(hWnd, TIMER_ID_FRESH, 5000, NULL);
			SFinEmergency = FALSE;
			ReadMobileConfigFile(&MBCongig_Calling);
			bAOCInfo        = FALSE;        
			InterfaceType   = INTERFACE_NONE;
			bListCurCalling = FALSE;        
			bMute           = FALSE;        
			bHolding        = FALSE;        		
			bKeyDown        = FALSE;        
			bHF =FALSE;
			bToEndCall = FALSE;		
            dShowAOCClew    = FALSE;
            BTAudioInterfaceType = ME_AUDIO_ANALOG;//模拟通道
            bTransfer = FALSE;
            iLineStatus = 1;

            bErrorMenu = FALSE;
            bSetVolume = FALSE;
            iCurrentVolume = GetHeadSetVolum();
            SetRect(&VolumeRect,20,10,GetScreenUsableWH1(SM_CXSCREEN),30);

			GetLocalTime(&sCurtime);
			iInitSecond = sCurtime.wSecond;        
			GetClientRect(hWnd,&listsize);
			hMenu = CreateMenu();
			if (FALSE == PDASetMenu(hWnd, hMenu))
			{
				DestroyMenu(hMenu);
				hMenu = NULL;
			}
			SendMessage(hWnd,MB_ACTIVE,0,0);

            f_sleep_register(CALL_DATA);//禁止睡眠
            SendMessage(hWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, CALLICON),(LPARAM)IN_CALL_ICON);

            if (MBCongig_Calling.bShowCallDuration) 
			    SetTimer(hWnd,IDC_TIMER_CALLING,TIMER_CALLING,NULL);

            SetTimer(hWnd,IDC_TIMER_INIT,TIMER_INIT,NULL);

            hdc = GetDC(hWnd);
            GetImageDimensionFromFile(INCALLING_ONE,&Incallsize);
            hBitmapCalling = CreateBitmapFromImageFile(hdc,INCALLING_ONE,&Color,&bTran);
            ReleaseDC(hWnd,hdc);

            if (bAutoHeadset)
            {
                PostMessage(hWnd,WM_COMMAND,IDC_HF,0);
            }
		}
        break;

	case WM_CLOSE:
		SendMessage(hWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, ENDCALLICON),0);
		DestroyWindow(hWnd);
		break;
		
    case MB_ACTIVE:
        if (bExtension)
		{
			if (!GetFlagStar())
			{
				sprintf(cSendDTMFNum, pSendDTMFNum, TXT_SENDDTMF, cPhoneNumberExtension);
				WaitWindowStateEx(hWnd, TRUE, cSendDTMFNum, NULL, NULL, NULL);
				PostMessage(hWnd,IDC_ME_SUPERADDDIAL,0,0);		
			}
		}
        nUsedItemNum = GetUsedItemNum();
        if (1 == nUsedItemNum)
            CallingType = MBCALLINGONE;
        else if ((1 < nUsedItemNum) && (nUsedItemNum <= MAX_COUNT))
            CallingType = MBCALLINGMULT;
        else
            CallingType = MBCALLINNONE;
		if (CallingType == MBCALLINNONE) 
		{
			CloseSendDTMFWnd();
		}		
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
        printf("\nMB MB_ACTIVE,nUsedItemNum:%d\n",nUsedItemNum);
#endif
		SetMenuSoftkey(hWnd);
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
        printf("MB MB_ACTIVE,消息结束\r\n");
#endif
        break;
		
    case WM_ACTIVATE:
        if (WA_INACTIVE != LOWORD(wParam))
            SetFocus(hFocus);
		else
			hFocus = GetFocus();
        break;
		
    case WM_TIMER:
        switch (wParam)
        {
		case TIMER_ID_FRESH:
			{
				SYSTEMTIME	SystemTime;
				char aTimeBuf[25], aDateBuf[25];
				//update time
				if(IsTimeChanged())
				{
					GetLocalTime(&SystemTime);	
					GetTimeDisplay(SystemTime, aTimeBuf, aDateBuf);
					SetWindowText(hWnd, aTimeBuf);
				}
			}
			break;

		case IDC_END_TIMER:
			PostMessage(hWnd, WM_CLOSE, 0, 0);
			break;
			
        case IDC_TIMER_INIT:
            KillTimer(hWnd,IDC_TIMER_INIT);
            ME_GetACMmax(hWnd,IDC_ME_MAXPRICE);
            ME_GetSALSstatus(hWnd,IDC_GETLINESTATUS);
            break;
			
        case IDC_TIMER_CALLING:
            ChangeTimeOnTimer();
            break;
			
        case IDC_TIMER_COST:
            if (!bCanGetCurrentCCM)
                break;
            bCanGetCurrentCCM = FALSE;
            ME_GetCCM(hWnd,IDC_ME_GETCCM);
            break;

        case IDC_TIMER_ERROR:
            if (bListCurCalling)
                break;
            if (-1 == ME_ListCurCalls(hCalling,IDC_ME_LISTCURCALLS))
            {
                bListCurCalling  = FALSE;
            }
            else
            {
                bListCurCalling  = TRUE;
                ListCurCallsType = LISTCURCALLS_REPEAT;
            }
            break;

        case IDC_TIMER_TRANSFER:
            MBListCurCalls(LISTCURCALLS_TRANSFER,wParam,lParam);
            break;
        }
        break;
		
		case WM_DESTROY :
            WaitWindowStateEx(NULL, FALSE, PLEASEWAITING, NULL, NULL, NULL);
            if (hLineBmp != NULL)
            {
                DeleteObject(hLineBmp);
                hLineBmp = NULL;
            }
			if (hIncallGif != NULL)
			{
				EndAnimatedGIF(hIncallGif);
				hIncallGif = NULL;
			}
			if (hBitPortrait != NULL)
			{
				DeleteObject(hBitPortrait);
				hBitPortrait = NULL;
			}
			CALLWND_DeleteDataByHandle(pGlobalCallListBuf, hCalling);
			hCalling      = NULL;
			PhoneGroupNum = 0;
			CallingType   = MBCALLINNONE;
			bCostLimit    = FALSE;
            bSingal       = FALSE;
            bAutoHeadset  = FALSE;
			if (hBitmapCalling != NULL)
			{
				DeleteObject(hBitmapCalling);
				hBitmapCalling = NULL;
			}
			KillTimer(hWnd,IDC_TIMER_CALLING);
			KillTimer(hWnd, TIMER_ID_FRESH);
			UnregisterClass(pClassName,NULL);
//			DlmNotify((WPARAM)PES_STCQUIT, (LPARAM)hInstance);
			EnableState();
			PhoneSetChannelCalling(FALSE);
            f_sleep_unregister(CALL_DATA);//允许睡眠
            SendMessage(hWnd,PWM_SETAPPICON,
                MAKEWPARAM(IMAGE_ICON, ENDCALLICON),0);//通话结束后取消icon

			SetAnswerCallFromBT(FALSE);
			SetBTDialout(FALSE);
			break;
			
		case WM_PAINT :
			hdc = BeginPaint( hWnd, NULL);
			CallingDisplay(hdc);
			EndPaint(hWnd, NULL);
			break;
			
		case WM_TODESKTOP:
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
			printf("\nmb MBCalling WM_TODESKTOP,wParam:%ld\n",wParam);
#endif
			if (0 == wParam)
				return FALSE;
			if (!IsWindow_MBRing() &&   
				!IsWindow_DialLink() && 
				!IsWindow_Dialup())     
				
				ExecuteHangup();
			return FALSE;

		case WM_KEYDOWN:
			if (PM_GetkeyLockStatus() || PM_GetPhoneLockStatus())
				isLock = TRUE;
			else
				isLock = FALSE;
			if (isLock == TRUE) 
				return PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
			else
			{
				switch(LOWORD(wParam))
				{
				case VK_F8://上
                    if (iCurrentVolume < iMaxVolume)
                    {
                        bSetVolume = TRUE;
                        iCurrentVolume++;
                        ME_SetSpeakerVolume(hWnd,IDC_SETVOLUME,iCurrentVolume);
                        InvalidateRect(hWnd,&VolumeRect,TRUE);
                    }
					break;
				case VK_F7://下
                    if (iCurrentVolume > iMinVolume)
                    {
                        bSetVolume = TRUE;
                        iCurrentVolume--;
                        ME_SetSpeakerVolume(hWnd,IDC_SETVOLUME,iCurrentVolume);
                        InvalidateRect(hWnd,&VolumeRect,TRUE);
                    }
					break;
				case VK_0:
				case VK_1:  
				case VK_2:  
				case VK_3:  
				case VK_4:  
				case VK_5:  
				case VK_6:  
				case VK_7:  
				case VK_8:  
				case VK_9:  
					if (MBCALLINNONE == CallingType)
					{
						SendMessage(GetCapture(), WM_CANCELMODE, 0, 0);
						SendMessage(hWnd, WM_CLOSE, 0,0);
//						DestroyWindow(hWnd);
					}
					else
					{
						char chSend[2] = "";
						chSend[0] = (LOWORD(wParam) - VK_0 + 48);
						SuperAddDialNumber(hWnd, chSend,TRUE,TRUE, LOWORD(wParam));
					}
					break;
				case VK_F3: 
					if (MBCALLINNONE == CallingType)
						SendMessage(hWnd, WM_CLOSE, 0,0);
						//DestroyWindow(hWnd);
					else
						SuperAddDialNumber(hWnd, pAsterisk,TRUE,TRUE,LOWORD(wParam));
					break;
				case VK_F4: 
					if (MBCALLINNONE == CallingType)
						SendMessage(hWnd, WM_CLOSE, 0,0);
					//	DestroyWindow(hWnd);
					else
						SuperAddDialNumber(hWnd,pWell,TRUE,TRUE,LOWORD(wParam));
					break;
					
				case VK_F1:
					switch (CallingType)
					{
					case MBCALLINNONE:
						SendMessage(hWnd, WM_CLOSE, 0,0);
						//DestroyWindow(hWnd);
						break;
					case MBCALLINGONE:
						SendMessage(hWnd,WM_COMMAND,IDM_ONE_HOLD,0);
						break;
					case MBCALLINGMULT:
						SendMessage(hWnd,WM_COMMAND,IDM_SWITCH2,0);
						break;
					}
					break;
				default:
                    {
                        HMENU hMenu1 = PDAGetMenu(hWnd);
                        if (!IsMenu(hMenu1))
                        {
                            printf("\r\n mb mbincall menu is invalid.\r\n");
                            bErrorMenu = TRUE;
                            hMenu1 = CreateMenu();
                            PDASetMenu(hWnd, hMenu1);
                        }
                    }
					return PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
				}
			}
			break;
			
		case WM_COMMAND:
			switch( LOWORD( wParam ))
			{
            //BlueTooth begin
            case IDC_BLUETOOTH:
                if (BTAudioInterfaceType == ME_AUDIO_ANALOG)//当前是模拟通道,需要切换到数字通道
                {
                    BT_SwitchChannel(ME_AUDIO_DIGITAL);
                }
                else//当前是数字通道,需要切换到模拟通道
                {
                    BT_SwitchChannel(ME_AUDIO_ANALOG);
                }
                break;
            //BlueTooth end
			case IDM_SENDDTMF:
				CreateSendDTMFWindow(NULL);
				break;
			case IDC_END_OK:
				PostMessage(hWnd, WM_CLOSE, 0, 0);
				break;
//			case IDC_BUTTON_MIDDLE:
//				SuperAddDialNumber(hWnd, NULL,TRUE,TRUE);
//				break;
			case IDC_ROBUTTON:				
				ExecuteHangup();
				break;
			case IDM_HANDSET:
				bHF = FALSE;
                AdjustVolumeForHandFree(FALSE);
				SendMessage(hWnd, MB_ACTIVE, 0, 0);
				break;
			case IDM_HANDFREE:
				bHF = TRUE;
                AdjustVolumeForHandFree(TRUE);
				SendMessage(hWnd, MB_ACTIVE, 0, 0);
				break;
			case IDC_HF:
                if (BTAudioInterfaceType == ME_AUDIO_DIGITAL)
                    break;

				if (!bHF) 
				{
                    AdjustVolumeForHandFree(TRUE);
					SendMessage(hWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)TXT_HANDSET);
					bHF = TRUE;
				}
				else
				{					
                    AdjustVolumeForHandFree(FALSE);
					SendMessage(hWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_HANDSFREE);
					bHF = FALSE;
				}
				SendMessage(hWnd, MB_ACTIVE, 0, 0);
				break;
			case IDC_PICK:
				if (-1 == ME_SwitchCallStatus(hCalling,IDC_ONE_SWITCH,ACTIVE_SWITCH,0))
					AppMessageBox(hCalling,SWITCHFAIL,TITLECAPTION,WAITTIMEOUT);
				else
					WaitWindowStateEx(NULL, TRUE, PLEASEWAITING, NULL, NULL, NULL);
				break;
			case IDC_PICKCONFERENCE:
				if (-1 == ME_SwitchCallStatus(hCalling,IDC_MULTME_SWITCH2,ACTIVE_SWITCH,0))
					AppMessageBox(hCalling,SWITCHFAIL,TITLECAPTION,WAITTIMEOUT);
				else
					WaitWindowStateEx(NULL, TRUE, PLEASEWAITING, NULL, NULL, NULL);
				break;
			case IDC_BTN_SWAP:
				{
					nUsedItemNum = GetUsedItemNum();
					if (nUsedItemNum == 2)
						SendMessage(hWnd, WM_COMMAND, IDM_SWITCH1, 0);
					else
						SendMessage(hWnd, WM_COMMAND, IDM_SWITCH2, 0);
				}
				break;
			case IDC_BTN_ENDACTIVECALL:
				SendMessage(hWnd, WM_COMMAND, IDM_HANGUPACTIVE, 0);
				break;
			case IDM_MUTE:
				if (!bMute)
				{
					if (-1 == ME_MuteControl(hWnd,IDC_ME_MUTE,TRUE))
						AppMessageBox(hWnd,MUTEFAIL,TITLECAPTION,WAITTIMEOUT);
					else
						WaitWindowStateEx(NULL, TRUE, PLEASEWAITING, NULL, NULL, NULL);
				}
				else
				{
					if (-1 == ME_MuteControl(hWnd,IDC_ME_MUTE,FALSE))
						AppMessageBox(hWnd,UNMUTEFAIL,TITLECAPTION,WAITTIMEOUT);
					else
						WaitWindowStateEx(NULL, TRUE, PLEASEWAITING, NULL, NULL, NULL);
				}
				break;

			case IDM_ONE_HOLD:
				if (-1 == ME_SwitchCallStatus(hCalling,IDC_ONE_SWITCH,ACTIVE_SWITCH,0))
					AppMessageBox(hCalling,SWITCHFAIL,TITLECAPTION,WAITTIMEOUT);
				else
					WaitWindowStateEx(NULL, TRUE, PLEASEWAITING, NULL, NULL, NULL);
				break;
			case IDM_PICK:
				if (-1 == ME_SwitchCallStatus(hCalling,IDC_ONE_SWITCH,ACTIVE_SWITCH,0))
					AppMessageBox(hCalling,SWITCHFAIL,TITLECAPTION,WAITTIMEOUT);
				else
					WaitWindowStateEx(NULL, TRUE, PLEASEWAITING, NULL, NULL, NULL);
				break;
				
			case IDM_SWITCH1:
				if (-1 == ME_SwitchCallStatus(hCalling,IDC_MULTME_SWITCH1,ACTIVE_SWITCH,0))
					AppMessageBox(hCalling,SWITCHFAIL,TITLECAPTION,WAITTIMEOUT);
				else
					WaitWindowStateEx(NULL, TRUE, PLEASEWAITING, NULL, NULL, NULL);
				break;
				
			case IDM_SWITCH2:
				if (-1 == ME_SwitchCallStatus(hCalling,IDC_MULTME_SWITCH2,ACTIVE_SWITCH,0))
					AppMessageBox(hCalling,SWITCHFAIL,TITLECAPTION,WAITTIMEOUT);
				else
					WaitWindowStateEx(NULL, TRUE, PLEASEWAITING, NULL, NULL, NULL);
				break;
				
				
			case IDM_HANGUPACTIVE:
				if (-1 == ME_SwitchCallStatus(hCalling,IDC_ME_CUT_ACTIVED,CUT_ACTIVED,0))
					AppMessageBox(hCalling,HANGUPFAIL,TITLECAPTION,WAITTIMEOUT);
				break;
				
			case IDM_HANGUPHOLD:
				if (-1 == ME_SwitchCallStatus(hCalling,IDC_ME_CUT_HELD,CUT_HELD,0))
					AppMessageBox(hWnd,HANGUPFAIL,TITLECAPTION,WAITTIMEOUT);
				break;
				
			case IDM_ALLHANGUP:
				bToEndCall = TRUE;
				if (-1 == ME_SwitchCallStatus(hCalling,IDC_MULTME_ALLHANGUP,CUT_ALL,0))
					AppMessageBox(hWnd,HANGUPFAIL,TITLECAPTION,WAITTIMEOUT);
				break;
				
			case IDM_ALLACTIVE:
				if (-1 == ME_SwitchCallStatus(hCalling,IDC_MULTME_ALLACTIVE,ADD_TO_ACTIVE,0))
					AppMessageBox(hCalling,ACTIVEFAIL,TITLECAPTION,WAITTIMEOUT);
				break;
				
			case IDM_SHIFT:
				if (-1 == ME_SwitchCallStatus(hCalling,IDC_ME_CALL_TRANSFER,CALL_TRANSFER,0))
					AppMessageBox(hCalling,ME_APPLY_FAIL,TITLECAPTION,WAITTIMEOUT);
				break;

			case IDM_PICKUPCALL:
				{
					int iActiveNum, iHoldNum;
					iActiveNum  = GetItemTypeNum(CALL_STAT_ACTIVE);
					iHoldNum    = GetItemTypeNum(CALL_STAT_HELD);
					//if ((iActiveNum > 1) && (1 == iHoldNum))
					{
						if (-1 == ME_SwitchCallStatus(hCalling,IDC_MULTME_ALLACTIVE,ADD_TO_ACTIVE,0))
							AppMessageBox(hCalling,ACTIVEFAIL,TITLECAPTION,WAITTIMEOUT);
					}
//					else
//					{
//						PLXTipsWin(NULL, NULL, NULL, NOTIFY_UNSUCCESSFUL,"", Notify_Failure, ML("Ok"), NULL,WAITTIMEOUT);						
//					}
				}
				break;

			case IDM_SINGLECALL:
				{
					int iActiveNum, iHoldNum;
					iActiveNum  = GetItemTypeNum(CALL_STAT_ACTIVE);
					iHoldNum    = GetItemTypeNum(CALL_STAT_HELD);
					if (((iActiveNum > 1) && (0 == iHoldNum)) || ((0 == iActiveNum) && (iHoldNum>1)))
//					if ((iActiveNum > 1) && (0 == iHoldNum)) 
					{
						if(0 == iHoldNum) 
							bMemberIcon = TRUE;
						else
							bMemberIcon = FALSE;
						tp = EXTRACTMEMBER;
						CreateMemberWnd(hWnd, tp);
					}
					else
					{
						PLXTipsWin(NULL, NULL, NULL, NOTIFY_UNSUCCESSFUL,"", Notify_Failure, ML("Ok"), NULL,WAITTIMEOUT);						
					}
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
					printf("mb MBCalling 单独通话 界面完成\r\n");
#endif
				}
				break;
				
			case IDM_SINGLEHANGUP:
				{
					int iActiveNum, iHoldNum;
					iActiveNum  = GetItemTypeNum(CALL_STAT_ACTIVE);
					iHoldNum    = GetItemTypeNum(CALL_STAT_HELD);					
					if(0 == iHoldNum || (iActiveNum > 1 && iHoldNum == 1)) 
						bMemberIcon = TRUE;
					else
						bMemberIcon = FALSE;
					tp = DROPMEMBER;
					CreateMemberWnd(hWnd, tp);
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
				printf("mb MBCalling 单独挂断 界面完成\r\n");
#endif
				}
				break;

			case IDC_LBUTTON_NONE:
			case IDC_RBUTTON_BACK:
				SendMessage(hWnd, WM_CLOSE, 0,0);
				//DestroyWindow(hWnd);
				break;
        }
        break;
		
	case IDC_EXTRACTMEMBER:
		{	
			int iActiveNum, iHoldNum;
			iActiveNum  = GetItemTypeNum(CALL_STAT_ACTIVE);
			iHoldNum    = GetItemTypeNum(CALL_STAT_HELD);
			if ((iActiveNum > 1) && (0 == iHoldNum)) 
			{
				
				iItemData = lParam;			
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
				printf("mb MBCalling 单独通话 iItemData:%d\r\n",iItemData);
#endif
				if ((iItemData < 0) || (iItemData > MAX_COUNT - 1))
					iItemData = 0;
				if (-1 == ME_SwitchCallStatus(hCalling,IDC_ME_ACTIVE_SWITCH_X,
					ACTIVE_SWITCH_X,PhoneItem[iItemData].Calls_Info.index))
					AppMessageBox(hWnd,ME_APPLY_FAIL,TITLECAPTION,WAITTIMEOUT);		
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
				printf("mb MBCalling 单独通话 iItemData:%d,index:%d\r\n",
					iItemData,PhoneItem[iItemData].Calls_Info.index);
#endif
			}
			if ((0 == iActiveNum) && (iHoldNum>1))
			{
				iItemData = lParam;			
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
				printf("mb MBCalling 单独通话 iItemData:%d\r\n",iItemData);
#endif
				if ((iItemData < 0) || (iItemData > MAX_COUNT - 1))
					iItemData = 0;
				if (-1 == ME_SwitchCallStatus(hCalling,WM_ME_EXTRACTSWITCH, ACTIVE_SWITCH,0))
					AppMessageBox(hWnd,ME_APPLY_FAIL,TITLECAPTION,WAITTIMEOUT);		
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
				printf("mb MBCalling 单独通话 iItemData:%d,index:%d\r\n",
					iItemData,PhoneItem[iItemData].Calls_Info.index);
#endif
			}
		}
		break;

	case WM_ME_EXTRACTSWITCH:
		switch (wParam)
		{
		case ME_RS_SUCCESS:
//			MBListCurCalls(LISTCURCALLS_SWITCH,wParam,lParam);
			if (-1 == ME_SwitchCallStatus(hCalling,IDC_ME_ACTIVE_SWITCH_X,
				ACTIVE_SWITCH_X,PhoneItem[iItemData].Calls_Info.index))
				AppMessageBox(hWnd,ME_APPLY_FAIL,TITLECAPTION,WAITTIMEOUT);		
			break;
		default:
			SendAbortCommand(wParam,lParam);
			AppMessageBox(hWnd,ME_APPLY_FAIL,TITLECAPTION,WAITTIMEOUT);
			break;
		}
		break;
		
	case IDC_DROPMEMBER:
		
		iItemData = lParam;
		
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
		printf("mb MBCalling 单独挂断 iItemData:%d\r\n",iItemData);
#endif
		if ((iItemData < 0) || (iItemData > MAX_COUNT - 1))
			iItemData = 0;
		if (-1 == ME_SwitchCallStatus(hWnd,IDC_ME_CUT_ACTIVED_X,
			CUT_ACTIVED_X,PhoneItem[iItemData].Calls_Info.index))
			AppMessageBox(hWnd,HANGUPFAIL,TITLECAPTION,WAITTIMEOUT);		
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
		printf("mb MBCalling 单独挂断 iItemData:%d,index:%d\r\n",
			iItemData,PhoneItem[iItemData].Calls_Info.index);
#endif
		break;

	case IDC_ME_MAXPRICE:
        switch (wParam)
        {
        case ME_RS_SUCCESS:
            ME_GetResult(&dAcmMax,sizeof(DWORD));
            if ((dAcmMax > 0) && (dAcmMax < 0xffffff))
            {
                if (-1 == ME_GetPricePerUnit(hWnd,IDC_ME_GETPRICEUNIT))
                    bAOCInfo = FALSE;
            }
			else
				bAOCInfo = FALSE;
            break;
        default:
            bAOCInfo = FALSE;
            break;
        }
        break;
		
	case IDC_ME_GETPRICEUNIT:
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
        printf("mb MBCalling IDC_ME_GETPRICEUNIT wParam:%ld,lParam:%ld\r\n",wParam,lParam);
#endif
		switch (wParam)
		{
		case ME_RS_SUCCESS:
			GetPriceUnit();
			break;
		default:
			bAOCInfo = FALSE;
			break;
		}
		break;
			
	case IDC_ME_GETCCM:
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
        printf("mb MBCalling IDC_ME_GETCCM wParam:%ld,lParam:%ld\r\n",wParam,lParam);
#endif
		switch (wParam)
		{
		case ME_RS_SUCCESS:
			if (-1 == ME_GetResult(&dCostInfo,sizeof(DWORD)))
			{
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
				printf("mb MBCalling IDC_ME_GETCCM ME_GetResult==-1\r\n");
#endif
			}
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
			printf("mb MBCalling dCostInfo:%ld\r\n",dCostInfo);
#endif
			bCanGetCurrentCCM = TRUE;
			InvalidateRect(hWnd,&rCostInfo,TRUE);
			break;
		default:
			SendAbortCommand(wParam,lParam);
			dCostInfo = 0;
			break;
		}        
		break;
				
	case IDC_ME_SUPERADDDIAL:
		if ((cPhoneNumberExtension[iExtension] != 0) && 
			(!GetListCurCalling()) &&                   
			(!IsAllType_Specific(CALL_STAT_HELD)))      
			ME_SendDTMF(hWnd,IDC_ME_SUPERADDDIAL,cPhoneNumberExtension[iExtension++]);
		else
		{
			WaitWindowStateEx(hWnd, FALSE, cSendDTMFNum, NULL, NULL, NULL);
			bExtension = FALSE;
		}
		break;
		
	case IDC_ME_MUTE:
		WaitWindowStateEx(NULL, FALSE, PLEASEWAITING, NULL, NULL, NULL);
		switch (wParam)
		{
		case ME_RS_SUCCESS:
			bMute = !bMute;
			hMenu = PDAGetMenu(hWnd);
			ModifyMenu(hMenu,IDM_MUTE,MF_BYCOMMAND|MF_ENABLED,IDM_MUTE,
				bMute ? TXT_UNMUTE : TXT_MUTE);
			
			InvalidateRect(hWnd,NULL,TRUE);
			break;
		default:
			SendAbortCommand(wParam,lParam);
			AppMessageBox(hWnd,bMute ? UNMUTEFAIL : MUTEFAIL,TITLECAPTION,WAITTIMEOUT);
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
			printf("mb MBCalling 执行静音失败,bMute:%d,wParam:%ld,lParam:%ld\r\n",
				bMute,wParam,lParam);
#endif
			break;
		}
		break;
					
	case IDC_ME_LISTCURCALLS:
//		MBWaitWindowState(FALSE);
		bListCurCalling = FALSE;
		switch (wParam)
		{
		case ME_RS_SUCCESS:
			if (OperateListCurCalls())
				SendMessage(hWnd,MB_ACTIVE,0,0);
			
			break;
		default:
            if (CallingType == MBCALLINGONE)
            {
                if (strcmp(PhoneItem[iCallingOneNum].cName,STATUS_INFO_EMERGENTCALL) == 0)
                {
                    AfterHangup_Single();
                    NotifyRingWndUpdate();
                    break;
                }
            }
			SendAbortCommand(wParam,lParam);
			AppMessageBox(hCalling,ME_APPLY_FAIL,TITLECAPTION,WAITTIMEOUT);
			break;
		}
		break;
												
	case IDC_ME_SHORTCUTSWITCH:
		switch (wParam)
		{
		case ME_RS_SUCCESS:
			MBListCurCalls(LISTCURCALLS_SWITCH,wParam,lParam);
			break;
		default:
			SendAbortCommand(wParam,lParam);
			AppMessageBox(hWnd,ME_APPLY_FAIL,TITLECAPTION,WAITTIMEOUT);
			break;
		}
		break;
		
	case IDC_ONE_SWITCH:
		WaitWindowStateEx(NULL, FALSE, PLEASEWAITING, NULL, NULL, NULL);
		switch (wParam)
		{
		case ME_RS_SUCCESS:								
			InvertItemStateOne(iCallingOneNum);
			SendMessage(hWnd, MB_ACTIVE, 0, 0);
			break;
		default:
			SendAbortCommand(wParam,lParam);
			AppMessageBox(hWnd,SWITCHFAIL,TITLECAPTION,WAITTIMEOUT);
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
			printf("mb MBCalling 单方通话切换失败,bHolding:%d,wParam:%ld,lParam:%ld\r\n",
				bHolding,wParam,lParam);
#endif
			break;
		}
		break;
								
	case IDC_MULTME_SWITCH1:
		WaitWindowStateEx(NULL, FALSE, PLEASEWAITING, NULL, NULL, NULL);
		switch (wParam)
		{
		case ME_RS_SUCCESS:
			InvertItemStateMult();
			SendMessage(hWnd,MB_ACTIVE,0,0);
			break;
		default:
			SendAbortCommand(wParam,lParam);
			AppMessageBox(hWnd,SWITCHFAIL,TITLECAPTION,WAITTIMEOUT);
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
			printf("mb MBCalling 多方通话切换失败,wParam:%ld,lParam:%ld\r\n",wParam,lParam);
#endif
			break;
		}
		break;
									
	case IDC_MULTME_SWITCH2:
		WaitWindowStateEx(NULL, FALSE, PLEASEWAITING, NULL, NULL, NULL);
		switch (wParam)
		{
		case ME_RS_SUCCESS:
			InvertItemStateMult();
			SendMessage(hWnd,MB_ACTIVE,0,0);
			break;
		default:
			SendAbortCommand(wParam,lParam);
			AppMessageBox(hWnd,SWITCHFAIL,TITLECAPTION,WAITTIMEOUT);
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
			printf("mb MBCalling 多方通话切换失败,wParam:%ld,lParam:%ld\r\n",wParam,lParam);
#endif
			break;
		}
		break;
												
	case WM_VOICE_HANGUPONE:
        BT_Hangup();//蓝牙挂断
        if (bTransfer)
            break;
		MBListCurCalls(LISTCURCALLS_HANGUP_ONE,wParam,lParam);
		break;
	case IDC_MULTME_ALLHANGUP:
		MBListCurCalls(LISTCURCALLS_HANGUP_ALL,wParam,lParam);
		break;
	case IDC_ME_CUT_HELD:
		MBListCurCalls(LISTCURCALLS_HANGUP_HELD,wParam,lParam);
		break;
	case IDC_ME_CUT_ACTIVED:
		MBListCurCalls(LISTCURCALLS_HANGUP_ACTIVE,wParam,lParam);
		break;
	case IDC_ME_CUT_ACTIVED_X:
		MBListCurCalls(LISTCURCALLS_HANGUP_CUT_X,wParam,lParam);
		break;
		
	case IDC_MULTME_ALLACTIVE:
		switch (wParam)
		{
		case ME_RS_SUCCESS:
		PhoneGroupNum = 1;
		SetAllType(CALL_STAT_ACTIVE);
		SendMessage(hWnd,MB_ACTIVE,0,0);
		break;
		default:
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
		printf("mb MBCalling 方通话的全部激活失败,wParam:%ld,lParam:%ld\r\n",wParam,lParam);
#endif
		SendAbortCommand(wParam,lParam);
		AppMessageBox(hWnd,ACTIVEFAIL,TITLECAPTION,WAITTIMEOUT);
		break;
		}
		break;
		
	case IDC_ME_ACTIVE_SWITCH_X:
		switch (wParam)
		{
		case ME_RS_SUCCESS:
			MBListCurCalls(LISTCURCALLS_ACTIVE_X,wParam,lParam);
			break;
		default:
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
			printf("mb MBCalling 将除指定序号外的所有通话变为保持失败,wParam:%ld,lParam:%ld\r\n",wParam,lParam);
#endif
			SendAbortCommand(wParam,lParam);
			AppMessageBox(NULL,ME_APPLY_FAIL,TITLECAPTION,WAITTIMEOUT);
			break;
		}
		break;
		
	case IDC_ME_CALL_TRANSFER:
		switch (wParam)
		{
		case ME_RS_SUCCESS:
			//MBListCurCalls(LISTCURCALLS_TRANSFER,wParam,lParam);
            bTransfer = TRUE;
            SetTimer(hWnd,IDC_TIMER_TRANSFER,3000,NULL);
            //ShowWindow(hWnd,SW_HIDE);
            //UpdateWindow(hWnd);
            PLXTipsWin(NULL, NULL, NULL, TXT_TRANSFER, TITLECAPTION,
                Notify_Info, NULL, NULL, 30);
			break;
		default:
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
			printf("mb MBCalling 转移失败wParam:%ld,lParam:%ld\r\n",wParam,lParam);
#endif
			SendAbortCommand(wParam,lParam);
			AppMessageBox(NULL,ME_APPLY_FAIL,TITLECAPTION,WAITTIMEOUT);
			break;
		}
		break;
		
	case IDC_ME_REPEAT:
		MBListCurCalls(LISTCURCALLS_REPEAT,wParam,lParam);
		break;
		
    case IDC_ME_SHORTOTHER:
		switch (wParam)
		{
		case ME_RS_SUCCESS:
			break;
		default:
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
			printf("mb MBCalling IDC_ME_SHORTOTHER失败wParam:%ld,lParam:%ld\r\n",wParam,lParam);
#endif
			SendAbortCommand(wParam,lParam);
			AppMessageBox(NULL,ME_APPLY_FAIL,TITLECAPTION,WAITTIMEOUT);
			break;
		}
		break;
		
    case IDC_GETLINESTATUS:
        switch (wParam)
        {
        case ME_RS_SUCCESS:
            {
                COLORREF Color;
                BOOL     bTran;

                if (hLineBmp != NULL)
                {
                    DeleteObject(hLineBmp);
                    hLineBmp = NULL;
                }
                hdc = GetDC(hWnd);
                ME_GetResult(&iLineStatus,sizeof(int));
                GetImageDimensionFromFile((iLineStatus == 1) ? LINE1BMP : LINE2BMP,
                    &LinePicSize);
                hLineBmp = CreateBitmapFromImageFile(hdc,(iLineStatus == 1) ? LINE1BMP : LINE2BMP,
                    &Color,&bTran);
                ReleaseDC(hWnd,hdc);
                InvalidateRect(hWnd,NULL,TRUE);
            }
            break;
        default:
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
            printf("mb MBCalling IDC_GETLINESTATUS 失败wParam:%ld,lParam:%ld\r\n",wParam,lParam);
#endif
            break;
        }
        break;

    case IDC_SETVOLUME:
        switch (wParam)
        {
        case ME_RS_SUCCESS:
            break;
        default:
            AppMessageBox(NULL,ME_APPLY_FAIL,TITLECAPTION,WAITTIMEOUT);
            break;
        }
        break;

	default :
		lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
		break;
    }
    return lResult;
}

void    InitMBCalling(void)
{
    int iFontHeight,iSW;
	
    iFontHeight = GetCharactorHeight(NULL);
    
    iSW = GetScreenUsableWH1(SM_CXSCREEN);
	
    SetRect(&rPhoto,iPortraitX,iPortraitY,
        iPortraitX + iPortraitW,iPortraitY + iPortraitH);
	
    SetRect(&rName      ,rPhoto.right,rPhoto.top,
        iSW,rPhoto.top + iFontHeight * 2);
    SetRect(&rPhoneCode ,rPhoto.right,rName.bottom + iFontHeight ,
        iSW,rName.bottom + iFontHeight * 2);
    SetRect(&rTime      ,rPhoto.left ,rPhoto.bottom + iFontHeight,
        iSW,rPhoto.bottom + iFontHeight * 2);
    SetRect(&rCostInfo  ,rPhoto.left ,rTime.bottom + iFontHeight ,
        iSW,rTime.bottom + iFontHeight * 2);
}

static  void    GetPriceUnit(void)
{
    if (-1 == ME_GetResult(&me_pucinfo,sizeof(ME_PUCINFO)))
    {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
        printf("mb MBCalling IDC_ME_GETPRICEUNIT ME_GetResult==-1\r\n");
#endif
        bAOCInfo = FALSE;
    }
    else
    {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
        printf("mb MBCalling,me_pucinfo.currency:%s,me_pucinfo.ppu:%s\r\n",
            me_pucinfo.currency,me_pucinfo.ppu);
#endif
        RemoveStringEndSpace(me_pucinfo.currency);
        RemoveStringEndSpace(me_pucinfo.ppu);
        if ((strlen(me_pucinfo.currency) == 0)/* && (strlen(me_pucinfo.ppu) == 0)*/)
        {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
            printf("mb MBCalling,没有设置计价信息\r\n");
#endif
            bAOCInfo = FALSE;
        }
        else
        {
            bAOCInfo          = TRUE;
            bCanGetCurrentCCM = TRUE;
            SetTimer(hCalling,IDC_TIMER_COST,TIMER_CALLING_COST,NULL);
            if (strlen(me_pucinfo.currency) > 0)
            {
                CostType = COSTTYPE_PRICE;
                if (StrIsDouble(me_pucinfo.ppu))
                    dCost = MB_atof(me_pucinfo.ppu);
                else
                    dCost = dZero;
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
                printf("mb MBCalling,设置了计价信息,%f\r\n",dCost);
#endif
            }
            else
            {
                CostType = COSTTYPE_UNIT;
                dCost    = dZero;
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
                printf("mb MBCalling,从ME_GetCCM函数读取\r\n");
#endif
            }
        }
    }    
}

static  void    ChangeTimeOnTimer(void)
{
    int i,iTimeMargin;
    SYSTEMTIME sCurtime;
	
    GetLocalTime(&sCurtime);
    if (sCurtime.wSecond == iInitSecond)
        return;
	
    if (sCurtime.wSecond > iInitSecond)
        iTimeMargin = sCurtime.wSecond - iInitSecond;
    else
        iTimeMargin = iSecond60 - iInitSecond + sCurtime.wSecond;
	
    iInitSecond = sCurtime.wSecond;
	
    switch (CallingType)
    {
    case MBCALLINGONE:
        PhoneItem[iCallingOneNum].TimeUsed.wSecond += iTimeMargin;
        if (PhoneItem[iCallingOneNum].TimeUsed.wSecond >= iSecond60)
        {
            PhoneItem[iCallingOneNum].TimeUsed.wSecond = 0;
            PhoneItem[iCallingOneNum].TimeUsed.wMinute++;
            if (PhoneItem[iCallingOneNum].TimeUsed.wMinute >= iSecond60)
            {
                PhoneItem[iCallingOneNum].TimeUsed.wHour++;
                PhoneItem[iCallingOneNum].TimeUsed.wMinute = 0;
            }
        }
        InvalidateRect(hCalling,&rTime ,TRUE);
        InvalidateRect(hCalling,&(incallif_initpaint.line3),TRUE);
		InvalidateRect(hCalling,&(incallif_initpaint.line4),TRUE);
        break;
    case MBCALLINGMULT:
        for (i = 0;i < MAX_COUNT;i++)
        {
            if (ITEMUSED == PhoneItem[i].ItemStat)
            {
                PhoneItem[i].TimeUsed.wSecond += iTimeMargin;
                if (PhoneItem[i].TimeUsed.wSecond >= iSecond60)
                {
                    PhoneItem[i].TimeUsed.wSecond = 0;
                    PhoneItem[i].TimeUsed.wMinute++;
                    if (PhoneItem[i].TimeUsed.wMinute >= iSecond60)
                    {
                        PhoneItem[i].TimeUsed.wHour++;
                        PhoneItem[i].TimeUsed.wMinute = 0;
                    }
                }
            }
        }
        switch (InterfaceType)
        {
        case INTERFACE_MULT1:
        case INTERFACE_MULT31:
        case INTERFACE_MULT32:
        case INTERFACE_MULT5:
        case INTERFACE_MULT6:
			
            break;
        case INTERFACE_MULT21:
        case INTERFACE_MULT22:
            break;
        }
        break;
    }
}

static  BOOL    SetMenuSoftkey(HWND hWnd)
{
    int         iActiveNum,iHoldNum;
    HDC         hdc;
	COLORREF        Color;
	BOOL            bTran;
	int iSour, i;
	
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
    printf("mb MBCalling SetMenuSoftkey:%d\r\n",CallingType);
#endif
    switch (CallingType)
    {
    case MBCALLINNONE:
        SetTimer(hWnd,IDC_END_TIMER,END_TIMER_DURATION,0);
		return TRUE;
		
    case MBCALLINGONE:
		closeMemberWnd();
		if (bHolding) 
			SendMessage(hWnd,PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_PICK,1),(LPARAM)TXT_RESTORE);
		else
        {
            if (BTAudioInterfaceType == ME_AUDIO_DIGITAL)
            {
                SendMessage(hWnd,PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_HF,1),(LPARAM)"");
            }
            else
            {
                if (bHF)
                    SendMessage(hWnd,PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_HF,1),(LPARAM)TXT_HANDSET);
                else
                    SendMessage(hWnd,PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_HF,1),(LPARAM)IDS_HANDSFREE);
            }
        }
        if ((BT_EquipmentConnected() == BT_CONNECTED) || 
			(BT_GetStatus() == BT_AUDIO_CONNECTED))//蓝牙设备已经连接
        {
			if (AnswerCallFromBT())
			{
				printf("\r\n AnswerCallFromBT\r\n");
				SendMessage(hWnd,PWM_SETBUTTONTEXT,0,(LPARAM)"");
                SendMessage(hWnd,PWM_CREATECAPTIONBUTTON, 
                    MAKEWPARAM(IDC_BLUETOOTH,0),(LPARAM)BT_SWITCHTOANALOG);
			}
			else
			{
				printf("\r\n Set button BlueTooth\r\n");
				if (GetBTDialout())
				{
					SendMessage(hWnd,PWM_CREATECAPTIONBUTTON, 
						MAKEWPARAM(IDC_BLUETOOTH,0),(LPARAM)BT_SWITCHTOANALOG);
				}
				else
				{
					SendMessage(hWnd,PWM_CREATECAPTIONBUTTON, 
						MAKEWPARAM(IDC_BLUETOOTH,0),(LPARAM)BT_SWITCHTODIGITAL);
				}
			}
        }
        else
        {
            SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,IDC_RBUTTON_BACK,(LPARAM)"");
        }
        iCallingOneNum = GetTheNUsedItem(1);
		if (-1 == iCallingOneNum)
			break;
		SetMBCallingMenu(hWnd, INTERFACE_ONE);
		SendMessage(hWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Options"); 
        memset(&abinfo,0x00,sizeof(ABINFO));

        if (strcmp(PhoneItem[iCallingOneNum].cName,STATUS_INFO_EMERGENTCALL) != 0)
        {
            if (MB_GetInfoFromPhonebook(PhoneItem[iCallingOneNum].Calls_Info.PhoneNum,&abinfo))
            {
                iSour = strlen(abinfo.szName);
                strncpy(PhoneItem[iCallingOneNum].cName,abinfo.szName,sizeof(PhoneItem[iCallingOneNum].cName) > iSour ? iSour + 1:sizeof(PhoneItem[iCallingOneNum].cName) - 1);
            }
            else
            {
                strcpy(PhoneItem[iCallingOneNum].cName, "");
            }
        }
        if (hIncallGif != NULL)
        {
            EndAnimatedGIF(hIncallGif);
            hIncallGif = NULL;
        }
        if (hBitPortrait != NULL)
        {
            DeleteObject(hBitPortrait);
            hBitPortrait = NULL;
        }
		bSetPortrait = FileIfExist(abinfo.szIcon);
		bIcon =  MBCongig_Calling.ALS;
        InitPaint(&incallif_initpaint, bSetPortrait, bIcon);
		hdc = GetDC(hWnd);
		if (CALL_STAT_ACTIVE == PhoneItem[iCallingOneNum].Calls_Info.Stat) {
			GetImageDimensionFromFile(MBPIC_INCALL,&sCartoon);
			hIncallGif = StartAnimatedGIFFromFile(hWnd,MBPIC_INCALL,
				incallif_initpaint.subcapmid.left,
				incallif_initpaint.subcapmid.top, DM_NONE);
		}
		if (CALL_STAT_HELD == PhoneItem[iCallingOneNum].Calls_Info.Stat) {
			
			GetImageDimensionFromFile(MBPIC_INCALLONHOLD,&sCartoon);
			hIncallGif = StartAnimatedGIFFromFile(hWnd,MBPIC_INCALLONHOLD,
				incallif_initpaint.subcapmid.left,
				incallif_initpaint.subcapmid.top, DM_NONE);
		}
		
		if (bSetPortrait) {
            GetImageDimensionFromFile(abinfo.szIcon,&sPortrait);
			InitPhoto(&incallif_initpaint, &sPortrait);
            hBitPortrait = CreateBitmapFromImageFile(hdc,abinfo.szIcon,&Color,&bTran);
		}
		ReleaseDC(hWnd,hdc);		
		hFocus = hWnd;
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
        printf("mb MBCalling 单方通话生成号码:%s,原号码:%s\r\n",cPhone,PhoneItem[iCallingOneNum].Calls_Info.PhoneNum);
#endif
        break;
		
    case MBCALLINGMULT:
		closeMemberWnd();
        iActiveNum  = GetItemTypeNum(CALL_STAT_ACTIVE);
        iHoldNum    = GetItemTypeNum(CALL_STAT_HELD);
		
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
        printf("mb MBCalling 多方通话,iActiveNum:%d,iHoldNum:%d\r\n",iActiveNum,iHoldNum);
#endif
        if ((1 == iActiveNum) && (1 == iHoldNum))
        {
			
			SendMessage(hWnd,PWM_CREATECAPTIONBUTTON, (WPARAM)IDC_BTN_ENDACTIVECALL,(LPARAM)TXT_ENDCALL);
			SendMessage(hWnd,PWM_CREATECAPTIONBUTTON, (WPARAM)MAKEWPARAM(IDC_BTN_SWAP,1),(LPARAM)TXT_SWITCH);
			SetMBCallingMenu(hWnd, INTERFACE_MULT1);
			SendMessage(hWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Options"); 
			for (i = 0;i < MAX_COUNT;i++)
			{
				if (ITEMUSED == PhoneItem[i].ItemStat)
				{
					if (CALL_STAT_ACTIVE == PhoneItem[i].Calls_Info.Stat)
					{
						if (MB_GetInfoFromPhonebook(PhoneItem[i].Calls_Info.PhoneNum,&abinfo))
						{
							iSour = strlen(abinfo.szName);
							strncpy(PhoneItem[i].cName,abinfo.szName,sizeof(PhoneItem[iCallingOneNum].cName) > iSour ? iSour + 1:sizeof(PhoneItem[iCallingOneNum].cName) - 1);
							
						}
						else 
							strcpy(PhoneItem[i].cName, "");
					}
					else if (CALL_STAT_HELD == PhoneItem[i].Calls_Info.Stat)
					{
						if (MB_GetInfoFromPhonebook(PhoneItem[i].Calls_Info.PhoneNum,&abinfo))
						{
							iSour = strlen(abinfo.szName);
							strncpy(PhoneItem[i].cName,abinfo.szName,sizeof(PhoneItem[iCallingOneNum].cName) > iSour ? iSour + 1:sizeof(PhoneItem[iCallingOneNum].cName) - 1);
							
						}
						else 
							strcpy(PhoneItem[i].cName, "");
					}
				}
			}    
			if (hIncallGif != NULL)
			{
				EndAnimatedGIF(hIncallGif);
				hIncallGif = NULL;
			}
			if (hBitPortrait != NULL)
			{
				DeleteObject(hBitPortrait);
				hBitPortrait = NULL;
			}
			if (MBCongig_Calling.ALS)
				bIcon = TRUE;
			else
				bIcon = FALSE;
			InitPaint(&incallif_initpaint, FALSE, bIcon);
			hdc = GetDC(hWnd);
			GetImageDimensionFromFile(MBPIC_INCALLTWO,&sCartoon);
			hIncallGif = StartAnimatedGIFFromFile(hWnd,MBPIC_INCALLTWO,
				incallif_initpaint.subcapmid.left,
				incallif_initpaint.subcapmid.top, DM_NONE);
			ReleaseDC(hWnd,hdc);				
        }
        else if ((iActiveNum > 1) && (0 == iHoldNum))
        {
            if (BTAudioInterfaceType == ME_AUDIO_DIGITAL)
            {
                SendMessage(hWnd,PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_HF,1),(LPARAM)"");
            }
			else
            {
                if (bHF)
                    SendMessage(hWnd,PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_HF,1),(LPARAM)TXT_HANDSET);
                else
                    SendMessage(hWnd,PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_HF,1),(LPARAM)IDS_HANDSFREE);
            }
			SendMessage(hWnd,PWM_SETBUTTONTEXT,0,(LPARAM)"");
            SetMBCallingMenu(hWnd, INTERFACE_MULT21);
			SendMessage(hWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Options"); 
			if (hIncallGif != NULL)
			{
				EndAnimatedGIF(hIncallGif);
				hIncallGif = NULL;
			}
			if (hBitPortrait != NULL)
			{
				DeleteObject(hBitPortrait);
				hBitPortrait = NULL;
			}
			if (MBCongig_Calling.ALS)
				bIcon = TRUE;
			else
				bIcon = FALSE;
			InitPaint(&incallif_initpaint, bSetPortrait, bIcon);
			hdc = GetDC(hWnd);
			GetImageDimensionFromFile(MBPIC_INCALL,&sCartoon);
			hIncallGif = StartAnimatedGIFFromFile(hWnd,MBPIC_INCALL,
				incallif_initpaint.subcapmid.left,
				incallif_initpaint.subcapmid.top, DM_NONE);
			ReleaseDC(hWnd,hdc);					
			
			
        }
        else if ((0 == iActiveNum) && (iHoldNum > 1))
        {
			SendMessage(hWnd,PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_PICKCONFERENCE,1),(LPARAM)TXT_RESTORE);
			SendMessage(hWnd,PWM_SETBUTTONTEXT,0,(LPARAM)"");
            SetMBCallingMenu(hWnd, INTERFACE_MULT22);
			SendMessage(hWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Options"); 
			if (hIncallGif != NULL)
			{
				EndAnimatedGIF(hIncallGif);
				hIncallGif = NULL;
			}
			if (hBitPortrait != NULL)
			{
				DeleteObject(hBitPortrait);
				hBitPortrait = NULL;
			}
			if (MBCongig_Calling.ALS)
				bIcon = TRUE;
			else
				bIcon = FALSE;
			InitPaint(&incallif_initpaint, bSetPortrait, bIcon);
			hdc = GetDC(hWnd);
			GetImageDimensionFromFile(MBPIC_INCALLONHOLD,&sCartoon);
			hIncallGif = StartAnimatedGIFFromFile(hWnd,MBPIC_INCALLONHOLD,
				incallif_initpaint.subcapmid.left,
				incallif_initpaint.subcapmid.top, DM_NONE);
			ReleaseDC(hWnd,hdc);					
			
        }
        else if ((1 == iActiveNum) && (iHoldNum > 1))
        {
			SendMessage(hWnd,PWM_CREATECAPTIONBUTTON, (WPARAM)IDC_BTN_ENDACTIVECALL,(LPARAM)TXT_ENDCALL);
			SendMessage(hWnd,PWM_CREATECAPTIONBUTTON, (WPARAM)MAKEWPARAM(IDC_BTN_SWAP,1),(LPARAM)TXT_SWITCH);
            SetMBCallingMenu(hWnd, INTERFACE_MULT31);
			SendMessage(hWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Options"); 
			for (i = 0;i < MAX_COUNT;i++)
			{
				if (ITEMUSED == PhoneItem[i].ItemStat)
				{
					if (CALL_STAT_ACTIVE == PhoneItem[i].Calls_Info.Stat)
					{
						if (MB_GetInfoFromPhonebook(PhoneItem[i].Calls_Info.PhoneNum,&abinfo))
						{
							iSour = strlen(abinfo.szName);
							strncpy(PhoneItem[i].cName,abinfo.szName,sizeof(PhoneItem[iCallingOneNum].cName) > iSour ? iSour + 1:sizeof(PhoneItem[iCallingOneNum].cName) - 1);
							
						}
						else 
							strcpy(PhoneItem[i].cName, "");
					}
					else if (CALL_STAT_HELD == PhoneItem[i].Calls_Info.Stat)
					{
						if (MB_GetInfoFromPhonebook(PhoneItem[i].Calls_Info.PhoneNum,&abinfo))
						{
							iSour = strlen(abinfo.szName);
							strncpy(PhoneItem[i].cName,abinfo.szName,sizeof(PhoneItem[iCallingOneNum].cName) > iSour ? iSour + 1:sizeof(PhoneItem[iCallingOneNum].cName) - 1);
							
						}
						else 
							strcpy(PhoneItem[i].cName, "");
					}
				}
			}    
			
			if (hIncallGif != NULL)
			{
				EndAnimatedGIF(hIncallGif);
				hIncallGif = NULL;
			}
			if (hBitPortrait != NULL)
			{
				DeleteObject(hBitPortrait);
				hBitPortrait = NULL;
			}
			if (MBCongig_Calling.ALS)
				bIcon = TRUE;
			else
				bIcon = FALSE;
			InitPaint(&incallif_initpaint, bSetPortrait, bIcon);
			hdc = GetDC(hWnd);
			GetImageDimensionFromFile(MBPIC_INCALLTWO,&sCartoon);
			hIncallGif = StartAnimatedGIFFromFile(hWnd,MBPIC_INCALLTWO,
				incallif_initpaint.subcapmid.left,
				incallif_initpaint.subcapmid.top, DM_NONE);
			ReleaseDC(hWnd,hdc);			
			
        }
        else if ((iActiveNum > 1) && (1 == iHoldNum))
        {
			SendMessage(hWnd,PWM_CREATECAPTIONBUTTON, (WPARAM)IDC_BTN_ENDACTIVECALL,(LPARAM)TXT_ENDCALL);
			SendMessage(hWnd,PWM_CREATECAPTIONBUTTON, (WPARAM)MAKEWPARAM(IDC_BTN_SWAP,1),(LPARAM)TXT_SWITCH);
            SetMBCallingMenu(hWnd, INTERFACE_MULT32);
			SendMessage(hWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Options"); 
			for (i = 0;i < MAX_COUNT;i++)
			{
				if (ITEMUSED == PhoneItem[i].ItemStat)
				{
					if (CALL_STAT_ACTIVE == PhoneItem[i].Calls_Info.Stat)
					{
						if (MB_GetInfoFromPhonebook(PhoneItem[i].Calls_Info.PhoneNum,&abinfo))
						{
							iSour = strlen(abinfo.szName);
							strncpy(PhoneItem[i].cName,abinfo.szName,sizeof(PhoneItem[iCallingOneNum].cName) > iSour ? iSour + 1:sizeof(PhoneItem[iCallingOneNum].cName) - 1);
							
						}
						else 
							strcpy(PhoneItem[i].cName, "");
					}
					else if (CALL_STAT_HELD == PhoneItem[i].Calls_Info.Stat)
					{
						if (MB_GetInfoFromPhonebook(PhoneItem[i].Calls_Info.PhoneNum,&abinfo))
						{
							iSour = strlen(abinfo.szName);
							strncpy(PhoneItem[i].cName,abinfo.szName,sizeof(PhoneItem[iCallingOneNum].cName) > iSour ? iSour + 1:sizeof(PhoneItem[iCallingOneNum].cName) - 1);
							
						}
						else 
							strcpy(PhoneItem[i].cName, "");
					}
				}
			}    
			
			
			if (hIncallGif != NULL)
			{
				EndAnimatedGIF(hIncallGif);
				hIncallGif = NULL;
			}
			if (hBitPortrait != NULL)
			{
				DeleteObject(hBitPortrait);
				hBitPortrait = NULL;
			}
			if (MBCongig_Calling.ALS)
				bIcon = TRUE;
			else
				bIcon = FALSE;
			InitPaint(&incallif_initpaint, bSetPortrait, bIcon);
			hdc = GetDC(hWnd);
			GetImageDimensionFromFile(MBPIC_INCALLTWO,&sCartoon);
			hIncallGif = StartAnimatedGIFFromFile(hWnd,MBPIC_INCALLTWO,
				incallif_initpaint.subcapmid.left,
				incallif_initpaint.subcapmid.top, DM_NONE);
			ReleaseDC(hWnd,hdc);			
			
			
			
        }
//		hFocus = hMBList;
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
        printf("mb MBCalling 多方通话 SetMenuSoftkey函数\r\n");
#endif
        break;
    }
    InvalidateRect(hWnd,NULL,TRUE);
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
    printf("mb MBCalling SetMenuSoftkey函数正常退出\r\n");
#endif
    return TRUE;
}

static  void    SetMBCallingMenu(HWND hWnd, INTERFACE iMenuType)
{
    HMENU hMenu, hsubmenu;
	int menuitems;
	int i;
	
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
    printf("mb MBCalling SetMBCallingMenu,iMenuType:%d,InterfaceType:%d\r\n",iMenuType,InterfaceType);
#endif

    hMenu = PDAGetMenu(hWnd);
    switch (iMenuType)
    {
    case INTERFACE_NONE:
    case INTERFACE_MULT5:
    case INTERFACE_MULT6:
        SendMessage(GetCapture(), WM_CANCELMODE, NULL, NULL);
        DestroyMenu(hMenu);
        PDASetMenu(hWnd,NULL);
        break;
    case INTERFACE_ONE: // in one call
		if (NULL == hMenu)
        {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
            printf("mb MBCalling SetMBCallingMenu LoadMenuIndirect fail,hMenu == NULL,iMenuType:%d\r\n",iMenuType);
            //break;
#endif
            hMenu = CreateMenu();
            PDASetMenu(hWnd, hMenu);
        }
        if (!IsMenu(hMenu))
        {
            DestroyMenu(hMenu);
            hMenu = CreateMenu();
            PDASetMenu(hWnd, hMenu);
        }
        if (hMenu != NULL) 
        {
			menuitems = GetMenuItemCount(hMenu);
			for(i=0; i<menuitems; i++)
				RemoveMenu(hMenu, 0, MF_BYPOSITION);

			if (bHolding)
				InsertMenu(hMenu, 0, MF_BYPOSITION, IDM_PICK, TXT_RESTORE);
			else
				InsertMenu(hMenu, 0, MF_BYPOSITION, IDM_ONE_HOLD, TXT_HOLD);

			if (bMute)
				InsertMenu(hMenu, 1, MF_BYPOSITION, IDM_MUTE, TXT_UNMUTE);
			else
				InsertMenu(hMenu, 1, MF_BYPOSITION, IDM_MUTE, TXT_MUTE);
			
			InsertMenu(hMenu, 2, MF_BYPOSITION, IDM_SENDDTMF, TXT_SENDDTMF);
		}
        break;

    case INTERFACE_MULT1:// one call in active and another in held
		if (NULL == hMenu)
        {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
            printf("mb MBCalling SetMBCallingMenu LoadMenuIndirect fail,hMenu == NULL,iMenuType:%d\r\n",iMenuType);
            break;
#endif
        }
		if (hMenu != NULL) 
        {
			menuitems = GetMenuItemCount(hMenu);
			for(i=0; i<menuitems; i++)
				RemoveMenu(hMenu, 0, MF_BYPOSITION);
			if (bHF) 				
				InsertMenu(hMenu, 0 , MF_BYPOSITION, IDM_HANDSET, TXT_HANDSET);
            else				
				InsertMenu(hMenu, 0 , MF_BYPOSITION, IDM_HANDFREE, TXT_HANDFREE);
            
			InsertMenu(hMenu, 1, MF_BYPOSITION, IDM_ALLHANGUP, TXT_ALLHANGUP);

			if (bMute) 				
				InsertMenu(hMenu, 2, MF_BYPOSITION, IDM_MUTE, TXT_UNMUTE);
			else				
				InsertMenu(hMenu, 2, MF_BYPOSITION, IDM_MUTE, TXT_MUTE);			
			
			InsertMenu(hMenu, 3, MF_BYPOSITION, IDM_SWITCH1, TXT_SWITCH);			
			InsertMenu(hMenu, 4, MF_BYPOSITION, IDM_ALLACTIVE, TXT_ALLACTIVE);			
			InsertMenu(hMenu, 5, MF_BYPOSITION, IDM_SHIFT, TXT_SHIFT);
			InsertMenu(hMenu, 6, MF_BYPOSITION, IDM_SENDDTMF, TXT_SENDDTMF);
		}
        break;

    case INTERFACE_MULT21:// a conference in active
        if (NULL == hMenu)
        {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
            printf("mb MBCalling SetMBCallingMenu LoadMenuIndirect fail,hMenu == NULL,iMenuType:%d\r\n",iMenuType);
            break;
#endif
        }
		if (hMenu != NULL) 
        {
			menuitems = GetMenuItemCount(hMenu);
			for(i=0; i<menuitems; i++)
				RemoveMenu(hMenu, 0, MF_BYPOSITION);

			if (bHF) 
				InsertMenu(hMenu, 0 , MF_BYPOSITION, IDM_HANDSET, TXT_HANDSET);
            else
				InsertMenu(hMenu, 0 , MF_BYPOSITION, IDM_HANDFREE, TXT_HANDFREE);

			InsertMenu(hMenu, 1, MF_BYPOSITION, IDM_ALLHANGUP, TXT_ALLHANGUP);
			hsubmenu = CreateMenu();
			InsertMenu(hMenu, 2 ,MF_BYPOSITION|MF_POPUP, (DWORD)hsubmenu ,(LPCSTR)TXT_MEMBERS);
			//InsertMenu(hsubmenu, 0, MF_BYPOSITION, IDM_PICKUPCALL, (LPCSTR)TXT_PICKUPCALL); 
			InsertMenu(hsubmenu, 1, MF_BYPOSITION, IDM_SINGLECALL, (LPCSTR)TXT_SINGLECALL); 
			InsertMenu(hsubmenu, 2, MF_BYPOSITION, IDM_SINGLEHANGUP, (LPCSTR)TXT_SINGLEHANGUP);
//			InsertMenu(hMenu, 2, MF_BYPOSITION, IDM_SINGLECALL  ,TXT_SINGLECALL);   
//			InsertMenu(hMenu, 3, MF_BYPOSITION, IDM_SINGLEHANGUP,TXT_SINGLEHANGUP); 
			InsertMenu(hMenu, 3, MF_BYPOSITION, IDM_SWITCH2     ,TXT_HOLDCONFERENCE);         
			if (bMute) 
				InsertMenu(hMenu, 4, MF_BYPOSITION, IDM_MUTE, TXT_UNMUTE);
			else
				InsertMenu(hMenu, 4, MF_BYPOSITION, IDM_MUTE, TXT_MUTE);

			InsertMenu(hMenu, 5, MF_BYPOSITION, IDM_SENDDTMF, TXT_SENDDTMF);			
		}		
        break;

    case INTERFACE_MULT22: // a conference in held
        if (NULL == hMenu)
        {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
            printf("mb MBCalling SetMBCallingMenu LoadMenuIndirect fail,hMenu == NULL,iMenuType:%d\r\n",iMenuType);
            break;
#endif
        }
		if (hMenu != NULL) {
			menuitems = GetMenuItemCount(hMenu);
			for(i=0; i<menuitems; i++)
				RemoveMenu(hMenu, 0, MF_BYPOSITION);
			if (bHF) 
				InsertMenu(hMenu, 0 , MF_BYPOSITION, IDM_HANDSET, TXT_HANDSET);
            else
				InsertMenu(hMenu, 0 , MF_BYPOSITION, IDM_HANDFREE, TXT_HANDFREE);
			InsertMenu(hMenu, 1, MF_BYPOSITION, IDM_ALLHANGUP, TXT_ALLHANGUP);
			hsubmenu = CreateMenu();
			InsertMenu(hMenu   ,2 ,MF_BYPOSITION|MF_POPUP, (DWORD)hsubmenu ,(LPCSTR)TXT_MEMBERS);   
			//InsertMenu(hsubmenu, 0, MF_BYPOSITION, IDM_PICKUPCALL, (LPCSTR)TXT_PICKUPCALL); 
			InsertMenu(hsubmenu, 1, MF_BYPOSITION, IDM_SINGLECALL, (LPCSTR)TXT_SINGLECALL); 
			InsertMenu(hsubmenu, 2, MF_BYPOSITION, IDM_SINGLEHANGUP, (LPCSTR)TXT_SINGLEHANGUP);
			InsertMenu(hMenu, 3, MF_BYPOSITION, IDM_SWITCH2  , TXT_PICKCONFERENCE);   
			if (bMute) 
				InsertMenu(hMenu, 4, MF_BYPOSITION, IDM_MUTE, TXT_UNMUTE);
			else
				InsertMenu(hMenu, 4, MF_BYPOSITION, IDM_MUTE, TXT_MUTE);
			InsertMenu(hMenu, 5, MF_BYPOSITION, IDM_SENDDTMF, TXT_SENDDTMF);	
			
		}
        break;

    case INTERFACE_MULT31: // one in active the others in held
        if (NULL == hMenu)
        {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
            printf("mb MBCalling SetMBCallingMenu LoadMenuIndirect fail,hMenu == NULL,iMenuType:%d\r\n",iMenuType);
            break;
#endif
        }
		if (hMenu != NULL) {
			menuitems = GetMenuItemCount(hMenu);
			for(i=0; i<menuitems; i++)
				RemoveMenu(hMenu, 0, MF_BYPOSITION);
			if (bHF) 
				InsertMenu(hMenu, 0 , MF_BYPOSITION, IDM_HANDSET, TXT_HANDSET);
            else
				InsertMenu(hMenu, 0 , MF_BYPOSITION, IDM_HANDFREE, TXT_HANDFREE);
//			InsertMenu(hMenu, 1, MF_BYPOSITION, IDM_ALLHANGUP, TXT_ALLHANGUP);
			InsertMenu(hMenu, 1, MF_BYPOSITION, IDM_HANGUPACTIVE, TXT_ALLHANGUP);
			hsubmenu = CreateMenu();
			InsertMenu(hMenu   ,2 ,MF_BYPOSITION|MF_POPUP, (DWORD)hsubmenu ,(LPCSTR)TXT_MEMBERS);   
			InsertMenu(hsubmenu, 0, MF_BYPOSITION, IDM_PICKUPCALL, (LPCSTR)TXT_PICKUPCALL); 
			//InsertMenu(hsubmenu, 1, MF_BYPOSITION, IDM_SINGLECALL, (LPCSTR)TXT_SINGLECALL); 
			//InsertMenu(hsubmenu, 2, MF_BYPOSITION, IDM_SINGLEHANGUP, (LPCSTR)TXT_SINGLEHANGUP);
			InsertMenu(hMenu, 3, MF_BYPOSITION, IDM_SWITCH2  , TXT_PICKCONFERENCE);   
			if (bMute) 
				InsertMenu(hMenu, 4, MF_BYPOSITION, IDM_MUTE, TXT_UNMUTE);
			else
				InsertMenu(hMenu, 4, MF_BYPOSITION, IDM_MUTE, TXT_MUTE);
			InsertMenu(hMenu, 5, MF_BYPOSITION, IDM_SENDDTMF, TXT_SENDDTMF);
		}
        break;

    case INTERFACE_MULT32: // one in held the others in active
        if (NULL == hMenu)
        {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
            printf("mb MBCalling SetMBCallingMenu LoadMenuIndirect fail,hMenu == NULL,iMenuType:%d\r\n",iMenuType);
            break;
#endif
        }
		if (hMenu != NULL) {
			menuitems = GetMenuItemCount(hMenu);
			for(i=0; i<menuitems; i++)
				RemoveMenu(hMenu, 0, MF_BYPOSITION);
			if (bHF) 
				InsertMenu(hMenu, 0 , MF_BYPOSITION, IDM_HANDSET, TXT_HANDSET);
            else
				InsertMenu(hMenu, 0 , MF_BYPOSITION, IDM_HANDFREE, TXT_HANDFREE);
//			InsertMenu(hMenu, 1, MF_BYPOSITION, IDM_ALLHANGUP, TXT_ALLHANGUP);
			InsertMenu(hMenu, 1, MF_BYPOSITION, IDM_HANGUPACTIVE, TXT_ALLHANGUP);
			hsubmenu = CreateMenu();
			InsertMenu(hMenu   ,2 ,MF_BYPOSITION|MF_POPUP, (DWORD)hsubmenu ,(LPCSTR)TXT_MEMBERS);   
			InsertMenu(hsubmenu, 0, MF_BYPOSITION, IDM_PICKUPCALL, (LPCSTR)TXT_PICKUPCALL); 
			//InsertMenu(hsubmenu, 1, MF_BYPOSITION, IDM_SINGLECALL, (LPCSTR)TXT_SINGLECALL); 
			InsertMenu(hsubmenu, 2, MF_BYPOSITION, IDM_SINGLEHANGUP, (LPCSTR)TXT_SINGLEHANGUP);
			InsertMenu(hMenu, 3, MF_BYPOSITION, IDM_SWITCH2  , TXT_HOLDCONFERENCE);   
			if (bMute) 
				InsertMenu(hMenu, 4, MF_BYPOSITION, IDM_MUTE, TXT_UNMUTE);
			else
				InsertMenu(hMenu, 4, MF_BYPOSITION, IDM_MUTE, TXT_MUTE);
			InsertMenu(hMenu, 5, MF_BYPOSITION, IDM_SENDDTMF, TXT_SENDDTMF);
		}
        break;
    }
    PDADrawMenuBar(hWnd);
    InterfaceType = iMenuType;
	
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
    printf("mb MBCalling SetMBCallingMenu 函数正常结束\r\n");
#endif
}

static  void    MB_RemoveAllMenu(HMENU hMenu)
{
    int iCount,i,j,iSubCount,iMenuID;
    HMENU hSubMenu;
	
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
    printf("mb MBCalling MB_RemoveAllMenu开始删除菜单\r\n");
#endif
    iCount = GetMenuItemCount(hMenu);
    for (i = iCount - 1;i >= 0 ;i--)
    {
        hSubMenu = GetSubMenu(hMenu,i);
        if (hSubMenu != NULL)
        {
            iSubCount = GetMenuItemCount(hSubMenu);
            for (j = iSubCount - 1;j >= 0 ;j--)
            {
                iMenuID = GetMenuItemID(hSubMenu,j);
                if ((iMenuID == IDM_MUTE) || 
                    (iMenuID == IDM_RECORD) || 
                    (iMenuID == IDM_VIEW) ||
                    (iMenuID == -1))
                    continue;
                RemoveMenu(hSubMenu,j,MF_BYPOSITION);
            }
            RemoveMenu(hMenu,i,MF_BYPOSITION);
        }
        else
        {
            iMenuID = GetMenuItemID(hMenu,i);
            if ((iMenuID == IDM_MUTE) || 
                (iMenuID == IDM_RECORD) || 
                (iMenuID == IDM_VIEW) ||
                (iMenuID == -1))
                continue;
            RemoveMenu(hMenu,i,MF_BYPOSITION);
        }
    }
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
    printf("mb MBCalling MB_RemoveAllMenu菜单删除完毕\r\n");
#endif
}

static  void    SetChangelessMenu(HMENU hMenu)
{
    ModifyMenu(hMenu,IDM_MUTE,MF_BYCOMMAND|MF_ENABLED,IDM_MUTE,
        bMute ? TXT_UNMUTE : TXT_MUTE);
}

static  void    CallingDisplay(HDC hdc)
{
    char    cLastTime[10] = "";
    char    cCostInfo[50] = "";
    int     i;
	char    showinfo[ME_PHONENUM_LEN] = "";
    char    statusinfo[20];
	HBRUSH  hbrush;
	HBRUSH  holdbrush;
	HFONT   hfont;
	HFONT   holdfont;
	COLORREF oldcolor;
	int nMode;
	
	hbrush = CreateBrush(BS_SOLID, RGB(166,202,240),0);
	holdbrush = SelectObject(hdc, hbrush);
	FillRect(hdc, &(incallif_initpaint.subcap), hbrush);
	SelectObject(hdc, holdbrush);
	DeleteObject(hbrush);
	PaintAnimatedGIF(hdc,hIncallGif);
	if (bMute)
	{
		oldcolor = SetBkColor(hdc,RGB(0,0,0));		
		nMode = SetBkMode(hdc,NEWTRANSPARENT);
		DrawImageFromFile(hdc, CALL_MUTED_ICON, 
			incallif_initpaint.subcapright.left, incallif_initpaint.subcapright.top, SRCCOPY);
		SetBkMode(hdc,nMode);		
		SetBkColor(hdc,oldcolor);
	}
    if (hLineBmp)
    {
        RECT rLine;
        oldcolor = SetBkColor(hdc,RGB(0,0,0));		
        nMode = SetBkMode(hdc,NEWTRANSPARENT);
        SetRect(&rLine,10,10,10 + LinePicSize.cx,10 + LinePicSize.cy);
        StretchBlt(hdc,
            rLine.left,rLine.top,
            rLine.right - rLine.left,rLine.bottom - rLine.top,//目的矩形
            (HDC)hLineBmp,0,0,LinePicSize.cx,LinePicSize.cy,//图象原矩形
            SRCCOPY);//把位图从源矩形拷贝到目的矩形，扩展或压缩位图以使其适合目的矩形的范围

        SetBkMode(hdc,nMode);
        SetBkColor(hdc,oldcolor);
    }
    if (bSetVolume)//设置音量
    {
    }
	if (GetFontHandle(&hfont, 1))
	{
		holdfont = SelectObject(hdc, hfont);
	}
    if (bAOCInfo)
    {
        if ((dCostInfo + 1 >= dAcmMax) && (!dShowAOCClew))
        {
            PLXTipsWin(NULL, NULL, NULL, "Near call cost limit", 
                "", Notify_Info, ML("Ok"), NULL, -1);
            dShowAOCClew = TRUE;
        }
        else
        {
            switch (CostType)
            {
            case COSTTYPE_UNIT:
                sprintf(cCostInfo,pCostInfo2,dCostInfo);
                break;
            case COSTTYPE_PRICE:
                //sprintf(cCostInfo,pCostInfo1,me_pucinfo.currency,dCostInfo * dCost);
                MB_Mtoa(dCostInfo * dCost,cCostInfo);
                MB_DealDouble(cCostInfo,3);
                break;
            }
        }
    }
    switch (CallingType)
    {
	case MBCALLINNONE:
		memset(&rTime, 0, sizeof(RECT));
		memset(&rCostInfo, 0, sizeof(RECT));
		if ((strlen(endtimeinfo)!=0)&&(strlen(endcostinfo)!=0))
		{
			DrawText(hdc,endshowinfo,strlen(endshowinfo),&(incallif_initpaint.line1),DT_HCENTER|DT_VCENTER);
			DrawText(hdc,endstatusinfo,strlen(endstatusinfo),&(incallif_initpaint.line2),DT_HCENTER|DT_VCENTER);
			DrawText(hdc,endtimeinfo,strlen(endtimeinfo),&(incallif_initpaint.line3),DT_HCENTER|DT_VCENTER);
			DrawText(hdc,endcostinfo,strlen(endcostinfo),&(incallif_initpaint.line4),DT_HCENTER|DT_VCENTER);
		}
		else if((strlen(endtimeinfo)!=0)&&(strlen(endcostinfo)==0))
		{
			DrawText(hdc,endshowinfo,strlen(endshowinfo),&(incallif_initpaint.line1),DT_HCENTER|DT_VCENTER);
			DrawText(hdc,endstatusinfo,strlen(endstatusinfo),&(incallif_initpaint.line2),DT_HCENTER|DT_VCENTER);
			DrawText(hdc,endtimeinfo,strlen(endtimeinfo),&(incallif_initpaint.line3),DT_HCENTER|DT_VCENTER);
		}
		else if((strlen(endtimeinfo)==0)&&(strlen(endcostinfo)!=0))
		{
			DrawText(hdc,endshowinfo,strlen(endshowinfo),&(incallif_initpaint.line1),DT_HCENTER|DT_VCENTER);
			DrawText(hdc,endstatusinfo,strlen(endstatusinfo),&(incallif_initpaint.line2),DT_HCENTER|DT_VCENTER);
			DrawText(hdc,endcostinfo,strlen(endcostinfo),&(incallif_initpaint.line3),DT_HCENTER|DT_VCENTER);
		}
		else if((strlen(endtimeinfo)==0)&&(strlen(endcostinfo)==0))
		{
			DrawText(hdc,endshowinfo,strlen(endshowinfo),&(incallif_initpaint.line2),DT_HCENTER|DT_VCENTER);
			DrawText(hdc,endstatusinfo,strlen(endstatusinfo),&(incallif_initpaint.line3),DT_HCENTER|DT_VCENTER);
		}		
		break;
    case MBCALLINGONE:
        StretchBlt(hdc,
            (GetScreenUsableWH1(SM_CXSCREEN) - Incallsize.cx) / 2,//incallif_initpaint.subcapmid.left,
            incallif_initpaint.subcapmid.top,
            Incallsize.cx,Incallsize.cy,
            (HDC)hBitmapCalling,0,0,Incallsize.cx,Incallsize.cy,SRCCOPY);

		if (strlen(PhoneItem[iCallingOneNum].cName) == 0) 
		{
			if (strlen(PhoneItem[iCallingOneNum].Calls_Info.PhoneNum)== 0) 
				sprintf(showinfo, UNKNOWNUMBER);
			else
				InitTxt(hdc,PhoneItem[iCallingOneNum].Calls_Info.PhoneNum,showinfo, 176, FALSE);
		}
		else
			InitTxt(hdc,PhoneItem[iCallingOneNum].cName,showinfo, 176, TRUE);
		
        if (strcmp(PhoneItem[iCallingOneNum].cName,STATUS_INFO_EMERGENTCALL) == 0)
        {
            DrawText(hdc,showinfo,-1,&(incallif_initpaint.line2),DT_HCENTER|DT_VCENTER);
            break;
        }
		if (bSetPortrait && (CALL_STAT_ACTIVE == PhoneItem[iCallingOneNum].Calls_Info.Stat))
		{ 
			if (MBCongig_Calling.bShowCallDuration && (!MBCongig_Calling.bShowCallCost))
			{
				memcpy(&rTime, &(incallif_initpaint.line4),sizeof(RECT));
				memset(&rCostInfo, 0, sizeof(RECT));
				DrawText(hdc,showinfo,strlen(showinfo),&(incallif_initpaint.line3),DT_HCENTER|DT_VCENTER);
				sprintf(cLastTime,pTime,
					PhoneItem[iCallingOneNum].TimeUsed.wHour,
					PhoneItem[iCallingOneNum].TimeUsed.wMinute,
					PhoneItem[iCallingOneNum].TimeUsed.wSecond); 
				DrawText(hdc,cLastTime,strlen(cLastTime),&(incallif_initpaint.line4),DT_HCENTER|DT_VCENTER);
				StretchBlt(hdc, 
					incallif_initpaint.photorect.left,
					incallif_initpaint.photorect.top,
					incallif_initpaint.photorect.right-incallif_initpaint.photorect.left,
					incallif_initpaint.photorect.bottom-incallif_initpaint.photorect.top,
					(HDC)hBitPortrait,0,0,sPortrait.cx,sPortrait.cy,
					SRCCOPY);
			}
			if (MBCongig_Calling.bShowCallCost && (!MBCongig_Calling.bShowCallDuration))
			{
				memcpy(&rCostInfo, &(incallif_initpaint.line4),sizeof(RECT));
				memset(&rTime, 0, sizeof(RECT));
				DrawText(hdc,showinfo,strlen(showinfo),&(incallif_initpaint.line3),DT_HCENTER|DT_VCENTER);
				DrawText(hdc,cCostInfo,strlen(cCostInfo),&(incallif_initpaint.line4),DT_HCENTER|DT_VCENTER);
				StretchBlt(hdc, 
					incallif_initpaint.photorect.left,
					incallif_initpaint.photorect.top,
					incallif_initpaint.photorect.right-incallif_initpaint.photorect.left,
					incallif_initpaint.photorect.bottom-incallif_initpaint.photorect.top,
					(HDC)hBitPortrait,0,0,sPortrait.cx,sPortrait.cy,
					SRCCOPY);
			}
			if ((!MBCongig_Calling.bShowCallDuration) && (!MBCongig_Calling.bShowCallCost))
			{
				memset(&rTime, 0, sizeof(RECT));
				memset(&rCostInfo, 0, sizeof(RECT));
				DrawText(hdc,showinfo,strlen(showinfo),&(incallif_initpaint.line3),DT_HCENTER|DT_VCENTER);
                DrawText(hdc,cCostInfo,-1,&(incallif_initpaint.line4),DT_CENTER);
				StretchBlt(hdc, 
					incallif_initpaint.photorect.left,
					incallif_initpaint.photorect.top,
					incallif_initpaint.photorect.right-incallif_initpaint.photorect.left,
					incallif_initpaint.photorect.bottom-incallif_initpaint.photorect.top,
					(HDC)hBitPortrait,0,0,sPortrait.cx,sPortrait.cy,
					SRCCOPY);
			}
			if ( MBCongig_Calling.bShowCallDuration && MBCongig_Calling.bShowCallCost )
			{
				memcpy(&rTime, &(incallif_initpaint.line3),sizeof(RECT));
				memcpy(&rCostInfo, &(incallif_initpaint.line4),sizeof(RECT));
				DrawText(hdc,showinfo,strlen(showinfo),&(incallif_initpaint.line2),DT_HCENTER|DT_VCENTER);
				sprintf(cLastTime,pTime,
					PhoneItem[iCallingOneNum].TimeUsed.wHour,
					PhoneItem[iCallingOneNum].TimeUsed.wMinute,
					PhoneItem[iCallingOneNum].TimeUsed.wSecond);
				DrawText(hdc,cLastTime,strlen(cLastTime),&(incallif_initpaint.line3),DT_HCENTER|DT_VCENTER);
				DrawText(hdc,cCostInfo,strlen(cCostInfo),&(incallif_initpaint.line4),DT_HCENTER|DT_VCENTER);
			}
		}
		if (bSetPortrait && (CALL_STAT_HELD == PhoneItem[iCallingOneNum].Calls_Info.Stat)) 
		{
			sprintf(statusinfo, STATUS_INFO_ONHOLD);
			if ((!MBCongig_Calling.bShowCallDuration) && (!MBCongig_Calling.bShowCallCost))
			{
				memset(&rTime, 0, sizeof(RECT));
				memset(&rCostInfo, 0, sizeof(RECT));
				DrawText(hdc,showinfo,strlen(showinfo),&(incallif_initpaint.line3),DT_HCENTER|DT_VCENTER);
				DrawText(hdc, statusinfo, strlen(statusinfo), &(incallif_initpaint.line4), DT_HCENTER|DT_VCENTER);
				StretchBlt(hdc, 
					incallif_initpaint.photorect.left,
					incallif_initpaint.photorect.top,
					incallif_initpaint.photorect.right-incallif_initpaint.photorect.left,
					incallif_initpaint.photorect.bottom-incallif_initpaint.photorect.top,
					(HDC)hBitPortrait,0,0,sPortrait.cx,sPortrait.cy,
					SRCCOPY);
			}
			if (MBCongig_Calling.bShowCallDuration && (!MBCongig_Calling.bShowCallCost))
			{
				memcpy(&rTime, &(incallif_initpaint.line3),sizeof(RECT));
				memset(&rCostInfo, 0, sizeof(RECT));
				DrawText(hdc,showinfo,strlen(showinfo),&(incallif_initpaint.line1),DT_HCENTER|DT_VCENTER);
				DrawText(hdc,statusinfo,strlen(statusinfo),&(incallif_initpaint.line2),DT_HCENTER|DT_VCENTER);
				sprintf(cLastTime,pTime,
					PhoneItem[iCallingOneNum].TimeUsed.wHour,
					PhoneItem[iCallingOneNum].TimeUsed.wMinute,
					PhoneItem[iCallingOneNum].TimeUsed.wSecond);
				DrawText(hdc,cLastTime,strlen(cLastTime),&(incallif_initpaint.line3),DT_HCENTER|DT_VCENTER);
                DrawText(hdc,cCostInfo,-1,&(incallif_initpaint.line4),DT_CENTER);
			}
			if (MBCongig_Calling.bShowCallCost && (!MBCongig_Calling.bShowCallDuration))
			{
				memcpy(&rCostInfo, &(incallif_initpaint.line3),sizeof(RECT));
				memset(&rTime, 0, sizeof(RECT));
				DrawText(hdc,showinfo,strlen(showinfo),&(incallif_initpaint.line1),DT_HCENTER|DT_VCENTER);
				DrawText(hdc,statusinfo,strlen(statusinfo),&(incallif_initpaint.line2),DT_HCENTER|DT_VCENTER);
				DrawText(hdc,cCostInfo,strlen(cCostInfo),&(incallif_initpaint.line3),DT_HCENTER|DT_VCENTER);
			}
			if ( MBCongig_Calling.bShowCallDuration && MBCongig_Calling.bShowCallCost )
			{
				memcpy(&rTime, &(incallif_initpaint.line3),sizeof(RECT));
				memcpy(&rCostInfo, &(incallif_initpaint.line4),sizeof(RECT));
				DrawText(hdc,showinfo,strlen(showinfo),&(incallif_initpaint.line1),DT_HCENTER|DT_VCENTER);
				DrawText(hdc,statusinfo,strlen(statusinfo),&(incallif_initpaint.line2),DT_HCENTER|DT_VCENTER);
				sprintf(cLastTime,pTime,
					PhoneItem[iCallingOneNum].TimeUsed.wHour,
					PhoneItem[iCallingOneNum].TimeUsed.wMinute,
					PhoneItem[iCallingOneNum].TimeUsed.wSecond);
				DrawText(hdc,cLastTime,strlen(cLastTime),&(incallif_initpaint.line3),DT_HCENTER|DT_VCENTER);
				DrawText(hdc,cCostInfo,strlen(cCostInfo),&(incallif_initpaint.line4),DT_HCENTER|DT_VCENTER);
			}
		}
		if ((!bSetPortrait) && (CALL_STAT_ACTIVE == PhoneItem[iCallingOneNum].Calls_Info.Stat)) 
		{
			if ((!MBCongig_Calling.bShowCallDuration) && (!MBCongig_Calling.bShowCallCost))
			{
				RECT singleinforect;
				memset(&rTime, 0, sizeof(RECT));
				memset(&rCostInfo, 0, sizeof(RECT));
				singleinforect.left = incallif_initpaint.line1.left;
				singleinforect.top = incallif_initpaint.line1.top;
				singleinforect.right = incallif_initpaint.line4.right;
				singleinforect.bottom = incallif_initpaint.line4.bottom;
				DrawText(hdc,showinfo,strlen(showinfo),&singleinforect,DT_HCENTER|DT_VCENTER);
			}
			if (MBCongig_Calling.bShowCallDuration && (!MBCongig_Calling.bShowCallCost))
			{
				memcpy(&rTime, &(incallif_initpaint.line3),sizeof(RECT));
				memset(&rCostInfo, 0, sizeof(RECT));
				DrawText(hdc,showinfo,strlen(showinfo),&(incallif_initpaint.line2),DT_HCENTER|DT_VCENTER);
				sprintf(cLastTime,pTime,
					PhoneItem[iCallingOneNum].TimeUsed.wHour,
					PhoneItem[iCallingOneNum].TimeUsed.wMinute,
					PhoneItem[iCallingOneNum].TimeUsed.wSecond);
				DrawText(hdc,cLastTime,strlen(cLastTime),&(incallif_initpaint.line3),DT_HCENTER|DT_VCENTER);
                DrawText(hdc,cCostInfo,-1,&(incallif_initpaint.line4),DT_CENTER);
			}
			if (MBCongig_Calling.bShowCallCost  && (!MBCongig_Calling.bShowCallDuration))
			{
				memcpy(&rCostInfo, &(incallif_initpaint.line3),sizeof(RECT));
				memset(&rTime, 0, sizeof(RECT));
				DrawText(hdc,showinfo,strlen(showinfo),&(incallif_initpaint.line2),DT_HCENTER|DT_VCENTER);
				DrawText(hdc,cCostInfo,strlen(cCostInfo),&(incallif_initpaint.line3),DT_HCENTER|DT_VCENTER);
			}
			if ( MBCongig_Calling.bShowCallDuration && MBCongig_Calling.bShowCallCost)
			{
				memcpy(&rTime, &(incallif_initpaint.line3),sizeof(RECT));
				memcpy(&rCostInfo, &(incallif_initpaint.line4),sizeof(RECT));
				DrawText(hdc,showinfo,strlen(showinfo),&(incallif_initpaint.line2),DT_HCENTER|DT_VCENTER);
				sprintf(cLastTime,pTime,
					PhoneItem[iCallingOneNum].TimeUsed.wHour,
					PhoneItem[iCallingOneNum].TimeUsed.wMinute,
					PhoneItem[iCallingOneNum].TimeUsed.wSecond);
				DrawText(hdc,cLastTime,strlen(cLastTime),&(incallif_initpaint.line3),DT_HCENTER|DT_VCENTER);
				DrawText(hdc,cCostInfo,strlen(cCostInfo),&(incallif_initpaint.line4),DT_HCENTER|DT_VCENTER);
			}
		}
		if ((!bSetPortrait) && (CALL_STAT_HELD == PhoneItem[iCallingOneNum].Calls_Info.Stat)) 
		{
			sprintf(statusinfo, STATUS_INFO_ONHOLD);
			if ((!MBCongig_Calling.bShowCallDuration) && (!MBCongig_Calling.bShowCallCost))
			{
				memset(&rTime, 0, sizeof(RECT));
				memset(&rCostInfo, 0, sizeof(RECT));
				DrawText(hdc,showinfo,strlen(showinfo),&(incallif_initpaint.line2),DT_HCENTER|DT_VCENTER);
				DrawText(hdc, statusinfo, strlen(statusinfo), &(incallif_initpaint.line3), DT_HCENTER|DT_VCENTER);
                DrawText(hdc,cCostInfo,-1,&(incallif_initpaint.line4),DT_CENTER);
			}
			if (MBCongig_Calling.bShowCallDuration && (!MBCongig_Calling.bShowCallCost))
			{
				memcpy(&rTime, &(incallif_initpaint.line3),sizeof(RECT));
				memset(&rCostInfo, 0, sizeof(RECT));
				DrawText(hdc,showinfo,strlen(showinfo),&(incallif_initpaint.line1),DT_HCENTER|DT_VCENTER);
				DrawText(hdc,statusinfo,strlen(statusinfo),&(incallif_initpaint.line2),DT_HCENTER|DT_VCENTER);
				sprintf(cLastTime,pTime,
					PhoneItem[iCallingOneNum].TimeUsed.wHour,
					PhoneItem[iCallingOneNum].TimeUsed.wMinute,
					PhoneItem[iCallingOneNum].TimeUsed.wSecond);
				DrawText(hdc,cLastTime,strlen(cLastTime),&(incallif_initpaint.line3),DT_HCENTER|DT_VCENTER);
                DrawText(hdc,cCostInfo,-1,&(incallif_initpaint.line4),DT_CENTER);
			}
			if (MBCongig_Calling.bShowCallCost && (!MBCongig_Calling.bShowCallDuration))
			{
				memcpy(&rCostInfo, &(incallif_initpaint.line3),sizeof(RECT));
				memset(&rTime, 0, sizeof(RECT));
				DrawText(hdc,showinfo,strlen(showinfo),&(incallif_initpaint.line1),DT_HCENTER|DT_VCENTER);
				DrawText(hdc,statusinfo,strlen(statusinfo),&(incallif_initpaint.line2),DT_HCENTER|DT_VCENTER);
				DrawText(hdc,cCostInfo,strlen(cCostInfo),&(incallif_initpaint.line3),DT_HCENTER|DT_VCENTER);
			}
			if ( MBCongig_Calling.bShowCallDuration && MBCongig_Calling.bShowCallCost )
			{
				memcpy(&rTime, &(incallif_initpaint.line3),sizeof(RECT));
				memcpy(&rCostInfo, &(incallif_initpaint.line4),sizeof(RECT));
				DrawText(hdc,showinfo,strlen(showinfo),&(incallif_initpaint.line1),DT_HCENTER|DT_VCENTER);
				DrawText(hdc,statusinfo,strlen(statusinfo),&(incallif_initpaint.line2),DT_HCENTER|DT_VCENTER);
				sprintf(cLastTime,pTime,
					PhoneItem[iCallingOneNum].TimeUsed.wHour,
					PhoneItem[iCallingOneNum].TimeUsed.wMinute,
					PhoneItem[iCallingOneNum].TimeUsed.wSecond);
				DrawText(hdc,cLastTime,strlen(cLastTime),&(incallif_initpaint.line3),DT_HCENTER|DT_VCENTER);
				DrawText(hdc,cCostInfo,strlen(cCostInfo),&(incallif_initpaint.line4),DT_HCENTER|DT_VCENTER);
			}
		}
        break;
		
    case MBCALLINGMULT:
		memset(&rTime, 0, sizeof(RECT));
		memset(&rCostInfo, 0, sizeof(RECT));
        switch (InterfaceType)
        {
		case INTERFACE_MULT1:			
			for (i = 0;i < MAX_COUNT;i++)
			{
				if (ITEMUSED == PhoneItem[i].ItemStat)
				{
					if (CALL_STAT_ACTIVE == PhoneItem[i].Calls_Info.Stat)
					{
						sprintf(statusinfo, STATUS_INFO_ACTIVE);
						if (strlen(PhoneItem[i].cName) == 0) 
						{
							if (strlen(PhoneItem[i].Calls_Info.PhoneNum)== 0) 
								sprintf(showinfo, UNKNOWNUMBER);
							else
								InitTxt(hdc,PhoneItem[i].Calls_Info.PhoneNum,showinfo, 176, FALSE);
						}
						else
							InitTxt(hdc,PhoneItem[i].cName,showinfo, 176, TRUE);
						DrawText(hdc, showinfo, strlen(showinfo), &(incallif_initpaint.line1), DT_HCENTER|DT_VCENTER);
						DrawText(hdc, statusinfo, strlen(statusinfo), &(incallif_initpaint.line2), DT_HCENTER|DT_VCENTER);
					}
					else if (CALL_STAT_HELD == PhoneItem[i].Calls_Info.Stat)
					{
						sprintf(statusinfo, STATUS_INFO_ONHOLD);
						if (strlen(PhoneItem[i].cName) == 0) 
						{
							if (strlen(PhoneItem[i].Calls_Info.PhoneNum)== 0) 
								sprintf(showinfo, UNKNOWNUMBER);
							else
								InitTxt(hdc,PhoneItem[i].Calls_Info.PhoneNum,showinfo, 176, FALSE);
						}
						else
							InitTxt(hdc,PhoneItem[i].cName,showinfo, 176, TRUE);
						DrawText(hdc, showinfo, strlen(showinfo), &(incallif_initpaint.line3), DT_HCENTER|DT_VCENTER);
						DrawText(hdc, statusinfo, strlen(statusinfo), &(incallif_initpaint.line4), DT_HCENTER|DT_VCENTER);
					}
				}
			}
			break;			
        case INTERFACE_MULT21:
		case INTERFACE_MULT22:
			{
				RECT conrect;
				sprintf(showinfo, TXT_CONFERENCE);
				conrect.left = incallif_initpaint.line1.left;
				conrect.top = incallif_initpaint.line1.top;
				conrect.right = incallif_initpaint.line4.right;
				conrect.bottom = incallif_initpaint.line4.bottom;
				DrawText(hdc,showinfo,strlen(showinfo),&conrect,DT_HCENTER|DT_VCENTER);
			}
            break;
        case INTERFACE_MULT31:
            for (i = 0;i < MAX_COUNT;i++)
            {
                if (ITEMUSED == PhoneItem[i].ItemStat)
                {
                    if (CALL_STAT_ACTIVE == PhoneItem[i].Calls_Info.Stat)
                    {
						sprintf(statusinfo, STATUS_INFO_ACTIVE);
						if (strlen(PhoneItem[i].cName) == 0) 
						{
							if (strlen(PhoneItem[i].Calls_Info.PhoneNum)== 0) 
								sprintf(showinfo, UNKNOWNUMBER);
							else
								InitTxt(hdc,PhoneItem[i].Calls_Info.PhoneNum,showinfo, 176, FALSE);
						}
						else
							InitTxt(hdc,PhoneItem[i].cName,showinfo, 176, TRUE);
						DrawText(hdc, showinfo, strlen(showinfo), &(incallif_initpaint.line1), DT_HCENTER|DT_VCENTER);
						DrawText(hdc, statusinfo, strlen(statusinfo), &(incallif_initpaint.line2), DT_HCENTER|DT_VCENTER);
                    }
                }
            }
			sprintf(showinfo, TXT_CONFERENCE);
			DrawText(hdc, showinfo, strlen(showinfo), &(incallif_initpaint.line3), DT_HCENTER|DT_VCENTER);
			sprintf(statusinfo, STATUS_INFO_ONHOLD);
			DrawText(hdc, statusinfo, strlen(statusinfo), &(incallif_initpaint.line4), DT_HCENTER|DT_VCENTER);
            break;
        case INTERFACE_MULT32:
            for (i = 0;i < MAX_COUNT;i++)
            {
                if (ITEMUSED == PhoneItem[i].ItemStat)
                {
                    if (CALL_STAT_HELD == PhoneItem[i].Calls_Info.Stat)
                    {
						sprintf(statusinfo, STATUS_INFO_ONHOLD);
						if (strlen(PhoneItem[i].cName) == 0) 
						{
							if (strlen(PhoneItem[i].Calls_Info.PhoneNum)== 0) 
								sprintf(showinfo, UNKNOWNUMBER);
							else
								InitTxt(hdc,PhoneItem[i].Calls_Info.PhoneNum,showinfo, 176, FALSE);
						}
						else
							InitTxt(hdc,PhoneItem[i].cName,showinfo, 176, TRUE);
						DrawText(hdc, showinfo, strlen(showinfo), &(incallif_initpaint.line3), DT_HCENTER|DT_VCENTER);
						DrawText(hdc, statusinfo, strlen(statusinfo), &(incallif_initpaint.line4), DT_HCENTER|DT_VCENTER);
                    }
                }
            }
			sprintf(showinfo, TXT_CONFERENCE);
			DrawText(hdc, showinfo, strlen(showinfo), &(incallif_initpaint.line1), DT_HCENTER|DT_VCENTER);
			sprintf(statusinfo, STATUS_INFO_ACTIVE);
			DrawText(hdc, statusinfo, strlen(statusinfo), &(incallif_initpaint.line2), DT_HCENTER|DT_VCENTER);
            break;
        }
        break;
    }
	if (GetFontHandle(&hfont, 1))
	{
		SelectObject(hdc, holdfont);
	}	
}

BOOL    IsAllType_Specific(int iCallType)
{
    int i;
	
    if ((iCallType != CALL_STAT_ACTIVE) && (iCallType != CALL_STAT_HELD))
        return FALSE;
	
    if (0 == GetUsedItemNum())
        return FALSE;
    for (i = 0;i < MAX_COUNT;i++)
    {
        if (ITEMUSED == PhoneItem[i].ItemStat)
        {
            if (iCallType != PhoneItem[i].Calls_Info.Stat)
                return FALSE;
        }
    }
    return TRUE;
}

static  BOOL    InvertItemStateOne(int index)
{
    if (ITEMUSED == PhoneItem[index].ItemStat)
    {
        if (CALL_STAT_ACTIVE == PhoneItem[index].Calls_Info.Stat)
		{
            PhoneItem[index].Calls_Info.Stat = CALL_STAT_HELD;
			bHolding = TRUE;
		}
        else if (CALL_STAT_HELD == PhoneItem[index].Calls_Info.Stat)
		{
            PhoneItem[index].Calls_Info.Stat = CALL_STAT_ACTIVE;
			bHolding = FALSE;
		}
        return TRUE;
    }
    return FALSE;
}

static  void    InvertItemStateMult(void)
{
    int i;
    for (i = 0;i < MAX_COUNT;i++)
    {
        if (ITEMUSED == PhoneItem[i].ItemStat)
        {
            if (CALL_STAT_ACTIVE == PhoneItem[i].Calls_Info.Stat)
                PhoneItem[i].Calls_Info.Stat = CALL_STAT_HELD;
            else if (CALL_STAT_HELD == PhoneItem[i].Calls_Info.Stat)
                PhoneItem[i].Calls_Info.Stat = CALL_STAT_ACTIVE;
        }
    }
}

static  BOOL    SetAllType(int nType)
{
    int i;
    for (i = 0;i < MAX_COUNT;i++)
    {
        if (ITEMUSED == PhoneItem[i].ItemStat)
            PhoneItem[i].Calls_Info.Stat = nType;
    }
    return TRUE;
}

static  void    RemTime(MBRecordCostTime * Cost,int nCall)
{
    Cost->nLastHour   = PhoneItem[nCall].TimeUsed.wHour;      
    Cost->nLastMin    = PhoneItem[nCall].TimeUsed.wMinute;        
    Cost->nLastSecond = PhoneItem[nCall].TimeUsed.wSecond;    
	
    if (CALL_DIR_ORIGINATED == PhoneItem[nCall].Calls_Info.Dir)
    {
        Cost->nOutSecond += PhoneItem[nCall].TimeUsed.wSecond;
        if (Cost->nOutSecond >= iSecond60)
        {
            Cost->nOutSecond -= iSecond60;
            Cost->nOutMin = Cost->nOutMin + PhoneItem[nCall].TimeUsed.wMinute + 1;
        }
        else
        {
            Cost->nOutMin += PhoneItem[nCall].TimeUsed.wMinute;
        }
        if (Cost->nOutMin >= iSecond60)
        {
            Cost->nOutMin -= iSecond60;
            Cost->nOutHour = Cost->nOutHour + PhoneItem[nCall].TimeUsed.wHour + 1;
        }
        else
        {
            Cost->nOutHour += PhoneItem[nCall].TimeUsed.wHour;
        }
    }
    else if (CALL_DIR_TERMINATED == PhoneItem[nCall].Calls_Info.Dir)
    {
        Cost->nInSecond += PhoneItem[nCall].TimeUsed.wSecond;
        if (Cost->nInSecond >= iSecond60)
        {
            Cost->nInSecond -= iSecond60;
            Cost->nInMin = Cost->nInMin + PhoneItem[nCall].TimeUsed.wMinute + 1;
        }
        else
        {
            Cost->nInMin += PhoneItem[nCall].TimeUsed.wMinute;
        }
        if (Cost->nInMin >= iSecond60)
        {
            Cost->nInMin -= iSecond60;
            Cost->nInHour = Cost->nInHour + PhoneItem[nCall].TimeUsed.wHour + 1;
        }
        else
        {
            Cost->nInHour += PhoneItem[nCall].TimeUsed.wHour;
        }
    }
}

BOOL    MBListCurCalls(OPERATETYPE OperateType,WPARAM wParam,LPARAM lParam)
{
    if (!IsWindow(hCalling))
        return FALSE;
	
//    MBWaitWindowState(TRUE);
    if (-1 == ME_ListCurCalls(hCalling,IDC_ME_LISTCURCALLS))
    {
        AppMessageBox(hCalling,ME_APPLY_FAIL,TITLECAPTION,WAITTIMEOUT);
        bListCurCalling = FALSE;
        return FALSE;
    }
    ListCurCallswParam = wParam;
    ListCurCallslParam = lParam;
    ListCurCallsType   = OperateType;
    bListCurCalling    = TRUE;
    return TRUE;
}

static  BOOL    OperateListCurCalls(void)
{
    CALLS_INFO calls_info[MAX_COUNT];
    int i,j,iDataLen;
    int iHangup = 0;        
    BOOL bAnswer = FALSE;   
    BOOL bWaiting = FALSE;  
    int iHeldNums;          
    int iActiveNums;        
	
    memset(calls_info,0x00,sizeof(calls_info));
    iDataLen = ME_GetResult(calls_info,sizeof(calls_info));
	
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
    printf("mb MBCalling 列当前通话之后执行的操作,iDataLen:%d,CallingType:%d\r\n",iDataLen,CallingType);
#endif
	
    if (iDataLen <= 0)
    {
        SendMessage(hCalling, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, ENDCALLICON),0);
        switch (CallingType)
        {
        case MBCALLINNONE:
            return TRUE;
        case MBCALLINGONE:
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
            printf("mb MBCalling OperateListCurCalls-当前是单方通话界面\r\n");
#endif
            AfterHangup_Single();
			//NotifyEmergencyCall();//不知道为什么调用
			NotifyRingWndUpdate();
            return FALSE;
			
        case MBCALLINGMULT:
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
            printf("mb MBCalling OperateListCurCalls-当前是多方通话界面\r\n");
#endif
            AfterHangup_Mult_All();
			//NotifyEmergencyCall();//不知道为什么调用
			NotifyRingWndUpdate();
            return FALSE;
        }
    }
    
    iHeldNums   = GetItemTypeNum(CALL_STAT_HELD);   
    iActiveNums = GetItemTypeNum(CALL_STAT_ACTIVE); 
	
    iDataLen /= sizeof(CALLS_INFO);
    
    for (i = 0;i < MAX_COUNT;i++)
    {
        for (j = 0;j < iDataLen;j++)
        {
            if ((PhoneItem[i].Calls_Info.index == calls_info[j].index) &&
                (PhoneItem[i].ItemStat == ITEMUSED))
            {
                PhoneItem[i].Calls_Info.Stat = calls_info[j].Stat;
                break;
            }
        }
        //if (iDataLen == iHeldNums + iActiveNums - 1)
		{
			if ((j == iDataLen) && (PhoneItem[i].ItemStat == ITEMUSED))
			{
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
				printf("mb MBCalling OperateListCurCalls,i:%d,号码:%s,该电话已经挂断\r\n",i,PhoneItem[i].Calls_Info.PhoneNum);
#endif
				AfterHangup_MultOne(i);
				//MBCallEndNodeWindow();
				iHangup++;
			}
		}
		//else
		{
		//	AfterHangup_MultOne(i);
		//	iHangup++;
		}
    }
	if (iHangup >0) 
	{
		MBCallEndNodeWindow();
		//NotifyEmergencyCall();//不知道为什么调用
		NotifyRingWndUpdate();
	}
    SetPhoneGroupNum();
	
    
    for (i = 0;i < iDataLen;i++)
    {
        for (j = 0;j < MAX_COUNT;j++)
        {
            if ((PhoneItem[j].Calls_Info.index == calls_info[i].index) &&
                (PhoneItem[j].ItemStat == ITEMUSED))
                break;
        }
        if (MAX_COUNT == j)
        {
            if ((calls_info[i].Stat == CALL_STAT_ACTIVE) || 
                (calls_info[i].Stat == CALL_STAT_HELD))
            {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
                printf("mb MBCalling OperateListCurCalls,i:%d,号码:%s,stat:%d,该电话已经被接入\r\n",
                    i,calls_info[i].PhoneNum,calls_info[i].Stat);
#endif
                MBRingAnswerCall();
                bAnswer = TRUE;
                break;
            }
            else if ((calls_info[i].Stat == CALL_STAT_INCOMING) || 
				(calls_info[i].Stat == CALL_STAT_WAITING))
            {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
                printf("mb MBCalling OperateListCurCalls 电话正在等待,%s,i:%d,index:%d\r\n",
                    calls_info[i].PhoneNum,i,calls_info[i].index);
#endif
                MessageMBRing(IDC_RING_HANGUP,0,0);
                bWaiting = TRUE;
                break;
            }
        }
    }
//	for (i = 0;i < iDataLen;i++)
//		InvertItemStateOne(i);
    if ((!bAnswer) && (iHangup > 0))
    {
        if (GetUsedItemNum() == 0)
        {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
            printf("mb MBCalling OperateListCurCalls 当前没有通话\r\n");
#endif
			SendMessage(hCalling, WM_CLOSE, 0,0);
            //DestroyWindow(hCalling);
            return FALSE;
        }
        if ((!bWaiting) &&                          
            (GetItemTypeNum(CALL_STAT_ACTIVE) == 0))
        {   
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
            printf("mb MBCalling OperateListCurCalls当前所有通话都是保持的,执行切换操作\r\n");
#endif
            MBCalling_Shortcut(ACTIVE_SWITCH,NULL);
			bHolding = FALSE;
            return FALSE;
        }
    }
	
    switch (ListCurCallsType)
    {
    case LISTCURCALLS_HANGUP_ONE:
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
        printf("mb MBCalling OperateListCurCalls 单独挂断失败\r\n");
#endif
		bToEndCall = FALSE;
        SendAbortCommand(ListCurCallswParam,ListCurCallslParam);
        AppMessageBox(hCalling,HANGUPFAIL,TITLECAPTION,WAITTIMEOUT);
        break;
		
    case LISTCURCALLS_HANGUP_ALL:
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
        printf("mb MBCalling OperateListCurCalls 全部挂断失败\r\n");
#endif
		bToEndCall = FALSE;
        SendAbortCommand(ListCurCallswParam,ListCurCallslParam);
        AppMessageBox(hCalling,HANGUPFAIL,TITLECAPTION,WAITTIMEOUT);
        break;
		
    case LISTCURCALLS_HANGUP_HELD:
        
        if ((iHeldNums > 0) && (iHangup != iHeldNums))
        {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
            printf("mb MBCalling OperateListCurCalls 原有保持通话个数:%d,挂断个数:%d\r\n",iHeldNums,iHangup);
#endif
            SendAbortCommand(ListCurCallswParam,ListCurCallslParam);
            AppMessageBox(hCalling,HANGUPFAIL,TITLECAPTION,WAITTIMEOUT);
        }
//		SendMessage(hCalling, MB_ACTIVE,0, 0);
        break;
		
    case LISTCURCALLS_HANGUP_ACTIVE:
        bHolding = FALSE;
        if ((iActiveNums > 0) && (iHangup != iActiveNums))
        {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
            printf("mb MBCalling OperateListCurCalls 原有激活通话个数:%d,挂断个数:%d\r\n",iHeldNums,iHangup);
#endif
            SendAbortCommand(ListCurCallswParam,ListCurCallslParam);
            AppMessageBox(hCalling,HANGUPFAIL,TITLECAPTION,WAITTIMEOUT);
        }
//		SendMessage(hCalling, MB_ACTIVE,0, 0);
        break;
		
    case LISTCURCALLS_HANGUP_CUT_X:
		bHolding = FALSE;
        if (1 != iHangup)
        {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
            printf("mb MBCalling OperateListCurCalls 挂断指定序号的激活通话失败\r\n");
#endif
            SendAbortCommand(ListCurCallswParam,ListCurCallslParam);
            AppMessageBox(hCalling,HANGUPFAIL,TITLECAPTION,WAITTIMEOUT);
        }
		SendMessage(hMemberWnd, WM_CLOSE, 0, 0);
//		SendMessage(hCalling, MB_ACTIVE,0, 0);
        break;
		
    case LISTCURCALLS_ACTIVE_X:
        if (GetItemTypeNum(CALL_STAT_ACTIVE) != 1)
        {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
            printf("mb MBCalling 密谈失败,iActiveNums:%d,iHeldNums:%d\r\n",iActiveNums,iHeldNums);
#endif
            SendAbortCommand(ListCurCallswParam,ListCurCallslParam);
            AppMessageBox(NULL,ME_APPLY_FAIL,TITLECAPTION,WAITTIMEOUT);
        }
		SendMessage(hMemberWnd, WM_CLOSE, 0, 0);
//		SendMessage(hCalling, MB_ACTIVE,0, 0);
        break;
		
    case LISTCURCALLS_TRANSFER:
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
        printf("mb MBCalling OperateListCurCalls 转移失败\r\n");
#endif
        SendAbortCommand(ListCurCallswParam,ListCurCallslParam);
        //AppMessageBox(NULL,ME_APPLY_FAIL,TITLECAPTION,WAITTIMEOUT);
        break;
	case LISTCURCALLS_SWITCH:
		iHeldNums   = GetItemTypeNum(CALL_STAT_HELD);   
		iActiveNums = GetItemTypeNum(CALL_STAT_ACTIVE); 
		if (iHeldNums >0 && iActiveNums ==0) 
			bHolding = TRUE;
		else
			bHolding = FALSE;
		break;
    }
    
	
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
    printf("mb MBCalling OperateListCurCalls 返回:%d\r\n",bAnswer);
#endif
    return (bAnswer ? FALSE : TRUE);
}

BOOL    SendAbortCommand(WPARAM wParam,LPARAM lParam)
{
    return FALSE;
}


void    SetCostLimit(BOOL bLimit)
{
    if (bLimit)
    {
        PLXTipsWin(NULL, NULL, NULL, "Call cost limit exceeded", 
            "", Notify_Info, ML("Ok"), NULL, -1);
    }
    bCostLimit = bLimit;
}
void    SetHangupSingalInfo(BOOL bSingalInfo)//当前电话断连是否是因为没有信号
{
    BT_HangupNetwork();//无信号挂断调用
    bSingal = bSingalInfo;
}
BOOL IsCallingWnd()
{
	return (IsWindow(hCalling)); 
}
HWND Get_CallingWnd()
{
	if (IsWindow(hCalling))
		return hCalling;
	else
		return NULL;
}

void CloseCallingWnd()
{
	int i;
	i = GetPhoneGroupNum();
	if (i == 1) 
		bToEndCall = TRUE;
	ExecuteHangup();
}

static HWND hMemberWnd;
static HWND hMemberList;
static MENULISTTEMPLATE MenuListElement;
static HMENULIST        MenuList;
static HWND hMEMBERWNDFOCUS;
static  LRESULT MemberWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );

static MEMBERWND mwndtype;
BOOL CreateMemberWnd(HWND hwnd, MEMBERWND type)
{
	WNDCLASS    wc;
	mwndtype = type;
	
    if (IsWindow(hMemberWnd))
    {
        
        ShowWindow(hMemberWnd,SW_SHOW);
        UpdateWindow(hMemberWnd);
        SetFocus(hMemberWnd);
        return TRUE;
    }
	
    wc.style         = CS_OWNDC;
    wc.lpfnWndProc   = MemberWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = "MEMBERWNDCLASS";
	
    if (!RegisterClass(&wc))
        return FALSE;
	
    hMemberWnd = CreateWindow("MEMBERWNDCLASS","", 
        WS_CAPTION|PWS_STATICBAR|PWS_NOTSHOWPI, 
        PLX_WIN_POSITION,hwnd,NULL,NULL,NULL);
	
    if (NULL == hMemberWnd)
    {
        UnregisterClass("MEMBERWNDCLASS",NULL);
        return FALSE;
    }
	CALLWND_InsertDataByHandle(pGlobalCallListBuf, hMemberWnd);	
    ShowWindow(hMemberWnd, SW_SHOW);
    UpdateWindow(hMemberWnd); 
    return (TRUE);	
}

static  LRESULT MemberWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
	LRESULT     lResult;
	RECT        listrect;
	int i;
	int listidx;
    lResult = (LRESULT)TRUE;
    switch ( wMsgCmd )
    {
    case WM_CREATE :
		if (EXTRACTMEMBER == mwndtype)
			SetWindowText(hWnd, TXT_SINGLECALL);
		else
			SetWindowText(hWnd, TXT_SINGLEHANGUP);
		SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,(WPARAM)IDC_RBUTTON_CANCEL,(LPARAM)TXT_BACK2);
		SendMessage(hWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select"); 
		GetClientRect(hWnd, &listrect);
		hMemberList = CreateWindow("LISTBOX", "", 
			WS_VISIBLE | WS_CHILD  | WS_VSCROLL | LBS_NOTIFY | LBS_BITMAP,
			0, 0 , listrect.right, listrect.bottom,
			hWnd, (HMENU)IDC_MEMBER_LIST, NULL, (PVOID)MenuList);
		if (hMemberList == NULL)
			return FALSE;
		listidx = 0;
		hActiveBmp = LoadImage(NULL, MEMBER_ACITVE_ICON, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		hHeldBmp = LoadImage(NULL, MEMBER_HELD_ICON, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		if (TRUE == bMemberIcon)
		{
			for (i = 0;i < MAX_COUNT;i++)
			{
				if (ITEMUSED == PhoneItem[i].ItemStat)
				{
					if (CALL_STAT_ACTIVE == PhoneItem[i].Calls_Info.Stat)
					{
						if (strlen(PhoneItem[i].cName) != 0)
							listidx = SendMessage(hMemberList,LB_ADDSTRING, 0, (LPARAM) (PhoneItem[i].cName));
						
						else
							listidx = SendMessage(hMemberList,LB_ADDSTRING, 0, (LPARAM)(PhoneItem[i].Calls_Info.PhoneNum));
						SendMessage(hMemberList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, listidx), (LPARAM)hActiveBmp);
					}
				}
			}
		}
		else
		{
			for (i = 0;i < MAX_COUNT;i++)
			{
				if (ITEMUSED == PhoneItem[i].ItemStat)
				{
					if (CALL_STAT_HELD == PhoneItem[i].Calls_Info.Stat)
					{
						if (strlen(PhoneItem[i].cName) != 0)
							listidx = SendMessage(hMemberList,LB_ADDSTRING, 0, (LPARAM) (PhoneItem[i].cName));
						
						else
							listidx = SendMessage(hMemberList,LB_ADDSTRING, 0, (LPARAM)(PhoneItem[i].Calls_Info.PhoneNum));
						SendMessage(hMemberList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, listidx), (LPARAM)hHeldBmp);
					}
				}
			}

		}
		SendMessage(hMemberList, LB_SETCURSEL, 0, 0);
		hMEMBERWNDFOCUS = hMemberList;
		break;
		
	case  WM_ACTIVATE:
		if (WA_INACTIVE != LOWORD(wParam))
			SetFocus(hMEMBERWNDFOCUS);
		else
			hMEMBERWNDFOCUS = GetFocus();
		break;
		
    case WM_CLOSE:
		DestroyWindow(hMemberWnd);
        break;
		
    case WM_DESTROY :
		CALLWND_DeleteDataByHandle(pGlobalCallListBuf, hMemberWnd);
		if (hActiveBmp)
		{
			DeleteObject(hActiveBmp);
			hActiveBmp = NULL;
		}
		if (hHeldBmp)
		{
			DeleteObject(hHeldBmp);
			hHeldBmp = NULL;
		}
		hMemberList =NULL;
		hMEMBERWNDFOCUS = NULL;
        UnregisterClass("MEMBERWNDCLASS",NULL);
        break;
				
    case WM_COMMAND:
        if (LOWORD( wParam ) ==IDC_RBUTTON_CANCEL) 
            SendMessage(hWnd, WM_CLOSE, 0, 0);
		break;
		
	case WM_KEYDOWN:
		if (LOWORD(wParam) == VK_F5)
		{
			i = SendMessage(hMemberList, LB_GETCURSEL, 0, 0);
			if (mwndtype == EXTRACTMEMBER) 
				SendMessage(hCalling, IDC_EXTRACTMEMBER,0, (LPARAM)i);
			if (mwndtype == DROPMEMBER)
				SendMessage(hCalling, IDC_DROPMEMBER,0, (LPARAM)i);
			
		}
		else
			lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
		break;
	default :
		lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
		break;
    }
    return lResult;
}

void closeMemberWnd()
{
	if (IsWindow(hMemberWnd)) 
	{
		SendMessage(hMemberWnd, WM_CLOSE, 0, 0);
	}
}

static void MBCallEndInfo(HWND hWnd, const char * pName,const char * pPhoneNumber,const char * pDuration,const char * pCost, BOOL IsShow)
{
	HDC hdc;
    if (hIncallGif != NULL)
	{
		EndAnimatedGIF(hIncallGif);
		hIncallGif = NULL;
	}
	if (hBitPortrait != NULL)
	{
		DeleteObject(hBitPortrait);
		hBitPortrait = NULL;
	}
	if (MBCongig_Calling.ALS)
		bIcon = TRUE;
	else
		bIcon = FALSE;
	InitPaint(&incallif_initpaint, FALSE, bIcon);
	hdc = GetDC(hWnd);
	GetImageDimensionFromFile(MBPIC_CALLEND,&sCartoon);
	hIncallGif = StartAnimatedGIFFromFile(hWnd,MBPIC_CALLEND,
		incallif_initpaint.subcapmid.left,
		incallif_initpaint.subcapmid.top, DM_NONE);	
	if (strlen(pName) == 0) 
	{
		if (strlen(pPhoneNumber)== 0) 
			sprintf(endshowinfo, UNKNOWNUMBER);
		else
			InitTxt(hdc,PhoneItem[iCallingOneNum].Calls_Info.PhoneNum,endshowinfo, 176, FALSE);
	}
	else
		InitTxt(hdc,(LPSTR)pName,(LPSTR)endshowinfo, 176, TRUE);	
	ReleaseDC(hWnd,hdc);
	sprintf(endstatusinfo, TXT_DISCONNECTED);
	if (strlen(pDuration) != 0) 
		sprintf(endtimeinfo, pDuration);
	else
		strcpy(endtimeinfo, "");	
	if (strlen(pCost) != 0)
		sprintf(endcostinfo, pCost);
	else
		strcpy(endcostinfo, "");
	if (!IsShow && IsWindowVisible(hWnd)) 
		ShowWindow(hWnd, SW_HIDE);
	if (IsShow && !(IsWindowVisible(hWnd)))
		ShowWindow(hWnd, SW_SHOW);
	SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDC_END_OK,1),(LPARAM)ML("Ok"));
	SendMessage(hWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)""); 
	InvalidateRect(hWnd,NULL,TRUE);
}

void SetShowSummaryFlag(BOOL f)
{
	SFinEmergency = f;
}

BOOL GetShowSummaryFlag()
{
	return SFinEmergency;
}
//蓝牙////////////////////////////////////////////////////////////////////////
void	SetBlueToothStatus(void)//检测到蓝牙设备,设置右softkey文字
{
	printf("\r\n SetBlueToothStatus 检测到蓝牙设备\r\n");
    //if (IsWindow(hCalling))
    {
		printf("\r\n SetBlueToothStatus 检测到蓝牙设备,设置右softkey文字\r\n");
		BTAudioInterfaceType = ME_AUDIO_DIGITAL;
		SendMessage(hCalling,PWM_CREATECAPTIONBUTTON, 
			MAKEWPARAM(IDC_BLUETOOTH,0),(LPARAM)BT_SWITCHTODIGITAL);

        SendMessage(hCalling,PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_HF,1),
            (LPARAM)"");
    }
}
void	SetBlueToothClear(void)//
{
	SendMessage(hCalling,PWM_SETBUTTONTEXT,0,(LPARAM)"");
	BTAudioInterfaceType = ME_AUDIO_ANALOG;
	//SendMessage(hCalling,PWM_CREATECAPTIONBUTTON, 
	//	MAKEWPARAM(IDC_BLUETOOTH,0),(LPARAM)"");
}
//通道切换成功之后调用用于设置文字
//AudioType:当前通道类型
void	SetBlueToothButtonInfo(AudioInterfaceType AudioType)
{
    //if (IsWindow(hCalling))
    {
		if (AudioType == ME_AUDIO_DIGITAL)
        {
			SendMessage(hCalling,PWM_CREATECAPTIONBUTTON, 
				MAKEWPARAM(IDC_BLUETOOTH,0),(LPARAM)BT_SWITCHTOANALOG);
			BTAudioInterfaceType = ME_AUDIO_DIGITAL;
			printf("\r\n SetBlueToothButtonInfo Mobile\r\n");
        }
        else
        {
			BTAudioInterfaceType = ME_AUDIO_ANALOG;
			SendMessage(hCalling,PWM_CREATECAPTIONBUTTON, 
				MAKEWPARAM(IDC_BLUETOOTH,0),(LPARAM)BT_SWITCHTODIGITAL);
			printf("\r\n SetBlueToothButtonInfo BlueTooth\r\n");
        }
        PostMessage(hCalling,MB_ACTIVE,0,0);
    }
}

BOOL    GetMuteStatus(void)
{
    return bMute;
}

void    CallingMute(BOOL bMute)
{
    //PostMessage(hCalling,WM_COMMAND,IDM_MUTE,0);
    ME_MuteControl(hCalling,IDC_ME_MUTE,!bMute);
}

void SetCallTransferTest(BOOL bTest)
{
    bCallTransferTest = bTest;
}

BOOL GetErrorMenu(void)
{
    return bErrorMenu;
}
