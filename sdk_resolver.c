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

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "sdk_resolver.h"

#define BUFF 64
#define ERROR_BUF 256

static int sdk_errno()
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
}

static int check_constrains(const int i, const int j)
{
  int k, l;
  int i2, j2;

  /* lines constrains */
  for(k=0; k<9; ++k)
  {
    if (k != j)
    {
//      fprintf(stderr, "Check constrains [%d][%d]%d - [%d][%d]%d\n", i, j, sdk_grid[i][j].value, i, k, sdk_grid[i][k].value);
      if (sdk_grid[i][j].value == sdk_grid[i][k].value)
        return 0;
    }
  }

  /* rows constrains */
  for(k=0; k<9; ++k)
  {
    if (k != i)
    {
      if (sdk_grid[i][j].value == sdk_grid[k][j].value)
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
        if (sdk_grid[i][j].value == sdk_grid[k][l].value)
        {
            return 0;
        }
      }
    }
  }
  return 1;
}

int sdk_resolveGrid()
{
  int i, j;
  i = 0;
  int computations = 0;

  while(i < 9)
  {
    j = 0;
    while(j < 9)
    {
      if (!sdk_grid[i][j].isBase)
      {
        do
        {
          sdk_grid[i][j].value += 1;
        }
        while (sdk_grid[i][j].value < 10 && !check_constrains(i, j));
        ++computations;

        if (sdk_grid[i][j].value == 10)
        {
          sdk_grid[i][j].value = 0;

          do {
            --j;
            if (j == -1)
            {
              j = 8; --i;
              if (i == -1)
              {
                return -1;
              }
            }
          }while(sdk_grid[i][j].isBase);
        }
        else ++j;
      }
      else ++j;
    }
    ++i;
  }

  return computations;
}

int sdk_openGrid(const char* path)
{
  FILE* grid = NULL;
  int i = 0;
  int j;
  char line[BUFF];
  char* c;

  if ((grid = fopen(path, "r")) == NULL)
    sdk_errno();
  fprintf(stderr, "Open file %s\n", path);

  while (i < 9)
  {
    if (fgets(line, BUFF, grid) == NULL)
      sdk_error("wrong grid format");

    j = 0;
    while (j < 9)
    {
      c = (char*) line + j * 2;

      if (!isdigit(*c))
        sdk_error("wrong char found : %c", *c);

      sdk_grid[i][j].value = atoi(c);
      if (sdk_grid[i][j].value != 0)
        sdk_grid[i][j].isBase = 1;
      else
        sdk_grid[i][j].isBase = 0;

      ++j;
    }
    ++i;
  }
}
