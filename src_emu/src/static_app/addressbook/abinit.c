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
#include "ABGlobal.h"

static CONTACT_INITDATA SIMData;

#define AB_OVERTIME             1000

#define AB_MSG_GETSIMPBOOKMEM   WM_USER+120
#define AB_MSG_GETSIMPBOOK      WM_USER+121
#define AB_MSG_GETSIMPBOOKLEN   WM_USER+122

HWND AB_SIMControlWnd(HWND hParent);
static LRESULT CALLBACK ABSIMCtrlWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static BOOL ABSIMCtrl_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct);
static void ABSIMCtrl_OnDestroy(HWND hWnd);
static void ABSIMCtrl_OnTimer(HWND hWnd,UINT id);
static void GetSIMPBook(HWND hWnd,WPARAM wParam,LPARAM lParam);
static void GetSIMPBookMem(HWND hWnd,WPARAM wParam,LPARAM lParam);
static void GetSIMPBookLen(HWND hWnd,WPARAM wParam,LPARAM lParam);
static void AB_InitSIMData(void);
static void DealSIMWrite(HWND hWnd,WPARAM wParam,LPARAM lParam);

void AB_SetSIMData(PCONTACT_INITDATA pData);
PCONTACT_INITDATA AB_GetSIMData(void);
/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL AB_InitSIM(void)
{
    PCONTACT_INITDATA pData;

    AB_InitSIMData();

    pData = AB_GetSIMData();

    pData->hCtrlWnd = AB_SIMControlWnd(NULL);

    if(pData->hCtrlWnd == NULL)
        return FALSE;

    return TRUE;
}
/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
void AB_InitSIMData(void)
{
    memset(&SIMData,0,sizeof(CONTACT_INITDATA));
}
/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
void AB_SetSIMData(PCONTACT_INITDATA pData)
{
    memcpy(&SIMData,pData,sizeof(CONTACT_INITDATA));
}
/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
PCONTACT_INITDATA AB_GetSIMData(void)
{
    return (&SIMData);
}

/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
HWND AB_SIMControlWnd(HWND hParent)
{
	WNDCLASS wc;
    HWND     hSIMWnd;

	wc.style         = 0;
	wc.lpfnWndProc   = ABSIMCtrlWndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = NULL;
	wc.hIcon         = NULL;
	wc.hCursor       = NULL;
	wc.hbrBackground = NULL;
	wc.lpszMenuName  = NULL;
	wc.lpszClassName  = "ABSIMCtrlWndClass";
	
	if (!RegisterClass(&wc))
		return NULL;
	
	hSIMWnd = CreateWindow(
		"ABSIMCtrlWndClass",
		"", 
		NULL, 
		0, 0, 0, 0, 
		hParent,
		NULL, 
		NULL, 
		NULL
		);

    if(hSIMWnd == NULL)
        UnregisterClass("ABSIMCtrlWndClass",NULL);
	
	return hSIMWnd;
}

/*********************************************************************\
* Function	   
* Purpose     
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
LRESULT CALLBACK ABSIMCtrlWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam,LPARAM lParam)
{
	LRESULT lResult;
	lResult = TRUE;
	
	switch (wMsgCmd)
	{
	case WM_CREATE:
        lResult = ABSIMCtrl_OnCreate(hWnd,(LPCREATESTRUCT)(lParam));
		break;

	case WM_TIMER:
		ABSIMCtrl_OnTimer(hWnd,(WPARAM)(UINT)(wParam));
		break;

    case AB_MSG_GETSIMPBOOKMEM:
		GetSIMPBookMem(hWnd,wParam,lParam);
        break;

    case AB_MSG_GETSIMPBOOK:
        GetSIMPBook(hWnd,wParam,lParam);
        break;

    case AB_MSG_GETSIMPBOOKLEN:
        GetSIMPBookLen(hWnd,wParam,lParam);
        break;

    case WM_DESTROY:
        ABSIMCtrl_OnDestroy(hWnd);
        break;

    case AB_MSG_SIM_WRITE:
        DealSIMWrite(hWnd,wParam,lParam);
        break;
		
	default:
		lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
		break;
	}
	
	return lResult;
}

/*********************************************************************\
* Function	ABSIMCtrl_OnCreate
* Purpose   WM_CREATE message handler of the main window
* Params
*			hWnd: Handle of the window
*			lpCreateStruct: Create Structure
* Return
*			TRUE: Success
*			FALSE: Fail
* Remarks
**********************************************************************/
static BOOL ABSIMCtrl_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
{
    if(GetSIMState() == 1)
    {        
        PCONTACT_INITDATA pData;
        
        pData = AB_GetSIMData();
        
        pData->nInit = TIMERID_GET_MEM;

        pData->nErrCtrl = 0;
        
        SetTimer(hWnd,pData->nInit,AB_OVERTIME,NULL);

        return TRUE;
    }

    return FALSE;
}
/*********************************************************************\
* Function	ABSIMCtrl_OnDestroy
* Purpose   WM_DESTROY message handler of the main window
* Params	hWnd:	Handle of the window
* Return	None
* Remarks
**********************************************************************/
static void ABSIMCtrl_OnDestroy(HWND hWnd)
{
    UnregisterClass("ABSIMCtrlWndClass", NULL);
}
/********************************************************************
* Function	   ABSIMCtrl_OnTimer
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void ABSIMCtrl_OnTimer(HWND hWnd,UINT id)
{    
    PCONTACT_INITDATA pData;

	KillTimer(hWnd,id);

	switch(id)
	{
	case TIMERID_GET_MEM:

        pData = AB_GetSIMData();

        if(pData->nErrCtrl > ERR_RETRY)
        {
            pData->nInit = SIMINIT_FAILED;

            return;
        }

        pData->nErrCtrl++;

        if(ME_GetPhonebookMemStatus( hWnd, AB_MSG_GETSIMPBOOKMEM, PBOOK_SM ) < 0)
        {
            pData->nInit = id;

			SetTimer(hWnd,id,AB_OVERTIME,NULL);
        }
        
		break;

	case TIMERID_GET_PHONEBOOK:	
        
        pData = AB_GetSIMData();
        
        if(pData->nErrCtrl > ERR_RETRY)
        {
            pData->nInit = SIMINIT_FAILED;

            return;
        }

        pData->nErrCtrl++;

		if(ME_ReadPhonebook( hWnd, AB_MSG_GETSIMPBOOK, PBOOK_SM, 1, pData->nTotalNum ) < 0)
        {
            SetTimer(hWnd,id,AB_OVERTIME,NULL);
        }
		break;

    case TIMERID_GET_PHONEBOOKLEN:
        
        pData = AB_GetSIMData();
        
        if(pData->nErrCtrl > ERR_RETRY)
        {
            pData->nInit = SIMINIT_FAILED;

            return;
        }

        pData->nErrCtrl++;

		if(ME_GetPhonebookNumberTextMaxLen( hWnd, AB_MSG_GETSIMPBOOKLEN ) < 0)
        {
            SetTimer(hWnd,id,AB_OVERTIME,NULL);
        }
        break;

	case SIMWRITE_OVERTIMER:
		{
			PostMessage(hWnd, AB_MSG_SIM_WRITE_FAIL, NULL, NULL);
		}
		break;

    default:
        break;
	}
}
/********************************************************************
* Function	   GetSIMPBookMem
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void GetSIMPBookMem(HWND hWnd,WPARAM wParam,LPARAM lParam)
{    
    PBMEM_INFO MemInfo;
    
    PCONTACT_INITDATA pData;
    
    pData = AB_GetSIMData();
    
    memset( &MemInfo, 0, sizeof( PBMEM_INFO ));
    
    MemInfo.Type = PBOOK_SM;
    
    if(( wParam != ME_RS_SUCCESS ) || ( ME_GetResult( &MemInfo, sizeof( PBMEM_INFO )) == -1 ))
    {
        SetTimer(hWnd,pData->nInit,AB_OVERTIME,NULL);
        
        return;
    }
    
    pData->nTotalNum = MemInfo.Total;

    pData->nNumber = MemInfo.Used;

    if(pData->nTotalNum == 0)
    {
        pData->nInit = SIMINIT_FAILED;
        
        return;
    }

    pData->pSIMPhoneBook = (PPHONEBOOK)malloc(sizeof(PHONEBOOK)*pData->nTotalNum);

    if(pData->pSIMPhoneBook == NULL)
    {
        pData->nInit = SIMINIT_FAILED;
        
        return;
    }

    memset(pData->pSIMPhoneBook,0,sizeof(PHONEBOOK)*pData->nTotalNum);
    
    pData->nInit = TIMERID_GET_PHONEBOOK;

    SetTimer(hWnd,pData->nInit,AB_OVERTIME,NULL);

    return;
}
/********************************************************************
* Function	   GetSIMPBookMem
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void GetSIMPBook(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
    PCONTACT_INITDATA pData;
    
    pData = AB_GetSIMData();

    if( wParam == ME_RS_SUCCESS )	//
    {        
        pData->nErrCtrl = 0;
        
        ME_GetResult( pData->pSIMPhoneBook, sizeof(PHONEBOOK) * pData->nTotalNum );
        
        pData->nInit = TIMERID_GET_PHONEBOOKLEN;
    }

    SetTimer(hWnd,pData->nInit,AB_OVERTIME,NULL);
}
/********************************************************************
* Function	   GetSIMPBookMem
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void GetSIMPBookLen(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
    PCONTACT_INITDATA pData;
    
    pData = AB_GetSIMData();

    if( wParam == ME_RS_SUCCESS )
    {        
        pData->nErrCtrl = 0;
        
        ME_GetResult( &(pData->MaxLenInfo), sizeof(PBNUMTEXT_INFO));

        pData->MaxLenInfo.iNumberMaxLen *= 2;

        pData->MaxLenInfo.iNumberMaxLen++;
        
        pData->nInit = SIMINIT_SUCCEEDED;

		SetABInitOK(TRUE);
    }
    else
    {
        SetTimer(hWnd,pData->nInit,AB_OVERTIME,NULL);
    }
}
/*********************************************************************\
* Function	   DealSIMWrite
* Purpose      
* Params	   
* Return	      
* Remarks	   
**********************************************************************/
static void DealSIMWrite(HWND hWnd,WPARAM wParam,LPARAM lParam)
{    
    if (wParam == ME_RS_SUCCESS)
    {
        PostMessage(hWnd, AB_MSG_SIM_WRITE_SUCC, NULL, NULL);
    }
    else
    {
        if(lParam == 24)
            PostMessage(hWnd, AB_MSG_SIM_WRITE_FULL, NULL, NULL);
        else
            PostMessage(hWnd, AB_MSG_SIM_WRITE_FAIL, NULL, NULL);
    }
}

/*********************************************************************\
* Function	   AB_UserListSM
* Purpose      
* Params	   
* Return	      
* Remarks	   
**********************************************************************/
BOOL AB_UserListSM(void)
{
    PCONTACT_INITDATA pData;

    pData = AB_GetSIMData();
    
    pData->nInit = TIMERID_GET_MEM;
    
    pData->nErrCtrl = 0;
    
    SetTimer(pData->hCtrlWnd,pData->nInit,AB_OVERTIME,NULL);
    
    return TRUE;
}
