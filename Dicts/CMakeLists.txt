#-----------------------------------------------------------------------------
# DICOM dictionary stuff


INSTALL_FILES(${GDCM_DATA_DIR} FILES 
  dicomV3.dic
  dicomVR.dic
  dicomTS.dic
  DicomDir.dic
)

# Nice trick to avoid creating a .h file each time we compile gdcm
# Since CONFIGURE_FILE do a 'copy if different'
# We have to write the file here since is contains 'DICOM_DICTIONARY'
# Which is a variable afterward...

# Following code contributing by Andy Cedilnik (Kitware)
FILE(READ "dicomV3.dic" ENT)
STRING(REGEX REPLACE "\n" ";" ENT "${ENT}")
SET(DICOM_DATA_DICTIONARY "")

FOREACH(line ${ENT})
  STRING(REGEX REPLACE
    "^([0-9a-f][0-9a-f][0-9a-f][0-9a-f]) ([0-9a-f][0-9a-f][0-9a-f][0-9a-f]) ([A-Z]+) ([A-Z?]+) (.*)$"
    "   { 0x\\1, 0x\\2, \"\\3\" , \"\\4\" , \"\\5\" }, " nline "${line}")
  SET(DICOM_DATA_DICTIONARY "${DICOM_DATA_DICTIONARY}\n${nline}")
ENDFOREACH(line)

SET(DICOM_DATA_DICTIONARY "${DICOM_DATA_DICTIONARY} \n { 0,0,0,0,0 }")

FILE(READ "dicomTS.dic" ENT_TS)
STRING(REGEX REPLACE "\n" ";" ENT_TS "${ENT_TS}")
SET(DICOM_TS_DICTIONARY "")

FOREACH(line ${ENT_TS})
  STRING(REGEX REPLACE
    "^([0-9.]+) +(.*)$"
    "  ts[\"\\1\"] = \"\\2\"; " nline "${line}")
  SET(DICOM_TS_DICTIONARY "${DICOM_TS_DICTIONARY}\n${nline}")
ENDFOREACH(line)

FILE(READ "dicomVR.dic" ENT_VR)
STRING(REGEX REPLACE ";" "/" ENT_VR "${ENT_VR}") # CMake doesn't like ';'
STRING(REGEX REPLACE "\n" ";" ENT_VR "${ENT_VR}")
SET(DICOM_VR_DICTIONARY "")

FOREACH(line ${ENT_VR})
  STRING(REGEX REPLACE
    "^([A-Z][A-Z]) (.*)/ +//.*$"
    "  vr[\"\\1\"] = \"\\2\"; " nline "${line}")
  SET(DICOM_VR_DICTIONARY "${DICOM_VR_DICTIONARY}\n${nline}")
ENDFOREACH(line)

FILE(READ "DicomDir.dic" ENT_DIR)
STRING(REGEX REPLACE "\\\\" "\\\\\\\\" ENT_DIR "${ENT_DIR}")
STRING(REGEX REPLACE ";" "/" ENT_DIR "${ENT_DIR}") # CMake doesn't like ';'
STRING(REGEX REPLACE "\n" ";" ENT_DIR "${ENT_DIR}")
SET(DICOM_DIR_DICTIONARY "")

FOREACH(line ${ENT_DIR})
  STRING(REGEX REPLACE
    "^(metaElem|patientElem|studyElem|serieElem|imageElem) *([a-f0-9]+) ([a-f0-9]+) [\"](.*)[\"](.*)$"
    "  { \"\\1\" , 0x\\2 , 0x\\3 , \"\\4\" }, \\5" nline "${line}")
  SET(DICOM_DIR_DICTIONARY "${DICOM_DIR_DICTIONARY}\n${nline}")
ENDFOREACH(line)

SET(DICOM_DIR_DICTIONARY "${DICOM_DIR_DICTIONARY} \n { 0,0,0,0 }")

# See above
FOREACH(file dicomV3.dic  dicomTS.dic dicomVR.dic DicomDir.dic)
  CONFIGURE_FILE("${GDCM_SOURCE_DIR}/Dicts/${file}"
    "${GDCM_BINARY_DIR}/Dicts/${file}.out")
ENDFOREACH(file)
CONFIGURE_FILE("${GDCM_SOURCE_DIR}/src/gdcmDefaultDicts.cxx.in" "${GDCM_BINARY_DIR}/src/gdcmDefaultDicts.cxx" IMMEDIATE)


#FILE(READ "dicomV3VM2004.dic" ENT2)
#STRING(REGEX REPLACE "\n" ";" ENT2 "${ENT2}")
#SET(DICOM_DICTIONARY2 "")
#
#FOREACH(line ${ENT2})
#  STRING(REGEX REPLACE
##    "^([0-9a-f][0-9a-f][0-9a-f][0-9a-f]) ([0-9a-f][0-9a-f][0-9a-f][0-9a-f]) ([A-Z]+) ([A-Z]+) (.*)([ \(RET\)]*)$"
#    "^([0-9a-f][0-9a-f][0-9a-f][0-9a-f]) ([0-9a-f][0-9a-f][0-9a-f][0-9a-f]) ([A-Z]+) ([0-9]|[1]-[0-9n]|[2]-[2][n]|[3]-[3][n]) (.*)$"
##    "   { \\1, \\2, \"\\3\" , \"\\4\" , \"\\5\", \"\\6\"}," nline "${line}")
#    "   { \\1, \\2, \"\\3\" , \"\\4\" , \"\\5\"}," nline "${line}")
#  SET(DICOM_DICTIONARY2 "${DICOM_DICTIONARY2}\n${nline}")
#ENDFOREACH(line)
#
#
#
##FILE(READ "04_06PU.txt" ENT3)
#FILE(READ "extract.txt" ENT3)
#
#STRING(REGEX REPLACE "\n" ";" ENT3 "${ENT3}")
#SET(DICOM_DICTIONARY3 "")
#
#FOREACH(line ${ENT3})
#  STRING(REGEX REPLACE
##    "^[(]([0-9a-fA-F]+),([0-9a-fA-F]+)[)] (.*) ([A-ZA-Z]*) ([[0-3][-][1-3n]*|[1-]*[0-9]+]*) ?([RET|Retired]*)$"
#    "^[(]([0-9a-fA-F]+),([0-9a-fA-F]+)[)]  +(.*)  +([A-ZA-Z   [[0-3][-][1-3n]*|[1-]*[0-9]+]]*)([ RET]*)$"
#    "   { \\1 , \\2, \\3, \\4, \\5  }," nline "${line}")
#  STRING(COMPARE NOTEQUAL "${line}" "${nline}" add)
#  IF(add)
#    #SET(DICOM_DICTIONARY3 "${DICOM_DICTIONARY3}\n${nline}")
#    MESSAGE("${nline}")
#  ELSE(add)
#    MESSAGE("WRONG:${nline}")
#  ENDIF(add)
#ENDFOREACH(line)


# Txt generation:
# ~/Software/xpdf-3.00-linux/pdftotext -f 9 -l 81 -raw -nopgbrk 04_06PU.PDF 04_06PU-3.txt
