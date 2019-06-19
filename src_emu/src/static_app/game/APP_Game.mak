# Microsoft Developer Studio Generated NMAKE File, Based on APP_Game.dsp
!IF "$(CFG)" == ""
CFG=APP_Game - Win32 Debug
!MESSAGE No configuration specified. Defaulting to APP_Game - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "APP_Game - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "APP_Game.mak" CFG="APP_Game - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "APP_Game - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
RSC=rc.exe
OUTDIR=.
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.
# End Custom Macros

ALL : "..\..\..\lib\game.lib" "$(OUTDIR)\Debug\APP_Game.bsc"


CLEAN :
	-@erase "$(INTDIR)\bitboard.obj"
	-@erase "$(INTDIR)\board.obj"
	-@erase "$(INTDIR)\DepBomb.obj"
	-@erase "$(INTDIR)\DepBomb.sbr"
	-@erase "$(INTDIR)\Diamond.obj"
	-@erase "$(INTDIR)\game5.obj"
	-@erase "$(INTDIR)\pao.obj"
	-@erase "$(INTDIR)\Poker.obj"
	-@erase "$(INTDIR)\PushBox.obj"
	-@erase "$(INTDIR)\robot.obj"
	-@erase "$(INTDIR)\SUPER5.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\Debug\APP_Game.bsc"
	-@erase "..\..\..\lib\game.lib"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LINK32=link.exe -lib
MTL=midl.exe
vvRSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Debug\APP_Game.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\DepBomb.sbr"

"$(OUTDIR)\Debug\APP_Game.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\..\..\lib\game.lib" 
LIB32_OBJS= \
	"$(INTDIR)\Poker.obj" \
	"$(INTDIR)\PushBox.obj" \
	"$(INTDIR)\DepBomb.obj" \
	"$(INTDIR)\pao.obj" \
	"$(INTDIR)\bitboard.obj" \
	"$(INTDIR)\board.obj" \
	"$(INTDIR)\game5.obj" \
	"$(INTDIR)\robot.obj" \
	"$(INTDIR)\SUPER5.obj" \
	"$(INTDIR)\Diamond.obj"

"..\..\..\lib\game.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

CPP_PROJ=/nologo /MLd /W3 /Gm /GX /ZI /Od /X /I "..\..\..\include" /I "..\..\..\include\public" /I "..\..\..\include\window" /I "..\..\..\include\image" /I "..\..\..\include\project" /I "..\..\..\include\me" /I "..\..\public\pub_control" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_DEBUG" /D "_EMULATE_" /D "_863BIRD_" /Fp"$(INTDIR)\APP_Game.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("APP_Game.dep")
!INCLUDE "APP_Game.dep"
!ELSE 
!MESSAGE Warning: cannot find "APP_Game.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "APP_Game - Win32 Debug"
SOURCE=.\poker\Poker.c

"$(INTDIR)\Poker.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\pushbox\PushBox.c

"$(INTDIR)\PushBox.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\DepBomb\DepBomb.c
CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /X /I "..\..\..\include" /I "..\..\..\include\public" /I "..\..\..\include\window" /I "..\..\..\include\image" /I "..\..\..\include\project" /I "..\..\..\include\me" /I "..\..\public\pub_control" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_DEBUG" /D "_EMULATE_" /D "_863BIRD_" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\APP_Game.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\DepBomb.obj"	"$(INTDIR)\DepBomb.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


SOURCE=.\paopao\pao.c

"$(INTDIR)\pao.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\Gobang\bitboard.c

"$(INTDIR)\bitboard.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\Gobang\board.c

"$(INTDIR)\board.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\Gobang\game5.c

"$(INTDIR)\game5.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\Gobang\robot.c

"$(INTDIR)\robot.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\Gobang\SUPER5.c

"$(INTDIR)\SUPER5.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\diamond\Diamond.c

"$(INTDIR)\Diamond.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

