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

#include <gtk/gtk.h>
#include "sdk_colors.h"

#ifdef GTK_ENABLE

#define SDK_NB_VISBLE_STEPS 15

static GtkWidget* grid_events[SDK_NB_VISBLE_STEPS];

GtkWidget* sdk_gui_init_sidebar()
{
  GtkWidget* frame;
  GtkWidget* frame_event;
  GtkWidget* table;
  GtkWidget* grid_event;
  GtkWidget* grid_event_box;
  GtkWidget* hbox;

  /* buttons */
  GtkWidget *prev, *forw;  /* previous, forward */

  int i;

  frame = gtk_frame_new("Game panel");
  gtk_container_set_border_width(GTK_CONTAINER(frame), 10);
  gtk_widget_set_size_request(frame, 200, -1);
  gtk_frame_set_label_align (frame, 0.5, 0.5);

  table = gtk_table_new(SDK_NB_VISBLE_STEPS+1, 1, TRUE);
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

    grid_events[i] = grid_event;
  }

  return frame;
}



#endif
