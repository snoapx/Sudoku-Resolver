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

#ifndef __SDK_RESOLVER_H
#define __SDK_RESOLVER_H

#define STR(s) #s
#define XSTR(s) STR(s)
#define SDK_VERSION_MAJOR 1
#define SDK_VERSION_MINOR 0
#define SDK_VERSION_RC 3
#define SDK_VERSION XSTR(SDK_VERSION_MAJOR)"."XSTR(SDK_VERSION_MINOR)"-rc"XSTR(SDK_VERSION_RC)

#define SDK_COMPILE_DATE __DATE__
#define SDK_COMPILE_TIME __TIME__

#define SDK_LICENSE \
"Sudoku Resolver - An easy-to-use tool for finding the solution of a\n\
Sudoku grid.\n\
Copyright (C) 2010  Sylvain Didelot - didelot.sylvain@gmail.com\n\
\n\
This program is free software: you can redistribute it and/or modify\n\
it under the terms of the GNU General Public License as published by\n\
the Free Software Foundation, either version 3 of the License, or\n\
(at your option) any later version.\n\
\n\
This program is distributed in the hope that it will be useful,\n\
but WITHOUT ANY WARRANTY; without even the implied warranty of\n\
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n\
GNU General Public License for more details.\n\
\n\
You should have received a copy of the GNU General Public License\n\
along with this program.  If not, see <http://www.gnu.org/licenses/>."

/* ERRORS */
#define SDK_ERR_FILE_NOT_FOUND 1
#define SDK_ERR_WRONG_GRID_FORMAT 2
#define SDK_ERR_UNKNOWN_CHAR 3

#ifdef __GNUC__
#define UNUSED __attribute__ ((unused))
#else
#define UNUSED
#endif

struct sdk_grid_entry_s
{
  int i;
  int j;

  int value;

  int isBase;
  int possibleValues[10];
};

typedef enum {
  SDK_FILE_FORMAT_LATEX,
  SDK_FILE_FORMAT_TEXT,
  SDK_FILE_FORMAT_PDF
} sdk_file_format;


/**
 *  sdk_resolveGrid()
 * \brief Resolves a Sudoku grid
 */
void sdk_resolveGrid(struct sdk_grid_entry_s grid[][9], struct sdk_grid_entry_s result[][9], int* nb_solutions, int* nb_computations, int one_solution);


/**
 *  sdk_generateGrid()
 * \brief Generate a Sudoku grid
 */
void sdk_generateGrid(struct sdk_grid_entry_s grid[][9], void (*func)(int));


/**
 *  sdk_resetGrid()
 * \brief Reset a grid. Each entry is set up
 */
void sdk_resetGrid(struct sdk_grid_entry_s grid[][9], int i, int j);

int sdk_checkConstrains(struct sdk_grid_entry_s grid[][9], struct sdk_grid_entry_s* entry, int value);

/**
 *  sdk_openGrid()
 * \brief Open a Sudoku grid from a path
 */
int sdk_openGrid(const char* path, struct sdk_grid_entry_s grid[][9]);


/**
 *  sdk_saveGrid()
 * \brief Save a Sudoku grid to a path
 */
int sdk_saveGrid(const char* path, struct sdk_grid_entry_s grid[][9], sdk_file_format format);


/**
 *  sdk_showGrid()
 * \brief Show a grid in terminal mode
 */
void sdk_showGrid();

#endif

