# gdcm Python wrapping stuff:

INCLUDE_DIRECTORIES(
  ${GDCM_SOURCE_DIR}/src
  ${PYTHON_INCLUDE_PATH}
  ${GDCM_BINARY_DIR}/
)

#-----------------------------------------------------------------------------
# SWIG Wrapping
#

# CMake 2.0 will fully support Swig wrapping so split based on cmake version:

# We need to pass that to swig to be sure all modules are generated in the proper dir
SET(CMAKE_SWIG_FLAGS "-outdir ${GDCM_BINARY_DIR}/gdcmPython")
# Looks like a bug to me in cmake 2.0.x:
SEPARATE_ARGUMENTS(CMAKE_SWIG_FLAGS)

FIND_PACKAGE(SWIG REQUIRED)
IF(SWIG_FOUND)
  INCLUDE(${SWIG_USE_FILE})
ENDIF(SWIG_FOUND)


SET_SOURCE_FILES_PROPERTIES(gdcm.i PROPERTIES CPLUSPLUS ON)
# BUG: This is only needed with cmake <= 2.0.5
SET_SOURCE_FILES_PROPERTIES(gdcm.i PROPERTIES SWIG_FLAGS "")
# END OF BUG
SWIG_ADD_MODULE(gdcm python gdcm.i)  #gdcm_wrap.cxx, or gdcmPYTHON_wrap.cxx
SWIG_LINK_LIBRARIES(gdcm gdcm ${PYTHON_LIBRARIES})

# Get rid of -Wall / -W / -pedantic if exist
SET(CMAKE_CXX_FLAGS "")
# I guess I need to put them back on this file:
#SET_SOURCE_FILES_PROPERTIES(
#  ${swig_generated_file_fullname}
#  PROPERTIES COMPILE_FLAGS "")

INSTALL_TARGETS(/lib/gdcm/ _gdcm)

#-----------------------------------------------------------------------------
# VTK Wrapping
# 
IF(GDCM_VTK)
  # Include the VTK library
  INCLUDE(${VTK_USE_FILE})

  INCLUDE_DIRECTORIES(
    ${GDCM_SOURCE_DIR}/vtk
  )

  SET(vtkgdcmPython_la_SOURCES
      ${GDCM_SOURCE_DIR}/vtk/vtkGdcmReader.cxx
      ${GDCM_SOURCE_DIR}/vtk/vtkGdcmWriter.cxx
  )
  SET_SOURCE_FILES_PROPERTIES(vtkGdcmReaderPython.cxx GENERATED)
  
  # Configure Python module, which is the plugin itself
  IF (GDCM_WRAP_PYTHON)
    IF(NOT VTK_WRAP_PYTHON)
      MESSAGE(FATAL_ERROR "Can't build gdcm python wrapping if VTK_WRAP_PYTHON is OFF" )
    ENDIF(NOT VTK_WRAP_PYTHON)
    VTK_WRAP_PYTHON2(vtkgdcmPython gdcmWrapSOURCES
                   ${vtkgdcmPython_la_SOURCES})
    # The C++ files must be made into a C++ library
    ADD_LIBRARY(vtkgdcmPython SHARED ${gdcmWrapSOURCES})  #MODULE on apple ?
    # set the libraries to link against
    #SET_TARGET_PROPERTIES(${foobar} PROPERTIES PREFIX "")

    #GET_TARGET_PROPERTY(swig_gdcm_fullpath "${SWIG_MODULE_gdcm_REAL_NAME}" LOCATION)
    TARGET_LINK_LIBRARIES (vtkgdcmPython
                           vtkgdcm
                           vtkCommonPython
                           vtkIOPython)
    INSTALL_TARGETS(/lib/gdcm/ vtkgdcmPython)

  ENDIF (GDCM_WRAP_PYTHON)
ENDIF(GDCM_VTK)

# generate a setup.py according to VTK installation
# put it in subdir in order to not override old one
#
#GET_FILENAME_COMPONENT(VTKPATH ${VTK_DIR}/../../ ABSOLUTE)
#CONFIGURE_FILE(
#    ${GDCM_SOURCE_DIR}/gdcmPython/setup.py.in
#    ${GDCM_BINARY_DIR}/setup.py
#)

#-----------------------------------------------------------------------------
# Python package configuration
FILE(WRITE ${GDCM_BINARY_DIR}/gdcmPython/gdcmVersion.py
    "gdcmVERSION=\"${GDCM_VERSION}\"\n"
    )

FILE(WRITE ${GDCM_BINARY_DIR}/gdcmPython/__init__.py
    "__all__ = [\"core\",\n"
    "           \"vtk\",\n"
    "          ]\n"
    )

FILE(WRITE ${GDCM_BINARY_DIR}/gdcmPython/core.py
    "from gdcmVersion import *\n"
    "import gdcm\n"
    "GDCM_DATA_ROOT = \"${GDCM_DATA_ROOT}\"\n"
    )

IF(GDCM_VTK)
  FILE(WRITE ${GDCM_BINARY_DIR}/gdcmPython/vtk.py
    "import os\n"
    "from gdcmVersion import *\n"
    "if os.name == 'posix':\n"
    "  from libvtkgdcmPython import *\n"
    "else:\n"
    "  from vtkgdcmPython import *\n"
    "GDCM_DATA_ROOT = \"${GDCM_DATA_ROOT}\"\n"
    )
ENDIF(GDCM_VTK)

#-----------------------------------------------------------------------------
# Python installation
IF(WIN32)
  FILE(WRITE ${GDCM_BINARY_DIR}/gdcmPython/gdcmPython.pth
       "${GDCM_BINARY_DIR}\n"
       "${GDCM_BINARY_DIR}/bin/release\n"
  )
ELSE(WIN32)
  FILE(WRITE ${GDCM_BINARY_DIR}/gdcmPython/gdcmPython.pth
       "${CMAKE_INSTALL_PREFIX}/lib/gdcm\n"
  )
ENDIF(WIN32)
FIND_PATH(PYTHON_INSTALL "site-packages"
    [HKEY_LOCAL_MACHINE\\SOFTWARE\\Python\\PythonCore\\2.4\\InstallPath]
    [HKEY_LOCAL_MACHINE\\SOFTWARE\\Python\\PythonCore\\2.3\\InstallPath]
    [HKEY_LOCAL_MACHINE\\SOFTWARE\\Python\\PythonCore\\2.2\\InstallPath]
    [HKEY_LOCAL_MACHINE\\SOFTWARE\\Python\\PythonCore\\2.1\\InstallPath]
    [HKEY_LOCAL_MACHINE\\SOFTWARE\\Python\\PythonCore\\2.0\\InstallPath]
    [HKEY_LOCAL_MACHINE\\SOFTWARE\\Python\\PythonCore\\1.6\\InstallPath]
    [HKEY_LOCAL_MACHINE\\SOFTWARE\\Python\\PythonCore\\1.5\\InstallPath]
    /usr/lib/python2.4
    /usr/lib/python2.3
    /usr/lib/python2.2
    /usr/lib/python2.1
    /usr/lib/python2.0
    /usr/lib/python1.6
    /usr/lib/python1.5
)
STRING(REGEX REPLACE "/usr(.*)/config.*" "\\1" PYTHON_INSTALL ${PYTHON_LIBRARY})

INSTALL_FILES(${PYTHON_INSTALL}/site-packages "\\.pth$")
INSTALL_FILES(/lib/gdcm/gdcmPython "\\.py$")
INSTALL_FILES(/lib/gdcm/gdcmPython FILES "gdcm.py")

#-----------------------------------------------------------------------------
# Add test from demo subdir:

SUBDIRS(demo)
