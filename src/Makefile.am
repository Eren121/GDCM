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

libgdcm_la_SOURCES=		\
	gdcmUtil.cxx 	\
	gdcmHeader.cxx    \
	gdcmElValue.cxx   \
	gdcmDictEntry.cxx \
	gdcmDict.cxx      \
	gdcmDictSet.cxx   \
	gdcmElValSet.cxx  \
	gdcmHeaderIdo.cxx \
	gdcmFile.cxx

libgdcmincludedir = $(includedir)
libgdcminclude_HEADERS = 	\
	config.h	\
	gdcm.h  	\
	gdcmIdo.h  	\
	gdcmUtil.h
