#-----------------------------------------------------------------------------
# DICOM dictionary stuff

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

# For now INSTALL doesn't work on Win32 
INSTALL_FILES(${GDCM_DATA_DIR} FILES 
  dicomV3.dic 
  dicomVR.dic 
  dicomTS.dic
  DicomDir.dic
  README)
