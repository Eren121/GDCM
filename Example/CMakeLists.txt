SET(EXAMPLE_SOURCES
  WriteDicom.cxx
)

# include stuff
INCLUDE_DIRECTORIES(
  ${GDCM_SOURCE_DIR}/src/
  ${GDCM_BINARY_DIR}/
)

ADD_EXECUTABLE(FindTags FindTags.cxx)
TARGET_LINK_LIBRARIES(FindTags gdcm)

ADD_EXECUTABLE(TestDcm2Acr TestDcm2Acr.cxx)
TARGET_LINK_LIBRARIES(TestDcm2Acr gdcm)

ADD_EXECUTABLE(TestWrite TestWrite.cxx)
TARGET_LINK_LIBRARIES(TestWrite gdcm)

ADD_EXECUTABLE(TestWriteSimple TestWriteSimple.cxx)
TARGET_LINK_LIBRARIES(TestWriteSimple gdcm)

ADD_EXECUTABLE(PrintDocument PrintDocument.cxx)
TARGET_LINK_LIBRARIES(PrintDocument gdcm)

ADD_EXECUTABLE(PrintFile PrintFile.cxx)
TARGET_LINK_LIBRARIES(PrintFile gdcm)

ADD_EXECUTABLE(PrintHeader PrintHeader.cxx)
TARGET_LINK_LIBRARIES(PrintHeader gdcm)

ADD_EXECUTABLE(WriteDicom WriteDicom.cxx)
TARGET_LINK_LIBRARIES(WriteDicom gdcm)

ADD_EXECUTABLE(Write Write.cxx)
TARGET_LINK_LIBRARIES(Write gdcm)

ADD_EXECUTABLE(WriteRead WriteRead.cxx)
TARGET_LINK_LIBRARIES(WriteRead gdcm)

ADD_EXECUTABLE(makeDicomDir makeDicomDir.cxx)
TARGET_LINK_LIBRARIES(makeDicomDir gdcm)

ADD_EXECUTABLE(PrintDicomDir PrintDicomDir.cxx)
TARGET_LINK_LIBRARIES(PrintDicomDir gdcm)

ADD_EXECUTABLE(BuildUpDicomDir BuildUpDicomDir.cxx)
TARGET_LINK_LIBRARIES(BuildUpDicomDir gdcm)

ADD_EXECUTABLE(TestCopyDicom TestCopyDicom.cxx)
TARGET_LINK_LIBRARIES(TestCopyDicom  gdcm)
