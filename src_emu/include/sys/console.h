/**************************************************************************\
 *
 *                      Hopen Software System
 *
 * Copyright (c) 1999-2000 by Hopen Software Engineering Co., Ltd. 
 *                       All Rights Reserved
 *
 * Model   :
 *
 * Purpose :     
 *  
 * Author  :     WangXinshe
 *
 *-------------------------------------------------------------------------
 *
 * $Source: /cvs/hopencvs/src/include/sys/console.h,v $
 * $Name:  $
 *
 * $Revision: 1.2 $     $Date: 2004/12/13 06:38:42 $
 * 
\**************************************************************************/

#ifndef _SYS_CONSOLE_H_
#define	_SYS_CONSOLE_H_

/*****************************************************************************
 *
 *    Namely as the console, as debugging the device which controls the port :
 *
 *    This kind of device as console at ordinary times , when there are 
 *    procedures to need debugging, it inputs and transfers to and debugs the 
 *    control procedure:
 *
 *    Every stature device of this kind of device has three pieces of virtual
 *    sub device :
 *
 *    virtual sub device 0  ----- virtual sub device NO. is euqal to actual sub
 *    device NO.;
 *    virtual sub device 1  ----- virtual sub device NO. is euqal to actual sub
 *    device NO. added by 0x1000;
 *
 *    Movements are the same as ordinary TTY device in 0 activities of virtual 
 *    sub device that if only, this equipment one. 
 *    If virtual sub device 1 activity, control virtual sub device 1 procedure 
 *    can is it require this device have the following movements to operate 
 *    through ioctl:
 *
 *        1.All input are belong to the virtual sub device 1.
 *        2.All input are belong to the virtual sub device 0.But control characters
 *          and following all input are belong to the virtual sub device 1 while 
 *          meeting the special character of controlling.  
 *        3.All output of virtual sub device 1 is regarded as input of virtual sub 
 *          device 0, and send to program of waiting for input data of console.
 *
 *
 *    When console device receive character of controlling,it send a SIGSTOP signal 
 *    to the device.
 *    Using this device without the process, sends SIGSTOP signal to process 0.
 *
 *
 *****************************************************************************
 */
/* special IO ctrl to interrupt of console */
/* Transform the goal of the data into the virtual sub device 0, set up the 
*  special character of touching off at the same time , after the device is received
*  to this special character , transform the goal of the data into the virtual sub 
*  device 1, send a SIGBREAK signal to this thread at the same time . */


#define	CON_SETSYNCCHAR			0x0010

/* Check whether there are characters of input. */
#define	CON_CHECKINPUT			0x0020	

#endif /* _CONSOLE_H_ */
