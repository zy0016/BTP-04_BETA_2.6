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

#include "BtDeviceNode.h"

void Free_DeviceNodeList(DeviceNodeLink Head)
{
	DeviceNodeLink Pointer;

	while(Head!=NULL)
	{
		Pointer=Head;
		Head=Head->Next;
		free(Pointer);
	}
}

DeviceNodeLink Append_DeviceNodeList(DeviceNodeLink Head,DeviceNodeLink DevInfo)
{
	DeviceNodeLink Pointer;
	
	if(Head)
	{
	  Pointer=Head;

	  while(Pointer->Next)
		    Pointer=Pointer->Next;

	  Pointer->Next=DevInfo;	    
	  DevInfo->Next=NULL;
	}
	else
	{
		Head=DevInfo;
	    DevInfo->Next=NULL;	    
	}
	
	return Head;
}

BOOL IsExsitDeviceInList(DeviceNodeLink Head,BTUINT8 *BdAddr)
{
	DeviceNodeLink Pointer;
	
	Pointer=Head;
    
	while(Pointer!=NULL)
	{
		if(strcmp(Pointer->DevBdAddr,BdAddr))
		     Pointer=Pointer->Next;
		else
			break;
	}
    
	if(Pointer==NULL)
		return FALSE;
	else
		return TRUE;
}
