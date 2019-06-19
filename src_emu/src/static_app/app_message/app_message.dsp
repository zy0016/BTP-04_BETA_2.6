# Microsoft Developer Studio Project File - Name="app_message" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=app_message - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "app_message.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "app_message.mak" CFG="app_message - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "app_message - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "app_message - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "app_message - Win32 Release"

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

!ELSEIF  "$(CFG)" == "app_message - Win32 Debug"

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
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\..\..\include" /I "..\..\..\include\window" /I "..\..\..\include\image" /I "..\..\..\include\setup" /I "..\..\..\include\project" /I "..\..\..\include\me" /I "..\..\..\include\ab" /I "..\..\..\include\public" /I "..\..\..\include\mail" /I "..\..\..\include\vcard" /I "..\..\..\include\Browser" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "_TEST_SMS_" /D "_EMULATE_" /YX /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\..\lib\app_message.lib"

!ENDIF 

# Begin Target

# Name "app_message - Win32 Release"
# Name "app_message - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "mu"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\mu\msgpublic.c
# End Source File
# Begin Source File

SOURCE=.\mu\muconst.h
# End Source File
# Begin Source File

SOURCE=.\mu\muentry.c
# End Source File
# Begin Source File

SOURCE=.\mu\muextern.c
# End Source File
# Begin Source File

SOURCE=.\mu\mufolder.c
# End Source File
# Begin Source File

SOURCE=.\mu\muglobal.h
# End Source File
# Begin Source File

SOURCE=.\mu\mulist.c
# End Source File
# Begin Source File

SOURCE=.\mu\mumain.c
# End Source File
# Begin Source File

SOURCE=.\mu\mumemory.c
# End Source File
# Begin Source File

SOURCE=.\mu\mumemory.h
# End Source File
# Begin Source File

SOURCE=.\mu\murs.h
# End Source File
# End Group
# Begin Group "sms"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\sms\cb.c
# End Source File
# Begin Source File

SOURCE=.\sms\mybutton.c
# End Source File
# Begin Source File

SOURCE=.\sms\smsapi.c
# End Source File
# Begin Source File

SOURCE=.\sms\smsconst.h
# End Source File
# Begin Source File

SOURCE=.\sms\smsdetail.c
# End Source File
# Begin Source File

SOURCE=.\sms\smsedit.c
# End Source File
# Begin Source File

SOURCE=.\sms\smsentry.c
# End Source File
# Begin Source File

SOURCE=.\sms\smsglobal.h
# End Source File
# Begin Source File

SOURCE=.\sms\smsinit.c
# End Source File
# Begin Source File

SOURCE=.\sms\smsmemory.c
# End Source File
# Begin Source File

SOURCE=.\sms\smsmemory.h
# End Source File
# Begin Source File

SOURCE=.\sms\smsreceive.c
# End Source File
# Begin Source File

SOURCE=.\sms\smsrs.h
# End Source File
# Begin Source File

SOURCE=.\sms\smssend.c
# End Source File
# Begin Source File

SOURCE=.\sms\smssetting.c
# End Source File
# Begin Source File

SOURCE=.\sms\smssim.c
# End Source File
# Begin Source File

SOURCE=.\sms\smsview.c
# End Source File
# Begin Source File

SOURCE=.\sms\spinlist.c
# End Source File
# Begin Source File

SOURCE=.\sms\temp.c
# End Source File
# Begin Source File

SOURCE=.\sms\vardvcal.c
# End Source File
# End Group
# Begin Group "mms"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\mms\MmsApi.c
# End Source File
# Begin Source File

SOURCE=.\mms\MmsCommon.c
# End Source File
# Begin Source File

SOURCE=.\mms\MmsCommon.h
# End Source File
# Begin Source File

SOURCE=.\mms\MmsDisplay.c
# End Source File
# Begin Source File

SOURCE=.\mms\MmsDisplay.h
# End Source File
# Begin Source File

SOURCE=.\mms\MmsEdit.c
# End Source File
# Begin Source File

SOURCE=.\mms\MmsEdit.h
# End Source File
# Begin Source File

SOURCE=.\mms\MmsGlobal.h
# End Source File
# Begin Source File

SOURCE=.\mms\MmsInsert.c
# End Source File
# Begin Source File

SOURCE=.\mms\MmsInsert.h
# End Source File
# Begin Source File

SOURCE=.\mms\MmsInterface.c
# End Source File
# Begin Source File

SOURCE=.\mms\MmsInterface.h
# End Source File
# Begin Source File

SOURCE=.\mms\MmsList.c
# End Source File
# Begin Source File

SOURCE=.\mms\Mmslist.h
# End Source File
# Begin Source File

SOURCE=.\mms\MmsProtocol.c
# End Source File
# Begin Source File

SOURCE=.\mms\MmsProtocol.h
# End Source File
# Begin Source File

SOURCE=.\mms\MmsSetting.c
# End Source File
# Begin Source File

SOURCE=.\mms\MmsSetting.h
# End Source File
# Begin Source File

SOURCE=.\mms\smilparse.c
# End Source File
# Begin Source File

SOURCE=.\mms\smilparse.h
# End Source File
# End Group
# Begin Group "email"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\email\MailComFunction.c
# End Source File
# Begin Source File

SOURCE=.\email\MailComFunction.h
# End Source File
# Begin Source File

SOURCE=.\email\MailDefine.h
# End Source File
# Begin Source File

SOURCE=.\email\MailDetail.c
# End Source File
# Begin Source File

SOURCE=.\email\MailDial.c
# End Source File
# Begin Source File

SOURCE=.\email\MailEdit.c
# End Source File
# Begin Source File

SOURCE=.\email\MailEdit.h
# End Source File
# Begin Source File

SOURCE=.\email\MailEntry.c
# End Source File
# Begin Source File

SOURCE=.\email\MailGet.c
# End Source File
# Begin Source File

SOURCE=.\email\MailGlobal.h
# End Source File
# Begin Source File

SOURCE=.\email\MailHeader.h
# End Source File
# Begin Source File

SOURCE=.\email\MailInterface.c
# End Source File
# Begin Source File

SOURCE=.\email\MailInterface.h
# End Source File
# Begin Source File

SOURCE=.\email\MailMailBox.c
# End Source File
# Begin Source File

SOURCE=.\email\MailMailBox.h
# End Source File
# Begin Source File

SOURCE=.\email\MailSend.c
# End Source File
# Begin Source File

SOURCE=.\email\MailSetting.c
# End Source File
# Begin Source File

SOURCE=.\email\MailString.h
# End Source File
# Begin Source File

SOURCE=.\email\MailStruct.h
# End Source File
# Begin Source File

SOURCE=.\email\MailView.c
# End Source File
# End Group
# End Group
# End Target
# End Project
