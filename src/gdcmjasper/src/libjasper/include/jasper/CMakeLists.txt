INCLUDE_REGULAR_EXPRESSION("^.*$") 

#libjasperinclude_HEADERS = \
#  jasper.h \
#  jas_config.h \
#  jas_config2.h \
#  jas_cm.h \
#  jas_fix.h \
#  jas_debug.h \
#  jas_getopt.h \
#  jas_icc.h \
#  jas_image.h \
#  jas_init.h \
#  jas_malloc.h \
#  jas_math.h \
#  jas_seq.h \
#  jas_stream.h \
#  jas_string.h \
#  jas_tvp.h \
#  jas_types.h \
#  jas_version.h
#
#libjasperincludedir = $(includedir)/jasper

SET(JAS_VERSION "1.701.0")

INCLUDE (${CMAKE_ROOT}/Modules/CheckIncludeFile.cmake)

CHECK_INCLUDE_FILE("dlfcn.h"    HAVE_DLFCN_H)
CHECK_INCLUDE_FILE("fcntl.h"    HAVE_FCNTL_H)
CHECK_INCLUDE_FILE("inttypes.h" HAVE_INTTYPES_H)
CHECK_INCLUDE_FILE("io.h"       HAVE_IO_H)
#/* Define to 1 if you have the `m' library (-lm). */
##define HAVE_LIBM 1

CHECK_INCLUDE_FILE("limits.h"    HAVE_LIMITS_H)
CHECK_INCLUDE_FILE("memory.h"    HAVE_MEMORY_H)
CHECK_INCLUDE_FILE("stdbool.h"   HAVE_STDBOOL_H)
CHECK_INCLUDE_FILE("stddef.h"    HAVE_STDDEF_H)
CHECK_INCLUDE_FILE("stdint.h"    HAVE_STDINT_H)
CHECK_INCLUDE_FILE("stdlib.h"    HAVE_STDLIB_H)
CHECK_INCLUDE_FILE("strings.h"   HAVE_STRINGS_H)
CHECK_INCLUDE_FILE("string.h"    HAVE_STRING_H)
CHECK_INCLUDE_FILE("sys/stat.h"  HAVE_SYS_STAT_H)
CHECK_INCLUDE_FILE("sys/types.h" HAVE_SYS_TYPES_H)
CHECK_INCLUDE_FILE("unistd.h"    HAVE_UNISTD_H)

#/* Have variable length arrays */
##define HAVE_VLA 1

#/* Define to 1 if you have the `vprintf' function. */
##define HAVE_VPRINTF 1

CHECK_INCLUDE_FILE("windows.h" HAVE_WINDOWS_H)
# Big endian thing:
INCLUDE (${CMAKE_ROOT}/Modules/TestBigEndian.cmake)
TEST_BIG_ENDIAN(WORDS_BIGENDIAN)

# let's have some fun with long long and __int64...I don't even know why this lib need those types anyway...
INCLUDE(${CMAKE_ROOT}/Modules/CheckTypeSize.cmake)
CHECK_TYPE_SIZE("char"   GDCM_SIZEOF_CHAR)
CHECK_TYPE_SIZE("short"  GDCM_SIZEOF_SHORT)
CHECK_TYPE_SIZE("int"    GDCM_SIZEOF_INT)
CHECK_TYPE_SIZE("long"   GDCM_SIZEOF_LONG)
CHECK_TYPE_SIZE("float"  GDCM_SIZEOF_FLOAT)
CHECK_TYPE_SIZE("double" GDCM_SIZEOF_DOUBLE)

CHECK_TYPE_SIZE("uint"   GDCM_SIZEOF_UINT)
CHECK_TYPE_SIZE("ushort" GDCM_SIZEOF_USHORT)
CHECK_TYPE_SIZE("ulong"  GDCM_SIZEOF_ULONG)

CHECK_TYPE_SIZE("long long" GDCM_SIZEOF_LONG_LONG)
CHECK_TYPE_SIZE("__int64"   GDCM_SIZEOF___INT64)

IF(GDCM_SIZEOF___INT64)
  STRING(ASCII 35 POUND)
  FILE(WRITE ${GDCMJASPER_BINARY_DIR}/gdcmTestCompareTypes.cxx
    "${POUND}define TYPE_LONG_LONG long long\n"
    "typedef GDCM_TEST_COMPARE_TYPE_1 Type1;\n"
    "typedef GDCM_TEST_COMPARE_TYPE_2 Type2;\n"
    "void function(Type1**) {}\n"
    "int main() {\n"
    "  Type2** p = 0;\n"
    "  function(p);\n"
    "  return 0; }")
  IF("GDCM_TYPE_SAME_LONG_AND___INT64" MATCHES "^GDCM_TYPE_SAME_LONG_AND___INT64$")
    MESSAGE(STATUS "Checking whether long and __int64 are the same type")
    TRY_COMPILE(GDCM_TYPE_SAME_LONG_AND___INT64
      ${GDCMJASPER_BINARY_DIR}/
      ${GDCMJASPER_BINARY_DIR}/gdcmTestCompareTypes.cxx
      COMPILE_DEFINITIONS
      -DGDCM_TEST_COMPARE_TYPE_1=long
      -DGDCM_TEST_COMPARE_TYPE_2=__int64
      OUTPUT_VARIABLE OUTPUT)
    IF(GDCM_TYPE_SAME_LONG_AND___INT64)
      MESSAGE(STATUS "Checking whether long and __int64 are the same type -- yes")
      SET(GDCM_TYPE_SAME_LONG_AND___INT64 1 CACHE INTERNAL "Whether long and __int64 are the same type")
      WRITE_FILE(${CMAKE_BINARY_DIR}/CMakeOutput.log
        "Determining whether long and __int64 are the same type "
        "passed with the following output:\n"
        "${OUTPUT}\n" APPEND)
    ELSE(GDCM_TYPE_SAME_LONG_AND___INT64)
      MESSAGE(STATUS "Checking whether long and __int64 are the same type -- no")
      SET(GDCM_TYPE_SAME_LONG_AND___INT64 0 CACHE INTERNAL "Whether long and __int64 are the same type")
      WRITE_FILE(${CMAKE_BINARY_DIR}/CMakeError.log
        "Determining whether long and __int64 are the same type "
        "failed with the following output:\n"
        "${OUTPUT}\n" APPEND)
    ENDIF(GDCM_TYPE_SAME_LONG_AND___INT64)
  ENDIF("GDCM_TYPE_SAME_LONG_AND___INT64" MATCHES "^GDCM_TYPE_SAME_LONG_AND___INT64$")
  IF(GDCM_SIZEOF_LONG_LONG)
    IF("GDCM_TYPE_SAME_LONG_LONG_AND___INT64" MATCHES "^GDCM_TYPE_SAME_LONG_LONG_AND___INT64$")
      MESSAGE(STATUS "Checking whether long long and __int64 are the same type")
      TRY_COMPILE(GDCM_TYPE_SAME_LONG_LONG_AND___INT64
        ${GDCMJASPER_BINARY_DIR}
        ${GDCMJASPER_SOURCE_DIR}/gdcmTestCompareTypes.cxx
        COMPILE_DEFINITIONS
        -DGDCM_TEST_COMPARE_TYPE_1=TYPE_LONG_LONG
        -DGDCM_TEST_COMPARE_TYPE_2=__int64
        OUTPUT_VARIABLE OUTPUT)
      IF(GDCM_TYPE_SAME_LONG_LONG_AND___INT64)
        MESSAGE(STATUS "Checking whether long long and __int64 are the same type -- yes")
        SET(GDCM_TYPE_SAME_LONG_LONG_AND___INT64 1 CACHE INTERNAL "Whether long long and __int64 are the same type")
        WRITE_FILE(${CMAKE_BINARY_DIR}/CMakeOutput.log
          "Determining whether long long and __int64 are the same type "
          "passed with the following output:\n"
          "${OUTPUT}\n" APPEND)
      ELSE(GDCM_TYPE_SAME_LONG_LONG_AND___INT64)
        MESSAGE(STATUS "Checking whether long long and __int64 are the same type -- no")
        SET(GDCM_TYPE_SAME_LONG_LONG_AND___INT64 0 CACHE INTERNAL "Whether long long and __int64 are the same type")
        WRITE_FILE(${CMAKE_BINARY_DIR}/CMakeError.log
          "Determining whether long long and __int64 are the same type "
          "failed with the following output:\n"
          "${OUTPUT}\n" APPEND)
      ENDIF(GDCM_TYPE_SAME_LONG_LONG_AND___INT64)
    ENDIF("GDCM_TYPE_SAME_LONG_LONG_AND___INT64" MATCHES "^GDCM_TYPE_SAME_LONG_LONG_AND___INT64$")
  ENDIF(GDCM_SIZEOF_LONG_LONG)
  IF(NOT GDCM_TYPE_SAME_LONG_AND___INT64)
    IF(NOT GDCM_TYPE_SAME_LONG_LONG_AND___INT64)
      #  VS 6 cannot convert unsigned __int64 to double unless the
      # "Visual C++ Processor Pack" is installed.
      IF("GDCM_TYPE_CONVERT_UI64_TO_DOUBLE" MATCHES "^GDCM_TYPE_CONVERT_UI64_TO_DOUBLE$")
        MESSAGE(STATUS "Checking whether unsigned __int64 can convert to double")
        FILE(WRITE ${GDCMJASPER_BINARY_DIR}/gdcmTestCompareTypes.cxx
          "${POUND}define TYPE_UNSIGNED___INT64 unsigned __int64\n"
          "typedef GDCM_TEST_CONVERT_TYPE_TO TypeTo;\n"
          "typedef GDCM_TEST_CONVERT_TYPE_FROM TypeFrom;\n"
          "void function(TypeTo& l, TypeFrom const& r) {\n"
          "  l = static_cast<TypeTo>(r); }\n"
          "int main() {\n"
          "  TypeTo tTo = TypeTo();\n"
          "  TypeFrom tFrom = TypeFrom();\n"
          "  function(tTo, tFrom);\n"
          "  return 0; }")
        TRY_COMPILE(GDCM_TYPE_CONVERT_UI64_TO_DOUBLE
          ${GDCMJASPER_BINARY_DIR}
          ${GDCMJASPER_BINARY_DIR}/gdcmTestConvertTypes.cxx
          COMPILE_DEFINITIONS
          -DGDCM_TEST_CONVERT_TYPE_FROM=TYPE_UNSIGNED___INT64
          -DGDCM_TEST_CONVERT_TYPE_TO=double
          OUTPUT_VARIABLE OUTPUT)
        IF(GDCM_TYPE_CONVERT_UI64_TO_DOUBLE)
          MESSAGE(STATUS "Checking whether unsigned __int64 can convert to double -- yes")
          SET(GDCM_TYPE_CONVERT_UI64_TO_DOUBLE 1 CACHE INTERNAL "Whether unsigned __int64 can convert to double")
          WRITE_FILE(${CMAKE_BINARY_DIR}/CMakeOutput.log
            "Determining whether unsigned __int64 can convert to double "
            "passed with the following output:\n"
            "${OUTPUT}\n" APPEND)
        ELSE(GDCM_TYPE_CONVERT_UI64_TO_DOUBLE)
          MESSAGE(STATUS "Checking whether unsigned __int64 can convert to double -- no")
          SET(GDCM_TYPE_CONVERT_UI64_TO_DOUBLE 0 CACHE INTERNAL "Whether unsigned __int64 can convert to double")
          WRITE_FILE(${CMAKE_BINARY_DIR}/CMakeError.log
            "Determining whether unsigned __int64 can convert to double "
            "failed with the following output:\n"
            "${OUTPUT}\n" APPEND)
        ENDIF(GDCM_TYPE_CONVERT_UI64_TO_DOUBLE)
      ENDIF("GDCM_TYPE_CONVERT_UI64_TO_DOUBLE" MATCHES "^GDCM_TYPE_CONVERT_UI64_TO_DOUBLE$")
    ENDIF(NOT GDCM_TYPE_SAME_LONG_LONG_AND___INT64)
  ENDIF(NOT GDCM_TYPE_SAME_LONG_AND___INT64)
ENDIF(GDCM_SIZEOF___INT64)

# Enable the "long long" type if it is available.  It is standard in
# C99 and C++03 but not in earlier standards.
SET(GDCM_TYPE_USE_LONG_LONG)
IF(GDCM_SIZEOF_LONG_LONG)
  SET(GDCM_TYPE_USE_LONG_LONG 1)
ENDIF(GDCM_SIZEOF_LONG_LONG)

# Enable the "__int64" type if it is available and unique.  It is not
# standard.
SET(GDCM_TYPE_USE___INT64)
IF(GDCM_SIZEOF___INT64)
  IF(NOT GDCM_TYPE_SAME_LONG_AND___INT64)
    IF(NOT GDCM_TYPE_SAME_LONG_LONG_AND___INT64)
      SET(GDCM_TYPE_USE___INT64 1)
    ENDIF(NOT GDCM_TYPE_SAME_LONG_LONG_AND___INT64)
  ENDIF(NOT GDCM_TYPE_SAME_LONG_AND___INT64)
ENDIF(GDCM_SIZEOF___INT64)

IF (WIN32)
  IF (BUILD_SHARED_LIBS)
#    ADD_DEFINITIONS(-DJASPERDLL)
    SET(JASPERDLL 1)
  ELSE (BUILD_SHARED_LIBS)
    SET(JASPERSTATIC 1)
  ENDIF (BUILD_SHARED_LIBS)
ENDIF (WIN32)

CONFIGURE_FILE(
  "${GDCMJASPER_SOURCE_DIR}/src/libjasper/include/jasper/jas_config.h.in"
  "${GDCMJASPER_BINARY_DIR}/src/libjasper/include/jasper/jas_config.h"
  @ONLY IMMEDIATE)
