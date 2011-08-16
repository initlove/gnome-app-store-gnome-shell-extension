CLEANFILES =

lib_LTLIBRARIES = libgnome-app-store-0.1.la

AM_CPPFLAGS =							\
	-I$(top_srcdir)						\
	$(LIBGNOME_APP_STORE_CFLAGS)					\
	$(XLIB_CFLAGS)						\
	-DG_LOG_DOMAIN=\"LibgnomeAppStore\"				\
	-DGNOMELOCALEDIR=\""$(prefix)/$(DATADIRNAME)/locale\""	\
	-DPNP_IDS=\""$(PNP_IDS)"\"				\
	$(DISABLE_DEPRECATED_CFLAGS)

AM_CFLAGS = $(WARN_CFLAGS)

introspection_sources = 		\
	libgnome-app-store.c		

libgnome_app_store_0_1_la_SOURCES = 	\
	$(introspection_sources)	

libgnome_app_store_0_1_la_LIBADD =	\
	$(XLIB_LIBS)		\
	$(LIBGNOME_APP_STORE_LIBS)

libgnome_app_store_0_1_la_LDFLAGS = \
	-version-info $(LT_VERSION) \
        -export-symbols-regex "^libgnome_.*" \
	-no-undefined

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libgnome-app-store-0.1.pc

libgnome_app_storedir = $(includedir)/libgnome-app-store-0.1/libgnome-app-store
libgnome_app_store_HEADERS = \
        libgnome-app-store.h      

-include $(INTROSPECTION_MAKEFILE)
INTROSPECTION_GIRS =
INTROSPECTION_SCANNER_ARGS = --add-include-path=$(srcdir) --warn-all
INTROSPECTION_COMPILER_ARGS = --includedir=$(srcdir)

if HAVE_INTROSPECTION

LibgnomeAppStore-0.1.gir: libgnome-app-store-0.1.la
LibgnomeAppStore_0_1_gir_INCLUDES = GObject-2.0 Gtk-3.0 GnomeDesktop-3.0
LibgnomeAppStore_0_1_gir_PACKAGES = gdk-pixbuf-2.0 glib-2.0 gobject-2.0 gio-2.0 gtk+-3.0 gnome-desktop-3.0 gsettings-desktop-schemas
LibgnomeAppStore_0_1_gir_CFLAGS = -DGNOME_DESKTOP_USE_UNSTABLE_API -I$(top_srcdir)
LibgnomeAppStore_0_1_gir_LIBS = libgnome-app-store-0.1.la
LibgnomeAppStore_0_1_gir_FILES = $(introspection_sources) $(libgnome_app_store_HEADERS)
LibgnomeAppStore_0_1_gir_SCANNERFLAGS = --identifier-prefix=Libgnome --symbol-prefix=libgnome_
INTROSPECTION_SCANNER_ARGS += $(patsubst %,--c-include='libgnome-app-store/%',$(libgnome_app_store_HEADERS))
INTROSPECTION_GIRS += LibgnomeAppStore-0.1.gir

girdir = $(datadir)/gir-1.0
gir_DATA = $(INTROSPECTION_GIRS)

typelibdir = $(libdir)/girepository-1.0
typelib_DATA = $(INTROSPECTION_GIRS:.gir=.typelib)

CLEANFILES += $(gir_DATA) $(typelib_DATA)
endif

EXTRA_DIST = \
	libgnome-app-store-0.1.pc.in			\
	libgnome-app-store-0.1-uninstalled.pc.in	

