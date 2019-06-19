# Microsoft Developer Studio Project File - Name="setup" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=setup - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "setup.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "setup.mak" CFG="setup - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "setup - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "setup - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/¿ª·¢Çø/src_emu/src/wshell", KACAAAAA"
# PROP Scc_LocalPath "..\..\wshell"
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "setup - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
LINK32=link.exe -lib
MTL=midl.exe
vvRSC=rc.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "setup - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
LINK32=link.exe -lib
MTL=midl.exe
vvRSC=rc.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /X /I "..\..\..\include" /I "..\..\..\include\setup" /I "..\..\..\include\public" /I "..\..\..\include20" /I "..\..\..\include\sys" /I "..\..\..\include\window" /I "..\..\..\include\image" /I "..\..\..\include\project" /I "..\..\..\include\me" /I "..\..\..\include\callapp" /I "..\..\..\include\ab" /I "..\..\..\include\benefon" /I "..\..\..\include/pm" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\..\lib\setup.lib"

!ENDIF 

# Begin Target

# Name "setup - Win32 Release"
# Name "setup - Win32 Debug"
# Begin Group "mobilesetting"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\mobilesetting\AutoCloseClew.c
# End Source File
# Begin Source File

SOURCE=.\mobilesetting\AutoCloseOpen.c
# End Source File
# Begin Source File

SOURCE=.\mobilesetting\AutoCloseOpen.h
# End Source File
# Begin Source File

SOURCE=.\mobilesetting\CallColorThemeSetting.c
# End Source File
# Begin Source File

SOURCE=.\mobilesetting\CallDateFormat.c
# End Source File
# Begin Source File

SOURCE=.\mobilesetting\CallDateFormat.h
# End Source File
# Begin Source File

SOURCE=.\mobilesetting\CallDateOnOptions.c
# End Source File
# Begin Source File

SOURCE=.\mobilesetting\CallDateOnOptions.h
# End Source File
# Begin Source File

SOURCE=.\mobilesetting\CallKeyLockSetting.c
# End Source File
# Begin Source File

SOURCE=.\mobilesetting\CallKeyLockSetting.h
# End Source File
# Begin Source File

SOURCE=.\mobilesetting\CallLevelAdjuster.c
# End Source File
# Begin Source File

SOURCE=.\mobilesetting\CallSelZoneUsedWindow.c
# End Source File
# Begin Source File

SOURCE=.\mobilesetting\CallSelZoneUsedWindow.h
# End Source File
# Begin Source File

SOURCE=.\mobilesetting\CallShortCutSetting.c
# End Source File
# Begin Source File

SOURCE=.\mobilesetting\CallShortCutSetting.h
# End Source File
# Begin Source File

SOURCE=.\mobilesetting\CallTimeFormat.c
# End Source File
# Begin Source File

SOURCE=.\mobilesetting\CallTimeFormat.h
# End Source File
# Begin Source File

SOURCE=.\mobilesetting\CallTimeZone.c
# End Source File
# Begin Source File

SOURCE=.\mobilesetting\CallTimeZone.h
# End Source File
# Begin Source File

SOURCE=.\mobilesetting\CallUnitSetting.c
# End Source File
# Begin Source File

SOURCE=.\mobilesetting\CallUnitSetting.h
# End Source File
# Begin Source File

SOURCE=.\mobilesetting\displaysetting.c
# End Source File
# Begin Source File

SOURCE=.\mobilesetting\displaysetting.h
# End Source File
# Begin Source File

SOURCE=.\mobilesetting\GenPhoSetting.c
# End Source File
# Begin Source File

SOURCE=.\mobilesetting\GenPhoSetting.h
# End Source File
# Begin Source File

SOURCE=.\mobilesetting\GPSTimeManager.c
# End Source File
# Begin Source File

SOURCE=.\mobilesetting\LangSettingList.c
# End Source File
# Begin Source File

SOURCE=.\mobilesetting\LangSettingList.h
# End Source File
# Begin Source File

SOURCE=.\mobilesetting\LevAdjControl.c
# End Source File
# Begin Source File

SOURCE=.\mobilesetting\mobilesetting.c
# End Source File
# Begin Source File

SOURCE=.\mobilesetting\mobilesetting.h
# End Source File
# Begin Source File

SOURCE=.\mobilesetting\SettInfoProcBuf.c
# End Source File
# Begin Source File

SOURCE=.\mobilesetting\timeediting.c
# End Source File
# Begin Source File

SOURCE=.\mobilesetting\timeediting.h
# End Source File
# Begin Source File

SOURCE=.\mobilesetting\timesetting.c
# End Source File
# Begin Source File

SOURCE=.\mobilesetting\timesetting.h
# End Source File
# End Group
# Begin Group "scenemode"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\scenemode\CallAlertGrp.c
# End Source File
# Begin Source File

SOURCE=.\scenemode\CallAlertGrp.h
# End Source File
# Begin Source File

SOURCE=.\scenemode\callcreatenewmode.c
# End Source File
# Begin Source File

SOURCE=.\scenemode\callcreatenewmode.h
# End Source File
# Begin Source File

SOURCE=.\scenemode\CallMissedEvent.c
# End Source File
# Begin Source File

SOURCE=.\scenemode\CallMissedEvent.h
# End Source File
# Begin Source File

SOURCE=.\scenemode\CallProfileList.c
# End Source File
# Begin Source File

SOURCE=.\scenemode\CallProfileList.h
# End Source File
# Begin Source File

SOURCE=.\scenemode\callprofilename.c
# End Source File
# Begin Source File

SOURCE=.\scenemode\callprofilename.h
# End Source File
# Begin Source File

SOURCE=.\scenemode\CallRingMode.c
# End Source File
# Begin Source File

SOURCE=.\scenemode\CallRingMode.h
# End Source File
# Begin Source File

SOURCE=.\scenemode\callringsetwindow.c
# End Source File
# Begin Source File

SOURCE=.\scenemode\callringsetwindow.h
# End Source File
# Begin Source File

SOURCE=.\scenemode\CallSelAlertGrp.c
# End Source File
# Begin Source File

SOURCE=.\scenemode\CallSelAlertGrp.h
# End Source File
# Begin Source File

SOURCE=.\scenemode\GetSetSMInfo.c
# End Source File
# Begin Source File

SOURCE=.\scenemode\profile.c
# End Source File
# Begin Source File

SOURCE=.\scenemode\profilemode.c
# End Source File
# Begin Source File

SOURCE=.\scenemode\profilemode.h
# End Source File
# End Group
# Begin Group "securitysetting"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\securitysetting\CODE.h
# End Source File
# Begin Source File

SOURCE=.\securitysetting\FixedDial.c
# End Source File
# Begin Source File

SOURCE=.\securitysetting\FixedDial.h
# End Source File
# Begin Source File

SOURCE=.\securitysetting\ModifyPassword.c
# End Source File
# Begin Source File

SOURCE=.\securitysetting\ModifyPassword.h
# End Source File
# Begin Source File

SOURCE=.\securitysetting\NewCode.c
# End Source File
# Begin Source File

SOURCE=.\securitysetting\NewCode.h
# End Source File
# Begin Source File

SOURCE=.\securitysetting\PhoneLock.c
# End Source File
# Begin Source File

SOURCE=.\securitysetting\PhoneLock.h
# End Source File
# Begin Source File

SOURCE=.\securitysetting\securitysetting.c
# End Source File
# Begin Source File

SOURCE=.\securitysetting\securitysetting.h
# End Source File
# Begin Source File

SOURCE=.\securitysetting\setupbutton.c
# End Source File
# End Group
# Begin Group "positioning"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\positioning\CallEditGPSName.c
# End Source File
# Begin Source File

SOURCE=.\positioning\CallEditGPSName.h
# End Source File
# Begin Source File

SOURCE=.\positioning\CallGPSSettingInfoView.c
# End Source File
# Begin Source File

SOURCE=.\positioning\CallSelGPSIcon.c
# End Source File
# Begin Source File

SOURCE=.\positioning\CallSetGPSNMEA.c
# End Source File
# Begin Source File

SOURCE=.\positioning\CallSetGPSNMEA.h
# End Source File
# Begin Source File

SOURCE=.\positioning\CallSetGPSOpti.c
# End Source File
# Begin Source File

SOURCE=.\positioning\CallSetGPSOpti.h
# End Source File
# Begin Source File

SOURCE=.\positioning\CallSetGPSRefIntCha.c
# End Source File
# Begin Source File

SOURCE=.\positioning\CallSetGPSRefIntCha.h
# End Source File
# Begin Source File

SOURCE=.\positioning\CallSetGPSRefIntvl.c
# End Source File
# Begin Source File

SOURCE=.\positioning\CallSetGPSRefIntvl.h
# End Source File
# Begin Source File

SOURCE=.\positioning\GetSetGPSMInfo.c
# End Source File
# Begin Source File

SOURCE=.\positioning\GPSProfileSettingList.c
# End Source File
# Begin Source File

SOURCE=.\positioning\GPSProfileSettingList.h
# End Source File
# Begin Source File

SOURCE=.\positioning\positioningsetting.c
# End Source File
# Begin Source File

SOURCE=.\positioning\positioningsetting.h
# End Source File
# End Group
# Begin Group "personal"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\personal\cartoon.c
# End Source File
# Begin Source File

SOURCE=.\personal\cartoon.h
# End Source File
# Begin Source File

SOURCE=.\personal\cartoon_pre.c
# End Source File
# Begin Source File

SOURCE=.\personal\GifInterface.c
# End Source File
# Begin Source File

SOURCE=.\personal\GifInterface.h
# End Source File
# Begin Source File

SOURCE=.\personal\MusicSetting.c
# End Source File
# Begin Source File

SOURCE=.\personal\MusicSetting.h
# End Source File
# Begin Source File

SOURCE=.\personal\personal.c
# End Source File
# Begin Source File

SOURCE=.\personal\personal.h
# End Source File
# Begin Source File

SOURCE=.\personal\Salutatory.c
# End Source File
# Begin Source File

SOURCE=.\personal\Salutatory.h
# End Source File
# End Group
# Begin Group "callsetting"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\callsetting\MBCallServ.c
# End Source File
# Begin Source File

SOURCE=.\callsetting\MBCallServ.h
# End Source File
# End Group
# Begin Group "setdial"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\setdial\setdial.c
# End Source File
# Begin Source File

SOURCE=.\setdial\setdial.h
# End Source File
# End Group
# Begin Group "Connectivity"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\connectivity\CallConnectionSetting.c
# End Source File
# Begin Source File

SOURCE=.\connectivity\CallConnectionSetting.h
# End Source File
# Begin Source File

SOURCE=.\connectivity\CallConSettingList.c
# End Source File
# Begin Source File

SOURCE=.\connectivity\CallConSettingList.h
# End Source File
# Begin Source File

SOURCE=.\connectivity\CallCreateNewConnection.c
# End Source File
# Begin Source File

SOURCE=.\connectivity\CallCreateNewConnection.h
# End Source File
# Begin Source File

SOURCE=.\connectivity\CallDatacallSetting.c
# End Source File
# Begin Source File

SOURCE=.\connectivity\CallDatacallSetting.h
# End Source File
# Begin Source File

SOURCE=.\connectivity\CallEditConnHomepage.c
# End Source File
# Begin Source File

SOURCE=.\connectivity\CallEditConnHomepage.h
# End Source File
# Begin Source File

SOURCE=.\connectivity\CallEditConnLogin.c
# End Source File
# Begin Source File

SOURCE=.\connectivity\CallEditConnLogin.h
# End Source File
# Begin Source File

SOURCE=.\connectivity\CallEditConnName.c
# End Source File
# Begin Source File

SOURCE=.\connectivity\CallEditConnName.h
# End Source File
# Begin Source File

SOURCE=.\connectivity\CallEditConnPort.c
# End Source File
# Begin Source File

SOURCE=.\connectivity\CallEditConnPort.h
# End Source File
# Begin Source File

SOURCE=.\connectivity\CallEditDialupNum.c
# End Source File
# Begin Source File

SOURCE=.\connectivity\CallEditDialupNum.h
# End Source File
# Begin Source File

SOURCE=.\connectivity\CallEditGateWay.c
# End Source File
# Begin Source File

SOURCE=.\connectivity\CallEditGateWay.h
# End Source File
# Begin Source File

SOURCE=.\connectivity\CallEditGPRSAccPoint.c
# End Source File
# Begin Source File

SOURCE=.\connectivity\CallEditGPRSAccPoint.h
# End Source File
# Begin Source File

SOURCE=.\connectivity\CallGPRSSetting.c
# End Source File
# Begin Source File

SOURCE=.\connectivity\CallGPRSSetting.h
# End Source File
# Begin Source File

SOURCE=.\connectivity\CallMsgingSetting.c
# End Source File
# Begin Source File

SOURCE=.\connectivity\CallMsgingSetting.h
# End Source File
# Begin Source File

SOURCE=.\connectivity\CallSelConnAuthen.c
# End Source File
# Begin Source File

SOURCE=.\connectivity\CallSelConnAuthen.h
# End Source File
# Begin Source File

SOURCE=.\connectivity\CallSelConnBearer.c
# End Source File
# Begin Source File

SOURCE=.\connectivity\CallSelConnBearer.h
# End Source File
# Begin Source File

SOURCE=.\connectivity\CallSelDatacalTpSp.c
# End Source File
# Begin Source File

SOURCE=.\connectivity\CallSelDatacalTpSp.h
# End Source File
# Begin Source File

SOURCE=.\connectivity\connectivity.c
# End Source File
# Begin Source File

SOURCE=.\connectivity\connectivity.h
# End Source File
# Begin Source File

SOURCE=.\connectivity\GSMRadioSetting.c
# End Source File
# Begin Source File

SOURCE=.\connectivity\GSMRadioSetting.h
# End Source File
# End Group
# Begin Group "ModemSetting"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ModemSetting\modemsetting.c
# End Source File
# End Group
# Begin Group "AccessorySetting "

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\AccessorySetting\AccessorySetting.c
# End Source File
# Begin Source File

SOURCE=.\AccessorySetting\AccessorySetting.h
# End Source File
# Begin Source File

SOURCE=.\AccessorySetting\AccSettingOptList.c
# End Source File
# Begin Source File

SOURCE=.\AccessorySetting\AccSettingOptList.h
# End Source File
# End Group
# Begin Group "SetupMenuList"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\setupmenulist\SettingMenuList.c
# End Source File
# Begin Source File

SOURCE=.\setupmenulist\SettingMenuList.h
# End Source File
# End Group
# Begin Group "projectmode"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\projectmode\project_AT.c
# End Source File
# Begin Source File

SOURCE=.\projectmode\project_memory.c
# End Source File
# Begin Source File

SOURCE=.\projectmode\project_SMS_FTA.c
# End Source File
# Begin Source File

SOURCE=.\projectmode\ProjectMode.c
# End Source File
# End Group
# Begin Group "menumanage"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\menumanage\FMenu.c
# End Source File
# Begin Source File

SOURCE=.\menumanage\FMenu.h
# End Source File
# Begin Source File

SOURCE=.\menumanage\Menu.h
# End Source File
# Begin Source File

SOURCE=.\menumanage\SMenu.c
# End Source File
# Begin Source File

SOURCE=.\menumanage\SMenu.h
# End Source File
# End Group
# End Target
# End Project
