SET(GDCM_MPEG2_SOURCES
  getpic.c
  motion.c
  getvlc.c
  gethdr.c
  getblk.c
  getbits.c
  store.c
  recon.c
  spatscal.c
  idct.c
  idctref.c
  display.c
  systems.c
  subspic.c
  verify.c
  )
ADD_LIBRARY(gdcmmpeg2 ${GDCM_MPEG2_SOURCES} )
IF(UNIX)
  TARGET_LINK_LIBRARIES(gdcmmpeg2 -lm)
ENDIF(UNIX)

ADD_EXECUTABLE(mpeg2dec mpeg2dec.c)
TARGET_LINK_LIBRARIES(mpeg2dec gdcmmpeg2)
