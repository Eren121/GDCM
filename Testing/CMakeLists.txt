# CMakeLists for Testing purpose
# Cxx file that need a file to run should be place aproprietly

SET(CXX_TEST_PATH ${EXECUTABLE_OUTPUT_PATH})

SET(GDCM_TESTS ${CXX_TEST_PATH}/gdcmTests)

# add test that does not require data:
# They usally return if no argument is given
SET(TEST_SOURCES
  TestBug.cxx
  TestDcm2Acr.cxx
  TestHash.cxx
  TestWrite.cxx
  TestReadWriteReadCompare.cxx
  TestWriteSimple.cxx
)

# add tests that require data
IF (GDCM_DATA_ROOT)
  SET(TEST_SOURCES ${TEST_SOURCES}     
    #PrintHeader.cxx       #require test.acr
    PrintAllDocument.cxx     #require test.acr
    PrintDicomDir.cxx
    TestFindTags.cxx
    TestChangeHeader.cxx
    TestDicomDir.cxx      #require DICOMDIR
    BuildUpDicomDir.cxx
    makeDicomDir.cxx
  )
  # add test that require VTK:
  IF(GDCM_VTK)
    INCLUDE_DIRECTORIES(
      ${GDCM_SOURCE_DIR}/vtk/
      )
    SET(TEST_SOURCES ${TEST_SOURCES}
      ShowDicom.cxx    
      )
  ENDIF(GDCM_VTK)
ENDIF (GDCM_DATA_ROOT)



# include stuff
INCLUDE_DIRECTORIES(
  ${GDCM_SOURCE_DIR}/src/
  ${GDCM_BINARY_DIR}/
)

CREATE_TEST_SOURCELIST(Tests gdcmTests.cxx ${TEST_SOURCES})
ADD_EXECUTABLE(gdcmTests ${Tests})
IF(GDCM_VTK)
  TARGET_LINK_LIBRARIES(gdcmTests vtkgdcm vtkRendering)
ELSE(GDCM_VTK)
  TARGET_LINK_LIBRARIES(gdcmTests gdcm)
ENDIF(GDCM_VTK)

SET (TestsToRun ${Tests})
REMOVE (TestsToRun gdcmTests.cxx)

# Loop over files and create executables
FOREACH(file ${TEST_SOURCES})
  GET_FILENAME_COMPONENT(name ${file} NAME_WE)
#  IF (GDCM_DATA_ROOT)
#    ADD_TEST(${name} ${GDCM_TESTS} ${name}
#        -D ${GDCM_DATA_ROOT}
#        -T ${GDCM_BINARY_DIR}/Testing/Temporary
#        -V Baseline/gdcm/${name}.png)
#  ELSE (GDCM_DATA_ROOT)
    ADD_TEST(${name} ${GDCM_TESTS} ${name})
#  ENDIF (GDCM_DATA_ROOT)
ENDFOREACH(file ${TEST_SOURCES})

#-----------------------------------------------------------------------------

# We need to test the reading of all dicom images in the gdcmData directory
# First parse this directory and extract all images

IF(${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} GREATER 1.9)
  FILE(GLOB GDCM_DATA_IMAGES_GLOB
    "${GDCM_DATA_ROOT}/*.acr"
    "${GDCM_DATA_ROOT}/*.dcm"
#    "${GDCM_DATA_ROOT}/*.nema"
    "${GDCM_DATA_ROOT}/*.ima"
    )
ELSE(${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} GREATER 1.9)   
  # Since there is a bug in cmake 1.8.3, I'll have to do two separate globs
  FILE(GLOB GDCM_DATA_IMAGES_GLOB_ACR
    "${GDCM_DATA_ROOT}/*.acr"
    )
  FILE(GLOB GDCM_DATA_IMAGES_GLOB_DCM
    "${GDCM_DATA_ROOT}/*.dcm"
    )
  FILE(GLOB GDCM_DATA_IMAGES_GLOB_NEMA
#    "${GDCM_DATA_ROOT}/*.nema"
    )
  FILE(GLOB GDCM_DATA_IMAGES_GLOB_IMA
    "${GDCM_DATA_ROOT}/*.ima"
    )
  SET(GDCM_DATA_IMAGES_GLOB 
    ${GDCM_DATA_IMAGES_GLOB_DCM} ${GDCM_DATA_IMAGES_GLOB_ACR}
    ${GDCM_DATA_IMAGES_GLOB_NEMA} ${GDCM_DATA_IMAGES_GLOB_IMA})
  #SET(GDCM_DATA_IMAGES_GLOB  ${GDCM_DATA_IMAGES_GLOB_ACR})
ENDIF(${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} GREATER 1.9)   

# Black list of images known to break lots of readers (efilm, xmedcon ...):
SET(BLACK_LIST
  "gdcm-JPEG-LossLessThoravision.dcm" # serial killer
  "cr172241.dcm"           #temporarily remove as it kills my gnome session (but not kde)
  "emptyImage.dcm"         #the png looks ugly...
  "gdcm-MR-PHILIPS-16-Multi-Seq.fixed.dcm" #weird looks exactly the same as gdcm-MR-PHILIPS-16-Multi-Seq.dcm
  "gdcm-MR-PHILIPS-16.dcm" #png looks ugly
  "irmPhlipsNew1.dcm"      #png looks ugly
  "mriThruVPRO.dcm"        #png looks ugly
  "US.3405.1.dcm"          #looks exactly the same as US.1.2.dcm
  )

#   gdcm-ACR-LibIDO seems to be cut
#   test.acr is very bright

# This is a GLOB expression, change it into a list separated with a comma and \n
SET(GDCM_DATA_IMAGES)
FOREACH(filename ${GDCM_DATA_IMAGES_GLOB})
  GET_FILENAME_COMPONENT(filename_temp ${filename} NAME)
  STRING(REGEX MATCH ${filename_temp} bad_dicom ${BLACK_LIST})
  IF(NOT bad_dicom)
    SET(GDCM_DATA_IMAGES "${GDCM_DATA_IMAGES}\n\"${filename_temp}\",")
  ENDIF(NOT bad_dicom)
ENDFOREACH(filename)

# Populate GDCM_DATA_IMAGES:
FILE(WRITE "${GDCM_BINARY_DIR}/gdcmDataImages.h"
  "const char * const gdcmDataImages[] = { ${GDCM_DATA_IMAGES}\n0 };\n" 
  )
