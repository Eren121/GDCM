SET(libgdcmjpg_la_SOURCES
 decomp.c
 huffd.c
 jpegutil.c
 mcu.c
 predict.c
 read.c
)

SET(libgdcmjpginclude_HEADERS
 io.h
 jpeg.h
 jpegless.h
 mcu.h
 predict.h
 proto.h
)

ADD_LIBRARY(gdcmljpeg ${libgdcmjpg_la_SOURCES})

INSTALL_FILES(/include FILES ${libgdcmjpeginclude_HEADERS})
INSTALL_TARGETS(/lib/ gdcmljpeg)