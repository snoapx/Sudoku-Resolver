/*
 * =====================================================================================
 *
 *       Filename:  sdk_resolver.c
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

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "sdk_resolver.h"

#define BUFF 64
#define ERROR_BUF 256


static struct sdk_grid_entry_s sdk_grid [9][9];

static int
sdk_errno()
{
  fprintf(stderr, "ERROR: %s\n", strerror(errno));
  abort();
}

static void
sdk_error (const char *fmt, ...)
{
  va_list ap;
  char line[ERROR_BUF];
  char vline[ERROR_BUF];

  va_start(ap, fmt);
  vsprintf(vline, fmt, ap);
  va_end(ap);

  sprintf(line, "ERROR: %s\n", vline);
  fprintf(stderr, line);

  abort();
}


/**
 *  sdk_showGrid()
 * @brief Print a grid in the console
 */
int sdk_showGrid()
{
  int i = 0;
  int j;

  while (i < 9)
  {
    j = 0;
    while (j < 9)
    {
      if (j == 0)
        if(sdk_grid[i][j].value == 0)
          fprintf(stdout, "| ");
        else
          fprintf(stdout, "|%d", sdk_grid[i][j].value);
      else if (j == 8)
        if(sdk_grid[i][j].value == 0)
          fprintf(stdout, "  |");
        else
          fprintf(stdout, " %d|", sdk_grid[i][j].value);
      else if (j%3 == 0)
        if(sdk_grid[i][j].value == 0)
          fprintf(stdout, "| ");
        else
          fprintf(stdout, "|%d", sdk_grid[i][j].value);
      else
        if(sdk_grid[i][j].value == 0)
          fprintf(stdout, "  ");
        else
          fprintf(stdout, " %d", sdk_grid[i][j].value);
      ++j;
    }
    if ((i%3 == 2) && i!=8)
      fprintf(stdout, "\n------+-----+------\n");
    else
      fprintf(stdout, "\n");
    ++i;
  }
  fprintf(stdout, "\n");
}

/* check constrains for a grid's entry */
static int check_constrains(struct sdk_grid_entry_s* entry, int value)
{
  int i, j;
  int k, l;
  int i2, j2;

  i = entry->i;
  j = entry->j;

  /* lines constrains */
  for(k=0; k<9; ++k)
  {
    if (k != j)
    {
      if (value == sdk_grid[i][k].value)
        return 0;
    }
  }

  /* rows constrains */
  for(k=0; k<9; ++k)
  {
    if (k != i)
    {
      if (value == sdk_grid[k][j].value)
        return 0;
    }
  }

  /* 3x3 square constrains */
  i2 = i/3 * 3;
  j2 = j/3 * 3;

  for(k=i2; k<(i2+3); ++k)
  {
    for(l=j2; l<(j2+3); ++l)
    {
      if (! ((k==i) & (l==j)))
      {
        if (value == sdk_grid[k][l].value)
        {
            return 0;
        }
      }
    }
  }
  return 1;
}

/* fill possible values for a grid entry */
static void fillPossibleValues(struct sdk_grid_entry_s* entry)
{
  int i;

  for (i=0; i<10; ++i)
  {
    entry->possibleValues[i] = i;
  }
}

/* fill possible values for a grid entry */
static void showPossibleValues(struct sdk_grid_entry_s* entry)
{
  int i;

  for (i=0; i<10; ++i)
  {
    fprintf(stderr, "%d ", entry->possibleValues[i]);
  }
  fprintf(stderr, "\n");
}


/* shuffle possible values for a grid entry*/
static void shufflePossibleValues(struct sdk_grid_entry_s* entry)
{
  int i, j, k;

  for (i=1; i<10; ++i)
  {
    j = i + rand() / (RAND_MAX / (10 - i) + 1);
    k = entry->possibleValues[j];
    entry->possibleValues[j] = entry->possibleValues[i];
    entry->possibleValues[i] = k;
  }
}

static struct sdk_grid_entry_s*
sdk_nextEntry(struct sdk_grid_entry_s* grid)
{
  int i, j;

  if ( (grid->i==8) && (grid->j==8))
    return NULL;
  else if (grid->j==8)
  {
    i=grid->i + 1;
    j=0;
  }
  else
  {
    i = grid->i;
    j = grid->j;
    ++j;
  };

  return &sdk_grid[i][j];
}

static void
sdk_resolveGrid_recurs(struct sdk_grid_entry_s grid[][9],
    struct sdk_grid_entry_s* entry,
    int* nb_solutions, int* nb_computations)
{
  int i;
  struct sdk_grid_entry_s* next = NULL;

  if (entry == NULL)
  {
    ++(*nb_solutions);
    memcpy(grid, sdk_grid, 9*9*sizeof(struct sdk_grid_entry_s));
//    fprintf(stderr,"%d solution found: \n", *nb_solutions);
    sdk_showGrid();
    return;
  } else if (entry->isBase) {
    next = sdk_nextEntry(entry);
    sdk_resolveGrid_recurs(grid, next, nb_solutions, nb_computations);
  } else  {
    i = 1;
    while(i < 10)
    {
      if (check_constrains(entry, entry->possibleValues[i]))
      {
        ++ *nb_computations;

        entry->value = entry->possibleValues[i];
        next = sdk_nextEntry(entry);
        sdk_resolveGrid_recurs(grid, next, nb_solutions, nb_computations);
      }
      ++i;
    }
    entry->value = 0;
  }
}

void
sdk_resolveGrid(struct sdk_grid_entry_s grid[][9], int* nb_solutions, int* nb_computations)
{
  sdk_resolveGrid_recurs(grid, &sdk_grid[0][0], nb_solutions, nb_computations);
}

int
sdk_generateGrid()
{
  int i,j;

  for(i=0; i<9; ++i)
  {
    for(j=0; j<9; ++j)
    {
      fillPossibleValues(&sdk_grid[i][j]);
      shufflePossibleValues(&sdk_grid[i][j]);
    }
  }
}

/**
 *  sdk_openGrid()
 * @brief Open a Sudoku grid file
 */
int
sdk_openGrid(const char* path, struct sdk_grid_entry_s grid[][9])
{
  FILE* grid_fd = NULL;
  int i = 0;
  int j;
  char line[BUFF];
  char* c;

  if ((grid_fd = fopen(path, "r")) == NULL)
    return SDK_ERR_FILE_NOT_FOUND;
  fprintf(stderr, "Open file %s\n", path);

  while (i < 9)
  {
    if (fgets(line, BUFF, grid_fd) == NULL)
      return SDK_ERR_WRONG_GRID_FORMAT;

    j = 0;
    while (j < 9)
    {
      c = (char*) line + j * 2;

      if (!isdigit(*c))
        return SDK_ERR_UNKNOWN_CHAR;

      sdk_grid[i][j].value = atoi(c);
      if (sdk_grid[i][j].value != 0)
        sdk_grid[i][j].isBase = 1;
      else
        sdk_grid[i][j].isBase = 0;

      sdk_grid[i][j].i = i;
      sdk_grid[i][j].j = j;

      fillPossibleValues(&sdk_grid[i][j]);
      shufflePossibleValues(&sdk_grid[i][j]);
//      showPossibleValues(&sdk_grid[i][j]);
      ++j;
    }
    ++i;
  }
  memcpy(grid, sdk_grid, 9*9*sizeof(struct sdk_grid_entry_s));

  return 0;
}
