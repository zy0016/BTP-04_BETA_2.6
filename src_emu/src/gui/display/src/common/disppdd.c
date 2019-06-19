/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Display Driver
 *
 * Purpose  : Implement the PDD_* function using old interface.
 *            
\**************************************************************************/

//#ifndef _EMULATE_
#if 1
#include <osver.h>
#include <assert.h>
#include <display.h>

struct OS_DisplayMode* OS_EnumDisplayMode(int index);
struct OS_DisplayMode * OS_GetDisplayMode(void);
void OS_SetDisplayMode(int mode);
void OS_UpdateScreen(int left, int top, int right, int bottom);

/*********************************************************************\
* Function	   PDD_EnumDisplayMode
* Purpose      Enum the display modes of the given device.
* Params	   
        dev:    (in)the display device index
        index:  (in)the display mode index
        pDisplayMode: (out)the display mode returned
* Return	 	   
        Success return TRUE and failed return FALSE;
* Remarks	  
        Realized by physical display driver. 
**********************************************************************/
BOOL PDD_EnumDisplayMode(int dev, int index, POS_DISPLAYMODE pDisplayMode)
{
    return TRUE;
}

/*********************************************************************\
* Function	   PDD_SetDisplayMode
* Purpose      Set the display mode of the given device.
* Params	   
        dev:    (in)the display device index
        index:  (in)the display mode index
* Return	 	   
        Success return TRUE and failed return FALSE;
* Remarks	   
        Realized by physical display driver. 
**********************************************************************/
BOOL PDD_SetDisplayMode(int dev, int index)
{
    return TRUE;
}

/*********************************************************************\
* Function	   PDD_GetDisplayMode
* Purpose      Get the current mode of the given device.
* Params	   
        dev:    (in)the display device index
        pDisplayMode: (out)the display mode returned
* Return	 	   
* Remarks	   
        Realized by physical display driver. 
**********************************************************************/
BOOL PDD_GetDisplayMode(int dev, POS_DISPLAYMODE pDisplayMode)
{
    POS_DISPLAYMODE p;

    ASSERT(pDisplayMode != NULL);

   p = OS_GetDisplayMode();
    if (p == NULL)
        return FALSE;

//    p->dsp_updatescreen = 1;
    *pDisplayMode = *p;
    return TRUE;
}

/*********************************************************************\
* Function	   PDD_UpdateScreen
* Purpose      Update the given screen.
* Params	   
        dev:    (in)the display device index
        left,top,right,bottom: (in)the update rect
* Return	 	   
* Remarks	   
        Realized by physical display driver. 
**********************************************************************/
void PDD_UpdateScreen(int dev, int left, int top, int right, int bottom)
{
    OS_UpdateScreen(left, top, right, bottom);
}

#endif //_EMULATE_
