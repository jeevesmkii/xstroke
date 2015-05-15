/* xlp_callback.c - data structures for registering callbacks for X events

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

#include "xlp_win.h"
#include "xlp_callback.h"

#ifdef DMALLOC
#include "dmalloc.h"
#endif

int xlp_callback_init(xlp_callback_t *cb, xlp_win_t *xlp_win,
		      Window window,int type,
		      pthread_mutex_t *mutex,
		      xlp_cb_fun_t cb_fun, void *data)
{
    cb->xlp_win = xlp_win;
    cb->window = window;
    cb->type = type;
    cb->mutex = mutex;
    cb->cb_fun = cb_fun;
    cb->data = data;
    cb->next = NULL;

    return 0;
}

void xlp_callback_deinit(struct xlp_callback *cb)
{
    cb->xlp_win = NULL;
    cb->window = None;
    cb->type = 0;
    cb->mutex = NULL;
    cb->cb_fun = NULL;
    cb->data = NULL;
    cb->next = NULL;
}
