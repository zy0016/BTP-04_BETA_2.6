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

#include "window.h"
#include "string.h"
#include "malloc.h"
#include "wappush.h"
#include "unistd.h"
#include "sys\stat.h"
#include "sys\types.h"
#include "fcntl.h"
#include "stdio.h"
#include "wUipub.h"
#include "winpda.h"
#include "mullang.h"
#include "plx_pdaex.h"
#include "winpda.h"
#include "pubapp.h"
#include "mu_def.h"
#include "wapi.h"
#include "WapPush.h"
#include "PushStruct.h"
#include "wappushapi.h"

static int nPushNext, nPushStep;
static int nDelNodeLen;
int PushMsgHandle[MAX_PUSH_NUM];
static int MsgHandle;

extern HWND hPushDetail;
extern HBITMAP hreadpushbmp;
extern HBITMAP hunreadpushbmp;

/*********************************************************************
* Function	WP_GetTotalCount
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
int WP_GetTotalCount(void)
{
	FILE *pFile;
	int total;

	pFile = fopen(FILE_PUSH_WAP, "rb");
	if (pFile == NULL)
		return 0;

	// 第一个四字节位置
	fread(&total, sizeof(int), 1, pFile);

	fclose(pFile);
	return total;
}

/*********************************************************************
* Function	WP_GetUnReadCount
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
int WP_GetUnReadCount(void)
{
	FILE *pFile;
	int unread;
	
	pFile = fopen(FILE_PUSH_WAP, "rb");
	if (pFile == NULL)
		return 0;
	
	// 第二个四字节位置
	fseek(pFile, sizeof(int), SEEK_SET);
	fread(&unread, sizeof(int), 1, pFile);
	
	fclose(pFile);
	return unread;
}

/*********************************************************************
* Function	WP_InsertRecord
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
int WP_InsertRecord(PPUSHFILERECORD pRecord)
{
	FILE *pFile, *pCopyFile;
	int total, unread, filesize;
	int offset, change;

	total = WP_GetTotalCount();
	unread = WP_GetUnReadCount();

	if (total == MAX_PUSH_NUM)
	{
		BOOL status;
		int TitleLen;
		int UrlLen;
		int remainder;
		char *data;

		// Full. We just delete the first and add to tail
		pFile = fopen(FILE_PUSH_WAP, "rb");
		if (pFile == NULL)
			return -1;

		fseek(pFile, 0, SEEK_END);
		filesize = ftell(pFile);

		fseek(pFile, 3*sizeof(int), SEEK_SET);
		fread(&status, sizeof(BOOL), 1, pFile);
		fseek(pFile, sizeof(int), SEEK_CUR);

		fread(&TitleLen, sizeof(int), 1, pFile);
		fseek(pFile, TitleLen, SEEK_CUR);
		fread(&UrlLen, sizeof(int), 1, pFile);
		fseek(pFile, UrlLen, SEEK_CUR);

		remainder = filesize - (5*sizeof(int)+sizeof(BOOL)+sizeof(int)+TitleLen+UrlLen);
		data = malloc(remainder + 1);
		memset(data, 0, remainder + 1);
		if (data == NULL)
		{
			fclose(pFile);
			return -1;
		}

		fread(data, remainder, 1, pFile);
		fclose(pFile);

		pCopyFile = fopen(FILE_PUSH_BACKUP, "wb");
		if (pCopyFile == NULL)
		{
			free(data);
			return -1;
		}
		fwrite(&total, sizeof(int), 1, pCopyFile);
		if (status != WP_PUSH_UNREAD)
			unread++;
		fwrite(&unread, sizeof(int), 1, pCopyFile);

		change = 3*sizeof(int)+sizeof(BOOL)+sizeof(int)+TitleLen+UrlLen;
		WP_ChangeOffset(data, remainder, change);

		fwrite(data, remainder, 1, pCopyFile);
		free(data);

		offset = 2*sizeof(int)+remainder;
		WP_AddToTail(pCopyFile, pRecord, offset);
		fclose(pCopyFile);

		remove(FILE_PUSH_WAP);
		rename(FILE_PUSH_BACKUP, FILE_PUSH_WAP);
	}
	else
	{
		// Check file exist or not
		if (total == 0)
		{
			pFile = fopen(FILE_PUSH_WAP, "wb");
			if (pFile == NULL)
				return -1;

			total  = 1;
			unread = 1;
			fwrite(&total, sizeof(int), 1, pFile);
			fwrite(&unread, sizeof(int), 1, pFile);

			offset = 2*sizeof(int);
			WP_AddToTail(pFile, pRecord, offset);
			fclose(pFile);
		}
		else
		{
			pFile = fopen(FILE_PUSH_WAP, "rb+");
			if (pFile == NULL)
				return -1;

			fseek(pFile, 0, SEEK_END);
			filesize = ftell(pFile);
			fseek(pFile, 0, SEEK_SET);

			total++;
			unread++;
			fwrite(&total, sizeof(int), 1, pFile);
			fwrite(&unread, sizeof(int), 1, pFile);

			fseek(pFile, 0, SEEK_END);
			offset = filesize;
			WP_AddToTail(pFile, pRecord, offset);
			fclose(pFile);
		}
	}

	return offset;
}

/*********************************************************************
* Function	WP_FormatMessage
* Purpose   
* Parameter	
* Return	BOOL
* Remarks	
**********************************************************************/
/*BOOL WP_FormatMessage(int MsgHandle)
{
	//int nType;
	PUSHFILERECORD Record;

	//nType = INBOXGetType(MsgHandle);
	//if (nType==CONTENT_TYPE_WAPSI || nType==CONTENT_TYPE_WAPSIC)
	{
		if (WP_GetRecord(MsgHandle, &Record))
		{
			int nlen, total, unread;
			char *szTemp;

			nlen = Record.TitleLen + Record.UrlLen + 3;
			szTemp = (char *)malloc(nlen + 1);

			if (szTemp == NULL)
			{
				SetWindowText(hPushDetail, Record.pszUri);
			}
			else
			{
				strcpy(szTemp, Record.pszTitle);
                strcat(szTemp, "\r\n");
				strcat(szTemp, Record.pszUri);
				SetWindowText(hPushDetail, szTemp);
			}

			free(szTemp);
			free(Record.pszTitle);
			free(Record.pszUri);

			WP_ResetFlag(MsgHandle, WP_PUSH_READ);

			total = WP_GetTotalCount();
			unread = WP_GetUnReadCount();

			//if (total == MAX_PUSH_NUM)
				//MsgNotify(unread, TRUE, MU_MDU_PUSH);
			//else
				//MsgNotify(unread, FALSE, MU_MDU_PUSH);
			return TRUE;
		}
		return FALSE;
	}

	return FALSE;
}*/

/*********************************************************************
* Function	WP_ResetFlag
* Purpose   
* Parameter	
* Return	BOOL
* Remarks	
**********************************************************************/
BOOL WP_ResetFlag(int offset, BOOL nFlag)
{
	FILE *pFile;
	int status;
	int total, unread;

	pFile = fopen(FILE_PUSH_WAP, "rb+");
	if (pFile == NULL)
		return FALSE;

	fread(&total, sizeof(int), 1, pFile);
	fread(&unread, sizeof(int), 1, pFile);

	fseek(pFile, offset+sizeof(int), SEEK_SET);
	fread(&status, sizeof(BOOL), 1, pFile);
	if (status == WP_PUSH_UNREAD && nFlag == WP_PUSH_READ)
		unread--;

	fseek(pFile, offset+sizeof(int), SEEK_SET);
	fwrite(&nFlag, sizeof(BOOL), 1, pFile);

	fseek(pFile, sizeof(int), SEEK_SET);
	fwrite(&unread, sizeof(int), 1, pFile);

	fclose(pFile);
	return TRUE;
}

/*********************************************************************
* Function	WP_GetRecord
* Purpose   
* Parameter	
* Return	BOOL
* Remarks	
**********************************************************************/
BOOL WP_GetRecord(int offset, PPUSHFILERECORD pRecord)
{
	FILE *pFile;

	pFile = fopen(FILE_PUSH_WAP, "rb");
	if (pFile == NULL)
		return FALSE;

	fseek(pFile, offset, SEEK_SET);
	fread(&(pRecord->offset), sizeof(int), 1, pFile);
	fread(&(pRecord->status), sizeof(BOOL), 1, pFile);
	fread(&(pRecord->type), sizeof(int), 1, pFile);

	fread(&(pRecord->TitleLen), sizeof(int), 1, pFile);
	pRecord->pszTitle = (char *)malloc(pRecord->TitleLen+1);
	memset(pRecord->pszTitle, 0, pRecord->TitleLen + 1);
	if (pRecord->pszTitle == NULL)
	{
		fclose(pFile);
		return FALSE;
	}
	memset(pRecord->pszTitle, 0, pRecord->TitleLen+1);
	fread(pRecord->pszTitle, pRecord->TitleLen, 1, pFile);
	fread(&(pRecord->UrlLen), sizeof(int), 1, pFile);
	pRecord->pszUri = (char *)malloc(pRecord->UrlLen+1);
	memset(pRecord->pszUri, 0, pRecord->UrlLen + 1);
	if (pRecord->pszUri == NULL)
	{
		free(pRecord->pszTitle);
		fclose(pFile);
		return FALSE;
	}
	memset(pRecord->pszUri, 0, pRecord->UrlLen+1);
	fread(pRecord->pszUri, pRecord->UrlLen, 1, pFile);
	fclose(pFile);
	return TRUE;
}

/*********************************************************************
* Function	WP_AddToTail
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
int WP_AddToTail(FILE* pFile, PPUSHFILERECORD pRecord, int offset)
{
//	char *tmp;
//	int tmplen;
	
	if (pFile==NULL || pRecord==NULL)
		return 0;

	fwrite(&offset, sizeof(int), 1, pFile);
	fwrite(&(pRecord->status), sizeof(BOOL), 1, pFile);
	fwrite(&(pRecord->type), sizeof(int), 1, pFile);
	fwrite(&(pRecord->TitleLen), sizeof(int), 1, pFile);
	
//	tmplen = MultiByteToUTF8(CP_ACP, 0, (LPCSTR)pRecord->pszTitle, -1, NULL, 0);
//	tmp = (char *)malloc(tmplen + 1);
//	memset(tmp, 0, tmplen + 1);
//	MultiByteToUTF8(CP_ACP, 0, (LPCSTR)pRecord->pszTitle, -1, tmp, tmplen);
//	tmp[tmplen] = '\0';

	fwrite(pRecord->pszTitle, pRecord->TitleLen, 1, pFile);
	fwrite(&(pRecord->UrlLen), sizeof(int), 1, pFile);
//	free(tmp);

//	tmplen = MultiByteToUTF8(CP_ACP, 0, (LPCSTR)pRecord->pszUri, -1, NULL, 0);
//	tmp = (char *)malloc(tmplen + 1);
//	memset(tmp, 0, tmplen + 1);
//	MultiByteToUTF8(CP_ACP, 0, (LPCSTR)pRecord->pszUri, -1, tmp, tmplen);
//	tmp[tmplen] = '\0';

	fwrite(pRecord->pszUri, pRecord->UrlLen, 1, pFile);
//	free(tmp);

	return 1;
}

/*********************************************************************
* Function	WP_ChangeOffset
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
int WP_ChangeOffset(char* data, int datalen, int change)
{
	char* head;
	int step = 0;
	int Totalstep = 0;

	int offset;
	int TitleLen;
	int UrlLen;

	head = data;
	if (head == NULL)
		return 0;

	while (Totalstep != datalen)
	{
		memcpy((char *)&offset, head, sizeof(int));
		offset -= change;

		memcpy(head, (char *)&offset, sizeof(int));
		step += sizeof(int);
		step += sizeof(BOOL);
		step += sizeof(int);

		memcpy((char *)&TitleLen, head+step, 4);
		step += sizeof(int);
		step += TitleLen;

		memcpy((char*)&UrlLen, head+step, 4);
		step += sizeof(int);
		step += UrlLen;

		head = head+step;
		Totalstep += step;
		step = 0;
	}

	return 1;
}

/*********************************************************************
* Function	WP_DeleteRecord
* Purpose   
* Parameter	
* Return	BOOL
* Remarks	
**********************************************************************/
BOOL WP_DeleteRecord(int offset)
{
	FILE *pFile, *pCopyFile;
	PUSHFILERECORD Record;
	int filesize;
	int total, unread;
	char *data;
	int left, right;

	pFile = fopen(FILE_PUSH_WAP, "rb");
	if (pFile == NULL)
		return FALSE;

	fseek(pFile, 0, SEEK_END);
	filesize = ftell(pFile);

	fseek(pFile, 0, SEEK_SET);
	fread(&total, sizeof(int), 1, pFile);
	fread(&unread, sizeof(int), 1, pFile);

	left = offset - 2*sizeof(int);
	if (left > 0)
	{
		data = malloc(left + 1);
		memset(data, 0, left + 1);
		if (data == NULL)
		{
			fclose(pFile);
			return FALSE;
		}
		fread(data, left, 1, pFile);
	}

	fread(&(Record.offset), sizeof(int), 1, pFile);
	fread(&(Record.status), sizeof(BOOL), 1, pFile);
	fread(&(Record.type), sizeof(int), 1, pFile);
	fread(&(Record.TitleLen), sizeof(int), 1, pFile);
	fseek(pFile, Record.TitleLen, SEEK_CUR);
	fread(&(Record.UrlLen), sizeof(int), 1, pFile);
	fseek(pFile, Record.UrlLen, SEEK_CUR);

	total--;
	if (Record.status == WP_PUSH_UNREAD)
		unread--;

	if (total == 0)
	{
		fclose(pFile);
		remove(FILE_PUSH_WAP);
		return TRUE;
	}

	pCopyFile = fopen(FILE_PUSH_BACKUP, "wb");
	if (pCopyFile == NULL)
	{
		fclose(pFile);
		return FALSE;
	}

	fwrite(&total, sizeof(int), 1, pCopyFile);
	fwrite(&unread, sizeof(int), 1, pCopyFile);
	if (left > 0)
	{
		fwrite(data, offset - 2*sizeof(int), 1, pCopyFile);
		free(data);
	}

	right = filesize - offset - 3*sizeof(int) - sizeof(BOOL) - sizeof(int) -
		Record.TitleLen - Record.UrlLen;

	if (right > 0)
	{
		data = malloc(right + 1);
		memset(data, 0, right + 1);
		if (data == NULL)
		{
			fclose(pFile);
			fclose(pCopyFile);

			remove(FILE_PUSH_BACKUP);
			return FALSE;
		}

		fread(data, right, 1, pFile);
		nDelNodeLen = 3*sizeof(int)+sizeof(BOOL)+sizeof(int)+Record.TitleLen+Record.UrlLen;
		WP_ChangeOffset(data, right, nDelNodeLen);

		fwrite(data, right, 1, pCopyFile);
		free(data);
	}

	fclose(pFile);
	fclose(pCopyFile);

	remove(FILE_PUSH_WAP);
	rename(FILE_PUSH_BACKUP, FILE_PUSH_WAP);

	return TRUE;
}

/*********************************************************************
* Function	WP_FindRecord
* Purpose   
* Parameter	
* Return	BOOL
* Remarks	
**********************************************************************/
/*
BOOL WP_FindRecord(int* pBuf, int nStart)
{
	FILE *pFile;
	int total, unread;
	PUSHFILERECORD Record;
	int i;

	pFile = fopen(FILE_PUSH_WAP, "rb");
	if (pFile == NULL)
		return FALSE;

	fread(&total, sizeof(int), 1, pFile);
	fread(&unread, sizeof(int), 1, pFile);

	for (i = 0; i <= nStart; i++)
	{
		fread(&(Record.offset), sizeof(int), 1, pFile);
		fread(&(Record.status), sizeof(BOOL), 1, pFile);
		fread(&(Record.type), sizeof(int), 1, pFile);

		fread(&(Record.TitleLen), sizeof(int), 1, pFile);
		fseek(pFile, Record.TitleLen, SEEK_CUR);
		fread(&(Record.UrlLen), sizeof(int), 1, pFile);
		fseek(pFile, Record.UrlLen, SEEK_CUR);
	}

	// 求偏移量
	*pBuf = Record.offset;

	fclose(pFile);
	return TRUE;
}*/


/*********************************************************************
* Function	WP_InitMsgHandle
* Purpose   
* Parameter	
* Return	void
* Remarks	
**********************************************************************/
void WP_InitMsgHandle(void)
{
	int i;

	for (i=0; i<MAX_PUSH_NUM; i++)
		PushMsgHandle[i] = -1;
}

/*********************************************************************
* Function	WP_MallocNode
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
int WP_MallocNode(void)
{
	int i;

	if (nPushNext == MAX_PUSH_NUM)
		return -1;

	for (i=0; i<MAX_PUSH_NUM; i++)
	{
		if (PushMsgHandle[i] == -1)
		{
			nPushNext++;
			return i;
		}
	}

	return -1;
}

/*********************************************************************
* Function	WP_FreeNode
* Purpose   
* Parameter	
* Return	BOOL
* Remarks	
**********************************************************************/
/*
BOOL WP_FreeNode(int MsgHandle, int offset)
{
	int i, j;

	for (i=0; i<MAX_PUSH_NUM; i++)
	{
		if (PushMsgHandle[i] == MsgHandle)
		{
			for (j=i+1; j<MAX_PUSH_NUM; j++)
			{
				if (PushMsgHandle[j] == -1)
					break;
				PushMsgHandle[j] -= offset;
			}

			PushMsgHandle[i] = -1;
			return TRUE;
		}
	}

	return FALSE;
}*/


/*********************************************************************
* Function	WP_NewPushArrival
* Purpose   
* Parameter	
* Return	void
* Remarks	
**********************************************************************/
void WP_NewPushArrival(int MsgHandle)
{
	int nSub;
	char *pSubject, *pAddress;
	PUSHFILERECORD Record;
	SYSTEMTIME sysTime;

	nSub = WP_MallocNode();
	if (nSub < 0)
		return;

	PushMsgHandle[nSub] = MsgHandle;
	//nType = INBOXGetType(PushMsgHandle[nSub]);
	//if (nType == CONTENT_TYPE_WAPSI || nType == CONTENT_TYPE_WAPSIC)
	{
		WP_GetRecord(PushMsgHandle[nSub], &Record);
		pSubject = Record.pszTitle;
		pAddress = Record.pszUri;
	}

	GetLocalTime(&sysTime);

	free(Record.pszTitle);
	free(Record.pszUri);

}

/*********************************************************************
* Function	WP_FillMsgNode
* Purpose   
* Parameter	
* Return	BOOL
* Remarks	
**********************************************************************/
/*BOOL WP_FillMsgNode(MU_MsgNode* msgnode, int status, SYSTEMTIME date, int handle, PCSTR subject, PCSTR address)
{
	int subLen, addrlen;
	int subWidth, addrWidth;
	int nFit;

	if (msgnode == NULL)
		return FALSE;

	if (subject == NULL || address == NULL)
		return FALSE;

	msgnode->msgtype = MU_MSG_PUSH;
	msgnode->status = status;
	msgnode->storage_type = MU_STORE_IN_FLASH;
	msgnode->maskdate = Datetime2INT(date.wYear, date.wMonth, date.wDay, date.wHour, date.wMinute, date.wSecond);
	msgnode->handle = handle;

	subLen = strlen(subject);
	if (subLen >= MU_SUBJECT_DIS_LEN)
	{
		subWidth = ENG_FONT_WIDTH * MU_SUBJECT_DIS_LEN;
		GetTextExtentExPoint(NULL, subject, subLen, subWidth, &nFit, NULL, NULL);
		strncpy(msgnode->subject, subject, nFit);
		msgnode->subject[nFit] = '\0';
	}
	else
		strcpy(msgnode->subject, subject);

	addrlen = strlen(address);
	if (addrlen >= MU_SUBJECT_DIS_LEN)
	{
		addrWidth = ENG_FONT_WIDTH * MU_SUBJECT_DIS_LEN;
		GetTextExtentExPoint(NULL, address, addrlen, addrWidth, &nFit, NULL, NULL);
		strncpy(msgnode->addr, address, nFit);
		msgnode->addr[nFit] = '\0';
	}
	else
		strcpy(msgnode->subject, address);

	return TRUE;
}*/

BOOL WP_GetHandleReady(HWND hWnd)
{
	FILE *pFile;
	int total, unread, i = 0, j = 0;
	int boollen, intlen;
	PUSHFILERECORD record;
//	char *szMulti;
//	int nLenMulti;

	pFile = fopen(FILE_PUSH_WAP, "rb");
	if (pFile == NULL)
		return FALSE;

	boollen = sizeof(BOOL);
	intlen = sizeof(int);

	SendMessage(hWnd, LB_RESETCONTENT, NULL, NULL);
	
	// 第一个四字节位置
	fread(&total, intlen, 1, pFile);
	fread(&unread, intlen, 1, pFile);
	fread(&record.offset, intlen, 1, pFile);
	
	do
	{
		PushMsgHandle[i] = record.offset;
		fread(&record.status, boollen, 1, pFile);
		fseek(pFile, intlen, SEEK_CUR);
		
		fread(&record.TitleLen, intlen, 1, pFile);
		record.pszTitle = (char *)malloc(record.TitleLen + 1);
		memset(record.pszTitle, 0, record.TitleLen + 1);
		fread(record.pszTitle, record.TitleLen, 1, pFile);

//		nLenMulti = UTF8ToMultiByte(CP_ACP, 0, record.pszTitle, -1, NULL, 0, NULL, NULL);
//		szMulti = (char *)malloc(nLenMulti + 1);
//		memset(szMulti, 0, nLenMulti + 1);
//		nLenMulti = UTF8ToMultiByte(CP_ACP, 0, record.pszTitle, -1, szMulti, nLenMulti, NULL, NULL);
//		szMulti[nLenMulti] = '\0';
		
		SendMessage(hWnd, LB_ADDSTRING, j, (LPARAM)record.pszTitle);
		
		fread(&record.UrlLen, intlen, 1, pFile);
		record.pszUri = (char *)malloc(record.UrlLen + 1);
		memset(record.pszUri, 0, record.UrlLen + 1);
		fread(record.pszUri, record.UrlLen, 1, pFile);
		SendMessage(hWnd, LB_SETAUXTEXT, (WPARAM)MAKEWPARAM(j, -1), (LPARAM)(LPSTR)(record.pszUri));

		if(record.status == 0)
			SendMessage(hWnd, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, j), (LPARAM)hreadpushbmp);
		else if(record.status == 1)
			SendMessage(hWnd, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, j), (LPARAM)hunreadpushbmp);

		j++;
		free(record.pszTitle);
		free(record.pszUri);
//		free(szMulti);
	}while ((fread(&record.offset, intlen, 1, pFile) != 0) && (i++ < MAX_PUSH_NUM));
	
	if(i != (MAX_PUSH_NUM - 1))
	{
		for(i; i < MAX_PUSH_NUM; i++)
			PushMsgHandle[i + 1] = -1;
	}

	fclose(pFile);
	return TRUE;
}

/*********************************************************************
* Function	TestPush
* Purpose   
* Parameter	
* Return	void
* Remarks	
**********************************************************************/

static int number = 0;

void TestPush(void)
{
	PUSHFILERECORD Record;
	
	Record.status = MU_STU_UNREAD;
	Record.type = CONTENT_TYPE_WAPSIC;
	
	Record.TitleLen = 4;
	Record.UrlLen = 40;
	
	Record.pszTitle = malloc(Record.TitleLen+1);
	if (Record.pszTitle == NULL)
		return;
	sprintf(Record.pszTitle, "%d", ++number);
	
	Record.pszUri = malloc(40+1);
	if (Record.pszUri == NULL)
	{
		free(Record.pszTitle);
		return;
	}
	strcpy(Record.pszUri, "0123456789012345678901234567890123456789");
	
	// 插入一条记录
	WP_InsertRecord(&Record);
	
	free(Record.pszTitle);
	free(Record.pszUri);
}



