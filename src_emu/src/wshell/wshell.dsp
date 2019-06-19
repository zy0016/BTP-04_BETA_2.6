# Microsoft Developer Studio Project File - Name="wshell" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=wshell - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "wshell.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "wshell.mak" CFG="wshell - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "wshell - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe
# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "WSHELL_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /X /I "../../include" /I "../../include/hopen" /I "../../include/project" /I "../../include/window" /I "../../include/image" /I "../../include/me" /I "../../include/setup" /I "../../include/ab" /I "../../include/public" /D "DEBUG" /D "_EMULATE_" /D "EMULATION" /D "_DEBUG" /D "WIN32" /D "_DEBUG_" /Fr /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /stack:0x400000 /subsystem:windows /dll /map /debug /machine:I386 /nodefaultlib:"libclib" /nodefaultlib:"libcmt.lib" /nodefaultlib:"msvcrt.lib" /nodefaultlib:"libcd.lib" /nodefaultlib:"libcmtd.lib" /nodefaultlib:"libc.lib" /nodefaultlib /pdbtype:sept
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy .\debug\wshell.dll ..\..\apps
# End Special Build Tool
# Begin Target

# Name "wshell - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\App.c
# End Source File
# Begin Source File

SOURCE=.\main.c
# End Source File
# Begin Source File

SOURCE=.\str_public.c
# End Source File
# Begin Source File

SOURCE=.\temp.c
# End Source File
# Begin Source File

SOURCE=.\Testapp.c
# End Source File
# Begin Source File

SOURCE=.\Testapp.h
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "lib"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\lib\display.lib
# End Source File
# Begin Source File

SOURCE=..\..\lib\hopen30.lib
# End Source File
# Begin Source File

SOURCE=..\..\lib\hp30emu.lib
# End Source File
# Begin Source File

SOURCE=..\..\lib\libcs.lib
# End Source File
# Begin Source File

SOURCE=..\..\lib\libm.lib
# End Source File
# Begin Source File

SOURCE=..\..\lib\MEdrv.lib
# End Source File
# Begin Source File

SOURCE=..\..\lib\MuLanguage.lib
# End Source File
# Begin Source File

SOURCE=..\..\lib\multiplexer.lib
# End Source File
# Begin Source File

SOURCE=..\..\lib\PLXzlib.lib
# End Source File
# Begin Source File

SOURCE=..\..\lib\Progman.lib
# End Source File
# Begin Source File

SOURCE=..\..\lib\public.lib
# End Source File
# Begin Source File

SOURCE=..\..\lib\window.lib
# End Source File
# Begin Source File

SOURCE=..\..\lib\Wap.lib
# End Source File
# Begin Source File

SOURCE=..\..\lib\tcpip.lib
# End Source File
# Begin Source File

SOURCE=..\..\lib\ssl.lib
# End Source File
# Begin Source File

SOURCE=..\..\lib\mms.lib
# End Source File
# Begin Source File

SOURCE=..\..\lib\cryptlib.lib
# End Source File
# Begin Source File

SOURCE=..\..\lib\whttp.lib
# End Source File
# Begin Source File

SOURCE=..\..\lib\app_call.lib
# End Source File
# Begin Source File

SOURCE=..\..\lib\smsdptch.lib
# End Source File
# Begin Source File

SOURCE=..\..\lib\imm.lib
# End Source File
# Begin Source File

SOURCE=..\..\lib\app_message.lib
# End Source File
# Begin Source File

SOURCE=..\..\lib\Dialer.lib
# End Source File
# Begin Source File

SOURCE=..\..\lib\setup.lib
# End Source File
# Begin Source File

SOURCE=..\..\lib\trafficlog.lib
# End Source File
# Begin Source File

SOURCE=..\..\lib\game.lib
# End Source File
# Begin Source File

SOURCE=..\..\lib\pim.lib
# End Source File
# Begin Source File

SOURCE=..\..\lib\registry.lib
# End Source File
# Begin Source File

SOURCE=..\..\lib\app_addressbook.lib
# End Source File
# Begin Source File

SOURCE=..\..\lib\dlm.lib
# End Source File
# Begin Source File

SOURCE=..\..\lib\BrsParser.lib
# End Source File
# Begin Source File

SOURCE=..\..\lib\ftpsvr.lib
# End Source File
# Begin Source File

SOURCE=..\..\lib\help.lib
# End Source File
# Begin Source File

SOURCE="..\..\lib\CPHS application.lib"
# End Source File
# Begin Source File

SOURCE=..\..\lib\objdebug.lib
# End Source File
# Begin Source File

SOURCE=..\..\lib\hpimage.lib
# End Source File
# Begin Source File

SOURCE=..\..\lib\mail.lib
# End Source File
# Begin Source File

SOURCE=..\..\lib\vCal.lib
# End Source File
# Begin Source File

SOURCE=..\..\lib\Brskernel.lib
# End Source File
# Begin Source File

SOURCE=..\..\lib\Bluetooth.lib
# End Source File
# Begin Source File

SOURCE=..\..\lib\IVT\btstack.lib
# End Source File
# Begin Source File

SOURCE=..\..\lib\IVT\btprofile.lib
# End Source File
# Begin Source File

SOURCE=..\..\lib\IVT\btsdk.lib
# End Source File
# Begin Source File

SOURCE=..\..\lib\IVT\btblock.lib
# End Source File
# Begin Source File

SOURCE=..\..\lib\benefon\LocationModule.lib
# End Source File
# Begin Source File

SOURCE=..\..\lib\zi8clib.lib
# End Source File
# Begin Source File

SOURCE=..\..\lib\benefon\GPSMonitor.lib
# End Source File
# Begin Source File

SOURCE=..\..\lib\prioman.lib
# End Source File
# Begin Source File

SOURCE=..\..\lib\plxmm.lib
# End Source File
# End Group
# End Target
# End Project
