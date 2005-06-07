INCLUDE_REGULAR_EXPRESSION("^.*$")

# all the include are there:
#INCLUDE_DIRECTORIES(${JASPER_SOURCE_DIR}/src/libjasper/include)

SET(libbase_la_SOURCES
  jas_cm.c
  jas_debug.c
  jas_getopt.c
  jas_image.c
  jas_icc.c
  jas_iccdata.c
  jas_init.c
  jas_malloc.c
  jas_seq.c
  jas_stream.c
  jas_string.c
  jas_tvp.c
  jas_version.c
  )
#ADD_LIBRARY(base ${libbase_la_SOURCES} )
