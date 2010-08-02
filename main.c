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

  if (argc != 2)
  {
    print_usage(argv[0]);
    return(EXIT_FAILURE);
  }

  sdk_gui_init(argc, argv);

  sdk_openGrid(argv[1]);
  sdk_showGrid();

  res = sdk_resolveGrid();
  if (res != -1)
  {
    fprintf(stderr, "\n\nSolution found !\n");
    sdk_showGrid();
    fprintf(stderr, "Resolved with %d computations\n", res);
    return(EXIT_SUCCESS);
  } else {
    fprintf(stderr, "No solution found !\n");
    return(EXIT_FAILURE);
  }
}

