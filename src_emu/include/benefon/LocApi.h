/*******************************************************************
 *
 *    Copyright:		Benefon Oyj
 *
 *    DESCRIPTION:      Location API
 *
 *    AUTHOR:			KAo
 *					   
 *    HISTORY:         
 * 
 *	  Rev	1.0			Initial working revision.
 *
 *******************************************************************/

#ifndef _NEW_LOCAPI_H
#define _NEW_LOCAPI_H

#include "window.h"

#define WM_GPS_SERVER			0x5000
#define LOC_NAV_DATA_UPDATE		WM_GPS_SERVER + 1
#define LOC_NAV_PARA_UPDATE		WM_GPS_SERVER + 2
#define LOC_REFRESH_POSITION	WM_GPS_SERVER + 3
#define LOC_TIME_UPDATING		WM_GPS_SERVER + 4

#define LOC_OK							1
#define LOC_TIMEOUT						2
#define LOC_REFRESH_STARTED				3
#define LOC_GPS_RESET_REFRESH_STARTED	4
#define LOC_TIME_UPDATING_STARTED		5

#define SW_VERSION_SIZE	100

typedef enum
{// don't change the order!
	FIX_RATE_1S_CONSTANT,
	FIX_RATE_1S,
	FIX_RATE_3S,
	FIX_RATE_6S,
	FIX_RATE_10S,
	FIX_RATE_30S,
	FIX_RATE_1MIN,
	FIX_RATE_10MIN,
	FIX_RATE_30MIN,
	FIX_RATE_1H,
	FIX_RATE_2H,
	FIX_RATE_6H,
	FIX_RATE_12H,
	FIX_RATE_24H,
	FIX_RATE_OFF,
	NBR_OF_FIX_RATE
} FIX_RATE;

typedef enum
{
	FIX_RATE_WHILE_CHARGING_1S_CONSTANT,
	FIX_RATE_WHILE_CHARGING_CURRENT_RATE,
	FIX_RATE_WHILE_CHARGING_OFF,
	NBR_OF_FIX_RATE_WHILE_CHARGING
} FIX_RATE_WHILE_CHARGING;

typedef enum
{
	GPS_OPERATING_MODE_SENSITIVE,
	GPS_OPERATING_MODE_ACCURATE,
	GPS_OPERATING_MODE_CUSTOM,
	NBR_OF_GPS_OPERATING_MODE
} GPS_OPERATING_MODE;

typedef enum
{
	GPS_NMEA_OUTPUT_OFF,
	GPS_NMEA_OUTPUT_4800,
	GPS_NMEA_OUTPUT_9600,
	NBR_OF_GPS_NMEA_OUTPUT
} GPS_NMEA_OUTPUT;

typedef enum
{
	APM_OPTION_OFF,
	APM_OPTION_DUTY_CYCLE,
	APM_OPTION_TIME_BETWEEN_FIXES
} APM_OPTION;

typedef enum
{
	NAV_DATA_MODE_ALL,
	NAV_DATA_MODE_USER_OPERATING_MODE,
	NAV_DATA_MODE_FIX_ONLY
} NAV_DATA_MODE;

typedef enum
{
	POS_MODE_NO_SOLUTION = 0,
	POS_MODE_1SV_KALMAN_SOLUTION,
	POS_MODE_2SV_KALMAN_SOLUTION,
	POS_MODE_3SV_KALMAN_SOLUTION,
	POS_MODE_MANY_SV_KALMAN_SOLUTION,
	POS_MODE_2D_LEAST_SQUARES_SOLUTION,
	POS_MODE_3D_LEAST_SQUARES_SOLUTION,
	POS_MODE_DEAD_RECKONING_SOLUTION,
	POS_MODE_OTHER
} POS_MODE;

typedef enum
{
	CHANNEL_STATE_NO_DATA_ON_CHANNEL = 0,
	CHANNEL_STATE_SATELLITE_WITHOUT_SIGNAL_LOCK,	// The satellite location is known from almanac information
	CHANNEL_STATE_SATELLITE_WITH_SIGNAL_LOCK,		// The satellite is being tracked
	CHANNEL_STATE_EPHEMERIS_DATA_AVAILABLE,			// The satellite is being tracked and ephemeris data is available
	CHANNEL_STATE_USED_IN_FIX						// The satellite is being tracked and is being used in fix
} CHANNEL_STATE;

typedef enum
{
	TRACE_LOG_USE_PHONE_MEMORY,
	TRACE_LOG_CLEAR_AND_USE_PHONE_MEMORY,
	TRACE_LOG_USE_CARD_MEMORY,
	TRACE_LOG_CLEAR_AND_USE_CARD_MEMORY,
	TRACE_LOG_USE_NONE_MEMORY
} TRACE_LOG;

typedef struct tagCHDATA
{
	INT				chSVid;						/* 1 .. 32 */
	FLOAT			azimuth;					/* 0.0 .. 360.0 */
	FLOAT			elevation;					/* 0.0 .. 90.0 */
	CHANNEL_STATE	state;
	INT				CNo;						/* 0 .. 64 */
} CHDATA, *PCHDATA;

typedef struct tagNAVDATA
{
	INT		UTCYear;							/* 1980 .. 3000 */
	INT		UTCMonth;							/* 1 .. 12 */
	INT		UTCDay;								/* 1 .. 31 */
	INT		UTCHour;							/* 0 .. 23 */
	INT		UTCMinute;							/* 0 .. 59 */
	FLOAT	UTCSecond;							/* 0.000 .. 59.999 */

	FLOAT	latitude;							/* -90.0000000 .. 90.0000000 °, +=North */
	FLOAT	longitude;							/* -180.0000000 to 180.0000000 °, +=East */
	FLOAT	altitude;							/* -2000.00 to 10000.00 m */

	FLOAT	speedOverGround;					/* 0.00 .. 655.00 m/s */
	FLOAT	courseOverGround;					/* 0.00 .. 360.00 ° */

	FLOAT	estimatedHorizontalPositionError;	/* 0.0 .. 400.0 m */

	DWORD	distanceTraveledSinceReset;			/* 0 .. 4294967295 m */

	CHDATA	chData[ 12 ];							

	LONG	X_position;							/* +/- 10000000 m */
	LONG	Y_position;							/* +/- 10000000 m */
	LONG	Z_position;							/* +/- 10000000 m */

	POS_MODE	mode;							/* Position mode */
	FLOAT		HDOP;							/* 0.0 .. 50.0 */

} NAVDATA, *PNAVDATA;

typedef struct tagSWVERSION
{
	char	string[ SW_VERSION_SIZE ];
} SWVERSION, *PSWVERSION;

typedef struct tagNAVPARAMETERS
{
	INT		dopSelection;						// DOP Selection - 0=Auto PDOP/HDOP, 1=PDOP, 2=HDOP, 3=GDOP, 4=Don't Use
	INT		navElevationMask;					// Navigation Elevation Mask x 10. Range -20.0 to 90.0 °
	INT		navPowerMask;						// Navigation Power Mask. Range 20 to 50 dBHz
	INT		pushToFix;							// Push-to-Fix Enabled/Disabled. ON=1, OFF=0
	UINT	LPOnTime;							// LP On-time. Range 200 - 900 msec
	UINT	LPInterval;							// LP Interval in msec. 
	INT		LPPowerCycling;						// LP Power Cycling Enabled/Disabled. ON=1, OFF=0
	UINT	LPMaxAcqSearchTime;					// LP Max. Acq. Search Time in msec. 
	UINT	LPMaxOffTime;						// LP Max. Off Time in msec.
	BYTE	APM;								// APM Enabled/Power Duty Cycle. Bit 7:1=APM enabled, 0=disabled
												// Bits 0-4: Power duty cycle, range: 1-20, 1=5%, 2=10%,..
	BYTE	APMNumFixes;						// APM Number of Fixes
	BYTE	APMTimeBetweenFixes;				// APM Time Between Fixes
	INT		horizontalVerticalErrorMax;			// APM Horizontal/Vertical Error Max in meters 
	BYTE	APMPriority;						// APM Time/Accu & Time/Duty Cycle Priority
												// 0=no priority imposed, 1=time between fixes, 2=duty cycle priority
} NAVPARAMETERS, *PNAVPARAMETERS; // mainly for debug usage

typedef struct tagREFRESHPOSITION
{
	HWND	handle;
	INT		maxSearchTime;
	BOOL	giveNote;
} REFRESHPOSITION, *PREFRESHPOSITION;

typedef struct tagGPSPROFILE
{
	FIX_RATE				fixRate;
	FIX_RATE_WHILE_CHARGING	fixRateWhileCharging;
	APM_OPTION				APMOption;
	GPS_OPERATING_MODE		operatingMode;
	INT						HDOP;				/* 1 .. 50 */
	INT						powerMask;			/* 20 .. 50 */
	GPS_NMEA_OUTPUT			nmeaOutput;			/* 0 - off, 4800 bps, 9600 bps */
	HWND					handle;
} GPSPROFILE, *PGPSPROFILE;

typedef struct tagNAVDATAREG
{
	HWND					handle;
	NAV_DATA_MODE			mode;
} NAVDATAREG, *PNAVDATAREG;

typedef struct tagUTCTIME
{
	WORD	UTCYear;							/* 1980 .. 3000 */
	BYTE	UTCMonth;							/* 1 .. 12 */
	BYTE	UTCDay;								/* 1 .. 31 */
	BYTE	UTCHour;							/* 0 .. 23 */
	BYTE	UTCMinute;							/* 0 .. 59 */
	BYTE	UTCSecond;							/* 0 .. 59 */
} UTCTIME, *PUTCTIME;

typedef struct tagTRACEMODE
{
	TRACE_LOG		memory; 
	BYTE			nbrOfTracePoints;
} TRACEMODE, *PTRACEMODE;

typedef struct tagTRACEPOINT
{
	WORD			UTCYear;					/* 1980 .. 3000. Note! If MSBit = 1 - first tp after powering on */
	BYTE			UTCMonth;					/* 1 .. 12 */
	BYTE			UTCDay;						/* 1 .. 31 */
	BYTE			UTCHour;					/* 0 .. 23 */
	BYTE			UTCMinute;					/* 0 .. 59 */
	BYTE			UTCSecond;					/* 0 .. 59 */

	LONG			X_position;					/* +/- 10000000 m */
	LONG			Y_position;					/* +/- 10000000 m */
	LONG			Z_position;					/* +/- 10000000 m */

	FLOAT			latitude;					/* -90.0000000 .. 90.0000000 °, +=North */
	FLOAT			longitude;					/* -180.0000000 to 180.0000000 °, +=East */
	FLOAT			altitude;					/* -2000.00 to 10000.00 m */

	BYTE			HDOP;						/* hdop x 10 */

} TRACEPOINT, *PTRACEPOINT;

typedef struct tagTRACEPOINTSTATUS
{
	TRACE_LOG		traceCurrentMemMode;		/* TRACE_LOG_USE_PHONE_MEMORY/TRACE_LOG_USE_CARD_MEMORY/TRACE_LOG_USE_NONE_MEMORY */
	WORD			nbrOfTracePoints;
	BYTE			nbrOfAllocPhoneTracePoints;	/* kB */
	BYTE			nbrOfAllocCardTracePoints;	/* kB */
} TRACEPOINTSTATUS, *PTRACEPOINTSTATUS;

//public:
int LocCreateServer( void );
int LocRegister( HWND aHandle, NAV_DATA_MODE aMode );
int LocUnregister( HWND aHandle );
int LocGetNavData( PNAVDATA aNavDataPtr );
int LocGetSWVersion( PSWVERSION aSWVersionPtr );
int LocSetBGGPSProfile( FIX_RATE				aFixRate, 
					    FIX_RATE_WHILE_CHARGING aFixRateWhileCharging, 
						APM_OPTION				aAPMOption,
						GPS_OPERATING_MODE		aOperatingMode, 
						INT						aHDOP,
						INT						aPowerMask,
						HWND					aHandle,
						INT						*pProfileReferenceNumber );
int LocDeleteBGGPSProfile( INT aProfileReferenceNumber );
int LocSetUIGPSProfile( PGPSPROFILE aGPSProfilePtr );
int LocNavigationParametersRequest( HWND aHandle );
int LocGetNavigationParameters( PNAVPARAMETERS aNavParaPtr );
int LocRefreshPosition( HWND aHandle, INT aMaxSearchTimeInMin, BOOL aGiveNote );
int LocResetGPS( HWND aHandle, INT aMaxSearchTimeInMin, BOOL aGiveNote );
int LocUpdateUTCTimeAndDate( HWND aHandle, INT aMaxSearchTimeInMin );
int LocGetUTCTimeAndDate( PUTCTIME aUTCTimePtr );
int LocSetChargerStatus( BOOL aChargerStatus );
int LocSetTraceMode( TRACE_LOG aMemory, BYTE aNbrOfKiloTracePoints );
int LocGetTracePoint( WORD aNbr, PTRACEPOINT aTracePointPtr );
int LocGetTracePointStatus( PTRACEPOINTSTATUS aTracePointStatus );

#endif _NEW_LOCAPI_H

