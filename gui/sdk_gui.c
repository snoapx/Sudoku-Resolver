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
#include <string.h>
#include <gdk/gdkkeysyms.h>
#include "sdk_gui.h"

static struct sdk_gui_entry_s gui_grid_entries[9*9];
static GtkTextBuffer* console_buff = NULL;
static GtkWidget *scroll = NULL;
static GtkWidget *dialog = NULL;
static GtkWidget *dialog_label = NULL;
static GtkWidget *MainWindow = NULL;
static GtkWidget *popup = NULL;
static GtkWidget *progress_bar;
static struct sdk_gui_entry_s *prev_selected_entry = NULL;

static struct sdk_grid_entry_s sdk_grid[9][9];
static struct sdk_grid_entry_s sdk_result[9][9];
static GdkColor c_gray = { 10, 0xaaaa, 0xaaaa, 0xaaaa };
static GdkColor c_light_gray = { 10, 0xdddd, 0xdddd, 0xdddd };
static GdkColor c_dark_gray = { 10, 0xaaaa, 0xaaaa, 0xaaaa };
static GdkColor c_red= { 10, 0xffff, 0xaaaa, 0xaaaa};
static GdkColor c_white= { 10, 0xffff, 0xffff, 0xffff};
static GdkColor c_dark= { 10, 0x6666, 0x6666, 0x6666};

static void showDialogBox(char* txt, int error)
{
  GtkWidget *dialog, *label, *okay_button;

  /*  Create the widgets */
  dialog = gtk_dialog_new();
  label = gtk_label_new (txt);
  okay_button = gtk_button_new_with_label("OK");

  /*  Ensure that the dialog box is destroyed when the user clicks
   *  ok. */
  gtk_signal_connect_object(GTK_OBJECT (okay_button), "clicked", G_CALLBACK(gtk_widget_destroy), GTK_OBJECT(dialog));
  gtk_container_add (GTK_CONTAINER (GTK_DIALOG(dialog)->action_area), okay_button);

  /*  Add the label, and show everything we've added to the
   *  dialog. */
  gtk_container_add (GTK_CONTAINER (GTK_DIALOG(dialog)->vbox), label);
  gtk_widget_show_all (dialog);
}

/* append a line to the program's console */
static void appendConsole(char* txt)
{
  GtkTextIter ei;

  gtk_text_buffer_get_end_iter(console_buff, &ei);
  gtk_text_buffer_insert(console_buff, &ei, txt, -1);
}


/*-----------------------------------------------------------------------------
 *  Progress Bar
 *-----------------------------------------------------------------------------*/
static void closeProgressBar()
{
  gtk_widget_hide_all(popup);
}

static void updateProgressBar(int i)
{
  gtk_progress_set_value (GTK_PROGRESS (progress_bar), i);
  while (gtk_events_pending())
    gtk_main_iteration();
}

static void initProgressBar()
{
  GtkWidget *label, *content_area;
  GtkAdjustment *adj;

  popup = gtk_dialog_new_with_buttons("Waiting...", GTK_WINDOW(MainWindow), GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT, NULL);
  content_area = gtk_dialog_get_content_area(GTK_DIALOG(popup));


  adj = (GtkAdjustment *) gtk_adjustment_new (0, 0, 80, 0, 0, 0);
  progress_bar = gtk_progress_bar_new_with_adjustment (adj);

  label = gtk_label_new("The Sudoku grid is generating...");
  gtk_container_add(GTK_CONTAINER(content_area), label);
  gtk_container_add(GTK_CONTAINER(content_area), progress_bar);

  gtk_widget_show_all(popup);
  while (gtk_events_pending())
    gtk_main_iteration();
}

/*-----------------------------------------------------------------------------
 *  Menu bar entries
 *-----------------------------------------------------------------------------*/

/* show an about dialog box */
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


/* save a grid into a file */
static void showSaveFile(gpointer callback_data, guint callback_action, GtkWidget *widget)
{
  GtkWidget* file_chooser;
  gchar* selected_filename;
  char buff[256];
  GtkFileFilter *filter;
  int error;

  file_chooser = gtk_file_chooser_dialog_new("Save a grid file",
      NULL,
      GTK_FILE_CHOOSER_ACTION_SAVE,
      GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
      GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
      NULL);
  filter = gtk_file_filter_new();
  gtk_file_filter_add_pattern(filter, "*.sdk");
  gtk_file_filter_add_pattern(filter, "*.tex");
  gtk_file_chooser_set_filter(GTK_FILE_CHOOSER(file_chooser), filter);


  gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (file_chooser), "grids/");

  if (gtk_dialog_run (GTK_DIALOG (file_chooser)) == GTK_RESPONSE_ACCEPT)
  {
    selected_filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (file_chooser));

    g_print ("Selected filename: %s\n", selected_filename);
    if (strstr(selected_filename, ".sdk") != NULL)
    {
      if ((error = sdk_saveGrid((char*) selected_filename, sdk_grid, SDK_FILE_FORMAT_TEXT)) != 0)
      {
        showDialogBox("Cannot save the file!", error);
      } else {
        sprintf(buff, "Grid saved as text format at %s!\n", selected_filename);
        sdk_gui_load_grid(sdk_grid);
        appendConsole(buff);
      }
    }
    else if(strstr(selected_filename, ".tex"))
    {
      if ((error = sdk_saveGrid((char*) selected_filename, sdk_grid, SDK_FILE_FORMAT_LATEX)) != 0)
      {
        showDialogBox("Cannot save the file!", error);
      } else {
        sprintf(buff, "Grid saved as LaTex format at %s!\n", selected_filename);
        sdk_gui_load_grid(sdk_grid);
        appendConsole(buff);
      }
    }
    else if(strstr(selected_filename, ".pdf"))
    {
      if ((error = sdk_saveGrid((char*) selected_filename, sdk_grid, SDK_FILE_FORMAT_PDF)) != 0)
      {
        showDialogBox("Cannot save the file!", error);
      } else {
        sprintf(buff, "Grid saved as pdf format at %s!\n", selected_filename);
        sdk_gui_load_grid(sdk_grid);
        appendConsole(buff);
      }
    }

    g_free (selected_filename);
  }
  gtk_widget_destroy (file_chooser);
}


/* open a grid file */
static void showOpenFile()
{
  GtkWidget* file_chooser;
  gchar* selected_filename;
  char buff[256];
  GtkFileFilter *filter;
  int error;

  file_chooser = gtk_file_chooser_dialog_new("Open a grid file",
      NULL,
      GTK_FILE_CHOOSER_ACTION_OPEN,
      GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
      GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
      NULL);
  filter = gtk_file_filter_new();
  gtk_file_filter_add_pattern(filter, "*.sdk");
  gtk_file_filter_add_pattern(filter, "*.tex");
  gtk_file_chooser_set_filter(GTK_FILE_CHOOSER(file_chooser), filter);

  gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (file_chooser), "grids/");

  if (gtk_dialog_run (GTK_DIALOG (file_chooser)) == GTK_RESPONSE_ACCEPT)
  {
    selected_filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (file_chooser));
    g_print ("Selected filename: %s\n", selected_filename);

    if ((error = sdk_openGrid((char*) selected_filename, sdk_grid)) != 0)
    {
      showDialogBox("Cannot open the file!", error);
    } else {
      sprintf(buff, "Grid %s opened!\n", selected_filename);
      sdk_gui_load_grid(sdk_grid);
      appendConsole(buff);
    }

    g_free (selected_filename);
  }
  gtk_widget_destroy (file_chooser);
}

/* reset all grid's entries to char "" */
static void resetGrid()
{
  int i;
  GtkWidget* txt = NULL;

  for(i=0; i<9*9; ++i)
  {
    txt = gui_grid_entries[i].widget;
    gtk_label_set_text(GTK_LABEL(txt), "");
  }
}

static void showNewFile()
{
  resetGrid();
}

static void showConsole(gpointer callback_data,
    guint callback_action, GtkWidget *menu_item)
{
  if(GTK_CHECK_MENU_ITEM(menu_item)->active)
    gtk_widget_show(scroll);
  else
    gtk_widget_hide(scroll);
}

static void showGenerateGrid()
{
  initProgressBar();

  sdk_generateGrid(sdk_grid, updateProgressBar);
  sdk_gui_load_grid(sdk_grid);
  appendConsole("New grid generated!\n");
  closeProgressBar();
}

static void setGridEditable(int boolean)
{
  int i;

  for (i=0; i<9*9; ++i)
  {
    gtk_editable_set_editable(GTK_EDITABLE(gui_grid_entries[i].widget), FALSE);
  }
}


static GtkItemFactoryEntry menu_items[] = {
  {"/File", NULL, NULL, 0, "<Branch>"},
  {"/File/New", NULL, showNewFile, 0, "<StockItem>", GTK_STOCK_NEW},
  {"/File/Generate grid", NULL, showGenerateGrid, 0, "<StockItem>", GTK_STOCK_EXECUTE},
  {"/File/Open", NULL, showOpenFile, 0, "<StockItem>", GTK_STOCK_OPEN},
  {"/File/Save", NULL, showSaveFile, 0, "<StockItem>", GTK_STOCK_SAVE},
  {"/File/sep1", NULL, NULL, 0, "<Separator>"},
  {"/File/Quit", NULL, gtk_main_quit, 0, "<StockItem>", GTK_STOCK_QUIT},

  {"/Options", NULL, NULL, 0, "<Branch>"},
  {"/Options/Show console", NULL, showConsole, 0, "<CheckItem>"},
  {"/Options/Edit Grid", NULL, setGridEditable, 0, "<CheckItem>"},

  {"/Help", NULL, NULL, 0, "<Branch>"},
  {"/Help/Manual", NULL, NULL, 0, "<StockItem>", GTK_STOCK_HELP},
  {"/Help/About", NULL, showAbout, 0, "<StockItem>", GTK_STOCK_ABOUT}
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


/**
 *  sdk_gui_load_grid()
 * @brief Load a Sudoku grid and init all sdk's entries
 */
  void
sdk_gui_load_grid(struct sdk_grid_entry_s grid[][9])
{
  int i, j;
  int count = 0;
  char num[10];

  for (i=0; i<9; ++i)
  {
    for (j=0; j<9; ++j)
    {
      if (grid[i][j].value == 0)
        gtk_label_set_text(GTK_LABEL(gui_grid_entries[count].widget), "");
      else
      {
        sprintf(num, "%d", grid[i][j].value);
        gtk_label_set_text(GTK_LABEL(gui_grid_entries[count].widget), num);
      }
      ++count;
    }
  }
}

void resolveGrid()
{
  char buff[256];
  int nb_computations;
  int nb_solutions;

  sdk_resolveGrid(sdk_grid, sdk_result, &nb_computations, &nb_solutions, 0);
  if (nb_solutions != 0)
  {
    sprintf(buff, "Solution found in %d computations!\n", nb_computations);
    showGridResults(buff);
    sdk_gui_load_grid(sdk_result);
  }
  else
  {
    sprintf(buff, "No solution found!\n");
    showGridResults(buff);
  }

  appendConsole(buff);
}

void button_press(GtkWidget *widget, GdkEventKey *event, gpointer data)
{
  gtk_widget_modify_bg (widget, GTK_STATE_NORMAL, &c_red);

  if ( (prev_selected_entry != NULL) && (prev_selected_entry->event_box != widget) )
    gtk_widget_modify_bg (prev_selected_entry->event_box, GTK_STATE_NORMAL, prev_selected_entry->color);

  prev_selected_entry = (struct sdk_gui_entry_s*) data;
  gtk_widget_grab_focus(widget);
}

void key_press(GtkWidget *widget, GdkEventKey *event, gpointer data)
{
  struct sdk_gui_entry_s* entry = data;

  sdk_grid[entry->i][entry->j].isBase = 1;
  switch(event->keyval)
  {
    case GDK_0 :
    case GDK_Delete :
      gtk_label_set_text(GTK_LABEL(entry->widget), "");
      sdk_grid[entry->i][entry->j].value = 0;
      sdk_grid[entry->i][entry->j].isBase = 0;
      break;
    case GDK_1 :
      gtk_label_set_text(GTK_LABEL(entry->widget), "1");
      sdk_grid[entry->i][entry->j].value = 1;
      break;
    case GDK_2 :
      gtk_label_set_text(GTK_LABEL(entry->widget), "2");
      sdk_grid[entry->i][entry->j].value = 2;
      break;
    case GDK_3 :
      gtk_label_set_text(GTK_LABEL(entry->widget), "3");
      sdk_grid[entry->i][entry->j].value = 3;
      break;
    case GDK_4 :
      gtk_label_set_text(GTK_LABEL(entry->widget), "4");
      sdk_grid[entry->i][entry->j].value = 4;
      break;
    case GDK_5 :
      gtk_label_set_text(GTK_LABEL(entry->widget), "5");
      sdk_grid[entry->i][entry->j].value = 5;
      break;
    case GDK_6 :
      gtk_label_set_text(GTK_LABEL(entry->widget), "6");
      sdk_grid[entry->i][entry->j].value = 6;
      break;
    case GDK_7 :
      gtk_label_set_text(GTK_LABEL(entry->widget), "7");
      sdk_grid[entry->i][entry->j].value = 7;
      break;
    case GDK_8 :
      gtk_label_set_text(GTK_LABEL(entry->widget), "8");
      sdk_grid[entry->i][entry->j].value = 8;
      break;
    case GDK_9 :
      gtk_label_set_text(GTK_LABEL(entry->widget), "9");
      sdk_grid[entry->i][entry->j].value = 9;
      break;
  }
}

void focus_in_event(GtkWidget *widget, GdkEventKey *event, gpointer data)
{
  GTK_WIDGET_SET_FLAGS (widget, GTK_HAS_FOCUS);
}

void focus_out_event(GtkWidget *widget, GdkEventKey *event, gpointer data)
{
  GTK_WIDGET_UNSET_FLAGS (widget, GTK_HAS_FOCUS);
}

/**
 *  sdk_gui_init()
 * @brief Initialize the Sudoku Resolver gui
 */
int sdk_gui_init(int argc, char **argv)
{
  /*  Variables */
  GtkWidget* table1 = NULL;
  GtkWidget* table2 = NULL;

  GtkWidget* txt = NULL;
  GtkWidget* menubar = NULL;
  GtkWidget* vbox = NULL;
  GtkWidget* resolve_btn = NULL;
  GtkWidget* console = NULL;
  PangoAttrList *attr_lst;
  PangoAttribute *attr;
  GtkWidget *event_box = NULL;
  GtkWidget *frame = NULL;

  char welcome_msg[256];
  int i, j;
  int count = 0;

  /* initialiszation of the GTK+ window*/
  gtk_init(&argc, &argv);

  /* window creation  */
  MainWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(MainWindow), "Sudoku Solver");
  gtk_window_set_position(GTK_WINDOW(MainWindow), GTK_WIN_POS_CENTER);
  gtk_window_set_resizable(GTK_WINDOW(MainWindow), FALSE);

  attr = pango_attr_size_new(20 * PANGO_SCALE);
  attr_lst = pango_attr_list_new();
  pango_attr_list_insert(attr_lst, attr);

  table1 = gtk_table_new(3,3,TRUE);

  gtk_container_set_border_width(GTK_CONTAINER(table1), 10);

  for (i=0; i<9; ++i)
  {
    frame = gtk_frame_new(NULL);
    gtk_frame_set_shadow_type(GTK_FRAME(frame), GTK_SHADOW_OUT);
    gtk_widget_modify_bg (frame, GTK_STATE_NORMAL, &c_dark);
    table2 = gtk_table_new(3,3,TRUE);
    gtk_container_add(GTK_CONTAINER(frame), table2);
    gtk_table_attach(GTK_TABLE(table1), frame, i/3, (i/3)+1, (i%3), (i%3)+1, GTK_FILL | GTK_EXPAND, GTK_FILL | GTK_EXPAND, 0, 0);

    for (j=0; j<9; ++j)
    {
      event_box = gtk_event_box_new();
      txt = gtk_label_new("");

      gtk_label_set_attributes(GTK_LABEL(txt), attr_lst);
      gtk_label_set_justify(GTK_LABEL(txt), GTK_JUSTIFY_CENTER);
      gtk_widget_set_size_request(event_box, 50, 50);

      gui_grid_entries[count].widget = txt;
      gui_grid_entries[count].event_box = event_box;
      gui_grid_entries[count].i = (i%3)*3 + ((j%3)+1);
      gui_grid_entries[count].j = (i/3)*3 + ((j/3)+1);

      GTK_WIDGET_SET_FLAGS (event_box,GTK_CAN_FOCUS);
      g_signal_connect(G_OBJECT(event_box), "button_press_event", G_CALLBACK(button_press), (gpointer) &gui_grid_entries[count]);
      g_signal_connect(G_OBJECT(event_box), "key_press_event", G_CALLBACK(key_press), (gpointer) &gui_grid_entries[count]);
      g_signal_connect(G_OBJECT(event_box), "focus_in_event", G_CALLBACK(focus_in_event), NULL);
      g_signal_connect(G_OBJECT(event_box), "focus_out_event", G_CALLBACK(focus_out_event), NULL);

      gtk_widget_modify_bg (event_box, GTK_STATE_NORMAL, &c_white);
      gui_grid_entries[count].color = &c_white;
      if (i%2)
      {
        if (j%2) {
          gtk_widget_modify_bg (event_box, GTK_STATE_NORMAL, &c_light_gray);
          gui_grid_entries[count].color = &c_light_gray;
        }
      }
      else
      {
        if (! (j%2)) {
          gtk_widget_modify_bg (event_box, GTK_STATE_NORMAL, &c_light_gray);
          gui_grid_entries[count].color = &c_light_gray;
        }
      }

      gtk_container_add(GTK_CONTAINER(event_box), txt);
      gtk_table_attach(GTK_TABLE(table2), event_box, j/3, (j/3)+1, j%3, (j%3)+1, GTK_FILL | GTK_EXPAND, GTK_FILL | GTK_EXPAND, 0, 0);

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

  sprintf(welcome_msg, "Application launched !\nWelcome to the Sudoku Solver version %s\n", SDK_VERSION);
  appendConsole(welcome_msg);
  gtk_container_add (GTK_CONTAINER (scroll), GTK_WIDGET (console));

  gtk_box_pack_start (GTK_BOX (vbox), menubar, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX (vbox), table1, FALSE, FALSE, 2);
  gtk_box_pack_start (GTK_BOX (vbox), resolve_btn, FALSE, FALSE, 2);
  gtk_box_pack_start (GTK_BOX (vbox), scroll, TRUE, TRUE, 0);

  /* Signals */
  g_signal_connect(G_OBJECT(MainWindow), "delete-event", G_CALLBACK(gtk_main_quit), NULL);
  g_signal_connect(G_OBJECT(resolve_btn), "clicked", G_CALLBACK(resolveGrid), NULL);

  gtk_widget_show_all(MainWindow);
  gtk_widget_hide(scroll);

  createGridResults();

  gtk_main();

  return EXIT_SUCCESS;
}
