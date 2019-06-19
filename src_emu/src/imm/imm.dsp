# Microsoft Developer Studio Project File - Name="imm" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=imm - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "imm.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "imm.mak" CFG="imm - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "imm - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "imm - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/提交工作区/src_vonga/imm", KPBAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "imm - Win32 Release"

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
# ADD CPP /nologo /W3 /GX /O2 /I "..\include" /I "..\include20" /I "..\load_ui" /I "..\imm" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "_EMULATE" /D "HOPEN_OS" /D "HANWANG_HW" /D "IME_KEYDOWN_USE" /YX /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\lib\imm.lib"

!ELSEIF  "$(CFG)" == "imm - Win32 Debug"

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
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /X /I ".\\" /I "..\..\include" /I "..\..\include\window" /I "..\..\include\image" /I "..\..\include\project" /I "..\..\include\ziime" /D "_EMULATE" /D "HANWANG_HW" /D "HOPEN_OS" /D "WIN32" /D "_DEBUG" /D "_LIB" /D "_MBCS" /D "IME_KEYDOWN_USE" /D "IME_ICON_USE" /D "ZI8_NO_GLOBAL_VARIABLES" /D "OEM_DEF" /YX /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\lib\imm.lib"

!ENDIF 

# Begin Target

# Name "imm - Win32 Release"
# Name "imm - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\imc.c
# End Source File
# Begin Source File

SOURCE=.\ime.c
# End Source File
# Begin Source File

SOURCE=.\imm.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\imc.h
# End Source File
# Begin Source File

SOURCE=.\ime.h
# End Source File
# Begin Source File

SOURCE=.\imm.h
# End Source File
# Begin Source File

SOURCE=.\immtype.h
# End Source File
# End Group
# Begin Group "IMECtrl"

# PROP Default_Filter ".c,.h"
# Begin Source File

SOURCE=.\IMECtrl\calledit.c
# End Source File
# Begin Source File

SOURCE=.\IMECtrl\defime.c
# End Source File
# Begin Source File

SOURCE=.\IMECtrl\imeedit.c
# End Source File
# Begin Source File

SOURCE=.\IMECtrl\imeeditex.c
# End Source File
# Begin Source File

SOURCE=.\IMECtrl\langsel.c
# End Source File
# End Group
# Begin Group "IMEMod"

# PROP Default_Filter ".c,.h"
# Begin Source File

SOURCE=.\IMEMod\imeabc.c
# End Source File
# Begin Source File

SOURCE=.\IMEMod\imenum.c
# End Source File
# Begin Source File

SOURCE=.\IMEMod\imesym.c
# End Source File
# End Group
# Begin Group "ZiIME"

# PROP Default_Filter ""
# Begin Group "zidata"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ZiIME\zidata\English\Zi8DatENPRC.c
# End Source File
# Begin Source File

SOURCE=.\ZiIME\zidata\Chinese\PRC\Zi8DatZH.c
# End Source File
# Begin Source File

SOURCE=.\ZiIME\zidata\zi8sdata.c
# End Source File
# End Group
# Begin Source File

SOURCE=.\ZiIME\ziabc.c
# End Source File
# Begin Source File

SOURCE=.\ZiIME\zichn.c
# End Source File
# Begin Source File

SOURCE=.\ZiIME\ziime.c
# End Source File
# Begin Source File

SOURCE=.\ZiIME\ziphn.c
# End Source File
# Begin Source File

SOURCE=.\ZiIME\zispell.c
# End Source File
# Begin Source File

SOURCE=.\ZiIME\zistk.c
# End Source File
# End Group
# End Target
# End Project
