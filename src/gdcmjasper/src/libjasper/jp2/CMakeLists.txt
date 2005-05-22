# all the include are there:
INCLUDE_DIRECTORIES(${JASPER_SOURCE_DIR}/src/libjasper/include)

SET(libjp2_la_SOURCES
  #  jp2_cod.h
  #  jp2_dec.h
  jp2_cod.c
  jp2_dec.c
  jp2_enc.c
  )
ADD_LIBRARY(jp2 ${libjp2_la_SOURCES})

