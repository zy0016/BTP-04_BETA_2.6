/***************************************************************************
*
*                      Pollex Mobile Platform
*
* Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
*                       All Rights Reserved 
*
* Module   : CaleDefine.h
*
* Purpose  : 
*
\**************************************************************************/

//#include "CaleHeader.h"

#define     _BENEFON_CALE_
#undef      _NOKIA_CALE_
/*
#ifndef _CALENDAR_SYSICONS_
#define _CALENDAR_SYSICONS_	// signal and power icon displayed
#endif
//*/
#undef _CALENDAR_SYSICONS_	// no system icons displayed

#define CALE_Print printf	//********

#ifdef  _CALENDAR_SYSICONS_
#define CALE_LEFTICON	SIGNALICON
#define CALE_RIGHTICON	POWERICON
#else
#define CALE_LEFTICON	NULL
#define CALE_RIGHTICON	NULL
#endif

//path
#define     CALE_FILE_PATH          "/mnt/flash/calendar/"

#define     CALE_MEETING_FILE       "CALMEETING.cal"        //save meeting date
#define     CALE_ANNIVERSARY_FILE   "CALANNIVER.cal"        //save anniversary date
#define     CALE_EVENT_FILE         "CALEVENT.cal"          //save event date
#define     CALE_TEMP_FILE          "CALTEMP.cal"           //when clean and callback space

#define     CALE_DELETE_FLAG        "CALdelete.cal"         //calendar delete flag
#define     CALE_CONFIG_FILE        "CALconfig.cal"         //config flag
#define     CALE_VCALE_SEND_FILE    "/mnt/flash/calendar/calendar.vcs"

//picture
//#define     CALE_PIC_CALENDAR       "/rom/pim/calendar/calendar.bmp"
#define     CALE_PIC_NEWENTRY       "/rom/message/unibox/mail_aff.bmp"
#define     CALE_PIC_MEETING        "/rom/pim/calendar/meeting.bmp"
#define     CALE_PIC_ANNI           "/rom/pim/calendar/anniversary.bmp"
#define     CALE_PIC_EVENT          "/rom/pim/calendar/memo.bmp"
#define     CALE_ICON_MEET          "/rom/pim/calendar/meeting.bmp"
#define     CALE_ICON_ANNI          "/rom/pim/calendar/anniversary.bmp"
#define     CALE_ICON_EVENT         "/rom/pim/calendar/memo.bmp"

#define     CALE_ICON_SELECT        "/rom/ime/radioon.bmp"//"/usr/local/lib/gui/resources/select.bmp"
#define     CALE_ICON_UNSELECT      "/rom/ime/radiooff.bmp"//"/usr/local/lib/gui/resources/unselect.bmp"

#define     CALE_FAIL		    	-1
#define     CALE_SUCCESS    		0

//message
#define     CALE_APP_CALC		    (WM_USER + 1101)
#define     CALE_APP_NEWC           (WM_USER + 1102)
#define     CALE_CALC_CHANGED       (WM_USER + 1103)
#define     CALE_CALC_SETCUR        (WM_USER + 1104)
#define     CALE_CALC_GETCUR        (WM_USER + 1105)
#define     CALE_CALC_GETLISTNO     (WM_USER + 1106)
#define     CALE_NEW_MEETING        (WM_USER + 1107)
#define     CALE_NEW_ANNI           (WM_USER + 1108)
#define     CALE_NEW_EVENT          (WM_USER + 1109)
#define     CALE_CONFIG_CHANGED     (WM_USER + 1110)
#define     CALE_ALARM_SYS          (WM_USER + 1111)
//#define     CALE_ALARM_TIMER        (WM_USER + 1112)
#define     CALE_ALARM_CHANGED      (WM_USER + 1113)
#define     CALE_NEWSUB_RETURN      (WM_USER + 1114)
#define     CALE_CONFIRM_MESSAGE    (WM_USER + 1115)

#define     CALE_ALARM_SNOOZE	    (WM_USER + 1116)
#define     CALE_ALARM_OFF		    (WM_USER + 1117)

//edit/button
#define     ID_EDIT_SUBJECT         (WM_USER + 1301)
#define     ID_EDIT_LOCATION        (WM_USER + 1302)
#define     ID_EDIT_STARTTIME       (WM_USER + 1303)
#define     ID_EDIT_ENDTIME         (WM_USER + 1304)
#define     ID_EDIT_STARTDATE       (WM_USER + 1305)
#define     ID_EDIT_ENDDATE         (WM_USER + 1306)
#define     ID_SPIN_ALARM           (WM_USER + 1307)
#define     ID_SPIN_REP             (WM_USER + 1308)
#define     ID_SPIN_SYNC            (WM_USER + 1309)
#define     ID_CALC_MONTH           (WM_USER + 1310)
#define     ID_CALC_WEEK            (WM_USER + 1311)
#define     ID_CALC_DAY             (WM_USER + 1312)
#define     ID_LISTBOX_DAY          (WM_USER + 1313)

#define     IDM_BUTTON_COMPLETE     (WM_USER + 1350)
#define     IDM_BUTTON_DELETE       (WM_USER + 1351)
#define     IDM_BUTTON_SEND         (WM_USER + 1352)
#define     IDM_BUTTON_QUIT         (WM_USER + 1353)
#define     IDM_BUTTON_OPEN         (WM_USER + 1354)
#define     IDM_BUTTON_EDIT         (WM_USER + 1355)
#define     IDM_BUTTON_MONTH        (WM_USER + 1356)
#define     IDM_BUTTON_WEEK         (WM_USER + 1357)
#define     IDM_BUTTON_DELMANY      (WM_USER + 1358)
#define     IDM_BUTTON_GOTO         (WM_USER + 1359)
#define     IDM_BUTTON_SETTING      (WM_USER + 1360)
#define		IDM_LISTBOX_ENTRY		(WM_USER + 1361)

#define     IDM_BUTTON_BYSMS        (WM_USER + 1370)
#define     IDM_BUTTON_BYMMS        (WM_USER + 1371)
#define     IDM_BUTTON_BYEMAIL      (WM_USER + 1372)
#define     IDM_BUTTON_BYBLUE       (WM_USER + 1373)

#define     IDM_BUTTON_DELSELECT    (WM_USER + 1374)
#define     IDM_BUTTON_DELDATE      (WM_USER + 1375)
#define     IDM_BUTTON_DELALL       (WM_USER + 1376)

#define     IDM_BUTTON_OK           (WM_USER + 1400)
#define     IDM_BUTTON_ALTER        (WM_USER + 1401)

#define		IDM_MEET_SETSEL			(WM_USER + 1500)
#define		IDM_IME_SETCURSOR		(WM_USER + 1509)

//window ID
#define     ID_CALE_WIN_MEET        (WM_USER + 1600)
#define     ID_CALE_WIN_ANNI        (WM_USER + 1601)
#define     ID_CALE_WIN_EVENT       (WM_USER + 1602)

#define     MAX_YEAR                2037//2050
#define     MIN_YEAR                2000//1980

#define		TIMERID_VK_DOWN			VK_DOWN
#define		TIMERID_VK_UP			VK_UP
#define		TIMER_KEY_LONG			300
#define		TIMER_KEY_REPEAT		100

//window area
#define     WND_X                   0               //main window area
#define     WND_Y                   15
#define     WND_WIDTH               176
#define     WND_HEIGHT              (220 - WND_Y)

#define     PARTWND_X               0               //display window area(edit\timeedit\spinbox)
#define     PARTWND_Y               0
#define     PARTWND_WIDTH           172
#define     PARTWND_HEIGHT          50

#define     SUBWND_X                1               //sub window area(month\week\day)
#define     SUBWND_Y                4
#define     SUBWND_WIDTH            174
#define     SUBWND_HEIGHT           145

//compart
#define     MAX_ROW                 7
#define     MAX_LINE                8
#define     MAX_ROW_WEEK            8               //week view row
#define     MAX_SCH                 42
#define     MAX_WEEK_ROW            24              //week sch row
#define     MAX_WEEK_LINE           7               //week sch line

//snooze
#define     MAX_SNOOZE              10
#define     SNOOZE_TIMEOUT          60

//draw date style
#define     CALE_SCHED_SYS          0x01
#define     CALE_SCHED_TODAY        0x02
#define     CALE_SCHED_MEET         0x04
#define     CALE_SCHED_ANNI         0x08
#define     CALE_SCHED_EVENT        0x10
#define     CALE_SCHED_OTHERMON     0x20

//month view item
#define     CALE_MONTH_DAY_WIDTH    18
#define     CALE_MONTH_DAY_HEIGHT   19
#define     CALE_MONTH_WEEK_WIDTH   (CALE_MONTH_DAY_WIDTH * 2)
#define     CALE_MONTH_WEEK_HEIGHT  CALE_MONHT_DAY_HEIGHT

//week view item
#define     CALE_WEEK_DATE_WIDTH    18
#define     CALE_WEEK_DATE_HEIGHT   16
#define     CALE_WEEK_TIME_WIDTH    34//CALE_WEEK_DATE_WIDTH * 2
#define     CALE_WEEK_TIME_HEIGHT   CALE_WEEK_DATE_HEIGHT

//color
#define     CALE_CAMBRIDGEBLUE      RGB(162, 176, 229)
#define     CALE_NAVYBLUE           RGB(56, 130, 204)
#define     CALE_RED                RGB(242, 16, 16)
#define     CALE_BLACK              RGB(43, 44, 45)
#define     CALE_WHITE              RGB(239, 241, 246)
#define     CALE_YELLOW             RGB(242, 178, 16)
#define     CALE_PURPLE             RGB(187, 193, 208)

//option
#define     ICON_OPTIONS                "Options"
#define     ICON_SELECT                 "Select"

#define     CALE_LIST_WIDTH         176

#define     GARY_FREE(p)                do { if(p) free(p); p=NULL;} while(0)
