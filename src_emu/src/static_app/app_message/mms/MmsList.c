/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : MMS
 *
 * Purpose  : the list interface	
 *            
\**************************************************************************/

#include "MmsList.h"
#include "MmsDisplay.h"

#define IDC_MMSINFO_LIST     201

static void MMS_SaveInfo(ACCEPTINFO*, ACCEPTINFO*);
static BOOL MMSInfo_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct);
static void MMSInfo_OnShowWnd(HWND hWnd, UINT state);
/******************************************************************** 
* Function	   CallDisplayInfo
* Purpose      display mms  info
* Params	   hwnd: window handle
               msg: message
               AcceptInfo:mms head info 
* Return	 	void
**********************************************************************/
void CallDisplayInfo(HWND hWndFrame, HWND hwnd, UINT msghandle, ACCEPTINFO AcceptInfo, 
					 BOOL bPre, BOOL bNext)
{
    WNDCLASS	wc;
    MMS_INFODATA  InfoData;
	HWND        hwndInfo;
	RECT        rClient;

    memset(&InfoData, 0, sizeof(MMS_INFODATA));
	InfoData.MsgInfo.hWnd = hwnd;
    InfoData.MsgInfo.msg = msghandle;
	InfoData.pmsgNode = GetNodeByHandle(msghandle);
	InfoData.bPre = bPre;
	InfoData.bNext = bNext;
	InfoData.hWndFrame = hWndFrame;

	MMS_SaveInfo(&InfoData.AcceptInfo, &AcceptInfo);
	
	wc.style         = 0;
	wc.lpfnWndProc   = InfoWndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = sizeof(MMS_INFODATA);
	wc.hInstance     = NULL;
	wc.hIcon         = NULL;
	wc.hCursor       = NULL;
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = "InfoWndClass";    
	wc.hbrBackground = NULL;
	
	UnregisterClass("InfoWndClass", NULL);
	RegisterClass(&wc);	

	GetClientRect(hWndFrame, &rClient);

	hwndInfo = CreateWindow(
		"InfoWndClass", 
		STR_MMS_INFO,
		WS_VISIBLE | WS_CHILD, 
		rClient.left, rClient.top, rClient.right-rClient.left, rClient.bottom-rClient.top,
		hWndFrame, NULL, NULL, (PVOID)&InfoData);
	
	if( hwndInfo != NULL )
	{
		ShowWindow( hwndInfo, SW_SHOW );
		UpdateWindow( hwndInfo );
	}
	else
	{
		UnregisterClass("InfoWndClass", NULL);
	}
	SetFocus(hwndInfo);
	
	if (AcceptInfo.infoType == INFO_RETRIEVE)
	{
		// left
		SendMessage(hWndFrame, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDM_INFOBACK, 1), 
			(LPARAM)ML("Retrieve"));
		// right
		SendMessage(hWndFrame, PWM_SETBUTTONTEXT, 0, (LPARAM)("Cancel"));
		SetWindowText(hWndFrame, ML("MMS Info"));
	}
	else if (AcceptInfo.infoType == INFO_REPORT)
	{
		// left
		SendMessage(hWndFrame, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDM_INFOBACK, 1), 
			(LPARAM)STR_MMS_OK);
		// right
		SendMessage(hWndFrame, PWM_SETBUTTONTEXT, 0, (LPARAM)(""));
		SetWindowText(hWndFrame, STR_MMS_REPORT);
	}
	else
	{
		// left
		SendMessage(hWndFrame, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDM_INFOBACK, 1), 
			(LPARAM)STR_MMS_OK);		
		// right
		SendMessage(hWndFrame, PWM_SETBUTTONTEXT, 0, (LPARAM)(""));
		SetWindowText(hWndFrame, ML("Details"));
	}
	// middle
	SendMessage(hWndFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)(""));

	if(AcceptInfo.infoType != INFO_REPORT)
	{
		if (bNext)
			SendMessage(hWndFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, RIGHTICON), 
			(LPARAM)MMS_RIGHTARROW_PATH);
		else	
			SendMessage(hWndFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, RIGHTICON), 
			(LPARAM)"");
		
		if (bPre)
			SendMessage(hWndFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, LEFTICON), 
			(LPARAM)MMS_LEFTARROW_PATH);
		else
			SendMessage(hWndFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, LEFTICON), 
			(LPARAM)"");
	}
	else
	{
		SendMessage(hWndFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, RIGHTICON), 
			(LPARAM)"");
		SendMessage(hWndFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, LEFTICON), 
			(LPARAM)"");
	}
	
}

/**************************************************************************** 
* Funcation: OnDispReportEnd
*           
\*****************************************************************************/
static void OnDispReportEnd(PMMS_INFODATA pInfoData)
{
    int    hReport;
    DREPORT   dReport;
    MU_MsgNode     msgnode;

	if(!pInfoData)
		return;
	
	if (pInfoData->pmsgNode == NULL)
		return;
	
    if (pInfoData->pmsgNode->nStatus == MMFT_REPORTED
		|| pInfoData->pmsgNode->nStatus == MMFT_REPORTING
		|| pInfoData->pmsgNode->nStatus == MMFT_UNREPORT)
    {
        chdir(MMS_FILEPATH);

        hReport = open(pInfoData->pmsgNode->msgname, O_RDONLY);
        if (hReport == -1)
            return;
        
        lseek(hReport, MMS_HEADER_SIZE, SEEK_SET);
        read(hReport, &dReport, sizeof(DREPORT));
        close(hReport);
        
        FillMsgNode(&msgnode, MU_MSG_MMS_DELIVER_REPORT, pInfoData->pmsgNode->nStatus, 
            dReport.getReportTime, pInfoData->MsgInfo.msg, STR_MMS_REPORT, 
            dReport.to, MU_STORE_IN_FLASH);
        
        SendMessage(pInfoData->MsgInfo.hWnd, PWM_MSG_MU_MODIFIED, MAKEWPARAM(MU_ERR_SUCC, MU_MDU_MMS), 
            (LPARAM)&msgnode);
    }
}

/******************************************************************** 
* Function	   InfoWndProc
* Purpose      wndproc
**********************************************************************/
static LRESULT InfoWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    LRESULT         lResult;
	HDC				hdc;
    WORD			mID;
    WORD			msgBox;
	PAINTSTRUCT		ps;
    RECT            rect;
    static ACCEPTINFO      AcceptInfo;    

	switch(wMsgCmd)
	{
	case WM_CREATE:
		lResult = MMSInfo_OnCreate(hWnd,(LPCREATESTRUCT)(lParam));				
		break;

    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        GetClientRect(hWnd, &rect);
        EndPaint(hWnd, &ps);
        break;    

	case WM_COMMAND:
		{
			mID = LOWORD(wParam);
			msgBox = HIWORD(lParam);
			switch(mID)
			{		
			case IDM_INFOBACK:
				SendMessage(hWnd, WM_CLOSE, NULL, NULL);
				break;				
			}		
		}
		break;
		
	case WM_ACTIVATE:
		break;

	case WM_SETFOCUS:
		SetFocus(GetDlgItem(hWnd,IDC_MMSINFO_LIST));
		break;

	case PWM_SHOWWINDOW:
		MMSInfo_OnShowWnd(hWnd, (UINT)LOWORD(wParam));
		break;
    
	case WM_KEYDOWN:
	{
		PMMS_INFODATA  pInfodata;

		pInfodata = GetUserData(hWnd);

		switch (LOWORD(wParam))
		{
		case VK_LEFT:
			if(pInfodata->bPre && pInfodata->AcceptInfo.infoType != INFO_REPORT)
			{
				PostMessage(hWnd,WM_CLOSE,0,0);
				PostMessage(pInfodata->MsgInfo.hWnd, PWM_MSG_MU_PRE, NULL, NULL); 
			}
			break;

		case VK_RIGHT:
			if(pInfodata->bNext && pInfodata->AcceptInfo.infoType != INFO_REPORT)
			{
				PostMessage(hWnd,WM_CLOSE,0,0);
				PostMessage(pInfodata->MsgInfo.hWnd, PWM_MSG_MU_NEXT, NULL, NULL); 
			}
			break;

		// left button
		case VK_RETURN:
			if (pInfodata->AcceptInfo.infoType == INFO_RETRIEVE)
			{
				if (GetMEIsSim() != 1)
				{
					MsgWin(NULL, NULL, 0, STR_MMS_NOSIM, STR_MMS_MMS,
						Notify_Alert, STR_MMS_OK, NULL, MMS_WAITTIME);
					PostMessage(hWnd, WM_CLOSE, NULL, NULL);
					break;
				}
				MMSPro_RecvDelay(pInfodata->pmsgNode->msgname);
			}
			PostMessage(hWnd, WM_CLOSE, NULL, NULL);
			break;

		// right button
		case VK_F10:
			if (pInfodata->AcceptInfo.infoType == INFO_RETRIEVE)
				PostMessage(hWnd, WM_CLOSE, NULL, NULL);
			break;

		default:
			return PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
		}
		break;
	}

    case WM_CLOSE:		
		{
			PMMS_INFODATA pInfoData = NULL;

			pInfoData = GetUserData(hWnd);

			OnDispReportEnd(pInfoData);
			DestroyWindow(hWnd);
			
		}
		break;
		
	case WM_DESTROY:
		{
			PMMS_INFODATA	pInfoData;

			pInfoData = GetUserData(hWnd);

			SendMessage(pInfoData->hWndFrame,PWM_CLOSEWINDOW,(WPARAM)hWnd,0);
			UnregisterClass("InfoWndClass", NULL);
		}
		break;

	default:
		lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
		break;
	}
	return lResult;
}

static int EditSubject(char *subject, int lineLen)
{
	int nWordNum = 0;
    int i;
	
	for (i=0 ; i<lineLen ; i++)
	{
		if (MMS_IsChinese(subject[i]))
		{
			nWordNum += 2;
			i++;
		}
		else
			nWordNum ++;
	}

	if (nWordNum > lineLen)
    {
		subject[lineLen - 1]='\0';
        nWordNum -= 2;
    }
    else
        subject[lineLen]='\0';

    return nWordNum;
}

static void MMS_SaveInfo(ACCEPTINFO* Info, ACCEPTINFO* AcceptInfo)
{
	memcpy(Info, AcceptInfo, sizeof(ACCEPTINFO));
}

static BOOL MMSInfo_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
{
	PMMS_INFODATA   pInfoData = NULL;
	HWND hFromView = NULL;
	HWND hWndFrame = NULL;
    int  nIndex = 0,nKilobyte = 0;
    char szData[20],szTime[20],szSize[20];
	char name[MAX_PBNAME_LEN];
	char *addTokens;
    char *address, *showaddr;
    RECT rect;
	BOOL bFind = FALSE;
	int  nShowaddrLen = 0;
    LISTBUF Listtmp;
	char sztemp[32];
	
	name[0] = 0;
	pInfoData = GetUserData(hWnd);
		
	memcpy(pInfoData,lpCreateStruct->lpCreateParams,sizeof(MMS_INFODATA));

	hWndFrame = pInfoData->hWndFrame;

	GetClientRect(hWnd,&rect);
	memset(&Listtmp, 0, sizeof(LISTBUF));
	PREBROW_InitListBuf(&Listtmp);
    // begin
	address = MMS_malloc(SIZE_1);	
    if(address == NULL)
        return 0;
	showaddr = MMS_malloc(1024);	
    if(showaddr == NULL)
        return 0;

	showaddr[0] = 0;

    if(pInfoData->AcceptInfo.infoType == INFO_ACCEPT 
		|| pInfoData->AcceptInfo.infoType == INFO_RECEIVE 
		|| pInfoData->AcceptInfo.infoType == INFO_RETRIEVE )
    {
        strcpy(address, pInfoData->AcceptInfo.From);
    }
    else
    {
        strcpy(address, pInfoData->AcceptInfo.To);
    }

    addTokens = MMS_chrtok(address,  MMS_ASEPCHR, MMS_ASEPCHRF);
 
    while(addTokens != NULL)
    {
        if (*addTokens == '\0')
        {
            addTokens = MMS_chrtok(NULL,  MMS_ASEPCHR, MMS_ASEPCHRF);
            continue;
        }            
        // get name in the contacts from phonenumber
        if (strchr(addTokens, '@') == NULL)
        {
            //if(strstr(addTokens,"+86") != NULL)
                //addTokens = addTokens + 3;
            bFind = MMS_FindNameInPB(addTokens, name, ADDR_MOBIL);
        }
        else
        {
            bFind = MMS_FindNameInPB(addTokens, name, ADDR_EMAIL);
        }
        if (bFind)
        {
			strcat(showaddr, name);
			strcat(showaddr, MMS_ASEPSTRF);
			nShowaddrLen += strlen(name);
			nShowaddrLen ++;
        }
        else
        {
            strcat(showaddr, addTokens);
			UTF8ToMultiByte(CP_ACP, 0, showaddr, -1, showaddr, 1023, NULL, NULL);
			strcat(showaddr, MMS_ASEPSTRF);
			nShowaddrLen += strlen(addTokens);
			nShowaddrLen ++;
        }
		showaddr[nShowaddrLen] = 0;
        addTokens = MMS_chrtok(NULL,  MMS_ASEPCHR, MMS_ASEPCHRF);
    }
    MMS_free(address);
	// end

	if (nShowaddrLen > 0)
		showaddr[nShowaddrLen - 1] = 0;

	if(pInfoData->AcceptInfo.infoType == INFO_REPORT)
	{
		PREBROW_AddData(&Listtmp, STR_MMS_RECIPIENT_TITLE, showaddr);
		PREBROW_AddData(&Listtmp, STR_MMS_DSTATUS, pInfoData->AcceptInfo.MsgClass);

		szData[0] = 0;
		szTime[0] = 0;
		
		GetTimeDisplay(pInfoData->AcceptInfo.acceptTime, szTime, szData);
		
		PREBROW_AddData(&Listtmp, STR_MMS_TIME_TITLE, szTime);
		PREBROW_AddData(&Listtmp, STR_MMS_DATE_TITLE, szData);
	}
	else
	{
		if(pInfoData->AcceptInfo.infoType == INFO_ACCEPT 
			|| pInfoData->AcceptInfo.infoType == INFO_RECEIVE 
			|| pInfoData->AcceptInfo.infoType == INFO_RETRIEVE )
		{
			PREBROW_AddData(&Listtmp, STR_MMS_FROM1, showaddr);
		}
		else	
			//if(pInfoData->AcceptInfo.infoType == INFO_SENT)
			PREBROW_AddData(&Listtmp, ML("To:"), showaddr);
		
		MMS_free(showaddr);
		
		UTF8ToMultiByte(CP_ACP, 0, pInfoData->AcceptInfo.Subject, -1, pInfoData->AcceptInfo.Subject, SIZE_1 - 1, NULL, NULL);
		if(strlen(pInfoData->AcceptInfo.Subject) > 18)
		{
			strncpy(sztemp, pInfoData->AcceptInfo.Subject, 15);
			sztemp[15] = '.';
			sztemp[16] = '.';
			sztemp[17] = '.';
			sztemp[18] = 0;
		}
		else
		{
			strcpy(sztemp, pInfoData->AcceptInfo.Subject);
		}
		
		PREBROW_AddData(&Listtmp, IDS_SUBJECT, sztemp);
		
		szData[0] = 0;
		szTime[0] = 0;
		
		GetTimeDisplay(pInfoData->AcceptInfo.acceptTime, szTime, szData);
		
		PREBROW_AddData(&Listtmp, IDS_DATE, szData);
		PREBROW_AddData(&Listtmp, IDS_TIME, szTime);
		PREBROW_AddData(&Listtmp, IDS_TYPE, STR_MMS_MMS);
		
		szSize[0] = 0;
		nKilobyte = pInfoData->AcceptInfo.MsgSize/1024;
		if(nKilobyte > 0)
			sprintf(szSize,"%d kB",nKilobyte);
		else
		{
			nKilobyte = pInfoData->AcceptInfo.MsgSize * 10 /1024;
			if(nKilobyte > 0)
				sprintf(szSize,"0.%d kB",nKilobyte);
			else
			{
				nKilobyte = pInfoData->AcceptInfo.MsgSize * 100 /1024;
				sprintf(szSize,"0.0%d kB",nKilobyte);
			}
		}
		
		PREBROW_AddData(&Listtmp, IDS_SIZE, szSize);
	}
	
	
	hFromView = CreateWindow(
		FORMVIEWER,
		"",
		WS_VISIBLE  |WS_CHILD | WS_VSCROLL,
		rect.left,
		rect.top,
		rect.right - rect.left,
		rect.bottom - rect.top,
		hWnd,
		(HMENU)IDC_MMSINFO_LIST,
		NULL, 
		(PVOID)&Listtmp);
	
	if (hFromView == NULL) 
		return FALSE;

    return TRUE;
}
/*********************************************************************\
* Function	MMSInfo_OnShowWnd
* Purpose   WM_ACTIVATE message handler of the main window
* Params
* Return    None
* Remarks
**********************************************************************/
static void MMSInfo_OnShowWnd(HWND hWnd, UINT state)
{
    HWND hList;
	HWND hWndFrame;
	PMMS_INFODATA	pInfoData;

	pInfoData = GetUserData(hWnd);
	hWndFrame = pInfoData->hWndFrame;
    hList = GetDlgItem(hWnd,IDC_MMSINFO_LIST);
    //if(state == WA_ACTIVE) 
	SetFocus(hList);

	if (pInfoData->AcceptInfo.infoType == INFO_RETRIEVE)
	{
		// left
		SendMessage(hWndFrame, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDM_INFOBACK, 1), 
			(LPARAM)ML("Retrieve"));
		// right
		SendMessage(hWndFrame, PWM_SETBUTTONTEXT, 0, (LPARAM)("Cancel"));
		SetWindowText(hWndFrame, ML("MMS Info"));
	}
	else if (pInfoData->AcceptInfo.infoType == INFO_REPORT)
	{
		// left
		SendMessage(hWndFrame, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDM_INFOBACK, 1), 
			(LPARAM)STR_MMS_OK);
		// right
		SendMessage(hWndFrame, PWM_SETBUTTONTEXT, 0, (LPARAM)(""));
		SetWindowText(hWndFrame, STR_MMS_REPORT);
	}
	else
	{
		// left
		SendMessage(hWndFrame, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDM_INFOBACK, 1), 
			(LPARAM)STR_MMS_OK);		
		// right
		SendMessage(hWndFrame, PWM_SETBUTTONTEXT, 0, (LPARAM)(""));
		SetWindowText(hWndFrame, ML("Details"));
	}
	SendMessage(hWndFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)ML(""));

	if(pInfoData->AcceptInfo.infoType != INFO_REPORT)
	{
		if (pInfoData->bNext)
			SendMessage(hWndFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, RIGHTICON), 
			(LPARAM)MMS_RIGHTARROW_PATH);
		else	
			SendMessage(hWndFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, RIGHTICON), 
			(LPARAM)"");
		
		if (pInfoData->bPre)
			SendMessage(hWndFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, LEFTICON), 
			(LPARAM)MMS_LEFTARROW_PATH);
		else
			SendMessage(hWndFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, LEFTICON), 
			(LPARAM)"");
	}
	else
	{
		SendMessage(hWndFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, RIGHTICON), 
			(LPARAM)"");
		SendMessage(hWndFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, LEFTICON), 
			(LPARAM)"");
	}
	
    return;
}
