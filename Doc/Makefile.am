HTML_DIR=$(datadir)/gdcm/html
TARGET_DIR=$(HTML_DIR)/$(DOC_MODULE)

EXTRA_DIST = \
  DoxyfileDeveloppers  \
  DoxyfileUsers        \
  DoxyDevelInstal.txt  \
  DoxyIntroduction.txt \
  DoxyPython.txt       \
  DoxyInstallation.txt \
  DoxyMainPage.txt

all-local: html.developper/index.html

if ENABLE_DOXYGEN
html.developper/index.html: DoxyfileDeveloppers
	doxygen DoxyfileDeveloppers
else
html.developper/index.html:
endif

clean-local:
	rm -f *~ *.bak
	rm -rf man latex html.developper rtf

install-data-local: html.developper
	$(mkinstalldirs) $(DESTDIR)$(TARGET_DIR)
	(installfiles=`echo html.developper/*`; \
        if test "$$installfiles" = 'html.developper/*'; \
        then echo '-- Nothing to install' ; \
        else \
          for i in $$installfiles; do \
            echo '-- Installing '$$i ; \
            $(INSTALL_DATA) $$i $(DESTDIR)$(TARGET_DIR); \
          done; \
        fi)

dist-hook:
	rm -rf $(distdir)/html.developper

.PHONY : html.developper latex man
