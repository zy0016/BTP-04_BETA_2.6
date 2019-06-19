/********************************************************************
  Copyright (C), 2004, IVT Corporation
  Module name:		sdk_ui.h
  Author:       
  Version:        
  Date: 
  Abstract:   
  Revision History:        

********************************************************************/

#ifndef _SDK_UI_H
#define _SDK_UI_H

#include "autoconf.h"
#include "sdk_def.h"
#include "sdk_init.h"
#include "sdk_loc.h"
#include "sdk_rmt.h"
#include "sdk_sec.h"
#include "sdk_svc.h"
#include "sdk_conn.h"
#include "sdk_shc.h"

#ifdef CONFIG_PHONE_TL
#include "sdk_ctp_tl.h"
#endif
#ifdef CONFIG_SPP
#include "sdk_spp.h"
#endif
#if (defined(CONFIG_HFP_AG) || defined(CONFIG_HEP_AG))
#include "sdk_hf_ag.h"
#endif
#ifdef CONFIG_FAX
#include "sdk_fax.h"
#endif
#ifdef CONFIG_DUN
#include "sdk_dun.h"
#endif
#ifdef CONFIG_OPP
#include "sdk_opp.h"
#endif
#ifdef CONFIG_FTP
#include "sdk_ftp.h"
#endif
#ifdef CONFIG_LAP
#include "sdk_lap.h"
#endif
#ifdef CONFIG_PAN
#include "sdk_pan.h"
#endif
#if (defined(CONFIG_HFP_HF) || defined(CONFIG_HEP_HS))
#include "sdk_hf_hf.h"
#endif

#include "sdk_wrap.h"
#include "sdk_hcrpp.h"
#include "sdk_av.h"


#endif
