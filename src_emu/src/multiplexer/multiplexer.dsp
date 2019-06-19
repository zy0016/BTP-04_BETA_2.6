# Microsoft Developer Studio Project File - Name="multiplexer" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=multiplexer - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "multiplexer.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "multiplexer.mak" CFG="multiplexer - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "multiplexer - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "multiplexer - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/¿ª·¢Çø/src_emu/src/multiplexer", OBCAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "multiplexer - Win32 Release"

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

!ELSEIF  "$(CFG)" == "multiplexer - Win32 Debug"

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
# ADD CPP /nologo /W4 /Gm /GX /ZI /Od /X /I "..\..\include20" /I "..\..\include20\ddi" /I "..\..\include\window" /I "..\..\include\project" /I "..\..\include\me" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "_EMULATE_" /YX /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\lib\multiplexer.lib"

!ENDIF 

# Begin Target

# Name "multiplexer - Win32 Release"
# Name "multiplexer - Win32 Debug"
# Begin Group "multiplex"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\multiplex\multchnl.c
# End Source File
# Begin Source File

SOURCE=.\multiplex\multchnl.h
# End Source File
# Begin Source File

SOURCE=.\multiplex\muxagent.c
# End Source File
# Begin Source File

SOURCE=.\multiplex\muxagent.h
# End Source File
# Begin Source File

SOURCE=.\multiplex\siem_mux.c
# End Source File
# Begin Source File

SOURCE=.\multiplex\siem_mux.h
# End Source File
# Begin Source File

SOURCE=.\multiplex\ti_mux.c
# End Source File
# Begin Source File

SOURCE=.\multiplex\ti_mux.h
# End Source File
# Begin Source File

SOURCE=.\multiplex\wave_mux.c
# End Source File
# Begin Source File

SOURCE=.\multiplex\wave_mux.h
# End Source File
# End Group
# End Target
# End Project
