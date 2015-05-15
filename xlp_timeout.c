/* xlp_timeout.c - data structures for registering callbacks for timeouts

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
#include <sys/time.h>

#include "xlp_timeout.h"

#ifdef DMALLOC
#include "dmalloc.h"
#endif

int xlp_timeout_init(xlp_timeout_t *to, long delay_ms,
		     pthread_mutex_t *mutex, xlp_to_fun_t to_fun, void *data)
{
    gettimeofday(&to->start_tv, NULL);
    to->delay_ms = delay_ms;
    to->mutex = mutex;
    to->to_fun = to_fun;
    to->data = data;
    to->next = NULL;

    return 0;
}

void xlp_timeout_deinit(xlp_timeout_t *to)
{
    to->delay_ms = 0;
    to->mutex = NULL;
    to->to_fun = NULL;
    to->data = NULL;
    to->next = NULL;
}
