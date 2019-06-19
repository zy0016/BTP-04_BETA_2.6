# Microsoft Developer Studio Generated NMAKE File, Based on APP_Public.dsp
!IF "$(CFG)" == ""
CFG=APP_Public - Win32 Debug
!MESSAGE No configuration specified. Defaulting to APP_Public - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "APP_Public - Win32 Release" && "$(CFG)" != "APP_Public - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
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
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "APP_Public - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\APP_Public.lib"


CLEAN :
	-@erase "$(INTDIR)\ColorSelect.obj"
	-@erase "$(INTDIR)\confirmw.obj"
	-@erase "$(INTDIR)\confirmwEx.obj"
	-@erase "$(INTDIR)\editex.obj"
	-@erase "$(INTDIR)\FormViewer.obj"
	-@erase "$(INTDIR)\FullScreenConfirm.obj"
	-@erase "$(INTDIR)\FullScreenTimeout.obj"
	-@erase "$(INTDIR)\GameHelp.obj"
	-@erase "$(INTDIR)\GetValidUrl.obj"
	-@erase "$(INTDIR)\GXSReset.obj"
	-@erase "$(INTDIR)\LevInd.obj"
	-@erase "$(INTDIR)\menulist.obj"
	-@erase "$(INTDIR)\OptionSelect.obj"
	-@erase "$(INTDIR)\PassChange.obj"
	-@erase "$(INTDIR)\PassCheck.obj"
	-@erase "$(INTDIR)\piclist.obj"
	-@erase "$(INTDIR)\picmultilist.obj"
	-@erase "$(INTDIR)\PicPreBrow.obj"
	-@erase "$(INTDIR)\playmusic.obj"
	-@erase "$(INTDIR)\PreBrow.obj"
	-@erase "$(INTDIR)\PreBrowHead.obj"
	-@erase "$(INTDIR)\pub_function.obj"
	-@erase "$(INTDIR)\Receivebox.obj"
	-@erase "$(INTDIR)\Recipient.obj"
	-@erase "$(INTDIR)\RGPreBrow.obj"
	-@erase "$(INTDIR)\Search.obj"
	-@erase "$(INTDIR)\SHPreBrow.obj"
	-@erase "$(INTDIR)\SMPreBrow.obj"
	-@erase "$(INTDIR)\smscount.obj"
	-@erase "$(INTDIR)\spinex.obj"
	-@erase "$(INTDIR)\SSPassCheck.obj"
	-@erase "$(INTDIR)\TextView.obj"
	-@erase "$(INTDIR)\timeedit.obj"
	-@erase "$(INTDIR)\tipswin.obj"
	-@erase "$(INTDIR)\TxPreBrow.obj"
	-@erase "$(INTDIR)\unitedit.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\Version.obj"
	-@erase "$(INTDIR)\WaitWin.obj"
	-@erase "$(INTDIR)\wsctrlex.obj"
	-@erase "$(OUTDIR)\APP_Public.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

LINK32=link.exe -lib
MTL=midl.exe
vvRSC=rc.exe
CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Fp"$(INTDIR)\APP_Public.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\APP_Public.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\APP_Public.lib" 
LIB32_OBJS= \
	"$(INTDIR)\ColorSelect.obj" \
	"$(INTDIR)\confirmw.obj" \
	"$(INTDIR)\confirmwEx.obj" \
	"$(INTDIR)\editex.obj" \
	"$(INTDIR)\FormViewer.obj" \
	"$(INTDIR)\FullScreenConfirm.obj" \
	"$(INTDIR)\FullScreenTimeout.obj" \
	"$(INTDIR)\GameHelp.obj" \
	"$(INTDIR)\GetValidUrl.obj" \
	"$(INTDIR)\LevInd.obj" \
	"$(INTDIR)\menulist.obj" \
	"$(INTDIR)\PassChange.obj" \
	"$(INTDIR)\PassCheck.obj" \
	"$(INTDIR)\piclist.obj" \
	"$(INTDIR)\picmultilist.obj" \
	"$(INTDIR)\PicPreBrow.obj" \
	"$(INTDIR)\PreBrow.obj" \
	"$(INTDIR)\PreBrowHead.obj" \
	"$(INTDIR)\Receivebox.obj" \
	"$(INTDIR)\Recipient.obj" \
	"$(INTDIR)\RGPreBrow.obj" \
	"$(INTDIR)\Search.obj" \
	"$(INTDIR)\SHPreBrow.obj" \
	"$(INTDIR)\SMPreBrow.obj" \
	"$(INTDIR)\spinex.obj" \
	"$(INTDIR)\SSPassCheck.obj" \
	"$(INTDIR)\TextView.obj" \
	"$(INTDIR)\timeedit.obj" \
	"$(INTDIR)\tipswin.obj" \
	"$(INTDIR)\TxPreBrow.obj" \
	"$(INTDIR)\unitedit.obj" \
	"$(INTDIR)\WaitWin.obj" \
	"$(INTDIR)\wsctrlex.obj" \
	"$(INTDIR)\GXSReset.obj" \
	"$(INTDIR)\OptionSelect.obj" \
	"$(INTDIR)\playmusic.obj" \
	"$(INTDIR)\pub_function.obj" \
	"$(INTDIR)\smscount.obj" \
	"$(INTDIR)\Version.obj"

"$(OUTDIR)\APP_Public.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "APP_Public - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "..\..\..\lib\public.lib" "$(OUTDIR)\APP_Public.bsc"


CLEAN :
	-@erase "$(INTDIR)\ColorSelect.obj"
	-@erase "$(INTDIR)\ColorSelect.sbr"
	-@erase "$(INTDIR)\confirmw.obj"
	-@erase "$(INTDIR)\confirmw.sbr"
	-@erase "$(INTDIR)\confirmwEx.obj"
	-@erase "$(INTDIR)\confirmwEx.sbr"
	-@erase "$(INTDIR)\editex.obj"
	-@erase "$(INTDIR)\editex.sbr"
	-@erase "$(INTDIR)\FormViewer.obj"
	-@erase "$(INTDIR)\FormViewer.sbr"
	-@erase "$(INTDIR)\FullScreenConfirm.obj"
	-@erase "$(INTDIR)\FullScreenConfirm.sbr"
	-@erase "$(INTDIR)\FullScreenTimeout.obj"
	-@erase "$(INTDIR)\FullScreenTimeout.sbr"
	-@erase "$(INTDIR)\GameHelp.obj"
	-@erase "$(INTDIR)\GameHelp.sbr"
	-@erase "$(INTDIR)\GetValidUrl.obj"
	-@erase "$(INTDIR)\GetValidUrl.sbr"
	-@erase "$(INTDIR)\GXSReset.obj"
	-@erase "$(INTDIR)\GXSReset.sbr"
	-@erase "$(INTDIR)\LevInd.obj"
	-@erase "$(INTDIR)\LevInd.sbr"
	-@erase "$(INTDIR)\menulist.obj"
	-@erase "$(INTDIR)\menulist.sbr"
	-@erase "$(INTDIR)\OptionSelect.obj"
	-@erase "$(INTDIR)\OptionSelect.sbr"
	-@erase "$(INTDIR)\PassChange.obj"
	-@erase "$(INTDIR)\PassChange.sbr"
	-@erase "$(INTDIR)\PassCheck.obj"
	-@erase "$(INTDIR)\PassCheck.sbr"
	-@erase "$(INTDIR)\piclist.obj"
	-@erase "$(INTDIR)\piclist.sbr"
	-@erase "$(INTDIR)\picmultilist.obj"
	-@erase "$(INTDIR)\picmultilist.sbr"
	-@erase "$(INTDIR)\PicPreBrow.obj"
	-@erase "$(INTDIR)\PicPreBrow.sbr"
	-@erase "$(INTDIR)\playmusic.obj"
	-@erase "$(INTDIR)\playmusic.sbr"
	-@erase "$(INTDIR)\PreBrow.obj"
	-@erase "$(INTDIR)\PreBrow.sbr"
	-@erase "$(INTDIR)\PreBrowHead.obj"
	-@erase "$(INTDIR)\PreBrowHead.sbr"
	-@erase "$(INTDIR)\pub_function.obj"
	-@erase "$(INTDIR)\pub_function.sbr"
	-@erase "$(INTDIR)\Receivebox.obj"
	-@erase "$(INTDIR)\Receivebox.sbr"
	-@erase "$(INTDIR)\Recipient.obj"
	-@erase "$(INTDIR)\Recipient.sbr"
	-@erase "$(INTDIR)\RGPreBrow.obj"
	-@erase "$(INTDIR)\RGPreBrow.sbr"
	-@erase "$(INTDIR)\Search.obj"
	-@erase "$(INTDIR)\Search.sbr"
	-@erase "$(INTDIR)\SHPreBrow.obj"
	-@erase "$(INTDIR)\SHPreBrow.sbr"
	-@erase "$(INTDIR)\SMPreBrow.obj"
	-@erase "$(INTDIR)\SMPreBrow.sbr"
	-@erase "$(INTDIR)\smscount.obj"
	-@erase "$(INTDIR)\smscount.sbr"
	-@erase "$(INTDIR)\spinex.obj"
	-@erase "$(INTDIR)\spinex.sbr"
	-@erase "$(INTDIR)\SSPassCheck.obj"
	-@erase "$(INTDIR)\SSPassCheck.sbr"
	-@erase "$(INTDIR)\TextView.obj"
	-@erase "$(INTDIR)\TextView.sbr"
	-@erase "$(INTDIR)\timeedit.obj"
	-@erase "$(INTDIR)\timeedit.sbr"
	-@erase "$(INTDIR)\tipswin.obj"
	-@erase "$(INTDIR)\tipswin.sbr"
	-@erase "$(INTDIR)\TxPreBrow.obj"
	-@erase "$(INTDIR)\TxPreBrow.sbr"
	-@erase "$(INTDIR)\unitedit.obj"
	-@erase "$(INTDIR)\unitedit.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\Version.obj"
	-@erase "$(INTDIR)\Version.sbr"
	-@erase "$(INTDIR)\WaitWin.obj"
	-@erase "$(INTDIR)\WaitWin.sbr"
	-@erase "$(INTDIR)\wsctrlex.obj"
	-@erase "$(INTDIR)\wsctrlex.sbr"
	-@erase "$(OUTDIR)\APP_Public.bsc"
	-@erase "..\..\..\lib\public.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

LINK32=link.exe -lib
MTL=midl.exe
vvRSC=rc.exe
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /ZI /Od /I "..\..\..\include" /I "..\..\..\include\public" /I "..\pub_interface" /I "..\..\..\include\window" /I "..\..\..\include\image" /I "..\..\..\include\project" /I "..\..\..\include\me" /I "..\..\..\include\setup" /I "..\..\..\include\ab" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_DEBUG" /D "_EMULATE_" /D "_863BIRD_" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\APP_Public.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\APP_Public.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\ColorSelect.sbr" \
	"$(INTDIR)\confirmw.sbr" \
	"$(INTDIR)\confirmwEx.sbr" \
	"$(INTDIR)\editex.sbr" \
	"$(INTDIR)\FormViewer.sbr" \
	"$(INTDIR)\FullScreenConfirm.sbr" \
	"$(INTDIR)\FullScreenTimeout.sbr" \
	"$(INTDIR)\GameHelp.sbr" \
	"$(INTDIR)\GetValidUrl.sbr" \
	"$(INTDIR)\LevInd.sbr" \
	"$(INTDIR)\menulist.sbr" \
	"$(INTDIR)\PassChange.sbr" \
	"$(INTDIR)\PassCheck.sbr" \
	"$(INTDIR)\piclist.sbr" \
	"$(INTDIR)\picmultilist.sbr" \
	"$(INTDIR)\PicPreBrow.sbr" \
	"$(INTDIR)\PreBrow.sbr" \
	"$(INTDIR)\PreBrowHead.sbr" \
	"$(INTDIR)\Receivebox.sbr" \
	"$(INTDIR)\Recipient.sbr" \
	"$(INTDIR)\RGPreBrow.sbr" \
	"$(INTDIR)\Search.sbr" \
	"$(INTDIR)\SHPreBrow.sbr" \
	"$(INTDIR)\SMPreBrow.sbr" \
	"$(INTDIR)\spinex.sbr" \
	"$(INTDIR)\SSPassCheck.sbr" \
	"$(INTDIR)\TextView.sbr" \
	"$(INTDIR)\timeedit.sbr" \
	"$(INTDIR)\tipswin.sbr" \
	"$(INTDIR)\TxPreBrow.sbr" \
	"$(INTDIR)\unitedit.sbr" \
	"$(INTDIR)\WaitWin.sbr" \
	"$(INTDIR)\wsctrlex.sbr" \
	"$(INTDIR)\GXSReset.sbr" \
	"$(INTDIR)\OptionSelect.sbr" \
	"$(INTDIR)\playmusic.sbr" \
	"$(INTDIR)\pub_function.sbr" \
	"$(INTDIR)\smscount.sbr" \
	"$(INTDIR)\Version.sbr"

"$(OUTDIR)\APP_Public.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\..\..\lib\public.lib" 
LIB32_OBJS= \
	"$(INTDIR)\ColorSelect.obj" \
	"$(INTDIR)\confirmw.obj" \
	"$(INTDIR)\confirmwEx.obj" \
	"$(INTDIR)\editex.obj" \
	"$(INTDIR)\FormViewer.obj" \
	"$(INTDIR)\FullScreenConfirm.obj" \
	"$(INTDIR)\FullScreenTimeout.obj" \
	"$(INTDIR)\GameHelp.obj" \
	"$(INTDIR)\GetValidUrl.obj" \
	"$(INTDIR)\LevInd.obj" \
	"$(INTDIR)\menulist.obj" \
	"$(INTDIR)\PassChange.obj" \
	"$(INTDIR)\PassCheck.obj" \
	"$(INTDIR)\piclist.obj" \
	"$(INTDIR)\picmultilist.obj" \
	"$(INTDIR)\PicPreBrow.obj" \
	"$(INTDIR)\PreBrow.obj" \
	"$(INTDIR)\PreBrowHead.obj" \
	"$(INTDIR)\Receivebox.obj" \
	"$(INTDIR)\Recipient.obj" \
	"$(INTDIR)\RGPreBrow.obj" \
	"$(INTDIR)\Search.obj" \
	"$(INTDIR)\SHPreBrow.obj" \
	"$(INTDIR)\SMPreBrow.obj" \
	"$(INTDIR)\spinex.obj" \
	"$(INTDIR)\SSPassCheck.obj" \
	"$(INTDIR)\TextView.obj" \
	"$(INTDIR)\timeedit.obj" \
	"$(INTDIR)\tipswin.obj" \
	"$(INTDIR)\TxPreBrow.obj" \
	"$(INTDIR)\unitedit.obj" \
	"$(INTDIR)\WaitWin.obj" \
	"$(INTDIR)\wsctrlex.obj" \
	"$(INTDIR)\GXSReset.obj" \
	"$(INTDIR)\OptionSelect.obj" \
	"$(INTDIR)\playmusic.obj" \
	"$(INTDIR)\pub_function.obj" \
	"$(INTDIR)\smscount.obj" \
	"$(INTDIR)\Version.obj"

"..\..\..\lib\public.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
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
!IF EXISTS("APP_Public.dep")
!INCLUDE "APP_Public.dep"
!ELSE 
!MESSAGE Warning: cannot find "APP_Public.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "APP_Public - Win32 Release" || "$(CFG)" == "APP_Public - Win32 Debug"
SOURCE=.\Pub_Control\ColorSelect.c

!IF  "$(CFG)" == "APP_Public - Win32 Release"


"$(INTDIR)\ColorSelect.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "APP_Public - Win32 Debug"


"$(INTDIR)\ColorSelect.obj"	"$(INTDIR)\ColorSelect.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\pub_control\confirmw.c

!IF  "$(CFG)" == "APP_Public - Win32 Release"


"$(INTDIR)\confirmw.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "APP_Public - Win32 Debug"


"$(INTDIR)\confirmw.obj"	"$(INTDIR)\confirmw.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\pub_control\confirmwEx.c

!IF  "$(CFG)" == "APP_Public - Win32 Release"


"$(INTDIR)\confirmwEx.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "APP_Public - Win32 Debug"


"$(INTDIR)\confirmwEx.obj"	"$(INTDIR)\confirmwEx.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\pub_control\editex.c

!IF  "$(CFG)" == "APP_Public - Win32 Release"


"$(INTDIR)\editex.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "APP_Public - Win32 Debug"


"$(INTDIR)\editex.obj"	"$(INTDIR)\editex.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\pub_control\FormViewer.c

!IF  "$(CFG)" == "APP_Public - Win32 Release"


"$(INTDIR)\FormViewer.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "APP_Public - Win32 Debug"


"$(INTDIR)\FormViewer.obj"	"$(INTDIR)\FormViewer.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\pub_control\FullScreenConfirm.c

!IF  "$(CFG)" == "APP_Public - Win32 Release"


"$(INTDIR)\FullScreenConfirm.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "APP_Public - Win32 Debug"


"$(INTDIR)\FullScreenConfirm.obj"	"$(INTDIR)\FullScreenConfirm.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\pub_control\FullScreenTimeout.c

!IF  "$(CFG)" == "APP_Public - Win32 Release"


"$(INTDIR)\FullScreenTimeout.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "APP_Public - Win32 Debug"


"$(INTDIR)\FullScreenTimeout.obj"	"$(INTDIR)\FullScreenTimeout.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Pub_Control\GameHelp.c

!IF  "$(CFG)" == "APP_Public - Win32 Release"


"$(INTDIR)\GameHelp.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "APP_Public - Win32 Debug"


"$(INTDIR)\GameHelp.obj"	"$(INTDIR)\GameHelp.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\pub_control\GetValidUrl.c

!IF  "$(CFG)" == "APP_Public - Win32 Release"


"$(INTDIR)\GetValidUrl.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "APP_Public - Win32 Debug"


"$(INTDIR)\GetValidUrl.obj"	"$(INTDIR)\GetValidUrl.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\pub_control\LevInd.c

!IF  "$(CFG)" == "APP_Public - Win32 Release"


"$(INTDIR)\LevInd.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "APP_Public - Win32 Debug"


"$(INTDIR)\LevInd.obj"	"$(INTDIR)\LevInd.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Pub_Control\menulist.c

!IF  "$(CFG)" == "APP_Public - Win32 Release"


"$(INTDIR)\menulist.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "APP_Public - Win32 Debug"


"$(INTDIR)\menulist.obj"	"$(INTDIR)\menulist.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Pub_Control\PassChange.c

!IF  "$(CFG)" == "APP_Public - Win32 Release"


"$(INTDIR)\PassChange.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "APP_Public - Win32 Debug"


"$(INTDIR)\PassChange.obj"	"$(INTDIR)\PassChange.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Pub_Control\PassCheck.c

!IF  "$(CFG)" == "APP_Public - Win32 Release"


"$(INTDIR)\PassCheck.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "APP_Public - Win32 Debug"


"$(INTDIR)\PassCheck.obj"	"$(INTDIR)\PassCheck.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\pub_control\piclist.c

!IF  "$(CFG)" == "APP_Public - Win32 Release"


"$(INTDIR)\piclist.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "APP_Public - Win32 Debug"


"$(INTDIR)\piclist.obj"	"$(INTDIR)\piclist.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\pub_control\picmultilist.c

!IF  "$(CFG)" == "APP_Public - Win32 Release"


"$(INTDIR)\picmultilist.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "APP_Public - Win32 Debug"


"$(INTDIR)\picmultilist.obj"	"$(INTDIR)\picmultilist.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Pub_Control\PicPreBrow.c

!IF  "$(CFG)" == "APP_Public - Win32 Release"


"$(INTDIR)\PicPreBrow.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "APP_Public - Win32 Debug"


"$(INTDIR)\PicPreBrow.obj"	"$(INTDIR)\PicPreBrow.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Pub_Control\PreBrow.c

!IF  "$(CFG)" == "APP_Public - Win32 Release"


"$(INTDIR)\PreBrow.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "APP_Public - Win32 Debug"


"$(INTDIR)\PreBrow.obj"	"$(INTDIR)\PreBrow.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Pub_Control\PreBrowHead.c

!IF  "$(CFG)" == "APP_Public - Win32 Release"


"$(INTDIR)\PreBrowHead.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "APP_Public - Win32 Debug"


"$(INTDIR)\PreBrowHead.obj"	"$(INTDIR)\PreBrowHead.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\pub_control\Receivebox.c

!IF  "$(CFG)" == "APP_Public - Win32 Release"


"$(INTDIR)\Receivebox.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "APP_Public - Win32 Debug"


"$(INTDIR)\Receivebox.obj"	"$(INTDIR)\Receivebox.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\pub_control\Recipient.c

!IF  "$(CFG)" == "APP_Public - Win32 Release"


"$(INTDIR)\Recipient.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "APP_Public - Win32 Debug"


"$(INTDIR)\Recipient.obj"	"$(INTDIR)\Recipient.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\pub_control\RGPreBrow.c

!IF  "$(CFG)" == "APP_Public - Win32 Release"


"$(INTDIR)\RGPreBrow.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "APP_Public - Win32 Debug"


"$(INTDIR)\RGPreBrow.obj"	"$(INTDIR)\RGPreBrow.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\pub_control\Search.c

!IF  "$(CFG)" == "APP_Public - Win32 Release"


"$(INTDIR)\Search.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "APP_Public - Win32 Debug"


"$(INTDIR)\Search.obj"	"$(INTDIR)\Search.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Pub_Control\SHPreBrow.c

!IF  "$(CFG)" == "APP_Public - Win32 Release"


"$(INTDIR)\SHPreBrow.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "APP_Public - Win32 Debug"


"$(INTDIR)\SHPreBrow.obj"	"$(INTDIR)\SHPreBrow.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\pub_control\SMPreBrow.c

!IF  "$(CFG)" == "APP_Public - Win32 Release"


"$(INTDIR)\SMPreBrow.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "APP_Public - Win32 Debug"


"$(INTDIR)\SMPreBrow.obj"	"$(INTDIR)\SMPreBrow.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\pub_control\spinex.c

!IF  "$(CFG)" == "APP_Public - Win32 Release"


"$(INTDIR)\spinex.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "APP_Public - Win32 Debug"


"$(INTDIR)\spinex.obj"	"$(INTDIR)\spinex.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\pub_control\SSPassCheck.c

!IF  "$(CFG)" == "APP_Public - Win32 Release"


"$(INTDIR)\SSPassCheck.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "APP_Public - Win32 Debug"


"$(INTDIR)\SSPassCheck.obj"	"$(INTDIR)\SSPassCheck.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\pub_control\TextView.c

!IF  "$(CFG)" == "APP_Public - Win32 Release"


"$(INTDIR)\TextView.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "APP_Public - Win32 Debug"


"$(INTDIR)\TextView.obj"	"$(INTDIR)\TextView.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\pub_control\timeedit.c

!IF  "$(CFG)" == "APP_Public - Win32 Release"


"$(INTDIR)\timeedit.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "APP_Public - Win32 Debug"


"$(INTDIR)\timeedit.obj"	"$(INTDIR)\timeedit.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Pub_Control\tipswin.c

!IF  "$(CFG)" == "APP_Public - Win32 Release"


"$(INTDIR)\tipswin.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "APP_Public - Win32 Debug"


"$(INTDIR)\tipswin.obj"	"$(INTDIR)\tipswin.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Pub_Control\TxPreBrow.c

!IF  "$(CFG)" == "APP_Public - Win32 Release"


"$(INTDIR)\TxPreBrow.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "APP_Public - Win32 Debug"


"$(INTDIR)\TxPreBrow.obj"	"$(INTDIR)\TxPreBrow.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\pub_control\unitedit.c

!IF  "$(CFG)" == "APP_Public - Win32 Release"


"$(INTDIR)\unitedit.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "APP_Public - Win32 Debug"


"$(INTDIR)\unitedit.obj"	"$(INTDIR)\unitedit.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Pub_Control\WaitWin.c

!IF  "$(CFG)" == "APP_Public - Win32 Release"


"$(INTDIR)\WaitWin.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "APP_Public - Win32 Debug"


"$(INTDIR)\WaitWin.obj"	"$(INTDIR)\WaitWin.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\pub_control\wsctrlex.c

!IF  "$(CFG)" == "APP_Public - Win32 Release"


"$(INTDIR)\wsctrlex.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "APP_Public - Win32 Debug"


"$(INTDIR)\wsctrlex.obj"	"$(INTDIR)\wsctrlex.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\pub_interface\GXSReset.c

!IF  "$(CFG)" == "APP_Public - Win32 Release"


"$(INTDIR)\GXSReset.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "APP_Public - Win32 Debug"


"$(INTDIR)\GXSReset.obj"	"$(INTDIR)\GXSReset.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\pub_interface\OptionSelect.c

!IF  "$(CFG)" == "APP_Public - Win32 Release"


"$(INTDIR)\OptionSelect.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "APP_Public - Win32 Debug"


"$(INTDIR)\OptionSelect.obj"	"$(INTDIR)\OptionSelect.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\pub_interface\playmusic.c

!IF  "$(CFG)" == "APP_Public - Win32 Release"


"$(INTDIR)\playmusic.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "APP_Public - Win32 Debug"


"$(INTDIR)\playmusic.obj"	"$(INTDIR)\playmusic.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\pub_interface\pub_function.c

!IF  "$(CFG)" == "APP_Public - Win32 Release"


"$(INTDIR)\pub_function.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "APP_Public - Win32 Debug"


"$(INTDIR)\pub_function.obj"	"$(INTDIR)\pub_function.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\pub_interface\smscount.c

!IF  "$(CFG)" == "APP_Public - Win32 Release"


"$(INTDIR)\smscount.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "APP_Public - Win32 Debug"


"$(INTDIR)\smscount.obj"	"$(INTDIR)\smscount.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\pub_interface\Version.c

!IF  "$(CFG)" == "APP_Public - Win32 Release"


"$(INTDIR)\Version.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "APP_Public - Win32 Debug"


"$(INTDIR)\Version.obj"	"$(INTDIR)\Version.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 


!ENDIF 

