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

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "time.h"

#include "BtFileOper.h"

#define BTRECVDATAPATH    "/mnt/flash/bluetooth/inbox"

void GetFileNameNoPath(char * fNameWithPath, char * fName)
{
	char * p1, * p2;
	
	p1 = p2 = fNameWithPath;
	while (*p2 != '\0'){
		if(*p2 == 47 || *p2 == 92 || *p2 == ':')	//47 左斜杠 92 右斜杠
			p1 = p2;
		p2++;
	}
	if(p1 != fNameWithPath)
		strcpy(fName, p1 + 1);
	else
		strcpy(fName, p1);
}


void GetFilePathNoName(char * fNameWithPath, char * fPath)
{
	char * p1, * p2;
	
	p1 = p2 = fNameWithPath;
	while (*p2 != '\0'){
		if(*p2 == 47 || *p2 == 92 || *p2 == ':')	
			p1 = p2;
		p2++;
	}
	strncpy(fPath, fNameWithPath, p1 - fNameWithPath + 1);
	fPath[p1 - fNameWithPath + 1] = '\0';
}

void GetSuffixFileName(char * szFullName, char * szName, char * szSuffix)
{
	char * p;
	int len;
	
	len = strlen(szFullName);
	p = szFullName + len;
	
	while(*p != '.' && p != szFullName){
		p--;
		len--;
	}
	
	if(szName != NULL){
		if (p == szFullName) {
			if(*p == '.')
				szName[0] = '\0';
			else
				strcpy(szName, szFullName);
		}
		else{
			strncpy(szName, szFullName, p - szFullName);
			szName[p - szFullName] = '\0';
		}
	}
	if(szSuffix != NULL){
		if(p == szFullName){
			szSuffix[0] = '\0';
		}
		else
			strcpy(szSuffix, p + 1);
	}
}

void RenameFileName(char *OldFileName, char *NewFileName)
{
	char szFilename[64];  //文件名，不含路径
	char szSuffix[32];  //后缀名

	int nRand;
	
	srand(time(0));
	nRand = rand();

	GetSuffixFileName(OldFileName, szFilename, szSuffix);

// 	sprintf(NewFileName, "%s%s%d%s%s", BTRECVDATAPATH, szFilename, nRand, ".", szSuffix);
    
	sprintf(NewFileName, "%s%d%s%s", szFilename, nRand, ".", szSuffix);
}
