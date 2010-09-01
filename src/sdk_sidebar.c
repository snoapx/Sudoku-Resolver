/*
 * =====================================================================================
 *
 *       Filename:  sdk_sidebar.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  08/31/2010 11:44:41 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Sylvain Didelot (), didelot.sylvain@gmail.com
 *        Company:
 *
 * =====================================================================================
 */

#ifdef GTK_ENABLE

#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include "sdk_colors.h"
#include "sdk_gui.h"
#include "sdk_sidebar.h"
#include "sdk_resolver.h"

#define SDK_NB_VISBLE_STEPS 15

static struct sdk_event_list_s* event_list = NULL;
static struct sdk_event_list_s* last_event = NULL;

static GtkWidget* grid_events[SDK_NB_VISBLE_STEPS];
static GtkWidget* grid_events_box[SDK_NB_VISBLE_STEPS];
static GtkWidget* nb_errors_label;
static int nb_errors = 0;


/* grid shown by the application */
static struct sdk_grid_entry_s (*sdk_grid)[9];
static struct sdk_grid_entry_s (*sdk_result)[9];

static void updateNbErrors(int i)
{
  char line[256];
  sprintf(line, "You have made %d errors", i);

  gtk_label_set_text(GTK_LABEL(nb_errors_label), line);
}

void sdk_gui_sidebar_reset_errors()
{
  nb_errors = 0;
  updateNbErrors(0);
}

void previousStepEvent(GtkWidget *widget, GdkEventKey *event, gpointer data) {
  sdk_gui_sidebar_del_event();
  sdk_gui_sidebar_refresh_list();
  sdk_gui_check_constraints();
}

void nextStepEvent(GtkWidget *widget, GdkEventKey *event, gpointer data) {

}

GtkWidget* sdk_gui_init_sidebar(struct sdk_grid_entry_s grid[][9], struct sdk_grid_entry_s result[][9])
{
  GtkWidget* frame;
  GtkWidget* frame_event;
  GtkWidget* table;
  GtkWidget* grid_event;
  GtkWidget* grid_event_box;
  GtkWidget* hbox;

  sdk_grid = grid;
  sdk_result = result;

  /* buttons */
  GtkWidget *prev, *forw;  /* previous, forward */

  int i;

  frame = gtk_frame_new("Game panel");
  gtk_container_set_border_width(GTK_CONTAINER(frame), 10);
  gtk_widget_set_size_request(frame, 200, -1);
  gtk_frame_set_label_align (GTK_FRAME(frame), 0.5, 0.5);

  table = gtk_table_new(SDK_NB_VISBLE_STEPS+2, 1, TRUE);
  gtk_container_add(GTK_CONTAINER(frame), table);

  hbox = gtk_hbox_new(FALSE, 0);
  prev = gtk_button_new_with_label("previous");
  forw = gtk_button_new_with_label("forward");
  gtk_container_add(GTK_CONTAINER(hbox), prev);
  gtk_container_add(GTK_CONTAINER(hbox), forw);
  gtk_table_attach(GTK_TABLE(table), hbox, 0, 1, 0, 0+1, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);


  for (i=1; i<SDK_NB_VISBLE_STEPS+1; ++i)
  {
    grid_event_box = gtk_event_box_new();
    gtk_widget_modify_bg (grid_event_box, GTK_STATE_NORMAL, &c_white);
    grid_event = gtk_label_new(NULL);
    frame_event = gtk_frame_new(NULL);
    gtk_container_add(GTK_CONTAINER(grid_event_box), grid_event);
    gtk_container_add(GTK_CONTAINER(frame_event), grid_event_box);
    gtk_table_attach(GTK_TABLE(table), frame_event, 0, 1, i, i+1, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);

    grid_events[i-1] = grid_event;
    grid_events_box[i-1] = grid_event_box;
  }

  nb_errors_label = gtk_label_new(NULL);
  nb_errors = 0;
  updateNbErrors(nb_errors);
  gtk_table_attach(GTK_TABLE(table), nb_errors_label, 0, 1, SDK_NB_VISBLE_STEPS+2, SDK_NB_VISBLE_STEPS+2+1, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);

  /* signals */
  g_signal_connect(G_OBJECT(prev), "clicked", G_CALLBACK(previousStepEvent), NULL);
  g_signal_connect(G_OBJECT(forw), "clicked", G_CALLBACK(nextStepEvent), NULL);

  return frame;
}


void sdk_gui_sidebar_reset() {
  struct sdk_event_list_s* del = event_list;
  struct sdk_event_list_s* tmp = NULL;

  while(del) {
    tmp = del->next;
    free(del);
    del = tmp;
  }

  event_list = NULL;
  last_event = NULL;
}

void sdk_gui_sidebar_refresh_list() {
  struct sdk_event_list_s* tmp = last_event;
  int nb = SDK_NB_VISBLE_STEPS;
  char line[256];

  while(tmp && nb)
  {
    sprintf(line, "[%d] [%d] : entry value=%d", tmp->i, tmp->j, tmp->value);
    gtk_label_set_text(GTK_LABEL(grid_events[SDK_NB_VISBLE_STEPS-nb]), line);

    if (playing_mode) {
      if (tmp->value == sdk_result[tmp->i][tmp->j].value) {
        gtk_widget_modify_bg (grid_events_box[SDK_NB_VISBLE_STEPS-nb], GTK_STATE_NORMAL, &c_green);
      }
      else
        gtk_widget_modify_bg (grid_events_box[SDK_NB_VISBLE_STEPS-nb], GTK_STATE_NORMAL, &c_red);
    }

    --nb;
    tmp = tmp->prev;
  }
  while(nb)
  {
    gtk_label_set_text(GTK_LABEL(grid_events[SDK_NB_VISBLE_STEPS-nb]), "");
    gtk_widget_modify_bg (grid_events_box[SDK_NB_VISBLE_STEPS-nb], GTK_STATE_NORMAL, &c_white);
    --nb;
  }
}

void sdk_gui_sidebar_del_event() {
  struct sdk_event_list_s *event = NULL;
  char line[256];

  if (!last_event)
    return;

  event = last_event->prev;
  sdk_grid[last_event->i][last_event->j].value = last_event->value;

  if (last_event->old_value)
  {
    sprintf(line, "%d", last_event->old_value);
    gtk_label_set_text(GTK_LABEL(sdk_grid[last_event->i][last_event->j].widget), line);
  }
  else
    gtk_label_set_text(GTK_LABEL(sdk_grid[last_event->i][last_event->j].widget), "");


  free(last_event);

  if (event)
  {
    button_press(sdk_grid[event->i][event->j].event_box, NULL, &sdk_grid[event->i][event->j]);
    event->next = NULL;
  }

  last_event = event;

}

void sdk_gui_sidebar_add_event(int i, int j, int value, int old_value) {
  struct sdk_event_list_s *event;

  if (last_event) {
    if ( (last_event->i == i) &&
        (last_event->j == j) ) {
      if (value == 0)
      {
        sdk_gui_sidebar_del_event();
        sdk_gui_sidebar_refresh_list();
        sdk_gui_check_constraints();
        return;
      }

      if (last_event->value != value)
      {
        last_event->value = value;
        last_event->old_value = old_value;

        /* check errors */
        if (last_event->value != sdk_result[last_event->i][last_event->j].value) {
          ++nb_errors;
          updateNbErrors(nb_errors);
        }
      }

      return;
    }
  }

  event = malloc(sizeof(struct sdk_event_list_s));

  event->i = i;
  event->j = j;
  event->value = value;
  event->old_value = old_value;
  event->next = NULL;

  /* empty list */
  if (event_list == NULL)
  {
    event_list = event;
    event->prev = NULL;
  } else {
    event->prev = last_event;
  }

  last_event = event;

  /* check errors */
  if (last_event->value != sdk_result[last_event->i][last_event->j].value) {
    ++nb_errors;
    updateNbErrors(nb_errors);
  }
}


#endif
