#IF(GDCM_WRAP_PYTHON)
#If we reach this dir we have selected this option


#TODO
#http://www.cmake.org/pipermail/cmake/2003-August/004185.html
#http://www.cmake.org/pipermail/cmake/2003-January/001092.html


INCLUDE_DIRECTORIES(
  ${GDCM_SOURCE_DIR}/src
  ${PYTHON_INCLUDE_PATH}
)

SET(GDCM_INC
  ${GDCM_SOURCE_DIR}/src
)

SET(SWIG_INC
  ${PYTHON_INCLUDES}
  ${GDCM_INC}
)

ADD_CUSTOM_TARGET(mathieu ALL)

SET(GDCM_PYTHON_SOURCES
  gdcm_wrap.cxx
)

SET_SOURCE_FILES_PROPERTIES(gdcm_wrap.cxx GENERATED)

ADD_LIBRARY(pygdcm ${GDCM_PYTHON_SOURCES})
#http://www.cmake.org/pipermail/cmake/2003-August/004190.html
#SET_TARGET_PROPERTIES(pygdcm PROPERTIES PREFIX "")
TARGET_LINK_LIBRARIES(pygdcm 
  ${PYTHON_LIBRARY}
  gdcm
)

#-----------------------------------------------------------------------------
# As of 09/10/2003 there was no FindSWIG.cmake
# later we should use SWIG instead of swig
# Build our custom command :
ADD_CUSTOM_COMMAND(
  SOURCE    ${GDCM_SOURCE_DIR}/gdcmPython/gdcm.i
  COMMAND   swig
  ARGS      -python -c++ -I${GDCM_SOURCE_DIR}/src -o 
            ${GDCM_BINARY_DIR}/gdcmPython/gdcm_wrap.cxx 
            ${GDCM_SOURCE_DIR}/gdcmPython/gdcm.i 
  TARGET    pygdcm
  OUTPUTS   ${GDCM_BINARY_DIR}/gdcmPython/gdcm_wrap.cxx
)

IF(GDCM_VTK)
  INCLUDE_DIRECTORIES(
    ${GDCM_SOURCE_DIR}/vtk
  )

  SET(vtkgdcmPython_la_SOURCES
      ${GDCM_SOURCE_DIR}/vtk/vtkGdcmReader.cxx
  )
  SET_SOURCE_FILES_PROPERTIES(vtkGdcmReaderPython.cxx GENERATED)
  
  # The C++ files must be made into a C++ library
  #ADD_LIBRARY ( vtkgdcmPython ${vtkgdcmPython_la_SOURCES})

  # set the libraries to link against
  #TARGET_LINK_LIBRARIES(vtkgdcmPython vtkCommon)

  # Configure Python module, which is the plugin itself
  IF (GDCM_WRAP_PYTHON)
    VTK_WRAP_PYTHON2(vtkgdcmPython gdcmfoo
                   ${vtkgdcmPython_la_SOURCES})
    ADD_LIBRARY(vtkgdcmPython SHARED ${gdcmfoo})
    TARGET_LINK_LIBRARIES (vtkgdcmPython
                         pygdcm
                         vtkgdcm
                         vtkCommonPython
                         vtkIOPython)
  ENDIF (GDCM_WRAP_PYTHON)
ENDIF(GDCM_VTK)

#I should install in path of PYTHON_LIBRARY
#GET_FILENAME_COMPONENT(PYTHON_LIB_INSTALL ${PYTHON_LIBRARY} PATH)
#GET_FILENAME_COMPONENT(SITEPACKAGE ${PYTHON_LIB_INSTALL}/../site-packages ABSOLUTE)
#MESSAGE(STATUS ${SITEPACKAGE})
#INSTALL_FILES(/include "\\.h$")
#INSTALL_TARGETS(/lib/ gdcm)

# When installing swig-generated python module one should pay attention
# to library naming convention, see here for more detail:
# http://www.swig.org/Doc1.3/Python.html#n6
# even with the $swig -module mylib, there is always a trailing underscore added
