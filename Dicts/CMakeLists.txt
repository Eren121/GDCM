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
#FILE(WRITE "/tmp/output.h.in" "@DICOM_DICTIONARY@" )

# Following code contributing by Andy Cedilnik
SET(INFILE "dicomV3.dic")

FILE(READ "${INFILE}" ENT)

STRING(REGEX REPLACE "\n" ";" ENT "${ENT}")
SET(DICOM_DICTIONARY
  "void FillDefault()\n{"
)

FOREACH(line ${ENT})
  STRING(REGEX REPLACE
#    "^([0-9a-f][0-9a-f][0-9a-f][0-9a-f]) ([0-9a-f][0-9a-f][0-9a-f][0-9a-f]) ([A-Z]+) ([A-Z]+) (.*)([ \(RET\)]*)$"
   # FIXME ? is not properly used, just luck
    "^([0-9a-f][0-9a-f][0-9a-f][0-9a-f]) ([0-9a-f][0-9a-f][0-9a-f][0-9a-f]) ([A-Z]+) ([A-Z?]+) (.*)$"
#    "   { \\1, \\2, \"\\3\" , \"\\4\" , \"\\5\", \"\\6\"}," nline "${line}")
#    "   { \\1, \\2, \"\\3\" , \"\\4\" , \"\\5\"}," nline "${line}")
    "  AddNewEntry( DictEntry ( 0x\\1, 0x\\2, \"\\3\" , \"\\4\" , \"\\5\"));" nline "${line}")
  SET(DICOM_DICTIONARY "${DICOM_DICTIONARY}\n${nline}")
ENDFOREACH(line)

SET(DICOM_DICTIONARY "${DICOM_DICTIONARY} \n }")

# See above
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/gdcmDefaultDict.h.in" "${GDCM_BINARY_DIR}/gdcmDefaultDict.h")

#MESSAGE("${DICOM_DICTIONARY}")

FILE(READ "dicomV3VM2004.dic" ENT)
STRING(REGEX REPLACE "\n" ";" ENT "${ENT}")
SET(DICOM_DICTIONARY2 "")

FOREACH(line ${ENT})
  STRING(REGEX REPLACE
#    "^([0-9a-f][0-9a-f][0-9a-f][0-9a-f]) ([0-9a-f][0-9a-f][0-9a-f][0-9a-f]) ([A-Z]+) ([A-Z]+) (.*)([ \(RET\)]*)$"
    "^([0-9a-f][0-9a-f][0-9a-f][0-9a-f]) ([0-9a-f][0-9a-f][0-9a-f][0-9a-f]) ([A-Z]+) ([0-9]|[1]-[0-9n]|[2]-[2][n]|[3]-[3][n]) (.*)$"
#    "   { \\1, \\2, \"\\3\" , \"\\4\" , \"\\5\", \"\\6\"}," nline "${line}")
    "   { \\1, \\2, \"\\3\" , \"\\4\" , \"\\5\"}," nline "${line}")
  SET(DICOM_DICTIONARY2 "${DICOM_DICTIONARY2}\n${nline}")
ENDFOREACH(line)

#MESSAGE("${DICOM_DICTIONARY2}")

