#-----------------------------------------------------------------------------
# Build jpeg lib first:
SUBDIRS(jpeg)

# "jpeglib.h" is defined here:
INCLUDE_DIRECTORIES(
  ${GDCM_SOURCE_DIR}/
  ${GDCM_BINARY_DIR}/ #for gdcmConfigure.h
#  ${GDCM_BINARY_DIR}/src/jpeg/libijg8  #created a build time
#  ${GDCM_BINARY_DIR}/src/jpeg/libijg12 #created a build time
)

SET(libgdcm_la_SOURCES
   gdcmBinEntry.cxx
   gdcmDebug.cxx
   gdcmDicomDir.cxx
   gdcmDicomDirElement.cxx
   gdcmDicomDirImage.cxx
   gdcmDicomDirMeta.cxx
   gdcmDicomDirObject.cxx
   gdcmDicomDirPatient.cxx
   gdcmDicomDirSerie.cxx
   gdcmDicomDirStudy.cxx
   gdcmDict.cxx
   gdcmDictEntry.cxx
   gdcmDictSet.cxx
   gdcmDirList.cxx
   gdcmDocEntry.cxx
   gdcmDocEntrySet.cxx
   gdcmDocument.cxx
   gdcmElementSet.cxx
   gdcmException.cxx
   gdcmFile.cxx
   gdcmGlobal.cxx
   gdcmHeader.cxx
   gdcmHeaderHelper.cxx
   gdcmJpeg12.cxx
   gdcmJpeg2000.cxx
   gdcmJpeg.cxx
   gdcmParsePixels.cxx
   gdcmPixelConvert.cxx
   gdcmRLE.cxx
   gdcmRLEFramesInfo.cxx
   gdcmSeqEntry.cxx
   gdcmSQItem.cxx
   gdcmTS.cxx
   gdcmUtil.cxx
   gdcmValEntry.cxx   
   gdcmVR.cxx
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
