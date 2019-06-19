# Microsoft Developer Studio Project File - Name="APP_Browser" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=APP_Browser - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "APP_Browser.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "APP_Browser.mak" CFG="APP_Browser - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "APP_Browser - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "APP_Browser - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "APP_Browser - Win32 Release"

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
# ADD CPP /nologo /W3 /GX /O2 /X /I "..\..\include20" /I "..\..\include20\ddi" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "_EMULATE_" /YX /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "APP_Browser - Win32 Debug"

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
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /X /I "..\..\..\include" /I "..\..\..\include\window" /I "..\..\..\include\project" /I "..\..\..\include\image" /I "..\..\..\include\me" /I "..\..\..\include\setup" /I "..\..\..\include\browser" /I "..\..\..\include\cryption" /I "..\..\..\include\public" /I "..\..\..\include\wap" /D "WIN32" /D "_EMULATE_" /D "_DEBUG" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\..\lib\Browser.lib"

!ENDIF 

# Begin Target

# Name "APP_Browser - Win32 Release"
# Name "APP_Browser - Win32 Debug"
# Begin Group "depend"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\depend\wtadepend.c
# End Source File
# End Group
# Begin Source File

SOURCE=.\AuthProc.c
# End Source File
# Begin Source File

SOURCE=.\AuthProc.h
# End Source File
# Begin Source File

SOURCE=.\cert.h
# End Source File
# Begin Source File

SOURCE=.\DownLoad.c
# End Source File
# Begin Source File

SOURCE=.\DownLoad.h
# End Source File
# Begin Source File

SOURCE=.\Hopen_DocCtrl.c
# End Source File
# Begin Source File

SOURCE=.\Hopen_DrawDev.c
# End Source File
# Begin Source File

SOURCE=.\UrlRatio.c
# End Source File
# Begin Source File

SOURCE=.\UrlRatio.h
# End Source File
# Begin Source File

SOURCE=.\wappush.c
# End Source File
# Begin Source File

SOURCE=.\WapPush.h
# End Source File
# Begin Source File

SOURCE=.\wappushapi.c
# End Source File
# Begin Source File

SOURCE=.\wappushapi.h
# End Source File
# Begin Source File

SOURCE=.\Wauthwin.c
# End Source File
# Begin Source File

SOURCE=.\WIE.c
# End Source File
# Begin Source File

SOURCE=.\WIE.h
# End Source File
# Begin Source File

SOURCE=.\WIEBk.c
# End Source File
# Begin Source File

SOURCE=.\WIEBK.h
# End Source File
# Begin Source File

SOURCE=.\wiego.c
# End Source File
# Begin Source File

SOURCE=.\wiego.h
# End Source File
# Begin Source File

SOURCE=.\wiepub.c
# End Source File
# Begin Source File

SOURCE=.\WieSaveas.c
# End Source File
# Begin Source File

SOURCE=.\WieSaveas.h
# End Source File
# Begin Source File

SOURCE=.\WIEsetting.c
# End Source File
# Begin Source File

SOURCE=.\WIEsetting.h
# End Source File
# Begin Source File

SOURCE=.\wievisited.c
# End Source File
# Begin Source File

SOURCE=.\wievisited.h
# End Source File
# Begin Source File

SOURCE=.\wUipub.h
# End Source File
# End Target
# End Project
