/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Display Driver
 *
 * Purpose  : Implement some commmon interfaces that is 
 *            independence of display mode.
 *            
\**************************************************************************/

#include <osver.h>
#include <assert.h>
#include <display.h>
#include "dispcomn.h"

#include "lineblt.c"

#include "fillline.c"
/*
** implements the scanline algorithm:
** used in the polygon, ellipse arc
*/
#include "scanline.c"

/*
**  implements the rop functions :
**
*/

#include "ropfunc.c"
