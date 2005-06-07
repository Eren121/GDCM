INCLUDE_REGULAR_EXPRESSION("^.*$")

# all the include are there:
#INCLUDE_DIRECTORIES(${JASPER_SOURCE_DIR}/src/libjasper/include)

SET(libpnm_la_SOURCES
  #  pnm_cod.h
  #  pnm_enc.h
  pnm_cod.c
  pnm_dec.c
  pnm_enc.c
  )

#ADD_LIBRARY(pnm ${libpnm_la_SOURCES})
