/* xlp_win.c - XLP functions for manipulating windows

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
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "xlp_win.h"
#include "xlp_color.h"

#ifdef DMALLOC
#include "dmalloc.h"
#endif

static void xlp_configure_notify_cb(XEvent *xev, void *data);

int xlp_win_init(xlp_win_t *xlp_win, xlp_t *xlp, char *geometry, char *default_geometry, char *fg_color, char *bg_color, char *font, char *name, char *class)
{
    XSizeHints size_hints;
    XClassHint class_hint;
    int x, y, width, height;
    Display *dpy = xlp->dpy;

    xlp_win->xlp = xlp;
    xlp_win->dpy = dpy;
    xlp_win->screen = DefaultScreen(dpy);

    size_hints.flags = PWinGravity;

    XWMGeometry(dpy, xlp_win->screen,
		geometry, default_geometry, 0,
		&size_hints, &x, &y, &width, &height,
		&size_hints.win_gravity);

    /* The GNOME panel doesn't work properly unless the size hints 
       specify a size, so be sure to provide one.  */
    size_hints.flags |= PBaseSize;
    size_hints.base_width = width;
    size_hints.base_height = height;

    xlp_color_init(&xlp_win->fg, xlp_win, fg_color);
    xlp_color_init(&xlp_win->bg, xlp_win, bg_color);

    xlp_win->window = XCreateSimpleWindow(dpy,
					  DefaultRootWindow(dpy),
					  x, y, width, height, 1,
					  xlp_win->fg.core_color.pixel,
					  xlp_win->bg.core_color.pixel);
    xlp_win->window_owner = 1;
    xlp_win->event_mask = NoEventMask;
    XSetWMNormalHints(dpy, xlp_win->window, &size_hints);

    /* We want to track the size/position of all windows */
    xlp_win_register_callback(xlp_win, ConfigureNotify,
			      xlp_configure_notify_cb, xlp_win);

    class_hint.res_name = name;
    class_hint.res_class = class;
    XSetClassHint(dpy, xlp_win->window, &class_hint);

    XStoreName(dpy, xlp_win->window, name);

    xlp_win->draw = XftDrawCreate(dpy, xlp_win->window,
				  DefaultVisual(dpy, xlp_win->screen),
				  DefaultColormap(dpy, xlp_win->screen));
    if (xlp_win->draw == NULL) {
	fprintf(stderr, "%s: An error occurred during XftDrawCreate\n", __FUNCTION__);
	return -1;
    }

    xlp_win->font = XftFontOpenName(dpy, xlp_win->screen, font);
    if (xlp_win->font == NULL) {
	fprintf(stderr, "%s: An error occurred during XftFontOpenName\n", __FUNCTION__);
	return -1;
    }

    return 0;
}

int xlp_win_init_from_window(xlp_win_t *xlp_win, xlp_t *xlp, Window window, char *fg_color, char *bg_color)
{
    xlp_win->xlp = xlp;
    xlp_win->dpy = xlp->dpy;
    xlp_win->window = window;
    xlp_win->window_owner = 0;
    xlp_win->screen = DefaultScreen(xlp->dpy);

    xlp_color_init(&xlp_win->fg, xlp_win, fg_color);
    xlp_color_init(&xlp_win->bg, xlp_win, bg_color);

    xlp_win->event_mask = NoEventMask;

    /* We want to track the size/position of all windows */
    xlp_win_register_callback(xlp_win, ConfigureNotify,
			      xlp_configure_notify_cb, xlp_win);

    xlp_win->draw = NULL;
    xlp_win->font = NULL;

    return 0;
}

void xlp_win_deinit(xlp_win_t *xlp_win)
{
    if (xlp_win->font) {
	XftFontClose(xlp_win->dpy, xlp_win->font);
	xlp_win->font = NULL;
    }
    if (xlp_win->draw) {
	XftDrawDestroy(xlp_win->draw);
	xlp_win->draw = NULL;
    }
    if (xlp_win->window_owner) {
	XDestroyWindow(xlp_win->dpy, xlp_win->window);
    }
    xlp_color_deinit(&xlp_win->bg, xlp_win);
    xlp_color_deinit(&xlp_win->fg, xlp_win);
}

int xlp_win_map(xlp_win_t *xlp_win)
{
    XMapWindow(xlp_win->dpy, xlp_win->window);

    return 0;
}

int xlp_win_unmap(xlp_win_t *xlp_win)
{
    XUnmapWindow(xlp_win->dpy, xlp_win->window);

    return 0;
}

int xlp_win_register_window_callback(xlp_win_t *xlp_win,
				     Window window,
				     int type,
				     xlp_cb_fun_t cb_fun, void *data)
{
    return xlp_register_window_callback(xlp_win->xlp, xlp_win,
					window, type, cb_fun, data);
}

int xlp_win_register_callback(xlp_win_t *xlp_win, int type,
			      xlp_cb_fun_t cb_fun, void *data)
{
    return xlp_register_callback(xlp_win->xlp, xlp_win, type, cb_fun, data);
}

int xlp_win_register_timeout(xlp_win_t *xlp_win, long delay_ms,
			     xlp_to_fun_t to_fun, void *data,
			     struct timeval *start_tv_ret)
{
    return xlp_register_timeout(xlp_win->xlp, delay_ms,
				to_fun, data, start_tv_ret);
}

static void xlp_configure_notify_cb(XEvent *xev, void *data)
{
    XConfigureEvent *cev = &xev->xconfigure;
    xlp_win_t *xlp_win = (xlp_win_t *) data;

    if (xev->xany.window == xlp_win->window) {
	xlp_win->x = cev->x;
	xlp_win->y = cev->y;
	xlp_win->width = cev->width;
	xlp_win->height = cev->height;
    }
}
