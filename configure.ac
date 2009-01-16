AC_INIT([wxReadBook], [1.0], [jingnan.si@gmail.com])
AC_CONFIG_SRCDIR(src/ReadBookApp.cpp)
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_PROG_CC
AC_PROG_CXX
AC_C_CONST
AC_HEADER_STDBOOL

AC_LANG(C++)

dnl enable debug
AC_ARG_ENABLE([debug],
     [  --enable-debug    Turn on debugging],
     [case "${enableval}" in
       yes) debug=yes ;;
       no)  debug=no ;;
       *) AC_MSG_ERROR([bad value ${enableval} for --enable-debug]) ;;
     esac],[debug=no])

if test "$debug" = yes; then
    DEBUG_FLAG=-g
else
    DEBUG_FLAG=
fi

dnl enable static
AC_ARG_ENABLE([static],
     [  --enable-static    Turn on static link to wxWidget],
     [case "${enableval}" in
       yes) static=yes ;;
       no)  static=no ;;
       *) AC_MSG_ERROR([bad value ${enableval} for --enable-static]) ;;
     esac],[static=no])

AC_CHECK_PROG(WX_CONFIG,[wx-config],yes,no)
if test "$WX_CONFIG" = no; then
	AC_MSG_FAILURE(wx-config not exists)
fi

CXXFLAGS="`wx-config --cxxflags --static=$static --debug=$debug` $DEBUG_FLAG"
CPPFLAGS="`wx-config --cppflags --static=$static --debug=$debug` $DEBUG_FLAG"
CFLAGS="`wx-config --cflags --static=$static --debug=$debug` $DEBUG_FLAG"
LIBS="`wx-config --libs --static=$static --debug=$debug`"

AC_CHECK_HEADERS(["wx/wx.h"],
	[AC_MSG_RESULT(checking for wx/wx.h... OK)], 
	[AC_MSG_FAILURE(wx/wx.h is not found)]
	)

AC_CONFIG_FILES([
	Makefile
	src/Makefile
])

AC_OUTPUT