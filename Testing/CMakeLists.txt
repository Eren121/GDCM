# TODO:
# http://www.cmake.org/pipermail/cmake/2001-November/002491.html
# So that dll is copied to each subdir where needed

SET(TEST_SOURCES
  PrintHeader.cxx
  testWrite.cxx
  testChangeEntete.cxx
  hashtest.cxx
  bug1.cxx
  pourFindTaggs.cxx
  dcm2acr.cxx
)

# include stuff
INCLUDE_DIRECTORIES(
  ${GDCM_SOURCE_DIR}/src/
)

# Loop over files and create executables
FOREACH(file ${TEST_SOURCES})
  GET_FILENAME_COMPONENT(name ${file} NAME_WE)
  ADD_EXECUTABLE(${name} ${file})
  TARGET_LINK_LIBRARIES(gdcm)
ENDFOREACH(file ${TEST_SOURCES})
