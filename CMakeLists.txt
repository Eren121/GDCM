PROJECT(GDCM)
CMAKE_MINIMUM_REQUIRED(VERSION 1.8)

#-----------------------------------------------------------------------------
# GDCM version number, usefull for packaging and doxygen doc:
SET(GDCM_VERSION_MAJOR 0)
SET(GDCM_VERSION_MINOR 4)
SET(GDCM_VERSION_PATCH 0)
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
SET(GDCM_BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS})

#-----------------------------------------------------------------------------
# Does not work as of 2004/04/05
#SET(CMAKE_CONFIGURATION_TYPES
#  Debug
#  Release
#)
#SET(CMAKE_BUILD_TYPE_INIT Debug)

#-----------------------------------------------------------------------------
# Output directories.
#Put all stuff in one single dir for Win32, otherwise dll are a pain:
IF(WIN32 AND GDCM_BUILD_SHARED_LIBS)
  SET(OUTPUT_LIB_DIR ${GDCM_SOURCE_DIR}/bin)
ELSE(WIN32 AND GDCM_BUILD_SHARED_LIBS)
  SET(OUTPUT_LIB_DIR ${GDCM_SOURCE_DIR}/lib)
ENDIF(WIN32 AND GDCM_BUILD_SHARED_LIBS)

SET (EXECUTABLE_OUTPUT_PATH ${GDCM_SOURCE_DIR}/bin CACHE PATH "Single output directory for building all executables.")
SET (LIBRARY_OUTPUT_PATH ${OUTPUT_LIB_DIR} CACHE PATH "Single output directory for building all libraries.")
MARK_AS_ADVANCED(LIBRARY_OUTPUT_PATH EXECUTABLE_OUTPUT_PATH)
SET(GDCM_LIBRARY_PATH "${LIBRARY_OUTPUT_PATH}")
SET(GDCM_EXECUTABLE_PATH "${EXECUTABLE_OUTPUT_PATH}")

#-----------------------------------------------------------------------------
# Build directory on which many applications depend
SUBDIRS(
  src
  Dicts
  Test
)

#-----------------------------------------------------------------------------
# Adding GDCM_DATA_ROOT
FIND_PATH(GDCM_DATA_ROOT gdcm-ACR-LibIDO.acr
	${GDCM_SOURCE_DIR}/../gdcmData 
	$ENV{GDCM_DATA}
	$ENV{PUB_DICT_PATH}/../../gdcmData
)

#-----------------------------------------------------------------------------
# Deals with problem on SunOS:
# ostrstream vs. ostringstream

#SET(GDCM_NO_ANSI_STRING_STREAM
#  ${CMAKE_NO_ANSI_STRING_STREAM}
#)
CONFIGURE_FILE(${GDCM_SOURCE_DIR}/gdcmConfigure.h.in
               ${GDCM_BINARY_DIR}/gdcmConfigure.h @ONLY IMMEDIATE)

#-----------------------------------------------------------------------------
# Add the testing directories
OPTION(GDCM_BUILD_TESTING "Test the project" ON)
IF(GDCM_BUILD_TESTING)
  ENABLE_TESTING()
  INCLUDE (${CMAKE_ROOT}/Modules/Dart.cmake)
  SUBDIRS(Test)
ENDIF(GDCM_BUILD_TESTING)


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

#-----------------------------------------------------------------------------
# Configure files with settings for use by the build.
# for third party to access gdcm through cmake
# to be rewritten to include only the necessary

CONFIGURE_FILE(
  ${GDCM_SOURCE_DIR}/gdcmConfig.cmake.in 
  ${GDCM_BINARY_DIR}/gdcmConfig.cmake @ONLY IMMEDIATE
  )

# Export uor build settings and library
# dependencies for the use by the user projects.
INCLUDE(
  ${CMAKE_ROOT}/Modules/CMakeExportBuildSettings.cmake)

CMAKE_EXPORT_BUILD_SETTINGS(
  ${GDCM_BINARY_DIR}/gdcmBuildSettings.cmake
  )

EXPORT_LIBRARY_DEPENDENCIES(
  ${GDCM_BINARY_DIR}/gdcmLibraryDepends.cmake
  )

# Intall the packaging files for use by FIND_PACKAGE(GDCM) in user projects.
INSTALL_FILES(/lin/gdcm FILES
  ${GDCM_SOURCE_DIR}/gdcmUse.cmake
  ${GDCM_BINARY_DIR}/gdcmConfig.cmake
  ${GDCM_BINARY_DIR}/gdcmBuildSettings.cmake
  ${GDCM_BINARY_DIR}/gdcmLibraryDepends.cmake
  )

#-----------------------------------------------------------------------------
# Copy / paste from old m4 script:

# EXTRA_DIST = \
#	AUTHORS	\
#	ChangeLog \
#	COPYING \
#	INSTALL \
#	NEWS \
#	README \
#	TODO \
#	gdcm.spec \
#       gdcm.dsw

#release:
#	$(MAKE) dist distdir=$(PACKAGE)-$(VERSION)

#snapshot:
#	$(MAKE) dist distdir=$(PACKAGE)`date +"%y%m%d"`
