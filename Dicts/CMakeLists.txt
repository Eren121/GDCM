#SET(dictsdir 
#  ${GDCM_SOURCE_DIR}/gdcm
#)

#SET(dicts_DATA
#  dicomV3.dic
#  README
#)

#SET(EXTRA_DIST
#  dicomV3.dic
#)

#IF(UNIX)
  INSTALL_FILES(${GDCM_DATA_DIR} FILES dicomV3.dic README)
#ENDIF(UNIX)