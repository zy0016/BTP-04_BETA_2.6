# Microsoft Developer Studio Project File - Name="APP_Public" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=APP_Public - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "APP_Public.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "APP_Public.mak" CFG="APP_Public - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "APP_Public - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "APP_Public - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/¿ª·¢Çø/src_emu/src/static_app/public", TTBAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "APP_Public - Win32 Release"

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

!ELSEIF  "$(CFG)" == "APP_Public - Win32 Debug"

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
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\..\..\include" /I "..\..\..\include\public" /I "..\pub_interface" /I "..\..\..\include\window" /I "..\..\..\include\image" /I "..\..\..\include\project" /I "..\..\..\include\me" /I "..\..\..\include\setup" /I "..\..\..\include\ab" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_DEBUG" /D "_EMULATE_" /D "_863BIRD_" /FR /YX /FD /GZ /c
# SUBTRACT CPP /X
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\..\lib\public.lib"

!ENDIF 

# Begin Target

# Name "APP_Public - Win32 Release"
# Name "APP_Public - Win32 Debug"
# Begin Group "pub_control"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Pub_Control\ColorSelect.c
# End Source File
# Begin Source File

SOURCE=.\pub_control\confirmw.c
# End Source File
# Begin Source File

SOURCE=.\pub_control\confirmwEx.c
# End Source File
# Begin Source File

SOURCE=.\pub_control\editex.c
# End Source File
# Begin Source File

SOURCE=.\pub_control\FormViewer.c
# End Source File
# Begin Source File

SOURCE=.\pub_control\FullScreenConfirm.c
# End Source File
# Begin Source File

SOURCE=.\pub_control\FullScreenTimeout.c
# End Source File
# Begin Source File

SOURCE=.\Pub_Control\GameHelp.c
# End Source File
# Begin Source File

SOURCE=.\pub_control\GetValidUrl.c
# End Source File
# Begin Source File

SOURCE=.\pub_control\LevInd.c
# End Source File
# Begin Source File

SOURCE=.\Pub_Control\menulist.c
# End Source File
# Begin Source File

SOURCE=.\Pub_Control\menulist.h
# End Source File
# Begin Source File

SOURCE=.\Pub_Control\PassChange.c
# End Source File
# Begin Source File

SOURCE=.\Pub_Control\PassCheck.c
# End Source File
# Begin Source File

SOURCE=.\pub_control\piclist.c
# End Source File
# Begin Source File

SOURCE=.\pub_control\picmultilist.c
# End Source File
# Begin Source File

SOURCE=.\Pub_Control\PicPreBrow.c
# End Source File
# Begin Source File

SOURCE=.\Pub_Control\PreBrow.c
# End Source File
# Begin Source File

SOURCE=.\Pub_Control\PreBrowHead.c
# End Source File
# Begin Source File

SOURCE=.\pub_control\Receivebox.c
# End Source File
# Begin Source File

SOURCE=.\pub_control\Recipient.c
# End Source File
# Begin Source File

SOURCE=.\pub_control\RGPreBrow.c
# End Source File
# Begin Source File

SOURCE=.\pub_control\Search.c
# End Source File
# Begin Source File

SOURCE=.\Pub_Control\SHPreBrow.c
# End Source File
# Begin Source File

SOURCE=.\pub_control\SMPreBrow.c
# End Source File
# Begin Source File

SOURCE=.\pub_control\spinex.c
# End Source File
# Begin Source File

SOURCE=.\pub_control\SSPassCheck.c
# End Source File
# Begin Source File

SOURCE=.\pub_control\TextView.c
# End Source File
# Begin Source File

SOURCE=.\pub_control\timeedit.c
# End Source File
# Begin Source File

SOURCE=.\Pub_Control\tipswin.c
# End Source File
# Begin Source File

SOURCE=.\pub_control\Tipswinforstk.c
# End Source File
# Begin Source File

SOURCE=.\Pub_Control\TxPreBrow.c
# End Source File
# Begin Source File

SOURCE=.\pub_control\unitedit.c
# End Source File
# Begin Source File

SOURCE=.\Pub_Control\WaitWin.c
# End Source File
# Begin Source File

SOURCE=.\pub_control\wildcard.c
# End Source File
# Begin Source File

SOURCE=.\pub_control\wsctrlex.c
# End Source File
# End Group
# Begin Group "pub_interface"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\pub_interface\GXSReset.c
# End Source File
# Begin Source File

SOURCE=.\pub_interface\OptionSelect.c
# End Source File
# Begin Source File

SOURCE=.\pub_interface\playmusic.c
# End Source File
# Begin Source File

SOURCE=.\pub_interface\pub_function.c
# End Source File
# Begin Source File

SOURCE=.\pub_interface\smscount.c
# End Source File
# Begin Source File

SOURCE=.\pub_interface\Version.c
# End Source File
# End Group
# End Target
# End Project
