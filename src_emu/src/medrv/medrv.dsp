# Microsoft Developer Studio Project File - Name="MEdrv" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=MEdrv - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "medrv.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "medrv.mak" CFG="MEdrv - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MEdrv - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "MEdrv - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/DB/src_emu/src_me/medrv", SFBAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "MEdrv - Win32 Release"

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
# ADD CPP /nologo /W3 /GX /O2 /I "..\include" /I "..\include\ddi" /D "NDEBUG" /D "_WINDOWS" /D "_EMULATE_" /YX /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "MEdrv - Win32 Debug"

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
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /X /I "../../include" /I "../../include20" /I "../../include20/ddi" /I "../../include/project" /I "../../include/me" /I "../../include/window" /I "../../include/image" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_EMULATE_" /D "_TCPIPMEMDEB_" /D "_TCPIPX86_" /D "_TCPIPASSERT_" /D "LITTLE_ENDIAN" /D "_IP_DWBROADCAST_" /D "_TCPIP_BULID_VC_" /D "_TCPIP_NOUI_" /D "TCPIP_SINGELMODUL" /D "_KERN200" /FR /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\lib\MEdrv.lib"

!ENDIF 

# Begin Target

# Name "MEdrv - Win32 Release"
# Name "MEdrv - Win32 Debug"
# Begin Source File

SOURCE=.\cbspdu.c
# End Source File
# Begin Source File

SOURCE=.\cbspdu.h
# End Source File
# Begin Source File

SOURCE=.\me_at.c
# End Source File
# Begin Source File

SOURCE=.\me_comn.c
# End Source File
# Begin Source File

SOURCE=.\me_comn.h
# End Source File
# Begin Source File

SOURCE=.\me_func.c
# End Source File
# Begin Source File

SOURCE=.\me_func.h
# End Source File
# Begin Source File

SOURCE=.\me_init.c
# End Source File
# Begin Source File

SOURCE=.\me_wnd.c
# End Source File
# Begin Source File

SOURCE=.\smspdu.c
# End Source File
# Begin Source File

SOURCE=.\smspdu.h
# End Source File
# End Target
# End Project
