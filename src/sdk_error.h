/*
 * =====================================================================================
 *
 *       Filename:  sdk_error.h
 *
 *    Description:  Sudoku resolver - A easy-to-use tool for finding the
 *                  solution of a Sudoku grid.
 *
 *        Created:  24/08/2010 22:42:53
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

/* ERRORS */
#define SDK_ERR_OK 0
#define SDK_ERR_FILE_NOT_FOUND 1
#define SDK_ERR_WRONG_GRID_FORMAT 2
#define SDK_ERR_UNKNOWN_CHAR 3

struct sdk_error_line
{
  char err_code[256];
  char err_desc[256];
};


const struct sdk_error_line sdk_error[] = {
  {"SDK_ERR_OK", "Command executed with success"},
  {"SDK_ERR_FILE_NOT_FOUND", "File not found"},
  {"SDK_ERR_WRONG_GRID_FORMAT", "Wrong grid format"},
  {"SDK_ERR_UNKNOWN_CHAR", "Unknown char found"}
};

