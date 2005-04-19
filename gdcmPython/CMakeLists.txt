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

IF(${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} GREATER 1.9)

  # We need to pass that to swig to be sure all modules are generated in the proper dir
  SET(CMAKE_SWIG_FLAGS "-outdir ${GDCM_BINARY_DIR}/gdcmPython")
  # Looks like a bug to me in cmake 2.0.x:
  SEPARATE_ARGUMENTS(CMAKE_SWIG_FLAGS)

  FIND_PACKAGE(SWIG REQUIRED)
  INCLUDE(${SWIG_USE_FILE})

  #SET_SOURCE_FILES_PROPERTIES(gdcm.i PROPERTIES PYTHON ON)
  SET_SOURCE_FILES_PROPERTIES(gdcm.i PROPERTIES CPLUSPLUS ON)
  # Setting SWIG_FLAGS to "-includeall" at the following line looks like
  # a bad idea since swig tries to recursively include all the referenced
  # files, "including" the C++ <sstream>, <list>, <map>... Of course swig
  # has no way of finding the path to those files (unless it is told by
  # cmake) since they are related to the compiler installation layout.
  # Anyhow, since this inclusion recursion is not necessary, just don't
  # do it.
  # Note: apparently the line is required in order to avoid a NOTFOUND
  #       as argument of swig on invocation of make.
  SET_SOURCE_FILES_PROPERTIES(gdcm.i PROPERTIES SWIG_FLAGS "")
  SWIG_ADD_MODULE(gdcm python gdcm.i)  #gdcm_wrap.cxx
  SWIG_LINK_LIBRARIES(gdcm gdcm ${PYTHON_LIBRARIES})

  SET(GDCM_CXX_FLAGS ${CMAKE_SWIG_FLAGS})
  # Get rid of -Wall / -W / -pedantic if exist
  SET(CMAKE_CXX_FLAGS "")
  # I guess I need to put them back on the this file:
  SET_SOURCE_FILES_PROPERTIES(${GDCM_BINARY_DIR}/gdcmPython/gdcm_wrap.cxx
                              PROPERTIES COMPILE_FLAGS "")
#  SET_TARGET_PROPERTIES(${GDCM_BINARY_DIR}/gdcmPython/gdcm_wrap.cxx
#                        PROPERTIES CMAKE_CXX_FLAGS "-foo")
  
ELSE(${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} GREATER 1.9)   

  # As of 09/10/2003 there was no FindSWIG.cmake
  # later we should use SWIG instead of swig
  FIND_PROGRAM(SWIG_EXECUTABLE
    NAMES swig-1.3 swig
  )
  
  IF(NOT SWIG_EXECUTABLE)
    MESSAGE(FATAL_ERROR "Swig could not be found wrapping is impossible")
  ENDIF(NOT SWIG_EXECUTABLE)

  # Build our custom command :
  ADD_CUSTOM_COMMAND(
    OUTPUT    ${GDCM_BINARY_DIR}/gdcmPython/gdcm_wrap.cxx
    COMMAND   ${SWIG_EXECUTABLE}
    ARGS      -python -c++ -I${GDCM_SOURCE_DIR}/src -o 
              ${GDCM_BINARY_DIR}/gdcmPython/gdcm_wrap.cxx 
              ${GDCM_SOURCE_DIR}/gdcmPython/gdcm.i 
    DEPENDS   ${GDCM_SOURCE_DIR}/gdcmPython/gdcm.i
    COMMENT   "Generating gdcm_wrap.cxx/gdcm.py based on gdcm.i"
  )
  # Since the file gdcm_wrap.cxx does not exist, mark it as GENERATED:
  SET_SOURCE_FILES_PROPERTIES(${GDCM_BINARY_DIR}/gdcmPython/gdcm_wrap.cxx GENERATED)

  ADD_LIBRARY(pygdcm gdcm_wrap.cxx)
  TARGET_LINK_LIBRARIES(pygdcm
    ${PYTHON_LIBRARY}
    gdcm
  )
  # http://www.cmake.org/pipermail/cmake/2003-August/004190.html
  #SET_TARGET_PROPERTIES(pygdcm PROPERTIES PREFIX "_")

  # This swig command will also outputs gdcm.py :
  SET_SOURCE_FILES_PROPERTIES(${GDCM_BINARY_DIR}/gdcmPython/gdcm.py GENERATED)

  # Instead of a simple copy operation let create our own gdcm.py file:
  # First read what swig dumped:
  IF(EXISTS ${GDCM_BINARY_DIR}/gdcmPython/gdcm.py)
    FILE(READ ${GDCM_BINARY_DIR}/gdcmPython/gdcm.py SWIG_GDCM_PY)
  ENDIF(EXISTS ${GDCM_BINARY_DIR}/gdcmPython/gdcm.py)

  # Then create the gdcm.py = gdcmVersion.py + gdcm.py (swig) file:
  FILE(WRITE ${GDCM_BINARY_DIR}/bin/gdcmVersion.py
    "import os\n"
    "gdcmVERSION=\"${GDCM_VERSION}\"\n"
    "GDCM_DATA_PATH=\"${GDCM_DATA_ROOT}\"\n\n"
    ${SWIG_GDCM_PY}
    )

  # Seems like they want instead a file called gdcmPython.py
  # Thus add the custom command to copy it to proper location:
#  ADD_CUSTOM_COMMAND(
#    TARGET pygdcm
#    POST_BUILD
#    COMMAND ${CMAKE_COMMAND}
#    ARGS -E copy 
#      ${GDCM_BINARY_DIR}/bin/gdcm.py 
#      ${GDCM_BINARY_DIR}/bin/gdcmPython.py
#  )

  # Here comes the hack to rename the library to a more 'swig' compliant scheme"
  SET(LIB_NAME ${CMAKE_SHARED_LIBRARY_PREFIX}pygdcm${CMAKE_SHARED_LIBRARY_SUFFIX})
  SET(EXE_DIR ${GDCM_BINARY_DIR}/Executable)
  IF(EXECUTABLE_OUTPUT_PATH)
    SET(EXE_DIR ${EXECUTABLE_OUTPUT_PATH})
  ENDIF(EXECUTABLE_OUTPUT_PATH)
  SET(LIB_DIR ${GDCM_BINARY_DIR}/Library/Shared)
  IF(LIBRARY_OUTPUT_PATH)
    SET(LIB_DIR ${LIBRARY_OUTPUT_PATH})
  ENDIF(LIBRARY_OUTPUT_PATH)

  IF(WIN32 OR APPLE)
    SET(SHARED_MUST_BE_IN_EXE_DIR 1)
  ENDIF(WIN32 OR APPLE)

  IF(NOT SHARED_MUST_BE_IN_EXE_DIR)
    SET(EXE_DIR ${LIB_DIR}) 
  ENDIF(NOT SHARED_MUST_BE_IN_EXE_DIR)

  ADD_CUSTOM_COMMAND(
    TARGET pygdcm
    POST_BUILD 
    COMMAND ${CMAKE_COMMAND} ARGS -E copy
    ${LIB_DIR}/${CMAKE_CFG_INTDIR}/${LIB_NAME}
    ${EXE_DIR}/${CMAKE_CFG_INTDIR}/_gdcm${CMAKE_SHARED_LIBRARY_SUFFIX}
    )

ENDIF(${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} GREATER 1.9)   

INSTALL_TARGETS(/lib/ _gdcm)

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
    INSTALL_TARGETS(/lib/ vtkgdcmPython)

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
# Install stuff:

# Cmake cannot handle installing since python wants stuff to be installed
# in /usr/lib/pythonx.x/site-package and not a a std /usr/lib
# Thus generate a setup.py file using distutils

# Create a gdcm.pth file:

FILE(WRITE ${GDCM_BINARY_DIR}/gdcmPython/gdcmVersion.py
    "gdcmVERSION=\"${GDCM_VERSION}\"\n"
    )

FILE(WRITE ${GDCM_BINARY_DIR}/gdcmPython/gdcm.pth
    "gdcmPython\n"
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
# Add test from demo subdir:

SUBDIRS(demo)
