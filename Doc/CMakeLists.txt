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

#all-local: html.developper/index.html

IF(GDCM_DOXYGEN)

#  ADD_CUSTOM_COMMAND(
#      SOURCE    ${GDCM_SOURCE_DIR}/Doc/DoxyfileDeveloppers
#      COMMAND   ${DOXYGEN}
#      ARGS      ${GDCM_SOURCE_DIR}/Doc/DoxyfileDeveloppers
#      TARGET    mathieu
#      OUTPUT   ${GDCM_BINARY_DIR}/gdcmPython/gdcm_wrap.cxx
#)

# http://www.cmake.org/pipermail/cmake/2003-April/003673.html
CONFIGURE_FILE(
    ${GDCM_SOURCE_DIR}/Doc/doxygen.config.in
    ${GDCM_BINARY_DIR}/Doc/DoxyfileDeveloppers
)

  ADD_CUSTOM_TARGET(doc
   ALL
   ${DOXYGEN}
   ${GDCM_BINARY_DIR}/Doc/DoxyfileDeveloppers)

  INSTALL_FILES(${HTML_DIR} "\\.css$")
  INSTALL_FILES(${HTML_DIR} "\\.gif$")
  INSTALL_FILES(${HTML_DIR} "\\.html$")
  #INSTALL_FILES(${HTML_DIR} "\\.pl$")
  #INSTALL_FILES(${HTML_DIR} "\\.stop$")
ENDIF(GDCM_DOXYGEN)

#if ENABLE_DOXYGEN
#html.developper/index.html: DoxyfileDeveloppers
#	doxygen DoxyfileDeveloppers
#else
#html.developper/index.html:
#endif

#clean-local:
#	rm -f *~ *.bak
#	rm -rf man latex html.developper rtf

#install-data-local: html.developper
#	$(mkinstalldirs) $(DESTDIR)$(TARGET_DIR)
#	(installfiles=`echo html.developper/*`; \
#        if test "$$installfiles" = 'html.developper/*'; \
#        then echo '-- Nothing to install' ; \
#        else \
#          for i in $$installfiles; do \
#            echo '-- Installing '$$i ; \
#            $(INSTALL_DATA) $$i $(DESTDIR)$(TARGET_DIR); \
#          done; \
#        fi)

#dist-hook:
#	rm -rf $(distdir)/html.developper

#.PHONY : html.developper latex man
