#IF(GDCM_WRAP_PYTHON)
#If we reach this dir we have selected this option


#TODO
#http://www.cmake.org/pipermail/cmake/2003-August/004185.html
#http://www.cmake.org/pipermail/cmake/2003-January/001092.html

#As of 09/10/2003 there was no FindSWIG.cmake

#gdcm_wrap.cxx: $(srcdir)/gdcm.i
#	$(SWIG)  -python -c++ $(SWIG_INC) -o $@ $<

INCLUDE_DIRECTORIES(
  ${GDCM_SOURCE_DIR}/src
)

SET(GDCM_INC
  ${GDCM_SOURCE_DIR}/src
#  -I$(top_builddir)
)

SET(SWIG_INC
  ${PYTHON_INCLUDES}
  ${GDCM_INC}
)


SET(GDCM_PYTHON_SOURCES
  gdcm_wrap.cxx
)

SET_SOURCE_FILES_PROPERTIES(gdcm_wrap.cxx GENERATED)

ADD_CUSTOM_COMMAND(
SOURCE    ${GDCM_SOURCE_DIR}/gdcmPython/gdcm.i
COMMAND   swig
ARGS      -python -c++ -I${GDCM_SOURCE_DIR}/src -o gdcm_wrap.cxx ${GDCM_SOURCE_DIR}/gdcmPython/gdcm.i 
OUTPUTS   gdcm_wrap.cxx
TARGET    pygdcm
)

ADD_LIBRARY(pygdcm ${GDCM_PYTHON_SOURCES})



IF(GDCM_VTK)
#  INCLUDE_DIRECTORIES(
#    ${GDCM_SOURCE_DIR}/vtk
#  )

  SET(vtkgdcmPython_la_SOURCES
#      vtkGdcmReaderPython.cxx
#      vtkGdcmInit.cxx
      ${GDCM_SOURCE_DIR}/vtk/vtkGdcmReader.cxx
  )
  SET_SOURCE_FILES_PROPERTIES(vtkGdcmReaderPython.cxx GENERATED)
  #vtkWrapPythonInit.c
  
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
