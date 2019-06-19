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

typedef struct tagSMS_Ctrl
{
    int   smshandle;
    unsigned short port;
}SMS_CTRL,*PSMS_CTRL;

static SMS_CTRL smsctrl[2];

#define SMS_WAPCARD_PORT        9204
#define SMS_WAPCAL_PORT         9205

static BOOL SMS_NewVcardVcal(char* pszPostFix,char* pszFileName);
static BOOL SaveAsVcal(Address_t* psrcaddr, unsigned char* pData, int nDataLen,char* pszFileName);
static BOOL SaveAsVcard(Address_t* psrcaddr, unsigned char* pData, int nDataLen,char* pszFileName);

static void SMS_CharsetTransfer(char* pSrc,int nLen,char* pData,int nMaxLen,int nCharset);
char * AB_strstr (const char * str1,const char * str2,int nstr1len);

BOOL SMS_VcardVcalPortInit(HWND hMsgWnd,UINT message)
{
    memset(smsctrl,0,sizeof(SMS_CTRL));
	
    //register vcard port 
    smsctrl[0].port = SMS_WAPCARD_PORT;

	if((smsctrl[0].smshandle = SMSDISPATCH_Open()) < 0 )
		return FALSE;
	
	SMSDISPATCH_Ioctl(smsctrl[0].smshandle, SMS_WAPCARD_PORT, hMsgWnd, message);
	
	//register vcal port
	smsctrl[1].port = SMS_WAPCAL_PORT;

	if((smsctrl[1].smshandle = SMSDISPATCH_Open()) < 0 )
		return FALSE;

	SMSDISPATCH_Ioctl(smsctrl[1].smshandle, SMS_WAPCAL_PORT, hMsgWnd, message);

	return TRUE;
}
/**************************************************************************\
函数功能：	SMS_SendPortSMS数据发送函数。
参数意义：	paddr			需要发送的目的地址。
			pData			需要发送的数据。
简单描述：	本函数用来使用SMS函数发送数据。
			事实上，现在我们的系统还不支持数据发送，我们实现本函数等待系统支持。
返回数值：	成功返回发送长度，失败返回SYSERR。
\**************************************************************************/
int SMS_SendPortSMS(HWND hSendCtlWnd, UINT uMsgCmd,int nMsgType, SMS_STORE * psmsstore)
{
    int ret = -1;
    Address_t destaddr;
    PORT_SEND portsend;

    memset(&destaddr,0,sizeof(Address_t));
    memset(&portsend,0,sizeof(PORT_SEND));

    if(nMsgType == MU_MSG_VCARD)
    {
        strcpy(destaddr.loweraddr,psmsstore->pszPhone);
        destaddr.port = smsctrl[0].port;
        strcpy(destaddr.SCA,psmsstore->fix.SCA);

        portsend.CodeMode = psmsstore->fix.dcs;
        portsend.DataLen = psmsstore->fix.Conlen;
        portsend.pData = psmsstore->pszContent;

        ret = SMSDISPATCH_Send(smsctrl[0].smshandle,hSendCtlWnd,uMsgCmd,destaddr,portsend);
    }
    else if(nMsgType == MU_MSG_VCAL)
    {
        strcpy(destaddr.loweraddr,psmsstore->pszPhone);
        destaddr.port = smsctrl[1].port;
        strcpy(destaddr.SCA,psmsstore->fix.SCA);

        portsend.CodeMode = psmsstore->fix.dcs;
        portsend.DataLen = psmsstore->fix.Conlen;
        portsend.pData = psmsstore->pszContent;

        ret = SMSDISPATCH_Send(smsctrl[1].smshandle,hSendCtlWnd,uMsgCmd,destaddr,portsend);
    }
                     
	return ret;
}

/**************************************************************************\

\**************************************************************************/
BOOL SMS_OnRecvPort(WPARAM para)
{
#define SMS_RECVBUF_LEN     1024

	int i = 0,len = 0;
	static char RecvBuf[SMS_RECVBUF_LEN];
	char* pbuf = NULL;
	int size = 0;
	Address_t srcaddr;
    PORT_RECV   port_recv;
    char szFileName[SMS_FILENAME_MAXLEN];
    SMS_STORE smsstore;
    DWORD   dwoffset;

    szFileName[0] = 0;

	for( i = 0; i < 2; i ++ )
	{
		//Find
		if( smsctrl[i].smshandle == HIWORD(para) )
			break;
	}

	if( i == 2 )
		return -1;
	
	size = LOWORD(para);
	
	if( size > SMS_RECVBUF_LEN )	
	{
		if( (pbuf = malloc(size)) == NULL )
			return -1;	
	}
	else
	{
		pbuf = RecvBuf;
		size = SMS_RECVBUF_LEN;
	}

	memset( pbuf, 0, size );

    memset(&port_recv,0,sizeof(PORT_RECV));

    port_recv.nBufLen = size;
    port_recv.pBuf = pbuf;

	len = SMSDISPATCH_Recv(smsctrl[i].smshandle, &srcaddr, &port_recv);

    if(len < 0)
    {
        if(port_recv.pBuf && (port_recv.pBuf != RecvBuf))
            free(port_recv.pBuf);
        return FALSE;
    }
    
    memset(&smsstore,0,sizeof(SMS_STORE));
    smsstore.fix.dcs = port_recv.nCodeMode;
    smsstore.fix.Conlen = port_recv.nDataLen;
    smsstore.fix.dwDateTime = String2DWORD(port_recv.Time);
    smsstore.fix.Phonelen = strlen(srcaddr.loweraddr)+1;
    strcpy(smsstore.fix.SCA,srcaddr.SCA);
    smsstore.fix.Stat = MU_STU_UNREAD;
    smsstore.fix.Status = 0;
    smsstore.fix.Type = SMS_SUBMIT;
    smsstore.fix.Udhlen  = 0;
    smsstore.pszContent = port_recv.pBuf;
    smsstore.pszPhone = srcaddr.loweraddr;
    smsstore.pszUDH = NULL;

#ifndef _EMULATE_
    PrioMan_CallMusicEx(PRIOMAN_PRIORITY_SMSAPP, 3000);
#endif
    
    DlmNotify(PMM_NEWS_ENABLE, 	PMF_MAIN_BKLIGHT);
	DlmNotify (MAKELONG(PMM_NEWS_SET_DELAY,PMS_DELAY_SHUT_LIGHT_MAIN), 20);

    switch(smsctrl[i].port) 
    {
    case SMS_WAPCARD_PORT:
        //SaveAsVcard(&srcaddr,port_recv.pBuf,port_recv.nDataLen,szFileName);
        if(SMS_SaveRecord(SMS_FILENAME_INBOX,&smsstore,&dwoffset))
        {
            SMS_ChangeCount(MU_INBOX,SMS_COUNT_UNREAD,1);
            SMS_ChangeCount(MU_INBOX,SMS_COUNT_ALL,1);
            
            if(MU_GetCurFolderType() == MU_INBOX)
                smsapi_NewRecord(dwoffset,&smsstore);
            
            MU_NewMsgArrival(MU_SMS_NOTIFY);
            
            SMS_NotifyIdle();
        }
        break;
        
    case SMS_WAPCAL_PORT:
        //SaveAsVcal(&srcaddr,port_recv.pBuf,port_recv.nDataLen,szFileName);
        if(SMS_SaveRecord(SMS_FILENAME_INBOX,&smsstore,&dwoffset))
        {
            SMS_ChangeCount(MU_INBOX,SMS_COUNT_UNREAD,1);
            SMS_ChangeCount(MU_INBOX,SMS_COUNT_ALL,1);
            
            if(MU_GetCurFolderType() == MU_INBOX)
                smsapi_NewRecord(dwoffset,&smsstore);
            
            MU_NewMsgArrival(MU_SMS_NOTIFY);
            
            SMS_NotifyIdle();
        }
    	break;

    default:
        break;
    }
    
    if(port_recv.pBuf && (port_recv.pBuf != RecvBuf))
        free(port_recv.pBuf);

    return TRUE;
}

/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL SMS_NewVcardVcal(char* pszPostFix,char* pszFileName)
{
    /*
     *  ASCII
     0~9 : 48~57
     A~Z : 65~90
     a~z : 97~122
     */
    SYSTEMTIME t;
    int i,j;
    char ichar;
    int f;
    BOOL bFirst = TRUE;
    
    GetLocalTime(&t);
    srand((unsigned)t.wMilliseconds);

    do 
    {
        if(!bFirst)
            close(f);

        for (i = 0; i < 10; i++)
        {
            j = rand()%3;
            switch(j) 
            {
            case 0:
                ichar = 48 + rand()%(57 - 48);
                break;
            case 1:
                ichar = 65 + rand()%(90 - 65);
                break;
            case 2:
                ichar = 97 + rand()%(122 - 97);
                break;
            default:
                break;
            }
            pszFileName[i] = ichar;
        }
        pszFileName[i] = '\0';
        strcat(pszFileName, pszPostFix);
        bFirst = FALSE;
    }while((f = open(pszFileName,O_RDONLY)) != -1);

    return TRUE;
}

static BOOL SaveAsVcard(Address_t* psrcaddr, unsigned char* pData, int nDataLen,char* pszFileName)
{
    char szOldPath[PATH_MAXLEN];
    int f;
    BYTE byUnread;

    szOldPath[0] = 0;
    
    getcwd(szOldPath,PATH_MAXLEN);

    chdir(PATH_DIR_SMS);
    
    if(strlen(pszFileName) == 0)
    {
        if(!SMS_NewVcardVcal(SMS_VCARD_PORTFIX,pszFileName))
        {
            chdir(szOldPath);
            return FALSE;
        }
        f = open(pszFileName,O_RDWR|O_CREAT,S_IRWXU);
        
        if( f == -1 )
        {
            chdir(szOldPath);
            return FALSE;
        }

        byUnread = MU_STU_UNREAD;
        write(f,&byUnread,sizeof(BYTE));
        write(f,psrcaddr,sizeof(Address_t));
        write(f,pData,nDataLen);
        
        close(f);
    }
    else
    {
        f = open(pszFileName,O_RDWR|O_CREAT,S_IRWXU);
        
        if( f == -1 )
        {
            chdir(szOldPath);
            return FALSE;
        }
        
        byUnread = MU_STU_READ;
        write(f,&byUnread,sizeof(BYTE));

        close(f);
    }

    chdir(szOldPath);

    return TRUE;
}

static BOOL SaveAsVcal(Address_t* psrcaddr, unsigned char* pData, int nDataLen,char* pszFileName)
{
    char szOldPath[PATH_MAXLEN];
    int f;
    BYTE byUnread;

    szOldPath[0] = 0;
    
    getcwd(szOldPath,PATH_MAXLEN);

    chdir(PATH_DIR_SMS);
    
    if(strlen(pszFileName) == 0)
    {
        if(!SMS_NewVcardVcal(SMS_VCAL_PORTFIX,pszFileName))
        {
            chdir(szOldPath);
            return FALSE;
        }
        
        f = open(pszFileName,O_RDWR|O_CREAT,S_IRWXU);
        
        if( f == -1 )
        {
            chdir(szOldPath);
            return FALSE;
        }

        byUnread = MU_STU_UNREAD;
        write(f,&byUnread,sizeof(BYTE));
        write(f,psrcaddr,sizeof(Address_t));
        write(f,pData,nDataLen);
        
        close(f);
    }
    else
    {

        f = open(pszFileName,O_RDWR|O_CREAT,S_IRWXU);
        
        if( f == -1 )
        {
            chdir(szOldPath);
            return FALSE;
        }
        
        byUnread = MU_STU_READ;
        write(f,&byUnread,sizeof(BYTE));

        close(f);
    }

    chdir(szOldPath);

    return TRUE;
}



/*********************************************************************\
* Function	   SMS_IsVcradOrVcal
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
int SMS_IsVcradOrVcal(SMS_STORE *psmsstore)
{
    char* pStart = NULL;
    vCard_Obj* pvCardObj = NULL;
    vCal_Obj* pvCalObj = NULL;
    int   nRet;
    char* pData;

    if(psmsstore->fix.Type == STATUS_REPORT)
        return MU_MSG_STATUS_REPORT;

    if(psmsstore->pszContent != NULL && psmsstore->fix.Conlen > 0)
    {
        if((pStart = AB_strstr(psmsstore->pszContent,"BEGIN:VCARD",psmsstore->fix.Conlen)) && 
           AB_strstr(psmsstore->pszContent,"END:VCARD",psmsstore->fix.Conlen) )
        {
            
            pData = (char*)malloc(psmsstore->fix.Conlen);

            if(pData == NULL)
                return MU_MSG_SMS;

            memcpy(pData,psmsstore->pszContent,psmsstore->fix.Conlen);

            nRet = vCard_Reader(pData,psmsstore->fix.Conlen,&pvCardObj);

            if(pvCardObj == NULL)
            {
                SMS_FREE(pData);

                return MU_MSG_SMS;
            }

            vCard_Clear(VCARD_OPT_READER,(PVOID)pvCardObj);
            
            SMS_FREE(pData);

            return MU_MSG_VCARD;
        }
        else if((pStart = AB_strstr(psmsstore->pszContent,"BEGIN:VCALENDAR",psmsstore->fix.Conlen)) && 
            AB_strstr(psmsstore->pszContent,"END:VCALENDAR",psmsstore->fix.Conlen) )
        {
            
            pData = (char*)malloc(psmsstore->fix.Conlen);

            if(pData == NULL)
                return MU_MSG_SMS;

            memcpy(pData,psmsstore->pszContent,psmsstore->fix.Conlen);

            nRet = vCal_Reader(pData,psmsstore->fix.Conlen,&pvCalObj);

            if(pvCalObj == NULL)
            {
                SMS_FREE(pData);

                return MU_MSG_SMS;
            }

            vCal_Clear(VCAL_OPT_READER,(PVOID)pvCalObj);

            SMS_FREE(pData);

            return MU_MSG_VCAL;
        }
    }

    return MU_MSG_SMS;
}



typedef struct tagSMSView_VcardVcal
{
    HMENU   hMenu;
    int     nFolderID;
    DWORD   handle;
    PVCARDVCAL   pVcardVcal;
    int     nMsgType;
    HWND    hwndmu;
    WNDPROC OldListWndProc;
    SMS_STORE pStore;
    BOOL    bPre;
    BOOL    bNext;
    HBITMAP hLeftArrow;
    HBITMAP hRightArrow;
}SMSVIEW_VCARDVAL, *PSMSVIEW_VCARDVAL;

#define IDC_SMSVIEWV_LIST           300
#define IDM_SMSVIEWV_SAVE           301
#define IDM_SMSVIEWV_MOVE           302
#define IDM_SMSVIEWV_MSGINFO        303
#define IDM_SMSVIEWV_DELETE         304
#define IDC_EXIT                    200

LRESULT SMSViewVcardOrVcalWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static BOOL SMSViewV_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct);
static void SMSViewV_OnActivate(HWND hwnd, UINT state);
static void SMSViewV_OnSetFocus(HWND hWnd);
static void SMSViewV_OnPaint(HWND hWnd);
static void SMSViewV_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags);
static LRESULT SMSViewV_OnCommand(HWND hWnd, int id, UINT codeNotify,LPARAM lParam);
static void SMSViewV_OnDestroy(HWND hWnd);
static void SMSViewV_OnClose(HWND hWnd);
static LRESULT CallListWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static void SMSViewV_OnMoveToFolder(HWND hWnd,BOOL bMove,int nNewFolder);
static void SMSViewV_OnDelete(HWND hWnd,BOOL bDelete);

BOOL SMS_VViewRegisterClass(void)
{
    WNDCLASS    wc;
    
    wc.style         = 0;
    wc.lpfnWndProc   = SMSViewVcardOrVcalWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = sizeof(SMSVIEW_VCARDVAL);
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName  = "SMSViewVcardOrVcalWndClass";
    
    if (!RegisterClass(&wc))
    {
        return FALSE;
    }

    return TRUE;
}

/*********************************************************************\
* Function	   SMS_IsVcradOrVcal
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL SMS_CreateViewVcardOrVcalWnd(HWND hFrameWnd,HWND hwndmu,SMS_STORE *psmsstore,int nMsgType,
                                  DWORD handle,int nFolderID,BOOL bPre,BOOL bNext)
{
    SMSVIEW_VCARDVAL CreateData;
    SMSVIEW_VCARDVAL *pData;
    vCard_Obj *pvCardObj = NULL;
    vCal_Obj *pvCalObj = NULL;
    HWND        hViewWnd = NULL;  
    char*       pszSaveMenu = NULL;
    char*       pStart = NULL;
    int         nRet;
    RECT        rcClient;
    char*       p;

    memset(&CreateData,0,sizeof(SMSVIEW_VCARDVAL));

    CreateData.nMsgType = nMsgType;
    CreateData.hwndmu = hwndmu;
    CreateData.bNext = bNext;
    CreateData.bPre = bPre;

    if(SMS_CopyStore(&(CreateData.pStore),psmsstore) == FALSE)
    {
        SMS_FREE(CreateData.pStore.pszUDH);
        SMS_FREE(CreateData.pStore.pszPhone);
        SMS_FREE(CreateData.pStore.pszContent);
        return FALSE;
    }

    if(nMsgType == MU_MSG_VCARD)
    {
        if(psmsstore->pszContent != NULL && psmsstore->fix.Conlen > 0)
        {
            if((pStart = AB_strstr(psmsstore->pszContent,"BEGIN:VCARD",psmsstore->fix.Conlen)) && 
                AB_strstr(psmsstore->pszContent,"END:VCARD",psmsstore->fix.Conlen) )
            {
            
                p = (char*)malloc(psmsstore->fix.Conlen);
                
                if(p == NULL)
                    return MU_MSG_SMS;
                
                memcpy(p,psmsstore->pszContent,psmsstore->fix.Conlen);
                
                nRet = vCard_Reader(p,psmsstore->fix.Conlen,&pvCardObj);
                
                SMS_FREE(p);

                if(pvCardObj == NULL)
                {
                    SMS_FREE(CreateData.pStore.pszUDH);
                    SMS_FREE(CreateData.pStore.pszPhone);
                    SMS_FREE(CreateData.pStore.pszContent);
                    return FALSE;
                }
                
                APP_AnalyseVcard(pvCardObj,&(CreateData.pVcardVcal));
                
                vCard_Clear(VCARD_OPT_READER,(PVOID)pvCardObj);
            }
        }
        else
        {
            SMS_FREE(CreateData.pStore.pszUDH);
            SMS_FREE(CreateData.pStore.pszPhone);
            SMS_FREE(CreateData.pStore.pszContent);
            return FALSE;
        }
    }
    else if(nMsgType == MU_MSG_VCAL)
    {
        if(psmsstore->pszContent != NULL && psmsstore->fix.Conlen > 0 )
        {
            if((pStart = AB_strstr(psmsstore->pszContent,"BEGIN:VCALENDAR",psmsstore->fix.Conlen)) && 
                AB_strstr(psmsstore->pszContent,"END:VCALENDAR",psmsstore->fix.Conlen) )
            {
                p = (char*)malloc(psmsstore->fix.Conlen);
                
                if(p == NULL)
                    return MU_MSG_SMS;
                
                memcpy(p,psmsstore->pszContent,psmsstore->fix.Conlen);
                
                nRet = vCal_Reader(p,psmsstore->fix.Conlen,&pvCalObj);
                
                SMS_FREE(p);

                if(pvCalObj == NULL)
                {
                    SMS_FREE(CreateData.pStore.pszUDH);
                    SMS_FREE(CreateData.pStore.pszPhone);
                    SMS_FREE(CreateData.pStore.pszContent);
                    return FALSE;
                }
                
                CALE_DecodeVcalToDisplay(pvCalObj,&(CreateData.pVcardVcal));
                
                vCal_Clear(VCAL_OPT_READER,(PVOID)pvCalObj);
            }
        }
        else
        {
            SMS_FREE(CreateData.pStore.pszUDH);
            SMS_FREE(CreateData.pStore.pszPhone);
            SMS_FREE(CreateData.pStore.pszContent);
            return FALSE;
        }
    }
    else
    {
        SMS_FREE(CreateData.pStore.pszUDH);
        SMS_FREE(CreateData.pStore.pszPhone);
        SMS_FREE(CreateData.pStore.pszContent);
        return FALSE;
    }

    CreateData.nFolderID = nFolderID; 
    CreateData.handle = handle;
    
    CreateData.hMenu = CreateMenu();
    
    GetClientRect(MuGetFrame(),&rcClient);

    hViewWnd = CreateWindow(
        "SMSViewVcardOrVcalWndClass", 
        "",
        WS_VISIBLE | WS_CHILD,
        rcClient.left,
        rcClient.top,
        rcClient.right - rcClient.left,
        rcClient.bottom - rcClient.top,
        MuGetFrame(),
        NULL,
        NULL, 
        (PVOID)&CreateData
        );
    
    if (!hViewWnd)
    {
        SMS_FREE(CreateData.pStore.pszUDH);
        SMS_FREE(CreateData.pStore.pszPhone);
        SMS_FREE(CreateData.pStore.pszContent);
        if(CreateData.nMsgType == MU_MSG_VCARD)
            APP_ABChainEarse(CreateData.pVcardVcal);
        return FALSE;
    }

    SetWindowText(MuGetFrame(),psmsstore->pszPhone);

    if(nMsgType == MU_MSG_VCARD)
        pszSaveMenu = (char*)IDS_SAVEVCARD;
    else
        pszSaveMenu = (char*)IDS_SAVEVCAL;

    AppendMenu(CreateData.hMenu, MF_ENABLED, IDM_SMSVIEWV_SAVE, pszSaveMenu);
    AppendMenu(CreateData.hMenu, MF_ENABLED, IDM_SMSVIEWV_MOVE, IDS_MOVE);
    AppendMenu(CreateData.hMenu, MF_ENABLED, IDM_SMSVIEWV_MSGINFO, IDS_MSGINFO);
    AppendMenu(CreateData.hMenu, MF_ENABLED, IDM_SMSVIEWV_DELETE, IDS_DELETE);
    
    PDASetMenu(MuGetFrame(),CreateData.hMenu);

    SetFocus(hViewWnd);

    SendMessage(MuGetFrame(), PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_EXIT,0), (LPARAM)IDS_BACK);
    SendMessage(MuGetFrame(), PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_OK,1), (LPARAM)"");
    SendMessage(MuGetFrame(), PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_OPTIONS);
    
    pData = GetUserData(hViewWnd);

    if(pData->bPre)
        SendMessage(MuGetFrame(), PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, LEFTICON),(LPARAM)SMS_ICO_ARROWLEFT);
    else
        SendMessage(MuGetFrame(), PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, LEFTICON),(LPARAM)"");

    if(pData->bNext)
        SendMessage(MuGetFrame(), PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, RIGHTICON),(LPARAM)SMS_ICO_ARROWRIGHT);
    else
        SendMessage(MuGetFrame(), PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, RIGHTICON),(LPARAM)"");

    ShowWindow(hViewWnd,SW_SHOW);
    UpdateWindow(hViewWnd);

    return TRUE;
}


/*********************************************************************\
* Function	SMSViewVcardOrVcalWndProc
* Purpose   Main window proc
* Params
*			hWnd: Handle of the window
*			wMsgCmd: Message ID
* Return
*			TRUE: Success
*			FALSE: Fail
* Remarks
**********************************************************************/
static LRESULT SMSViewVcardOrVcalWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    LRESULT lResult = TRUE;

	switch (wMsgCmd)
    {
    case WM_CREATE:
        lResult = SMSViewV_OnCreate(hWnd,(LPCREATESTRUCT)(lParam));
        break;

    case WM_ACTIVATE:
    case PWM_SHOWWINDOW:
        SMSViewV_OnActivate(hWnd,(UINT)LOWORD(wParam));
        break;

    case WM_SETFOCUS:
        SMSViewV_OnSetFocus(hWnd);
        break;
        
    case WM_PAINT:
        SMSViewV_OnPaint(hWnd);
        break;

    case WM_KEYDOWN:
        SMSViewV_OnKey(hWnd,(UINT)(wParam),(int)(short)LOWORD(lParam),(UINT)HIWORD(lParam));
        break;

    case WM_COMMAND:
        lResult = SMSViewV_OnCommand(hWnd,(int)(LOWORD(wParam)),(UINT)HIWORD(wParam),lParam);
        break;
        
    case WM_CLOSE:
        SMSViewV_OnClose(hWnd);
        break;

    case WM_DESTROY:
        SMSViewV_OnDestroy(hWnd);
        break;

    case WM_SELECTFOLDER:
        SMSViewV_OnMoveToFolder(hWnd,(BOOL)wParam,(int)lParam);
        break;

    case WM_SUREDELETE:
        SMSViewV_OnDelete(hWnd,(BOOL)lParam);
        break;

    default:     
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
	}

    return lResult;

}
/*********************************************************************\
* Function	SMSViewV_OnCreate
* Purpose   WM_CREATE message handler of the main window
* Params
*			hWnd: Handle of the window
*			lpCreateStruct: Create Structure
* Return
*			TRUE: Success
*			FALSE: Fail
* Remarks
**********************************************************************/
static BOOL SMSViewV_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
{
    RECT rect; 
    HWND hList;   
    SMSVIEW_VCARDVAL *pCreateData;
    int  i;
    VCARDVCAL * pTmp = NULL;

    pCreateData = (SMSVIEW_VCARDVAL*)GetUserData(hWnd);

    memcpy(pCreateData,(SMSVIEW_VCARDVAL*)(lpCreateStruct->lpCreateParams),sizeof(SMSVIEW_VCARDVAL));

    GetClientRect(hWnd,&rect);
    
    hList = CreateWindow(
        "LISTBOX",
        "",
        WS_VISIBLE | WS_CHILD | WS_VSCROLL | LBS_BITMAP | LBS_MULTILINE,
        rect.left,
        rect.top,
        rect.right - rect.left,
        rect.bottom - rect.top,
        hWnd,
        (HMENU)IDC_SMSVIEWV_LIST,
        NULL,
        NULL);
   
    if(hList == NULL)
        return FALSE;

    pCreateData->OldListWndProc = (WNDPROC)SetWindowLong(hList,GWL_WNDPROC,(LONG)CallListWndProc);
    
    pTmp = pCreateData->pVcardVcal;
    
    while(pTmp)
    {            
        i = SendMessage(hList, LB_ADDSTRING, -1, (LPARAM)(pTmp->pszTitle));
        SendMessage(hList, LB_SETAUXTEXT, MAKEWPARAM(i, -1), (LPARAM)(pTmp->pszContent));
        SendMessage(hList, LB_SETITEMDATA, i, (LPARAM)pTmp);
        
        pTmp = pTmp->pNext;
    }
    
    SendMessage(hList, LB_SETCURSEL, 0, 0);


    return TRUE;
    
}
/*********************************************************************\
* Function	SMSViewV_OnActivate
* Purpose   WM_ACTIVATE message handler of the main window
* Params
* Return    None
* Remarks
**********************************************************************/
static void SMSViewV_OnActivate(HWND hWnd, UINT state)
{
    HWND hLst;
    SMSVIEW_VCARDVAL *pCreateData;

    pCreateData = (SMSVIEW_VCARDVAL*)GetUserData(hWnd);

    hLst = GetDlgItem(hWnd,IDC_SMSVIEWV_LIST);

    SetFocus(hLst);

    SetWindowText(MuGetFrame(),pCreateData->pStore.pszPhone);
    
    PDASetMenu(MuGetFrame(),pCreateData->hMenu);
    
    SendMessage(MuGetFrame(), PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_BACK);
    SendMessage(MuGetFrame(), PWM_SETBUTTONTEXT, 1, (LPARAM)"");
    SendMessage(MuGetFrame(), PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_OPTIONS);
    
    if(pCreateData->bPre)
        SendMessage(MuGetFrame(), PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, LEFTICON),(LPARAM)SMS_ICO_ARROWLEFT);
    else
        SendMessage(MuGetFrame(), PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, LEFTICON),(LPARAM)"");

    if(pCreateData->bNext)
        SendMessage(MuGetFrame(), PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, RIGHTICON),(LPARAM)SMS_ICO_ARROWRIGHT);
    else
        SendMessage(MuGetFrame(), PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, RIGHTICON),(LPARAM)"");
    
    return;
}
/*********************************************************************\
* Function	SMSViewV_OnSetFocus
* Purpose   WM_ACTIVATE message handler of the main window
* Params
* Return    None
* Remarks
**********************************************************************/
static void SMSViewV_OnSetFocus(HWND hWnd)
{
    HWND hLst;

    hLst = GetDlgItem(hWnd,IDC_SMSVIEWV_LIST);

    SetFocus(hLst);

    return;
}
/*********************************************************************\
* Function	SMSViewV_OnPaint
* Purpose   WM_PAINT message handler of the main window
* Params	hWnd: Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void SMSViewV_OnPaint(HWND hWnd)
{
    HDC hdc = BeginPaint(hWnd, NULL);
	
    EndPaint(hWnd, NULL);

	return;
}

/*********************************************************************\
* Function	SMSViewV_OnKey
* Purpose   WM_KEYDOWN message handler of the main window
* Params
*			hWnd: Handle of the window
*			vk:	Virtual key code
*			fDown: Is key donw
*			cRepeat: Key repeat rate
*			flags:	flag of key down
* Return	None
* Remarks
**********************************************************************/
static void SMSViewV_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags)
{
    SMSVIEW_VCARDVAL *pCreateData;

    pCreateData = GetUserData(hWnd);
	
    switch (vk)
	{
	case VK_F10:
		PostMessage(hWnd,WM_CLOSE,NULL,NULL);
        break;
        
    case VK_LEFT:
        if(pCreateData->bPre)
        {
            PostMessage(pCreateData->hwndmu,PWM_MSG_MU_PRE,NULL,NULL); 
            PostMessage(hWnd,WM_CLOSE,0,0);
        }
        break;

    case VK_RIGHT:
        if(pCreateData->bNext)
        {
            PostMessage(pCreateData->hwndmu,PWM_MSG_MU_NEXT,NULL,NULL);
            PostMessage(hWnd,WM_CLOSE,0,0);
        }
        break;

    case VK_F5:
		PDADefWindowProc(GetParent(hWnd), WM_KEYDOWN, vk, MAKELPARAM(cRepeat, flags));
        break;

    default:
		PDADefWindowProc(hWnd, WM_KEYDOWN, vk, MAKELPARAM(cRepeat, flags));
		break;
	}

	return;
}
/*********************************************************************\
* Function	SMSViewV_OnCommand
* Purpose   WM_COMMAND message handler of the main window
* Params
*			hWnd:	Handle of the window
*			id:		Id of command message
*			hwndCtl: Handle of the window which sended this message
*			codeNotify:Notify code of the message
* Return	None
* Remarks
**********************************************************************/
static LRESULT SMSViewV_OnCommand(HWND hWnd, int id, UINT codeNotify,LPARAM lParam)
{
    SMSVIEW_VCARDVAL *pCreateData;
    HWND hLst;
    char szCaption[50];

    pCreateData = (SMSVIEW_VCARDVAL*)GetUserData(hWnd);
    hLst = GetDlgItem(hWnd,IDC_SMSVIEWV_LIST);

	switch(id)
	{
    case IDC_SMSVIEWV_LIST:
        if(codeNotify == LBN_SETFONT)
        {            
            HFONT hFont = NULL;
            
            GetFontHandle(&hFont, (lParam == 0) ? SMALL_FONT : LARGE_FONT);
            
            return (LRESULT)hFont;
        }
        break;

    case IDM_SMSVIEWV_DELETE:    
        if(pCreateData->nFolderID == -1)
            break;

        szCaption[0] = 0;

        GetWindowText(GetParent(hWnd),szCaption,49);

        PLXConfirmWinEx(GetParent(hWnd),hWnd,IDS_DELETEMESSAGE, Notify_Request, szCaption, IDS_YES, IDS_NO,WM_SUREDELETE);
        
        break;

    case IDM_SMSVIEWV_SAVE:
        if(pCreateData->nMsgType == MU_MSG_VCARD)
        {
            if(APP_SaveVcard(pCreateData->pStore.pszContent,pCreateData->pStore.fix.Conlen) == TRUE)
            {
                szCaption[0] = 0;
                
                GetWindowText(GetParent(hWnd),szCaption,49);
                
                PLXTipsWin(NULL,NULL,0,IDS_SAVEDTOCONTACT, szCaption ,Notify_Success , IDS_OK , NULL, WAITTIMEOUT);
            }
        }
        else if(pCreateData->nMsgType == MU_MSG_VCAL)
        {
            vCal_Obj* pvCalObj = NULL;

            char* psztemp = NULL;

            psztemp = (char*)malloc(pCreateData->pStore.fix.Conlen);

            if(psztemp == NULL)
                break;

            memcpy(psztemp,pCreateData->pStore.pszContent,pCreateData->pStore.fix.Conlen);

            vCal_Reader(psztemp,pCreateData->pStore.fix.Conlen,&pvCalObj);
            
            SMS_FREE(psztemp);

            if(pvCalObj != NULL)
            {                
                if(CALE_SaveFromSms(pvCalObj) == TRUE)
                {
                    szCaption[0] = 0;
                    
                    GetWindowText(GetParent(hWnd),szCaption,49);
                    
                    PLXTipsWin(NULL,NULL,0,IDS_SAVEDTOCALENDAR, szCaption ,Notify_Success , IDS_OK , NULL, WAITTIMEOUT);
                }
            }
            vCal_Clear(VCAL_OPT_READER,(PVOID)pvCalObj);
        }
        break;

    case IDM_SMSVIEWV_MOVE:
        MU_FolderSelection(hWnd, hWnd, WM_SELECTFOLDER, pCreateData->nFolderID);
        break;
        
    case IDM_SMSVIEWV_MSGINFO:
        SMS_CreateDetailWnd(GetParent(hWnd),&(pCreateData->pStore));
        break;

    default:
        break;
    }

    return TRUE;
}
/*********************************************************************\
* Function	SMSViewV_OnDestroy
* Purpose   WM_DESTROY message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void SMSViewV_OnDestroy(HWND hWnd)
{
    SMSVIEW_VCARDVAL* pData;
    
    pData = GetUserData(hWnd);

    DestroyMenu(pData->hMenu);

    if(pData->nMsgType == MU_MSG_VCARD)
        APP_ABChainEarse(pData->pVcardVcal);

    SMS_FREE(pData->pStore.pszContent);
    SMS_FREE(pData->pStore.pszPhone);
    SMS_FREE(pData->pStore.pszUDH);

    return;

}
/*********************************************************************\
* Function	SMSViewV_OnClose
* Purpose   WM_CLOSE message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void SMSViewV_OnClose(HWND hWnd)
{
    SendMessage(GetParent(hWnd),PWM_CLOSEWINDOW,(WPARAM)hWnd,NULL);

    DestroyWindow (hWnd);
	
    return;

}

/*********************************************************************\
* Function	CallListWndProc
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
static LRESULT CallListWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    LRESULT lResult = TRUE;
    HWND    hParent;
    SMSVIEW_VCARDVAL* pData;

    hParent = GetParent(hWnd);
    
    pData = GetUserData(hParent);
    
    switch(wMsgCmd)
    {         
    case WM_KEYDOWN:
        switch (LOWORD(wParam))
        { 
        case VK_RIGHT:
        case VK_LEFT:
        case VK_F1:
            PostMessage(hParent,wMsgCmd,wParam,lParam);
            return lResult;     

        default:
            return CallWindowProc(pData->OldListWndProc, hWnd, wMsgCmd, wParam, lParam);
        }
    default:
        return CallWindowProc(pData->OldListWndProc, hWnd, wMsgCmd, wParam, lParam);
    }
    return lResult;       
}
/*********************************************************************\
* Function     SMSViewV_OnMoveToFolder
* Purpose      
* Params      
* Return       
**********************************************************************/
static void SMSViewV_OnMoveToFolder(HWND hWnd,BOOL bMove,int nNewFolder)
{
    SMSVIEW_VCARDVAL* pCreateData;
    char* pszOldFileName,*pszNewFileName;
    DWORD dwOffset;
    char szFileName[SMS_FILENAME_MAXLEN];
    char szOldPath[PATH_MAXLEN];
    SMS_INDEX* handle;
    char* pszCaption = NULL;
    ABNAMEOREMAIL ABName;
    
    if(bMove == FALSE)
        return;

    if(nNewFolder == -1)
        return;

    pCreateData = GetUserData(hWnd);

    if(pCreateData->nFolderID == -1)
        return;
    
    if(pCreateData->nFolderID == nNewFolder)
        return;

    memset(&ABName,0,sizeof(ABNAMEOREMAIL));

    if(APP_GetNameByPhone(pCreateData->pStore.pszPhone,&ABName))
    {
        pszCaption = ABName.szName;
    }
    else
    {
        pszCaption = pCreateData->pStore.pszPhone;
    }

    handle = (SMS_INDEX*)(pCreateData->handle);
    if(handle->byStoreType == STORE_INDEX)
    {
        MESTORE_INFO MEStoreInfo;
        SMS_INITDATA *pData;
        int Stat = MU_STU_READ;
        int i;
        
        pData = SMS_GetInitData();
        
        memset(&MEStoreInfo,0,sizeof(MESTORE_INFO));

        SMS_ReadOneMEStore(&MEStoreInfo,handle->index);

        MEStoreInfo.nfolderid = nNewFolder;
        
        SMS_WriteOneMEStore(&MEStoreInfo,handle->index);
        
        smsapi_Delete((DWORD)handle);
        
        SMS_ChangeCount(nNewFolder,SMS_COUNT_ALL,1);
        
        for(i = 0 ; i < pData->nMEMaxCount ; i++)
        {
            if(pData->pMESMSInfo[i].Index == handle->index)
            {
                Stat = pData->pMESMSInfo[i].Stat;
                break;
            }
        }
        
        SMS_ChangeCount(pCreateData->nFolderID,SMS_COUNT_ALL,-1);
                       
        //SendMessage(pCreateData->hwndmu ,PWM_MSG_MU_DELETE,MAKEWPARAM(MU_ERR_SUCC,MU_MDU_SMS),(LPARAM)pCreateData->handle);
        
        PostMessage(hWnd,WM_CLOSE,0,0);
        
        PLXTipsWin(NULL, NULL, 0, (char*)IDS_MOVED, pszCaption, Notify_Success, (char*)IDS_OK, "", WAITTIMEOUT);
        
        return;
    }
    
    switch(pCreateData->nFolderID)
    {   
    case MU_REPORT:
    case MU_DRAFT:
        return;
        
    case MU_INBOX: // a big file
        pszOldFileName = SMS_FILENAME_INBOX;
        break;
        
    case MU_SENT:
        pszOldFileName = SMS_FILENAME_SENT;
        break;
        
    case MU_MYFOLDER:
        pszOldFileName = SMS_FILENAME_MYFOLDER;
        break;
        
    case MU_OUTBOX:
        if(nNewFolder != MU_DRAFT)
            return;//small to small
        
        pCreateData->pStore.fix.Stat = MU_STU_DRAFT;
        
        if(SMS_SaveFile(MU_DRAFT,&(pCreateData->pStore),szFileName,pCreateData->nMsgType) == FALSE)
            return;
        
        szOldPath[0] = 0;
        
        getcwd(szOldPath,PATH_MAXLEN);  
        chdir(PATH_DIR_SMS);  
        
        remove(((SMS_INDEX*)(pCreateData->handle))->szFileName);
        
        chdir(szOldPath); 
        
        pCreateData = (SMSVIEW_VCARDVAL*)GetUserData(hWnd);
        
        SendMessage(pCreateData->hwndmu,PWM_MSG_MU_DELETE,MAKEWPARAM(MU_ERR_SUCC,MU_MDU_SMS),(LPARAM)pCreateData->handle);
        PostMessage(hWnd,WM_CLOSE,0,0);
        
        return;
        
    default:
        szFileName[0] = 0;
        sprintf(szFileName,"%s%.8d%s",SMS_FILE_FOLDER_MARK,pCreateData->nFolderID,SMS_FILE_POSTFIX);
        pszOldFileName = szFileName;
        break;
    }
    
    
    
    switch(nNewFolder)
    {   
    case MU_INBOX: // a big file
        pszNewFileName = SMS_FILENAME_INBOX;
        break;
        
    case MU_MYFOLDER:
        pszNewFileName = SMS_FILENAME_MYFOLDER;
        break;
        
    case MU_DRAFT:
    case MU_OUTBOX:
    case MU_SENT:
    case MU_REPORT:
        return;
        
    default:
        szFileName[0] = 0;
        sprintf(szFileName,"%s%.8d%s",SMS_FILE_FOLDER_MARK,nNewFolder,SMS_FILE_POSTFIX);
        pszNewFileName = szFileName;
        break;
    }
    
    pCreateData->pStore.fix.Stat = MU_STU_READ;
    
    if(SMS_SaveRecord(pszNewFileName,&(pCreateData->pStore),&dwOffset) == FALSE)
        return;
    
    SMS_ChangeCount(nNewFolder,SMS_COUNT_ALL,1);
    
    getcwd(szOldPath,PATH_MAXLEN);  
    chdir(PATH_DIR_SMS);  
    
    if(-1 == DeleteWithOffset(pszOldFileName,((SMS_INDEX*)(pCreateData->handle))->dwOffest))
    {
        chdir(szOldPath);
        return;
    }
    
    SMS_ChangeCount(pCreateData->nFolderID,SMS_COUNT_ALL,-1);
    
    SMS_ChangeCount(pCreateData->nFolderID,SMS_COUNT_DEL,1);
    
    chdir(szOldPath); 
    pCreateData = (SMSVIEW_VCARDVAL*)GetUserData(hWnd);
    
    SendMessage(pCreateData->hwndmu,PWM_MSG_MU_DELETE,MAKEWPARAM(MU_ERR_SUCC,MU_MDU_SMS),(LPARAM)pCreateData->handle);
    
    PLXTipsWin(NULL, NULL, 0, (char*)IDS_MOVED, pszCaption, Notify_Success, (char*)IDS_OK, "", WAITTIMEOUT);

    PostMessage(hWnd,WM_CLOSE,0,0);
}
/*********************************************************************\
* Function     SMSViewV_OnDelete
* Purpose      
* Params      
* Return       
**********************************************************************/
static void SMSViewV_OnDelete(HWND hWnd,BOOL bDelete)
{
    SMSVIEW_VCARDVAL* pCreateData;
    char* pszOldFileName;
    char szFileName[SMS_FILENAME_MAXLEN];
    char szOldPath[PATH_MAXLEN];
    DWORD handle;
    char szCaption[64];
    
    if(bDelete == FALSE)
        return;
    
    pCreateData = GetUserData(hWnd);

    szCaption[0] = 0;

    GetWindowText(GetParent(hWnd),szCaption,63);

    szCaption[63] = 0;

    handle = pCreateData->handle;

    if(handle && ((SMS_INDEX*)handle)->byStoreType == STORE_INDEX)
    {
        SMS_INITDATA *pData;
        int i;
        SMS_INFO* pMESMSInfo = NULL;
        MSG myMsg;
        MESTORE_INFO MEStoreInfo;
        int stat;
        
        pData = SMS_GetInitData();
        
        for(i = 0 ; i < pData->nMEMaxCount ; i++)
        {
            if(pData->pMESMSInfo[i].Index == ((SMS_INDEX*)handle)->index)
            {
                pMESMSInfo = &(pData->pMESMSInfo[i]);
                
                break;
            }
        }
        
        if(pMESMSInfo == NULL)
        {
            return;
        }
        
        memset(&MEStoreInfo,0,sizeof(MESTORE_INFO));
        
        SMS_ReadOneMEStore(&MEStoreInfo,pMESMSInfo->Index);
        
        stat = pMESMSInfo->Stat;

        if(SIM_Delete(pMESMSInfo->Index,SMS_MEM_MT))
        {
            WaitWindowStateEx(NULL,TRUE,IDS_DELETING,IDS_SMS,NULL,NULL);
            
            while(GetMessage(&myMsg, NULL, 0, 0)) 
            {
                if((myMsg.message == MSG_SIM_DELETE_SUCC) && (myMsg.hwnd == SMS_GetSIMCtrlWnd()))
                {
                    pData->nMECount--;
                    
                    SMS_ChangeCount(pCreateData->nFolderID,SMS_COUNT_ALL,-1);

                    SMS_SetFull(FALSE);
                    
                    SMS_NotifyIdle();
                    
                    MEStoreInfo.nfolderid = 0;
                    
                    SMS_WriteOneMEStore(&MEStoreInfo,pMESMSInfo->Index);

                    memset(pMESMSInfo,0,sizeof(SMS_INFO));
                    
                    WaitWindowStateEx(NULL,FALSE,IDS_DELETING,IDS_SMS,NULL,NULL);
                    
                    SendMessage(pCreateData->hwndmu ,PWM_MSG_MU_DELETE,MAKEWPARAM(MU_ERR_SUCC,MU_MDU_SMS),(LPARAM)pCreateData->handle);
                    
                    PostMessage(hWnd,WM_CLOSE,0,0);
                    
                    PLXTipsWin(NULL, NULL, 0, (char*)IDS_DELETED, szCaption, Notify_Success, (char*)IDS_OK, "", WAITTIMEOUT);
  
                    return;
                }
                else if(myMsg.hwnd == SMS_GetSIMCtrlWnd())
                {
                    SendMessage(pCreateData->hwndmu,PWM_MSG_MU_DELETE,MAKEWPARAM(MU_ERR_FAILED,MU_MDU_SMS),handle);
                    
                    WaitWindowStateEx(NULL,FALSE,IDS_DELETING,IDS_SMS,NULL,NULL);
                    
                    return;
                }
                TranslateMessage(&myMsg);
                DispatchMessage(&myMsg);
            }
        }     
    }
    switch(pCreateData->nFolderID)
    {   
    case MU_REPORT:
    case MU_DRAFT:
    case MU_OUTBOX:
        return;
        
    case MU_INBOX: // a big file
        pszOldFileName = SMS_FILENAME_INBOX;
        break;
        
    case MU_SENT:
        pszOldFileName = SMS_FILENAME_SENT;
        break;
        
    case MU_MYFOLDER:
        pszOldFileName = SMS_FILENAME_MYFOLDER;
        break;
        
    default:
        szFileName[0] = 0;
        sprintf(szFileName,"%s%.8d%s",SMS_FILE_FOLDER_MARK,pCreateData->nFolderID,SMS_FILE_POSTFIX);
        pszOldFileName = szFileName;
        break;
    }
    
    getcwd(szOldPath,PATH_MAXLEN);  
    chdir(PATH_DIR_SMS);  
    
    DeleteWithOffset(pszOldFileName,((SMS_INDEX*)(pCreateData->handle))->dwOffest);
    
    SMS_ChangeCount(pCreateData->nFolderID,SMS_COUNT_ALL,-1);
    
    SMS_ChangeCount(pCreateData->nFolderID,SMS_COUNT_DEL,1);
    
    chdir(szOldPath); 
    
    SMS_TransferData();
    
    PLXTipsWin(NULL, NULL, 0, (char*)IDS_DELETED, szCaption, Notify_Success, (char*)IDS_OK, "", WAITTIMEOUT);

    SendMessage(pCreateData->hwndmu,PWM_MSG_MU_DELETE,MAKEWPARAM(MU_ERR_SUCC,MU_MDU_SMS),(LPARAM)pCreateData->handle);
    PostMessage(hWnd,WM_CLOSE,0,0);
}
/*********************************************************************\
* Function     SMSViewV_OnDelete
* Purpose      
* Params      
* Return       
**********************************************************************/
BOOL SMS_CopyStore(SMS_STORE *pDec,SMS_STORE* pSrc)
{
    memcpy(&(pDec->fix),&(pSrc->fix),sizeof(SMS_STOREFIX));

    if(pSrc->fix.Udhlen > 0)
    {
        pDec->pszUDH = (char*)malloc(pSrc->fix.Udhlen);
        if(pDec->pszUDH == NULL)
        {
            return FALSE;
        }
        strcpy(pDec->pszUDH,pSrc->pszUDH);
    }

    if(pSrc->fix.Phonelen > 0)
    {
        pDec->pszPhone = (char*)malloc(pSrc->fix.Phonelen);
        if(pDec->pszPhone == NULL)
        {
            return FALSE;
        }
        strcpy(pDec->pszPhone,pSrc->pszPhone);
    }

    if(pSrc->fix.Conlen > 0)
    {
        pDec->pszContent = (char*)malloc(pSrc->fix.Conlen);
        if(pDec->pszContent == NULL)
        {
            return FALSE;
        }
        strcpy(pDec->pszContent,pSrc->pszContent);
    }

    return TRUE;
}
/*********************************************************************\
* Function     AB_strstr
* Purpose      
* Params      
* Return       
**********************************************************************/
char * AB_strstr (const char * str1,const char * str2,int nstr1len)
{
    char *cp = (char *) str1;
    char *s1, *s2;
    int i;
    
    if ( !*str2 )
        return((char *)str1);
    
    for(i = 0 ; i < nstr1len ; i++)
    {
        s1 = cp;
        s2 = (char *) str2;
        
        while ( *s2 && !(*s1-*s2) )
            s1++, s2++;
        
        if (!*s2)
            return(cp);
        
        cp++;
    }
    
    return(NULL);
    
}
