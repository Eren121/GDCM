#-----------------------------------------------------------------------------
# Rebuild gdcm whenever a file starting with gdcm* is modified
INCLUDE_REGULAR_EXPRESSION("^gdcm.*$")

SUBDIRS(
  gdcmjpeg
  #gdcmmpeg2
  gdcmjasper
  gdcmjpegls
  )

# "jpeglib.h" is defined here:
INCLUDE_DIRECTORIES(
  ${GDCM_SOURCE_DIR}/src
  ${GDCM_SOURCE_DIR}/src/gdcmmpeg2/src/mpeg2dec
  ${GDCM_BINARY_DIR} #for gdcmConfigure.h
  ${GDCM_BINARY_DIR}/src #for gdcmjpeg 8, 12 and 16 
  ${GDCM_BINARY_DIR}/src/gdcmjasper/src/libjasper/include
  ${GDCM_SOURCE_DIR}/src/gdcmjasper/src/libjasper/include
)

SET(libgdcm_la_SOURCES
   gdcmArgMgr.cxx
   gdcmBase.cxx
   gdcmDataEntry.cxx
   gdcmDebug.cxx
   gdcmDicomDir.cxx
   gdcmDicomDirElement.cxx
   gdcmDicomDirImage.cxx
   gdcmDicomDirMeta.cxx
   gdcmDicomDirObject.cxx
   gdcmDicomDirPatient.cxx
   gdcmDicomDirVisit.cxx
   gdcmDicomDirSerie.cxx
   gdcmDicomDirStudy.cxx
   gdcmDicomEntry.cxx
   gdcmDict.cxx
   gdcmDictEntry.cxx
   gdcmDictGroupName.cxx
   gdcmDictSet.cxx
   gdcmDirList.cxx
   gdcmDocEntry.cxx
   gdcmDocEntryArchive.cxx
   gdcmDocEntrySet.cxx
   gdcmDocument.cxx
   gdcmElementSet.cxx
   gdcmException.cxx
   gdcmFile.cxx
   gdcmFileHelper.cxx
   gdcmGlobal.cxx
   gdcmJPEGFragment.cxx
   gdcmJPEGFragmentsInfo.cxx
   gdcmJpeg8.cxx
   gdcmJpeg12.cxx
   gdcmJpeg16.cxx
   gdcmJpeg2000.cxx
   #   gdcmMpeg.cxx
   gdcmOrientation.cxx
   gdcmPixelReadConvert.cxx
   gdcmPixelWriteConvert.cxx
   gdcmRLEFrame.cxx
   gdcmRLEFramesInfo.cxx
   gdcmSeqEntry.cxx
   gdcmSerieHelper.cxx
   gdcmSQItem.cxx
   gdcmTS.cxx
   gdcmUtil.cxx
   gdcmValidator.cxx
   gdcmVR.cxx
   ${GDCM_BINARY_DIR}/src/gdcmDefaultDicts.cxx
   )

ADD_LIBRARY(gdcm ${libgdcm_la_SOURCES})
TARGET_LINK_LIBRARIES(gdcm 
  gdcmjpeg8 
  gdcmjpeg12
  gdcmjpeg16
  #  gdcmmpeg2
  gdcmjasper
)
IF(WIN32)
  IF(NOT BORLAND)
    TARGET_LINK_LIBRARIES(gdcm wsock32 snmpapi)
  ENDIF(NOT BORLAND)
ENDIF(WIN32)

# Need nsl to resolve gethostbyname on SunOS-5.8
# and socket also
IF(CMAKE_SYSTEM MATCHES "SunOS.*")
  TARGET_LINK_LIBRARIES(gdcm socket nsl)
ENDIF(CMAKE_SYSTEM MATCHES "SunOS.*")

INSTALL_FILES(/include/gdcm "\\.h$")
INSTALL_TARGETS(/lib/gdcm/ gdcm)
