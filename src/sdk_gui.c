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

#ifdef GTK_ENABLE

#include <stdlib.h>
#include <stdio.h>
#include <gtk/gtk.h>
#include <sdk_resolver.h>
#include <string.h>
#include <gdk/gdkkeysyms.h>

#include "sdk_gui.h"
#include "sdk_colors.h"
#include "sdk_error.h"

/* console buffer at the bottom of the sdk grid */
static GtkTextBuffer* console_buff = NULL;
/* scroll bar of the console buffer */
static GtkWidget *scroll = NULL;
/* sdk resolver main window */
static GtkWidget *MainWindow = NULL;
/* waiting popup window */
static GtkWidget *popup = NULL;
/* waiting progress bar */
static GtkWidget *progress_bar;
/* previous entry selected in the sdk */
static struct sdk_grid_entry_s *prev_selected_entry = NULL;

/* grid shown by the application */
static struct sdk_grid_entry_s sdk_grid[9][9];
/* variable where the result of a grid is stored */
static struct sdk_grid_entry_s sdk_result[9][9];

/* show a dialog box */
static void showDialogBox(char* txt, int error, GtkMessageType type)
{
  GtkWidget *dialog;
  char buff[512];

  /*  Create the widgets */
  if (type == GTK_MESSAGE_ERROR)
    sprintf(buff, "Error code : %s\nError descritpion : %s\n\n%s", sdk_error[error].err_code, sdk_error[error].err_desc, txt);
  else if (type == GTK_MESSAGE_INFO)
    sprintf(buff, "Information : %s", txt);

  dialog = gtk_message_dialog_new(GTK_WINDOW(MainWindow),
      GTK_DIALOG_DESTROY_WITH_PARENT,
      type,
      GTK_BUTTONS_OK,
      buff);
//  gtk_window_set_title(GTK_WINDOW(dialog));
  gtk_dialog_run(GTK_DIALOG(dialog));
  gtk_widget_destroy(dialog);
}

/* append a line to the program's console */
static void appendConsole(char* txt)
{
  GtkTextIter ei;

  gtk_text_buffer_get_end_iter(console_buff, &ei);
  gtk_text_buffer_insert(console_buff, &ei, txt, -1);
}

  void
sdk_gui_load_grid(struct sdk_grid_entry_s grid[][9])
{
  int i, j;
  char num[10];

  for (i=0; i<9; ++i)
  {
    for (j=0; j<9; ++j)
    {
      sdk_grid[i][j].value = grid[i][j].value;
      if (grid[i][j].value == 0) {
        gtk_label_set_text(GTK_LABEL(grid[i][j].widget), "");
      } else {
        sprintf(num, "%d", grid[i][j].value);
        gtk_label_set_text(GTK_LABEL(grid[i][j].widget), num);
      }
    }
  }
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
        showDialogBox("Cannot open the selected file", error, GTK_MESSAGE_ERROR );
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
        showDialogBox("Cannot open the selected file", error, GTK_MESSAGE_ERROR);
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
        showDialogBox("Cannot save the file!", error, GTK_MESSAGE_ERROR);
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
      showDialogBox("Cannot open the file!", error, GTK_MESSAGE_ERROR);
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
  int i,j;
  GtkWidget* txt = NULL;


  for(i=0; i<9; ++i)
  {
    for(j=0; j<9; ++j)
    {
      sdk_resetGrid(sdk_grid, i, j);
      txt = sdk_grid[i][j].widget;
      gtk_label_set_text(GTK_LABEL(txt), "");
    }
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

static void showGenerateGrid(gpointer callback_data, guint callback_action, GtkWidget *widget)

{
  initProgressBar();

  switch(callback_action) {
    case 0:
      sdk_generateGrid(sdk_grid, updateProgressBar, EASY_GRID);
      appendConsole("New EASY grid generated !\n");
      break;
    case 1:
      sdk_generateGrid(sdk_grid, updateProgressBar, MEDIUM_GRID);
      appendConsole("New MEDIUM grid generated !\n");
      break;
    case 2:
      sdk_generateGrid(sdk_grid, updateProgressBar, DIFFICULT_GRID);
      appendConsole("New DIFFICULT grid generated !\n");
      break;
  }

  sdk_gui_load_grid(sdk_grid);
  closeProgressBar();
}

static void setGridEditable(int boolean)
{
  int i, j;

  for (i=0; i<9; ++i)
  {
    for (j=0; j<9; ++j)
    {
      gtk_editable_set_editable(GTK_EDITABLE(sdk_grid[i][j].widget), FALSE);
    }
  }
}

static void resolveGrid()
{
  char buff[256];
  int nb_computations = 0;
  int nb_solutions = 0;

  sdk_resolveGrid(sdk_grid, sdk_result, &nb_solutions, &nb_computations, 0);

    if (nb_solutions != 0)
  {
    sprintf(buff, "Solution found in %d computations!\n", nb_computations);
    sdk_gui_load_grid(sdk_result);
    showDialogBox(buff, 0, GTK_MESSAGE_INFO);
  }
  else
  {
    sprintf(buff, "No solution found!\n");
    showDialogBox(buff, 0, GTK_MESSAGE_INFO);
  }

  appendConsole(buff);
}

static void showManual(gpointer callback_data, guint callback_action, GtkWidget *widget)
{
  showDialogBox("Function not implemented", 0, GTK_MESSAGE_INFO);
}

static GtkItemFactoryEntry menu_items[] = {
  {"/File", NULL, NULL, 0, "<Branch>"},
  {"/File/New", NULL, showNewFile, 0, "<StockItem>", GTK_STOCK_NEW},

  {"/File/Generate grid", NULL, NULL, 0, "<Branch>", GTK_STOCK_EXECUTE},
  {"/File/Generate grid/Easy", NULL, showGenerateGrid, 0, "<StockItem>", GTK_STOCK_EXECUTE},
  {"/File/Generate grid/Medium", NULL, showGenerateGrid, 1, "<StockItem>", GTK_STOCK_EXECUTE},
  {"/File/Generate grid/Difficult", NULL, showGenerateGrid, 2, "<StockItem>", GTK_STOCK_EXECUTE},

  {"/File/Resolve grid", NULL, resolveGrid, 0, "<StockItem>", GTK_STOCK_FIND},
  {"/File/Open", NULL, showOpenFile, 0, "<StockItem>", GTK_STOCK_OPEN},
  {"/File/Save", NULL, showSaveFile, 0, "<StockItem>", GTK_STOCK_SAVE},
  {"/File/sep1", NULL, NULL, 0, "<Separator>"},
  {"/File/Quit", NULL, gtk_main_quit, 0, "<StockItem>", GTK_STOCK_QUIT},

  {"/Options", NULL, NULL, 0, "<Branch>"},
  {"/Options/Show console", NULL, showConsole, 0, "<CheckItem>"},
//²  {"/Options/Edit Grid", NULL, setGridEditable, 0, "<CheckItem>"},

  {"/Help", NULL, NULL, 0, "<Branch>"},
  {"/Help/Manual", NULL, showManual, 0, "<StockItem>", GTK_STOCK_HELP},
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

void button_press(GtkWidget *widget, GdkEventKey *event, gpointer data)
{
//  fprintf(stderr, "HERE %p %p %p",widget, event, data);
  gtk_widget_modify_bg (widget, GTK_STATE_NORMAL, &c_red);

  if ( (prev_selected_entry != NULL) && (prev_selected_entry->event_box != widget) )
    gtk_widget_modify_bg (prev_selected_entry->event_box, GTK_STATE_NORMAL, prev_selected_entry->color);

  prev_selected_entry = (struct sdk_grid_entry_s*) data;
  gtk_widget_grab_focus(widget);
}


void checkConstraints()
{
  int i, j;

  for (i = 0; i < 9; ++i)
  {
    for (j = 0; j < 9; ++j)
    {
      if (sdk_grid[i][j].value != 0) {
        if (sdk_checkConstrains(sdk_grid, &sdk_grid[i][j], sdk_grid[i][j].value) == 0) {
          gtk_widget_modify_fg(sdk_grid[i][j].widget, GTK_STATE_NORMAL, &c_dark_red);
        } else {
          gtk_widget_modify_fg(sdk_grid[i][j].widget, GTK_STATE_NORMAL, &c_black);
        }
      }
    }
  }
}

#define UPDATE_ENTRY(x) \
      gtk_label_set_text(GTK_LABEL(entry->widget), STR(x)); \
      sdk_grid[entry->i][entry->j].value = x; \
      checkConstraints();

#define UPDATE_SIGNALS \
      g_signal_emit_by_name(G_OBJECT(sdk_grid[i][j].event_box), "focus_out_event"); \
      g_signal_emit_by_name(G_OBJECT(sdk_grid[_i][_j].event_box), "focus_in_event"); \
      g_signal_emit_by_name(G_OBJECT(sdk_grid[_i][_j].event_box), "button_press_event");

void key_press(GtkWidget *widget, GdkEventKey *event, gpointer data)
{
  struct sdk_grid_entry_s* entry = data;
  int i, j, _i, _j;

  sdk_grid[entry->i][entry->j].isBase = 1;
  _i = i = entry->i;
  _j = j = entry->j;

  switch(event->keyval)
  {
    case GDK_0 :
    case GDK_Delete :
      gtk_label_set_text(GTK_LABEL(entry->widget), "");
      sdk_grid[entry->i][entry->j].value = 0;
      sdk_grid[entry->i][entry->j].isBase = 0;
      break;
    case GDK_1 :
      UPDATE_ENTRY(1);
      break;
    case GDK_2 :
      UPDATE_ENTRY(2);
      break;
    case GDK_3 :
      UPDATE_ENTRY(3);
      break;
    case GDK_4 :
      UPDATE_ENTRY(4);
      break;
    case GDK_5 :
      UPDATE_ENTRY(5);
      break;
    case GDK_6 :
      UPDATE_ENTRY(6);
      break;
    case GDK_7 :
      UPDATE_ENTRY(7);
      break;
    case GDK_8 :
      UPDATE_ENTRY(8);
      break;
    case GDK_9 :
      UPDATE_ENTRY(9);
      break;
    case GDK_Up :
      if (j == 0) _j = 8; else --_j;
      UPDATE_SIGNALS;
      break;
    case GDK_Down :
      if (j == 8) _j = 0; else ++_j;
      UPDATE_SIGNALS;
      break;
    case GDK_Left :
      if (i == 0) _i = 8; else --_i;
      UPDATE_SIGNALS;
      break;
    case GDK_Right :
      if (i == 8) _i = 0; else ++_i;
      UPDATE_SIGNALS;
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
  PangoAttribute *attr;
  PangoAttrList *attr_lst;
  GtkWidget *event_box = NULL;
  GtkWidget *frame = NULL;
  char welcome_msg[256];
  int i, j, _i, _j;

  /* initialiszation of the GTK+ window*/
  gtk_init(&argc, &argv);

  /* window creation  */
  MainWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(MainWindow), "Sudoku Solver");
  gtk_window_set_position(GTK_WINDOW(MainWindow), GTK_WIN_POS_CENTER);
  gtk_window_set_resizable(GTK_WINDOW(MainWindow), FALSE);

  /* pango style */
  attr = pango_attr_size_new(20 * PANGO_SCALE);
  attr_lst = pango_attr_list_new();
  pango_attr_list_insert(attr_lst, attr);

  /* main table of the grid */
  table1 = gtk_table_new(3,3,TRUE);
  gtk_container_set_border_width(GTK_CONTAINER(table1), 10);

  for (i=0; i<9; ++i)
  {
    frame = gtk_frame_new(NULL);
    gtk_frame_set_shadow_type(GTK_FRAME(frame), GTK_SHADOW_OUT);
    gtk_widget_modify_bg (frame, GTK_STATE_NORMAL, &c_dark);

    /* create the secondary tables */
    table2 = gtk_table_new(3,3,TRUE);
    gtk_container_add(GTK_CONTAINER(frame), table2);
    gtk_table_attach(GTK_TABLE(table1), frame, i/3, (i/3)+1, (i%3), (i%3)+1, GTK_FILL | GTK_EXPAND, GTK_FILL | GTK_EXPAND, 0, 0);

    for (j=0; j<9; ++j)
    {
      event_box = gtk_event_box_new();
      txt = gtk_label_new("");
      gtk_widget_modify_fg(txt, GTK_STATE_NORMAL, &c_black);

      gtk_label_set_attributes(GTK_LABEL(txt), attr_lst);
      gtk_label_set_justify(GTK_LABEL(txt), GTK_JUSTIFY_CENTER);
      gtk_widget_set_size_request(event_box, 50, 50);

      _j = (i%3)*3 + ((j%3));
      _i =(i/3)*3 + ((j/3));
      sdk_grid[_i][_j].widget = txt;
      sdk_grid[_i][_j].event_box = event_box;
      sdk_grid[_i][_j].i = _i;
      sdk_grid[_i][_j].j = _j;

      GTK_WIDGET_SET_FLAGS (event_box,GTK_CAN_FOCUS);

      /* set the background of each event-box */
      gtk_widget_modify_bg (event_box, GTK_STATE_NORMAL, &c_white);
      sdk_grid[_i][_j].color = &c_white;
      if (i%2) {
        if (j%2) {
          gtk_widget_modify_bg (event_box, GTK_STATE_NORMAL, &c_light_gray);
          sdk_grid[_i][_j].color = &c_light_gray;
        }
      } else {
        if (! (j%2)) {
          gtk_widget_modify_bg (event_box, GTK_STATE_NORMAL, &c_light_gray);
          sdk_grid[_i][_j].color = &c_light_gray;
        }
      }

      gtk_container_add(GTK_CONTAINER(event_box), txt);
      gtk_table_attach(GTK_TABLE(table2), event_box, j/3, (j/3)+1, j%3, (j%3)+1, GTK_FILL | GTK_EXPAND, GTK_FILL | GTK_EXPAND, 0, 0);

      /* add signals to the event_box */
      g_signal_connect(G_OBJECT(event_box), "button_press_event", G_CALLBACK(button_press), (gpointer) &sdk_grid[_i][_j]);
      g_signal_connect(G_OBJECT(event_box), "key_press_event", G_CALLBACK(key_press), (gpointer) &sdk_grid[_i][_j]);
      g_signal_connect(G_OBJECT(event_box), "focus_in_event", G_CALLBACK(focus_in_event), NULL);
      g_signal_connect(G_OBJECT(event_box), "focus_out_event", G_CALLBACK(focus_out_event), NULL);
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

  resetGrid();

  gtk_main();

  return EXIT_SUCCESS;
}

#endif
