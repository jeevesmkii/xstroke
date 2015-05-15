/* sprintf_alloc.c -- like sprintf with memory allocation

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
#include <stdarg.h>
#include <string.h>

#include "sprintf_alloc.h"

#ifdef DMALLOC
#include "dmalloc.h"
#endif

static int vsprintf_alloc(char **str, const char *fmt, va_list ap);

int sprintf_alloc(char **str, const char *fmt, ...)
{
    int ret;
    va_list ap;

    va_start(ap, fmt);
    ret = vsprintf_alloc(str, fmt, ap);
    va_end(ap);

    return ret;
}

int sprintf_append_alloc(char **str, const char *fmt, ...)
{
    char *appendage, *new_str;
    int appendage_len, new_len;
    va_list ap;

    va_start(ap, fmt);
    appendage_len = vsprintf_alloc(&appendage, fmt, ap);
    va_end(ap);

    new_len = ((*str) ? strlen(*str) : 0) + appendage_len + 1;
    if (*str) {
	new_str = realloc(*str, new_len);
    } else {
	new_str = malloc(new_len);
	if (new_str) {
	    new_str[0] = '\0';
	}
    }
    if (new_str == NULL) {
	free(appendage);
	return strlen(*str);
    }

    *str = new_str;
    strcat(*str, appendage);
    free(appendage);

    return new_len;
}

/* ripped more or less straight out of PRINTF(3) */
static int vsprintf_alloc(char **str, const char *fmt, va_list ap)
{
    char *new_str;
    /* Guess we need no more than 100 bytes. */
    int n, size = 100;
 
    if ((*str = malloc (size)) == NULL)
	return -1;
    while (1) {
	/* Try to print in the allocated space. */
	n = vsnprintf (*str, size, fmt, ap);
	/* If that worked, return the size. */
	if (n > -1 && n < size)
	    return n;
	/* Else try again with more space. */
	if (n > -1)    /* glibc 2.1 */
	    size = n+1; /* precisely what is needed */
	else           /* glibc 2.0 */
	    size *= 2;  /* twice the old size */
	new_str = realloc(*str, size);
	if (new_str == NULL) {
	    free(*str);
	    *str = NULL;
	    return -1;
	}
	*str = new_str;
    }
}
