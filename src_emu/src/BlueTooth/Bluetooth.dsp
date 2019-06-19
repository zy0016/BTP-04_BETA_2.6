# Microsoft Developer Studio Project File - Name="Bluetooth" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=Bluetooth - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Bluetooth.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Bluetooth.mak" CFG="Bluetooth - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Bluetooth - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Bluetooth - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Bluetooth - Win32 Release"

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

!ELSEIF  "$(CFG)" == "Bluetooth - Win32 Debug"

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
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\..\include" /I "..\..\include\window" /I "..\..\include\me" /I "..\..\include\project" /I "..\..\include\image" /I "..\..\include\public" /I "..\..\include\sys" /I "..\..\include\setup" /I "..\..\include\IVT\sdk" /I "..\..\include\IVT\stack" /I "..\..\include\bluetooth" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "_EMULATE_" /YX /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\lib\Bluetooth.lib"

!ENDIF 

# Begin Target

# Name "Bluetooth - Win32 Release"
# Name "Bluetooth - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\BtApi.c
# End Source File
# Begin Source File

SOURCE=.\BtDelSelPaired.c
# End Source File
# Begin Source File

SOURCE=.\BtDeviceNode.c
# End Source File
# Begin Source File

SOURCE=.\BtFileOper.c
# End Source File
# Begin Source File

SOURCE=.\BtHandfreeHeadset.c
# End Source File
# Begin Source File

SOURCE=.\BtLowCall.c
# End Source File
# Begin Source File

SOURCE=.\BtMain.c
# End Source File
# Begin Source File

SOURCE=.\BtMsg.c
# End Source File
# Begin Source File

SOURCE=.\BtMsgInfo.c
# End Source File
# Begin Source File

SOURCE=.\BtPaired.c
# End Source File
# Begin Source File

SOURCE=.\BtPairedDevNode.c
# End Source File
# Begin Source File

SOURCE=.\BtPairedName.c
# End Source File
# Begin Source File

SOURCE=.\BtPasscode.c
# End Source File
# Begin Source File

SOURCE=.\BtPhoneName.c
# End Source File
# Begin Source File

SOURCE=.\BtSearchDevices.c
# End Source File
# Begin Source File

SOURCE=.\BtSendData.c
# End Source File
# Begin Source File

SOURCE=.\BtSettings.c
# End Source File
# Begin Source File

SOURCE=.\BtStatus.c
# End Source File
# Begin Source File

SOURCE=.\BtThread.c
# End Source File
# Begin Source File

SOURCE=.\BtViewMsg.c
# End Source File
# Begin Source File

SOURCE=.\BtVisibility.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\BtDeviceNode.h
# End Source File
# Begin Source File

SOURCE=.\BtFileOper.h
# End Source File
# Begin Source File

SOURCE=.\BtLowCall.h
# End Source File
# Begin Source File

SOURCE=.\BtMain.h
# End Source File
# Begin Source File

SOURCE=.\BtMsg.h
# End Source File
# Begin Source File

SOURCE=.\BtPairedDevNode.h
# End Source File
# Begin Source File

SOURCE=.\BtPasscode.h
# End Source File
# Begin Source File

SOURCE=.\BtString.h
# End Source File
# Begin Source File

SOURCE=.\BtThread.h
# End Source File
# End Group
# End Target
# End Project
