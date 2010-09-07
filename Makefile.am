SUBDIRS = addons src doc 
EXTRA_DIST = Makefile.msw \
	wxReadBook.sln 

dist-hook:
	cp -rf $(srcdir)/wxReadBook*.sln $(distdir)
	cp -rf $(srcdir)/wxreadbook.desktop.in $(distdir)

all-local: wxreadbook.desktop

clean-local: 
	-rm -rf wxreadbook.desktop
	-rm -rf wxReadBook

wxreadbook.desktop: wxreadbook.desktop.in
	-sed \
	-e 's|[@]prefix@|$(prefix)|g' \
	-e 's|[@]exec_prefix@|$(exec_prefix)|g' \
	-e 's|[@]bindir@|$(bindir)|g' \
	-e 's|[@]datarootdir@|$(datarootdir)|g' \
	< "$<" > "$@"

install-data-local:
	test -z "$(datadir)/applications" || $(MKDIR_P) "$(datadir)/applications"
	$(INSTALL_DATA) wxreadbook.desktop $(datadir)/applications
