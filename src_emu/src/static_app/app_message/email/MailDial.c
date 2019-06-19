/***************************************************************************
*
*                      Pollex Mobile Platform
*
* Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
*                       All Rights Reserved 
*
* Module   : MailDial.c
*
* Purpose  : 
*
\**************************************************************************/

#include "MailHeader.h"

#define  MAIL_HANGUP_TIMERID	1

static SYSTEMTIME BeginTime;
static BOOL bGet;
static int SleepHandle;

BOOL RegisterMailDialClass();
void UnregisterMailDialClass();
BOOL CreateMailDialWnd();
static LRESULT MailDialWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, 
                               LPARAM lParam);
static void MAIL_DialHangUp(HWND TmphWnd);
BOOL MAIL_RegisterMailDial(HWND hWnd);
BOOL MAIL_UnRegisterMailDial(HWND hWnd);

static BOOL MAIL_LogWrite(SYSTEMTIME * pBegintime);

BOOL RegisterMailDialClass()
{
    WNDCLASS wc;
    
    wc.style            = 0;
    wc.lpfnWndProc      = MailDialWndProc;
    wc.cbClsExtra       = 0;
    wc.cbWndExtra       = 0;
    wc.hInstance        = NULL;
    wc.hIcon            = NULL;
    wc.hCursor          = NULL;
    wc.hbrBackground    = GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName     = NULL;
    wc.lpszClassName    = "MailDialWndClass";
    
    if(!RegisterClass(&wc))
        return FALSE;
    else
        return TRUE;
}

void UnregisterMailDialClass()
{
    UnregisterClass("MailDialWndClass", NULL);
}

BOOL CreateMailDialWnd()
{
    if(IsWindow(HwndMailDial))
    {
    }
    else
    {
        if(!RegisterMailDialClass())
            return FALSE;
        
        HwndMailDial = CreateWindow(
            "MailDialWndClass",
            "",
            0,
            0, 0, 0, 0,
            NULL,
            NULL,
            NULL,
            NULL);
    }
    
    if(HwndMailDial == NULL)
    {
        return FALSE;
    }
    
    return TRUE;
}

static LRESULT MailDialWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, 
                               LPARAM lParam)
{
    LRESULT lResult;
    lResult = (LRESULT)TRUE;

    switch(wMsgCmd)
    {
    case WM_CREATE:
        {
        }
    	break;

    case WM_MAIL_DIAL_DIALUP:
        {
            switch(lParam)
            {
            //connect network successfully
            case DIALER_RUN_NETCONNECT:
                {
                    GlobalMailConfigHead.GLConStatus = MAILBOX_CON_ONLINE;
                    
                    if(GlobalMailConfigHead.GLNetFlag & MAIL_CON_TYPE_SEND)
                    {
                        PostMessage(HwndMailSend, WM_COMMAND, WM_MAIL_DIAL_SUCCESS, 0);
                    }
                    if((GlobalMailConfigHead.GLNetFlag & MAIL_CON_TYPE_HIDEREC) ||
                        (GlobalMailConfigHead.GLNetFlag & MAIL_CON_TYPE_SHOWREC))
                    {
                        PostMessage(HwndMailGet, WM_COMMAND, WM_MAIL_DIAL_SUCCESS, 0);
                    }
                }
                break;

            //connect network unsuccessfully
            case DIALER_RUN_NETBROKEN:
            case DIALER_RUN_NET_ABNORMAL:
                {
                    GlobalMailConfigHead.GLConStatus = MAILBOX_CON_IDLE;

                    if(GlobalMailConfigHead.GLNetFlag & MAIL_CON_TYPE_SEND)
                    {
                        PostMessage(HwndMailSend, WM_COMMAND, WM_MAIL_DIAL_CANCEL, 0);
                    }
                    if((GlobalMailConfigHead.GLNetFlag & MAIL_CON_TYPE_HIDEREC) ||
                        (GlobalMailConfigHead.GLNetFlag & MAIL_CON_TYPE_SHOWREC))
                    {
                        PostMessage(HwndMailGet, WM_COMMAND, WM_MAIL_DIAL_CANCEL, 0);
                    }
                    f_EnablePowerOff(RETRIEVMAIL);
                }
                break;

            default:
                break;
            }
        }
    	break;

    case WM_MAIL_DIAL_AGAIN:
        {
            GlobalMailConfigHead.GLNetFlag = 0;

            if((HWND)wParam == HwndMailSend)
            {
                //send mail window want to use the net
                GlobalMailConfigHead.GLNetFlag |= MAIL_CON_TYPE_SEND;
            }
            if((HWND)wParam == HwndMailGet)
            {
                //get mail window want to use the net
                if((int)lParam == 1)
                {
                    GlobalMailConfigHead.GLNetFlag |= MAIL_CON_TYPE_SHOWREC;
                }
                else
                {
                    GlobalMailConfigHead.GLNetFlag |= MAIL_CON_TYPE_HIDEREC;
                }
            }

            if(GlobalMailConfigHead.GLConStatus != MAILBOX_CON_DIAL)
            {
                if(!MAIL_RegisterMailDial(hWnd))
                {
                    if(GlobalMailConfigHead.GLNetFlag & MAIL_CON_TYPE_SEND)
                    {
                        PostMessage(HwndMailSend, WM_COMMAND, WM_MAIL_DIAL_CANCEL, 0);
                    }
                    if((GlobalMailConfigHead.GLNetFlag & MAIL_CON_TYPE_HIDEREC) ||
                        (GlobalMailConfigHead.GLNetFlag & MAIL_CON_TYPE_SHOWREC))
                    {
                        PostMessage(HwndMailGet, WM_COMMAND, WM_MAIL_DIAL_CANCEL, 0);
                    }
                    GlobalMailConfigHead.GLConStatus = MAILBOX_CON_IDLE;
                }
                else
                {
                    f_DisablePowerOff(RETRIEVMAIL);
                    GlobalMailConfigHead.GLConStatus = MAILBOX_CON_DIAL;
                }
            }
        }
        break;

    case WM_MAIL_DIAL_HANDUP:
        {
            if(wParam == MAIL_DIAL_HANDUP_NOW)
            {
                MAIL_DialHangUp(hWnd);
            }
            else
            {
                if((GlobalMailConfigHead.GLConStatus != MAILBOX_CON_IDLE) &&
                    (GlobalMailConfigHead.GLConStatus != MAILBOX_CON_DISCONNECT))
                {
                    KillTimer(hWnd, MAIL_HANGUP_TIMERID);
                    SetTimer(hWnd, MAIL_HANGUP_TIMERID, 30000, 0);
                }
            }
        }
        break;

    case WM_TIMER:
        {
            if(wParam == MAIL_HANGUP_TIMERID)
            {
                KillTimer(hWnd, MAIL_HANGUP_TIMERID);
                MAIL_DialHangUp(hWnd);
            }
        }
        break;

    case WM_CLOSE:
        {
        }
        break;

    case WM_DESTROY:
        {
        }
        break;

    default:
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;        
    }

    return lResult;
}

static void MAIL_DialHangUp(HWND TmphWnd)
{
    if(GlobalMailConfigHead.GLConStatus != MAILBOX_CON_IDLE)
    {
        MAIL_UnRegisterMailDial(TmphWnd);
        f_EnablePowerOff(RETRIEVMAIL);
    }

    MAIL_DealNextConnect();
    
    return;
}

/**************************************************************************** 
* Function : MAIL_RegisterMailDial
*
* Purpose  : call dial interface 
*			 to dial on the network
*
\*****************************************************************************/
BOOL MAIL_RegisterMailDial(HWND hWnd)
{
    int iResult;

    iResult = DIALER_GetLineState();

#if 0
    DIALER_INVALIDSTAGE		0;		//IDLE status
    DIALER_CONNECTSTAGE		1;		//connect status
    DIALER_PPPSTAGE			2;		//PPP arrange
    DIALER_NETWORKSTAGE		3;		//network status
    DIALER_HANGUPSTAGE		4;		//hangup status
#endif

    printf("\r\nGARY_MAIL:DIALER_GetLineState return is %d\r\n", iResult);

    if(iResult == DIALER_NETWORKSTAGE)
    {
        return FALSE;
    }

    if(iResult == DIALER_INVALIDSTAGE)
    {
        DIALER_GPRSINFO TempDial;
        UDB_ISPINFO uIspInfo;

        if(GlobalMailConfigHead.GLConnect == 0)
        {
            if(!(GlobalMailConfigHead.GLNetFlag & MAIL_CON_TYPE_HIDEREC))
            {
                PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_NOMAILBOX, NULL, Notify_Failure, 
                    IDP_MAIL_BOOTEN_OK, NULL, 20);
            }

            return FALSE;
        }
        memset(&uIspInfo, 0x0, sizeof(UDB_ISPINFO));
        if(!IspReadInfoByID(&uIspInfo, GlobalMailConfigHead.GLConnect))
        {
            if(!(GlobalMailConfigHead.GLNetFlag & MAIL_CON_TYPE_HIDEREC))
            {
                PLXTipsWin(NULL, NULL, 0, IDP_MAIL_STRING_NOMAILBOX, NULL, Notify_Failure, 
                    IDP_MAIL_BOOTEN_OK, NULL, 20);
            }
            
            return FALSE;
        }
        
        //log and gprs
        memset(&BeginTime, 0x0, sizeof(SYSTEMTIME));
        GetLocalTime(&BeginTime);
        if((GlobalMailConfigHead.GLNetFlag & MAIL_CON_TYPE_HIDEREC) ||
            (GlobalMailConfigHead.GLNetFlag & MAIL_CON_TYPE_SHOWREC))
        {
            bGet = TRUE;
        }
        else if(GlobalMailConfigHead.GLNetFlag & MAIL_CON_TYPE_SEND)
        {
            bGet = FALSE;
        }
        
        memset(&TempDial, 0x0, sizeof(DIALER_GPRSINFO));
        strcpy(TempDial.APN, uIspInfo.ISPPhoneNum1);
        strcpy(TempDial.UserID, uIspInfo.ISPUserName);
        strcpy(TempDial.PassWord, uIspInfo.ISPPassword);
        
        if(uIspInfo.DtType == DIALER_RUNMODE_GPRS)// GPRS call
        {
            iResult = DIALMGR_SelConnect(hWnd, WM_MAIL_DIAL_DIALUP, DIALDEF_SELFGPRS, &TempDial, FALSE);
        }
        else if(uIspInfo.DtType == DIALER_RUNMODE_DATA)// data call
        {
            iResult = DIALMGR_SelConnect(hWnd, WM_MAIL_DIAL_DIALUP, DIALDEF_SELFDATA, &TempDial, FALSE);
        }
        else
        {
            return FALSE;
        }

        printf("\r\nGARY_MAIL:DIALMGR_SelConnect return is %d\r\n", iResult);

#if 0
        DIALER_REQ_ACCEPT =  0;		// accept
        DIALER_REQ_EXIST  =  1;		// exist
        DIALER_REQ_REFUSE =  2;		// refuse
        DIALER_MCH_FAILURE=  3;		// failure
        DIALER_INVALID_ISP=  4;		// invalid ISP
#endif
        if(iResult != DIALER_REQ_ACCEPT)
        {
            return FALSE;
        }
        return TRUE;
    }
    
    return FALSE;
}

/**************************************************************************** 
* Function : MAIL_UnRegisterMailDial
*
* Purpose  : hangup dial
*
\*****************************************************************************/
BOOL MAIL_UnRegisterMailDial(HWND hWnd)
{
    int iResult;

    iResult = DIALER_GetLineState();

    if((iResult == DIALER_NETWORKSTAGE) ||
        (iResult == DIALER_CONNECTSTAGE) || 
        (iResult == DIALER_PPPSTAGE))
    {
        //log and gprs
        MAIL_LogWrite(&BeginTime);
        if(DIALMGR_HangUp(hWnd) == DIALER_REQ_ACCEPT)
            return TRUE;
    }
    return FALSE;
}

// GPRSCounter should be called after hangup 
extern int Get_MAC_Handle();
extern int MAC_GetOption	(int, unsigned, char*, int);
extern int AddGPRSData(unsigned long senddata, unsigned long receivedata);

//int Log_Write (LOGRECORD* plogrec)
static BOOL MAIL_LogWrite(SYSTEMTIME * pBegintime)
{
    SYSTEMTIME  EndTime;
    LOGRECORD *plogrec;
    void      *ptmprec;
    unsigned long begintm;
    unsigned long endtm;
    int len, apnlen;
    UDB_ISPINFO uIspInfo;
    //int i;

#define OPT_GPRS_TXFLOW		22		/* GPRS send flowdata */
#define OPT_GPRS_RXFLOW		23		/* GPRS receive flowdata */
    
    unsigned long gprs_tx;
    unsigned long gprs_rx;
    int           machandle;
    
    machandle = Get_MAC_Handle();
    MAC_GetOption(machandle, OPT_GPRS_TXFLOW, (char *)&gprs_tx, sizeof(long));
    MAC_GetOption(machandle, OPT_GPRS_RXFLOW, (char *)&gprs_rx, sizeof(long));
    
    AddGPRSData(gprs_tx, gprs_rx);
    
    GetLocalTime(&EndTime);

    memset(&uIspInfo, 0x0, sizeof(UDB_ISPINFO));
    /*if(bGet)
    {
        i = GlobalMailConfigHead.CurConnectConfig->GLConnect;
    }
    else
    {
        i = GlobalMailConfigHead.CurConnectConfig->GLConnect;
    }*/
    if(!IspReadInfoByID(&uIspInfo, GlobalMailConfigHead.GLConnect))
    {
        return FALSE;
    }

    if(strlen(uIspInfo.ISPPhoneNum1) == 0)
    {
        strcpy(uIspInfo.ISPPhoneNum1, "cmnet");
    }
    
    apnlen = strlen(uIspInfo.ISPPhoneNum1);
    len = sizeof(LOGRECORD) + apnlen;
    
    ptmprec = NULL;
    ptmprec = malloc(len + 1);
    if(ptmprec == NULL)
    {
        return FALSE;
    }
    memset(ptmprec, 0, len + 1);
    plogrec = (LOGRECORD *)ptmprec;
    
    if(bGet)
    {
        plogrec->direction = DRT_RECIEVE;
    }
    else
    {
        plogrec->direction = DRT_SEND;
    }

    LOG_STtoFT(pBegintime, &begintm);
    LOG_STtoFT(&EndTime, &endtm);
    
    plogrec->begintm = begintm;

    if(uIspInfo.DtType == DIALER_RUNMODE_GPRS)
    {
        plogrec->u.gprs.sendcounter = gprs_tx;
        plogrec->u.gprs.recievecounter = gprs_rx;
        plogrec->u.gprs.endtm = endtm;
        plogrec->u.gprs.duration = endtm - begintm;

        plogrec->type = TYPE_GPRS;
    }
    else
    {
        plogrec->u.data.duration = endtm - begintm;
        plogrec->u.data.endtm = endtm;
        
        plogrec->type = TYPE_DATA;
    }

    plogrec->index = 0;
    strncpy(plogrec->APN, uIspInfo.ISPPhoneNum1, apnlen);
    plogrec->APN[apnlen] = 0;

    printf("\r\n");
    printf("GARY_LOG_WRITE: apnlen = %d\r\n", apnlen);
    printf("GARY_LOG_WRITE: len = %d\r\n", len);
    printf("GARY_LOG_WRITE: ISPPhoneNum1 = %s\r\n", uIspInfo.ISPPhoneNum1);
    printf("GARY_LOG_WRITE: plogrec->APN = %s\r\n", plogrec->APN);
    printf("GARY_LOG_WRITE: sendcounter = %d\r\n", plogrec->u.gprs.sendcounter);
    printf("GARY_LOG_WRITE: recievecounter = %d\r\n", plogrec->u.gprs.recievecounter);
    printf("GARY_LOG_WRITE: gprs_tx = %d\r\n", gprs_tx);
    printf("GARY_LOG_WRITE: gprs_rx = %d\r\n", gprs_rx);
    printf("GARY_LOG_WRITE: plogrec = %x\r\n", plogrec);
    printf("\r\n");

    if (-1 == Log_Write(plogrec)) 
    {
        GARY_FREE(ptmprec);
        return FALSE;
    }
    else 
    {
        GARY_FREE(ptmprec);
        return TRUE;
    }
}
