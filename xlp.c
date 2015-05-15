/* xlp_event.c - event handling functions

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
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <signal.h>
#include <limits.h>
#include <sys/time.h>
#include <X11/Xlib.h>

#include "xlp.h"

#ifdef DMALLOC
#include "dmalloc.h"
#endif

static long event_mask_for_type(int type);
static int add_callback(xlp_t *xlp, xlp_callback_t *cb);

#ifndef XLP_WITHOUT_THREADS
static void remove_timeout_and_adjust_others(xlp_t *xlp, xlp_timeout_t *timeout);
static void *xlp_timeout_thread_start(void *arg);
#endif

static void *xlp_x_event_thread_start(void *arg);

int xlp_init(xlp_t *xlp, char *display)
{
    int i;

    pthread_mutex_init(&xlp->dpy_mutex, NULL);

    for (i=0; i < LASTEvent; i++) {
	xlp->callbacks[i] = NULL;
    }

    pthread_mutex_init(&xlp->timeouts_mutex, NULL);
    pthread_cond_init(&xlp->timeouts_cond, NULL);

    pthread_mutex_lock(&xlp->timeouts_mutex);
    xlp->timeouts = NULL;
    pthread_mutex_unlock(&xlp->timeouts_mutex);

    xlp->dpy = XOpenDisplay(display);
    if (xlp->dpy == NULL) {
	fprintf(stderr, "%s: An error occurred trying to open display %s\n",
		__FUNCTION__, display);
	return EIO;
    }

    return 0;
}

void xlp_deinit(xlp_t *xlp)
{
    int i;
    xlp_callback_t *cb, *prev_cb;
    xlp_timeout_t *to, *prev_to;

    pthread_mutex_destroy(&xlp->dpy_mutex);

    for (i=0; i < LASTEvent; i++) {
	cb = xlp->callbacks[i];
	while (cb) {
	    prev_cb = cb;
	    cb = cb->next;
	    xlp_callback_deinit(prev_cb);
	    /* alloc'ed in xlp_register_window_callback */
	    free(prev_cb);
	}
	xlp->callbacks[i] = NULL;
    }

    pthread_mutex_lock(&xlp->timeouts_mutex);
    to = xlp->timeouts;
    while (to) {
	prev_to = to;
	to = to->next;
	xlp_timeout_deinit(prev_to);
	/* alloc'ed in xlp_register_timeout */
	free(prev_to);
    }
    xlp->timeouts = NULL;
    pthread_mutex_unlock(&xlp->timeouts_mutex);

    pthread_cond_destroy(&xlp->timeouts_cond);
    pthread_mutex_destroy(&xlp->timeouts_mutex);

    XCloseDisplay(xlp->dpy);
    xlp->dpy = NULL;
}

int xlp_register_window_callback(xlp_t *xlp, xlp_win_t *xlp_win,
				 Window window, int type,
				 xlp_cb_fun_t cb_fun, void *data)
{
    xlp_callback_t *cb;

    /* free'ed in xlp_deinit */
    cb = malloc(sizeof(xlp_callback_t));
    if (cb == NULL) {
	fprintf(stderr, "%s: Out of memory\n", __FUNCTION__);
	return ENOMEM;
    }

    xlp_callback_init(cb, xlp_win, window, type,
		      &xlp_win->xlp->dpy_mutex,
		      cb_fun, data);
    add_callback(xlp, cb);

    xlp_win->event_mask |= event_mask_for_type(type);
    XSelectInput(xlp_win->dpy, xlp_win->window, xlp_win->event_mask);

    return 0;
}

int xlp_register_callback(xlp_t *xlp, xlp_win_t *xlp_win, int type,
			  xlp_cb_fun_t cb_fun, void *data)
{
    return xlp_register_window_callback(xlp, xlp_win, xlp_win->window, type,
					cb_fun, data);
}


int xlp_register_timeout(xlp_t *xlp, long delay_ms,
			 xlp_to_fun_t to_fun, void *data,
			 struct timeval *start_tv_ret)
{
    xlp_timeout_t *to;


    /* free'ed in xlp_deinit or remove_timeout_and_adjust_others */
    to = malloc(sizeof(xlp_timeout_t));
    if (to == NULL) {
	fprintf(stderr, "%s: Out of memory\n", __FUNCTION__);
	return ENOMEM;
    }
    xlp_timeout_init(to, delay_ms, &xlp->dpy_mutex, to_fun, data);

    if (start_tv_ret) {
	*start_tv_ret = to->start_tv;
    }

    pthread_mutex_lock(&xlp->timeouts_mutex);

    /* Don't care about the order of timeouts, place them at the head
       of the list where it is easy. */
    to->next = xlp->timeouts;
    xlp->timeouts = to;

    pthread_cond_signal(&xlp->timeouts_cond);

    pthread_mutex_unlock(&xlp->timeouts_mutex);

    return 0;
}

static int add_callback(xlp_t *xlp, xlp_callback_t *cb)
{
    xlp_callback_t *last;

    cb->next = NULL;

    /* New callbacks go to the end of the list */
    last = xlp->callbacks[cb->type];
    if (last == NULL) {
	xlp->callbacks[cb->type] = cb;
    } else {
	while (last->next)
	    last = last->next;
	last->next = cb;
    }

    return 0;
}

int xlp_main_loop_start(xlp_t *xlp)
{
    int err;
    sigset_t set;

    xlp->exit_requested = 0;
    xlp->exit_code = 0;

    sigemptyset(&set);
    sigaddset(&set, SIGALRM);
    sigaddset(&set, SIGUSR1);
    err = pthread_sigmask(SIG_BLOCK, &set, NULL);
    if (err) {
	fprintf(stderr, "%s: Error calling pthread_sigmask.\n", __FUNCTION__);
	return err;
    }

#ifdef XLP_WITHOUT_THREADS
    fprintf(stderr, "%s: ERROR: xlp was compiled with XLP_WITHOUT_THREADS defined.\n"
	    "\tThis means that no xlp_timeouts will work!\n", __FUNCTION__);
#else
    err = pthread_create(&xlp->timeout_thread, NULL,
			 xlp_timeout_thread_start, xlp);
    if (err) {
	fprintf(stderr, "%s: Error starting timeout_thread.\n",	__FUNCTION__);
	return err;
    }
    pthread_detach(xlp->timeout_thread);
#endif
    
    return (int) xlp_x_event_thread_start(xlp);
}

void xlp_main_loop_stop(xlp_t *xlp, int exit_code)
{
    xlp->exit_requested = 1;
    xlp->exit_code = exit_code;
}

static void *xlp_x_event_thread_start(void *arg)
{
    xlp_t *xlp = (xlp_t *) arg;
    struct timespec ts = { 0, 100000};
    while (! xlp->exit_requested)  {
	XEvent xev;
	xlp_callback_t *cb;

	pthread_mutex_lock(&xlp->dpy_mutex);
	while (! XPending(xlp->dpy)) {
	    pthread_mutex_unlock(&xlp->dpy_mutex);
	    nanosleep(&ts, NULL);
	    pthread_mutex_lock(&xlp->dpy_mutex);
	}
	XNextEvent(xlp->dpy, &xev);
	pthread_mutex_unlock(&xlp->dpy_mutex);
	for (cb = xlp->callbacks[xev.type]; cb; cb = cb->next) {
	    if (xev.xany.window == cb->window) {
		pthread_mutex_lock(cb->mutex);
		(cb->cb_fun)(&xev, cb->data);
		XSync(xlp->dpy, False);
		pthread_mutex_unlock(cb->mutex);
	    }
	}
    }
    return (void *) xlp->exit_code;
}

#ifndef XLP_WITHOUT_THREADS
static void *xlp_timeout_thread_start(void *arg)
{
    xlp_t *xlp = (xlp_t *) arg;
    xlp_timeout_t *to, *min;
    xlp_timeout_t long_max;
    sigset_t alrmset;
    struct itimerval timerval;
    int sig, err;

    long_max.delay_ms = LONG_MAX;

    sigemptyset(&alrmset);
    sigaddset(&alrmset, SIGALRM | SIGINT);

    while(1) {
	pthread_mutex_lock(&xlp->timeouts_mutex);

	min = &long_max;
	for (to = xlp->timeouts; to; to = to->next) {
	    if (to->delay_ms < min->delay_ms) {
		min = to;
	    }
	}
	if (min == &long_max) {
	    pthread_cond_wait(&xlp->timeouts_cond, &xlp->timeouts_mutex);
	    pthread_mutex_unlock(&xlp->timeouts_mutex);
	} else {
	    pthread_mutex_unlock(&xlp->timeouts_mutex);
	    timerval.it_interval.tv_sec = 0;
	    timerval.it_interval.tv_usec = 0;
	    timerval.it_value.tv_sec = min->delay_ms / 1000;
	    timerval.it_value.tv_usec = (min->delay_ms & 1000) * 1000;
	    setitimer(ITIMER_REAL, &timerval, NULL);
	    err = sigwait(&alrmset, &sig);
	    if (err == 0 && sig == SIGALRM) {
		pthread_mutex_lock(min->mutex);
		(min->to_fun)(min->start_tv, min->data);
		XSync(xlp->dpy, False);
		pthread_mutex_unlock(min->mutex);
	    } else if (sig == SIGINT) {
		break;
	    }
	    remove_timeout_and_adjust_others(xlp, min);
	}
    }

    return (void *) 0;
}

/* XXX: UGH! What was I thinking when I wrote this???  This is about
   the most horrid timeout handling I could imagine.  Instead, each
   timeout should make a start_tv and end_tv as soon as possible after
   creation. Then, eliminate all this arithmetic which is accumulating
   timing errors.
*/
static void remove_timeout_and_adjust_others(xlp_t *xlp, xlp_timeout_t *timeout)
{
    xlp_timeout_t *to, *to_last;

    pthread_mutex_lock(&xlp->timeouts_mutex);

    to_last = NULL;
    for (to = xlp->timeouts; to; to = to->next) {
	if (to == timeout) {
	    if (to_last) {
		to_last->next = to->next;
	    } else {
		xlp->timeouts = to->next;
	    }
	} else {
	    to->delay_ms -= timeout->delay_ms;
	}
    }
    free(to);

    pthread_mutex_unlock(&xlp->timeouts_mutex);
}
#endif /* not XLP_WITHOUT_THREADS */

/*
static char *event_name_for_type(int type)
{
    switch(type) {
    case KeyPress:
	return "KeyPress";
    case KeyRelease:
	return "KeyRelease";
    case ButtonPress:
	return "ButtonPress";
    case ButtonRelease:
	return "ButtonRelease";
    case MotionNotify:
	return "MotionNotify";
    case EnterNotify:
	return "EnterNotify";
    case LeaveNotify:
	return "LeaveNotify";
    case FocusIn:
	return "FocusIn";
    case FocusOut:
	return "FocusOut";
    case KeymapNotify:
	return "KeymapNotify";
    case Expose:
	return "Expose";
    case GraphicsExpose:
	return "GraphicsExpose";
    case NoExpose:
	return "NoExpose";
    case VisibilityNotify:
	return "VisibilityNotify";
    case CreateNotify:
	return "CreateNotify";
    case DestroyNotify:
	return "DestroyNotify";
    case UnmapNotify:
	return "UnmapNotify";
    case MapNotify:
	return "MapNotify";
    case ReparentNotify:
	return "ReparentNotify";
    case ConfigureNotify:
	return "ConfigureNotify";
    case ConfigureRequest:
	return "ConfigureRequest";
    case GravityNotify:
	return "GravityNotify";
    case ResizeRequest:
	return "ResizeRequest";
    case CirculateNotify:
	return "CirculateNotify";
    case CirculateRequest:
	return "CirculateRequest";
    case PropertyNotify:
	return "PropertyNotify";
    case SelectionClear:
	return "SelectionClear";
    case SelectionRequest:
	return "SelectionRequest";
    case SelectionNotify:
	return "SelectionNotify";
    case ColormapNotify:
	return "ColorMapNotify";
    case ClientMessage:
	return "ClientMessage";
    case MappingNotify:
	return "MappingNotify";
    default:
	fprintf(stderr, "%s:%d: ERROR: Unknown event type %d\n", __FILE__, __LINE__, type);
	return "(ERROR: Unknown event type)";
    }
    
}
*/

static long event_mask_for_type(int type)
{
    switch(type) {
    case KeyPress:
	return KeyPressMask;
    case KeyRelease:
	return KeyReleaseMask;
    case ButtonPress:
	return ButtonPressMask;
    case ButtonRelease:
	return ButtonReleaseMask;
    case MotionNotify:
	return PointerMotionMask | ButtonMotionMask;
    case EnterNotify:
	return EnterWindowMask;
    case LeaveNotify:
	return LeaveWindowMask;
    case FocusIn:
    case FocusOut:
	return FocusChangeMask;
    case KeymapNotify:
	return KeymapStateMask;
    case Expose:
    case GraphicsExpose:
    case NoExpose:
	return ExposureMask;
    case VisibilityNotify:
	return VisibilityChangeMask;
    case CreateNotify:
	return SubstructureNotifyMask;
    case DestroyNotify:
    case UnmapNotify:
    case MapNotify:
	return StructureNotifyMask;
    case MapRequest:
	return SubstructureRedirectMask;
    case ReparentNotify:
    case ConfigureNotify:
	return StructureNotifyMask;
    case ConfigureRequest:
	return SubstructureRedirectMask;
    case GravityNotify:
	return StructureNotifyMask;
    case ResizeRequest:
	return ResizeRedirectMask;
    case CirculateNotify:
	return StructureNotifyMask;
    case CirculateRequest:
	return SubstructureRedirectMask;
    case PropertyNotify:
	return PropertyChangeMask;
    case SelectionClear:
    case SelectionRequest:
    case SelectionNotify:
	return NoEventMask;
    case ColormapNotify:
	return ColormapChangeMask;
    case ClientMessage:
    case MappingNotify:
	return NoEventMask;
    default:
	fprintf(stderr, "%s:%d: ERROR: Unknown event type %d\n", __FILE__, __LINE__, type);
	return NoEventMask;
    }
}
