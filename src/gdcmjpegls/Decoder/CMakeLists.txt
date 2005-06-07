INCLUDE_REGULAR_EXPRESSION("^.*$")

CHECK_INCLUDE_FILE("unistd.h"    HAVE_UNISTD_H)
CONFIGURE_FILE(
  "${GDCMJPEGLS_SOURCE_DIR}/Decoder/jpegls_config.h.in"
  "${GDCMJPEGLS_BINARY_DIR}/Decoder/jpegls_config.h"
  @ONLY IMMEDIATE)

INCLUDE_DIRECTORIES("${GDCMJPEGLS_BINARY_DIR}/Decoder")

SET(GDCMJPEGLS_SRCS
  global.c 
  jpegmark.c 
  initialize.c 
  decoder.c 
  lossless_d.c 
  lossy_d.c 
  bitio.c 
  melcode.c)

ADD_LIBRARY(gdcmjpegls ${GDCMJPEGLS_SRCS})

# TODO must do the INSTALL_ blah thing
