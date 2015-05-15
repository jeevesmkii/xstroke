/* rec_parse.h -- Grammar for xstroke recognition control file

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

#ifndef REC_PARSE_H
#define REC_PARSE_H

#include "rec_parse_tab.h"

int rec_parse(rec_t *rec, char *file_name);

#endif
