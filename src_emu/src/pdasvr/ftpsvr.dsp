# Microsoft Developer Studio Project File - Name="ftpsvr" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=ftpsvr - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ftpsvr.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ftpsvr.mak" CFG="ftpsvr - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ftpsvr - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "ftpsvr - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/¿ª·¢Çø/src_emu/src/wshell", KACAAAAA"
# PROP Scc_LocalPath "..\wshell"
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ftpsvr - Win32 Release"

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
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "ftpsvr - Win32 Debug"

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
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /X /I "../../include20" /I "../../include" /I "../../include/public" /I "../../include/image" /I "../../include/me" /I "../../include20/ddi" /I "../../include/window" /I "../../include/project" /I "../include/ddi" /I "../build12/inc" /D "DEBUG" /D "_EMULATE_" /D "EMULATION" /D "_HOPEN" /D "_DEBUG" /D "_KERN200" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\lib\ftpsvr.lib"

!ENDIF 

# Begin Target

# Name "ftpsvr - Win32 Release"
# Name "ftpsvr - Win32 Debug"
# Begin Source File

SOURCE=.\ts_view.c
# End Source File
# Begin Source File

SOURCE=.\tsdata.c
# End Source File
# Begin Source File

SOURCE=.\tsftpapi.c
# End Source File
# Begin Source File

SOURCE=.\tsftpsys.c
# End Source File
# Begin Source File

SOURCE=.\tsmain.h
# End Source File
# Begin Source File

SOURCE=.\tsprot.c
# End Source File
# End Target
# End Project
