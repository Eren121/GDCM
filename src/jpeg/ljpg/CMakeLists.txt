#LTVERSION = 0:1:0

#lib_LTLIBRARIES = libgdcmjpg.la

##CXXFLAGS = @CXXFLAGS@

SET(libgdcmjpg_la_SOURCES
 decomp.c   \
 huffd.c    \
 jpegutil.c \
 mcu.c      \
 predict.c  \
 read.c
)

#libgdcmjpgincludedir = $(includedir)
SET(llibgdcmjpginclude_HEADERS
 io.h       \
 jpeg.h     \
 jpegless.h \
 mcu.h      \
 predict.h  \
 proto.h
)

#EXTRA_DIST = \
#   jconfig.linux \
#   jconfig.vc

ADD_LIBRARY(gdcmjpeg ${libgdcmjpeg_la_SOURCES})

#INSTALL_FILES(/include "\\.h$")
INSTALL_FILES(/include FILES ${libgdcmjpeginclude_HEADERS})
#INSTALL_FILES(/include FILES jconfig.linux jconfig.vc)
INSTALL_FILES(/include FILES jconfig.linux)

INSTALL_TARGETS(/lib/ gdcmjpeg)