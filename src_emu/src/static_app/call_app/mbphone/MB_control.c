#include    "winpda.h"
#include    "string.h"
#include     "stdlib.h"
#include    "mullang.h"
#include    "setting.h"
#include    "plx_pdaex.h"
#include    "pubapp.h"
#include    "MB_control.h"

/* call window order  */
typedef struct callwnd_order {
	HWND hPreWnd;
	HWND hCurWnd;
} CALL_WND_ORDER;

static CALL_WND_ORDER  gCallWndOrder;
static HINSTANCE hInstance;
static int wndnum;


/*********************************************************************\
* Function	   
* Purpose     
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
void	CALLWND_InitListBuf	(PCALLWNDLISTBUF pListBuf)
{
	pListBuf->nCurrentIndex	=	-1;
	pListBuf->nDataNum		=	0;
	pListBuf->pCurrentData	=	NULL;
	pListBuf->pDataHead		=	NULL;
}

/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL	CALLWND_AddData (PCALLWNDLISTBUF pListBuf, HWND	hwnd, const char* szClassName)
{
	PCALLWNDLISTNODE	tempdata;
	PCALLWNDLISTNODE	pTail;

	if (pListBuf->nDataNum == 0)//Initialize the chain
	{
		if ( NULL == ( pListBuf->pDataHead = malloc ( sizeof (CALLWNDLISTNODE) )) )
			return FALSE;

		pListBuf->pCurrentData	=	pListBuf->pDataHead;

		pListBuf->pCurrentData->nIndex	=	pListBuf->nDataNum;
		pListBuf->pCurrentData->hwnd	= hwnd;
		strcpy( pListBuf->pCurrentData->pClassName, szClassName );

		pListBuf->pCurrentData->pNext =	pListBuf->pDataHead;
		pListBuf->pCurrentData->pPre  = pListBuf->pDataHead;		


		pListBuf->nCurrentIndex	=	pListBuf->pCurrentData->nIndex;
		pListBuf->nDataNum++;
		return TRUE;
	}
	//add node to the chain
	if (NULL == (tempdata = malloc ( sizeof(CALLWNDLISTNODE) ) ) )
		return FALSE;	

	tempdata->nIndex = pListBuf->nDataNum;
	strcpy(tempdata->pClassName , szClassName);
	
	pTail = pListBuf->pDataHead->pPre;

	pTail->pNext = tempdata;
	tempdata->pNext = pListBuf->pDataHead;

	tempdata->pPre  = pTail;
	pListBuf->pDataHead->pPre = tempdata;	
	
	pListBuf->nCurrentIndex = tempdata->nIndex;
	pListBuf->pCurrentData = tempdata;
	pListBuf->nDataNum++;

	return TRUE;
}


/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
void	CALLWND_FreeListBuf (PCALLWNDLISTBUF pListBuf)
{
	PCALLWNDLISTNODE	temp;

	while ( pListBuf->pCurrentData != NULL )
	{
		pListBuf->pCurrentData->pPre->pNext = NULL;
		temp = pListBuf->pCurrentData;
		pListBuf->pCurrentData = pListBuf->pCurrentData->pNext;		
		free ( temp );
	}

	pListBuf->nCurrentIndex	=	-1;
	pListBuf->nDataNum		=	0;
	pListBuf->pDataHead		=	NULL;
}

PCALLWNDLISTNODE	CALLWND_GetPDATA (PCALLWNDLISTBUF pListBuf, int nIndex )
{	
	PCALLWNDLISTNODE	pTempData;
	int			i;

	if ( nIndex > pListBuf->nDataNum || nIndex < 0)
	{
		
		return NULL;
	}
		

	pTempData = pListBuf->pDataHead;
	i	=	0;

	while( i != nIndex )
	{
		pTempData = pTempData->pNext;
		i ++;
	}

	return pTempData;
}

/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/

BOOL	CALLWND_DelData (PCALLWNDLISTBUF pListBuf, int nIndex)
{

	PCALLWNDLISTNODE	pTempData;
	PCALLWNDLISTNODE	pPre;
	PCALLWNDLISTNODE	pRepairIndex;
	int			i;

	if ( nIndex > pListBuf->nDataNum )
		return FALSE;

	if ( pListBuf->nDataNum == 1)
	{
		free (pListBuf->pDataHead );
		pListBuf->pCurrentData = NULL;
		pListBuf->pDataHead = NULL;
		pListBuf->nDataNum = 0;
		return TRUE;
	}

	if (nIndex == 0)
	{
		pTempData = pListBuf->pDataHead;
		pPre	  = pTempData->pPre;

		pPre->pNext = pTempData->pNext;
		pTempData->pNext->pPre = pPre;

		pRepairIndex = pListBuf->pDataHead = pTempData->pNext;
		//repair the index
		for (i = pRepairIndex->nIndex; i<pListBuf->nDataNum; i++)
		{
			pRepairIndex->nIndex--;
			pRepairIndex = pRepairIndex->pNext;
		}
		if (pTempData == pListBuf->pCurrentData) {
			pListBuf->pCurrentData = pListBuf->pCurrentData->pNext;
		}
		free (pTempData);

		pListBuf->nDataNum --;
		return TRUE;
	}

	pTempData = pListBuf->pDataHead;	
	i	=	0;

	while( i != nIndex )
	{
        if (pTempData == NULL)
            return TRUE;
		pTempData = pTempData->pNext;
		i++;
	}

	pPre = pTempData->pPre;

	pPre->pNext = pTempData->pNext;
	pTempData->pNext->pPre = pPre;
	//rebuild the index of the chain
	//if the Node is the last,need not to repair index
	if (nIndex != (pListBuf->nDataNum - 1))
	{
		pRepairIndex = pTempData->pNext;
		for (i = pRepairIndex->nIndex; i<pListBuf->nDataNum; i++)
		{
			pRepairIndex->nIndex--;
			pRepairIndex = pRepairIndex->pNext;
		}
	}
	if (pTempData == pListBuf->pCurrentData) {
		pListBuf->pCurrentData = pListBuf->pCurrentData->pNext;
	}
	free(pTempData);
	pListBuf->nDataNum --;

	return TRUE;
}

/*********************************************************************\
* Function	   PREBROW_FindData
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/

int		CALLWND_FindData (PCALLWNDLISTBUF pListBuf, HWND hWnd)
{
	int			i;
	PCALLWNDLISTNODE	pTempData;

	if ( pListBuf->nDataNum == 0)
		return -1;

	pTempData = pListBuf->pDataHead;	
	i	=	0;

	while( pTempData->hwnd != hWnd )
	{		
		pTempData = pTempData->pNext;
		i++;
		if ( i >= pListBuf->nDataNum )
			return -1;
	}

	return i;
}


BOOL	CALLWND_InsertData (PCALLWNDLISTBUF pListBuf, int nIndex, PCSTR szClassName, HWND hwnd)
{
	PCALLWNDLISTNODE	pInsertData;
	PCALLWNDLISTNODE	pRepairIndex;
	PCALLWNDLISTNODE	pCurData;
	int					i;

	if(nIndex > pListBuf->nDataNum)
	{
		return FALSE;
	}
	if(nIndex == pListBuf->nDataNum)
	{
		return CALLWND_AddData ( pListBuf, hwnd,  szClassName);
	}
	//create inserted node 
	if (NULL == (pInsertData = ((PCALLWNDLISTNODE)malloc(sizeof(CALLWNDLISTNODE)))))
	{
		return FALSE;
	}
	strcpy(pInsertData->pClassName,szClassName);
	pInsertData->nIndex	= nIndex;
	pInsertData->hwnd = hwnd;
	
	
	
	//get the original node which index equal to nindex;
	pCurData = CALLWND_GetPDATA(pListBuf,nIndex);
	
	
	//insert node 
	pCurData->pPre->pNext = pInsertData;
	pInsertData->pPre = pCurData->pPre;
	pCurData->pPre = pInsertData;
	pInsertData->pNext = pCurData;

	if (nIndex == 0)
	{
		pListBuf->pDataHead = pInsertData;		
	}
	//repair index
	pRepairIndex = pCurData;
	for (i = pCurData->nIndex; i<pListBuf->nDataNum; i++)
	{
		pRepairIndex->nIndex++;
		pRepairIndex = pRepairIndex->pNext;
		if (pRepairIndex == pListBuf->pDataHead)
		{
			break;
		}
	}
	pListBuf->nDataNum++;
	return TRUE;
}

BOOL	CALLWND_InsertDataByHandle(PCALLWNDLISTBUF pListBuf, HWND hWnd)
{
	int	i;
	char	classname[50];
	i = CALLWND_FindData ( pListBuf,  hWnd);
	if(-1 == i)
	{
		GetClassName(hWnd, classname, 50);
		//return CALLWND_AddData(pListBuf, hWnd, classname);
		return CALLWND_InsertData (pListBuf, 0, classname, hWnd);
	}
	else
	{
		CALLWND_DelData (pListBuf, i);
		//return CALLWND_AddData(pListBuf, hWnd, classname);
		return CALLWND_InsertData (pListBuf, 0, classname, hWnd);
	}
	
}

BOOL	CALLWND_DeleteDataByHandle(PCALLWNDLISTBUF pListBuf, HWND hWnd)
{
	int	i;
	i = CALLWND_FindData ( pListBuf,  hWnd);
	return CALLWND_DelData (pListBuf, i);
}

DWORD   MBDial_AppControl (int nCode, void* pInstance, WPARAM wParam, LPARAM lParam )
{
//    WNDCLASS wc;
    DWORD dwRet = TRUE;
	int i;
	PCALLWNDLISTNODE pshowwndnode;

    switch (nCode)
    {
    case APP_INIT :
        hInstance = pInstance;
//		memset(&gCallWndOrder, 0, sizeof(gCallWndOrder));
//		pGlobalCallListBuf = malloc(sizeof(CALLWNDLISTBUF));
//		CALLWND_InitListBuf(pGlobalCallListBuf);
        break;
		
    case APP_ACTIVE :
		PMShowIdle();

		wndnum = pGlobalCallListBuf->nDataNum;
		for(i = wndnum-1; i >=0; i--)
		{
			pshowwndnode = CALLWND_GetPDATA(pGlobalCallListBuf, i);
			if (IsWindow(pshowwndnode->hwnd))
			{
				ShowWindow(pshowwndnode->hwnd, SW_SHOW);
				ShowOwnedPopups(pshowwndnode->hwnd, SW_SHOW);			
			}
		}
        break;
		
    case APP_DESTROY:
        PrioMan_EndCallMusic(PRIOMAN_PRIORITY_PHONE,TRUE);
        break;

    case APP_INACTIVE :
		PMHideIdle();
		wndnum = pGlobalCallListBuf->nDataNum;
		for(i = 0; i < wndnum; i++)
		{
			pshowwndnode = CALLWND_GetPDATA(pGlobalCallListBuf, i);
			if (IsWindow(pshowwndnode->hwnd))
			{
                //if (!IsCallingWnd())
                {
                    ShowOwnedPopups(pshowwndnode->hwnd, SW_HIDE);		
				    ShowWindow(pshowwndnode->hwnd, SW_HIDE);	
                }
			}
		}
/*
		if (IsWindow(gCallWndOrder.hPreWnd))
		{
			ShowOwnedPopups(gCallWndOrder.hPreWnd, SW_HIDE);
			ShowWindow(gCallWndOrder.hPreWnd, SW_HIDE);
		}
		if (IsWindow(gCallWndOrder.hCurWnd))
		{
			ShowOwnedPopups(gCallWndOrder.hCurWnd, SW_HIDE);
			ShowWindow(gCallWndOrder.hCurWnd, SW_HIDE);
		}*/

        break;
		
    default :
        break;
    }
    return dwRet;
}

void Set_CallWndOrder(HWND hpre, HWND hcur)
{
	gCallWndOrder.hPreWnd = hpre;
	gCallWndOrder.hCurWnd = hcur;
}
HINSTANCE ReturnHinstance(void)//返回电话实例
{
    return hInstance;
}
