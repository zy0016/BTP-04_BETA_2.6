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
#include "hpwin.h"
#include "winpda.h"
#include "string.h"

static COLORSCHEME		ColScheme;
BOOL			WINAPI	SetColorScheme(PCOLORSCHEME pColScheme)
{
	memcpy(&ColScheme, pColScheme, sizeof(COLORSCHEME));
	return TRUE;
}
PCOLORSCHEME	WINAPI  GetColorScheme(void)
{
	return &ColScheme;
}
