#VTK GDCM subdir

INCLUDE_DIRECTORIES(
  ${GDCM_SOURCE_DIR}/src/
)

SET(VTKGDCM_LIB_SRCS
  vtkGdcmReader.cxx
)

ADD_LIBRARY(vtkgdcm ${VTKGDCM_LIB_SRCS})
TARGET_LINK_LIBRARIES(vtkgdcm 
  gdcm
  vtkCommon
  vtkIO
  vtkFiltering
)

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

SET(vtkgdcmViewer_SOURCES
  vtkgdcmViewer.cxx
  #vtkGdcmReader.cxx 
)
ADD_EXECUTABLE(vtkgdcmViewer ${vtkgdcmViewer_SOURCES})
TARGET_LINK_LIBRARIES(vtkgdcmViewer 
  vtkgdcm
  vtkRendering
)