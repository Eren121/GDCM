# Microsoft Developer Studio Project File - Name="vtkgdcm" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=vtkgdcm - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "vtkgdcm.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "vtkgdcm.mak" CFG="vtkgdcm - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "vtkgdcm - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "vtkgdcm - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "vtkgdcm - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "VTKGDCM_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "$(VTKPATH)\include\vtk" /I "..\..\src" /I "..\..\src\jpeg\libijg8" /I "..\..\src\jpeg\libijg12" /I "..\..\src\jpeg\ljpg" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "VTKGDCM_EXPORTS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "NDEBUG"
# ADD RSC /l 0x40c /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 Wsock32.lib kernel32.lib user32.lib gdi32.lib /nologo /dll /machine:I386 /out:"..\..\lib\vtkgdcm.dll" /libpath:"$(VTKPATH)\lib\vtk ../../lib/"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy ..\..\lib\vtkgdcm.dll ..\	copy Release\vtkgdcm.lib ..\..\lib\vtkgdcm.lib
# End Special Build Tool

!ELSEIF  "$(CFG)" == "vtkgdcm - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "VTKGDCM_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "$(VTKPATH)\include\vtk" /I "..\..\src" /I "..\..\src\jpeg\libijg8" /I "..\..\src\jpeg\libijg12" /I "..\..\src\jpeg\ljpg" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "VTKGDCM_EXPORTS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "_DEBUG"
# ADD RSC /l 0x40c /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 Wsock32.lib kernel32.lib user32.lib gdi32.lib /nologo /dll /debug /machine:I386 /nodefaultlib:"msvcrt" /out:"..\..\lib\vtkgdcm.dll" /pdbtype:sept /libpath:"$(VTKPATH)\lib\vtk ../../lib/"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy ..\..\lib\vtkgdcm.dll ..\	copy Debug\vtkgdcm.lib ..\..\lib\vtkgdcm.lib
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "vtkgdcm - Win32 Release"
# Name "vtkgdcm - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\src\gdcmDict.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\gdcmDictEntry.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\gdcmDictSet.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\gdcmElValSet.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\gdcmElValue.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\gdcmException.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\gdcmFile.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\gdcmHeader.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\gdcmHeaderHelper.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\gdcmJpeg.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\gdcmJpeg12.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\gdcmJpeg2000.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\gdcmParse.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\gdcmRLE.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\gdcmTS.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\gdcmUtil.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\gdcmVR.cxx
# End Source File
# Begin Source File

SOURCE=..\vtkGdcmReader.cxx
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\vtkGdcmReader.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=c:\Creatis\vtkDistrib\lib\vtk\vtkIO.lib
# End Source File
# Begin Source File

SOURCE=c:\Creatis\vtkDistrib\lib\vtk\vtkCommon.lib
# End Source File
# Begin Source File

SOURCE=c:\Creatis\vtkDistrib\lib\vtk\vtkFiltering.lib
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\lib\libgdcmljpeg.lib
# End Source File
# Begin Source File

SOURCE=..\..\lib\libgdcmijpeg8.lib
# End Source File
# Begin Source File

SOURCE=..\..\lib\libgdcmijpeg12.lib
# End Source File
# End Target
# End Project
