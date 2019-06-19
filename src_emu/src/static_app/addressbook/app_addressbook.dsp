# Microsoft Developer Studio Project File - Name="app_addressbook" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=app_addressbook - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "app_addressbook.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "app_addressbook.mak" CFG="app_addressbook - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "app_addressbook - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "app_addressbook - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "app_addressbook - Win32 Release"

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

!ELSEIF  "$(CFG)" == "app_addressbook - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "app_addressbook___Win32_Debug"
# PROP BASE Intermediate_Dir "app_addressbook___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "app_addressbook___Win32_Debug"
# PROP Intermediate_Dir "app_addressbook___Win32_Debug"
# PROP Target_Dir ""
LINK32=link.exe -lib
MTL=midl.exe
vvRSC=rc.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\..\..\include" /I "..\..\..\include\window" /I "..\..\..\include\project" /I "..\..\..\include\image" /I "..\..\..\include\me" /I "..\..\..\include\setup" /I "..\..\..\include\IVT\stack" /I "..\..\..\include\IVT\sdk" /I "..\..\..\include\bluetooth" /I "..\..\..\include\public" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "_EMULATE_" /YX /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\..\lib\app_addressbook.lib"

!ENDIF 

# Begin Target

# Name "app_addressbook - Win32 Release"
# Name "app_addressbook - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\abdatabase.c
# End Source File
# Begin Source File

SOURCE=.\abedit.c
# End Source File
# Begin Source File

SOURCE=.\abentry.c
# End Source File
# Begin Source File

SOURCE=.\abextern.c
# End Source File
# Begin Source File

SOURCE=.\abgroups.c
# End Source File
# Begin Source File

SOURCE=.\abindex.c
# End Source File
# Begin Source File

SOURCE=.\abinit.c
# End Source File
# Begin Source File

SOURCE=.\abmain.c
# End Source File
# Begin Source File

SOURCE=.\abmemory.c
# End Source File
# Begin Source File

SOURCE=.\abpicker.c
# End Source File
# Begin Source File

SOURCE=.\abquickdialing.c
# End Source File
# Begin Source File

SOURCE=.\absearchwnd.c
# End Source File
# Begin Source File

SOURCE=.\absim.c
# End Source File
# Begin Source File

SOURCE=.\abview.c
# End Source File
# Begin Source File

SOURCE=.\Tmp.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\abconst.h
# End Source File
# Begin Source File

SOURCE=.\abglobal.h
# End Source File
# Begin Source File

SOURCE=.\abrs.h
# End Source File
# End Group
# End Target
# End Project
