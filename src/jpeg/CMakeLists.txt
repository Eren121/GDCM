SUBDIRS(
  libijg16 
  libijg12 
  libijg8
)

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
)

# Lossy (DCT) codec:
SET(comp_lossy_SRCS
jfdctint.c
jclossy.c

# lossless
jclossls.c

jdarith.c
jcodec.c
jcscale.c
jcshuff.c
jclhuff.c
jcpred.c
jcdiffct.c
)

# decompression library object files
SET(decompression_SRCS
jdapimin.c jdapistd.c jdtrans.c jdatasrc.c jdmaster.c
jdinput.c jdmarker.c jdhuff.c jdphuff.c jdmainct.c jdcoefct.c
jdpostct.c jddctmgr.c jidctfst.c jidctflt.c jidctint.c jidctred.c
jdsample.c jdcolor.c jquant1.c jquant2.c jdmerge.c)

SET(decomp_lossy_SRCS
 jdlossls.c jdlossy.c
jcarith.c
jdlhuff.c
jddiffct.c
jdpred.c
jdscale.c
jdshuff.c
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
jlossy.h
jlossls.h
  )

SET(JPEG_SOURCES
  ${systemdependent_SRCS}
  ${common_SRCS}
  ${compression_SRCS}
  ${decompression_SRCS}

${comp_lossy_SRCS}
${decomp_lossy_SRCS}
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
SET(JPEG16_SOURCES)
FOREACH(file ${JPEG_SOURCES})
  # Duplicate jpeg source in subdir libijg8
  CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/libijg/${file}"
    "${CMAKE_CURRENT_BINARY_DIR}/libijg8/${file}"
    )
  SET(JPEG8_SOURCES ${JPEG8_SOURCES} "${CMAKE_CURRENT_BINARY_DIR}/libijg8/${file}")

  # Duplicate jpeg source in subdir libijg12
  CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/libijg/${file}"
    "${CMAKE_CURRENT_BINARY_DIR}/libijg12/${file}"
    )
  SET(JPEG12_SOURCES ${JPEG12_SOURCES} "${CMAKE_CURRENT_BINARY_DIR}/libijg12/${file}")

  # Duplicate jpeg source in subdir libijg16
  CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/libijg/${file}"
    "${CMAKE_CURRENT_BINARY_DIR}/libijg16/${file}"
    )
  SET(JPEG16_SOURCES ${JPEG16_SOURCES} "${CMAKE_CURRENT_BINARY_DIR}/libijg16/${file}")

ENDFOREACH(file)

# Now copy the header files
FOREACH(file ${JPEG_HEADER_FILES})
  CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/libijg/${file}"
    "${CMAKE_CURRENT_BINARY_DIR}/libijg8/${file}"
    )
  CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/libijg/${file}"
    "${CMAKE_CURRENT_BINARY_DIR}/libijg12/${file}"
    )
  CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/libijg/${file}"
    "${CMAKE_CURRENT_BINARY_DIR}/libijg16/${file}"
    )
ENDFOREACH(file)

# Particular case jmorecfg.h needs to be specified either 8, 12 or 16 bits:
SET(GDCM_BITS_IN_JSAMPLE 8)
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/libijg/jmorecfg.h
               ${CMAKE_CURRENT_BINARY_DIR}/libijg8/jmorecfg.h @ONLY IMMEDIATE)
SET(BITS_TYPE_JPEG_MANGLE_NAME gdcm_mangle_8bits.h)
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/libijg/jconfig.h.in
               ${CMAKE_CURRENT_BINARY_DIR}/libijg8/jconfig.h @ONLY IMMEDIATE)
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/libijg/gdcm_mangle_8bits.h
               ${CMAKE_CURRENT_BINARY_DIR}/libijg8/gdcm_mangle_8bits.h @ONLY IMMEDIATE)



SET(GDCM_BITS_IN_JSAMPLE 12)
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/libijg/jmorecfg.h
               ${CMAKE_CURRENT_BINARY_DIR}/libijg12/jmorecfg.h @ONLY IMMEDIATE)
SET(BITS_TYPE_JPEG_MANGLE_NAME gdcm_mangle_12bits.h)
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/libijg/jconfig.h.in
               ${CMAKE_CURRENT_BINARY_DIR}/libijg12/jconfig.h @ONLY IMMEDIATE)
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/libijg/gdcm_mangle_12bits.h
               ${CMAKE_CURRENT_BINARY_DIR}/libijg12/gdcm_mangle_12bits.h @ONLY IMMEDIATE)



SET(GDCM_BITS_IN_JSAMPLE 16)
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/libijg/jmorecfg.h
               ${CMAKE_CURRENT_BINARY_DIR}/libijg16/jmorecfg.h @ONLY IMMEDIATE)
SET(BITS_TYPE_JPEG_MANGLE_NAME gdcm_mangle_16bits.h)
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/libijg/jconfig.h.in
               ${CMAKE_CURRENT_BINARY_DIR}/libijg16/jconfig.h @ONLY IMMEDIATE)
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/libijg/gdcm_mangle_16bits.h
               ${CMAKE_CURRENT_BINARY_DIR}/libijg16/gdcm_mangle_16bits.h @ONLY IMMEDIATE)
