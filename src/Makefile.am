SUBDIRS = jpeg

LTVERSION = 0:1:0

lib_LTLIBRARIES = libgdcm.la

CXXFLAGS = @CXXFLAGS@ -DPUB_DICT_PATH=\"$(datadir)/gdcm/\" \
           -D__STDC_LIMIT_MACROS  # Refer to stdint.h

libgdcm_la_LIBADD = \
   $(top_builddir)/src/jpeg/libijg12/libgdcmijpeg12.la  \
   $(top_builddir)/src/jpeg/libijg8/libgdcmijpeg8.la    \
   $(top_builddir)/src/jpeg/ljpg/libgdcmjpg.la 

libgdcm_la_SOURCES=		\
	gdcmException.cxx	\
	gdcmUtil.cxx 		\
	gdcmHeader.cxx    \
	gdcmHeaderHelper.cxx    \
	gdcmElValue.cxx   \
	gdcmDictEntry.cxx \
	gdcmDict.cxx      \
	gdcmDictSet.cxx   \
	gdcmElValSet.cxx  \
	gdcmHeaderIdo.cxx \
	gdcmFile.cxx	 \
	gdcmVR.cxx	 \
	gdcmTS.cxx	 \
	gdcmJpeg.cxx	 \
	gdcmJpeg12.cxx	 \
	gdcmJpeg2000.cxx \
	gdcmRLE.cxx      \
        gdcmParse.cxx

libgdcmincludedir = $(includedir)
libgdcminclude_HEADERS =   \
	gdcmException.h	   \
	gdcmHeader.h	   \
	gdcmHeaderHelper.h \
	gdcmCommon.h       \
	gdcm.h             \
	gdcmDictSet.h      \
	gdcmDict.h         \
	gdcmDictEntry.h    \
	gdcmElValue.h      \
	gdcmElValSet.h     \
	gdcmVR.h           \
	gdcmTS.h           \
	gdcmFile.h

EXTRA_DIST =            \
	gdcmUtil.h      

