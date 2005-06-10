PROJECT(GDCM)
CMAKE_MINIMUM_REQUIRED(VERSION 2.0)
# okay in fact you need at least cmake 2.0.4 to get swig working properly

#-----------------------------------------------------------------------------
# GDCM version number, usefull for packaging and doxygen doc:
SET(GDCM_MAJOR_VERSION 1)
SET(GDCM_MINOR_VERSION 1)
SET(GDCM_BUILD_VERSION 0)
SET(GDCM_VERSION 
    "${GDCM_MAJOR_VERSION}.${GDCM_MINOR_VERSION}.${GDCM_BUILD_VERSION}")

#-----------------------------------------------------------------------------
# datadir in CMake:
SET(GDCM_DATA_DIR "/share/gdcm/" CACHE INTERNAL 
  "Install location for data (relative to prefix).")
MARK_AS_ADVANCED(GDCM_DATA_DIR)

#-----------------------------------------------------------------------------
# Build shared lib by default
OPTION(GDCM_BUILD_SHARED_LIBS "Build GDCM with shared libraries." ON)
SET(BUILD_SHARED_LIBS ${GDCM_BUILD_SHARED_LIBS})

OPTION(GDCM_DEBUG "Turn verbosity of some statement ON." OFF)
MARK_AS_ADVANCED(GDCM_DEBUG)

#-----------------------------------------------------------------------------
SET (EXECUTABLE_OUTPUT_PATH ${GDCM_BINARY_DIR}/bin CACHE PATH "Single output directory for building all executables.")
SET (LIBRARY_OUTPUT_PATH ${GDCM_BINARY_DIR}/bin CACHE PATH "Single output directory for building all libraries.")
MARK_AS_ADVANCED(LIBRARY_OUTPUT_PATH EXECUTABLE_OUTPUT_PATH)

#-----------------------------------------------------------------------------
# Help outside projects link against gdcm

EXPORT_LIBRARY_DEPENDENCIES(${GDCM_BINARY_DIR}/GDCMLibraryDepends.cmake)

# Save the compiler settings so another project can import them.
INCLUDE(${CMAKE_ROOT}/Modules/CMakeExportBuildSettings.cmake)
CMAKE_EXPORT_BUILD_SETTINGS(${GDCM_BINARY_DIR}/GDCMBuildSettings.cmake)
#INSTALL_FILES(/lib/gdcm .cmake GDCMBuildSettings)

# For GDCM from build tree:
SET(GDCM_USE_FILE ${GDCM_SOURCE_DIR}/GDCMUse.cmake)
SET(GDCM_INCLUDE_DIRS
  ${GDCM_BINARY_DIR}/     #for gdcmConfigure.h
  ${GDCM_SOURCE_DIR}/src  #for main gdcm include
  )
SET(GDCM_LIBRARY_DIRS ${LIBRARY_OUTPUT_PATH})
SET(GDCM_LIBRARY_DEPENDS_FILE ${GDCM_BINARY_DIR}/GDCMLibraryDepends.cmake)
CONFIGURE_FILE(${GDCM_SOURCE_DIR}/GDCMConfig.cmake.in
               ${GDCM_BINARY_DIR}/GDCMConfig.cmake @ONLY IMMEDIATE)

# For installed GDCM:
SET(GDCM_USE_FILE ${CMAKE_INSTALL_PREFIX}/lib/GDCM-${GDCM_VERSION}/GDCMUse.cmake)
SET(GDCM_INCLUDE_DIRS ${CMAKE_INSTALL_PREFIX}/include/GDCM-${GDCM_VERSION})
SET(GDCM_LIBRARY_DIRS ${CMAKE_INSTALL_PREFIX}/lib/ExodusII-${GDCM_VERSION})
SET(GDCM_LIBRARY_DEPENDS_FILE ${CMAKE_INSTALL_PREFIX}/lib/GDCM-${GDCM_VERSION}/GDCMLibraryDepends.cmake)
CONFIGURE_FILE(${GDCM_SOURCE_DIR}/GDCMConfig.cmake.in
               ${GDCM_BINARY_DIR}/Utilities/GDCMConfig.cmake @ONLY IMMEDIATE)

#-----------------------------------------------------------------------------
# Test if compiler defines the pseudo-macro __FUNCTION__
IF("GDCM_COMPILER_HAS_FUNCTION" MATCHES "^GDCM_COMPILER_HAS_FUNCTION$")
  MESSAGE(STATUS "Checking support for __FUNCTION__ in compiler")
  TRY_COMPILE(GDCM_COMPILER_HAS_FUNCTION
              ${GDCM_BINARY_DIR}/CMakeTmp/Function
              ${GDCM_SOURCE_DIR}/CMake/gdcmTestFUNCTION.cxx
              OUTPUT_VARIABLE OUTPUT)
  IF(GDCM_COMPILER_HAS_FUNCTION)
    MESSAGE(STATUS "Checking support for __FUNCTION__ -- yes")
    SET(GDCM_COMPILER_HAS_FUNCTION 1 CACHE INTERNAL "Support for extension C __FUNCTION__")
    WRITE_FILE(${CMAKE_BINARY_DIR}/CMakeOutput.log
      "Determining if the C compiler supports __FUNCTION__ "
      "passed with the following output:\n"
      "${OUTPUT}\n" APPEND)
  ELSE(GDCM_COMPILER_HAS_FUNCTION)
    MESSAGE(STATUS "Checking support for __FUNCTION__ -- no")
    SET(GDCM_COMPILER_HAS_FUNCTION 0 CACHE INTERNAL "Support for extension C __FUNCTION__")
    WRITE_FILE(${CMAKE_BINARY_DIR}/CMakeError.log
      "Determining if the C compiler supports __FUNCTION__ "
      "failed with the following output:\n"
      "${OUTPUT}\n" APPEND)
  ENDIF(GDCM_COMPILER_HAS_FUNCTION)
ENDIF("GDCM_COMPILER_HAS_FUNCTION" MATCHES "^GDCM_COMPILER_HAS_FUNCTION$")

#-----------------------------------------------------------------------------
# Build directory on which many applications depend
SUBDIRS(
  Dicts
  src
)
#-----------------------------------------------------------------------------
# Example subdir:

OPTION(BUILD_EXAMPLES "Build GDCM examples." ON)
IF(BUILD_EXAMPLES)
  SUBDIRS(Example)
ENDIF(BUILD_EXAMPLES)

#-----------------------------------------------------------------------------
# Adding GDCM_DATA_ROOT
FIND_PATH(GDCM_DATA_ROOT gdcm-ACR-LibIDO.acr
  ${GDCM_SOURCE_DIR}/../gdcmData 
  $ENV{GDCM_DATA}
  $ENV{PUB_DICT_PATH}/../../gdcmData
)

# Set the place for the public dictionary, must be set before gdcmConfigure.h
#SET( GDCM_PUB_DICT_PATH "${CMAKE_INSTALL_PREFIX}${GDCM_DATA_DIR}")
FIND_PATH(GDCM_PUB_DICT_PATH dicomV3.dic
  ${GDCM_SOURCE_DIR}/Dicts
  $ENV{PUB_DICT_PATH}
)

#-----------------------------------------------------------------------------
# SunOS + old gcc fixes:
INCLUDE (${CMAKE_ROOT}/Modules/CMakeBackwardCompatibilityCXX.cmake)
IF(CMAKE_NO_ANSI_STRING_STREAM)
  SET(GDCM_NO_ANSI_STRING_STREAM 1)
ENDIF(CMAKE_NO_ANSI_STRING_STREAM)

# Big endian thing:
INCLUDE (${CMAKE_ROOT}/Modules/TestBigEndian.cmake)
TEST_BIG_ENDIAN(GDCM_WORDS_BIGENDIAN)

INCLUDE (${CMAKE_ROOT}/Modules/CheckIncludeFile.cmake)
IF(UNIX) #Avoid polluting Win32 cmakecache
  CHECK_INCLUDE_FILE("stdint.h"       CMAKE_HAVE_STDINT_H)
  CHECK_INCLUDE_FILE("inttypes.h"     CMAKE_HAVE_INTTYPES_H)
ENDIF(UNIX)

# Check if header file exists and add it to the list.
INCLUDE (${CMAKE_ROOT}/Modules/CheckIncludeFiles.cmake)
MACRO(CHECK_INCLUDE_FILE_CONCAT FILE VARIABLE)
  CHECK_INCLUDE_FILES("${GDCM_INCLUDES};${FILE}" ${VARIABLE})
  IF(${VARIABLE})
    SET(GDCM_INCLUDES ${GDCM_INCLUDES} ${FILE})
  ENDIF(${VARIABLE})
ENDMACRO(CHECK_INCLUDE_FILE_CONCAT)

# Need those header for GetMacAddress in Util
# This is tricky as you need to figure out the proper order to
# test the inclusion of files
CHECK_INCLUDE_FILE_CONCAT("unistd.h"       CMAKE_HAVE_UNISTD_H)
CHECK_INCLUDE_FILE_CONCAT("stdlib.h"       CMAKE_HAVE_STDLIB_H)
IF(UNIX) #Avoid polluting Win32 cmakecache
  CHECK_INCLUDE_FILE_CONCAT("sys/ioctl.h"    CMAKE_HAVE_SYS_IOCTL_H)
  CHECK_INCLUDE_FILE_CONCAT("sys/socket.h"   CMAKE_HAVE_SYS_SOCKET_H)
  CHECK_INCLUDE_FILE_CONCAT("sys/sockio.h"   CMAKE_HAVE_SYS_SOCKIO_H)
  CHECK_INCLUDE_FILE_CONCAT("net/if.h"       CMAKE_HAVE_NET_IF_H)
  CHECK_INCLUDE_FILE_CONCAT("netinet/in.h"   CMAKE_HAVE_NETINET_IN_H)
  CHECK_INCLUDE_FILE_CONCAT("net/if_dl.h"    CMAKE_HAVE_NET_IF_DL_H)
  CHECK_INCLUDE_FILE_CONCAT("net/if_arp.h"   CMAKE_HAVE_NET_IF_ARP_H)
ENDIF(UNIX)

#-----------------------------------------------------------------------------
# Test if os defines a lenght for sockaddr
# only doing the test when we have sys/socket
IF( ${CMAKE_HAVE_SYS_SOCKET_H} )
  IF("HAVE_SA_LEN" MATCHES "^HAVE_SA_LEN$")
    STRING(ASCII 35 POUND)
    FILE(WRITE ${GDCM_BINARY_DIR}/CMakeTmp/gdcmTestHAVESALEN.c
      "${POUND}include <sys/types.h>\n"
      "${POUND}include <sys/socket.h>\n"
      "int main() { struct sockaddr sa; sa.sa_len = 0; }")
    MESSAGE(STATUS "Checking if ifreq has a sa_len")
    TRY_COMPILE(HAVE_SA_LEN
      ${GDCM_BINARY_DIR}
      ${GDCM_BINARY_DIR}/CMakeTmp/gdcmTestHAVESALEN.c
      OUTPUT_VARIABLE OUTPUT)
    IF(HAVE_SA_LEN)
      MESSAGE(STATUS "Checking if ifreq has a sa_len -- yes")
      SET(HAVE_SA_LEN 1 CACHE INTERNAL "Support if ifreq has a sa_len")
      WRITE_FILE(${CMAKE_BINARY_DIR}/CMakeOutput.log
        "Determining if ifreq has a sa_len"
        "passed with the following output:\n"
        "${OUTPUT}\n" APPEND)
    ELSE(HAVE_SA_LEN)
      MESSAGE(STATUS "Checking if ifreq has a sa_len -- no")
      SET(HAVE_SA_LEN 0 CACHE INTERNAL "Support if ifreq has a sa_len")
      WRITE_FILE(${CMAKE_BINARY_DIR}/CMakeError.log
        "Determining if ifreq has a sa_len"
        "failed with the following output:\n"
        "${OUTPUT}\n" APPEND)
    ENDIF(HAVE_SA_LEN)
  ENDIF("HAVE_SA_LEN" MATCHES "^HAVE_SA_LEN$")
ENDIF( ${CMAKE_HAVE_SYS_SOCKET_H} )

#-----------------------------------------------------------------------------
# Force Big Endian emulation on little endian:
OPTION(GDCM_FORCE_BIGENDIAN_EMULATION "Force Big Endian Emulation. Do use if you don't know what you are doing." OFF)

CONFIGURE_FILE(${GDCM_SOURCE_DIR}/gdcmConfigure.h.in
               ${GDCM_BINARY_DIR}/gdcmConfigure.h @ONLY IMMEDIATE)

INSTALL_FILES(/include .h gdcmConfigure.h)

#-----------------------------------------------------------------------------
# Add the testing directories
OPTION(BUILD_TESTING "Build testing." ON)

IF(BUILD_TESTING)
  CONFIGURE_FILE(${GDCM_SOURCE_DIR}/CMake/CTestCustom.ctest.in
      ${GDCM_BINARY_DIR}/CMake/CTestCustom.ctest @ONLY)
  ENABLE_TESTING()
  INCLUDE (${CMAKE_ROOT}/Modules/Dart.cmake)
  MARK_AS_ADVANCED(DART_ROOT)
  SUBDIRS(Testing)
ENDIF(BUILD_TESTING)


#-----------------------------------------------------------------------------
OPTION(GDCM_VTK "Build VTK-GDCM Lib." OFF)
IF(GDCM_VTK)
  # Search VTK
  FIND_PACKAGE(VTK)
  # If vtk found
  IF(VTK_FOUND)
    #INCLUDE(${VTK_USE_FILE})
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
OPTION(GDCM_DOCUMENTATION "Build source documentation using doxygen." OFF)
# if doxygen enable
IF(GDCM_DOCUMENTATION)
  # Search Doxygen:
  FIND_PACKAGE(Doxygen)
  IF(DOXYGEN)
    SUBDIRS(Doc)
  ENDIF(DOXYGEN)
ENDIF(GDCM_DOCUMENTATION)

#-----------------------------------------------------------------------------
IF(WIN32)
  OPTION(GDCM_INSTALLER "Build the install setup using InnoSetup." OFF)
ENDIF(WIN32)
IF(UNIX)
  OPTION(GDCM_INSTALLER "Prepare the rpmBuild use." OFF)
ENDIF(UNIX)
IF(GDCM_INSTALLER)
  SUBDIRS(Builder)
ENDIF(GDCM_INSTALLER)

