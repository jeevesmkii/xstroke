/* printf_alloca.c -- like sprintf with memory allocation

   Copyright 2001 Carl Worth

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
*/

#ifndef SPRINTF_ALLOC_H
#define SPRINTF_ALLOC_H

int sprintf_alloc(char **str, const char *fmt, ...);
int sprintf_append_alloc(char **str, const char *fmt, ...);

#endif
