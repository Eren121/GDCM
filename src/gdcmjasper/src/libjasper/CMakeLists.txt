
SUBDIRS(base
  bmp
  jp2  
  jpc  
  jpg  
  mif  
  pgx  
  pnm  
  ras
  )

ADD_LIBRARY(gdcmjasper)
TARGET_LINK_LIBRARIES(gdcmjasper
  base
  bmp
  jp2
  jpc
  jpg
  mif
  pgx
  pnm
  ras)
