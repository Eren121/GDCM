# Microsoft Developer Studio Project File - Name="libgdcmijpeg8" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=libgdcmijpeg8 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "libgdcmijpeg8.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libgdcmijpeg8.mak" CFG="libgdcmijpeg8 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libgdcmijpeg8 - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "libgdcmijpeg8 - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "libgdcmijpeg8 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release__libgdcmijpeg8"
# PROP Intermediate_Dir "Release__libgdcmijpeg8"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LIBGDCMIJPEG8_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LIBGDCMIJPEG8_EXPORTS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "NDEBUG"
# ADD RSC /l 0x40c /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 gdi32.lib kernel32.lib /nologo /dll /machine:I386 /nodefaultlib /out:"..\..\lib\libgdcmijpeg8.dll"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy    Release__libgdcmijpeg8\libgdcmijpeg8.lib    ..\..\lib\   	copy    ..\..\lib\libgdcmijpeg8.dll    ..\..\gdcmPython\   	copy    ..\..\lib\libgdcmijpeg8.dll    ..\..\vtk\ 
# End Special Build Tool

!ELSEIF  "$(CFG)" == "libgdcmijpeg8 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug__libgdcmijpeg8"
# PROP Intermediate_Dir "Debug__libgdcmijpeg8"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LIBGDCMIJPEG8_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LIBGDCMIJPEG8_EXPORTS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "_DEBUG"
# ADD RSC /l 0x40c /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib msvcrtd.lib /nologo /dll /debug /machine:I386 /nodefaultlib /out:"..\..\lib\libgdcmijpeg8.dll" /pdbtype:sept
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy    Debug__libgdcmijpeg8\libgdcmijpeg8.lib    ..\..\lib\   	copy    ..\..\lib\libgdcmijpeg8.dll    ..\..\gdcmPython\   	copy    ..\..\lib\libgdcmijpeg8.dll    ..\..\vtk\ 
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "libgdcmijpeg8 - Win32 Release"
# Name "libgdcmijpeg8 - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\jpeg\libijg8\jcapimin.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\libijg8\jcapistd.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\libijg8\jccoefct.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\libijg8\jccolor.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\libijg8\jcdctmgr.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\libijg8\jchuff.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\libijg8\jcinit.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\libijg8\jcmainct.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\libijg8\jcmarker.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\libijg8\jcmaster.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\libijg8\jcomapi.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\libijg8\jcparam.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\libijg8\jcphuff.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\libijg8\jcprepct.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\libijg8\jcsample.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\libijg8\jctrans.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\libijg8\jdapimin.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\libijg8\jdapistd.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\libijg8\jdatadst.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\libijg8\jdatasrc.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\libijg8\jdcoefct.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\libijg8\jdcolor.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\libijg8\jddctmgr.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\libijg8\jdhuff.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\libijg8\jdinput.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\libijg8\jdmainct.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\libijg8\jdmarker.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\libijg8\jdmaster.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\libijg8\jdmerge.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\libijg8\jdphuff.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\libijg8\jdpostct.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\libijg8\jdsample.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\libijg8\jdtrans.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\libijg8\jerror.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\libijg8\jfdctflt.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\libijg8\jfdctfst.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\libijg8\jfdctint.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\libijg8\jidctflt.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\libijg8\jidctfst.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\libijg8\jidctint.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\libijg8\jidctred.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\libijg8\jmemmgr.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\libijg8\jmemnobs.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\libijg8\jquant1.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\libijg8\jquant2.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\libijg8\jutils.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
