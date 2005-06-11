
# Convenient library are not portable (at least does not work on Win32). 
# Therefore instead of doing a SUBDIRS, let's do an INCLUDE of the subdirs
# and build the global library here...
SUBDIRS(include/jasper) # This is one can stay here

MACRO(GDCM_CONVENIENT_LIBRARY)
  SET(GDCMJASPER_SOURCES)
  FOREACH(SUBDIR ${ARGV})
    INCLUDE("${GDCMJASPER_SOURCE_DIR}/src/libjasper/${SUBDIR}/CMakeLists.txt")
    #SET(convenient_lib${SUBDIR}_la_SOURCES)
    FOREACH(file ${lib${SUBDIR}_la_SOURCES})
      #SET(convenient_lib${SUBDIR}_la_SOURCES
      #  "${GDCMJASPER_SOURCE_DIR}/src/libjasper/${SUBDIR}/${file}"
      #  "${convenient_lib${SUBDIR}_la_SOURCES}"
      #  )
      SET(GDCMJASPER_SOURCES
        ${GDCMJASPER_SOURCE_DIR}/src/libjasper/${SUBDIR}/${file}
        ${GDCMJASPER_SOURCES}
        )
    ENDFOREACH(file)
  ENDFOREACH(SUBDIR)
ENDMACRO(GDCM_CONVENIENT_LIBRARY)

# Yeah ! I know how come I only did jpeg 8bits... bla bla
# I'll wait until I actually such beast of jpeg2000 + some
# crazy 16bits lossless jpeg...
IF(HAVE_LIBJPEG)
  INCLUDE_DIRECTORIES(${GDCM_BINARY_DIR}/src/gdcmjpeg/8)
ENDIF(HAVE_LIBJPEG)

# For more info about this patch see the thread on gcc:
# [ld: common symbols not allowed with MH_DYLIB output format with the -multi_module option]
# http://www.mail-archive.com/gcc@gcc.gnu.org/msg05981.html
IF(APPLE)
  SET_SOURCE_FILES_PROPERTIES(
    "${GDCMJASPER_SOURCE_DIR}/src/libjasper/jpc/jpc_t1cod.c"
    PROPERTIES COMPILE_FLAGS -fno-common)
ENDIF(APPLE)

INCLUDE_DIRECTORIES(${GDCMJASPER_BINARY_DIR}/src/libjasper/include)
INCLUDE_DIRECTORIES(${GDCMJASPER_SOURCE_DIR}/src/libjasper/include)
GDCM_CONVENIENT_LIBRARY(base bmp jp2 jpc jpg mif pgx pnm ras) 

#MESSAGE("src= ${GDCMJASPER_SOURCES}")
ADD_LIBRARY(gdcmjasper ${GDCMJASPER_SOURCES})
IF(HAVE_LIBJPEG)
  TARGET_LINK_LIBRARIES(gdcmjasper gdcmjpeg8)
ENDIF(HAVE_LIBJPEG)


