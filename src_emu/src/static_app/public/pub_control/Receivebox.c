/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : control
 *
 * Purpose  : the operations to the data structure about the control receivebox
 *            
\**************************************************************************/

#include "Recipient.h"
#include "sys/stat.h"
/*********************************************************************\
* Function	   PREBROW_InitListBuf
* Purpose     
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
void	RECIPIENT_InitListBuf	(PRECIPIENTLISTBUF pListBuf)
{
	pListBuf->nCurrentIndex	=	-1;
	pListBuf->nDataNum		=	0;
	pListBuf->pCurrentData	=	NULL;
	pListBuf->pDataHead		=	NULL;
}

/*********************************************************************\
* Function	   PREBROW_AddData
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL	RECIPIENT_AddData (PRECIPIENTLISTBUF pListBuf, const char* szData, const char* szFullData, BOOL bExistInAB)
{
	PRECIPIENTLISTNODE	tempdata;
	PRECIPIENTLISTNODE	pTail;

	if (pListBuf->nDataNum == 0)//Initialize the chain
	{
		if ( NULL == ( pListBuf->pDataHead = malloc ( sizeof (RECIPIENTLISTNODE) )) )
			return FALSE;

		pListBuf->pCurrentData	=	pListBuf->pDataHead;

		pListBuf->pCurrentData->nIndex	=	pListBuf->nDataNum;
		pListBuf->pCurrentData->bExistInAB = bExistInAB;
		strcpy( pListBuf->pCurrentData->szShowName, szData );
		strcpy( pListBuf->pCurrentData->szPhoneNum, szFullData );

		pListBuf->pCurrentData->pNext =	pListBuf->pDataHead;
		pListBuf->pCurrentData->pPre  = pListBuf->pDataHead;		


		pListBuf->nCurrentIndex	=	pListBuf->pCurrentData->nIndex;
		pListBuf->nDataNum++;
		return TRUE;
	}
	//add node to the chain
	if (NULL == (tempdata = malloc ( sizeof(RECIPIENTLISTNODE) ) ) )
		return FALSE;	

	tempdata->nIndex = pListBuf->nDataNum;
	strcpy(tempdata->szShowName , szData);
	strcpy(tempdata->szPhoneNum , szFullData);
	tempdata->bExistInAB = bExistInAB;

	pTail = pListBuf->pDataHead->pPre;

	pTail->pNext = tempdata;
	tempdata->pNext = pListBuf->pDataHead;

	tempdata->pPre  = pTail;
	pListBuf->pDataHead->pPre = tempdata;	
	
	pListBuf->nDataNum++;

	return TRUE;
}

/*********************************************************************\
* Function	   PREBROW_GetData
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL	RECIPIENT_GetData (PRECIPIENTLISTBUF pListBuf, int nIndex, char* szGetData, char* szFullData,BOOL bExist)
{
	if ( (pListBuf->pCurrentData = RECIPIENT_GetPDATA ( pListBuf, nIndex )) == NULL)
	{
	
		return FALSE;
	}
	strcpy (szGetData, pListBuf->pCurrentData->szShowName);	
	strcpy (szFullData, pListBuf->pCurrentData->szPhoneNum);
	bExist = pListBuf->pCurrentData->bExistInAB;
	return TRUE;
}


/*********************************************************************\
* Function	   PREBROW_FreeListBuf
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
void	RECIPIENT_FreeListBuf (PRECIPIENTLISTBUF pListBuf)
{
	PRECIPIENTLISTNODE	temp;
	pListBuf->pCurrentData = pListBuf->pDataHead;
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

PRECIPIENTLISTNODE	RECIPIENT_GetPDATA (PRECIPIENTLISTBUF pListBuf, int nIndex )
{	
	PRECIPIENTLISTNODE	pTempData;

	if ( nIndex >= pListBuf->nDataNum || nIndex < 0)
	{
		
		return NULL;
	}
		

	pTempData = pListBuf->pDataHead;
	

	while( pTempData->nIndex != nIndex )
	{
		pTempData = pTempData->pNext;
	}

	return pTempData;
}

/*********************************************************************\
* Function	   PREBROW_DelData
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/

BOOL	RECIPIENT_DelData (PRECIPIENTLISTBUF pListBuf, int nIndex)
{

	PRECIPIENTLISTNODE	pTempData;
	PRECIPIENTLISTNODE	pPre;
	PRECIPIENTLISTNODE	pRepairIndex;
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

		free (pTempData);

		pListBuf->nDataNum --;
		return TRUE;
	}

	pTempData = pListBuf->pDataHead;	
	i	=	0;

	while( i != nIndex )
	{
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

int		RECIPIENT_FindData (PRECIPIENTLISTBUF pListBuf, char* szFindData)
{
	int			i;
	PRECIPIENTLISTNODE	pTempData;

	if ( pListBuf->nDataNum == 0)
		return -1;

	pTempData = pListBuf->pDataHead;	
	i	=	0;

	while( strcmp (pTempData->szShowName, szFindData) != 0  )
	{		
		pTempData = pTempData->pNext;
		i++;
		if ( i >= pListBuf->nDataNum )
			return -1;
	}

	return i;
}


BOOL	RECIPIENT_InsertData (PRECIPIENTLISTBUF pListBuf, int nIndex, PCSTR szData, PCSTR szFullData, BOOL bExistInAB)
{
	PRECIPIENTLISTNODE	pInsertData;
	PRECIPIENTLISTNODE	pRepairIndex;
	PRECIPIENTLISTNODE	pCurData;
	int					i;

	//check up the validity
	if (nIndex > pListBuf->nDataNum || nIndex < 0) {
		return FALSE;
	}
	//insert at the end equal to adding new node
	if (nIndex == pListBuf->nDataNum) {
		return RECIPIENT_AddData(pListBuf, szData, szFullData, bExistInAB);
	}
	//create inserted node 
	if (NULL == (pInsertData = ((PRECIPIENTLISTNODE)malloc(sizeof(RECIPIENTLISTNODE)))))
	{
		return FALSE;
	}
	strcpy(pInsertData->szPhoneNum,szFullData);
	strcpy(pInsertData->szShowName,szData);
	pInsertData->bExistInAB = bExistInAB;
	pInsertData->nIndex	= nIndex;

	
	//get the original node which index equal to nindex;
	pCurData = RECIPIENT_GetPDATA(pListBuf,nIndex);
	
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

BOOL	RECIPIENT_DivideData (PRECIPIENTLISTBUF pListBuf, int nIndex, int nPrelen)
{
	return TRUE;
}

BOOL	RECIPIENT_ModifyData (PRECIPIENTLISTBUF pListBuf, int nIndex,PSTR szShowName, PSTR szFullName, BOOL bAB)
{
	PRECIPIENTLISTNODE	pTempData;
	pTempData = RECIPIENT_GetPDATA(pListBuf,nIndex);
	if (!pTempData)
	{
		return FALSE;
	}
	strcpy(pTempData->szPhoneNum,szFullName);
	strcpy(pTempData->szShowName,szShowName);
	pTempData->bExistInAB = bAB;
	return TRUE;
}

void	UpdateContextWnd(HWND hWnd, PRECIPIENTLISTBUF pListBuf, WNDPROC RecipientProc)
{
	PRECIPIENTLISTNODE	pTempData;
	int		i;
	RecipientProc(hWnd,EM_SETSEL,0,-1);
	RecipientProc(hWnd,WM_CLEAR,0,0);
	if (!pListBuf) {
		return;
	}
	pTempData = pListBuf->pDataHead;
	if (!pTempData) {
		return;
	}
	for (i = pTempData->nIndex; i<pListBuf->nDataNum; i++)
	{
		RecipientProc(hWnd,EM_REPLACESEL,0,(LPARAM)pTempData->szShowName);
		RecipientProc(hWnd, WM_CHAR, ';', 0);
		RecipientProc(hWnd, WM_CHAR, 0x0a0d, 0);
		pTempData = pTempData->pNext;
		if (!pTempData) {
			break;
		}
	}
	InvalidateRect(hWnd,NULL,TRUE);
}

char * plxstrtok (char * string,  const char * control )
{
	unsigned char *str;
	const unsigned char *ctrl = control;
	
	unsigned char map[32];
	int count;
	static char *nextoken;
	
	
	/* Clear control map */
	for (count = 0; count < 32; count++)
		map[count] = 0;
	
	/* Set bits in delimiter table */
	do {
		map[*ctrl >> 3] |= (1 << (*ctrl & 7));
	} while (*ctrl++);
	
	if (string)
		str = string;
	else
		str = nextoken;
	
	while ( (map[*str >> 3] & (1 << (*str & 7))) && *str )
		str++;
	
	string = str;
	
	for ( ; *str ; str++ )
		if ( map[*str >> 3] & (1 << (*str & 7)) ) {
			*str++ = '\0';
			break;
		}
		
        nextoken = str;
		
        if ( string == str )
			return NULL;
        else
			return string;
}
char* plxstrrev(char * string)
{
	char *start = string;
	char *left = string;
	char ch;
	
	while (*string++)                 /* find end of string */
		;
	string -= 2;
	
	while (left < string)
	{
		ch = *left;
		*left++ = *string;
		*string-- = ch;
	}
	
	return(start);
}

BOOL	PLXstrchr(PSTR pSrc, int c)
{
	int	nLen, i;
	if (!pSrc) {
		return FALSE;
	}
	nLen = strlen(pSrc);
	for (i=0; i<nLen; i++)
	{
		if ((int)(BYTE)(*(pSrc+i)) == c) {
			return TRUE;
		}
	}
	return FALSE;
}
/*
 *	如果real 〉1，将实数real的整数部分转为字符串，否则转换小数点后的一位
 */
char *floattoa(float real, char *fstring)
{
	int		integer;
	int		decimal;
	float	decDomain;
	
	char	intstring[10] = "";
	char	decstring[4] = "";

	if (!fstring) {
		return NULL;
	}
	integer = (int)real;
	if (integer >= 1) {
		itoa(integer,intstring, 10);
		sprintf(fstring, "%s", intstring);
		return fstring;
	}
	itoa(integer,intstring, 10);
	decDomain = real - integer;
	decimal = (int)(10*decDomain);
	itoa(decimal,decstring, 10);

	sprintf(fstring, "%s.%s", intstring,decstring);
	 return fstring;
}
float	GetFileSizeByte(PCSTR pFileName)
{
	struct stat	fs;
	memset(&fs, 0, sizeof(struct stat));
	stat(pFileName, &fs);
	return (float)fs.st_size;
}
