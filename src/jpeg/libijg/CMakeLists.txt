# JPEG 8 bits project

PROJECT(GDCMJPEG)
INCLUDE_REGULAR_EXPRESSION("^(jchuff|jconfig|jdct|jdhuff|jerror|jinclude|jmemsys|jmorecfg|jpegint|jpeglib|jversion|jpeg|gdcm).*$")
INCLUDE_DIRECTORIES(${GDCMJPEG_SOURCE_DIR})


# memmgr back ends: compile only one of these into a working library
# (For now, let's use the mode that requires the image fit into memory.
# This is the recommended mode for Win32 anyway.)
SET(systemdependent_SRCS jmemnobs.c)

# library object files common to compression and decompression
SET(common_SRCS
jcomapi.c jutils.c jerror.c jmemmgr.c
)

# compression library object files
SET(compression_SRCS
jcapimin.c jcapistd.c jctrans.c jcparam.c jdatadst.c jcinit.c
jcmaster.c jcmarker.c jcmainct.c jcprepct.c jccoefct.c jccolor.c
jcsample.c jchuff.c jcphuff.c jcdctmgr.c jfdctfst.c jfdctflt.c
jfdctint.c
)

# decompression library object files
SET(decompression_SRCS
jdapimin.c jdapistd.c jdtrans.c jdatasrc.c jdmaster.c
jdinput.c jdmarker.c jdhuff.c jdphuff.c jdmainct.c jdcoefct.c
jdpostct.c jddctmgr.c jidctfst.c jidctflt.c jidctint.c jidctred.c
jdsample.c jdcolor.c jquant1.c jquant2.c jdmerge.c
)

ADD_LIBRARY(gdcmijpeg8 ${systemdependent_SRCS} ${common_SRCS} ${compression_SRCS} ${decompression_SRCS})

INSTALL_TARGETS(/lib/ gdcmijpeg8)
INSTALL_FILES(/include .h jconfig.linux jconfig.vc)

# Attempt to generate two libraries: jpeg 8bits and 12 bits using only one
# source directory

# For now disable it

#extremly dangerous as there could be a /usr/include/jconfig.h from regular jpeg lib
#INCLUDE_DIRECTORIES(${GDCMJPEG_BINARY_DIR})

SET(BITS_TYPE_JPEG_MANGLE_NAME gdcm_mangle_8bits.h)
CONFIGURE_FILE(${GDCMJPEG_SOURCE_DIR}/jconfig.h.in
               ${GDCMJPEG_BINARY_DIR}/jconfig.h @ONLY)
#ADD_LIBRARY(gdcmijpeg8 ${systemdependent_SRCS} ${common_SRCS} ${compression_SRCS} ${decompression_SRCS})

SET(BITS_TYPE_JPEG_MANGLE_NAME gdcm_mangle_12bits.h)
CONFIGURE_FILE(${GDCMJPEG_SOURCE_DIR}/jconfig.h.in
               ${GDCMJPEG_BINARY_DIR}/jconfig.h @ONLY)
#ADD_LIBRARY(gdcmijpeg12 ${systemdependent_SRCS} ${common_SRCS} ${compression_SRCS} ${decompression_SRCS})
