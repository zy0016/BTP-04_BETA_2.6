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
#include "muglobal.h"

#define MEM_LEN                 (4 * 1024)
#define DATA_LEN                (MEM_LEN - 4)
#define BLOCK_LEN               sizeof(MU_CHAINNODE)
#define ARRAY_NUM               (DATA_LEN / BLOCK_LEN)
#define DATA_ACTUAL_LEN         (BLOCK_LEN * ARRAY_NUM)           

static BYTE* p_header;
static PMU_CHAINNODE p_start;
static int    n_tail;
/*********************************************************************\
* Function	   MM_Construct
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
void MM_Construct(void)
{
    p_header = NULL;
    p_start = NULL;
    n_tail = 0;
}
/*********************************************************************\
* Function	   MM_Destroy
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
void MM_Destroy(void)
{
    BYTE *p;
    DWORD tmp;

    p = p_header;
    
    while( p )
    {
        tmp = *(DWORD*)(p + DATA_LEN);
        
        free( p );

        p = (BYTE*)tmp;
    }

    p_header = NULL;
    p_start = NULL;
    n_tail = 0;
}
/*********************************************************************\
* Function	   MM_malloc
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
void* MM_malloc (void)
{
    DWORD tmp;
    void* ptmp;

	if( !p_header )
    {
        p_header = malloc(MEM_LEN);
        
        if(!p_header)
            return NULL;

        tmp = 0;
        *(DWORD*)(p_header + DATA_LEN) = tmp;
         
        p_start = (PMU_CHAINNODE)p_header;
        n_tail = 0;
        return (&p_start[n_tail]);
    }

    n_tail ++;
    
    //if( &p_start[n_tail] + sizeof(MU_CHAINNODE) >= ((char*)p_start + DATA_ACTUAL_LEN) )
	
	if(n_tail >= (MEM_LEN / sizeof(MU_CHAINNODE)))
    {
        ptmp = malloc(MEM_LEN);

        *(DWORD*)((char*)p_start + DATA_LEN) = (DWORD)ptmp;

        if(!ptmp)
            return NULL;

		*(DWORD*)((char*)ptmp + DATA_LEN) = 0;
		
        p_start = (PMU_CHAINNODE)ptmp;       
        n_tail = 0;
        return (&p_start[n_tail]);
    }
    
    return (&p_start[n_tail]);
}
/*********************************************************************\
* Function	   MM_free
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
void MM_free (void* memblock)
{
    return;
}
