#
# Targets
#

# Before making a release, the LTVERSION string should be modified.
# The string is of the form C:R:A.
# - If interfaces have been changed or added, but binary compatibility has
#   been preserved, change to C+1:0:A+1
# - If binary compatibility has been broken (eg removed or changed interfaces)
#   change to C+1:0:0
# - If the interface is the same as the previous version, change to C:R+1:A

LTVERSION = 0:1:0

lib_LTLIBRARIES = libgdcm.la

CXXFLAGS = @CXXFLAGS@ -DPUB_DICT_PATH=\"$(datadir)/gdcm/\" \
           -D__STDC_LIMIT_MACROS  # Refer to stdint.h

libgdcm_la_SOURCES=		\
	gdcmException.cxx	\
	gdcmUtil.cxx 		\
	gdcmHeader.cxx    \
	gdcmElValue.cxx   \
	gdcmDictEntry.cxx \
	gdcmDict.cxx      \
	gdcmDictSet.cxx   \
	gdcmElValSet.cxx  \
	gdcmHeaderIdo.cxx \
	gdcmFile.cxx	\
	gdcmVR.cxx	\
	gdcmJpeg.cxx

libgdcmincludedir = $(includedir)
libgdcminclude_HEADERS = \
	gdcmException.h	\
	gdcmHeader.h  	\
	gdcmCommon.h	\
	gdcm.h  	\
	gdcmDictSet.h	\
	gdcmDict.h	\
	gdcmDictEntry.h	\
	gdcmElValue.h	\
	gdcmElValSet.h	\
	gdcmVR.h	\
	gdcmFile.h

EXTRA_DIST = \
	gdcmUtil.h	\
	gdcmIdo.h  	\
	iddcmjpeg.h
