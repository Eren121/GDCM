PROJECT(OPENJPEG)

#-----------------------------------------------------------------------------
# OPENJPEG version number, usefull for packaging and doxygen doc:
SET(OPENJPEG_MAJOR_VERSION 0)
SET(OPENJPEG_MINOR_VERSION 97)
SET(OPENJPEG_BUILD_VERSION 0)
SET(OPENJPEG_VERSION
      "${OPENJPEG_MAJOR_VERSION}.${OPENJPEG_MINOR_VERSION}.${OPENJPEG_BUILD_VERSION}")

#-----------------------------------------------------------------------------
# Build shared lib by default
OPTION(OPENJPEG_BUILD_SHARED_LIBS "Build OPENJPEG with shared libraries." ON)
SET(BUILD_SHARED_LIBS ${OPENJPEG_BUILD_SHARED_LIBS})

#-----------------------------------------------------------------------------
SET (EXECUTABLE_OUTPUT_PATH ${OPENJPEG_BINARY_DIR}/bin CACHE PATH "Single output directory for building all executables.")
SET (LIBRARY_OUTPUT_PATH ${OPENJPEG_BINARY_DIR}/bin CACHE PATH "Single output directory for building all libraries.")
MARK_AS_ADVANCED(LIBRARY_OUTPUT_PATH EXECUTABLE_OUTPUT_PATH)

SUBDIRS(
    libopenjpeg
    codec
    )
