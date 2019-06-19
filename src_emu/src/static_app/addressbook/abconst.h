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
#ifndef _AB_CONST_H_
#define _AB_CONST_H_

#define AB_MSG_REFRESHLIST      WM_USER+100
#define WM_EXIT                 WM_USER+600
#define WM_NEWCONTACT           WM_USER+1
#define WM_EDITCONTACT          WM_USER+2
#define WM_DATACHANGE           WM_USER+3
#define WM_ADDFIELD             WM_USER+4
#define WM_REMOVEFIELD          WM_USER+5
#define WM_GROUPDELETED         WM_USER+6
#define WM_SIMDELETED           WM_USER+7
#define WM_SETTONE              WM_USER+8
#define WM_SETPIC               WM_USER+9
#define WM_DELETECONTACT        WM_USER+10
#define WM_DELETEALL            WM_USER+11
#define WM_MAKECALL             WM_USER+12
#define WM_WRITESMS             WM_USER+13
#define WM_WRITEMMS             WM_USER+14
#define WM_WRITEEMAIL           WM_USER+15
#define WM_DELSELECT			WM_USER+16
#define WM_SELECT				WM_USER+17
#define WM_QDIALASSIGN			WM_USER+18
#define WM_QDIALREMOVE			WM_USER+19
#define WM_SAVECONTACT			WM_USER+20
#define WM_SUREELETESEL         WM_USER+21
#define WM_SUREELETEALL         WM_USER+22
#define WM_COPYSELECT           WM_USER+23
#define WM_COPYALL              WM_USER+24
#define WM_SUREADDMEMBER        WM_USER+25
#define WM_DATACHANGECONTACT    WM_USER+26
#define WM_REMOVEMEMBER         WM_USER+27
#define WM_REMOVEALL            WM_USER+28
#define WM_REMOVESEL            WM_USER+29
#define WM_SUREREMOVESEL        WM_USER+30
#define WM_UPDATE               WM_USER+31
#define WM_SELECTFIELD          WM_USER+32
#define WM_SELECTCONTACT        WM_USER+33
#define WM_SAVE                 WM_USER+34
#define WM_CANCELCOPYING        WM_USER+35
#define WM_CANCELCOPYFROMSIM    WM_USER+36
#define WM_GETVMN               WM_USER+37
#define WM_REPLACE              WM_USER+38

#define IDC_OK                  100
#define IDC_CANCEL              200
#define MEMORY_100K             (1024 * 100)
#define MEMORY_BASE_SIZE        5
#define MEMORY_MAX_SIZE         10
#define MEMORY_LINK_SIZE        4
#define AB_MEMORY_SNAP          3

#define AB_FREE(p)              do { if(p) free(p); p=NULL;} while(0)      

#define ICON_WIDTH              22
#define ICON_HEIGHT             16

#define IDC_AB_MAIN             100
#define IDC_AB_SIMVIEW          200
#define IDC_AB_SIM              300
#define IDC_AB_VIEW             400

#define AB_BMP_ARROWRIGHT       "/rom/contact/right.bmp"
#define AB_BMP_ARROWLEFT        "/rom/contact/left.bmp"
#define AB_ICON_ARROWRIGHT		"/rom/public/arrow_right.ico"
#define AB_ICON_ARROWLEFT		"/rom/public/arrow_left.ico"
#define AB_BMP_SEARCH           "/rom/contact/search.bmp"
#define AB_BMP_NEWCONTACT       "/rom/contact/newcontact.bmp"
#define AB_BMP_CONTACT          "/rom/contact/contact.bmp"
#define AB_BMP_GROUPS           "/rom/contact/groups.bmp"
#define AB_BMP_QUICKDIALLING    "/rom/contact/quickdialling.bmp"
#define AB_BMP_SIMPHONEBOOK     "/rom/contact/simphonebook.bmp"
#define AB_BMP_NEWGROUP         "/rom/contact/newgroup.bmp"
#define AB_BMP_GROUPITEM        "/rom/contact/groupitem.bmp"
#define AB_BMP_ADDMEMBER        "/rom/contact/addmember.bmp"
#define AB_BMP_MEMBER           "/rom/contact/contact.bmp"
#define AB_BMP_NEWSIM           "/rom/contact/newsim.bmp"
#define AB_BMP_SIMITEM          "/rom/contact/simitem.bmp"
#define AB_BMP_SELECT           "/rom/contact/rb_select.bmp"
#define AB_BMP_NORMAL           "/rom/contact/rb_normal.bmp"
#define AB_BMP_CBSELECT         "/rom/contact/cb_select.bmp"
#define AB_BMP_CBNORMAL         "/rom/contact/cb_normal.bmp"

#define PATH_DIR_AB             "/mnt/flash/addressbook"
#define PATH_TEMP_FILE			"ab.vcf"
#define PATH_FILE_GROUP         "contractgroup.inf"
#define PATH_FILE_ID            "contractid.inf"
#define PATH_FILE_TEMP          "temp.tmp"
#define PATH_FILE_CONTACT       "contract.ab"
#define PATH_FILE_ABQDIAL		"quickdialing.inf"
#define PATH_FILE_GROUPID       "groupid.inf"
#define PATH_FILE_GROUPTONE     "grouptone.inf"

#define PATH_MAXLEN             256

#define AB_TEMPSPACE_MAX        100*1024

#define AB_DEL_MASK             0

#define TEXT_WIDTH              150
#define OMIT_SUFFIX             '.'
#define SUFFIX_REPEAT           3

#define ICON_OPTIONS        "Options"
#define ICON_SELECT         "Select"


#define ERR_RETRY               0x1fffffff


#define AB_QDIAL_MAXNUM         8

#define AB_VCARD_FILENAMELEN    15


#define PATH_DIR_AB_BT          "/mnt/flash/addressbook/"

#define AB_VCARD_PORTFIX        ".vcf"

#define UTF8_STRLEN     256

#define COMPARE_TEL_COUNT		6
#endif // _AB_CONST_H_
