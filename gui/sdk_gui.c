/*
 * =====================================================================================
 *
 *       Filename:  sdk_resolver.h
 *
 *    Description:  Sudoku resolver - A easy-to-use tool for finding the
 *                  solution of a Sudoku grid.
 *
 *        Created:  07/22/2010 07:54:00 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Sylvain Didelot, didelot.sylvain@gmail.com
 *
 * =====================================================================================
 *
 * Sudoku Resolver - An easy-to-use tool for finding the solution of a
 * Sudoku grid.
 * Copyright (C) 2010  Sylvain Didelot - didelot.sylvain@gmail.com
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>."
 */

#include <stdlib.h>
#include <stdio.h>
#include <gtk/gtk.h>
#include <sdk_resolver.h>
#include "sdk_gui.h"

static GtkWidget* gui_grid_entries[9*9];
static GtkTextBuffer* console_buff = NULL;
static GtkWidget* scroll = NULL;
static GtkWidget* dialog = NULL;
static GtkWidget* dialog_label = NULL;


/*-----------------------------------------------------------------------------
 *  Menu bar entries
 *-----------------------------------------------------------------------------*/

/* *
 * Open an About dialog box
 * */
static void showAbout()
{
  GtkWidget* about = NULL;
  char comments[256];
  gchar* authors[] = {"Sylvain Didelot <didelot.sylvain@gmail.com", NULL};

  sprintf(comments, "A easy-to-use tool for finding the solution of a Sudoku grid\n. Program compiled the %s at %s", SDK_COMPILE_DATE, SDK_COMPILE_TIME);

  about = gtk_about_dialog_new();
  gtk_show_about_dialog (NULL,
      "program-name", "Sudoku Resolver",
      "authors", authors,
      "logo", NULL,
      "license", SDK_LICENSE,
      "version", SDK_VERSION,
      "title", "About Sudoku Resolver",
      "comments", comments,
      NULL);
}

static void appendConsole(char* txt)
{
  GtkTextIter ei;

  gtk_text_buffer_get_end_iter(console_buff, &ei);
  gtk_text_buffer_insert(console_buff, &ei, txt, -1);
}

/* *
 * Open a OpenFile dialog box
 * */
static void showOpenFile()
{
  GtkWidget* file_chooser;
  gchar* selected_filename;
  char buff[256];

  file_chooser = gtk_file_chooser_dialog_new("Open a grid file",
      NULL,
      GTK_FILE_CHOOSER_ACTION_OPEN,
      GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
      GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
      NULL);

  gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (file_chooser), "grids/");


  if (gtk_dialog_run (GTK_DIALOG (file_chooser)) == GTK_RESPONSE_ACCEPT)
  {
    selected_filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (file_chooser));
    g_print ("Selected filename: %s\n", selected_filename);

    sdk_openGrid((char*) selected_filename);

    sprintf(buff, "Grid %s opened!\n", selected_filename);
    sdk_gui_load_grid();
    appendConsole(buff);
    g_free (selected_filename);
  }
  gtk_widget_destroy (file_chooser);
}

static void resetGrid()
{
  int i;
  GtkWidget* txt = NULL;

  for(i=0; i<9*9; ++i)
  {
    txt = gui_grid_entries[i];
    gtk_entry_set_text(GTK_ENTRY(txt), "");
  }
}

static void showNewFile()
{
  resetGrid();
}

static void showConsole()
{
  if (gtk_widget_get_visible(scroll))
    gtk_widget_hide(scroll);
  else
    gtk_widget_show(scroll);
}

static void showGenerateGrid()
{
  //TODO
}

static void setGridEditable(int boolean)
{
  int i;

  for (i=0; i<9*9; ++i)
  {
    gtk_editable_set_editable(GTK_EDITABLE(gui_grid_entries[i]), FALSE);
  }
}


static GtkItemFactoryEntry menu_items[] = {
  {"/File", NULL, NULL, 0, "<Branch>"},
  {"/File/New", NULL, showNewFile, 0, NULL},
  {"/File/Generate grid", NULL, showGenerateGrid, 0, NULL},
  {"/File/Open", NULL, showOpenFile, 0, NULL},
  {"/File/Save", NULL, NULL, 0, NULL},
  {"/File/sep1", NULL, NULL, 0, "<Separator>"},
  {"/File/Quit", NULL, gtk_main_quit, 0, NULL},

  {"/Option", NULL, NULL, 0, "<Branch>"},
  {"/Option/Show console", NULL, showConsole, 0, "<CheckItem>"},
  {"/Option/Edit Grid", NULL, setGridEditable, 0, "<CheckItem>"},

  {"/Help", NULL, NULL, 0, "<Branch>"},
  {"/Help/Manual", NULL, NULL, 0, NULL},
  {"/Help/About", NULL, showAbout, 0, NULL},
};


static void createMenu(GtkWidget* window, GtkWidget** menubar)
{
  GtkItemFactory* item_factory;
  GtkAccelGroup *accel_group;
  gint nmenu_items = sizeof(menu_items) / sizeof (menu_items[0]);

  accel_group = gtk_accel_group_new();

  item_factory = gtk_item_factory_new(GTK_TYPE_MENU_BAR, "<main>", accel_group);
  gtk_item_factory_create_items(item_factory, nmenu_items, menu_items, NULL);

  gtk_window_add_accel_group(GTK_WINDOW(window), accel_group);

  if (menubar)
    *menubar = gtk_item_factory_get_widget(item_factory, "<main>");
}

static void showGridResults(char* txt)
{
  gtk_label_set_text(GTK_LABEL(dialog_label), txt);
  gtk_widget_show_all (dialog);
}

static void createGridResults()
{
  GtkWidget* ok_btn = NULL;

  dialog = gtk_dialog_new();
  gtk_window_set_title(GTK_WINDOW(dialog), "Sudoku grid results");
  gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);

  dialog_label = gtk_label_new(NULL);
  ok_btn = gtk_button_new_with_label("Okay");
  gtk_signal_connect_object(GTK_OBJECT (ok_btn), "clicked", G_CALLBACK(gtk_widget_hide), GTK_OBJECT(dialog));
  gtk_container_add (GTK_CONTAINER (GTK_DIALOG(dialog)->action_area), ok_btn);
  gtk_container_add (GTK_CONTAINER (GTK_DIALOG(dialog)->vbox), dialog_label);

  gtk_widget_hide_all (dialog);
}


/*
 * ===  FUNCTION  ======================================================================
 *         Name:  sdk_gui_load_grid
 *  Description:  Load a grid into the gui
 * =====================================================================================
 */
  void
sdk_gui_load_grid()
{
  int i, j;
  int count = 0;
  char num[10];

  for (i=0; i<9; ++i)
  {
    for (j=0; j<9; ++j)
    {
      if (sdk_grid[i][j].value == 0)
        gtk_entry_set_text(GTK_ENTRY(gui_grid_entries[count]), "");
      else
      {
        sprintf(num, "%d", sdk_grid[i][j].value);
        gtk_entry_set_text(GTK_ENTRY(gui_grid_entries[count]), num);
      }
      ++count;
    }
  }
}

void resolveGrid()
{
  char buff[256];
  int nb_computations;

  nb_computations = sdk_resolveGrid();
  if (nb_computations != -1)
  {
    sprintf(buff, "Solution found in %d computations!\n", nb_computations);
    showGridResults(buff);
    sdk_gui_load_grid();
  }
  else
  {
    sprintf(buff, "No solution found!\n");
    showGridResults(buff);
  }

  appendConsole(buff);
}


int sdk_gui_init(int argc, char **argv)
{
  /*  Variables */
  GtkWidget* MainWindow = NULL;
  GtkWidget* table = NULL;
  GtkWidget* txt = NULL;
  GtkWidget* menubar = NULL;
  GtkWidget* vbox = NULL;
  GtkWidget* resolve_btn = NULL;
  GtkWidget* console = NULL;

  char welcome_msg[256];

  int i, j;
  int count = 0;

  /*  Initialisation de GTK+ */
  gtk_init(&argc, &argv);

  /*  Création de la fenêtre */
  MainWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(MainWindow), "Sudoku Solver");
  gtk_window_set_position(GTK_WINDOW(MainWindow), GTK_WIN_POS_CENTER);
  gtk_window_set_resizable(GTK_WINDOW(MainWindow), FALSE);

  table = gtk_table_new(9,9,TRUE);
  for(i=0; i<9; ++i)
  {
    for(j=0; j<9; ++j)
    {
      txt = gtk_entry_new();
      gtk_entry_set_text(GTK_ENTRY(txt), "");
      gtk_entry_set_width_chars(GTK_ENTRY(txt), 2);
      gtk_entry_set_alignment (GTK_ENTRY(txt), 0.5);
      gtk_entry_set_max_length (GTK_ENTRY(txt), 1);
      gtk_table_attach_defaults(GTK_TABLE(table), txt, j, j+1, i, i+1);
      gui_grid_entries[count] = txt;
      ++count;
    }
  }

  vbox = gtk_vbox_new(FALSE, 0);
  gtk_container_add(GTK_CONTAINER(MainWindow), vbox);
  gtk_widget_show(vbox);

  createMenu(MainWindow, &menubar);
  gtk_widget_show(menubar);

  resolve_btn = gtk_button_new_with_label("Resolve now!");
  gtk_widget_show(resolve_btn);

  scroll = gtk_scrolled_window_new(NULL, NULL);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scroll),
      GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  console = gtk_text_view_new();
  console_buff = gtk_text_view_get_buffer(GTK_TEXT_VIEW(console));

  sprintf(welcome_msg, "Application launched !\nWelcome to the Sudoku Solver version %d.%d\n", SDK_VERSION_MAJOR, SDK_VERSION_MINOR);
  appendConsole(welcome_msg);
  gtk_container_add (GTK_CONTAINER (scroll), GTK_WIDGET (console));

  gtk_box_pack_start (GTK_BOX (vbox), menubar, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX (vbox), table, FALSE, FALSE, 2);
  gtk_box_pack_start (GTK_BOX (vbox), resolve_btn, FALSE, FALSE, 2);
  gtk_box_pack_start (GTK_BOX (vbox), scroll, TRUE, TRUE, 0);

  /* Signals */
  g_signal_connect(G_OBJECT(MainWindow), "delete-event", G_CALLBACK(gtk_main_quit), NULL);
  g_signal_connect(G_OBJECT(resolve_btn), "clicked", G_CALLBACK(resolveGrid), NULL);


  /*  Affichage et boucle évènementielle */
  gtk_widget_show_all(MainWindow);
  gtk_widget_hide(scroll);

  createGridResults();

  gtk_main();

  /*  On quitte.. */
  return EXIT_SUCCESS;
}
