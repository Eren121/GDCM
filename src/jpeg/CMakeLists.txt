SUBDIRS(
  libijg12 
  libijg8
  ljpg
)

# JPEG 8 bits project

#PROJECT(GDCMJPEG)
#INCLUDE_REGULAR_EXPRESSION("^(jchuff|jconfig|jdct|jdhuff|jerror|jinclude|jmemsys|jmorecfg|jpegint|jpeglib|jversion|jpeg|gdcm).*$")
#INCLUDE_DIRECTORIES(${GDCMJPEG_SOURCE_DIR})


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

SET(JPEG_HEADER_FILES
  jchuff.h  
  jdct.h  
  jdhuff.h  
  jerror.h  
  jinclude.h  
  jmemsys.h  
#  jmorecfg.h     #no this one need to be configured
  jpegint.h  
  jpeglib.h  
  jversion.h
  )

SET(JPEG_SOURCES
  ${systemdependent_SRCS}
  ${common_SRCS}
  ${compression_SRCS}
  ${decompression_SRCS}
  )

IF (WIN32)
  IF (BUILD_SHARED_LIBS)
    ADD_DEFINITIONS(-DJPEGDLL)
  ELSE (BUILD_SHARED_LIBS)
    ADD_DEFINITIONS(-DJPEGSTATIC)
  ENDIF (BUILD_SHARED_LIBS)
ENDIF (WIN32)

# Attempt to generate two libraries: jpeg 8bits and 12 bits using only one
# source directory

SET(JPEG8_SOURCES)
SET(JPEG12_SOURCES)
#  MESSAGE("WOOOO")
FOREACH(file ${JPEG_SOURCES})
  # Duplicate jpeg source in subdir libijg8
  CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/libijg/${file}"
    "${CMAKE_CURRENT_BINARY_DIR}/libijg8/${file}"
    )
  SET(JPEG8_SOURCES ${JPEG8_SOURCES} "${CMAKE_CURRENT_BINARY_DIR}/libijg8/${file}")

  SET(BITS_TYPE_JPEG_MANGLE_NAME gdcm_mangle_8bits.h)
  CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/libijg/jconfig.h.in
                 ${CMAKE_CURRENT_BINARY_DIR}/libijg8/jconfig.h @ONLY IMMEDIATE)
  CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/libijg/gdcm_mangle_8bits.h
                 ${CMAKE_CURRENT_BINARY_DIR}/libijg8/gdcm_mangle_8bits.h @ONLY IMMEDIATE)

  #MESSAGE("${CMAKE_CURRENT_BINARY_DIR}/libijg8/${file}")
  # Duplicate jpeg source in subdir libijg12
  CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/libijg/${file}"
    "${CMAKE_CURRENT_BINARY_DIR}/libijg12/${file}"
    )
  SET(JPEG12_SOURCES ${JPEG12_SOURCES} "${CMAKE_CURRENT_BINARY_DIR}/libijg12/${file}")

  SET(BITS_TYPE_JPEG_MANGLE_NAME gdcm_mangle_12bits.h)
  CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/libijg/jconfig.h.in
                 ${CMAKE_CURRENT_BINARY_DIR}/libijg12/jconfig.h @ONLY IMMEDIATE)
  CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/libijg/gdcm_mangle_12bits.h
                 ${CMAKE_CURRENT_BINARY_DIR}/libijg12/gdcm_mangle_12bits.h @ONLY IMMEDIATE)
ENDFOREACH(file)

# Now copy the header file
FOREACH(file ${JPEG_HEADER_FILES})
  CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/libijg/${file}"
    "${CMAKE_CURRENT_BINARY_DIR}/libijg8/${file}"
    )
  CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/libijg/${file}"
    "${CMAKE_CURRENT_BINARY_DIR}/libijg12/${file}"
    )
ENDFOREACH(file)
# Particular case jmorecfg.h needs to be specified either 8 or 12 bits:
SET(GDCM_BITS_IN_JSAMPLE 8)
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/libijg/jmorecfg.h
               ${CMAKE_CURRENT_BINARY_DIR}/libijg8/jmorecfg.h @ONLY IMMEDIATE)
SET(GDCM_BITS_IN_JSAMPLE 12)
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/libijg/jmorecfg.h
               ${CMAKE_CURRENT_BINARY_DIR}/libijg12/jmorecfg.h @ONLY IMMEDIATE)

#  MESSAGE("${JPEG8_SOURCES}")

#ADD_LIBRARY(gdcmijpeg8 ${systemdependent_SRCS} ${common_SRCS} ${compression_SRCS} ${decompression_SRCS})
#ADD_LIBRARY(gdcmijpeg8 ${JPEG8_SOURCES})
#ADD_LIBRARY(gdcmijpeg12 ${JPEG12_SOURCES})

#INSTALL_TARGETS(/lib/ gdcmijpeg8)
#INSTALL_TARGETS(/lib/ gdcmijpeg12)
#INSTALL_FILES(/include .h jconfig.linux jconfig.vc)
