/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : 
 *
 * Purpose  : 
 *
\**************************************************************************/
#include "smsglobal.h"
#include <sms/smsdptch.h>

static HWND hSmsHideWnd = NULL; 
static char szVMN1[ME_PHONENUM_LEN];
static char szVMN2[ME_PHONENUM_LEN];

static SMS_INITDATA smsinitdata;

typedef enum
{
    TIMERID_SET_FORMAT = 300,	
    TIMERID_SET_PROTOCOL,
    TIMERID_GET_SERVICE,
    TIMERID_GET_IND,	
    TIMERID_SET_IND,
    TIMERID_GET_SIMCOUNT,
    TIMERID_GET_COUNT,	
    TIMERID_GET_MELIST,
    TIMERID_SET_STORE,
    TIMERID_GET_SIMPROFILE,
    TIMERID_GET_VMN,
    TIMERID_SET_CB,
    TIMERID_GET_CB,
    TIMERID_SET_MTMEM,
}SMSINIT_TIMERID;

#define OVERTIME		1000
#define RESET_IND_TIME	60000
#define RECOMBINE_TIME	(1000 * 60 * 5)

//#define	WM_SMS_GETSCA			WM_USER+101
#define	WM_SMS_GETINDSTATUS		WM_USER+102
//#define	WM_SMS_SETSCA			WM_USER+103
#define	WM_SMS_SETINDSTATUS		WM_USER+104
#define WM_SMS_MEMCOUNT			WM_USER+105
#define WM_SMS_REPORT			WM_USER+106
#define WM_SMS_SETFORMAT		WM_USER+107
#define WM_SMS_SETPROTOCOL		WM_USER+108
#define WM_SMS_MELIST           WM_USER+109
#define WM_SMS_PORT             WM_USER+110
#define WM_SMS_SIMMEMCOUNT      WM_USER+111
#define WM_SMS_MTMEMCOUNT       WM_USER+112
#define WM_SMS_GETVMN           WM_USER+113
#define WM_SMS_GETVMNRECORD1    WM_USER+114
#define WM_SMS_GETVMNRECORD2    WM_USER+115
#define WM_SMS_GETSIMPROFILE    WM_USER+116
#define WM_SMS_GETPROFILERECORD WM_USER+117
#define WM_SMS_GETSERVICE       WM_USER+118
#define WM_SET_SERVICE          WM_USER+119
#define WM_GET_CB               WM_USER+120
#define WM_GET_CBID             WM_USER+121
#define WM_OPENING_CANCEL       WM_USER+122
#define WM_SMS_SETMTMEM         WM_USER+123

static LRESULT CALLBACK SMSHideWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static BOOL SMSHide_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct);
static void SMSHide_OnPaint(HWND hWnd);
static void SMSHide_OnDestroy(HWND hWnd);
static void SMSHide_OnClose(HWND hWnd);
static void SMSHide_OnTimer(HWND hWnd,UINT id);

static BOOL SetFormat(HWND hWnd);
static BOOL SetProtocol(HWND hWnd);
static BOOL GetSCA(HWND hWnd);
static BOOL GetINDStatus(HWND hWnd);
static BOOL SetInitINDStatus(HWND hWnd);
static BOOL GetSIMSMSCount(HWND hWnd);
static BOOL GetSMSCount(HWND hWnd);
static BOOL GetSMSFromME(HWND hWnd);
static BOOL GetMTSMSCount(HWND hWnd);
static BOOL GetVMN(HWND hWnd);

static void DealSimpleReturn(HWND hWnd,WPARAM wParam,LPARAM lParam);
static void DealGetSCA(HWND hWnd,WPARAM wParam,LPARAM lParam);
static void DealGetINDStatus(HWND hWnd,WPARAM wParam,LPARAM lParam);
static void DealSetINDStatus(HWND hWnd,WPARAM wParam,LPARAM lParam);
static void DealGetSIMSMSCount(HWND hWnd,WPARAM wParam,LPARAM lParam);
static void DealGetSMSCount(HWND hWnd,WPARAM wParam,LPARAM lParam);
static void DealGetMEList(HWND hWnd,WPARAM wParam,LPARAM lParam);
static void DealGetMTSMSCount(HWND hWnd,WPARAM wParam,LPARAM lParam);
static void SMS_SetInitState(BOOL bOK);
static BOOL DealVMN(HWND hWnd,WPARAM wParam,LPARAM lParam);
static BOOL DealVMNRecord(HWND hWnd,WPARAM wParam,LPARAM lParam,int nLine);
static BOOL GetSIMProfile(HWND hWnd);
static void DealSIMProfile(HWND hWnd,WPARAM wParam,LPARAM lParam);
static void DealGetSIMProfileRecord(HWND hWnd,WPARAM wParam,LPARAM lParam);
BOOL SMS_ParseMailboxNumber(unsigned char *SrcData, unsigned int ResultLen, unsigned char *DesData);
static BOOL GetService(HWND hWnd);
static void DealService(HWND hWnd,WPARAM wParam,LPARAM lParam);
static void DealSetService(HWND hWnd,WPARAM wParam,LPARAM lParam);
static void DealGetCB(HWND hWnd,WPARAM wParam,LPARAM lParam);
static void DealGetCBID(HWND hWnd,WPARAM wParam,LPARAM lParam);
static BOOL GetCB(HWND hWnd);
static int SMS_HexStringToInt(const char *pString,unsigned int nDigit);
static void DealSetBC(HWND hWnd,WPARAM wParam,LPARAM lParam);
BOOL SetCB(HWND hWnd);
BOOL SetMTMemory(void);
static void DealSetMTMem(HWND hWnd,WPARAM wParam,LPARAM lParam);

void DealReport(HWND hWnd,WPARAM wParam,LPARAM lParam);
void SMS_SetService(SMS_Service nService);
static int	SMSAnalyse_PhoneNum( char *pContext, char *pPDUStr);
int SMS_AnalyseCPHSRecord(SMS_CPHS_INFO * psms_chps_info,char * str,int len);

static SMSINIT_TIMERID Timerid = TIMERID_SET_FORMAT;
static BOOL bReadSMSOK = FALSE;

/********************************************************************
* Function	   SMS_Init
* Purpose      
* Params	   
* Return	   
* Remarks	   
**********************************************************************/
BOOL SMS_Init(void)
{
	WNDCLASS wc;

    mkdir("/mnt/flash/sms/",S_IRUSR);

    SMS_InitMode();

    SMS_InitData();

    SendNode_Init();

    MaybeDelete();

    SMS_SetInitState(FALSE);

	if (!IsWindow(hSmsHideWnd))
	{
		wc.style         = 0;
		wc.lpfnWndProc   = SMSHideWndProc;
		wc.cbClsExtra    = 0;
		wc.cbWndExtra    = sizeof(int);
		wc.hInstance     = NULL;
		wc.hIcon         = NULL;
		wc.hCursor       = NULL;
		wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
		wc.lpszMenuName  = NULL;
		wc.lpszClassName = "SMSHideWndClass";
		
		if (!RegisterClass(&wc))
			return FALSE;
	
		hSmsHideWnd = CreateWindow(
			"SMSHideWndClass", 
			"", 
			WS_CAPTION,
			0,0,0,0,  
			NULL, 
			NULL,
			NULL, 
			NULL
			);
		if (!hSmsHideWnd)
        {
            UnregisterClass("SMSHideWndClass",NULL);
			return FALSE;
        }

        SMS_EditRegisterClass();

        SMS_DetailRegisterClass();

        SMS_SettingListRegisterClass();
        
        SMS_VViewRegisterClass();

        SMS_ViewRegisterClass();

        SMS_CreateControlSIMWnd();

        SMS_CreateSendControlWnd();

        SMS_VcardVcalPortInit(hSmsHideWnd,WM_SMS_PORT);
	
		ShowWindow(hSmsHideWnd, SW_HIDE);
	}
	return TRUE;
}
/********************************************************************
* Function	   SMSHideWndProc
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static LRESULT CALLBACK SMSHideWndProc(HWND hWnd, UINT message, WPARAM wParam, 
									   LPARAM lParam)
{
    LRESULT lResult = TRUE;

	switch (message)
    {
    case WM_CREATE:
        lResult = SMSHide_OnCreate(hWnd,(LPCREATESTRUCT)(lParam));
        break;

    case WM_PAINT:
        SMSHide_OnPaint(hWnd);
        break;
        
    case WM_CLOSE:
        SMSHide_OnClose(hWnd);
        break;

    case WM_DESTROY:
        SMSHide_OnDestroy(hWnd);
        break;
		
	case WM_TIMER:
		SMSHide_OnTimer(hWnd,(WPARAM)(UINT)(wParam));
		break;	
        
	case WM_SMS_SETFORMAT:
		DealSimpleReturn(hWnd,wParam,lParam);
		break;

	case WM_SMS_SETPROTOCOL:
		DealSimpleReturn(hWnd,wParam,lParam);
		break;	

	case WM_SMS_SETBC:
		DealSetBC(hWnd,wParam,lParam);
		break;	
        
//	case WM_SMS_GETSCA:
//		DealGetSCA(hWnd,wParam,lParam);
//		break;
		
	case WM_SMS_GETINDSTATUS:
		DealGetINDStatus(hWnd,wParam,lParam);
		break;	

    case WM_SMS_SIMMEMCOUNT:
		DealGetSIMSMSCount(hWnd,wParam,lParam);
        break;
        
	case WM_SMS_MEMCOUNT:
		DealGetSMSCount(hWnd,wParam,lParam);
		break;

    case WM_SMS_MELIST:
		DealGetMEList(hWnd,wParam,lParam);
        break;

    case WM_SMS_MTMEMCOUNT:
		DealGetMTSMSCount(hWnd,wParam,lParam);
        break;
        
	case WM_SMS_REPORT:
		DealReport(hWnd,wParam,lParam);
 		break;

	case WM_SMS_SETINDSTATUS:
		DealSetINDStatus(hWnd,wParam,lParam);
		break;

    case WM_SMS_PORT:
        SMS_OnRecvPort(wParam);
        break;

    case WM_SMS_GETVMN:
		DealVMN(hWnd,wParam,lParam);
        break;
        
    case WM_SMS_GETVMNRECORD1:
		DealVMNRecord(hWnd,wParam,lParam,1);
        break;

    case WM_SMS_GETVMNRECORD2:
		DealVMNRecord(hWnd,wParam,lParam,2);
        break;

    case WM_SMS_GETSIMPROFILE:
		DealSIMProfile(hWnd,wParam,lParam);
        break;

    case WM_SMS_GETPROFILERECORD:
        DealGetSIMProfileRecord(hWnd,wParam,lParam);
        break;

    case WM_SMS_GETSERVICE:
        DealService(hWnd,wParam,lParam);
        break;

    case WM_SET_SERVICE:
        DealSetService(hWnd,wParam,lParam);
        break;

    case WM_GET_CB:
        DealGetCB(hWnd,wParam,lParam);
        break;

    case WM_GET_CBID:
        DealGetCBID(hWnd,wParam,lParam);
        break;

    case WM_SMS_SETMTMEM:
		DealSetMTMem(hWnd,wParam,lParam);
        break;

    default:     
        lResult = PDADefWindowProc(hWnd, message, wParam, lParam);
        break;
	}

    return lResult;

}
/*********************************************************************\
* Function	SMSHide_OnCreate
* Purpose   WM_CREATE message handler of the main window
* Params
*			hWnd: Handle of the window
*			lpCreateStruct: Create Structure
* Return
*			TRUE: Success
*			FALSE: Fail
* Remarks
**********************************************************************/
static BOOL SMSHide_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
{	
//	ReadSetupFile(&(Sms_Config));
//	
// 	GetLocalSmsCount();
	
    SMS_SetFull(FALSE);

 	if( GetSIMState() == 1 )
	{
        memset(szVMN1,0,ME_PHONENUM_LEN);

        memset(szVMN2,0,ME_PHONENUM_LEN);
        
        Timerid = TIMERID_SET_FORMAT;

		SetTimer(hWnd,Timerid,OVERTIME,NULL);
	}
    else
    {    
        SMS_InitCBCounter();
        
        SMS_CounterInit();

        SMS_NotifyIdle(); 
    }

	SMSDISPATCH_Register(NULL, (PSMSTransact_f)App_SMSReceive);
	
	return TRUE;
}
/*********************************************************************\
* Function	SMSHide_OnPaint
* Purpose   WM_PAINT message handler of the main window
* Params	hWnd: Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void SMSHide_OnPaint(HWND hWnd)
{
    HDC hdc = BeginPaint(hWnd, NULL);

	EndPaint(hWnd, NULL);

	return;
}
/*********************************************************************\
* Function	SMSHide_OnDestroy
* Purpose   WM_DESTROY message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void SMSHide_OnDestroy(HWND hWnd)
{	
    SMSDISPATCH_UnReg(NULL);

    hSmsHideWnd = NULL;
    
    UnregisterClass("SMSHideWndClass", NULL); 
}
/*********************************************************************\
* Function	SMSHide_OnClose
* Purpose   WM_CLOSE message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void SMSHide_OnClose(HWND hWnd)
{
    DestroyWindow(hWnd);
}
/********************************************************************
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void SMSHide_OnTimer(HWND hWnd,UINT id)
{
	KillTimer(hWnd,id);

#ifdef _SMS_DEBUG_
    printf("\r\n*****SMS Debug Info*****  SMSHide Ontimer timerid = %d \r\n",id);
#endif
        
	switch(id)
	{
	case TIMERID_SET_FORMAT:
		if ( !SetFormat(hWnd) )
			SetTimer(hWnd,id,OVERTIME,NULL);
		break;

	case TIMERID_SET_PROTOCOL:		
		if ( !SetProtocol(hWnd) )
			SetTimer(hWnd,id,OVERTIME,NULL);
		break;
//
//	case TIMERID_GET_SCA:
//		if( !GetSCA(hWnd) )
//			SetTimer(hWnd,id,OVERTIME,NULL);
//		break;
    case TIMERID_GET_SERVICE:
		if( !GetService(hWnd) )
			SetTimer(hWnd,id,OVERTIME,NULL);
        break;
		
	case TIMERID_GET_IND:
		if( !GetINDStatus(hWnd) )
			SetTimer(hWnd,id,OVERTIME,NULL);
		break;
		
	case TIMERID_SET_IND:
		if( !SetInitINDStatus(hWnd) )
			SetTimer(hWnd,id,OVERTIME,NULL);
		break;

    case TIMERID_GET_SIMCOUNT:
		if( !GetSIMSMSCount(hWnd) )
			SetTimer(hWnd,id,OVERTIME,NULL);
        break;
		
	case TIMERID_GET_COUNT:
		if( !GetSMSCount(hWnd) )
			SetTimer(hWnd,id,OVERTIME,NULL);
		break;

    case TIMERID_GET_MELIST:
		if ( !GetSMSFromME(hWnd) )
			SetTimer(hWnd, id, OVERTIME, NULL);
		break;

    case TIMERID_SET_STORE:
		if( !GetMTSMSCount(hWnd) )
			SetTimer(hWnd,id,OVERTIME,NULL);
        break;

    case TIMERID_GET_SIMPROFILE:
        if( !GetSIMProfile(hWnd) )
            SetTimer(hWnd,id,OVERTIME,NULL);
        break;
        
    case TIMERID_GET_VMN:
		if( !GetVMN(hWnd) )
			SetTimer(hWnd,id,OVERTIME,NULL);
        break;

    case TIMERID_GET_CB:
		if( !GetCB(hWnd) )
			SetTimer(hWnd,id,OVERTIME,NULL);
        break;

    case TIMERID_SET_CB:
        if( !SetCB(hWnd) )
            SetTimer(hWnd,id,OVERTIME,NULL);
        break;

    case TIMERID_SET_MTMEM:
        if( !SetMTMemory() )
            SetTimer(hWnd,TIMERID_SET_MTMEM,OVERTIME,NULL);
        break;

    default:
        DealMultiPageSMSTimeOut(id);
        break;
	}
}
/********************************************************************
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL SetFormat(HWND hWnd)
{
	if(ME_SMS_SetFormat(hWnd,WM_SMS_SETFORMAT,TRUE) >= 0)
		return TRUE;
	else
		return FALSE;
}
/********************************************************************
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL SetProtocol(HWND hWnd)
{
	if(ME_SMS_SetProtocol(hWnd,WM_SMS_SETPROTOCOL,TRUE) >= 0)
		return TRUE;
	else
		return FALSE;
}
/********************************************************************
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void DealSimpleReturn(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	if (wParam==ME_RS_SUCCESS)
	{
		Timerid++;
	}
	SetTimer(hWnd,Timerid,OVERTIME,NULL);
}
/********************************************************************
* Function	   GetSCA
* Purpose      
* Params	   
* Return	 ¡¡
* Remarks	   
**********************************************************************/
//static BOOL GetSCA(HWND hWnd)
//{
//	if(ME_SMS_GetSCA(hWnd,WM_SMS_GETSCA) >= 0)
//		return TRUE;
//	else
//		return FALSE;
//}
/********************************************************************
* Function	   DealGetSCA
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
//static void DealGetSCA(HWND hWnd,WPARAM wParam,LPARAM lParam)
//{
//	int buflen;
//    int  nSize = 0,i = 0;
//    char newSCA[ME_PHONENUM_LEN+1];
//    BOOL bExist = FALSE;
//    PSMS_SETTINGCHAIN pHeader = NULL,pTemp;;
//
//	if (wParam == ME_RS_SUCCESS)
//	{
//		buflen = 0;
//		buflen = ME_GetResult(NULL,0);
//
//		if(buflen < 0)
//			newSCA[0] = 0;
//		else
//			ME_GetResult(newSCA,buflen);
//
//        if(strlen(newSCA) != 0)
//        {
//            SMS_ReadSetting(&pHeader);
//                 
//            pTemp = pHeader;
//            
//            while(pTemp)
//            {
//                if(strcmp(newSCA,pTemp->Setting.szSCA) == 0)
//                {
//                    bExist = TRUE;
//
//                    break;
//                }
//
//                pTemp = pTemp->pNext;
//            }
//
//            if(bExist == FALSE)
//            {
//                SMS_SETTINGCHAIN *pNewNode;
//                
//                pNewNode = Setting_New();
//                
//                if(pNewNode != NULL)
//                {   
//                    pNewNode->dwoffset = 0xffff;
//                    pNewNode->Setting.bReplyPath = FALSE;
//                    pNewNode->Setting.bReport = FALSE;
//                    pNewNode->Setting.nConnection = 0;
//                    pNewNode->Setting.nPID = PID_IMPLICIT;
//                    pNewNode->Setting.nValidity = 5;
//                    SMS_FindUseableName(pHeader,pNewNode->Setting.szProfileName);
//                    strcpy(pNewNode->Setting.szSCA,newSCA);
//                    pNewNode->Setting.nID = SMS_FindUseableID(pHeader);
//                    
//                    SMS_WriteSetting(&(pNewNode->Setting),&(pNewNode->dwoffset));
//
//                    Setting_Insert(&pHeader,pNewNode);
//                }
//            }
//
//            Setting_Erase(pHeader);
//        }
//		Timerid++;
//	}
//    
//	SetTimer(hWnd, Timerid, OVERTIME, NULL);
//}
/********************************************************************
* Function	   GetINDStatus
* Purpose      
* Params	   
* Return	 ¡¡  
* Remarks	   
**********************************************************************/
static BOOL GetINDStatus(HWND hWnd)
{
	if(ME_SMS_GetIndStatus(hWnd,WM_SMS_GETINDSTATUS) >= 0)
		return TRUE;
	else
		return FALSE;
}

/********************************************************************
* Function	   DealGetINDStatus
* Purpose      
* Params	   
* Return	   
* Remarks	   
**********************************************************************/
static void DealGetINDStatus(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	int buflen;
	IND_STATUS INDTemp;
	
	if (wParam==ME_RS_SUCCESS)
	{
		buflen = 0;
		buflen = ME_GetResult(NULL,0);
		if(buflen > 0 && buflen <= sizeof(IND_STATUS))
		{
			if(-1 != ME_GetResult(&INDTemp,buflen))
			{
				if(INDTemp.Mt != IND_BUFFER || INDTemp.Ds != IND_BUFFER || INDTemp.Bm != IND_DIRECT)
					Timerid = TIMERID_SET_IND;
				else
					Timerid = TIMERID_GET_SIMCOUNT;
			}
		}
	}
	SetTimer(hWnd,Timerid,OVERTIME,NULL);
}
/********************************************************************
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL SetInitINDStatus(HWND hWnd)
{
	IND_STATUS INDTemp;

	INDTemp.Bm = IND_DIRECT;
	INDTemp.Mt = IND_BUFFER;
	INDTemp.Ds = IND_BUFFER;
	
	if(0 <= ME_SMS_SetIndStatus(hWnd,WM_SMS_SETINDSTATUS,&INDTemp))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
/********************************************************************
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void DealSetINDStatus(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	if (wParam==ME_RS_SUCCESS)
	{
//		Sms_Config.Ind_Status.Mt = INDTemp.Mt;
//		Sms_Config.Ind_Status.Bm = INDTemp.Bm; 
//		Sms_Config.Ind_Status.Ds = INDTemp.Ds;
		Timerid = TIMERID_GET_SIMCOUNT;
	}
#if 0
    Timerid = TIMERID_GET_SIMCOUNT;
#endif

	SetTimer(hWnd,Timerid,OVERTIME,NULL);
}
/********************************************************************
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL GetSMSCount(HWND hWnd)
{
	if(ME_SMS_GetMemStatus(hWnd,WM_SMS_MEMCOUNT,SMS_MEM_ME) >= 0)//????????
		return TRUE;
	else
		return FALSE;
}
/********************************************************************
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void DealGetSMSCount(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	SMSMEM_INFO SmsMem_Info;
	int buflen;
    PSMS_INITDATA pData;
	
	if (wParam==ME_RS_SUCCESS)
	{
		buflen = 0;
		buflen = ME_GetResult(NULL,0);
		if(buflen > 0 && buflen <= sizeof(SMSMEM_INFO))
		{
			if(-1 != ME_GetResult(&(SmsMem_Info),buflen))
			{
                pData = SMS_GetInitData();

                pData->nMECount = SmsMem_Info.nMem1Used;

                pData->nMEMaxCount = SmsMem_Info.nMem1Total;
                
                pData->pMESMSInfo = (SMS_INFO*)malloc(pData->nMEMaxCount*sizeof(SMS_INFO));
                
                if( pData->pMESMSInfo == NULL )
                    return;
                
                memset(pData->pMESMSInfo, 0, pData->nMEMaxCount*sizeof(SMS_INFO));
                
                if(pData->nMECount == 0)
                {
                    Timerid = TIMERID_SET_STORE;
                }
                else
                {
                    Timerid = TIMERID_GET_MELIST;
                    
                    //return;
                }                
            }
        }
	}
    SetTimer(hWnd,Timerid,OVERTIME,NULL);
}
/********************************************************************
* Function	   DealGetMTSMSCount
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void DealGetMTSMSCount(HWND hWnd,WPARAM wParam,LPARAM lParam)
{	
	if (wParam==ME_RS_SUCCESS)
	{
        Timerid = TIMERID_GET_SIMPROFILE;
	}
    SetTimer(hWnd,Timerid,OVERTIME,NULL);
}
/********************************************************************
* Function	   GetVMN
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL GetVMN(HWND hWnd)
{
    if(ME_GetCPHSparameters(hWnd,WM_SMS_GETVMN,0X6F17) >= 0)
        return TRUE;
    else
        return FALSE;
}

/********************************************************************
* Function	   GetService
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL GetService(HWND hWnd)
{
    if(ME_GetSMSservice(hWnd,WM_SMS_GETSERVICE) >= 0)
        return TRUE;
    else
        return FALSE;
}

/********************************************************************
* Function	   DealService
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void DealService(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
    PSMS_INITDATA pData;

    if(wParam != ME_RS_SUCCESS)
    {
        SetTimer(hWnd,Timerid,OVERTIME,NULL);

        return;
    }

    pData = SMS_GetInitData();

    ME_GetResult(&(pData->nService),sizeof(int));

    Timerid++;

    SetTimer(hWnd,Timerid,OVERTIME,NULL);
    
    return;
}
/********************************************************************
* Function	   GetCB
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL GetCB(HWND hWnd)
{
    if(ME_GetCPHSparameters(hWnd,WM_GET_CB,0X6F45) >= 0)
        return TRUE;
    else
        return FALSE;
}
/********************************************************************
* Function	   DealGetCB
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void DealGetCB(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
    PSMS_INITDATA pData;
    CPHS_struct  result;

    if(wParam != ME_RS_SUCCESS)
    {
        SetTimer(hWnd,Timerid,OVERTIME,NULL);
        
        return;
    }

    pData = SMS_GetInitData();

    ME_GetResult(&result,sizeof(CPHS_struct));
    
    if(result.Type == FileContent)
    {    
        SMS_AutoSend();

        SMS_SetInitState(TRUE);
        
        SMS_CounterInit();
        
        DlmNotify(PS_NEWMSG,1);
        
        SMS_NotifyIdle(); 
        
        pData->nSIMCBFileLen = result.Content_Len;

        pData->pszSIMCBFile = (char*)malloc(result.Content_Len*2);

        if(pData->pszSIMCBFile == NULL)
        {
            return;
        }

        memset(pData->pszSIMCBFile,0,result.Content_Len*2);

        ME_ReadCPHSFile(hWnd,WM_GET_CBID, 0X6F45, result.Content_Len);
    }
    else
        SMS_InitCBCounter();

    return;
}
/********************************************************************
* Function	   DealGetCB
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void DealGetCBID(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
    PSMS_INITDATA pData;
    int i,nMax;
    short tempID;
    char* p;
    char  temp[5];
    int nCB[CB_MAX];
    BOOL bNew = FALSE;

	if (wParam != ME_RS_SUCCESS)
    {
        SetTimer(hWnd,Timerid,OVERTIME,NULL);
        
        return;
    }

    pData = SMS_GetInitData();

    ME_GetResult(pData->pszSIMCBFile,pData->nSIMCBFileLen*2);

    nMax = pData->nSIMCBFileLen/2;

    memset(&nCB,0,sizeof(int)*CB_MAX);

    CB_Read(nCB);

    for(i = 1,p = pData->pszSIMCBFile; i <= nMax ; i++)
    {
        temp[0] = 0;

        strncpy(temp,p,4);
        
        temp[4] = 0;

        p += 4;

        tempID = SMS_HexStringToInt(temp,4);

        if(tempID != -1)
        {
            if(nCB[tempID] == -1)
            {
                nCB[tempID] = tempID;
                
                bNew = TRUE;
            }
        }
    }
    
    if(bNew)
        CB_Write(nCB);

    SMS_FREE(pData->pszSIMCBFile);

    SMS_InitCBCounter();

    return;
}

/********************************************************************
* Function	   SMS_HexStringToInt
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static int SMS_HexStringToInt(const char *pString,unsigned int nDigit)
{
    char* pcString;
    char * p;
    int iPriDigit,iCurDigit,i;

    pcString = (char*)malloc(nDigit+1);

    if(pcString == NULL)
        return -1;

    memset(pcString,0,nDigit+1);

    p = pcString;

    if (strlen(pcString) > nDigit)
    {
        SMS_FREE(pcString);
        return -1;
    }

    strncpy(pcString,pString,nDigit);
    
    strupr(pcString);

    iPriDigit = 0;

    iCurDigit = 0;

    for(i = 0 ; i < (int)nDigit ; i++)
    {
        if (*p == '\0')
        {
            SMS_FREE(pcString);
            return iPriDigit * 16 + iCurDigit;
        }
        
        if (('0' <= *p) && (*p <= '9'))
            iCurDigit = *p - '0';
        else if (('A' <= *p) && (*p <= 'F'))
            iCurDigit = *p - 'A' + 10;
        else
        {
            SMS_FREE(pcString);
            return -1;
        }

        iPriDigit = iPriDigit * 16 + iCurDigit;
        
        p++;
    }

    SMS_FREE(pcString);
    return iPriDigit;
}

/********************************************************************
* Function	   DealVMN
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL DealVMN(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
    CPHS_struct  result;
    int  nRecordnum;
    int i;

    ME_GetResult(&result,sizeof(CPHS_struct));
    
    if(result.Type == RecordContent)
    {
        nRecordnum = result.Content_Len / result.Record_Len;

        for(i = 1 ; i <= min(nRecordnum,2) ; i++)
            ME_ReadCPHSRecord(hWnd,WM_SMS_GETVMNRECORD1+i-1, 0X6F17, result.Record_Len, i);
    }
    else
    {
        SMS_SetSIMVMN("",1);
        SMS_SetSIMVMN("",2);
    }
    
    Timerid = TIMERID_SET_CB;
    
    SetTimer(hWnd,Timerid,OVERTIME,NULL);

    return TRUE;
}
/********************************************************************
* Function	   DealVMNRecord
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL DealVMNRecord(HWND hWnd,WPARAM wParam,LPARAM lParam,int nline)
{
    char szSrcVMN[20];
    char szDesVMN[10];
    int nlen;

	if (wParam==ME_RS_SUCCESS)
	{
        nlen = ME_GetResult(szSrcVMN,20);
        
        memset(szDesVMN,0,10);

        SMS_ParseMailboxNumber(szSrcVMN,nlen,szDesVMN);

        SMS_SetSIMVMN(szDesVMN,nline);

        return TRUE;
	}
    return FALSE;
}
/********************************************************************
* Function	   ParseMailboxNumber
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL SMS_ParseMailboxNumber(unsigned char *SrcData, unsigned int ResultLen, unsigned char *DesData)
{
#define DEFINELEN	14
#define NUMBERLEN	10

	unsigned char *NumberLocation = NULL;
	unsigned int i = 0;
    char szTemp[21];
    char szChar[3];
    char* p;
    int  nlen;
    int  n;

	//ResultLen = x + DEFINELEN; Location = x+3 ~ x+12

	NumberLocation = SrcData + ((ResultLen-DEFINELEN) + (3-1))*2;

	for(i=0; i<NUMBERLEN; i++)
	{
		if((szTemp[i*2] = *(NumberLocation+(i*2+1))) == 'F')
		{
			szTemp[i*2] = '\0';
			break;
		}
		
		if((szTemp[i*2+1] = *(NumberLocation+i*2)) == 'F')
		{
			szTemp[i*2+1] = '\0';
			break;
		}
	}

    szTemp[20] = '\0';

    p = szTemp;

    nlen = strlen(p);

    i = 0;

    while(nlen > 0)
    {
        strncpy(szChar,p,2);
        szChar[2] = '\0';
        n = atoi(szChar);

        switch(n)
        {
        case 30:
            DesData[i] = '0';
        	break;
        case 31:
            DesData[i] = '1';
        	break;
        case 32:
            DesData[i] = '2';
            break;
        case 33:
            DesData[i] = '3';
            break;
        case 34:
            DesData[i] = '4';
        	break;
        case 35:
            DesData[i] = '5';
        	break;
        case 36:
            DesData[i] = '6';
            break;
        case 37:
            DesData[i] = '7';
            break;
        case 38:
            DesData[i] = '8';
        	break;
        case 39:
            DesData[i] = '9';
        	break;

        default:
            return FALSE;
        }

        p -= 2;
        nlen -= 2;
        i++;
    }

    return TRUE;
}
/********************************************************************
* Function	   GetSIMProfile
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL GetSIMProfile(HWND hWnd)
{
    if(ME_GetCPHSparameters(hWnd,WM_SMS_GETSIMPROFILE,0X6F42) >= 0)
        return TRUE;
    else
        return FALSE;
}
/********************************************************************
* Function	   DealSIMProfile
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void DealSIMProfile(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
    CPHS_struct  result;
    int     i;
    PSMS_INITDATA pData;

    if(wParam == ME_RS_SUCCESS)
        Timerid = TIMERID_GET_VMN;

    ME_GetResult(&result,sizeof(CPHS_struct));
    
    if(result.Type == RecordContent)
    {        
        pData = SMS_GetInitData();

        pData->SIMProfileData.nRecordLen = result.Record_Len;

        pData->SIMProfileData.nRecordNum = result.Content_Len / result.Record_Len;

        pData->SIMProfileData.pSIMProfile = (PSMS_SIMPROFILE)malloc(pData->SIMProfileData.nRecordNum 
            * sizeof(SMS_SIMPROFILE));

        if(pData->SIMProfileData.pSIMProfile == NULL)
            return;//???????????????

        memset(pData->SIMProfileData.pSIMProfile,0,pData->SIMProfileData.nRecordNum * sizeof(SMS_SIMPROFILE));

        pData->SIMProfileData.nCurProfile = 0;
  
        for(i = 1 ; i <= pData->SIMProfileData.nRecordNum ; i++)
            ME_ReadCPHSRecord(hWnd,WM_SMS_GETPROFILERECORD, 0X6F42, result.Record_Len, i);
    }
    
    SetTimer(hWnd,Timerid,OVERTIME,NULL);
}

/********************************************************************
* Function	   DealGetSIMProfileRecord
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void DealGetSIMProfileRecord(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
    PSMS_INITDATA pData;
    int nLen;
    LPBYTE lpby,p;
    SMS_CPHS_INFO sms_chps_info;
    PSMS_SETTINGCHAIN pHeader = NULL;
    PSMS_SETTINGCHAIN pTemp = NULL;
    BOOL bExist = FALSE;

    if(wParam == ME_RS_SUCCESS)
    {
        pData = SMS_GetInitData();

        lpby = (LPBYTE)malloc(pData->SIMProfileData.nRecordLen*2);

        if(lpby)
        {    
            memset(lpby,0,pData->SIMProfileData.nRecordLen*2);
            
            nLen = ME_GetResult(lpby,pData->SIMProfileData.nRecordLen*2);

            p = lpby - 28;

            memset(&sms_chps_info,0,sizeof(SMS_CPHS_INFO));

            SMS_AnalyseCPHSRecord(&sms_chps_info,lpby,pData->SIMProfileData.nRecordLen*2);
            
            if(strlen(sms_chps_info.SCA) != 0)
            {
                SMS_ReadSetting(&pHeader);
                
                pTemp = pHeader;
                
                while(pTemp)
                {
                    if(strcmp(sms_chps_info.SCA,pTemp->Setting.szSCA) == 0)
                    {
                        SMS_SaveActivateID(pTemp->Setting.nID);

                        bExist = TRUE;
                        
                        break;
                    }
                    
                    pTemp = pTemp->pNext;
                }
                
                if(bExist == FALSE)
                {
                    SMS_SETTINGCHAIN *pNewNode;
                    
                    pNewNode = Setting_New();
                    
                    if(pNewNode != NULL)
                    {   
                        pNewNode->dwoffset = 0xffff;
                        pNewNode->Setting.bReplyPath = FALSE;
                        pNewNode->Setting.bReport = FALSE;
                        if(pData->nService == SMS_GSMpre)
                            pNewNode->Setting.nConnection = 0;
                        else
                            pNewNode->Setting.nConnection = 1;
                        pNewNode->Setting.nPID = sms_chps_info.ProtocolIdentifier;
                        pNewNode->Setting.nValidity = sms_chps_info.ValidityPeriod;
                        SMS_FindUseableName(pHeader,pNewNode->Setting.szProfileName);
                        strcpy(pNewNode->Setting.szSCA,sms_chps_info.SCA);
                        pNewNode->Setting.nID = SMS_FindUseableID(pHeader);
                        
                        SMS_SaveActivateID(pNewNode->Setting.nID);

                        SMS_WriteSetting(&(pNewNode->Setting),&(pNewNode->dwoffset));

                        Setting_Insert(&pHeader,pNewNode);
                    }
                }
                
                Setting_Erase(pHeader);
            }            
            pData->SIMProfileData.nCurProfile++;

            SMS_FREE(lpby);
        }
    }
}
/********************************************************************
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL GetSIMSMSCount(HWND hWnd)
{
	if(ME_SMS_GetMemStatus(hWnd,WM_SMS_SIMMEMCOUNT,SMS_MEM_SM) >= 0)
		return TRUE;
	else
		return FALSE;
}
/********************************************************************
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void DealGetSIMSMSCount(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	SMSMEM_INFO SmsMem_Info;
	int buflen;
    SMS_INITDATA* pData;
	
	if (wParam==ME_RS_SUCCESS)
	{
		buflen = 0;
		buflen = ME_GetResult(NULL,0);
		if(buflen > 0 && buflen <= sizeof(SMSMEM_INFO))
		{
			if(-1 != ME_GetResult(&(SmsMem_Info),buflen))
			{
                pData = SMS_GetInitData();

                pData->nSIMCount = SmsMem_Info.nMem1Used;

                pData->nSIMMaxCount = SmsMem_Info.nMem1Total;

                Timerid = TIMERID_GET_COUNT;

            }
        }
	}
    SetTimer(hWnd,Timerid,OVERTIME,NULL);
}
/*********************************************************************\
* Function	   ReadSMSFromME
* Purpose      
* Params	   
* Return	   
* Remarks	   
**********************************************************************/
static BOOL GetSMSFromME(HWND hWnd)
{
	if (0 <= ME_SMS_List(hWnd, WM_SMS_MELIST, SMS_ALL, TRUE, SMS_MEM_ME))
		return TRUE;
	else
	    return FALSE;
}
/*********************************************************************\
* Function	   GetMTSMSCount
* Purpose      
* Params	   
* Return	   
* Remarks	   
**********************************************************************/
static BOOL GetMTSMSCount(HWND hWnd)
{
	if(ME_SMS_GetMemStatus(hWnd,WM_SMS_MTMEMCOUNT,SMS_MEM_MT) >= 0)
		return TRUE;
	else
		return FALSE;
}
/********************************************************************
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void DealGetMEList(HWND hWnd,WPARAM wParam,LPARAM lParam)
{	
    int buflen, i;
    PSMS_INITDATA pData;

	if (wParam == ME_RS_SUCCESS)
	{
		buflen = 0;
		buflen = ME_GetResult(NULL, 0);

        pData = SMS_GetInitData();

		if (buflen > 0 )
		{			
            if(buflen > (int)(pData->nMECount * sizeof(SMS_INFO)))
                pData->nMECount = buflen / sizeof(SMS_INFO);

			if (-1 != ME_GetResult(pData->pMESMSInfo, buflen))
			{
                SMS_STORE smsstore;
                DWORD dwOffset;

                for (i = pData->nMECount-1; i >= 0 ; i--)
                {
                    
                    if(pData->pMESMSInfo[i].Type == STATUS_REPORT)
                    {
                        if(SMS_UpdateReport(&(pData->pMESMSInfo[i])))
                        {
                            REPORT_NODE*  pNode = NULL;
                            
                            pNode = Report_FindNode(pData->pMESMSInfo[i].MR);
                            
                            if(pNode != NULL)
                                Report_Delete(pNode); 
                        }
                        
                        SIM_Delete(pData->pMESMSInfo[i].Index,SMS_MEM_MT);
                        
                        pData->nMECount--;
                        
                    }
                    else
                    {
                        memset(&smsstore,0,sizeof(SMS_STORE));
                        smsstore.fix.dcs = pData->pMESMSInfo[i].dcs;
                        smsstore.fix.dwDateTime = String2DWORD(pData->pMESMSInfo[i].SendTime);
                        smsstore.fix.Phonelen = strlen(pData->pMESMSInfo[i].SenderNum)+1;
                        strcpy(smsstore.fix.SCA,pData->pMESMSInfo[i].SCA);
                        smsstore.fix.Status = pData->pMESMSInfo[i].Status;
                        smsstore.fix.Stat = MU_STU_UNREAD;
                        smsstore.fix.Type = pData->pMESMSInfo[i].Type;
                        smsstore.fix.Udhlen  = pData->pMESMSInfo[i].udhl;
                        smsstore.pszPhone = pData->pMESMSInfo[i].SenderNum;
                        smsstore.pszUDH = pData->pMESMSInfo[i].UDH;
                        smsstore.fix.Conlen = pData->pMESMSInfo[i].ConLen;
                        smsstore.pszContent = pData->pMESMSInfo[i].Context;
                        
                        if(IsFlashEnough() == TRUE)
                        {
                            //                        SMS_ParseContent(pSMS_Info[i].dcs, pSMS_Info[i].Context , pSMS_Info[i].ConLen ,&smsstore);
                            
                            if(SMS_SaveRecord(SMS_FILENAME_INBOX,&smsstore,&dwOffset))
                            {
                                SMS_ChangeCount(MU_INBOX,SMS_COUNT_UNREAD,1);
                                SMS_ChangeCount(MU_INBOX,SMS_COUNT_ALL,1);
                                
                                SIM_Delete(pData->pMESMSInfo[i].Index,SMS_MEM_ME);
                            }
                            
                            SMS_NotifyIdle();   
                            
                            memset(&(pData->pMESMSInfo[i]),0,sizeof(SMS_INFO));
                            pData->nMECount--;
                        }
                    }
                    
                }
                
                if(pData->nMECount >= pData->nMEMaxCount 
                    && pData->nSIMCount >= pData->nSIMMaxCount)
                {
                    SMS_SetFull(TRUE);
                    
                    SMS_NotifyIdle(); 
                }

                Timerid = TIMERID_SET_STORE;
			}
		}	
	}
	SetTimer(hWnd, Timerid, OVERTIME, NULL);
}
/********************************************************************
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL SMS_Acknowledge(void)
{
    if(ME_SMS_Acknowledge(hSmsHideWnd, WM_SMS_REPORT) < 0)
    {
        SetTimer(hSmsHideWnd, TIMERID_SET_IND, RESET_IND_TIME, NULL);
        return FALSE;
    }
    
    return TRUE;
}
/********************************************************************
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL SMS_RecombineTimeout(int nTimerId)
{
	KillTimer(hSmsHideWnd, nTimerId);
 
    return(SetTimer(hSmsHideWnd, nTimerId, RECOMBINE_TIME, NULL));
}
/********************************************************************
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void SMS_SetInitState(BOOL bOK)
{
    bReadSMSOK = bOK;
}
/********************************************************************
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL SMS_IsInitOK(void)
{
    return bReadSMSOK;
}
/********************************************************************
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
void SMS_SetSIMVMN(char* szVMN,int nline)
{
    if(nline == 1)
        strcpy(szVMN1,szVMN);
    else
        strcpy(szVMN2,szVMN);
}

/********************************************************************
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
void SMS_GetSIMVMN(char* szVMN,int nline)
{
    if(nline == 1)
        strcpy(szVMN,szVMN1);
    else
        strcpy(szVMN,szVMN2);
}
/********************************************************************
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
void SMS_InitData(void)
{
    memset(&smsinitdata,0,sizeof(SMS_INITDATA));
}

/********************************************************************
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
PSMS_INITDATA SMS_GetInitData(void)
{
    return &smsinitdata;
}

/********************************************************************
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
void SMS_SetService(SMS_Service nService)
{
    int * pn;

    pn = GetUserData(hSmsHideWnd);

    *pn = nService;

    if(ME_SetSMSservice(hSmsHideWnd,WM_SET_SERVICE,nService) >= 0 )
        return;
}

/********************************************************************
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void DealSetService(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
    PSMS_INITDATA pData;
    int * pn;

    pn = GetUserData(hWnd);
    
    if(wParam == ME_RS_SUCCESS)
    {
        pData = SMS_GetInitData();
        
        pData->nService = *pn;
    }
}

/********************************************************************
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL SMS_Opening(void)
{
    MSG msg;

    if(SMS_IsInitOK()==FALSE && GetSIMState())
    {
        WaitWin(hSmsHideWnd,TRUE,IDS_OPENING,NULL,NULL,IDS_CANCEL,WM_OPENING_CANCEL);  
           
        while (GetMessage(&msg, NULL, 0, 0))
        {	
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            
            if(msg.hwnd == hSmsHideWnd)
            {
                if(msg.message == WM_OPENING_CANCEL)
                {
                    WaitWin(hSmsHideWnd,FALSE,IDS_OPENING,NULL,NULL,IDS_CANCEL,WM_OPENING_CANCEL);  
                    
                    return FALSE;
                }
            }
            else
            {
                if(SMS_IsInitOK()==TRUE)
                {
                    WaitWin(hSmsHideWnd,FALSE,IDS_OPENING,NULL,NULL,IDS_CANCEL,WM_OPENING_CANCEL);  
                    
                    return TRUE;
                }
            }
        }
        return FALSE;
    }
    else
        return TRUE;
}

/*********************************************************************\
* Function	CB_Set
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
BOOL CB_Set(CELL_BROADCAST *pCB_Setting)
{
    CELL_BROADCAST CB_Setting;
    int nTopicDetection = 0;

    memset(&CB_Setting,0,sizeof(CELL_BROADCAST));

    CB_ReadSetting(&CB_Setting,&nTopicDetection);

    memset(&CB_Setting,0,sizeof(CELL_BROADCAST));

    memcpy(&CB_Setting,pCB_Setting,sizeof(CELL_BROADCAST));

    if(nTopicDetection == 0)
        CB_Setting.ids[0] = 0;
    else
        strcat(CB_Setting.ids,"50");

    if(CB_Setting.mode == 1)
    {
        CB_Setting.mode = TRUE;
        strcpy(CB_Setting.ids,"50");
    }
    else
    {
        CB_Setting.mode = TRUE;
    }

    if(ME_SMS_SetCellBroadcast(hSmsHideWnd, NULL, &CB_Setting) >= 0)
        return TRUE;
    else
        return FALSE;
}

/*********************************************************************\
* Function	SetCB
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
BOOL SetCB(HWND hWnd)
{
    CELL_BROADCAST CB_Setting;
    int nTopicDetection = 0;

    memset(&CB_Setting,0,sizeof(CELL_BROADCAST));

    CB_ReadSetting(&CB_Setting,&nTopicDetection);

    if(nTopicDetection == 0)
        CB_Setting.ids[0] = 0;
    else
        strcat(CB_Setting.ids,"50");

    if(CB_Setting.mode == 1)
    {
        CB_Setting.mode = TRUE;
        strcpy(CB_Setting.ids,"50");
    }
    else 
    {
        CB_Setting.mode = TRUE;
    }

    if(ME_SMS_SetCellBroadcast(hWnd, WM_SMS_SETBC, &CB_Setting) >= 0)
        return TRUE;
    else
        return FALSE;
}

/*********************************************************************\
* Function	DealSetBC
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
static void DealSetBC(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
    if(wParam != ME_RS_SUCCESS)
    {
        SetTimer(hWnd,Timerid,OVERTIME,NULL);

        return;
    }

    Timerid = TIMERID_GET_CB;

    SetTimer(hWnd,Timerid,OVERTIME,NULL);

}
/*********************************************************************\
* Function	   SetMTMemory
* Purpose      
* Params	   
* Return	   
* Remarks	   
**********************************************************************/
BOOL SetMTMemory(void)
{
	if(ME_SMS_GetMemStatus(hSmsHideWnd,WM_SMS_SETMTMEM,SMS_MEM_MT) >= 0)
		return TRUE;
	else
		return FALSE;
}
/*********************************************************************\
* Function	   DealSetMTMem
* Purpose      
* Params	   
* Return	   
* Remarks	   
**********************************************************************/
static void DealSetMTMem(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	if (wParam != ME_RS_SUCCESS)
	{
        SetTimer(hWnd,TIMERID_SET_MTMEM,OVERTIME,NULL);
	}
}
/*********************************************************************\
* Function	   SMS_AnalyseCPHSRecord
* Purpose      
* Params	   
* Return	   
* Remarks	   
**********************************************************************/
int SMS_AnalyseCPHSRecord(SMS_CPHS_INFO * psms_chps_info,char * str,int nLen)
{
    char * pCphs = str;
    char cExcursion[30] = "",cSCA[ME_PHONENUM_LEN + 1] = "";
    unsigned int iSCALen;
    int iIndex;
    char dcs;
    int nloop = nLen;
    char* p = str;
    BOOL  bGet = FALSE;
    int Y;

    memset(psms_chps_info,0x00,sizeof(SMS_CPHS_INFO));

    while(nloop > 0)
    {
        if(*p != 'F')
        {
            bGet = TRUE;
            break;
        }
        p++;
        nloop--;
    }

    if(bGet == FALSE)
        return -1;

    Y = nLen - (28*2);

    pCphs += Y;

    strncpy(cExcursion,pCphs,2);
    psms_chps_info->ParameterIndicators = SMS_HexStringToInt(cExcursion,2);

    pCphs += 2;

    pCphs += 24;
    
    memset(cExcursion,0x00,sizeof(cExcursion));
    strncpy(cExcursion,pCphs,2);//08
    iSCALen = SMS_HexStringToInt(cExcursion,2) * 2;
    
    pCphs += 2;
    if (iSCALen > 0)
    {
        if (strstr(pCphs,"91") != NULL)
        {
            strcpy(psms_chps_info->SCA,"+");
            iIndex = 1;
            pCphs += 2;
        }
        else
        {
            iIndex = 0;
        }
        iSCALen -= 2;
        if (iSCALen > 0)
        {
            strncpy(cSCA,pCphs,iSCALen);
            SMSAnalyse_PhoneNum(psms_chps_info->SCA + iIndex,cSCA);
        }
    }

    if (iIndex == 1)
        pCphs += 20;
    else
        pCphs += 22;

    memset(cExcursion,0x00,sizeof(cExcursion));
    strncpy(cExcursion,pCphs,2);
    if (strcmp(cExcursion,"21") == 0)
        psms_chps_info->ProtocolIdentifier = PID_TELEX;
    else if (strcmp(cExcursion,"22") == 0)
        psms_chps_info->ProtocolIdentifier = PID_TELEFAX;
    else if (strcmp(cExcursion,"24") == 0)
        psms_chps_info->ProtocolIdentifier = PID_VOICE;
    else if (strcmp(cExcursion,"25") == 0)
        psms_chps_info->ProtocolIdentifier = PID_ERMES;
    else if (strcmp(cExcursion,"26") == 0)
        psms_chps_info->ProtocolIdentifier = PID_NPAGING;
    else if (strcmp(cExcursion,"31") == 0)
        psms_chps_info->ProtocolIdentifier = PID_X400;
    else if (strcmp(cExcursion,"32") == 0)
        psms_chps_info->ProtocolIdentifier = PID_EMAIL;
    else
        psms_chps_info->ProtocolIdentifier = PID_IMPLICIT;

    pCphs += 2;
    memset(cExcursion,0x00,sizeof(cExcursion));
    strncpy(cExcursion,pCphs,2);
    dcs = cExcursion[1];
    switch (dcs)
    {
    case '0':
    case '4':
    case '8':
    case 'C':
        psms_chps_info->DataCodingScheme = SMS_CLASS_0;
        break;
    case '1':
    case '5':
    case '9':
    case 'D':
        psms_chps_info->DataCodingScheme = SMS_CLASS_1;
        break;
    case '2':
    case '6':
    case 'A':
    case 'E':
        psms_chps_info->DataCodingScheme = SMS_CLASS_2;
        break;
    case '3':
    case '7':
    case 'B':
    case 'F':
        psms_chps_info->DataCodingScheme = SMS_CLASS_3;
        break;
    }

    pCphs += 2;
    memset(cExcursion,0x00,sizeof(cExcursion));
    strncpy(cExcursion,pCphs,2);
    if (strcmp(cExcursion,"0B") == 0)
        psms_chps_info->ValidityPeriod = SMS_1HOUR;
    else if (strcmp(cExcursion,"47") == 0)
        psms_chps_info->ValidityPeriod = SMS_6HOUR;
    else if (strcmp(cExcursion,"A7") == 0)
        psms_chps_info->ValidityPeriod = SMS_24HOUR;
    else if (strcmp(cExcursion,"A9") == 0)
        psms_chps_info->ValidityPeriod = SMS_72HOUR;
    else if (strcmp(cExcursion,"AD") == 0)
        psms_chps_info->ValidityPeriod = SMS_1WEEK;
    else if (strcmp(cExcursion,"FF") == 0)
        psms_chps_info->ValidityPeriod = SMS_MAXTIME;
    else
        psms_chps_info->ValidityPeriod = SMS_MAXTIME;

    return sizeof(SMS_CPHS_INFO);
}
/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static int	SMSAnalyse_PhoneNum( char *pContext, char *pPDUStr)
{
    int i;
    for( i=0; ;i+=2 )
    {
        if( pPDUStr[i] == 0 )
        {
            pContext[i] = 0;
            if( pContext[i-1]=='F')
                pContext[i-1] = 0;
            break;
        }
        if ( pPDUStr[i+1] == 'A' )
            pContext[i] = '*';
        else if ( pPDUStr[i+1] == 'B' )
            pContext[i] = '#';
        else if ( pPDUStr[i+1] == 'C' )
            pContext[i] = 'a';
        else if ( pPDUStr[i+1] == 'D' )
            pContext[i] = 'b';
        else if ( pPDUStr[i+1] == 'E' )
            pContext[i] = 'c';
        else
            pContext[i] = pPDUStr[i+1];
        
        if ( pPDUStr[i] == 'A' )
            pContext[i+1] = '*';
        else if ( pPDUStr[i] == 'B' )
            pContext[i+1] = '#';
        else if ( pPDUStr[i] == 'C' )
            pContext[i+1] = 'a';
        else if ( pPDUStr[i] == 'D' )
            pContext[i+1] = 'b';
        else if ( pPDUStr[i] == 'E' )
            pContext[i+1] = 'c';
        else
            pContext[i+1] = pPDUStr[i];
    }
    return 0;
}
