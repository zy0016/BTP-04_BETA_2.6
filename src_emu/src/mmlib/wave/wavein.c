/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : wavein.c
 *
 * Purpose  : Implement interface for program designer
 *
\**************************************************************************/

#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

#include "utils.h"
#include "hopen/soundcard.h"
#include "plxmm.h"

/*********************************************************************
* Function	waveInSetChannel
* Purpose   
* Parameter	
* Return	MMRESULT
* Remarks	
**********************************************************************/
WINMMAPI MMRESULT WINAPI waveInSetChannel(HWAVEIN hwi, BOOL bSet, DWORD nflags)
{
	return MMSYSERR_NOERROR;
}
