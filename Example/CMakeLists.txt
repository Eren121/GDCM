# include stuff
INCLUDE_DIRECTORIES(
  ${GDCM_SOURCE_DIR}/src/
  ${GDCM_BINARY_DIR}/
)

SET(EXAMPLE_SOURCES
#names starting with 'ex' are examples  
  exBuildUpDicomDir
  exReadPapyrus
  exReadWriteFile
  exAnonymize
  exAnonymizeNoLoad  # without loading the Pixel Data
  exColorToRGB
  exGC
  exImageLighten
  exOverlaysACR
            
#the following are utilities
  PrintDicomDir
  PrintDocument
  PrintFile
  PrintHeader
  FindTags
  MakeDicomDir
  AnonymizeDicomDir # without loading it as a gdcm::DicomDir

#the following will be transformed into 'examples', or 'utilities'
#              or will be removed
 
  #test
  FlatHashTablePrint
  TestCopyDicom
  TestChangeHeader
  TestFromScratch
  TestWrite
  TestWriteSimple
  Volume2Dicom
  WriteDicomSimple
  WriteRead
)

FOREACH(name ${EXAMPLE_SOURCES})
  ADD_EXECUTABLE(${name} ${name}.cxx)
  TARGET_LINK_LIBRARIES(${name} gdcm)
  INSTALL_TARGETS(/bin/ ${name})
ENDFOREACH(name)

