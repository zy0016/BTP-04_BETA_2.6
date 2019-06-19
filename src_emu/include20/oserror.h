/**************************************************************************\
 *                      Hopen Software System
 *
 * Copyright (c) 1999-2000 by Hopen Software Engineering Co., Ltd. 
 *                       All Rights Reserved
 *
 * Model   :	HOPEN OS Kernel System
 *
 * Purpose :	Error Codes
 *  
 * Author  :	Hejia Li
 *
 *-------------------------------------------------------------------------
 *
 * $Archive:: /hopen2.0/kernel2.0/include/oserror.h                 $
 * $Workfile:: oserror.h                                            $
 * $Revision:: 6  $     $Date:: 03-02-24 3:54p                      $
 *
\**************************************************************************/

#define	E_NOERROR		0

#define E_INVALIDHANDLE 1
#define E_DISCARDED		2
#define	E_CLOSED		3
#define	E_BUSY			4
#define	E_NOSPACE		5

#define E_INVAL			11
#define E_TIMEOUT		12
#define E_MBOXFULL		13
#define E_QUEUEFULL		14
#define E_MEMORY		15
#define	E_DELETED		16
#define	E_HANDLE		17
#define	E_QUEUEEMPTY	18
#define	E_SIGNAL		19

#define	E_NAME			20
#define	E_DEADLOCK		21

#define E_RDONLY		101
#define E_INVALID		102
#define E_PATH			104
#define E_OPEN			105
#define E_ERROR			106
#define E_NOTFOUND		107
#define E_OUTRANGE		108
#define	E_SHARE			109

#define E_WOULDBLOCK	110
#define E_EXIST			111
