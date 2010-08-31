/*
 * =====================================================================================
 *
 *       Filename:  sdk_grid.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  08/31/2010 07:44:05 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Sylvain Didelot (), didelot.sylvain@gmail.com
 *        Company:
 *
 * =====================================================================================
 */

#ifndef __SDK_SIDEBAR_H
#define __SDK_SIDEBAR_H
#ifdef GTK_ENABLE

/**
 *  sdk_gui_grid_add()
 * \brief Add an entry into the sdk grid
 *
 * \param i i index of the sdk grid
 * \param j j index of the sdk grid
 * \param value value of the sdk grid located at the position i,j
 */
#define sdk_gui_grid_add(i, j, value) { \
  sdk_grid[i][j].value = value; \
  if (value) \
  gtk_label_set_text(sdk_grid[i][j].widget, STR(value)); \
  gtk_label_set_text(sdk_grid[i][j].widget, "");



  /**
   *  sdk_gui_init_grid()
   * \brief Initialize a new sdk grid
   *
   * \param grid sdk grid
   */
  GtkWidget* sdk_gui_init_grid(struct sdk_grid_entry_s grid[][9]);


#endif
#endif

