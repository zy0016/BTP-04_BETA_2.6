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
��������
GetCursel
�������ܣ�
�����û������ַ����е�ǰoffset�����ķ���������String�����õ���url��offset�Լ�String�ĳ��ȡ�
�������壺
pStrSource     Դ�ַ���
textLength--�ַ�������
int curOffset  ��ǰѡ��String��offset
int curlength  ��ǰѡ��String�ĳ���
int *offset    ���ҵ�����offset
int* length    ���ҵ���String��offset(���������ʼλ�õ�)
int bType      ����Email(TVS_EMAIL)��Url(TVS_URL)��Number(TVS_NUMBER)��or TVS_COORDINATE
int bForward   ���ҷ��� 1---last; 0----next
����ֵ��
�ɹ��ҵ��û�����ҵ�URL���򷵻�1
���򷵻أ�1
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
��������
GetCurselNextCoordinate
�������ܣ�
�����û������ַ����е�ǰoffset֮��ķ���������Coordinate�����õ���Coordinate��offset�Լ�Coordinate�ĳ��ȡ�
�������壺
pStrSource--Դ�ַ���
textLength--�ַ�������
int curOffset ��ǰѡ��Coordinate��offset
int curlength  ��ǰѡ��Coordinate�ĳ���
int *offset   ���ҵ�����offset
int* nLength   ���ҵ���Coordinate��offset
����ֵ��
�ɹ��ҵ��û�����ҵ�Coordinate���򷵻�1
���򷵻أ�1
��ע��ֻ֧�ֲ�������(1,2) �����꣬�����Ҫ֧�ֶ�ά���꣬���������

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
        if(*pCurrent != '(')//��ʼ�ַ�����������'('
        {
            pCurrent++;
            pEnd++;
        }
        else
        {
            int isValidate = 0;
            pEnd++;
            if(*pEnd>0x39||*pEnd<0x030)//'('������Ĳ������֣���ô��Ч
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
                    
                    //��Ҫ��֤'('��','֮��ֻ�ܳ�������,��ֻ��֧����ά����
                    if(*pEnd == ',')
                    {
#ifndef SUPPORT_MULTI_COOR
                        if(validate == 0)
                            validate = 1;
                        else
                            break;//����˷�֧����������һ�����ϵ�','
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
                            break;//�Ѿ����Եó���Ч
                        
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
                        isValidate++;//ֻ�д�ʱ����2����Ч
                        
                        if(isValidate == 2)
                        {
                            *offset = pCurrent - pStrSource;
                            if(*offset<0)
                                *offset = 0;//ȷ��������ִ���
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
��������
GetCurselPreCoordinate
�������ܣ�
�����û������ַ����е�ǰoffset֮ǰ�ķ���������Coordinate�����õ���Coordinate��offset�Լ�Coordinate�ĳ��ȡ�
�������壺
pStrSource--Դ�ַ���
textLength--�ַ�������
int curOffset ��ǰѡ��Coordinate��offset
int curlength  ��ǰѡ��Coordinate�ĳ���
int *offset   ���ҵ�����offset
int* nLength   ���ҵ���Coordinate��offset
����ֵ��
�ɹ��ҵ��û�����ҵ�Coordinate���򷵻�1
���򷵻أ�1
��ע��ֻ֧�ֲ�������(1,2) �����꣬�����Ҫ֧�ֶ�ά���꣬���������
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
        if(*pCurrent != ')')//��ʼ�ַ�����������')'
        {
            pCurrent--;
            pEnd--;
        }
        else
        {
            int isValidate = 0;
            pEnd--;
            if(*pEnd>0x39||*pEnd<0x030)//')'������Ĳ������֣���ô��Ч
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
                    if(*pEnd == ',')//��Ҫ��֤'('��','֮��ֻ�ܳ�������
                    {
#ifndef SUPPORT_MULTI_COOR
                        if(validate == 0)
                            validate = 1;
                        else
                            break;//����˷�֧����������һ�����ϵ�','
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
                            break;//�Ѿ����Եó���Ч
                        
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
                        isValidate++;//ֻ�д�ʱ����2����Ч
                        
                        if(isValidate == 2)
                        {
                            *offset = pEnd - pStrSource;
                            if(*offset<0)
                                *offset = 0;//ȷ��������ִ���
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
��������
GetCurselCoordinate
�������ܣ�
�����û������ַ����е�ǰoffset�����ķ���������Coordinate�����õ���Coordinate��offset�Լ�Coordinate�ĳ��ȡ�
�������壺
pStrSource--Դ�ַ���
textLength--�ַ�������
int curOffset ��ǰѡ��url��offset
int curlength  ��ǰѡ��url�ĳ���
int bForward      ��ǰ�������
int *offset   ���ҵ�����offset
int* nLength   ���ҵ���url��offset
����ֵ��
�ɹ��ҵ��û�����ҵ�Coordinate���򷵻�1
���򷵻أ�1
***********************************************************************************/
static int GetCurselCoordinate(PSTR pStrSource,int textLength,int curOffset,int curlength,int *offset,int* nLength,int bForward)
{
    if(bForward == 0)
		return GetCurselNextCoordinate(pStrSource,textLength,curOffset,curlength,offset,nLength);
	else
		return GetCurselPreCoordinate(pStrSource,textLength,curOffset,curlength,offset,nLength);
}


/*
*	��ô��г����������ֵĸ���
*/
static BOOL IsNotValidate(char ch) 
{
	return (((ch < 0x30) || (ch> 0x39)) && (ch!='*'&& ch!='#'&&ch!='+'));
}
/***********************************************************************************
��������
GetCurselNextNumber
�������ܣ�
�����û������ַ����е�ǰoffset֮��ķ���������number�����õ���Number��offset�Լ�number�ĳ��ȡ�
�������壺
pStrSource--Դ�ַ���
textLength--�ַ�������
int curOffset ��ǰѡ��Number��offset
int curlength  ��ǰѡ��Number�ĳ���
int *offset   ���ҵ�����offset
int* nLength   ���ҵ���Number��offset
����ֵ��
�ɹ��ҵ��û�����ҵ�Number���򷵻�1
���򷵻أ�1
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
		if(IsNotValidate(*pCurrent))//��ʼ�ַ�����������'*' '# '+'��������
		{
			pCurrent++;
			pEnd++;
		}
		else
		{
			//�����������м��������'*' '# 'p' 'w' '+' �Լ�����
			pEnd++;
			while((!IsNotValidate(*pEnd)||(*pEnd=='p')||(*pEnd == 'w'))&&*pEnd != '\0')
			{
				pEnd++;
			}
			
			//��ʱ�жϳ��ȣ������������Ҫ��
			if((pEnd-pCurrent)>=2&&(pEnd-pCurrent)<=41)
			{
				BOOL validate = FALSE;
				/*
				*  ��֤������������������ֳ��֣�������Ϊ��Ч(����"*#+")
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
��������
GetCurselPreNumber
�������ܣ�
�����û������ַ����е�ǰoffset֮ǰ�ķ���������Number�����õ���Number��offset�Լ�Number�ĳ��ȡ�
�������壺
pStrSource--Դ�ַ���
textLength--�ַ�������
int curOffset ��ǰѡ��Number��offset
int curlength  ��ǰѡ��Number�ĳ���
int *offset   ���ҵ�����offset
int* nLength   ���ҵ���Number��offset
����ֵ��
�ɹ��ҵ��û�����ҵ�Number���򷵻�1
���򷵻أ�1
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
		//�����������м��������'*' '# 'p' 'w' '+' �Լ�����
		if(IsNotValidate(*pCurrent)&&(*pCurrent != 'p')&&(*pCurrent != 'w'))//��ʼ�ַ�����������'*' '# '+'��������
		{
			pCurrent--;
			pEnd--;
		}
		else
		{
			pCurrent--;
			while(!IsNotValidate(*pCurrent)&& pCurrent >= pStrSource)
			{
				if(*pCurrent == 'p'||*pCurrent == 'w')//����м����'p' 'w' 
				{
					//�����֮ǰ������Ч�ַ�˵�����м���ֵļ����ƶ���ǰ
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
			//��ʱ�жϳ��ȣ������������Ҫ��
			if((pEnd-pCurrent)>=2&&(pEnd-pCurrent)<=41)
			{
				BOOL validate = FALSE;
				/*
				*  ��֤������������������ֳ��֣�������Ϊ��Ч(����"*#+")
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
						*offset = 0;//ȷ��������ִ���
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
��������
GetCurselNumber
�������ܣ�
�����û������ַ����е�ǰoffset�����ķ���������Number�����õ���Number��offset�Լ�Number�ĳ��ȡ�
�������壺
pStrSource--Դ�ַ���
int curOffset ��ǰѡ��url��offset
int curlength  ��ǰѡ��url�ĳ���
int bForward      ��ǰ�������
int *offset   ���ҵ�����offset
int* nLength   ���ҵ���url��offset
����ֵ��
�ɹ��ҵ��û�����ҵ�Number���򷵻�1
���򷵻أ�1
***********************************************************************************/
static int GetCurselNumber(PSTR pStrSource,int textLength,int curOffset,int curlength,int *offset,int* nLength,int bForward)
{
	if(bForward == 0)
		return GetCurselNextNumber(pStrSource,textLength,curOffset,curlength,offset,nLength);
	else
		return GetCurselPreNumber(pStrSource,textLength,curOffset,curlength,offset,nLength);
}

/***********************************************************************************
��������
GetCurselNextEmail
�������ܣ�
�����û������ַ����е�ǰoffset֮��ķ���������Email�����õ���Email��offset�Լ�Email�ĳ��ȡ�
�������壺
pStrSource--Դ�ַ���
textLength--�ַ�������
int curOffset ��ǰѡ��Email��offset
int curlength  ��ǰѡ��Email�ĳ���
int *offset   ���ҵ�����offset
int* nLength   ���ҵ���Email��offset
����ֵ��
�ɹ��ҵ��û�����ҵ�Email���򷵻�1
���򷵻أ�1
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
		if((BYTE)*pCurrent!=0xFD)//�Ȳ���'@'����
		{
			pCurrent++;
			pEnd++;
		}
		else//���ҵ�'@'���ţ���ôpCurrentָ��ǰ�Ƶ�' ' ��
		{
			int validate = 0;
			pStart = pCurrent;
			while(*pStart != ' ' && *pStart != '\r' && *pStart != '\n' && strlen(pStart) < length )
			{
				pStart--;//��¼�ִ���ʼλ��
			}
			if(*pStart == ' ' || *pStart == '\r' || *pStart == '\n')//ֻ�е������ո�ʱ��ָ���ǰ��ָ���һ���ַ�������ָ�뵽���ʱ�ַ�����ͷ������Ҫ��ǰ����
				pStart += 1;//ָ���һ���ַ�
			
			while(*pEnd!=' '&&*pEnd != '\0'&&*pEnd!='\r'&&*pEnd!='\n')
			{
				if(*pEnd == '.'&&(BYTE)*(pEnd-1)!=0xFD&&*(pEnd-1)!='.')//��֤֮�����ַ�,��֤'.'�������ַ�
				{
					if((BYTE)*(pEnd+1)!=0xFD&&*(pEnd+1)!='.'&&
						*(pEnd+1)!=' '&&*(pEnd+1) != '\0'&&
						*(pEnd+1)!='\r'&&*(pEnd+1)!='\n')
						validate = 1;
				}
				pEnd++;//һֱ�ƶ����ո�Ϊֹ
			}
			
			pEnd -= 1;//ָ�����һ���ַ�
			
			//		pEmail = malloc(sizeof(char)*(pEnd - pStart+2));
			
			//	strncpy(pEmail,pStart,(pEnd-pStart+1));
			//	pEmail[pEnd - pStart + 1] = '\0';
			//static BOOL MAIL_CheckReceiverValid(char *szReceiver)
			
			if(validate)
			{
				*offset = pStart - pStrSource;
				if(*offset<0)
					*offset = 0;//ȷ��������ִ���
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
��������
GetCurselPreEmail
�������ܣ�
�����û������ַ����е�ǰoffset֮ǰ�ķ���������Email�����õ���Email��offset�Լ�Email�ĳ��ȡ�
�������壺
pStrSource--Դ�ַ���
textLength--�ַ�������
int curOffset ��ǰѡ��Email��offset
int curlength  ��ǰѡ��Email�ĳ���
int *offset   ���ҵ�����offset
int* nLength   ���ҵ���Email��offset
����ֵ��
�ɹ��ҵ��û�����ҵ�Email���򷵻�1
���򷵻أ�1
***********************************************************************************/
static int GetCurselPreEmail(PSTR pStrSource,int textLength,int curOffset,int curlength,int *offset,int* nLength)
{
	unsigned int length = (unsigned int)textLength;
	PSTR pCurrent, pEnd,pStart;
	char* pEmail;
	
	
	pCurrent = pStrSource + curOffset;
	pEnd = pStrSource + curOffset;
	//����ָ������ǰ�ƶ�
	while( strlen(pCurrent) < length )//��pCurrentָ���ƶ��������ַ���ͷ�������ѭ��
	{
		if((BYTE)*pCurrent!=0xFD)//�Ȳ���'@'����
		{
			pCurrent--;
			pEnd--;
		}
		else//���ҵ�'@'���ţ���ôpCurrentָ��ǰ�Ƶ�' ' ��
		{
			int validate = 0;
			pStart = pCurrent;
			while(*pStart != ' '&& *pStart != '\r' && *pStart != '\n' && pCurrent > pStrSource)
			{
				pStart--;//��¼�ִ���ʼλ��
				if(pStart == pStrSource)
					break;
			}
			if(*pStart == ' ' || *pStart == '\r' || *pStart == '\n')//ֻ�е������ո�ʱ��ָ���ǰ��ָ���һ���ַ�������ָ�뵽���ʱ�ַ�����ͷ������Ҫ��ǰ����
				pStart += 1;//ָ���һ���ַ�
			
			while(*pEnd!=' '&&*pEnd != '\0'&&*pEnd!='\r'&&*pEnd!='\n')
			{
				if(*pEnd == '.'&&(BYTE)*(pEnd-1)!=0xFD&&*(pEnd-1)!='.')//��֤֮�����ַ�
				{
					if((BYTE)*(pEnd+1)!=0xFD&&*(pEnd+1)!='.'&&
						*(pEnd+1)!=' '&&*(pEnd+1) != '\0'&&
						*(pEnd+1)!='\r'&&*(pEnd+1)!='\n')
						validate = 1;
				}
				pEnd++;//һֱ�ƶ����ո�Ϊֹ
			}
			
			pEnd -= 1;//ָ�����һ���ַ�
			
			//	pEmail = malloc(sizeof(char)*(pEnd - pStart+2));
			
			//	strncpy(pEmail,pStart,(pEnd-pStart+1));
			//	pEmail[pEnd - pStart + 1] = '\0';
			
			//static BOOL MAIL_CheckReceiverValid(char *szReceiver)
			
			if(validate)
			{
				*offset = pStart - pStrSource;
				if(*offset<0)
					*offset = 0;//ȷ��������ִ���
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
��������
GetCurselEmail
�������ܣ�
�����û������ַ����е�ǰoffset�����ķ���������Email�����õ���Email��offset�Լ�Email�ĳ��ȡ�
�������壺
pStrSource--Դ�ַ���
textLength--�ַ�������
int curOffset ��ǰѡ��url��offset
int curlength  ��ǰѡ��url�ĳ���
int bForward      ��ǰ�������
int *offset   ���ҵ�����offset
int* nLength   ���ҵ���url��offset
����ֵ��
�ɹ��ҵ��û�����ҵ�Email���򷵻�1
���򷵻أ�1
***********************************************************************************/
static int GetCurselEmail(PSTR pStrSource,int textLength,int curOffset,int curlength,int *offset,int* nLength,int bForward)
{
	if(bForward == 0)
		return GetCurselNextEmail(pStrSource,textLength,curOffset,curlength,offset,nLength);
	else
		return GetCurselPreEmail(pStrSource,textLength, curOffset,curlength,offset,nLength);
}


/***********************************************************************************
��������
GetCurselNextUrl
�������ܣ�
�����û������ַ����е�ǰoffset֮��ķ���������url�����õ���url��offset�Լ�url�ĳ��ȡ�
�������壺
pStrSource--Դ�ַ���
textLength--�ַ�������
int curOffset ��ǰѡ��url��offset
int curlength  ��ǰѡ��url�ĳ���
int *offset   ���ҵ�����offset
int *nLength   ���ҵ���url��offset
����ֵ��
�ɹ��ҵ��û�����ҵ�URL���򷵻�1
���򷵻أ�1
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
				//�����Ч�ԣ�����Ч���������һ
				pUrl = malloc(sizeof(char)*(pEnd-pCurrent+2));
				strncpy(pUrl,pCurrent,(pEnd-pCurrent+1));
				pUrl[pEnd - pCurrent + 1] = '\0';
				urlLength = GetValidUrlEx(pUrl,(pEnd-pCurrent+1));
				if(urlLength>0)
				{
					*nLength = urlLength;
					if(*nLength<0)
						*nLength = -*nLength;
					*offset = pCurrent - pStrSource;//���url��ƫ����
					if(*offset<0)
						*offset = 0;//ȷ��������ִ���
					free(pUrl);
					pUrl = NULL;
					return 1;
				}
				// 				else
				//				{
				//					pCurrent = pCurrent + 6;//����"http://
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
				
				//�����Ч�ԣ�����Ч���������һ
				pUrl = malloc(sizeof(char)*(pEnd-pCurrent+2));
				strncpy(pUrl,pCurrent,(pEnd-pCurrent+1));
				pUrl[pEnd - pCurrent + 1] = '\0';				
                urlLength = GetValidUrlEx(pUrl,(pEnd-pCurrent+1));
				if(urlLength>0)
				{
					*nLength = urlLength;
					if(*nLength<0)
						*nLength = -*nLength;
					*offset = pCurrent - pStrSource;//���url��ƫ���� 
					if(*offset<0)
						*offset = 0;//ȷ��������ִ���
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
				
				//�����Ч�ԣ�����Ч���������һ
				pUrl = malloc(sizeof(char)*(pEnd-pCurrent+2));
				strncpy(pUrl,pCurrent,(pEnd-pCurrent+1));
				pUrl[pEnd - pCurrent + 1] = '\0';				
                urlLength = GetValidUrlEx(pUrl,(pEnd-pCurrent+1));
				if(urlLength>0)
				{
					*nLength = urlLength;
					if(*nLength<0)
						*nLength = -*nLength;
					*offset = pCurrent - pStrSource;//���url��ƫ���� 
					if(*offset<0)
						*offset = 0;//ȷ��������ִ���
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
��������
GetCurselPreUrl
�������ܣ�
�����û������ַ����е�ǰoffset֮ǰ�ķ���������url�����õ���url��offset�Լ�url�ĳ��ȡ�
�������壺
pStrSource--Դ�ַ���
textLength--�ַ�������
int curOffset ��ǰѡ��url��offset
int curlength  ��ǰѡ��url�ĳ���
int *offset   ���ҵ�����offset
int* nLength   ���ҵ���url��offset
����ֵ��
�ɹ��ҵ��û�����ҵ�URL���򷵻�1
���򷵻أ�1
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
            //��ʱ��pEndָ�������ֱ�������� �� ������
            pCurrent = pCurrent - 6;
            pEnd++;
            
            if(*pEnd != ' '&&*pEnd != ','&&*pEnd != '\r'&&
                *pEnd != '\n')//��http://���������Ч�ַ���ô�����ƶ�
            {
                int urlLength = 0;
                
                while(*pEnd != ' '&&*pEnd!='\0'&&*pEnd!='\r'&&*pEnd!='\n')
                {
                    pEnd++;
                }
                pEnd -= 1;
                
                //�����Ч�ԣ�����Ч���������һ
                pUrl = malloc(sizeof(char)*(pEnd-pCurrent+2));
                strncpy(pUrl,pCurrent,(pEnd-pCurrent+1));
                pUrl[pEnd - pCurrent + 1] = '\0';
                urlLength = GetValidUrlEx(pUrl,(pEnd-pCurrent+1));
                if(urlLength >0)
                {
                    *nLength = urlLength;
                    if(*nLength<0)
                        *nLength = -*nLength;
                    *offset = pCurrent - pStrSource;//���url��ƫ���� 
                    if(*offset<0)
                        *offset = 0;//ȷ��������ִ���
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
            //��ʱ��pEndָ�������ֱ�������� �� ������
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
                
                //�����Ч�ԣ�����Ч���������һ
                pUrl = malloc(sizeof(char)*(pEnd-pCurrent+2));
                strncpy(pUrl,pCurrent,(pEnd-pCurrent));
                pUrl[pEnd - pCurrent + 1] = '\0';
                urlLength = GetValidUrlEx(pUrl,(pEnd-pCurrent+1));
                if(urlLength>0)
                {
                    *nLength = urlLength;
                    if(*nLength<0)
                        *nLength = -*nLength;
                    *offset = pCurrent - pStrSource;//���url��ƫ���� 
                    if(*offset<0)
                        *offset = 0;//ȷ��������ִ���
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
            //��ʱ��pEndָ�������ֱ�������� �� ������
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
                
                //�����Ч�ԣ�����Ч���������һ
                pUrl = malloc(sizeof(char)*(pEnd-pCurrent+2));
                strncpy(pUrl,pCurrent,(pEnd-pCurrent));
                pUrl[pEnd - pCurrent + 1] = '\0';
                urlLength = GetValidUrlEx(pUrl,(pEnd-pCurrent+1));
                if(urlLength>0)
                {
                    *nLength = urlLength;
                    if(*nLength<0)
                        *nLength = -*nLength;
                    *offset = pCurrent - pStrSource;//���url��ƫ���� 
                    if(*offset<0)
                        *offset = 0;//ȷ��������ִ���
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
��������
GetCurselURL
�������ܣ�
�����û������ַ����е�ǰoffset�����ķ���������url�����õ���url��offset�Լ�url�ĳ��ȡ�
�������壺
pStrSource--Դ�ַ���
textLength--�ַ�������
int curOffset ��ǰѡ��url��offset
int curlength  ��ǰѡ��url�ĳ���
int bForward      ��ǰ�������
int *offset   ���ҵ�����offset
int *nLength   ���ҵ���url��offset
����ֵ��
�ɹ��ҵ��û�����ҵ�URL���򷵻�1
���򷵻أ�1
***********************************************************************************/
static int GetCurselUrl(PSTR pStrSource,int textLength,int curOffset,int curlength,int *offset,int* nLength,int bForward)
{
	if(bForward == 0)
		return GetCurselNextUrl(pStrSource,textLength,curOffset,curlength,offset,nLength);
	else
		return GetCurselPreUrl(pStrSource,textLength,curOffset,curlength,offset,nLength);
}

	
