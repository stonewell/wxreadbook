SUBDIRS = addons src doc 
EXTRA_DIST = Makefile.msw \
	wxReadBook.sln 

dist-hook:
	cp -rf $(srcdir)/wxReadBook*.sln $(distdir)

install-data-local:
	test -z "$(datadir)/applications" || $(MKDIR_P) "$(datadir)/applications"
	$(INSTALL_DATA) wxreadbook.desktop $(datadir)/applications
