INCLUDE_REGULAR_EXPRESSION("^.*$")

# all the include are there:
#INCLUDE_DIRECTORIES(${JASPER_SOURCE_DIR}/src/libjasper/include)

#EXTRA_DIST = README

SET(libmif_la_SOURCES
  #  mif_cod.h
  mif_cod.c
  )

#ADD_LIBRARY(mif ${libmif_la_SOURCES})
