/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Module Name:
    inbox_dep.h
Abstract:
	This module defines the prototype of all the system depedent functions.
Author:
    Luo hongbo
Revision History:
	2002.5		created
---------------------------------------------------------------------------*/
#ifndef INBOX_DEP_H
#define INBOX_DEP_H

#define INBOX_HDL    DWORD
#define INVALID_INBOX_HDL		0
#define INBOX_FIND_HDL			long
#define INVALID_INBOX_FIND_HDL	-1


INBOX_HDL INBOX_OpenObj(const char *path,const char *name);
INBOX_HDL INBOX_CreateObj(const char *path,const char *name);
void  INBOX_CloseObj(INBOX_HDL inbox_hdl);
int   INBOX_WriteObj(INBOX_HDL inbox_hdl,UCHAR *buf,DWORD bytes_to_write);	/* the actual written size ;*/
DWORD INBOX_ReadObj(INBOX_HDL inbox_hdl,UCHAR *buf,DWORD bytes_to_read);	
DWORD INBOX_GetObjSize(INBOX_HDL inbox_hdl); 
int INBOX_DeleteObj(const char* path,const char *name);

INBOX_FIND_HDL INBOX_FindFirstObj(const char *path,UCHAR *name);
int INBOX_FindNextObj(INBOX_FIND_HDL find_hdl,UCHAR *name);
void INBOX_FindClose(INBOX_FIND_HDL find_hdl);

#endif
