SET(EXAMPLE_SOURCES
  WriteDicom.cxx
)

# include stuff
INCLUDE_DIRECTORIES(
  ${GDCM_SOURCE_DIR}/src/
  ${GDCM_BINARY_DIR}/
)

ADD_EXECUTABLE(WriteDicom WriteDicom.cxx)
TARGET_LINK_LIBRARIES(WriteDicom gdcm)
