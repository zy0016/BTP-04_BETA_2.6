#ifndef _POSITIONINGSETTING_H
#define _POSITIONINGSETTING_H

#define pClassName  "GPSsettingClass"
#define IDM_EDIT    100
#define IDM_ACTIVE  101
#define IDM_DEL     102
#define IDM_OPEN	103
#define IDC_BUTTON_NEW          2
#define IDC_BUTTON_QUIT         3//quit
#define IDC_BUTTON_EDIT         4//edit
#define IDC_MODELIST            5
#define IDC_BUTTON_OPEN           6
#define CALLMSG_RELOADLIST      WM_USER + 100
#define CALLBACK_DELACTPRO      WM_USER + 101
#define CALLBACK_DELPRO         WM_USER + 102

#define SETT_TIPWIN_WIDTH       165


void    GPS_FreeProfileSpace(void);

#endif //PositioningSetting.h
