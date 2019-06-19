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


/*********************************************************************\
* Function	   RealizeCursor
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static int RealizeCursor(PDEV pDev, PDRVLOGCURSOR pLogCursor,
                           PPHYSCURSOR pPhysCursor)
{
    return 1;
}

/*********************************************************************\
* Function	   UnrealizeCursor
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static int UnrealizeCursor(PDEV pDev, PPHYSCURSOR pCursor)
{
    return 0;
}

/******************************************************************\
Function:   ShowCursor
Param:      
    pDev    ָ��������ʾ�豸��ָ�룻
    pCursor ָ���������ָ�룬Ϊ�ձ�ʾ�����ù�ꣻ
    mode    ��ʾ����״̬�� Ϊ0��ʾ���أ�Ϊ1��ʾ��ʾ��ꡣ
\******************************************************************/

static int ShowCursor(PDEV pDev, PPHYSCURSOR pCursor, int mode)
{
    return 0;
}

/*********************************************************************\
* Function	   SetCursorPos
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static int SetCursorPos(PDEV pDev, int x, int y)
{
    return 0;
}

/*********************************************************************\
* Function	   CheckCursor
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static int CheckCursor(PDEV pDev)
{
    return 0;
}

/*********************************************************************\
* Function	 _ExcludeCursor  
* Purpose    Hide the current cursor if the cursor area is intersected 
             with the specified rectangle(draw operation area).  
* Params	
    pRC     Pointer of the specified rectangle
* Return	 	   
* Remarks	Each drawing operation must call this function first.   
**********************************************************************/
static void _ExcludeCursor(PDEV pDev, PRECT pRC)
{
    RECT rc;
    /* memory device, return directly. */
    if (pDev->bUpdate < 0)
        return;

    /* In real environment, bUpdate == 0 means LCDC, == 1 means no LCDC */
#ifndef _EMULATE_
    if (pDev->bUpdate == 1)
#endif
    {
        rc = *pRC;
        NormalizeRect(&rc);
        MergeUpdate(pDev, rc.x1, rc.y1, rc.x2, rc.y2);
    }

    return;
}

/*********************************************************************\
* Function	   _UnexcludeCursor
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void _UnexcludeCursor(PDEV pDev)
{
#ifdef _EMULATE_
    if (pDev->bUpdate > 0)
        UpdateScreen(pDev);
#endif

    return;
}
