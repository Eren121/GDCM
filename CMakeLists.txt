PROJECT(GDCM)
CMAKE_MINIMUM_REQUIRED(VERSION 1.6)

# The GDCM version number.
SET(GDCM_VERSION_MAJOR 1)
SET(GDCM_VERSION_MINOR 9)
SET(GDCM_VERSION_PATCH 0)
SET(GDCM_VERSION "${GDCM_VERSION_MAJOR}.${GDCM_VERSION_MINOR}")
SET(GDCM_VERSION_FULL "${GDCM_VERSION}.${GDCM_VERSION_PATCH}")

SET(GDCM_DATA_DIR "/share/gdcm" CACHE STRING 
  "Install location for data (relative to prefix).")
MARK_AS_ADVANCED(GDCM_DATA_DIR)


OPTION(BUILD_SHARED_LIBS "Build GDCM with shared libraries." ON)
SET(GDCM_BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS})

INCLUDE_DIRECTORIES(
)

#Build directory on which many applications depend
SUBDIRS(src
        Test
        Dicts)

FIND_PACKAGE(VTK)
#If vtk found
IF(VTK_FOUND)
  OPTION(GDCM_VTK "Build VTK-GDCM Lib." OFF)
  IF(GDCM_VTK)
    INCLUDE(${VTK_USE_FILE})
    SUBDIRS(vtk)
  ENDIF(GDCM_VTK)
ENDIF(VTK_FOUND)

FIND_PACKAGE(PythonLibs)
#If PythonLibs found
IF(PYTHON_LIBRARY)
  OPTION(GDCM_WRAP_PYTHON "Wrap GDCM classes into the Python language." OFF)
  #if wrapping enable
  IF(GDCM_WRAP_PYTHON)
    SUBDIRS(gdcmPython)
  ENDIF(GDCM_WRAP_PYTHON)
ENDIF(PYTHON_LIBRARY)

#Doxygen stuff
#  SUBDIRS(Doc)


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
