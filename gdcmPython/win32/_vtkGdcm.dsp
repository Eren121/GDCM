# Microsoft Developer Studio Project File - Name="_vtkGdcm" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=_vtkGdcm - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "_vtkGdcm.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "_vtkGdcm.mak" CFG="_vtkGdcm - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "_vtkGdcm - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "_vtkGdcm - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "_vtkGdcm - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_VTKGDCM_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\..\src\jpeg\libijg8" /I "..\..\vtk" /I "$(VTKPATH)\include\vtk" /I "..\..\src" /I "$(CREATIS)\python22\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_VTKGDCM_EXPORTS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "NDEBUG"
# ADD RSC /l 0x40c /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib Wsock32.lib /nologo /dll /machine:I386 /out:"..\vtkgdcmPython.dll"

!ELSEIF  "$(CFG)" == "_vtkGdcm - Win32 Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_VTKGDCM_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\\" /I "$(VTKPATH)\include\vtk" /I "..\..\src" /I "..\..\src\jpeg\libijg8" /I "..\..\vtk" /I "$(CREATIS)\python22\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_VTKGDCM_EXPORTS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "_DEBUG"
# ADD RSC /l 0x40c /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /out:"..\vtkGdcmPython.dll" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "_vtkGdcm - Win32 Release"
# Name "_vtkGdcm - Win32 Debug"
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

SOURCE=..\..\src\gdcmHeaderIdo.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\gdcmJpeg.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\gdcmJpegIdo.cxx
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

SOURCE=..\..\src\jpeg\libijg8\jcapimin.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\libijg8\jcapistd.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\libijg8\jccoefct.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\libijg8\jccolor.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\libijg8\jcdctmgr.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\libijg8\jchuff.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\libijg8\jcinit.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\libijg8\jcmainct.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\libijg8\jcmarker.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\libijg8\jcmaster.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\libijg8\jcomapi.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\libijg8\jcparam.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\libijg8\jcphuff.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\libijg8\jcprepct.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\libijg8\jcsample.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\libijg8\jctrans.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\libijg8\jdapimin.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\libijg8\jdapistd.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\libijg8\jdatadst.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\libijg8\jdatasrc.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\libijg8\jdcoefct.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\libijg8\jdcolor.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\libijg8\jddctmgr.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\libijg8\jdhuff.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\libijg8\jdinput.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\libijg8\jdmainct.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\libijg8\jdmarker.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\libijg8\jdmaster.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\libijg8\jdmerge.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\libijg8\jdphuff.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\libijg8\jdpostct.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\libijg8\jdsample.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\libijg8\jdtrans.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\libijg8\jerror.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\libijg8\jfdctflt.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\libijg8\jfdctfst.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\libijg8\jfdctint.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\libijg8\jidctflt.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\libijg8\jidctfst.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\libijg8\jidctint.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\libijg8\jidctred.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\libijg8\jmemmgr.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\libijg8\jmemnobs.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\libijg8\jquant1.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\libijg8\jquant2.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\libijg8\jutils.c
# End Source File
# Begin Source File

SOURCE=.\vtkGdcmInit.cxx
# End Source File
# Begin Source File

SOURCE=..\..\vtk\vtkGdcmReader.cxx
# End Source File
# Begin Source File

SOURCE=..\vtkGdcmReaderPython.cxx
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\init.h

!IF  "$(CFG)" == "_vtkGdcm - Win32 Release"

# Begin Custom Build - Performing Custom Build Step on $(VTKPATH)\bin\initwrappythonwin32 vtkgdcmPython vtkGdmReader > $(ProjDir)\vtkGdcmInit.cxx
ProjDir=.
InputPath=.\init.h

"$(ProjDir)\vtkGdcmInit.cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(VTKPATH)\bin\initwrappythonwin32 vtkgdcmPython vtkGdcmReader > $(ProjDir)\vtkGdcmInit.cxx

# End Custom Build

!ELSEIF  "$(CFG)" == "_vtkGdcm - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\vtk\vtkGdcmReader.h

!IF  "$(CFG)" == "_vtkGdcm - Win32 Release"

# Begin Custom Build - Performing Custom Build Step on $(VTKPATH)\bin\vtkwrappython "$(InputDir)\$(InputName).h" $(VTKPATH)\Examples\Build\vtkMy\Wrapping\hints 1  $(ProjDir)\..\$(InputName)Python.cxx
InputDir=\Projects\gdcm\vtk
ProjDir=.
InputPath=..\..\vtk\vtkGdcmReader.h
InputName=vtkGdcmReader

"$(ProjDir)\..\$(InputName)Python.cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(VTKPATH)\bin\vtkwrappython "$(InputDir)\$(InputName).h" $(VTKPATH)\Examples\Build\vtkMy\Wrapping\hints 1  $(ProjDir)\..\$(InputName)Python.cxx

# End Custom Build

!ELSEIF  "$(CFG)" == "_vtkGdcm - Win32 Debug"

!ENDIF 

# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=c:\Creatis\Python22\libs\python22.lib
# End Source File
# Begin Source File

SOURCE=C:\Creatis\vtkDistrib\lib\vtk\vtkIOPython.lib
# End Source File
# Begin Source File

SOURCE=c:\Creatis\vtkDistrib\lib\vtk\vtkCommonPython.lib
# End Source File
# Begin Source File

SOURCE=c:\Creatis\vtkDistrib\lib\vtk\vtkIO.lib
# End Source File
# Begin Source File

SOURCE=c:\Creatis\vtkDistrib\lib\vtk\vtkCommon.lib
# End Source File
# Begin Source File

SOURCE=C:\Creatis\vtkDistrib\lib\vtk\vtkFilteringPython.lib
# End Source File
# Begin Source File

SOURCE=c:\Creatis\vtkDistrib\lib\vtk\vtkFiltering.lib
# End Source File
# End Group
# End Target
# End Project
