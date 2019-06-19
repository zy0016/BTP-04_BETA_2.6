/***************************************************************************
*
*                      Pollex Mobile Platform
*
* Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
*                       All Rights Reserved 
*
* Module   : CaleGlobal.h
*
* Purpose  : 
*
\**************************************************************************/

static const char *nMonthEnString[12] = 
{
    "January", "February", "March", "April", "May", "June", 
        "July", "August", "September", "October", "November", "December" 
};

static const char *nWeekEnString[7] = {
    "Su", "Mo", "Tu", "We", "Th", "Fr", "Sa"
};

static const short Days[12] = {
    31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365
};

//view type
typedef enum
{
    CALE_VIEW_MONTH = 1,
    CALE_VIEW_WEEK,
    CALE_VIEW_DAY,
}CALE_VIEW;

//local flag
typedef enum
{
    CALE_UNLOCAL = 0,
    CALE_LOCAL,
}CALE_LOCAL_FLAG;

//repeat type
typedef enum
{
    CALE_REP_NONE = 0,
    CALE_REP_DAY,
    CALE_REP_WEEK,
    CALE_REP_TWOWEEK,
    CALE_REP_MONTH,
    CALE_REP_YEAR,
}CALE_REPEAT_TYPE;

//repeat flag
typedef enum
{
    CALE_UNREP = 0,
    CALE_REP,
}CALE_REP_FLAG;

//compare result
typedef enum
{
    Early = 0,
    Equality,
    Late,
}CALE_COMPARE_RESULT;

//Color Index Start
typedef enum
{
    MCSC_BACKGROUND = 0,
    MCSC_MONTHBK,
    MCSC_TEXT,
    MCSC_TITLEBK,
    MCSC_TITLETEXT,
    MCSC_HEADERBK,
    MCSC_HEADERTEXT,
    MCSC_TRAILINGTEXT,
    MCSC_SATTEXT,
    MCSC_SUNTEXT,
    MCSC_GRIDLINE,
}CALE_COLOR_START;

//week start on
typedef enum
{
    START_SUNDAY = 0,
    START_MONDAY,
    START_TUERSDAY,
    START_WEDNESDAY,
    START_THURSDAY,
    START_FRIDAY,
    START_SATURDAY,
}CALE_WEEK_START;

//default week view
typedef enum
{
    WEEK_VIEW_NUMBER = 0,
    WEEK_VIEW_SCOPE,
}CALE_WEEK_VIEW;

//delete flag
typedef enum
{
    CALE_DEL_DEL = 0,
    CALE_DEL_BEFORE,
    CALE_DEL_NOW,
    CALE_DEL_AFTER,
    CALE_DEL_SELECT,
    CALE_DEL_ALL,
}CALE_DEL_FLAG;

//alarm style
typedef enum
{
    CALE_NO_ALARM = 0,
    CALE_ON_TIME,
    CALE_BEFORE_5MIN,
    CALE_BEFORE_10MIN,
    CALE_BEFORE_15MIN,
    CALE_BEFORE_30MIN,
    CALE_BEFORE_45MIN,
    CALE_BEFORE_1HOUR,
    CALE_BEFORE_6HOUR,
    CALE_BEFORE_12HOUR,
    CALE_BEFORE_1DAY,
    CALE_BEFORE_1WEEK,
}CALE_ALARM_STYLE;

//snooze style
typedef enum
{
    CALE_SNOOZE_OFF = 0,
    CALE_SNOOZE_5MIN,
    CALE_SNOOZE_10MIN,
    CALE_SNOOZE_30MIN,
}CALE_SNOOZE_STYLE;

//judge result
typedef enum
{
    CALE_RETURN_SUCCESS = 0,
    CALE_ERROR_EDIT,
	CALE_ERROR_BADDATE,
    CALE_ERROR_STARTDATE,
    CALE_ERROR_STARTTIME,
    CALE_ERROR_ENDDATE,
    CALE_ERROR_ENDTIME,
//	CALE_ERROR_ALARM
}CALE_JUDGE_RESULT;
