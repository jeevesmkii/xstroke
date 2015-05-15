/* args.c - parse command-line args
 
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
#include <getopt.h>
#include <string.h>
#include <math.h>

#include "args.h"
#include "sprintf_alloc.h"

#ifdef DMALLOC
#include "dmalloc.h"
#endif

int args_init(args_t *args)
{
    char *home = getenv("HOME");

    args->argv = NULL;
    args->display = ARGS_DEFAULT_DISPLAY;
    args->geometry = ARGS_DEFAULT_GEOMETRY;
    args->fg = ARGS_DEFAULT_FG;
    args->bg = ARGS_DEFAULT_BG;
    args->class = ARGS_DEFAULT_CLASS;
    args->name = ARGS_DEFAULT_NAME;
    args->width = ARGS_DEFAULT_WIDTH;
    args->font = ARGS_DEFAULT_FONT;
    sprintf_alloc(&args->alphabet, "%s/%s", home ? home : ".", ARGS_DEFAULT_ALPHABET);
    args->logdir = ARGS_DEFAULT_LOGDIR;
    args->logmax = ARGS_DEFAULT_LOGMAX;
    args->lograw = ARGS_DEFAULT_LOGRAW;
    args->nograb = ARGS_DEFAULT_NOGRAB;
    args->rotation = ARGS_DEFAULT_ROTATION;
    args->rotation_limit = ARGS_DEFAULT_ROTATION_LIMIT;
    args->button = ARGS_DEFAULT_BUTTON;
    args->hold_time_ms = ARGS_DEFAULT_HOLD_TIME_MS;
    args->auto_disable = ARGS_DEFAULT_AUTO_DISABLE;
    args->verify = ARGS_DEFAULT_VERIFY;

    return 0;
}

void args_deinit(args_t *args)
{
    free(args->alphabet);
}

void args_usage(char *argv[])
{
    fprintf(stderr, "Usage: %s [options]\n", argv[0]);
    fprintf(stderr, "Valid options are:\n");
    fprintf(stderr, "\t-h, -help\n");
    fprintf(stderr, "\t-d, -display <display_name>\n");
    fprintf(stderr, "\t-g, -geometery <geom_spec>\n");
    fprintf(stderr, "\t-fg, -foreground <fg_color>\n");
    fprintf(stderr, "\t-bg, -background <bg_color>\n");
    fprintf(stderr, "\t-c, -class <class>\n");
    fprintf(stderr, "\t-n, -name <instance_name>\n");
    fprintf(stderr, "\t-w, -width <stroke_width>\n");
    fprintf(stderr, "\t-fn, -font <font_name>\n");
    fprintf(stderr, "\t-a, -alphabet <alphabet_file>\n");
    fprintf(stderr, "\t-ld, -logdir <log_directory>\n");
    fprintf(stderr, "\t-lm, -logmax <maximum strokes to log +/- 20%%>\n");
    fprintf(stderr, "\t-lr, -lograw (include raw stroke data in logs)\n");
    fprintf(stderr, "\t-G, -nograb (disable X server grab -- for debugging)\n");
    fprintf(stderr, "\t-r, -rotate (enable adaptive rotation)\n");
    fprintf(stderr, "\t-R, -norotate (disable adaptive rotation)\n");
    fprintf(stderr, "\t-b, -button (which button xstroke will grab -- 0 for any)\n");
    fprintf(stderr, "\t-ad, -auto (enable automatic disabling of recognition for certaing windows)\n");
    fprintf(stderr, "\t-Ad, -noauto (no automatic disabling -- xstroke works on all windows)\n");
    fprintf(stderr, "\t-hold hold_time (milliseconds)\n");
    fprintf(stderr, "\t-verify <raw_log_file> (verify that alphabet covers strokes in raw_log_file)\n");
}

int args_parse(args_t *args, int argc, char *argv[])
{
    int c;
    int option_index = 0;
    int parse_err = 0;
    static struct option long_options[] = {
	{"help", 0, 0, 'h'},
	{"display", 1, 0, 'd'},
	{"geometry", 1, 0, 'g'},
	{"fg", 1, 0, 'F'},
	{"foreground", 1, 0, 'F'},
	{"bg", 1, 0, 'B'},
	{"background", 1, 0, 'B'},
	{"class", 1, 0, 'c'},
	{"name", 1, 0, 'n'},
	{"width", 1, 0, 'w'},
	{"fn", 1, 0, 'f'},
	{"font", 1, 0, 'f'},
	{"alphabet", 1, 0, 'a'},
	{"logdir", 1, 0, 'L'},
	{"ld", 1, 0, 'L'},
	{"logmax", 1, 0, 'M'},
	{"lm", 1, 0, 'M'},
	{"lograw", 0, 0, 'Z'},
	{"lr", 0, 0, 'Z'},
	{"nograb", 0, 0, 'G'},
	{"rotate", 0, 0, 'r'},
	{"rotation", 0, 0, 'r'},
	{"norotate", 0, 0, 'R'},
	{"norotation", 0, 0, 'R'},
	{"button", 1, 0, 'b'},
	{"hold", 1, 0, 'H'},
	{"ad", 0, 0, 'Y'},
	{"auto", 0, 0, 'Y'},
	{"Ad", 0, 0, 'A'},
	{"noauto", 0, 0, 'A'},
	{"verify", 1, 0, 'V'},
	{0, 0, 0, 0}
    };

    args->argv = argv;
    args->argc = argc;

    while (1) {
	c = getopt_long_only(argc, argv, "hd:g:F:B:c:n:w:f:a:L:M:GrRb:H:V:", long_options, &option_index);
	if (c == -1)
	    break;

	switch (c) {
	case 'h':
	    parse_err++;
	    break;
	case 'd':
	    args->display = optarg;
	    break;
	case 'g':
	    args->geometry = optarg;
	    break;
	case 'F':
	    args->fg = optarg;
	    break;
	case 'B':
	    args->bg = optarg;
	    break;
	case 'c':
	    args->class = optarg;
	    break;
	case 'n':
	    args->name = optarg;
	    break;
	case 'w':
	    args->width = atoi(optarg);
	    break;
	case 'f':
	    args->font = optarg;
	    break;
	case 'a':
	    free(args->alphabet);
	    args->alphabet = strdup(optarg);
	    break;
	case 'L':
	    args->logdir = optarg;
	    break;
	case 'M':
	    args->logmax = atoi(optarg);
	    break;
	case 'Z':
	    args->lograw = 1;
	    break;
	case 'G':
	    args->nograb = 1;
	    break;
	case 'r':
	    args->rotation = 1;
	    args->rotation_limit = 2.0 * M_PI;
	    break;
	case 'R':
	    args->rotation = 0;
	    break;
	case 'b':
	    args->button = atoi(optarg);
	    break;
	case 'H':
	    args->hold_time_ms = atoi(optarg);
	    break;
	case 'Y':
	    args->auto_disable = 1;
	    break;
	case 'A':
	    args->auto_disable = 0;
	    break;
	case 'V':
	    args->verify = optarg;
	    break;
	case ':':
	    parse_err++;
	    break;
	case '?':
	    parse_err++;
	    break;
	default:
	    printf("Confusion: getopt_long returned %d\n", c);
	}
    }
    
    if (parse_err) {
	return -parse_err;
    } else {
	return optind;
    }
}

