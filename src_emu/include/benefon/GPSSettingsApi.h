/***************************************************************************
*
*                      BTP-04
*
* Copyright (c) Benefon Oyj
*                       All Rights Reserved
*
* Model   : 
*
* Purpose : 
*
* Author  : KR
*
* Date	  : 20.06.2005
*
*-------------------------------------------------------------------------
*
* $Archive::                                                       $
* $Workfile::                                                      $
* $Revision::    $     $Date::                                     $
* 
***************************************************************************/

#ifndef _NEW_GPSSETTINGSAPI_H
#define _NEW_GPSSETTINGSAPI_H

#include "window.h"

#define NBR_OF_GPS_PROFILES		6
#define PROFILE_NAME_MAX_LEN	16

typedef enum
{
	GPS_OFF,
	GPS_PROFILE1,
	GPS_PROFILE2,
	GPS_PROFILE3,
	GPS_PROFILE4,
	GPS_PROFILE5,
	GPS_PROFILE6
} GPS_PROFILES;

GPSPROFILE dummy;
static GPSPROFILE gpsProfile[ NBR_OF_GPS_PROFILES + 1 ];
static int activeProfile;

typedef struct DB_GPS_PROFILE
{
	FIX_RATE				fixRate;
	FIX_RATE_WHILE_CHARGING	fixRateWhileCharging;
	GPS_OPERATING_MODE		operatingMode;
	GPS_NMEA_OUTPUT			nmeaOutput;
	char					name[ PROFILE_NAME_MAX_LEN+1 ];
};

typedef struct DB_GPS_PROFILES
{
	int activeProfile;
	struct DB_GPS_PROFILE dbGpsProfile[NBR_OF_GPS_PROFILES+1];
};
static struct DB_GPS_PROFILES dbGpsProfiles;

//GPS settings public:
BOOL ShowGPSSettingsView();

// GPS data base public:
void InitGPSMonitor( void );
BOOL GpsLastFixTime_Set( unsigned long time );
unsigned long GpsLastFixTime_Get( void );
int GetActiveProfile( void );
BOOL SetActiveProfile( int index );
struct DB_GPS_PROFILE GetDbProfile( int index );

#endif _NEW_GPSSETTINGSAPI_H

