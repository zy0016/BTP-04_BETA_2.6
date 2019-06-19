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

#include    "setdial.h"

static const char * pClassName = "SetupDialWndClass";
static const char * pClassNameExt = "SetupDialWndExClass";
static HWND         hWndApp = NULL;
static BOOL         bShow; 
static HINSTANCE    hInstance;
static HWND         hCallWnd;
static int          hCallMsg;

static UDB_ISPINFO  *gIspInfo;

extern  void IntToString( int nValue, char *buf );
extern  void Mms_ChangeConnect(void);
extern	ConnIndexTbl* Sett_GetConnNameTbl(int* iNum);

int nGlobal_Isp_Num; //number of ISP connection

/********************************************************************
* Function     DeleteIspInfo()
* Purpose      delete connection
* Params       hDialList:
               
* Return       
* Remarks      none
**********************************************************************/
BOOL     DeleteIspInfo(int iNum)
{
    int IspNum,i;
    UDB_ISPINFO Isp_info;

    IspNum = IspGetNum();

    if(iNum < 0 || iNum+1>IspNum)
        return FALSE;

	for(i=1;i<IspNum-iNum;i++)
	{
		if(IspReadInfo(&Isp_info, iNum + i))
		IspSaveInfo(&Isp_info,iNum+i-1);
		else break;
	}
    if(-1==truncate(DIALFILE,sizeof(UDB_ISPINFO)*(IspNum-1)))
		return FALSE;

    nGlobal_Isp_Num--;
    return TRUE;
}
/*********************************************************************
*   Function:  SetConnectionUsage
*   Purpose :  
*   Argument:  int nType   
               int nIndex  
*   Return  :  TRUE/FALSE.
*   Explain :  .             
*   NOTE    :  . 
**********************************************************************/

BOOL SetConnectionUsage(int nType, int nIndex)
{
    return SetDefaultDialIndex(nType, nIndex);    
}

/*********************************************************************
*   Function:  SetDefaultDialIndex
*   Purpose :  
*   Argument:  int nType   
               int nIndex  (if nIndex == -1, reset the connection for nType)
*   Return  :  TRUE/FALSE.
*   Explain :  .             
*   NOTE    :  . 
**********************************************************************/
static  BOOL    SetDefaultDialIndex(int nType, int nIndex)
{
    int         idefindex;
    UDB_ISPINFO uispinfo;

    if (nType != DIALDEF_WAP && nType != DIALDEF_MAIL && nType != DIALDEF_MMS)
        return FALSE;

    idefindex = GetDefaultDialIndex(nType);

	if (idefindex == nIndex) //re-do the setting;
		return TRUE;

	if (idefindex >= 0)
	{
		memset(&uispinfo, 0, sizeof(UDB_ISPINFO));
		IspReadInfo(&uispinfo,idefindex);
		uispinfo.PrimarySelect = 0;
		IspSaveInfo(&uispinfo,idefindex);
	}

	if (nIndex == -1)			//set none connection;
		return TRUE;
	
	memset(&uispinfo, 0, sizeof(UDB_ISPINFO));
		
	IspReadInfo(&uispinfo,nIndex);

	switch(nType)
	{
	case DIALDEF_WAP:
		uispinfo.PrimarySelect = nType;
		IspSaveInfo(&uispinfo,nIndex);
		break;
	case DIALDEF_MAIL:
	case DIALDEF_MMS:
		if (uispinfo.PrimarySelect == 0)
		{
			uispinfo.PrimarySelect = nType;
			IspSaveInfo(&uispinfo,nIndex);
		}
		break;
	default:
		break;
	}
	return TRUE;
}
/*********************************************************************
*   Function:  GetDefaultDialIndex
*   Purpose :  
*   Argument:  int nType   
*   Return  :  .
*   Explain :  .             
*   NOTE    :  . 
**********************************************************************/
static  int     GetDefaultDialIndex(int nType)
{
    int         i,inum;
    UDB_ISPINFO ispinfo;

    if (nType != DIALDEF_WAP && nType != DIALDEF_MAIL && nType != DIALDEF_MMS)
        return -1;

    inum = IspIsNum();
    for (i = 0;i < inum;i++)
    {
        IspReadInfo(&ispinfo,i);
        if ((int)ispinfo.PrimarySelect == nType)
            return i;
    }
    return -1;
}

/*************************************************************
*   Function:  IspCreateNewInfo
*   Purpose :  create new connection.
*   Argument:  UDB_ISPINFO  *uIspInfo  
               int          iNum          
*   Return  :  .
*   Explain :  .             
*   NOTE    :  . 
**************************************************************/
BOOL IspCreateNewInfo(UDB_ISPINFO * uIspInfo)
{
    char buf[5];
    int nConnID;
	int nConnNum;
	ConnIndexTbl* pConnTbl = NULL;
	int ConnTblNum = 0;
	int i,nTmp=0;

	nConnNum = IspGetNum();
	
	if (IsFlashAvailable(0) == 0)
	{
		printf("\r\n not enough flash space\r\n");
		return FALSE;
	}

	pConnTbl = Sett_GetConnNameTbl(&ConnTblNum);

	if (pConnTbl != NULL && ConnTblNum > 0)
	{
		do 
		{
			nTmp++;
			memset(uIspInfo->ISPName, 0,  UDB_K_NAME_SIZE + 1);

			sprintf(uIspInfo->ISPName, "%s %d", ML("Connection"), nTmp);
			
			for (i =0; i<ConnTblNum; i++)
			{
				if (strncmp(uIspInfo->ISPName, pConnTbl[i].ConnInfoName, UDB_K_NAME_SIZE + 1) == 0)
					break;
			}
		} while(i < ConnTblNum);
		
	}
	else
	{
		memset(uIspInfo->ISPName, 0,  UDB_K_NAME_SIZE + 1);
		IntToString(IspGetNum()+1, buf);
		strcpy(uIspInfo->ISPName, ML("Connection"));
		strcat(uIspInfo->ISPName, " ");
		strcat(uIspInfo->ISPName, buf);
	}
    
    nConnID = FS_GetPrivateProfileInt(SN_CONNSETT,KN_CONNID,0,SETDIALFILE);
    nConnID++;
    uIspInfo->ISPID = nConnID;

    IspSaveInfo(uIspInfo, IspGetNum());
    nGlobal_Isp_Num++;

    FS_WritePrivateProfileInt(SN_CONNSETT,KN_CONNID,nConnID,SETDIALFILE);    
    return TRUE;
}
/*************************************************************
*   Function:  IspReadInfoByID
*   Purpose :  
*   Argument:  UDB_ISPINFO  *uIspInfo  
               int          iIDnum          
*   Return  :  FALSE: no info of ID; TRUE: find info 
*   Explain :  .             
*   NOTE    :  . 
**************************************************************/
BOOL IspReadInfoByID(UDB_ISPINFO * uIspInfo, int iIDnum)
{
    int nInfo, i;
    nInfo = IspGetNum();
	
    if(iIDnum < 1)
        return FALSE;

    for ( i=0; i<nInfo; i++)
    {
        IspReadInfo(uIspInfo, i);
        if(uIspInfo->ISPID == (unsigned long)iIDnum)
        {
            return TRUE;
        }
    }
    
    return FALSE;
}
/*************************************************************
*   Function:  Sett_ConnInfoChanged()
*   Purpose :  
*   Argument:  
*   Return  :  
*   Explain :               
*   NOTE    :   
**************************************************************/
static void Sett_ConnInfoChanged(void)
{
	Mms_ChangeConnect();
}
/*************************************************************
*   Function:  IspSaveInfo
*   Purpose :  
*   Argument:  UDB_ISPINFO  *uIspInfo  
               int          iNum           
*   Return  :  .
*   Explain :  .             
*   NOTE    :  . 
**************************************************************/
BOOL    IspSaveInfo(UDB_ISPINFO  * uIspInfo ,int iNum)
{
    int		hf;
    long    lfseek = 0;
    DWORD   dfsize = 0;
	struct stat	*buf = NULL;

    if (iNum < 0) 
        return FALSE;
    
    if(-1 == (hf = ZC_Open(DIALFILE, O_RDWR, 0)))
    {
        if (-1 == (hf = ZC_Open(DIALFILE, O_RDWR|O_CREAT, S_IRWXU)))
        {
            return FALSE;
		}
    }
    lfseek = iNum * sizeof(UDB_ISPINFO);
	buf = malloc(sizeof(struct stat));
	
    if(buf == NULL)		
    {
        ZC_Close(hf);
        return FALSE;
    }
	
    memset(buf, 0, sizeof(struct stat));
    stat(DIALFILE, buf);
	
    dfsize = buf->st_size;
	
    free(buf);
    buf = NULL;
   
    if (-1 == lseek(hf, lfseek, SEEK_CUR))
    {
        ZC_Close(hf);
        return FALSE;
    }
	if (strlen(uIspInfo->ISPName) != 0)
	{
		if (-1 != write(hf, uIspInfo, sizeof(UDB_ISPINFO)))
		{
			ZC_Close(hf);
			Sett_ConnInfoChanged(); //Inform app using the connection;
			return TRUE;
		} 
		else
		{
			ZC_Close(hf);
			return FALSE;
		}
	}
	else
	{
		ZC_Close(hf);
		return FALSE;
	}
}
/*************************************************************
*   Function:  IspReadInfo
*   Purpose :  
*   Argument:  UDB_ISPINFO  *uIspInfo 
               int          iNum        
*   Return  :  .
*   Explain :  .             
*   NOTE    :  . 
**************************************************************/
BOOL    IspReadInfo(UDB_ISPINFO  *uIspInfo ,int iNum)
{
    int		   hf;
    int      filesize, len;
	struct stat	*buf = NULL;
	
    if (iNum < 0)
        return FALSE;
    
    if(-1 == (hf = ZC_Open(DIALFILE, O_RDWR, S_IRWXU)))
    {
        SetUp_SetDialCfg();
        hf = ZC_Open(DIALFILE, O_RDWR,0);

        if (-1 == hf)
		{
            return FALSE;
		}
    }
	
	buf = malloc(sizeof(struct stat));
	
    if(buf == NULL)
    {
        ZC_Close(hf);
        return FALSE;
    }
	
    memset(buf, 0, sizeof(struct stat));
    stat(DIALFILE, buf);
	
    filesize = buf->st_size;
	
    free(buf);
    buf = NULL;
	
    if (filesize % sizeof(UDB_ISPINFO) != 0)
    {
        ZC_Close(hf);
        unlink(DIALFILE);
        return FALSE;
    }

    if (-1 == lseek(hf, iNum * sizeof(UDB_ISPINFO), SEEK_SET))
    {
        ZC_Close(hf);
        return FALSE;
    }
	memset(uIspInfo, 0, sizeof(UDB_ISPINFO));

	len = read(hf, uIspInfo, sizeof(UDB_ISPINFO));

    if (sizeof(UDB_ISPINFO) != len )
    {
        ZC_Close(hf);
        return FALSE;
    }
    ZC_Close(hf);
    return TRUE;
}
/**************************************************************
*Build up the dial config file
*
* SetUp_SetDialCfg(void) 
*
***************************************************************/
void    SetUp_SetDialCfg(void) 
{
    int        hf;
//    int         i;
/*
    UDB_ISPINFO uFirstIspInfo[MAX_DEF];

    memset(uFirstIspInfo,0x00,sizeof(UDB_ISPINFO) * MAX_DEF);

    uFirstIspInfo[0].ISPID          = 1 ; //默认从1开始
    uFirstIspInfo[0].DtType         = DIALER_RUNMODE_GPRS;//GPRS mode
    uFirstIspInfo[0].PrimarySelect  = DIALDEF_WAP; //设置为浏览器默认的拨号方式
    strcpy(uFirstIspInfo[0].ISPName,ML("Tmp use for WAP")) ;//"17266"
    strcpy(uFirstIspInfo[0].ISPUserName, "wap");
    strcpy(uFirstIspInfo[0].ISPPassword, "wap");
    strcpy(uFirstIspInfo[0].ISPPhoneNum1,"cmwap") ;//17266

    uFirstIspInfo[1].ISPID          = 2 ; 
    uFirstIspInfo[1].DtType         = DIALER_RUNMODE_DATA; //CSD mode
    uFirstIspInfo[1].PrimarySelect  = DIALDEF_MAIL;//设置为邮件默认的拨号方式
    strcpy(uFirstIspInfo[1].ISPName,ML("Tmp use for Mail")) ;//"17201"
    strcpy(uFirstIspInfo[1].ISPUserName, "172") ;
    strcpy(uFirstIspInfo[1].ISPPassword, "172") ;
    strcpy(uFirstIspInfo[1].ISPPhoneNum1,"cmnet") ;//17201
 */
   
    if(-1 == (hf = ZC_Open(DIALFILE, O_RDWR,0)))
    {
        if (-1 == (hf = ZC_Open(DIALFILE, O_RDWR|O_CREAT,S_IRWXU)))
		{
			return;
		}

        else
        {
/*
            for (i = 0;i < MAX_DEF;i++)
            {
                if (sizeof(UDB_ISPINFO) != (int)write(hf, &uFirstIspInfo[i], sizeof(UDB_ISPINFO)))
                {
                    close(hf);
                    unlink(DIALFILE);
                    return;
                }
			}
            
*/			ZC_Close(hf);
            FS_WritePrivateProfileInt(SN_CONNSETT,KN_CONNID,MAX_DEF,SETDIALFILE);
    	}

    }
	else
	{
		ZC_Close(hf);
	}
}
/********************************************************************
* Function     Setup_GetDialType(int nType)
* Purpose      
* Params       int nType:DIALDEF_WAP/DIALDEF_MAIL
* Return       0: CSD 
               1: GPRS
               -1:False
* Remarks      
**********************************************************************/
int     Setup_GetDialType(int nType)
{
    UDB_ISPINFO  uTempInfo;
    memset(&uTempInfo, 0x00, sizeof(UDB_ISPINFO));

    Setup_ReadDefDial(&uTempInfo, nType);

    if(uTempInfo.DtType < 0 || uTempInfo.ISPID <= 0)
        return -1;

    return uTempInfo.DtType;
}
/**************************************************************
*
*  Setup_ReadDefDial(UDB_ISPINFO  *uIspInfo ,int nType) 
* 
* wap:	DIALDEF_WAP
* Mail: DIALDEF_MAIL
***************************************************************/
BOOL    Setup_ReadDefDial(UDB_ISPINFO  *uIspInfo ,int nType) 
{
    int             i,nNum = 0;
    BOOL            bRead = FALSE;
    int		          hf;
    UDB_ISPINFO     uTempInfo;
	struct stat		*buf=NULL;

    memset(&uTempInfo,0x00, sizeof(UDB_ISPINFO));

    if (nType != DIALDEF_WAP && nType != DIALDEF_MAIL)
        nType = DIALDEF_WAP;

/*
    if(uIspInfo == NULL)
        return FALSE;
*/

    if(-1 == (hf = ZC_Open(DIALFILE, O_RDWR,0)))
    {
/*
        if(nType == DIALDEF_WAP)
        {
            uTempInfo.ISPID         = 1 ; 
            uTempInfo.DtType        = DIALER_RUNMODE_GPRS; 
            uTempInfo.PrimarySelect = DIALDEF_WAP;//nType
            strcpy(uTempInfo.ISPName,DIAL_TXT0) ;//"17266"
            strcpy(uTempInfo.ISPUserName, "wap") ;
            strcpy(uTempInfo.ISPPassword, "wap") ;
            strcpy(uTempInfo.ISPPhoneNum1,"cmwap") ; //17266
        }
        else if(nType == DIALDEF_MAIL)
        {
            uTempInfo.ISPID         = 2 ; 
            uTempInfo.DtType        = DIALER_RUNMODE_GPRS ;
            uTempInfo.PrimarySelect = DIALDEF_MAIL;
            strcpy(uTempInfo.ISPName,DIAL_TXT1) ;//"17201"
            strcpy(uTempInfo.ISPUserName, "172") ;
            strcpy(uTempInfo.ISPPassword, "172") ;
            strcpy(uTempInfo.ISPPhoneNum1,"cmnet") ;//17201
        }
*/
        SetUp_SetDialCfg();
//        memcpy(uIspInfo,&uTempInfo,sizeof(UDB_ISPINFO));
        return FALSE;
    }
	
	buf = malloc(sizeof(struct stat));
	
    if(buf == NULL)
    {
        ZC_Close(hf);
        return NULL;
    }
	
    memset(buf, 0, sizeof(struct stat));
    stat(DIALFILE, buf);
    
	nNum = buf->st_size / sizeof(UDB_ISPINFO);

	free(buf);
    buf = NULL;

    for (i = 0; i < nNum; i++)
    {
        if (sizeof(UDB_ISPINFO) != read(hf, &uTempInfo, sizeof(UDB_ISPINFO)))
        {
            ZC_Close(hf);
            return FALSE;
        }
        if ((int)uTempInfo.PrimarySelect == nType)
        {
            memcpy(uIspInfo, &uTempInfo, sizeof(UDB_ISPINFO));
            bRead = TRUE;
            break;
        }
    }
    ZC_Close(hf);
    return bRead;
}

/*************************************************************
*   Function:  IspGetNum
*   Purpose :  
*   Argument:  none  
*   Return  :  .
*   Explain :  .             
*   NOTE    :  . 
**************************************************************/

int IspGetNum(void)
{
    return nGlobal_Isp_Num;
}

/*************************************************************
*   Function:  IspIsNum
*   Purpose :  
*   Argument:  none  
*   Return  :  .
*   Explain :  .             
*   NOTE    :  . 
**************************************************************/

int     IspIsNum(void)
{
    int      filesize;
    int		 hf;
	struct stat	*buf = NULL;

    if(-1 == (hf = ZC_Open(DIALFILE, O_RDWR,0)))
    {
        return 0;
    }
	buf = malloc(sizeof(struct stat));
	
    if(buf == NULL)
    {
        ZC_Close(hf);
        return 0;
    }
	
    memset(buf, 0, sizeof(struct stat));
    stat(DIALFILE, buf);
	
    filesize = buf->st_size;
	
    free(buf);
    buf = NULL;

    if (filesize % sizeof(UDB_ISPINFO) != 0)
    {
        ZC_Close(hf);
        unlink(DIALFILE);
        return 0;
    }
    else
    {
        ZC_Close(hf);
        nGlobal_Isp_Num = filesize / sizeof(UDB_ISPINFO);
        return (filesize / sizeof(UDB_ISPINFO));
    }
}
