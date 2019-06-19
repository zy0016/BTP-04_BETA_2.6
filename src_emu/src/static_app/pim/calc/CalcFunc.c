/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : calculator
 *
 * Purpose  : count function
 *            
\**************************************************************************/

#include "CalcFunc.h"

//extern char *gcvt(double number, int ndigit, char *buf);
extern char *ecvt(double number, int ndigits, int *decpt, int *sign);

/******************************************************************************
* Funcation: StrToOprand
*
* Purpose  : charater string change operated symbols
*
* Arguments: 
*            
******************************************************************************/
OPRAND StrToOprand(char* s)
{
	OPRAND ret;
	char* p;
	OPRAND act;
	BOOL bSign;

	bSign=FALSE;
	ret=0;
	p=s;
	if(*p=='-')
	{
		bSign=TRUE;
		p++;
	}

	while(*p)
	{
		if(*p=='.')
			break;
		ret*=10;
		ret+=(*p-'0');
		p++;
	}
	if(*p=='.')
	{
		act=10;
		p++;
		while(*p)
		{
			ret+=((OPRAND)(*p-'0'))/act;
			act*=10;
			p++;
		}
	}
	return (bSign?-ret:ret);
}

/******************************************************************************
* Funcation: StrToNumber
*
* Purpose  : charater string change number
*
* Arguments: 
*            
******************************************************************************/
OPRAND StrToNumber(char* s)
{
	return (StrToOprand(s));
}

/******************************************************************************
* Funcation: OprandToStr
*
* Purpose  : operated symbol change character string
*
* Arguments: 
*            
******************************************************************************/
void OprandToStr(OPRAND op, char* buf)
{
	char tmp[MAX_NUM * 2];

#ifdef _863BIRD_//PHOSPHOR
    {
        int decimal;
        int sign = 0;
        int i = 0;
        int j = 0;
        char *str;

        if(op == 0)
        {
            strcpy(buf, "0.");
            return;
        }

        str = ecvt(op, MAX_NUM - 1, &decimal, &sign);
        if(decimal <= 0)
            str = ecvt(op, (MAX_NUM - 1) + decimal - 1, &decimal, &sign);

        if(sign == 1)
        {
            tmp[i++] = '-';
        }

        if(decimal <= 0)
        {
            tmp[i++] = '0';
            tmp[i++] = '.';
            while(decimal < 0)
            {
                tmp[i++] = '0';
                decimal++;
            }
        }
        else 
        {
            while(j < decimal)
            {
                tmp[i++] = str[j++];
            }
            tmp[i++] = '.';
        }
        while(str[j] != '\0')
        {
            tmp[i++] = str[j++];
        }
        tmp[i] = '\0';

        i--;
        while(tmp[i] == '0')
        {
            tmp[i--] = '\0';
            if(tmp[i] == '.')
                break;
        }
    }
#endif

	if(op < 0)
	{
		buf[MAX_NUM+1]=0;
		buf[MAX_NUM]='.';
		strncpy(buf, tmp, MAX_NUM+1);
	}
	else
	{
		buf[MAX_NUM]=0;
		strncpy(buf, tmp, MAX_NUM);
	}
}

/******************************************************************************
* Funcation: NumberToStr
*
* Purpose  : number change character string
*
* Arguments: 
*            
******************************************************************************/
void NumberToStr(OPRAND op, char* buffer)
{
	OprandToStr(op, buffer);	
}

/******************************************************************************
* Funcation: InitMachine
*
* Purpose  : initilize calculator
*
* Arguments: 
*            
******************************************************************************/
void InitMachine(void)
{
	CM_mem=0;
	CM_mode=CALC_MODE_DEC;
	CMO_C();
}

/******************************************************************************
* Funcation: GetResult
*
* Purpose  : get counting result
*
* Arguments: 
*            
******************************************************************************/
BOOL Calc_GetResult(char* buf)
{
#ifdef _RESOURCE
static const char   *lpszOverFlow,
                    *lpszSqrt,
                    *lpszLog,
                    *lpszDiv0,
                    *lpszSqr,
                    *lpszInvalidation;
#endif

	if(CM_errorno!=CME_NOERROR)
	{
#ifndef _RESOURCE
		switch(CM_errorno)
		{
		case CME_OVERFLOW:
			strcpy(buf,IDS_ERR_OVERFLOW);
			break;
		case CME_DIV0:
			strcpy(buf,IDS_ERR_DIV0);
			break;
		}
#else
		lpszOverFlow=LoadStringPtr(hInstanceRes,IDS_ERR_OVERFLOW);
		lpszSqrt=LoadStringPtr(hInstanceRes,IDS_ERR_SQRT);
		lpszLog=LoadStringPtr(hInstanceRes,IDS_ERR_LOG);
		lpszDiv0=LoadStringPtr(hInstanceRes,IDS_ERR_DIV0);
		lpszSqr=LoadStringPtr(hInstanceRes,IDS_ERR_SQR);
		lpszInvalidation=LoadStringPtr(hInstanceRes,IDS_ERR_INVALIDATION);

        switch(CM_errorno)
		{
		case CME_OVERFLOW:
			strcpy(buf,lpszOverFlow);
			break;
		case CME_SQRT:
			strcpy(buf,lpszSqrt);
			break;
		case CME_LOG:
			strcpy(buf,lpszLog);
			break;
		case CME_DIV0:
			strcpy(buf,lpszDiv0);
			break;
		case CME_SQR:
			strcpy(buf,lpszSqr);
			break;
		case CME_INVALIDATION:
			strcpy(buf,lpszInvalidation);
			break;
		}
#endif
		return FALSE;
	}

	if(strlen(CM_buffer)==0)
		strcpy(buf,"0");// 0. or 0
	else
		strcpy(buf,CM_buffer);

	if(!strchr(buf,'.'))
		strcat(buf,".");

	return TRUE;
}

/******************************************************************************
* Funcation: Calc_GetMode
*
* Purpose  : 
*
* Arguments: 
*            
******************************************************************************/
int Calc_GetMode(void)
{
	return CM_mode;
}

/******************************************************************************
* Funcation: Calc_IsMem
*
* Purpose  : judge whether memory has data or not
*
* Arguments: 
*            
******************************************************************************/
BOOL Calc_IsMem(void)
{
	return (CM_mem!=0);
}

/******************************************************************************
* Funcation: Calc_Input
*
* Purpose  : deal user's operation
*
* Arguments: 
*            
******************************************************************************/
int Calc_Input(int op)
{
	if(op>=CALC_NUM0 && op<=CALC_NUM9)
	{
		if(CM_flag==3)
		{
			CM_backop=CM_op;
			CM_op=OP_NONE;
		}
		if(!CMO_NUM((char)('0'+op)))
			return -1;
	}
	if(op == CALC_DOT)
		CMO_DOT();
	if (op == CALC_MADD)
		CMO_MADD();
	if(op >= CALC_DIV && op <= CALC_MR)
	{
		if(CM_flag==4) 
			CM_op=OP_NONE;
		(*(funcs[op-CALC_OP_BASE]))();
	}
	if(op == CALC_EQUEL)
		CMO_EQU();
	return 0;
}

/******************************************************************************
* Funcation: CMO_C
*
* Purpose  : clear all
******************************************************************************/
void CMO_C(void)
{
	CM_op=OP_NONE;
	CM_backop=OP_NONE;
	CM_oprand1=0;
	CM_oprand2=0;
	CM_buffer[0]=0;
	CM_errorno=CME_NOERROR;
	CM_dotted=FALSE;
	CM_flag=0;
	CM_isexp=FALSE;
	CMO_emptystake();
}

/******************************************************************************
* Funcation: CMO_NUM
*
* Purpose  : deal inputting number
******************************************************************************/
BOOL CMO_NUM(char cTag)
{
	int len;

	if(CM_flag != 1)
	{
		CM_buffer[0]=0;
		CM_dotted=FALSE;
	}
	if(	CM_flag == 1 && MF_flag==1)//when press MR, then press num,
	{	                             //display num should be input num.
	
		CM_buffer[0]=0;
		CM_dotted=FALSE;
		MF_flag = 0;
	}
	len=strlen(CM_buffer);
	if(CM_flag==2 || CM_flag==5)			
		CM_isexp=FALSE;
	if(CM_isexp==FALSE)			
	{
		if(!CM_dotted && CM_buffer[0]!='-')			
		{
			if(len>=(MAX_NUM-1)) return FALSE;
		}
		else
		{
			if(len>=MAX_NUM) return FALSE;
		}
		if(len==0 && cTag=='0')
		{
			CM_flag=1;			
			return TRUE;
		}
		CM_buffer[len]=cTag;
		CM_buffer[len+1]=0;
		CM_flag=1;
	}
	return TRUE;
}

/******************************************************************************
* Funcation: CMO_DOT
*
* Purpose  : deal inputting point
******************************************************************************/
void CMO_DOT(void)
{	
	int len;

	if(CM_isexp==FALSE && CM_mode==CALC_MODE_DEC)		
	{	
		if(	CM_flag == 1 && MF_flag==1) //when press MR, then press dot operator,
		{	                             //display num should be zero.		
			CM_buffer[0]='0';
			CM_buffer[1]='.';
			CM_buffer[2]=0;
			CM_dotted=TRUE;
			MF_flag = 0;
			return;
		}												
		
		if(CM_flag==3)				
		{
			CM_backop=CM_op;
			CM_op=OP_NONE;
		}
		if(CM_flag!=1)
		{
			CM_buffer[0]=0;
			CM_dotted=FALSE;
		}
		if(CM_dotted)
			return;
		
		len=strlen(CM_buffer);
		if(len>=MAX_NUM) return;
		if(len==0)
		{
			CM_buffer[len++]='0';
			CM_dotted=TRUE;
		}
		CM_buffer[len]='.';
		CM_buffer[len+1]=0;
		CM_dotted=TRUE;
		CM_flag=1;
		
	}
}

/******************************************************************************
* Funcation: CMO_BACK
*
* Purpose  : deal CMO_BACK
******************************************************************************/
void CMO_BACK(void)
{
	int len;

	if(CM_flag!=1) return;
	len=strlen(CM_buffer);
	if(len<=0) return;

	len--;
	if(CM_buffer[len-1]=='-')
		CM_buffer[0]=0;	
	if((CM_buffer[len]=='.' && CM_dotted))
	{
		CM_dotted=FALSE;
		if(CM_buffer[0]=='0')
		{
		// 	ASSERT(len==1);
			len--;
		}
	}
	CM_buffer[len]=0;
}

/******************************************************************************
* Funcation: CMO_AS
*
* Purpose  : deal +/-   
******************************************************************************/
void CMO_AS(void)
{
	char* p;
	int len;

	if(CM_flag!=2)
	{
		len=strlen(CM_buffer);
		if(len==0) return;
		if(CM_flag==3 && CM_oprand1==0)
			return;
		if(CM_buffer[0]!='-')
		{
			p=CM_buffer+len;
			*(p+1)=0;
			while(p>CM_buffer)
			{
				*p=*(p-1);
				p--;
			}
			*p='-';
		}
		else
		{
			p=CM_buffer;
			while(*p)
			{
				*p=*(p+1);
				p++;
			}
		}
		CM_curoprand=CM_curoprand*(-1);

		if(1!=CM_flag && 3!=CM_flag)

		{
			CM_flag=1;
			CM_op=OP_NONE;
		}
	}
	else
	{
		CM_buffer[0]=0;
		CM_flag=1;
		CM_dotted=FALSE;
	}
}

/******************************************************************************
* Funcation: CMO_AC
*
* Purpose  : delete inputting data
******************************************************************************/
void CMO_AC(void)
{
	CM_errorno=CME_NOERROR;
	CM_dotted=FALSE;
	CM_buffer[0]=0;
//	CMO_NUM((char)('0'+CALC_NUM0));
}

/******************************************************************************
* Funcation: CMO_MADD
*
* Purpose  : deal inputting M+
******************************************************************************/
void CMO_MADD(void)
{
	if(CM_errorno==CME_NOERROR)
	{
		if(3==CM_flag)
		{
			CM_op=OP_NONE;			
		}
		if(CM_flag==1)
		{
			CM_curoprand=StrToNumber(CM_buffer);
			CM_mem+=CM_curoprand;
		}
		else
		{
			if(CM_flag!=2)		//**warn**//
				CM_mem+=CM_curoprand;
		}
		OPRAND_MAX=9999999999999.0;
		OPRAND_MIN=0.000000000001;
		if(fabs(CM_mem)>OPRAND_MAX || ((fabs(CM_mem)<OPRAND_MIN) && (CM_mem!=0)))
			CM_errorno=CME_OVERFLOW;
		MF_flag=1;   
	//	CM_flag = 6;	
	}
}

/******************************************************************************
* Funcation: CMO_MS
*
* Purpose  : deal inputting MS
******************************************************************************/
void CMO_MS(void)
{
	if(CM_errorno == CME_NOERROR)

	{
		if(3==CM_flag)
		{
			CM_op=OP_NONE;			
		}
		if(CM_flag==1)
		{
			CM_curoprand=StrToNumber(CM_buffer);
			CM_mem=CM_curoprand;
		}
		else
		{
			if(CM_flag!=2)		
				CM_mem=CM_curoprand;
			else
			{
				CM_mem=0;	
				CM_curoprand = 0;
				CM_buffer[0]=0;
			}
			
		}
		MF_flag=1;   
		CM_flag = 1;
	}
}

/******************************************************************************
* Funcation: CMO_MC
*
* Purpose  : deal inputting MC
******************************************************************************/
void CMO_MC(void)
{
	if(CM_errorno == CME_NOERROR)
	{
		if(CM_flag == 2)
		{		
			CM_curoprand = 0;
			CM_buffer[0]=0;
		}
		CM_mem=0;
		MF_flag=1;          //indicate that press MR and if next input is num or dot, this flag
		                     //will be used.
		CM_flag=1;
	
	}
	
}

/******************************************************************************
* Funcation: CMO_MR
*
* Purpose  : deal inputting MR
******************************************************************************/
void CMO_MR(void)
{
	if(CM_errorno==CME_NOERROR)
	{
		if(3==CM_flag)			
			CM_op = OP_NONE;
		CM_curoprand=CM_mem;
		NumberToStr(CM_mem,CM_buffer);
		MF_flag=1;          //indicate that press MR and if next input is num or dot, this flag
		                       //will be used.
		CM_flag=1;
	}
}


/******************************************************************************
* Funcation: CMO_DIV
*
* Purpose  : deal inputting /
******************************************************************************/
void CMO_DIV(void)
{
	if(CM_flag==3)
	{
		CM_oprand1=CM_curoprand;
		if(pfstake>0)
			g_fstake[pfstake-1]=CM_oprand1;
		CMO_instake(OP_DIV,1);
	}
	if(CM_flag==2)
	{
		CMO_outstake(1);
		CMO_instake(OP_DIV,1);
	}
	
	if (CM_flag==1 && postake==0)
		CMO_emptystake();
	CM_backop=OP_NONE;

	if(CM_flag==6)									
		CM_op=OP_NONE;
	if(CM_flag!=2 && CM_flag!=3)
        CM_calc(OP_DIV);
	CM_op=OP_DIV;
	CM_flag=2;	
	MF_flag=0;
}

/******************************************************************************
* Funcation: CMO_MUL
*
* Purpose  : deal inputting *
******************************************************************************/
void CMO_MUL(void)
{
	if(CM_flag==3)
	{
		CM_oprand1=CM_curoprand;
		if(pfstake>0)
			g_fstake[pfstake-1]=CM_oprand1;
		CMO_instake(OP_MUL,1);
	}
	if(CM_flag==2)
	{
		CMO_outstake(1);
		CMO_instake(OP_MUL,1);
	}
	if (CM_flag==1 && postake==0)
		CMO_emptystake();
	CM_backop=OP_NONE;

	if(CM_flag==6)								
		CM_op=OP_NONE;
	if(CM_flag!=2 && CM_flag!=3)
		CM_calc(OP_MUL);
	CM_op=OP_MUL;
	CM_flag=2;	
	MF_flag=0;
}

/******************************************************************************
* Funcation: CMO_SUB
*
* Purpose  : deal inputting -
******************************************************************************/
void CMO_SUB(void)
{
	if(CM_flag == 3)
	{
		CM_oprand1=CM_curoprand;
		if(pfstake>0)
			g_fstake[pfstake-1]=CM_oprand1;
		CMO_instake(OP_SUB,1);
	}
	if(CM_flag== 2)
	{
		CMO_outstake(1);
		CMO_instake(OP_SUB,1);
	}

	if (CM_flag==1 && postake==0)
		CMO_emptystake();
	CM_backop=OP_NONE;

	if(CM_flag==6)								
		CM_op=OP_NONE;
	if(CM_flag!=2 && CM_flag!=3)
		CM_calc(OP_SUB);
	CM_op=OP_SUB;
	CM_flag=2;	
	MF_flag = 0;
}

/******************************************************************************
* Funcation: CMO_ADD
*
* Purpose  : deal inputting +
******************************************************************************/
void CMO_ADD(void)
{
	if(CM_flag==3)
	{
		CM_oprand1=CM_curoprand;
		if(pfstake >0)
			g_fstake[pfstake-1]=CM_oprand1;
        CMO_instake(OP_ADD,1);
	}
	if(CM_flag==2)
	{
		CMO_outstake(1);
		CMO_instake(OP_ADD,1);
	}
	if(CM_flag==1 && postake==0)
		CMO_emptystake();
    CM_backop = OP_NONE;
	if(CM_flag==6)								
		CM_op=OP_NONE;
	if(CM_flag!=2 && CM_flag!=3)
        CM_calc(OP_ADD);
	CM_op=OP_ADD;
	CM_flag=2;	
	MF_flag=0;
}

/******************************************************************************
* Funcation: CMO_PER
*
* Purpose  : deal inputting %
******************************************************************************/
void CMO_PER(void)
{
	if(1==CM_flag)
		CM_oprand2=StrToNumber(CM_buffer);
	CM_oprand2=CM_oprand1*CM_oprand2/100;
	if(CM_oprand2>9999999999999.0)
		CM_errorno=CME_OVERFLOW;
	else
	{
		NumberToStr(CM_oprand2,CM_buffer);		
		CM_curoprand=CM_oprand2;
		CM_flag=5;
	}
}

/******************************************************************************
* Funcation: CMO_EQU
*
* Purpose  : deal inputting =
******************************************************************************/
void CMO_EQU(void)
{
	
	CM_calc(OP_EQU);
	CM_flag=3;
	CM_isexp=FALSE;
}

/******************************************************************************
* Funcation: CM_calc
*
* Purpose  : deal counting
******************************************************************************/
void CM_calc(int curop)
{
	switch(CM_flag)
	{
		case 0:	
			CM_oprand1=CM_oprand2=0;
			if(curop!=OP_EQU)
			{
				CMO_instake(CM_oprand1,0);
				CMO_instake(curop,1);
			}
			break;
		case 1:	
			CM_curoprand=StrToNumber(CM_buffer);
			CMO_instake(CM_curoprand,0);

			if(curop!=OP_EQU)				
			{
				if(postake==0)
				{
					CMO_instake(curop,1);
					CM_oprand1=CM_oprand2=CM_curoprand;
					break;
				}
 				while(postake>0)
				{
					if((curop==OP_MUL || curop==OP_DIV) && (CMO_staketop(1)==OP_ADD || CMO_staketop(1)==OP_SUB))
					{
						CM_oprand1=CM_curoprand;
						break;
					}

					CM_oprand2=CMO_outstake(0);
					CM_oprand1=CMO_outstake(0);
					CM_op=(int)CMO_outstake(1);
					switch(CM_op)
					{
					case OP_ADD:
						CM_oprand1+=CM_oprand2;
						break;
					case OP_SUB:
						CM_oprand1-=CM_oprand2;
						break;
					case OP_MUL:
						CM_oprand1*=CM_oprand2;
						break;
					case OP_DIV:
						if(CM_oprand2==0)
							CM_errorno=CME_DIV0;
						else
							CM_oprand1/=CM_oprand2;
						break;
					}
					CMO_instake(CM_oprand1,0);
				}
				CMO_instake(curop,1);
				break;
			}

		case 2:	
		case 4:	
		case 5:
		case 6:
			if(CM_backop==OP_NONE)
			{
				CM_oprand2=CM_curoprand;
				if(CM_flag==2)
				{
					while (postake>1)
						CMO_outstake(1);
					while (pfstake>1)
						CMO_outstake(0);
					CMO_instake(CMO_staketop(0),0);
				}
			}
			else
			{
				CMO_outstake(0);
				CM_op=CM_backop;
				CM_flag=3;
			}

		case 3:
			if(CM_op==OP_NONE)					
			{
				if(CM_flag==1)				
					CM_oprand1=CM_oprand2=StrToNumber(CM_buffer);
				else
					CM_oprand1=CM_oprand2=CM_curoprand;
			}
			else
			{
				if(CM_flag==3)
				{
					CMO_instake(CM_op,1);
					CMO_instake(CM_oprand2,0);
				}
				while(postake>0)
				{
					CM_oprand2=CMO_outstake(0);
					CM_oprand1=CMO_outstake(0);
					CM_op=(int)CMO_outstake(1);
					switch(CM_op)
					{
						case OP_ADD:
							CM_oprand1+=CM_oprand2;	
							break;
						case OP_SUB:
							CM_oprand1-=CM_oprand2;	
							break;
						case OP_MUL:
							CM_oprand1*=CM_oprand2;	
							break;
						case OP_DIV:
							if(CM_oprand2==0)
								CM_errorno=CME_DIV0;
							else
								CM_oprand1/=CM_oprand2;	
							break;
					}
					CMO_instake(CM_oprand1,0);
				}
			}
			break;
	}

	OPRAND_MAX=999999999999.0;
	OPRAND_MIN=0.00000000001;
	if(fabs(CM_oprand1)>OPRAND_MAX)
		CM_errorno=CME_OVERFLOW;
	if((fabs(CM_oprand1)<OPRAND_MIN) && CM_oprand1!=0)
		CM_errorno=CME_OVERFLOW;//	CM_oprand1=0;

	if(CM_errorno==CME_NOERROR)
		NumberToStr(CM_oprand1,CM_buffer);
	CM_curoprand=CM_oprand1;
}

/******************************************************************************
* Funcation: CMO_1DIVX
*
* Purpose  : get backward's number
******************************************************************************/
void CMO_1DIVX(void)
{
	if(CM_flag==1)
		CM_curoprand=StrToNumber(CM_buffer);
	if(CM_curoprand==0)
	{
		CM_errorno=CME_DIV0;
		return;
	}
	CM_curoprand=1/CM_curoprand;
	NumberToStr(CM_curoprand,CM_buffer);
	CM_flag=5;										
}

OPRAND CMO_outstake(int tag)
{
	OPRAND ret;
	if(tag==0)
	{
		if(pfstake==0)
			return 0;
		ret=g_fstake[--pfstake];
		g_fstake[pfstake]=0;
		return (ret);
	}
	else
	{
		if(postake==0)
			return 0;
		ret=g_ostake[--postake];
		g_ostake[postake]=0;
		return (ret);
	}
}

void CMO_instake(OPRAND op, int tag)
{
	if(tag==0)
		g_fstake[pfstake++]=op;
	else
		g_ostake[postake++]=op;
}

OPRAND CMO_staketop(int tag)
{
	if(tag==0)
	{
		if(pfstake==0)
			return 0;
		return g_fstake[pfstake-1];
	}
	else
	{
		if(postake==0)
			return 0;
		return g_ostake[postake-1];
	}
}

void CMO_emptystake(void)
{
	while(pfstake>0)
		CMO_outstake(0);
	while(postake>0)
		CMO_outstake(1);
}
