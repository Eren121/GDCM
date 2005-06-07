INCLUDE_REGULAR_EXPRESSION("^.*$")

# all the include are there:
#INCLUDE_DIRECTORIES(${JASPER_SOURCE_DIR}/src/libjasper/include)

SET(libbmp_la_SOURCES
  #  bmp_cod.h
  #  bmp_enc.h
  bmp_cod.c
  bmp_dec.c
  bmp_enc.c
  )

#ADD_LIBRARY(bmp ${libbmp_la_SOURCES})
