# CMakeLists for Testing purpose
# Cxx file that need a file to run should be place aproprietly

SET(CXX_TEST_PATH ${EXECUTABLE_OUTPUT_PATH})

SET(GDCM_TESTS ${CXX_TEST_PATH}/gdcmTests)

# add test that does not require data:
# They usally return if no argument is given
SET(TEST_SOURCES
  PrintBug.cxx
  dcm2acr.cxx
  hashtest.cxx
  testWrite.cxx
)

# add tests that require data
IF (GDCM_DATA_ROOT)
SET(TEST_SOURCES ${TEST_SOURCES}     
  PrintHeader.cxx     #require test.acr
  PrintDicomDir.cxx
  pourFindTaggs.cxx
  testChangeEntete.cxx
  testDicomDir.cxx    #require DICOMDIR
  BuildUpDicomDir.cxx
  TestReadWrite.cxx

# Disable makeDicomDir for now
#  makeDicomDir.cxx
)
ENDIF (GDCM_DATA_ROOT)

# include stuff
INCLUDE_DIRECTORIES(
  ${GDCM_SOURCE_DIR}/src/
  ${GDCM_BINARY_DIR}/
)

CREATE_TEST_SOURCELIST(Tests gdcmTests.cxx ${TEST_SOURCES})
ADD_EXECUTABLE(gdcmTests ${Tests})
TARGET_LINK_LIBRARIES(gdcmTests gdcm)
SET (TestsToRun ${Tests})
REMOVE (TestsToRun gdcmTests.cxx)

# Loop over files and create executables
FOREACH(file ${TEST_SOURCES})
  GET_FILENAME_COMPONENT(name ${file} NAME_WE)
#  IF (GDCM_DATA_ROOT)
#    ADD_TEST(${name} ${CXX_TEST_PATH}/gdcmTests ${name}
#        -D ${GDCM_DATA_ROOT}
#        -T ${GDCM_BINARY_DIR}/Testing/Temporary
#        -V Baseline/gdcm/${name}.png)
#  ELSE (GDCM_DATA_ROOT)
    ADD_TEST(${name} ${GDCM_TESTS} ${name})
#  ENDIF (GDCM_DATA_ROOT)
ENDFOREACH(file ${TEST_SOURCES})




