/* xlp_callback.h - data structures for registering callbacks for X events

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

#ifndef XLP_CALLBACK_H
#define XLP_CALLBACK_H

#include <pthread.h>
#include "xlp_win.h"

typedef void (*xlp_cb_fun_t)(XEvent *xev, void *data);
struct xlp_callback
{
    struct xlp_win *xlp_win;
    Window window;
    int type;
    pthread_mutex_t *mutex;
    xlp_cb_fun_t cb_fun;
    void *data;
    struct xlp_callback *next;
};
typedef struct xlp_callback xlp_callback_t;

int xlp_callback_init(struct xlp_callback *cb, struct xlp_win *xlp_win,
		      Window window, int type,
		      pthread_mutex_t *mutex,
		      xlp_cb_fun_t cb_fun, void *data);
void xlp_callback_deinit(struct xlp_callback *cb);

#endif
