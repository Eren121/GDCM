SUBDIRS(jpeg)

#CXXFLAGS = @CXXFLAGS@ -DPUB_DICT_PATH=\"$(datadir)/gdcm/\" \
#           -D__STDC_LIMIT_MACROS  # Refer to stdint.h


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
	gdcmJpegIdo.cxx
)
#Why one should not use ADD_DEFINITIONS
#http://www.cmake.org/pipermail/cmake/2003-September/004286.html
#ADD_DEFINITIONS(-DPUB_DICT_PATH=\\"${CMAKE_INSTALL_PREFIX}${GDCM_DATA_DIR}/\\")
#ADD_DEFINITIONS(-D__STDC_LIMIT_MACROS)

SET_SOURCE_FILES_PROPERTIES(${libgdcm_la_SOURCES} 
  PROPERTIES 
  COMPILE_FLAGS  -DPUB_DICT_PATH=\\"${CMAKE_INSTALL_PREFIX}${GDCM_DATA_DIR}\\"
  COMPILE_FLAGS  -D__STDC_LIMIT_MACROS
)


#libgdcminclude_HEADERS =   \
#	gdcmException.h	   \
#	gdcmHeader.h	   \
#	gdcmHeaderHelper.h \
#	gdcmCommon.h       \
#	gdcm.h             \
#	gdcmDictSet.h      \
#	gdcmDict.h         \
#	gdcmDictEntry.h    \
#	gdcmElValue.h      \
#	gdcmElValSet.h     \
#	gdcmVR.h           \
#	gdcmTS.h           \
#	gdcmFile.h

#EXTRA_DIST =            \
#	gdcmUtil.h      \
#	gdcmIdo.h       \
#	iddcmjpeg.h




ADD_LIBRARY(gdcm ${libgdcm_la_SOURCES} )
#TODO this is not working out of the box (on the first time)
IF(WIN32)
  TARGET_LINK_LIBRARIES(gdcm 
    gdcmijpeg8 
    gdcmijpeg12
    Wsock32.lib
  )
ELSE(WIN32)
  TARGET_LINK_LIBRARIES(gdcm 
    gdcmijpeg8 
    gdcmijpeg12
  )
ENDIF(WIN32)