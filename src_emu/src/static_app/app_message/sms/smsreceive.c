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

#define IE_FIELD_LEN		1//Information-Element-Identifier "A" 1 octet
#define LIE_FIELD_LEN		1//Length of Information-Element "A" 1 octet
#define IE_SAR_ID		0x00
#define IE_SAR_ID8		0x08
#define IE_SAR_LEN		0x03
#define IE_SAR_LEN8		0x04


#define CTLBLK_COUNT		10
#define NETWORK_PKT_LEN     160

typedef struct tagSMS_Segment
{
	int          	len;
	unsigned char 	pdata[NETWORK_PKT_LEN];
}SMS_SEGMENT;	

typedef struct tagSMS_Datagram
{	
	unsigned short		refnum;
	unsigned char		maxfragment;
	unsigned char		arrivedfragment;
    SMS_STOREFIX        smsstore;
    int                 nTimerId;
    char                szFileName[SMS_FILENAME_MAXLEN];
    char                *pszPhone;
    char                *pszUDH;
	SMS_SEGMENT			*parrSegs;
    struct tagSMS_Datagram *pPioneer;
    struct tagSMS_Datagram *pNext;
}SMS_DATAGRAM;

static SMS_INFO TempInstantSmsInfo;
static SMS_DATAGRAM *pDataGram = NULL;
static PSMS_INSTANTCHAIN pInstant = NULL;
static BOOL bSMSFull = FALSE;
static BOOL bAcknowledge = TRUE;

static SMS_INSTANTCHAIN* Instant_New(void);
static void Instant_Insert(SMS_INSTANTCHAIN* pChainNode);
void Instant_Delete(SMS_INSTANTCHAIN* pChainNode);
static void Instant_Fill(SMS_INSTANTCHAIN* pChainNode,SMS_INFO *psmsinfo);
static void Instant_Erase(void);
static void Datagram_Delete(SMS_DATAGRAM *pChainNode);
static void Datagram_Insert(SMS_DATAGRAM* pChainNode);
static void RecombineSMS(SMS_INFO *pSMSInfo);
BOOL SMS_ParseContent(int dcs , char* pszContent, int nContenLen,SMS_STORE *psmsstore);

BOOL SMS_DeleteFile(char* szFileName);

/********************************************************************
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
int App_SMSReceive(int nType, void *pInfo)
{	
    SMS_INFO *pSMSInfo;
    CBS_INFO *pCBInfo;

    //for normal sms and status report
    switch(nType)
    {
    case NORMAL_SMS:
        
#ifdef _SMS_DEBUG_
        printf("\r\n*****SMS Debug Info*****  SMS_Receive Msg nType = %d, index = %d ,udhl = %d \r\n",
            nType,((SMS_INFO*)pInfo)->Index,((SMS_INFO*)pInfo)->udhl);
#endif
        pSMSInfo = (SMS_INFO*)pInfo;
    
        //for sms class 0 
        if (pSMSInfo->Class == SMS_CLASS_0)
        {            
            if(bAcknowledge == TRUE && SMS_Acknowledge())
            {
                bAcknowledge = FALSE;
                memcpy(&TempInstantSmsInfo, pSMSInfo, sizeof(SMS_INFO));
            }
            return 1;
        }
        else if(pSMSInfo->Class == SMS_CLASS_2)
        {
            SMS_INITDATA *pData;
            
            SMS_FillTrafficLog(pSMSInfo->SenderNum,DRT_RECIEVE,0,1,SMS_RECEIVE);
            
            pData = SMS_GetInitData();
            
            pData->nSIMCount++;
            
            MU_NewMsgArrival(MU_SMS_NOTIFY);
            
            SMS_NewSIMRecord(pSMSInfo);
            
            DlmNotify(PMM_NEWS_ENABLE, 	PMF_MAIN_BKLIGHT);
            DlmNotify (MAKELONG(PMM_NEWS_SET_DELAY,PMS_DELAY_SHUT_LIGHT_MAIN), 20);
          
#ifndef _EMULATE_
            PrioMan_CallMusicEx(PRIOMAN_PRIORITY_SMSAPP, 3000);
#endif
            
            if(pData->nSIMCount >= pData->nSIMMaxCount 
                && pData->nMECount >= pData->nMEMaxCount)
            {
                SMS_SetFull(TRUE);
                
                SMS_NotifyIdle();
            }
            
            return 1;
        }
        
        
        if(pSMSInfo->Index != 0)
        {	
            //normal SMS
            if( pSMSInfo->udhl == 0 )
            {
                SMS_FillTrafficLog(pSMSInfo->SenderNum,DRT_RECIEVE,0,1,SMS_RECEIVE);
                
                if(IsFlashEnough())
                {
                    
                    SMS_STORE smsstore;
                    DWORD     dwOffset;
                    
                    memset(&smsstore,0,sizeof(SMS_STORE));
                    //unread counter++
                    //sms counter++
                    smsstore.fix.dcs = pSMSInfo->dcs;
                    smsstore.fix.dwDateTime = String2DWORD(pSMSInfo->SendTime);
                    smsstore.fix.Phonelen = strlen(pSMSInfo->SenderNum)+1;
                    strcpy(smsstore.fix.SCA,pSMSInfo->SCA);
                    smsstore.fix.Status = pSMSInfo->Status;
                    if(pSMSInfo->Stat == SMS_UNREAD)
                        smsstore.fix.Stat = MU_STU_UNREAD;
                    else
                        smsstore.fix.Stat = MU_STU_READ;
                    smsstore.fix.Type = pSMSInfo->Type;
                    smsstore.fix.Udhlen  = pSMSInfo->udhl;
                    smsstore.pszPhone = pSMSInfo->SenderNum;
                    smsstore.pszUDH = pSMSInfo->UDH;
                    smsstore.fix.Conlen = pSMSInfo->ConLen;
                    smsstore.pszContent = pSMSInfo->Context;
                    
                    if(SMS_SaveRecord(SMS_FILENAME_INBOX,&smsstore,&dwOffset))
                    {
                        //unread counter++
                        //sms counter++
                        SMS_ChangeCount(MU_INBOX,SMS_COUNT_UNREAD,1);
                        SMS_ChangeCount(MU_INBOX,SMS_COUNT_ALL,1);
                        
                        SIM_Delete(pSMSInfo->Index,SMS_MEM_MT);
                    }

                    if(MU_GetCurFolderType() == MU_INBOX)
                    {
#ifdef _SMS_DEBUG_
                        printf("\r\n pSMSInfo->dcs = %d,pSMSInfo.ConLen = %d,pSMSInfo.Context = %s \r\n",pSMSInfo->dcs,pSMSInfo->ConLen,pSMSInfo->Context);
#endif
                        SMS_ParseContent(pSMSInfo->dcs, pSMSInfo->Context , pSMSInfo->ConLen ,&smsstore);
                        
#ifdef _SMS_DEBUG_
                        printf("\r\n smsstore.ConLen = %d,smsstore.Context = %s \r\n",smsstore.fix.Conlen,smsstore.pszContent);
#endif      
                        smsapi_NewRecord(dwOffset,&smsstore);
                        
                        SMS_FREE(smsstore.pszContent);
                    }
                    
                    MU_NewMsgArrival(MU_SMS_NOTIFY);

                    SMS_NotifyIdle();
                    //free

                }
                else
                {
                    SMS_INITDATA *pData;
                    int i;
                    
                    pData = SMS_GetInitData();

                    if(pData->nMEMaxCount == 0)
                    {
                        DlmNotify(PMM_NEWS_ENABLE, 	PMF_MAIN_BKLIGHT);
                        DlmNotify (MAKELONG(PMM_NEWS_SET_DELAY,PMS_DELAY_SHUT_LIGHT_MAIN), 20);
                        
#ifndef _EMULATE_
                        PrioMan_CallMusicEx(PRIOMAN_PRIORITY_SMSAPP, 3000);
#endif
                        return 1;
                    }
                    
                    if(pData->nMECount >= pData->nMEMaxCount)
                    {
                        pData->nSIMCount++;
                        
                        SMS_NewSIMRecord(pSMSInfo);
                    }
                    else
                    {
                        pData->nMECount++;
                        printf("\r\n ###### Recieve ME SMS ###### pData->nMECount = %d\r\n",pData->nMECount);
                        SMS_ChangeCount(MU_INBOX,SMS_COUNT_UNREAD,1);
                        SMS_ChangeCount(MU_INBOX,SMS_COUNT_ALL,1);
                        
                        printf("\r\n ###### Recieve ME SMS ###### pData->nMEMaxCount = %d\r\n",pData->nMEMaxCount);
                        
                        for(i = 0 ; i < pData->nMEMaxCount ; i++)
                        {
                            if(pData->pMESMSInfo[i].Index == 0)
                            {
                                memcpy(&(pData->pMESMSInfo[i]),pSMSInfo,sizeof(SMS_INFO));
                                
                                break;
                            }
                        }
                        
                        if(MU_GetCurFolderType() == MU_INBOX)
                        {
                            smsapi_NewME(pSMSInfo->Index,pSMSInfo);
                        }
                    }
                    
                    MU_NewMsgArrival(MU_SMS_NOTIFY);
                    
                    if(pData->nSIMCount >= pData->nSIMMaxCount 
                        && pData->nMECount >= pData->nMEMaxCount)
                        SMS_SetFull(TRUE);

                    SMS_NotifyIdle();
                    
                }
                
                DlmNotify(PMM_NEWS_ENABLE, 	PMF_MAIN_BKLIGHT);
                DlmNotify (MAKELONG(PMM_NEWS_SET_DELAY,PMS_DELAY_SHUT_LIGHT_MAIN), 20);
                
#ifndef _EMULATE_
                PrioMan_CallMusicEx(PRIOMAN_PRIORITY_SMSAPP, 3000);
#endif
            }
            else//concatenated SMS
            {    
                if(IsFlashEnough() == FALSE)
                {
                    SMS_INITDATA *pData;
                    int i;
                    
                    pData = SMS_GetInitData();
                    
                    if(pData->nMEMaxCount == 0)
                    {
                        DlmNotify(PMM_NEWS_ENABLE, 	PMF_MAIN_BKLIGHT);
                        DlmNotify (MAKELONG(PMM_NEWS_SET_DELAY,PMS_DELAY_SHUT_LIGHT_MAIN), 20);
                        
#ifndef _EMULATE_
                        PrioMan_CallMusicEx(PRIOMAN_PRIORITY_SMSAPP, 3000);
#endif
                        return 1;
                    }
                    
                    if(pData->nMECount >= pData->nMEMaxCount)
                    {
                        pData->nSIMCount++;

                        SMS_NewSIMRecord(pSMSInfo);
                    }
                    else
                    {
                        pData->nMECount++;
                        SMS_ChangeCount(MU_INBOX,SMS_COUNT_UNREAD,1);
                        SMS_ChangeCount(MU_INBOX,SMS_COUNT_ALL,1);
                        
                        for(i = 0 ; i < pData->nMEMaxCount ; i++)
                        {
                            if(pData->pMESMSInfo[i].Index == 0)
                            {
                                memcpy(&(pData->pMESMSInfo[i]),pSMSInfo,sizeof(SMS_INFO));
                                
                                break;
                            }
                        }
                        
                        if(MU_GetCurFolderType() == MU_INBOX)
                        {
                            smsapi_NewME(pSMSInfo->Index,pSMSInfo);
                        }
                    }
                    
                    MU_NewMsgArrival(MU_SMS_NOTIFY);
                    
                    if(pData->nSIMCount >= pData->nSIMMaxCount 
                        && pData->nMECount >= pData->nMEMaxCount)
                        SMS_SetFull(TRUE);
                    
                    SMS_NotifyIdle();

                    DlmNotify(PMM_NEWS_ENABLE, 	PMF_MAIN_BKLIGHT);
                    DlmNotify (MAKELONG(PMM_NEWS_SET_DELAY,PMS_DELAY_SHUT_LIGHT_MAIN), 20);
                    
#ifndef _EMULATE_
                    PrioMan_CallMusicEx(PRIOMAN_PRIORITY_SMSAPP, 3000);
#endif
                    
                }
                else
                {
#ifdef _SMS_DEBUG_
                    printf("\r\n SMSApp:  Receive SMS index = %d \r\n",pSMSInfo->Index);
#endif
                    RecombineSMS(pSMSInfo);
      
#ifdef _SMS_DEBUG_              
                    printf("\r\n SMSApp: RecombineSMS end \r\n");
#endif
                }
            }
            
        }
        //else cmt,csd need to consummate
//        else
//        {
//            SMS_INITDATA *pData;
//
//            pData = SMS_GetInitData();
//
//            if(pData->nMECount >= pData->nMEMaxCount)
//            {
//                pData->nSIMCount++;
//                if(pData->nSIMCount >= pData->nSIMMaxCount)
//                    SMS_NotifyIdle();
//                    
//                //IsWindow(hSIMWnd) Notify refresh list
//            }
//            else
//                pData->nMECount++;
//        }
        break;

    case STATUS_REPORT_SMS:
        
        pSMSInfo = (SMS_INFO*)pInfo;

        if(pSMSInfo->Index != 0)
        {	
            //normal SMS
            if( pSMSInfo->udhl == 0 )
            {
                if(SMS_UpdateReport(pSMSInfo))
                {
                    REPORT_NODE*  pNode = NULL;

                    pNode = Report_FindNode(pSMSInfo->MR);
                    
                    if(pNode != NULL)
                        Report_Delete(pNode); 
      
                    DlmNotify(PMM_NEWS_ENABLE, 	PMF_MAIN_BKLIGHT);
                    DlmNotify (MAKELONG(PMM_NEWS_SET_DELAY,PMS_DELAY_SHUT_LIGHT_MAIN), 20);
                    
#ifndef _EMULATE_
                    PrioMan_CallMusicEx(PRIOMAN_PRIORITY_SMSAPP, 3000);
#endif
                    //MU_NewMsgArrival(MU_SMS_NOTIFY);
                    
                    //free
                }

                SIM_Delete(pSMSInfo->Index,SMS_MEM_MT);
                
            }            
        }
        //else cmt,csd need to consummate
    	break;

    case BC_SMS:
        
        pCBInfo = (CBS_INFO*)pInfo;
        {
//            if(IsFlashEnough())
            {
                char szNr[4];

                szNr[0] = 0;

                itoa(pCBInfo->MId,szNr,10);

                if(pCBInfo->MId == 50)
                {
                    if(Get_ShowCellInfo() == TRUE)
                    {
                        char *pszContent = NULL;
                        int nLen = 0;
                        
                        SMS_ParseContentEx(pCBInfo->Code, pCBInfo->Data , pCBInfo->DataLen ,
                            &pszContent,&nLen);
                        
                        PM_GetCellIdFunc(pszContent);

                        SMS_FREE(pszContent);
                    }
                }

                SMS_FillTrafficLog(szNr,DRT_RECIEVE,0,1,SMS_RECEIVE);
                
                CB_WriteMessage(pCBInfo);
                
                CB_Refresh(pCBInfo);

                DlmNotify(PMM_NEWS_ENABLE, 	PMF_MAIN_BKLIGHT);
                DlmNotify (MAKELONG(PMM_NEWS_SET_DELAY,PMS_DELAY_SHUT_LIGHT_MAIN), 20);
                                
#ifndef _EMULATE_
                PrioMan_CallMusicEx(PRIOMAN_PRIORITY_SMSAPP, 3000);
#endif
            }
        }
        break;

    default:
        break;
    }

	return 1;
}

/*********************************************************************\
* Function	   Instant_New
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static SMS_INSTANTCHAIN* Instant_New(void)
{
    SMS_INSTANTCHAIN* p = NULL;

    p = (SMS_INSTANTCHAIN*)malloc(sizeof(SMS_INSTANTCHAIN));
    memset(p,0,sizeof(SMS_INSTANTCHAIN));

    return p;
}
/*********************************************************************\
* Function	   Instant_New
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void Instant_Fill(SMS_INSTANTCHAIN* pChainNode,SMS_INFO *psmsinfo)
{
    memcpy(&(pChainNode->smsinfo),psmsinfo,sizeof(SMS_INFO));
}
/*********************************************************************\
* Function	   Instant_Insert
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void Instant_Insert(SMS_INSTANTCHAIN* pChainNode)
{
    SMS_INSTANTCHAIN* p;

    if(pInstant)
    {
        p = pInstant;

        while( p )
        {
            if( p->pNext )
                p = p->pNext;
            else
                break;
        }

        p->pNext = pChainNode;
        pChainNode->pPioneer = p;
        pChainNode->pNext = NULL;
    }
    else
    {
        pInstant = pChainNode;
        pChainNode->pNext = NULL;
        pChainNode->pPioneer = NULL;
    }
}
/*********************************************************************\
* Function	   Instant_Delete
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
void Instant_Delete(SMS_INSTANTCHAIN* pChainNode)
{
    if( pChainNode == pInstant )
    {
        if(pChainNode->pNext)
        {
            pInstant = pChainNode->pNext;
            pChainNode->pNext->pPioneer = NULL;
        }
        else
            pInstant = NULL;
    }
    else if( !pChainNode->pNext )
    {
        pChainNode->pPioneer->pNext = NULL;
    }
    else
    {
        pChainNode->pPioneer->pNext = pChainNode->pNext;
        pChainNode->pNext->pPioneer = pChainNode->pPioneer;
    }

    free(pChainNode);
    pChainNode = NULL;
}
/*********************************************************************\
* Function	   Instant_Erase
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void Instant_Erase(void)
{
    SMS_INSTANTCHAIN* p;    
    SMS_INSTANTCHAIN* ptemp;

    p = pInstant;

    while( p )
    {
        ptemp = p->pNext;       
        free(p);
        p = ptemp;
    }

    pInstant = NULL;
}
/*********************************************************************\
* Function	   GetInstant
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
PSMS_INSTANTCHAIN GetInstant(void)
{
    SMS_INSTANTCHAIN* p;    
    SMS_INSTANTCHAIN* ptemp = NULL;

    p = pInstant;

    while( p )
    {
        ptemp = p;

        p = p->pNext;
    }

    return ptemp;
}
/*********************************************************************\
* Function	   RecombineSMS
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void RecombineSMS(SMS_INFO *pSMSInfo)
{
	int i;
	unsigned char maxfragment;
	unsigned char sequence;
	unsigned short refnum;
	BOOL sarflg = FALSE;
    SMS_DATAGRAM *pCurDatagram;
    char szOldPath[PATH_MAXLEN];    

	for(i=0; i<pSMSInfo->udhl; )
	{
#ifdef _SMS_DEBUG_
        printf("\r\n RecombineSMS pSMSInfo->UDH = %s\r\n",pSMSInfo->UDH);
#endif
		switch(pSMSInfo->UDH[i]) 
		{
		case IE_SAR_ID:

			if((i == pSMSInfo->udhl - 1) || ((i + 1 + pSMSInfo->UDH[i+1]) > pSMSInfo->udhl - 1))
				return;
			
			if(pSMSInfo->UDH[i+1] == IE_SAR_LEN)
			{
				refnum = (unsigned short)pSMSInfo->UDH[i+2];
				maxfragment = pSMSInfo->UDH[i+3];
				sequence = pSMSInfo->UDH[i+4];
#ifdef _SMS_DEBUG_
                printf("\r\n RecombineSMS sequence = %d \r\n",sequence);
#endif
				if( (sequence == 0) || (maxfragment == 0) )
					return;
				
				i += 1 + IE_SAR_LEN + 1; /*next UHD IE*/
				sarflg = (maxfragment <= 1) ? FALSE : TRUE;
			}
			else // if length of address is not 4 bits
			{
				return;
			}

			break;
		case IE_SAR_ID8:
			if((i == pSMSInfo->udhl - 1) || ((i + 1 + pSMSInfo->UDH[i+1]) > pSMSInfo->udhl - 1))
				return;
			
			if(pSMSInfo->UDH[i+1] == IE_SAR_LEN8)
			{
				refnum = (unsigned short)(((pSMSInfo->UDH[i+3]) << 8) | (unsigned char)(pSMSInfo->UDH[i+2]));
				maxfragment = pSMSInfo->UDH[i+4];
				sequence = pSMSInfo->UDH[i+5];
                
#ifdef _SMS_DEBUG_
                printf("\r\n RecombineSMS sequence = %d \r\n",sequence);
#endif
				if ((sequence == 0) || (maxfragment == 0) )
					return;
				
				i += 1 + IE_SAR_LEN8 + 1; /*next UHD IE*/
				sarflg = (maxfragment <= 1) ? FALSE : TRUE;
			}
			else // if length of address is not 4 bits				
			{
				return;
			}
			break;
		default:
			
			if((i == pSMSInfo->udhl - 1) || ((i + 1 + pSMSInfo->UDH[i+1]) > pSMSInfo->udhl - 1))
				return;
			
			i += pSMSInfo->UDH[i+1] + IE_FIELD_LEN + LIE_FIELD_LEN; /*next UHD IE*/
		}

	}

    if(sarflg == FALSE)  // only one part
    {
        
        SMS_FillTrafficLog(pSMSInfo->SenderNum,DRT_RECIEVE,0,1,SMS_RECEIVE);
                
        if(IsFlashEnough())
        {
            SMS_STORE smsstore;
            DWORD     dwOffset;

            memset(&smsstore,0,sizeof(SMS_STORE));
            //unread counter++
            //sms counter++
            smsstore.fix.dcs = pSMSInfo->dcs;
            smsstore.fix.Conlen = pSMSInfo->ConLen;
            smsstore.fix.dwDateTime = String2DWORD(pSMSInfo->SendTime);
            smsstore.fix.Phonelen = strlen(pSMSInfo->SenderNum)+1;
            strcpy(smsstore.fix.SCA,pSMSInfo->SCA);
            if(pSMSInfo->Stat == SMS_UNREAD)
                smsstore.fix.Stat = MU_STU_UNREAD;
            else
                smsstore.fix.Stat = MU_STU_READ;
            smsstore.fix.Status = pSMSInfo->Status;
            smsstore.fix.Type = pSMSInfo->Type;
            smsstore.fix.Udhlen  = pSMSInfo->udhl;
            smsstore.pszContent = pSMSInfo->Context;
            smsstore.pszPhone = pSMSInfo->SenderNum;
            smsstore.pszUDH = pSMSInfo->UDH;
            smsstore.fix.Conlen = pSMSInfo->ConLen;
            smsstore.pszContent = pSMSInfo->Context;

            //SMS_ParseContent(pSMSInfo->dcs, pSMSInfo->Context , pSMSInfo->ConLen ,&smsstore);
            
            if(SMS_SaveRecord(SMS_FILENAME_INBOX,&smsstore,&dwOffset))
            {
                SMS_ChangeCount(MU_INBOX,SMS_COUNT_UNREAD,1);
                SMS_ChangeCount(MU_INBOX,SMS_COUNT_ALL,1);
                
                SIM_Delete(pSMSInfo->Index,SMS_MEM_MT);
            }
            if(MU_GetCurFolderType() == MU_INBOX)
            {
                SMS_ParseContent(pSMSInfo->dcs, pSMSInfo->Context , pSMSInfo->ConLen ,&smsstore);
                smsapi_NewRecord(dwOffset,&smsstore);
                SMS_FREE(smsstore.pszContent);
            }

            MU_NewMsgArrival(MU_SMS_NOTIFY);

            SMS_NotifyIdle();
      
            DlmNotify(PMM_NEWS_ENABLE, 	PMF_MAIN_BKLIGHT);
            DlmNotify (MAKELONG(PMM_NEWS_SET_DELAY,PMS_DELAY_SHUT_LIGHT_MAIN), 20);
            
#ifndef _EMULATE_
            PrioMan_CallMusicEx(PRIOMAN_PRIORITY_SMSAPP, 3000);
#endif
            //free
        }
        return;
    }

    pCurDatagram = pDataGram;
    while( pCurDatagram )
    {
        if(strcmp(pCurDatagram->pszPhone,pSMSInfo->SenderNum) == 0 
            && (pCurDatagram->refnum == refnum))
            break;
        else
            pCurDatagram = pCurDatagram->pNext;
    }

    if(pCurDatagram == NULL)
    {
        pCurDatagram = (SMS_DATAGRAM *)malloc(sizeof(SMS_DATAGRAM)+sizeof(SMS_SEGMENT)*maxfragment);
        
        if(pCurDatagram == NULL)
            return;

#ifdef _SMS_DEBUG_
        printf("\r\n SMSApp: pCurDatagram address is ----> %d \r\n",pCurDatagram);
        printf("\r\n SMSApp: pCurDatagram size is ----> %d \r\n",sizeof(SMS_DATAGRAM)+sizeof(SMS_SEGMENT)*maxfragment);
#endif

        memset(pCurDatagram,0,(sizeof(SMS_DATAGRAM)+sizeof(SMS_SEGMENT)*maxfragment));
         
        Datagram_Insert(pCurDatagram);
    }

    
#ifdef _SMS_DEBUG_
    printf("\r\n SMSApp: maxfragment --->  %d\r\n",maxfragment);
#endif

    if(pCurDatagram->arrivedfragment == 0)
    {
        SMS_STORE smsstore;
        DWORD     dwOffset;

#ifdef _SMS_DEBUG_
        printf("\r\n !!!!!! SMSApp: frisr sequence is ----> %d !!!!!! \r\n",sequence);
#endif
        
        memset(&smsstore,0,sizeof(SMS_STORE));
        //unread counter++
        //sms counter++
        smsstore.fix.dcs = pSMSInfo->dcs;
        smsstore.fix.Conlen = pSMSInfo->ConLen;
        smsstore.fix.dwDateTime = String2DWORD(pSMSInfo->SendTime);
        smsstore.fix.Phonelen = strlen(pSMSInfo->SenderNum)+1;
        strcpy(smsstore.fix.SCA,pSMSInfo->SCA);
        if(pSMSInfo->Stat == SMS_UNREAD)
            smsstore.fix.Stat = MU_STU_UNREAD;
        else
            smsstore.fix.Stat = MU_STU_READ;
        
#ifdef _SMS_DEBUG_
        printf("\r\n smsstore.fix.Stat = %d \r\n",smsstore.fix.Stat);
#endif
        smsstore.fix.Status = pSMSInfo->Status;
        smsstore.fix.Type = pSMSInfo->Type;
        smsstore.fix.Udhlen  = pSMSInfo->udhl;
        smsstore.pszContent = pSMSInfo->Context;
        smsstore.pszPhone = pSMSInfo->SenderNum;
        smsstore.pszUDH = pSMSInfo->UDH;
        smsstore.fix.Conlen = pSMSInfo->ConLen;
        smsstore.pszContent = pSMSInfo->Context;
        
        pCurDatagram->refnum = refnum;
        pCurDatagram->maxfragment = maxfragment;
        pCurDatagram->arrivedfragment = 1;
        pCurDatagram->nTimerId = refnum + 1;
        pCurDatagram->smsstore.dcs = pSMSInfo->dcs;
        pCurDatagram->smsstore.dwDateTime = String2DWORD(pSMSInfo->SendTime);
        pCurDatagram->smsstore.Type = pSMSInfo->Type;
        strcpy(pCurDatagram->smsstore.SCA,pSMSInfo->SCA);
        if(pSMSInfo->Stat == SMS_UNREAD)
            pCurDatagram->smsstore.Stat = MU_STU_UNREAD;
        else
            pCurDatagram->smsstore.Stat = MU_STU_READ;
        pCurDatagram->smsstore.Status = pSMSInfo->Status;
        pCurDatagram->smsstore.Udhlen = pSMSInfo->udhl;
        pCurDatagram->smsstore.Phonelen = strlen(pSMSInfo->SenderNum)+1;
        pCurDatagram->pszPhone = malloc(pCurDatagram->smsstore.Phonelen);

#ifdef _SMS_DEBUG_
        printf("\r\n SMSApp: pCurDatagram->pszPhone address is ----> %d \r\n",pCurDatagram->pszPhone);
        printf("\r\n SMSApp: pCurDatagram->pszPhone size is ----> %d \r\n",pCurDatagram->smsstore.Phonelen);
        printf("\r\n SMSApp: pSMSInfo->SenderNum size is ----> %d \r\n",strlen(pSMSInfo->SenderNum));
#endif

        pCurDatagram->pszUDH = malloc(pCurDatagram->smsstore.Udhlen);
        
#ifdef _SMS_DEBUG_
        printf("\r\n SMSApp: pCurDatagram->pszUDH address is ----> %d \r\n",pCurDatagram->pszUDH);
        printf("\r\n SMSApp: pCurDatagram->pszUDH size is ----> %d \r\n",pCurDatagram->smsstore.Udhlen);
#endif

        if(!pCurDatagram->pszPhone || !pCurDatagram->pszUDH)
            return;
        strcpy(pCurDatagram->pszPhone,pSMSInfo->SenderNum);
        strcpy(pCurDatagram->pszUDH,pSMSInfo->UDH);

        pCurDatagram->parrSegs = (SMS_SEGMENT*)(pCurDatagram+1);

#ifdef _SMS_DEBUG_        
        printf("\r\n SMSApp: 0 len is ----> %d \r\n",pSMSInfo->ConLen);
#endif
        pCurDatagram->parrSegs->len = pSMSInfo->ConLen;
        memcpy(pCurDatagram->parrSegs->pdata,pSMSInfo->Context,pSMSInfo->ConLen);

        szOldPath[0] = 0;
        
        getcwd(szOldPath,PATH_MAXLEN);
        
        chdir(PATH_DIR_SMS);
        
        pCurDatagram->szFileName[0] = 0;
        
        SMS_NewFileName(MU_INBOX,pCurDatagram->szFileName);
        
        chdir(szOldPath);

        //SMS_ParseContent(pSMSInfo->dcs, pSMSInfo->Context , pSMSInfo->ConLen ,&smsstore);
        
        if(SMS_SaveRecord(pCurDatagram->szFileName,&smsstore,&dwOffset))
        {
            SIM_Delete(pSMSInfo->Index,SMS_MEM_MT);
        }

#ifdef _SMS_DEBUG_
        printf("\r\n SMSApp: arrivedfragment ----> %d \r\n",pCurDatagram->arrivedfragment);
#endif
        SMS_RecombineTimeout(pCurDatagram->nTimerId);
    }
    else
    {
#ifdef _SMS_DEBUG_
        printf("\r\n !!!!!! SMSApp: sequence is ----> %d !!!!!! \r\n",sequence);
#endif
        pCurDatagram->arrivedfragment++;
       
#ifdef _SMS_DEBUG_ 
        printf("\r\n SMSApp: %d len is ----> %d \r\n",sequence,pSMSInfo->ConLen);
#endif
        (pCurDatagram->parrSegs+sequence-1)->len = pSMSInfo->ConLen;
        memcpy((pCurDatagram->parrSegs+sequence-1)->pdata, pSMSInfo->Context, 
            (pCurDatagram->parrSegs+sequence-1)->len);

#ifdef _SMS_DEBUG_        
        printf("\r\n !!!!!! (pCurDatagram->parrSegs+%d-1)->pdata = %s ,len = %d !!!!!! \r\n",sequence,
            (pCurDatagram->parrSegs+sequence-1)->pdata,(pCurDatagram->parrSegs+sequence-1)->len);
#endif

#ifdef _SMS_DEBUG_        
        printf("\r\n SMSApp: arrivedfragment ----> %d \r\n",pCurDatagram->arrivedfragment);
#endif
        if(pCurDatagram->arrivedfragment == pCurDatagram->maxfragment)
        {
            SMS_STORE smsstore;
            DWORD     dwOffset;
            int       ndatelen,i;
            char      *p,*q;
            
            SMS_FillTrafficLog(pSMSInfo->SenderNum,DRT_RECIEVE,0,pCurDatagram->maxfragment,SMS_RECEIVE);
            
            memset(&smsstore,0,sizeof(SMS_STORE));
            //unread counter++
            //sms counter++
            smsstore.fix.dcs = pSMSInfo->dcs;
            smsstore.fix.Conlen = pSMSInfo->ConLen;
            smsstore.fix.dwDateTime = String2DWORD(pSMSInfo->SendTime);
            smsstore.fix.Phonelen = strlen(pSMSInfo->SenderNum)+1;
            strcpy(smsstore.fix.SCA,pSMSInfo->SCA);
            if(pSMSInfo->Stat == SMS_UNREAD)
                smsstore.fix.Stat = MU_STU_UNREAD;
            else
                smsstore.fix.Stat = MU_STU_READ;
#ifdef _SMS_DEBUG_
            printf("\r\n smsstore.fix.Stat = %d \r\n",smsstore.fix.Stat); 
#endif
            smsstore.fix.Status = pSMSInfo->Status;
            smsstore.fix.Type = pSMSInfo->Type;
            smsstore.fix.Udhlen  = pSMSInfo->udhl;
            smsstore.pszPhone = pSMSInfo->SenderNum;
            smsstore.pszUDH = pSMSInfo->UDH;
            smsstore.pszContent = pSMSInfo->Context;
            ndatelen = 0;
            for(i=0;i<pCurDatagram->maxfragment;i++)
            {
#ifdef _SMS_DEBUG_
                printf("\r\n SMSApp: (pCurDatagram->parrSegs+i)->len is ----> %d \r\n",(pCurDatagram->parrSegs+i)->len);
#endif
                ndatelen +=  (pCurDatagram->parrSegs+i)->len;
#ifdef _SMS_DEBUG_
                printf("\r\n SMSApp: datalen is ----> %d \r\n",ndatelen);
#endif
            }

            if(pSMSInfo->dcs == DCS_UCS2)
                p = (char*)malloc(ndatelen+1);
            else
                p = (char*)malloc(ndatelen+2);

#ifdef _SMS_DEBUG_
            printf("\r\n SMSApp: p address is ----> %d \r\n",p);
            
            printf("\r\n SMSApp: p size is ----> %d \r\n",ndatelen);
#endif
            
            if( p == NULL)
            { 
                //SMS_ParseContent(pSMSInfo->dcs, pSMSInfo->Context , pSMSInfo->ConLen ,&smsstore);
                
                if(SMS_SaveRecord(pCurDatagram->szFileName,&smsstore,&dwOffset))
                {
                    SMS_ChangeCount(MU_INBOX,SMS_COUNT_UNREAD,1);
                    SMS_ChangeCount(MU_INBOX,SMS_COUNT_ALL,1);
                    
                    SIM_Delete(pSMSInfo->Index,SMS_MEM_MT);
                }
                //Show all of sms
                Datagram_Delete(pCurDatagram);
                //kill timer

                if(MU_GetCurFolderType() == MU_INBOX)
                {
                    SMS_ParseContent(pSMSInfo->dcs, pSMSInfo->Context , pSMSInfo->ConLen ,&smsstore);
                    smsapi_NewRecord(dwOffset,&smsstore);
                    SMS_FREE(smsstore.pszContent);
                }
                
                MU_NewMsgArrival(MU_SMS_NOTIFY);
                
                SMS_NotifyIdle();
                
                DlmNotify(PMM_NEWS_ENABLE, 	PMF_MAIN_BKLIGHT);
                DlmNotify (MAKELONG(PMM_NEWS_SET_DELAY,PMS_DELAY_SHUT_LIGHT_MAIN), 20);
                
#ifndef _EMULATE_
                PrioMan_CallMusicEx(PRIOMAN_PRIORITY_SMSAPP, 3000);
#endif
                return;
            }
            q = p;
            
            if(pSMSInfo->dcs == DCS_UCS2)
                memset(p,0,sizeof(ndatelen+2));
            else
                memset(p,0,sizeof(ndatelen+1));
            
            for(i=0;i<pCurDatagram->maxfragment;i++)
            {
                memcpy(q,(pCurDatagram->parrSegs+i)->pdata,(pCurDatagram->parrSegs+i)->len);
                q += (pCurDatagram->parrSegs+i)->len;
            }
            *q = '\0';
            if(pSMSInfo->dcs == DCS_UCS2)
            {
                q++;
                *q = '\0';
            }
      
            smsstore.pszContent = p;
            smsstore.fix.Conlen = ndatelen;

            //need modify
            
            if(SMS_SaveRecord(SMS_FILENAME_INBOX,&smsstore,&dwOffset))
            {
                SMS_ChangeCount(MU_INBOX,SMS_COUNT_UNREAD,1);
                SMS_ChangeCount(MU_INBOX,SMS_COUNT_ALL,1);

                SMS_DeleteFile(pCurDatagram->szFileName);
                SIM_Delete(pSMSInfo->Index,SMS_MEM_MT);
            }
            
            if(MU_GetCurFolderType() == MU_INBOX)
            {
                SMS_ParseContent(pSMSInfo->dcs, p , ndatelen ,&smsstore);
                smsapi_NewRecord(dwOffset,&smsstore);
                SMS_FREE(smsstore.pszContent);
            }

            MU_NewMsgArrival(MU_SMS_NOTIFY);

            SMS_NotifyIdle();
            
            DlmNotify(PMM_NEWS_ENABLE, 	PMF_MAIN_BKLIGHT);
            DlmNotify (MAKELONG(PMM_NEWS_SET_DELAY,PMS_DELAY_SHUT_LIGHT_MAIN), 20);
            
#ifndef _EMULATE_
            PrioMan_CallMusicEx(PRIOMAN_PRIORITY_SMSAPP, 3000);
#endif
            
            //delete node
            Datagram_Delete(pCurDatagram);

#ifdef _SMS_DEBUG_
            printf("\r\n SMSApp: p address is ----> %d \r\n",p);
#endif
            SMS_FREE(p);
            //kill timer

#ifdef _SMS_DEBUG_
            printf("\r\n SMSApp: Free p end \r\n");
#endif
        }
        else
        {
            
            SMS_STORE smsstore;
            DWORD     dwOffset;
            
            memset(&smsstore,0,sizeof(SMS_STORE));
            //unread counter++
            //sms counter++
            smsstore.fix.dcs = pSMSInfo->dcs;
            smsstore.fix.Conlen = pSMSInfo->ConLen;
            smsstore.fix.dwDateTime = String2DWORD(pSMSInfo->SendTime);
            smsstore.fix.Phonelen = strlen(pSMSInfo->SenderNum)+1;
            strcpy(smsstore.fix.SCA,pSMSInfo->SCA);
            if(pSMSInfo->Stat == SMS_UNREAD)
                smsstore.fix.Stat = MU_STU_UNREAD;
            else
                smsstore.fix.Stat = MU_STU_READ;
            smsstore.fix.Status = pSMSInfo->Status;
            smsstore.fix.Type = pSMSInfo->Type;
            smsstore.fix.Udhlen  = pSMSInfo->udhl;
            smsstore.pszContent = pSMSInfo->Context;
            smsstore.pszPhone = pSMSInfo->SenderNum;
            smsstore.pszUDH = pSMSInfo->UDH;
            smsstore.fix.Conlen = pSMSInfo->ConLen;
            smsstore.pszContent = pSMSInfo->Context;
            
            //SMS_ParseContent(pSMSInfo->dcs, pSMSInfo->Context , pSMSInfo->ConLen ,&smsstore);
            
            if(SMS_SaveRecord(pCurDatagram->szFileName,&smsstore,&dwOffset))
            {
                SIM_Delete(pSMSInfo->Index,SMS_MEM_MT);
            }
            SMS_RecombineTimeout(pCurDatagram->nTimerId);
        }
    }

	return;
}
/*********************************************************************\
* Function	   Datagram_Delete
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void Datagram_Delete(SMS_DATAGRAM *pChainNode)
{
    if( pChainNode == pDataGram )
    {
        if(pChainNode->pNext)
        {
            pDataGram = pChainNode->pNext;
            pChainNode->pNext->pPioneer = NULL;
        }
        else
            pDataGram = NULL;
    }
    else if( !pChainNode->pNext )
    {
        pChainNode->pPioneer->pNext = NULL;
    }
    else
    {
        pChainNode->pPioneer->pNext = pChainNode->pNext;
        pChainNode->pNext->pPioneer = pChainNode->pPioneer;
    }

#ifdef _SMS_DEBUG_
    printf("\r\n SMSApp: pChainNode->pszPhone address is ----> %d \r\n",pChainNode->pszPhone);
#endif
    SMS_FREE(pChainNode->pszPhone);
#ifdef _SMS_DEBUG_
    printf("\r\n SMSApp: pChainNode->pszUDH address is ----> %d \r\n",pChainNode->pszUDH);
#endif
    SMS_FREE(pChainNode->pszUDH);
    {
        int i;
        i = 0;
    }

#ifdef _SMS_DEBUG_
    printf("\r\n SMSApp: pChainNode address is ----> %d \r\n",pChainNode);
#endif
    SMS_FREE(pChainNode);
#ifdef _SMS_DEBUG_
    printf("\r\n SMSApp: pChainNode address is ----> end \r\n");
#endif
}
/*********************************************************************\
* Function	   Datagram_Insert
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void Datagram_Insert(SMS_DATAGRAM* pChainNode)
{
    SMS_DATAGRAM* p;

    if(pDataGram)
    {
        p = pDataGram;

        while( p )
        {
            if( p->pNext )
                p = p->pNext;
            else
                break;
        }

        p->pNext = pChainNode;
        pChainNode->pPioneer = p;
        pChainNode->pNext = NULL;
    }
    else
    {
        pDataGram = pChainNode;
        pChainNode->pNext = NULL;
        pChainNode->pPioneer = NULL;
    }
}
/*********************************************************************\
* Function	   SMS_SaveRecord
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL SMS_SaveRecord(char* szFileName,SMS_STORE *psmsstore,DWORD* pdwOffset)
{
    char szOldPath[PATH_MAXLEN];
    int  f;
    struct stat statbuf;
    BYTE byDel;
    DWORD dwRecSize;
    int  nConlen = 0,nPhonelen = 0,nUdhlen = 0;

    szOldPath[0] = 0;
    
    getcwd(szOldPath,PATH_MAXLEN);

    chdir(PATH_DIR_SMS);

    f = open(szFileName,O_RDWR|O_CREAT, S_IRWXU);

    if( f == -1 )
    {
        chdir(szOldPath);
        return FALSE;
    }
    
#ifdef _SMS_DEBUG_
    printf("\r\n*****SMS Debug Info*****  SMS_SaveRecord  file handle = %d \r\n",f);
#endif

    memset(&statbuf,0,sizeof(struct stat));
    stat(szFileName,&statbuf);
    *pdwOffset = statbuf.st_size;

    lseek(f,statbuf.st_size,SEEK_SET);

    if(psmsstore->fix.Conlen > 0)
        nConlen = psmsstore->fix.Conlen;
    
    if(psmsstore->fix.Phonelen > 0)
        nPhonelen = psmsstore->fix.Phonelen;
    
    if(psmsstore->fix.Udhlen > 0)
        nUdhlen = psmsstore->fix.Udhlen;

    byDel = SMS_USED;
    write(f,&byDel,sizeof(BYTE));
    dwRecSize = sizeof(SMS_STOREFIX)+nConlen+nPhonelen+nUdhlen;
    write(f,&dwRecSize,sizeof(DWORD));
    write(f,&(psmsstore->fix),sizeof(SMS_STOREFIX));
    write(f,psmsstore->pszPhone,nPhonelen);
    write(f,psmsstore->pszContent,nConlen);
    write(f,psmsstore->pszUDH,nUdhlen);

    close(f);

    chdir(szOldPath);

    return TRUE;
}
/*********************************************************************\
* Function	   SMS_DeleteFile
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL SMS_DeleteFile(char* szFileName)
{
    char szOldPath[PATH_MAXLEN];

    szOldPath[0] = 0;
    
    getcwd(szOldPath,PATH_MAXLEN);

    chdir(PATH_DIR_SMS);

    remove(szFileName);

    chdir(szOldPath);

    return TRUE;
}
/*********************************************************************\
* Function	   SMS_ParseContent
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL SMS_ParseContent(int dcs , char* pszContent, int nContenLen,SMS_STORE *psmsstore)
{
    if (dcs == DCS_GSM)
    {
        psmsstore->fix.Conlen = GSMToMultiByte(pszContent, nContenLen, NULL, 0, NULL, NULL);

        psmsstore->pszContent = malloc(psmsstore->fix.Conlen+1);

        if(psmsstore->pszContent == NULL)
            return FALSE;
        
        GSMToMultiByte(pszContent, nContenLen, (LPWSTR)psmsstore->pszContent, psmsstore->fix.Conlen+1, NULL, NULL);

        psmsstore->pszContent[psmsstore->fix.Conlen] = 0;

        psmsstore->fix.Conlen++;
    }
    else if (dcs == DCS_UCS2)
    {
        psmsstore->fix.Conlen = WideCharToMultiByte(CP_ACP, 0, (LPWSTR)pszContent, nContenLen/2, 
            pszContent, 0, NULL, NULL);

        psmsstore->pszContent = malloc(psmsstore->fix.Conlen+1);

        if(psmsstore->pszContent == NULL)
            return FALSE;
        
        WideCharToMultiByte(CP_ACP, 0, (LPWSTR)pszContent, nContenLen/2, 
            psmsstore->pszContent, psmsstore->fix.Conlen+1, NULL, NULL);

        psmsstore->pszContent[psmsstore->fix.Conlen] = 0;

        psmsstore->fix.Conlen++;
    }
    else if (dcs == DCS_8BIT)
	{
		psmsstore->fix.Conlen = nContenLen;

        psmsstore->pszContent = malloc(psmsstore->fix.Conlen+1);

        if(psmsstore->pszContent == NULL)
            return FALSE;

        memcpy(psmsstore->pszContent,pszContent,psmsstore->fix.Conlen);

        psmsstore->pszContent[psmsstore->fix.Conlen] = 0;
        
        psmsstore->fix.Conlen++;
	}
	
    return TRUE;
}
/*********************************************************************\
* Function	   DealReport
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
void DealReport(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
    bAcknowledge = TRUE;

    if (wParam==ME_RS_SUCCESS)
	{        
        SMS_INSTANTCHAIN* pNewNode;

        SMS_FillTrafficLog(TempInstantSmsInfo.SenderNum,DRT_RECIEVE,0,1,SMS_RECEIVE);

        DlmNotify(PMM_NEWS_ENABLE, 	PMF_MAIN_BKLIGHT);
        DlmNotify (MAKELONG(PMM_NEWS_SET_DELAY,PMS_DELAY_SHUT_LIGHT_MAIN), 20);
        
#ifndef _EMULATE_
        PrioMan_CallMusicEx(PRIOMAN_PRIORITY_SMSAPP, 3000);
#endif
        
        pNewNode = Instant_New();
        
        if(!pNewNode)
            return;
        
        Instant_Fill(pNewNode,&TempInstantSmsInfo);
        
        Instant_Insert(pNewNode); 
        
        CallAppEntryEx("SMS",0,0);
    }
}
/*********************************************************************\
* Function	   DealReport
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
void DealMultiPageSMSTimeOut(int Timerid)
{
    SMS_DATAGRAM* pDataTemp;
    SMS_STORE smsstore;
    DWORD     dwOffset;
    int       ndatelen,i;
    char      *p,*q;
    int       nSeg;
    
    pDataTemp = pDataGram;
    
    while (pDataTemp)
    {
        if(pDataTemp->nTimerId == Timerid)
        {
            ndatelen = 0;

            nSeg = 0;
            
            for(i=0;i<pDataTemp->maxfragment;i++)
            {
                if((pDataTemp->parrSegs+i)->len > 0)
                    nSeg++;

                ndatelen +=  (pDataTemp->parrSegs+i)->len;
            }
            
            SMS_FillTrafficLog(pDataTemp->pszPhone,DRT_RECIEVE,0,nSeg,SMS_RECEIVE);            
            
            memset(&smsstore,0,sizeof(SMS_STORE));
            //unread counter++
            //sms counter++
            smsstore.fix.dcs = pDataTemp->smsstore.dcs;
            smsstore.fix.Conlen = pDataTemp->smsstore.Conlen;
            smsstore.fix.dwDateTime = pDataTemp->smsstore.dwDateTime;
            smsstore.fix.Phonelen = strlen(pDataTemp->pszPhone)+1;
            strcpy(smsstore.fix.SCA,pDataTemp->smsstore.SCA);
            smsstore.fix.Stat = pDataTemp->smsstore.Stat;
            smsstore.fix.Status = pDataTemp->smsstore.Status;
            smsstore.fix.Type = pDataTemp->smsstore.Type;
            smsstore.fix.Udhlen  = pDataTemp->smsstore.Udhlen;
            smsstore.pszPhone = pDataTemp->pszPhone;
            smsstore.pszUDH = pDataTemp->pszUDH;
            
            ndatelen = 0;
            
            for(i=0;i<pDataTemp->maxfragment;i++)
            {
                ndatelen +=  (pDataTemp->parrSegs+i)->len;
            }
            
            if(smsstore.fix.dcs == DCS_UCS2)
                p = (char*)malloc(ndatelen+1);
            else
                p = (char*)malloc(ndatelen+2);
            
            if( p == NULL)
            { 
                ;
                //                SMS_ChangeCount(MU_INBOX,SMS_COUNT_UNREAD,1);
                //                SMS_ChangeCount(MU_INBOX,SMS_COUNT_ALL,1);
                //                
                //                if(SMS_SaveRecord(pCurDatagram->szFileName,&smsstore,&dwOffset))
                //                {
                //                    SIM_Delete(pSMSInfo->Index,SMS_MEM_MT);
                //                }
                //                //Show all of sms
                //                Datagram_Delete(pCurDatagram);
                //                //kill timer

                return;
            }

            q = p;
            
            if(smsstore.fix.dcs == DCS_UCS2)
                memset(p,0,sizeof(ndatelen+2));
            else
                memset(p,0,sizeof(ndatelen+1));
            
            for(i=0;i<pDataTemp->maxfragment;i++)
            {
                memcpy(q,(pDataTemp->parrSegs+i)->pdata,(pDataTemp->parrSegs+i)->len);
                q += (pDataTemp->parrSegs+i)->len;
            }
            *q = '\0';
            if(smsstore.fix.dcs == DCS_UCS2)
            {
                q++;
                *q = '\0';
            }
            
            smsstore.pszContent = p;
            smsstore.fix.Conlen = ndatelen;
            
            //need modify
            
            if(SMS_SaveRecord(SMS_FILENAME_INBOX,&smsstore,&dwOffset))
            {
                SMS_ChangeCount(MU_INBOX,SMS_COUNT_UNREAD,1);
                SMS_ChangeCount(MU_INBOX,SMS_COUNT_ALL,1);
                
                SMS_DeleteFile(pDataTemp->szFileName);
            }
            
            if(MU_GetCurFolderType() == MU_INBOX)
            {
                SMS_ParseContent(smsstore.fix.dcs, p , ndatelen ,&smsstore);
                smsapi_NewRecord(dwOffset,&smsstore);
                SMS_FREE(smsstore.pszContent);
            }
            
            MU_NewMsgArrival(MU_SMS_NOTIFY);
            
            SMS_NotifyIdle();
            
            DlmNotify(PMM_NEWS_ENABLE, 	PMF_MAIN_BKLIGHT);
            DlmNotify (MAKELONG(PMM_NEWS_SET_DELAY,PMS_DELAY_SHUT_LIGHT_MAIN), 20);
            
#ifndef _EMULATE_
            PrioMan_CallMusicEx(PRIOMAN_PRIORITY_SMSAPP, 3000);
#endif
            
            //delete node
            Datagram_Delete(pDataTemp);
            
            SMS_FREE(p);
            //kill timer

            return;
        }
        else
            pDataTemp = pDataTemp->pNext;
    }
}

/*********************************************************************\
* Function	   SMS_IsFull
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL SMS_IsFull(void)
{
    return bSMSFull;
}
/*********************************************************************\
* Function	   SMS_SetFull
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
void SMS_SetFull(BOOL bFull)
{
    bSMSFull = bFull;
}
/*********************************************************************\
* Function	   SMS_FillTrafficLog
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL SMS_FillTrafficLog(char* pszNum,DRTTYPE direction,unsigned long SmsID,int nFrag,SMSSTATUS status)
{
    PLOGRECORD plogrecord;
    int len;
    SYSTEMTIME sy;
    unsigned long lTime;
    
    len = strlen(pszNum);
    plogrecord = (PLOGRECORD)malloc(sizeof(LOGRECORD)+len);
    if(plogrecord != NULL)
    {
        memset(plogrecord,0,sizeof(LOGRECORD)+len);
        
        memset(&sy,0,sizeof(SYSTEMTIME));
        
        GetLocalTime(&sy);
        
        LOG_STtoFT(&sy,&lTime);
        
        plogrecord->begintm = lTime;
        plogrecord->direction = direction;
        plogrecord->type = TYPE_SMS;
        plogrecord->u.sms.SmsID = SmsID;
        plogrecord->u.sms.smscounter = nFrag;
        plogrecord->u.sms.status = status;
        
        strcpy(plogrecord->APN,pszNum);
        
        Log_Write(plogrecord);
        
        SMS_FREE(plogrecord);

        return TRUE;
    }
    return FALSE;
}
