INCLUDE_REGULAR_EXPRESSION("^.*$")

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
