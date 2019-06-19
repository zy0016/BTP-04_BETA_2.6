# Microsoft Developer Studio Project File - Name="display" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=display - Win32 RGB12_RGB32_Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "display.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "display.mak" CFG="display - Win32 RGB12_RGB32_Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "display - Win32 RGB12_Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "display - Win32 RGB16_Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "display - Win32 RGB24_Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "display - Win32 RGB32_Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "display - Win32 MONO_RGB16_Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "display - Win32 MONO_RGB32_Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "display - Win32 MONO_RGB12_RGB16_Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "display - Win32 RGB12_RGB32_Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "display - Win32 RGB12_Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "display___Win32_RGB12_Debug"
# PROP BASE Intermediate_Dir "display___Win32_RGB12_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "display___Win32_RGB12_Debug"
# PROP Intermediate_Dir "display___Win32_RGB12_Debug"
# PROP Target_Dir ""
LINK32=link.exe -lib
# ADD BASE CPP /nologo /Zp16 /MDd /W4 /GX /ZI /Od /X /I "../../../../include" /I "../../../../../include" /D "EMULATION" /D "_EMULATE_" /D "WIN32" /D "_DEBUG" /FR /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /Zp16 /MDd /W3 /GX /ZI /Od /X /I "..\..\..\sysinc" /I "..\..\..\..\include" /I "..\..\..\include" /I "..\..\src\include" /I "..\..\src\rgbcom" /D "EMULATION" /D "_EMULATE_" /D "WIN32" /D "_DEBUG" /D LDD_MODE=DDM_RGB12_4440 /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x804
# ADD RSC /l 0x804
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../lib/display.lib"
# ADD LIB32 /nologo /out:"../../../../lib/display.lib"

!ELSEIF  "$(CFG)" == "display - Win32 RGB16_Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "display___Win32_RGB16_Debug"
# PROP BASE Intermediate_Dir "display___Win32_RGB16_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "display___Win32_RGB16_Debug"
# PROP Intermediate_Dir "display___Win32_RGB16_Debug"
# PROP Target_Dir ""
LINK32=link.exe -lib
# ADD BASE CPP /nologo /Zp16 /MDd /W4 /GX /ZI /Od /X /I "../../../../include" /I "../../../../../include" /D "EMULATION" /D "_EMULATE_" /D "WIN32" /D "_DEBUG" /FR /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /Zp16 /MDd /W3 /GX /ZI /Od /X /I "..\..\..\sysinc" /I "..\..\..\..\include" /I "..\..\..\include" /I "..\..\src\include" /I "..\..\src\rgbcom" /D "EMULATION" /D "_EMULATE_" /D "WIN32" /D "_DEBUG" /D LDD_MODE=DDM_RGB16_565 /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x804
# ADD RSC /l 0x804
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../lib/display.lib"
# ADD LIB32 /nologo /out:"../../../../lib/display.lib"

!ELSEIF  "$(CFG)" == "display - Win32 RGB24_Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "display___Win32_RGB24_Debug"
# PROP BASE Intermediate_Dir "display___Win32_RGB24_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "display___Win32_RGB24_Debug"
# PROP Intermediate_Dir "display___Win32_RGB24_Debug"
# PROP Target_Dir ""
LINK32=link.exe -lib
# ADD BASE CPP /nologo /Zp16 /MDd /W4 /GX /ZI /Od /X /I "../../../../include" /I "../../../../../include" /D "EMULATION" /D "_EMULATE_" /D "WIN32" /D "_DEBUG" /FR /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /Zp16 /MDd /W3 /GX /ZI /Od /X /I "..\..\..\sysinc" /I "..\..\..\include" /I "..\..\src\include" /I "..\..\src\rgbcom" /D "EMULATION" /D "_EMULATE_" /D "WIN32" /D "_DEBUG" /D LDD_MODE=DDM_RGB18_BGR /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x804
# ADD RSC /l 0x804
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../lib/display.lib"
# ADD LIB32 /nologo /out:"../../../lib/display.lib"

!ELSEIF  "$(CFG)" == "display - Win32 RGB32_Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "display___Win32_RGB32_Debug"
# PROP BASE Intermediate_Dir "display___Win32_RGB32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "display___Win32_RGB32_Debug"
# PROP Intermediate_Dir "display___Win32_RGB32_Debug"
# PROP Target_Dir ""
LINK32=link.exe -lib
# ADD BASE CPP /nologo /Zp16 /MDd /W4 /GX /ZI /Od /X /I "../../../../include" /I "../../../../../include" /D "EMULATION" /D "_EMULATE_" /D "WIN32" /D "_DEBUG" /FR /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /Zp16 /MDd /W3 /GX /ZI /Od /X /I "..\..\..\sysinc" /I "..\..\..\include" /I "..\..\src\include" /I "..\..\src\rgbcom" /D "EMULATION" /D "_EMULATE_" /D "WIN32" /D "_DEBUG" /D LDD_MODE=DDM_RGB18_BGR0 /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x804
# ADD RSC /l 0x804
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../lib/display.lib"
# ADD LIB32 /nologo /out:"../../../lib/display.lib"

!ELSEIF  "$(CFG)" == "display - Win32 MONO_RGB16_Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "display___Win32_MONO_RGB16_Debug"
# PROP BASE Intermediate_Dir "display___Win32_MONO_RGB16_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "display___Win32_MONO_RGB16_Debug"
# PROP Intermediate_Dir "display___Win32_MONO_RGB16_Debug"
# PROP Target_Dir ""
LINK32=link.exe -lib
# ADD BASE CPP /nologo /Zp16 /MDd /W3 /GX /ZI /Od /X /I "..\..\..\sysinc" /I "..\..\..\include" /I "..\..\src\include" /D "EMULATION" /D "_EMULATE_" /D "WIN32" /D "_DEBUG" /D LDD_MODE=DDM_RGB18_BGR0 /FR /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /Zp16 /MDd /W3 /GX /ZI /Od /X /I "..\..\..\sysinc" /I "..\..\..\include" /I "..\..\src\include" /D "EMULATION" /D "_EMULATE_" /D "WIN32" /D "_DEBUG" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x804
# ADD RSC /l 0x804
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../../lib/display.lib"
# ADD LIB32 /nologo /out:"../../../lib/display.lib"

!ELSEIF  "$(CFG)" == "display - Win32 MONO_RGB32_Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "display___Win32_MONO_RGB32_Debug"
# PROP BASE Intermediate_Dir "display___Win32_MONO_RGB32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "display___Win32_MONO_RGB32_Debug"
# PROP Intermediate_Dir "display___Win32_MONO_RGB32_Debug"
# PROP Target_Dir ""
LINK32=link.exe -lib
# ADD BASE CPP /nologo /Zp16 /MDd /W3 /GX /ZI /Od /X /I "..\..\..\sysinc" /I "..\..\..\include" /I "..\..\src\include" /D "EMULATION" /D "_EMULATE_" /D "WIN32" /D "_DEBUG" /FR /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /Zp16 /MDd /W3 /GX /ZI /Od /X /I "..\..\..\sysinc" /I "..\..\..\include" /I "..\..\src\include" /D "EMULATION" /D "_EMULATE_" /D "WIN32" /D "_DEBUG" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x804
# ADD RSC /l 0x804
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../../lib/display.lib"
# ADD LIB32 /nologo /out:"../../../lib/display.lib"

!ELSEIF  "$(CFG)" == "display - Win32 MONO_RGB12_RGB16_Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "display___Win32_MONO_RGB12_RGB16_Debug"
# PROP BASE Intermediate_Dir "display___Win32_MONO_RGB12_RGB16_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "display___Win32_MONO_RGB12_RGB16_Debug"
# PROP Intermediate_Dir "display___Win32_MONO_RGB12_RGB16_Debug"
# PROP Target_Dir ""
LINK32=link.exe -lib
# ADD BASE CPP /nologo /Zp16 /MDd /W3 /GX /ZI /Od /X /I "..\..\..\sysinc" /I "..\..\..\include" /I "..\..\src\include" /D "EMULATION" /D "_EMULATE_" /D "WIN32" /D "_DEBUG" /FR /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /Zp16 /MDd /W3 /GX /ZI /Od /X /I "..\..\..\sysinc" /I "..\..\..\include" /I "..\..\src\include" /D "EMULATION" /D "_EMULATE_" /D "WIN32" /D "_DEBUG" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x804
# ADD RSC /l 0x804
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../../lib/display.lib"
# ADD LIB32 /nologo /out:"../../../lib/display.lib"

!ELSEIF  "$(CFG)" == "display - Win32 RGB12_RGB32_Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "display___Win32_RGB12_RGB32_Debug"
# PROP BASE Intermediate_Dir "display___Win32_RGB12_RGB32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "display___Win32_RGB12_RGB32_Debug"
# PROP Intermediate_Dir "display___Win32_RGB12_RGB32_Debug"
# PROP Target_Dir ""
LINK32=link.exe -lib
# ADD BASE CPP /nologo /Zp16 /MDd /W3 /GX /ZI /Od /X /I "..\..\..\sysinc" /I "..\..\..\include" /I "..\..\src\include" /I "..\..\src\rgbcom" /D "EMULATION" /D "_EMULATE_" /D "WIN32" /D "_DEBUG" /D LDD_MODE=DDM_RGB12_4440 /FR /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /Zp16 /MDd /W3 /GX /ZI /Od /X /I "..\..\..\sysinc" /I "..\..\..\include" /I "..\..\src\include" /D "EMULATION" /D "_EMULATE_" /D "WIN32" /D "_DEBUG" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x804
# ADD RSC /l 0x804
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../../lib/display.lib"
# ADD LIB32 /nologo /out:"../../../lib/display.lib"

!ENDIF 

# Begin Target

# Name "display - Win32 RGB12_Debug"
# Name "display - Win32 RGB16_Debug"
# Name "display - Win32 RGB24_Debug"
# Name "display - Win32 RGB32_Debug"
# Name "display - Win32 MONO_RGB16_Debug"
# Name "display - Win32 MONO_RGB32_Debug"
# Name "display - Win32 MONO_RGB12_RGB16_Debug"
# Name "display - Win32 RGB12_RGB32_Debug"
# Begin Group "RGB"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\RGB\RGB\dispconf.h

!IF  "$(CFG)" == "display - Win32 RGB12_Debug"

!ELSEIF  "$(CFG)" == "display - Win32 RGB16_Debug"

!ELSEIF  "$(CFG)" == "display - Win32 RGB24_Debug"

!ELSEIF  "$(CFG)" == "display - Win32 RGB32_Debug"

!ELSEIF  "$(CFG)" == "display - Win32 MONO_RGB16_Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "display - Win32 MONO_RGB32_Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "display - Win32 MONO_RGB12_RGB16_Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "display - Win32 RGB12_RGB32_Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\src\RGB\display.c

!IF  "$(CFG)" == "display - Win32 RGB12_Debug"

!ELSEIF  "$(CFG)" == "display - Win32 RGB16_Debug"

!ELSEIF  "$(CFG)" == "display - Win32 RGB24_Debug"

!ELSEIF  "$(CFG)" == "display - Win32 RGB32_Debug"

!ELSEIF  "$(CFG)" == "display - Win32 MONO_RGB16_Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "display - Win32 MONO_RGB32_Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "display - Win32 MONO_RGB12_RGB16_Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "display - Win32 RGB12_RGB32_Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\src\RGB\RGB\rgb.c

!IF  "$(CFG)" == "display - Win32 RGB12_Debug"

!ELSEIF  "$(CFG)" == "display - Win32 RGB16_Debug"

!ELSEIF  "$(CFG)" == "display - Win32 RGB24_Debug"

!ELSEIF  "$(CFG)" == "display - Win32 RGB32_Debug"

!ELSEIF  "$(CFG)" == "display - Win32 MONO_RGB16_Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "display - Win32 MONO_RGB32_Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "display - Win32 MONO_RGB12_RGB16_Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "display - Win32 RGB12_RGB32_Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "MONO_RGB"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\MONO_RGB\MONO\dispco1.h

!IF  "$(CFG)" == "display - Win32 RGB12_Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "display - Win32 RGB16_Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "display - Win32 RGB24_Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "display - Win32 RGB32_Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "display - Win32 MONO_RGB16_Debug"

!ELSEIF  "$(CFG)" == "display - Win32 MONO_RGB32_Debug"

!ELSEIF  "$(CFG)" == "display - Win32 MONO_RGB12_RGB16_Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "display - Win32 RGB12_RGB32_Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\src\MONO_RGB\RGB\dispconf.h

!IF  "$(CFG)" == "display - Win32 RGB12_Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "display - Win32 RGB16_Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "display - Win32 RGB24_Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "display - Win32 RGB32_Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "display - Win32 MONO_RGB16_Debug"

!ELSEIF  "$(CFG)" == "display - Win32 MONO_RGB32_Debug"

!ELSEIF  "$(CFG)" == "display - Win32 MONO_RGB12_RGB16_Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "display - Win32 RGB12_RGB32_Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\src\MONO_RGB\display.c

!IF  "$(CFG)" == "display - Win32 RGB12_Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "display - Win32 RGB16_Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "display - Win32 RGB24_Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "display - Win32 RGB32_Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "display - Win32 MONO_RGB16_Debug"

# ADD CPP /D LDD_MODE=DDM_RGB16_565

!ELSEIF  "$(CFG)" == "display - Win32 MONO_RGB32_Debug"

# ADD CPP /D LDD_MODE=DDM_RGB18_BGR0

!ELSEIF  "$(CFG)" == "display - Win32 MONO_RGB12_RGB16_Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "display - Win32 RGB12_RGB32_Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\src\MONO_RGB\MONO\mono.c

!IF  "$(CFG)" == "display - Win32 RGB12_Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "display - Win32 RGB16_Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "display - Win32 RGB24_Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "display - Win32 RGB32_Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "display - Win32 MONO_RGB16_Debug"

# ADD CPP /I "..\..\src\monocom"

!ELSEIF  "$(CFG)" == "display - Win32 MONO_RGB32_Debug"

# ADD CPP /I "..\..\src\monocom"

!ELSEIF  "$(CFG)" == "display - Win32 MONO_RGB12_RGB16_Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "display - Win32 RGB12_RGB32_Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\src\MONO_RGB\RGB\rgb.c

!IF  "$(CFG)" == "display - Win32 RGB12_Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "display - Win32 RGB16_Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "display - Win32 RGB24_Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "display - Win32 RGB32_Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "display - Win32 MONO_RGB16_Debug"

# ADD CPP /I "..\..\src\rgbcom" /D LDD_MODE=DDM_RGB16_565

!ELSEIF  "$(CFG)" == "display - Win32 MONO_RGB32_Debug"

# ADD CPP /I "..\..\src\rgbcom" /D LDD_MODE=DDM_RGB18_BGR0

!ELSEIF  "$(CFG)" == "display - Win32 MONO_RGB12_RGB16_Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "display - Win32 RGB12_RGB32_Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "MONO_RGB12_RGB16"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\MONO_RGB12_RGB16\MONO\dispco1.h

!IF  "$(CFG)" == "display - Win32 RGB12_Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "display - Win32 RGB16_Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "display - Win32 RGB24_Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "display - Win32 RGB32_Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "display - Win32 MONO_RGB16_Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "display - Win32 MONO_RGB32_Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "display - Win32 MONO_RGB12_RGB16_Debug"

!ELSEIF  "$(CFG)" == "display - Win32 RGB12_RGB32_Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\src\MONO_RGB12_RGB16\RGB\dispco16.h

!IF  "$(CFG)" == "display - Win32 RGB12_Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "display - Win32 RGB16_Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "display - Win32 RGB24_Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "display - Win32 RGB32_Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "display - Win32 MONO_RGB16_Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "display - Win32 MONO_RGB32_Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "display - Win32 MONO_RGB12_RGB16_Debug"

!ELSEIF  "$(CFG)" == "display - Win32 RGB12_RGB32_Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\src\MONO_RGB12_RGB16\display.c

!IF  "$(CFG)" == "display - Win32 RGB12_Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "display - Win32 RGB16_Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "display - Win32 RGB24_Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "display - Win32 RGB32_Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "display - Win32 MONO_RGB16_Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "display - Win32 MONO_RGB32_Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "display - Win32 MONO_RGB12_RGB16_Debug"

!ELSEIF  "$(CFG)" == "display - Win32 RGB12_RGB32_Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\src\MONO_RGB12_RGB16\MONO\mono.c

!IF  "$(CFG)" == "display - Win32 RGB12_Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "display - Win32 RGB16_Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "display - Win32 RGB24_Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "display - Win32 RGB32_Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "display - Win32 MONO_RGB16_Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "display - Win32 MONO_RGB32_Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "display - Win32 MONO_RGB12_RGB16_Debug"

# ADD CPP /I "..\..\src\monocom"

!ELSEIF  "$(CFG)" == "display - Win32 RGB12_RGB32_Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\src\MONO_RGB12_RGB16\RGB\rgb.c

!IF  "$(CFG)" == "display - Win32 RGB12_Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "display - Win32 RGB16_Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "display - Win32 RGB24_Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "display - Win32 RGB32_Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "display - Win32 MONO_RGB16_Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "display - Win32 MONO_RGB32_Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "display - Win32 MONO_RGB12_RGB16_Debug"

# ADD CPP /I "..\..\src\rgbcom"

!ELSEIF  "$(CFG)" == "display - Win32 RGB12_RGB32_Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "RGB12_RGB18"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\RGB12_RGB18\RGB\dispco12.h

!IF  "$(CFG)" == "display - Win32 RGB12_Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "display - Win32 RGB16_Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "display - Win32 RGB24_Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "display - Win32 RGB32_Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "display - Win32 MONO_RGB16_Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "display - Win32 MONO_RGB32_Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "display - Win32 MONO_RGB12_RGB16_Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "display - Win32 RGB12_RGB32_Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\src\RGB12_RGB18\RGB\dispco18.h

!IF  "$(CFG)" == "display - Win32 RGB12_Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "display - Win32 RGB16_Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "display - Win32 RGB24_Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "display - Win32 RGB32_Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "display - Win32 MONO_RGB16_Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "display - Win32 MONO_RGB32_Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "display - Win32 MONO_RGB12_RGB16_Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "display - Win32 RGB12_RGB32_Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\src\RGB12_RGB18\display.c

!IF  "$(CFG)" == "display - Win32 RGB12_Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "display - Win32 RGB16_Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "display - Win32 RGB24_Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "display - Win32 RGB32_Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "display - Win32 MONO_RGB16_Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "display - Win32 MONO_RGB32_Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "display - Win32 MONO_RGB12_RGB16_Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "display - Win32 RGB12_RGB32_Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\src\RGB12_RGB18\RGB\rgb12.c

!IF  "$(CFG)" == "display - Win32 RGB12_Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "display - Win32 RGB16_Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "display - Win32 RGB24_Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "display - Win32 RGB32_Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "display - Win32 MONO_RGB16_Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "display - Win32 MONO_RGB32_Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "display - Win32 MONO_RGB12_RGB16_Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "display - Win32 RGB12_RGB32_Debug"

# ADD CPP /I "..\..\src\rgbcom"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\src\RGB12_RGB18\RGB\rgb18.c

!IF  "$(CFG)" == "display - Win32 RGB12_Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "display - Win32 RGB16_Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "display - Win32 RGB24_Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "display - Win32 RGB32_Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "display - Win32 MONO_RGB16_Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "display - Win32 MONO_RGB32_Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "display - Win32 MONO_RGB12_RGB16_Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "display - Win32 RGB12_RGB32_Debug"

# ADD CPP /I "..\..\src\rgbcom"

!ENDIF 

# End Source File
# End Group
# Begin Group "common"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\common\dispcomn.c
# End Source File
# End Group
# Begin Group "include"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\include\dispcomn.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\display.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\disppdd.h
# End Source File
# Begin Source File

SOURCE=..\..\src\include\hpdisp.h
# End Source File
# Begin Source File

SOURCE=..\..\src\include\pattern.h
# End Source File
# End Group
# Begin Group "os"

# PROP Default_Filter ""
# End Group
# Begin Group "rgbcom"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\rgbcom\bitblt.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\src\rgbcom\bltnto16.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\src\rgbcom\bltnto24.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\src\rgbcom\circle.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\src\rgbcom\cursor.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\src\rgbcom\dddline.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\src\rgbcom\dibitblt.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\src\rgbcom\dispfunc.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\src\rgbcom\distrblt.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\src\rgbcom\ellipse.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\src\rgbcom\line.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\src\rgbcom\patblt.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\src\rgbcom\polygon.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\src\rgbcom\screen.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\src\rgbcom\strblt.c
# PROP Exclude_From_Build 1
# End Source File
# End Group
# End Target
# End Project
