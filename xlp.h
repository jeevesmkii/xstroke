/* xlp.h - top-level include for XLP

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

#ifndef XLP_H
#define XLP_H

#include <X11/Xlib.h>

#include "xlp_win.h"
#include "xlp_callback.h"
#include "xlp_timeout.h"

struct xlp
{
    Display *dpy;
    
    pthread_t timeout_thread;
    pthread_mutex_t dpy_mutex;

    xlp_callback_t *callbacks[LASTEvent];

    xlp_timeout_t *timeouts;
    pthread_cond_t timeouts_cond;
    pthread_mutex_t timeouts_mutex;

    int exit_requested;
    int exit_code;
};
typedef struct xlp xlp_t;

int xlp_init(xlp_t *xlp, char *display);
void xlp_deinit(xlp_t *xlp);

int xlp_main_loop_start(xlp_t *xlp);
void xlp_main_loop_stop(xlp_t *xlp, int exit_code);

int xlp_register_window_callback(xlp_t *xlp, struct xlp_win *xlp_win,
				 Window window, int type,
				 xlp_cb_fun_t cb_fun, void *data);
int xlp_register_callback(xlp_t *xlp, struct xlp_win *xlp_win, int type,
			  xlp_cb_fun_t cb_fun, void *data);
int xlp_register_timeout(xlp_t *xlp, long delay_ms,
			 xlp_to_fun_t to_fun, void *data,
			 struct timeval *start_tv_ret);

#endif
