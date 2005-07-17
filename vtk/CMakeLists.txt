#-----------------------------------------------------------------------------
# VTK GDCM Reader subdir
# Rebuild gdcm whenever a file starting with vtk* is modified
INCLUDE_REGULAR_EXPRESSION("^vtk.*$")

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

# let's do some APPLE/WIN32 magic here
#IF(WIN32)
#  SET(GUI_EXECUTABLE WIN32)
#ENDIF(WIN32)
IF(APPLE)
  IF(VTK_USE_CARBON)
    SET(GUI_EXECUTABLE MACOSX_BUNDLE)
  ENDIF(VTK_USE_CARBON)
  IF(VTK_USE_COCOA)
    SET(GUI_EXECUTABLE MACOSX_BUNDLE)
  ENDIF(VTK_USE_COCOA)
  # when using X11 on Apple you don't need this...
ENDIF(APPLE)

ADD_EXECUTABLE(vtkgdcmViewer ${GUI_EXECUTABLE} ${vtkgdcmViewer_SOURCES})
TARGET_LINK_LIBRARIES(vtkgdcmViewer
  vtkgdcm
  vtkRendering
)
ADD_EXECUTABLE(vtkgdcmViewer2 ${GUI_EXECUTABLE} vtkgdcmViewer2.cxx)
TARGET_LINK_LIBRARIES(vtkgdcmViewer2
  vtkgdcm
  vtkRendering
)
ADD_EXECUTABLE(vtkgdcmSerieViewer ${GUI_EXECUTABLE} vtkgdcmSerieViewer.cxx)
TARGET_LINK_LIBRARIES(vtkgdcmSerieViewer
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
INSTALL_FILES(/include/gdcm FILES vtkGdcmReader.h)
INSTALL_FILES(/include/gdcm FILES vtkGdcmWriter.h)

INSTALL_TARGETS(/lib/gdcm/ vtkgdcm)
INSTALL_TARGETS(/bin/ vtkgdcmViewer)
INSTALL_TARGETS(/bin/ vtkgdcmViewer2)
INSTALL_TARGETS(/bin/ vtkgdcmSerieViewer)
