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
#include "winpda.h"
#include "string.h"
#include "malloc.h"
#include "stdio.h"
#include "stdlib.h"
#include "plx_pdaex.h"
#include "wapi.h"
#include "setting.h"
#include "pubapp.h"
#include "wUipub.h"

char szHomePage[URLNAMELEN] = "";
int nHomepageType = URL_NULL;
int curUrlType;
unsigned short nCPort = 0;
int DialSuccessFlag = 0;
static char szWmlCurPage[URLNAMELEN] = "";

/*********************************************************************\
* for public                                                         
\*********************************************************************/
static BOOL WML_IsWindowEdit(HWND hWnd)
{
    char pClassName[100];

    GetClassName(hWnd, pClassName, 100);    
    if (strcmp(pClassName, "INPUTEDIT") == 0 ||
        strcmp(pClassName, "DINPUTEDIT") == 0 ||
        strcmp(pClassName, "EDIT") == 0)
    {
        LONG Style;

        Style = GetWindowLong(hWnd, GWL_STYLE);        
        if ((Style & ES_READONLY) == 0)
            return TRUE;
        else
            return FALSE;
    }
    
    return FALSE;
}

void Wml_ONCancelExit(HWND hParent)
{
    if (!WML_IsWindowEdit(GetFocus()))
    {
        PostMessage(hParent, WM_CLOSE, NULL, NULL);
        return;
    }
    else
    {
        int nCaret_start, nCaret_end;
        SendMessage(GetFocus(), EM_GETSEL, (WPARAM)&nCaret_start, 
            (WPARAM)&nCaret_end);
        
        if ((nCaret_start) || (nCaret_end))
        {
            SendMessage(GetFocus(), WM_KEYDOWN, VK_BACK, 0);
            SendMessage(GetFocus(), WM_CHAR, VK_BACK, 0);
        }
        else
            PostMessage(hParent, WM_CLOSE, NULL, NULL);
    }
}

/*********************************************************************\
* Function       
* Purpose      
* Params       
* Return            
* Remarks       
**********************************************************************/
int GetConnectType(int nportvalue)
{
    if (nportvalue == NULL_PORT)
        return CN_NONE;
    if (nportvalue == WSP_Datagram_PORT || nportvalue == WSP_WTLS_Datagram_PORT)
        return CN_NOCONNECT;

    return CN_CONNECT;
}

int GetCurentPage(char *gUrl, int *ptype)
{
    int nlen;
    
    if (gUrl == NULL || ptype == NULL) 
        return -1;
    
    *ptype = curUrlType;
    
    nlen = strlen(szWmlCurPage);
    if (nlen > 0)
    {
        if (nlen <= URLNAMELEN-1)
            strcpy(gUrl, szWmlCurPage);
        else
        {
            strncpy(gUrl, szWmlCurPage, URLNAMELEN -1);
            szWmlCurPage[URLNAMELEN-1] = '\0';
        }

        return nlen;
    }
    
    return 0;
}

int SetCurentPage(char *sUrl, int ntype)
{
    int nlen;
    
    curUrlType = ntype;
    if (sUrl == NULL ) 
        return -1;
    nlen = strlen(sUrl);
    if (nlen <= URLNAMELEN-1)
        strcpy(szWmlCurPage, sUrl);
    else
    {
        strncpy(szWmlCurPage, sUrl, URLNAMELEN -1);
        szWmlCurPage[URLNAMELEN-1] = '\0';
    }

	printf("+++++++++++++++szWmlCurPage = %s+++++++++++++++++++\r\n", szWmlCurPage);
    return nlen;
}

int LocalFileUrlStd(char *szFileUrl)
{
    int k = 0;
    char UrlStr[URLNAMELEN];
    
    strcpy(UrlStr, szFileUrl);
    
    while (UrlStr[k] != ':' )
    {
        k++;
        if (k > URLNAMELEN -2)
            break;
    }
    UrlStr[k] = 0;   
    if (strcasecmp(UrlStr, LOCALPATH) == 0)    
    {
        return 1;                
    }
    
    if (strcasecmp(UrlStr, FLASHPATH) == 0)
    {        
        strcpy(UrlStr, "file://");
        strncat(UrlStr, szFileUrl, URLNAMELEN - strlen(UrlStr) -2);
        strcpy(szFileUrl, UrlStr);
        return 2;
    }    
    
    strcpy(UrlStr, "file://");
    strcat(UrlStr, FLASHPATH);
    strncat(UrlStr, szFileUrl, URLNAMELEN - strlen(UrlStr) -2);
    strcpy(szFileUrl, UrlStr);
    
    return 0;
}

BOOL JudgetoStandardUrl(char *dst, char *src, int *type)
{
    if (strncasecmp(src, "HTTP://", 7) != 0)
    {
        if (isLocalFile(src))
        {
            if (dst) 
                strcpy(dst, src);
            *type = URL_LOCAL;
        }
        else
        {                        
            if (dst) 
            {
                strcpy(dst, "http://");
                strncat(dst, src, URLNAMELEN - 8);
            }
            *type = URL_REMOTE;
        }
    }
    else
    {
        if (dst) 
            strncpy(dst, src, URLNAMELEN -1);
        *type = URL_REMOTE;
    }
    return  TRUE;
}

BOOL GetDialSucFlag()
{
    return DialSuccessFlag;    
}

/*函数strdup的模拟函数*/
char* StrDup( const char* SRCSTR )
{
    int len;    
    char* DstStr;
    
    if (SRCSTR == NULL) 
        return NULL;
    
    len = strlen(SRCSTR);
    
    DstStr = malloc(len + 1);
    if (DstStr == NULL)
        return NULL;
    strcpy(DstStr, SRCSTR);
    
    return DstStr;
}

char* strlwr (char* string)
{
    char * cp;

    for (cp = string; *cp; ++cp)
    {
		if ('A' <= *cp && *cp <= 'Z')
			*cp += 'a' - 'A';
    }

    return(string);
}

#define alphanum(c)		isalnum(c)

#define mark(c)			\
	((c==0x2d) ||\
	(c==0x11)  ||\
	(c==0x2e)  ||\
	(c==0x21)  ||\
	(c==0xff)  ||\
	(c==0x2a)  ||\
	(c==0x27)  ||\
	(c==0x28)  ||\
	(c==0x29) )

#define escaped(c)		( c==0x25 )

#define unreserved(c)	( alphanum(c) || mark(c) )

#define reserved(c)		\
	((c==0x3b) ||\
	(c==0x2f)  ||\
	(c==0x3f)  ||\
	(c==0x3a)  ||\
	(c==0xfd)  ||\
	(c==0x26)  ||\
	(c==0x3d)  ||\
	(c==0x2b)  ||\
	(c==0x02)  ||\
	(c==0x2c) )

#define part_reserved(c) \
	((c==0x26) ||\
	(c==0x3d)  ||\
	(c==0x2b)  ||\
	(c==0x02)  ||\
	(c==0x2c) )

#define uric(c)			( reserved(c) || unreserved(c) || escaped(c) || (c==0x23))

#define isalpha(c)	(((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))?TRUE:FALSE)
#define isalnum(c)	((c >= '0' && c <= '9')?TRUE:FALSE)
#define isxdigit(c) (((c >= '0' && c <= '9') ||(c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))?TRUE:FALSE)

int WML_DeleteHex( char* s )
{
	int i, j, size;
	char* x;
	int ret = 0;

	size = strlen(s);
	x = (char*)malloc(size+1);
	if (x == NULL)
		return 0;

	j = 0;

	for( i=0; i<size; i++ )
	{
		x[j++] = s[i];

		if( s[i] == 0x25 )
		{	
			if( i+2 >= size ) 
				goto Return;

			if( !isxdigit(s[i+1]) || !isxdigit(s[i+2]) ) 
				goto Return;

			i += 2;	
		} 
	}

	x[j] = 0;

	strcpy( s, x );
	
	ret = 1;

Return:
	free(x);

	return ret;
}

BOOL WML_DeleteSpace( char* s )
{
	int i, j, size;
	char* x;

	size = strlen(s);
	x = (char*)malloc(size+1);
	if (x == NULL)
		return FALSE;

	j = 0;

	for( i=0; i<size; i++ )
	{
		if( s[i] == 0x20 ) 
			continue;

		x[j++] = s[i];
	}

	x[j] = 0;

	strcpy( s, x );
	if(strlen(s) == 0)
	{
		free(x);
		return FALSE;
	}
	else
	{
		free(x);
		return TRUE;
	}
}

int isURLValid( char *purl )			//lack: It must have some problems. Take care.
{	
	char c, *p1, *p2, *p3, *url;
	int i, len, sign;
	int scheme=0, authority=0;
	int ret = FALSE;
	char *NewUrl;
	
	if (purl == NULL)
		return FALSE;

	NewUrl = (char*)malloc(strlen(purl) + 1);
	if (NewUrl == NULL)
		return FALSE;
	strcpy(NewUrl, purl);

	WML_DeleteSpace( purl );

	if( WML_DeleteHex( NewUrl ) == 0 ) 
		goto Return;

	url = NewUrl;

	// step1: check scheme;
	if( ( p1 = strchr( url, 0x3a ) ) != NULL ) 
	{

		scheme = 1;
 
		p2 = url; url = p1 + 1;

		len = p1 - p2;

		if( (len<=0) || ( !isalpha(p2[0]) ) )
			goto Return;

		for( i =1; i<len; i++ ){

			c = p2[i];

			if( alphanum(c) ) continue;
			if( (c==0x2b) || (c==0x2d) || (c==0x2e) ) continue; //why '+''-''.' is valid?

			goto Return;
		}
	}
	
	if( scheme == 1 ){

	//step2: check authority;
	if( strstr( url, "//" ) == url ){

		authority = 1;

		url += 2;

		p2 = url;

		if( ( p3 = strchr( p2, 0x2f ) ) != NULL )
			len = p3 - p2;
		else
			len = strlen( p2 );

		for( i=0; i<len; i++ ){

			c = p2[i];

			if( unreserved(c) || escaped(c) || part_reserved(c) || 
						(c==0x3b) || (c==0x3a) || (c==0xfd) )
				continue;
			else 
				goto Return;
		}

		url += i;

	}// end of if( strstr( url, "//" ) == url )

	if( (authority==0) && (url[0]!=0x2f)	){ //opaque_path

		len = strlen(url);
		if( len < 1 ) 
			goto Return;

		c = url[0];

		if( !( unreserved(c) || escaped(c) || part_reserved(c) ||
			(c==0x3b) || (c==0x3a) || (c==0xfd) || (c==0x3f) ) )
			goto Return;

		for( i=1; i<len; i++ ){

			if( url[i] == 0x23 ){

				url += i+1;
				break;
			}

			if( !uric(url[i]) )
				goto Return;
		}

		if( i == len ) 
		{
			ret = TRUE;
			goto Return;
		}
	}

	}// end of if( scheme == 1 )

	
	//step3: abs_path

	if( url[0] != 0x2f ){	//rel_segment

		len = strlen(url);

		i = 0;

		while( (i<len) && 
			   ( (c=url[i++]) != 0x2f ) &&
			   ( c != 0x23 ) &&
			   ( c != 0x3f )
			  ) 
		{

			if( !( unreserved(c) || escaped(c) || part_reserved(c) ||
						(c==0x3b) || (c==0xfd) ) )
				goto Return;
		}

		if( i==len )
		{
			ret = TRUE;
			goto Return;
		}
		url += i;
	}


#define SEGMENT		0
#define PARAM		1

	sign = SEGMENT;
	len = strlen(url);

	// here , url[o] must be '/' or '#' '?'
	for( i=1; i<len; i++ ){	//abs_path

		c = url[i];

		if( (c==0x3f) || (c==0x23) ) break;

		switch( sign ){
		case SEGMENT:

			if( c== 0x3b ) { sign = PARAM; break; }
			
			if( !( unreserved(c) || escaped(c) || part_reserved(c) ||
					(c==0x3a) || (c==0xfd) || (c==0x2f) ) )
				goto Return;

			break;

		case PARAM:

			if( c== 0x2f ) { sign = SEGMENT; break; }

			if( !( unreserved(c) || escaped(c) || part_reserved(c) ||
					(c==0x3a) || (c==0xfd) || (c==0x3b) ) )
				goto Return;

			break;
		}// end of switch
	}

	for( i++; i<len ; i++ ){ // query or fragment;

		c = url[i];

		if( !uric(c) )
			goto Return;
	}

	ret = TRUE;

Return:
	free(NewUrl);

	return ret;
}

