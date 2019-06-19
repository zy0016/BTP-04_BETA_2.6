
#ifndef COOKIES_H_
#define COOKIES_H_

/* cookies数据结构 */
typedef struct tagCookie COOKIE;
struct tagCookie
{
    char		*pName;
    char		*pValue;
    char		*pDomain;
    char		*pPath;
    unsigned long tExpiration;
    int			bSecure;
	COOKIE		*pNext;
};
/*
void Cookie_SetDefault(COOKIE *pCookie, char *pUrl, char *pHost);
int Cookie_SetName(COOKIE *pCookie, const char *pName);
int Cookie_SetValue(COOKIE *pCookie, const char *pValue);
int Cookie_SetPath(COOKIE *pCookie, const char *pPath);
int Cookie_SetDomain(COOKIE *pCookie, const char *pDomain);
int Cookie_SetSecure(COOKIE *pCookie, int bSecure);
time_t Cookie_SetExpiration(COOKIE *pCookie, time_t tExpiration);
int Cookie_IsValidate(COOKIE *pCookie, char *pUrl, char *pHost);
int Cookies_Add(COOKIE *pCookie);
void Cookie_Free(COOKIE *pCookie);
int Cookie_Garbage(void);
*/

int Cookie_Parse(char *pSetCookie, char *pUrl, char *pHost);
COOKIE *Cookie_Lookup(char *pUrl, char *pHost);
int Cookie_ToString(COOKIE *pCookie, char *pBuf, int *BufLen);
int Cookie_GetSize(COOKIE *pCookie);
void Cookie_Destroy(void);

char *GetNextParam(char ** pStr);
char *GetNextToken(char **pStr, int bValue);

#endif
