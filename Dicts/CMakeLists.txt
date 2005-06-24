#-----------------------------------------------------------------------------
# DICOM dictionary stuff

#-----------------------------------------------------------------------------
# Generate our own 'DICOM' dictionary, based on the following dictionaries:
SET(ALL_DICTS
  dicomV3.dic
  Papyrus.dic
  NIH.dic     #the forbidden one...
  dicomV3PhilipsIntera.dic
  )

# The following line make sure we are not appending on an already existing file
#FILE(WRITE "${GDCM_BINARY_DIR}/Dicts/gdcm.dic" "")
#FOREACH(dict ${ALL_DICTS})
#  FILE(READ "${GDCM_SOURCE_DIR}/Dicts/${dict}" dict_append)
#  FILE(APPEND "${GDCM_BINARY_DIR}/Dicts/gdcm.dic" ${dict_append})
#ENDFOREACH(dict)

# Better solution
SET(TEST_DICT_BODY)
FOREACH(dict ${ALL_DICTS})
  FILE(READ "${GDCM_SOURCE_DIR}/Dicts/${dict}" dict_append)
  SET(TEST_DICT_BODY "${TEST_DICT_BODY}\n${dict_append}")
ENDFOREACH(dict)
CONFIGURE_FILE("${GDCM_SOURCE_DIR}/Dicts/gdcm.dic.in"
  "${GDCM_BINARY_DIR}/Dicts/gdcm.dic" @ONLY IMMEDIATE)

#-----------------------------------------------------------------------------
# Nice trick to avoid creating a .h file each time we compile gdcm
# Since CONFIGURE_FILE do a 'copy if different'
# We have to write the file here since is contains 'DICOM_DICTIONARY'
# Which is a variable afterward...

# Following code contributing by Andy Cedilnik (Kitware)
FILE(READ "${GDCM_BINARY_DIR}/Dicts/gdcm.dic" ENT)
STRING(REGEX REPLACE "\r?\n" ";" ENT "${ENT}")
SET(DICOM_DATA_DICTIONARY "")

FOREACH(line ${ENT})
  STRING(REGEX REPLACE
    "^([0-9a-f][0-9a-f][0-9a-f][0-9a-f]) ([0-9a-f][0-9a-f][0-9a-f][0-9a-f]) ([A-Z]+) ([1-9n-]+) (.*)$"
    "   {0x\\1, 0x\\2, \"\\3\" , \"\\4\" , \"\\5\"}, " nline "${line}")
  #MESSAGE( "${nline}" )
  SET(DICOM_DATA_DICTIONARY "${DICOM_DATA_DICTIONARY}\n${nline}")
ENDFOREACH(line)


#FILE(READ "dicomV3PhilipsIntera.dic" ENT)
#STRING(REGEX REPLACE "\r?\n" ";" ENT "${ENT}")
#SET(DICOM_DATA_DICTIONARY "")
#FOREACH(line ${ENT})
#  STRING(REGEX REPLACE
#    "^([0-9a-f][0-9a-f][0-9a-f][0-9a-f]) ([0-9a-f][0-9a-f][0-9a-f][0-9a-f]) ([A-Z]+) ([1-9n-]+) (.*)$"
#    "   {0x\\1, 0x\\2, \"\\3\" , \"\\4\" , \"\\5\"}, " nline "${line}")
#  #SET(DICOM_DATA_DICTIONARY "${DICOM_DATA_DICTIONARY}\n${nline}")
#  MESSAGE( "${nline}" )
#ENDFOREACH(line)


SET(DICOM_DATA_DICTIONARY "${DICOM_DATA_DICTIONARY} \n {0,0,0,0,0}")

#-----------------------------------------------------------------------------
# ... for DicomTS file
FILE(READ "dicomTS.dic" ENT_TS)
STRING(REGEX REPLACE "\r?\n" ";" ENT_TS "${ENT_TS}")
SET(DICOM_TS_DICTIONARY "")

FOREACH(line ${ENT_TS})
  STRING(REGEX REPLACE
    "^([0-9.]+) +(.*)$"
    "  ts[\"\\1\"] = \"\\2\"; " nline "${line}")
  SET(DICOM_TS_DICTIONARY "${DICOM_TS_DICTIONARY}\n${nline}")
ENDFOREACH(line)

#-----------------------------------------------------------------------------
# ... for DicomVR file
FILE(READ "dicomVR.dic" ENT_VR)
STRING(REGEX REPLACE ";" "/" ENT_VR "${ENT_VR}") # CMake doesn't like ';'
STRING(REGEX REPLACE "\r?\n" ";" ENT_VR "${ENT_VR}")
SET(DICOM_VR_DICTIONARY "")

FOREACH(line ${ENT_VR})
  STRING(REGEX REPLACE
    "^([A-Z][A-Z]) (.*)/ +//.*$"
    "  vr[\"\\1\"] = \"\\2\"; " nline "${line}")
  SET(DICOM_VR_DICTIONARY "${DICOM_VR_DICTIONARY}\n${nline}")
ENDFOREACH(line)

#-----------------------------------------------------------------------------
# ... for DictGroupName file
FILE(READ "DictGroupName.dic" ENT_TS)
STRING(REGEX REPLACE "\r?\n" ";" ENT_TS "${ENT_TS}")
SET(DICT_GROUP_NAME_DICTIONARY "")

FOREACH(line ${ENT_TS})
  STRING(REGEX REPLACE
    "^([0-9a-f]+) +(.*)$"
    "  groupName[0x\\1] = \\2; " nline "${line}")
  SET(DICT_GROUP_NAME_DICTIONARY "${DICT_GROUP_NAME_DICTIONARY}\n${nline}")
ENDFOREACH(line)

#-----------------------------------------------------------------------------
# ... for DicomDir file
FILE(READ "DicomDir.dic" ENT_DIR)
STRING(REGEX REPLACE "\\\\" "\\\\\\\\" ENT_DIR "${ENT_DIR}")
STRING(REGEX REPLACE ";" "/" ENT_DIR "${ENT_DIR}") # CMake doesn't like ';'
STRING(REGEX REPLACE "\r?\n" ";" ENT_DIR "${ENT_DIR}")
SET(DICOM_DIR_DICTIONARY "")

FOREACH(line ${ENT_DIR})
  STRING(REGEX REPLACE
    "^(metaElem|patientElem|studyElem|serieElem|imageElem) *([a-f0-9]+) ([a-f0-9]+) [\"](.*)[\"](.*)$"
    "  {\"\\1\" , 0x\\2 , 0x\\3 , \"\\4\"}, \\5" nline "${line}")
  SET(DICOM_DIR_DICTIONARY "${DICOM_DIR_DICTIONARY}\n${nline}")
ENDFOREACH(line)

#-----------------------------------------------------------------------------
# ... Set the dictionnary
SET(DICOM_DIR_DICTIONARY "${DICOM_DIR_DICTIONARY} \n {0,0,0,0}")

#FOREACH(file dicomV3.dic dicomTS.dic dicomVR.dic DicomDir.dic DictGroupName.dic)
#  CONFIGURE_FILE("${GDCM_SOURCE_DIR}/Dicts/${file}"
#    "${GDCM_BINARY_DIR}/Dicts/${file}.out")
#ENDFOREACH(file)

CONFIGURE_FILE("${GDCM_SOURCE_DIR}/src/gdcmDefaultDicts.cxx.in" 
  "${GDCM_BINARY_DIR}/src/gdcmDefaultDicts.cxx" IMMEDIATE)

#-----------------------------------------------------------------------------
INSTALL_FILES(${GDCM_DATA_DIR} .dic
  dicomV3
  dicomVR
  dicomTS
  DicomDir
  DictGroupName
  gdcm
)

