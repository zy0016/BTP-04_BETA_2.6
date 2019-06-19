 /***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : MMS
 *
 * Purpose  : display the MMS
 *            
\**************************************************************************/

#include "MmsGlobal.h"
#include "smilparse.h"
#include "MmsDisplay.h"
#include "vcaglb.h"
#include "MmsInterface.h"
#include "MmsUi.h"
#include "PWBE.h"
#include "vcardvcal.h"
/*********************************************************************\
                    define consts
**********************************************************************/

#define MMS_IME_SAVE            WM_USER + 706
#define MMS_IME_SAVEOBJ         WM_USER + 705
#define MMS_DEL_MSG             WM_USER + 707
#define MMS_MOVE_FOLDER         WM_USER + 708
/**********************************************
*
*   menu template
*
***********************************************/
// write message sub menu
/*static const MENUITEMTEMPLATE WriteMenu[] = 
{
    //{ MF_STRING,    IDM_WRITE_SMS,   ML("SMS"),    NULL },
    { MF_STRING,    IDM_WRITE_MMS,   ML("MMS"),    NULL },
    { MF_STRING,    IDM_WRITE_EMAIL, ML("E-mail"), NULL },
    { MF_END, 0, NULL, NULL }
};
// reply message sub menu
static const MENUITEMTEMPLATE ReplyMenu[] = 
{
    { MF_STRING,    IDM_REPLY_SENDER,  ML("To sender"), NULL },
    { MF_STRING,    IDM_REPLY_ALL,     ML("To all"),    NULL },
    { MF_STRING,    IDM_REPLY_SMS,     ML("Via SMS"),   NULL },
    { MF_END, 0, NULL, NULL }
};
// save to contacts sub menu
static const MENUITEMTEMPLATE SavecontactMenu[] = 
{
    { MF_STRING,    IDM_SAVECONTACT_NEW,  ML("New contacts"),   NULL },
    { MF_STRING,    IDM_SAVECONTACT_ADD,  ML("Add to contact"), NULL },
    { MF_END, 0, NULL, NULL }
};
// find sub menu
static const MENUITEMTEMPLATE ReplyMenu[] = 
{
    { MF_STRING,    IDM_FIND_PHONE,  ML("Phone number"), NULL },
    { MF_STRING,    IDM_FIND_EMAIL,  ML("E-mail address"),    NULL },
    { MF_STRING,    IDM_FIND_URL,    ML("Web address"),   NULL },
	{ MF_STRING,    IDM_FIND_COORD,  ML("Coordinates"), NULL },
    { MF_END, 0, NULL, NULL }
};
*/
/*********************************************************************\
                    函数声明
**********************************************************************/
LRESULT MMSDisplayWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static void DispImage(HWND hWnd, const PSTR pIMGData, int imgType,
                       int x, int y, int width, int height);
static void DispText(HDC hdc, long nFirstLine, int nAllLines, int x, int y);
static void MoveDispWnd(HWND hWnd, int iDerection, int x);
static BOOL CreateEditCtrl(HWND hWnd);
static void SetDispEdit(void);
static int  CanMoved(HWND hwnd, MultiMMS * pMMs, int iDerection);
static int  ReadMMSFromFile(PSTR mmsFileName);
static int  CalTxtLineLen(PCSTR str);
static int  CalLineNumbers(TotalMMS * tMMs, int nSlide);
static int  MakeRandomFileName(PSTR pFileNameBuf, PSTR pSuffix);
static void MmsView_SetWndText(HWND hWndFrame, HWND hWnd);

static BOOL CallImeDirect(HWND hWnd, PSTR oldName, UINT nType);
static void GetValidName(char *origionName, int imgOrAudio, int nType, 
                         char *validName);
static void MoveDispWndDrag(HWND hWnd, int iDerection, int move);
static void CountScrollPos(HWND hWnd, SCROLLINFO vsi);
static void CountImagePos(int width, int height, int *stretchWidth, 
                          int *stretchHeight, int *sx);
static void OnPaintImage(HDC hMemdc, int imgType, int sWidth, int sHeight, int StartX);
static BOOL PaintJpgHardware(HWND hCall, SIZE OrgSize);
static BOOL PaintObjJpgHardware(HWND hCall, HDC hMemdc, HDC hdc,SIZE OrgSize);

static void MMSView_SetIndicator(HWND hWnd);
static BOOL MMS_ObjectsReco(mmsMetaNode *pAttatch);
BOOL MMS_SelectDestination(HWND hWndFrame, HWND hWnd, int msg, PCSTR title, 
					PCSTR pName, PCSTR pdata, int datalen, int metaType);

static void CannotSupportRing( BYTE bRingIsEnd);
static void MmsObj_OnShowWnd(HWND hWnd);
static HWND MMS_ViewVcardOrVcal(HWND hFrameWnd, HWND hWnd, void* pData, int len, int nType);

void StopRingCallBackFun( BYTE bRingIsEnd);

void MMS_PaintBkFunc(HGIFANIMATE hGIFAnimate, BOOL End, int x, int y, HDC hdcMem);

BOOL MMS_ViewImage(HWND hWndFrame, HWND hWndCall, mmsMetaNode *pImg);
BOOL MMS_SaveObject(HWND hWndFrame, HWND hWnd, int msg, PCSTR title, 
					PCSTR pName, PCSTR pdata, int datalen);
BOOL MMS_InputFilename(HWND hWndFrame, HWND hWndCall, int msg, PCSTR oldfilename, 
					   PCSTR title, PCSTR data, int datalen);

extern BOOL    APP_CallPhoneNumber(const char * pPhoneNumber);
extern BOOL APP_EditSMS(HWND hParent,const char* PSZRECIPIENT,const char* PSZCONTENT);
extern BOOL CreateMailEditWnd(HWND hParent, const char *PSZRECIPIENT, const char *PSZRECIPIENT2, 
        const char *PSZTITLE, const char *PSZCONTENT, const char *PSZATTENMENT, 
        const SYSTEMTIME *PTIME, DWORD handle, int folderid);
extern int vCard_Reader( unsigned char* pData, int Datalen, vCard_Obj** ppObj );
extern int vCard_Clear( int type, void * pbuf  );
extern int vCal_Reader( unsigned char* pData, int Datalen, vCal_Obj** ppObj );
extern int vCal_Clear( int type, void * pbuf  );
extern mmsMetaNode* GetAttatchNotInSmil(mmsMetaNode *p);
extern void MMSC_ModifyFolderUnread(int nFolder, int nSign, int nCount);
extern int WBM_ADD_FROMURL(char *szinput, HWND hWnd);
extern HBROWSER App_WapRequestUrl(char *szurl);

extern MmsWndClass MMSWindowClass[MAX_CLASS_NUM];
/*********************************************************************\
                    全局变量
**********************************************************************/
PSTR     imgfix[MAX_IMGTYPE_NUM] = {".", ".gif", ".jpg", ".bmp", ".wbmp", ".png"};
PSTR     audiofix[MAX_AUDIOTYPE_NUM] = {".", ".amr", ".mid", ".wav", ".mmf"};
PSTR     txtfix[MAX_TXTTYPE_NUM] = {".", ".txt", ".vcs", ".vcf"};

// when view mms, it need to parse the mms firstly
int ReadAndParse(const char* pszFileName,TotalMMS *dispMMS)
{
    int         hFile;    
    char        *pBody;
    PARSETRELT  *pMMS = NULL;   
    TAG_SMIL    *ParseSmil;
    int         bodyLen;

    chdir(FLASHPATH);
    hFile = MMS_CreateFile(pszFileName, O_RDONLY);
    if (-1 == hFile)
    {
        MMSMSGWIN("open file failure", STR_MMS_MMS, MMS_WAITTIME);
        return ERR_NOTFOUND;
    }
    // 判断是否mms文件
    if (!MMS_IsMyfile(hFile))
    {
        MMS_CloseFile(hFile);
        return ERR_NOTMMS;
    }

    pMMS = MMS_malloc(sizeof(PARSETRELT));
    ParseSmil = MMS_malloc(sizeof(TAG_SMIL));
    
    if (!pMMS || !ParseSmil) 
    {
        MMS_free(ParseSmil);
        MMS_free(pMMS);
        MMS_CloseFile(hFile);
        return ERR_MALLOCFAI;
    }
    memset(pMMS, 0, sizeof(PARSETRELT));    
    memset(ParseSmil,0,sizeof(TAG_SMIL));
    
    pMMS->ParseSmil = ParseSmil;
    pMMS->hMeta = NULL;

    lseek(hFile, MMS_HEADER_SIZE, SEEK_SET);
    read(hFile, &dispMMS->mms.wspHead, sizeof(WSPHead));

    bodyLen = MMS_GetFileSize(pszFileName) - MMS_HEADER_SIZE - sizeof(WSPHead);

    pBody = MMS_malloc(bodyLen);

    read(hFile, pBody, bodyLen);

    MMS_CloseFile(hFile);

    pMMS->Parsehead = dispMMS->mms.wspHead;

	MMSMSGWIN("MMS:Before parse\r\n");
    if (stricmp(pMMS->Parsehead.ConType, "application/vnd.wap.multipart.related") == 0 
        || stricmp(pMMS->Parsehead.ConType, "application/vnd.wap.multipart.mixed") == 0 )
        ParseRecvContent(pBody,bodyLen,pMMS); 
    else if(strnicmp(pMMS->Parsehead.ConType, "text/", 5) == 0 )
        ParseText(pBody, bodyLen, pMMS);
    else if(strnicmp(pMMS->Parsehead.ConType, "image/", 6) == 0 )
        ParseImage(pBody, bodyLen, pMMS);
     
	MMSMSGWIN("MMS:after parse\r\n");
    MMS_free(pBody);

    if (pMMS->ParseSmil == NULL || pMMS->ParseSmil->nParNum == 0)
        dispMMS->mms.wspHead.nSlidnum = 1;
    else if (pMMS->ParseSmil->nParNum > MMS_MAXNUM)
        dispMMS->mms.wspHead.nSlidnum = MMS_MAXNUM;
    else
        dispMMS->mms.wspHead.nSlidnum = pMMS->ParseSmil->nParNum;

	MMSMSGWIN("MMS:Before convertype\r\n");
    ConvertType(pMMS, dispMMS);

	MMSMSGWIN("MMS:after convertype\r\n");
    dispMMS->pAttatch = pMMS->hMeta;

    if (stricmp(pMMS->Parsehead.ConType, "application/vnd.wap.multipart.mixed") == 0 )
        dispMMS->mms.wspHead.nSlidnum = pMMS->ParseSmil->nParNum;

	MMSMSGWIN("MMS:before free smil\r\n");
    PARSER_FreeSmil(pMMS->ParseSmil);
    MMS_free(pMMS);
    
    return RETURN_OK;
}

// free meta which MMS_malloc in ReadAndParse
void FreeMeta(mmsMetaNode *pMeta)
{   
    mmsMetaNode *pnode;

    while(pMeta)
    {
        pnode = pMeta;
        pMeta = pnode->pnext  ;
        
        MMS_free(pnode->Content_Id);
        MMS_free(pnode->Content_Location);
        MMS_free(pnode->Content_Type);
        MMS_free(pnode->Metadata);      
        MMS_free(pnode);
    }
}
// when view end, it should free the resouce and update icons
static void OnDispEnd(PMMS_VIEWCREATEDATA pViewData)
{
    PMSGHANDLENAME pmsgHandle;
    MU_MsgNode     msgnode;
    int            hMmsFile;
    WSPHead        wspHead;
    int            nret;
    int            nCurFolder;
    
	if (pViewData->pMsgHandle == NULL)
	{
		FreeMeta(pViewData->pTmms->pAttatch);
		
#ifdef MMS_DEBUG
		EndObjectDebug();
#endif
		return;
	}

    nCurFolder = GetCurFolder();
	pmsgHandle = pViewData->pMsgHandle;

    if (pmsgHandle->nStatus == MMFT_UNREAD)
    {
        nret = MMS_ModifyType(pmsgHandle->msgname, TYPE_READED, -1);
        if(nret)
        {
            ModifyMsgNode((DWORD)pmsgHandle, pmsgHandle->msgname, MMFT_READED);
            MMSPro_ReadReply(pmsgHandle->msgname);
            pmsgHandle->nStatus = MMFT_READED;
            
//            if ( nCurFolder == MU_INBOX)
//            {
                MMSC_ModifyUnread(COUNT_DEC, 0);
                MMSC_ModifyFolderUnread(nCurFolder, COUNT_DEC, 0);
//            }
        }
    }

    chdir(FLASHPATH);
    hMmsFile = MMS_CreateFile(pmsgHandle->msgname, O_RDONLY);
    lseek(hMmsFile, MMS_HEADER_SIZE, SEEK_SET);
    read(hMmsFile, &wspHead, sizeof(WSPHead));
    MMS_CloseFile(hMmsFile);
    
    if (nCurFolder == MMS_GetFileFolder(pmsgHandle->msgname))
    {
        FillMsgNode(&msgnode, MU_MSG_MMS, pmsgHandle->nStatus, wspHead.date, 
            (DWORD)pmsgHandle, wspHead.subject, wspHead.from, NULL);

		if(MMS_GetFileType(pmsgHandle->msgname) == TYPE_SENDED)
			FillMsgNode(&msgnode, MU_MSG_MMS, pmsgHandle->nStatus, wspHead.date, (DWORD)pmsgHandle,
                    wspHead.subject, wspHead.to, NULL);
		else
			FillMsgNode(&msgnode, MU_MSG_MMS, pmsgHandle->nStatus, wspHead.date, (DWORD)pmsgHandle,
                    wspHead.subject, wspHead.from, NULL);
				
        SendMessage(pViewData->DispMsgInfo.hWnd, PWM_MSG_MU_MODIFIED, 
			MAKEWPARAM(MU_ERR_SUCC, MU_MDU_MMS), 
            (LPARAM)&msgnode);        
    }
        
    FreeMeta(pViewData->pTmms->pAttatch);

#ifdef MMS_DEBUG
            EndObjectDebug();
#endif
}

// read mms in inbox,sent,sendbox etc.
BOOL MMS_ReadMsg(HWND hWnd, PMSGHANDLENAME  pMsgNode, BOOL bPre, BOOL bNext)
{
    int				 nRet;
	int				 nType = OTHER_VIEW;
	static TotalMMS         DisplayTotalMMS;
	
#ifdef MMS_DEBUG
    StartObjectDebug();
#endif
	WaitWindowStateEx(NULL, TRUE, STR_MMS_OPENNING, NULL, NULL, STR_MMS_CANCEL);
    //MMS_WaitWindowState(NULL, TRUE);

    
	MMSMSGWIN("MMS:Before readandparse\r\n");
    nRet = ReadAndParse(pMsgNode->msgname, &DisplayTotalMMS);
	MMSMSGWIN("MMS:after readandparse\r\n");
    if (nRet != RETURN_OK)
    {
        WaitWindowStateEx(NULL, FALSE, STR_MMS_OPENNING, NULL, NULL, STR_MMS_CANCEL);
        return FALSE;
    }

    //if want to add judge avs type
    //add code here
    
	if (pMsgNode->nStatus == MMFT_UNREAD || pMsgNode->nStatus == MMFT_READED
		|| pMsgNode->nStatus == MMFT_UNRECV)
		nType = RECEIVE_VIEW;

	MMSMSGWIN("MMS:Before calldisplaywnd\r\n");
    CallDisplayWnd(&DisplayTotalMMS, MuGetFrame(), hWnd, 0, nType, pMsgNode, bPre, bNext); 

	MMSMSGWIN("MMS:after calldisplaywnd\r\n");
	WaitWindowStateEx(NULL, FALSE, STR_MMS_OPENNING, NULL, NULL, STR_MMS_CANCEL);
    //MMS_WaitWindowState(NULL, FALSE);

    return TRUE;
}


/*********************************************************************\
*           create  display window     
**********************************************************************/
void CallDisplayWnd( TotalMMS * pmms, HWND hWndFrame, HWND hWndPre, UINT msg, UINT nType, 
					PMSGHANDLENAME pHandle, BOOL bPre, BOOL bNext)
{
    WNDCLASS				wc;
    HWND					hwndDisplay;
	HMENU                   hMenu = NULL;
	MMS_VIEWCREATEDATA		mViewData;
	RECT                    rClient;
    TotalMMS*				pDispMms;

    if (pmms == NULL || hWndPre == NULL || hWndFrame == NULL)
    {
        SendMessage(hWndPre, msg, nType, 0);
        return;
    }

	pDispMms = malloc(sizeof(TotalMMS));
	memcpy(pDispMms, pmms, sizeof(TotalMMS));
	
	mViewData.hWndFrame = hWndFrame;
	mViewData.pMsgHandle = pHandle;
    mViewData.pTmms = pDispMms;
    mViewData.curSlide = 0;
    
    mViewData.DispMsgInfo.hWnd = hWndPre;
    mViewData.DispMsgInfo.msg = msg;
    mViewData.DispMsgInfo.nType = nType;
	mViewData.bPre = bPre;
	mViewData.bNext = bNext;

    //register class    
    // Register mms main window class
    wc.style         = 0;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = sizeof(MMS_VIEWCREATEDATA);
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.lpszMenuName  = NULL;
    wc.lpfnWndProc   = MMSWindowClass[6].wndProc;
    wc.lpszClassName = MMSWindowClass[6].szClassName;
    wc.hbrBackground = NULL;
        
    RegisterClass(&wc);

	// no options
	if (nType != EDIT_VIEW)
		hMenu = CreateMenu();
	GetClientRect(hWndFrame, &rClient);
	mViewData.hMenu = hMenu;
	mViewData.hWndTextView = NULL;

	hwndDisplay = CreateWindow(
		MMSWindowClass[6].szClassName,
		MMS_DISPLAY_APP,
		WS_VISIBLE | WS_CHILD, 
        rClient.left, rClient.top, rClient.right-rClient.left, rClient.bottom-rClient.top,
		hWndFrame, 
		NULL,
        NULL, 
        (PVOID)&mViewData
		);

	MmsView_SetWndText(hWndFrame, hwndDisplay);

	SetFocus(hwndDisplay);
	if (nType != EDIT_VIEW)
		PDASetMenu(hWndFrame, hMenu);
	// left button
	if (mViewData.DispMsgInfo.nType == RECEIVE_VIEW)
		SendMessage(hWndFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)ML("Reply"));
	else if (!MMS_ObjectsReco(mViewData.pTmms->pAttatch) 
		&& mViewData.DispMsgInfo.nType != EDIT_VIEW)
		SendMessage(hWndFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)ML("Forward"));
	else
		SendMessage(hWndFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)(""));

	SendMessage(hWndFrame, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(MENU_DISPLAY_EXIT, 0), 
		(LPARAM)ML("Back"));
	if (nType == EDIT_VIEW)
		SendMessage(hWndFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)ML(""));
	else
		SendMessage(hWndFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)("Options"));

	MMSView_SetIndicator(hwndDisplay);
}
// set window text of view
static void MmsView_SetWndText(HWND hWndFrame, HWND hWnd)
{
	PMMS_VIEWCREATEDATA		pViewData;
	ABNAMEOREMAIL           abname;
	char                    *dupTo = NULL;
	char					*token = NULL;
	char                    name[SIZE_1];
	int                     len = 0, len1;
	char					dispName[SIZE_1];
	char					At[2] = {(char)0xFD, 0};

	pViewData = GetUserData(hWnd);
	memset(&abname, 0, sizeof(ABNAMEOREMAIL));

	name[0] = 0;

	len1 = UTF8ToMultiByte(CP_ACP, 0, pViewData->pTmms->mms.wspHead.from, -1, dispName, SIZE_1 - 1,NULL, NULL);
	dispName[len1] = 0;

	if (pViewData->DispMsgInfo.nType == RECEIVE_VIEW)		// received
	{	
		if(strstr(dispName, At))
		{
			if (APP_GetNameByEmail(dispName, &abname))
				SetWindowText(hWndFrame, abname.szName);
			else
			{
				if (pViewData->pTmms->mms.wspHead.from[0] == 0)
					SetWindowText(hWndFrame, MMS_DISPLAY_APP);
				else
				{
					SetWindowText(hWndFrame, dispName);
				}
			}
		}
		else
		{
			if (APP_GetNameByPhone(dispName, &abname))
				SetWindowText(hWndFrame, abname.szName);
			else
			{
				if (pViewData->pTmms->mms.wspHead.from[0] == 0)
					SetWindowText(hWndFrame, MMS_DISPLAY_APP);
				else
				{
					SetWindowText(hWndFrame, dispName);
				}
			}
		}
	}
	else if (pViewData->DispMsgInfo.nType == EDIT_VIEW)		// edit view
		SetWindowText(hWndFrame, MMS_DISPLAY_APP);
	else
	{
		dupTo = MMS_malloc(strlen(pViewData->pTmms->mms.wspHead.to) + 1);
		len1 = UTF8ToMultiByte(CP_ACP, 0, pViewData->pTmms->mms.wspHead.to, -1, 
			dupTo, strlen(pViewData->pTmms->mms.wspHead.to),NULL, NULL);
		dupTo[len1] = 0;
		//strcpy(dupTo, pViewData->pTmms->mms.wspHead.to);
		token = MMS_chrtok(dupTo, MMS_ASEPCHR, MMS_ASEPCHRF);
		while (token != NULL) 
		{
			if (*token != '\0')
			{
				if(strstr(token, At))
				{
					if (APP_GetNameByEmail(token, &abname))
					{
						strcat(name, abname.szName);
						len += strlen(abname.szName);					
					}
					else
					{
						strcat(name, token);
						len += strlen(token);
					}
				}
				else
				{
					if (APP_GetNameByPhone(token, &abname))
					{
						strcat(name, abname.szName);
						len += strlen(abname.szName);					
					}
					else
					{
						strcat(name, token);
						len += strlen(token);
					}
				}
				
				strcat(name, ";");
				len++;
				if (len >= 64)
					break;
			}
			token = MMS_chrtok(NULL, MMS_ASEPCHR, MMS_ASEPCHRF);
		}// end while

		len = strlen(name);
		if(len > 0)
		{
			if(name[len - 1] == ';')
				name[len - 1] = 0;
		}
		
		if (name[0] == 0)
			SetWindowText(hWndFrame, MMS_DISPLAY_APP);
		else
			SetWindowText(hWndFrame, name);
	}//end else
}
// set the icon on title bar
static void MMSView_SetIndicator(HWND hWnd)
{
	PMMS_VIEWCREATEDATA  pData;
	
	pData = GetUserData(hWnd);
	
	if (pData->pTmms->mms.slide[pData->curSlide].pAudio)
	{
		if (pData->bNext)
			SendMessage(pData->hWndFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, RIGHTICON), 
				(LPARAM)MMS_RIGHT2_PATH);
		else
			SendMessage(pData->hWndFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, RIGHTICON), 
				(LPARAM)MMS_RIGHTAUDIO_PATH);
	}
	else if (pData->bNext)
		SendMessage(pData->hWndFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, RIGHTICON), 
			(LPARAM)MMS_RIGHTARROW_PATH);
	else	
		SendMessage(pData->hWndFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, RIGHTICON), 
			(LPARAM)"");
	
	if (pData->bPre)
		SendMessage(pData->hWndFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, LEFTICON), 
			(LPARAM)MMS_LEFTARROW_PATH);
	else
		SendMessage(pData->hWndFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, LEFTICON), 
		(LPARAM)"");
}

// on mms display window porc WM_CREATE
static BOOL MmsView_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
{
	PMMS_VIEWCREATEDATA       pCreateData;
	HWND                      hWndFrame;
	BOOL                      bImage;
	PVOID                     pImagadata = NULL,pTextData = NULL;
	int                       imglen = 0, txtlen = 0;

	pCreateData = GetUserData(hWnd);
	
    memcpy(pCreateData,lpCreateStruct->lpCreateParams,sizeof(MMS_VIEWCREATEDATA));

	hWndFrame = pCreateData->hWndFrame;
    
	if (pCreateData->pTmms->mms.slide[pCreateData->curSlide].pAudio)
		SetTimer(hWnd, TIMER_PLAY_AUDIO, 1, NULL);  
    
	// create Text viewer
	bImage = !(pCreateData->pTmms->mms.slide[pCreateData->curSlide].pImage == NULL);
	if (bImage)
	{
		pImagadata = pCreateData->pTmms->mms.slide[pCreateData->curSlide].pImage->Metadata;
		imglen = pCreateData->pTmms->mms.slide[pCreateData->curSlide].pImage->Metalen;
	}
	if (pCreateData->pTmms->mms.slide[pCreateData->curSlide].pText)
	{
		pTextData = pCreateData->pTmms->mms.slide[pCreateData->curSlide].pText->Metadata;
		txtlen = pCreateData->pTmms->mms.slide[pCreateData->curSlide].pText->Metalen;
	}
	pCreateData->hWndTextView = PlxTextView(hWndFrame, hWnd, pTextData, txtlen, 
		bImage, NULL, pImagadata, imglen);

	return TRUE;
}

// judge wether the mms contains objects :images and sounds
static BOOL MMS_JudgeContainObjects(mmsMetaNode *pAttatch)
{
	mmsMetaNode *p;

	p = pAttatch;

	while (p)
	{
//		if (p->MetaType/10 == META_AUDIO || p->MetaType/10 == META_IMG 
// 			||p->MetaType == META_TEXT_VCALE || p->MetaType == META_TEXT_VCARD)
			return TRUE;
		p = p->pnext;
	}
	return FALSE;
}

// judge wether the mms contains objects which can not be recognized
static BOOL MMS_ObjectsReco(mmsMetaNode *pAttatch)
{
	mmsMetaNode *p;

	p = pAttatch;

	while (p)
	{
		if (p->MetaType == META_OTHER)
			return TRUE;
		p = p->pnext;
	}
	return FALSE;
}

// response the WM_InitMenu in view window proc
static void MmsView_OnInitmenu(HWND hWnd)
{
	PMMS_VIEWCREATEDATA			pCreateData;
	HMENU  hMenu = NULL, hSubWrite = NULL, hSubReply = NULL;
	HMENU  hSubFind = NULL, hSubSaveContact = NULL;
	int    nMenuItem = 0;
	DWORD  high, low;
	int    ret;

	pCreateData = GetUserData(hWnd);
	
    hMenu = pCreateData->hMenu;
    if(hMenu)
    {
        nMenuItem = GetMenuItemCount(hMenu);
        while(nMenuItem > 0)
        {
            nMenuItem--;
            DeleteMenu(hMenu,nMenuItem,MF_BYPOSITION);
        }
    }

	ret = SendMessage(pCreateData->hWndTextView, TVM_GETCURHL, (WPARAM)&high, (LPARAM)&low);
	// go to url,add to bookmarks
	// show only if there is highlighted web address in msg body
	if (ret != -1 && low == TVS_URL)
	{
		AppendMenu(hMenu, MF_ENABLED, IDM_GOTO_URL, ML("Go to URL"));
		AppendMenu(hMenu, MF_ENABLED, IDM_ADDTO_BK, ML("Add to bookmarks"));
	}

	// view on map,set as destination,save as waypoint
	// show only if there is highlighted coordinate in msg body
	if (ret != -1 && low == TVS_COORDINATE)
	{
		AppendMenu(hMenu, MF_ENABLED, IDM_VIEW_MAP, ML("View on map"));
		AppendMenu(hMenu, MF_ENABLED, IDM_SET_DEST, ML("Set as destination"));
		AppendMenu(hMenu, MF_ENABLED, IDM_SAVE_POINT, ML("Save as waypoint"));
	}

	// write message....
	// show only if there is highlighted phone number or email address in msg body
	if (ret != -1 && (low == TVS_EMAIL || low == TVS_NUMBER))
	{
		hSubWrite = CreateMenu();
		if (low == TVS_NUMBER)		//phone
			AppendMenu(hSubWrite, MF_ENABLED, IDM_WRITE_SMS, ML("SMS"));
		AppendMenu(hSubWrite, MF_ENABLED, IDM_WRITE_MMS, ML("MMS"));
		if (low == TVS_EMAIL)		// email address
			AppendMenu(hSubWrite, MF_ENABLED, IDM_WRITE_EMAIL, ML("E-mail"));

		AppendMenu(hMenu,MF_ENABLED|MF_POPUP, (DWORD)hSubWrite,ML("Write message"));
	}

	// show only if the current msg slide contains sound objects
	if (pCreateData->pTmms->mms.slide[pCreateData->curSlide].pAudio)
	{
		AppendMenu(hMenu, MF_ENABLED, IDM_PLAY_SOUND, ML("Play sound"));
	}
	// show only if the current msg slide contains image objects
	if (pCreateData->pTmms->mms.slide[pCreateData->curSlide].pImage)
	{
		AppendMenu(hMenu, MF_ENABLED, IDM_VIEW_IMG, ML("View image"));
	}

	// show only if the msg is received
	if (pCreateData->DispMsgInfo.nType == RECEIVE_VIEW)  // received
	{
		hSubReply = CreateMenu();
		AppendMenu(hSubReply, MF_ENABLED, IDM_REPLY_SENDER, ML("To sender"));

		if(MMS_GetRecNum(hWnd, (DWORD)pCreateData->pMsgHandle) > 1)
			AppendMenu(hSubReply, MF_ENABLED, IDM_REPLY_ALL, ML("To all"));

		AppendMenu(hSubReply, MF_ENABLED, IDM_REPLY_SMS, ML("Via SMS"));

		AppendMenu(hMenu,MF_ENABLED|MF_POPUP, (DWORD)hSubReply,ML("Reply"));
	}

	// not shown if the attachment can not be recognized.
	if ( !MMS_ObjectsReco(pCreateData->pTmms->pAttatch))
	{
		AppendMenu(hMenu, MF_ENABLED, IDM_MMS_FORWARD, ML("Forward"));
	}
	// show only if the msg is received or if there is highlighted items
	// in msg body
	if (pCreateData->DispMsgInfo.nType == RECEIVE_VIEW || ret != -1 )  // received
	{
		hSubSaveContact = CreateMenu();
		AppendMenu(hSubSaveContact, MF_ENABLED, IDM_SAVECONTACT_NEW, ML("New contact"));
		AppendMenu(hSubSaveContact, MF_ENABLED, IDM_SAVECONTACT_ADD, ML("Add to contact"));

		AppendMenu(hMenu,MF_ENABLED|MF_POPUP, (DWORD)hSubSaveContact,ML("Save to Contacts"));
	}
	// show only if the msg contains media objects,i.e. images and sounds
	if (MMS_JudgeContainObjects(pCreateData->pTmms->pAttatch))
	{
		AppendMenu(hMenu, MF_ENABLED, IDM_MMS_OBJECT, ML("Objects"));
	}
	// show only if there is no highlighted items in msg body and if there is text in msg body
	if (pCreateData->pTmms->mms.slide[pCreateData->curSlide].pText 
		&& pCreateData->pTmms->mms.slide[pCreateData->curSlide].pText->Metalen 
		&& ret == -1 )
	{
		hSubFind = CreateMenu();
		AppendMenu(hSubFind, MF_ENABLED, IDM_FIND_PHONE, ML("Phone number"));
		AppendMenu(hSubFind, MF_ENABLED, IDM_FIND_EMAIL, ML("E-mail address"));
		AppendMenu(hSubFind, MF_ENABLED, IDM_FIND_URL, ML("Web address"));
		AppendMenu(hSubFind, MF_ENABLED, IDM_FIND_COORD, ML("Coordinates"));

		AppendMenu(hMenu,MF_ENABLED|MF_POPUP, (DWORD)hSubFind,ML("Find"));
	}
	// shown only if there is highlighted items in msg body
	if (ret != -1)
	{
		AppendMenu(hMenu, MF_ENABLED, IDM_MMS_HIDE, ML("Hide found items"));
	}
	
	if (pCreateData->DispMsgInfo.nType != 0)	// not from edit
	{
		if (GetCurFolder() == MU_INBOX)
		{
			if(CanMoveToFolder())
				AppendMenu(hMenu, MF_ENABLED, IDM_MOVETO_FOLD, ML("Move to folder"));
		}
		else
			AppendMenu(hMenu, MF_ENABLED, IDM_MOVETO_FOLD, ML("Move to folder"));
			
	}
	AppendMenu(hMenu, MF_ENABLED, IDM_MMS_INFO, ML("Details"));

	if (pCreateData->DispMsgInfo.nType != 0)	// not from edit
		AppendMenu(hMenu, MF_ENABLED, IDM_MMS_DELETE, ML("Delete"));
}

// response the WM_COMMAND in displywndproc
static void MmsView_OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	WORD            mID;
    WORD            msgBox;
	PMMS_VIEWCREATEDATA   pViewData;
	
	mID = LOWORD(wParam);
	msgBox = HIWORD(lParam);
	pViewData = GetUserData(hWnd);
	
	switch(mID)
	{
	case MENU_DISPLAY_EXIT:
		PostMessage(hWnd, WM_CLOSE, 0, 0);
		break;

	case IDM_GOTO_URL:
	case IDM_ADDTO_BK:
	{
		int ret, offset, len;
		DWORD high, low;
		char  *pUrl = NULL;

		// get the high lighted items
		ret = SendMessage(pViewData->hWndTextView, TVM_GETCURHL, (WPARAM)&high, (LPARAM)&low);
		if (ret != -1 && low == TVS_URL)	// highlighted
		{
			offset = LOWORD(high);
			len = HIWORD(high);
			pUrl = MMS_malloc(len + 1);
			strncpy(pUrl, 
				pViewData->pTmms->mms.slide[pViewData->curSlide].pText->Metadata + offset,
				len);
			pUrl[len] = 0;
			// open browser and go to high lighted web address(goto url)
			if (mID == IDM_GOTO_URL)
				App_WapRequestUrl(pUrl);
			// add highlighted web address to browser bookmark(add to bk)
			else if (mID == IDM_ADDTO_BK)
				WBM_ADD_FROMURL(pUrl, pViewData->hWndFrame);
			MMS_free(pUrl);
		}
		break;
	}

	case IDM_VIEW_MAP:
		// get the high lighted items...
		// view highlighted coordinates on the map....
		break;
		
	case IDM_SET_DEST:
		// get the high lighted items...
		// set highlighted coordinates as destination....
		break;

	case IDM_SAVE_POINT:
		// get the high lighted items...
		// Save highlighted coordinates as waypoint....
		break;
    
	case IDM_WRITE_SMS:
	case IDM_WRITE_MMS:
	case IDM_WRITE_EMAIL:
	{		
		// get the high lighted items
		// select highlighted phone number or email address to recipient
		// of the msg and open mail editor
		SYSTEMTIME time;
		int		ret = 0, offset = 0,len = 0;
		DWORD   high = 0, low = 0;
		char    *pAddr = NULL;

		ret = SendMessage(pViewData->hWndTextView, TVM_GETCURHL, (WPARAM)&high, (LPARAM)&low);
		if (ret == -1)
			break;
		// get highlighted items
		offset = LOWORD(high);
		len = HIWORD(high);
		pAddr = MMS_malloc(len + 1);
		strncpy(pAddr, 
			pViewData->pTmms->mms.slide[pViewData->curSlide].pText->Metadata + offset,
			len);
		pAddr[len] = 0;
		if (low == TVS_NUMBER)	// highlighted
		{
			if (mID == IDM_WRITE_SMS)
				APP_EditSMS(pViewData->hWndFrame, pAddr, NULL);
			else if (mID == IDM_WRITE_MMS)
				APP_EditMMS(pViewData->hWndFrame, hWnd, 0, MMS_CALLEDIT_MOBIL,pAddr);		
		}
		else if (low == TVS_EMAIL)
		{
			if (mID == IDM_WRITE_MMS)
				APP_EditMMS(pViewData->hWndFrame, hWnd, 0, MMS_CALLEDIT_MOBIL,pAddr);
			else if (mID == IDM_WRITE_EMAIL)
			{
				GetLocalTime(&time);
				CreateMailEditWnd(pViewData->hWndFrame, pAddr, NULL, NULL, NULL, NULL, 
					&time, -1, -1);
			}
		}
		MMS_free(pAddr);
		break;
	}

	case IDM_PLAY_SOUND:
		MMS_PlayAudio(hWnd, pViewData->pTmms->mms.slide[pViewData->curSlide].pAudio->Content_Location, 
                      pViewData->pTmms->mms.slide[pViewData->curSlide].pAudio->MetaType%10,
                      pViewData->pTmms->mms.slide[pViewData->curSlide].pAudio->Metadata, 
                      pViewData->pTmms->mms.slide[pViewData->curSlide].pAudio->Metalen,
                      StopRingCallBackFun);
		break;

	case IDM_VIEW_IMG:
		// create a window for this function
		MMS_ViewImage(pViewData->hWndFrame, hWnd, 
			pViewData->pTmms->mms.slide[pViewData->curSlide].pImage);
		break;

	case IDM_REPLY_SENDER:
		// should pass the sender??
		if (pViewData->pMsgHandle == 0)
			break;
		CallMMSEditWnd(pViewData->pMsgHandle->msgname,NULL, hWnd, 0, MMS_CALLEDIT_REPLY,
                    NULL, NULL);
		break;

	case IDM_REPLY_ALL:
		if (pViewData->pMsgHandle == 0)
			break;
		CallMMSEditWnd(pViewData->pMsgHandle->msgname, NULL, hWnd, 0, MMS_CALLEDIT_REPLYALL,
                    NULL, NULL);
		break;

	case IDM_REPLY_SMS:
		// call sms editor
		APP_EditSMS(pViewData->hWndFrame, pViewData->pTmms->mms.wspHead.from, NULL);
		break;

	case IDM_MMS_FORWARD:
		if (pViewData->pMsgHandle == 0)
			break;
		CallMMSEditWnd(pViewData->pMsgHandle->msgname, NULL, hWnd, 0, 
			MMS_CALLEDIT_TRAN, NULL, NULL);
		break;

	case IDM_SAVECONTACT_NEW:
	case IDM_SAVECONTACT_ADD:
	{
		int		ret = 0, offset = 0,len = 0;
		DWORD   high = 0, low = 0;
		char    *pAddr = NULL;
		ABNAMEOREMAIL  ab;

		ab.szName[0] = 0;
			
		ret = SendMessage(pViewData->hWndTextView, TVM_GETCURHL, (WPARAM)&high, (LPARAM)&low);
		// get highlighted items
		if (ret != -1 && low != TVS_COORDINATE)	// highlighted
		{
			offset = LOWORD(high);
			len = HIWORD(high);
			pAddr = MMS_malloc(len + 1);
			strncpy(pAddr, 
				pViewData->pTmms->mms.slide[pViewData->curSlide].pText->Metadata + offset,
				len);
			pAddr[len] = 0;
			// call pb interface
			if (low == TVS_URL)
				ab.nType = AB_URL;
			else if (low == TVS_NUMBER)
				ab.nType = AB_NUMBER;
			else if (low == TVS_EMAIL)
				ab.nType = AB_EMAIL;

			if (len < AB_MAXLEN_EMAILADDR)
				strcpy(ab.szTelOrEmail, pAddr);
			else
			{
				strncpy(ab.szTelOrEmail, pAddr, AB_MAXLEN_EMAILADDR - 1);
				ab.szTelOrEmail[AB_MAXLEN_EMAILADDR - 1] = 0;
			}

			len = UTF8ToMultiByte(CP_ACP, 0, ab.szTelOrEmail, -1, ab.szTelOrEmail, AB_MAXLEN_EMAILADDR - 1, NULL, NULL);
			ab.szTelOrEmail[len] = 0;

			if (mID == IDM_SAVECONTACT_NEW)
				APP_SaveToAddressBook(pViewData->hWndFrame, hWnd,0, &ab, AB_NEW);
			else
				APP_SaveToAddressBook(pViewData->hWndFrame, hWnd,0, &ab, AB_UPDATE);
			MMS_free(pAddr);
		}
		else
		{			
			if (pViewData->DispMsgInfo.nType == RECEIVE_VIEW)	//receive
			{
				pAddr = pViewData->pTmms->mms.wspHead.from;
				if(strstr(pViewData->pTmms->mms.wspHead.from, "@"))
					ab.nType = AB_EMAIL;
				else
					ab.nType = AB_NUMBER;
				
				if (strlen(pAddr) < AB_MAXLEN_EMAILADDR)
				{
					strcpy(ab.szTelOrEmail, pAddr);
				}
				else
				{
					strncpy(ab.szTelOrEmail, pAddr, AB_MAXLEN_EMAILADDR - 1);
					ab.szTelOrEmail[AB_MAXLEN_EMAILADDR - 1] = 0;
				}
				
				len = UTF8ToMultiByte(CP_ACP, 0, ab.szTelOrEmail, -1, ab.szTelOrEmail, AB_MAXLEN_EMAILADDR - 1, NULL, NULL);
				ab.szTelOrEmail[len] = 0;

				//save the sender to the pb
				if (mID == IDM_SAVECONTACT_NEW)
					APP_SaveToAddressBook(pViewData->hWndFrame, hWnd,0, 
					&ab, AB_NEW);
				else
					APP_SaveToAddressBook(pViewData->hWndFrame, hWnd,0, 
					&ab, AB_UPDATE);
			}
		}
		break;
	}


	case IDM_MMS_OBJECT:
	{
		int		type = VIEW_CALLOBJ;

		if (pViewData->DispMsgInfo.nType == EDIT_VIEW)
			type = EDIT_CALLOBJ;
		CallMMSObjList(pViewData->hWndFrame, hWnd, pViewData->pTmms->pAttatch, type);
		break;
	}

	case IDM_FIND_URL:
	{
		int ret = 0;

		ret = SendMessage(pViewData->hWndTextView, TVM_FINDNHL, 0, TVS_URL);
		if (ret == -1)		// search found no results
			MsgWin(NULL, NULL, 0, ML("No matches found"), STR_MMS_MMS,
				Notify_Info, STR_MMS_OK, NULL, WAITTIMEOUT);
		break;
	}

	case IDM_FIND_EMAIL:
	{
		int ret = 0;

		ret = SendMessage(pViewData->hWndTextView, TVM_FINDNHL, 0, TVS_EMAIL);
		if (ret == -1)		// search found no results
			MsgWin(NULL, NULL, 0, ML("No matches found"), STR_MMS_MMS,
				Notify_Info, STR_MMS_OK, NULL, WAITTIMEOUT);
		break;
	}

	case IDM_FIND_PHONE:
	{
		int ret = 0;

		ret = SendMessage(pViewData->hWndTextView, TVM_FINDNHL, 0, TVS_NUMBER);
		if (ret == -1)		// search found no results
			MsgWin(NULL, NULL, 0, ML("No matches found"), STR_MMS_MMS,
				Notify_Info, STR_MMS_OK, NULL, WAITTIMEOUT);
		break;
	}

	case IDM_FIND_COORD:
	{
		int ret = 0;

		ret = SendMessage(pViewData->hWndTextView, TVM_FINDNHL, 0, TVS_COORDINATE);
		if (ret == -1)		// search found no results
			MsgWin(NULL, NULL, 0, ML("No matches found"), STR_MMS_MMS, 
				Notify_Info, STR_MMS_OK, NULL, WAITTIMEOUT);
		break;
	}

	case IDM_MMS_HIDE:
		// remove highlights from the msg body
		SendMessage(pViewData->hWndTextView, TVM_HIDEHL, 0, 0);
		break;

	case IDM_MOVETO_FOLD:
		if (pViewData->pMsgHandle == 0)
			break;
		MU_FolderSelection(NULL, hWnd, MMS_MOVE_FOLDER,
			MMS_GetFileFolder(pViewData->pMsgHandle->msgname));
		break;

	case IDM_MMS_INFO:
	{
		ACCEPTINFO  headInfo;
		
		memset(&headInfo, 0, sizeof(ACCEPTINFO));
		
		headInfo.acceptTime = pViewData->pTmms->mms.wspHead.date;               //time
		strcpy(headInfo.From, pViewData->pTmms->mms.wspHead.from);              //from
		strcpy(headInfo.To, pViewData->pTmms->mms.wspHead.to);                  //to
		strcpy(headInfo.Subject, pViewData->pTmms->mms.wspHead.subject);        //subject
		headInfo.MsgSize = pViewData->pTmms->mms.wspHead.nMmsSize;
		if (pViewData->pMsgHandle->nStatus == MMFT_SENDED)
			headInfo.infoType = INFO_SENT;
		else if (pViewData->pMsgHandle->nStatus == MMFT_UNREAD || MMFT_READED)
			headInfo.infoType = INFO_RECEIVE;
        else
			headInfo.infoType = INFO_HEAD;										//mms type                      
		CallDisplayInfo(pViewData->hWndFrame, hWnd, 0, headInfo,FALSE, FALSE);                        //show mms information window
		break;
	}
		
	case IDM_MMS_DELETE:		
		PLXConfirmWinEx(pViewData->hWndFrame, hWnd, ML("Delete message?"), Notify_Request, 
				STR_MMS_MMS, STR_MMS_YES, STR_MMS_NO, MMS_DEL_MSG);
		break;

	default:
        PDADefWindowProc(hWnd, WM_COMMAND, wParam, lParam);
        break;
	}
}

// in view, if user press send key,then make a phone call
static BOOL MmsView_MakePhoneCall(HWND hWnd)
{
	PMMS_VIEWCREATEDATA          pViewData;
	char              *phonenum;
	int               ret, offset, len;
	DWORD             wparam, lparam;

	pViewData = GetUserData(hWnd);
	// if there is high lighted
	ret = SendMessage(pViewData->hWndTextView, TVM_GETCURHL, (WPARAM)&wparam, (LPARAM)&lparam);
	if (ret == -1 || (ret != -1 && lparam != TVS_NUMBER))
	{
		if (pViewData->DispMsgInfo.nType == RECEIVE_VIEW)	//receive
			APP_CallPhoneNumber(pViewData->pTmms->mms.wspHead.from);

		return FALSE;
	}
	else
	{		
		offset = LOWORD(wparam);
		len = HIWORD(wparam);
		
		phonenum = MMS_malloc(len + 1);
		strncpy(phonenum, 
			pViewData->pTmms->mms.slide[pViewData->curSlide].pText->Metadata + offset,
			len);
		phonenum[len] = 0;	
		APP_CallPhoneNumber(phonenum);
		MMS_free(phonenum);
	}
	return TRUE;
}
// response to the viewwndporc WM_KEYDOWN
static BOOL MmsView_OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	PMMS_VIEWCREATEDATA  pData;
	BOOL                 bImage = FALSE;
	char                 *pImagedata = NULL, *pTextData = NULL;
	int                  imglen = 0, txtlen = 0;
	HWND                 hWndFrame = NULL;
	mmsMetaNode*         pAudio;
	
	pData = GetUserData(hWnd);
	hWndFrame = pData->hWndFrame;

	switch(LOWORD(wParam))
	{	
	case VK_F1:
		MmsView_MakePhoneCall(hWnd);
		break;
		
	case VK_RETURN:
	{
		char ButtonText[8];
		
		SendMessage(pData->hWndFrame, PWM_GETBUTTONTEXT, 1, (LPARAM)ButtonText);
		
		if (0 == strcmp(ButtonText, ML("Reply")))
			SendMessage(hWnd, WM_COMMAND,IDM_REPLY_SENDER, 0);
		else if (0 == strcmp(ButtonText, ML("Forward")))
			SendMessage(hWnd, WM_COMMAND, IDM_MMS_FORWARD, 0);
		break;
	}

	case VK_LEFT:		
        if(pData->bPre)
        {
            PostMessage(pData->DispMsgInfo.hWnd, PWM_MSG_MU_PRE, NULL, NULL);
			PostMessage(hWnd,WM_CLOSE,0,0);
        }
		break;

	case VK_RIGHT:		
        if(pData->bNext)
        {
            PostMessage(pData->DispMsgInfo.hWnd, PWM_MSG_MU_NEXT,NULL,NULL); 
			PostMessage(hWnd,WM_CLOSE,0,0);
        }
		break;

		// go to previous slide
	case VK_UP:
		if (pData->curSlide > 0)
		{	
			if(pData->pTmms->mms.slide[pData->curSlide].pAudio != NULL)
			{
				MMS_StopPlay();
			}

			pData->curSlide --;

			pAudio	= pData->pTmms->mms.slide[pData->curSlide].pAudio;
			 
			if(pAudio)
			{ 
				MMS_PlayAudio(hWnd, pAudio->Content_Location, 
					pAudio->MetaType%10,
					pAudio->Metadata, 
					pAudio->Metalen,
					StopRingCallBackFun);
			}

			DestroyWindow(pData->hWndTextView);
			// create Text viewer
			bImage = !(pData->pTmms->mms.slide[pData->curSlide].pImage == NULL);
			if (bImage)
			{
				pImagedata = pData->pTmms->mms.slide[pData->curSlide].pImage->Metadata;
				imglen = pData->pTmms->mms.slide[pData->curSlide].pImage->Metalen;
			}
			if (pData->pTmms->mms.slide[pData->curSlide].pText)
			{
				pTextData = pData->pTmms->mms.slide[pData->curSlide].pText->Metadata;
				txtlen = pData->pTmms->mms.slide[pData->curSlide].pText->Metalen;
			}
			pData->hWndTextView = PlxTextView(hWndFrame, hWnd, pTextData, txtlen, 
				bImage, NULL, pImagedata, imglen);
			SetFocus(pData->hWndTextView);
			MMSView_SetIndicator(hWnd);
		}
		break;
		
		// go to next slide
	case VK_DOWN:
		if (pData->curSlide < pData->pTmms->mms.wspHead.nSlidnum -1)
		{	
				
			if(pData->pTmms->mms.slide[pData->curSlide].pAudio != NULL)
			{
				MMS_StopPlay();
			}

			pData->curSlide ++;

			pAudio	= pData->pTmms->mms.slide[pData->curSlide].pAudio;
			 
			if(pAudio)
			{ 
				MMS_PlayAudio(hWnd, pAudio->Content_Location, 
					pAudio->MetaType%10,
					pAudio->Metadata, 
					pAudio->Metalen,
					StopRingCallBackFun);
			}

			DestroyWindow(pData->hWndTextView);
			// create Text viewer
			bImage = !(pData->pTmms->mms.slide[pData->curSlide].pImage == NULL);
			if (bImage)
			{
				pImagedata = pData->pTmms->mms.slide[pData->curSlide].pImage->Metadata;
				imglen = pData->pTmms->mms.slide[pData->curSlide].pImage->Metalen;
			}
			if (pData->pTmms->mms.slide[pData->curSlide].pText)
			{
				pTextData = pData->pTmms->mms.slide[pData->curSlide].pText->Metadata;
				txtlen = pData->pTmms->mms.slide[pData->curSlide].pText->Metalen;
			}
			pData->hWndTextView = PlxTextView(hWndFrame, hWnd, pTextData, txtlen, 
				bImage, NULL, pImagedata, imglen);
			SetFocus(pData->hWndTextView);
			MMSView_SetIndicator(hWnd);
		}
		break;
				
	case VK_F10://ESC
		SendMessage(hWnd, WM_CLOSE, 0, 0);
		break;

	case VK_F5:
		if(pData->DispMsgInfo.nType != EDIT_VIEW)
			PDADefWindowProc(pData->hWndFrame, WM_KEYDOWN, wParam, lParam);
		break;

	default:
		return PDADefWindowProc(hWnd, WM_KEYDOWN, wParam, lParam);
	}
	return TRUE;
}
// response to the viewwndporc WM_TIMER
static void MmsView_OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	PMMS_VIEWCREATEDATA  pViewData;
	int					 rtn;		

	pViewData = GetUserData(hWnd);

	if(wParam == TIMER_PLAY_AUDIO)     //delay
	{			
		mmsMetaNode *pAudio = pViewData->pTmms->mms.slide[pViewData->curSlide].pAudio;

		if(pAudio)
		{ 
			rtn = MMS_PlayAudio(hWnd, pAudio->Content_Location, 
				pAudio->MetaType%10,
				pAudio->Metadata, 
				pAudio->Metalen,
				StopRingCallBackFun);
			
			if(rtn)
				pViewData->bMusicPlay = TRUE;
			else
			{
				//play fail
				//we can't receive music-stop message,we send it by ourself
				CannotSupportRing(1);
			}
		}		
		KillTimer(hWnd, TIMER_PLAY_AUDIO);		
	}
	else if(wParam == TIMER_PLAY_MULTI_SLIDE)    // start
	{
/*		HWND hCaptureWnd;
		
		hCaptureWnd = GetCapture();
		if(hCaptureWnd == hWnd)
			SendMessage(hCaptureWnd, WM_CANCELMODE, NULL, NULL);
		
		if (pViewData->curSlide == pViewData->pTmms->mms.wspHead.nSlidnum - 1)
		{
			pViewData->curSlide = -1;
		}
		KillTimer(hWnd, TIMER_PLAY_MULTI_SLIDE);
		SendMessage(hWnd, WM_COMMAND, IDM_SLIDENEXT, 0);
		
		UpdateWindow(hWnd);
		UpdateScreen();*/
	}
}

// response to the WM_ACTIVATE and PWM_SHOWWINDOW in dispwndproc
static void MmsView_OnShowWnd(HWND hWnd, WPARAM wParam, LPARAM lParam)
{	
	HWND		hWndFrame = NULL;
	PMMS_VIEWCREATEDATA pCreateData;

	pCreateData = GetUserData(hWnd);

	hWndFrame = pCreateData->hWndFrame;
	//SetWindowText(hWndFrame, MMS_DISPLAY_APP);
	MmsView_SetWndText(hWndFrame, hWnd);
	SetFocus(pCreateData->hWndTextView);
	
	if (pCreateData->DispMsgInfo.nType == RECEIVE_VIEW)
		SendMessage(hWndFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)ML("Reply"));
	else if (!MMS_ObjectsReco(pCreateData->pTmms->pAttatch) 
		&& pCreateData->DispMsgInfo.nType != EDIT_VIEW)
		SendMessage(hWndFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)ML("Forward"));
	else
		SendMessage(hWndFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)(""));

	SendMessage(hWndFrame, PWM_SETBUTTONTEXT, 0, (LPARAM)ML("Back"));
	if (pCreateData->DispMsgInfo.nType == EDIT_VIEW)
		SendMessage(hWndFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)(""));
	else
		SendMessage(hWndFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)("Options"));

	MMSView_SetIndicator(hWnd);
	/*if (pCreateData->bPre)
		SendMessage(hWndFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON), 
			(LPARAM)pCreateData->hBmpLeft);
	else
		SendMessage(hWndFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON), 
			(LPARAM)NULL);

	if (pCreateData->bNext)
		SendMessage(hWndFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON), 
			(LPARAM)pCreateData->hBmpRight);
	else
		SendMessage(hWndFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON), 
			(LPARAM)NULL);
*/
	if (pCreateData->DispMsgInfo.nType != EDIT_VIEW)
		PDASetMenu(hWndFrame, pCreateData->hMenu);
}
// response to the WM_Destroy in displaywndproc
static void MmsView_OnDestroy(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	PMMS_VIEWCREATEDATA        pViewData;
	HWND                       hWndFrame;

	pViewData = GetUserData(hWnd);
	hWndFrame = pViewData->hWndFrame;
	MMS_StopPlay();
	
//	SendMessage(pViewData->hWndFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, RIGHTICON), 
//		(LPARAM)"");
//	SendMessage(pViewData->hWndFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, LEFTICON), 
//		(LPARAM)"");

	DestroyWindow(pViewData->hWndTextView);
	DestroyMenu(pViewData->hMenu);
	//if caller has define the display-end's message, we send this message
	//if not, we call OnDispEnd
	if(pViewData->DispMsgInfo.msg != 0 && pViewData->DispMsgInfo.hWnd != NULL)
		SendMessage(pViewData->DispMsgInfo.hWnd, pViewData->DispMsgInfo.msg, 0, 0);
	else
		OnDispEnd(pViewData);
	
	free(pViewData->pTmms);
	
#if LILY_DEBUG 
	checknum();
#endif
	
	SendMessage(hWndFrame,PWM_CLOSEWINDOW,(WPARAM)hWnd,0);
	UnregisterClass(MMSWindowClass[6].szClassName, NULL);
}

/*********************************************************************\
the window proc of sending mms  
**********************************************************************/
LRESULT MMSDisplayWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    LRESULT			lResult;
	PMMS_VIEWCREATEDATA       pCreateData;
    HDC             hdc;
    PAINTSTRUCT     ps;
        
    switch(wMsgCmd)
    {
    case WM_CREATE:
		lResult = MmsView_OnCreate(hWnd, (LPCREATESTRUCT)(lParam));
        break;
    
    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        EndPaint(hWnd,&ps);
        break;

    case WM_INITMENU:
		MmsView_OnInitmenu(hWnd);
        break;      
        
    case WM_PLAYDELAY:
        break;

	case PRIOMAN_MESSAGE_READDATA:
		PrioMan_ReadData(wParam, lParam);
		break;
		
	case PRIOMAN_MESSAGE_WRITEDATA:
		PrioMan_WriteData(wParam, lParam);
		break;

	case PRIOMAN_MESSAGE_PLAYOVER:
		{
			pCreateData = GetUserData(hWnd);

			MMS_StopPlay();	
		}
		break;
	
	case PRIOMAN_MESSAGE_BREAKOFF:
		{
			pCreateData = GetUserData(hWnd);

			MMS_StopPlay();	
		}
		break;

    case WM_COMMAND:
	{
		PMMS_VIEWCREATEDATA		pData;

		pData = GetUserData(hWnd);
		if(wParam == (WPARAM)pData->hMenu)
		{
			//WM_INITMENU
			MmsView_OnInitmenu(hWnd);
			break;
		}
		MmsView_OnCommand(hWnd, wParam, lParam);
        break;
	}

	case MMS_DEL_MSG:
		if (lParam == 1)
		{
			PMMS_VIEWCREATEDATA  pViewData;

			pViewData = GetUserData(hWnd);

			if (pViewData->pMsgHandle == 0)
				break;

			chdir(FLASHPATH);
			if (MMS_DeleteFile(pViewData->pMsgHandle->msgname)==0)
			{
				SendMessage(pViewData->DispMsgInfo.hWnd, PWM_MSG_MU_DELETE, 
					MAKEWPARAM(MU_ERR_FAILED, MU_MDU_MMS), 0);
				break;
			}
			else
			{
				SendMessage(pViewData->DispMsgInfo.hWnd, PWM_MSG_MU_DELETED_RESP,
					MAKEWPARAM(MU_ERR_SUCC, MU_MDU_MMS),
					(LPARAM)(DWORD)pViewData->pMsgHandle);
				
				FreeMsgHandle((DWORD)pViewData->pMsgHandle);
				pViewData->pMsgHandle = NULL;
				MsgWin(NULL, NULL, 0, ML("Deleted"), 
						STR_MMS_MMS, Notify_Success,
                        STR_MMS_OK, NULL,WAITTIMEOUT);
			}
			SendMessage(hWnd, WM_CLOSE, NULL, NULL);
		}
		break;

	case WM_TIMER:
		MmsView_OnTimer(hWnd, wParam, lParam);		
		break;		
		
	case MMS_MOVE_FOLDER:
	{
		int desFold;
		PMMS_VIEWCREATEDATA  pViewData;

		if (wParam == 0)
			break;

		pViewData = GetUserData(hWnd);
		desFold = (int)lParam;
		MMS_ModifyFolder(pViewData->pMsgHandle->msgname, desFold);
		SendMessage(pViewData->DispMsgInfo.hWnd, PWM_MSG_MU_DELETE, 
								MAKEWPARAM(MU_ERR_SUCC, MU_MDU_MMS), 
								(LPARAM)(DWORD)pViewData->pMsgHandle);
		MsgWin(NULL, NULL, 0, ML("Message moved."), 
						STR_MMS_MMS, Notify_Success,
                        STR_MMS_OK, NULL,WAITTIMEOUT);
		PostMessage(hWnd, WM_CLOSE, 0, 0);
		break;
	}
		
	case WM_KEYDOWN:
		MmsView_OnKeyDown(hWnd, wParam, lParam);
		break;
	
    case WM_ACTIVATE:
		break;

	case PWM_SHOWWINDOW:
		MmsView_OnShowWnd(hWnd, wParam, lParam);
		break;

	case WM_SETFOCUS:
		{
			PMMS_VIEWCREATEDATA  pViewData;

			pViewData = GetUserData(hWnd);
			SetFocus(pViewData->hWndTextView);
		}
		break;

    case WM_CLOSE:
        DestroyWindow(hWnd);
        break;
        
    case WM_DESTROY:
		MmsView_OnDestroy(hWnd, wParam, lParam);
        break;
        
    default:
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
    return lResult;
}


static void GetValidName(char *origionName, int imgOrAudio, int nType, 
                         char *validName)
{
    char     *pDot = NULL;
    char     suffix[6];
    int      origionLen, nameLen;
    int      len, nRet;
    char     *tmpName;

    origionLen = strlen(origionName);

    tmpName = origionName;
    tmpName += origionLen - 1;
    
    while (*tmpName)
    {
        if (*tmpName == '.')
        {
            pDot = tmpName;
            break;
        }
        
        tmpName--;
    }

    if (pDot != NULL)
        nameLen = pDot - origionName;
    else
        nameLen = origionLen;

    if (nameLen > NEWNAME_LEN)
        tmpName = origionName + (nameLen - NEWNAME_LEN);
    else
        tmpName = origionName;
        
    nRet = GetFileNameSuffix(tmpName, suffix);
    
    if (nRet)
    {
        // the source file name has suffix
        if (imgOrAudio == NEWNAME_IMAGE)
        {
            // the suffix is valid
            if(stricmp(suffix, imgfix[nType]) == 0)            
            {
                strcpy(validName, tmpName);
                return;
            }
            // if the suffix if error ,then create a new suffix
            else
            {                
                len = pDot - tmpName;
                strncpy(validName, tmpName, len);                
                strcpy(validName + len, imgfix[nType]);
                return;
            }
        }
        else if (imgOrAudio == NEWNAME_AUDIO)
        {
            if(stricmp(suffix, audiofix[nType]) == 0)            
            {
                strcpy(validName, tmpName);
                return;
            }
            else
            {
               len = pDot - tmpName;
               strncpy(validName, tmpName, len);
               strcpy(validName + len, audiofix[nType]);
               return;
            }
        }
    }
    else
    {
        // the source file name has no suffix
        strcpy(validName, tmpName);
        if (imgOrAudio == NEWNAME_IMAGE)
            strcat(validName, imgfix[nType]);
        else if (imgOrAudio == NEWNAME_AUDIO)
            strcat(validName, audiofix[nType]);
    }
}

//create random file name according to suffix
static int MakeRandomFileName(PSTR pFileNameBuf, PSTR pSuffix)
{
    /*
     *  ASCII 
     0~9 : 48~57
     A~Z : 65~90
     a~z : 97~122
     */
    int i, j;
    char ichar;
    SYSTEMTIME t;
 //   _FIND_DATA fd;
    int  hFound;

    
    GetLocalTime(&t);
    srand((unsigned)t.wMilliseconds);
    chdir(FLASHPATH);
   
    hFound = -1;
    do 
    {
        if (-1 != hFound)
            close(hFound);
        for (i = 0; i < 8; i++)
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
            pFileNameBuf[i] = ichar;
        }
        pFileNameBuf[i] = '\0';
        strcat(pFileNameBuf, pSuffix);
    } while(-1 !=(hFound = open(pFileNameBuf, O_RDONLY|O_WRONLY)));
    return 1;
}
/*********************************************************************\
* Function     PlayWav
* Purpose      play wav
* Params       wavFileName: the file name fo wav
               wavBody: the data of wav
               wavSize: the length of wave data
* Return        void
**********************************************************************/
static void PlayWav(char *wavFileName, const char *wavBody, int wavSize)
{
#if MUSIC_DEBUG
    if(PlayMusic(wavFileName, 0, 0) != 0)
        return;    
    CreateWavFile(wavBody, wavSize, wavFileName);
    PlayMusic(wavFileName, 0, 0);
#endif
}
/*********************************************************************\
* Function     PlayMmf
* Purpose      play mmf
* Params       mmfFileName: the file name of mmf
               mmfBody: the data of mmf
               mmfSize: the length of mmf data
* Return       void
**********************************************************************/
static void PlayMmf(char *mmfFileName, const char *mmfBody, int mmfSize)
{
    int     hMmf;
    char    mmfName[MAX_FILENAME];
    char    pSuffix[6];

    sprintf(mmfName, "/mnt/flash/%s", mmfFileName);
    hMmf = open(mmfName, O_RDWR|O_CREAT, S_IRWXU);
    if (hMmf == -1)
    {
        if (!GetFileNameSuffix(mmfFileName, pSuffix))
            return;
        MakeRandomFileName(mmfFileName, pSuffix);
        sprintf(mmfName, "/mnt/flash/%s", mmfFileName);    
        hMmf = open(mmfName, O_RDWR|O_CREAT, S_IRWXU);
        if (-1 == hMmf)
            return;
    }
    write(hMmf, (PVOID)mmfBody, mmfSize);
    close(hMmf);
//#ifdef SEF_BAIYUNSHAN
//    SEF_Play_Ring(mmfName, 0, 0);
//#endif
}

/*********************************************************************\
* Function    _CALLBACK StopRingCallBackFun 
* Purpose      the callback function of audio stoped
* Params       bRingIsEnd:true: the audio stop commonly
* Return       void
**********************************************************************/
void StopRingCallBackFun( BYTE bRingIsEnd)
{
}

/*********************************************************************\
* Function    _CALLBACK CannotSupportRing
* Purpose      
**********************************************************************/
static void CannotSupportRing( BYTE bRingIsEnd)
{ 
}

/**********************************************
*
*   menu template
*
***********************************************/
#define IDM_OBJ_OPEN    601
#define IDM_OBJ_SAVE    602
#define IDM_OBJ_DEL     603
#define IDM_OBJ_STOP    607

#define IDC_OBJ_LIST    604
#define IDC_OBJ_TEXT    605
#define IDM_MSG_OK      606

static const MENUITEMTEMPLATE MmsObjMenu[] = 
{
    {MF_STRING, IDM_OBJ_OPEN, "Brow", NULL},
    {MF_STRING, IDM_OBJ_SAVE, "Save", NULL},
    { MF_END, 0, NULL, NULL }
};

static const MENUTEMPLATE MMSObjMenuTemplate = 
{
    0,
    (PMENUITEMTEMPLATE)MmsObjMenu
};

static LRESULT MMSObjWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static BOOL CreateObjLst(HWND hWnd, LPCREATESTRUCT lpCreateStruct);
static BOOL OnOpenObject(HWND hWndFrame, HWND hWnd, mmsMetaNode *p);
static BOOL OnInputObjName(HWND hFrameWnd, HWND hWnd, mmsMetaNode *p);

//object
static void ObjCountScrollPos(HWND hWnd, SCROLLINFO vsi);
static void ObjCountImagePos(int width, int height, int *stretchWidth, 
                          int *stretchHeight, int *sx);
static void ObjOnPaintImage(HWND hWnd, HDC hMemdc,HDC hdc, int imgType, int sWidth, int sHeight, int StartX);
static void ObjDispImage(HWND hWnd, const PSTR pIMGData, int imgType,
               int x, int y, int width, int height);
static void ObjMoveDispWnd(HWND hWnd, int iDerection, int move);
static void ObjMoveDispWndDrag(HWND hWnd, int iDerection, int move);
static int  ObjCanMoved(HWND hWnd, int iDerection);

void StopOBJRingCallBackFun( BYTE bRingIsEnd );
//static BOOL CallVcardEdit(mmsMetaNode* pNode);
// static BOOL CallVcalenderEdit(mmsMetaNode* pNode);

/*********************************************************************\
* Function     CallMMSObjList
* Purpose      create a list for mms objects
* Params       hWnd
               mmsMetaNode
* Return       Bool
**********************************************************************/
BOOL CallMMSObjList(HWND hWndFrame, HWND hWndParent, mmsMetaNode *pObjMeta, int nCallType)
{
    HWND                hWndMMSObj;
	HMENU				hMenuObj;
    WNDCLASS            wco;
    MMS_OBJCREATEDATA   ObjData;
	RECT                rClient;
        
    wco.style         = 0;
    wco.cbClsExtra    = 0;
    wco.cbWndExtra    = sizeof(MMS_OBJCREATEDATA);;
    wco.hInstance     = NULL;
    wco.hIcon         = NULL;
    wco.hCursor       = NULL;
    wco.lpszMenuName  = NULL;
    wco.lpfnWndProc   = MMSObjWndProc;
    wco.lpszClassName = "MMSObjClassName";
    wco.hbrBackground = NULL;
    
    RegisterClass(&wco);
    
    // initialize the create data
    ObjData.bPlayMusic = FALSE;
    ObjData.oldObjName[0] = 0;
    ObjData.pObj = pObjMeta;
	ObjData.hWndFrame = hWndFrame;
    
    hMenuObj = LoadMenuIndirect((PMENUTEMPLATE)&MMSObjMenuTemplate);
    ModifyMenu(hMenuObj,IDM_OBJ_OPEN, MF_BYCOMMAND,IDM_OBJ_OPEN,
        (LPCSTR)STR_MMS_OPEN);

    if (nCallType == VIEW_CALLOBJ)
    {        
        ModifyMenu(hMenuObj,IDM_OBJ_SAVE, MF_BYCOMMAND,IDM_OBJ_SAVE,
            (LPCSTR)STR_MMS_SAVE);        
    }
    else
    {
        // Edit_callobj
        RemoveMenu(hMenuObj, IDM_OBJ_SAVE, MF_BYCOMMAND);
        AppendMenu(hMenuObj, MF_ENABLED, IDM_OBJ_DEL, ML("Remove"));
    }

    ObjData.hMenuObj = hMenuObj;

	GetClientRect(hWndFrame, &rClient);
    hWndMMSObj = CreateWindow(
        "MMSObjClassName",
        STR_MMS_OBJECT,
        WS_VISIBLE | WS_CHILD, 
        rClient.left, rClient.top, rClient.right-rClient.left, rClient.bottom-rClient.top,
        hWndFrame, 
        NULL,
        NULL, 
        (PVOID)&ObjData);

	printf("CreateWindow MMSObjClassName\r\n");
	
	SetWindowText(hWndFrame, STR_MMS_OBJECT);
	PDASetMenu(hWndFrame, hMenuObj);
    ShowWindow(hWndMMSObj, SW_SHOW);
    UpdateWindow(hWndMMSObj);       

	SetFocus(hWndMMSObj);
	SendMessage(hWndFrame, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDM_OBJ_OPEN, 1), 
		(LPARAM)STR_MMS_OPEN);
	SendMessage(hWndFrame, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDB_EXIT, 0), 
		(LPARAM)ML("Back"));
	SendMessage(hWndFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)ML("Options"));
	SendMessage(hWndFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON), 
		(LPARAM)NULL);
	SendMessage(hWndFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON), 
			(LPARAM)NULL);
    return TRUE;
}

static void MmsObj_OnInitmenu(HWND hWnd)
{
	PMMS_OBJCREATEDATA pObjData;
	HMENU hMenuObj;
	
	pObjData = GetUserData(hWnd);
	hMenuObj = pObjData->hMenuObj;

	if(pObjData->bPlayMusic)
		ModifyMenu(hMenuObj,IDM_OBJ_OPEN, MF_BYCOMMAND,IDM_OBJ_STOP,
		(LPCSTR)STR_MMS_STOP);
	else
		ModifyMenu(hMenuObj,IDM_OBJ_STOP, MF_BYCOMMAND,IDM_OBJ_OPEN,
		(LPCSTR)STR_MMS_OPEN);
}

static void MmsObj_OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	WORD            mID;
    WORD            msgBox;

	mID = LOWORD(wParam);
	msgBox = HIWORD(wParam);
	switch(mID)
	{
	case IDB_EXIT:
		PostMessage(hWnd, WM_CLOSE, 0, 0);
		break;
		
	case IDC_OBJ_LIST:
        {
            PMMS_OBJCREATEDATA pObjData;
			
            pObjData = GetUserData(hWnd);
			
            switch(HIWORD(wParam))
            {
            case LBN_DBLCLK:        //double click list
                {
                    SendMessage(hWnd, WM_COMMAND, IDM_OBJ_OPEN, 0);
                }
                break;
            case LBN_SELCHANGE:             
                if(pObjData->bPlayMusic)
                {
                    MMS_StopPlay();
                    pObjData->bPlayMusic = FALSE;

					if(pObjData->bPlayMusic)
						SendMessage(pObjData->hWndFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)STR_MMS_STOP);	
					else
						SendMessage(pObjData->hWndFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)STR_MMS_OPEN);	
                }
                break;
            default:
                break;
            }           
            break;
        }
		
	case IDM_OBJ_OPEN:
		{
			int index;
			HWND hWndObjList;
			PMMS_OBJCREATEDATA	pObjData;
			mmsMetaNode  *p;

			pObjData = GetUserData(hWnd);
			
			hWndObjList = GetDlgItem(hWnd,IDC_OBJ_LIST);
			index = SendMessage(hWndObjList, LB_GETCURSEL, 0, 0);
			if (index == -1)
				break;
			p = (mmsMetaNode *)SendMessage(hWndObjList, LB_GETITEMDATA, index, 0);
			OnOpenObject(pObjData->hWndFrame, hWnd, p);

			break;
		}
	case IDM_OBJ_STOP:
        {
            PMMS_OBJCREATEDATA pObjData;
            
            pObjData = GetUserData(hWnd);
			
            if(pObjData->bPlayMusic)
            {
                MMS_StopPlay();
				
                pObjData->bPlayMusic = FALSE;

				if(pObjData->bPlayMusic)
					SendMessage(pObjData->hWndFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)STR_MMS_STOP);	
				else
					SendMessage(pObjData->hWndFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)STR_MMS_OPEN);
            }
            break;
        }
		
	case IDM_OBJ_SAVE:
        {
            int index;
            HWND hWndObjList;
            PMMS_OBJCREATEDATA pObjData;
			mmsMetaNode  *p;
            
            pObjData = GetUserData(hWnd);
            
            if(pObjData->bPlayMusic)
            {
                MMS_StopPlay();
                pObjData->bPlayMusic = FALSE;

				SendMessage(pObjData->hWndFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)STR_MMS_OPEN);
            }       
            
            hWndObjList = GetDlgItem(hWnd,IDC_OBJ_LIST);
            index = SendMessage(hWndObjList, LB_GETCURSEL, 0, 0);
			if (index == -1)
				break;
			p = (mmsMetaNode *)SendMessage(hWndObjList, LB_GETITEMDATA, index, 0);
            OnInputObjName(pObjData->hWndFrame, hWnd, p);
            break;
        }
	case IDM_OBJ_DEL:
        {
            int index;      
            HWND hWndObjList;
			mmsMetaNode  *p;
			
            hWndObjList = GetDlgItem(hWnd,IDC_OBJ_LIST);
            index = SendMessage(hWndObjList, LB_GETCURSEL, 0, 0);
			if (index == -1)
				break;
			{
				PMMS_OBJCREATEDATA pObjData;
				
				pObjData = GetUserData(hWnd);
				
				if(pObjData->bPlayMusic)
				{
					MMS_StopPlay();
					pObjData->bPlayMusic = FALSE;
					
					SendMessage(pObjData->hWndFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)STR_MMS_OPEN);
				}
			}
			p = (mmsMetaNode *)SendMessage(hWndObjList, LB_GETITEMDATA, index, 0);
            DeleteAttatchByIndex(hWnd, p);
			
            break;
        }
	default:
		break;
	}

}

static void MmsObj_OnShowWnd(HWND hWnd)
{
	HWND hList;
	HWND hWndFrame;
	PMMS_OBJCREATEDATA pObjData;
	int  nCount = 0;

	pObjData = GetUserData(hWnd);
	hWndFrame = pObjData->hWndFrame;
	
    hList = GetDlgItem(hWnd,IDC_OBJ_LIST);

	SetWindowText(hWndFrame, STR_MMS_OBJECT);
	SendMessage(hWndFrame, PWM_SETBUTTONTEXT, 0, (LPARAM)ML("Back"));
	
	SendMessage(hWndFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON), 
			(LPARAM)NULL);
	SendMessage(hWndFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON), 
			(LPARAM)NULL);

	nCount = SendMessage(hList, LB_GETCOUNT, 0, 0);
	if (nCount == 0)
	{
		//SetFocus(hWnd);
		ShowWindow(hList, SW_HIDE);
		SetFocus(hWnd);
		SendMessage(hWndFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)"");		
		SendMessage(hWndFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
		PDASetMenu(pObjData->hWndFrame, NULL);
	}
	else
	{
		ShowWindow(hList, SW_SHOW);
		SetFocus(hList);

		if(pObjData->bPlayMusic)
			SendMessage(hWndFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)STR_MMS_STOP);	
		else
			SendMessage(hWndFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)STR_MMS_OPEN);	

		SendMessage(hWndFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)ML("Options"));
		PDASetMenu(hWndFrame, pObjData->hMenuObj);
	}
}

static void ObjListEmpty(HWND hWnd, HWND hWndObjList)
{
	PMMS_OBJCREATEDATA    pObjData;

	pObjData = GetUserData(hWnd);

	//SetFocus(hWnd);
	ShowWindow(hWndObjList, SW_HIDE);
	SetFocus(hWnd);
	PDASetMenu(pObjData->hWndFrame, NULL);
	SendMessage(pObjData->hWndFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)"");	
	SendMessage(pObjData->hWndFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
}
/*********************************************************************\
            the window proc of mms  object
**********************************************************************/
static LRESULT MMSObjWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    LRESULT         lResult;
    HDC             hdc;
	HWND            hWndObjList = NULL;

    switch(wMsgCmd)
    {
    case WM_CREATE:
        if (!CreateObjLst(hWnd, (LPCREATESTRUCT)(lParam)))
            return FALSE;
        break;

    case WM_PAINT:
        hdc = BeginPaint(hWnd, NULL);
		hWndObjList = GetDlgItem(hWnd, IDC_OBJ_LIST);
		if (0 == SendMessage(hWndObjList, LB_GETCOUNT, 0, 0))
		{
			RECT rcClient;
			int nBkMode;
			
			GetClientRect(hWnd, &rcClient);

			ClearRect(hdc, &rcClient, COLOR_TRANSBK);
			ShowWindow(hWndObjList, SW_HIDE);
			nBkMode = SetBkMode(hdc, BM_TRANSPARENT);
			DrawText(hdc,ML("No objects"),-1,&rcClient,DT_VCENTER|DT_CENTER);
			SetBkMode(hdc, nBkMode);
		}
        EndPaint(hWnd,NULL);
        break;

    case WM_COMMAND:
	{
		PMMS_OBJCREATEDATA pObjData;

		pObjData = GetUserData(hWnd);
		if(wParam == (WPARAM)pObjData->hMenuObj)
		{
			//WM_INITMENU
			MmsObj_OnInitmenu(hWnd);
			break;
		}
		MmsObj_OnCommand(hWnd, wParam, lParam);
        break;
	}

    case WM_SETFOCUS:
	{
		HWND    hWndList;

		hWndList = GetDlgItem(hWnd,IDC_OBJ_LIST);
		if (FALSE == IsWindowVisible(hWndList))
			SetFocus(hWnd);
		else
			SetFocus(hWndList);
        break;
	}
	
	case PRIOMAN_MESSAGE_READDATA:
		PrioMan_ReadData(wParam, lParam);
		break;
		
	case PRIOMAN_MESSAGE_WRITEDATA:
		PrioMan_WriteData(wParam, lParam);
		break;

	case PRIOMAN_MESSAGE_PLAYOVER:
		{
			PMMS_OBJCREATEDATA pObjData;
			
			pObjData = GetUserData(hWnd);
			MMS_StopPlay();
			pObjData->bPlayMusic = FALSE;
			SendMessage(pObjData->hWndFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)STR_MMS_OPEN);	
		}
		break;

	case PRIOMAN_MESSAGE_BREAKOFF:
		{
			PMMS_OBJCREATEDATA pObjData;
			
			pObjData = GetUserData(hWnd);
			MMS_StopPlay();
			pObjData->bPlayMusic = FALSE;
			SendMessage(pObjData->hWndFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)STR_MMS_OPEN);	
		}
		break;

	case PWM_SHOWWINDOW:
		MmsObj_OnShowWnd(hWnd);
		break;

    case WM_INITMENU:
		MmsObj_OnInitmenu(hWnd);
        break;

    case WM_KEYDOWN:
	{
		PMMS_OBJCREATEDATA	pObjData;

		pObjData = GetUserData(hWnd);

        switch (LOWORD(wParam))
        {
		case VK_RETURN:
			PostMessage(hWnd, WM_COMMAND, IDM_OBJ_OPEN, 0);
			break;

		case VK_F5:
			PDADefWindowProc(pObjData->hWndFrame, wMsgCmd, wParam, lParam);
			break;

        case VK_F10:
            PostMessage(hWnd, WM_CLOSE, 0, 0);
            break;
        
        default:
            return PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        }
        break;
	}

	case MMS_REMOVE_IMG:
	case MMS_REMOVE_AUDIO:
	case MMS_REMOVE_TEXT:
	case MMS_REMOVE_OBJ:
	{
		int index;
		HWND hWndObjList = NULL;
		HWND hWndEdit = NULL;
		char classname[32];
		
		classname[0] = 0;
		hWndObjList = GetDlgItem(hWnd,IDC_OBJ_LIST);
		index = SendMessage(hWndObjList, LB_GETCURSEL, 0, 0);
		if (index == -1)
			break;
		if (lParam == 1)
		{
			hWndEdit = hWnd;
			do 
			{
				hWndEdit = GetWindow(hWndEdit, GW_HWNDNEXT);
				GetClassName(hWndEdit, classname, 32);
				
				if(strcmp(classname, MMSWindowClass[5].szClassName) == 0)
				{
					break;
				}
				
			} while(hWndEdit);
			
			SendMessage(hWndObjList, LB_DELETESTRING, index, 0);
			index --;
			if (0 == SendMessage(hWndObjList, LB_GETCOUNT, 0, 0))
			{
				ObjListEmpty(hWnd, hWndObjList);
			}
			else
			{
				if(index < 0)
					index = 0;
				
				SendMessage(hWndObjList,LB_SETCURSEL,index,0);
			}	

			// delete
			SendMessage(hWndEdit, wMsgCmd, wParam, lParam);
		}
		break;
	}

    case WM_CLOSE:        
        DestroyWindow(hWnd);
        UnregisterClass("MMSObjClassName", NULL);
        break;

    case WM_DESTROY:
    {
        PMMS_OBJCREATEDATA pObjData;
		int                i = 0;
        
        pObjData = GetUserData(hWnd);

        if(pObjData->bPlayMusic)
        {
            MMS_StopPlay();
            pObjData->bPlayMusic = FALSE;
        }
		for (i = 0; i<3; i++)
		{
			DeleteObject(pObjData->hBmpObj[i]);
		}
		SendMessage(pObjData->hWndFrame,PWM_CLOSEWINDOW,(WPARAM)hWnd,0);
        break;
    }

    default:
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
    return lResult;
}
/*********************************************************************\
*       create a listbox for objects
**********************************************************************/
static BOOL CreateObjLst(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
{
    HWND  hWndObjList;
    mmsMetaNode *p;
    char        *pItem;
    char        *pName;
    int         nNameLen;
    int         nObjLen = 0, index;
    HDC         hdc;
    COLORREF    Color;
    BOOL        bTran;
    PMMS_OBJCREATEDATA  pObjData;
	char        pNameShow[MAX_PATH_LEN];
	HBITMAP    pBitmap = NULL;

	//printf("enter CreateObjLst \r\n");

	pNameShow[0] = 0;
    pObjData = GetUserData(hWnd);
    memcpy(pObjData,lpCreateStruct->lpCreateParams,sizeof(MMS_OBJCREATEDATA));

    hdc = GetDC(hWnd);
    pObjData->hBmpObj[1] = CreateBitmapFromImageFile(hdc, MMS_OBJIMG_PATH, &Color, &bTran);
	pObjData->hBmpObj[0] = CreateBitmapFromImageFile(hdc, MMS_OBJWAV_PATH, &Color, &bTran);
	pObjData->hBmpObj[2] = CreateBitmapFromImageFile(hdc, MMS_OBJTXT_PATH, &Color, &bTran);
    ReleaseDC(hWnd, hdc);

    hWndObjList = CreateWindow(
        "LISTBOX",
        "", 
        WS_CHILD | WS_VISIBLE | WS_VSCROLL | LBS_BITMAP | LBS_MULTILINE,
        LIST_X,
        LIST_Y,
        LIST_WIDTH,
        LIST_HEIGHT + 4,
        hWnd, 
        (HMENU)IDC_OBJ_LIST,
        NULL, 
        NULL ); 

    if (NULL == hWndObjList)
        return FALSE;

	//printf("hWndObjList = 0x%x\r\n", hWndObjList);
    p = pObjData->pObj;
    while (p) 
    {
//		if (p->MetaType/10 == META_TEXT && p->MetaType != META_TEXT_VCALE
//			&& p->MetaType != META_TEXT_VCARD)
//		{
//			p = p->pnext;
//			continue;
// 		}

		if(p->MetaType/10 == META_TEXT && p->Metalen == 0)
		{
			p = p->pnext;
			continue;
		}

        if (p->Content_Location)
            pName = p->Content_Location;
        else if (p->Content_Id)
            pName = p->Content_Id;
        else
        {
            if (p->MetaType/10 == META_IMG) // image
            {
                pName = imgfix[p->MetaType%10];
				pBitmap = pObjData->hBmpObj[1];
            }
            else if (p->MetaType/10 == META_AUDIO)//audio
            {
                pName = audiofix[p->MetaType%10];
				pBitmap = pObjData->hBmpObj[0];
            }
            else if(p->MetaType/10 == META_TEXT)
            {
                pName = txtfix[p->MetaType%10];
				pBitmap = pObjData->hBmpObj[2];
            }            
        }

		if (p->MetaType/10 == META_IMG) // image
			pBitmap = pObjData->hBmpObj[1];
		else if (p->MetaType/10 == META_AUDIO)//audio
			pBitmap = pObjData->hBmpObj[0];
		else if(p->MetaType/10 == META_TEXT)
			pBitmap = pObjData->hBmpObj[2];

        nNameLen = strlen(pName);
        pItem = MMS_malloc(8);

        if ((nObjLen = p->Metalen/1024) == 0)
        {
            nObjLen = p->Metalen*10/1024;
            sprintf(pItem, "0.%d kB", nObjLen);
        }
        else
            sprintf(pItem, "%d kB", nObjLen);

		nNameLen = UTF8ToMultiByte(CP_ACP, 0, pName, -1, pNameShow, MAX_PATH_LEN,NULL, NULL);
		pNameShow[nNameLen] = 0;

        index = SendMessage(hWndObjList, LB_ADDSTRING, 0, (LPARAM)pNameShow);
		
        SendMessage(hWndObjList, LB_SETIMAGE, (WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)index),
            (LPARAM)pBitmap);
        SendMessage(hWndObjList, LB_SETAUXTEXT, MAKEWPARAM(index, -1), (LPARAM)pItem);
		SendMessage(hWndObjList, LB_SETITEMDATA, index, (LPARAM)p);
        MMS_free(pItem);

        p = p->pnext;

		//printf("Add a object, p = %d name = %s", p, pNameShow);
    }

    SendMessage(hWndObjList, LB_SETCURSEL, 0, 0);

	//printf("leave CreateObjLst\r\n");

    return TRUE;
}
/*********************************************************************\
* Function     onInputObjName
* Purpose      save a object
* Params       hWnd
               index
* Return       Bool
**********************************************************************/
static BOOL OnInputObjName(HWND hFrameWnd, HWND hWnd, mmsMetaNode *p)
{
	char    szTemp[MAX_PATH_LEN];
//	char    name[MAX_PATH_LEN];
	    
    if (p->Content_Location)
        strcpy(szTemp, p->Content_Location);
    else if (p->Content_Id)
        strcpy(szTemp, p->Content_Id);

//	if (p->MetaType/10 == META_AUDIO)
//		sprintf(name, "%s%s", FLASH_AUDIOPATH, pObjData->oldObjName);
//	else if (p->MetaType/10 == META_IMG)
//		sprintf(name, "%s%s", FLASH_IMGPATH, pObjData->oldObjName);
//	if (p->MetaType/10 == META_TEXT)
//		sprintf(name, "%s%s", FLASH_TEXTPATH, pObjData->oldObjName);

	if (p->MetaType == META_TEXT_VCARD )
	{
		if(APP_SaveVcard(p->Metadata, p->Metalen) == TRUE)
		{
			MsgWin(NULL,NULL,0,ML("Save to contacts"), STR_MMS_MMS ,
				Notify_Success, STR_MMS_OK, NULL, WAITTIMEOUT);
			return TRUE;
		}
		return FALSE;
	}
	else if (p->MetaType == META_TEXT_VCALE)
	{
		vCal_Obj* pObj;
		unsigned char* pTempData;

		pTempData = malloc(p->Metalen);
		if(pTempData == NULL)
			return FALSE;
		memcpy(pTempData, p->Metadata, p->Metalen);

		vCal_Reader(pTempData, p->Metalen, &pObj);
		
		if(CALE_SaveFromSms(pObj) == TRUE)
		{
			vCal_Clear(VCAL_OPT_READER, pObj);
			free(pTempData);
			MsgWin(NULL,NULL,0,ML("Save to calendar"), STR_MMS_MMS ,
				Notify_Success, STR_MMS_OK, NULL, WAITTIMEOUT);
			return TRUE;
		}

		vCal_Clear(VCAL_OPT_READER, pObj);
		free(pTempData);
		return FALSE;
	}
	else
		//return MMS_SaveObject(pObjData->hWndFrame, NULL, 0, NULL, 
		//	name, p->Metadata, p->Metalen);
		return MMS_SelectDestination(hFrameWnd, NULL, 0, NULL, 
			szTemp, p->Metadata, p->Metalen, p->MetaType/10);
}

BOOL MMS_SaveObject(HWND hWndFrame, HWND hWnd, int msg, PCSTR title, 
					PCSTR pName, PCSTR pdata, int datalen)
{
	int     h;
	int     ret;
	char	tipmsg[64], tipmsgmul[128];
//	char	pUTFname[MAX_PATH_LEN];
	char	pPathName[MAX_PATH_LEN];
    char	pFileName[MAX_FILENAME];

	tipmsg[0] = 0;
	pPathName[0] = 0;
	pFileName[0] = 0;
	tipmsgmul[0] = 0;

	MMS_ParseFileName(pName, pFileName, pPathName);

	if (!IsLegalFileName(pFileName))
	{
		MsgWin(NULL, NULL, 0, ML("Invalid file name. Please define another name"),
			NULL,
			Notify_Alert, STR_MMS_OK, NULL, WAITTIMEOUT);

		MMS_InputFilename(hWndFrame, hWnd, msg, pName, title, pdata, datalen);
		
		return FALSE;
	}

	h = MMS_CreateFile(pName, O_CREAT|O_RDWR|O_EXCL);
	if (h == -1)
	{
		MMS_InputFilename(hWndFrame, hWnd, msg, pName, title, pdata, datalen);
		return FALSE;
	}
	ret = write(h, pdata, datalen);
	MMS_CloseFile(h);

	if (ret == datalen)
	{		
		//MMS_ParseFileName(pName, pFileName, pPathName);

		//sprintf(tipmsg, "%s: Saved", pFileName);
		//UTF8ToMultiByte(CP_ACP, 0, tipmsg, -1, tipmsgmul, 128,NULL, NULL);
		
		MsgWin(NULL, NULL, 0, STR_SAVED/*ML(tipmsgmul)*/, NULL,
			Notify_Success, STR_MMS_OK, NULL, WAITTIMEOUT);
	}
	else
		MsgWin(NULL, NULL, 0, ML("Saveing failed"), NULL,
			Notify_Failure, STR_MMS_OK, NULL, WAITTIMEOUT);
	PostMessage(hWnd, msg, 0, 0);
    return TRUE;
}
//////////////////////////////////////////////////////////////////////////
static LRESULT MMSOpenObjWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
/*********************************************************************\
* Function     onOpenObject
* Purpose      open a object
* Params       hWnd
               index
* Return       Bool
**********************************************************************/

static BOOL OnOpenObject(HWND hWndFrame, HWND hWnd, mmsMetaNode *p)
{
    int                     i = 0;
    WNDCLASS                wcopen;
    HWND                    hWndOpenObj;
	RECT                    rClient;
    MMS_OBJOPENCREATEDATA   ObjOpenData;
	PMMS_OBJCREATEDATA      pObjData;

	pObjData = GetUserData(hWnd);
    
    if (p->MetaType/10 == META_AUDIO)//audio
    {
		if(!pObjData->bPlayMusic)
		{
			pObjData->bPlayMusic = TRUE;
			
			MMS_PlayAudio(hWnd, p->Content_Location, p->MetaType%10, 
				p->Metadata, p->Metalen, StopOBJRingCallBackFun);
			
			SendMessage(pObjData->hWndFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)STR_MMS_STOP);
			
		}
		else
		{
			pObjData->bPlayMusic = FALSE;
			MMS_StopPlay();
			SendMessage(pObjData->hWndFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)STR_MMS_OPEN);
		}
        
        return TRUE;
    }   
    else if (p->MetaType == META_OTHER)
    {
        return FALSE;
    }

    ObjOpenData.pNode = p;
	ObjOpenData.hWndFrame = hWndFrame;
        
    wcopen.style         = 0;
    wcopen.cbClsExtra    = 0;
    wcopen.cbWndExtra    = sizeof(MMS_OBJOPENCREATEDATA);
    wcopen.hInstance     = NULL;
    wcopen.hIcon         = NULL;
    wcopen.hCursor       = NULL;
    wcopen.lpszMenuName  = NULL;
    wcopen.lpfnWndProc   = MMSOpenObjWndProc;
    wcopen.lpszClassName = "MMSOpenObjClassName";
    wcopen.hbrBackground = NULL;  

    RegisterClass(&wcopen);

    GetClientRect(hWndFrame, &rClient);

    hWndOpenObj = CreateWindow(
        "MMSOpenObjClassName",
        ML("Objects"),
        WS_VISIBLE | WS_CHILD, 
        rClient.left, rClient.top, rClient.right-rClient.left, rClient.bottom-rClient.top,
        hWndFrame,
        NULL,
        NULL, 
        &(ObjOpenData));

    ShowWindow(hWndOpenObj, SW_SHOW);
    UpdateWindow(hWndOpenObj);  

	SetFocus(hWndOpenObj);
	// left
	SendMessage(hWndFrame, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDB_EXIT, 0),
		(LPARAM)ML("Back"));
	SendMessage(hWndFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
	SendMessage(hWndFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
	SendMessage(hWndFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON), 
		(LPARAM)NULL);
	SendMessage(hWndFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON), 
			(LPARAM)NULL);

    return TRUE;
}

/*********************************************************************\
            the window proc of open mms object
**********************************************************************/
static LRESULT MMSOpenObjWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    LRESULT         lResult;
    HDC             hdc;
    PAINTSTRUCT     psObj;
    WORD            mID;
    PCREATESTRUCT   pCreateStruct;
    
    switch(wMsgCmd)
    {
    case WM_CREATE:
    {
        PMMS_OBJOPENCREATEDATA  pObjOpenData;

        pCreateStruct = (PCREATESTRUCT)lParam;
        pObjOpenData = GetUserData(hWnd);

        memcpy(pObjOpenData,pCreateStruct->lpCreateParams,
            sizeof(MMS_OBJOPENCREATEDATA));
        
		if(pObjOpenData->pNode->MetaType == META_TEXT_VCALE)
		{
			pObjOpenData->hWndOpen = MMS_ViewVcardOrVcal(pObjOpenData->hWndFrame, hWnd, pObjOpenData->pNode->Metadata, 
				pObjOpenData->pNode->Metalen, META_TEXT_VCALE);
		}
		else if(pObjOpenData->pNode->MetaType == META_TEXT_VCARD)
		{
			pObjOpenData->hWndOpen = MMS_ViewVcardOrVcal(pObjOpenData->hWndFrame, hWnd, pObjOpenData->pNode->Metadata, 
				pObjOpenData->pNode->Metalen, META_TEXT_VCARD);
		}
        else if (pObjOpenData->pNode->MetaType/10 == META_TEXT)//text
        {            
			pObjOpenData->hWndOpen = PlxTextView(pObjOpenData->hWndFrame, hWnd, 
				pObjOpenData->pNode->Metadata, 
				pObjOpenData->pNode->Metalen, 
				FALSE, NULL, NULL, 0);
        }
		else if (pObjOpenData->pNode->MetaType/10 == META_IMG)
		{
			pObjOpenData->hWndOpen = PlxTextView(pObjOpenData->hWndFrame, hWnd, 
				NULL, 0, TRUE, NULL,
				pObjOpenData->pNode->Metadata, 
				pObjOpenData->pNode->Metalen);
		}
        break;
    }

    case WM_COMMAND:
        mID = LOWORD(wParam);

        switch(mID)
        {
        case IDB_EXIT:
            PostMessage(hWnd, WM_CLOSE, 0, 0);
            break;
            
        default:
            lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
            break;
        }        
        break;

    case WM_PAINT:        
        hdc = BeginPaint(hWnd,&psObj);
        
        EndPaint(hWnd,&psObj);
        break;

	case WM_SETFOCUS:
	{
		PMMS_OBJOPENCREATEDATA  pData;

		pData = GetUserData(hWnd);
		SetFocus(pData->hWndOpen);
		break;
	}

    case WM_KEYDOWN:
        switch(LOWORD(wParam))
        {
        case VK_F10:
            PostMessage(hWnd, WM_CLOSE, 0, 0);
            break;
            
        default:
            return PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        }
        break;

    case WM_ACTIVATE:
		break;

	case PWM_SHOWWINDOW:
    {
        PMMS_OBJOPENCREATEDATA  pObjOpenData;
		HWND                    hWndFrame;
        
        pObjOpenData = GetUserData(hWnd);
		hWndFrame = pObjOpenData->hWndFrame;		
        
		SendMessage(hWndFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
		SendMessage(hWndFrame, PWM_SETBUTTONTEXT, 0, (LPARAM)ML("Back"));
		SendMessage(hWndFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
		SendMessage(hWndFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON), 
			(LPARAM)NULL);
		SendMessage(hWndFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON), 
			(LPARAM)NULL);

        break;
    }     
    case WM_CLOSE:
        DestroyWindow(hWnd);
        break;

    case WM_DESTROY:
	{
		PMMS_OBJOPENCREATEDATA  pObjOpenData;

		pObjOpenData = GetUserData(hWnd);

		SendMessage(pObjOpenData->hWndFrame,PWM_CLOSEWINDOW,(WPARAM)hWnd,0);
        UnregisterClass("MMSOpenObjClassName", NULL);       
        break;
	}

    default:
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
    return lResult;
}

/*********************************************************************\
* Function    _CALLBACK StopOBJRingCallBackFun 
* Purpose      
* Params       bRingIsEnd:true: the audio stoped commonly
* Return       void
**********************************************************************/

void StopOBJRingCallBackFun( BYTE bRingIsEnd )
{
//    bPlayMusic = FALSE;
    // can move to initmenu ??
//    ModifyMenu(hMenuObj,IDM_OBJ_STOP, MF_BYCOMMAND,IDM_OBJ_OPEN,
//            (LPCSTR)STR_MMS_OPEN);
}


//////////////////////////////////////////////////////////////////////////
//Creat a new window for view image
// from view ui
#define  IDM_SAVE_IMG  780

static LRESULT MmsViewImgWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);

BOOL MMS_ViewImage(HWND hWndFrame, HWND hWndCall, mmsMetaNode *pImg)
{   
    HWND            hwndViewImg = NULL;
	RECT			rClient;
	MMS_VIEWIMGCREATEDATA  Imgdata;
	WNDCLASS		wc;
	int len;
	
    wc.style         = 0;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = sizeof(MMS_VIEWIMGCREATEDATA);
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.lpszMenuName  = NULL;       
    wc.lpfnWndProc   = MmsViewImgWndProc;
    wc.lpszClassName = "MMSViewImage";
    wc.hbrBackground = NULL;
	    
    RegisterClass(&wc);

	Imgdata.hMenuImg = CreateMenu();
	AppendMenu(Imgdata.hMenuImg, MF_ENABLED, IDM_SAVE_IMG, ML("Save"));
	Imgdata.hWndCall = hWndCall;
	Imgdata.pImg = pImg;
	Imgdata.hGif = NULL;
	Imgdata.hWndFrame = hWndFrame;
	GetClientRect(hWndFrame, &rClient);
	Imgdata.imgname[0] = 0;
	MMS_GetNameWithoutSuffix(pImg->Content_Location, Imgdata.imgname);
	len = UTF8ToMultiByte(CP_ACP, 0, Imgdata.imgname, -1, Imgdata.imgname, strlen(Imgdata.imgname),NULL, NULL);
	Imgdata.imgname[len] = 0;

    hwndViewImg = CreateWindow(
        "MMSViewImage",
        Imgdata.imgname,
        WS_VISIBLE | WS_CHILD, 
        rClient.left, rClient.top, rClient.right-rClient.left, rClient.bottom-rClient.top,
        hWndFrame, 
        NULL,
        NULL, 
        (PVOID)&Imgdata);

	SetWindowText(hWndFrame, Imgdata.imgname);
	PDASetMenu(hWndFrame, Imgdata.hMenuImg);
	SetFocus(hwndViewImg);
    ShowWindow(hwndViewImg, SW_SHOW);
    UpdateWindow(hwndViewImg);
    
    return TRUE;
}

static BOOL MMSViewImg_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
{
    HWND    hWndFrame = NULL;
	PMMS_VIEWIMGCREATEDATA  pCreateData;
    
    pCreateData = GetUserData(hWnd);
    
    memcpy(pCreateData,lpCreateStruct->lpCreateParams,sizeof(MMS_VIEWIMGCREATEDATA));
	
	hWndFrame = pCreateData->hWndFrame;
	if (hWndFrame == NULL)
		return FALSE;

    // right button
    SendMessage(hWndFrame, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(770, 0), 
        (LPARAM)ML("Back"));
    // left button
    SendMessage(hWndFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)("Save"));

    SendMessage(hWndFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)ML("Options"));    

	SendMessage(hWndFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON), 
			(LPARAM)NULL);
	SendMessage(hWndFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON), 
			(LPARAM)NULL);

	if (pCreateData->pImg->MetaType%10 == IMG_GIF)
		PostMessage(hWnd, WM_BEGINDRAWIMG, 0, 0);
    return TRUE;
}

static void MMSViewImg_OnPaint(HWND hWnd)
{
	PMMS_VIEWIMGCREATEDATA pCreateData;
    HDC      hdc;
    COLORREF OldClr;
    int      sWidth,sHeight;
    RECT     rect;
	SIZE     sz;
    
    pCreateData = GetUserData(hWnd);
  
    hdc = BeginPaint(hWnd, NULL);
    GetClientRect(hWnd, &rect);
    ClearRect(hdc, &rect, COLOR_TRANSBK);
    
    OldClr = SetBkColor(hdc, COLOR_TRANSBK);
    
	if (pCreateData->pImg == NULL)
		return;

	GetImageDimensionFromData(pCreateData->pImg->Metadata,
                              pCreateData->pImg->Metalen, 
							  &sz);

	sWidth = sz.cx < SCREEN_WIDTH ? sz.cx : SCREEN_WIDTH;
	
	sHeight = sz.cy *	sWidth/sz.cx;
		
	sHeight = sHeight < SCREEN_HEIGHT ? sHeight : SCREEN_HEIGHT;
	
	if (sHeight == SCREEN_HEIGHT)
		sWidth = sWidth * sHeight/SCREEN_HEIGHT;
	
	if (pCreateData->pImg->MetaType == META_IMG_GIF)
		PaintAnimatedGIF(hdc, pCreateData->hGif);
	else
		DrawImageFromDataEx(hdc, 
			pCreateData->pImg->Metadata, 
			(SCREEN_WIDTH - sWidth)/2, (SCREEN_HEIGHT - sHeight)/2,
			sWidth, sHeight,  
			pCreateData->pImg->Metalen, SRCCOPY);

    SetBkColor(hdc, OldClr);
    EndPaint(hWnd,NULL);
}

// save image,if have the same name??
static BOOL MMSViewImg_OnSave(PMMS_VIEWIMGCREATEDATA  pCreateData)
{
	int hFile;
	
	chdir(FLASH_IMGPATH);
	hFile = MMS_CreateFile(pCreateData->pImg->Content_Location, O_RDWR|O_CREAT);
	if (hFile == -1)
	{
		return FALSE;
	}
	write(hFile, pCreateData->pImg->Metadata, pCreateData->pImg->Metalen);
	MMS_CloseFile(hFile);
	PLXTipsWin(NULL, NULL, 0, ML("Image saved"),STR_MMS_MMS,Notify_Success, 
				STR_MMS_OK, NULL, MMS_WAITTIME );
	return TRUE;
}

static void MMSViewImg_OnCommand(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
	PMMS_VIEWIMGCREATEDATA pCreateData;
    WORD mID;
    UINT codeNotify = HIWORD(wParam);
    
    pCreateData = GetUserData(hWnd);

    mID = LOWORD(wParam);

	if (mID == IDM_SAVE_IMG)
	{
		OnInputObjName(pCreateData->hWndFrame, hWnd, pCreateData->pImg);
	}
}

static void MMSViewImg_OnKeydown(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    PMMS_VIEWIMGCREATEDATA pCreateData;
    
    pCreateData = GetUserData(hWnd);

    switch (LOWORD(wParam))
    {
		// left
	case VK_RETURN:	// save img
		OnInputObjName(pCreateData->hWndFrame, hWnd, pCreateData->pImg);		
		break;

		// right
    case VK_F10:
        PostMessage(hWnd,WM_CLOSE,NULL,NULL);
        break;

	case VK_F5:
		PDADefWindowProc(pCreateData->hWndFrame, wMsgCmd, wParam, lParam);
		break;

    default:
        PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
    }
}

static void MMSViewImg_OnShowWnd(HWND hWnd)
{
	HWND	hWndFrame = NULL;
	PMMS_VIEWIMGCREATEDATA  pCreateData;

	pCreateData = GetUserData(hWnd);

	SetFocus(hWnd);
	hWndFrame = pCreateData->hWndFrame;
	SetWindowText(hWndFrame, pCreateData->imgname);
	
	// right button
	SendMessage(hWndFrame, PWM_SETBUTTONTEXT, 0, (LPARAM)ML("Back"));

    // left button
    SendMessage(hWndFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)ML("Save"));
	
    SendMessage(hWndFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)ML("Options"));

	PDASetMenu(hWndFrame, pCreateData->hMenuImg);

	SendMessage(hWndFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON), 
			(LPARAM)NULL);
	SendMessage(hWndFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON), 
			(LPARAM)NULL);
}

static void MMSViewImg_OnDestory(HWND hWnd)
{
	PMMS_VIEWIMGCREATEDATA  pCreateData;

	pCreateData = GetUserData(hWnd);
	EndAnimatedGIF(pCreateData->hGif);
	DestroyMenu(pCreateData->hMenuImg);

	SendMessage(pCreateData->hWndFrame,PWM_CLOSEWINDOW,(WPARAM)hWnd,0);

    UnregisterClass("MMSViewImage", NULL);
}

void MMS_PaintBkFunc(HGIFANIMATE hGIFAnimate, BOOL End, int x, int y, HDC hdcMem)
{
    int                width, height;
    RECT            rect;

    width = GetDeviceCaps(hdcMem, HORZRES);
    height = GetDeviceCaps(hdcMem, VERTRES);

    rect.bottom = height;
    rect.left = 0;
    rect.top = 0;
    rect.right = width;

    ClearRect(hdcMem, &rect, COLOR_TRANSBK);
}

static void MMSViewImg_OnGif(HWND hWnd)
{   
    int sWidth, sHeight;
    HDC hdc;
	SIZE sz;
	PMMS_VIEWIMGCREATEDATA pCreateData;

    pCreateData= GetUserData(hWnd);
    
    if (NULL == pCreateData->pImg)
        return;
    
	GetImageDimensionFromData(pCreateData->pImg->Metadata,
                              pCreateData->pImg->Metalen, 
							  &sz);

	sWidth = sz.cx < SCREEN_WIDTH ? sz.cx : SCREEN_WIDTH;
	
	sHeight = sz.cy *	sWidth/sz.cx;
		
	sHeight = sHeight < SCREEN_HEIGHT ? sHeight : SCREEN_HEIGHT;
	
	if (sHeight == SCREEN_HEIGHT)
		sWidth = sWidth * sHeight/SCREEN_HEIGHT;

    switch(pCreateData->pImg->MetaType) 
    {
    case META_IMG_GIF:
        pCreateData->hGif = StartAnimatedGIFFromDataEx(hWnd, 
			pCreateData->pImg->Metadata, pCreateData->pImg->Metalen,
			(SCREEN_WIDTH - sWidth)/2, (SCREEN_HEIGHT - sHeight)/2, 
			sWidth, sHeight, DM_NONE);
        SetPaintBkFunc(pCreateData->hGif, MMS_PaintBkFunc);
        hdc = GetDC(hWnd);
        PaintAnimatedGIF (hdc, pCreateData->hGif);
        ReleaseDC(hWnd, hdc);
        break;
    case META_IMG_JPG:
    case META_IMG_BMP:
    case META_IMG_WBMP:
        InvalidateRect(hWnd, NULL, TRUE);
        break;
    default:
        break;
    }
    return;
}

/******************************************************************** 
            the window proc of edit tmms.mms       
**********************************************************************/
static LRESULT MmsViewImgWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    LRESULT         lResult;

    switch(wMsgCmd)
    {
    case WM_CREATE: 
        lResult = MMSViewImg_OnCreate(hWnd,(LPCREATESTRUCT)(lParam));
        break;
        
    case WM_PAINT:
        MMSViewImg_OnPaint(hWnd);
        break;

	case WM_BEGINDRAWIMG:
		MMSViewImg_OnGif(hWnd);
		break;

    case WM_INITMENU:
        break;
        
    case WM_COMMAND:
		if(wParam == NULL)
		{
			break;
		}

        MMSViewImg_OnCommand(hWnd, wMsgCmd, wParam , lParam);
        break;
        
    case WM_KEYDOWN:
        MMSViewImg_OnKeydown(hWnd, wMsgCmd, wParam, lParam);
        break;
    case WM_ACTIVATE:
		break;

	case PWM_SHOWWINDOW:
        MMSViewImg_OnShowWnd(hWnd);
        break;

    case WM_CLOSE:
        DestroyWindow(hWnd);
        break;
        
    case WM_DESTROY:
        MMSViewImg_OnDestory(hWnd);
        break;

    default:
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
    return lResult;
}

//////////////////////////////////////////////////////////////////////////
// the proc of input file name
static LRESULT MmsInputNameWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);

BOOL MMS_InputFilename(HWND hWndFrame, HWND hWndCall, int msg, PCSTR oldfilename, 
					   PCSTR title, PCSTR data, int datalen)
{   
    HWND            hwndInput = NULL;
	RECT			rClient;
	MMS_INPUTCREATEDATA   CreateData;
	WNDCLASS		wc;
	HWND hEdit;

	
    wc.style         = 0;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = sizeof(MMS_INPUTCREATEDATA);
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.lpszMenuName  = NULL;       
    wc.lpfnWndProc   = MmsInputNameWndProc;
    wc.lpszClassName = "MMSInputName";
    wc.hbrBackground = NULL;
	    
    RegisterClass(&wc);

	CreateData.hWndCall = hWndCall;
	CreateData.msg = msg;
	CreateData.hWndFrame = hWndFrame;	
	strcpy(CreateData.oldName, oldfilename);
	CreateData.nameShow[0] = 0;
	CreateData.title[0] = 0;

	if(title == NULL)
	{
		GetWindowText(hWndFrame, CreateData.title, 20);
		CreateData.title[20] = 0;
	}
	else
	{
		if (strlen(title) >= 31)
		{
			strncpy(CreateData.title, title, 31);
			CreateData.title[31] = 0;
		}
		else
			strcpy(CreateData.title, title);
	}
	

	CreateData.data = data;
	CreateData.datalen = datalen;

	GetClientRect(hWndFrame, &rClient);

    hwndInput = CreateWindow(
        "MMSInputName",
        "",
        WS_VISIBLE | WS_CHILD, 
        rClient.left, rClient.top, rClient.right-rClient.left, rClient.bottom-rClient.top,
        hWndFrame, 
        NULL,
        NULL, 
        (PVOID)&CreateData);

//	SetWindowText(hWndFrame, CreateData.nameShow);

	hEdit = GetDlgItem(hwndInput, IDC_EDIT_NAME);
	SetFocus(hEdit);
	SendMessage(hEdit, EM_SETSEL, -1, -1);

    ShowWindow(hwndInput, SW_SHOW);
    UpdateWindow(hwndInput);
    
    return TRUE;
}

static BOOL MMSInput_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
{
    PMMS_INPUTCREATEDATA  pCreateData;
	IMEEDIT  ie_name;
	HWND     hWndEditName;
	RECT     rect;
	char     name[MAX_FILENAME];
	char	pPathName[MAX_PATH_LEN];
    char	pFileName[MAX_FILENAME];

	pPathName[0] = 0;
	pFileName[0] = 0;
	name[0] = 0;
    
    pCreateData = GetUserData(hWnd);
    
    memcpy(pCreateData,lpCreateStruct->lpCreateParams,sizeof(MMS_INPUTCREATEDATA));
	
	if (pCreateData->hWndFrame == NULL)
		return FALSE;

    // right button
//    SendMessage(pCreateData->hWndFrame, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(770, 0), 
//        (LPARAM)ML("Back"));
    // left button
    SendMessage(pCreateData->hWndFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)("Save"));

	SendMessage(pCreateData->hWndFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON), 
			(LPARAM)NULL);
	SendMessage(pCreateData->hWndFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON), 
			(LPARAM)NULL);

	SendMessage(pCreateData->hWndFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)ML(""));
	GetClientRect(hWnd, &rect);

	MMS_ParseFileName(pCreateData->oldName, pFileName, pPathName);
	MMS_GetNameWithoutSuffix(pFileName, name);

//	UTF8ToMultiByte(CP_ACP, 0, name, -1, name, strlen(name) + 1,NULL, NULL);
	memset(&ie_name, 0, sizeof(IMEEDIT));            
    ie_name.hwndNotify   = hWnd;
    ie_name.dwAttrib    = 0;            
    ie_name.pszImeName = NULL;
    ie_name.pszTitle = NULL;
    ie_name.uMsgSetText = NULL;

	rect.top = 40;
	rect.bottom = 80;

	strcpy(pCreateData->nameShow, name);

    hWndEditName = CreateWindow(
        "IMEEDIT",
        "",
        WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL,
        rect.left, rect.top, (rect.right - rect.left), (rect.bottom - rect.top),
        hWnd,
        (HMENU)IDC_EDIT_NAME,
        NULL,
        (PVOID)&ie_name);
    if (NULL == hWndEditName)
        return FALSE;

    SendMessage(hWndEditName, EM_LIMITTEXT, NEWNAME_LEN, NULL);
	SetWindowText(hWndEditName, name);

	MsgWin(NULL, NULL, 0, ML("Please define another name"), pCreateData->title,
				Notify_Alert, STR_MMS_OK, NULL, WAITTIMEOUT);
    return TRUE;
}

static void MMSInput_OnKeydown(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    PMMS_INPUTCREATEDATA pCreateData;
	HWND                 hWndName;
	char                 name[64];
	char				 utf8Name[64];
	char                 fullname[MAX_PATH_LEN];
	char                 suffix[32];
	char	pPathName[MAX_PATH_LEN];
    char	pFileName[MAX_FILENAME];
//	int len;
    
    pCreateData = GetUserData(hWnd);
	hWndName = GetDlgItem(hWnd, IDC_EDIT_NAME);

	name[0] = 0;
	fullname[0] = 0;
	suffix[0] = 0;
	pPathName[0] = 0;
	pFileName[0] = 0;

    switch (LOWORD(wParam))
    {
		// left
	case VK_RETURN:
		{
			GetWindowText(hWndName, name, GetWindowTextLength(hWndName)+1);

			if (name[0] == 0)
			{
				MsgWin(NULL, NULL, 0, ML("Please define name"),
					pCreateData->title,
					Notify_Alert, STR_MMS_OK, NULL, WAITTIMEOUT);
				break;
			}
			if (!IsLegalFileName(name))
			{
				MsgWin(NULL, NULL, 0, ML("Invalid file name. Please define another name"),
					pCreateData->title,
					Notify_Alert, STR_MMS_OK, NULL, WAITTIMEOUT);
				break;
			}

//			len = MultiByteToUTF8(CP_ACP, 0, name, -1, utf8Name, MAX_PATH_LEN);
//			utf8Name[len] = 0;
			strcpy(utf8Name, name);

			MMS_ParseFileName(pCreateData->oldName, pFileName, pPathName);
			MMS_GetFileSuffix(pCreateData->oldName, suffix);
			sprintf(fullname, "%s%s%s", pPathName, utf8Name, suffix);
			
			MMS_SaveObject(pCreateData->hWndFrame, pCreateData->hWndCall,
				pCreateData->msg, pCreateData->title, 
				fullname, pCreateData->data, pCreateData->datalen);
			SendMessage(hWnd, WM_CLOSE, NULL, NULL);
		}
		break;

		// right
    case VK_F10:
		PostMessage(pCreateData->hWndCall, pCreateData->msg, 0, 0);
        PostMessage(hWnd,WM_CLOSE,NULL,NULL);
        break;

    default:
        PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
    }
}

static void MMSInput_OnShowWnd(HWND hWnd)
{
	HWND	hWndFrame = NULL;
	PMMS_INPUTCREATEDATA  pCreateData;

	pCreateData = GetUserData(hWnd);

	hWndFrame = pCreateData->hWndFrame;
	SetFocus(hWnd);
	SetWindowText(hWndFrame, pCreateData->nameShow);	
	// right button
	//SendMessage(hWndFrame, PWM_SETBUTTONTEXT, 0, (LPARAM)ML("Back"));
    // left button
    SendMessage(hWndFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)ML("Save"));
	SendMessage(hWndFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)ML(""));
}

static void MMSInput_OnDestory(HWND hWnd)
{
	PMMS_INPUTCREATEDATA  pCreateData;

	pCreateData = GetUserData(hWnd);

	SendMessage(pCreateData->hWndFrame,PWM_CLOSEWINDOW,(WPARAM)hWnd,0);

    UnregisterClass("MMSInputName", NULL);
}

static LRESULT MmsInputNameWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    LRESULT         lResult;
	HDC             hdc;
	RECT			rcClient;
	int				nBkMode;

    switch(wMsgCmd)
    {
    case WM_CREATE: 
        lResult = MMSInput_OnCreate(hWnd,(LPCREATESTRUCT)(lParam));
        break;
 
	case WM_PAINT:
		hdc = BeginPaint(hWnd, NULL);		
		GetClientRect(hWnd, &rcClient);
		rcClient.bottom = 30;
		ClearRect(hdc, &rcClient, COLOR_TRANSBK);
		nBkMode = SetBkMode(hdc, BM_TRANSPARENT);
		DrawText(hdc,ML("New name:"),-1,&rcClient,DT_LEFT);
		SetBkMode(hdc, nBkMode);
        EndPaint(hWnd,NULL);		
		break;

    case WM_KEYDOWN:
        MMSInput_OnKeydown(hWnd, wMsgCmd, wParam, lParam);
        break;

    case WM_ACTIVATE:
		break;

	case PWM_SHOWWINDOW:
        MMSInput_OnShowWnd(hWnd);
        break;

	case WM_SETFOCUS:
		SetFocus(GetDlgItem(hWnd, IDC_EDIT_NAME));
		break;

    case WM_CLOSE:
        DestroyWindow(hWnd);
        break;
        
    case WM_DESTROY:
        MMSInput_OnDestory(hWnd);
        break;

    default:
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
    return lResult;
}

static HWND MMS_ViewVcardOrVcal(HWND hFrameWnd, HWND hWnd, void* pData, int len, int nType)
{
	vCal_Obj* pvCalObj = NULL;
	vCard_Obj* pvCardObj = NULL;
	PVCARDVCAL pVcardVcal = NULL;
	PVCARDVCAL pTemp = NULL;
	RECT rect;
	LISTBUF Listtmp;
	HWND hFromView;
	unsigned char* pTempData;

	memset(&Listtmp, 0, sizeof(LISTBUF));
	PREBROW_InitListBuf(&Listtmp);

	pTempData = malloc(len);
	if(pTempData == NULL)
		return NULL;

	memcpy(pTempData, pData, len);

	if(nType == META_TEXT_VCALE)
	{
		vCal_Reader(pTempData,len,&pvCalObj);
		if(pvCalObj == NULL)
		{
			free(pTempData);
			return NULL;
		}

		CALE_DecodeVcalToDisplay(pvCalObj,&pVcardVcal);

		vCal_Clear(VCAL_OPT_READER,(PVOID)pvCalObj);
	}
	else if(nType == META_TEXT_VCARD)
	{
		vCard_Reader(pTempData,len,&pvCardObj);
		if(pvCardObj == NULL)
		{
			free(pTempData);
			return NULL;
		}

		APP_AnalyseVcard(pvCardObj,&pVcardVcal);

		vCard_Clear(VCARD_OPT_READER,(PVOID)pvCardObj);
	}

	if(pVcardVcal == NULL)
		return FALSE;
	
	pTemp = pVcardVcal;

	while(pTemp)
	{
		PREBROW_AddData(&Listtmp, pTemp->pszTitle, pTemp->pszContent);
		pTemp = pTemp->pNext;
	}

	GetClientRect(hWnd,&rect);

	hFromView = CreateWindow(
		FORMVIEWER,
		"",
		WS_VISIBLE  |WS_CHILD | WS_VSCROLL,
		rect.left,
		rect.top,
		rect.right - rect.left,
		rect.bottom - rect.top,
		hWnd,
		(HMENU)IDC_FORMVIEW_LIST,
		NULL, 
		(PVOID)&Listtmp);
	
	free(pTempData);
	
	if (hFromView == NULL) 
		return NULL;

	SetFocus(hFromView);
    return hFromView;
	
}
