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

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <types.h>
#include <stat.h>
#include <unistd.h>
#include <vfs.h>
#include <fcntl.h>
#include <hpdef.h>

#include "exch_global.h"
#include "exch_file.h"

extern EXCHMONEYINFONODE *pExchNodeHead;
extern EXCHMONEYINFONODE *pExchNodeEnd;
extern EXCHMONEYINFONODE *pExchNodeCur;
extern EXCHMONEYINFONODE *pExchNodeFirst;
extern EXCHMONEYINFONODE *pExchNodeSecond;
extern EXCHMONEYINFONODE *pExchNodeBase;

extern char cExch_Value1[VALUE_MAX_LENGTH +1];
extern char cExch_Value2[VALUE_MAX_LENGTH+1];

const char ExchName[TOP_REC_NUMBER][CURRENCY_MAX_LENGTH +1] = 
{
	("DKK"),//µ¤Âó¿ËÀÊ
	("EUR"),//Å·Ôª
	("GBP"),//Ó¢°÷
	("NOK"),//Å²Íþ¿ËÀÊ
	("SEK"),//Èðµä¿ËÀÊ
    ("USD")//ÃÀÔª
};

const double dRMBRate[TOP_REC_NUMBER] =
{
	0,
	1.0,
	0,
	0,
	0,
	0
};

int nCurrencyNum = 0;
/**********************************************************************
* Function	Exch_InitRateFile
* Purpose  
* Params	
* Return	
* Remarks
**********************************************************************/
int Exch_InitRateFile(void)
{
	EXCHMONEY money[TOP_REC_NUMBER];
	int i;
	int hFile;
	
	chdir(EXCH_FLASH_PATH);
 	if ((hFile = open(EXCH_RATE_FILE, O_RDWR | O_CREAT, 0)) == INVALID_HANDLE_VALUE)
		return 0;
	for(i =0; i < TOP_REC_NUMBER; i++)
	{
		strcpy(money[i].name, ML(ExchName[i]));
		
		if (0 == strcmp((char *)Default_Currecy1, money[i].name))
			money[i].nDefault = 1;
		else
		{
			if (0 == strcmp((char *)Default_Currecy2, money[i].name))
				money[i].nDefault = 2;
			else
				money[i].nDefault = 0;
		}
		
		money[i].rate = dRMBRate[i];
		if (money[i].rate > 0)
			money[i].bBase = TRUE;
		else
			money[i].bBase = FALSE;
	
		if (write(hFile, &money[i], sizeof(EXCHMONEY)) != sizeof(EXCHMONEY))
		{
			close(hFile);
			return 0;
		}
	}
	

	close(hFile);
	return 1;
}
/**********************************************************************
* Function	Exch_InitValueFile
* Purpose  
* Params	
* Return	
* Remarks
**********************************************************************/
int Exch_InitValueFile()
{
	int hFile;

	chdir(EXCH_FLASH_PATH);
	if ((hFile = open(EXCH_VALUE_FILE, O_RDWR | O_CREAT, 0)) == INVALID_HANDLE_VALUE)
		return 0;

	memset(cExch_Value1, 0, VALUE_MAX_LENGTH +1);
	memset(cExch_Value2, 0, VALUE_MAX_LENGTH +1);
	
	strcpy(cExch_Value1, "0");
	strcpy(cExch_Value2, "0");

	if (write(hFile, cExch_Value1, VALUE_MAX_LENGTH +1) != VALUE_MAX_LENGTH +1)
	{
		close(hFile);
		return 0;
	}
	if (write(hFile, cExch_Value2, VALUE_MAX_LENGTH +1) != VALUE_MAX_LENGTH +1)
	{
		close(hFile);
		return 0;
	}
	
	close(hFile);
	return 1;
}
/**********************************************************************
* Function	Exch_GetValueFromFile
* Purpose  
* Params	
* Return	
* Remarks
**********************************************************************/
int Exch_GetValueFromFile()
{
	int hFile;
	int iReadSize;
	
	chdir(EXCH_FLASH_PATH);
	if ((hFile = open(EXCH_VALUE_FILE, O_RDONLY, 0)) == INVALID_HANDLE_VALUE)
		return 0; 
 
	iReadSize = read(hFile, cExch_Value1, VALUE_MAX_LENGTH +1);
	if(-1 == iReadSize)
	{
		close(hFile);
		return 0;
	}

	iReadSize = read(hFile, cExch_Value2, VALUE_MAX_LENGTH +1);
	if(-1 == iReadSize)
	{
		close(hFile);
		return 0;
	}
	
	close(hFile);
	return 1;
}

/**********************************************************************
* Function	Exch_SaveTwoValueToFile
* Purpose  
* Params	
* Return	
* Remarks
**********************************************************************/
BOOL Exch_SaveTwoValueToFile()
{
	int hFile;

	chdir(EXCH_FLASH_PATH);

	if(-1==truncate(EXCH_VALUE_FILE,0))
		return FALSE;

	if ((hFile = open(EXCH_VALUE_FILE, O_RDWR, 0)) == INVALID_HANDLE_VALUE)
		return FALSE;

	if (write(hFile, cExch_Value1, VALUE_MAX_LENGTH +1) != VALUE_MAX_LENGTH +1)
	{
		close(hFile);
		return FALSE;
	}
	if (write(hFile, cExch_Value2, VALUE_MAX_LENGTH +1) != VALUE_MAX_LENGTH +1)
	{
		close(hFile);
		return FALSE;
	}
	
	close(hFile);

	return TRUE;

}



/**********************************************************************
* Function	Exch_GetAllNodeFromFile
* Purpose  
* Params	
* Return	
* Remarks
**********************************************************************/
int Exch_GetAllNodeFromFile(void)
{
	int hFile;
	EXCHMONEY money;
	int iReadSize;
	
	chdir(EXCH_FLASH_PATH);
	if ((hFile = open(EXCH_RATE_FILE, O_RDONLY, 0)) == INVALID_HANDLE_VALUE)
		return 0; 
	
	while(1)
	{
		iReadSize = read(hFile, &money, ExchMoneySize);
		if(-1 == iReadSize)
		{
			close(hFile);
			return 0;
		}
		if ( 0 == iReadSize) 
		{
			if (pExchNodeFirst != NULL && pExchNodeSecond == NULL)
				pExchNodeSecond = pExchNodeFirst;

			if (pExchNodeFirst == NULL && pExchNodeSecond != NULL)
				pExchNodeFirst = pExchNodeSecond;

			if (pExchNodeFirst == NULL && pExchNodeSecond == NULL)
			{
				pExchNodeFirst = pExchNodeHead;
				pExchNodeSecond = pExchNodeHead;
			}

			close(hFile);
			return 1;
		}

		if(!Exch_AddNode(&money))
		{
			close(hFile);
			return 0;
		}
	}
	

	close(hFile);

	return 1;
}
/**********************************************************************
* Function	Exch_AddNode
* Purpose  
* Params	
* Return	
* Remarks
**********************************************************************/
int Exch_AddNode(EXCHMONEY *pMoney)
{
	EXCHMONEYINFONODE *pNode;

	pNode = (EXCHMONEYINFONODE *)malloc(ExchNodeSize);
	if(NULL == pNode)
		return 0;
	memcpy(&pNode->ExchMoneyInfo, pMoney, ExchMoneySize);
	if(NULL == pExchNodeHead)
	{	
		pExchNodeHead = pExchNodeEnd = pNode;
		pNode->pPreInfo = NULL;
		pNode->pNextInfo = NULL;
	}
	else
	{
		pNode->pNextInfo = NULL;
		pNode->pPreInfo = pExchNodeEnd;
		pExchNodeEnd->pNextInfo = pNode;
		pExchNodeEnd = pNode;
	}
	if (1 == pMoney->nDefault)
		pExchNodeFirst = pNode;
	if (2 == pMoney->nDefault)
		pExchNodeSecond = pNode;
	if (pMoney->bBase)
		pExchNodeBase = pNode;
	nCurrencyNum++;
	return 1;
}
/**********************************************************************
* Function	DeleteNode
* Purpose  
* Params	
* Return	
* Remarks
**********************************************************************/
int Exch_DeleteNode(EXCHMONEYINFONODE *pNode)
{
	EXCHMONEYINFONODE *pNodBefore, *pNodeBehind;

	pNodBefore = pNode->pPreInfo;
	pNodeBehind = pNode->pNextInfo;
    if (NULL != pNodBefore)
		pNodBefore->pNextInfo = pNodeBehind;
	else
		pExchNodeHead = pNodeBehind;
	if (NULL != pNodeBehind)
		pNodeBehind->pPreInfo = pNodBefore;
	else
		pExchNodeEnd = pNodBefore;
	 
	nCurrencyNum --;
	free(pNode);
	pNode = NULL;
	return 1;

}
/**********************************************************************
* Function	Exch_FindNode
* Purpose  
* Params	
* Return	
* Remarks
**********************************************************************/
EXCHMONEYINFONODE *Exch_FindNode(char *pName)
{
	EXCHMONEYINFONODE *pNode;
	
	pNode = pExchNodeHead;
	while (pNode != NULL) 
	{
		if (0 == strcmp(pNode->ExchMoneyInfo.name, pName))
			return pNode;
		pNode = pNode->pNextInfo;
	}
	return NULL;
}
/**********************************************************************
* Function	Exch_EditNode
* Purpose  
* Params	
* Return	
* Remarks
**********************************************************************/
int Exch_EditNode(EXCHMONEY *pWillEdit,  EXCHMONEY *pHasEdit)
{
	EXCHMONEYINFONODE *pNode = NULL;

	pNode = Exch_FindNode(pWillEdit->name);
	if(NULL == pNode)
		return 0;
	strcpy(pNode->ExchMoneyInfo.name, pHasEdit->name);
	pNode->ExchMoneyInfo.rate = pHasEdit->rate;
	return 1;
}
/**********************************************************************
* Function	MovePoint
* Purpose  
* Params	
* Return	
* Remarks
**********************************************************************/
void Exch_LocatePnter(int Index)
{
	EXCHMONEYINFONODE *pNode = NULL;
	int i;

	pNode = pExchNodeHead;
	
	for(i = 0; i < Index; i++)
		pNode = pNode->pNextInfo;
	pExchNodeCur = pNode;
}
/**********************************************************************
* Function	Exch_SaveAllNodeToFile
* Purpose  
* Params	
* Return	
* Remarks
**********************************************************************/
BOOL Exch_SaveAllNodeToFile(void)
{
	EXCHMONEYINFONODE *pNode;
	int hFile;

	if (NULL == pExchNodeHead)
		return 0;
	pNode = pExchNodeHead;

	chdir(EXCH_FLASH_PATH);
	if(-1==truncate(EXCH_RATE_FILE,0))
		return 0;
	if ((hFile = open(EXCH_RATE_FILE, O_RDWR, 0)) == INVALID_HANDLE_VALUE)
		return 0;	
	while (pNode != NULL) 
	{
		if(write(hFile, &pNode->ExchMoneyInfo, ExchMoneySize) == !ExchMoneySize)
		{
			close(hFile);
			return 0;
		}
		pNode = pNode->pNextInfo;
	}
	 
	close(hFile);
	return 1;
}
/**********************************************************************
* Function	Exch_DeleteAllNodeToFile
* Purpose  
* Params	
* Return	
* Remarks
**********************************************************************/
BOOL Exch_DeleteAllNodeToFile()
{
	EXCHMONEYINFONODE *pNode1, *pNode2;
	int hFile;

	
	if (NULL == pExchNodeHead)
		return 0;
	pNode1 = pExchNodeHead;
	pNode2 = pExchNodeHead;

	chdir(EXCH_FLASH_PATH);

	if(-1==truncate(EXCH_RATE_FILE,0))
		return 0;

		
	if ((hFile = open(EXCH_RATE_FILE, O_RDWR, 0)) == INVALID_HANDLE_VALUE)
		return 0;
	while (pNode1 != NULL) 
	{
		pNode2 = pNode1->pNextInfo;
		if (pNode1->ExchMoneyInfo.bBase)
		{
			write(hFile, &pNode1->ExchMoneyInfo, ExchMoneySize);
			pExchNodeHead = pNode1;
			pExchNodeEnd = pNode1;
			pExchNodeFirst = pNode1;
			pExchNodeSecond = pNode1;
			pExchNodeBase = pNode1;
			pNode1->pPreInfo = NULL;
			pNode1->pNextInfo = NULL;
		}
		else
			free(pNode1);
		pNode1 = pNode2;		
	}
	
	nCurrencyNum = 1;
	close(hFile);
	return 1;
}
