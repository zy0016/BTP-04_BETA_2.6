# Microsoft Developer Studio Project File - Name="APP_Game" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=APP_Game - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "APP_Game.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "APP_Game.mak" CFG="APP_Game - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "APP_Game - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/¿ª·¢Çø/src_emu/src_me/static_app/game", MJBAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe
# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "APP_Game___Win32_Debug"
# PROP BASE Intermediate_Dir "APP_Game___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ""
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
LINK32=link.exe -lib
MTL=midl.exe
vvRSC=rc.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /X /I "..\..\..\include" /I "..\..\..\include\public" /I "..\..\..\include\window" /I "..\..\..\include\image" /I "..\..\..\include\project" /I "..\..\..\include\me" /I "..\..\public\pub_control" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_DEBUG" /D "_EMULATE_" /D "_863BIRD_" /YX /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
# SUBTRACT RSC /x
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"Debug/APP_Game.bsc"
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\..\lib\game.lib"
# Begin Target

# Name "APP_Game - Win32 Debug"
# Begin Group "poker"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\poker\Poker.c
# End Source File
# Begin Source File

SOURCE=.\poker\Poker.h
# End Source File
# End Group
# Begin Group "pushbox"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\pushbox\PushBox.c
# End Source File
# Begin Source File

SOURCE=.\pushbox\PushBox.h
# End Source File
# Begin Source File

SOURCE=.\pushbox\PushBoxrs.h
# End Source File
# End Group
# Begin Group "DepBomb"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DepBomb\BombRs.h
# End Source File
# Begin Source File

SOURCE=.\DepBomb\DepBomb.c
# ADD CPP /FR
# SUBTRACT CPP /WX /O<none>
# End Source File
# Begin Source File

SOURCE=.\DepBomb\DepBomb.h
# End Source File
# End Group
# Begin Group "paopao"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\paopao\pao.c
# End Source File
# Begin Source File

SOURCE=.\paopao\pao.h
# End Source File
# Begin Source File

SOURCE=.\paopao\paors.h
# End Source File
# End Group
# Begin Group "GoBang"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Gobang\bitboard.c
# End Source File
# Begin Source File

SOURCE=.\Gobang\board.c
# End Source File
# Begin Source File

SOURCE=.\Gobang\game5.c
# End Source File
# Begin Source File

SOURCE=.\Gobang\resource.h
# End Source File
# Begin Source File

SOURCE=.\Gobang\robot.c
# End Source File
# Begin Source File

SOURCE=.\Gobang\SUPER5.c
# End Source File
# Begin Source File

SOURCE=.\Gobang\super5.h
# End Source File
# End Group
# Begin Group "diamond"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\diamond\Diamond.c
# End Source File
# Begin Source File

SOURCE=.\diamond\Diamond.h
# End Source File
# Begin Source File

SOURCE=.\diamond\DiamonRs.h
# End Source File
# End Group
# End Target
# End Project
