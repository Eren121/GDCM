HTML_DIR=$(datadir)/gdcm/html
TARGET_DIR=$(HTML_DIR)/$(DOC_MODULE)

EXTRA_DIST = Doxyfile requirement.txt

all-local: html/index.html

if ENABLE_DOXYGEN
html/index.html: Doxyfile
	doxygen Doxyfile
else
html/index.html:
endif

clean-local:
	rm -f *~ *.bak
	rm -rf man latex html rtf

install-data-local: html
	$(mkinstalldirs) $(DESTDIR)$(TARGET_DIR)
	(installfiles=`echo html/*`; \
        if test "$$installfiles" = 'html/*'; \
        then echo '-- Nothing to install' ; \
        else \
          for i in $$installfiles; do \
            echo '-- Installing '$$i ; \
            $(INSTALL_DATA) $$i $(DESTDIR)$(TARGET_DIR); \
          done; \
        fi)

dist-hook:
	rm -rf $(distdir)/html

.PHONY : html latex man
