/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 *    Copyright (c) 2005-2005 by Pollex Mobile Software Co., Ltd.
 *                       All Rights Reserved
 *
 * Module   :  wsctrlex.c
 *
 * Purpose  :  
 *
\**************************************************************************/

#include "pubapp.h"

BOOL SPINBOXEX_RegisterClass(void);
BOOL EDITEX_RegisterClass(void);
BOOL UNITEDIT_RegisterClass(void);
BOOL TIMEEDIT_RegisterClass(void);
BOOL WILDCARD_RegisterClass(void);
BOOL PICLIST_RegisterClass(void);
BOOL PICMULTILIST_RegisterClass(void);
BOOL PICLISTEX_RegisterClass(void);
BOOL PICMULTILISTEX_RegisterClass(void);
BOOL LEVIND_RegisterClass(void);

/**********************************************************************
 * Function     WSCTRLEX_Init
 * Purpose      
 * Params       void
 * Return       
 * Remarks      
 **********************************************************************/

BOOL WSCTRLEX_Init(void)
{
    if (!SPINBOXEX_RegisterClass())
        return FALSE;

    if (!EDITEX_RegisterClass())
        return FALSE;

    if (!UNITEDIT_RegisterClass())
        return FALSE;

    if (!TIMEEDIT_RegisterClass())
        return FALSE;

    if (!WILDCARD_RegisterClass())
        return FALSE;
    
	if (!PICLIST_RegisterClass())
		return FALSE;

	if (!PICMULTILIST_RegisterClass())
		return FALSE;

	if (!PICLISTEX_RegisterClass())
		return FALSE;

	if (!PICMULTILISTEX_RegisterClass())
		return FALSE;

	if (!LEVIND_RegisterClass())
		return	FALSE;

    return TRUE;
}

/**********************************************************************
 * Function     WSCTRLEX_Exit
 * Purpose      
 * Params       void
 * Return       
 * Remarks      
 **********************************************************************/

BOOL WSCTRLEX_Exit(void)
{
    UnregisterClass("SPINBOXEX", NULL);
    UnregisterClass("EDITEX", NULL);
    UnregisterClass("UNITEDIT", NULL);
    UnregisterClass("TIMEEDIT", NULL);
    UnregisterClass("WILDCARD", NULL);
	UnregisterClass("PICLIST", NULL);
	UnregisterClass("PICMULTILIST", NULL);
	UnregisterClass("PICLISTEX", NULL);
	UnregisterClass("PICMULTILISTEX", NULL);
	UnregisterClass("LEVIND",NULL);

    return TRUE;
}
