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
//#include "hpimage.h"
#include "string.h"
#include "math.h"
#include "str_plx.h"
#include "mullang.h"

#define APP_LANGUAGE_CHINESE

#define CALC_NUM0		0					
#define CALC_NUM1		1
#define CALC_NUM2		2
#define CALC_NUM3		3
#define CALC_NUM4		4
#define CALC_NUM5		5
#define CALC_NUM6		6
#define CALC_NUM7		7
#define CALC_NUM8		8
#define CALC_NUM9		9
#define CALC_DIV		10
#define CALC_BK			11
#define CALC_ADD		12
#define CALC_MC			13
#define CALC_C			14
#define CALC_MINUS		15
#define CALC_MS			16
#define CALC_CE			17
#define CALC_MULTI		18
#define CALC_MR			19
#define CALC_DOT		20
#define CALC_MADD		21
#define CALC_EQUEL		22

#define OP_NONE				0				
#define OP_ADD				1
#define OP_SUB				2
#define OP_DIV				3
#define OP_MUL				4
#define OP_XPOWERY			5
#define OP_XSQRT			6
#define OP_EQU				7

#define CME_NOERROR			0			
#define CME_OVERFLOW		1
#define CME_DIV0			2
#define CME_SQRT			3
#define CME_LOG				4
#define CME_SQR				5
#define CME_INVALIDATION	6

#define CALC_MODE_DEC		1				
#define CALC_MODE_HEX		2
#define CALC_MODE_OCT		3
#define CALC_MODE_BIN		4

BOOL CMO_NUM(char cTag);				
void CMO_AC(void);
void CMO_C(void);
void CMO_MADD(void);
void CMO_MC(void);
void CMO_MR(void);
void CMO_MS(void);
void CMO_PER(void);
void CMO_BACK(void);
void CMO_DIV(void);
void CMO_MUL(void);
void CMO_SUB(void);
void CMO_DOT(void);
void CMO_EQU(void);
void CMO_ADD(void);
void CMO_AS(void);
void CMO_1DIVX(void);
void CM_calc(int curop);

#define	IDS_ERR_OVERFLOW		ML("E")
#define	IDS_ERR_DIV0			ML("E")

#define CALC_OP_BASE  CALC_DIV			

typedef void CMO_FUNC(void);			


typedef double OPRAND;					

OPRAND CMO_outstake(int tag);
void CMO_instake(OPRAND op, int tag);
OPRAND CMO_staketop(int tag);
void CMO_emptystake(void);

static CMO_FUNC* funcs[] =				
{
	CMO_DIV,
	CMO_BACK,
	CMO_ADD,
	CMO_MC,
	CMO_C,
	CMO_SUB,
	CMO_MS,
	CMO_AC,
	CMO_MUL,
	CMO_MR,
//	CMO_DOT,
//	CMO_MADD,
//	CMO_AS,
//	CMO_EQU,
};

#define MAX_NUM	13
static OPRAND CM_oprand1, CM_oprand2;	
static OPRAND CM_curoprand;				
static int CM_op;						
static int CM_backop;					
OPRAND CM_mem;					
static int CM_errorno;					
static char CM_buffer[MAX_NUM*3];	
static int CM_mode;						
static BOOL CM_dotted = FALSE;			
static OPRAND OPRAND_MAX;
static OPRAND OPRAND_MIN;
static BOOL CM_isexp = FALSE;			
static OPRAND g_fstake[3];
static OPRAND g_ostake[3];
static int pfstake = 0;
static int postake = 0;
static int MF_flag = 0; //if TRUE that indicate user input MC MS MR M+.
static int CM_flag;		//0 - 什么都没输 1 - 输入的是数字 2 - 输入的是操作(双操作数) 
						//3 - 输入的是=  4 - 输入的是HEX,OCT,BIN,DEC 
						//5 - MR 6 - 输入的是MC、M+、MS
						//输入的是操作(单操作数)、PI
