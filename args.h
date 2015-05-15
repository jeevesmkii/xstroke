/* args.h - parse command-line args
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

#ifndef ARGS_H
#define ARGS_H

struct args
{
    int argc;
    char **argv;
    char *display;
    char *geometry;
    char *fg;
    char *bg;
    char *class;
    char *name;
    int width;
    char *font;
    char *alphabet;
    char *logdir;
    int logmax;
    int lograw;
    int nograb;
    int rotation;
    double rotation_limit;
    int button;
    int hold_time_ms;
    int auto_disable;
    char *verify;
};
typedef struct args args_t;

#include "xstroke.h"

#include <X11/X.h>
#include <math.h>

#define ARGS_DEFAULT_DISPLAY NULL
#ifdef USE_SYSTEM_TRAY
#define ARGS_DEFAULT_GEOMETRY "16x16"
#else
#define ARGS_DEFAULT_GEOMETRY "32x16"
#endif
#ifdef USE_PIXMAPS
#define ARGS_DEFAULT_FG "#000000"
#else
#define ARGS_DEFAULT_FG "#eeeeee"
#endif
#define ARGS_DEFAULT_BG "#666699"
#define ARGS_DEFAULT_CLASS "Xstroke"
#define ARGS_DEFAULT_NAME "xstroke"
#define ARGS_DEFAULT_WIDTH 3
#ifdef USE_PIXMAPS
#define ARGS_DEFAULT_FONT "mono-6.5"
#else
#define ARGS_DEFAULT_FONT "mono-8"
#endif
/* Realative to ${HOME} */
#define ARGS_DEFAULT_ALPHABET ".xstroke/alphabet"
#define ARGS_FALLBACK_ALPHABET XSTROKE_CONF_DIR "/alphabet"
#define ARGS_DEFAULT_LOGDIR "/var/log/xstroke"
#define ARGS_DEFAULT_LOGMAX 200
#define ARGS_DEFAULT_LOGRAW 0
#define ARGS_DEFAULT_ROTATION 1
#define ARGS_DEFAULT_ROTATION_LIMIT (M_PI_4 / 4.0)
#define ARGS_DEFAULT_BUTTON AnyButton
#define ARGS_DEFAULT_HOLD_TIME_MS 750
#define ARGS_DEFAULT_AUTO_DISABLE 1
#define ARGS_DEFAULT_VERIFY NULL

#ifdef XSTROKE_NOGRAB_SERVER
# define ARGS_DEFAULT_NOGRAB 1
#else
# define ARGS_DEFAULT_NOGRAB 0
#endif

int args_init(args_t *args);
void args_deinit(args_t *args);
int args_parse(args_t *args, int argc, char *argv[]);
void args_usage(char *argv[]);

#endif
