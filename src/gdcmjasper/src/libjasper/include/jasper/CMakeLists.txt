INCLUDE_REGULAR_EXPRESSION("^(jas).*$")


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

CHECK_INCLUDE_FILE("dlfcn.h"       HAVE_DLFCN_H)
CHECK_INCLUDE_FILE("fcntl.h" HAVE_FCNTL_H)
CHECK_INCLUDE_FILE("inttypes.h" HAVE_INTTYPES_H)
CHECK_INCLUDE_FILE("io.h" HAVE_IO_H)
#/* Define to 1 if you have the `m' library (-lm). */
##define HAVE_LIBM 1

CHECK_INCLUDE_FILE("limits.h" HAVE_LIMITS_H)
CHECK_INCLUDE_FILE("memory.h" HAVE_MEMORY_H)
CHECK_INCLUDE_FILE("stdbool.h" HAVE_STDBOOL_H)
CHECK_INCLUDE_FILE("stddef.h" HAVE_STDDEF_H)
CHECK_INCLUDE_FILE("stdint.h" HAVE_STDINT_H)
CHECK_INCLUDE_FILE("stdlib.h" HAVE_STDLIB_H)
CHECK_INCLUDE_FILE("strings.h" HAVE_STRINGS_H)
CHECK_INCLUDE_FILE("string.h" HAVE_STRING_H)
CHECK_INCLUDE_FILE("sys/stat.h" HAVE_SYS_STAT_H)
CHECK_INCLUDE_FILE("sys/types.h" HAVE_SYS_TYPES_H)
CHECK_INCLUDE_FILE("unistd.h" HAVE_UNISTD_H)

#/* Have variable length arrays */
##define HAVE_VLA 1

#/* Define to 1 if you have the `vprintf' function. */
##define HAVE_VPRINTF 1

CHECK_INCLUDE_FILE("windows.h" HAVE_WINDOWS_H)
# Big endian thing:
INCLUDE (${CMAKE_ROOT}/Modules/TestBigEndian.cmake)
TEST_BIG_ENDIAN(WORDS_BIGENDIAN)

CONFIGURE_FILE(
  ${JASPER_SOURCE_DIR}/src/libjasper/include/jasper/jas_config.h.in
  ${JASPER_BINARY_DIR}/src/libjasper/include/jasper/jas_config.h
  @ONLY IMMEDIATE)
