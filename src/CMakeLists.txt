#-----------------------------------------------------------------------------
# Build jpeg lib first:
SUBDIRS(jpeg)

# "jpeglib.h" is defined here:
INCLUDE_DIRECTORIES(
  ${GDCM_SOURCE_DIR}/src/jpeg/libijg8
  #${GDCM_SOURCE_DIR}/jpeg/libijg12 #all files have been renamed, appending 12
  #${GDCM_SOURCE_DIR}/jpeg/ljpg     # JPEG lib from xmedcom 
  ${GDCM_BINARY_DIR}/ #for gdcmConfigure.h
)

SET(libgdcm_la_SOURCES
   gdcmDirList.cxx
   gdcmDebug.cxx
   gdcmDict.cxx
   gdcmDictEntry.cxx
   gdcmDictSet.cxx
   gdcmException.cxx
   gdcmGlobal.cxx
   gdcmJpeg12.cxx
   gdcmJpeg2000.cxx
   gdcmJpeg.cxx
   gdcmTS.cxx
   gdcmUtil.cxx
   gdcmVR.cxx
   gdcmFile.cxx
   gdcmHeader.cxx
   gdcmHeaderHelper.cxx
   gdcmParsePixels.cxx
   gdcmRLE.cxx
   gdcmDocEntry.cxx
   gdcmDocEntrySet.cxx
   gdcmBinEntry.cxx
   gdcmSeqEntry.cxx
   gdcmSQItem.cxx
   gdcmElementSet.cxx
   gdcmValEntry.cxx   
   gdcmDicomDirElement.cxx
   gdcmDocument.cxx
   gdcmDicomDir.cxx
   gdcmDicomDirMeta.cxx
   gdcmDicomDirPatient.cxx
   gdcmDicomDirStudy.cxx
   gdcmDicomDirSerie.cxx
   gdcmDicomDirImage.cxx
   gdcmDicomDirObject.cxx
   )

ADD_LIBRARY(gdcm ${libgdcm_la_SOURCES} )
IF(UNIX)
  TARGET_LINK_LIBRARIES(gdcm 
    gdcmijpeg8 
    gdcmijpeg12
    gdcmljpeg     # JPEG lib from xmedcom 
  )
ELSE(UNIX)
  TARGET_LINK_LIBRARIES(gdcm 
    gdcmijpeg8 
    gdcmijpeg12
    gdcmljpeg
    Wsock32.lib   #doesn't exist on cygwin
  )
ENDIF(UNIX)

#The following is not working because when a header file is not found it tries 
#to find one in the binary dir
#INSTALL_FILES(/include .h ${libgdcm_la_SOURCES})
#INSTALL_FILES(/include FILES gdcmIdo.h iddcmjpeg.h)
INSTALL_FILES(/include "\\.h$")

INSTALL_TARGETS(/lib/ gdcm)
