/* log.h -- logging facilities for xstroke

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

#ifndef LOG_H
#define LOG_H

struct log
{
    char *filename;
    FILE *file;
    int cnt;
    int threshold_min;
    int threshold_max;
};
typedef struct log log_t;

int log_init(log_t *log, char *filename, int threshold);
void log_deinit(log_t *log);
int log_msg(log_t *log, const char *msg);

#endif
