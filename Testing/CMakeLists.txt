#For later : rewrite this things for more consistencies, such as a 'FOREACH'

#TODO:
#http://www.cmake.org/pipermail/cmake/2001-November/002491.html
#So that dll is copied to each subdir where needed

LINK_LIBRARIES(gdcm)
INCLUDE_DIRECTORIES(
  ${GDCM_SOURCE_DIR}/src/
)

SET(PrintHeader_SOURCES
  PrintHeader.cxx
)
ADD_EXECUTABLE(PrintHeader ${PrintHeader_SOURCES})

SET(testWrite_SOURCES
  testWrite.cxx
)
ADD_EXECUTABLE(testWrite ${testWrite_SOURCES})

SET(testChangeEntete_SOURCES
  testChangeEntete.cxx
)
ADD_EXECUTABLE(testChangeEntete ${testChangeEntete_SOURCES})

SET(hashtest_SOURCES
  hashtest.cxx
)
ADD_EXECUTABLE(hashtest ${hashtest_SOURCES})

SET(bug1_SOURCES
  bug1.cxx
)
ADD_EXECUTABLE(bug1 ${bug1_SOURCES})

SET(pourFindTaggs_SOURCES
  pourFindTaggs.cxx
)
ADD_EXECUTABLE(pourFindTaggs ${pourFindTaggs_SOURCES})

SET(dcm2acr_SOURCES
  dcm2acr.cxx
)
ADD_EXECUTABLE(dcm2acr ${dcm2acr_SOURCES})
