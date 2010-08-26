/*
 * =====================================================================================
 *
 *       Filename:  sdk_error.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  24/08/2010 22:42:53
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Sylvain Didelot, didelot.sylvain@gmail.com
 *
 * =====================================================================================
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

