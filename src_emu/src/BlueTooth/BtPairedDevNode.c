/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Bluetooth module
 *
 * Purpose  : 
 *
\**************************************************************************/

#include "BtPairedDevNode.h"

void FreePairedDevNode(PPAIREDDEVNODE Head)
{
	PPAIREDDEVNODE Pointer;

	while(Head!=NULL)
	{
		Pointer=Head;
		Head=Head->Next;
		free(Pointer);
	}
}


PPAIREDDEVNODE AppendPairedDevNode(PPAIREDDEVNODE Head,PPAIREDDEVNODE New)
{
	PPAIREDDEVNODE Pointer;
	Pointer=Head;

	if(Head==NULL)
	{
		Head=New;
		New->Next=NULL;
	}
    else                       //添加到链表的最后
	{
        while(Pointer->Next)         
			Pointer=Pointer->Next;		
		
		Pointer->Next=New;
		New->Next=NULL;

    }		
    
  return Head;
}

PPAIREDDEVNODE DeletePairedDevNode(PPAIREDDEVNODE Head,BTDEVHDL Dev_Hdl)
{
    PPAIREDDEVNODE pPairedDevTemp,p=NULL;
	
	pPairedDevTemp=Head;

	while (pPairedDevTemp->PairedDevHandle!=Dev_Hdl && pPairedDevTemp->Next!=NULL)				 
	{
		p = pPairedDevTemp;
		pPairedDevTemp = pPairedDevTemp->Next; 
	}
				 
	if(pPairedDevTemp->PairedDevHandle==Dev_Hdl) 
	{
		if(pPairedDevTemp==Head) 
		{
		    Head=Head->Next;
			free(pPairedDevTemp); 
		}
		else
		{
			p->Next=pPairedDevTemp->Next;
			free(pPairedDevTemp);
		}
	}

    return Head;
}

PPAIREDDEVNODE GetDeviceNodePointer(PPAIREDDEVNODE Head,BTDEVHDL dev_hdl)
{
	PPAIREDDEVNODE pCurPointer;
	
	pCurPointer=Head;

    while(pCurPointer!=NULL)
	{
		if(pCurPointer->PairedDevHandle!=dev_hdl)
		   pCurPointer=pCurPointer->Next;	
		else
			break;
	}
	
	return pCurPointer;
	
}

PPAIREDDEVNODE GetPairedDevInfoFromFile()
{
	FILE *fp;
	
	long wholesize;
	int count;
	int i;

	int j;

    BTDEVHDL CurNodeDevHandle;

	PPAIREDDEVNODE pPairedDevHead=NULL,pPairedDevTemp;
	
	PAIREDDEVFIlEINFO PairedDevFileInfo;
		
	fp=fopen(BTPAIREDDEVINFOFILENAME,"rb+");
	
	if(fp==NULL)
		return NULL;
	else
	{
	   fseek(fp,0,SEEK_END);
	   wholesize=ftell(fp);
         
       count=wholesize/sizeof(PAIREDDEVFIlEINFO);

	   fseek(fp,0,SEEK_SET);    //重新定位文件读指针到文件头

	   for(i=0;i<count;i++)
	   {
		   fread(&PairedDevFileInfo,sizeof(PAIREDDEVFIlEINFO),1,fp);
		   
		   pPairedDevTemp=(PPAIREDDEVNODE)malloc(sizeof(PAIREDDEVNODE));

		   memset(pPairedDevTemp,0,sizeof(PAIREDDEVNODE));
		   
           memcpy(pPairedDevTemp->PairedDevBdAddr,PairedDevFileInfo.PairedDevBdAddr,BTSDK_BDADDR_LEN);
		   
		   printf("\r\n######In GetPairedDevInfoFromFile#######\r\n");
		   printf("\r\n######PairedDevFileInfo.PairedDevName=%s#######\r\n",PairedDevFileInfo.PairedDevName);

           for(j = 5; j > 0; j--)
			   printf("%02X:", PairedDevFileInfo.PairedDevBdAddr[j]);
		   printf("%02X", PairedDevFileInfo.PairedDevBdAddr[0]);

		   strcpy(pPairedDevTemp->PairedDevAliasName,PairedDevFileInfo.PairedDevAliasName);
		   pPairedDevTemp->PairedDevAuthor=PairedDevFileInfo.PairedDevAuthor;
		   
           CurNodeDevHandle=GetDeviceHandle(PairedDevFileInfo.PairedDevBdAddr);
           pPairedDevTemp->PairedDevHandle=CurNodeDevHandle;

           printf("\r\n######CurNodeDevHandle=0X%08X#######\r\n",CurNodeDevHandle);

           strcpy(pPairedDevTemp->PairedDevName,PairedDevFileInfo.PairedDevName);
               		   
		   if(pPairedDevHead==NULL)
             pPairedDevHead=AppendPairedDevNode(pPairedDevHead,pPairedDevTemp);
		   else
             AppendPairedDevNode(pPairedDevHead,pPairedDevTemp);		   
	   }

	      fclose(fp);
	}

	return pPairedDevHead;
}

void SetPairedDevInfoToFile(PPAIREDDEVNODE Head)
{
	FILE *fp;
	PPAIREDDEVNODE pCurPointer;
	PAIREDDEVFIlEINFO PairedDevFileInfo;

	int j;

	fp=fopen(BTPAIREDDEVINFOFILENAME,"wb+");
	
	if(fp==NULL)
		return;
	else
	{
		pCurPointer=Head;

		while(pCurPointer!=NULL)
		{
            memset(&PairedDevFileInfo,0,sizeof(PAIREDDEVFIlEINFO));
			
            memcpy(PairedDevFileInfo.PairedDevBdAddr,pCurPointer->PairedDevBdAddr,BTSDK_BDADDR_LEN);
            
			printf("\r\n######In SetPairedDevInfoToFile#######\r\n");
            printf("\r\n######pCurPointer->PairedDevName=%s#######\r\n",pCurPointer->PairedDevName);

			for(j = 5; j > 0; j--)
				printf("%02X:", pCurPointer->PairedDevBdAddr[j]);
			printf("%02X", pCurPointer->PairedDevBdAddr[0]);

			strcpy(PairedDevFileInfo.PairedDevName,pCurPointer->PairedDevName);
			strcpy(PairedDevFileInfo.PairedDevAliasName,pCurPointer->PairedDevAliasName);
            PairedDevFileInfo.PairedDevAuthor=pCurPointer->PairedDevAuthor;
            
			fwrite(&PairedDevFileInfo,sizeof(PAIREDDEVFIlEINFO),1,fp);

			pCurPointer=pCurPointer->Next;
		}

		fclose(fp);
		
	}
}
