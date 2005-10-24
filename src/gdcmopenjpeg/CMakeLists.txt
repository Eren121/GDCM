PROJECT(OPENJPEG)

#-----------------------------------------------------------------------------
# OPENJPEG version number, usefull for packaging and doxygen doc:
SET(OPENJPEG_MAJOR_VERSION 0)
SET(OPENJPEG_MINOR_VERSION 97)
SET(OPENJPEG_BUILD_VERSION 0)
SET(OPENJPEG_VERSION
      "${OPENJPEG_MAJOR_VERSION}.${OPENJPEG_MINOR_VERSION}.${OPENJPEG_BUILD_VERSION}")


#-----------------------------------------------------------------------------
SUBDIRS(
    libopenjpeg
    #codec
    )

# TODO, technically we should not have to ...
# http://www.crc.ricoh.com/~gormish/jpeg2000conformance/
