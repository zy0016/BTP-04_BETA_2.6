# Microsoft Developer Studio Project File - Name="Progman" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=Progman - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Progman.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Progman.mak" CFG="Progman - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Progman - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Progman - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/RWP_SRC_Emu/Progman", WOAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Progman - Win32 Release"

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
vvRSC=rc.exe
MTL=midl.exe
LINK32=link.exe -lib
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "../include" /I "../include/ddi" /I "../build12/inc" /I "../ImeAsp" /D "DEBUG" /D "_EMULATE_" /D "EMULATION" /D "_HOPEN" /D "_DEBUG" /D "_KERN200" /YX /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "Progman - Win32 Debug"

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
vvRSC=rc.exe
MTL=midl.exe
LINK32=link.exe -lib
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /X /I "../../include/sms" /I "../../include/callapp" /I "../../include" /I "../../include/hopen" /I "../../include/project" /I "../../include/window" /I "../../include/image" /I "../../include/me" /I "../../include/setup" /I "../../include/public" /I "../../include/ab" /I "../../include/benefon" /I "../../include/browser" /I "../../include/pm" /D "DEBUG" /D "_EMULATE_" /D "EMULATION" /D "_HOPEN" /D "_DEBUG" /D "_KERN300" /D "WIN32" /D "_863BIRD_" /YX /FD /GZ /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\lib\Progman.lib"

!ENDIF 

# Begin Target

# Name "Progman - Win32 Release"
# Name "Progman - Win32 Debug"
# Begin Group "source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\AppDesk.c
# End Source File
# Begin Source File

SOURCE=.\AppDirs.c
# End Source File
# Begin Source File

SOURCE=.\AppFile.c
# End Source File
# Begin Source File

SOURCE=.\AppGrp.C
# End Source File
# Begin Source File

SOURCE=.\AppMain.c
# End Source File
# Begin Source File

SOURCE=.\AppPath.c
# End Source File
# Begin Source File

SOURCE=.\chongdian.c
# End Source File
# Begin Source File

SOURCE=.\compatable.c
# End Source File
# Begin Source File

SOURCE=.\GpsApp.c
# End Source File
# Begin Source File

SOURCE=.\group.c
# End Source File
# Begin Source File

SOURCE=.\hook.c
# End Source File
# Begin Source File

SOURCE=.\NotiProg.C
# End Source File
# Begin Source File

SOURCE=.\path.c
# End Source File
# Begin Source File

SOURCE=.\PMALARM.C
# End Source File
# Begin Source File

SOURCE=.\PMI.C
# End Source File
# Begin Source File

SOURCE=.\PowerOff.c
# End Source File
# Begin Source File

SOURCE=.\Progman.c
# End Source File
# Begin Source File

SOURCE=.\siminit.c
# End Source File
# End Group
# Begin Group "header"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\AppChar.h
# End Source File
# Begin Source File

SOURCE=.\AppDesk.h
# End Source File
# Begin Source File

SOURCE=.\AppDirs.h
# End Source File
# Begin Source File

SOURCE=.\AppFile.h
# End Source File
# Begin Source File

SOURCE=.\AppMain.h
# End Source File
# Begin Source File

SOURCE=.\progman.h
# End Source File
# Begin Source File

SOURCE=.\shortcut.h
# End Source File
# End Group
# End Target
# End Project
