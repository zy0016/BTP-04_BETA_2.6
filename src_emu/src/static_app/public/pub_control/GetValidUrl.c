
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define alphanum(c)		isalnum(c)

#define mark(c)			\
						( (c=='-') ||\
						  (c=='_') ||\
						  (c=='.') ||\
						  (c=='!') ||\
						  (c=='~') ||\
						  (c=='*') ||\
						  (c=='\'')||\
						  (c=='(') ||\
						  (c==')') )

#define escaped(c)		( c=='%' )

#define unreserved(c)	( alphanum(c) || mark(c) )

#define reserved(c)		\
						( (c==';') ||\
						  (c=='/') ||\
						  (c=='?') ||\
						  (c==':') ||\
						  (c=='@') ||\
						  (c=='&') ||\
						  (c=='=') ||\
						  (c=='+') ||\
						  (c=='$') ||\
						  (c==',') )

#define part_reserved(c) \
						 (	(c=='&') ||\
							(c=='=') ||\
							(c=='+') ||\
							(c=='$') ||\
							(c==',') )

#define uric(c)			( reserved(c) || unreserved(c) || escaped(c) || (c=='#'))




static int plx_isValid( char *purl )
{	

	char c, *p1, *p2, *p3, *url;
	int i, len, sign;
	int scheme=0, authority=0;
	int ret = 0;
	
	if (purl == NULL)
		return 0;

	url = purl;

	// step1: check scheme;
	if( ( p1 = strchr( url, ':' ) ) != NULL ) 
	{

		scheme = 1;

		p2 = url; url = p1 + 1;

		len = p1 - p2;

		if( (len<=0) || ( !isalpha(p2[0]) ) )
			goto Return;

		for( i =1; i<len; i++ ){

			c = p2[i];

			if( alphanum(c) ) continue;
			if( (c=='+') || (c=='-') || (c=='.') ) continue; //why '+''-''.' is valid?

			goto Return;
		}
	}
	
	if( scheme == 1 ){

	//step2: check authority;
	if( strstr( url, "//" ) == url ){

		authority = 1;

		url += 2;

		p2 = url;

		if( ( p3 = strchr( p2, '/' ) ) != NULL )
			len = p3 - p2;
		else
			len = strlen( p2 );

		for( i=0; i<len; i++ ){

			c = p2[i];

			if( unreserved(c) || escaped(c) || part_reserved(c) || 
						(c==';') || (c==':') || (c=='@') )
				continue;
			else 
				goto Return;
		}

		url += i;

	}// end of if( strstr( url, "//" ) == url )

	if( (authority==0) && (url[0]!='/')	){ //opaque_path

		len = strlen(url);
		if( len < 1 ) 
			goto Return;

		c = url[0];

		if( !( unreserved(c) || escaped(c) || part_reserved(c) ||
			(c==';') || (c==':') || (c=='@') || (c=='?') ) )
			goto Return;

		for( i=1; i<len; i++ ){

			if( url[i] == '#' ){

				url += i+1;
				break;
			}

			if( !uric(url[i]) )
				goto Return;
		}

		if( i == len ) 
		{
			ret = 1;
			goto Return;
		}
	}

	}// end of if( scheme == 1 )

	
	//step3: abs_path

	if( url[0] != '/' ){	//rel_segment

		len = strlen(url);

		i = 0;

		while( (i<len) && 
			   ( (c=url[i++]) != '/' ) &&
			   ( c != '#' ) &&
			   ( c != '?' )
			  ) 
		{

			if( !( unreserved(c) || escaped(c) || part_reserved(c) ||
						(c==';') || (c=='@') ) )
				goto Return;
		}

		if( i==len )
		{
			ret = 1;
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

		if( (c=='?') || (c=='#') ) break;

		switch( sign ){
		case SEGMENT:

			if( c==';' ) { sign = PARAM; break; }
			
			if( !( unreserved(c) || escaped(c) || part_reserved(c) ||
					(c==':') || (c=='@') || (c=='/') ) )
				goto Return;

			break;

		case PARAM:

			if( c=='/' ) { sign = SEGMENT; break; }

			if( !( unreserved(c) || escaped(c) || part_reserved(c) ||
					(c==':') || (c=='@') || (c==';') ) )
				goto Return;

			break;
		}// end of switch
	}
    
    if (i < len)
    {
        for( i++; i<len ; i++ ){ // query or fragment;
            
            c = url[i];
            
            if( !uric(c) )
                goto Return;
        }
    }

	ret = 1;

Return:

	//return ret;
	return (url + i - purl);
}

int GetValidUrlEx(const char * url, int len)
{
	int iUrlLen;
	int iValidLen;
	char *pUrl = (char*)url;

	if (url == NULL)
		return 0;

	iUrlLen = strlen(url);
	if (iUrlLen > len)
	{
		pUrl = (char *)malloc(len + 1);
		strncpy(pUrl, url, len);
		pUrl[len] = 0;
	}

	iValidLen = plx_isValid(pUrl);

    if (iValidLen > len)
        iValidLen = len;

	if (pUrl != url)
		free(pUrl);

	return iValidLen;
}
