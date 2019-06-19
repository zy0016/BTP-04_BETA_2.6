# Microsoft Developer Studio Project File - Name="image" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=image - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "image.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "image.mak" CFG="image - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "image - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "image - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/SCP_SRC_EMU/imagelib", XYAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "image - Win32 Release"

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
vvRSC=rc.exe
MTL=midl.exe
LINK32=link.exe -lib
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

!ELSEIF  "$(CFG)" == "image - Win32 Debug"

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
vvRSC=rc.exe
MTL=midl.exe
LINK32=link.exe -lib
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /Zp4 /W3 /Gm /GX /ZI /Od /I "../../../../../include" /I "../../../../../include/window" /I "../../../../../include/project" /I "../../../../../include/image" /I "../../../../../include/hal" /D "_DEBUG" /D "_KERN200" /D "_EMULATE_" /FR /YX /FD /GZ /c
# SUBTRACT CPP /X
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../../../../lib/hpimage.lib"

!ENDIF 

# Begin Target

# Name "image - Win32 Release"
# Name "image - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\src\BmpCvert.c
# End Source File
# Begin Source File

SOURCE=..\..\src\DrawImg.c
# End Source File
# Begin Source File

SOURCE=..\..\src\DrawImg.h
# End Source File
# Begin Source File

SOURCE=..\..\src\DrawJpeg.c
# End Source File
# Begin Source File

SOURCE=..\..\src\DrawPng.c
# End Source File
# Begin Source File

SOURCE=..\..\src\drawwbmp.c
# End Source File
# Begin Source File

SOURCE=..\..\src\filesys.c
# End Source File
# Begin Source File

SOURCE=..\..\src\filesys.h
# End Source File
# Begin Source File

SOURCE=..\..\src\plximg.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# Begin Group "gif"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\gif\dgif.c
# End Source File
# Begin Source File

SOURCE=..\..\src\gif\dgif.h
# End Source File
# Begin Source File

SOURCE=..\..\src\gif\EGIF_LIB.C
# End Source File
# Begin Source File

SOURCE=..\..\src\gif\gif_fsm.c
# End Source File
# Begin Source File

SOURCE=..\..\src\gif\gif_fsm.h
# End Source File
# Begin Source File

SOURCE=..\..\src\gif\GIF_HASH.C
# End Source File
# Begin Source File

SOURCE=..\..\src\gif\GIF_HASH.H
# End Source File
# Begin Source File

SOURCE=..\..\src\gif\GIF_LIB.H
# End Source File
# Begin Source File

SOURCE=..\..\src\gif\gif_lwz.c
# End Source File
# Begin Source File

SOURCE=..\..\src\gif\mymemmgr.c
# End Source File
# End Group
# Begin Group "jpeg"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\jpeg\CDERROR.H
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\CDJPEG.H
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\djpeg.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\djpeg.h
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\JCAPIMIN.C
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\JCAPISTD.C
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\JCCOEFCT.C
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\JCCOLOR.C
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\JCDCTMGR.C
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\JCHUFF.C
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\JCHUFF.H
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\JCINIT.C
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\JCMAINCT.C
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\JCMARKER.C
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\JCMASTER.C
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\jcomapi.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\jconfig.h
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\JCPARAM.C
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\JCPHUFF.C
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\JCPREPCT.C
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\JCSAMPLE.C
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\JCTRANS.C
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\jdapimin.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\jdapistd.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\JDATADST.C
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\JDATASRC.C
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\jdcoefct.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\jdcolor.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\jdct.h
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\jddctmgr.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\jdhuff.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\jdhuff.h
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\jdinput.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\jdmainct.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\jdmarker.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\jdmaster.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\JDMERGE.C
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\jdphuff.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\jdpostct.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\jdsample.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\JDTRANS.C
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\JERROR.C
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\jerror.h
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\JFDCTFLT.C
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\JFDCTFST.C
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\JFDCTINT.C
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\JIDCTFLT.C
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\jidctfst.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\jidctint.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\JIDCTRED.C
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\jinclude.h
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\jmemmgr.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\JMEMNOBS.C
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\jmemsys.h
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\jmorecfg.h
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\jpegint.h
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\jpeglib.h
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\JPEGLIB1.H
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\JQUANT1.C
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\JQUANT2.C
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\jutils.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\jversion.h
# End Source File
# End Group
# Begin Group "png"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\png\png.c
# End Source File
# Begin Source File

SOURCE=..\..\src\png\png.h
# End Source File
# Begin Source File

SOURCE=..\..\src\png\pngconf.h
# End Source File
# Begin Source File

SOURCE=..\..\src\png\pngerror.c
# End Source File
# Begin Source File

SOURCE=..\..\src\png\pngget.c
# End Source File
# Begin Source File

SOURCE=..\..\src\png\pngmem.c
# End Source File
# Begin Source File

SOURCE=..\..\src\png\pngpread.c
# End Source File
# Begin Source File

SOURCE=..\..\src\png\pngread.c
# End Source File
# Begin Source File

SOURCE=..\..\src\png\pngrio.c
# End Source File
# Begin Source File

SOURCE=..\..\src\png\pngrtran.c
# End Source File
# Begin Source File

SOURCE=..\..\src\png\pngrutil.c
# End Source File
# Begin Source File

SOURCE=..\..\src\png\pngset.c
# End Source File
# Begin Source File

SOURCE=..\..\src\png\pngtrans.c
# End Source File
# Begin Source File

SOURCE=..\..\src\png\pngvcrd.c
# End Source File
# Begin Source File

SOURCE=..\..\src\png\pngwio.c
# End Source File
# Begin Source File

SOURCE=..\..\src\png\pngwrite.c
# End Source File
# Begin Source File

SOURCE=..\..\src\png\pngwtran.c
# End Source File
# Begin Source File

SOURCE=..\..\src\png\pngwutil.c
# End Source File
# End Group
# End Target
# End Project
