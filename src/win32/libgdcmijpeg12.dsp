# Microsoft Developer Studio Project File - Name="libgdcmijpeg12" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=libgdcmijpeg12 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "libgdcmijpeg12.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libgdcmijpeg12.mak" CFG="libgdcmijpeg12 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libgdcmijpeg12 - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "libgdcmijpeg12 - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "libgdcmijpeg12 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release__libgdcmijpeg12"
# PROP Intermediate_Dir "Release__libgdcmijpeg12"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LIBGDCMIJPEG12_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LIBGDCMIJPEG12_EXPORTS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "NDEBUG"
# ADD RSC /l 0x40c /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 gdi32.lib kernel32.lib /nologo /dll /machine:I386 /nodefaultlib /out:"..\..\lib\libgdcmijpeg12.dll"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy     Release__libgdcmijpeg12\libgdcmijpeg12.lib     ..\..\lib\    	copy     ..\..\lib\libgdcmijpeg12.dll     ..\..\gdcmPython\    	copy     ..\..\lib\libgdcmijpeg12.dll     ..\..\vtk\ 	copy  ..\..\lib\libgdcmijpeg12.dll            ..\..\test\ 
# End Special Build Tool

!ELSEIF  "$(CFG)" == "libgdcmijpeg12 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug__libgdcmijpeg12"
# PROP Intermediate_Dir "Debug__libgdcmijpeg12"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LIBGDCMIJPEG12_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LIBGDCMIJPEG12_EXPORTS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "_DEBUG"
# ADD RSC /l 0x40c /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib msvcrtd.lib /nologo /dll /debug /machine:I386 /nodefaultlib /out:"..\..\lib\libgdcmijpeg12.dll" /pdbtype:sept
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy     Debug__libgdcmijpeg12\libgdcmijpeg12.lib     ..\..\lib\    	copy     ..\..\lib\libgdcmijpeg12.dll     ..\..\gdcmPython\    	copy     ..\..\lib\libgdcmijpeg12.dll     ..\..\vtk\ 	copy  ..\..\lib\libgdcmijpeg12.dll            ..\..\test\ 
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "libgdcmijpeg12 - Win32 Release"
# Name "libgdcmijpeg12 - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\jpeg\libijg12\jcapimin12.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\libijg12\jcapistd12.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\libijg12\jccoefct12.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\libijg12\jccolor12.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\libijg12\jcdctmgr12.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\libijg12\jchuff12.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\libijg12\jcinit12.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\libijg12\jcmainct12.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\libijg12\jcmarker12.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\libijg12\jcmaster12.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\libijg12\jcomapi12.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\libijg12\jcparam12.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\libijg12\jcphuff12.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\libijg12\jcprepct12.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\libijg12\jcsample12.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\libijg12\jctrans12.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\libijg12\jdapimin12.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\libijg12\jdapistd12.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\libijg12\jdatadst12.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\libijg12\jdatasrc12.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\libijg12\jdcoefct12.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\libijg12\jdcolor12.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\libijg12\jddctmgr12.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\libijg12\jdhuff12.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\libijg12\jdinput12.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\libijg12\jdmainct12.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\libijg12\jdmarker12.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\libijg12\jdmaster12.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\libijg12\jdmerge12.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\libijg12\jdphuff12.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\libijg12\jdpostct12.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\libijg12\jdsample12.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\libijg12\jdtrans12.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\libijg12\jerror12.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\libijg12\jfdctflt12.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\libijg12\jfdctfst12.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\libijg12\jfdctint12.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\libijg12\jidctflt12.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\libijg12\jidctfst12.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\libijg12\jidctint12.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\libijg12\jidctred12.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\libijg12\jmemmgr12.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\libijg12\jmemnobs12.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\libijg12\jquant112.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\libijg12\jquant212.c
# End Source File
# Begin Source File

SOURCE=..\jpeg\libijg12\jutils12.c
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
