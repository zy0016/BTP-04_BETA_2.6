/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : StkAT.c
 *
 * Purpose  : 
 *
\**************************************************************************/

#include "window.h"
#include "malloc.h"
#include "string.h"
#include "stdio.h"

#include "plx_pdaex.h"
#include "pubapp.h"
#include "winpda.h"

#include "me_wnd.h"
#include "assert.h"

#include "StkMain.h"
#include "StkRs.h"
#include "StkAT.h"

///////////////////////////////////////////////////////////////
extern HWND hStkWndApp;
extern STK_PARAM STK_SendStruct;
extern SETUPMENU SetupMenu;
extern SELECTITEM SelectItem;
extern MENUITEM SetupMenuItem[STK_MAC_MENUCOUNT], SelectMenuItem[STK_MAC_MENUCOUNT];

extern REFRESH Refresh;
extern SETUPEVENTLIST SetupEventList;
extern SETUPCALL SetupCall;
extern SENDSS SendSS;
extern SENDUSSD SendUSSD;
extern SENDSHORTMESSAGE SendShortMessage;
extern SENDDTMF SendDTMF;
extern PLAYTONE PlayTone;
extern DISPLAYTEXT DisplayText;
extern GETINKEY GetInkey;
extern GETINPUT GetInput;
extern SETUPIDLEMODETEXT SetupIdleModeText;

///////////////////////////////////////////////////////////////
static BYTE STK_Convert_Hex2BYTE(char SrcHex);
static char STK_Convert_BYTE2Hex(BYTE SrcBYTE);

static int  STK_Proat_Refresh(char* pContent, PREFRESH pRefresh);
static int  STK_Proat_SetupEventList(char* pContent, PSETUPEVENTLIST pSetupEventList);
static int  STK_Proat_SetupCall(char* pContent, PSETUPCALL pSetupCall);
static int  STK_Proat_SendSS(char* pContent, PSENDSS pSendSS);
static int  STK_Proat_SendUSSD(char* pContent, PSENDUSSD pSendUSSD);
static int  STK_Proat_SendShortMessage(char* pContent, PSENDSHORTMESSAGE pSendShortMessage);
static int  STK_Proat_SendDTMF(char* pContent, PSENDDTMF pSendDTMF);
static int  STK_Proat_PlayTone(char* pContent, PPLAYTONE pPlayTone);
static int  STK_Proat_DisplayText(char* pContent, PDISPLAYTEXT pDisplayText);
static int  STK_Proat_GetInkey(char* pContent, PGETINKEY pGetInkey);
static int  STK_Proat_GetInput(char* pContent, PGETINPUT pGetInput);
static int  STK_Proat_SetupIdleModeText(char* pContent, PSETUPIDLEMODETEXT pSetupIdleModeText);

/*********************************************************************
* Function	STK_Convert_Hex2String
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
int STK_Convert_Hex2String(char *SrcStr, BYTE *DestStr, int ScrLen, int DestLen)
{
	int i, j;

	for (i=0,j=0; i<ScrLen&&j<DestLen; i+=2,j++)
	{
		DestStr[j] = STK_Convert_Hex2BYTE(*(SrcStr+i))<<4 | STK_Convert_Hex2BYTE(*(SrcStr+i+1));
	}
	DestStr[j] = 0;

	return j;
}

/*********************************************************************
* Function	STK_Convert_String2Hex
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
int STK_Convert_String2Hex(BYTE *SrcStr, char *DestStr, int ScrLen, int DestLen)
{
	int i, j;

	for (i=0,j=0; i<ScrLen&&j<=DestLen-2; i++,j+=2)
	{
		DestStr[j] = STK_Convert_BYTE2Hex((BYTE)(SrcStr[i]>>4));
		DestStr[j+1] = STK_Convert_BYTE2Hex((BYTE)(SrcStr[i]&0x0F));
	}
	DestStr[j] = 0;

	return j;
}

/*********************************************************************
* Function	STK_Convert_Hex2BYTE
* Purpose   
* Parameter	
* Return	BYTE
* Remarks	
**********************************************************************/
static BYTE STK_Convert_Hex2BYTE(char SrcHex)
{
	if (SrcHex>='0' && SrcHex<='9')
		return SrcHex-'0';
	else if (SrcHex>='A' && SrcHex<='F')
		return SrcHex-'A'+10;
	else if (SrcHex>='a' && SrcHex<='f')
		return SrcHex-'a'+10;

	return 0;
}

/*********************************************************************
* Function	STK_Convert_BYTE2Hex
* Purpose   
* Parameter	
* Return	char
* Remarks	
**********************************************************************/
static char STK_Convert_BYTE2Hex(BYTE SrcBYTE)
{
	if (SrcBYTE <= 0x09) // SrcBYTE >= 0x00
		return SrcBYTE+'0';
	else if (SrcBYTE>=0x0A && SrcBYTE<=0x0F)
		return SrcBYTE-10+'A';

	return 0;
}

/*********************************************************************
* Function	STK_Search_Integer
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
int STK_Search_Integer(char *SrcStr, int *DestInt, BOOL First)
{
	int i, Count;
	char *pBegin;
	char CodeInt[STK_LEN_INTEGER+1];

	if (First)
	{
		if ((pBegin = strstr(SrcStr, ":" )) == NULL)
			return -1;
		pBegin += 1;
	}
	else
		pBegin = SrcStr;

	for (i=0; i<STK_LEN_INTEGER; i++)
	{
		if (*(pBegin+i) == ',' || *(pBegin+i) == 0)
		{
			Count = pBegin+i-SrcStr;
			if (*(pBegin+i) == ',')
				Count += 1;
			break;
		}
		CodeInt[i] = *(pBegin+i);
	}

	if (i >= STK_LEN_INTEGER)
		return -1;

	CodeInt[i] = 0;
	*DestInt = atoi(CodeInt);

	return Count;
}

/*********************************************************************
* Function	STK_Search_String
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
int STK_Search_String(char *SrcStr, BYTE *DestStr)
{
	int Count;
	int Len, cpyLen;
	char *pBegin, *pEnd;
	char MidStr[STK_LEN_STRING*2+1];

	if ((pBegin = strstr(SrcStr, "\"")) == NULL)
	{
		DestStr[0] = 0;
		return -1;
	}

	if ((pEnd = strstr(pBegin+1, "\"")) == NULL)
	{
		DestStr[0] = 0;
		return -1;
	}

	Len = pEnd-pBegin-1;
	cpyLen = (Len <= STK_LEN_STRING*2) ? Len : (STK_LEN_STRING*2);

	memcpy(MidStr, pBegin+1, cpyLen);
	MidStr[cpyLen] = 0;

	Count = pEnd-SrcStr+1;
	if (*(pEnd+1) == ',')
		Count += 1;

	STK_Convert_Hex2String(MidStr, DestStr, cpyLen, cpyLen/2);
	return Count;
}

/*********************************************************************
* Function	STK_InvertOrder
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
int STK_InvertOrder(char *Str, int Len)
{
	int i;
	char c1, c2;

	for (i=0; i<Len; i+=2)
	{
		c1 = *(Str+i);
		c2 = *(Str+i+1);
		*(Str+i) = c2;
		*(Str+i+1) = c1;
	}

	return 0;
}

/*********************************************************************
* Function	STK_Proat_Malloc
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
int STK_Proat_Malloc(void)
{
	int i;

	SetupMenu.titleText = (char *)malloc(STK_MAC_MENUCOMMAND);
	SelectItem.titleText = (char *)malloc(STK_MAC_MENUCOMMAND);
	for (i=0; i<STK_MAC_MENUCOUNT; i++)
	{
		SetupMenuItem[i].itemText = (char *)malloc(STK_MAC_MENUCOMMAND);
		SelectMenuItem[i].itemText = (char *)malloc(STK_MAC_MENUCOMMAND);
	}

	SetupCall.confirmationText = (char *)malloc(STK_MAC_STRINGCOMMAND);
	SetupCall.calledNumber = (char *)malloc(STK_MAC_STRINGCOMMAND);
	SetupCall.callSetupText = (char *)malloc(STK_MAC_STRINGCOMMAND);
	SendSS.text = (char *)malloc(STK_MAC_STRINGCOMMAND);
	SendUSSD.text = (char *)malloc(STK_MAC_STRINGCOMMAND);
	SendShortMessage.textInfo = (char *)malloc(STK_MAC_STRINGCOMMAND);
	SendDTMF.text = (char *)malloc(STK_MAC_STRINGCOMMAND);
	PlayTone.infoText = (char *)malloc(STK_MAC_STRINGCOMMAND);
	DisplayText.text = (char *)malloc(STK_MAC_STRINGCOMMAND);
	GetInkey.text = (char *)malloc(STK_MAC_STRINGCOMMAND);
	GetInput.text = (char *)malloc(STK_MAC_STRINGCOMMAND);
	GetInput.defaultText = (char *)malloc(STK_MAC_STRINGCOMMAND);
	SetupIdleModeText.text = (char *)malloc(STK_MAC_STRINGCOMMAND);

	return 0;
}

/*********************************************************************
* Function	STK_Proat_Free
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
int STK_Proat_Free(void)
{
	int i;

	free(SetupMenu.titleText);
	free(SelectItem.titleText);
	for (i=0; i<STK_MAC_MENUCOUNT; i++)
	{
		free(SetupMenuItem[i].itemText);
		free(SelectMenuItem[i].itemText);
	}

	free(SetupCall.confirmationText);
	free(SetupCall.calledNumber);
	free(SetupCall.callSetupText);
	free(SendSS.text);
	free(SendUSSD.text);
	free(SendShortMessage.textInfo);
	free(SendDTMF.text);
	free(PlayTone.infoText);
	free(DisplayText.text);
	free(GetInkey.text);
	free(GetInput.text);
	free(GetInput.defaultText);
	free(SetupIdleModeText.text);

	return 0;
}

/*********************************************************************
* Function	STK_Proat_SetupMenu
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
// ^SSTGI: <cmdType>, <commandDetails>, <numOfItems>, <titleText>,
// <menuItemIconsPresent>, <menuItemIconsQualifier>, <titleIconQualifier>,
// <titleIconId> <CR> <LF>
// ^SSTGI: <cmdType>, <itemId>, <itemText>, <nextActionId>, <iconId> <CR>
// <LF>
int STK_Proat_SetupMenu(char ppContent[STK_MAC_MENUCOUNT][STK_MAC_MENUCOMMAND], PSETUPMENU pSetupMenu, MENUITEM pSetupMenuItem[STK_MAC_MENUCOUNT])
{
	int i, cmdType, commandDetails, numOfItems, itemId, nextActionId, offset;
	int menuItemIconsPresent, menuItemIconsQualifier;
	int titleIconQualifier, titleIconId;
	int iconId;
	BYTE titleText[STK_LEN_STRING+1], itemText[STK_LEN_STRING+1];
	int titleLen, itemLen;
	char* pBuffer;

	pBuffer = ppContent[0];
	offset = STK_Search_Integer(pBuffer, &cmdType, TRUE);

	pBuffer += offset;
	offset = STK_Search_Integer(pBuffer, &commandDetails, FALSE);

	pBuffer += offset;
	offset = STK_Search_Integer(pBuffer, &numOfItems, FALSE);

	pBuffer += offset;
	offset = STK_Search_String(pBuffer, titleText);
	titleLen = (offset - 3) / 2;
	// 解决无匹配双引号titleLen为-1
	if (titleLen < 0)
		titleLen = 0;

	pBuffer += offset;
	offset = STK_Search_Integer(pBuffer, &menuItemIconsPresent, FALSE);

	pBuffer += offset;
	offset = STK_Search_Integer(pBuffer, &menuItemIconsQualifier, FALSE);

	pBuffer += offset;
	offset = STK_Search_Integer(pBuffer, &titleIconQualifier, FALSE);

	pBuffer += offset;
	offset = STK_Search_Integer(pBuffer, &titleIconId, FALSE);

	pSetupMenu->preference = commandDetails & 0x01;
	pSetupMenu->help = (commandDetails & 0x80) >> 7;
	pSetupMenu->numOfItems = numOfItems;
	memcpy(pSetupMenu->titleText, titleText, titleLen);
	pSetupMenu->titleLen = titleLen;
	STK_InvertOrder(pSetupMenu->titleText, pSetupMenu->titleLen);
	pSetupMenu->menuItemIconsPresent = menuItemIconsPresent;
	pSetupMenu->menuItemIconsPresent = menuItemIconsQualifier;
	pSetupMenu->menuItemIconsPresent = titleIconQualifier;
	pSetupMenu->menuItemIconsPresent = titleIconId;

	for (i=1; i<=numOfItems; i++)
	{
		pBuffer = ppContent[i];
		offset = STK_Search_Integer(pBuffer, &cmdType, TRUE);

		pBuffer += offset;
		offset = STK_Search_Integer(pBuffer, &itemId, FALSE);

		pBuffer += offset;
		offset = STK_Search_String(pBuffer, itemText);
		itemLen = (offset - 3) / 2;
		// 解决无匹配双引号itemLen为-1
		if (itemLen < 0)
			itemLen = 0;

		pBuffer += offset;
		offset = STK_Search_Integer(pBuffer, &nextActionId, FALSE);

		pBuffer += offset;
		offset = STK_Search_Integer(pBuffer, &iconId, FALSE);

		pSetupMenuItem[i-1].itemId = itemId;
		memcpy(pSetupMenuItem[i-1].itemText, itemText, itemLen);
		pSetupMenuItem[i-1].itemLen = itemLen;
		STK_InvertOrder(pSetupMenuItem[i-1].itemText, pSetupMenuItem[i-1].itemLen);
		pSetupMenuItem[i-1].nextActionId = nextActionId;
		pSetupMenuItem[i-1].iconId = iconId;
	}

	return 0;
}

/*********************************************************************
* Function	STK_Proat_SelectItem
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
// ^SSTGI: <cmdType>, <commandDetails>, <numOfItems>, <titleText>,
// <defaultItemId>, <itemIconsPresent>, <itemIconsQualifier>, <titleIconQualifier>,
// <titleIconId> <CR> <LF>
// ^SSTGI: <cmdType>, <itemId>, <itemText>, <nextActionId>, <iconId>
// <CR> <LF>
int STK_Proat_SelectItem(char ppContent[STK_MAC_MENUCOUNT][STK_MAC_MENUCOMMAND], PSELECTITEM pSelectItem, MENUITEM pSelectMenuItem[STK_MAC_MENUCOUNT])
{
	int i, cmdType, commandDetails, numOfItems, defaultItemId, itemId, nextActionId, offset;
	int itemIconsPresent, itemIconsQualifier;
	int titleIconQualifier, titleIconId;
	int iconId;
	BYTE titleText[STK_LEN_STRING+1], itemText[STK_LEN_STRING+1];
	int titleLen, itemLen;
	char* pBuffer;

	pBuffer = ppContent[0];
	offset = STK_Search_Integer(pBuffer, &cmdType, TRUE);

	pBuffer += offset;
	offset = STK_Search_Integer(pBuffer, &commandDetails, FALSE);

	pBuffer += offset;
	offset = STK_Search_Integer(pBuffer, &numOfItems, FALSE);

	pBuffer += offset;
	offset = STK_Search_String(pBuffer, titleText);
	titleLen = (offset - 3) / 2;
	// 解决无匹配双引号titleLen为-1
	if (titleLen < 0)
		titleLen = 0;

	pBuffer += offset;
	offset = STK_Search_Integer(pBuffer, &defaultItemId, FALSE);

	pBuffer += offset;
	offset = STK_Search_Integer(pBuffer, &itemIconsPresent, FALSE);

	pBuffer += offset;
	offset = STK_Search_Integer(pBuffer, &itemIconsQualifier, FALSE);

	pBuffer += offset;
	offset = STK_Search_Integer(pBuffer, &titleIconQualifier, FALSE);

	pBuffer += offset;
	offset = STK_Search_Integer(pBuffer, &titleIconId, FALSE);

	pSelectItem->presentation = commandDetails & 0x01;
	pSelectItem->choice = (commandDetails & 0x02) >> 1;
	pSelectItem->preference = (commandDetails & 0x04) >> 2;
	pSelectItem->help = (commandDetails & 0x80) >> 7;
	pSelectItem->numOfItems = numOfItems;
	memcpy(pSelectItem->titleText, titleText, titleLen);
	pSelectItem->titleLen = titleLen;
	STK_InvertOrder(pSelectItem->titleText, pSelectItem->titleLen);
	pSelectItem->defaultItemId = defaultItemId;
	pSelectItem->itemIconsPresent = itemIconsPresent;
	pSelectItem->itemIconsPresent = itemIconsQualifier;
	pSelectItem->itemIconsPresent = titleIconQualifier;
	pSelectItem->itemIconsPresent = titleIconId;

	for (i=1; i<=numOfItems; i++)
	{
		pBuffer = ppContent[i];
		offset = STK_Search_Integer(pBuffer, &cmdType, TRUE);

		pBuffer += offset;
		offset = STK_Search_Integer(pBuffer, &itemId, FALSE);

		pBuffer += offset;
		offset = STK_Search_String(pBuffer, itemText);
		itemLen = (offset - 3) / 2;
		// 解决无匹配双引号itemLen为-1
		if (itemLen < 0)
			itemLen = 0;

		pBuffer += offset;
		offset = STK_Search_Integer(pBuffer, &nextActionId, FALSE);

		pBuffer += offset;
		offset = STK_Search_Integer(pBuffer, &iconId, FALSE);

		pSelectMenuItem[i-1].itemId = itemId;
		memcpy(pSelectMenuItem[i-1].itemText, itemText, itemLen);
		pSelectMenuItem[i-1].itemLen = itemLen;
		STK_InvertOrder(pSelectMenuItem[i-1].itemText, pSelectMenuItem[i-1].itemLen);
		pSelectMenuItem[i-1].nextActionId = nextActionId;
		pSelectMenuItem[i-1].iconId = iconId;
	}

	return 0;
}

/*********************************************************************
* Function	STK_Proat_String
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
int STK_Proat_String(int Indicate, char* pContent)
{
	switch (Indicate)
	{
	case STK_PAC_REFRESH:
		STK_Proat_Refresh(pContent, &Refresh);
		break;

	case STK_PAC_SETUPEVENTLIST:
		STK_Proat_SetupEventList(pContent, &SetupEventList);
		break;

	case STK_PAC_SETUPCALL:
		STK_Proat_SetupCall(pContent, &SetupCall);
		break;

	case STK_PAC_SENDSS:
		STK_Proat_SendSS(pContent, &SendSS);
		break;

	case STK_PAC_SENDUSSD:
		STK_Proat_SendUSSD(pContent, &SendUSSD);
		break;

	case STK_PAC_SENDSHORTMESSAGE:
		STK_Proat_SendShortMessage(pContent, &SendShortMessage);
		break;

	case STK_PAC_SENDDTMF:
		STK_Proat_SendDTMF(pContent, &SendDTMF);
		break;

	case STK_PAC_PLAYTONE:
		STK_Proat_PlayTone(pContent, &PlayTone);
		break;

	case STK_PAC_DISPLAYTEXT:
		STK_Proat_DisplayText(pContent, &DisplayText);
		break;

	case STK_PAC_GETINKEY:
		STK_Proat_GetInkey(pContent, &GetInkey);
		break;

	case STK_PAC_GETINPUT:
		STK_Proat_GetInput(pContent, &GetInput);
		break;

	case STK_PAC_SETUPIDLEMODETEXT:
		STK_Proat_SetupIdleModeText(pContent, &SetupIdleModeText);
		break;

	default:
		break;
	}

	return 0;
}

/*********************************************************************
* Function	STK_Proat_Refresh
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
// ^SSTGI: <cmdType>, <commandDetails> <CR> <LF>
static int STK_Proat_Refresh(char* pContent, PREFRESH pRefresh)
{
	int cmdType, commandDetails, offset;
	char* pBuffer;

	pBuffer = pContent;
	offset = STK_Search_Integer(pBuffer, &cmdType, TRUE);

	pBuffer += offset;
	offset = STK_Search_Integer(pBuffer, &commandDetails, FALSE);

	pRefresh->refresh = commandDetails;

	return 0;
}

/*********************************************************************
* Function	STK_Proat_SetupEventList
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
// ^SSTGI: <cmdType>, <commandDetails>, <eventList> <CR> <LF>
static int STK_Proat_SetupEventList(char* pContent, PSETUPEVENTLIST pSetupEventList)
{
	int cmdType, commandDetails, eventList, offset;
	char* pBuffer;

	pBuffer = pContent;
	offset = STK_Search_Integer(pBuffer, &cmdType, TRUE);

	pBuffer += offset;
	offset = STK_Search_Integer(pBuffer, &commandDetails, FALSE);

	pBuffer += offset;
	offset = STK_Search_Integer(pBuffer, &eventList, FALSE);

	pSetupEventList->evtUA = eventList & 0x10;
	pSetupEventList->evtISA = eventList & 0x20;
	pSetupEventList->evtLS = eventList & 0x80;
	pSetupEventList->evtBT = eventList & 0x100;

	return 0;
}

/*********************************************************************
* Function	STK_Proat_SetupCall
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
// ^SSTGI: <cmdType>, <commandDetails>, <confirmationText>,
// <calledNumber>, <callSetupText>, <confirmationIconQualifier>,
// <confirmationIconId>, <callSetupIconQualifier>, <callSetupIconId> <CR> <LF>
static int STK_Proat_SetupCall(char* pContent, PSETUPCALL pSetupCall)
{
	int cmdType, commandDetails, offset;
	int confirmationIconQualifier, confirmationIconId;
	int callSetupIconQualifier, callSetupIconId;
	BYTE confirmationText[STK_LEN_STRING+1], calledNumber[STK_LEN_STRING+1], callSetupText[STK_LEN_STRING+1];
	int confirmationLen, calledNumberLen, callSetupLen;
	char* pBuffer;

	pBuffer = pContent;
	offset = STK_Search_Integer(pBuffer, &cmdType, TRUE);

	pBuffer += offset;
	offset = STK_Search_Integer(pBuffer, &commandDetails, FALSE);

	pBuffer += offset;
	offset = STK_Search_String(pBuffer, confirmationText);
	confirmationLen = (offset - 3) / 2;
	// 解决无匹配双引号confirmationLen为-1
	if (confirmationLen < 0)
		confirmationLen = 0;

	pBuffer += offset;
	offset = STK_Search_String(pBuffer, calledNumber);
	calledNumberLen = (offset - 3) / 2;
	// 解决无匹配双引号calledNumberLen为-1
	if (calledNumberLen < 0)
		calledNumberLen = 0;

	pBuffer += offset;
	offset = STK_Search_String(pBuffer, callSetupText);
	callSetupLen = (offset - 3) / 2;
	// 解决无匹配双引号callSetupLen为-1
	if (callSetupLen < 0)
		callSetupLen = 0;

	pBuffer += offset;
	offset = STK_Search_Integer(pBuffer, &confirmationIconQualifier, FALSE);

	pBuffer += offset;
	offset = STK_Search_Integer(pBuffer, &confirmationIconId, FALSE);

	pBuffer += offset;
	offset = STK_Search_Integer(pBuffer, &callSetupIconQualifier, FALSE);

	pBuffer += offset;
	offset = STK_Search_Integer(pBuffer, &callSetupIconId, FALSE);

	pSetupCall->callType = commandDetails;
	memcpy(pSetupCall->confirmationText, confirmationText, confirmationLen);
	pSetupCall->confirmationLen = confirmationLen;
	STK_InvertOrder(pSetupCall->confirmationText, pSetupCall->confirmationLen);
	memcpy(pSetupCall->calledNumber, calledNumber, calledNumberLen);
	pSetupCall->calledNumberLen = calledNumberLen;
	STK_InvertOrder(pSetupCall->calledNumber, pSetupCall->calledNumberLen);
	memcpy(pSetupCall->callSetupText, callSetupText, callSetupLen);
	pSetupCall->callSetupLen = callSetupLen;
	STK_InvertOrder(pSetupCall->callSetupText, pSetupCall->callSetupLen);
	pSetupCall->confirmationIconQualifier = confirmationIconQualifier;
	pSetupCall->confirmationIconId = confirmationIconId;
	pSetupCall->callSetupIconQualifier = callSetupIconQualifier;
	pSetupCall->callSetupIconId = callSetupIconId;

	return 0;
}

/*********************************************************************
* Function	STK_Proat_SendSS
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
// ^SSTGI: <cmdType>, [<commandDetails>], [<text>], <iconQualifier>, <iconId>
// <CR>, <LF>
static int STK_Proat_SendSS(char* pContent, PSENDSS pSendSS)
{
	int cmdType, commandDetails, offset;
	int iconQualifier, iconId;
	BYTE text[STK_LEN_STRING+1];
	int textLen;
	char* pBuffer;

	pBuffer = pContent;
	offset = STK_Search_Integer(pBuffer, &cmdType, TRUE);

	pBuffer += offset;
	offset = STK_Search_Integer(pBuffer, &commandDetails, FALSE);

	pBuffer += offset;
	offset = STK_Search_String(pBuffer, text);
	textLen = (offset - 3) / 2;
	// 解决无匹配双引号textLen为-1
	if (textLen < 0)
		textLen = 0;

	pBuffer += offset;
	offset = STK_Search_Integer(pBuffer, &iconQualifier, FALSE);

	pBuffer += offset;
	offset = STK_Search_Integer(pBuffer, &iconId, FALSE);

	memcpy(pSendSS->text, text, textLen);
	pSendSS->textLen = textLen;
	STK_InvertOrder(pSendSS->text, pSendSS->textLen);
	pSendSS->iconQualifier = iconQualifier & 0x01;
	pSendSS->iconId = iconId;

	return 0;
}

/*********************************************************************
* Function	STK_Proat_SendUSSD
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
// ^SSTGI: <cmdType>, [<commandDetails>], [<text>], <iconQualifier>, <iconId>
// <CR> <LF>
static int STK_Proat_SendUSSD(char* pContent, PSENDUSSD pSendUSSD)
{
	int cmdType, commandDetails, offset;
	int iconQualifier, iconId;
	BYTE text[STK_LEN_STRING+1];
	int textLen;
	char* pBuffer;

	pBuffer = pContent;
	offset = STK_Search_Integer(pBuffer, &cmdType, TRUE);

	pBuffer += offset;
	offset = STK_Search_Integer(pBuffer, &commandDetails, FALSE);

	pBuffer += offset;
	offset = STK_Search_String(pBuffer, text);
	textLen = (offset - 3) / 2;
	// 解决无匹配双引号textLen为-1
	if (textLen < 0)
		textLen = 0;

	pBuffer += offset;
	offset = STK_Search_Integer(pBuffer, &iconQualifier, FALSE);

	pBuffer += offset;
	offset = STK_Search_Integer(pBuffer, &iconId, FALSE);

	memcpy(pSendUSSD->text, text, textLen);
	pSendUSSD->textLen = textLen;
	STK_InvertOrder(pSendUSSD->text, pSendUSSD->textLen);
	pSendUSSD->iconQualifier = iconQualifier & 0x01;
	pSendUSSD->iconId = iconId;

	return 0;
}

/*********************************************************************
* Function	STK_Proat_SendShortMessage
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
// ^SSTGI: <cmdType>, <commandDetails>, <textInfo>, <iconQualifier>, <iconId>
// <CR> <LF>
static int STK_Proat_SendShortMessage(char* pContent, PSENDSHORTMESSAGE pSendShortMessage)
{
	int cmdType, commandDetails, offset;
	int iconQualifier, iconId;
	BYTE textInfo[STK_LEN_STRING+1];
	int infoLen;
	char* pBuffer;

	pBuffer = pContent;
	offset = STK_Search_Integer(pBuffer, &cmdType, TRUE);

	pBuffer += offset;
	offset = STK_Search_Integer(pBuffer, &commandDetails, FALSE);

	pBuffer += offset;
	offset = STK_Search_String(pBuffer, textInfo);
	infoLen = (offset - 3) / 2;
	// 解决无匹配双引号infoLen为-1
	if (infoLen < 0)
		infoLen = 0;

	pBuffer += offset;
	offset = STK_Search_Integer(pBuffer, &iconQualifier, FALSE);

	pBuffer += offset;
	offset = STK_Search_Integer(pBuffer, &iconId, FALSE);

	memcpy(pSendShortMessage->textInfo, textInfo, infoLen);
	pSendShortMessage->infoLen = infoLen;
	STK_InvertOrder(pSendShortMessage->textInfo, pSendShortMessage->infoLen);
	pSendShortMessage->iconQualifier = iconQualifier & 0x01;
	pSendShortMessage->iconId = iconId;

	return 0;
}

/*********************************************************************
* Function	STK_Proat_SendDTMF
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
// ^SSTGI: <cmdType>, <commandDetails>, <text>, <iconQualifier>, <iconId>
// <CR> <LF>
static int STK_Proat_SendDTMF(char* pContent, PSENDDTMF pSendDTMF)
{
	int cmdType, commandDetails, offset;
	int iconQualifier, iconId;
	BYTE text[STK_LEN_STRING+1];
	int textLen;
	char* pBuffer;

	pBuffer = pContent;
	offset = STK_Search_Integer(pBuffer, &cmdType, TRUE);

	pBuffer += offset;
	offset = STK_Search_Integer(pBuffer, &commandDetails, FALSE);

	pBuffer += offset;
	offset = STK_Search_String(pBuffer, text);
	textLen = (offset - 3) / 2;
	// 解决无匹配双引号textLen为-1
	if (textLen < 0)
		textLen = 0;

	pBuffer += offset;
	offset = STK_Search_Integer(pBuffer, &iconQualifier, FALSE);

	pBuffer += offset;
	offset = STK_Search_Integer(pBuffer, &iconId, FALSE);

	memcpy(pSendDTMF->text, text, textLen);
	pSendDTMF->textLen = textLen;
	STK_InvertOrder(pSendDTMF->text, pSendDTMF->textLen);
	pSendDTMF->iconQualifier = iconQualifier & 0x01;
	pSendDTMF->iconId = iconId;

	return 0;
}

/*********************************************************************
* Function	STK_Proat_PlayTone
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
// ^SSTGI: <cmdType>, <commandDetails>, <infoText>, <tone>, <durationUnit>,
// <duration>, <iconQualifier>, <iconId> <CR> <LF>
static int STK_Proat_PlayTone(char* pContent, PPLAYTONE pPlayTone)
{
	int cmdType, commandDetails, tone, durationUnit, duration, offset;
	int iconQualifier, iconId;
	BYTE infoText[STK_LEN_STRING+1];
	int infoLen;
	char* pBuffer;

	pBuffer = pContent;
	offset = STK_Search_Integer(pBuffer, &cmdType, TRUE);

	pBuffer += offset;
	offset = STK_Search_Integer(pBuffer, &commandDetails, FALSE);

	pBuffer += offset;
	offset = STK_Search_String(pBuffer, infoText);
	infoLen = (offset - 3) / 2;
	// 解决无匹配双引号infoLen为-1
	if (infoLen < 0)
		infoLen = 0;

	pBuffer += offset;
	offset = STK_Search_Integer(pBuffer, &tone, FALSE);

	pBuffer += offset;
	offset = STK_Search_Integer(pBuffer, &durationUnit, FALSE);

	pBuffer += offset;
	offset = STK_Search_Integer(pBuffer, &duration, FALSE);

	pBuffer += offset;
	offset = STK_Search_Integer(pBuffer, &iconQualifier, FALSE);

	pBuffer += offset;
	offset = STK_Search_Integer(pBuffer, &iconId, FALSE);

	memcpy(pPlayTone->infoText, infoText, infoLen);
	pPlayTone->infoLen = infoLen;
	STK_InvertOrder(pPlayTone->infoText, pPlayTone->infoLen);
	pPlayTone->tone = tone;
	pPlayTone->durationUnit = durationUnit;
	pPlayTone->duration = duration;
	pPlayTone->iconQualifier = iconQualifier & 0x01;
	pPlayTone->iconId = iconId;

	return 0;
}

/*********************************************************************
* Function	STK_Proat_DisplayText
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
// ^SSTGI: <cmdType>, <commandDetails>, <text>, <immediateResponse>,
// <iconQualifier>, <iconId> <CR> <LF>
static int STK_Proat_DisplayText(char* pContent, PDISPLAYTEXT pDisplayText)
{
	int cmdType, commandDetails, immediateResponse, offset;
	int iconQualifier, iconId;
	BYTE text[STK_LEN_STRING+1];
	int textLen;
	char* pBuffer;

	pBuffer = pContent;
	offset = STK_Search_Integer(pBuffer, &cmdType, TRUE);

	pBuffer += offset;
	offset = STK_Search_Integer(pBuffer, &commandDetails, FALSE);

	pBuffer += offset;
	offset = STK_Search_String(pBuffer, text);
	textLen = (offset - 3) / 2;
	// 解决无匹配双引号textLen为-1
	if (textLen < 0)
		textLen = 0;

	pBuffer += offset;
	offset = STK_Search_Integer(pBuffer, &immediateResponse, FALSE);

	pBuffer += offset;
	offset = STK_Search_Integer(pBuffer, &iconQualifier, FALSE);

	pBuffer += offset;
	offset = STK_Search_Integer(pBuffer, &iconId, FALSE);

	pDisplayText->priority = commandDetails & 0x01;
	pDisplayText->clear = (commandDetails & 0x80) >> 7;
	memcpy(pDisplayText->text, text, textLen);
	pDisplayText->textLen = textLen;
	STK_InvertOrder(pDisplayText->text, pDisplayText->textLen);
	pDisplayText->response = immediateResponse;
	pDisplayText->iconQualifier = iconQualifier & 0x01;
	pDisplayText->iconId = iconId;

	return 0;
}

/*********************************************************************
* Function	STK_Proat_GetInkey
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
// ^SSTGI: <cmdType>, <commandDetails>, <text>, <iconQualifier>, <iconId>
// <CR> <LF>
static int STK_Proat_GetInkey(char* pContent, PGETINKEY pGetInkey)
{
	int cmdType, commandDetails, offset;
	int iconQualifier, iconId;
	BYTE text[STK_LEN_STRING+1];
	int textLen;
	char* pBuffer;

	pBuffer = pContent;
	offset = STK_Search_Integer(pContent, &cmdType, TRUE);

	pBuffer += offset;
	offset = STK_Search_Integer(pBuffer, &commandDetails, FALSE);

	pBuffer += offset;
	offset = STK_Search_String(pBuffer, text);
	textLen = (offset - 3) / 2;
	// 解决无匹配双引号textLen为-1
	if (textLen < 0)
		textLen = 0;

	pBuffer += offset;
	offset = STK_Search_Integer(pBuffer, &iconQualifier, FALSE);

	pBuffer += offset;
	offset = STK_Search_Integer(pBuffer, &iconId, FALSE);

	pGetInkey->inputFormat = commandDetails & 0x01;
	pGetInkey->character = (commandDetails & 0x02) >> 1;
	pGetInkey->yesNo = (commandDetails & 0x04) >> 2;
	pGetInkey->help = (commandDetails & 0x80) >> 7;
	memcpy(pGetInkey->text, text, textLen);
	pGetInkey->textLen = textLen;
	STK_InvertOrder(pGetInkey->text, pGetInkey->textLen);
	pGetInkey->iconQualifier = iconQualifier & 0x01;
	pGetInkey->iconId = iconId;

	return 0;
}

/*********************************************************************
* Function	STK_Proat_GetInput
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
// ^SSTGI: <cmdType>, <commandDetails>, <text>, <responseMin>,
// <responseMax>, [<defaultText>], <iconQualifier>, <iconId> <CR> <LF>
static int STK_Proat_GetInput(char* pContent, PGETINPUT pGetInput)
{
	int cmdType, commandDetails, responseMin, responseMax, offset;
	int iconQualifier, iconId;
	BYTE text[STK_LEN_STRING+1], defaultText[STK_LEN_STRING+1];
	int textLen, defaultLen;
	char* pBuffer;

	pBuffer = pContent;
	offset = STK_Search_Integer(pContent, &cmdType, TRUE);

	pBuffer += offset;
	offset = STK_Search_Integer(pBuffer, &commandDetails, FALSE);

	pBuffer += offset;
	offset = STK_Search_String(pBuffer, text);
	textLen = (offset - 3) / 2;
	// 解决无匹配双引号textLen为-1
	if (textLen < 0)
		textLen = 0;

	pBuffer += offset;
	offset = STK_Search_Integer(pBuffer, &responseMin, FALSE);

	pBuffer += offset;
	offset = STK_Search_Integer(pBuffer, &responseMax, FALSE);

	pBuffer += offset;
	offset = STK_Search_String(pBuffer, defaultText);
	defaultLen = (offset - 3) / 2;
	// 解决无匹配双引号defaultLen为-1
	if (defaultLen < 0)
		defaultLen = 0;

	pBuffer += offset;
	offset = STK_Search_Integer(pBuffer, &iconQualifier, FALSE);

	pBuffer += offset;
	offset = STK_Search_Integer(pBuffer, &iconId, FALSE);

	pGetInput->inputFormat = commandDetails & 0x01;
	pGetInput->character = (commandDetails & 0x02) >> 1;
	pGetInput->hidden = (commandDetails & 0x04) >> 2;
	pGetInput->pack = (commandDetails & 0x08) >> 3;
	pGetInput->help = (commandDetails & 0x80) >> 7;
	memcpy(pGetInput->text, text, textLen);
	pGetInput->textLen = textLen;
	STK_InvertOrder(pGetInput->text, pGetInput->textLen);
	pGetInput->responseMin = responseMin;
	pGetInput->responseMax = responseMax;
	memcpy(pGetInput->defaultText, defaultText, defaultLen);
	pGetInput->defaultLen = defaultLen;
	STK_InvertOrder(pGetInput->defaultText, pGetInput->defaultLen);
	pGetInput->iconQualifier = iconQualifier & 0x01;
	pGetInput->iconId = iconId;

	return 0;
}

/*********************************************************************
* Function	STK_Proat_SetupIdleModeText
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
// ^SSTGI: <cmdType>, <commandDetails>, <text>, <iconQualifier>, <iconId>
// <CR> <LF>
static int STK_Proat_SetupIdleModeText(char* pContent, PSETUPIDLEMODETEXT pSetupIdleModeText)
{
	int cmdType, commandDetails, offset;
	int iconQualifier, iconId;
	BYTE text[STK_LEN_STRING+1];
	int textLen;
	char* pBuffer;

	pBuffer = pContent;
	offset = STK_Search_Integer(pContent, &cmdType, TRUE);

	pBuffer += offset;
	offset = STK_Search_Integer(pBuffer, &commandDetails, FALSE);

	pBuffer += offset;
	offset = STK_Search_String(pBuffer, text);
	textLen = (offset - 3) / 2;
	// 解决无匹配双引号textLen为-1
	if (textLen < 0)
		textLen = 0;

	pBuffer += offset;
	offset = STK_Search_Integer(pBuffer, &iconQualifier, FALSE);

	pBuffer += offset;
	offset = STK_Search_Integer(pBuffer, &iconId, FALSE);

	memcpy(pSetupIdleModeText->text, text, textLen);
	pSetupIdleModeText->textLen = textLen;
	STK_InvertOrder(pSetupIdleModeText->text, pSetupIdleModeText->textLen);
	pSetupIdleModeText->iconQualifier = iconQualifier & 0x01;
	pSetupIdleModeText->iconId = iconId;

	return 0;
}

/*********************************************************************
* Function	STK_FormatStr
* Purpose   
* Parameter	
* Return	void
* Remarks	
**********************************************************************/
void STK_FormatStr(char *pszStrIn, char *pszStrOut)
{
	int i, Lines, nOffset;
	SIZE Size;
	char szBuf[30];
	char *pstrNext;

	i = 0;
	pstrNext = pszStrIn;
	*pszStrOut = '\0';

	GetTextExtentPoint(NULL, "0", strlen("0"), &Size);
	if (Size.cy > 18)
		Lines = 2;
	else
		Lines = 3;

	do {
		GetTextExtentExPoint(NULL, pstrNext, strlen(pstrNext), 166, &nOffset, NULL, NULL);
		memset(szBuf, 0, 30);
		strncpy(szBuf, pstrNext, nOffset);
		if (i == 0)
		{
			i++;
			strcpy(pszStrOut, szBuf);
		}
		else if (i == 1 && Lines == 3)
		{
			i++;
			strcat(pszStrOut, szBuf);
		}
		else
		{
			GetTextExtentExPoint(NULL, pstrNext, strlen(pstrNext), 150, &nOffset, NULL, NULL);
			memset(szBuf, 0, 30);
			strncpy(szBuf, pstrNext, nOffset);
			strcat(pszStrOut, szBuf);
			pstrNext += nOffset;
			if (*pstrNext == '\0')
				break;
			strcat(pszStrOut, "...");
			break;
		}
		strcat(pszStrOut, "\n");
		pstrNext += nOffset;
		if (*pstrNext == '\0')
			break;
	} while (nOffset != 0);
}

/*********************************************************************
* Function	STK_WaitWin
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
void STK_WaitWin(HWND hWnd, BOOL bWait)
{
	static BOOL bLastWait = FALSE;

	if (bLastWait == bWait)
		return;
	else
	{
		bLastWait = bWait;
		WaitWin(hWnd, bWait,
			IDS_STK_COMMUNICATING, NULL,
			NULL, NULL, NULL);
	}
}

/*********************************************************************
* Function	STK_Proat_Request
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
int STK_Proat_Request(int CmdType)
{
	char atCmd[20];

	sprintf(atCmd, "AT^SSTGI=%d\r", CmdType);
	strcpy(STK_SendStruct.cmd, atCmd);
	STK_SendStruct.timeout = 10000;
	STK_WaitWin(hStkWndApp, TRUE);
	if (-1 == ME_STK_Send(hStkWndApp, WM_STK_PACINFO_RECV, &STK_SendStruct))
		return -1;

	return 0;
}

/*********************************************************************
* Function	STK_Proat_Response
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
int STK_Proat_Response(int CmdType, int State, char *TextData, int TextLen, int ID)
{
	char *pData;
	char atCmd[1024];

	switch (CmdType)
	{
	case STK_PAC_SELECTITEM:
		sprintf(atCmd, "AT^SSTR=36,%d,%d\r", State, ID);
		strcpy(STK_SendStruct.cmd, atCmd);
		STK_SendStruct.timeout = 10000;
		STK_WaitWin(hStkWndApp, TRUE);
		if (-1 == ME_STK_Send(hStkWndApp, WM_STK_RESP_RECV, &STK_SendStruct))
			return -1;
		break;
	case STK_PAC_SETUPMENU:
		sprintf(atCmd, "AT^SSTR=211,%d,%d\r", State, ID);
		strcpy(STK_SendStruct.cmd, atCmd);
		STK_SendStruct.timeout = 10000;
		STK_WaitWin(hStkWndApp, TRUE);
		if (-1 == ME_STK_Send(hStkWndApp, WM_STK_RESP_RECV, &STK_SendStruct))
			return -1;
		break;
	case STK_PAC_GETINPUT:
		if (TextLen == 0)
		{
			sprintf(atCmd, "AT^SSTR=35,%d,,\"\\1b\"\r", State);
			strcpy(STK_SendStruct.cmd, atCmd);
			STK_SendStruct.timeout = 10000;
			STK_WaitWin(hStkWndApp, TRUE);
			if (-1 == ME_STK_Send(hStkWndApp, WM_STK_RESP_RECV, &STK_SendStruct))
				return -1;
			break;
		}
		pData = (char *)malloc(TextLen*2+1);
		STK_InvertOrder(TextData, TextLen);
		STK_Convert_String2Hex((BYTE *)TextData, pData, TextLen, TextLen*2);
		sprintf(atCmd, "AT^SSTR=35,%d,,\"%s\"\r", State, pData);
		strcpy(STK_SendStruct.cmd, atCmd);
		STK_SendStruct.timeout = 30000;
		STK_WaitWin(hStkWndApp, TRUE);
		if (-1 == ME_STK_Send(hStkWndApp, WM_STK_RESP_RECV, &STK_SendStruct))
			return -1;
		free(pData);
		break;
	case STK_PAC_GETINKEY:
		if (TextLen == 0)
		{
			sprintf(atCmd, "AT^SSTR=34,%d,,\"\\1b\"\r", State);
			strcpy(STK_SendStruct.cmd, atCmd);
			STK_SendStruct.timeout = 10000;
			STK_WaitWin(hStkWndApp, TRUE);
			if (-1 == ME_STK_Send(hStkWndApp, WM_STK_RESP_RECV, &STK_SendStruct))
				return -1;
			break;
		}
		pData = (char *)malloc(TextLen*2+1);
		STK_InvertOrder(TextData, TextLen);
		STK_Convert_String2Hex((BYTE *)TextData, pData, TextLen, TextLen*2);
		sprintf(atCmd, "AT^SSTR=34,%d,,\"%s\"\r", State, pData);
		strcpy(STK_SendStruct.cmd, atCmd);
		STK_SendStruct.timeout = 10000;
		STK_WaitWin(hStkWndApp, TRUE);
		if (-1 == ME_STK_Send(hStkWndApp, WM_STK_RESP_RECV, &STK_SendStruct))
			return -1;
		free(pData);
		break;
	default:
		sprintf(atCmd, "AT^SSTR=%d,%d\r", CmdType, State);
		strcpy(STK_SendStruct.cmd, atCmd);
		if (CmdType == STK_PAC_SETUPCALL)
			STK_SendStruct.timeout = 100000;
		else
			STK_SendStruct.timeout = 10000;
		STK_WaitWin(hStkWndApp, TRUE);
		if (-1 == ME_STK_Send(hStkWndApp, WM_STK_RESP_RECV, &STK_SendStruct))
			return -1;
		break;
	}

	return 0;
}

// 7bit编码
// 输入: pSrc - 源字符串指针
//       nSrcLength - 源字符串长度
// 输出: pDst - 目标编码串指针
// 返回: 目标编码串长度
int gsmEncode7bit(const char* pSrc, unsigned char* pDst, int nSrcLength)
{
	int nSrc;		// 源字符串的计数值
	int nDst;		// 目标编码串的计数值
	int nChar;		// 当前正在处理的组内字符字节的序号，范围是0-7
	unsigned char nLeft;	// 上一字节残余的数据

	// 计数值初始化
	nSrc = 0;
	nDst = 0;

	// 将源串每8个字节分为一组，压缩成7个字节
	// 循环该处理过程，直至源串被处理完
	// 如果分组不到8字节，也能正确处理
	while (nSrc < nSrcLength)
	{
		// 取源字符串的计数值的最低3位
		nChar = nSrc & 7;

		// 处理源串的每个字节
		if (nChar == 0)
		{
			// 组内第一个字节，只是保存起来，待处理下一个字节时使用
			nLeft = *pSrc;
		}
		else
		{
			// 组内其它字节，将其右边部分与残余数据相加，得到一个目标编码字节
			*pDst = (*pSrc << (8-nChar)) | nLeft;

			// 将该字节剩下的左边部分，作为残余数据保存起来
			nLeft = *pSrc >> nChar;

			// 修改目标串的指针和计数值
			pDst++;
			nDst++;
		}

		// 修改源串的指针和计数值
		pSrc++;
		nSrc++;
	}

	// 返回目标串长度
	return nDst;
}

// 7bit解码
// 输入: pSrc - 源编码串指针
//       nSrcLength - 源编码串长度
// 输出: pDst - 目标字符串指针
// 返回: 目标字符串长度
int gsmDecode7bit(const unsigned char* pSrc, char* pDst, int nSrcLength)
{
	int nSrc;		// 源字符串的计数值
	int nDst;		// 目标解码串的计数值
	int nByte;		// 当前正在处理的组内字节的序号，范围是0-6
	unsigned char nLeft;	// 上一字节残余的数据

	// 计数值初始化
	nSrc = 0;
	nDst = 0;

	// 组内字节序号和残余数据初始化
	nByte = 0;
	nLeft = 0;

	// 将源数据每7个字节分为一组，解压缩成8个字节
	// 循环该处理过程，直至源数据被处理完
	// 如果分组不到7字节，也能正确处理
	while (nSrc<nSrcLength)
	{
		// 将源字节右边部分与残余数据相加，去掉最高位，得到一个目标解码字节
		*pDst = ((*pSrc << nByte) | nLeft) & 0x7F;

		// 将该字节剩下的左边部分，作为残余数据保存起来
		nLeft = *pSrc >> (7-nByte);

		// 修改目标串的指针和计数值
		pDst++;
		nDst++;

		// 修改字节计数值
		nByte++;

		// 到了一组的最后一个字节
		if (nByte == 7)
		{
			// 额外得到一个目标解码字节
			*pDst = nLeft;

			// 修改目标串的指针和计数值
			pDst++;
			nDst++;

			// 组内字节序号和残余数据初始化
			nByte = 0;
			nLeft = 0;
		}

		// 修改源串的指针和计数值
		pSrc++;
		nSrc++;
	}

	// 输出字符串加个结束符
	*pDst = '\0';

	// 返回目标串长度
	return nDst;
}
