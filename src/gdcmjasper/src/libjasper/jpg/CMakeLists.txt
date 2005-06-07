INCLUDE_REGULAR_EXPRESSION("^.*$")

# all the include are there:
#INCLUDE_DIRECTORIES(${JASPER_SOURCE_DIR}/src/libjasper/include)

#EXTRA_DIST = README

#if HAVE_LIBJPEG
#MISCSOURCES = jpg_dec.c jpg_enc.c
#else
#MISCSOURCES = jpg_dummy.c
#endif

SET(libjpg_la_SOURCES
  #  jpg_cod.h
  #  jpg_enc.h
  #  jpg_jpeglib.h
  #  $(MISCSOURCES)
  jpg_val.c
  #  )
  #SET(EXTRA_libjpg_la_SOURCES
  #  jpg_dec.c
  #  jpg_enc.c
  jpg_dummy.c
  )

#ADD_LIBRARY(jpg ${libjpg_la_SOURCES} ${EXTRA_libjpg_la_SOURCES})

