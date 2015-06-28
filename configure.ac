AC_PREREQ([2.66])
AC_INIT([mate-applet-switch], [0.0.1], [mcrapet@gmail.com], [mate-applet-switch], [https://github.com/mcrapet/mate-applet-switch])

AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE([1.9 tar-ustar dist-xz no-dist-gzip foreign no-define])
AC_CONFIG_MACRO_DIR(m4)
AM_SILENT_RULES([yes])

dnl Checks for programs
AC_PROG_CC
AC_PROG_INSTALL

dnl Gettext
dnl AM_GLIB_GNU_GETTEXT
dnl GETTEXT_PACKAGE=mate-applet-switch
dnl dnl AM_GNU_GETTEXT_VERSION(0.18.1)
dnl AC_SUBST(GETTEXT_PACKAGE)
dnl AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE, "$GETTEXT_PACKAGE", GETTEXT_PACKAGE)
dnl if test "$USE_NLS" != "yes" ; then
dnl   AC_MSG_WARN(["gettext not found. You're losing internationalization."])
dnl fi

dnl Use pkg-config to check for dependencies
PKG_PROG_PKG_CONFIG
GLIB_REQUIRED=2.22.0
GIO_REQUIRED=2.15.3
LIBPANEL4_REQUIRED=1.7.0
GTK_REQUIRED=2.24.0

dnl Check for requires packages
PKG_CHECK_MODULES(GLIB, glib-2.0 >= $GLIB_REQUIRED gobject-2.0 >= $GLIB_REQUIRED)
PKG_CHECK_MODULES(GIO, gio-2.0 >= $GIO_REQUIRED gio-unix-2.0)

dnl FIXME: check later for GTK+ 3.0
PKG_CHECK_MODULES(GTK, gtk+-2.0 >= $GTK_REQUIRED)
PKG_CHECK_MODULES(MATE_APPLETS4, libmatepanelapplet-4.0 >= $LIBPANEL4_REQUIRED)

dnl Used in org.mate.* service files
if test "x$prefix" = "xNONE" ; then
  LIBEXECDIR=/usr/local/libexec
else
  LIBEXECDIR=$prefix/libexec
fi
if test "$libexecdir" != '${exec_prefix}/libexec'; then
  LIBEXECDIR=$libexecdir
fi
AC_SUBST([LIBEXECDIR])

AC_OUTPUT([Makefile
           src/Makefile
           src/org.mate.applets.SwitchApplet.mate-panel-applet
           src/org.mate.panel.applet.SwitchApplet.service
           src/org.mate.panel.applet.SwitchApplet.gschema.xml
           images/Makefile])

AC_MSG_NOTICE([
------------------------------------------------
${PACKAGE_NAME} Version ${PACKAGE_VERSION}

Prefix: '${prefix}'
Compiler: '${CC} ${CFLAGS} ${CPPFLAGS}'
------------------------------------------------
Configuration complete. Now, run make.
])
