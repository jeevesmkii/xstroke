/* xlp_color.h - XLP functions for manipulating colors

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

#ifndef XLP_COLOR_H
#define XLP_COLOR_H

#include <X11/extensions/Xrender.h>
#include <X11/Xft/Xft.h>
#include "xlp_win.h"

struct xlp_color
{
    XColor core_color;
    XRenderColor color;
    XftColor xft_color;
};
typedef struct xlp_color xlp_color_t;

struct xlp_win;
int xlp_color_init(xlp_color_t *xlp_color, struct xlp_win *xlp_win, char *color);
void xlp_color_deinit(xlp_color_t *xlp_color, struct xlp_win *xlp_win);

#endif
