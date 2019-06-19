# Microsoft Developer Studio Generated NMAKE File, Based on dlm.dsp
!IF "$(CFG)" == ""
CFG=dlm - Win32 Debug
!MESSAGE No configuration specified. Defaulting to dlm - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "dlm - Win32 Release" && "$(CFG)" != "dlm - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "dlm.mak" CFG="dlm - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "dlm - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "dlm - Win32 Debug" (based on "Win32 (x86) Static Library")
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

!IF  "$(CFG)" == "dlm - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\dlm.lib"


CLEAN :
	-@erase "$(INTDIR)\dlmui.obj"
	-@erase "$(INTDIR)\funclistarray.obj"
	-@erase "$(INTDIR)\loader.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\dlm.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

vvRSC=rc.exe
MTL=midl.exe
LINK32=link.exe -lib
CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Fp"$(INTDIR)\dlm.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\dlm.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\dlm.lib" 
LIB32_OBJS= \
	"$(INTDIR)\loader.obj" \
	"$(INTDIR)\dlmui.obj" \
	"$(INTDIR)\funclistarray.obj"

"$(OUTDIR)\dlm.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "dlm - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\..\lib\dlm.lib"


CLEAN :
	-@erase "$(INTDIR)\dlmui.obj"
	-@erase "$(INTDIR)\funclistarray.obj"
	-@erase "$(INTDIR)\loader.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "..\..\lib\dlm.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

vvRSC=rc.exe
MTL=midl.exe
LINK32=link.exe -lib
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /ZI /Od /I "..\..\include" /I "..\..\include\window" /I "..\..\include\project" /I "..\..\include\image" /I "..\..\include\sys" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Fp"$(INTDIR)\dlm.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ  /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\dlm.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\..\lib\dlm.lib" 
LIB32_OBJS= \
	"$(INTDIR)\loader.obj" \
	"$(INTDIR)\dlmui.obj" \
	"$(INTDIR)\funclistarray.obj"

"..\..\lib\dlm.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 

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
!IF EXISTS("dlm.dep")
!INCLUDE "dlm.dep"
!ELSE 
!MESSAGE Warning: cannot find "dlm.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "dlm - Win32 Release" || "$(CFG)" == "dlm - Win32 Debug"
SOURCE=.\dlmui.c

"$(INTDIR)\dlmui.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\funclistarray.c

"$(INTDIR)\funclistarray.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\loader.c

"$(INTDIR)\loader.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 

