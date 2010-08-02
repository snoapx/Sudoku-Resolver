/*
 * =====================================================================================
 *
 *       Filename:  sdk_gui.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  07/31/2010 11:57:29 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (),
 *        Company:
 *
 * =====================================================================================
 */

#include <stdlib.h>
#include <stdio.h>
#include <gtk/gtk.h>
#include <sdk_resolver.h>
#include "sdk_gui.h"

static GtkWidget* gui_grid_entries[9*9];


/*-----------------------------------------------------------------------------
 *  Menu bar entries
 *-----------------------------------------------------------------------------*/

/* *
 * Open an About dialog box
 * */
static void showAbout()
{
  GtkWidget* about = NULL;

  about = gtk_about_dialog_new();
  gtk_show_about_dialog (NULL,
      "program-name", "Sudoku Resolver",
      "logo", NULL,
      "title", "About Sudoku Resolver",
      "comments", "Sudoku Resolver for 9*9 grids",
      NULL);
}

static void showOpenFile_handler(GtkWidget* widget, gpointer user_data)
{
  GtkWidget* file_selector = GTK_WIDGET(user_data);
  const gchar* selected_filename;

  selected_filename = gtk_file_selection_get_filename (GTK_FILE_SELECTION (file_selector));
  g_print ("Selected filename: %s\n", selected_filename);

  sdk_openGrid((char*) selected_filename);
  sdk_gui_load_grid();

}

/* *
 * Open a OpenFile dialog box
 * */
static void showOpenFile()
{
  GtkWidget* file_selector;

  file_selector = gtk_file_selection_new("Please select a file for editing.");

  /* signals connections */
  g_signal_connect(GTK_FILE_SELECTION (file_selector)->ok_button,
      "clicked",
      G_CALLBACK(showOpenFile_handler),
      file_selector);
  g_signal_connect_swapped (GTK_FILE_SELECTION (file_selector)->ok_button,
      "clicked",
      G_CALLBACK (gtk_widget_destroy),
      file_selector);
  g_signal_connect_swapped (GTK_FILE_SELECTION (file_selector)->cancel_button,
      "clicked",
      G_CALLBACK (gtk_widget_destroy),
      file_selector);

  gtk_widget_show(file_selector);

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

static GtkItemFactoryEntry menu_items[] = {
  {"/File", NULL, NULL, 0, "<Branch>"},
  {"/File/New", NULL, showNewFile, 0, NULL},
  {"/File/Open", NULL, showOpenFile, 0, NULL},
  {"/File/Save", NULL, NULL, 0, NULL},
  {"/File/sep1", NULL, NULL, 0, "<Separator>"},
  {"/File/Quit", NULL, gtk_main_quit, 0, NULL},

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
  sdk_resolveGrid();
  g_print("Sudoku resolved!\n");
  sdk_gui_load_grid();
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
  GtkTextBuffer* buff = NULL;

  int i, j;
  int count = 0;

  /*  Initialisation de GTK+ */
  gtk_init(&argc, &argv);

  /*  Création de la fenêtre */
  MainWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(MainWindow), "Sudoku solver by Sylvain Didelot (2010)");
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

  console = gtk_text_view_new();
  buff = gtk_text_view_get_buffer(GTK_TEXT_VIEW(console));
  gtk_text_buffer_set_text(buff, "Welcome to the Sudoku Solver", -1);

  gtk_box_pack_start (GTK_BOX (vbox), menubar, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX (vbox), table, FALSE, FALSE, 2);
  gtk_box_pack_start (GTK_BOX (vbox), resolve_btn, FALSE, FALSE, 2);
  gtk_box_pack_start (GTK_BOX (vbox), console, FALSE, FALSE, 2);

  /* Signals */
  g_signal_connect(G_OBJECT(MainWindow), "delete-event", G_CALLBACK(gtk_main_quit), NULL);
  g_signal_connect(G_OBJECT(resolve_btn), "clicked", G_CALLBACK(resolveGrid), NULL);


  /*  Affichage et boucle évènementielle */
  gtk_widget_show_all(MainWindow);
  gtk_main();

  /*  On quitte.. */
  return EXIT_SUCCESS;
}
