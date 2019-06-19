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

#define  AB_MOD             4
#define  INDEX_BASE_MAX     1000

typedef struct tagDATAMEMORY
{
	LPBYTE	m_start;          //start
    DWORD   m_wptr;           //the position of write offset
	DWORD	m_size;           //size of malloc
	DWORD	m_freespace;      //size of free space
	DWORD	m_deletedspace;   //size of delete space
}DATAMEMORY, *PDATAMEMORY;

PMEMORY_NAME    *pIndexID = NULL;
PMEMORY_NAME    *pIndexName = NULL;
PMEMORY_TEL     *pIndexTel = NULL;
PMEMORY_EMAIL   *pIndexEmail = NULL;

int nIDCounter,nNameCounter,nTelCounter,nEmailCounter;

int nID,nName,nTel,nEmail;

static DATAMEMORY DataMemory;

/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
void AB_EmptyMemory(void)
{
    memset(DataMemory.m_start,0x00,DataMemory.m_size);
    
    DataMemory.m_freespace = DataMemory.m_size;

    DataMemory.m_deletedspace = 0;

    DataMemory.m_wptr = 0;

    pIndexName = (PMEMORY_NAME*)realloc(pIndexName,sizeof(PMEMORY_NAME)*(INDEX_BASE_MAX+1));

    memset(pIndexName,0x00,sizeof(PMEMORY_NAME)*(INDEX_BASE_MAX+1));
  
    pIndexTel = (PMEMORY_TEL*)realloc(pIndexTel,sizeof(PMEMORY_TEL)*(INDEX_BASE_MAX+1));

    memset(pIndexTel,0x00,sizeof(PMEMORY_TEL)*(INDEX_BASE_MAX+1));

    pIndexEmail = (PMEMORY_EMAIL*)realloc(pIndexEmail,sizeof(PMEMORY_EMAIL)*(INDEX_BASE_MAX+1));

    memset(pIndexEmail,0x00,sizeof(PMEMORY_EMAIL)*(INDEX_BASE_MAX+1));

    pIndexID = (PMEMORY_NAME*)realloc(pIndexID,sizeof(PMEMORY_NAME)*(INDEX_BASE_MAX+1));

    memset(pIndexID,0x00,sizeof(PMEMORY_NAME)*(INDEX_BASE_MAX+1));

    nID = 0;
    nName = 0;
    nTel = 0;
    nEmail = 0;

    nIDCounter = 1;
    nNameCounter = 1;
    nTelCounter = 1;
    nEmailCounter = 1;
}
/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL AB_MallocMemory(unsigned int nSize)
{
    memset(&DataMemory,0x00,sizeof(DATAMEMORY));

    DataMemory.m_size = nSize;

    if(DataMemory.m_size == 0)
        return FALSE;

    DataMemory.m_start = (LPBYTE)malloc(DataMemory.m_size);

    if(DataMemory.m_start == NULL)
        return FALSE;

    memset(DataMemory.m_start,0x00,DataMemory.m_size);

    DataMemory.m_freespace = DataMemory.m_size;

    DataMemory.m_deletedspace = 0;

    DataMemory.m_wptr = 0;

    pIndexName = (PMEMORY_NAME*)malloc(sizeof(PMEMORY_NAME)*(INDEX_BASE_MAX+1));

    if(pIndexName == NULL)
    {
        AB_FREE(DataMemory.m_start);

        return FALSE;
    }
    
    pIndexTel = (PMEMORY_TEL*)malloc(sizeof(PMEMORY_TEL)*(INDEX_BASE_MAX+1));

    if(pIndexTel == NULL)
    {
        AB_FREE(pIndexName);

        AB_FREE(DataMemory.m_start);

        return FALSE;
    }

    pIndexEmail = (PMEMORY_EMAIL*)malloc(sizeof(PMEMORY_EMAIL)*(INDEX_BASE_MAX+1));

    if(pIndexEmail == NULL)
    {
        AB_FREE(pIndexName);
        
        AB_FREE(pIndexTel);

        AB_FREE(DataMemory.m_start);

        return FALSE;
    }

    pIndexID = (PMEMORY_NAME*)malloc(sizeof(PMEMORY_NAME)*(INDEX_BASE_MAX+1));

    if(pIndexID == NULL)
    {
        AB_FREE(pIndexEmail);

        AB_FREE(pIndexName);
        
        AB_FREE(pIndexTel);

        AB_FREE(DataMemory.m_start);

        return FALSE;
    }

    nID = 0;
    nName = 0;
    nTel = 0;
    nEmail = 0;

    nIDCounter = 1;
    nNameCounter = 1;
    nTelCounter = 1;
    nEmailCounter = 1;

    return TRUE;
}
/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL AB_InsertName(DWORD dwoffset,DWORD ID,int nGroup,BYTE byNamelen,char* pszName)
{
    int npad = 0;
    DWORD datalen = 0;
    LPBYTE p;

    if(DataMemory.m_start == NULL)
        return FALSE;

    datalen = sizeof(DWORD)*2+sizeof(int)+sizeof(BYTE)+byNamelen;

    if(DataMemory.m_freespace < datalen) // memory not enough
        return FALSE;

    //bynamelen include '\0'

    npad = (byNamelen+sizeof(BYTE)) % AB_MOD;
    
    npad = AB_MOD - npad;

    p = DataMemory.m_start + DataMemory.m_wptr;
    
    *(DWORD*)p = ID; 
    p += sizeof(DWORD);
    *(DWORD*)p = dwoffset;
    p += sizeof(DWORD);
    *(int*)p = nGroup;
    p += sizeof(int);
    *(BYTE*)p = byNamelen;
    p += sizeof(BYTE);
    if(byNamelen == 1)
    {
        *(BYTE*)p = '\0';
        p += sizeof(BYTE);
    }
    else
    {
        memcpy(p,pszName,byNamelen);
        p += sizeof(BYTE)*byNamelen;
    }
    
    if(npad > 0)
        memset(p,'\0',npad);

    nName++;
    nID++;

    if(pIndexName != NULL && nName > (INDEX_BASE_MAX*nNameCounter))
    {
        nNameCounter++;

        nIDCounter++;

        pIndexName = (PMEMORY_NAME*)realloc(pIndexName,sizeof(PMEMORY_NAME)*(INDEX_BASE_MAX*nNameCounter+1));

        if(pIndexName == NULL)
            return FALSE;

        pIndexID = (PMEMORY_NAME*)realloc(pIndexID,sizeof(PMEMORY_NAME)*(INDEX_BASE_MAX*nIDCounter+1));

        if(pIndexID == NULL)
            return FALSE;

        memset(pIndexName+INDEX_BASE_MAX*(nNameCounter-1)+1,0x00,sizeof(PMEMORY_NAME)*INDEX_BASE_MAX);

        memset(pIndexID+INDEX_BASE_MAX*(nIDCounter-1)+1,0x00,sizeof(PMEMORY_NAME)*INDEX_BASE_MAX);
    }

    pIndexName[nName] = (PMEMORY_NAME)(DataMemory.m_start + DataMemory.m_wptr);
    
    pIndexID[nID] = (PMEMORY_NAME)(DataMemory.m_start + DataMemory.m_wptr);

    DataMemory.m_wptr += datalen+npad;

    DataMemory.m_freespace = DataMemory.m_size - DataMemory.m_wptr;

    return TRUE;
}
/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL AB_InsertTel(DWORD ID,int nTelID,BYTE byTellen,char* pszTel)
{
    int npad = 0;
    DWORD datalen = 0;
    LPBYTE p;
	int code;
	char* pString;

    if(DataMemory.m_start == NULL)
        return FALSE;

    datalen = sizeof(DWORD)+sizeof(BYTE)+sizeof(int)+ sizeof(int) + byTellen;

    if(DataMemory.m_freespace < datalen) // memory not enough
        return FALSE;

    //bynamelen include '\0'

    npad = (byTellen+sizeof(BYTE)) % AB_MOD;
    
    npad = AB_MOD - npad;

	if(strlen(pszTel) <= COMPARE_TEL_COUNT)
	{
		code = atoi(pszTel);
	}
	else
	{
		pString = pszTel + strlen(pszTel) - COMPARE_TEL_COUNT;
		code = atoi(pString);
	}

    p = DataMemory.m_start + DataMemory.m_wptr;
    
    *(DWORD*)p = ID;
    p += sizeof(DWORD);
    *(int*)p = nTelID;
    p += sizeof(int);
	*(int*)p = code;
	p += sizeof(int);
    *(BYTE*)p = byTellen;
    p += sizeof(BYTE);
    if(byTellen == 1)
    {
        *(BYTE*)p = '\0';
        p += sizeof(BYTE); 
    }
    else
    {
        memcpy(p,pszTel,byTellen);
        p += sizeof(BYTE)*byTellen; 
    }
    
    if(npad > 0)
        memset(p,'\0',npad);

    nTel++;

    if(pIndexTel != NULL && nTel > (INDEX_BASE_MAX*nTelCounter))
    {
        nTelCounter++;

        pIndexTel = (PMEMORY_TEL*)realloc(pIndexTel,sizeof(PMEMORY_TEL)*(INDEX_BASE_MAX*nTelCounter+1));

        if(pIndexTel == NULL)
            return FALSE;

        memset(pIndexTel+INDEX_BASE_MAX*(nTelCounter-1)+1,0x00,sizeof(PMEMORY_TEL)*INDEX_BASE_MAX);
    }

    pIndexTel[nTel] = (PMEMORY_TEL)(DataMemory.m_start + DataMemory.m_wptr);

    DataMemory.m_wptr += datalen+npad;

    DataMemory.m_freespace = DataMemory.m_size - DataMemory.m_wptr;

    return TRUE;
}
/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL AB_InsertEmail(DWORD ID,int nEmailID,BYTE byEmaillen,char* pszEmail)
{
    int npad = 0;
    DWORD datalen = 0;
    LPBYTE p;

    if(DataMemory.m_start == NULL)
        return FALSE;

    datalen = sizeof(DWORD)+sizeof(BYTE)+sizeof(int)+ sizeof(int) + byEmaillen;

    if(DataMemory.m_freespace < datalen) // memory not enough
        return FALSE;

    //bynamelen include '\0'

    npad = (byEmaillen+sizeof(BYTE)) % AB_MOD;

    npad = AB_MOD - npad;

    p = DataMemory.m_start + DataMemory.m_wptr;
    
    *(DWORD*)p = ID;
    p += sizeof(DWORD);
    *(int*)p = nEmailID;
    p += sizeof(int);
	*(int*)p = 0;			//unused
    p += sizeof(int);
    *(BYTE*)p = byEmaillen;
    p += sizeof(BYTE);
    if(byEmaillen == 1)
    {
        *(BYTE*)p = '\0';
        p += sizeof(BYTE); 
    }
    else
    {
        memcpy(p,pszEmail,byEmaillen);
        p += sizeof(BYTE)*byEmaillen; 
    }
    
    if(npad > 0)
        memset(p,'\0',npad);

    nEmail++;

    if(pIndexEmail != NULL && nEmail > (INDEX_BASE_MAX*nEmailCounter))
    {
        nEmailCounter++;

        pIndexEmail = (PMEMORY_EMAIL*)realloc(pIndexEmail,sizeof(PMEMORY_EMAIL)*(INDEX_BASE_MAX*nEmailCounter+1));

        if(pIndexEmail == NULL)
            return FALSE;

        memset(pIndexEmail+INDEX_BASE_MAX*(nEmailCounter-1)+1,0x00,sizeof(PMEMORY_EMAIL)*INDEX_BASE_MAX);
    }

    pIndexEmail[nEmail] = (PMEMORY_EMAIL)(DataMemory.m_start + DataMemory.m_wptr);

    DataMemory.m_wptr += datalen+npad;

    DataMemory.m_freespace = DataMemory.m_size - DataMemory.m_wptr;

    return TRUE;
}
