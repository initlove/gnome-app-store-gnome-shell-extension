AC_INIT([libgnome-app-store], 0.1)
AC_CONFIG_SRCDIR([libgnome-app-store])

AM_INIT_AUTOMAKE([1.9 foreign no-dist-gzip dist-bzip2 tar-ustar])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

LT_VERSION=2:0:0
AC_SUBST(LT_VERSION)

LT_PREREQ([2.2.6])
LT_INIT([dlopen win32-dll disable-static])

IT_PROG_INTLTOOL([0.40.6])

AC_PROG_CC
PKG_PROG_PKG_CONFIG

dnl If you add a version number here, you *must* add an AC_SUBST line for
dnl it too, or it will never make it into the spec file!

GNOME_DESKTOP_REQUIRED=3.0
GDK_PIXBUF_REQUIRED=2.21.3
GTK_REQUIRED=3.0
GLIB_REQUIRED=2.19.1
XRANDR_REQUIRED=1.2
GSETTINGS_DESKTOP_SCHEMAS_REQUIRED=0.1.4
XEXT_REQUIRED=1.2

AC_SUBST(GTK_REQUIRED)
AC_SUBST(GLIB_REQUIRED)
AC_SUBST(GDK_PIXBUF_REQUIRED)
AC_SUBST(XRANDR_REQUIRED)
AC_SUBST(XEXT_REQUIRED)

dnl we need x11 for GnomeBG

PKG_CHECK_MODULES(XLIB, x11,
    X11_PACKAGE=x11,
    [X11_PACKAGE=
     AC_PATH_XTRA
     if test "x$no_x" = xyes; then
       AC_MSG_ERROR("no (requires X development libraries)")
     else
       XLIB_LIBS="$X_PRE_LIBS $X_LIBS -lX11 $X_EXTRA_LIBS"
       XLIB_CFLAGS=$X_CFLAGS
     fi])
AC_SUBST(X11_PACKAGE)
AC_SUBST(XLIB_CFLAGS)
AC_SUBST(XLIB_LIBS)

dnl Checks for X's RANDR extension

AC_MSG_CHECKING(for xrandr)
if $PKG_CONFIG --atleast-version $XRANDR_REQUIRED xrandr; then
  AC_MSG_RESULT(yes)
  AC_DEFINE(HAVE_RANDR, 1,
            [Define if the xrandr-$XRANDR_REQUIRED library is present])
  have_randr=yes
  RANDR_PACKAGE=xrandr
else
  AC_MSG_RESULT(no)
  have_randr=no
  RANDR_PACKAGE=
fi

AC_SUBST(RANDR_PACKAGE)

dnl pkg-config dependency checks

PKG_CHECK_MODULES(LIBGNOME_APP_STORE, gnome-desktop-3.0 >= $GNOME_DESKTOP_REQUIRED, gdk-pixbuf-2.0 >= $GDK_PIXBUF_REQUIRED gtk+-3.0 >= $GTK_REQUIRED glib-2.0 >= $GLIB_REQUIRED gio-2.0 >= $GLIB_REQUIRED gsettings-desktop-schemas >= $GSETTINGS_DESKTOP_SCHEMAS_REQUIRED $RANDR_PACKAGE xext >= $XEXT_REQUIRED)

dnl gnome-doc-utils stuff

GNOME_DOC_INIT

dnl Language Support

GETTEXT_PACKAGE=libgnome-app-store-0.1
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE, "$GETTEXT_PACKAGE",
		   [The gettext translation domain])
AC_SUBST(GETTEXT_PACKAGE)

AM_GLIB_GNU_GETTEXT

GOBJECT_INTROSPECTION_CHECK([0.9.7])

AC_CONFIG_FILES([
Makefile
libgnome-app-store/Makefile
libgnome-app-store/libgnome-app-store-0.1.pc
libgnome-app-store/libgnome-app-store-0.1-uninstalled.pc
po/Makefile.in
])

AC_OUTPUT
