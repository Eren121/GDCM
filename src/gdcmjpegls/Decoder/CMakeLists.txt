INCLUDE_REGULAR_EXPRESSION("^.*$")

INCLUDE_DIRECTORIES("${GDCMJPEGLS_BINARY_DIR}")

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
#ADD_EXECUTABLE(locod ${GDCMJPEGLS_SRCS})

# TODO must do the INSTALL_ blah thing
