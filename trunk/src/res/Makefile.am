EXTRA_DIST = file.bmp file32.bmp \
	folder.bmp folder32.bmp \
	file.xpm file32.xpm \
	folder.xpm folder32.xpm \
	BOOK06.xpm BOOK06.ICO \
	wxreadbook.png

install-data-local:
	test -z "$(datadir)/pixmaps" || $(MKDIR_P) "$(datadir)/pixmaps"
	$(INSTALL_DATA) $(srcdir)/wxreadbook.png $(datadir)/pixmaps
