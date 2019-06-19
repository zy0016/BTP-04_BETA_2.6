/***************************************************************************
*
*                      Pollex Mobile Platform
*
* Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
*                       All Rights Reserved 
*
* Module   : Pass Change
*
* Purpose  : Provide a control to select a color.    
*            
\**************************************************************************/

#include "window.h"
#include "str_public.h"
#include "str_plx.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "pubapp.h"

static int GetCurselNextCoordinate(PSTR pStrSource,int textLength,int curOffset,int curlength,int *offset,int* nLength);
static int GetCurselPreCoordinate(PSTR pStrSource,int textLength,int curOffset,int curlength,int *offset,int* nLength);
static int GetCurselCoordinate(PSTR pStrSource,int textLength,int curOffset,int curlength,int *offset,int* nLength,int bForward);
static BOOL IsNotValidate(char ch) ;
static int GetCurselNextNumber(PSTR pStrSource,int textLength,int curOffset,int curlength,int *offset,int* nLength);
static int GetCurselPreNumber(PSTR pStrSource,int textLength,int curOffset,int curlength,int *offset,int* nLength);
static int GetCurselNumber(PSTR pStrSource,int textLength,int curOffset,int curlength,int *offset,int* nLength,int bForward);
static int GetCurselNextEmail(PSTR pStrSource,int textLength,int curOffset,int curlength,int *offset,int* nLength);
static int GetCurselPreEmail(PSTR pStrSource,int textLength,int curOffset,int curlength,int *offset,int* nLength);
static int GetCurselEmail(PSTR pStrSource,int textLength,int curOffset,int curlength,int *offset,int* nLength,int bForward);
static int GetCurselNextUrl(PSTR pStrSource,int textLength,int curOffset,int curlength,int *offset,int* nLength);
static int GetCurselPreUrl(PSTR pStrSource,int textLength,int curOffset,int curlength,int *offset,int* nLength);
static int GetCurselUrl(PSTR pStrSource,int textLength,int curOffset,int curlength,int *offset,int* nLength,int bForward);

/***********************************************************************************
函数名：
GetCursel
函数功能：
查找用户给定字符串中当前offset附近的符合条件的String，并得到该url的offset以及String的长度。
参数含义：
pStrSource     源字符串
textLength--字符串长度
int curOffset  当前选中String的offset
int curlength  当前选中String的长度
int *offset    查找到到的offset
int* length    查找到的String的offset(均相对于起始位置的)
int bType      查找Email(TVS_EMAIL)、Url(TVS_URL)、Number(TVS_NUMBER)，or TVS_COORDINATE
int bForward   查找方向 1---last; 0----next
返回值：
成功找到用户想查找的URL，则返回1
否则返回－1
***********************************************************************************/
int GetCursel(PSTR pStrSource,int textLength,int curOffset,int curlength,int *offset,int* length,int bType,int bForward)
{
	int result = -1;
	switch(bType)
	{
   	case TVS_NUMBER:
			result =  GetCurselNumber(pStrSource,textLength,curOffset,curlength,offset,length,bForward);
		break;
   	case TVS_URL:
			result = GetCurselUrl(pStrSource,textLength,curOffset,curlength,offset,length,bForward);
		break;
   	case TVS_EMAIL:
			result = GetCurselEmail(pStrSource,textLength,curOffset,curlength,offset,length,bForward);
		break;
   	case TVS_COORDINATE:
			result = GetCurselCoordinate(pStrSource,textLength,curOffset,curlength,offset,length,bForward);
		break;
   	default:
			result = -1;
			break;
	}
	return result;
	
}

/***********************************************************************************
函数名：
GetCurselNextCoordinate
函数功能：
查找用户给定字符串中当前offset之后的符合条件的Coordinate，并得到该Coordinate的offset以及Coordinate的长度。
参数含义：
pStrSource--源字符串
textLength--字符串长度
int curOffset 当前选中Coordinate的offset
int curlength  当前选中Coordinate的长度
int *offset   查找到到的offset
int* nLength   查找到的Coordinate的offset
返回值：
成功找到用户想查找的Coordinate，则返回1
否则返回－1
备注：只支持查找形如(1,2) 的坐标，如果需要支持多维坐标，将下述宏打开

***********************************************************************************/
//#define SUPPORT_MULTI_COOR
static int GetCurselNextCoordinate(PSTR pStrSource,int textLength,int curOffset,int curlength,int *offset,int* nLength)
{
    int length = textLength;
    PSTR pCurrent, pEnd,pstrTemp1,pstrTemp2;
    pCurrent = pStrSource + curOffset + curlength;
    pEnd = pStrSource + curOffset + curlength;
    while(*pCurrent!='\0')
    {
        if(*pCurrent != '(')//起始字符可以允许是'('
        {
            pCurrent++;
            pEnd++;
        }
        else
        {
            int isValidate = 0;
            pEnd++;
            if(*pEnd>0x39||*pEnd<0x030)//'('后面跟的不是数字，那么无效
            {
                pCurrent = pEnd;
                pEnd++;
                pCurrent++; 
            }
            else
            {
                int validate = 0;
                while(*pEnd!=')'&&*pEnd!=' '&&*pEnd !='\0'&&*pEnd!='\r'&&*pEnd!='\n')
                {
                    pEnd++;
                    
                    //需要保证'('与','之间只能出现数字,且只是支持两维坐标
                    if(*pEnd == ',')
                    {
#ifndef SUPPORT_MULTI_COOR
                        if(validate == 0)
                            validate = 1;
                        else
                            break;//进入此分支表明出现了一个以上的','
#else
                        validate = 1;
#endif
                        pstrTemp1 = pEnd-1;//
                        while(pstrTemp1 != pCurrent)
                        {
                            if(*pstrTemp1>0x39||*pstrTemp1<0x030)
                            {
                                isValidate--;
                                break;
                            }
                            pstrTemp1--;
                        }
                        isValidate++;	    
                        
                        if(isValidate != 1)
                            break;//已经可以得出无效
                        
                        pstrTemp1 = pEnd + 1;
                    }
                    
                    if(*pEnd == ')'&&validate == 1)
                    {
                        pstrTemp2 = pEnd - 1;
                        while(pstrTemp2 != pstrTemp1)
                        {
                            if(*pstrTemp2>0x39||*pstrTemp2<0x030)
                            {
                                isValidate--;
                                break;
                            }
                            pstrTemp2--;
                        }
                        isValidate++;//只有此时等于2才有效
                        
                        if(isValidate == 2)
                        {
                            *offset = pCurrent - pStrSource;
                            if(*offset<0)
                                *offset = 0;//确保不会出现错误
                            *nLength = pEnd - pCurrent +1;
                            if(*nLength<0)
                                *nLength = -*nLength;
                            return 1;
                        }
                        else
                            break;
                    }
                }                
                pCurrent = pEnd;
                pEnd++;
                pCurrent++; 
                
            }	
            
        }
    }
    return -1;
}

/***********************************************************************************
函数名：
GetCurselPreCoordinate
函数功能：
查找用户给定字符串中当前offset之前的符合条件的Coordinate，并得到该Coordinate的offset以及Coordinate的长度。
参数含义：
pStrSource--源字符串
textLength--字符串长度
int curOffset 当前选中Coordinate的offset
int curlength  当前选中Coordinate的长度
int *offset   查找到到的offset
int* nLength   查找到的Coordinate的offset
返回值：
成功找到用户想查找的Coordinate，则返回1
否则返回－1
备注：只支持查找形如(1,2) 的坐标，如果需要支持多维坐标，将下述宏打开
***********************************************************************************/
//#define SUPPORT_MULTI_COOR
  
static int GetCurselPreCoordinate(PSTR pStrSource,int textLength,int curOffset,int curlength,int *offset,int* nLength)
{
    unsigned int length = (unsigned int)textLength;
    PSTR pCurrent, pEnd,pstrTemp1,pstrTemp2;
    pCurrent = pStrSource + curOffset;
    pEnd = pStrSource + curOffset;
    while(strlen(pCurrent)<length)
    {
        if(*pCurrent != ')')//起始字符可以允许是')'
        {
            pCurrent--;
            pEnd--;
        }
        else
        {
            int isValidate = 0;
            pEnd--;
            if(*pEnd>0x39||*pEnd<0x030)//')'后面跟的不是数字，那么无效
            {
                pCurrent = pEnd;
                pEnd--;
                pCurrent--; 
            }
            else
            {
                int validate = 0;
                while(*pEnd!='('&&*pEnd!=' '&&pEnd >pStrSource)
                {
                    pEnd--;
                    if(*pEnd == ',')//需要保证'('与','之间只能出现数字
                    {
#ifndef SUPPORT_MULTI_COOR
                        if(validate == 0)
                            validate = 1;
                        else
                            break;//进入此分支表明出现了一个以上的','
#else
                        validate = 1;
#endif
                        
                        pstrTemp1 = pEnd+1;//
                        while(pstrTemp1 != pCurrent)
                        {
                            if(*pstrTemp1>0x39||*pstrTemp1<0x030)
                            {
                                isValidate--;
                                break;
                            }
                            pstrTemp1++;
                        }
                        isValidate++;	    
                        
                        if(isValidate != 1)
                            break;//已经可以得出无效
                        
                        pstrTemp1 = pEnd - 1;
                    }
                    
                    if(*pEnd == '('&&validate == 1)
                    {
                        pstrTemp2 = pEnd + 1;
                        while(pstrTemp2 != pstrTemp1)
                        {
                            if(*pstrTemp2>0x39||*pstrTemp2<0x030)
                            {
                                isValidate--;
                                break;
                            }
                            pstrTemp2++;
                        }
                        isValidate++;//只有此时等于2才有效
                        
                        if(isValidate == 2)
                        {
                            *offset = pEnd - pStrSource;
                            if(*offset<0)
                                *offset = 0;//确保不会出现错误
                            *nLength =pCurrent - pEnd +1;
                            if(*nLength<0)
                                *nLength = -*nLength;
                            return 1;
                        }
                        else
                            break;
                    }
                }
                
                pCurrent = pEnd;
                pEnd--;
                pCurrent--; 
            }	
            
        }
    }
    return -1;
}	   
/***********************************************************************************
函数名：
GetCurselCoordinate
函数功能：
查找用户给定字符串中当前offset附近的符合条件的Coordinate，并得到该Coordinate的offset以及Coordinate的长度。
参数含义：
pStrSource--源字符串
textLength--字符串长度
int curOffset 当前选中url的offset
int curlength  当前选中url的长度
int bForward      向前还是向后
int *offset   查找到到的offset
int* nLength   查找到的url的offset
返回值：
成功找到用户想查找的Coordinate，则返回1
否则返回－1
***********************************************************************************/
static int GetCurselCoordinate(PSTR pStrSource,int textLength,int curOffset,int curlength,int *offset,int* nLength,int bForward)
{
    if(bForward == 0)
		return GetCurselNextCoordinate(pStrSource,textLength,curOffset,curlength,offset,nLength);
	else
		return GetCurselPreCoordinate(pStrSource,textLength,curOffset,curlength,offset,nLength);
}


/*
*	获得串中出现连续数字的个数
*/
static BOOL IsNotValidate(char ch) 
{
	return (((ch < 0x30) || (ch> 0x39)) && (ch!='*'&& ch!='#'&&ch!='+'));
}
/***********************************************************************************
函数名：
GetCurselNextNumber
函数功能：
查找用户给定字符串中当前offset之后的符合条件的number，并得到该Number的offset以及number的长度。
参数含义：
pStrSource--源字符串
textLength--字符串长度
int curOffset 当前选中Number的offset
int curlength  当前选中Number的长度
int *offset   查找到到的offset
int* nLength   查找到的Number的offset
返回值：
成功找到用户想查找的Number，则返回1
否则返回－1
***********************************************************************************/
static int GetCurselNextNumber(PSTR pStrSource,int textLength,int curOffset,int curlength,int *offset,int* nLength)
{
	int	j = 0;
	int length = textLength;
	PSTR pCurrent, pEnd;
	pCurrent = pStrSource + curOffset + curlength;
	pEnd = pStrSource + curOffset + curlength;
	while(*pCurrent!='\0')
	{
		if(IsNotValidate(*pCurrent))//起始字符可以允许是'*' '# '+'或者数字
		{
			pCurrent++;
			pEnd++;
		}
		else
		{
			//结束符或者中间则可以是'*' '# 'p' 'w' '+' 以及数字
			pEnd++;
			while((!IsNotValidate(*pEnd)||(*pEnd=='p')||(*pEnd == 'w'))&&*pEnd != '\0')
			{
				pEnd++;
			}
			
			//此时判断长度，如果长度满足要求
			if((pEnd-pCurrent)>=2&&(pEnd-pCurrent)<=41)
			{
				BOOL validate = FALSE;
				/*
				*  保证这个连续号码中有数字出现，否则视为无效(例如"*#+")
				*/
				for(j = 0; j < (pEnd - pCurrent); j++)
				{
					if( *( pCurrent + j )>=0x30 && *( pCurrent + j ) <= 0x39 )
					{
						validate = TRUE;
						break;
					}
				}
				if(validate)
				{
					validate = FALSE;
					*offset = pCurrent - pStrSource;
					if(*offset<0)
						*offset = 0;
					*nLength = pEnd - pCurrent;
					if(*nLength<0)
						*nLength = -*nLength;
					return 1;
				}
				else
				{
					pCurrent = pEnd;
					pCurrent++;
					pEnd++;
				}
			}
			else
			{
				pCurrent = pEnd;
				pCurrent++;
				pEnd++;
			}
		}
	}
	return -1;
}

/***********************************************************************************
函数名：
GetCurselPreNumber
函数功能：
查找用户给定字符串中当前offset之前的符合条件的Number，并得到该Number的offset以及Number的长度。
参数含义：
pStrSource--源字符串
textLength--字符串长度
int curOffset 当前选中Number的offset
int curlength  当前选中Number的长度
int *offset   查找到到的offset
int* nLength   查找到的Number的offset
返回值：
成功找到用户想查找的Number，则返回1
否则返回－1
***********************************************************************************/
static int GetCurselPreNumber(PSTR pStrSource,int textLength,int curOffset,int curlength,int *offset,int* nLength)
{
	int j = 0;
	unsigned int length = (unsigned int)textLength;
	PSTR pCurrent;
	PSTR pEnd;
	
	pCurrent = pStrSource + curOffset;
	pEnd = pStrSource + curOffset;
	while(strlen(pCurrent) < length)
	{
		//结束符或者中间则可以是'*' '# 'p' 'w' '+' 以及数字
		if(IsNotValidate(*pCurrent)&&(*pCurrent != 'p')&&(*pCurrent != 'w'))//起始字符可以允许是'*' '# '+'或者数字
		{
			pCurrent--;
			pEnd--;
		}
		else
		{
			pCurrent--;
			while(!IsNotValidate(*pCurrent)&& pCurrent >= pStrSource)
			{
				if(*pCurrent == 'p'||*pCurrent == 'w')//如果中间出现'p' 'w' 
				{
					//则如果之前还有有效字符说明是中间出现的继续移动向前
					if((pCurrent>pStrSource)&&!IsNotValidate(*(pCurrent-1)))
					{
						pCurrent--;
						continue;
					}
					else
						break;
				}
				pCurrent--;
			}
			//此时判断长度，如果长度满足要求
			if((pEnd-pCurrent)>=2&&(pEnd-pCurrent)<=41)
			{
				BOOL validate = FALSE;
				/*
				*  保证这个连续号码中有数字出现，否则视为无效(例如"*#+")
				*/
				for(j = 0; j < (pEnd - pCurrent); j++)
				{
					if( *( pCurrent + j )>=0x30 && *( pCurrent + j ) <= 0x39 )
					{
						validate = TRUE;
						break;
					}
				}
				if(validate)
				{
					validate = FALSE;
					*offset = pCurrent - pStrSource + 1;
					if(*offset<0)
						*offset = 0;//确保不会出现错误
					*nLength = pEnd - pCurrent;
					if(*nLength<0)
						*nLength = -*nLength;
					return 1;
				}
				else
				{
					pEnd = pCurrent;
					pCurrent--;
					pEnd--;
				}
			}
			else
			{
				pEnd = pCurrent;
				pCurrent--;
				pEnd--;
			}
		}
	}
	return -1;
}	   
/***********************************************************************************
函数名：
GetCurselNumber
函数功能：
查找用户给定字符串中当前offset附近的符合条件的Number，并得到该Number的offset以及Number的长度。
参数含义：
pStrSource--源字符串
int curOffset 当前选中url的offset
int curlength  当前选中url的长度
int bForward      向前还是向后
int *offset   查找到到的offset
int* nLength   查找到的url的offset
返回值：
成功找到用户想查找的Number，则返回1
否则返回－1
***********************************************************************************/
static int GetCurselNumber(PSTR pStrSource,int textLength,int curOffset,int curlength,int *offset,int* nLength,int bForward)
{
	if(bForward == 0)
		return GetCurselNextNumber(pStrSource,textLength,curOffset,curlength,offset,nLength);
	else
		return GetCurselPreNumber(pStrSource,textLength,curOffset,curlength,offset,nLength);
}

/***********************************************************************************
函数名：
GetCurselNextEmail
函数功能：
查找用户给定字符串中当前offset之后的符合条件的Email，并得到该Email的offset以及Email的长度。
参数含义：
pStrSource--源字符串
textLength--字符串长度
int curOffset 当前选中Email的offset
int curlength  当前选中Email的长度
int *offset   查找到到的offset
int* nLength   查找到的Email的offset
返回值：
成功找到用户想查找的Email，则返回1
否则返回－1
***********************************************************************************/
static int GetCurselNextEmail(PSTR pStrSource,int textLength,int curOffset,int curlength,int *offset,int* nLength)
{
	unsigned int length = (unsigned int)textLength;
	PSTR pCurrent, pEnd,pStart;
	//char* pEmail;
	
	
	pCurrent = pStrSource + curOffset + curlength;
	pEnd = pStrSource + curOffset + curlength;
	while(*pCurrent!='\0')
	{
		if((BYTE)*pCurrent!=0xFD)//先查找'@'符号
		{
			pCurrent++;
			pEnd++;
		}
		else//若找到'@'符号，那么pCurrent指针前移到' ' 处
		{
			int validate = 0;
			pStart = pCurrent;
			while(*pStart != ' ' && *pStart != '\r' && *pStart != '\n' && strlen(pStart) < length )
			{
				pStart--;//记录字串起始位置
			}
			if(*pStart == ' ' || *pStart == '\r' || *pStart == '\n')//只有当遇到空格时，指针才前移指向第一个字符，否则指针到达的时字符串的头，不需要在前移了
				pStart += 1;//指向第一个字符
			
			while(*pEnd!=' '&&*pEnd != '\0'&&*pEnd!='\r'&&*pEnd!='\n')
			{
				if(*pEnd == '.'&&(BYTE)*(pEnd-1)!=0xFD&&*(pEnd-1)!='.')//保证之间有字符,保证'.'后面有字符
				{
					if((BYTE)*(pEnd+1)!=0xFD&&*(pEnd+1)!='.'&&
						*(pEnd+1)!=' '&&*(pEnd+1) != '\0'&&
						*(pEnd+1)!='\r'&&*(pEnd+1)!='\n')
						validate = 1;
				}
				pEnd++;//一直移动到空格为止
			}
			
			pEnd -= 1;//指向最后一个字符
			
			//		pEmail = malloc(sizeof(char)*(pEnd - pStart+2));
			
			//	strncpy(pEmail,pStart,(pEnd-pStart+1));
			//	pEmail[pEnd - pStart + 1] = '\0';
			//static BOOL MAIL_CheckReceiverValid(char *szReceiver)
			
			if(validate)
			{
				*offset = pStart - pStrSource;
				if(*offset<0)
					*offset = 0;//确保不会出现错误
				*nLength = pEnd - pStart + 1;
				if(*nLength<0)
					*nLength = -*nLength;
				//  	free(pEmail);
				return 1;
			}
			else
			{
				//  	free(pEmail);
				//pEmail = NULL;
				pEnd = pCurrent;
				pCurrent++;
				pEnd++;
			}
		}
	}
	return -1;
}

/***********************************************************************************
函数名：
GetCurselPreEmail
函数功能：
查找用户给定字符串中当前offset之前的符合条件的Email，并得到该Email的offset以及Email的长度。
参数含义：
pStrSource--源字符串
textLength--字符串长度
int curOffset 当前选中Email的offset
int curlength  当前选中Email的长度
int *offset   查找到到的offset
int* nLength   查找到的Email的offset
返回值：
成功找到用户想查找的Email，则返回1
否则返回－1
***********************************************************************************/
static int GetCurselPreEmail(PSTR pStrSource,int textLength,int curOffset,int curlength,int *offset,int* nLength)
{
	unsigned int length = (unsigned int)textLength;
	PSTR pCurrent, pEnd,pStart;
	char* pEmail;
	
	
	pCurrent = pStrSource + curOffset;
	pEnd = pStrSource + curOffset;
	//由于指针是向前移动
	while( strlen(pCurrent) < length )//当pCurrent指针移动到整个字符串头，则结束循环
	{
		if((BYTE)*pCurrent!=0xFD)//先查找'@'符号
		{
			pCurrent--;
			pEnd--;
		}
		else//若找到'@'符号，那么pCurrent指针前移到' ' 处
		{
			int validate = 0;
			pStart = pCurrent;
			while(*pStart != ' '&& *pStart != '\r' && *pStart != '\n' && pCurrent > pStrSource)
			{
				pStart--;//记录字串起始位置
				if(pStart == pStrSource)
					break;
			}
			if(*pStart == ' ' || *pStart == '\r' || *pStart == '\n')//只有当遇到空格时，指针才前移指向第一个字符，否则指针到达的时字符串的头，不需要在前移了
				pStart += 1;//指向第一个字符
			
			while(*pEnd!=' '&&*pEnd != '\0'&&*pEnd!='\r'&&*pEnd!='\n')
			{
				if(*pEnd == '.'&&(BYTE)*(pEnd-1)!=0xFD&&*(pEnd-1)!='.')//保证之间有字符
				{
					if((BYTE)*(pEnd+1)!=0xFD&&*(pEnd+1)!='.'&&
						*(pEnd+1)!=' '&&*(pEnd+1) != '\0'&&
						*(pEnd+1)!='\r'&&*(pEnd+1)!='\n')
						validate = 1;
				}
				pEnd++;//一直移动到空格为止
			}
			
			pEnd -= 1;//指向最后一个字符
			
			//	pEmail = malloc(sizeof(char)*(pEnd - pStart+2));
			
			//	strncpy(pEmail,pStart,(pEnd-pStart+1));
			//	pEmail[pEnd - pStart + 1] = '\0';
			
			//static BOOL MAIL_CheckReceiverValid(char *szReceiver)
			
			if(validate)
			{
				*offset = pStart - pStrSource;
				if(*offset<0)
					*offset = 0;//确保不会出现错误
				*nLength = pEnd - pStart+1;
				if(*nLength<0)
					*nLength = -*nLength;
				//  	free(pEmail);
				pEmail = NULL;
				return 1;
			}
			else
			{
				//  	free(pEmail);
				pEnd = pCurrent;
				pCurrent--;
				pEnd--;
			}
		}
	}
	return -1;
}	   
/***********************************************************************************
函数名：
GetCurselEmail
函数功能：
查找用户给定字符串中当前offset附近的符合条件的Email，并得到该Email的offset以及Email的长度。
参数含义：
pStrSource--源字符串
textLength--字符串长度
int curOffset 当前选中url的offset
int curlength  当前选中url的长度
int bForward      向前还是向后
int *offset   查找到到的offset
int* nLength   查找到的url的offset
返回值：
成功找到用户想查找的Email，则返回1
否则返回－1
***********************************************************************************/
static int GetCurselEmail(PSTR pStrSource,int textLength,int curOffset,int curlength,int *offset,int* nLength,int bForward)
{
	if(bForward == 0)
		return GetCurselNextEmail(pStrSource,textLength,curOffset,curlength,offset,nLength);
	else
		return GetCurselPreEmail(pStrSource,textLength, curOffset,curlength,offset,nLength);
}


/***********************************************************************************
函数名：
GetCurselNextUrl
函数功能：
查找用户给定字符串中当前offset之后的符合条件的url，并得到该url的offset以及url的长度。
参数含义：
pStrSource--源字符串
textLength--字符串长度
int curOffset 当前选中url的offset
int curlength  当前选中url的长度
int *offset   查找到到的offset
int *nLength   查找到的url的offset
返回值：
成功找到用户想查找的URL，则返回1
否则返回－1
***********************************************************************************/
static int GetCurselNextUrl(PSTR pStrSource,int textLength,int curOffset,int curlength,int *offset,int* nLength)
{
	unsigned int length = (unsigned int)textLength;
	PSTR pCurrent, pEnd;
	PSTR pUrl;
	
	pCurrent = pStrSource+curOffset+curlength;
	pEnd = pStrSource+curOffset+curlength;
	
	if((unsigned int)(curOffset + curlength) >length)
		return -1;
	
	while(*pCurrent != '\0') 
	{
		if((*pCurrent == 'h'||*pCurrent == 'H')&&strlen(pCurrent)<8)
			return -1;
		else if((*pCurrent == 'w'||*pCurrent == 'W')&&strlen(pCurrent)<5)
			return -1;
//		if(*pCurrent == 'h'&&*(pCurrent+1) == 't'&&
//			*(pCurrent+2) == 't'&&*(pCurrent+3) == 'p'&&*(pCurrent+4) == ':'&&
//			*(pCurrent+5) == '/'&&*(pCurrent+6) == '/')
		if(strnicmp(pCurrent, "http://", 7) == 0)
		{
			if(*(pCurrent+7) != ' '&&*(pCurrent+7) != ','&&*(pCurrent+7) != '\r'&&
				*(pCurrent+7) != '\n')
			{
				int urlLength = 0;
				
				//mxlin add
				if(pEnd == pCurrent)
				{
					pEnd++;
				}
				//end

				while((*pEnd != ' ') && (*pEnd!='\0') &&(*(pEnd-1)!='\r')&&(*(pEnd-1)!='\n'))
				{
					pEnd++;
				}
				pEnd -= 1;
				//检查有效性，若有效，则计数加一
				pUrl = malloc(sizeof(char)*(pEnd-pCurrent+2));
				strncpy(pUrl,pCurrent,(pEnd-pCurrent+1));
				pUrl[pEnd - pCurrent + 1] = '\0';
				urlLength = GetValidUrlEx(pUrl,(pEnd-pCurrent+1));
				if(urlLength>0)
				{
					*nLength = urlLength;
					if(*nLength<0)
						*nLength = -*nLength;
					*offset = pCurrent - pStrSource;//获得url的偏移量
					if(*offset<0)
						*offset = 0;//确保不会出现错误
					free(pUrl);
					pUrl = NULL;
					return 1;
				}
				// 				else
				//				{
				//					pCurrent = pCurrent + 6;//跳过"http://
				//					pCurrent = pEnd;
				//				}
			}
			else
			{
				pCurrent++;
				pEnd++;
			}
		}
//		else if(*pCurrent == 'w'&&*(pCurrent+1) == 'w'&&
//			*(pCurrent+2) == 'w'&&*(pCurrent+3) == '.')
		else if(strnicmp(pCurrent, "www.", 4) == 0)
		{
			
			if(*(pCurrent+4) != ' '&&*(pCurrent+4) != ','&&*(pCurrent+4) != '\r'&&
				*(pCurrent+4) != '\n')
			{
				int urlLength = 0;
				
				while(*pEnd != ' '&& *pEnd!='\0' && *pEnd!='\r' && *pEnd!='\n')
				{
					pEnd++;
				}
                pEnd -= 1;
				
				//检查有效性，若有效，则计数加一
				pUrl = malloc(sizeof(char)*(pEnd-pCurrent+2));
				strncpy(pUrl,pCurrent,(pEnd-pCurrent+1));
				pUrl[pEnd - pCurrent + 1] = '\0';				
                urlLength = GetValidUrlEx(pUrl,(pEnd-pCurrent+1));
				if(urlLength>0)
				{
					*nLength = urlLength;
					if(*nLength<0)
						*nLength = -*nLength;
					*offset = pCurrent - pStrSource;//获得url的偏移量 
					if(*offset<0)
						*offset = 0;//确保不会出现错误
					free(pUrl);
					pUrl = NULL;
					return 1;
				}
				//				else
				//				{
				//					  pCurrent = pCurrent + 4;
				//					  pEnd = pCurrent;
				//				  }
			}
			else 
			{
				pCurrent++;
				pEnd++;
			}
			
		}
//        else if(*pCurrent == 'w'&&*(pCurrent+1) == 'a'&&
//			*(pCurrent+2) == 'p'&&*(pCurrent+3) == '.')
		else if(strnicmp(pCurrent, "wap.", 4) == 0)
		{
			
			if(*(pCurrent+4) != ' '&&*(pCurrent+4) != ','&&*(pCurrent+4) != '\r'&&
				*(pCurrent+4) != '\n')
			{
				int urlLength = 0;
				
				while(*pEnd != ' '&& *pEnd!='\0' && *pEnd!='\r' && *pEnd!='\n')
				{
					pEnd++;
				}
				pEnd -= 1;
				
				//检查有效性，若有效，则计数加一
				pUrl = malloc(sizeof(char)*(pEnd-pCurrent+2));
				strncpy(pUrl,pCurrent,(pEnd-pCurrent+1));
				pUrl[pEnd - pCurrent + 1] = '\0';				
                urlLength = GetValidUrlEx(pUrl,(pEnd-pCurrent+1));
				if(urlLength>0)
				{
					*nLength = urlLength;
					if(*nLength<0)
						*nLength = -*nLength;
					*offset = pCurrent - pStrSource;//获得url的偏移量 
					if(*offset<0)
						*offset = 0;//确保不会出现错误
					free(pUrl);
					pUrl = NULL;
					return 1;
				}
				//				else
				//				{
				//					  pCurrent = pCurrent + 4;
				//					  pEnd = pCurrent;
				//				  }
			}
			else 
			{
				pCurrent++;
				pEnd++;
			}
		}
		else 
		{
			pCurrent++;
			pEnd++;
		}
		
	}
	
	return -1;
}

/***********************************************************************************
函数名：
GetCurselPreUrl
函数功能：
查找用户给定字符串中当前offset之前的符合条件的url，并得到该url的offset以及url的长度。
参数含义：
pStrSource--源字符串
textLength--字符串长度
int curOffset 当前选中url的offset
int curlength  当前选中url的长度
int *offset   查找到到的offset
int* nLength   查找到的url的offset
返回值：
成功找到用户想查找的URL，则返回1
否则返回－1
***********************************************************************************/
static int GetCurselPreUrl(PSTR pStrSource,int textLength,int curOffset,int curlength,int *offset,int* nLength)
{
	unsigned int length = (unsigned int)textLength;
	PSTR pCurrent, pEnd;
	PSTR pUrl;
	
	pCurrent = pStrSource + curOffset;
	pEnd = pStrSource + curOffset;
	
    if(curOffset<5)
        return -1;
    
    while(strlen(pCurrent) < length) 
    {
        if(*pCurrent == '/'&&(pCurrent -pStrSource)<6)
            return -1;
        else if(*pCurrent == '.'&&(pCurrent - pStrSource)<3)
            return -1;
//        if(*pCurrent == '/'&&*(pCurrent-1) == '/'&&
//            *(pCurrent-2) == ':'&&*(pCurrent-3) == 'p'&&*(pCurrent-4) == 't'&&
//            *(pCurrent-5) == 't'&&*(pCurrent-6) == 'h')
		if(strnicmp(pCurrent-6, "http://", 7) == 0)
        {
            //此时将pEnd指针向后移直到遇到‘ ’ 结束符
            pCurrent = pCurrent - 6;
            pEnd++;
            
            if(*pEnd != ' '&&*pEnd != ','&&*pEnd != '\r'&&
                *pEnd != '\n')//若http://后面既是无效字符那么继续移动
            {
                int urlLength = 0;
                
                while(*pEnd != ' '&&*pEnd!='\0'&&*pEnd!='\r'&&*pEnd!='\n')
                {
                    pEnd++;
                }
                pEnd -= 1;
                
                //检查有效性，若有效，则计数加一
                pUrl = malloc(sizeof(char)*(pEnd-pCurrent+2));
                strncpy(pUrl,pCurrent,(pEnd-pCurrent+1));
                pUrl[pEnd - pCurrent + 1] = '\0';
                urlLength = GetValidUrlEx(pUrl,(pEnd-pCurrent+1));
                if(urlLength >0)
                {
                    *nLength = urlLength;
                    if(*nLength<0)
                        *nLength = -*nLength;
                    *offset = pCurrent - pStrSource;//获得url的偏移量 
                    if(*offset<0)
                        *offset = 0;//确保不会出现错误
                    free(pUrl);
                    pUrl = NULL;
                    return 1;
                }
            }
            else
            {
                pEnd = pCurrent;
                pCurrent -- ;
                pEnd--;
            }
        }
//        else if(*pCurrent == '.'&&*(pCurrent-1) == 'w'&&
//            *(pCurrent-2) == 'w'&&*(pCurrent-3) == 'w')
		else if(strnicmp(pCurrent-3, "www.", 4) == 0)
        {
            //此时将pEnd指针向后移直到遇到‘ ’ 结束符
            pCurrent = pCurrent - 3;
            pEnd++;
            if(*pEnd != ' '&&*pEnd != ','&&*pEnd != '\r'&&
                *pEnd != '\n')
            {
                int urlLength = 0;
                
                while(*pEnd != ' '&&*pEnd!='\0' &&*pEnd!='\r'&&*pEnd!='\n')
                {
                    pEnd++;
                }
                pEnd -= 1;
                
                //检查有效性，若有效，则计数加一
                pUrl = malloc(sizeof(char)*(pEnd-pCurrent+2));
                strncpy(pUrl,pCurrent,(pEnd-pCurrent));
                pUrl[pEnd - pCurrent + 1] = '\0';
                urlLength = GetValidUrlEx(pUrl,(pEnd-pCurrent+1));
                if(urlLength>0)
                {
                    *nLength = urlLength;
                    if(*nLength<0)
                        *nLength = -*nLength;
                    *offset = pCurrent - pStrSource;//获得url的偏移量 
                    if(*offset<0)
                        *offset = 0;//确保不会出现错误
                    free(pUrl);
                    pUrl = NULL;
                    return 1;
                }
            }
            else
            {
                pCurrent--;
                pEnd--;
            }
        }
//        else if(*pCurrent == '.'&&*(pCurrent-1) == 'p'&&
//            *(pCurrent-2) == 'a'&&*(pCurrent-3) == 'w')
		else if(strnicmp(pCurrent-3, "wap.", 4) == 0)
        {
            //此时将pEnd指针向后移直到遇到‘ ’ 结束符
            pCurrent = pCurrent - 3;
            pEnd++;
            if(*pEnd != ' '&&*pEnd != ','&&*pEnd != '\r'&&
                *pEnd != '\n')
            {
                int urlLength = 0;
                
                while(*pEnd != ' '&&*pEnd!='\0' &&*(pEnd-1)!='\r'&&*(pEnd-1)!='\n')
                {
                    pEnd++;
                }
                pEnd -= 1;
                
                //检查有效性，若有效，则计数加一
                pUrl = malloc(sizeof(char)*(pEnd-pCurrent+2));
                strncpy(pUrl,pCurrent,(pEnd-pCurrent));
                pUrl[pEnd - pCurrent + 1] = '\0';
                urlLength = GetValidUrlEx(pUrl,(pEnd-pCurrent+1));
                if(urlLength>0)
                {
                    *nLength = urlLength;
                    if(*nLength<0)
                        *nLength = -*nLength;
                    *offset = pCurrent - pStrSource;//获得url的偏移量 
                    if(*offset<0)
                        *offset = 0;//确保不会出现错误
                    free(pUrl);
                    pUrl = NULL;
                    return 1;
                }
            }
            else
            {
                pCurrent--;
                pEnd--;
            }
        }
        else 
        {
            pCurrent--;
            pEnd--;
        }
        
        }
        return -1;
}	   
/***********************************************************************************
函数名：
GetCurselURL
函数功能：
查找用户给定字符串中当前offset附近的符合条件的url，并得到该url的offset以及url的长度。
参数含义：
pStrSource--源字符串
textLength--字符串长度
int curOffset 当前选中url的offset
int curlength  当前选中url的长度
int bForward      向前还是向后
int *offset   查找到到的offset
int *nLength   查找到的url的offset
返回值：
成功找到用户想查找的URL，则返回1
否则返回－1
***********************************************************************************/
static int GetCurselUrl(PSTR pStrSource,int textLength,int curOffset,int curlength,int *offset,int* nLength,int bForward)
{
	if(bForward == 0)
		return GetCurselNextUrl(pStrSource,textLength,curOffset,curlength,offset,nLength);
	else
		return GetCurselPreUrl(pStrSource,textLength,curOffset,curlength,offset,nLength);
}

	
