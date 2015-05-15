/* xlp_color.c - XLP functions for manipulating colors

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
#include <errno.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "xlp_win.h"
#include "xlp_color.h"

#ifdef DMALLOC
#include "dmalloc.h"
#endif

int xlp_color_init(xlp_color_t *xlp_color, xlp_win_t *xlp_win, char *color)
{
    int err;

    err = XParseColor(xlp_win->dpy,
		      DefaultColormap(xlp_win->dpy, xlp_win->screen),
		      color, &xlp_color->core_color);
    if (err == 0) {
	fprintf(stderr, "__FUNCTION__: Failed to parse color %s\n", color);
	return EINVAL;
    }

    xlp_color->color.red = xlp_color->core_color.red;
    xlp_color->color.green = xlp_color->core_color.green;
    xlp_color->color.blue = xlp_color->core_color.blue;
    xlp_color->color.alpha = 0xffff;
    XAllocColor(xlp_win->dpy, DefaultColormap(xlp_win->dpy, xlp_win->screen),
		&xlp_color->core_color);
    XftColorAllocValue(xlp_win->dpy,
		       DefaultVisual(xlp_win->dpy, xlp_win->screen),
		       DefaultColormap(xlp_win->dpy, xlp_win->screen),
		       &xlp_color->color, &xlp_color->xft_color);

    return err;
}

void xlp_color_deinit(xlp_color_t *xlp_color, xlp_win_t *xlp_win)
{
    XftColorFree(xlp_win->dpy,
		 DefaultVisual(xlp_win->dpy, xlp_win->screen),
		 DefaultColormap(xlp_win->dpy, xlp_win->screen),
		 &xlp_color->xft_color);
    XFreeColors(xlp_win->dpy, DefaultColormap(xlp_win->dpy, xlp_win->screen),
		&xlp_color->core_color.pixel, 1, 0);
}
