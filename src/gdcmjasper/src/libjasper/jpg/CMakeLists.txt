INCLUDE_REGULAR_EXPRESSION("^.*$")

#EXTRA_DIST = README

IF(HAVE_LIBJPEG)
  SET(MISCSOURCES
    jpg_dec.c
    jpg_enc.c)
ELSE(HAVE_LIBJPEG)
  SET(MISCSOURCES
    jpg_dummy.c)
ENDIF(HAVE_LIBJPEG)

SET(libjpg_la_SOURCES
  #  jpg_cod.h
  #  jpg_enc.h
  #  jpg_jpeglib.h
  jpg_val.c
  ${MISCSOURCES}
  )

