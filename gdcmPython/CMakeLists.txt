# gdcm Python wrapping stuff:

INCLUDE_DIRECTORIES(
  ${GDCM_SOURCE_DIR}/src
  ${PYTHON_INCLUDE_PATH}
  ${GDCM_BINARY_DIR}/
)
SUBDIRS(demo)

#-----------------------------------------------------------------------------
# SWIG Wrapping
#

# CMake 2.0 will fully support Swig wrapping so split based on cmake version:

IF(${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} GREATER 1.9)

  FIND_PACKAGE(SWIG REQUIRED)
  INCLUDE(${SWIG_USE_FILE})

  #INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

  #SET(CMAKE_SWIG_FLAGS "")

  SET_SOURCE_FILES_PROPERTIES(gdcm.i PROPERTIES CPLUSPLUS ON)
  SET_SOURCE_FILES_PROPERTIES(gdcm.i PROPERTIES SWIG_FLAGS "-includeall")
  SWIG_ADD_MODULE(gdcm python
    gdcm.i gdcm_wrap.cxx)
  SWIG_LINK_LIBRARIES(gdcm ${PYTHON_LIBRARIES})
  TARGET_LINK_LIBRARIES(_gdcm gdcm)

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
  SET_SOURCE_FILES_PROPERTIES(gdcm_wrap.cxx GENERATED)

  ADD_LIBRARY(pygdcm gdcm_wrap.cxx)
  TARGET_LINK_LIBRARIES(pygdcm
    ${PYTHON_LIBRARY}
    gdcm
  )
  # http://www.cmake.org/pipermail/cmake/2003-August/004190.html
  #SET_TARGET_PROPERTIES(pygdcm PROPERTIES PREFIX "_")

  # This swig command will also outputs gdcm.py :
  SET_SOURCE_FILES_PROPERTIES(gdcm.py GENERATED)

  # Instead of a simple copy operation let create our own gdcm.py file:
  # First read what swig dumped:
  FILE(READ ${GDCM_BINARY_DIR}/gdcmPython/gdcm.py SWIG_GDCM_PY)

  # Then create the gdcm.py = gdcmVersion.py + gdcm.py (swig) file:
  FILE(WRITE ${GDCM_BINARY_DIR}/bin/gdcm.py
    "import os\n"
    "gdcmVERSION=\"${GDCM_VERSION_FULL}\"\n"
    "GDCM_DATA_PATH=\"${GDCM_DATA_ROOT}\"\n\n"
    ${SWIG_GDCM_PY}
    )

  # Seems like they want instead a file called gdcmPython.py
  # Thus add the custom command to copy it to proper location:
  ADD_CUSTOM_COMMAND(
    TARGET pygdcm
    POST_BUILD
    COMMAND ${CMAKE_COMMAND}
    ARGS -E copy 
      ${GDCM_BINARY_DIR}/bin/gdcm.py 
      ${GDCM_BINARY_DIR}/bin/gdcmPython.py
  )

  # Here comes the hack to rename the library to a more 'swig' compliant scheme"
  ADD_CUSTOM_COMMAND(
    TARGET pygdcm
    POST_BUILD
    COMMAND ${CMAKE_COMMAND}
    ARGS -E copy 
      ${GDCM_BINARY_DIR}/bin/libpygdcm.so
      ${GDCM_BINARY_DIR}/bin/_gdcm.so
  )

  # where are library stored ?
#  SET(GDCM_LIB_DIR ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR})
#  ADD_CUSTOM_COMMAND(
#    TARGET pygdcm
#    POST_BUILD
#    COMMAND ${CMAKE_COMMAND}
#    ARGS -E copy
#      ${EXECUTABLE_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/${MY_LIB_NAME}${CMAKE_SHLIB_SUFFIX}
#      ${EXECUTABLE_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/vtkXY.pvm
#    TARGET vtkXY
#  )

  # Remove temporary libpygdcm.so library:
#  ADD_CUSTOM_COMMAND(
#    TARGET pygdcm
#    POST_BUILD
#    COMMAND ${CMAKE_COMMAND}
#    ARGS -E remove ${GDCM_BINARY_DIR}/bin/libpygdcm.so
#  )
#  GET_TARGET_PROPERTY(foo pygdcm LOCATION)
#  MESSAGE(FATAL_ERROR ${foo})

ENDIF(${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} GREATER 1.9)   


#-----------------------------------------------------------------------------
# VTK Wrapping
# 
IF(GDCM_VTK)
  INCLUDE_DIRECTORIES(
    ${GDCM_SOURCE_DIR}/vtk
  )

  SET(vtkgdcmPython_la_SOURCES
      ${GDCM_SOURCE_DIR}/vtk/vtkGdcmReader.cxx
  )
  SET_SOURCE_FILES_PROPERTIES(vtkGdcmReaderPython.cxx GENERATED)
  
  # Configure Python module, which is the plugin itself
  IF (GDCM_WRAP_PYTHON)
    VTK_WRAP_PYTHON2(vtkgdcmPython gdcmfoo
                   ${vtkgdcmPython_la_SOURCES})
    # The C++ files must be made into a C++ library
    ADD_LIBRARY(vtkgdcmPython SHARED ${gdcmfoo})
    # set the libraries to link against
    TARGET_LINK_LIBRARIES (vtkgdcmPython
                           #_pygdcm
                           vtkgdcm
                           vtkCommonPython
                           vtkIOPython)
  ENDIF (GDCM_WRAP_PYTHON)
  
  #generate a setup.py according to VTK installation 
  #put it in subdir in order to not override old one
  # as it is a pain...override existing file...
  
  GET_FILENAME_COMPONENT(VTKPATH ${VTK_DIR}/../../ ABSOLUTE)
  CONFIGURE_FILE(
      ${GDCM_SOURCE_DIR}/gdcmPython/setup.py.in
      ${GDCM_BINARY_DIR}/setup.py
  )
  #MESSAGE(STATUS ${VTKPATH})
ENDIF(GDCM_VTK)

#-----------------------------------------------------------------------------
# Install stuff:

#Should install in path of PYTHON_LIBRARY and not the default CMAKE_INSTALL_PREFIX

GET_FILENAME_COMPONENT(PYTHON_LIB_INSTALL ${PYTHON_LIBRARY} PATH)
GET_FILENAME_COMPONENT(SITEPACKAGE 
  ${PYTHON_LIB_INSTALL}/../site-packages ABSOLUTE)
#MESSAGE(STATUS ${SITEPACKAGE})
#INSTALL_FILES(/include "\\.h$")
#INSTALL_TARGETS(/lib/ gdcm)

# gdcm.py is a file generated by swig
# then from python you can do import gdcm, and it will load _gdcm.so
#INSTALL_TARGETS(/lib/ gdcm.py)
#INSTALL_PROGRAMS(/lib/ gdcm.py)
#INSTALL_FILES(/lib/ .py gdcm.py)

# When installing swig-generated python module one should pay attention
# to library naming convention, see here for more detail:
# http://www.swig.org/Doc1.3/Python.html#n6
# even with the $swig -module mylib, there is always a trailing underscore added
