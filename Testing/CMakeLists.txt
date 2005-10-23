# CMakeLists for Testing purpose
# Cxx file that need a file to run should be place aproprietly

SET(CXX_TEST_PATH ${EXECUTABLE_OUTPUT_PATH})

SET(GDCM_TESTS ${CXX_TEST_PATH}/gdcmTests)

# add test that does not require data:
# They usally return if no argument is given
SET(TEST_SOURCES
  TestInline
  TestUtil.cxx
  TestBug.cxx
  TestHash.cxx
  TestTS.cxx
  TestVR.cxx
  TestDictGroupName.cxx
  TestDict.cxx
  TestDataEntry.cxx
  TestWriteSimple.cxx
  TestImageSet.cxx
  TestDicomDirElement.cxx
  TestDicomString.cxx
  #TestValidate.cxx
)

# add tests that require data
IF (GDCM_DATA_ROOT)
  SET(TEST_SOURCES ${TEST_SOURCES}
    TestFileAccessors.cxx
    TestMakeIcon.cxx
    TestLoadAllDocumentsNoPrivNoSeq.cxx # includes generated gdcmDataImages.h
    TestPrintAllDocument.cxx         # includes generated gdcmDataImages.h
    #TestAllEntryVerify.cxx           # includes generated gdcmDataImages.h
    TestAllReadCompareDicom.cxx      # includes generated gdcmDataImages.h
    TestReadWriteReadCompare.cxx     # includes generated gdcmDataImages.h 
    #TestCopyDicom.cxx                # includes generated gdcmDataImages.h 
    #    TestCopyRescaleDicom.cxx         # includes generated gdcmDataImages.h 
    #TestChangeHeader.cxx
    #    TestAnonymize.cxx
    TestDirList.cxx
    TestDicomDir.cxx                 # require DICOMDIR
    TestBuildUpDicomDir.cxx          # writes a file named "NewDICOMDIR"
    TestMakeDicomDir.cxx             # writes a file named "NewDICOMDIR"
    TestSerieHelper.cxx              # uses gdcmData as a default root directory    
    TestAllVM.cxx
  )
  # add test that require VTK:
  IF(GDCM_VTK)
    IF("${VTK_MAJOR_VERSION}" LESS 4)
      MESSAGE(FATAL_ERROR "This VTK version is not supported, you are on your own !")
    ELSE("${VTK_MAJOR_VERSION}" LESS 4)
      IF("${VTK_MAJOR_VERSION}" EQUAL 4 AND "${VTK_MINOR_VERSION}" EQUAL 2)
        MESSAGE(FATAL_ERROR "This VTK version is not supported, you are on your own !"
                             "Although gdcm should compile fine. You just need to turn BUILD_TESTING:=OFF")
      ENDIF("${VTK_MAJOR_VERSION}" EQUAL 4 AND "${VTK_MINOR_VERSION}" EQUAL 2)
      IF("${VTK_MAJOR_VERSION}" EQUAL 4 AND "${VTK_MINOR_VERSION}" EQUAL 4 AND "${VTK_BUILD_VERSION}" LESS 3)
        MESSAGE(FATAL_ERROR "This VTK version is not supported, you are on your own !"
                             "Although gdcm should compile fine. You just need to turn BUILD_TESTING:=OFF")
      ENDIF("${VTK_MAJOR_VERSION}" EQUAL 4 AND "${VTK_MINOR_VERSION}" EQUAL 4 AND "${VTK_BUILD_VERSION}" LESS 3)
      # Include the VTK library since we know this is going to work
      INCLUDE(${VTK_USE_FILE})

      INCLUDE_DIRECTORIES(
        ${GDCM_SOURCE_DIR}/vtk
      )
      SET(TEST_SOURCES ${TEST_SOURCES}
        VTKTestRead.cxx
        VTKTestReadSeq.cxx
        VTKTestWrite.cxx
        VTKTestWriteSeq.cxx
      )
    ENDIF("${VTK_MAJOR_VERSION}" LESS 4)
  ENDIF(GDCM_VTK)
ENDIF (GDCM_DATA_ROOT)



# include stuff
INCLUDE_DIRECTORIES(
  ${GDCM_SOURCE_DIR}/src
  ${GDCM_BINARY_DIR}
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
  ADD_TEST(${name} ${GDCM_TESTS} ${name})
ENDFOREACH(file ${TEST_SOURCES})

#-----------------------------------------------------------------------------

# We need to test the reading of all dicom images in the gdcmData directory
# First parse this directory and extract all images

FILE(GLOB GDCM_DATA_IMAGES_GLOB
  "${GDCM_DATA_ROOT}/*.acr"
  "${GDCM_DATA_ROOT}/*.dcm"
  "${GDCM_DATA_ROOT}/*.nema"
  "${GDCM_DATA_ROOT}/*.ima"
  )

# Black list of images known to break lots of readers (efilm, xmedcon ...):
SET(BLACK_LIST
  # Multiframe Papyrus format image
  # Just to remember this format exists, and is gdcm::File compliant
  # (NOT gdcm::FileHelper ...)
  # any contribution is welcome
  "PET-cardio-Multiframe-Papyrus.dcm"
  # From David Clunie's Compresion Dataset  
  "D_CLUNIE_CT1_JLSN.dcm"
  "D_CLUNIE_CT1_JLSL.dcm"
  #Not a Dicom Image, but a RT Object
  "RS2.16.124.113534.2.3.5.0.120040728.21342.353.12747_anonimized.dcm" 
  #Dicom MPEG image
  "ETIAM_video_002.dcm"
  #Something stupid occured in the code for LibIDO images.
  #I Black list them for a while. JP
  "gdcm-ACR-LibIDO.acr"
  "LIBIDO-24-ACR_NEMA-Rectangle.dcm"
  #16 Bits per pixel images with LUT 16 Bits.
  #I wait to be 100% sure before releasing them. JP
  "rle16sti.dcm"
  "rle16loo.dcm"   
  # Jasper cannot handle this image, only kakadu so far, unless you patch jasper
  # but then it breaks other images...
  "CT_Phillips_JPEG2K_Decompr_Problem.dcm"
  #
  # ------------
  #
  # I probabely messed up smething with the .png, .tst of
  # the images I recently added to the Test Suite
  # I Black list them, until I find where is the pb.
  "US-RGB-8-esopecho.dcm"
  "MR_Philips_Intera_No_PrivateSequenceImplicitVR.dcm"
  "MR_Philips_Intera_PrivateSequenceImplicitVR.dcm"
  "MR_SIEMENS_forceLoad29-1010_29-1020.dcm"
  # An element contains a value of 4, but no data follows
  # So, the next (group, elem) is not correctly read
  "gdcm-MR-PHILIPS-16-Multi-Seq.dcm"
  )

# Create a specific list of dicom files that we know are part of a sequence
SET(SEQ_LIST
  "SIEMENS_MAGNETOM-12-MONO2-FileSeq[0-9].dcm"
  )

# Add a special test that requires dciodvfy from dicom3tools
INCLUDE(${GDCM_SOURCE_DIR}/CMake/FindDicom3Tools.cmake)

# This is a GLOB expression, change it into a list separated with a comma and \n
SET(GDCM_DATA_IMAGES)
SET(GDCM_DATA_SEQ_IMAGES)
FOREACH(filename ${GDCM_DATA_IMAGES_GLOB})
  GET_FILENAME_COMPONENT(filename_temp ${filename} NAME)
  IF(DCIODVFY_FOUND)
    ADD_TEST("DCIODVFY-INPUT-${filename_temp}" "${DCIODVFY_EXECUTABLE}" "${filename}")
    ADD_TEST("DCIODVFY-OUTPUT-${filename_temp}" "${DCIODVFY_EXECUTABLE}" "${GDCM_DATA_ROOT}/BaselineDicom/${filename_temp}")
  ENDIF(DCIODVFY_FOUND)
  STRING(REGEX MATCH ${filename_temp} bad_dicom ${BLACK_LIST})
  IF(NOT bad_dicom)
    SET(GDCM_DATA_IMAGES "${GDCM_DATA_IMAGES}\n\"${filename_temp}\",")
    # SEQ_LIST is a regular expression:
    STRING(REGEX MATCH ${SEQ_LIST} is_seq ${filename_temp})
    IF(is_seq)
      SET(GDCM_DATA_SEQ_IMAGES "${GDCM_DATA_SEQ_IMAGES}\n\"${filename_temp}\",")
    ENDIF(is_seq)
  ENDIF(NOT bad_dicom)
ENDFOREACH(filename)

# Populate GDCM_DATA_IMAGES:
FILE(WRITE "${GDCM_BINARY_DIR}/gdcmDataImages.h"
  "const char * const gdcmDataImages[] = { ${GDCM_DATA_IMAGES}\n0 };\n" 
)

# Populate GDCM_DATA_SEQ_IMAGES:
FILE(WRITE "${GDCM_BINARY_DIR}/gdcmDataSeqImages.h"
  "const char * const gdcmDataSeqImages[] = { ${GDCM_DATA_SEQ_IMAGES}\n0 };\n" 
)

#-----------------------------------------------------------------------------
# Trying to solve a bizarre problem when loading shared lib if gdcm is static
# Therefore I disable the test when building SHAREDLIBS
IF(UNIX AND NOT GDCM_BUILD_SHARED_LIBS)
  ADD_LIBRARY(dynmodule MODULE dynmodule.cxx)
  TARGET_LINK_LIBRARIES(dynmodule gdcm)
  ADD_EXECUTABLE(loadmodule loadmodule.cxx )
  TARGET_LINK_LIBRARIES(loadmodule gdcm -ldl)
  ADD_TEST(loadmodule ${CXX_TEST_PATH}/loadmodule)
ENDIF(UNIX AND NOT GDCM_BUILD_SHARED_LIBS)



