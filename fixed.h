/* fixed.h -- Simple routines to aid with fixed-point arithmetic

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

#ifndef FIXED_H
#define FIXED_H

int f_to_fixed(double val, int precision);
double fixed_to_f(long int val, int precision);
int fixed_to_i_round(long int val, int precision);
int fixed_to_i_trunc(long int val, int precision);
int fixed_adjust_precision(long int val, int precision_old, int precision_new);

#endif
