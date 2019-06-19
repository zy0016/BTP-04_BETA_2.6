# Microsoft Developer Studio Project File - Name="app_call" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=app_call - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "app_call.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "app_call.mak" CFG="app_call - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "app_call - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "app_call - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/¿ª·¢Çø/src_emu/src/wshell", KACAAAAA"
# PROP Scc_LocalPath "..\..\wshell"
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "app_call - Win32 Release"

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

!ELSEIF  "$(CFG)" == "app_call - Win32 Debug"

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
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "../../../include" /I "../../../include/project" /I "../../../include/ab" /I "../../../include/setup" /I "../../../include/callapp" /I "../../../include/window" /I "../../../include/image" /I "../../../include/me" /I "../../../include/public" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "_EMULATE_" /D "CAPP_DEBUG" /YX /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\..\lib\app_call.lib"

!ENDIF 

# Begin Target

# Name "app_call - Win32 Release"
# Name "app_call - Win32 Debug"
# Begin Group "mbphone"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\mbphone\MB_control.c
# End Source File
# Begin Source File

SOURCE=.\mbphone\MB_GSM.c
# End Source File
# Begin Source File

SOURCE=.\mbphone\MB_GSM.h
# End Source File
# Begin Source File

SOURCE=.\mbphone\MB_SendDTMF.c
# End Source File
# Begin Source File

SOURCE=.\mbphone\MBCallEnd.c
# End Source File
# Begin Source File

SOURCE=.\mbphone\MBCallEnd.h
# End Source File
# Begin Source File

SOURCE=.\mbphone\MBDialLink.c
# End Source File
# Begin Source File

SOURCE=.\mbphone\MBDialup.c
# End Source File
# Begin Source File

SOURCE=.\mbphone\MBDialup.h
# End Source File
# Begin Source File

SOURCE=.\mbphone\MBInCall.c
# End Source File
# Begin Source File

SOURCE=.\mbphone\MBInCall.h
# End Source File
# Begin Source File

SOURCE=.\mbphone\MBPstMsg.c
# End Source File
# Begin Source File

SOURCE=.\mbphone\mbpub_function.c
# End Source File
# Begin Source File

SOURCE=.\mbphone\MBRing.c
# End Source File
# Begin Source File

SOURCE=.\mbphone\MBRing.h
# End Source File
# End Group
# Begin Group "logs"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\logs\gprsdata.c
# End Source File
# Begin Source File

SOURCE=.\logs\log_callcosts.c
# End Source File
# Begin Source File

SOURCE=.\logs\log_callcounters.c
# End Source File
# Begin Source File

SOURCE=.\logs\log_communication.c
# End Source File
# Begin Source File

SOURCE=.\logs\log_counters.c
# End Source File
# Begin Source File

SOURCE=.\logs\log_data.c
# End Source File
# Begin Source File

SOURCE=.\logs\log_gprscounters.c
# End Source File
# Begin Source File

SOURCE=.\logs\log_main.c
# End Source File
# Begin Source File

SOURCE=.\logs\log_main.h
# End Source File
# Begin Source File

SOURCE=.\logs\MBRecordListExt.c
# End Source File
# Begin Source File

SOURCE=.\logs\MBRecordValidPin2.c
# End Source File
# Begin Source File

SOURCE=.\logs\MBRecordView.c
# End Source File
# Begin Source File

SOURCE=.\logs\MBRecordView.h
# End Source File
# End Group
# Begin Group "ss"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ss\btp_btaddr.c
# End Source File
# Begin Source File

SOURCE=.\ss\btp_pminfo.c
# End Source File
# Begin Source File

SOURCE=.\ss\btp_sw.c
# End Source File
# Begin Source File

SOURCE=.\ss\checkNSLC.c
# End Source File
# Begin Source File

SOURCE=.\ss\checkSPSL.c
# End Source File
# Begin Source File

SOURCE=.\ss\ss_callbarring.c
# End Source File
# Begin Source File

SOURCE=.\ss\ss_calldivert.c
# End Source File
# Begin Source File

SOURCE=.\ss\ss_callwaiting.c
# End Source File
# Begin Source File

SOURCE=.\ss\ss_imei.c
# End Source File
# End Group
# Begin Group "ussd"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ussd\ussd_replywnd.c
# End Source File
# Begin Source File

SOURCE=.\ussd\ussd_showwnd.c
# End Source File
# End Group
# End Target
# End Project
