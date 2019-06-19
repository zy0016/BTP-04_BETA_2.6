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

#include "windows.h"

#include "stdio.h"
#include "string.h"
#include "winbase.h"

#define MAX_NAMELEN	64
#define MAX_FILELEN	10000
#define MAX_FUNCNUM	500	



char* funcname_tab[MAX_FUNCNUM];

extern int ReadFileToBuf (const char* filename, char* filebuf);
extern int WriteBufToFile (char* filename, char* filebuf);
extern int GetFuncnameTab (const char* filename, char** funcname_tab);
extern void ReplaceFuncname(char* newname, char* oldname, char* filebufSrc, char* filebufDes);
extern int MakeFuncTabFile (const char** funcname_tab, char* filename);

void main(int argc, char** argv)
{
	char* filebufSrc = NULL;
	char* funcname;
	int i =0;
	char inputf1[MAX_NAMELEN];
	char inputf2[MAX_NAMELEN];

	if ((argc != 3)|| (argv[1] == NULL) || (argv[2] == NULL))
		return;
	strcpy(inputf1, argv[1]);//argv[1]:function list
	strcpy(inputf2, argv[2]);//argv[2]:template.s

	if (GetFuncnameTab(inputf1, funcname_tab) != 0)
	{
		printf("GetFuncnameTab failed!!!\r\n");
		return;
	}
	if ((filebufSrc = malloc(MAX_FILELEN)) == NULL)
		return;
	if (ReadFileToBuf(inputf2, filebufSrc) != 0)
	{
		printf("ReadFileToBuf failed!!!\r\n");
		return;
	}
	
	funcname = funcname_tab[i];

	CreateDirectory("stub",0);
	
	while (funcname != NULL)
	{
	
		char* filebufDes = NULL;
		char filename[MAX_NAMELEN];
	
		const char* suffix = ".S";
	
		/* construct file name */
		if(!strcpy(filename, "stub/"))
			return;
		if(!strcat(filename, funcname))
			return;
		strcat (filename, suffix);
		if ((filebufDes = malloc(MAX_FILELEN)) == NULL)
			return ;
		ReplaceFuncname (funcname, "template", filebufSrc, filebufDes);

		if (WriteBufToFile (filename, filebufDes) != 0)
		{	
			printf("WriteBufToFile %s failed!!!\r\n", filename);
			free(filebufDes);
			return ;
		}
		free(filebufDes);
		i++;
		funcname = funcname_tab[i];

	}
	free(filebufSrc);
#if 0
	if (MakeFuncTabFile(funcname_tab, "func_tab.c") != 0)
	{	
		printf("MakeFuncTabFile func_tab.c failed!!!\r\n");
		return ;
	}
#endif
	return ;
	
}

int ReadFileToBuf (const char* filename, char* filebuf)
{
	FILE* fp;
	unsigned int size;
	memset(filebuf, 0, MAX_FILELEN);
	if ((fp = fopen(filename, "r")) == NULL)
		return -1;
	size = fread(filebuf, 1, MAX_FILELEN, fp);
	filebuf[size - 1] = '\0';
	strcat(filebuf, "\r\n");
	return 0;
}

int WriteBufToFile (char* filename, char* filebuf)
{
	FILE* fp;
	if ((fp = fopen(filename, "w")) == NULL)
		return -1;
	fputs(filebuf, fp);
	return 0;	
}

#if 0
int GetFuncnameTab (const char* filename, char** funcname_tab)
{
	char filebuf[MAX_FILELEN];
	char* endpos;
	char* startpos;
	char bracket, ch;
	char* funcname;
	char* pbuf;
	int count1, count2;
	int i =0;
	
	
	bracket = '(';
	if (ReadFileToBuf(filename, filebuf) != 0)
		return -1;
	
	pbuf = filebuf;
	endpos = strchr(pbuf, bracket);
	if (endpos == NULL)
		return -1;
	while (endpos != NULL)
	{
		pbuf = endpos + 1;
		endpos = endpos - 1;
		ch = *endpos;
		while (isspace(ch))
		{
			endpos--;
			ch = *endpos;
		}
		startpos = (char*)((unsigned long)endpos);
		while((!isspace(ch))&&(ch != '*'))
		{
			startpos--;
			ch = *startpos;
		}
		startpos ++;
		if ((funcname = malloc(MAX_NAMELEN)) == NULL)
			return -1;
		strncpy (funcname, startpos, endpos-startpos+1);
		funcname[endpos-startpos+1] = '\0';
		funcname_tab[i] = funcname;
		i++;
		endpos = strchr (pbuf, bracket);
	}

	funcname_tab[i] = NULL;
	
	return 0;
}
#endif


int GetFuncnameTab (const char* filename, char** funcname_tab)
{

	char funcname[MAX_NAMELEN];
	FILE* fp;
	char* name;
	int len, i =0;

	if ((fp = fopen (filename, "r"))==NULL)
		return -1;
	while (!feof(fp))
	{
		
		if((name = fgets (funcname, MAX_FILELEN, fp))== NULL)
			return 0;
		while (*name == '\t'|| *name == ' ')
			name++;
		len = strlen(name);
		if (len == 0)
			continue;
		while (len > 0 && (
				name[len-1] == '\r'||
				name[len-1] == '\n'||
				name[len-1] == '\t'||
				name[len-1] == ' ') ) {
			name[len-1] = 0;
			len --;
		}
		if (len !=0 ) {
			funcname_tab[i]=strdup(name);
			i++;
		}
	}
	return 0;
	
}

void ReplaceFuncname(char* newname, char* oldname, char* filebufSrc, char* filebufDes)
{
	char* firstpos;
	char* str;
	
	filebufDes[0] = '\0';
	firstpos = filebufSrc;
	str = strstr(filebufSrc, oldname);
	
	while (str!=NULL)
	{
		
		strncat(filebufDes, firstpos, str-firstpos);
		strcat(filebufDes, newname);
		str = &str[strlen(oldname)];
		firstpos = str;
		str = strstr(str, oldname);
	}
	strcat (filebufDes, "\r\n");
	return;
}


int MakeFuncTabFile (const char** funcname_tab, char* filename)
{
	char filebuf[MAX_FILELEN];
	const char* funcname;
	int i = 0;

	filebuf[0] = '\0';
	strcpy (filebuf, "const struct func_table func_tab[]={\r\n\t");
	funcname = funcname_tab[i];
	while (funcname != NULL)
	{
		strcat (filebuf, "{(void*)");
		strcat (filebuf, funcname);
		strcat (filebuf, ", \"");
		strcat (filebuf, funcname);
		strcat (filebuf, "\"},\r\n\t");

		i++;
		funcname = funcname_tab[i];	
	}
	strcat (filebuf, "{0,0}\r\n\t};\r\n");
	if (WriteBufToFile (filename, filebuf) != 0)
		return -1;
	return 0;
}