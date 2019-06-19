/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Help application
 *
 * Purpose  : provide to get topic and help information
 *            
\**************************************************************************/

//#include "plx_kern.h"
#include "window.h"
#include "string.h"
//#include "oserror.h"
//#include "fileapi.h"
#include "unistd.h"
#include "types.h"
#include "stat.h"
#include "fcntl.h"
#include "mullang.h"

#include "help.h"
#define ContentMaxLength	512
#define KeyMaxLength		50
#define SectionMaxLength    50

long Help_GetAllSectionNames( char *lpReturn, 
												 unsigned long nSize, 
												 char *lpFileName );

long Help_GetAllKeyNamesInSection(						
						char *lpAppName,  /* points to section name */
						char *lpString[],		/* pointer to string to add */
						int  nSize,
						char *lpFileName  /* points to initialization filename */
);



/* Delete string function from file system */
static char *CopyString( char *dest, const char *src )
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

static char *CatString( char *dest, const char *src )
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

		return (char)(c + 'a' - 'A');
	else 
		return c;

}

static char ToUpper( char c )
{
	if( c >= 'a' && c <= 'z' )
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

static void IntToString( int nValue, char *buf )
{
	char stack[15] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	int i = 0;
	int temp = nValue;
	if( nValue < 0 )
		temp = -nValue;
	while( temp != 0 )
	{
		stack[i++] = (char)(temp-(temp/10)*10);
		temp = temp / 10;
	}
	temp = 0;
	if( nValue < 0 )
		buf[temp++] = '-';
	i --; 
	while( i >= 0 )
	{
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
	
/*
		do
		{
			ret = read( handle, &c, 1 );
			if( ret != 1 ) 
				return -1; 
			numRead ++ ;
			if( c > 0x20 ) 
				break;
			
		} while( ret == 1 );*/
	
//	buf[i++] = c;

	do
	{
		ret = read( handle, &c, 1 );
		if( ret != 1 ) 
			break;
		numRead ++;
		if( c == 0x0d  )
		{
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
	
	return numRead;
}

/*
static int FInsert(int handle, int offset, void * data, int length)
{
	int fsize;
	void * tem;
	fsize = GetFileSize( handle );

	if (length <= 0)
		return 0;

	if( offset < fsize )
	{
		tem = LocalAlloc(LMEM_FIXED, fsize - offset );
		if (tem==NULL)
		{
			OS_SetError( E_MEMORY );
			return -1;
		}
		lseek( handle, offset, SEEK_SET );
		if( read( handle, tem, fsize - offset ) == -1 )
		{
			LocalFree( tem );
			return -1;
		}
	}
	lseek( handle, offset, SEEK_SET );
	write( handle, data, length );
	if( offset < fsize )
	{
		lseek(handle, offset+length, SEEK_SET );
		write( handle, tem, fsize - offset );
		LocalFree( tem );
	}
	return 0;
}*/

/*
static int FDelete(int handle, int offset, int length)
{
	char *buf;
	int len;
	len = GetFileSize( handle ) - offset - length;
	buf = LocalAlloc(LMEM_FIXED, len );
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
			TruncateFile( handle, 2 );
		}
		else
			TruncateFile( handle, offset + len );
	}
	LocalFree( buf );
	return len;
}*/

static BOOL ContentEndOrNo(char * buff)
{
	int length = 0;

	while (*buff) {
		length++;
		buff++;
	}
	buff--;

	if(*buff == '0')
	{
		buff--;
		if (*buff == '\\')
			return TRUE;
	}

	return FALSE;
}

static BOOL ContinueRead(int handle, char *ptr)
{
	char *end;

	if(ReadLine( handle, ptr , ContentMaxLength) <= 0 )
		return -1;

/*
	while( *ptr && *ptr<=0x20 ) 
		ptr++;*/


	if( *ptr == '[' )
		return -1;

	end = strstr( ptr, "=" );
	if (end)
		return -1;

	return 1;
}

#define NOFILE -1
#define NOSECTION -2
#define NOKEY -3

static int SetBeginReadPostion(int handle, const char *flag)
{
	char readbuf[ContentMaxLength];
	char *ptr;
	char Section[SectionMaxLength] = "[";
	int ret;

	CatString( Section, flag );
	CatString( Section, "]" );

	while( (ret=ReadLine( handle, readbuf, sizeof(readbuf) )) > 0 )
	{
		ptr = readbuf;
		/*
			while( *ptr && *ptr<=0x20 ) 
						ptr++;*/
			
		if( *ptr != '[')
			continue;
		if( StringLength(ptr) < StringLength(Section) ) 
			continue;
		if( CompareStringNoCase( ptr, Section ) == 0 )
		{
			ret = 1;
			break;
		}
		memset( readbuf, 0, sizeof(readbuf) );
	}
	if( ret == -1 ) 
		return -1;
	return ret;
}
static int FindSection(int handle, const char *flag)
{
	return SetBeginReadPostion(handle, flag);
}

static int FindKey(int handle, char *flag, char *readbuf, int size)
{
	int ret;
	char *end, *ptr;

	while( (ret=ReadLine( handle, readbuf, size)) >0 )
	{
		
		ptr=readbuf;
		/*
			while( *ptr && *ptr<=0x20 ) 
						ptr++;*/
			
		
		if( *ptr == '[' )
		{
			ret = -1;
			break;
		}
		end = strstr( ptr, "=" );
		if (end)
		{
			*end = 0;
			if( CompareStringNoCase( ptr, flag) == 0 )
			{	
				*end = '=';
				ret =1;
				break;
			}
		}
	}

	return ret;
}

long __Help_GetPrivateProfileString( 
						char *lpAppName,  /* points to section name */
						char *lpKeyName,  /* points to key name */
						const char *lpDefault,  /* points to default string */
						char *lpReturnedString,  /* points to destination buffer */
						long nSize,				/* size of destination buffer */
						char *lpFileName  /* points to initialization filename */
)
{
	long ret;
	int handle;
	char *ptr = NULL;
	int length = 0;
	BOOL bEnd = FALSE;
	char cLinebuf[KeyMaxLength + ContentMaxLength];
	char *pTmp;


	handle = open( lpFileName, O_RDONLY, 0, 0 );
	if( handle == -1 )
	{
//		close( handle );
		CopyString( lpReturnedString, lpDefault );
		return NOFILE;
	}
/* find section;*/
	ret = FindSection(handle, lpAppName);

/* No this section;*/
	if( ret < 0 )
	{
		close( handle );
		CopyString( lpReturnedString, lpDefault );
		return NOSECTION;
	}
/*find key*/
	memset(cLinebuf, 0, sizeof(cLinebuf));
	ret = FindKey(handle, lpKeyName, cLinebuf, KeyMaxLength + ContentMaxLength);
	
/* Not Found;*/
	if( ret < 0 )
	{ 
		close( handle );
		CopyString( lpReturnedString, lpDefault );
		return NOKEY;
	}
/* Found;*/
	
	ptr = cLinebuf;
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

	while(!ContentEndOrNo(ptr))
	{
		if(ContinueRead(handle, ptr))
			CatString(lpReturnedString , ptr);
		else
			break;
	}
	
	pTmp = lpReturnedString;
	while(*(pTmp++) != NULL);
	*(pTmp-3) = 0;
	
	
	close( handle );
	return 0;
}

/*
long Help_GetPrivateProfileInt
				( char *lpAppName,  / * address of section name * /
				char *lpKeyName,  / * address of key name * /
 				unsigned long nDefault,   		/ * return value if key name is not found * /
 				char *lpFileName  / * address of initialization filename * /
)
{
	char buf[64];

	if( __Help_GetPrivateProfileString(
									lpAppName, 
									lpKeyName, 
									"", 
									buf, 
									sizeof(buf), 
									lpFileName)  < 0 )
		return nDefault;
	else
		return StringToInt( buf );
}*/


long Help_GetPrivateProfileString( 
						char *lpAppName,  /* points to section name */
						char *lpKeyName,  /* points to key name */
						const char *lpDefault,  /* points to default string */
						char *lpReturnedString,  /* points to destination buffer */
						unsigned long nSize,				/* size of destination buffer */
						char *lpFileName  /* points to initialization filename */
)
{
	/*
	if( lpAppName == NULL )
		return Help_GetAllSectionNames( lpReturnedString, nSize, lpFileName);
	if( lpKeyName == NULL )
		return Help_GetAllKeyNamesInSection(						
										lpAppName, 
										lpReturnedString,
										nSize,
										lpFileName );*/

	__Help_GetPrivateProfileString(
				lpAppName, lpKeyName, lpDefault,
				lpReturnedString, nSize, lpFileName );
	return StringLength( lpReturnedString );			
}

/*
static long NoFileWriteString( 
						char *lpAppName,  / * points to section name * /
						char *lpKeyName,  / * points to key name * /
						char *lpString,		/ * pointer to string to add * /
						char *lpFileName  / * points to initialization filename * /
)
{
	int handle;
	char buf[ContentMaxLength] = "[";

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
						char *lpAppName,  / * points to section name * /
						char *lpKeyName,  / * points to key name * /
						char *lpString,		/ * pointer to string to add * /
						char *lpFileName  / * points to initialization filename * /
)
{
	int handle;
	char buf[ContentMaxLength] = "[";

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
						char *lpAppName,  / * points to section name * /
						char *lpKeyName,  / * points to key name * /
						char *lpString,		/ * pointer to string to add * /
						char *lpFileName  / * points to initialization filename * /
)
{
	char buf[ContentMaxLength];
	char readbuf[ContentMaxLength];
	int handle;
	int offset = 0;
	int ret; 
	char Section[256]="[";
	char *ptr;

	CatString( Section, lpAppName );
	CatString( Section, "]" );

/ *	CopyString( buf, lpKeyName );
	CatString( buf, "=" );
	CatString( buf, lpString );
	CatString( buf, "\r\n" );
* /
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
//	ret = FInsert( handle, offset, buf, StringLength(buf) );
	close( handle );
	return ret;
}*/


/*
static long ChangeProfileString( 
						char *lpAppName,  / * points to section name * /
						char *lpKeyName,  / * points to key name * /
						char *lpString,		/ * pointer to string to add * /
						char *lpFileName  / * points to initialization filename * /
)
{
	char buf[ContentMaxLength];
	int handle;
	int offset = 0;
	int ret;
	char Section[256]="[";
	char *ptr;
	//liuyuan添加
	int strl = 0;
	int Found = 0;
	//liuyuan添加
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
//		ret = FDelete( handle, sectionbegin, offset-sectionbegin-ret );
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

	if( FDelete( handle, offset, ret ) == -1 )
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

#ifdef OLD_CODE		// kent 19991123
	if( strl > StringLength(buf) )
	{
		write( handle, buf, StringLength(buf) );
		
		//for( ;ret > StringLength(buf); ret -- )
		for( ;strl > StringLength(buf); strl -- )
			write( handle, " ", 1 );
	}
	else if ( strl < StringLength(buf) )
	{
		//WriteFile( handle, buf, ret );
		//FInsert( handle, offset+ret, buf+ret, StringLength(buf)-ret );
		write( handle, buf, strl );
		FInsert( handle, offset+strl, buf+strl, StringLength(buf)-strl );
	}
	else / *如果长度相等* /
		write( handle, buf, strl );
#endif		// kent 19991123
	close( handle );
	return 0;
}*/


/*
long FS_WritePrivateProfileString( 
						char *lpAppName,  / * points to section name * /
						char *lpKeyName,  / * points to key name * /
						char *lpString,		/ * pointer to string to add * /
						char *lpFileName  / * points to initialization filename * /
)
{
	int ret;
	char buf[ContentMaxLength];
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
}*/


/*
long FS_WritePrivateProfileInt( 
						char *lpAppName,  / * points to section name * /
						char *lpKeyName,  / * points to key name * /
						long	 nValue,			/ * pointer to string to add * /
						char *lpFileName  / * points to initialization filename * /
)
{
	char buf[64];
	IntToString( nValue, buf );
	
	return FS_WritePrivateProfileString
						( lpAppName, 
							lpKeyName, 
							buf, 
							lpFileName );
	
}*/


long Help_GetAllSectionNames( char *lpReturn, 
												 unsigned long nSize, 
												 char *lpFileName )
{
	int hFile;
	char buf[ContentMaxLength];
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


long Help_GetAllKeyNamesInSection(						
						char *lpAppName,  /* points to section name */
						char *lpString[],		/* pointer to string to add */
						int  nSize,
						char *lpFileName  /* points to initialization filename */
)
{
	char readbuf[ContentMaxLength];
	int handle;
	int ret; 
	char *ptr, *end;
	int retVal = 0;
	char currLanguage[10];

	handle = open( lpFileName, O_RDONLY, 0, 0 );
	if( handle == -1 )
		return -1;
	memset( readbuf, 0, sizeof(readbuf));
	memset(currLanguage, 0, 10);

    strcpy(currLanguage, GetActiveLanguage());
	if (strlen(currLanguage) == 0)
	{
		strcpy(currLanguage, "english");
	}

	if (0 == strcmp(lpFileName, TopicFilePath) )
	{
		ret = SetBeginReadPostion(handle, currLanguage);
		if( ret == -1 ) 
		{
			close( handle );
			return -1;
		}
	}
	
	ret = FindSection(handle, lpAppName);
	if( ret == -1 ) 
	{
		close( handle );
		return -1;
	}
	
	while( (ret=ReadLine( handle, readbuf, sizeof(readbuf) )) > 0 )
	{
		ptr = readbuf;
	//	while( *ptr && *ptr<=0x20 ) 
	//		ptr++;
		
		if( *ptr == '[' )
			break;
		end = strstr( ptr, "=" );
		if( end )
		{
			*end = 0;
			if( nSize > end-ptr + 1 )
			{
				memcpy( lpString[retVal], ptr, end-ptr+1 );
				retVal ++;
			}
			else
				break;
		}
	}
	lpString[retVal] = 0;
	close( handle );
	return retVal;
}
int Help_HasKeyOrNoInSection(const char *lpAppName, char *lpFileName )
{
	char readbuf[ContentMaxLength];
	int handle;
	int ret; 
	char *ptr;
	char currLanguage[10];

	handle = open( lpFileName, O_RDONLY, 0, 0 );
	if( handle == -1 )
		return -1;
	memset( readbuf, 0, sizeof(readbuf));
    memset(currLanguage, 0, 10);

    strcpy(currLanguage, GetActiveLanguage());
	if (strlen(currLanguage) == 0)
	{
		strcpy(currLanguage, "english");
	}
	ret = SetBeginReadPostion(handle, currLanguage);
	if( ret == -1 ) 
	{
		close( handle );
		return -1;
	}
	ret = FindSection(handle, lpAppName);
	if( ret == -1 ) 
	{
		close( handle );
		return -1;
	}
	
	while( (ReadLine( handle, readbuf, sizeof(readbuf) )) > 0 )
	{
		ptr = readbuf;
	/*
		while( *ptr && *ptr<=0x20 ) 
				ptr++;*/
	
		if( *ptr == '[' )
		{
			ret = 0;
			break;
		}
		if(strstr( ptr, "=" ))
		{
			ret = 1;
			break;
		}	
	}
	close( handle );
	return ret;
}


/*
long Help_ClearKey()
{
	return 0;
}*/

/* END OF PROGRAM FILE */
