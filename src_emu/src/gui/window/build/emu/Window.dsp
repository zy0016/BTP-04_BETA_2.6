# Microsoft Developer Studio Project File - Name="Window" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=Window - Win32 ST_Mobile Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Window.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Window.mak" CFG="Window - Win32 ST_Mobile Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Window - Win32 ST Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "Window - Win32 MT Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "Window - Win32 ST_Mobile Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Window - Win32 ST Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Window___Win32_ST_Debug"
# PROP BASE Intermediate_Dir "Window___Win32_ST_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Window___Win32_ST_Debug"
# PROP Intermediate_Dir "Window___Win32_ST_Debug"
# PROP Target_Dir ""
LINK32=link.exe -lib
MTL=midl.exe
vvRSC=rc.exe
# ADD BASE CPP /nologo /Zp16 /W4 /Gm /Gi /GX /ZI /Od /X /I "../../src" /I "../../src/include" /I "../../../../include" /I "../../../include" /D "_WINDOWS" /D "_MBCS" /D "WIN32" /D "_DEBUG" /D "_EMULATE_" /D __ST_PLX_GUI=1 /FR /YX /FD /GZ /c
# ADD CPP /nologo /Zp16 /W4 /Gm /Gi /GX /ZI /Od /I "../../src" /I "../../src/include" /I "../../../include" /I "../../../../../include" /I "../../../../../include/window" /I "../../../../../include/project" /I "../../../../../include/image" /D "_WINDOWS" /D "_MBCS" /D "WIN32" /D "_DEBUG" /D "_EMULATE_" /D __ST_PLX_GUI=1 /D "PIP_TRANS" /FR /YX /FD /GZ /c
# SUBTRACT CPP /X
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../../../lib/window.lib"
# ADD LIB32 /nologo /out:"../../../../../lib/window.lib"

!ELSEIF  "$(CFG)" == "Window - Win32 MT Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Window___Win32_MT_Debug"
# PROP BASE Intermediate_Dir "Window___Win32_MT_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Window___Win32_MT_Debug"
# PROP Intermediate_Dir "Window___Win32_MT_Debug"
# PROP Target_Dir ""
LINK32=link.exe -lib
MTL=midl.exe
vvRSC=rc.exe
# ADD BASE CPP /nologo /Zp16 /W4 /Gm /Gi /GX /ZI /Od /X /I "../../src" /I "../../src/include" /I "../../../../include" /I "../../../include" /D "_WINDOWS" /D "_MBCS" /D "WIN32" /D "_DEBG" /D "_EMULATE_" /D __ST_PLX_GUI=1 /FR /YX /FD /GZ /c
# ADD CPP /nologo /Zp16 /W4 /Gm /Gi /GX /ZI /Od /I "../../src" /I "../../src/include" /I "../../../include" /I "../../../../../include" /I "../../../../../include/window" /I "../../../../../include/project" /I "../../../../../include/image" /D "_WINDOWS" /D "_MBCS" /D "WIN32" /D "_DEBUG" /D "_EMULATE_" /D __MT_PLX_GUI=1 /FR /YX /FD /GZ /c
# SUBTRACT CPP /X
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../../../lib/window.lib"
# ADD LIB32 /nologo /out:"../../../../lib/window.lib"

!ELSEIF  "$(CFG)" == "Window - Win32 ST_Mobile Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Window___Win32_ST_Mobile_Debug"
# PROP BASE Intermediate_Dir "Window___Win32_ST_Mobile_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Window___Win32_ST_Mobile_Debug"
# PROP Intermediate_Dir "Window___Win32_ST_Mobile_Debug"
# PROP Target_Dir ""
LINK32=link.exe -lib
MTL=midl.exe
vvRSC=rc.exe
# ADD BASE CPP /nologo /Zp16 /W4 /Gm /Gi /GX /ZI /Od /X /I "../../src" /I "../../src/include" /I "../../../../include" /I "../../../include" /D "_WINDOWS" /D "_MBCS" /D "WIN32" /D "_DEBUG" /D "_EMULATE_" /D __ST_PLX_GUI=1 /FR /YX /FD /GZ /c
# ADD CPP /nologo /Zp16 /W4 /Gm /Gi /GX /ZI /Od /I "../../src" /I "../../src/include" /I "../../../include" /I "../../../../../include" /I "../../../../../include/window" /I "../../../../../include/image" /I "../../../../../include/project" /D "_WINDOWS" /D "_MBCS" /D "WIN32" /D "_DEBUG" /D "_EMULATE_" /D __ST_PLX_GUI=1 /D "PIP_TRANS" /FR /YX /FD /GZ /c
# SUBTRACT CPP /X
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../../../lib/window.lib"
# ADD LIB32 /nologo /out:"../../../../../lib/window.lib"

!ENDIF 

# Begin Target

# Name "Window - Win32 ST Debug"
# Name "Window - Win32 MT Debug"
# Name "Window - Win32 ST_Mobile Debug"
# Begin Group "kernel"

# PROP Default_Filter "cpp;c;cxx;rc;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\src\kernel\apigdi.c
# End Source File
# Begin Source File

SOURCE=..\..\src\kernel\apirgn.c
# End Source File
# Begin Source File

SOURCE=..\..\src\kernel\clstbl.c
# End Source File
# Begin Source File

SOURCE=..\..\src\kernel\clstbl.h
# End Source File
# Begin Source File

SOURCE=..\..\src\kernel\curobj.h
# End Source File
# Begin Source File

SOURCE=..\..\src\kernel\defwndbs.c
# End Source File
# Begin Source File

SOURCE=..\..\src\kernel\devdisp.c
# End Source File
# Begin Source File

SOURCE=..\..\src\kernel\devfont.c
# End Source File
# Begin Source File

SOURCE=..\..\src\kernel\devfont.h
# End Source File
# Begin Source File

SOURCE=..\..\src\kernel\devinput.c
# End Source File
# Begin Source File

SOURCE=..\..\src\kernel\devinput.h
# End Source File
# Begin Source File

SOURCE=..\..\src\kernel\devprn.c
# End Source File
# Begin Source File

SOURCE=..\..\src\kernel\devprn.h
# End Source File
# Begin Source File

SOURCE=..\..\src\kernel\devqueue.c
# End Source File
# Begin Source File

SOURCE=..\..\src\kernel\devqueue.h
# End Source File
# Begin Source File

SOURCE=..\..\src\kernel\error.c
# End Source File
# Begin Source File

SOURCE=..\..\src\kernel\graphdev.c
# End Source File
# Begin Source File

SOURCE=..\..\src\kernel\message.c
# End Source File
# Begin Source File

SOURCE=..\..\src\kernel\msgqueue.c
# End Source File
# Begin Source File

SOURCE=..\..\src\kernel\msgqueue.h
# End Source File
# Begin Source File

SOURCE=..\..\src\kernel\rectmgr.c
# End Source File
# Begin Source File

SOURCE=..\..\src\kernel\resmgr.c
# End Source File
# Begin Source File

SOURCE=..\..\src\kernel\stockobj.h
# End Source File
# Begin Source File

SOURCE=..\..\src\kernel\sysmetri.c
# End Source File
# Begin Source File

SOURCE=..\..\src\kernel\winclass.c
# End Source File
# Begin Source File

SOURCE=..\..\src\kernel\wincoord.c
# End Source File
# Begin Source File

SOURCE=..\..\src\kernel\winexpos.c
# End Source File
# Begin Source File

SOURCE=..\..\src\kernel\winmap.c
# End Source File
# Begin Source File

SOURCE=..\..\src\kernel\winmem.c
# End Source File
# Begin Source File

SOURCE=..\..\src\kernel\winobj.c
# End Source File
# Begin Source File

SOURCE=..\..\src\kernel\winpaint.c
# End Source File
# Begin Source File

SOURCE=..\..\src\kernel\winpos.c
# End Source File
# Begin Source File

SOURCE=..\..\src\kernel\winroot.c
# End Source File
# Begin Source File

SOURCE=..\..\src\kernel\wsapi.c
# End Source File
# Begin Source File

SOURCE=..\..\src\kernel\wscaret.c
# End Source File
# Begin Source File

SOURCE=..\..\src\kernel\wscaret.h
# End Source File
# Begin Source File

SOURCE=..\..\src\kernel\wsclipbo.c
# End Source File
# Begin Source File

SOURCE=..\..\src\kernel\wscursor.c
# End Source File
# Begin Source File

SOURCE=..\..\src\kernel\wscursor.h
# End Source File
# Begin Source File

SOURCE=..\..\src\kernel\wsgraph.c
# End Source File
# Begin Source File

SOURCE=..\..\src\kernel\wsgraph.h
# End Source File
# Begin Source File

SOURCE=..\..\src\kernel\wshook.c
# End Source File
# Begin Source File

SOURCE=..\..\src\kernel\wshook.h
# End Source File
# Begin Source File

SOURCE=..\..\src\kernel\wsinit.c
# End Source File
# Begin Source File

SOURCE=..\..\src\kernel\wsobj.h
# End Source File
# Begin Source File

SOURCE=..\..\src\kernel\wsobmp.c
# End Source File
# Begin Source File

SOURCE=..\..\src\kernel\wsobmp.h
# End Source File
# Begin Source File

SOURCE=..\..\src\kernel\wsobrush.c
# End Source File
# Begin Source File

SOURCE=..\..\src\kernel\wsobrush.h
# End Source File
# Begin Source File

SOURCE=..\..\src\kernel\wsocur.c
# End Source File
# Begin Source File

SOURCE=..\..\src\kernel\wsocur.h
# End Source File
# Begin Source File

SOURCE=..\..\src\kernel\wsodc.c
# End Source File
# Begin Source File

SOURCE=..\..\src\kernel\wsodc.h
# End Source File
# Begin Source File

SOURCE=..\..\src\kernel\wsofont.c
# End Source File
# Begin Source File

SOURCE=..\..\src\kernel\wsofont.h
# End Source File
# Begin Source File

SOURCE=..\..\src\kernel\wsoicon.h
# End Source File
# Begin Source File

SOURCE=..\..\src\kernel\wsopen.c
# End Source File
# Begin Source File

SOURCE=..\..\src\kernel\wsopen.h
# End Source File
# Begin Source File

SOURCE=..\..\src\kernel\wsorgn.c
# End Source File
# Begin Source File

SOURCE=..\..\src\kernel\wsorgn.h
# End Source File
# Begin Source File

SOURCE=..\..\src\kernel\wsotable.c
# End Source File
# Begin Source File

SOURCE=..\..\src\kernel\wsownd.h
# End Source File
# Begin Source File

SOURCE=..\..\src\kernel\wsthread.c
# End Source File
# Begin Source File

SOURCE=..\..\src\kernel\wstimer.c
# End Source File
# End Group
# Begin Group "include"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\include\devdisp.h
# End Source File
# Begin Source File

SOURCE=..\..\src\include\font.h
# End Source File
# Begin Source File

SOURCE=..\..\src\include\graphdev.h
# End Source File
# Begin Source File

SOURCE=..\..\src\include\hpfile.h
# End Source File
# Begin Source File

SOURCE=..\..\src\include\hpwin.h
# End Source File
# Begin Source File

SOURCE=..\..\src\include\importms.h
# End Source File
# Begin Source File

SOURCE=..\..\src\include\inputmgr.h
# End Source File
# Begin Source File

SOURCE=..\..\src\include\osal.h
# End Source File
# Begin Source File

SOURCE=..\..\src\include\warning.h
# End Source File
# Begin Source File

SOURCE=..\..\src\include\windowx.h
# End Source File
# Begin Source File

SOURCE=..\..\src\include\winmem.h
# End Source File
# Begin Source File

SOURCE=..\..\src\include\wsinit.h
# End Source File
# Begin Source File

SOURCE=..\..\src\include\wssysmsg.h
# End Source File
# Begin Source File

SOURCE=..\..\src\include\wsthread.h
# End Source File
# Begin Source File

SOURCE=..\..\src\include\wstimer.h
# End Source File
# End Group
# Begin Group "fontdrv"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\fontdrv\big5togb.h
# End Source File
# Begin Source File

SOURCE=..\..\src\fontdrv\fontdrv.c
# End Source File
# Begin Source File

SOURCE=..\..\src\fontdrv\fontdrv.h
# End Source File
# End Group
# Begin Group "main"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\main\wsapitbl.h
# End Source File
# Begin Source File

SOURCE=..\..\src\main\wsmain.c
# End Source File
# End Group
# Begin Group "ui"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\ui\apidlg.c
# End Source File
# Begin Source File

SOURCE=..\..\src\ui\apimenu.c
# End Source File
# Begin Source File

SOURCE=..\..\src\ui\apiscrol.c
# End Source File
# Begin Source File

SOURCE=..\..\src\ui\button.c
# End Source File
# Begin Source File

SOURCE=..\..\src\ui\colorscheme.c
# End Source File
# Begin Source File

SOURCE=..\..\src\ui\combobox.c
# End Source File
# Begin Source File

SOURCE=..\..\src\ui\control.h
# End Source File
# Begin Source File

SOURCE=..\..\src\ui\defdlg.c
# End Source File
# Begin Source File

SOURCE=..\..\src\ui\defdlg.h
# End Source File
# Begin Source File

SOURCE=..\..\src\ui\defwnd.c
# End Source File
# Begin Source File

SOURCE=..\..\src\ui\defwnd.h
# End Source File
# Begin Source File

SOURCE=..\..\src\ui\listbox.c
# End Source File
# Begin Source File

SOURCE=..\..\src\ui\menuobj.c
# End Source File
# Begin Source File

SOURCE=..\..\src\ui\menuobj.h
# End Source File
# Begin Source File

SOURCE=..\..\src\ui\menuwnd.c
# End Source File
# Begin Source File

SOURCE=..\..\src\ui\menuwnd.h
# End Source File
# Begin Source File

SOURCE=..\..\src\ui\mledit.c
# End Source File
# Begin Source File

SOURCE=..\..\src\ui\msg_res.h
# End Source File
# Begin Source File

SOURCE=..\..\src\ui\msgbox.c
# End Source File
# Begin Source File

SOURCE=..\..\src\ui\multilistbox.c
# End Source File
# Begin Source File

SOURCE=..\..\src\ui\numspin.c
# End Source File
# Begin Source File

SOURCE=..\..\src\ui\scroll.c
# End Source File
# Begin Source File

SOURCE=..\..\src\ui\static.c
# End Source File
# Begin Source File

SOURCE=..\..\src\ui\strspin.c
# End Source File
# Begin Source File

SOURCE=..\..\src\ui\wsui.c
# End Source File
# End Group
# Begin Group "printer"

# PROP Default_Filter ""
# End Group
# Begin Group "export"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\..\include\hp_diag.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\include\hpdef.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\include\osver.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\include\virtkey.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\include\windebug.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\include\Window.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\include\windowx.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\include\winpda.h
# End Source File
# End Group
# Begin Group "misc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\misc\codepage.c
# End Source File
# Begin Source File

SOURCE=..\..\src\misc\memory.c
# End Source File
# Begin Source File

SOURCE=..\..\src\misc\wstime.c
# End Source File
# End Group
# Begin Group "hgos"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\hgos\filesys.c
# End Source File
# Begin Source File

SOURCE=..\..\src\hgos\input20.c
# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\src\hgos\input30.c
# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\src\hgos\inputmgr.c
# End Source File
# Begin Source File

SOURCE=..\..\src\hgos\osal.c
# End Source File
# Begin Source File

SOURCE=..\..\src\hgos\profile.c
# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1
# End Source File
# End Group
# Begin Group "mobile"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\mobile\xscale\winpda.c
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\..\..\include\errno.h
# End Source File
# End Target
# End Project
