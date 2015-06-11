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

#include <config.h>
#include <glib.h>
#include <gio/gio.h>
#include <gtk/gtk.h>
#include <mate-panel-applet.h>
#include <mate-panel-applet-gsettings.h>
#include <string.h>

#include "applet.h"

/* GSettings constants */
//#define COMMAND0_KEY    "command0" /* see command_keys[0] */
#define SHOW_STDOUT_KEY "stdout"

/* Max output length accepted from commands */
#define MAX_OUTPUT_LENGTH 30

#define MAX_TOGGLE 2

typedef struct {
  MatePanelApplet  *applet;
  GSettings        *settings;

  GtkWidget        *label;
  GtkWidget        *image;
  GtkWidget        *hbox;
  int              current;

  gchar            *commands[MAX_TOGGLE];
  gboolean         dirty[MAX_TOGGLE];
  GdkPixbuf        *pixbuf[MAX_TOGGLE];
} SwitchApplet;

static void switch_about_callback (GtkAction *action, SwitchApplet *switch_applet);
static void switch_settings_callback (GtkAction *action, SwitchApplet *switch_applet);
static gboolean command_execute (SwitchApplet *switch_applet, int index);
static void settings_apply (GtkDialog *dialog, gint response_id, SwitchApplet *switch_applet);

static const GtkActionEntry applet_menu_actions [] = {
  { "Preferences", GTK_STOCK_PROPERTIES, "_Preferences", NULL, NULL, G_CALLBACK (switch_settings_callback) },
  { "About", GTK_STOCK_ABOUT, "_About", NULL, NULL, G_CALLBACK (switch_about_callback) }
};

/* Menu resource (xxx-applet-menu.xml) */
static char *ui = "<menuitem name='Item 1' action='Preferences' />"
                  "<menuitem name='Item 2' action='About' />";

/* GSettings keys and image filenames */
static const char *command_keys[] = { "command0", "command1" };
static const char *command_icons[] = { APPLET_ICON_BUTTON_ON, APPLET_ICON_BUTTON_OFF };

static gboolean
switch_applet_next (GtkWidget *widget, GdkEventButton *event, SwitchApplet *switch_applet)
{
  /* Left click press */
  if (event->type == GDK_BUTTON_PRESS && event->button == 1) {
    switch_applet->current = (switch_applet->current + 1) % MAX_TOGGLE;
    if (switch_applet->pixbuf[switch_applet->current]) {
      gtk_image_set_from_pixbuf(GTK_IMAGE(switch_applet->image),
          switch_applet->pixbuf[switch_applet->current]);
    }

    command_execute (switch_applet, switch_applet->current);
    return TRUE;
  }
  return FALSE;
}

static void
switch_applet_destroy (MatePanelApplet *mate_applet, SwitchApplet *switch_applet)
{
  int i;
  g_assert (switch_applet);

  for (i = 0; i < MAX_TOGGLE; i++)
  {
    if (switch_applet->commands[i] != NULL)
    {
      g_free (switch_applet->commands[i]);
      switch_applet->commands[i] = NULL;
    }
  }

  g_object_unref (switch_applet->settings);
}

/* Show the about dialog */
static void
switch_about_callback (GtkAction *action, SwitchApplet *switch_applet)
{
  const char* authors[] = { "Matthieu Crapet <mcrapet@gmail.com>", NULL };

  gtk_show_about_dialog(NULL,
      "version", PACKAGE_VERSION,
      "copyright", "Copyright © 2015 Matthieu Crapet",
      "authors", authors,
      "comments", "Toggle between two commands.",
      "logo-icon-name", APPLET_ICON,
      //"license", "GPLv2",
      "website", PACKAGE_URL,
      NULL);
}

/* Show the preferences dialog */
static void
switch_settings_callback (GtkAction *action, SwitchApplet *switch_applet)
{
  GtkWidget *dialog;
  GtkWidget *table;
  GtkWidget *widget;
  int i;

  for (i = 0; i < MAX_TOGGLE; i++)
    switch_applet->dirty[i] = FALSE;

  dialog = gtk_dialog_new_with_buttons(APPLET_NAME " Preferences",
      NULL,
      GTK_DIALOG_MODAL,
      GTK_STOCK_APPLY,
      GTK_RESPONSE_APPLY,
      NULL);

  table = gtk_table_new (2, 3, FALSE);
  gtk_table_set_row_spacings (GTK_TABLE(table), 12);
  gtk_table_set_col_spacings (GTK_TABLE(table), 12);

  gtk_dialog_set_default_response (GTK_DIALOG(dialog), GTK_RESPONSE_APPLY);
  gtk_window_set_default_size (GTK_WINDOW (dialog), 350, 150);

  widget = gtk_label_new ("Command 1:");
  gtk_misc_set_alignment (GTK_MISC (widget), 1.0, 0.5);
  gtk_table_attach (GTK_TABLE(table), widget, 1, 2, 0, 1,
      GTK_FILL, GTK_FILL,
      0, 0);
  widget = gtk_entry_new ();
  gtk_table_attach (GTK_TABLE(table), widget, 2, 3, 0, 1,
      GTK_EXPAND | GTK_FILL | GTK_SHRINK, GTK_FILL,
      0, 0);
  g_settings_bind (switch_applet->settings, command_keys[0], widget, "text", G_SETTINGS_BIND_DEFAULT);

  widget = gtk_label_new ("Command 2:");
  gtk_misc_set_alignment (GTK_MISC (widget), 1.0, 0.5);
  gtk_table_attach (GTK_TABLE(table), widget, 1, 2, 1, 2,
      GTK_FILL, GTK_FILL,
      0, 0);
  widget = gtk_entry_new ();
  gtk_table_attach (GTK_TABLE(table), widget, 2, 3, 1, 2,
      GTK_EXPAND | GTK_FILL | GTK_SHRINK, GTK_FILL,
      0, 0);
  g_settings_bind (switch_applet->settings, command_keys[1], widget, "text", G_SETTINGS_BIND_DEFAULT);

  widget = gtk_check_button_new_with_label ("Show stdout in a label");
  gtk_misc_set_alignment (GTK_MISC (widget), 1.0, 0.5);
  gtk_table_attach (GTK_TABLE(table), widget, 1, 3, 2, 3,
      GTK_FILL, GTK_FILL,
      0, 0);
  g_settings_bind (switch_applet->settings, SHOW_STDOUT_KEY, widget, "active", G_SETTINGS_BIND_DEFAULT);

  gtk_box_pack_start (GTK_BOX (gtk_dialog_get_content_area (GTK_DIALOG (dialog))), table, TRUE, TRUE, 0);

  g_signal_connect (dialog, "response", G_CALLBACK (settings_apply), switch_applet);

  gtk_widget_show_all (GTK_WIDGET (dialog));
}

/* GSettings signal callbacks */
static void
settings_command_changed (GSettings *settings, gchar *key, SwitchApplet *switch_applet)
{
  if (strncmp (key, command_keys[0], 7) == 0) { // "command"
    gchar *command;
    int index = (int)key[7];

    if (index >= 0x30 && index <= 0x39)
      index -= 0x30;
    else if (index >= MAX_TOGGLE)
      index = MAX_TOGGLE - 1;

    command = g_settings_get_string (switch_applet->settings, command_keys[index]);

    if (switch_applet->commands[index])
      g_free (switch_applet->commands[index]);

    /* Remove trailing spaces */
    int len = strlen(command);
    gboolean update = FALSE;
    while (g_str_has_suffix (command, " ")) {
      command[len---1] = 0; // :P
      update = TRUE;
    }
    // FIXME: Crash?
    //if (update)
    //  g_settings_set_string (switch_applet->settings, command_keys[index], command);

    switch_applet->commands[index] = command;
    switch_applet->dirty[index] = TRUE;
  }
}

static void
settings_apply (GtkDialog *dialog, gint response_id, SwitchApplet *switch_applet)
{
  if (switch_applet->dirty[switch_applet->current])
    command_execute (switch_applet, switch_applet->current);
  gtk_widget_destroy(GTK_WIDGET (dialog));
}

static gboolean
command_execute (SwitchApplet *switch_applet, int index)
{
  gchar *output = NULL;
  gint ret = 0;

  gchar buf[2*MAX_OUTPUT_LENGTH];

  if (index < 0)
    return TRUE;

  if (switch_applet->commands[index] &&
      switch_applet->commands[index][0] != 0) {

    if (g_spawn_command_line_sync (switch_applet->commands[index], &output, NULL,
          &ret, NULL)) {

      if ((output != NULL) && (output[0] != 0)) {

        /* check output length */
        if (strlen(output) > MAX_OUTPUT_LENGTH) {
          GString *strip_output;
          strip_output = g_string_new_len (output, MAX_OUTPUT_LENGTH);
          g_free (output);
          output = strip_output->str;
          g_string_free (strip_output, FALSE);
        }

        /* remove trailing '\n' to avoid aligment problems */
        int len = strlen(output);
        while (g_str_has_suffix (output, "\n")) {
          output[len---1] = 0; // :P
        }

        gtk_label_set_text (GTK_LABEL(switch_applet->label), output);
      } else {
        gtk_label_set_text (GTK_LABEL(switch_applet->label), "");
      }

      g_snprintf(buf, sizeof(buf), "%s\nExit status: %u", switch_applet->commands[index], ret >> 8);
      gtk_widget_set_tooltip_text (GTK_WIDGET (switch_applet->image), buf);

    } else {
      gtk_label_set_text (GTK_LABEL(switch_applet->label), "(E)");
      gtk_widget_set_tooltip_text (GTK_WIDGET (switch_applet->image), "ERROR: Can't spawn process");
    }
  } else {
    gtk_label_set_text (GTK_LABEL(switch_applet->label), "(E)");
    gtk_widget_set_tooltip_text (GTK_WIDGET (switch_applet->image), "ERROR: Empty command");
  }

  g_free (output);
  return FALSE;
}

static gboolean
switch_applet_setup (MatePanelApplet* applet)
{
  SwitchApplet *switch_applet;
  int i;

  g_set_application_name(APPLET_NAME);
  gtk_window_set_default_icon_name(APPLET_ICON);

  mate_panel_applet_set_flags (applet, MATE_PANEL_APPLET_EXPAND_MINOR);
  mate_panel_applet_set_background_widget (applet, GTK_WIDGET (applet));

  switch_applet = g_malloc0(sizeof(SwitchApplet));
  switch_applet->applet = applet;
  switch_applet->settings = g_settings_new_with_path(APPLET_SCHEMA, SCHEMA_PATH);

  switch_applet->hbox = gtk_hbox_new (FALSE, 0);

  guint sz = mate_panel_applet_get_size(applet);
  for (i = 0; i < MAX_TOGGLE; i++) {
    switch_applet->commands[i] = g_settings_get_string (switch_applet->settings,
        command_keys[i]);

    /* Load icon and rescale it to fit panel */
    switch_applet->pixbuf[i] = gdk_pixbuf_new_from_file_at_scale(command_icons[i % 2],
        -1, sz, TRUE, NULL);
  }

  switch_applet->image = gtk_image_new_from_pixbuf(switch_applet->pixbuf[0]);
  switch_applet->label = gtk_label_new ("");

  /* We add the Gtk label into the applet */
  gtk_box_pack_start (GTK_BOX (switch_applet->hbox),
      GTK_WIDGET (switch_applet->image),
      TRUE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX (switch_applet->hbox),
      GTK_WIDGET (switch_applet->label),
      TRUE, TRUE, 0);

  gtk_container_add (GTK_CONTAINER (applet),
      GTK_WIDGET (switch_applet->hbox));

  gtk_widget_show_all (GTK_WIDGET (switch_applet->applet));

  g_signal_connect(G_OBJECT (switch_applet->applet), "destroy",
      G_CALLBACK (switch_applet_destroy),
      switch_applet);
  g_signal_connect(G_OBJECT (switch_applet->applet), "button-press-event",
      G_CALLBACK (switch_applet_next),
      switch_applet);

  /* GSettings signals */
  g_signal_connect(switch_applet->settings,
      "changed",
      G_CALLBACK (settings_command_changed),
      switch_applet);
  g_settings_bind (switch_applet->settings,
      SHOW_STDOUT_KEY,
      switch_applet->label,
      "visible",
      G_SETTINGS_BIND_DEFAULT);

  /* set up context menu */
  GtkActionGroup *action_group = gtk_action_group_new (APPLET_NAME " Actions");
  //gtk_action_group_set_translation_domain (action_group, GETTEXT_PACKAGE);
  gtk_action_group_add_actions (action_group,
      applet_menu_actions,
      G_N_ELEMENTS (applet_menu_actions),
      switch_applet);
  mate_panel_applet_setup_menu (switch_applet->applet, ui, action_group);
  g_object_unref(action_group);

  /* First command execution */
  command_execute (switch_applet, switch_applet->current);

  return TRUE;
}

/* This function, called by mate-panel, will create the applet */
static gboolean
switch_factory (MatePanelApplet* mate_applet, const char* iid, gpointer data)
{
  gboolean retval = FALSE;

  if (!g_strcmp0 (iid, APPLET_ID))
    retval = switch_applet_setup (mate_applet);

  return retval;
}

/* Main : required by mate-panel applet library */
MATE_PANEL_APPLET_OUT_PROCESS_FACTORY(APPLET_FACTORY,
    PANEL_TYPE_APPLET, APPLET_NAME, switch_factory, NULL)
