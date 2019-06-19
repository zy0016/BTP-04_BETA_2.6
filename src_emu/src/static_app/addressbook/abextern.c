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
#include "abglobal.h"

BOOL AB_SelectFieldRegisterClass(void);
LRESULT ABSelectFieldWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static BOOL ABSelectField_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct);
static void ABSelectField_OnActivate(HWND hwnd, UINT state);
static void ABSelectField_OnPaint(HWND hWnd);
static void ABSelectField_OnSetFocus(HWND hwnd);
static void ABSelectField_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags);
static LRESULT ABSelectField_OnCommand(HWND hWnd, int id, UINT codeNotify, LPARAM lParam);
static void ABSelectField_OnDestroy(HWND hWnd);
static void ABSelectField_OnClose(HWND hWnd);

#define IDC_ABSELECTFILDE_LIST  300

static BOOL APP_SelectField(HWND hFrameWnd,HWND hMsgWnd,UINT uMsgCmd,ABNAMEOREMAIL* pABName, 
							BOOL bUpdate, DWORD id);

BOOL AB_SetQDialWnd(HWND hFrameWnd,HWND hMsgWnd,UINT uMsgCmd,WORD wDDIdx);
/*********************************************************************\
* Function	   APP_GetInfoByPhone
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL APP_GetInfoByPhone(const char* pPhone, ABINFO* pInfo)
{
//    MEMORY_TEL * pTel;
    MEMORY_NAME * pName;
    CONTACT_ITEMCHAIN* pTemp,*pItem;
    BOOL bGetTone = FALSE;
    int nTelID, count;
	int id[10];
    
    if(pPhone == NULL || pInfo == NULL)
        return FALSE;

	memset(pInfo, 0, sizeof(ABINFO));

    if(strlen(pPhone) > (AB_MAXLEN_TEL-1))
        return FALSE;

	count = AB_FindByTel((char*)pPhone, id, 10);
	if(count != 1)
        return FALSE;

    pName = Memory_Find((PVOID)id[0],AB_FIND_ID);

    if(pName == NULL)
        return FALSE;
    
    pItem = NULL;
    
    strcpy(pInfo->szName,AB_GetNameString(pName));

    strcpy(pInfo->szTel,pPhone);

    pInfo->nGroupID = pName->nGroup;

    AB_ReadRecord(pName->dwoffset,&pItem,&nTelID);
    
    pTemp = pItem;
    
    while(pTemp)
    {
        if(pTemp->dwMask == AB_TYPEMASK_PIC)
        {    
            strcpy(pInfo->szIcon,pTemp->pszData);
        }
        else if(pTemp->dwMask == AB_TYPEMASK_TONE)
        {
            strcpy(pInfo->szRing,pTemp->pszData);
            bGetTone = TRUE;
        }
        pTemp = pTemp->pNext;
    }

    Item_Erase(pItem);

    if(bGetTone == FALSE)
    {
        if(pName->nGroup != 0)
            AB_GetGroupToneByID(pName->nGroup,pInfo->szRing,AB_MAXLEN_RING);
    }

    return TRUE;
}

/*********************************************************************\
* Function	   APP_GetNameByPhone
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
#if 0
BOOL APP_GetNameByPhone(const char* pPhone, ABNAMEOREMAIL* pABName)
{
    MEMORY_TEL * pTel;
    MEMORY_NAME * pName;
    
    if(pPhone == NULL || pABName == NULL)
        return FALSE;

    if(strlen(pPhone) > (AB_MAXLEN_TEL-1))
        return FALSE;

    pTel = Memory_Find((PVOID)pPhone,AB_FIND_TEL);

    if(pTel == NULL)
        return FALSE;

    pName = Memory_Find((PVOID)pTel->id,AB_FIND_ID);

    if(pName == NULL)
        return FALSE;

    strcpy(pABName->szName,AB_GetNameString(pName));

    strcpy(pABName->szTelOrEmail,pPhone);

    pABName->nType = AB_NUMBER;

    return TRUE;
}
#endif

BOOL APP_GetNameByPhone(const char* pPhone, ABNAMEOREMAIL* pABName)
{
    MEMORY_NAME * pName;
	int id[10];
	int count;
    
    if(pPhone == NULL || pABName == NULL)
        return FALSE;

    if(strlen(pPhone) > (AB_MAXLEN_TEL-1))
        return FALSE;

	printf("Calling APP_GetNameByPhone, pPhone = %s\r\n", pPhone);

	count = AB_FindByTel((char*)pPhone, id, 10);

	printf("Find %d \r\n", count);
	
    if(count != 1)
        return FALSE;

    pName = Memory_Find((PVOID)id[0],AB_FIND_ID);

    if(pName == NULL)
        return FALSE;

    strcpy(pABName->szName,AB_GetNameString(pName));

    strcpy(pABName->szTelOrEmail,pPhone);

    pABName->nType = AB_NUMBER;

    return TRUE;
}

/*********************************************************************\
* Function	   APP_GetNameByEmail
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL APP_GetNameByEmail(const char* pEmailAddr, ABNAMEOREMAIL* pABName)
{
    MEMORY_TEL * pEmail;
    MEMORY_NAME * pName;
    
    if(pEmailAddr == NULL || pABName == NULL)
        return FALSE;
    
    if(strlen(pEmailAddr) > (AB_MAXLEN_EMAIL-1))
        return FALSE;

    pEmail = Memory_Find((PVOID)pEmailAddr,AB_FIND_EMAIL);

    if(pEmail == NULL)
        return FALSE;

    pName = Memory_Find((PVOID)pEmail->id,AB_FIND_ID);

    if(pName == NULL)
        return FALSE;

    strcpy(pABName->szName,AB_GetNameString(pName));

    strcpy(pABName->szTelOrEmail,pEmailAddr);

    pABName->nType = AB_EMAIL;

    return TRUE;
}

/*********************************************************************\
* Function	   APP_GetQuickDial
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL APP_GetQuickDial(WORD wDDIdx, ABNAMEOREMAIL* pABName)
{
    AB_QDIAL* pData;
    int i;
    MEMORY_NAME * pName;
    CONTACT_ITEMCHAIN* pTemp,*pItem;
    int nTelID;
    
    if(pABName == NULL)
        return FALSE;

    if(wDDIdx > 9 || wDDIdx < 1)
        return FALSE;

    if(wDDIdx == 1)
    {
        if(GetVoiceMailBox(pABName->szTelOrEmail,AB_MAXLEN_EMAILADDR))//voice mailbox
        {    
            if(strlen(pABName->szTelOrEmail))
            {
                pABName->nType = AB_NUMBER;

                return TRUE;
            }
            else
                return FALSE;
        }
        else 
            return FALSE;
    }

    pData = (AB_QDIAL*)malloc(sizeof(AB_QDIAL)*AB_QDIAL_MAXNUM);
    
    memset(pData,0,sizeof(AB_QDIAL)*AB_QDIAL_MAXNUM);

    AB_ReadQDial(pData);

    for(i = 0 ; i < AB_QDIAL_MAXNUM ; i++ )
    {
        if(pData[i].nNO < 2 || pData[i].nNO > 9)
            continue;

        if(pData[i].nNO == wDDIdx)
        {
            pName = Memory_Find((PVOID)pData[i].id,AB_FIND_ID);

            if(pName == NULL)
            {
                free(pData);

                pData = NULL;

                return FALSE;
            }

            pItem = NULL;

            AB_ReadRecord(pName->dwoffset,&pItem,&nTelID);
			
            pTemp = pItem;

			while(pTemp)
			{
				if(AB_IsTel(pTemp->dwMask))
				{
                    if(pTemp->nID == pData[i].nTelID)
                    {
                        
                        strcpy(pABName->szName,AB_GetNameString(pName));
                        
                        strcpy(pABName->szTelOrEmail,pTemp->pszData);
                        
                        Item_Erase(pItem);

                        pABName->nType = AB_NUMBER;

                        return TRUE;
                    }
				}
				pTemp = pTemp->pNext;
			}
			Item_Erase(pItem);
        }
    }

    return FALSE;
}

/*********************************************************************\
* Function	   APP_GetGroupInfo
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL APP_GetGroupInfo(GROUP_INFO* pGroup_Info, int* pnGroupCounter)
{
    int handle;
    char szOldPath[PATH_MAXLEN];
    struct stat buf;
    LONG datalen;
    GROUP_INFO temp;

    szOldPath[0] = 0;
    
    getcwd(szOldPath,PATH_MAXLEN);  
    
    chdir(PATH_DIR_AB); 

    memset(&buf,0,sizeof(struct stat));
    
    stat(PATH_FILE_GROUP,(struct stat*)&buf);
    
    datalen = buf.st_size;
    
    handle = open(PATH_FILE_GROUP,O_RDONLY,S_IRUSR);        
    
    if(handle == -1)
    {
        chdir(szOldPath);    
    
        return FALSE;
    }  
    
    if(pGroup_Info)
    {    
        *pnGroupCounter = 0;

        while(datalen > 0)
        {
            read(handle,&pGroup_Info[*pnGroupCounter],sizeof(GROUP_INFO));

            if(pGroup_Info[*pnGroupCounter].nDel != AB_DEL_MASK)
                (*pnGroupCounter)++;

            datalen -= sizeof(GROUP_INFO);
        }

        close(handle);

        chdir(szOldPath);    

        return TRUE;
    }
    else
    {
        *pnGroupCounter = 0;

        while(datalen > 0)
        {
            read(handle,&temp,sizeof(GROUP_INFO));

            if(temp.nDel != AB_DEL_MASK)
                (*pnGroupCounter)++;

            datalen -= sizeof(GROUP_INFO);
        }

        close(handle);

        chdir(szOldPath);    

        return TRUE;
    }
    
    close(handle);
    
    chdir(szOldPath);    
    
    return FALSE;
}

/*********************************************************************\
* Function	   APP_GetOnePhoneOrEmail
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
/*
* uMsgCmd 
* wParam = TURE means pick up one; wParam = FLASE means cancel
* lParam = (ABNAMEOREMAIL*)
*/
//exist UI

BOOL APP_GetOnePhoneOrEmail(HWND hFrameWnd, HWND hMsgWnd, UINT uMsgCmd, AB_PICK_TYPE nType)
{
    
    return ABCreateContactPickerWnd(hFrameWnd,hMsgWnd, NULL, uMsgCmd,nType,FALSE,0,TRUE,FALSE);

}

/*********************************************************************\
* Function	   APP_GetMultiPhoneOrEmail
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
/*
* uMsgCmd 
* wParam = number of the selected
* lParam = (ABNAMEOREMAIL*) Array
*/
//exist UI
BOOL APP_GetMultiPhoneOrEmail(HWND hFrameWnd, HWND hMsgWnd, UINT uMsgCmd, 
                              AB_PICK_TYPE nType,ABNAMEOREMAIL* pArray,int nCount,int nMax)
{

    return ABCreateMultiNoOrEmailPickerWnd(hFrameWnd,hMsgWnd,uMsgCmd,nType,pArray,nCount,nMax);

}

/*********************************************************************\
* Function	   APP_SaveToAddressBook
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
/*
* uMsgCmd 
* wParam = TURE means save ok; wParam = FLASE means cancel or failed
* lParam = (ABNAMEOREMAIL*)
*/
BOOL APP_SaveToAddressBook(HWND hFrameWnd, HWND hMsgWnd, UINT uMsgCmd, ABNAMEOREMAIL* pABName,AB_SAVE_MODE nMode)
{
    if(nMode == AB_NEW)
    {  
        return APP_SaveContact(hFrameWnd,hMsgWnd,uMsgCmd,pABName,TRUE);
    }
    else
    {        
//        return FALSE;
        
        return APP_SaveContact(hFrameWnd,hMsgWnd,uMsgCmd,pABName,FALSE);
    }

    return FALSE;
}

/*********************************************************************\
* Function	   APP_GetSIMRecByOrder
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
int APP_GetSIMRecByOrder(int nIndex, PHONEBOOK* pPB)
{
    
    PCONTACT_INITDATA pData;
    
    pData = AB_GetSIMData();

    if(pData->nInit == SIMINIT_SUCCEEDED)
    {
        int i;

        for(i = 0 ; i <pData->nTotalNum ; i++)
        {
            if(pData->pSIMPhoneBook[i].Index == nIndex)
            {
                memcpy(pPB,&(pData->pSIMPhoneBook[i]),sizeof(PHONEBOOK));
                
                return AB_GETSIMREC_SUCC;
            }
        }
    }
    else
    {
        if(pData->nErrCtrl > ERR_RETRY)
            AB_UserListSM();

        return AB_GETSIMREC_INIT;
    }

    return AB_GETSIMREC_NOTFOUNT;
}

/*********************************************************************\
* Function	   APP_SetQuickDial
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
HWND APP_SetQuickDial(HWND hMsgWnd, UINT uMsgCmd,WORD wDDIdx)
{
    HWND hFrameWnd = NULL;
    
    if(wDDIdx > 9 || wDDIdx < 1)
        return NULL;

    hFrameWnd = CreateFrameWindow(WS_CAPTION | PWS_STATICBAR);

    AB_SetQDialWnd(hFrameWnd,hMsgWnd,uMsgCmd,wDDIdx);

    return hFrameWnd;
}




BOOL AB_SetQDialRegisterClass(void);
LRESULT ABSetQDialWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static BOOL ABSetQDial_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct);
static void ABSetQDial_OnActivate(HWND hwnd, UINT state);
static void ABSetQDial_OnPaint(HWND hWnd);
static void ABSetQDial_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags);
static void ABSetQDial_OnCommand(HWND hWnd, int id, UINT codeNotify);
static void ABSetQDial_OnDestroy(HWND hWnd);
static void ABSetQDial_OnClose(HWND hWnd);
static void ABSetQDial_OnGetQDial(HWND hWnd,DWORD nTelID,DWORD id);
static void ABSetQDial_OnGetVMN(HWND hWnd,BOOL bSet,char* pszVMN);

typedef struct tagAB_SetQDialData
{
    HWND                  hFrameWnd;
    HWND                  hMsgWnd;
    UINT                  uMsgCmd;
    ABNAMEOREMAIL         ABName;
    WORD                  wDDIdx;
    AB_QDIAL              QDail[AB_QDIAL_MAXNUM];
}AB_SETQDIALDATA,*PAB_SETQDIALDATA;


BOOL AB_SetQDialRegisterClass(void)
{
    WNDCLASS    wc;

    wc.style         = 0;
    wc.lpfnWndProc   = ABSetQDialWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = sizeof(AB_SETQDIALDATA);
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName  = "ABSetQDialWndClass";
    
    return(RegisterClass(&wc));
}


/*********************************************************************\
* Function	   AB_SetQDialWnd
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL AB_SetQDialWnd(HWND hFrameWnd,HWND hMsgWnd,UINT uMsgCmd,WORD wDDIdx)
{
    AB_SETQDIALDATA CreateData;
    AB_SETQDIALDATA *pData;
    HWND            hWnd;
    
    memset(&CreateData,0,sizeof(AB_SETQDIALDATA));

    CreateData.hFrameWnd = hFrameWnd;
    CreateData.hMsgWnd = hMsgWnd;
    CreateData.uMsgCmd = uMsgCmd;
    CreateData.wDDIdx = wDDIdx;

    hWnd = CreateWindow(
        "ABSetQDialWndClass",
        "", 
        WS_VISIBLE | WS_CHILD, 
        0,
        0,
        0,
        0,
        hFrameWnd,
        NULL, 
        NULL, 
        (PVOID)&CreateData
        );
    
    if (!hWnd)
    {
        return FALSE;
    }

    SetFocus(hWnd);
    
    SendMessage(hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_OK,1), (LPARAM)"");
    SendMessage(hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_CANCEL,0), (LPARAM)"");

    if(wDDIdx == 1)
    {
        char szMailbox[ME_PHONENUM_LEN];

        memset(szMailbox,0,ME_PHONENUM_LEN);
        
        SMS_CreateSettingEditWnd(hFrameWnd,hWnd,WM_GETVMN,(char*)IDS_MESSAGING,szMailbox,FALSE,TRUE);
    }
    else
    {
        pData = GetUserData(hWnd);        
        
        AB_ReadQDial(pData->QDail);

        SetWindowText(hFrameWnd,IDS_QUICKDIAL);
        
        if(ABCreateContactPickerWnd(hFrameWnd,hWnd, NULL, WM_QDIALASSIGN,PICK_NUMBER,FALSE,0,FALSE, TRUE) == FALSE
            || AB_GetContactCount() == 0)
        {
            PostMessage(hWnd,WM_QDIALASSIGN,FALSE,NULL);
        }
    }
    
    return TRUE;
}

LRESULT ABSetQDialWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    LRESULT lResult = TRUE;

	switch (wMsgCmd)
    {
    case WM_CREATE:
        lResult = ABSetQDial_OnCreate(hWnd,(LPCREATESTRUCT)(lParam));
        break;

    case WM_ACTIVATE:
    case PWM_SHOWWINDOW:
        ABSetQDial_OnActivate(hWnd,(UINT)LOWORD(wParam));
        break;

    case WM_PAINT:
        ABSetQDial_OnPaint(hWnd);
        break;

    case WM_KEYDOWN:
        ABSetQDial_OnKey(hWnd,(UINT)(wParam),(int)(short)LOWORD(lParam),(UINT)HIWORD(lParam));
        break;

    case WM_COMMAND:
        ABSetQDial_OnCommand(hWnd,(int)(LOWORD(wParam)),(UINT)HIWORD(wParam));
        break;
        
    case WM_CLOSE:
        ABSetQDial_OnClose(hWnd);
        break;

    case WM_DESTROY:
        ABSetQDial_OnDestroy(hWnd);
        break;

    case WM_QDIALASSIGN:
        ABSetQDial_OnGetQDial(hWnd,(DWORD)wParam,(DWORD)lParam);
        break;

    case WM_GETVMN:
        ABSetQDial_OnGetVMN(hWnd,(BOOL)wParam,(char*)lParam);
        break;

    default:     
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
	}

    return lResult;

}
/*********************************************************************\
* Function	ABSetQDial_OnCreate
* Purpose   WM_CREATE message handler of the main window
* Params
*			hWnd: Handle of the window
*			lpCreateStruct: Create Structure
* Return
*			TRUE: Success
*			FALSE: Fail
* Remarks
**********************************************************************/
static BOOL ABSetQDial_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
{    
    AB_SETQDIALDATA *pData;
    
    pData = GetUserData(hWnd);

    memcpy(pData,lpCreateStruct->lpCreateParams,sizeof(AB_SETQDIALDATA));
            
    return TRUE;
    
}
/*********************************************************************\
* Function	ABSetQDial_OnActivate
* Purpose   WM_ACTIVATE message handler of the main window
* Params
* Return    None
* Remarks
**********************************************************************/
static void ABSetQDial_OnActivate(HWND hwnd, UINT state)
{
    SetFocus(hwnd);

    return;
}

/*********************************************************************\
* Function	ABSetQDial_OnPaint
* Purpose   WM_PAINT message handler of the main window
* Params	hWnd: Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void ABSetQDial_OnPaint(HWND hWnd)
{
	HDC hdc = BeginPaint(hWnd, NULL);

	EndPaint(hWnd, NULL);

	return;
}

/*********************************************************************\
* Function	ABSetQDial_OnKey
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
static void ABSetQDial_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags)
{
    
    switch (vk)
	{
	default:
		PDADefWindowProc(hWnd, WM_KEYDOWN, vk, MAKELPARAM(cRepeat, flags));
		break;
	}

	return;
}
/*********************************************************************\
* Function	ABSetQDial_OnCommand
* Purpose   WM_COMMAND message handler of the main window
* Params
*			hWnd:	Handle of the window
*			id:		Id of command message
*			hwndCtl: Handle of the window which sended this message
*			codeNotify:Notify code of the message
* Return	None
* Remarks
**********************************************************************/
static void ABSetQDial_OnCommand(HWND hWnd, int id, UINT codeNotify)
{
	switch(id)
	{
    default:
        break;
    }
}
/*********************************************************************\
* Function	ABSetQDial_OnDestroy
* Purpose   WM_DESTROY message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void ABSetQDial_OnDestroy(HWND hWnd)
{

    return;
    
}
/*********************************************************************\
* Function	ABSetQDial_OnClose
* Purpose   WM_CLOSE message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void ABSetQDial_OnClose(HWND hWnd)
{
    AB_SETQDIALDATA *pData;
    
    pData = GetUserData(hWnd);

    SendMessage(pData->hFrameWnd,PWM_CLOSEWINDOW,(WPARAM)hWnd,NULL);

    DestroyWindow(hWnd);

    return;

}
/*********************************************************************\
* Function	ABSetQDial_OnGetQDial
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
static void ABSetQDial_OnGetQDial(HWND hWnd,DWORD nTelID,DWORD id)
{
    AB_SETQDIALDATA *pData;
    CONTACT_ITEMCHAIN *pItem,*pTemp;
    int  nTelIDTemp = 0;   
    MEMORY_NAME *pName = NULL;
    BOOL bFind;

    pData = GetUserData(hWnd);

    if(id == -1 || nTelID == -1)
    {
        SendMessage(pData->hMsgWnd,pData->uMsgCmd,FALSE,NULL);
        
        PostMessage(hWnd,WM_CLOSE,0,0);

        return;
    }
    
    pData->QDail[pData->wDDIdx-2].nNO = pData->wDDIdx;
    pData->QDail[pData->wDDIdx-2].id = id;
    pData->QDail[pData->wDDIdx-2].nTelID = nTelID;

    AB_WriteQDial(pData->QDail);
 
    pName = Memory_Find((PVOID)id,AB_FIND_ID);

    if(pName == NULL)
    {
       SendMessage(pData->hMsgWnd,pData->uMsgCmd,FALSE,NULL);
    
       PostMessage(hWnd,WM_CLOSE,0,0);

       return;
    }
    
    bFind = FALSE;

    pItem = NULL;
    
    AB_ReadRecord(pName->dwoffset,&pItem,&nTelIDTemp);
   
    pTemp = pItem;

    pData->ABName.nType = AB_NUMBER;

    strcpy(pData->ABName.szName,AB_GetNameString(pName));

    while(pTemp)
    {
        if(pTemp->nID == (int)nTelID)
        {
            strcpy(pData->ABName.szTelOrEmail,pTemp->pszData);
         
            bFind = TRUE;

            break;
        }
        pTemp = pTemp->pNext;
    }
    
    Item_Erase(pItem);
    
    if(bFind)
        SendMessage(pData->hMsgWnd,pData->uMsgCmd,TRUE,(LPARAM)&(pData->ABName));
    else
        SendMessage(pData->hMsgWnd,pData->uMsgCmd,FALSE,NULL);

    PostMessage(hWnd,WM_CLOSE,0,0);
}

/*********************************************************************\
* Function	ABSetQDial_OnGetVMN
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
static void ABSetQDial_OnGetVMN(HWND hWnd,BOOL bSet,char* pszVMN)
{
    AB_SETQDIALDATA *pData;
    
    pData = GetUserData(hWnd);

    if(bSet == TRUE)
        PLXTipsWin(NULL,NULL,NULL,IDS_VMNSET,IDS_VOICEMAILBOX,Notify_Success,IDS_OK,NULL,WAITTIMEOUT);
    
	if(bSet == FALSE)
		SendMessage(pData->hMsgWnd,pData->uMsgCmd,(WPARAM)FALSE,(LPARAM)NULL);
	else
	{
		ABNAMEOREMAIL ABName;

		SMS_SaveVMN(pszVMN);
		
		memset(&ABName,0,sizeof(ABNAMEOREMAIL));
		
		ABName.nType = AB_NUMBER;
		
		strcpy(ABName.szTelOrEmail,pszVMN);
		
		SendMessage(pData->hMsgWnd,pData->uMsgCmd,(WPARAM)TRUE,(LPARAM)&ABName);	
	}

    PostMessage(hWnd,WM_CLOSE,0,0);
}




BOOL AB_SaveContactRegisterClass(void);
LRESULT ABSaveContactWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static BOOL ABSaveContact_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct);
static void ABSaveContact_OnActivate(HWND hwnd, UINT state);
static void ABSaveContact_OnPaint(HWND hWnd);
static void ABSaveContact_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags);
static void ABSaveContact_OnCommand(HWND hWnd, int id, UINT codeNotify);
static void ABSaveContact_OnDestroy(HWND hWnd);
static void ABSaveContact_OnClose(HWND hWnd);
static void ABSaveContact_OnSelectField(HWND hWnd,BOOL bSelect,DWORD dwMask);
static void ABSaveContact_OnSelectContract(HWND hWnd,BOOL bSelect,DWORD id);
static void ABSaveContact_OnSave(HWND hWnd,BOOL bSaved,char* pszName);
static void ABSaveContact_OnReplace(HWND hWnd,BOOL bReplace);
typedef struct tagAB_SaveContactData
{
    HWND                  hFrameWnd;
    HWND                  hMsgWnd;
    UINT                  uMsgCmd;
    ABNAMEOREMAIL         ABName;
    BOOL                  bNew;
    DWORD                 id;
    char                  szCaption[50];
}AB_SAVECONTACTDATA,*PAB_SAVECONTACTDATA;


BOOL AB_SaveContactRegisterClass(void)
{
    WNDCLASS    wc;

    wc.style         = 0;
    wc.lpfnWndProc   = ABSaveContactWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = sizeof(AB_SAVECONTACTDATA);
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName  = "ABSaveContactWndClass";
    
    return(RegisterClass(&wc));
}


/*********************************************************************\
* Function	   APP_SaveContact
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL APP_SaveContact(HWND hFrame,HWND hMsgWnd,UINT uMsgCmd,ABNAMEOREMAIL *pABName,BOOL bNew)
{
    AB_SAVECONTACTDATA CreateData;
    HWND        hSaveContactWnd;
    HWND		hFrameWnd;

    memset(&CreateData,0,sizeof(AB_SAVECONTACTDATA));

	if(hFrame == NULL)
		hFrameWnd = CreateFrameWindow(WS_CAPTION | PWS_STATICBAR);
	else
		hFrameWnd = hFrame;

    CreateData.hFrameWnd = hFrameWnd;
    CreateData.hMsgWnd = hMsgWnd;
    CreateData.uMsgCmd = uMsgCmd;
    CreateData.bNew = bNew;
    memcpy(&(CreateData.ABName),pABName,sizeof(ABNAMEOREMAIL));

    GetWindowText(hFrameWnd,CreateData.szCaption,49);
    CreateData.szCaption[49] = 0;

    hSaveContactWnd = CreateWindow(
        "ABSaveContactWndClass",
        "", 
        WS_VISIBLE | WS_CHILD, 
        0,
        0,
        0,
        0,
        hFrameWnd,
        NULL, 
        NULL, 
        (PVOID)&CreateData
        );
    
    if (!hSaveContactWnd)
    {
        return FALSE;
    }

    //SetFocus(hSaveContactWnd);

    if(bNew)
    {
		SetWindowText(hFrameWnd, IDS_NEW);
		
        if(pABName->nType == AB_URL)
        {
            CONTACT_ITEMCHAIN *pItem = NULL;
            int nTelID = 0;

            if(AB_SaveGenChain(&pItem,&nTelID,AB_TYPEMASK_HTTPURL,pABName->szTelOrEmail) == TRUE)
            {       
                AB_CreateEditContactWnd(hFrameWnd,hSaveContactWnd,WM_SAVE,pItem,pItem,TRUE,-1,-1,nTelID,NULL);
                
                Item_Erase(pItem);
            }
        }
        else
            APP_SelectField(hFrameWnd,hSaveContactWnd,WM_SELECTFIELD,pABName, FALSE, 0);
    }
    else
        ABCreateContactPickerWnd(hFrameWnd,hSaveContactWnd, NULL, WM_SELECTCONTACT,PICK_NUMBERANDEMAIL,TRUE,0,TRUE, FALSE);

    
    return TRUE;
}

LRESULT ABSaveContactWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    LRESULT lResult = TRUE;

	switch (wMsgCmd)
    {
    case WM_CREATE:
        lResult = ABSaveContact_OnCreate(hWnd,(LPCREATESTRUCT)(lParam));
        break;

    case WM_ACTIVATE:
    case PWM_SHOWWINDOW:
        ABSaveContact_OnActivate(hWnd,(UINT)LOWORD(wParam));
        break;

    case WM_PAINT:
        ABSaveContact_OnPaint(hWnd);
        break;

    case WM_KEYDOWN:
        ABSaveContact_OnKey(hWnd,(UINT)(wParam),(int)(short)LOWORD(lParam),(UINT)HIWORD(lParam));
        break;

    case WM_COMMAND:
        ABSaveContact_OnCommand(hWnd,(int)(LOWORD(wParam)),(UINT)HIWORD(wParam));
        break;
        
    case WM_CLOSE:
        ABSaveContact_OnClose(hWnd);
        break;

    case WM_DESTROY:
        ABSaveContact_OnDestroy(hWnd);
        break;

    case WM_SELECTFIELD:
        ABSaveContact_OnSelectField(hWnd,(BOOL)wParam,(DWORD)lParam);
        break;

    case WM_SELECTCONTACT:
        ABSaveContact_OnSelectContract(hWnd,(BOOL)wParam,(DWORD)lParam);
        break;

    case WM_SAVE:
        ABSaveContact_OnSave(hWnd,(BOOL)wParam,(char*)lParam);
        break;

    default:     
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
	}

    return lResult;

}
/*********************************************************************\
* Function	ABSaveContact_OnCreate
* Purpose   WM_CREATE message handler of the main window
* Params
*			hWnd: Handle of the window
*			lpCreateStruct: Create Structure
* Return
*			TRUE: Success
*			FALSE: Fail
* Remarks
**********************************************************************/
static BOOL ABSaveContact_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
{    
    AB_SAVECONTACTDATA *pData;
    
    pData = GetUserData(hWnd);

    memcpy(pData,lpCreateStruct->lpCreateParams,sizeof(AB_SAVECONTACTDATA));
            
    return TRUE;
    
}
/*********************************************************************\
* Function	ABSaveContact_OnActivate
* Purpose   WM_ACTIVATE message handler of the main window
* Params
* Return    None
* Remarks
**********************************************************************/
static void ABSaveContact_OnActivate(HWND hwnd, UINT state)
{
    SetFocus(hwnd);

    return;
}

/*********************************************************************\
* Function	ABSaveContact_OnPaint
* Purpose   WM_PAINT message handler of the main window
* Params	hWnd: Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void ABSaveContact_OnPaint(HWND hWnd)
{
	HDC hdc = BeginPaint(hWnd, NULL);

	EndPaint(hWnd, NULL);

	return;
}

/*********************************************************************\
* Function	ABSaveContact_OnKey
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
static void ABSaveContact_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags)
{
    
    switch (vk)
	{
	default:
		PDADefWindowProc(hWnd, WM_KEYDOWN, vk, MAKELPARAM(cRepeat, flags));
		break;
	}

	return;
}
/*********************************************************************\
* Function	ABSaveContact_OnCommand
* Purpose   WM_COMMAND message handler of the main window
* Params
*			hWnd:	Handle of the window
*			id:		Id of command message
*			hwndCtl: Handle of the window which sended this message
*			codeNotify:Notify code of the message
* Return	None
* Remarks
**********************************************************************/
static void ABSaveContact_OnCommand(HWND hWnd, int id, UINT codeNotify)
{
	switch(id)
	{
    default:
        break;
    }
}
/*********************************************************************\
* Function	ABSaveContact_OnDestroy
* Purpose   WM_DESTROY message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void ABSaveContact_OnDestroy(HWND hWnd)
{

    return;
    
}
/*********************************************************************\
* Function	ABSaveContact_OnClose
* Purpose   WM_CLOSE message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void ABSaveContact_OnClose(HWND hWnd)
{
    AB_SAVECONTACTDATA *pData;
    
    pData = GetUserData(hWnd);

    SendMessage(pData->hFrameWnd,PWM_CLOSEWINDOW,(WPARAM)hWnd,NULL);

    DestroyWindow(hWnd);

    return;

}
/*********************************************************************\
* Function	ABSaveContact_OnSelectField
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
static void ABSaveContact_OnSelectField(HWND hWnd,BOOL bSelect,DWORD dwMask)
{
    CONTACT_ITEMCHAIN *pItem;
    int nTelID;
    AB_SAVECONTACTDATA *pData;
    
    if(bSelect == FALSE)
    {
        PostMessage(hWnd,WM_CLOSE,0,0);

        return;
    }
    
    pData = GetUserData(hWnd);

    pItem = NULL;

    if(AB_SaveGenChain(&pItem,&nTelID,dwMask,pData->ABName.szTelOrEmail) == TRUE)
    {
        AB_CreateEditContactWnd(pData->hFrameWnd,hWnd,WM_SAVE,pItem,pItem,TRUE,-1,-1,nTelID,NULL);

        Item_Erase(pItem);
    }
    
    //PostMessage(hWnd,WM_CLOSE,0,0);
}

/*********************************************************************\
* Function	ABSaveContact_OnSelectContact
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
static void ABSaveContact_OnSelectContract(HWND hWnd,BOOL bSelect,DWORD id)
{
    CONTACT_ITEMCHAIN *pItem;
    int nTelID;
    AB_SAVECONTACTDATA *pData;
    MEMORY_NAME * p;           
 
    pData = GetUserData(hWnd);   

    if(bSelect == FALSE)
    {
        
        SendMessage(pData->hMsgWnd,pData->uMsgCmd,FALSE,NULL);

        PostMessage(hWnd,WM_CLOSE,0,0);

        return;
    }
    
    pData->id = id;

    p = Memory_Find((PVOID)id,AB_FIND_ID);

    pItem = NULL;

    AB_ReadRecord(p->dwoffset,&pItem,&nTelID);

/*
        if(pData->ABName.nType == AB_URL)
        {
            if(AB_IsExistMask(pItem,AB_TYPEMASK_HTTPURL) == TRUE)
            {
                PLXConfirmWinEx(pData->hFrameWnd,hWnd,IDS_REPLACE,Notify_Request,NULL,IDS_OK,IDS_CANCEL,WM_REPLACE);
    
                Item_Erase(pItem);
    
                return;
            }
            else
            {
                CONTACT_ITEMCHAIN *pNewItem;
                int nIndex;
                
                nIndex = GetIndexByMask(AB_TYPEMASK_HTTPURL);
                
                pNewItem = Item_New(Contact_Item[nIndex].dwMask);
                
                if(pNewItem == NULL)
                {
                    Item_Erase(pItem);
                    
                    SendMessage(pData->hMsgWnd,pData->uMsgCmd,FALSE,NULL);
    
                    PostMessage(hWnd,WM_CLOSE,0,0);
                    
                    return;
                }
                            
                strcpy(pNewItem->pszData,pData->ABName.szTelOrEmail);
                
                Item_Insert(&pItem,pNewItem);
                
                AB_CreateEditContactWnd(pData->hFrameWnd,hWnd,WM_SAVE,pItem,pNewItem,FALSE,p->dwoffset,p->id,nTelID,NULL);
            }
        }
        else
        {
            APP_SelectField(pData->hFrameWnd,hWnd,WM_SELECTFIELD,&(pData->ABName));
        }*/
    
	
	APP_SelectField(pData->hFrameWnd,hWnd,WM_SAVE,&(pData->ABName), TRUE, id);
    Item_Erase(pItem);
}

/*********************************************************************\
* Function	ABSaveContact_OnSave
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
static void ABSaveContact_OnSave(HWND hWnd,BOOL bSaved,char* pszName)
{
    AB_SAVECONTACTDATA *pData; 

    pData = GetUserData(hWnd);
        
    if(bSaved == FALSE)
    {
        SendMessage(pData->hMsgWnd,pData->uMsgCmd,FALSE,NULL);

        PostMessage(hWnd,WM_CLOSE,0,0);

        return;
    }
    
    strcpy(pData->ABName.szName,pszName);

    SendMessage(pData->hMsgWnd,pData->uMsgCmd,TRUE,(LPARAM)&(pData->ABName));

    PostMessage(hWnd,WM_CLOSE,0,0);
}

typedef struct tagAB_SelectFieldData
{
    HWND                  hFrameWnd;
    HWND                  hMsgWnd;
    UINT                  uMsgCmd;
    ABNAMEOREMAIL         ABName;
	BOOL				  bUpdate;
	DWORD				  id;
	CONTACT_ITEMCHAIN*    pItem;
	int					  nTelID;
	HBITMAP				  hButton;
	HBITMAP				  hButtonSel;
    char                  szCaption[50];
}AB_SELECTFIELDDATA,*PAB_SELECTFIELDDATA;

BOOL AB_SelectFieldRegisterClass(void)
{
    WNDCLASS    wc;

    wc.style         = 0;
    wc.lpfnWndProc   = ABSelectFieldWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = sizeof(AB_SELECTFIELDDATA);
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName  = "ABSelectFieldWndClass";
    
    return(RegisterClass(&wc));
}


static BOOL APP_SelectField(HWND hFrameWnd,HWND hMsgWnd,UINT uMsgCmd,ABNAMEOREMAIL* pABName, 
							BOOL bUpdate, DWORD id)
{
    AB_SELECTFIELDDATA CreateData;
    HWND        hWnd;
    RECT        rcClient;
    
    memset(&CreateData,0,sizeof(AB_SELECTFIELDDATA));

    CreateData.hFrameWnd = hFrameWnd;
    CreateData.hMsgWnd = hMsgWnd;
    CreateData.uMsgCmd = uMsgCmd;
	CreateData.bUpdate = bUpdate;
	CreateData.id = id;
    memcpy(&(CreateData.ABName),pABName,sizeof(ABNAMEOREMAIL));

    GetWindowText(hFrameWnd,CreateData.szCaption,49);
    CreateData.szCaption[49] = 0;

    GetClientRect(hFrameWnd,&rcClient);

    hWnd = CreateWindow(
        "ABSelectFieldWndClass",
        "", 
        WS_VISIBLE | WS_CHILD, 
        rcClient.left,
        rcClient.top,
        rcClient.right - rcClient.left,
        rcClient.bottom - rcClient.top,
        hFrameWnd,
        NULL, 
        NULL, 
        (PVOID)&CreateData
        );
    
    if (!hWnd)
    {
        return FALSE;
    }

    SetFocus(hWnd);
    
    SendMessage(hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(200, 0), (LPARAM)IDS_CANCEL);
	SendMessage(hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(200, 1), (LPARAM)"");
    SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_SELECT);
    
    SendMessage(hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON),(LPARAM)NULL);
    SendMessage(hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON),(LPARAM)NULL);

    return TRUE;
}

LRESULT ABSelectFieldWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    LRESULT lResult = TRUE;

	switch (wMsgCmd)
    {
    case WM_CREATE:
        lResult = ABSelectField_OnCreate(hWnd,(LPCREATESTRUCT)(lParam));
        break;

    case WM_ACTIVATE:
    case PWM_SHOWWINDOW:
        ABSelectField_OnActivate(hWnd,(UINT)LOWORD(wParam));
        break;

    case WM_SETFOCUS:
        ABSelectField_OnSetFocus(hWnd);
        break;

    case WM_PAINT:
        ABSelectField_OnPaint(hWnd);
        break;

	case WM_TIMER:
		{
			HWND hList;
            int  index;
            DWORD dwMask;
			AB_SELECTFIELDDATA* pData;
			
			pData = (AB_SELECTFIELDDATA*)GetUserData(hWnd);

            hList = GetDlgItem(hWnd,IDC_ABSELECTFILDE_LIST);

            index = SendMessage(hList,LB_GETCURSEL,0,0);

            if(index == LB_ERR)
                break;
			
			KillTimer(hWnd, 1);

			dwMask = (DWORD)SendMessage(hList,LB_GETITEMDATA,index,NULL);
			
			SendMessage(pData->hMsgWnd,pData->uMsgCmd,(WPARAM)TRUE,(LPARAM)dwMask);

			PostMessage(hWnd,WM_CLOSE,NULL,NULL);
		}
	
		break;

    case WM_KEYDOWN:
        ABSelectField_OnKey(hWnd,(UINT)(wParam),(int)(short)LOWORD(lParam),(UINT)HIWORD(lParam));
        break;

    case WM_COMMAND:
        lResult = ABSelectField_OnCommand(hWnd,(int)(LOWORD(wParam)),(UINT)HIWORD(wParam), lParam);
        break;

    case WM_REPLACE:
        ABSaveContact_OnReplace(hWnd,(BOOL)lParam);
        break;

    case WM_CLOSE:
        ABSelectField_OnClose(hWnd);
        break;

    case WM_DESTROY:
        ABSelectField_OnDestroy(hWnd);
        break;

    default:     
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
	}

    return lResult;

}
/*********************************************************************\
* Function	ABSelectField_OnCreate
* Purpose   WM_CREATE message handler of the main window
* Params
*			hWnd: Handle of the window
*			lpCreateStruct: Create Structure
* Return
*			TRUE: Success
*			FALSE: Fail
* Remarks
**********************************************************************/
static BOOL ABSelectField_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
{    
    RECT rect;
    AB_SELECTFIELDDATA *pData;
    int  insert = 0;
    int i;
    HWND hList;
	int style = 0;
	SIZE size;
    
    pData = GetUserData(hWnd);

    memcpy(pData,lpCreateStruct->lpCreateParams,sizeof(AB_SELECTFIELDDATA));
    
    GetClientRect(hWnd,&rect);

    GetImageDimensionFromFile(AB_BMP_NORMAL,&size);

    pData->hButton = LoadImage(NULL, AB_BMP_NORMAL, IMAGE_BITMAP,
		size.cx, size.cy, LR_LOADFROMFILE);
	
	GetImageDimensionFromFile(AB_BMP_SELECT,&size);

	pData->hButtonSel = LoadImage(NULL, AB_BMP_SELECT, IMAGE_BITMAP,
		size.cx, size.cy, LR_LOADFROMFILE);
	
	if(pData->bUpdate)
		style |= WS_VISIBLE | WS_CHILD | WS_VSCROLL | LBS_BITMAP | LBS_MULTILINE;
	else
		style |= WS_VISIBLE | WS_CHILD | WS_VSCROLL | LBS_BITMAP;

    hList = CreateWindow(
        "LISTBOX",
        "",
        style,
        rect.left,
        rect.top,
        rect.right - rect.left,
        rect.bottom - rect.top,
        hWnd,
        (HMENU)IDC_ABSELECTFILDE_LIST,
        NULL,
        NULL);
   
    if(hList == NULL)
        return FALSE;

	if(!pData->bUpdate)
	{
		//only list all available item
		switch(pData->ABName.nType)
		{
		case AB_NUMBER:
			for(i = 0 ; i < AB_EDITCONTACT_CTRNUM ; i++)
			{
				if(AB_IsTel(Contact_Item[i].dwMask))
				{
					insert = SendMessage(hList,LB_ADDSTRING,-1,(LPARAM)Contact_Item[i].pszCpation);
					SendMessage(hList,LB_SETITEMDATA,insert,(LPARAM)Contact_Item[i].dwMask);
					SendMessage(hList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, insert), (LPARAM)pData->hButton);
				}
			}
			break;
			
		case AB_EMAIL:
			for(i = 0 ; i < AB_EDITCONTACT_CTRNUM ; i++)
			{
				if(AB_IsEmail(Contact_Item[i].dwMask))
				{
					insert = SendMessage(hList,LB_ADDSTRING,-1,(LPARAM)Contact_Item[i].pszCpation);
					SendMessage(hList,LB_SETITEMDATA,insert,(LPARAM)Contact_Item[i].dwMask);
					SendMessage(hList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, insert), (LPARAM)pData->hButton);
				}
			}
			break;
			
		default:
		case AB_URL:
			return FALSE;
		}
	}
    else
	{
		//list all available item and the old record's content
		CONTACT_ITEMCHAIN* pItem;
		CONTACT_ITEMCHAIN* pTemp;
		int  nTelID = 0;
		MEMORY_NAME *p;
		char        szCaption[AB_MAXLEN_FIRSTNAME*2];
		BOOL bFind = FALSE;

		pItem = NULL;

        p = Memory_Find((PVOID)(pData->id),AB_FIND_ID);

        AB_ReadRecord(p->dwoffset,&pItem,&nTelID);

		AB_GetFullName(pItem, szCaption);
		strncpy(pData->szCaption, szCaption, 49);
		pData->szCaption[49] = 0;

		SetWindowText(pData->hFrameWnd, pData->szCaption);

		pData->pItem = pItem;
		pData->nTelID = nTelID;

		switch(pData->ABName.nType)
		{
		case AB_NUMBER:
			for(i = 0 ; i < AB_EDITCONTACT_CTRNUM ; i++)
			{
				pTemp = pItem;

				if(AB_IsTel(Contact_Item[i].dwMask))
				{
					while(pTemp)
					{
						if(pTemp->dwMask == Contact_Item[i].dwMask)
						{
							insert = SendMessage(hList,LB_ADDSTRING,-1,(LPARAM)Contact_Item[i].pszCpation);
							SendMessage(hList,LB_SETITEMDATA,insert,(LPARAM)pTemp);
							SendMessage(hList, LB_SETAUXTEXT, MAKEWPARAM(insert, -1), (LPARAM)pTemp->pszData);
							bFind = TRUE;
						}
						pTemp = pTemp->pNext;
					}
					
					if(!bFind || !Contact_Item[i].bOnly)
					{
						insert = SendMessage(hList,LB_ADDSTRING,-1,(LPARAM)Contact_Item[i].pszCpation);
						SendMessage(hList,LB_SETITEMDATA,insert,(LPARAM)0);
					}

					bFind = FALSE;
				}
			}
			break;
			
		case AB_EMAIL:
			for(i = 0 ; i < AB_EDITCONTACT_CTRNUM ; i++)
			{
				pTemp = pItem;
				
				if(AB_IsEmail(Contact_Item[i].dwMask))
				{
					while(pTemp)
					{
						if(pTemp->dwMask == Contact_Item[i].dwMask)
						{
							insert = SendMessage(hList,LB_ADDSTRING,-1,(LPARAM)Contact_Item[i].pszCpation);
							SendMessage(hList,LB_SETITEMDATA,insert,(LPARAM)pTemp);
							SendMessage(hList, LB_SETAUXTEXT, MAKEWPARAM(insert, -1), (LPARAM)pTemp->pszData);
							bFind = TRUE;
						}
						pTemp = pTemp->pNext;
					}
					
					if(!bFind || !Contact_Item[i].bOnly)
					{
						insert = SendMessage(hList,LB_ADDSTRING,-1,(LPARAM)Contact_Item[i].pszCpation);
						SendMessage(hList,LB_SETITEMDATA,insert,(LPARAM)0);
					}

					bFind = FALSE;
					
				}
			}
			break;
			
		
		case AB_URL:
			{
				int index;

				pTemp = pItem;

				index = GetIndexByMask(AB_TYPEMASK_HTTPURL);

				while(pTemp)
				{
					if(pTemp->dwMask == AB_TYPEMASK_HTTPURL)
					{
						insert = SendMessage(hList,LB_ADDSTRING,-1,(LPARAM)Contact_Item[index].pszCpation);
						SendMessage(hList,LB_SETITEMDATA,insert,(LPARAM)pTemp);
						SendMessage(hList, LB_SETAUXTEXT, MAKEWPARAM(insert, -1), (LPARAM)pTemp->pszData);
						bFind = TRUE;
						break;
					}
					pTemp = pTemp->pNext;
				}

				if(!bFind)
				{
					insert = SendMessage(hList,LB_ADDSTRING,-1,(LPARAM)Contact_Item[index].pszCpation);
					
					SendMessage(hList,LB_SETITEMDATA,insert,(LPARAM)0);
				}
				
			}

			break;
		
		default:
			break;
		}
	}

    SendMessage(hList, LB_SETCURSEL, 0, 0);
    
        
    return TRUE;
    
}

/*********************************************************************\
* Function	ABSaveContact_OnReplace
* Purpose   
* Params	
* Return	
* Remarks
**********************************************************************/
static void ABSaveContact_OnReplace(HWND hWnd,BOOL bReplace)
{
    AB_SELECTFIELDDATA *pData; 
	CONTACT_ITEMCHAIN* pNewItem;
	CONTACT_ITEMCHAIN* pItem;
	HWND hList;
	DWORD dwoffset, nGroup;
	int index, nCount;
	MEMORY_NAME *p;
	
    pData = GetUserData(hWnd);
	hList = GetDlgItem(hWnd,IDC_ABSELECTFILDE_LIST);
    index = SendMessage(hList,LB_GETCURSEL,0,0);

	if(index == LB_ERR)
    {
		SendMessage(pData->hMsgWnd,pData->uMsgCmd,FALSE,NULL);

        PostMessage(hWnd,WM_CLOSE,0,0);

        return;
    }

    if(bReplace == FALSE)
    {
//        SendMessage(pData->hMsgWnd,pData->uMsgCmd,FALSE,NULL);

//        PostMessage(hWnd,WM_CLOSE,0,0);

        return;
    }
    
	p = Memory_Find((PVOID)(pData->id),AB_FIND_ID);
	
	pItem = (CONTACT_ITEMCHAIN*)SendMessage(hList,LB_GETITEMDATA,index,NULL);

	pNewItem = Item_New(pItem->dwMask);
	
	pNewItem->dwMask = pItem->dwMask;
	pNewItem->bChange = pItem->bChange;
	pNewItem->nID = pItem->nID;
	memcpy(pNewItem->pszData, pData->ABName.szTelOrEmail, pNewItem->nMaxLen);
	
	Item_Delete(&(pData->pItem), pItem);
	Item_Insert(&(pData->pItem), pNewItem);
	
	//add new item
	dwoffset = p->dwoffset;
	nGroup = p->nGroup;
	
	if(AB_SaveRecord(&dwoffset,&(pData->id),&nGroup,FALSE,
		pData->nTelID,pData->pItem) == FALSE)
	{
		//failed
		SendMessage(pData->hMsgWnd, pData->uMsgCmd, FALSE, 0);
		PostMessage(hWnd,WM_CLOSE,NULL,NULL);
		return;
	}

	AB_NotifyWnd((PVOID)AB_GetIndexByID(pIndexName,pData->id,nName),
		AB_MODE_DELETE,AB_OBJECT_NAME,AB_MDU_CONTRACT);  

	//delete old one
	nCount = AB_DeleteFromIndexByID(pIndexTel,pData->id,nTel,FALSE);
	nTel -= nCount;
	nCount = AB_DeleteFromIndexByID(pIndexEmail,pData->id,nEmail,FALSE);
	nEmail -= nCount;
	nCount = AB_DeleteFromIndexByID(pIndexID,pData->id,nID,FALSE);
	nID -= nCount;
	nCount = AB_DeleteFromIndexByID(pIndexName,pData->id,nName,TRUE);
	nName--;   
	
	AB_Insert2Table(dwoffset,pData->id,nGroup,pData->pItem);
	SendMessage(pData->hMsgWnd, pData->uMsgCmd, TRUE, (LPARAM)(AB_GetNameString(p)));
	PLXTipsWin(NULL,NULL,0,IDS_REPLACED, NULL, Notify_Success , IDS_OK , NULL, WAITTIMEOUT);
	PostMessage(hWnd,WM_CLOSE,NULL,NULL);
	return;
}

/*********************************************************************\
* Function	ABSelectField_OnActivate
* Purpose   WM_ACTIVATE message handler of the main window
* Params
* Return    None
* Remarks
**********************************************************************/
static void ABSelectField_OnActivate(HWND hwnd, UINT state)
{
    HWND hLst;
    AB_SELECTFIELDDATA *pData;
    
    pData = GetUserData(hwnd);

    hLst = GetDlgItem(hwnd,IDC_ABSELECTFILDE_LIST);

    SetFocus(hLst);

    SendMessage(pData->hFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_CANCEL);
    SendMessage(pData->hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)ICON_SELECT);
	SendMessage(pData->hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)"");

    SendMessage(pData->hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON),(LPARAM)NULL);
    SendMessage(pData->hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON),(LPARAM)NULL);

    SetWindowText(pData->hFrameWnd,pData->szCaption);

    return;
}
/*********************************************************************\
* Function	ABSelectField_OnSetFocus
* Purpose   WM_ACTIVATE message handler of the main window
* Params
* Return    None
* Remarks
**********************************************************************/
static void ABSelectField_OnSetFocus(HWND hwnd)
{
    HWND hLst;
    
    hLst = GetDlgItem(hwnd,IDC_ABSELECTFILDE_LIST);

    SetFocus(hLst);

    return;
}
/*********************************************************************\
* Function	ABSelectField_OnPaint
* Purpose   WM_PAINT message handler of the main window
* Params	hWnd: Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void ABSelectField_OnPaint(HWND hWnd)
{
	HDC hdc = BeginPaint(hWnd, NULL);

	EndPaint(hWnd, NULL);

	return;
}

/*********************************************************************\
* Function	ABSelectField_OnKey
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
static void ABSelectField_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags)
{
    AB_SELECTFIELDDATA* pData;

    pData = (AB_SELECTFIELDDATA*)GetUserData(hWnd);
    
    switch (vk)
	{
	case VK_F10:
		SendMessage(pData->hMsgWnd,pData->uMsgCmd,(WPARAM)FALSE,NULL);

		PostMessage(hWnd,WM_CLOSE,NULL,NULL);
		break;

    case VK_F5:
        {
            HWND hList;
            int  index;

            hList = GetDlgItem(hWnd,IDC_ABSELECTFILDE_LIST);

            index = SendMessage(hList,LB_GETCURSEL,0,0);

            if(index == LB_ERR)
                break;

			if(!pData->bUpdate)
			{
				SendMessage(hList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, index), (LPARAM)pData->hButtonSel);
				SetTimer(hWnd, 1, 500, NULL);
			}
            else
			{
				CONTACT_ITEMCHAIN* pItem;

				pItem = (CONTACT_ITEMCHAIN*)SendMessage(hList,LB_GETITEMDATA,index,NULL);

				if(pItem == 0)
				{
					//new item
					char  name[128];
					int   id, nTelID = 0, nCount;
					int   dwMask;
					CONTACT_ITEMCHAIN* pNewItem;
					DWORD dwoffset;
					int nGroup;
					MEMORY_NAME* p;

					SendMessage(hList, LB_GETTEXT, index, (LPARAM)name);
					id = GetIndexByName(name);

					dwMask = Contact_Item[id].dwMask;
            
					pNewItem = Item_New(dwMask);
					
					if(AB_IsTel(dwMask) || AB_IsEmail(dwMask))
					{
						pNewItem->nID = pData->nTelID;
						pData->nTelID += 1;
					}

					strcpy(pNewItem->pszData,pData->ABName.szTelOrEmail);
					
					Item_Insert(&(pData->pItem),pNewItem);

					p = Memory_Find((PVOID)(pData->id),AB_FIND_ID);

					//add new item
					dwoffset = p->dwoffset;
					nGroup = p->nGroup;

					if(AB_SaveRecord(&dwoffset,&(pData->id),&nGroup,FALSE,
						pData->nTelID,pData->pItem) == FALSE)
					{
						//failed
						SendMessage(pData->hMsgWnd, pData->uMsgCmd, FALSE, 0);
						PostMessage(hWnd,WM_CLOSE,NULL,NULL);
						break;
					}

					AB_NotifyWnd((PVOID)AB_GetIndexByID(pIndexName,pData->id,nName),
						AB_MODE_DELETE,AB_OBJECT_NAME,AB_MDU_CONTRACT);  
					//delete old one
					nCount = AB_DeleteFromIndexByID(pIndexTel,pData->id,nTel,FALSE);
					nTel -= nCount;
					nCount = AB_DeleteFromIndexByID(pIndexEmail,pData->id,nEmail,FALSE);
					nEmail -= nCount;
					nCount = AB_DeleteFromIndexByID(pIndexID,pData->id,nID,FALSE);
					nID -= nCount;
					nCount = AB_DeleteFromIndexByID(pIndexName,pData->id,nName,TRUE);
					nName--;   
					
					AB_Insert2Table(dwoffset,pData->id,nGroup,pData->pItem);
					SendMessage(pData->hMsgWnd, pData->uMsgCmd, TRUE, (LPARAM)(AB_GetNameString(p)));
					PLXTipsWin(NULL,NULL,0,IDS_SAVED, NULL, Notify_Success , IDS_OK , NULL, WAITTIMEOUT);
				
					PostMessage(hWnd,WM_CLOSE,NULL,NULL);
				}
				else
				{
					//cover exist item
					PLXConfirmWinEx(pData->hFrameWnd,hWnd,IDS_REPLACE,Notify_Request,NULL,IDS_YES,IDS_NO,WM_REPLACE);					
				}
			}
            
        }
        break;

	default:
		PDADefWindowProc(hWnd, WM_KEYDOWN, vk, MAKELPARAM(cRepeat, flags));
		break;
	}

	return;
}
/*********************************************************************\
* Function	ABSelectField_OnCommand
* Purpose   WM_COMMAND message handler of the main window
* Params
*			hWnd:	Handle of the window
*			id:		Id of command message
*			hwndCtl: Handle of the window which sended this message
*			codeNotify:Notify code of the message
* Return	None
* Remarks
**********************************************************************/
static LRESULT ABSelectField_OnCommand(HWND hWnd, int id, UINT codeNotify, LPARAM lParam)
{
	switch(id)
	{
	case IDC_ABSELECTFILDE_LIST:
        if(codeNotify == LBN_SETFONT)
        {            
            HFONT hFont = NULL;
            
            GetFontHandle(&hFont, (lParam == 0) ? SMALL_FONT : LARGE_FONT);
            
            return (LRESULT)hFont;
        }
        break;

    default:
        break;
    }

	return 0;
}
/*********************************************************************\
* Function	ABSelectField_OnDestroy
* Purpose   WM_DESTROY message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void ABSelectField_OnDestroy(HWND hWnd)
{
	AB_SELECTFIELDDATA *pData;
    
    pData = GetUserData(hWnd);

	DeleteObject(pData->hButton);
	DeleteObject(pData->hButtonSel);
    return;

}
/*********************************************************************\
* Function	ABSelectField_OnClose
* Purpose   WM_CLOSE message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void ABSelectField_OnClose(HWND hWnd)
{
    AB_SELECTFIELDDATA *pData;
    
    pData = GetUserData(hWnd);

	if(pData->pItem)
		Item_Erase(pData->pItem);
	
    SendMessage(pData->hFrameWnd,PWM_CLOSEWINDOW,(WPARAM)hWnd,NULL);

    DestroyWindow(hWnd);

    return;

}


BOOL APP_SaveVcard(char* pVcardData,int nDataLen)
{
    DWORD dwoffset = -1;
    DWORD id = -1;
    int nGroup = -1;
    int nTelID = 1;

    char* pStart = NULL;
    vCard_Obj* pvCardObj = NULL;
    CONTACT_ITEMCHAIN *pItem;
        
    if(pVcardData != NULL && nDataLen > 0)
    {
        if((pStart = strstr(pVcardData,"BEGIN:VCARD")) && 
            strstr(pVcardData,"END:VCARD") )
        {
            vCard_Reader(pStart,nDataLen,&pvCardObj);
            
            if(pvCardObj == NULL)
                return FALSE;
            
            pItem = NULL;
                        
            AB_AnalyseVcard(pvCardObj,&pItem,&nTelID);
                 
            vCard_Clear(VCARD_OPT_READER,(PVOID)pvCardObj);

            //need modify
            AB_SaveRecord(&dwoffset,&id,&nGroup,FALSE,nTelID,pItem);
            
            AB_Insert2Table(dwoffset,id,nGroup,pItem);
            
            Item_Erase(pItem);

            return TRUE;
        }
        else 
            return FALSE;
    }

    return FALSE;
}

void APP_ABChainEarse(VCARDVCAL *pChain)
{
    VCARDVCAL* p;    
    VCARDVCAL* ptemp;

    p = pChain;

    while( p )
    {
        ptemp = p->pNext;
        
        if(p->pszTitle)
            AB_FREE(p->pszTitle);

        if(p->pszContent)
            AB_FREE(p->pszContent);

        AB_FREE(p);
        p = ptemp;
    }
}

BOOL APP_AnalyseVcard(vCard_Obj* pvCardObj,VCARDVCAL **ppChain)
{   
    CONTACT_ITEMCHAIN *pItem,*pTemp;

    int nTelID = 1;

    int nIndex;

    int nLenTitle,nLenCon;

    VCARDVCAL *pNewChain;

    static VCARDVCAL *pCur;

    pItem = NULL;
    
    *ppChain = NULL;
    
    AB_AnalyseVcard(pvCardObj,&pItem,&nTelID);

    pTemp = pItem;

    while(pTemp)
    {       
        nIndex = GetIndexByMask(pTemp->dwMask);

        nLenTitle = strlen(Contact_Item[nIndex].pszCpation);

        if(nLenTitle > 0)
        {
            pNewChain = (VCARDVCAL*)malloc(sizeof(VCARDVCAL));

            if(pNewChain == NULL)
            {
                APP_ABChainEarse(*ppChain);
                return FALSE;
            }

            memset(pNewChain,0,sizeof(VCARDVCAL));

            pNewChain->pszTitle = (char*)malloc(nLenTitle+1);

            if(pNewChain->pszTitle == NULL)
            {
                APP_ABChainEarse(*ppChain);
                return FALSE;
            }

            memcpy(pNewChain->pszTitle,Contact_Item[nIndex].pszCpation,nLenTitle+1);

            if(pTemp->dwMask == AB_TYPEMASK_DATE)
                nLenCon = VCARD_BIRTH_LEN;
            else
                nLenCon = strlen(pTemp->pszData);

            if(nLenCon > 0)
            {
                pNewChain->pszContent = (char*)malloc(nLenCon+1);
                
                if(pNewChain->pszContent == NULL)
                {
                    APP_ABChainEarse(*ppChain);
                    return FALSE;
                }
                
                if(pTemp->dwMask == AB_TYPEMASK_DATE)
                {
                    char szTime[50];
                    
                    char szDate[50];
                    
                    szTime[0] = 0;
                    
                    szDate[0] = 0;
                    
                    GetTimeDisplay(*((SYSTEMTIME*)(pTemp->pszData)),szTime,szDate);

                    strcpy(pNewChain->pszContent,szDate);
                }
                else
                    memcpy(pNewChain->pszContent,pTemp->pszData,nLenCon+1);
                
            }

            if(*ppChain == NULL)
            {
                *ppChain = pNewChain;
            }
            else
            {
                pCur->pNext = pNewChain;
            }
            pCur = pNewChain;
        }

        pTemp = pTemp->pNext;
    }

    Item_Erase(pItem);

    return TRUE;
}
/*********************************************************************\
* Function	   APP_FreeSource
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
void APP_FreeSource(char* pszVcardFileName)
{

    if(pszVcardFileName == NULL)
        return;

    remove(pszVcardFileName);
}

static LPBYTE AB_atolocal(LPBYTE p,LPBYTE by)
{
    memcpy(by,p+3,1);

    memcpy(by+1,p+2,1);

    memcpy(by+2,p+1,1);
    
    memcpy(by+3,p,1);

    return by;
}

/*********************************************************************\
* Function	   AB_GenItembyData
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL AB_GenItembyData(LPBYTE pData,LONG nDataLen,CONTACT_ITEMCHAIN** ppItem,int* pnTelID,int* pnGroup)
{
    BYTE    *p;
    LONG    LEN;
    DWORD   Mask;
    CONTACT_ITEMCHAIN* pNewItem;
    int     nUTF8Len;
    int     nMultibyteLen;
    BYTE    by[4];
    SYSTEMTIME *psy;
    char    year[5];

    *ppItem = NULL;
    
    LEN = nDataLen;

    p = pData;

    *pnGroup = *((int*)(AB_atolocal(p,by)));

    p += sizeof(int);

    LEN -= sizeof(int);

    *pnTelID = *((int*)(AB_atolocal(p,by)));

    p += sizeof(int);

    LEN -= sizeof(int);

    while(LEN > 0)
    {
        Mask = *((DWORD*)(AB_atolocal(p,by)));

        p += sizeof(DWORD);

        LEN -= sizeof(DWORD);

        switch(Mask)
        {
        case AB_TYPEMASK_DATE:
            pNewItem = Item_New(Mask);
            
            if(pNewItem == NULL)
            {
                Item_Erase(*ppItem);
                
                return FALSE;
            }

            pNewItem->dwMask = Mask;
            pNewItem->bChange = TRUE;

            psy = (SYSTEMTIME*)(pNewItem->pszData);

            memset(psy,0,sizeof(SYSTEMTIME));

            memcpy(year,p,4);
            
            year[4] = 0;

            psy->wYear = atoi(year);
            
            p += 4;

            LEN -= 4;
            
            memcpy(year,p,2);
            
            year[2] = 0;

            psy->wMonth = atoi(year);
            
            p += 2;

            LEN -= 2;
            
            memcpy(year,p,2);
            
            year[2] = 0;

            psy->wDay = atoi(year);
            
            p += 2;

            LEN -= 2;

            Item_Insert(ppItem,pNewItem);

        	break;

        case AB_TYPEMASK_PIC:
        case AB_TYPEMASK_TONE:
            
            pNewItem = (CONTACT_ITEMCHAIN*)malloc(sizeof(CONTACT_ITEMCHAIN));
            
            if(pNewItem == NULL)
            {
                Item_Erase(*ppItem);
                
                return FALSE;
            }
            
            memset(pNewItem,0,sizeof(CONTACT_ITEMCHAIN));
            
            pNewItem->dwMask = Mask;

            nUTF8Len = *((int*)(AB_atolocal(p,by)));
            
            p += sizeof(int);
            
            LEN -= sizeof(int);

            pNewItem->nMaxLen = UTF8ToMultiByte(CP_ACP, 0, (LPCSTR)p, -1, (LPSTR)NULL, 0, NULL, NULL);

            pNewItem->pszData = (char*)malloc(pNewItem->nMaxLen);
            
            if(pNewItem->pszData == NULL)
            {
                Item_Erase(*ppItem);
                
                free(pNewItem);
                
                return FALSE;
            }

            UTF8ToMultiByte(CP_ACP, 0, p, -1, pNewItem->pszData, pNewItem->nMaxLen, NULL, NULL);
            
            pNewItem->bChange = TRUE;

            p += nUTF8Len;
            
            LEN -= nUTF8Len;

            Item_Insert(ppItem,pNewItem);
        	
            break;

        default:            
            pNewItem = Item_New(Mask);
            
            if(pNewItem == NULL)
            {
                Item_Erase(*ppItem);
                
                return FALSE;
            }
            
            pNewItem->dwMask = Mask;
            pNewItem->bChange = TRUE;
            
            if(AB_IsTel(Mask) || AB_IsEmail(Mask))
            {   
                pNewItem->nID = *((DWORD*)(AB_atolocal(p,by)));
                
                p += sizeof(DWORD);
                
                LEN -= sizeof(DWORD);
            }
            
            nUTF8Len = *((int*)(AB_atolocal(p,by)));
            
            p += sizeof(int);
            
            LEN -= sizeof(int);

            nMultibyteLen = UTF8ToMultiByte(CP_ACP, 0, (LPCSTR)p, -1, (LPSTR)pNewItem->pszData, pNewItem->nMaxLen,NULL,NULL);
            
            p += nUTF8Len;

            LEN -= nUTF8Len;
            
            Item_Insert(ppItem,pNewItem);

            break;
        }
    }

    return TRUE;
}
/*********************************************************************\
* Function	   APP_AddApi
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL APP_AddApi(DWORD* pid,LPBYTE pData,LONG nDataLen)
{ 
    DWORD   dwoffset = -1;
    int     nGroup;
    int     nTelID;
    CONTACT_ITEMCHAIN* pItem;

    pItem = NULL;

    if(AB_GenItembyData(pData,nDataLen,&pItem,&nTelID,&nGroup) == FALSE)
        return FALSE;

    if(AB_SaveRecord(&dwoffset,pid,&nGroup,TRUE,nTelID,pItem))
    {
        AB_Insert2Table(dwoffset,*pid,nGroup,pItem);

        Item_Erase(pItem);

        return TRUE;
    }
    else
    {
        Item_Erase(pItem);

        return FALSE;
    }
}
/*********************************************************************\
* Function	   APP_ModifyApi
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL APP_ModifyApi(DWORD id,LPBYTE pData,int nDataLen)
{
    DWORD   dwoffset = -1;
    int     nGroup;
    int     nTelID;
    CONTACT_ITEMCHAIN* pItem;
    int     nCount;
    MEMORY_NAME * p;
    HWND    hViewWnd;

    //p = (MEMORY_NAME*)AB_GetNameString(Memory_Find((PVOID)(id),AB_FIND_ID));
	
	p = Memory_Find((PVOID)(id),AB_FIND_ID);

    if(p == NULL)
        return FALSE;

    pItem = NULL;

    if(AB_GenItembyData(pData,nDataLen,&pItem,&nTelID,&nGroup) == FALSE)
        return FALSE;

    if(AB_SaveRecord(&(p->dwoffset),&id,&nGroup,TRUE,nTelID,pItem))
    {
		AB_NotifyWnd((PVOID)AB_GetIndexByID(pIndexName,id,nName),
			AB_MODE_DELETE,AB_OBJECT_NAME,AB_MDU_CONTRACT);
		
		nCount = AB_DeleteFromIndexByID(pIndexTel,id,nTel,FALSE);
		nTel -= nCount;
		nCount = AB_DeleteFromIndexByID(pIndexEmail,id,nEmail,FALSE);
		nEmail -= nCount;
		nCount = AB_DeleteFromIndexByID(pIndexID,id,nID,FALSE);
		nID -= nCount;
		nCount = AB_DeleteFromIndexByID(pIndexName,id,nName,TRUE);
		nName--;
		
		AB_Insert2Table(p->dwoffset,id,nGroup,pItem);
		
		hViewWnd = FindWindow("ABViewContactWndClass","");
		
		if(hViewWnd)
		{
			ABVIEWCREATEDATA *pCreateData;
			
			pCreateData = (ABVIEWCREATEDATA*)GetUserData(hViewWnd);
			
			if(id == pCreateData->id)
			{
				SendMessage(hViewWnd,WM_CLOSE,0,0);
				
				AB_CreateViewContactWnd(pCreateData->hFrameWnd,pItem,p->dwoffset,p->id,nTelID);
			}
		}
		
		
		
		Item_Erase(pItem);
		
		return TRUE;
    }
    else
    {
        Item_Erase(pItem);

        return FALSE;
    }
}
/*********************************************************************\
* Function	   APP_DeleteApi
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL APP_DeleteApi(DWORD id)
{
    int nCount;
    MEMORY_NAME * p;
    HWND hViewWnd;
    HWND hFrameWnd;

    //p = (MEMORY_NAME*)AB_GetNameString(Memory_Find((PVOID)(id),AB_FIND_ID));

	p = Memory_Find((PVOID)(id),AB_FIND_ID);

	printf("APP_DeleteApi id = %d\r\n", id);

    if(p == NULL)
        return FALSE;

    if(AB_DeleteRecord(p->dwoffset))
    {
        AB_NotifyWnd((PVOID)AB_GetIndexByID(pIndexName,id,nName),
			AB_MODE_DELETE,AB_OBJECT_NAME,AB_MDU_CONTRACT);

        nCount = AB_DeleteFromIndexByID(pIndexTel,id,nTel,FALSE);
        nTel -= nCount;
        nCount = AB_DeleteFromIndexByID(pIndexEmail,id,nEmail,FALSE);
        nEmail -= nCount;
        nCount = AB_DeleteFromIndexByID(pIndexID,id,nID,FALSE);
        nID -= nCount;
        nCount = AB_DeleteFromIndexByID(pIndexName,id,nName,TRUE);
        nName--;

        hFrameWnd = AB_GetFrameWnd();

        if(hFrameWnd != NULL)
        {
            hViewWnd = GetDlgItem(hFrameWnd,IDC_AB_VIEW);
            
            if(hViewWnd)
            {
                ABVIEWCREATEDATA *pCreateData;
                
                pCreateData = (ABVIEWCREATEDATA*)GetUserData(hViewWnd);
                
                if(id == pCreateData->id)
                    PostMessage(hViewWnd,WM_CLOSE,0,0);
            }
        }

        return TRUE;
    }
    return FALSE;
}
