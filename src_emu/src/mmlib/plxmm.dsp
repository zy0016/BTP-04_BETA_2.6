# Microsoft Developer Studio Project File - Name="plxmm" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=plxmm - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "plxmm.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "plxmm.mak" CFG="plxmm - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "plxmm - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "plxmm - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/plxmm/build/emu", SAAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "plxmm - Win32 Release"

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

!ELSEIF  "$(CFG)" == "plxmm - Win32 Debug"

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
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\..\include" /I "..\..\include\project" /I "..\..\include\window" /I ".\amr\amrnb" /I ".\amr\amrwb" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "_EMULATE_" /FR /FD /GZ /c
# SUBTRACT CPP /X /YX
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\lib\plxmm.lib"

!ENDIF 

# Begin Target

# Name "plxmm - Win32 Release"
# Name "plxmm - Win32 Debug"
# Begin Group "amr"

# PROP Default_Filter ""
# Begin Group "amrwb"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\amr\amrwb\dec_acelp.c
# End Source File
# Begin Source File

SOURCE=.\amr\amrwb\dec_dtx.c
# End Source File
# Begin Source File

SOURCE=.\amr\amrwb\dec_gain.c
# End Source File
# Begin Source File

SOURCE=.\amr\amrwb\dec_if.c
# End Source File
# Begin Source File

SOURCE=.\amr\amrwb\dec_lpc.c
# End Source File
# Begin Source File

SOURCE=.\amr\amrwb\dec_main.c
# End Source File
# Begin Source File

SOURCE=.\amr\amrwb\dec_rom.c
# End Source File
# Begin Source File

SOURCE=.\amr\amrwb\dec_util.c
# End Source File
# Begin Source File

SOURCE=.\amr\amrwb\if_rom.c
# End Source File
# End Group
# Begin Group "amrnb"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\amr\amrnb\amrconv.c
# End Source File
# Begin Source File

SOURCE=.\amr\amrnb\amrdec.c
# End Source File
# Begin Source File

SOURCE=.\amr\amrnb\sphdec.c
# End Source File
# End Group
# Begin Source File

SOURCE=.\amr\amrface.c
# End Source File
# Begin Source File

SOURCE=.\amr\amrface.h
# End Source File
# End Group
# Begin Group "midi"

# PROP Default_Filter ""
# End Group
# Begin Group "mp3"

# PROP Default_Filter ""
# End Group
# Begin Group "wave"

# PROP Default_Filter ""
# End Group
# Begin Source File

SOURCE=.\plxmm.c
# End Source File
# End Target
# End Project
