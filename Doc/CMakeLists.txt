#-----------------------------------------------------------------------------
# Build the documentation

SET(HTML_DIR
  ${GDCM_DATA_DIR}/html
)

SET(TARGET_DIR
  ${HTML_DIR}/${DOC_MODULE}
)

#EXTRA_DIST = \
#  DoxyfileDeveloppers  \
#  DoxyfileUsers        \
#  DoxyDevelInstal.txt  \
#  DoxyIntroduction.txt \
#  DoxyPython.txt       \
#  DoxyInstallation.txt \
#  DoxyMainPage.txt

IF(GDCM_DOXYGEN)

#  ADD_CUSTOM_COMMAND(
#      SOURCE    ${GDCM_SOURCE_DIR}/Doc/DoxyfileDeveloppers
#      COMMAND   ${DOXYGEN}
#      ARGS      ${GDCM_SOURCE_DIR}/Doc/DoxyfileDeveloppers
#      TARGET    mathieu
#      OUTPUT   ${GDCM_BINARY_DIR}/gdcmPython/gdcm_wrap.cxx
#)

# http://www.cmake.org/pipermail/cmake/2003-April/003673.html
# Too bad CMake doesn't support multiline ""
SET(GDCM_DOXYGEN_INPUT_COMMON
  "${GDCM_SOURCE_DIR}/src   ${GDCM_SOURCE_DIR}/Doc/DoxyPython.txt   ${GDCM_SOURCE_DIR}/Doc/DoxyMainPage.txt  ${GDCM_SOURCE_DIR}/Doc/DoxyInstallation.txt  ${GDCM_SOURCE_DIR}/Doc/DoxyIntroduction.txt"
)
SET(GDCM_DOXYGEN_EXCLUDE_COMMON
)

#Should rewrote this thing with FOREACH:
#-----------------------------------------------------------------------------
# Build the documentation for developpers

  SET(GDCM_HTML_OUTPUT
    html.developper
  )
  SET(GDCM_DOXYGEN_INPUT
    "${GDCM_DOXYGEN_INPUT_COMMON} ${GDCM_SOURCE_DIR}/Doc/DoxyDevelInstal.txt"
  )
  SET(GDCM_DOXYGEN_EXCLUDE
    "${GDCM_DOXYGEN_EXCLUDE_COMMON}  ${GDCM_SOURCE_DIR@/src/iddcmjpeg.h     ${GDCM_SOURCE_DIR@/src/src/gdcmJpeg.cxx"
  )
  CONFIGURE_FILE(
      ${GDCM_SOURCE_DIR}/Doc/doxygen.config.in
      ${GDCM_BINARY_DIR}/Doc/DoxyfileDeveloppers
      IMMEDIATE
  )

  ADD_CUSTOM_TARGET(doc-developer
   ALL
   ${DOXYGEN}
   ${GDCM_BINARY_DIR}/Doc/DoxyfileDeveloppers)

#-----------------------------------------------------------------------------
# Build the documentation for user

  SET(GDCM_HTML_OUTPUT
    html.user
  )
  SET(GDCM_DOXYGEN_INPUT
    "${GDCM_DOXYGEN_INPUT_COMMON}"
  )
  SET(GDCM_DOXYGEN_EXCLUDE
    "${GDCM_DOXYGEN_EXCLUDE_COMMON}"
  )
  CONFIGURE_FILE(
      ${GDCM_SOURCE_DIR}/Doc/doxygen.config.in
      ${GDCM_BINARY_DIR}/Doc/DoxyfileUsers
      IMMEDIATE
  )

  ADD_CUSTOM_TARGET(doc-user
   ALL
   ${DOXYGEN}
   ${GDCM_BINARY_DIR}/Doc/DoxyfileUsers)


  INSTALL_FILES(${HTML_DIR} "\\.css$")
  INSTALL_FILES(${HTML_DIR} "\\.gif$")
  INSTALL_FILES(${HTML_DIR} "\\.html$")
  #INSTALL_FILES(${HTML_DIR} "\\.pl$")
  #INSTALL_FILES(${HTML_DIR} "\\.stop$")
ENDIF(GDCM_DOXYGEN)

#-----------------------------------------------------------------------------
# m4 that hasn't been translated

#dist-hook:
#	rm -rf $(distdir)/html.developper

#.PHONY : html.developper latex man
