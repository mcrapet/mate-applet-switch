libexec_PROGRAMS = switch_applet 
switch_applet_SOURCES = switch.c applet.h
switch_applet_LDADD = $(MATE_APPLETS4_LIBS)  $(GLIB_LIBS) $(GIO_LIBS) $(GTK_LIBS)
switch_applet_CFLAGS = $(MATE_APPLETS4_CFLAGS) $(GLIB_CFLAGS) $(GIO_CFLAGS) $(GTK_CFLAGS) \
                       -DIMAGEDIR=$(datadir)/pixmaps/mate-switch-applet

appletdir = $(datadir)/mate-panel/applets
applet_DATA = org.mate.applets.SwitchApplet.mate-panel-applet

servicedir = $(datadir)/dbus-1/services
service_DATA = org.mate.panel.applet.SwitchApplet.service

gsettingsdir = $(datadir)/glib-2.0/schemas
gsettings_DATA = org.mate.panel.applet.SwitchApplet.gschema.xml

install-data-hook:
	glib-compile-schemas $(DESTDIR)$(datadir)/glib-2.0/schemas 2>/dev/null

CLEANFILES = $(applet_DATA) $(service_DATA) $(gsettings_DATA)
