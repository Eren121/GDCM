# CMakeLists for Testing purpose
# Cxx file that need a file to run should be place aproprietly

SET(CXX_TEST_PATH ${EXECUTABLE_OUTPUT_PATH})

SET(GDCM_TESTS ${CXX_TEST_PATH}/gdcmTests)

# add test that does not require data:
# They usally return if no argument is given
SET(TEST_SOURCES
  TestBug.cxx
  TestHash.cxx
)

# add tests that require data
IF (GDCM_DATA_ROOT)
  SET(TEST_SOURCES ${TEST_SOURCES}     
    TestReadWriteReadCompare.cxx     # includes generated gdcmDataImages.h 
    PrintAllDocument.cxx             # includes generated gdcmDataImages.h
    TestAllReadCompareDicom.cxx      # includes generated gdcmDataImages.h
    TestAllEntryVerify.cxx           # includes generated gdcmDataImages.h
    PrintDicomDir.cxx
    #TestChangeHeader.cxx
    TestDicomDir.cxx                 # require DICOMDIR
    BuildUpDicomDir.cxx              # writes a file named "NewDICOMDIR"
    makeDicomDir.cxx                 # writes a file named "NewDICOMDIR"
    TestCopyDicom.cxx
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
    "${GDCM_DATA_ROOT}/*.nema"
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
    "${GDCM_DATA_ROOT}/*.nema"
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
  # serial killer (also segfaults vtkgdcmViewer, but NOT PrintFile)
  "gdcm-JPEG-LossLessThoravision.dcm"
  
  # Mathieu: temporarily remove as it kills my gnome session (but not kde)
  # JPR: also breaks TestReadWriteReadCompare : Pixel differ (as expanded
  #      in memory)  
  "KODAK-12-MONO1-Odd_Terminated_Sequence.dcm"
  
  #segfaults TestReadWriteReadCompare (same pb as below) --> Fixed
  #segfaults ShowDicom
  "gdcm-MR-SIEMENS-16-1.acr"

  #Break ShowDicom 
  "TestReadWriteReadCompare.dcm"

  # see comments in gdcmData ref file TestAllEntryVerifyReference.txt
  # JPR 2004/20/09: doesn't break TestReadWriteReadCompare anymore
  #"SIEMENS_GBS_III-16-ACR_NEMA_1.acr"
  
  # Frog 2004/07/29: segfaults TestReadWriteReadCompare (pixels not
  #                  parsed properly):
  # JPR 2004/20/09: doesn't break TestReadWriteReadCompare anymore
  #    "GE_DLX-8-MONO2-Multiframe-Jpeg_Lossless.dcm"

  # JPR 2004/20/09: doesn't break TestReadWriteReadCompare anymore
  #"PICKER-16-MONO2-Nested_icon.dcm"

  # JPR 2004/20/09: doesn't break TestReadWriteReadCompare anymore
  #"SIEMENS_Sonata-16-MONO2-Value_Multiplicity.dcm"
  
  # JPR 2004/20/09: doesn't break TestReadWriteReadCompare anymore
  # Fails on TestAllReadCompareDicom at the RE-reading stage with message:
  #    Big trouble : Pixel Element (7fe0,10) NOT found
  # The temporary file GDCMDATAHOME/BaselineDicom/SIEMENS_Sonata-12-MONO2-SQ.dcm
  # stops after the first binEntry (binary entry) as can be seen with
  #   " gdcmbin/bin/PrintHeader 
  #         GDCMDATAHOME/BaselineDicom/SIEMENS_Sonata-12-MONO2-SQ.dcm "
  # and effectively the pixels are missing (as well as all the entries
  # after tag 0029|1010)
  # Note: this file was previously known as E00001S03I0015.dcm
  #"SIEMENS_Sonata-12-MONO2-SQ.dcm"
  
  # JPR 2004/20/09: doesn't break TestReadWriteReadCompare anymore
  # As of 14/09 this image creates a crash:
  # Program received signal SIGSEGV, Segmentation fault.
  #    0x4032bc4b in gdcmHeader::GetLUTRGBA (this=0x8149228)
  #    at /home/malaterre/Creatis/gdcm/src/gdcmHeader.cxx:1170
  #    1170          *a = lutR[i*mult+1];
  "OT-PAL-8-face.dcm"
  
  # Files which won't break the test suite but whose png dump looks ugly.
  # JPR: png looks ugly. so What ? ctest compliant --> un-black listed
  #   "THERALYS-12-MONO2-Uncompressed-E_Film_Template.dcm"
  #   "PHILIPS_Gyroscan-8-MONO2-Odd_Sequence.dcm"
  #   "PHILIPS_Gyroscan-12-MONO2-Jpeg_Lossless.dcm"
  "SIEMENS_GBS_III-16-ACR_NEMA_1.acr"
  )


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
