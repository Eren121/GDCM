# JPEG 12 bits project

INCLUDE_DIRECTORIES(
   ${GDCM_BINARY_DIR}/src/jpeg/libijg12
   )
ADD_LIBRARY(gdcmijpeg12 ${JPEG12_SOURCES})
INSTALL_TARGETS(/lib/ gdcmijpeg12)

