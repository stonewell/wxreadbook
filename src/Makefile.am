SUBDIRS = 7Zip res
bin_PROGRAMS = wxReadBook

AM_CXXFLAGS = -I$(srcdir)/../includes

wxReadBook_SOURCES = ArchiveExplorerDlg.cpp \
	nsBIG5Statistics.cpp \
	BookMarksDlg.cpp \
	nsEUCJPStatistics.cpp \
	nsEUCKRStatistics.cpp \
	nsEUCTWStatistics.cpp \
	nsGB2312Statistics.cpp \
	nsBIG5Verifier.cpp \
	nsEUCStatistics.cpp \
	nsVerifier.cpp \
	ReadBookApp.cpp \
	ReadBookCanvas.cpp \
	ReadBookDoc.cpp \
	ReadBookMainFrm.cpp \
	ReadBookPreference.cpp \
	ReadBookPreferenceDlg.cpp \
	ReadBookView.cpp \
	nsCP1252Verifier.cpp \
	nsDetector.cpp \
	nsEUCJPVerifier.cpp \
	nsEUCKRVerifier.cpp \
	nsEUCSampler.cpp \
	nsEUCTWVerifier.cpp \
	nsGB18030Verifier.cpp \
	nsGB2312Verifier.cpp \
	nsHZVerifier.cpp \
	nsICharsetDetectionObserver.cpp \
	nsISO2022CNVerifier.cpp \
	nsISO2022JPVerifier.cpp \
	nsISO2022KRVerifier.cpp \
	nsPSMDetector.cpp \
	nsSJISVerifier.cpp \
	nsUCS2BEVerifier.cpp \
	nsUCS2LEVerifier.cpp \
	nsUTF8Verifier.cpp \
	ArchiveExplorerDlg.h \
	nsBIG5Statistics.h \
	BookMarksDlg.h \
	nsEUCJPStatistics.h \
	nsEUCKRStatistics.h \
	nsEUCTWStatistics.h \
	nsGB2312Statistics.h \
	nsBIG5Verifier.h \
	nsEUCStatistics.h \
	nsVerifier.h \
	ReadBookApp.h \
	ReadBookCanvas.h \
	ReadBookDoc.h \
	ReadBookMainFrm.h \
	ReadBookPreference.h \
	ReadBookPreferenceDlg.h \
	ReadBookView.h \
	nsCP1252Verifier.h \
	nsDetector.h \
	nsEUCJPVerifier.h \
	nsEUCKRVerifier.h \
	nsEUCSampler.h \
	nsEUCTWVerifier.h \
	nsGB18030Verifier.h \
	nsGB2312Verifier.h \
	nsHZVerifier.h \
	nsICharsetDetectionObserver.h \
	nsICharsetDetector.h \
	nsISO2022CNVerifier.h \
	nsISO2022JPVerifier.h \
	nsISO2022KRVerifier.h \
	nsPSMDetector.h \
	nsSJISVerifier.h \
	nsString.h \
	nsUCS2BEVerifier.h \
	nsUCS2LEVerifier.h \
	nsUTF8Verifier.h \
	ReadBookBufferedDoc.h \
	ReadBookBufferedDoc.cpp \
	ReadBookBufferedView.h \
	ReadBookBufferedView.cpp \
	GBBig5Table.cpp \
	GBBig5Table.h 	\
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
	UnicodeHelper.h \
	UnicodeHelper.cpp
	
wxReadBook_LDADD = 7Zip/libwxreadbook7zip.a \
	$(srcdir)/../libs/lib7zip.a \
	$(srcdir)/../libs/libcppcommon.a \
	$(srcdir)/../libs/libcppwindows.a

EXTRA_DIST = wxReadBook.vcproj wxReadBook.rc 

dist-hook:
	cp -rf $(srcdir)/wxReadBook*.vcproj $(distdir)

all-local:
	cp wxReadBook ..
