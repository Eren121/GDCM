# JPEG 8 bits project

INCLUDE_DIRECTORIES(
   ${GDCM_BINARY_DIR}/src/jpeg/libijg8
   )
ADD_LIBRARY(gdcmijpeg8 ${JPEG8_SOURCES})
INSTALL_TARGETS(/lib/ gdcmijpeg8)

