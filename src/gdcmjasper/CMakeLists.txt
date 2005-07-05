PROJECT(GDCMJASPER C)

#-----------------------------------------------------------------------------
# Test how compiler handle the case of a malloc(0). If it returns 0
# then we are doomed (jasper does not support this case)
IF("NULL_ON_MALLOC_0" MATCHES "^NULL_ON_MALLOC_0$")
  STRING(ASCII 35 POUND)
  FILE(WRITE ${GDCMJASPER_BINARY_DIR}/jasperNULLmalloc.c
    "${POUND}include <stdlib.h>\n"
    "int main() { return (int)malloc(0); }")
  MESSAGE(STATUS "Checking if compiler return NULL on malloc(0)")
  TRY_RUN(RESULT_NULL_ON_MALLOC_0 COMPILE_NULL_ON_MALLOC_0
      "${GDCMJASPER_BINARY_DIR}"
      "${GDCMJASPER_BINARY_DIR}/jasperNULLmalloc.c"
      OUTPUT_VARIABLE OUTPUT)
  IF(RESULT_NULL_ON_MALLOC_0 EQUAL 0)
    MESSAGE(STATUS "Checking if compiler return NULL on malloc(0) -- yes")
    SET(NULL_ON_MALLOC_0 1 CACHE INTERNAL "Check if compiler returns NULL on malloc(0)")
    WRITE_FILE(${CMAKE_BINARY_DIR}/CMakeOutput.log
      "Determining how compiler implement malloc(0)"
      "passed with the following output:\n"
      "${OUTPUT}\n" APPEND)
  ELSE(RESULT_NULL_ON_MALLOC_0 EQUAL 0)
    MESSAGE(STATUS "Checking if compiler returns NULL on malloc(0) -- no")
    SET(NULL_ON_MALLOC_0 0 CACHE INTERNAL "Check if compiler returns NULL on malloc(0)")
    WRITE_FILE(${CMAKE_BINARY_DIR}/CMakeError.log
      "Determining how compiler implement malloc(0)"
      "failed with the following output:\n"
      "${OUTPUT}\n" APPEND)
  ENDIF(RESULT_NULL_ON_MALLOC_0 EQUAL 0)
ENDIF("NULL_ON_MALLOC_0" MATCHES "^NULL_ON_MALLOC_0$")

#OPTION(HAVE_LIBJPEG "Do you have ijg" OFF)
IF(GDCM_SOURCE_DIR)
  SET(HAVE_LIBJPEG ON CACHE BOOL "Do you have ijg" FORCE)
ELSE(GDCM_SOURCE_DIR)
  SET(HAVE_LIBJPEG OFF CACHE BOOL "Do you have ijg" FORCE)
ENDIF(GDCM_SOURCE_DIR)

MARK_AS_ADVANCED(HAVE_LIBJPEG)

SUBDIRS(src)
