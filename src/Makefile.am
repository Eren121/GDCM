SUBDIRS = jpeg

LTVERSION = 0:1:0

lib_LTLIBRARIES = libgdcm.la

CXXFLAGS = @CXXFLAGS@ -DPUB_DICT_PATH=\"$(datadir)/gdcm/\" \
           -D__STDC_LIMIT_MACROS  # Refer to stdint.h

libgdcm_la_LIBADD = \
   $(top_builddir)/src/jpeg/libijg12/libgdcmijpeg12.la  \
   $(top_builddir)/src/jpeg/libijg8/libgdcmijpeg8.la    \
   $(top_builddir)/src/jpeg/ljpg/libgdcmjpg.la 

libgdcm_la_SOURCES=                \
   gdcmDicomDir.cxx                \
   gdcmDicomDirElement.cxx         \
   gdcmDict.cxx                    \
   gdcmDictEntry.cxx               \
   gdcmDictSet.cxx                 \
   gdcmDirList.cxx                 \
   gdcmException.cxx               \
   gdcmFile.cxx                    \
   gdcmHeader.cxx                  \
   gdcmHeaderEntry.cxx             \
   gdcmHeaderHelper.cxx            \
   gdcmImage.cxx                   \
   gdcmJpeg12.cxx                  \
   gdcmJpeg2000.cxx                \
   gdcmJpeg.cxx                    \
   gdcmObject.cxx                  \
   gdcmParsePixels.cxx             \
   gdcmParser.cxx                  \
   gdcmPatient.cxx                 \
   gdcmRLE.cxx                     \
   gdcmSerie.cxx                   \
   gdcmStudy.cxx                   \
   gdcmTS.cxx                      \
   gdcmUtil.cxx                    \
   gdcmVR.cxx 


libgdcmincludedir = $(includedir)
libgdcminclude_HEADERS =           \
	gdcmCommon.h                    \
	gdcmDicomDir.h                  \
   gdcmDicomDirElement.h           \
	gdcmDictEntry.h                 \
	gdcmDict.h                      \
	gdcmDictSet.h                   \
	gdcmDirList.h                   \
	gdcmException.h                 \
	gdcmFile.h                      \
	gdcm.h                          \
	gdcmHeaderEntry.h               \
	gdcmHeader.h                    \
	gdcmHeaderHelper.h              \
	gdcmImage.h                     \
	gdcmObject.h                    \
	gdcmParser.h                    \
	gdcmPatient.h                   \
	gdcmSerie.h                     \
	gdcmStudy.h                     \
	gdcmVR.h                        \
	gdcmTS.h                      


EXTRA_DIST =   \
	gdcmUtil.h      

