#-----------------------------------------------------------------------------
# VTK GDCM Reader subdir

#-----------------------------------------------------------------------------
# Include the VTK library
INCLUDE(${VTK_USE_FILE})

# Need gdcm header files
INCLUDE_DIRECTORIES(
  ${GDCM_SOURCE_DIR}/src/
  ${GDCM_BINARY_DIR}/
)

SET(VTKGDCM_LIB_SRCS
  vtkGdcmReader.cxx
  vtkGdcmWriter.cxx
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
SET(vtkgdcmdemo_SOURCES
  vtkGdcmDemo.cxx
)
ADD_EXECUTABLE(vtkgdcmdemo ${vtkgdcmdemo_SOURCES})
TARGET_LINK_LIBRARIES(vtkgdcmdemo
  vtkgdcm
  vtkGraphics
  vtkRendering
)

#-----------------------------------------------------------------------------
SET(vtkgdcmViewer_SOURCES
  vtkgdcmViewer.cxx
)
ADD_EXECUTABLE(vtkgdcmViewer ${vtkgdcmViewer_SOURCES})
TARGET_LINK_LIBRARIES(vtkgdcmViewer
  vtkgdcm
  vtkRendering
)
ADD_EXECUTABLE(vtkgdcmViewer2 vtkgdcmViewer2.cxx)
TARGET_LINK_LIBRARIES(vtkgdcmViewer2
  vtkgdcm
  vtkRendering
)

#-----------------------------------------------------------------------------
SET(vtkWriteDicom_SOURCES
  vtkWriteDicom.cxx
)
ADD_EXECUTABLE(vtkWriteDicom ${vtkWriteDicom_SOURCES})
TARGET_LINK_LIBRARIES(vtkWriteDicom
  vtkgdcm
  vtkIO
  vtkRendering
)

#-----------------------------------------------------------------------------
# Install vtk header
INSTALL_FILES(/include FILES vtkGdcmReader.h)
INSTALL_FILES(/include FILES vtkGdcmWriter.h)

INSTALL_TARGETS(/lib/ vtkgdcm)

