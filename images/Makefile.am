base_iconsdir = $(datadir)/icons/mate
iconsdir = $(base_iconsdir)/48x48/apps
#icons_DATA = switch-applet.png

pixmapsdir  = $(datadir)/pixmaps/mate-switch-applet
pixmaps_DATA =	\
  switch_on.png \
  switch_off.png

gtk_update_icon_cache = gtk-update-icon-cache -f -t $(base_iconsdir)
install-data-hook: update-icon-cache
uninstall-hook: update-icon-cache
update-icon-cache:
	@-if test -z "$(DESTDIR)"; then \
		echo "Updating Gtk icon cache."; \
		$(gtk_update_icon_cache); \
	else \
		echo "*** Icon cache not updated. After install, run this:"; \
		echo "***   $(gtk_update_icon_cache)"; \
	fi

EXTRA_DIST = \
	$(pixmaps_DATA)
