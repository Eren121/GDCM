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
"//
#include \"gdcmDicomDictionaryEntry.h\"

static gdcmDicomDictionaryEntry[] =
{")

FOREACH(line ${ENT})
  STRING(REGEX REPLACE
    "^([0-9a-f][0-9a-f][0-9a-f][0-9a-f]) ([0-9a-f][0-9a-f][0-9a-f][0-9a-f]) ([A-Z]+) ([A-Z]+) (.*)([ \(RET\)]*)$"
    "   { \\1, \\2, \"\\3\" , \"\\4\" , \"\\5\", \"\\6\"}," nline "${line}")
  SET(DICOM_DICTIONARY "${DICOM_DICTIONARY}\n${nline}")
ENDFOREACH(line)

SET(DICOM_DICTIONARY "${DICOM_DICTIONARY}
   0
};")

# See above
#CONFIGURE_FILE("/tmp/output.h.in" "/tmp/output.h")

#MESSAGE("${DICOM_DICTIONARY}")

