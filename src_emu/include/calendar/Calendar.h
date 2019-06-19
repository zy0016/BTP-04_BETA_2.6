/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Calendar.h
 *
 * Purpose  : 
 *
\**************************************************************************/
#ifndef __CALE_EXTERN_HEADER__
#define __CALE_EXTERN_HEADER__

#include "vcardvcal.h"

//datainfo length
#define     MAX_CALE_SUBJECT_LEN    30
#define     MAX_CALE_LOCATION_LEN   30
#define     MAX_CALE_OCCASION_LEN   30

//calendar type
typedef enum
{
    CALE_MEETING = 1,
    CALE_EVENT,
    CALE_ANNI,
}CALE_ENTRY;

//calendar meeting node
typedef struct tagCaleMeetingNode
{
    char    MeetSubject[MAX_CALE_SUBJECT_LEN + 1];      //meeting subject
    char    MeetLocation[MAX_CALE_LOCATION_LEN + 1];    //meeting location
    
    int     MeetStartYear;                              //meeting start time:year
    int     MeetStartMonth;                             //meeting start time:month
    int     MeetStartDay;                               //meeting start time:day
    int     MeetStartWeek;                              //meeting start time:week
    int     MeetStartHour;                              //meeting start time:hour
    int     MeetStartMinute;                            //meeting start time:minute
    
    int     MeetAlarmFlag;                              //meeting alarm(whether or not)
    int     MeetAlarmTimes;                             //meeting alarm(how many times)
    int     MeetRepeatFlag;                             //meeting repeat(whether or not)
    int     MeetRepeatStyle;                            //meeting repeat(style:day\week\month\year)
    BYTE    MeetRepeatWeek;                             //meeting repeat week(monday\tuesday\wednesday\thursday\friday\saturday\sunday)
    
    int     MeetEndYear;                                //meeting end time:year
    int     MeetEndMonth;                               //meeting end time:month
    int     MeetEndDay;                                 //meeting end time:day
    int     MeetEndHour;                                //meeting end time:hour
    int     MeetEndMinute;                              //meeting end time:minute
    
    int     MeetSync;                                   //meeting syncronization
    int     LocalFlag;                                  //delete(whether or not)
}CaleMeetingNode;

//calendar anniversary
typedef struct tagCaleAnniNode
{
    char    AnniOccasion[MAX_CALE_OCCASION_LEN + 1];
    int     AnniDisYear;                                //Event display date
    int     AnniDisMonth;
    int     AnniDisDay;
    int     AnniYear;                                   //anniversary time:year
    int     AnniMonth;                                  //anniversary time:month
    int     AnniDay;                                    //anniversary time:day
    int     AnniHour;                                   //anniversary time:hour
    int     AnniMinute;                                 //anniversary time:minute
    int     AnniAlarmFlag;                              //anniversary alarm(whether or not)
    int     AnniAlarmTimes;                             //anniversary alarm(how many times)
    int     AnniSync;                                   //anniversary syncronization
    int     LocalFlag;                                  //delete(whether or not)
}CaleAnniNode;

//calendar event
typedef struct tagCaleEventNode
{
    char    EventSubject[MAX_CALE_SUBJECT_LEN + 1];     //Event subject
    int     EventStartYear;                             //Event start time:year
    int     EventStartMonth;                            //Event start time:month
    int     EventStartDay;                              //Event start time:day
    int     EventStartHour;                             //Event start time:hour
    int     EventStartMinute;                           //Event start time:minute
    int     EventEndYear;                               //Event end time:year
    int     EventEndMonth;                              //Event end time:month
    int     EventEndDay;                                //Event end time:day
    int     EventEndHour;                               //Event end time:hour
    int     EventEndMinute;                             //Event end time:minute
    int     EventSync;                                  //Event syncronization
    int     LocalFlag;                                  //delete(whether or not)
}CaleEventNode;

int CALE_ExternAddApi(int CaleEntry, BYTE *pRecord);
BOOL CALE_ExternDelApi(int CaleEntry, int CaleID);
BOOL CALE_ExternModApi(int CaleEntry, int CaleID, BYTE *pRecord, int size);
BOOL CALE_SaveFromSms(vCal_Obj *pObj);
BOOL CALE_DecodeVcalToDisplay(vCal_Obj *pObj, VCARDVCAL **ppChain);
long CALE_StatFlashFolder();

#endif
