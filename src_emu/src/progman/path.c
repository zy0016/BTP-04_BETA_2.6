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

#include "window.h"

#include "path.h"
//protocol
static const APPPATH  ScriptPath =	
{
    "script",
    NULL
};
//progman
static const APPPATH  ProgmanPath =	
{
    "progman",
    NULL
};
//pim
static const APPPATH  CPHSPath =	
{
    "cphs",
    NULL
};
static const APPPATH  FavorPath =	
{
    "favorite",
    NULL
};
static const APPPATH  ClockPath =	
{
    "clock",
    NULL
};
static const APPPATH  PcsyncPath =	
{
    "pcsync",
    NULL
};
static const APPPATH  ExchPath =	
{
    "exch",
    NULL
};
static const APPPATH  CalcPath =	
{
    "calc",
    NULL
};
static const APPPATH  NotepadPath =	
{
    "notepad",
    NULL
};
static const APPPATH  CalendarPath =	
{
    "calendar",
    NULL
};
static const APPPATH  AudioPath =	
{
    "audio",
    NULL
};
static const APPPATH  PicturePath =	
{
    "pictures",
    NULL
};
static const APPPATH  MemmanagePath =	
{
    "memmanage",
    NULL
};
static const APPPATH  HelpPath =	
{
    "help",
    NULL
};
static const APPPATH  BluetoothPath =	
{
    "bluetooth",
    NULL
};
//message
static const APPPATH  SmsPath =	
{
    "sms",
    NULL
};
static const APPPATH  MmsPath =	
{
    "mms",
    NULL
};
static const APPPATH  AddressbookPath =	
{
    "addressbook",
    NULL
};
static const APPPATH  CallPath =	
{
    "call",
    NULL
};
static const APPPATH  STKPath =	
{
    "stk",
    NULL
};
static const APPPATH  CalllogsPath =	
{
    "calllogs",
    NULL
};
static const APPPATH  WapPath =	
{
    "wapbrowser",
    NULL
};
static const APPPATH  EmailPath =	
{
    "email",
    NULL
};
static const APPPATH  UniboxPath =	
{
    "unibox",
    NULL
};
//game
static const APPPATH  GamePath =	
{
    "game",
    NULL
};
//setup
static const APPPATH  SetupPath =	
{
    "setup",
    NULL
};
//PDA Sync
static const APPPATH  PdasyncPath =	
{
    "pdasync",
    NULL
};
//public
static const APPPATH  PublicPath =	
{
    "pubcontrol",
    NULL
};
//input
static const APPPATH  ImmPath =	
{
    "imm",
    NULL
};

const APPPATH *AppPath[] =
{
	&ScriptPath,
	&ProgmanPath,
	&CPHSPath,
	&FavorPath,
	&ClockPath,
	&PcsyncPath,
	&ExchPath,
	&CalcPath,
	&NotepadPath,
	&CalendarPath,
	&AudioPath,
	&PicturePath,
	&MemmanagePath,
	&HelpPath,
	&BluetoothPath,
	&SmsPath,
	&MmsPath,
	&AddressbookPath,
	&CallPath,
	&STKPath,
	&CalllogsPath,
	&WapPath,
	&EmailPath,
	&UniboxPath,
	&GamePath,   
	&SetupPath,
	&PdasyncPath,
	&PublicPath,
	&ImmPath,
	NULL
};
//create path in MINISD
//txt
static const APPPATH  TxtPath =	
{
    "txt",
    NULL
};
//audio
static const APPPATH  AudiommcPath =	
{
    "audio",
    NULL
};
//picture
static const APPPATH  PicPath =	
{
    "pictures",
    NULL
};

const APPPATH *AppPathMmc[] =
{
	&TxtPath,
	&AudiommcPath,
	&PicPath,
	NULL
};
