/**************************************************************************\
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Model   :	PROFILE
 *
 * Purpose :	Provide some functions to get or set initialization information
 *  
 * Author  :     
 *
 * 
\**************************************************************************/


//#include "hp_kern.h"
#include "window.h"
#include "string.h"
#include "oserror.h"
//#include "fileapi.h"
#include "unistd.h"
#include "types.h"
#include "stat.h"
#include "fcntl.h"
#include "mullang.h"
#include "stype.h"
#include "stdlib.h"
#include "fapi.h"

/**********************/
#define LMEM_FIXED 0x0000

/**********************/
long FS_GetAllSectionNames( char *lpReturn, 
												 unsigned long nSize, 
												 char *lpFileName );

long FS_GetAllKeyNamesInSection(						
						char *lpAppName,  /* points to section name */
						char *lpString,		/* pointer to string to add */
						int  nSize,
						char *lpFileName  /* points to initialization filename */
);



/* Delete string function from file system */
static char *CopyString( char *dest, char *src )
{
	char *ret = dest;
	while( *src ) *dest++ = *src++;
	*dest = 0;
	return ret;
}

static char *CopyStringN( char *dest, char *src, int n )
{
	char *ret = dest;
	while( *src && n-- ) *dest++ = *src++;
	*dest = 0;
	return ret;
}

static char *CatString( char *dest, char *src )
{
	char *ret = dest;
	while( *dest != 0 ) dest ++;
	CopyString( dest, src );
	return ret;
}

static int StringLength( char * str )
{
	int nLen = 0;
	while( *str ) nLen++, str++;
	return nLen;
}

static int CompareString( char *str1, char *str2 )
{
	while( *str1 || *str2 )
	{
		if( *str1 > *str2 ) 
			return 1;
		if( *str1 < *str2 )
			return -1;
		str1++;
		str2++;
	}
	return 0;
}

static char ToLower( char c )
{
	if( c >= 'A' && c <= 'Z' )
/*		return c + 'a' - 'A';*/
		return (char)(c + 'a' - 'A');
	else 
		return c;
}

static char ToUpper( char c )
{
	if( c >= 'a' && c <= 'z' )
/*		return (c + 'A' - 'a');*/
		return (char)(c + 'A' - 'a');
	else
		return c;
}

static int CompareStringN( char *str1, char *str2, int n )
{
	while( (*str1 || *str2) && n-- )
	{
		if( *str1 > *str2 ) 
			return 1;
		if( *str1 < *str2 )
			return -1;
		str1++;
		str2++;
	}
	return 0;
}

static int CompareStringNoCase( char *str1, char *str2 )
{
	char c1, c2;
	while( (*str1 || *str2) )
	{
		c1 = ToUpper( *str1++ );
		c2 = ToUpper( *str2++ );
		if( c1 > c2 )
			return 1;
		if( c1 < c2 )
			return -1;
	}
	return 0;
}

static int CompareStringNoCaseN( char *str1, char *str2, int n )
{
	char c1, c2;
	while( (*str1 || *str2) && n-- )
	{
		c1 = ToUpper( *str1++ );
		c2 = ToUpper( *str2++ );
		if( c1 > c2 )
			return 1;
		if( c1 < c2 )
			return -1;
	}
	return 0;
}

static char *FindString( char *string, char * substr )
{
	char * ptr;
	int sublen;
	sublen = StringLength( substr );
	ptr = string;
	while( *ptr )
	{
		if( CompareStringN( ptr, substr, sublen ) == 0 )
			return ptr;
		ptr ++;
	}
	return NULL;
}

static char *FindStringNoCase( char *string, char * substr )
{
	char * ptr;
	int sublen;
	sublen = StringLength( substr );
	ptr = string;
	while( *ptr )
	{
		if( CompareStringNoCaseN( ptr, substr, sublen ) == 0 )
			return ptr;
		ptr ++;
	}
	return NULL;
}

static char *FindChar( char *string, char c )
{
	while( *string )
	{
		if( *string == c )
			return string;
		string ++;
	}
	return NULL;
}

static int StringToInt( char *str )
{
	int ret = 0;
	char *temp = str;
	if( *str == '-' || *str == '+')
		str++;
	while( *str >='0' && *str <= '9' )
	{
		ret = ret * 10;
		ret += *str - '0';
		str++;
	}
	if( *temp == '-' )
		ret = -ret;
	return ret;
}

void IntToString( int nValue, char *buf )
{
	char stack[15] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	int i = 0;
	int temp = nValue;
	if( nValue < 0 )
		temp = -nValue;
	while( temp != 0 )
	{
/*		stack[i++] = temp-(temp/10)*10;*/
		stack[i++] = (char)(temp-(temp/10)*10);
		temp = temp / 10;
	}
	temp = 0;
	if( nValue < 0 )
		buf[temp++] = '-';
	i --; 
	while( i >= 0 )
	{
/*		buf[temp++] = (stack[i--] + '0');*/
		buf[temp++] = (char)(stack[i--] + '0');
	}
	buf[temp] = 0;
}


static int ReadLine( int handle, char *buf, int maxsize )
{
	int i = 0;
	int numRead = 0;
	char c;
	int ret = 1;
	
	do
	{
		ret = read( handle, &c, 1 );
		if( ret != 1 ) 
			return -1; 
		numRead ++ ;
		if( c > 0x20 ) 
			break;
	} while( ret == 1 );
	buf[i++] = c;
	do
	{
		ret = read( handle, &c, 1 );
		if( ret != 1 ) break;
		numRead ++;
		if( c == 0x0d  ) 
		{
			/*在一行的结尾遇到回车和换行符时，读字符的个数也要累加，但是并不放到buf中*/
			buf[i++] = 0;

			ret = read( handle, &c, 1 );
			if( ret )
			{
				if( c != 0x0A )
				{
					lseek( handle, -1, SEEK_CUR );
					break;
				}
				numRead ++;
			}

			break;
		}
		buf[i++] = c;
	} while( i < maxsize-1 );
//	buf[i] = 0;
	return numRead;
}

static int FInsert(int handle, int offset, void * data, int length)
{
	int fsize;
	void * tem;
	fsize = GetFileSize((HANDLE)handle );

	if (length <= 0)
		return 0;

	if( offset < fsize )
	{
        tem =malloc(fsize - offset);   //tem = LocalAlloc(LMEM_FIXED, fsize - offset );
		if (tem==NULL)
		{
//			OS_SetError( E_MEMORY );
			return -1;
		}
		lseek( handle, offset, SEEK_SET );
		if( read( handle, tem, fsize - offset ) == -1 )
		{
//			LocalFree( tem );
            free( tem );
			return -1;
		}
	}
	lseek( handle, offset, SEEK_SET );
	write( handle, data, length );
	if( offset < fsize )
	{
		lseek(handle, offset+length, SEEK_SET );
		write( handle, tem, fsize - offset );
		free(tem);//LocalFree( tem );
	}
	return 0;
}

static int FDelete(int handle, const char* path,int offset, int length)
{
	char *buf;
	int len;
/*
    char *pfilename;
    fdopen(handle,)
*/
	len = GetFileSize( (HANDLE)handle ) - offset - length;
    buf=malloc(len);//	buf = LocalAlloc(LMEM_FIXED, len );
	if (buf == NULL)
		return -1;
	lseek( handle, offset+length, SEEK_SET );
	len = read( handle, buf, len );
	if( len >= 0 )
	{
		lseek( handle, offset, SEEK_SET );
		write( handle, buf, len );
		if( offset + len == 0 )
		{
			lseek( handle, 0, SEEK_SET );
			write( handle, "\r\n", 2 );
			truncate( path, 2 );
		}
		else
			truncate( path, offset + len );
	}
	free(buf);//LocalFree( buf );
	return len;
}

#define NOFILE -1
#define NOSECTION -2
#define NOKEY -3

static long __FS_GetPrivateProfileString( 
						char *lpAppName,  /* points to section name */
						char *lpKeyName,  /* points to key name */
						char *lpDefault,  /* points to default string */
						char *lpReturnedString,  /* points to destination buffer */
						long nSize,				/* size of destination buffer */
						char *lpFileName  /* points to initialization filename */
)
{
	long ret;
	int handle;
	char buf[512];
	char Section[256]="[";
	char *ptr;

/*连接Section,使其成为 [string]的形式*/
	CatString( Section, lpAppName );
	CatString( Section, "]" );

	/*打开ProFile文件*/
	handle = open( lpFileName, O_RDONLY, 0, 0 );
	if( handle == -1 )
	{
//		close( handle );
		/*返回缺省字符*/
		CopyString( lpReturnedString, lpDefault );
		return NOFILE;
	}
/* find section;*/
	while( (ret=ReadLine( handle, buf, sizeof(buf) )) > 0 )
	{
		ptr = buf;
		/*跳过空格及不可见字符*/
		while( *ptr && *ptr<=0x20 ) 
			ptr++;
		/*比较两个字符串的长度*/
		if( StringLength(ptr) < StringLength(Section) ) 
			continue;
		/*比较两个字符串是否相等*/
		if( CompareStringNoCase( ptr, Section ) == 0 )
			break;
	}
/* No this section;*/
	if( ret <= 0 )
	{
		close( handle );
		CopyString( lpReturnedString, lpDefault );
		return NOSECTION;
	}
/*find key*/
	while( (ret=ReadLine( handle, buf, sizeof(buf) )) >0 )
	{
		char *end;
		ptr=buf;
		while( *ptr && *ptr<=0x20 ) 
			ptr++;
		/*如果找到了下一个Section，证明没有要找的key*/
		if( *ptr == '[' )
		{
			ret = -1;
			break;
		}
		end = strstr( ptr, "=" );
		if (end)
		{
			*end = 0;
			if( CompareStringNoCase( ptr, lpKeyName ) == 0 )
			{	
				*end = '=';
				break;
			}
		}
	}
/* Not Found;*/
	if( ret <= 0 )
	{ 
		close( handle );
		CopyString( lpReturnedString, lpDefault );
		return NOKEY;
	}
/* Found;*/
	ptr = FindString( ptr, "=" );
	if(!ptr) 
	{
		close( handle );
		CopyString( lpReturnedString, lpDefault );
		return -1;
	}
	ptr ++;
	
	if( StringLength( ptr ) < nSize )
		CopyString( lpReturnedString, ptr );
	else 
		CopyStringN( lpReturnedString, ptr, nSize );
	
	close( handle );
	return 0;
}

long FS_GetPrivateProfileInt
				( char *lpAppName,  /* address of section name */
				char *lpKeyName,  /* address of key name */
 				unsigned long nDefault,   		/* return value if key name is not found */
 				char *lpFileName  /* address of initialization filename */
)
{
	char buf[64];

	if( __FS_GetPrivateProfileString(
									lpAppName, 
									lpKeyName, 
									"", 
									buf, 
									sizeof(buf), 
									lpFileName)  < 0 )
		return nDefault;
	else
		return StringToInt( buf );
}

long FS_GetPrivateProfileString( 
						char *lpAppName,  /* points to section name */
						char *lpKeyName,  /* points to key name */
						char *lpDefault,  /* points to default string */
						char *lpReturnedString,  /* points to destination buffer */
						unsigned long nSize,				/* size of destination buffer */
						char *lpFileName  /* points to initialization filename */
)
{
	if( lpAppName == NULL )
		return FS_GetAllSectionNames( lpReturnedString, 
																	nSize, 
																	lpFileName );
	if( lpKeyName == NULL )
		return FS_GetAllKeyNamesInSection(						
										lpAppName, 
										lpReturnedString,
										nSize,
										lpFileName );


	__FS_GetPrivateProfileString(
				lpAppName, lpKeyName, lpDefault,
				lpReturnedString, nSize, lpFileName );
	return StringLength( lpReturnedString );			
}

static long NoFileWriteString( 
						char *lpAppName,  /* points to section name */
						char *lpKeyName,  /* points to key name */
						char *lpString,		/* pointer to string to add */
						char *lpFileName  /* points to initialization filename */
)
{
	int handle;
	char buf[512] = "[";

	CatString( buf, lpAppName );
	CatString( buf, "]\r\n" );
	CatString( buf, lpKeyName );
	CatString( buf, "=" );
	CatString( buf, lpString );
	CatString( buf, "\r\n" );
	
	handle = open( lpFileName, O_CREAT | O_WRONLY, 0, 0 );
	if( handle == -1 ) 
		return -1;
	if(	write( handle, buf, StringLength( buf ) ) == -1 )
	{
		close( handle );
		return -1;
	}
	close( handle );
	return 0;
}

static long NoSectionWriteString( 
						char *lpAppName,  /* points to section name */
						char *lpKeyName,  /* points to key name */
						char *lpString,		/* pointer to string to add */
						char *lpFileName  /* points to initialization filename */
)
{
	int handle;
	char buf[512] = "[";

	CatString( buf, lpAppName );
	CatString( buf, "]\r\n" );
	CatString( buf, lpKeyName );
	CatString( buf, "=" );
	CatString( buf, lpString );
	CatString( buf, "\r\n" );

	handle = open( lpFileName, O_RDWR, 0, 0 );
	if( handle == -1 ) return -1;
	lseek( handle, 0, SEEK_END );
	if(	write( handle, buf, StringLength( buf ) ) == -1 )
	{
		close( handle );
		return -1;
	}
	close( handle );
	return 0;
}

static long NoKeyWriteString( 
						char *lpAppName,  /* points to section name */
						char *lpKeyName,  /* points to key name */
						char *lpString,		/* pointer to string to add */
						char *lpFileName  /* points to initialization filename */
)
{
	char buf[512];
	char readbuf[512];
	int handle;
	int offset = 0;
	int ret; 
	char Section[256]="[";
	char *ptr;

	CatString( Section, lpAppName );
	CatString( Section, "]" );

/*	CopyString( buf, lpKeyName );
	CatString( buf, "=" );
	CatString( buf, lpString );
	CatString( buf, "\r\n" );
*/
//	CopyString( buf, "\r\n" );
	CopyString( buf, lpKeyName );
	CatString( buf, "=" );
	CatString( buf, lpString );
	CatString( buf, "\r\n" );

	handle = open( lpFileName, O_RDWR, 0, 0 );
	if( handle == -1 )
		return -1;
	memset( readbuf, 0, sizeof(readbuf) );
	while( (ret=ReadLine( handle, readbuf, sizeof(readbuf) )) > 0 )
	{
		offset += ret;
		ptr = readbuf;
		while( *ptr && *ptr<=0x20 ) 
			ptr++;
		if( StringLength(ptr) < StringLength(Section) ) 
			continue;
		if( CompareStringNoCase( ptr, Section ) == 0 )
			break;
		memset( readbuf, 0, sizeof(readbuf) );
	}
	if( ret == -1 ) 
	{
		close( handle );
		return -1;
	}
	ret = FInsert( handle, offset, buf, StringLength(buf) );
	close( handle );
	return ret;
}

static long ChangeProfileString( 
						char *lpAppName,  /* points to section name */
						char *lpKeyName,  /* points to key name */
						char *lpString,		/* pointer to string to add */
						char *lpFileName  /* points to initialization filename */
)
{
	char buf[512];
	int handle;
	int offset = 0;
	int ret;
	char Section[256]="[";
	char *ptr;
	int strl = 0;
	int Found = 0;
	int sectionbegin;
	CatString( Section, lpAppName );
	CatString( Section, "]" );

	handle = open( lpFileName, O_RDWR, 0, 0 );

	if( handle == -1 ) 
		return -1;
	
	while( (ret=ReadLine( handle, buf, sizeof(buf) )) > 0 )
	{
		offset += ret;
		ptr = buf;
		while( *ptr && *ptr<=0x20 ) 
			ptr++;
		if( StringLength(ptr) < StringLength(Section) ) 
			continue;
		if( CompareStringNoCase( ptr, Section ) == 0 )
			break;
	}
	if( ret == -1 ) 
	{
		close( handle );
		return -1;
	}
	sectionbegin = offset - ret;
	while( (ret=ReadLine( handle, buf, sizeof(buf) )) >0 )
	{
		char *end;
		offset += ret;
		ptr=buf;
		while( *ptr && *ptr<=0x20 )
			ptr++;

		if( *ptr == '[' )
			break;
		if( lpKeyName )
		{
			end = strstr( ptr, "=" );
			if( end )
			{
				*end = 0;
				if( CompareStringNoCase( ptr, lpKeyName ) == 0 )
				{
					strl = StringLength(ptr) ; 
					*end = '=';
					Found = 1;
					break;
				}
			}
		}
	}

	if( lpKeyName == NULL )
	{
		if( ret < 0 )
			ret = 0;
		ret = FDelete( handle,lpFileName, sectionbegin, offset-sectionbegin-ret );
		close( handle );
		return ret;
	}

	if( Found == 0 )
	{
		close( handle );
		return -1;
	}


//???????????????
//	offset = offset - ret + 1;
	offset = offset - ret;
//???????????????
	lseek( handle, offset, SEEK_SET );

	if( FDelete( handle,lpFileName, offset, ret ) == -1 )
	{
		close( handle );
		return -1;
	}
	if( !lpString )
	{
		close (handle);
		return 0;
	}
	CopyString( buf, lpKeyName );
	CatString( buf, "=" );
	CatString( buf, lpString );
	CatString( buf, "\r\n" );

	FInsert ( handle, offset, buf, strlen(buf));
//	CatString( buf, "\r\n" );


//	if( ret > StringLength(buf) )
#ifdef OLD_CODE		
	if( strl > StringLength(buf) )
	{
		write( handle, buf, StringLength(buf) );
		
		//for( ;ret > StringLength(buf); ret -- )
		for( ;strl > StringLength(buf); strl -- )
			write( handle, " ", 1 );
	}
	else if ( strl < StringLength(buf) )
	{
		//write( handle, buf, ret );
		//FInsert( handle, offset+ret, buf+ret, StringLength(buf)-ret );
		write( handle, buf, strl );
		FInsert( handle, offset+strl, buf+strl, StringLength(buf)-strl );
	}
	else /*如果长度相等*/
		write( handle, buf, strl );
#endif		// kent 19991123
	close( handle );
	return 0;
}

long FS_WritePrivateProfileString( 
						char *lpAppName,  /* points to section name */
						char *lpKeyName,  /* points to key name */
						char *lpString,		/* pointer to string to add */
						char *lpFileName  /* points to initialization filename */
)
{
	int ret;
	char buf[512];
	char Section[128];
//	char Key[128];

	CopyString( Section, "[" );
	CatString( Section, lpAppName );
	CatString( Section, "]" );
//	if( lpKeyName )
// 		CopyString( Key, lpKeyName );

	if( lpAppName && lpKeyName && lpString )
		ret =  __FS_GetPrivateProfileString(
									lpAppName, 
									lpKeyName, 
									"", 
									buf, 
									sizeof(buf), 
									lpFileName);
	else
		ret = 0;

	switch ( ret )
	{
		case NOFILE:
			return NoFileWriteString( lpAppName, lpKeyName, lpString, lpFileName );
		case NOSECTION:
			return NoSectionWriteString( lpAppName, lpKeyName, lpString, lpFileName );
		case NOKEY:
			return NoKeyWriteString( lpAppName, lpKeyName, lpString, lpFileName );
		default:
			return ChangeProfileString( lpAppName, lpKeyName, lpString, lpFileName );
	}
}

long FS_WritePrivateProfileInt( 
						char *lpAppName,  /* points to section name */
						char *lpKeyName,  /* points to key name */
						long	 nValue,			/* pointer to string to add */
						char *lpFileName  /* points to initialization filename */
)
{
	char buf[64];
	IntToString( nValue, buf );
	
	return FS_WritePrivateProfileString
						( lpAppName, 
							lpKeyName, 
							buf, 
							lpFileName );
	
}

long FS_GetAllSectionNames( char *lpReturn, 
												 unsigned long nSize, 
												 char *lpFileName )
{
	int hFile;
	char buf[512];
	int ret, retVal = 0;
	char *beg, *end;

	ret = 0;
	hFile = open( lpFileName, O_RDONLY, 0, 0 );
	if( hFile == -1 )
	{
		*lpReturn = 0;
		return ret;
	}
	while( (ret=ReadLine( hFile, buf, sizeof(buf) )) >0 )
	{
		if( buf[0] == '[' )
		{
			beg = buf;
			while( *beg <= 0x20 && *beg )
				beg ++;
			end = strstr( beg, "]" );
			if( end )
			{
				*end = 0;
				if( nSize-retVal > (unsigned long)(end-beg + 1) )
				{
					memcpy( lpReturn+retVal, beg+1, end-beg );
					retVal += end-beg;
				}
				else
					break;
			}
		}
	}
	lpReturn[retVal] = 0;
	close( hFile );
	return retVal;
}

long FS_GetAllKeyNamesInSection(						
						char *lpAppName,  /* points to section name */
						char *lpString,		/* pointer to string to add */
						int  nSize,
						char *lpFileName  /* points to initialization filename */
)
{
	char readbuf[512];
	int handle;
	int ret; 
	char Section[256]="[";
	char *ptr, *end;
	int retVal = 0;

	CatString( Section, lpAppName );
	CatString( Section, "]" );

	handle = open( lpFileName, O_RDWR, 0, 0 );
	if( handle == -1 )
		return -1;
	memset( readbuf, 0, sizeof(readbuf) );
	while( (ret=ReadLine( handle, readbuf, sizeof(readbuf) )) > 0 )
	{
		ptr = readbuf;
		while( *ptr && *ptr<=0x20 ) 
			ptr++;
		if( StringLength(ptr) < StringLength(Section) ) 
			continue;
		if( CompareStringNoCase( ptr, Section ) == 0 )
			break;
		memset( readbuf, 0, sizeof(readbuf) );
	}
	if( ret == -1 ) 
	{
		close( handle );
		return -1;
	}
	while( (ret=ReadLine( handle, readbuf, sizeof(readbuf) )) > 0 )
	{
		ptr = readbuf;
		while( *ptr && *ptr<=0x20 ) 
			ptr++;
		if( *ptr == '[' )
			break;
		end = strstr( ptr, "=" );
		if( end )
		{
			*end = 0;
			if( nSize-retVal > end-ptr + 1 )
			{
				memcpy( lpString+retVal, ptr, end-ptr+1 );
				retVal += end-ptr+1;
			}
			else
				break;
		}
	}
	lpString[retVal] = 0;
	close( handle );
	return retVal;
}

long FS_ClearKey()
{
	return 0;
}
/* END OF PROGRAM FILE */
