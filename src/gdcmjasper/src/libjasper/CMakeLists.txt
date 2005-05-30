
# Convenient library are not portable (at least does not work on Win32). 
# Therefore instead of doing a SUBDIRS, let's do an INCLUDE of the subdirs
# and build the global library here...
SUBDIRS(include/jasper) # This is one can stay here

MACRO(GDCM_CONVENIENT_LIBRARY)
  #MESSAGE("ARGC=${ARGC}")
  #MESSAGE("ARGV=${ARGV}")
  SET(GDCMJASPER_SOURCES)
  FOREACH(SUBDIR ${ARGV})
    INCLUDE("${GDCMJASPER_SOURCE_DIR}/src/libjasper/${SUBDIR}/CMakeLists.txt")
    #MESSAGE("FILES=${lib${SUBDIR}_la_SOURCES}")
    #SET(convenient_lib${SUBDIR}_la_SOURCES)
    FOREACH(file ${lib${SUBDIR}_la_SOURCES})
      #MESSAGE("${SUBDIR}/${file}")
      #MESSAGE("BAR ${convenient_lib${SUBDIR}_la_SOURCES}")
      #MESSAGE("${GDCMJASPER_SOURCE_DIR}/src/libjasper/${SUBDIR}/${file}")
      #SET(convenient_lib${SUBDIR}_la_SOURCES
      #  "${GDCMJASPER_SOURCE_DIR}/src/libjasper/${SUBDIR}/${file}"
      #  "${convenient_lib${SUBDIR}_la_SOURCES}"
      #  )
      SET(GDCMJASPER_SOURCES
        ${GDCMJASPER_SOURCE_DIR}/src/libjasper/${SUBDIR}/${file}
        ${GDCMJASPER_SOURCES}
        )
    ENDFOREACH(file)
    #MESSAGE("R= ${convenient_lib${SUBDIR}_la_SOURCES}")
  ENDFOREACH(SUBDIR)
ENDMACRO(GDCM_CONVENIENT_LIBRARY)

GDCM_CONVENIENT_LIBRARY(base bmp jp2 jpc jpg mif pgx pnm ras) 

#MESSAGE("src= ${GDCMJASPER_SOURCES}")
ADD_LIBRARY(gdcmjasper ${GDCMJASPER_SOURCES})


