/*
 * =====================================================================================
 *
 *       Filename:  main.c
 *
 *    Description:  Main function of sdk_resolver
 *
 *        Version:  1.0
 *        Created:  07/22/2010 09:05:04 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Sylvain Didelot, didelot.sylvain@gmail.com
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <stdlib.h>

#include "sdk_resolver.h"
#include "sdk_gui.h"

void print_usage(char* prog_name)
{
  fprintf(stderr, "Usage: %s PATH_GRID\n", prog_name);
}

int main(int argc, char** argv)
{
  int res;
  int nb_solutions = 0;
  int nb_computations = 0;
  struct sdk_grid_entry_s sdk_grid [9][9];

  if (argc != 1)
  {
    sdk_openGrid(argv[1], sdk_grid);
    sdk_showGrid();

    sdk_resolveGrid(sdk_grid, &nb_solutions, &nb_computations);

    if (nb_solutions != 0)
    {
      fprintf(stderr, "%d solutions found in %d computations\n", nb_solutions, nb_computations);
      return(EXIT_SUCCESS);
    } else {
      return(EXIT_FAILURE);
    }
  }

  sdk_gui_init(argc, argv);
}

