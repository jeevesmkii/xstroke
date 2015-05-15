/* xlp_win.h - XLP functions for manipulating windows

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

#ifndef XLP_WIN_H
#define XLP_WIN_H

#include <X11/Xlib.h>

#include "xlp.h"
#include "xlp_color.h"
#include "xlp_callback.h"
#include "xlp_timeout.h"

struct xlp_win
{
    struct xlp *xlp;
    Display *dpy;
    int screen;
    Window window;
    int window_owner;
    long event_mask;
    int x, y;
    int width, height;
    struct xlp_color fg;
    struct xlp_color bg;
    XftDraw *draw;
    XftFont *font;
};
typedef struct xlp_win xlp_win_t;

int xlp_win_init(xlp_win_t *xlp_win, struct xlp *xlp, char *geometry, char *default_geometry, char *fg_color, char *bg_color, char *font, char *name, char *class);
int xlp_win_init_from_window(xlp_win_t *xlp_win, struct xlp *xlp, Window window, char *fg_color, char *bg_color);
void xlp_win_deinit(xlp_win_t *xlp_win);

int xlp_win_map(xlp_win_t *xlp_win);
int xlp_win_unmap(xlp_win_t *xlp_win);

int xlp_win_register_callback(xlp_win_t *xlp_win, int type,
			      xlp_cb_fun_t cb_fun, void *data);
int xlp_win_register_window_callback(xlp_win_t *xlp_win,
				     Window window,
				     int type,
				     xlp_cb_fun_t cb_fun, void *data);
int xlp_win_register_timeout(xlp_win_t *xlp_win, long delay_ms,
			     xlp_to_fun_t to_fun, void *data,
			     struct timeval *start_tv_ret);


#endif /* XLP_WIN_H */
