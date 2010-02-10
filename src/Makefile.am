SUBDIRS = 7Zip res
bin_PROGRAMS = wxReadBook

AM_CXXFLAGS = -I$(srcdir)/../addons/lib7zip/Lib7Zip \
	-I$(srcdir)/../addons/libtextprocess/src/core/ \
	-I$(srcdir)/../addons/libportablethread/includes

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
	presentation/ReadBookDoc.cpp \
	ReadBookMainFrm.cpp \
	ReadBookPreference.cpp \
	ReadBookPreferenceDlg.cpp \
	presentation/ReadBookView.cpp \
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
	presentation/ReadBookDoc.h \
	ReadBookMainFrm.h \
	ReadBookPreference.h \
	ReadBookPreferenceDlg.h \
	presentation/ReadBookView.h \
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
	presentation/buffered/ReadBookBufferedDoc.h \
	presentation/buffered/ReadBookBufferedDoc.cpp \
	presentation/buffered/ReadBookBufferedView.h \
	presentation/buffered/ReadBookBufferedView.cpp \
	unicode/GBBig5Table.cpp \
	unicode/GBBig5Table.h 	\
	presentation/simple/ReadBookSimpleView.h \
	presentation/simple/ReadBookSimpleView.cpp \
	presentation/simple/ReadBookSimpleDoc.h \
	presentation/simple/ReadBookSimpleDoc.cpp \
	presentation/simple/ReadBookPage.h \
	presentation/simple/ReadBookPage.cpp \
	presentation/simple/ReadBookLine.h \
	presentation/simple/ReadBookLine.cpp \
	presentation/simple/ReadBookChar.h \
	presentation/simple/ReadBookChar.cpp \
	unicode/UnicodeHelper.h \
	unicode/UnicodeHelper.cpp \
	presentation/ReadBookDC.h \
	presentation/ReadBookDC.cpp \
	presentation/simple/ObjectCache.cpp \
	presentation/simple/ObjectCache.h \
	presentation/ReadBookKeys.h \
	ReadBookDocManager.h \
	ReadBookDocManager.cpp \
	ReadBookHelperFunc.h \
	ReadBookHelperFunc.cpp \
	version.h
	
wxReadBook_LDADD = 7Zip/libwxreadbook7zip.a \
	../addons/libtextprocess/libtextprocess.a \
	../addons/lib7zip/Lib7Zip/lib7zip.a

EXTRA_DIST = wxReadBook.vcproj wxReadBook.rc 

dist-hook:
	cp -rf $(srcdir)/wxReadBook*.vcproj $(distdir)

all-local:
	cp wxReadBook ..
