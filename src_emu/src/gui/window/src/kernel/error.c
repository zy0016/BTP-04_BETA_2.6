/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : Implements functions for error no maintainance
 *            
\**************************************************************************/

#include "hpwin.h"

static int nErrorNo;

/*
**  Function : GetLastError
**  Purpose  :
**      Returns the last error code value.
*/
int WINAPI GetLastError(void)
{
    return nErrorNo;
}

/*
**  Function : SetLastError
**  Purpose  :
**      Set the last error code value.
*/
void WINAPI SetLastError(int errorno)
{
    nErrorNo = errorno;
}
