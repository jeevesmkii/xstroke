/* rec_lex.h -- Lexical anaylzer for xstroke recognition control file

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

#ifndef REC_LEX_H
#define REC_LEX_H

int rec_lex_initial_file(char *filename);
char *rec_lex_location_alloc(void);
void rec_lex_newlines_wanted(int wanted);

#endif
