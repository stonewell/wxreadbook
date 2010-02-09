SUBDIRS = 7Zip res
bin_PROGRAMS = wxReadBook

AM_CXXFLAGS = -I$(srcdir)/../includes

wxReadBook_SOURCES = ArchiveExplorerDlg.cpp \
	ns/nsBIG5Statistics.cpp \
	BookMarksDlg.cpp \
	ns/nsEUCJPStatistics.cpp \
	ns/nsEUCKRStatistics.cpp \
	ns/nsEUCTWStatistics.cpp \
	ns/nsGB2312Statistics.cpp \
	ns/nsBIG5Verifier.cpp \
	ns/nsEUCStatistics.cpp \
	ns/nsVerifier.cpp \
	ReadBookApp.cpp \
	ReadBookCanvas.cpp \
	ReadBookDoc.cpp \
	ReadBookMainFrm.cpp \
	ReadBookPreference.cpp \
	ReadBookPreferenceDlg.cpp \
	ReadBookView.cpp \
	ns/nsCP1252Verifier.cpp \
	ns/nsDetector.cpp \
	ns/nsEUCJPVerifier.cpp \
	ns/nsEUCKRVerifier.cpp \
	ns/nsEUCSampler.cpp \
	ns/nsEUCTWVerifier.cpp \
	ns/nsGB18030Verifier.cpp \
	ns/nsGB2312Verifier.cpp \
	ns/nsHZVerifier.cpp \
	ns/nsICharsetDetectionObserver.cpp \
	ns/nsISO2022CNVerifier.cpp \
	ns/nsISO2022JPVerifier.cpp \
	ns/nsISO2022KRVerifier.cpp \
	ns/nsPSMDetector.cpp \
	ns/nsSJISVerifier.cpp \
	ns/nsUCS2BEVerifier.cpp \
	ns/nsUCS2LEVerifier.cpp \
	ns/nsUTF8Verifier.cpp \
	ArchiveExplorerDlg.h \
	ns/nsBIG5Statistics.h \
	BookMarksDlg.h \
	ns/nsEUCJPStatistics.h \
	ns/nsEUCKRStatistics.h \
	ns/nsEUCTWStatistics.h \
	ns/nsGB2312Statistics.h \
	ns/nsBIG5Verifier.h \
	ns/nsEUCStatistics.h \
	ns/nsVerifier.h \
	ReadBookApp.h \
	ReadBookCanvas.h \
	ReadBookDoc.h \
	ReadBookMainFrm.h \
	ReadBookPreference.h \
	ReadBookPreferenceDlg.h \
	ReadBookView.h \
	ns/nsCP1252Verifier.h \
	ns/nsDetector.h \
	ns/nsEUCJPVerifier.h \
	ns/nsEUCKRVerifier.h \
	ns/nsEUCSampler.h \
	ns/nsEUCTWVerifier.h \
	ns/nsGB18030Verifier.h \
	ns/nsGB2312Verifier.h \
	ns/nsHZVerifier.h \
	ns/nsICharsetDetectionObserver.h \
	ns/nsICharsetDetector.h \
	ns/nsISO2022CNVerifier.h \
	ns/nsISO2022JPVerifier.h \
	ns/nsISO2022KRVerifier.h \
	ns/nsPSMDetector.h \
	ns/nsSJISVerifier.h \
	ns/nsString.h \
	ns/nsUCS2BEVerifier.h \
	ns/nsUCS2LEVerifier.h \
	ns/nsUTF8Verifier.h \
	ReadBookBufferedDoc.h \
	ReadBookBufferedDoc.cpp \
	ReadBookBufferedView.h \
	ReadBookBufferedView.cpp \
	unicode/GBBig5Table.cpp \
	unicode/GBBig5Table.h 	\
	ReadBookSimpleView.h \
	ReadBookSimpleView.cpp \
	ReadBookSimpleDoc.h \
	ReadBookSimpleDoc.cpp \
	ReadBookPage.h \
	ReadBookPage.cpp \
	ReadBookLine.h \
	ReadBookLine.cpp \
	ReadBookChar.h \
	ReadBookChar.cpp \
	unicode/UnicodeHelper.h \
	unicode/UnicodeHelper.cpp \
	ReadBookDC.h \
	ReadBookDC.cpp \
	ObjectCache.cpp \
	ObjectCache.h \
	ReadBookKeys.h \
	version.h
	
wxReadBook_LDADD = 7Zip/libwxreadbook7zip.a \
	../addons/libtextprocess/libtextprocess.a \
	$(srcdir)/../libs/lib7zip.a \
	$(srcdir)/../libs/libcppcommon.a \
	$(srcdir)/../libs/libcppwindows.a

EXTRA_DIST = wxReadBook.vcproj wxReadBook.rc 

dist-hook:
	cp -rf $(srcdir)/wxReadBook*.vcproj $(distdir)

all-local:
	cp wxReadBook ..
