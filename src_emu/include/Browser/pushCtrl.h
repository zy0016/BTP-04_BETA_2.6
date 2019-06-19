/**************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   :  wap browser
 *
 * Purpose  :  macro and struct for WAP PUSH
 *
\**************************************************************************/

#define ACTION_NONE          1
#define ACTION_LOW           2
#define ACTION_MEDIUM        3
#define ACTION_HIGH          4
#define ACTION_DELETE        5
#define ACTION_CACHE		 6

#define DATETIEMLEN	21

typedef struct tagSIINFO
{
	char *pszTitle;
	char *pszUri;
	char *pszId;
	char szCreateTime[DATETIEMLEN];
	char szExTime[DATETIEMLEN];
	unsigned short nActionType;
	unsigned short nRead;		//0ÎªÒÑ¶Á£¬1ÎªÎ´¶Á
} SIINFO,*PSIINFO;


