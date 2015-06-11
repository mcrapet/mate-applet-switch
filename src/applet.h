/*
 * Switch applet for MATE panel
 * Copyright (C) 2015  Matthieu Crapet <mcrapet@gmail.com>
 *
 * This file is part of Switch MATE applet.
 *
 * This applet is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This applet is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this applet.  If not, see <http://www.gnu.org/licenses/>.
 */

/* Applet constants */
#define APPLET_FACTORY "SwitchAppletFactory"
#define APPLET_ID      "SwitchApplet"
#define APPLET_NAME    "Switch applet"
#define APPLET_ICON    "terminal" // file.png (24x24)

/* Image directory is given as -D in Makefile */
#define APPLET_ICON_BUTTON_ON  G_STRINGIFY(IMAGEDIR)"/switch_on.png"
#define APPLET_ICON_BUTTON_OFF G_STRINGIFY(IMAGEDIR)"/switch_off.png"

#define APPLET_SCHEMA "org.mate.panel.applet.SwitchApplet"
#define SCHEMA_PATH   "/org/mate/panel/objects/switch/"
