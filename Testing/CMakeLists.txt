# TODO:
# http://www.cmake.org/pipermail/cmake/2001-November/002491.html
# So that dll is copied to each subdir where needed

SET(CXX_TEST_PATH ${EXECUTABLE_OUTPUT_PATH})

SET(GDCM_TESTS ${CXX_TEST_PATH}/gdcmTests)

#INCLUDE (${CMAKE_ROOT}/Modules/CheckIncludeFile.cmake)
#CHECK_INCLUDE_FILE("stdint.h"    CMAKE_HAVE_STDINT_H)

#IF(NOT CMAKE_HAVE_STDINT_H)
#  SET(GDCM_TEST_COMPILE_FLAGS "-DHAVE_NO_STDINT_H")
#ENDIF(NOT CMAKE_HAVE_STDINT_H)


SET(TEST_SOURCES
  PrintHeader.cxx #require test.acr
  bug1.cxx        #require test.acr
  dcm2acr.cxx     #require test.acr
  hashtest.cxx
  makeDicomDir.cxx
  testDicomDir.cxx
  testWrite.cxx
)

# add tests that require data
IF (GDCM_DATA_ROOT)
SET(TEST_SOURCES ${TEST_SOURCES}     
  PrintDicomDir.cxx
  pourFindTaggs.cxx
  testChangeEntete.cxx
)
ENDIF (GDCM_DATA_ROOT)

# include stuff
INCLUDE_DIRECTORIES(
  ${GDCM_SOURCE_DIR}/src/
  ${GDCM_BINARY_DIR}/
)

CREATE_TEST_SOURCELIST(Tests gdcmCxxTests.cxx ${TEST_SOURCES})
ADD_EXECUTABLE(gdcmCxxTests ${Tests})
TARGET_LINK_LIBRARIES(gdcmCxxTests gdcm)
SET (TestsToRun ${Tests})
REMOVE (TestsToRun gdcmCxxTests.cxx)

#ADD_EXECUTABLE(gdcmTests gdcmTests.cxx ${TEST_SOURCES})
#TARGET_LINK_LIBRARIES(gdcmTests gdcm)

# Loop over files and create executables
FOREACH(file ${TEST_SOURCES})
  GET_FILENAME_COMPONENT(name ${file} NAME_WE)
#  IF (GDCM_DATA_ROOT)
#    ADD_TEST(${TName} ${CXX_TEST_PATH}/gdcmCxxTests ${TName}
#        -D ${VTK_DATA_ROOT}
#        -T ${VTK_BINARY_DIR}/Testing/Temporary
#        -V Baseline/gdcm/${TName}.png)
#  ELSE (GDCM_DATA_ROOT)
    ADD_TEST(${name} ${GDCM_TESTS} ${name})
    #ADD_TEST(${TName} ${CXX_TEST_PATH}/gdcmCxxTests ${TName}) 
#  ENDIF (GDCM_DATA_ROOT)
ENDFOREACH(file ${TEST_SOURCES})




