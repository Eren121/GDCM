#-----------------------------------------------------------------------------
# Build jpeg lib first:
SUBDIRS(jpeg)

# "jpeglib.h" is defined here:
INCLUDE_DIRECTORIES(
  ${GDCM_SOURCE_DIR}/jpeg/libijg8
  #${GDCM_SOURCE_DIR}/jpeg/libijg12 #all files have been renamed, appending 12
  #${GDCM_SOURCE_DIR}/jpeg/ljpg     # JPEG lib from xmedcom 
)

SET(libgdcm_la_SOURCES
	gdcmException.cxx
	gdcmUtil.cxx
	gdcmHeader.cxx
	gdcmHeaderHelper.cxx
	gdcmElValue.cxx
	gdcmDictEntry.cxx
	gdcmDict.cxx
	gdcmDictSet.cxx
	gdcmElValSet.cxx
	gdcmHeaderIdo.cxx
	gdcmFile.cxx
	gdcmVR.cxx
	gdcmTS.cxx
	gdcmJpeg.cxx
	gdcmJpeg12.cxx
	gdcmJpeg2000.cxx
	gdcmRLE.cxx
	gdcmParse.cxx
)
#-----------------------------------------------------------------------------
# Why one should not use ADD_DEFINITIONS
# http://www.cmake.org/pipermail/cmake/2003-September/004286.html

#-----------------------------------------------------------------------------
# __STDC_LIMIT_MACROS -> Refer to stdint.h
# http://www.cmake.org/pipermail/cmake/2003-October/004369.html
# http://www.cmake.org/pipermail/cmake/2003-October/004373.html

SET_SOURCE_FILES_PROPERTIES(${libgdcm_la_SOURCES}
  PROPERTIES
  COMPILE_FLAGS "-D__STDC_LIMIT_MACROS  -DPUB_DICT_PATH=\\\"${CMAKE_INSTALL_PREFIX}${GDCM_DATA_DIR}\\\""
)

ADD_LIBRARY(gdcm ${libgdcm_la_SOURCES} )
#TODO this is not working out of the box (on the first time)
IF(UNIX)
  TARGET_LINK_LIBRARIES(gdcm 
    gdcmijpeg8 
    gdcmijpeg12
    gdcmjpeg     # JPEG lib from xmedcom 
  )
ELSE(UNIX)
  TARGET_LINK_LIBRARIES(gdcm 
    gdcmijpeg8 
    gdcmijpeg12
    gdcmjpeg
    Wsock32.lib   #doesn't exist on cygwin
  )
ENDIF(UNIX)

#The following is not working because when a header file is not found it tries 
#to find one in the binary dir
#INSTALL_FILES(/include .h ${libgdcm_la_SOURCES})
#INSTALL_FILES(/include FILES gdcmIdo.h iddcmjpeg.h)
INSTALL_FILES(/include "\\.h$")

INSTALL_TARGETS(/lib/ gdcm)
#INSTALL_FILES(/include/vtk .h ${Kit_SRCS})