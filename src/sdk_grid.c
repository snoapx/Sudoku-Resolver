/*
 * =====================================================================================
 *
 *       Filename:  sdk_grid.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  08/31/2010 07:44:00 PM
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

#include "sdk_resolver.h"
#include "sdk_colors.h"
#include "sdk_gui.h"

GtkWidget* sdk_gui_init_grid(struct sdk_grid_entry_s grid[][9])
{
  GtkWidget* table1 = NULL;
  GtkWidget* table2 = NULL;
  GtkWidget* txt = NULL;

  PangoAttribute *attr;
  PangoAttrList *attr_lst;
  GtkWidget *event_box = NULL;
  GtkWidget *frame = NULL;
  int i, j, _i, _j;

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
    gtk_table_attach(GTK_TABLE(table1), frame, i%3, (i%3)+1, (i/3), (i/3)+1, GTK_FILL | GTK_EXPAND, GTK_FILL | GTK_EXPAND, 0, 0);

    for (j=0; j<9; ++j)
    {
      event_box = gtk_event_box_new();
      txt = gtk_label_new("");
      gtk_widget_modify_fg(txt, GTK_STATE_NORMAL, &c_black);

      gtk_label_set_attributes(GTK_LABEL(txt), attr_lst);
      gtk_label_set_justify(GTK_LABEL(txt), GTK_JUSTIFY_CENTER);
      gtk_widget_set_size_request(event_box, 50, 50);

      _i =(i/3)*3 + ((j/3));
      _j = (i%3)*3 + ((j%3));
      grid[_i][_j].widget = txt;
      grid[_i][_j].event_box = event_box;
      grid[_i][_j].i = _i;
      grid[_i][_j].j = _j;

      GTK_WIDGET_SET_FLAGS (event_box,GTK_CAN_FOCUS);

      /* set the background of each event-box */
      gtk_widget_modify_bg (event_box, GTK_STATE_NORMAL, &c_white);
      grid[_i][_j].color = &c_white;
      if (i%2) {
        if (j%2) {
          gtk_widget_modify_bg (event_box, GTK_STATE_NORMAL, &c_light_gray);
          grid[_i][_j].color = &c_light_gray;
        }
      } else {
        if (! (j%2)) {
          gtk_widget_modify_bg (event_box, GTK_STATE_NORMAL, &c_light_gray);
          grid[_i][_j].color = &c_light_gray;
        }
      }

      gtk_container_add(GTK_CONTAINER(event_box), txt);
      gtk_table_attach(GTK_TABLE(table2), event_box, j%3, (j%3)+1, j/3, (j/3)+1, GTK_FILL | GTK_EXPAND, GTK_FILL | GTK_EXPAND, 0, 0);

      /* add signals to the event_box */
      g_signal_connect(G_OBJECT(event_box), "button_press_event", G_CALLBACK(button_press), (gpointer) &grid[_i][_j]);
      g_signal_connect(G_OBJECT(event_box), "key_press_event", G_CALLBACK(key_press), (gpointer) &grid[_i][_j]);

      /* signal connections not needed */
      //      g_signal_connect(G_OBJECT(event_box), "focus_in_event", G_CALLBACK(focus_in_event), NULL);
      //      g_signal_connect(G_OBJECT(event_box), "focus_out_event", G_CALLBACK(focus_out_event), NULL);
    }
  }
  return table1;
}

#endif

