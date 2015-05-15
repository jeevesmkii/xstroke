/* matrix.c -- Fixed-point 2D linear algebra operations with matrices

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

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "matrix.h"
#include "fixed.h"

#ifdef DMALLOC
#include "dmalloc.h"
#endif

int matrix_init(matrix_t *matrix, int precision)
{
    matrix->precision_bits = precision;
    matrix_set_identity(matrix);
    
    return 0;
}

void matrix_set_explicit(matrix_t *matrix,
			double m00, double m01, double m02,
			double m10, double m11, double m12,
			double m20, double m21, double m22)
{
    matrix->m[0][0] = f_to_fixed(m00, matrix->precision_bits);
    matrix->m[0][1] = f_to_fixed(m01, matrix->precision_bits);
    matrix->m[0][2] = f_to_fixed(m02, matrix->precision_bits);

    matrix->m[1][0] = f_to_fixed(m10, matrix->precision_bits);
    matrix->m[1][1] = f_to_fixed(m11, matrix->precision_bits);
    matrix->m[1][2] = f_to_fixed(m12, matrix->precision_bits);
    
    matrix->m[2][0] = f_to_fixed(m20, matrix->precision_bits);
    matrix->m[2][1] = f_to_fixed(m21, matrix->precision_bits);
    matrix->m[2][2] = f_to_fixed(m22, matrix->precision_bits);
}

void matrix_set_identity(matrix_t *matrix)
{
    matrix_set_explicit(matrix,
			1.0, 0.0, 0.0,
			0.0, 1.0, 0.0,
			0.0, 0.0, 1.0);
}

void matrix_set_translate(matrix_t *matrix, double x_off, double y_off)
{
    matrix_set_explicit(matrix,
			1.0, 0.0, x_off,
			0.0, 1.0, y_off,
			0.0, 0.0, 1.0);
}

void matrix_set_scale(matrix_t *matrix, double x_scale, double y_scale)
{
    matrix_set_explicit(matrix,
			x_scale,     0.0, 0.0,
			0.0,     y_scale, 0.0,
			0.0,         0.0, 1.0);
}

void matrix_set_rotate(matrix_t *matrix, double theta)
{
    matrix_set_explicit(matrix,
			cos(theta), -sin(theta), 0.0,
			sin(theta),  cos(theta), 0.0,
			0.0,                0.0, 1.0);
}

void matrix_set_rotate_about(matrix_t *matrix, double theta, double x, double y)
{
    matrix_t tmp, to_origin, rot, from_origin;

    /* XXX: This only works if (int) x and (int) y fit in 16 bits. It
       would be better to dynamically compute the maximum allowable
       precision for the rotation and intermediate matrices. */
    matrix_init(&to_origin, 0);
    matrix_init(&rot, 16);
    matrix_init(&tmp, 16);
    matrix_init(&from_origin, 0);

    matrix_set_translate(&to_origin, -x, -y);
    matrix_set_rotate(&rot, theta);
    matrix_multiply(&tmp, &rot, &to_origin);

    matrix_set_translate(&from_origin, x, y);
    matrix_multiply(matrix, &from_origin, &tmp);
}

void matrix_multiply(matrix_t *result, matrix_t *a, matrix_t *b)
{
    int i, j, k;
    int intermediate_precision;
    long int val;

    intermediate_precision = a->precision_bits + b->precision_bits;
    for (j=0; j<3; j++) {
	for (i=0; i<3; i++) {
	    val = 0;
	    for (k=0; k<3; k++) {
		val += fixed_adjust_precision(a->m[j][k] * b->m[k][i],
					      intermediate_precision,
					      result->precision_bits);
	    }
	    result->m[j][i] = val;
	}
    }
}

void matrix_copy(matrix_t *dest, matrix_t *src)
{
    int i, j;

    for (j=0; j<3; j++) {
	for (i=0; i<3; i++) {
	    dest->m[j][i] = src->m[j][i];
	}
    }
}
