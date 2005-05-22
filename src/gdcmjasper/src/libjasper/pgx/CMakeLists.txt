# all the include are there:
INCLUDE_DIRECTORIES(${JASPER_SOURCE_DIR}/src/libjasper/include)

SET(libpgx_la_SOURCES
  #  pgx_cod.h
  #  pgx_enc.h
  pgx_cod.c
  pgx_dec.c
  pgx_enc.c
  )

ADD_LIBRARY(pgx ${libpgx_la_SOURCES})
