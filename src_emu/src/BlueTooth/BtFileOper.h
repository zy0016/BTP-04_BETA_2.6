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

#ifndef _BTFILEOPER_H_
#define _BTFILEOPER_H_

void GetFileNameNoPath(char * fNameWithPath, char * fName);
void GetFilePathNoName(char * fNameWithName, char * fPath);
void GetSuffixFileName(char * szFullName, char * szName, char * szSuffix);
void RenameFileName(char *OldFileName, char *NewFileName);

#endif
