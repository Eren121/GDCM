# JPEG 16 bits project
# There is no 16bits DCT alorithm thus this is only for the lossless jpeg.

INCLUDE_DIRECTORIES(
   ${GDCM_BINARY_DIR}/src/jpeg/libijg16
   )
ADD_LIBRARY(gdcmijpeg16 ${JPEG16_SOURCES})
INSTALL_TARGETS(/lib/ gdcmijpeg16)

