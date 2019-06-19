/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : StkAT.h
 *
 * Purpose  : 
 *
\**************************************************************************/

#ifndef _STK_AT_H_
#define _STK_AT_H_

// Macros of Menu Struct
#define STK_MAC_MENUCOUNT				20
#define STK_MAC_MENUCOMMAND				100
#define STK_MAC_STRINGCOMMAND			512

// Length of Search String
#define STK_LEN_INTEGER					10
#define STK_LEN_STRING					512

// Proactive Commands
#define STK_PAC_REFRESH					1
#define STK_PAC_SETUPEVENTLIST			5
#define STK_PAC_SETUPCALL				16
#define STK_PAC_SENDSS					17
#define STK_PAC_SENDUSSD				18
#define STK_PAC_SENDSHORTMESSAGE		19
#define STK_PAC_SENDDTMF				20
#define STK_PAC_LAUNCHBROWSER			21
#define STK_PAC_PLAYTONE				32
#define STK_PAC_DISPLAYTEXT				33
#define STK_PAC_GETINKEY				34
#define STK_PAC_GETINPUT				35
#define STK_PAC_SELECTITEM				36
#define STK_PAC_SETUPMENU				37
#define STK_PAC_SETUPIDLEMODETEXT		40

// Termination of Proactive Commands
#define STK_TEM_REFRESH					101
#define STK_TEM_SETUPEVENTLIST			105
#define STK_TEM_SENDDTMF				120
#define STK_TEM_PLAYTONE				132
#define STK_TEM_DISPLAYTEXT				133
#define STK_TEM_GETINKEY				134
#define STK_TEM_GETINPUT				135
#define STK_TEM_SELECTITEM				136
#define STK_TEM_SETUPMENU				137
#define STK_TEM_SETUPIDLEMODETEXT		140

// Event Commands
#define STK_EVT_MENUITEMSELECTION		211
#define STK_EVT_USERACTIVITY			232
#define STK_EVT_IDLESCREENAVAILABLE		233
#define STK_EVT_BROWSERTERMINATION		234
#define STK_EVT_LANGUAGESELECTION		235

// Additional Commands
#define STK_NOT_SIMAPP2MAINMENU			254
#define STK_NOT_SIMRRESET				255

// States of Remote-SAT
#define STK_STA_RESET		0
#define STK_STA_OFF			1
#define STK_STA_IDLE		2
#define STK_STA_PAC			3
#define STK_STA_WAIT		4

// AT^SSTR Remote-SAT Response
#define STK_REP_SUCCESS			0		// Command performed successfully
#define STK_REP_ENDSESSION		16		// Proactive SIM session terminated by user
#define STK_REP_BACKWARD		17		// Backward move in the proactive SIM session requested by the user
#define STK_REP_NORESPONSE		18		// No response from user
#define STK_REP_HELPREQUEST		19		// Help information required by the user
#define STK_REP_USSDSS			20		// USSD/SS Transact terminated by user
#define STK_REP_UNABLE			32		// ME currently unable to process command
#define STK_REP_SCREENBUSY		132		// ME currently unable to process command - screen is busy
#define STK_REP_NOTACCEPT		34		// User did not accept the proactive command
#define STK_REP_CALLDOWN		35		// User cleared down call before connection or network release

// AT^SSTGI Remote-SAT Get Information 每 Refresh (1)
typedef enum tagRefreshMode
{
	modeSIMINITORFFCN = 0,	// SIM Initialization and Full File Change Notification;
	modeFCN,				// File Change Notification;
	modeSIMINITORFCN,		// SIM Initialization and File Change Notification;
	modeSIMINIT,			// SIM Initialization;
	modeSIMRESET			// SIM Reset; ME is performing a SIM reset.
}RefreshMode;

// AT^SSTGI Remote-SAT Get Information 每 Set Up Event List (5)
typedef enum tagEventList
{
	modeNotIn = 0,			// Not in Event List;
	modeSignaledTo			// Events have to be signaled to the ME.
}EventList;

// AT^SSTGI Remote-SAT Get Information 每 Setup Call (16)
typedef enum tagCallType
{
	IfNoOtherCalls = 0,		// Setup if no other calls;
	IfNoOtherCallsRedial,	// Setup if no other calls, with redial;
	PutOnHold,				// Put other calls on hold first;
	PutOnHoldRedial,		// Put other calls on hold first, with redial;
	Disconnect,				// Disconnect other calls first;
	DisconnectRedial		// Disconnect other calls first, with redial.
}CallType;

// AT^SSTGI Remote-SAT Get Information 每 Play Tone (32)
typedef enum tagToneMode
{
	// Standard supervisory tones:
	modeDialTone = 1,		// Dial tone;
	modeBusyTone,			// Called subscriber busy;
	modeCongestion,			// Congestion;
	modePathACK,			// Radio path acknowledge;
	modeCallDropped,		// Radio path not available / Call dropped;
	modeError,				// Error / Special information;
	modeCallWaiting,		// Call waiting tone;
	modeRingingTone,		// Ringing tone;
	// ME proprietary tones:
	modeGeneralBeep = 10,	// General beep;
	modePositiveACK,		// Positive acknowledgement tone;
	modeNegativeACK,		// Negative acknowledgement or error tone.
}ToneMode;

// AT^SSTGI Remote-SAT Get Information 每 Play Tone (32)
typedef enum tagToneUnit
{
	unitMinute = 0,			// Minutes;
	unitSecond,				// Seconds;
	unit10Secs,				// Tenths of Seconds.
}ToneUnit;

// AT^SSTGI Remote-SAT Get Information 每 Display Text (33)
typedef enum tagPriority
{
	prioNormal = 0,			// Normal priority;
	prioHigh				// High priority.
}Priority;

// AT^SSTGI Remote-SAT Get Information 每 Display Text (33)
typedef enum tagClearMode
{
	modeAutoDelay = 0,		// Clear message after a delay;
	modeClearbyUser			// Wait for user to clear message.
}ClearMode;

// AT^SSTGI Remote-SAT Get Information 每 Display Text (33)
typedef enum tagResponseMode
{
	modeTextClear = 0,		// Send TERMINAL RESPONSE when text clears from screen;
	modeImmediately			// TERMINAL RESPONSE sent immediately.
}ResponseMode;

// AT^SSTGI Remote-SAT Get Information 每 Get Inkey (34)
// AT^SSTGI Remote-SAT Get Information 每 Get Input (35)
typedef enum tagInputFormat
{
	fmtDigital = 0,			// Digits (0-9, *, # and +) only;
	fmtAlphabet				// Alphabet set.
}InputFormat;

// AT^SSTGI Remote-SAT Get Information 每 Get Inkey (34)
// AT^SSTGI Remote-SAT Get Information 每 Get Input (35)
typedef enum tagCharacterMode
{
	modeGSM = 0,			// SMS default alphabet (GSM character set);
	modeUCS2				// UCS2 alphabet.
}CharacterMode;

// AT^SSTGI Remote-SAT Get Information 每 Get Inkey (34)
typedef enum tagYESNOMode
{
	modeCharacter = 0,		// Character sets defined by bit 1 and bit 2 are enabled;
	modeRequested			// Disabled and the "Yes/No" response is requested.
}YESNOMode;

// AT^SSTGI Remote-SAT Get Information 每 Get Input (35)
typedef enum tagHiddenMode
{
	modeRevealed = 0,		// ME may echo user input on the display;
	modeHidden				// User input shall not be revealed in any way.
}HiddenMode;

// AT^SSTGI Remote-SAT Get Information 每 Get Input (35)
typedef enum tagPackMode
{
	modeUnpacked = 0,		// User input to be in unpacked format;
	modePacked				// User input to be in SMS packed format.
}PackMode;

// AT^SSTGI Remote-SAT Get Information 每 Select Item (36)
typedef enum tagPresentationMode
{
	modeNoPresentation = 0,	// Presentation type is not specified;
	modePresentation		// Presentation type is specified in bit 2.
}PresentationMode;

// AT^SSTGI Remote-SAT Get Information 每 Select Item (36)
typedef enum tagChoiceMode
{
	modeDataValue = 0,		// Presentation as a choice of data values if bit 1 is '1';
	modeNavigationOption	// Presentation as a choice of navigation options if bit 1 is '1'.
}ChoiceMode;

// AT^SSTGI Remote-SAT Get Information 每 Select Item (36)
// AT^SSTGI Remote-SAT Get Information 每 Setup Menu (37)
typedef enum tagPreferenceMode
{
	modeNoPreference = 0,	// No selection preference;
	modePreference			// Selection using soft key preferred.
}PreferenceMode;

// AT^SSTGI Remote-SAT Get Information 每 Get Inkey (34)
// AT^SSTGI Remote-SAT Get Information 每 Get Input (35)
// AT^SSTGI Remote-SAT Get Information 每 Select Item (36)
// AT^SSTGI Remote-SAT Get Information 每 Setup Menu (37)
typedef enum tagHelpMode
{
	modeNoHelp = 0,			// No help information available;
	modeHelp				// Help information available.
}HelpMode;

// AT^SSTGI Remote-SAT Get Information 每 Refresh (1)
typedef struct tagREFRESH{
	RefreshMode refresh;		// Unsigned Integer, used as enumeration.
}REFRESH, *PREFRESH;

// AT^SSTGI Remote-SAT Get Information 每 Set Up Event List (5)
typedef struct tagSETUPEVENTLIST{
	EventList evtUA;			// Unsigned integer, used as bitfield: bit 5.
	EventList evtISA;			// Unsigned integer, used as bitfield: bit 6.
	EventList evtLS;			// Unsigned integer, used as bitfield: bit 8.
	EventList evtBT;			// Unsigned integer, used as bitfield: bit 9.
}SETUPEVENTLIST, *PSETUPEVENTLIST;

// AT^SSTGI Remote-SAT Get Information 每 Setup Call (16)
typedef struct tagSETUPCALL{
	CallType callType;			// Disposition of other calls during a call setup.
	char* confirmationText;		// String for user confirmation stage.
	int confirmationLen;		// Len of string for user confirmation stage.
	char* calledNumber;			// String containing called number.
	int calledNumberLen;		// Len of string containing called number.
	char* callSetupText;		// String for call setup stage.
	int callSetupLen;			// Len of string for call setup stage.
	int confirmationIconQualifier;	// Unsigned Integer, range 0 每 255, used as a bit-field.
	int confirmationIconId;		// 0-255, 0: No icon.
	int callSetupIconQualifier;	// Unsigned Integer, range 0 每 255, used as a bit-field.
	int callSetupIconId;		// 0-255, 0: No icon.
}SETUPCALL, *PSETUPCALL;

// AT^SSTGI Remote-SAT Get Information 每 Send SS (17)
typedef struct tagSENDSS{
	char* text;					// String.
	int textLen;				// Len of string.
	int iconQualifier;			// Unsigned Integer, range 0 每 255, used as a bit-field.
	int iconId;					// 0-255, 0: No icon.
}SENDSS, *PSENDSS;

// AT^SSTGI Remote-SAT Get Information 每 Send USSD (18)
typedef struct tagSENDUSSD{
	char* text;					// String.
	int textLen;				// Len of string.
	int iconQualifier;			// Unsigned Integer, range 0 每 255, used as a bit-field.
	int iconId;					// 0-255, 0: No icon.
}SENDUSSD, *PSENDUSSD;

// AT^SSTGI Remote-SAT Get Information 每 Send Short Message (19)
typedef struct tagSENDSHORTMESSAGE{
	char* textInfo;				// String to provide user with information.
	int infoLen;				// Len of string to provide user with information.
	int iconQualifier;			// Unsigned Integer, range 0 每 255, used as a bit-field.
	int iconId;					// 0-255, 0: No icon.
}SENDSHORTMESSAGE, *PSENDSHORTMESSAGE;

// AT^SSTGI Remote-SAT Get Information 每 Send DTMF (20)
typedef struct tagSENDDTMF{
	char* text;					// String to provide user with information.
	int textLen;				// Len of string to provide user with information.
	int iconQualifier;			// Unsigned Integer, range 0 每 255, used as a bit-field.
	int iconId;					// 0-255, 0: No icon.
}SENDDTMF, *PSENDDTMF;

// AT^SSTGI Remote-SAT Get Information 每 Play Tone (32)
typedef struct tagPLAYTONE{
	char* infoText;				// String to accompany tone.
	int infoLen;				// Len of string to accompany tone.
	ToneMode tone;				// Tone that the ME generates.
	ToneUnit durationUnit;		// Minutes / Seconds / Tenths of Second.
	int duration;				// Duration of tone, expressed in units (1-255).
	int iconQualifier;			// Unsigned Integer, range 0 每 255, used as a bit-field.
	int iconId;					// 0-255, 0: No icon.
}PLAYTONE, *PPLAYTONE;

// AT^SSTGI Remote-SAT Get Information 每 Display Text (33)
typedef struct tagDISPLAYTEXT{
	Priority priority;			// Unsigned Integer, used as bitfield: bit 1.
	ClearMode clear;			// Unsigned Integer, used as bitfield: bit 8.
	char* text;					// String to be displayed (up to 240 bytes).
	int textLen;				// Len of string to be displayed (up to 240 bytes).
	ResponseMode response;		// Indicates when to send TERMINAL RESPONSE.
	int iconQualifier;			// Unsigned Integer, range 0 每 255, used as a bit-field.
	int iconId;					// 0-255, 0: No icon.
}DISPLAYTEXT, *PDISPLAYTEXT;

// AT^SSTGI Remote-SAT Get Information 每 Get Inkey (34)
typedef struct tagGETINKEY{
	InputFormat inputFormat;	// Unsigned Integer, used as bitfield: bit 1.
	CharacterMode character;	// Unsigned Integer, used as bitfield: bit 2.
	YESNOMode yesNo;			// Unsigned Integer, used as bitfield: bit 3.
	HelpMode help;				// Unsigned Integer, used as bitfield: bit 8.
	char* text;					// String as prompt for text.
	int textLen;				// Len of string as prompt for text.
	int iconQualifier;			// Unsigned Integer, range 0 每 255, used as a bit-field.
	int iconId;					// 0-255, 0: No icon.
}GETINKEY, *PGETINKEY;

// AT^SSTGI Remote-SAT Get Information 每 Get Input (35)
typedef struct tagGETINPUT{
	InputFormat inputFormat;	// Unsigned Integer, used as bitfield: bit 1.
	CharacterMode character;	// Unsigned Integer, used as bitfield: bit 2.
	HiddenMode hidden;			// Unsigned Integer, used as bitfield: bit 3.
	PackMode pack;				// Unsigned Integer, used as bitfield: bit 4.
	HelpMode help;				// Unsigned Integer, used as bitfield: bit 8.
	char* text;					// String as prompt for text.
	int textLen;				// Len of string as prompt for text.
	int responseMin;			// Minimum length of user input (0 每 255).
	int responseMax;			// Maximum length of user input (0 每 255).
	char* defaultText;			// String supplied as default response text.
	int defaultLen;				// Len of string supplied as default response text.
	int iconQualifier;			// Unsigned Integer, range 0 每 255, used as a bit-field.
	int iconId;					// 0-255, 0: No icon.
}GETINPUT, *PGETINPUT;

// AT^SSTGI Remote-SAT Get Information 每 Select Item (36)
typedef struct tagSELECTITEM{
	PresentationMode presentation;	// Unsigned Integer, used as bitfield: bit 1.
	ChoiceMode choice;			// Unsigned Integer, used as bitfield: bit 2.
	PreferenceMode preference;	// Unsigned Integer, used as bitfield: bit 3.
	HelpMode help;				// Unsigned Integer, used as bitfield: bit 8.
	int numOfItems;				// Number of menu items in the list.
	char* titleText;			// String giving menu title.
	int titleLen;				// Len of menu title.
	int defaultItemId;			// Id of default item.
	int itemIconsPresent;		// 0 = No icons, 1 = Icons present.
	int itemIconsQualifier;		// Unsigned Integer, range 0 每 255, used as a bit-field.
	int titleIconQualifier;		// Unsigned Integer, range 0 每 255, used as a bit-field.
	int titleIconId;			// 0-255, 0: No icon.
}SELECTITEM, *PSELECTITEM;

// AT^SSTGI Remote-SAT Get Information 每 Setup Menu (37)
typedef struct tagSETUPMENU{
	PreferenceMode preference;	// Unsigned Integer, used as bitfield: bit 1.
	HelpMode help;				// Unsigned Integer, used as bitfield: bit 8.
	int numOfItems;				// Number of menu items in the list.
	char* titleText;			// String giving menu title.
	int titleLen;				// Len of menu title.
	int menuItemIconsPresent;	// 0 = No icons, 1 = Icons present.
	int menuItemIconsQualifier;	// Unsigned Integer, range 0 每 255, used as a bit-field.
	int titleIconQualifier;		// Unsigned Integer, range 0 每 255, used as a bit-field.
	int titleIconId;			// 0-255, 0: no icon.
}SETUPMENU, *PSETUPMENU;

// AT^SSTGI Remote-SAT Get Information 每 Select Item (36)
// AT^SSTGI Remote-SAT Get Information 每 Setup Menu (37)
typedef struct tagMENUITEM{
	int itemId;					// Menu item identifier (1 每 <numOfItems>).
	char* itemText;				// Title of menu item.
	int itemLen;				// Len of menu item.
	int nextActionId;			// The next proactive command type to be issued.
	int iconId;					// 0-255, 0: No icon.
}MENUITEM, *PMENUITEM, **PPMENUITEM;

// AT^SSTGI Remote-SAT Get Information 每 Setup Idle Mode Text (40)
typedef struct tagSETUPIDLEMODETEXT{
	char* text;					// String to display when TA in Idle Mode.
	int textLen;				// Len of string to display when TA in Idle Mode.
	int iconQualifier;			// Unsigned Integer, range 0 每 255, used as a bit-field.
	int iconId;					// 0-255, 0: No icon.
}SETUPIDLEMODETEXT, *PSETUPIDLEMODETEXT;

// AT^SSTGI Remote-SAT Get Information 每 Get Icon Data (250)
typedef struct tagGETICONDATA{
	int iconId;					// Unsigned Integer, range 0 每 255.
	int iconCount;				// 0: request black/white icon, 1: coloured icon.
	int clutEntries;			// Minimum width of requested icon, range is 2 每 255.
	int BitsPerPoint;			// Minimum height of requested icon, range is 2 每255.
	int iconWidth;				// Maximum width of requested icon, range is 16 每 256.
	int iconHeight;				// Maximum height of requested icon, range is 16 每 256.
	int iconDataLength;			// Number of icon data bytes
}GETICONDATA, *PGETICONDATA, **PPGETICONDATA;

typedef struct tagCLUTDATA{
	int dataType;				// Unsigned Integer, range 0-255.
								// 0: CLUT data line tag, 1: Icon data line tag.
	int lineNumber;				// Data line counter.
	int dataLen;				// Number of data bytes following on this line, maximal 200.
	char* clutData;				// Binary coded data stream containing one RGB byte triplet for every colour.
	int clutDataLen;			// Len of clutData.
}CLUTDATA, *PCLUTDATA, **PPCLUTDATA;

typedef struct tagICONDATA{
	int dataType;				// Unsigned Integer, range 0-255.
								// 0: CLUT data line tag, 1: Icon data line tag.
	int lineNumber;				// Data line counter.
	int dataLen;				// Number of data bytes following on this line, maximal 200.
	char* iconData;				// Binary coded data stream containing the icon data.
	int iconDatalen;			// Len of iconData.
}ICONDATA, *PICONDATA, **PPICONDATA;

#endif // _STK_AT_H_
