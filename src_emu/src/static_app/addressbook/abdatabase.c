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

static DWORD idMemory;

extern int nName,nTel,nEmail;

BOOL AB_SaveRecord(DWORD* dwoffset,DWORD* id,int *pnGroup,BOOL bSaveGroup,int nTelID,CONTACT_ITEMCHAIN* pItem);
BOOL AB_ChangeGroup(DWORD* pdwoffset,DWORD* pid,int *pnGroup);
DWORD AB_GetUnusedID(void);
void AB_SetIDMemory(DWORD id);
DWORD AB_GetIDMemory(void);
void AB_LoadIDMemory(void);
BOOL AB_SaveIDMemory(void);
static LONG AB_GetNeedMemorySize(const char* PSZFILENAME);
static BOOL AB_GetUseableFileName(int nRecordLen,char* pszFileName);
static BOOL AB_GetUnusedFileName(char* pszFileName);
/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL AB_SaveRecord(DWORD* pdwoffset,DWORD* pid,int *pnGroup,BOOL bSaveGroup,int nTelID,CONTACT_ITEMCHAIN* pItem)
{
    CONTACT_DBHEADER dbhead;
    CONTACT_ITEMCHAIN *ptemp;
    char szOldPath[PATH_MAXLEN];
    int  f,fold;
    int  nUnUsed = AB_UNUSED;
    struct stat buf;
    BYTE byDataLen = 0;
    char szFileName[PATH_MAXLEN];
    char szOldFileName[PATH_MAXLEN];
    BOOL bFind = FALSE;
    int  nRecordLen = 0;
    int  nFileID;
    char* p;
    DWORD dwrealoffset;
    BYTE utf8len;
    char szutf8[UTF8_STRLEN];
    BOOL bSameFile;

#ifndef _EMULATE_
	KickDog();
#endif

    szFileName[0] = 0;

    szOldPath[0] = 0;
    
    getcwd(szOldPath,PATH_MAXLEN);  
    
    chdir(PATH_DIR_AB);

    ptemp = pItem;
    
    nRecordLen = 0;

    while(ptemp)
    {
        if(ptemp->dwMask == AB_TYPEMASK_DATE)
        {
            nRecordLen += (sizeof(DWORD) + sizeof(SYSTEMTIME));
        }
        else
        {
            if(strlen(ptemp->pszData) > 0)
            {
                nRecordLen += sizeof(DWORD);
                
                if(AB_IsTel(ptemp->dwMask) || AB_IsEmail(ptemp->dwMask))
                    nRecordLen += sizeof(int);
                
                nRecordLen += sizeof(BYTE);
                
                utf8len = (BYTE)MultiByteToUTF8(CP_ACP, 0, (LPCSTR)ptemp->pszData, -1, (LPSTR)NULL, 0);

                nRecordLen += utf8len+1;
            }
        }
        ptemp = ptemp->pNext;
    }

    if( AB_GetUseableFileName(nRecordLen + sizeof(CONTACT_DBHEADER),szFileName) )
        bFind = TRUE;
    
    memset(&dbhead,0,sizeof(CONTACT_DBHEADER));

    if(bFind == FALSE)
    {
        AB_GetUnusedFileName(szFileName);
    }
    
    f = open(szFileName, O_RDWR | O_CREAT,S_IRWXU);

    if( f == -1 )
    {
        chdir(szOldPath);
        
        return FALSE;
    }

    if(*pdwoffset == -1)
    {
        dbhead.Used = AB_USED;
     
        dbhead.id = AB_GetUnusedID();

        *pid = dbhead.id;
        
        if(bSaveGroup)
            dbhead.nGroup = *pnGroup;
        else
        {
            dbhead.nGroup = -1;
            
            *pnGroup = dbhead.nGroup;
        }
    }
    else
    {
        dwrealoffset = *pdwoffset & 0x00ffffff;
        
        szOldFileName[0] = 0;
        
        nFileID = (int)(*pdwoffset >> 24);
        
        sprintf(szOldFileName,"%d%s",nFileID,PATH_FILE_CONTACT);
        
        if(stricmp(szOldFileName,szFileName) != 0)
            bSameFile = FALSE;
        else
            bSameFile = TRUE;

        if(bSameFile == FALSE)
        {
            fold = open(szOldFileName, O_RDWR ,S_IRWXU);

            if(fold == -1)
            {
                close(f);

                return FALSE;
            }
        }
        else
            fold = f;

        lseek(fold,dwrealoffset,SEEK_SET);
        
        read(fold,&dbhead,sizeof(CONTACT_DBHEADER));

        lseek(fold,dwrealoffset,SEEK_SET);

        write(fold,&nUnUsed,sizeof(int));

        if(bSaveGroup)
            dbhead.nGroup = *pnGroup;
        else
            *pnGroup = dbhead.nGroup;

        if(bSameFile == FALSE)
            close(fold);
    }

	dbhead.Used = AB_USED;

    dbhead.nTelID = nTelID;

    dbhead.dwOffset = nRecordLen;
             
    memset(&buf,0,sizeof(struct stat));
    
    stat(szFileName,(struct stat*)&buf);
    
    lseek(f,buf.st_size,SEEK_SET);

    write(f,&dbhead,sizeof(CONTACT_DBHEADER));

    ptemp = pItem;

    while(ptemp)
    {
        if(ptemp->dwMask == AB_TYPEMASK_DATE)
        {
            write(f,&(ptemp->dwMask),sizeof(DWORD));
            
            write(f,(SYSTEMTIME*)(ptemp->pszData),sizeof(SYSTEMTIME));
        }
        else
        {
            byDataLen = strlen(ptemp->pszData);
            
            if( byDataLen > 0)
            {
                memset(szutf8,'\0',UTF8_STRLEN);

                utf8len = (BYTE)MultiByteToUTF8(CP_ACP, 0, (LPCSTR)ptemp->pszData, -1, (LPSTR)szutf8, UTF8_STRLEN);

                utf8len += 1;
                
                write(f,&(ptemp->dwMask),sizeof(DWORD));
                
                if(AB_IsTel(ptemp->dwMask) || AB_IsEmail(ptemp->dwMask))
                    write(f,&(ptemp->nID),sizeof(int));
                
                write(f,&utf8len,sizeof(BYTE));
                
                write(f,szutf8,utf8len);
            }
        }
        ptemp = ptemp->pNext;
    }

    close(f);

    chdir(szOldPath);

    *pdwoffset = buf.st_size;

    p = strstr(szFileName,PATH_FILE_CONTACT);

    if(p != NULL)
        *p = '\0';
    else
        return FALSE;

    nFileID = atoi(szFileName);

    *pdwoffset |= (nFileID << 24);

    return TRUE;
}

BOOL AB_ChangeGroup(DWORD* pdwoffset,DWORD* pid,int *pnGroup)
{
	DWORD dwrealoffset;
	char szFileName [PATH_MAXLEN];
	int hFile;
	int nFileID;
	int len;
	CONTACT_DBHEADER dbhead;

	dwrealoffset = *pdwoffset & 0x00ffffff;
	
	szFileName[0] = 0;
	
	nFileID = (int)(*pdwoffset >> 24);
	
	sprintf(szFileName,"%d%s",nFileID,PATH_FILE_CONTACT);
	chdir(PATH_DIR_AB);
	hFile = open(szFileName, O_RDWR ,S_IRWXU);
	
	if(hFile == -1)
	{		
		return FALSE;
	}

	lseek(hFile,dwrealoffset,SEEK_SET);
        
    read(hFile,&dbhead,sizeof(CONTACT_DBHEADER));
	
	if(dbhead.id == *pid)
	{
		dbhead.nGroup = *pnGroup;
		lseek(hFile,dwrealoffset,SEEK_SET);
		len = write(hFile,&dbhead,sizeof(CONTACT_DBHEADER));
		close(hFile);
	}
	else
	{
		close(hFile);
		return FALSE;
	}

	return TRUE;
}
/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL AB_ReadRecord(DWORD dwoffset,PCONTACT_ITEMCHAIN* ppItem,int * pnTelID)
{
    CONTACT_DBHEADER dbhead;
    char szOldPath[PATH_MAXLEN];
    PCONTACT_ITEMCHAIN pNewItem = NULL;
    int  f;
    int nIndex;
    DWORD dwMask;
    LONG  dwDataLen = 0;
    BYTE bydatalen = 0;
    int  nFileID;
    char szFileName[PATH_MAXLEN];
    DWORD dwrealoffset;
	int  nTelID = 0;
    BYTE widecharlen;
    char szutf8[UTF8_STRLEN];
	
#ifndef _EMULATE_
	KickDog();
#endif
	
    szOldPath[0] = 0;

    szFileName[0] = 0;

    nFileID = (int)(dwoffset >> 24);

    dwrealoffset = dwoffset & 0x00ffffff;
    
    sprintf(szFileName,"%d%s",nFileID,PATH_FILE_CONTACT);
        
    memset(&dbhead,0,sizeof(CONTACT_DBHEADER));

    getcwd(szOldPath,PATH_MAXLEN);  
    
    chdir(PATH_DIR_AB);
    
    f = open(szFileName, O_RDWR | O_CREAT,S_IRWXU);

    if( f == -1 )
    {
        chdir(szOldPath);
        
        return FALSE;
    }

    lseek(f,dwrealoffset,SEEK_SET);
        
    read(f,&dbhead,sizeof(CONTACT_DBHEADER));

	*pnTelID = dbhead.nTelID;

    dwDataLen = dbhead.dwOffset;

    while(dwDataLen > 0)
    {
        read(f,&dwMask,sizeof(DWORD));

        switch(dwMask)
        {
        case AB_TYPEMASK_PIC:
        case AB_TYPEMASK_TONE:
            {
                
                read(f,&bydatalen,sizeof(BYTE));
                
                pNewItem = (CONTACT_ITEMCHAIN*)malloc(sizeof(CONTACT_ITEMCHAIN));
                
                if(pNewItem == NULL)
                {
                    Item_Erase(*ppItem);
                    
                    close(f);
                    
                    chdir(szOldPath);
                    
                    return FALSE;
                }
                
                memset(pNewItem,0,sizeof(CONTACT_ITEMCHAIN));
                
                read(f,szutf8,bydatalen);

                widecharlen = UTF8ToMultiByte(CP_ACP, 0, szutf8, -1, NULL, 0, NULL, NULL);
                
                pNewItem->pszData = (char*)malloc(widecharlen+1);
                
                if(pNewItem->pszData == NULL)
                {
                    Item_Erase(*ppItem);
                    
                    free(pNewItem);
                    
                    close(f);
                    
                    chdir(szOldPath);
                    
                    return FALSE;
                }
                
                pNewItem->dwMask = dwMask;

                UTF8ToMultiByte(CP_ACP, 0, szutf8, -1, pNewItem->pszData, widecharlen+1, NULL, NULL);

                pNewItem->nMaxLen = widecharlen+1;
                
                Item_Insert(ppItem,pNewItem);
                
                dwDataLen -= (bydatalen + sizeof(DWORD) + sizeof(BYTE));
            }
        	break;

        case AB_TYPEMASK_DATE:
            {
                nIndex = GetIndexByMask(dwMask);
                
                pNewItem = Item_New(Contact_Item[nIndex].dwMask);
                
                if(pNewItem == NULL)
                {
                    Item_Erase(*ppItem);
                    
                    close(f);
                    
                    chdir(szOldPath);
                    
                    return FALSE;
                }
                
                read(f,(SYSTEMTIME*)(pNewItem->pszData),sizeof(SYSTEMTIME));
                
                Item_Insert(ppItem,pNewItem);
                
                dwDataLen -= (sizeof(DWORD) + sizeof(SYSTEMTIME));
            }
            break;
            
        default:
            {
                
                if(AB_IsTel(dwMask) || AB_IsEmail(dwMask))
                {
                    read(f,&nTelID,sizeof(int));
                }
                
                read(f,&bydatalen,sizeof(BYTE));
                
                nIndex = GetIndexByMask(dwMask);
                
                pNewItem = Item_New(Contact_Item[nIndex].dwMask);
                
                if(pNewItem == NULL)
                {
                    Item_Erase(*ppItem);
                    
                    close(f);
                    
                    chdir(szOldPath);
                    
                    return FALSE;
                }
                pNewItem->nID = nTelID;
                
                read(f,szutf8,bydatalen);
                
                widecharlen = UTF8ToMultiByte(CP_ACP, 0, szutf8, -1, pNewItem->pszData, pNewItem->nMaxLen, NULL, NULL);
                
                Item_Insert(ppItem,pNewItem);
                
                if(AB_IsTel(dwMask) || AB_IsEmail(dwMask))
                    dwDataLen -= (bydatalen + sizeof(DWORD) + sizeof(int) + sizeof(BYTE));
                else
                    dwDataLen -= (bydatalen + sizeof(DWORD) + sizeof(BYTE));
            }
            break;
        }
    }

    close(f);

    chdir(szOldPath);

    return TRUE;
}
/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
DWORD AB_GetUnusedID(void)
{
    DWORD id;

    id = AB_GetIDMemory();

    id++;

    AB_SetIDMemory(id);

    return id;
}
/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
void AB_SetIDMemory(DWORD id)
{
    idMemory = id;

    AB_SaveIDMemory();
}
/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
DWORD AB_GetIDMemory(void)
{
    return idMemory;
}
/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
void AB_LoadIDMemory(void)
{
    char szOldPath[PATH_MAXLEN];
    int  f;
    DWORD id;

    szOldPath[0] = 0;

    getcwd(szOldPath,PATH_MAXLEN);  
    
    chdir(PATH_DIR_AB);
    
    f = open(PATH_FILE_ID,O_RDONLY);

    if( f == -1 )
        AB_SetIDMemory(0);
    else
    {
        read(f,&id,sizeof(DWORD));

        AB_SetIDMemory(id);

        close(f);
    }
    
    chdir(szOldPath);
}
/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL AB_SaveIDMemory(void)
{
    char szOldPath[PATH_MAXLEN];
    int  f;
    DWORD id;

    szOldPath[0] = 0;

    getcwd(szOldPath,PATH_MAXLEN);  
    
    chdir(PATH_DIR_AB);
    
    f = open(PATH_FILE_ID, O_RDWR | O_CREAT,S_IRWXU);
 
    if( f == -1 )
    {
        chdir(szOldPath);
        
        return FALSE;
    }
    else
    {
        id = AB_GetIDMemory();

        write(f,&id,sizeof(DWORD));

        close(f);
    }
    
    chdir(szOldPath);

    return TRUE;
}
/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL AB_InitData(void)
{
    char szOldPath[PATH_MAXLEN];
    int  f;
    struct stat buf;
    long dwDataLen = 0,dwPerDataLen = 0;
    char szTemp[AB_MAXLEN_EMAIL];
    char szName[AB_MAXLEN_FIRSTNAME*2];
    CONTACT_DBHEADER dbhead;
    DWORD dwMask;
    int nIndex;
    LONG nMemorySize = 0;
    BYTE bydatalen = 0;
    int  n;
    DIR* pDir;
    struct dirent* pData;
    DWORD dwrealoffset;
    char szFileName[PATH_MAXLEN];
    char* p;
    int  nFileID;
	int  nTelID = 0;
    char szutf8[UTF8_STRLEN];

    pNotifyHeader = NULL;

    getcwd(szOldPath,PATH_MAXLEN);  
    
    chdir(PATH_DIR_AB);
    
    pDir = opendir(PATH_DIR_AB);
    
    if(pDir != NULL)
    {
        while( (pData = readdir(pDir)) != NULL )
        {
            if(strstr(pData->d_name,PATH_FILE_CONTACT) != NULL)
            {   
                nMemorySize += AB_GetNeedMemorySize(pData->d_name);
            }
        }
        closedir(pDir);
    }

    n = nMemorySize/1024 + 1;

    n += MEMORY_BASE_SIZE;

    if(n > MEMORY_MAX_SIZE)
        n = MEMORY_MAX_SIZE;
    
    if(AB_MallocMemory(n*MEMORY_100K+MEMORY_LINK_SIZE) == FALSE)
    {
        chdir(szOldPath);

        return FALSE;
    }

    AB_LoadIDMemory();

    AB_LoadGroupID();
    
    pDir = opendir(PATH_DIR_AB);
    
    if(pDir != NULL)
    {
        while( (pData = readdir(pDir)) != NULL )
        {
            if(strstr(pData->d_name,PATH_FILE_CONTACT) != NULL)
            {    
                strcpy(szFileName,pData->d_name);

                p = strstr(szFileName,PATH_FILE_CONTACT);
                
                if(p != NULL)
                    *p = '\0';
                
                nFileID = atoi(szFileName);

                memset(&buf,0,sizeof(struct stat));
                
                stat(pData->d_name,(struct stat*)&buf);
                
                szOldPath[0] = 0;
                
                f = open(pData->d_name, O_RDWR | O_CREAT,S_IRWXU);
                
                if( f == -1 )
                {
                    closedir(pDir);

                    chdir(szOldPath);
                    
                    return FALSE;
                }
                
                dwDataLen = buf.st_size;
                
                while(dwDataLen > 0)
                {
                    read(f,&dbhead,sizeof(CONTACT_DBHEADER));
                    
                    if(dbhead.Used == AB_USED)
                    {
                        dwPerDataLen = dbhead.dwOffset;
                        
                        szName[0] = 0;
                        
                        while(dwPerDataLen > 0)
                        {
                            szTemp[0] = 0;
                            
                            read(f,&dwMask,sizeof(DWORD));
                            
                            if(dwMask != AB_TYPEMASK_DATE)
                            {
                                if(AB_IsTel(dwMask) || AB_IsEmail(dwMask))
                                    read(f,&nTelID,sizeof(int));
                                
                                read(f,&bydatalen,sizeof(BYTE));
                            }

                            nIndex = GetIndexByMask(dwMask);
                            
                            switch(dwMask)
                            {
                            case AB_TYPEMASK_FIRSTNAME:
                                read(f,szutf8,bydatalen);

                                UTF8ToMultiByte(CP_ACP, 0, szutf8, bydatalen, szTemp, AB_MAXLEN_EMAIL, NULL, NULL);
                                
                                if(strlen(szName) == 0)
                                    strcpy(szName,szTemp);
                                else
                                {
#ifndef LANGUAGE_CHN
                                    strcat(szName," ");
#else
#endif
                                    strcat(szName,szTemp);
                                }
                                break;
                                
                            case AB_TYPEMASK_LASTNAME:
                                if(strlen(szName) == 0)
                                {
                                    read(f,szutf8,bydatalen);
                                    
                                    UTF8ToMultiByte(CP_ACP, 0, szutf8, bydatalen, szName, AB_MAXLEN_FIRSTNAME*2, NULL, NULL);
                                }
                                else
                                {
                                    strcpy(szTemp,szName);
                                    
                                    read(f,szutf8,bydatalen);
                                    
                                    UTF8ToMultiByte(CP_ACP, 0, szutf8, bydatalen, szName, AB_MAXLEN_FIRSTNAME*2, NULL, NULL);
#ifndef LANGUAGE_CHN                                    
                                    strcat(szName," ");
#else
#endif
                                    strcat(szName,szTemp);
                                }
                                break;
                                
                            case AB_TYPEMASK_TEL:
                            case AB_TYPEMASK_TELHOME:
                            case AB_TYPEMASK_TELWORK:
                            case AB_TYPEMASK_MOBILE:
                            case AB_TYPEMASK_MOBILEHOME:
                            case AB_TYPEMASK_MOBILEWORK:
                            case AB_TYPEMASK_FAX:
                            case AB_TYPEMASK_PAGER:
                            case AB_TYPEMASK_COMPANYTEL:
                            case AB_TYPEMASK_DTMF:
                                
                                read(f,szutf8,bydatalen);
                                
                                UTF8ToMultiByte(CP_ACP, 0, szutf8, bydatalen, szTemp, AB_MAXLEN_FIRSTNAME, NULL, NULL);
                                
                                AB_InsertTel(dbhead.id,nTelID,(BYTE)(strlen(szTemp)+1),szTemp);
                                break;
                                
                            case AB_TYPEMASK_EMAIL:
                            case AB_TYPEMASK_EMAILWORK:
                            case AB_TYPEMASK_EMAILHOME:
                                
                                read(f,szutf8,bydatalen);
                                
                                UTF8ToMultiByte(CP_ACP, 0, szutf8, bydatalen, szTemp, AB_MAXLEN_FIRSTNAME, NULL, NULL);
                                
                                AB_InsertEmail(dbhead.id,nTelID,(BYTE)(strlen(szTemp)+1),szTemp);
                                break;

                            case AB_TYPEMASK_DATE:
                                lseek(f,sizeof(SYSTEMTIME),SEEK_CUR);
                                break;
                                
                            default:
                                lseek(f,bydatalen,SEEK_CUR);
                                break;
                                
                            }
                            
                            if(dwMask == AB_TYPEMASK_DATE)
                                dwPerDataLen -= (sizeof(DWORD) + sizeof(SYSTEMTIME));
							else if(AB_IsTel(dwMask) || AB_IsEmail(dwMask))
								dwPerDataLen -= (bydatalen + sizeof(BYTE) + sizeof(int) + sizeof(DWORD));
							else
								dwPerDataLen -= (bydatalen + sizeof(BYTE) + sizeof(DWORD));
                        }
                        
                        dwrealoffset = buf.st_size-dwDataLen;                
                        
                        dwrealoffset |= nFileID << 24;
                        
                        AB_InsertName(dwrealoffset,dbhead.id,dbhead.nGroup,(BYTE)(strlen(szName)+1),szName);
                    }
                    else
                    {
                        lseek(f,dbhead.dwOffset,SEEK_CUR);
                    }
                    
                    dwDataLen -= (sizeof(CONTACT_DBHEADER) + dbhead.dwOffset);
                }
                
                close(f);
            }
        }
        closedir(pDir);
    }

    chdir(szOldPath);

    AB_HeapSort(pIndexID,g_CmpFunc[3],nID);
    
    AB_HeapSort(pIndexName,g_CmpFunc[0],nName);

    AB_HeapSort(pIndexTel,g_CmpFunc[1],nTel);

    AB_HeapSort(pIndexEmail,g_CmpFunc[2],nEmail);

    return TRUE;
}
/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static LONG AB_GetNeedMemorySize(const char* PSZFILENAME)
{
    char szOldPath[PATH_MAXLEN];
    int  f;
    struct stat buf;
    long dwDataLen = 0,dwPerDataLen = 0;
    CONTACT_DBHEADER dbhead;
    DWORD dwMask;
    int nIndex;
    BYTE bydatalen = 0;
    LONG lSize = 0;
	int  nTelID = 0;
    BOOL bName;
    
    getcwd(szOldPath,PATH_MAXLEN);  
    
    chdir(PATH_DIR_AB);

    memset(&buf,0,sizeof(struct stat));
    
    stat(PSZFILENAME,(struct stat*)&buf);

    szOldPath[0] = 0;
    
    f = open(PSZFILENAME, O_RDWR | O_CREAT,S_IRWXU);

    if( f == -1 )
    {
        chdir(szOldPath);
        
        return -1;
    }
    
    dwDataLen = buf.st_size;

    while(dwDataLen > 0)
    {
        read(f,&dbhead,sizeof(CONTACT_DBHEADER));
        
        if(dbhead.Used == AB_USED)
        {        
            dwPerDataLen = dbhead.dwOffset;
            
            bydatalen = 0;
            
            while(dwPerDataLen > 0)
            {           
                bName = FALSE;

                read(f,&dwMask,sizeof(DWORD));
				
                if(dwMask != AB_TYPEMASK_DATE)
                {
                    if(AB_IsTel(dwMask) || AB_IsEmail(dwMask))
                        read(f,&nTelID,sizeof(int));
                    
                    read(f,&bydatalen,sizeof(BYTE));
                }
                
                nIndex = GetIndexByMask(dwMask);
                
                switch(dwMask)
                {
                case AB_TYPEMASK_FIRSTNAME:
                case AB_TYPEMASK_LASTNAME:
                    lSize += bydatalen;
                    break;

                case AB_TYPEMASK_TEL:
                case AB_TYPEMASK_TELHOME:
                case AB_TYPEMASK_TELWORK:
                case AB_TYPEMASK_MOBILE:
                case AB_TYPEMASK_MOBILEHOME:
                case AB_TYPEMASK_MOBILEWORK:
                case AB_TYPEMASK_FAX:
                case AB_TYPEMASK_PAGER:
                case AB_TYPEMASK_COMPANYTEL:
                case AB_TYPEMASK_DTMF:
                case AB_TYPEMASK_EMAIL:
                case AB_TYPEMASK_EMAILWORK:
                case AB_TYPEMASK_EMAILHOME:
                    lSize += bydatalen;
                    lSize += sizeof(DWORD) + sizeof(int) + sizeof(BYTE);
                    break;
                    
                default:
                    break;
                    
                }
                if(dwMask == AB_TYPEMASK_DATE)
                    lseek(f,sizeof(SYSTEMTIME),SEEK_CUR);
                else
                    lseek(f,bydatalen,SEEK_CUR);

                lSize += sizeof(DWORD)*2 + sizeof(int) + sizeof(BYTE);
             
                if(dwMask == AB_TYPEMASK_DATE)
                    dwPerDataLen -= (sizeof(DWORD)+sizeof(SYSTEMTIME));
				else if(AB_IsTel(dwMask) || AB_IsEmail(dwMask))
					dwPerDataLen -= (sizeof(DWORD)+sizeof(int)+sizeof(BYTE)+bydatalen);
				else
					dwPerDataLen -= (sizeof(DWORD)+sizeof(BYTE)+bydatalen);
                
                lSize += AB_MEMORY_SNAP;
            }
        }
        else
        {
            lseek(f,dbhead.dwOffset,SEEK_CUR);
        }

        dwDataLen -= (sizeof(CONTACT_DBHEADER) + dbhead.dwOffset);
    }

    close(f);

    chdir(szOldPath);

    return lSize;
}
/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL AB_GetUseableFileName(int nRecordLen,char* pszFileName)
{
    char szOldPath[PATH_MAXLEN];
    struct stat buf;
    DIR* pDir;
    struct dirent* pData;

    getcwd(szOldPath,PATH_MAXLEN);  
    
    chdir(PATH_DIR_AB);

    pDir = opendir(PATH_DIR_AB);
    
    if(pDir != NULL)
    {
        while( (pData = readdir(pDir)) != NULL )
        {
            if(strstr(pData->d_name,PATH_FILE_CONTACT) != NULL)
            {
                memset(&buf,0,sizeof(struct stat));
                
                stat(pData->d_name,(struct stat*)&buf);
                
                if((buf.st_size + nRecordLen) <= MEMORY_100K)
                {
                    strcpy(pszFileName,pData->d_name);

                    closedir(pDir);

                    chdir(szOldPath);

                    return TRUE;
                }
            }
        }
        closedir(pDir);
    }

    return FALSE;
}
/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL AB_GetUnusedFileName(char* pszFileName)
{
    char szOldPath[PATH_MAXLEN];
    int  n = 1;
    int  f;

    getcwd(szOldPath,PATH_MAXLEN);  
    
    chdir(PATH_DIR_AB);

    while( TRUE )
    {
        pszFileName[0] = 0;

        sprintf(pszFileName,"%d%s",n,PATH_FILE_CONTACT);

        f = open(pszFileName,O_RDONLY);

        if(f == -1)
        {
            chdir(szOldPath);
            
            return TRUE;
        }

        close(f);

        n++;
    }

    chdir(szOldPath);

    return FALSE;
}

/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL AB_DeleteRecord(DWORD dwoffset)
{
    char szOldPath[PATH_MAXLEN];
    int  f;
    int  nFileID;
    char szFileName[PATH_MAXLEN];
    DWORD dwrealoffset;
    int  nUnUsed = AB_UNUSED;

#ifndef _EMULATE_
	KickDog();
#endif

    szOldPath[0] = 0;

    szFileName[0] = 0;

    nFileID = (int)(dwoffset >> 24);

	printf("AB_DeleteRecord dwoffset = 0x%x  nFileID = %d\r\n", dwoffset, nFileID);

    dwrealoffset = dwoffset & 0x00ffffff;
    
    sprintf(szFileName,"%d%s",nFileID,PATH_FILE_CONTACT);

    getcwd(szOldPath,PATH_MAXLEN);  
    
    chdir(PATH_DIR_AB);
    
    f = open(szFileName, O_RDWR | O_CREAT,S_IRWXU);

    if( f == -1 )
    {
        chdir(szOldPath);
        
        return FALSE;
    }

    lseek(f,dwrealoffset,SEEK_SET);
        
    write(f,&nUnUsed,sizeof(int));

    close(f);

    chdir(szOldPath);

    return TRUE;
}
/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL AB_DeleteAllFile(void)
{
    char szOldPath[PATH_MAXLEN];
    DIR* pDir;
    struct dirent* pData;
	
#ifndef _EMULATE_
	KickDog();
#endif

    getcwd(szOldPath,PATH_MAXLEN);  
    
    chdir(PATH_DIR_AB);

    pDir = opendir(PATH_DIR_AB);
    
    if(pDir != NULL)
    {
        while( (pData = readdir(pDir)) != NULL )
        {
            if(strstr(pData->d_name,PATH_FILE_CONTACT) != NULL)
            {
                remove(pData->d_name);
            }
        }
        closedir(pDir);
    }

    chdir(szOldPath);

    return TRUE;
}
/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL AB_ReadQDial(PAB_QDIAL pQdial)
{
    char szOldPath[PATH_MAXLEN];
	int f;

    getcwd(szOldPath,PATH_MAXLEN);  
    
    chdir(PATH_DIR_AB);

    f = open(PATH_FILE_ABQDIAL,O_RDONLY);
    
	if(f == -1)
		return FALSE;

	read(f,pQdial,sizeof(AB_QDIAL)*AB_QDIAL_MAXNUM);

	close(f);

    chdir(szOldPath);

    return TRUE;
}
/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL AB_WriteQDial(PAB_QDIAL pQdial)
{
    char szOldPath[PATH_MAXLEN];
	int f;

    getcwd(szOldPath,PATH_MAXLEN);  
    
    chdir(PATH_DIR_AB);

    f = open(PATH_FILE_ABQDIAL,O_WRONLY|O_CREAT,S_IRWXU);
    
	if(f == -1)
		return FALSE;

	write(f,pQdial,sizeof(AB_QDIAL)*AB_QDIAL_MAXNUM);

	close(f);

    chdir(szOldPath);

    return TRUE;
}
