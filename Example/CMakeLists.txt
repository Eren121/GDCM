SET(EXAMPLE_SOURCES
  WriteDicom.cxx
)

# include stuff
INCLUDE_DIRECTORIES(
  ${GDCM_SOURCE_DIR}/src/
  ${GDCM_BINARY_DIR}/
)


ADD_EXECUTABLE(PrintDocument PrintDocument.cxx)
TARGET_LINK_LIBRARIES(PrintDocument gdcm)

ADD_EXECUTABLE(PrintFile PrintFile.cxx)
TARGET_LINK_LIBRARIES(PrintFile gdcm)

ADD_EXECUTABLE(Write Write.cxx)
TARGET_LINK_LIBRARIES(Write gdcm)

ADD_EXECUTABLE(WriteDicom WriteDicom.cxx)
TARGET_LINK_LIBRARIES(WriteDicom gdcm)

ADD_EXECUTABLE(WriteRead WriteRead.cxx)
TARGET_LINK_LIBRARIES(WriteRead gdcm)
