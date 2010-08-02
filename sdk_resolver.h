/*
 * =====================================================================================
 *
 *       Filename:  sdk_resolver.h
 *
 *    Description:  Sudoku resolver.
 *
 *        Version:  1.0
 *        Created:  07/22/2010 07:54:00 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Sylvain Didelot, didelot.sylvain@gmail.com
 *
 * =====================================================================================
 */

#ifndef __SDK_RESOLVER_H
#define __SDK_RESOLVER_H

struct sdk_grid_entry_s
{
  int value;
  int isBase;
};

int sdk_openGrid(const char* path);
int sdk_resolveGrid();
int sdk_showGrid();


#endif

