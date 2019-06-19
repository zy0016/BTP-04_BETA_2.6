# Microsoft Developer Studio Generated NMAKE File, Based on medrv.dsp
!IF "$(CFG)" == ""
CFG=MEdrv - Win32 Debug
!MESSAGE No configuration specified. Defaulting to MEdrv - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "MEdrv - Win32 Release" && "$(CFG)" != "MEdrv - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "medrv.mak" CFG="MEdrv - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MEdrv - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "MEdrv - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "MEdrv - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\medrv.lib"


CLEAN :
	-@erase "$(INTDIR)\cbspdu.obj"
	-@erase "$(INTDIR)\me_at.obj"
	-@erase "$(INTDIR)\me_comn.obj"
	-@erase "$(INTDIR)\me_func.obj"
	-@erase "$(INTDIR)\me_init.obj"
	-@erase "$(INTDIR)\me_wnd.obj"
	-@erase "$(INTDIR)\smspdu.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\medrv.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

LINK32=link.exe -lib
MTL=midl.exe
CPP=cl.exe
CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "..\include" /I "..\include\ddi" /D "NDEBUG" /D "_WINDOWS" /D "_EMULATE_" /Fp"$(INTDIR)\medrv.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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

RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\medrv.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\medrv.lib" 
LIB32_OBJS= \
	"$(INTDIR)\cbspdu.obj" \
	"$(INTDIR)\me_at.obj" \
	"$(INTDIR)\me_comn.obj" \
	"$(INTDIR)\me_func.obj" \
	"$(INTDIR)\me_init.obj" \
	"$(INTDIR)\me_wnd.obj" \
	"$(INTDIR)\smspdu.obj"

"$(OUTDIR)\medrv.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "MEdrv - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "..\..\lib\MEdrv.lib" "$(OUTDIR)\medrv.bsc"


CLEAN :
	-@erase "$(INTDIR)\cbspdu.obj"
	-@erase "$(INTDIR)\cbspdu.sbr"
	-@erase "$(INTDIR)\me_at.obj"
	-@erase "$(INTDIR)\me_at.sbr"
	-@erase "$(INTDIR)\me_comn.obj"
	-@erase "$(INTDIR)\me_comn.sbr"
	-@erase "$(INTDIR)\me_func.obj"
	-@erase "$(INTDIR)\me_func.sbr"
	-@erase "$(INTDIR)\me_init.obj"
	-@erase "$(INTDIR)\me_init.sbr"
	-@erase "$(INTDIR)\me_wnd.obj"
	-@erase "$(INTDIR)\me_wnd.sbr"
	-@erase "$(INTDIR)\smspdu.obj"
	-@erase "$(INTDIR)\smspdu.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\medrv.bsc"
	-@erase "..\..\lib\MEdrv.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

LINK32=link.exe -lib
MTL=midl.exe
CPP=cl.exe
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /ZI /Od /X /I "../include20" /I "../include20/ddi" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_EMULATE_" /D "_TCPIPMEMDEB_" /D "_TCPIPX86_" /D "_TCPIPASSERT_" /D "LITTLE_ENDIAN" /D "_IP_DWBROADCAST_" /D "_TCPIP_BULID_VC_" /D "_TCPIP_NOUI_" /D "TCPIP_SINGELMODUL" /D "_KERN200" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

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

RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\medrv.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\cbspdu.sbr" \
	"$(INTDIR)\me_at.sbr" \
	"$(INTDIR)\me_comn.sbr" \
	"$(INTDIR)\me_func.sbr" \
	"$(INTDIR)\me_init.sbr" \
	"$(INTDIR)\me_wnd.sbr" \
	"$(INTDIR)\smspdu.sbr"

"$(OUTDIR)\medrv.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\..\lib\MEdrv.lib" 
LIB32_OBJS= \
	"$(INTDIR)\cbspdu.obj" \
	"$(INTDIR)\me_at.obj" \
	"$(INTDIR)\me_comn.obj" \
	"$(INTDIR)\me_func.obj" \
	"$(INTDIR)\me_init.obj" \
	"$(INTDIR)\me_wnd.obj" \
	"$(INTDIR)\smspdu.obj"

"..\..\lib\MEdrv.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("medrv.dep")
!INCLUDE "medrv.dep"
!ELSE 
!MESSAGE Warning: cannot find "medrv.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "MEdrv - Win32 Release" || "$(CFG)" == "MEdrv - Win32 Debug"
SOURCE=.\cbspdu.c

!IF  "$(CFG)" == "MEdrv - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /I "..\include" /I "..\include\ddi" /D "NDEBUG" /D "_WINDOWS" /D "_EMULATE_" /Fp"$(INTDIR)\medrv.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\cbspdu.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MEdrv - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /X /I "../include20/ddi" /I "../../include20" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_EMULATE_" /D "_TCPIPMEMDEB_" /D "_TCPIPX86_" /D "_TCPIPASSERT_" /D "LITTLE_ENDIAN" /D "_IP_DWBROADCAST_" /D "_TCPIP_BULID_VC_" /D "_TCPIP_NOUI_" /D "TCPIP_SINGELMODUL" /D "_KERN200" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\cbspdu.obj"	"$(INTDIR)\cbspdu.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\me_at.c

!IF  "$(CFG)" == "MEdrv - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /I "..\include" /I "..\include\ddi" /D "NDEBUG" /D "_WINDOWS" /D "_EMULATE_" /Fp"$(INTDIR)\medrv.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\me_at.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MEdrv - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /X /I "../../include20/ddi" /I "../../include20" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_EMULATE_" /D "_TCPIPMEMDEB_" /D "_TCPIPX86_" /D "_TCPIPASSERT_" /D "LITTLE_ENDIAN" /D "_IP_DWBROADCAST_" /D "_TCPIP_BULID_VC_" /D "_TCPIP_NOUI_" /D "TCPIP_SINGELMODUL" /D "_KERN200" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\me_at.obj"	"$(INTDIR)\me_at.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\me_comn.c

!IF  "$(CFG)" == "MEdrv - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /I "..\include" /I "..\include\ddi" /D "NDEBUG" /D "_WINDOWS" /D "_EMULATE_" /Fp"$(INTDIR)\medrv.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\me_comn.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MEdrv - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /X /I "../../include20" /I "../../include20/ddi" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_EMULATE_" /D "_TCPIPMEMDEB_" /D "_TCPIPX86_" /D "_TCPIPASSERT_" /D "LITTLE_ENDIAN" /D "_IP_DWBROADCAST_" /D "_TCPIP_BULID_VC_" /D "_TCPIP_NOUI_" /D "TCPIP_SINGELMODUL" /D "_KERN200" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\me_comn.obj"	"$(INTDIR)\me_comn.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\me_func.c

!IF  "$(CFG)" == "MEdrv - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /I "..\include" /I "..\include\ddi" /D "NDEBUG" /D "_WINDOWS" /D "_EMULATE_" /Fp"$(INTDIR)\medrv.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\me_func.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MEdrv - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /X /I "../../include20" /I "../../include20/ddi" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_EMULATE_" /D "_TCPIPMEMDEB_" /D "_TCPIPX86_" /D "_TCPIPASSERT_" /D "LITTLE_ENDIAN" /D "_IP_DWBROADCAST_" /D "_TCPIP_BULID_VC_" /D "_TCPIP_NOUI_" /D "TCPIP_SINGELMODUL" /D "_KERN200" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\me_func.obj"	"$(INTDIR)\me_func.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\me_init.c

!IF  "$(CFG)" == "MEdrv - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /I "..\include" /I "..\include\ddi" /D "NDEBUG" /D "_WINDOWS" /D "_EMULATE_" /Fp"$(INTDIR)\medrv.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\me_init.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MEdrv - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /X /I "../../include20" /I "../../include20/ddi" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_EMULATE_" /D "_TCPIPMEMDEB_" /D "_TCPIPX86_" /D "_TCPIPASSERT_" /D "LITTLE_ENDIAN" /D "_IP_DWBROADCAST_" /D "_TCPIP_BULID_VC_" /D "_TCPIP_NOUI_" /D "TCPIP_SINGELMODUL" /D "_KERN200" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\me_init.obj"	"$(INTDIR)\me_init.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\me_wnd.c

!IF  "$(CFG)" == "MEdrv - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /I "..\include" /I "..\include\ddi" /D "NDEBUG" /D "_WINDOWS" /D "_EMULATE_" /Fp"$(INTDIR)\medrv.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\me_wnd.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MEdrv - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /X /I "../../include20" /I "../../include20/ddi" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_EMULATE_" /D "_TCPIPMEMDEB_" /D "_TCPIPX86_" /D "_TCPIPASSERT_" /D "LITTLE_ENDIAN" /D "_IP_DWBROADCAST_" /D "_TCPIP_BULID_VC_" /D "_TCPIP_NOUI_" /D "TCPIP_SINGELMODUL" /D "_KERN200" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\me_wnd.obj"	"$(INTDIR)\me_wnd.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\smspdu.c

!IF  "$(CFG)" == "MEdrv - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /I "..\include" /I "..\include\ddi" /D "NDEBUG" /D "_WINDOWS" /D "_EMULATE_" /Fp"$(INTDIR)\medrv.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\smspdu.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MEdrv - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /X /I "../../include20" /I "../../include20/ddi" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_EMULATE_" /D "_TCPIPMEMDEB_" /D "_TCPIPX86_" /D "_TCPIPASSERT_" /D "LITTLE_ENDIAN" /D "_IP_DWBROADCAST_" /D "_TCPIP_BULID_VC_" /D "_TCPIP_NOUI_" /D "TCPIP_SINGELMODUL" /D "_KERN200" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\smspdu.obj"	"$(INTDIR)\smspdu.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 


!ENDIF 

