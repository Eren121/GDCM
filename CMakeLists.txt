PROJECT(GDCM)
CMAKE_MINIMUM_REQUIRED(VERSION 1.8)

# The GDCM version number.
SET(GDCM_VERSION_MAJOR 0)
SET(GDCM_VERSION_MINOR 4)
SET(GDCM_VERSION_PATCH 0)
SET(GDCM_VERSION "${GDCM_VERSION_MAJOR}.${GDCM_VERSION_MINOR}")
SET(GDCM_VERSION_FULL "${GDCM_VERSION}.${GDCM_VERSION_PATCH}")

#-----------------------------------------------------------------------------
# Output directories.
SET (LIBRARY_OUTPUT_PATH ${GDCM_SOURCE_DIR}/lib CACHE PATH "Single output directory for building all libraries.")
SET (EXECUTABLE_OUTPUT_PATH ${GDCM_SOURCE_DIR}/lib CACHE PATH "Single output directory for building all executables.")
MARK_AS_ADVANCED(LIBRARY_OUTPUT_PATH EXECUTABLE_OUTPUT_PATH)
SET(GDCM_LIBRARY_PATH "${LIBRARY_OUTPUT_PATH}")
SET(GDCM_EXECUTABLE_PATH "${EXECUTABLE_OUTPUT_PATH}")

#-----------------------------------------------------------------------------
# datadir in CMake:
SET(GDCM_DATA_DIR "/share/gdcm" CACHE STRING 
  "Install location for data (relative to prefix).")
MARK_AS_ADVANCED(GDCM_DATA_DIR)

#-----------------------------------------------------------------------------
# Build shared lib by default
OPTION(BUILD_SHARED_LIBS "Build GDCM with shared libraries." ON)
SET(GDCM_BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS})

#INCLUDE_DIRECTORIES(
#)

#-----------------------------------------------------------------------------
# Build directory on which many applications depend
SUBDIRS(
  src
  Test
  Dicts
)

#-----------------------------------------------------------------------------
# Search VTK
FIND_PACKAGE(VTK)
# If vtk found
IF(VTK_FOUND)
  OPTION(GDCM_VTK "Build VTK-GDCM Lib." OFF)
  IF(GDCM_VTK)
    INCLUDE(${VTK_USE_FILE})
    SUBDIRS(vtk)
  ENDIF(GDCM_VTK)
ENDIF(VTK_FOUND)

#-----------------------------------------------------------------------------
# Search Python libs:
FIND_PACKAGE(PythonLibs)
# If PythonLibs found
IF(PYTHON_LIBRARY)
  OPTION(GDCM_WRAP_PYTHON "Wrap GDCM classes into the Python language." OFF)
  #if wrapping enable
  IF(GDCM_WRAP_PYTHON)
    SUBDIRS(gdcmPython)
  ENDIF(GDCM_WRAP_PYTHON)
ENDIF(PYTHON_LIBRARY)

#-----------------------------------------------------------------------------
# Search Doxygen:
FIND_PACKAGE(Doxygen)
IF(DOXYGEN)
  OPTION(GDCM_DOXYGEN "Build source documentation using doxygen." OFF)
  # if doxygen enable
  IF(GDCM_DOXYGEN)
    SUBDIRS(Doc)
  ENDIF(GDCM_DOXYGEN)
ENDIF(DOXYGEN)

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
#  gdcm.dsw

#release:
#	$(MAKE) dist distdir=$(PACKAGE)-$(VERSION)

#snapshot:
#	$(MAKE) dist distdir=$(PACKAGE)`date +"%y%m%d"`
