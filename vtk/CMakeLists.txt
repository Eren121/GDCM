#-----------------------------------------------------------------------------
# VTK GDCM Reader subdir

#-----------------------------------------------------------------------------
# Need gdcm header files
INCLUDE_DIRECTORIES(
  ${GDCM_SOURCE_DIR}/src/
)

SET(VTKGDCM_LIB_SRCS
  vtkGdcmReader.cxx
)

#-----------------------------------------------------------------------------
# Create vtkgdcm library
ADD_LIBRARY(vtkgdcm ${VTKGDCM_LIB_SRCS})
TARGET_LINK_LIBRARIES(vtkgdcm 
  gdcm
  vtkCommon
  vtkIO
  vtkFiltering
)

#-----------------------------------------------------------------------------
# See with frog why m4 didn't link to lib, and include cxx file instead ?
SET(vtkgdcmdemo_SOURCES
  testvtkGdcmReader.cxx
#  vtkGdcmReader.cxx
)
ADD_EXECUTABLE(vtkgdcmdemo ${vtkgdcmdemo_SOURCES})
TARGET_LINK_LIBRARIES(vtkgdcmdemo 
  vtkgdcm
  vtkGraphics
  vtkRendering
)

#-----------------------------------------------------------------------------
# See with frog why m4 didn't link to lib, and include cxx file instead ?
SET(vtkgdcmViewer_SOURCES
  vtkgdcmViewer.cxx
  #vtkGdcmReader.cxx 
)
ADD_EXECUTABLE(vtkgdcmViewer ${vtkgdcmViewer_SOURCES})
TARGET_LINK_LIBRARIES(vtkgdcmViewer 
  vtkgdcm
  vtkRendering
)

# Install vtk header
INSTALL_FILES(/include FILES vtkGdcmReader.h)