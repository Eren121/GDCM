# include stuff
INCLUDE_DIRECTORIES(
  ${GDCM_SOURCE_DIR}/src
  ${GDCM_BINARY_DIR}
)

SET(EXAMPLE_SOURCES
#names starting with 'ex' are examples  
  exReadPapyrus
  exReadWriteFile
  exColorToRGB
  exGrey2RGB
  exGC
  exImageLighten
  #exOverlaysACR
  exOverlaysDCM
  exCurveData
  exExtractTag
  exSerieHelper
  exXCoherentFileSet
  exExtractDicomTags
                
#the following are utilities
  PrintDicomDir
  PrintFile
  MakeDicomDir
  AnonymizeDicomDir # without loading it as a gdcm::DicomDir
  Anonymize         # for full gdcm readable files
  AnonymizeNoLoad   # without loading the Pixel Data
  AnonymizeMultiPatient # without loading the Pixel Data
  PatchHeader
  PhilipsToBrucker2 
  #ToInTag
  MagnetomVisionToBrucker   
  ReWrite
  ReWriteExtended
  RawToDicom
  TestValidate
  #BatchUncompress
  
#the following will be transformed into 'examples', or 'utilities'
#              or will be removed
#
# Better you don't use them (not fully checked ...)
 
  FindTags
  FlatHashTablePrint
  Volume2Dicom
  WriteDicomSimple
  WriteRead
  exCTPET 
)

FOREACH(name ${EXAMPLE_SOURCES})
  ADD_EXECUTABLE(${name} ${name}.cxx )
  TARGET_LINK_LIBRARIES(${name} gdcm)
  INSTALL_TARGETS(/bin/ ${name})
ENDFOREACH(name)

