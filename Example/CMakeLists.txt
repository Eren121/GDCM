# include stuff
INCLUDE_DIRECTORIES(
  ${GDCM_SOURCE_DIR}/src/
  ${GDCM_BINARY_DIR}/
)

SET(EXAMPLE_SOURCES
  BuildUpDicomDir
  FindTags
  FlatHashTablePrint
  makeDicomDir
  PrintDocument
  PrintFile
  PrintHeader
  PrintDicomDir
  TestCopyDicom
  TestChangeHeader
  TestFromScratch
  TestPapyrus
  TestWrite
  TestWriteSimple
  Volume2Dicom
  WriteDicomSimple
  Write
  WriteRead
  WriteDicom
)

FOREACH(name ${EXAMPLE_SOURCES})
  ADD_EXECUTABLE(${name} ${name}.cxx)
  TARGET_LINK_LIBRARIES(${name} gdcm)
ENDFOREACH(name)

