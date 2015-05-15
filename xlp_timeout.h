/* xlp_timeout.h - data structures for registering callbacks for timeouts

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

#ifndef XLP_TIMEOUT_H
#define XLP_TIMEOUT_H

#include <sys/time.h>
#include <pthread.h>

typedef void (*xlp_to_fun_t)(struct timeval start_tv, void *data);

struct xlp_timeout
{
    pthread_mutex_t *mutex;
    struct timeval start_tv;
    unsigned long delay_ms;
    xlp_to_fun_t to_fun;
    void *data;
    struct xlp_timeout *next;
};
typedef struct xlp_timeout xlp_timeout_t;

int xlp_timeout_init(struct xlp_timeout *to, long delay_ms,
		     pthread_mutex_t *mutex,
		     xlp_to_fun_t to_fun, void *data);
void xlp_timeout_deinit(struct xlp_timeout *to);

#endif
