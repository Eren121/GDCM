SET(GDCM_MPEG2_SOURCES
  mpeg2dec.c
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

