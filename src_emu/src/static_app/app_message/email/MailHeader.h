/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : MailHeader.h
 *
 * Purpose  : 
 *
\**************************************************************************/
#ifndef __EMAIL_HEADER__
#define __EMAIL_HEADER__

//#define MAIL_DEBUG
#define _MAIL_EMU_

#include "window.h"
#include "stdlib.h"
#include "stdio.h"
#include "unistd.h"
#include "hpdef.h"
#include "String.h"
#include "sys/types.h"
#include "sys/stat.h"
#include "sys/vfs.h"
#include "sys/ipc.h"
#include "fcntl.h"
#include "dirent.h"
#include "errno.h"
#include "pubapp.h"
#include "mullang.h"
#include "imesys.h"
#include "recipient.h"
#include "plxmail.h"
#include "pmalarm.h"
#include "msg822.h"
#include "dialmgr.h"
#include "time.h"
#include "setting.h"
#include "plx_pdaex.h"
#include "log.h"
#include "setup.h"
#include "sndmgr.h"
#include "plxmm.h"
#include "prioman.h"

#ifdef MAIL_DEBUG
#include "plxdebug.h"
#endif

#include "public/PreBrowHead.h"
#include "PreBrow.h"
#include "msgunibox.h"
#include "msgpublic.h"
#include "PhonebookExt.h"
#include "project/MmsUi.h"
#include "project/vcardvcal.h"

#include "MailDefine.h"
#include "MailGlobal.h"
#include "MailStruct.h"
#include "MailString.h"

#endif
