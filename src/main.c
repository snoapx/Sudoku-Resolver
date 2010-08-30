/*
 * =====================================================================================
 *
 *       Filename:  main.c
 *
 *    Description:  Sudoku resolver - A easy-to-use tool for finding the
 *                  solution of a Sudoku grid.
 *
 *        Created:  07/22/2010 09:05:04 PM
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

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>


#include "sdk_resolver.h"

#ifdef GTK_ENABLE
#include "sdk_gui.h"
#endif

/* print the usage of the program */
void print_usage(char* prog_name)
{
  fprintf(stderr, "Usage: %s PATH_GRID\n", prog_name);
}

/* main function */
int main(int argc, char** argv)
{
  int nb_solutions = 0;
  int nb_computations = 0;
  struct sdk_grid_entry_s sdk_grid [9][9];

  if (argc != 1)
  {
    sdk_openGrid(argv[1], sdk_grid);
    sdk_showGrid(sdk_grid);

    sdk_resolveGrid(sdk_grid, NULL, &nb_solutions, &nb_computations, 0);
    sdk_showGrid(sdk_grid);

    if (nb_solutions != 0)
    {
      fprintf(stderr, "%d solutions found in %d computations\n", nb_solutions, nb_computations);
      return(EXIT_SUCCESS);
    } else {
      return(EXIT_FAILURE);
    }
  } else {
#ifdef GTK_ENABLE
    FILE *fp = fopen(SDK_LOCK, "rb");

    if (fp == NULL)
    {
      if (errno == ENOENT)
      {
        FILE *fpw = fopen(SDK_LOCK, "wrb");
        if (fpw == NULL)
        {
          fprintf(stderr, "Cannot write into the current directory");
          return(EXIT_FAILURE);
        } else {
          fputs("sudoku", fpw);
          fclose(fpw);
        }
      } else {
        fprintf(stderr, "Cannot write into the current directory");
        return(EXIT_FAILURE);
      }
    } else {
      fprintf(stderr, "Cannot launch 2 instances of the program.\nIf the program has crashed the last time and that you know what you are doing, remove the file "SDK_LOCK"!\n");
      fclose(fp);
      return(EXIT_FAILURE);
    }

    sdk_gui_init(argc, argv);
#else
    print_usage(argv[0]);
#endif
  }
  return(EXIT_SUCCESS);
}
