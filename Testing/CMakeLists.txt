# TODO:
# http://www.cmake.org/pipermail/cmake/2001-November/002491.html
# So that dll is copied to each subdir where needed

SET(CXX_TEST_PATH ${EXECUTABLE_OUTPUT_PATH})

SET(GDCM_TESTS ${CXX_TEST_PATH}/gdcmTests)

INCLUDE (${CMAKE_ROOT}/Modules/CheckIncludeFile.cmake)
CHECK_INCLUDE_FILE("stdint.h"    CMAKE_HAVE_STDINT_H)

IF(NOT CMAKE_HAVE_STDINT_H)
  SET(GDCM_TEST_COMPILE_FLAGS "-DHAVE_NO_STDINT_H")
ENDIF(NOT CMAKE_HAVE_STDINT_H)


SET(TEST_SOURCES
  PrintHeader.cxx
  testWrite.cxx
  testChangeEntete.cxx
  hashtest.cxx
  bug1.cxx
  pourFindTaggs.cxx
  dcm2acr.cxx
  PrintDicomDir.cxx
  makeDicomDir.cxx
  testDicomDir.cxx
)

# include stuff
INCLUDE_DIRECTORIES(
  ${GDCM_SOURCE_DIR}/src/
)

# Loop over files and create executables
FOREACH(file ${TEST_SOURCES})
  #Doh ! I need to specify each time the compile flags !
  IF(GDCM_TEST_COMPILE_FLAGS)
  SET_SOURCE_FILES_PROPERTIES(${file}
    PROPERTIES
    COMPILE_FLAGS ${GDCM_TEST_COMPILE_FLAGS}
  )
  ENDIF(GDCM_TEST_COMPILE_FLAGS)
  GET_FILENAME_COMPONENT(name ${file} NAME_WE)
  ADD_TEST(${name} ${GDCM_TESTS} ${name})
ENDFOREACH(file ${TEST_SOURCES})


ADD_EXECUTABLE(gdcmTests gdcmTests.cxx ${TEST_SOURCES})
TARGET_LINK_LIBRARIES(gdcmTests gdcm)


