# Microsoft Developer Studio Project File - Name="APP_Pim" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=APP_Pim - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "APP_Pim.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "APP_Pim.mak" CFG="APP_Pim - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "APP_Pim - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/¿ª·¢Çø/src_emu/src/static_app/pim", FPBAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe
# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "APP_Pim___Win32_Debug"
# PROP BASE Intermediate_Dir "APP_Pim___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ""
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
vvRSC=rc.exe
MTL=midl.exe
LINK32=link.exe -lib
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /X /I "..\..\..\include" /I "..\..\..\include\window" /I "..\..\..\include\project" /I "..\..\..\include\sys" /I "..\..\..\include\image" /I "..\..\..\include\setup" /I "..\..\..\include\public" /I "..\..\..\include\me" /I "..\..\..\include\ab" /I "..\..\..\include\callapp" /I "..\..\..\include\bluetooth" /I "..\..\..\include\IVT\stack" /I "..\..\..\include\IVT\sdk" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_DEBUG" /D "_EMULATE_" /D "_863BIRD_" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"Debug/APP_Pim.bsc"
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\..\lib\pim.lib"
# Begin Target

# Name "APP_Pim - Win32 Debug"
# Begin Group "Notepad"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\notepad\NoteBrowser.c
# End Source File
# Begin Source File

SOURCE=.\notepad\NoteBrowser.h
# End Source File
# Begin Source File

SOURCE=.\notepad\Notepad.c
# End Source File
# Begin Source File

SOURCE=.\notepad\Notepad.h
# End Source File
# Begin Source File

SOURCE=.\notepad\TextFile.c
# End Source File
# Begin Source File

SOURCE=.\notepad\TextFile.h
# End Source File
# End Group
# Begin Group "Calculator"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\calc\CalcFunc.c
# End Source File
# Begin Source File

SOURCE=.\calc\CalcFunc.h
# End Source File
# Begin Source File

SOURCE=.\calc\CalcMain.c
# End Source File
# Begin Source File

SOURCE=.\calc\CalcMain.h
# End Source File
# End Group
# Begin Group "exch"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\exch\exch.c
# End Source File
# Begin Source File

SOURCE=.\exch\Exch.h
# End Source File
# Begin Source File

SOURCE=.\exch\exch_currandrate.c
# End Source File
# Begin Source File

SOURCE=.\exch\exch_currandrate.h
# End Source File
# Begin Source File

SOURCE=.\exch\exch_editvalue.c
# End Source File
# Begin Source File

SOURCE=.\exch\exch_editvalue.h
# End Source File
# Begin Source File

SOURCE=.\exch\exch_file.c
# End Source File
# Begin Source File

SOURCE=.\exch\exch_file.h
# End Source File
# Begin Source File

SOURCE=.\exch\exch_global.h
# End Source File
# Begin Source File

SOURCE=.\exch\exch_select.c
# End Source File
# Begin Source File

SOURCE=.\exch\exch_select.h
# End Source File
# Begin Source File

SOURCE=.\exch\exch_set.c
# End Source File
# Begin Source File

SOURCE=.\exch\exch_set.h
# End Source File
# End Group
# Begin Group "picbrowser"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\picbrowser\BrowsePic.c
# End Source File
# Begin Source File

SOURCE=.\picbrowser\PicBrowser.c
# End Source File
# Begin Source File

SOURCE=.\picbrowser\PicBrowser.h
# End Source File
# Begin Source File

SOURCE=.\picbrowser\piclistex.c
# End Source File
# Begin Source File

SOURCE=.\picbrowser\picmultilistex.c
# End Source File
# Begin Source File

SOURCE=.\picbrowser\ProcessDir.c
# End Source File
# Begin Source File

SOURCE=.\picbrowser\ProcessMultiSel.c
# End Source File
# End Group
# Begin Group "memmanage"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\memmanage\MemManage.c
# End Source File
# Begin Source File

SOURCE=.\memmanage\MemManage.h
# End Source File
# End Group
# Begin Group "favorite"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\favorite\favorite.h
# End Source File
# Begin Source File

SOURCE=.\favorite\FavoriteMain.c
# End Source File
# End Group
# Begin Group "calendar"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\calendar\CaleAlarm.c
# End Source File
# Begin Source File

SOURCE=.\calendar\CaleAlarm.h
# End Source File
# Begin Source File

SOURCE=.\calendar\CaleBrowse.c
# End Source File
# Begin Source File

SOURCE=.\calendar\CaleBrowse.h
# End Source File
# Begin Source File

SOURCE=.\calendar\CaleComFunction.c
# End Source File
# Begin Source File

SOURCE=.\calendar\CaleComFunction.h
# End Source File
# Begin Source File

SOURCE=.\calendar\CaleConfig.c
# End Source File
# Begin Source File

SOURCE=.\calendar\CaleDayView.c
# End Source File
# Begin Source File

SOURCE=.\calendar\CaleDefine.h
# End Source File
# Begin Source File

SOURCE=.\calendar\CaleEntry.c
# End Source File
# Begin Source File

SOURCE=.\calendar\CaleGlobal.h
# End Source File
# Begin Source File

SOURCE=.\calendar\CaleHeader.h
# End Source File
# Begin Source File

SOURCE=.\calendar\CaleMonthView.c
# End Source File
# Begin Source File

SOURCE=.\calendar\CaleSaveApi.c
# End Source File
# Begin Source File

SOURCE=.\calendar\CaleSaveApi.h
# End Source File
# Begin Source File

SOURCE=.\calendar\CaleString.h
# End Source File
# Begin Source File

SOURCE=.\calendar\CaleStruct.h
# End Source File
# Begin Source File

SOURCE=.\calendar\CaleWeekView.c
# End Source File
# Begin Source File

SOURCE=.\calendar\vCalendar.c
# End Source File
# End Group
# Begin Group "clock"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\clock\AlmConfigure.c
# End Source File
# Begin Source File

SOURCE=.\clock\Almglobal.h
# End Source File
# Begin Source File

SOURCE=.\clock\AlmingWnd.c
# End Source File
# Begin Source File

SOURCE=.\clock\AlmMain.c
# End Source File
# Begin Source File

SOURCE=.\clock\AlmMain.h
# End Source File
# Begin Source File

SOURCE=.\clock\AlmSet.c
# End Source File
# Begin Source File

SOURCE=.\clock\AlmSet.h
# End Source File
# Begin Source File

SOURCE=.\clock\AlmSet_detail.c
# End Source File
# Begin Source File

SOURCE=.\clock\clock.c
# End Source File
# Begin Source File

SOURCE=.\clock\clock.h
# End Source File
# Begin Source File

SOURCE=.\clock\clock_timezone.c
# End Source File
# Begin Source File

SOURCE=.\clock\clock_timezone.h
# End Source File
# End Group
# Begin Group "sndmanage"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\sndmanage\SndExtern.c
# End Source File
# Begin Source File

SOURCE=.\sndmanage\SndManage.c
# End Source File
# Begin Source File

SOURCE=.\sndmanage\SndManage.h
# End Source File
# Begin Source File

SOURCE=.\sndmanage\SndMultiSel.c
# End Source File
# Begin Source File

SOURCE=.\sndmanage\SndString.h
# End Source File
# Begin Source File

SOURCE=.\sndmanage\SndUsrApi.c
# End Source File
# Begin Source File

SOURCE=.\sndmanage\SndUsrApi.h
# End Source File
# Begin Source File

SOURCE=.\sndmanage\SoundDir.c
# End Source File
# End Group
# End Target
# End Project
