PROJECT(GDCM)
CMAKE_MINIMUM_REQUIRED(VERSION 1.8)

#-----------------------------------------------------------------------------
# GDCM version number, usefull for packaging and doxygen doc:
SET(GDCM_VERSION_MAJOR 0)
SET(GDCM_VERSION_MINOR 4)
SET(GDCM_VERSION_PATCH 1)
SET(GDCM_VERSION "${GDCM_VERSION_MAJOR}.${GDCM_VERSION_MINOR}")
SET(GDCM_VERSION_FULL "${GDCM_VERSION}.${GDCM_VERSION_PATCH}")

#-----------------------------------------------------------------------------
# datadir in CMake:
SET(GDCM_DATA_DIR "/share/gdcm/" CACHE INTERNAL 
  "Install location for data (relative to prefix).")
MARK_AS_ADVANCED(GDCM_DATA_DIR)

#-----------------------------------------------------------------------------
# Build shared lib by default
OPTION(BUILD_SHARED_LIBS "Build GDCM with shared libraries." ON)

OPTION(GDCM_DEBUG "Turn verbosity of some statement ON." OFF)

#-----------------------------------------------------------------------------
SET (EXECUTABLE_OUTPUT_PATH ${GDCM_BINARY_DIR}/bin CACHE PATH "Single output directory for building all executables.")
SET (LIBRARY_OUTPUT_PATH ${GDCM_BINARY_DIR}/bin CACHE PATH "Single output directory for building all libraries.")
MARK_AS_ADVANCED(LIBRARY_OUTPUT_PATH EXECUTABLE_OUTPUT_PATH)

#-----------------------------------------------------------------------------
# Build directory on which many applications depend
SUBDIRS(
  src
  Dicts
  Example
)

#-----------------------------------------------------------------------------
# Adding GDCM_DATA_ROOT
FIND_PATH(GDCM_DATA_ROOT gdcm-ACR-LibIDO.acr
  ${GDCM_SOURCE_DIR}/../gdcmData 
  $ENV{GDCM_DATA}
  $ENV{PUB_DICT_PATH}/../../gdcmData
)

# Set the place for the public dictionary, must be set before gdcmConfigure.h
SET( GDCM_PUB_DICT_PATH "${CMAKE_INSTALL_PREFIX}${GDCM_DATA_DIR}")

#-----------------------------------------------------------------------------
# SunOS + old gcc fixes:
INCLUDE (${CMAKE_ROOT}/Modules/CMakeBackwardCompatibilityCXX.cmake)
IF(CMAKE_NO_ANSI_STRING_STREAM)
  SET(GDCM_NO_ANSI_STRING_STREAM 1)
ENDIF(CMAKE_NO_ANSI_STRING_STREAM)

INCLUDE (${CMAKE_ROOT}/Modules/CheckIncludeFile.cmake)
CHECK_INCLUDE_FILE("stdint.h" CMAKE_HAVE_STDINT_H)

CONFIGURE_FILE(${GDCM_SOURCE_DIR}/gdcmConfigure.h.in
               ${GDCM_BINARY_DIR}/gdcmConfigure.h @ONLY IMMEDIATE)

INSTALL_FILES(/include .h gdcmConfigure.h)

#-----------------------------------------------------------------------------
# Add the testing directories
OPTION(BUILD_TESTING "Build testing." ON)

IF(BUILD_TESTING)
  ENABLE_TESTING()
  INCLUDE (${CMAKE_ROOT}/Modules/Dart.cmake)
  SUBDIRS(Test)
ENDIF(BUILD_TESTING)


#-----------------------------------------------------------------------------
OPTION(GDCM_VTK "Build VTK-GDCM Lib." OFF)
IF(GDCM_VTK)
  # Search VTK
  FIND_PACKAGE(VTK)
  # If vtk found
  IF(VTK_FOUND)
    INCLUDE(${VTK_USE_FILE})
    SUBDIRS(vtk)
  ENDIF(VTK_FOUND)
ENDIF(GDCM_VTK)

#-----------------------------------------------------------------------------
OPTION(GDCM_WRAP_PYTHON "Wrap GDCM classes into the Python language." OFF)
#if wrapping enable
IF(GDCM_WRAP_PYTHON)
  # Search Python libs:
  FIND_PACKAGE(PythonLibs)
  # If PythonLibs found
  IF(PYTHON_LIBRARY)
    SUBDIRS(gdcmPython)
  ENDIF(PYTHON_LIBRARY)
ENDIF(GDCM_WRAP_PYTHON)

#-----------------------------------------------------------------------------
OPTION(GDCM_DOXYGEN "Build source documentation using doxygen." OFF)
# if doxygen enable
IF(GDCM_DOXYGEN)
  # Search Doxygen:
  FIND_PACKAGE(Doxygen)
  IF(DOXYGEN)
    SUBDIRS(Doc)
  ENDIF(DOXYGEN)
ENDIF(GDCM_DOXYGEN)

