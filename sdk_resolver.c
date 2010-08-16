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
#include <time.h>
#include <ctype.h>
#include "sdk_resolver.h"

#define BUFF 64
#define ERROR_BUF 256


//static struct sdk_grid_entry_s sdk_grid [9][9];

UNUSED
  static int sdk_errno()
{
  fprintf(stderr, "ERROR: %s\n", strerror(errno));
  abort();
}

UNUSED
  static void sdk_error (const char *fmt, ...)
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
void sdk_showGrid(struct sdk_grid_entry_s grid[][9])
{
  int i = 0;
  int j;

  while (i < 9)
  {
    j = 0;
    while (j < 9)
    {
      if (j == 0)
        if(grid[i][j].value == 0)
          fprintf(stdout, "| ");
        else
          fprintf(stdout, "|%d", grid[i][j].value);
      else if (j == 8)
        if(grid[i][j].value == 0)
          fprintf(stdout, "  |");
        else
          fprintf(stdout, " %d|", grid[i][j].value);
      else if (j%3 == 0)
        if(grid[i][j].value == 0)
          fprintf(stdout, "| ");
        else
          fprintf(stdout, "|%d", grid[i][j].value);
      else
        if(grid[i][j].value == 0)
          fprintf(stdout, "  ");
        else
          fprintf(stdout, " %d", grid[i][j].value);
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
static int check_constrains(struct sdk_grid_entry_s grid[][9],
struct sdk_grid_entry_s* entry, int value)
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
//      fprintf(stderr, "i=%d, k=%d, value=%d\n", i, k, value);
      if (value == grid[i][k].value)
        return 0;
    }
  }

  /* rows constrains */
  for(k=0; k<9; ++k)
  {
    if (k != i)
    {
      if (value == grid[k][j].value)
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
        if (value == grid[k][l].value)
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
UNUSED
static void showPossibleValues(struct sdk_grid_entry_s* entry)
{
  int i;

  for (i=0; i<10; ++i)
  {
    fprintf(stderr, "%d ", entry->possibleValues[i]);
  }
  fprintf(stderr, "\n");
}

static void traceGridPath(int* array)
{
  int i, j, k;

  for (i=0; i<80; ++i)
  {
    j = i + rand() / (RAND_MAX / (81 - i));
    k = array[j];
    array[j] = array[i];
    array[i] = k;
  }
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
sdk_nextEntry(struct sdk_grid_entry_s grid[][9],
struct sdk_grid_entry_s* entry)
{
  int i, j;

  if ( (entry->i==8) && (entry->j==8))
    return NULL;
  else if (entry->j==8)
  {
    i=entry->i + 1;
    j=0;
  }
  else
  {
    i = entry->i;
    j = entry->j;
    ++j;
  };

  return &grid[i][j];
}

static void
sdk_resolveGrid_recurs(struct sdk_grid_entry_s grid[][9],
    struct sdk_grid_entry_s result[][9],
    struct sdk_grid_entry_s* entry,
    int* nb_solutions, int* nb_computations, int one_solution)
{
  int i;
  struct sdk_grid_entry_s* next = NULL;

  if (entry == NULL)
  {
    ++(*nb_solutions);
    if (result != NULL)
      memcpy(result, grid, 81 * sizeof(struct sdk_grid_entry_s));
    return;
  } else if (entry->isBase) {
    next = sdk_nextEntry(grid, entry);
    sdk_resolveGrid_recurs(grid, result, next, nb_solutions, nb_computations, one_solution);
  } else  {
    i = 1;
    while(i < 10)
    {
      if (check_constrains(grid, entry, entry->possibleValues[i]))
      {
        ++ *nb_computations;

        entry->value = entry->possibleValues[i];
        next = sdk_nextEntry(grid, entry);
        sdk_resolveGrid_recurs(grid, result, next, nb_solutions, nb_computations, one_solution);
      }
      if ( ((*nb_solutions == 1) && (one_solution == 1)))
       {
          return;
       }

      ++i;
    }
    entry->value = 0;
  }
}

static void setBase(struct sdk_grid_entry_s grid[][9], int isBase)
{
  int i, j;

  for (i=0; i<9; ++i)
  {
    for (j=0; j<9; ++j)
    {
      grid[i][j].isBase = isBase;
    }
  }

}

static void fillPathEntries(int* path)
{
  int i;

  for (i=0; i<81; ++i)
  {
    path[i]=i+1;
  }
}

static void removePathEntry(struct sdk_grid_entry_s grid[][9], int entry)
{
  grid[entry/9][entry%9].value = 0;
  grid[entry/9][entry%9].isBase = 0;
}

void
sdk_resolveGrid(struct sdk_grid_entry_s grid[][9],
    struct sdk_grid_entry_s result[][9], int* nb_solutions, int* nb_computations, int one_solution)
{
  sdk_resolveGrid_recurs(grid, result, &grid[0][0], nb_solutions, nb_computations, one_solution);
}

void
sdk_generateGrid(struct sdk_grid_entry_s grid[][9], void (*func)(int))
{
  int i,j;
  int nb_solutions = 0;
  int nb_computations = 0;
  struct sdk_grid_entry_s tmp_grid [9][9];
  int path[81];

  srand(time(NULL));
  for(i=0; i<9; ++i)
  {
    for(j=0; j<9; ++j)
    {
      fillPossibleValues(&grid[i][j]);
      shufflePossibleValues(&grid[i][j]);
      grid[i][j].isBase = 0;
      grid[i][j].value = 0;
      grid[i][j].i = i;
      grid[i][j].j = j;
    }
  }
  sdk_resolveGrid(grid, NULL, &nb_solutions, &nb_computations, 1);
  setBase(grid, 1);
  fillPathEntries(path);
  traceGridPath(path);

  memcpy(tmp_grid, grid, 81 * sizeof(struct sdk_grid_entry_s));

  for(i=0; i<81; ++i)
  {
    fprintf(stderr, ".");
    func(i);

    nb_solutions = 0;
    nb_computations = 0;

    removePathEntry(tmp_grid, path[i]);

    sdk_resolveGrid(tmp_grid, NULL, &nb_solutions, &nb_computations, 0);
    if (nb_solutions != 1)
    {
      memcpy(tmp_grid, grid, 81 * sizeof(struct sdk_grid_entry_s));
    }
    else
    {
      memcpy(grid, tmp_grid, 81 * sizeof(struct sdk_grid_entry_s));
    }
  }
  fprintf(stderr, ".\n");

  nb_solutions = 0;
  nb_computations = 0;
  sdk_resolveGrid(tmp_grid, NULL, &nb_solutions, &nb_computations, 0);

  //TODO Check solutions number
  fprintf(stderr, "Number of solutions : %d\n", nb_solutions);
}

/**
 *  sdk_openGrid()
 * @brief Save a Sudoku grid into a file
 */
int
sdk_saveGrid(const char* path, struct sdk_grid_entry_s grid[][9], sdk_file_format format)
{
  FILE* grid_fd = NULL;
  int i = 0;
  int j;
  char line[BUFF];

  if ((grid_fd = fopen(path, "w")) == NULL)
    return SDK_ERR_FILE_NOT_FOUND;
  fprintf(stderr, "Save file %s\n", path);

  switch(format)
  {
    case SDK_FILE_FORMAT_TEXT:
      while (i < 9)
      {
        sprintf(line, "%d %d %d %d %d %d %d %d %d\n",
            grid[i][0].value, grid[i][1].value, grid[i][2].value,
            grid[i][3].value, grid[i][4].value, grid[i][5].value,
            grid[i][6].value, grid[i][7].value, grid[i][8].value);

        fputs(line, grid_fd);

        ++i;
      }
      break;
    case SDK_FILE_FORMAT_LATEX:

      fputs("\\documentclass[12pt,a4paper]{article}", grid_fd);
      fputs("\\begin{document}\n", grid_fd);
      fputs("\\begin{tabular}{||c|c|c||c|c|c||c|c|c||}\n\\hline\\hline\n", grid_fd);
      while(i < 9)
      {
        j = 0;
        while(j < 9)
        {
          if (grid[i][j].value != 0)
            sprintf(line, "%d", grid[i][j].value);
          else
            sprintf(line, " ");
          fputs(line, grid_fd);

          if (j != 8)
          {
            fputs("&", grid_fd);
          }
          else
          {
            fputs(" \\\\\n", grid_fd);
          }
          ++j;
        }

        if ((i+1)%3)
          fputs("\\hline\n", grid_fd);
        else
          fputs("\\hline\\hline\n", grid_fd);

        ++i;
      }
      fputs("\\end{tabular}\n", grid_fd);
      fputs("\\end{document}", grid_fd);
      break;

  }
  fclose(grid_fd);

  return 0;
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

      grid[i][j].value = atoi(c);
      if (grid[i][j].value != 0)
        grid[i][j].isBase = 1;
      else
        grid[i][j].isBase = 0;

      grid[i][j].i = i;
      grid[i][j].j = j;

      fillPossibleValues(&grid[i][j]);
      ++j;
    }
    ++i;
  }

  fclose(grid_fd);
  return 0;
}
