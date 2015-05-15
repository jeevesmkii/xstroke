/* dir_engine.c -- recognizer that characterizes strokes by direction

   Copyright (C) 2000 Carl Worth

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
#include <errno.h>
#include <string.h>
#include <math.h>

#include "regex_feature.h"
#include "dir_engine.h"
#include "sprintf_alloc.h"
#include "fixed.h"

#ifdef DMALLOC
#include "dmalloc.h"
#endif

/* dir_engine_funcs are defined statically in rec_engine.c
rec_engine_funcs_t engine_funcs =
{
    dir_priv_alloc,
    dir_priv_free,
    dir_feature_data_alloc,
    dir_feature_data_free,
    dir_classify_stroke,
    dir_classification_str_alloc,
    dir_free_classification,
    dir_recognize_feature
};
*/

struct seg
{
    int dir;
    int length;
    int first_pt;
    int last_pt;
    struct seg *next;
};
typedef struct seg seg_t;

struct seg_list
{
    seg_t *head;
    seg_t *tail;
};
typedef struct seg_list seg_list_t;

struct seg_stroke
{
    stroke_t *stroke;
    seg_list_t segs;
};
typedef struct seg_stroke seg_stroke_t;

static int seg_stroke_init(seg_stroke_t *ss, stroke_t *stroke);
static void seg_stroke_deinit(seg_stroke_t *ss);
static void seg_stroke_resolve_ambiguities(seg_stroke_t *ss);
static int seg_stroke_merge_identical(seg_stroke_t *ss);
static int seg_stroke_merge_similar(seg_stroke_t *ss);
static void seg_stroke_find_lengths(seg_stroke_t *ss);
static char *seg_stroke_str_alloc(seg_stroke_t *ss, int include_length);

static int seg_init_from_stroke_at(seg_t *seg, stroke_t *stroke, int first, int *last_ret);
static void seg_deinit(seg_t *seg);
static int seg_dir_from_slope(int dy, int dx);
static int seg_dir_ambiguous(seg_t *seg);
static int seg_dir_adjacent(seg_t *seg, seg_t *other);

static int seg_list_init(seg_list_t *list);
static void seg_list_deinit(seg_list_t *list);
static void seg_list_append(seg_list_t *list, seg_t *seg);
static void seg_list_delete_after(seg_list_t *list, seg_t *seg);

static char dir_chars[16] = {'<', '1', '`', '3', '^', '5', '\'', '7', '>', '9', 'l', 'B', 'v', 'D', '/', 'F'};
#define DIR_MAX_WINDOW 27
static char dir_lookups[DIR_MAX_WINDOW][DIR_MAX_WINDOW] = {
    {'*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*'},
    {'*','*','*','*','*','*','*','*','*','3','3','4','4','4','4','4','5','5','*','*','*','*','*','*','*','*','*'},
    {'*','*','*','*','*','*','*','3','3','3','3','4','4','4','4','4','5','5','5','5','*','*','*','*','*','*','*'},
    {'*','*','*','*','*','2','2','3','3','3','3','3','4','4','4','5','5','5','5','5','6','6','*','*','*','*','*'},
    {'*','*','*','*','2','2','2','3','3','3','3','3','4','4','4','5','5','5','5','5','6','6','6','*','*','*','*'},
    {'*','*','*','2','2','2','2','2','3','3','3','3','4','4','4','5','5','5','5','6','6','6','6','6','*','*','*'},
    {'*','*','*','2','2','2','2','2','2','3','3','3','4','4','4','5','5','5','6','6','6','6','6','6','*','*','*'},
    {'*','*','1','1','1','2','2','2','2','3','3','3','4','4','4','5','5','5','6','6','6','6','7','7','7','*','*'},
    {'*','*','1','1','1','1','2','2','2','2','3','3','3','4','5','5','5','6','6','6','6','7','7','7','7','*','*'},
    {'*','1','1','1','1','1','1','1','2','2','2','3','3','4','5','5','6','6','6','7','7','7','7','7','7','7','*'},
    {'*','1','1','1','1','1','1','1','1','2','2','3','3','4','5','5','6','6','7','7','7','7','7','7','7','7','*'},
    {'*','0','0','1','1','1','1','1','1','1','1','.','.','.','.','.','7','7','7','7','7','7','7','7','8','8','*'},
    {'*','0','0','0','0','0','0','0','1','1','1','.','.','.','.','.','7','7','7','8','8','8','8','8','8','8','*'},
    {'*','0','0','0','0','0','0','0','0','0','0','.','.','.','.','.','8','8','8','8','8','8','8','8','8','8','*'},
    {'*','0','0','0','0','0','0','0','F','F','F','.','.','.','.','.','9','9','9','8','8','8','8','8','8','8','*'},
    {'*','0','0','F','F','F','F','F','F','F','F','.','.','.','.','.','9','9','9','9','9','9','9','9','8','8','*'},
    {'*','F','F','F','F','F','F','F','F','E','E','D','D','C','B','B','A','A','9','9','9','9','9','9','9','9','*'},
    {'*','F','F','F','F','F','F','F','E','E','E','D','D','C','B','B','A','A','A','9','9','9','9','9','9','9','*'},
    {'*','*','F','F','F','F','E','E','E','E','D','D','D','C','B','B','B','A','A','A','A','9','9','9','9','*','*'},
    {'*','*','F','F','F','E','E','E','E','D','D','D','C','C','C','B','B','B','A','A','A','A','9','9','9','*','*'},
    {'*','*','*','E','E','E','E','E','E','D','D','D','C','C','C','B','B','B','A','A','A','A','A','A','*','*','*'},
    {'*','*','*','E','E','E','E','E','D','D','D','D','C','C','C','B','B','B','B','A','A','A','A','A','*','*','*'},
    {'*','*','*','*','E','E','E','D','D','D','D','D','C','C','C','B','B','B','B','B','A','A','A','*','*','*','*'},
    {'*','*','*','*','*','E','E','D','D','D','D','D','C','C','C','B','B','B','B','B','A','A','*','*','*','*','*'},
    {'*','*','*','*','*','*','*','D','D','D','D','C','C','C','C','C','B','B','B','B','*','*','*','*','*','*','*'},
    {'*','*','*','*','*','*','*','*','*','D','D','C','C','C','C','C','B','B','*','*','*','*','*','*','*','*','*'},
    {'*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*'}
};

int dir_priv_alloc(rec_engine_t *engine)
{
    return 0;
}

void dir_priv_free(rec_engine_t *engine)
{
    /* Nothing to do here */
}

void *dir_feature_data_alloc(rec_engine_t *engine, char *feature_data_str)
{
    void *ret_val;
    char *s, *d;
    char *esc_str = malloc(strlen(feature_data_str)*2 + 1);
    if (esc_str == NULL) {
	fprintf(stderr, "%s: Out of memory\n", __FUNCTION__);
	return NULL;
    }

    /* Escape special regexp characters */
    s = feature_data_str;
    d = esc_str;
    while (*s) {
	if (*s == '^' || *s == '\\') {
	    *d++ = '\\';
	}
	*d++ = *s++;
    }
    *d++ = '\0';
    ret_val = regex_feature_alloc(esc_str);
    free(esc_str);

    return ret_val;
}

void dir_feature_data_free(rec_engine_t *engine, void *feature_data)
{
    regex_feature_free((regex_t *) feature_data);
}

/*
static int find_stroke_dir_at(stroke_t *stroke, int i)
{
    pt_t *pt_a, *pt_b;
    int dx, dy;
    int p_bits = stroke->precision_bits;
    static int dir_lookups[3][3] = {
	{3, 2, 1},
	{4, 0, 0},
	{5, 6, 7}
    };

    if (i + 1 > stroke->num_pts) {
	fprintf(stderr, "%s: ERROR: argument i=%d invalid for stroke->num_pts=%d\n",
		__FUNCTION__, i, stroke->num_pts);
	return 0;
    }

    pt_a = &stroke->pts[i];
    pt_b = &stroke->pts[i+1];
    dx = fixed_to_i_round(pt_b->x - pt_a->x, p_bits);
    dy = fixed_to_i_round(pt_b->y - pt_a->y, p_bits);

    if (abs(dx) > 1 || abs(dy) > 1) {
	fprintf(stderr, "%s: ERROR: interval from (%d, %d) - (%d, %d) has Manhattan distance > 2\n",
		__FUNCTION__,
		fixed_to_i_round(pt_a->x, p_bits),
		fixed_to_i_round(pt_a->y, p_bits),
		fixed_to_i_round(pt_b->x, p_bits),
		fixed_to_i_round(pt_b->y, p_bits));
	return 0;
    }

    return dir_lookups[dy+1][dx+1];
}
*/

void dir_classify_stroke(rec_engine_t *engine, stroke_t *stroke)
{
    seg_stroke_t seg_stroke;

    seg_stroke_init(&seg_stroke, stroke);
    seg_stroke_resolve_ambiguities(&seg_stroke);
    stroke->classifications[engine->num] = seg_stroke_str_alloc(&seg_stroke, 0);
    seg_stroke_deinit(&seg_stroke);
}

void dir_length_classify_stroke(rec_engine_t *engine, stroke_t *stroke)
{
    seg_stroke_t seg_stroke;

    seg_stroke_init(&seg_stroke, stroke);
    seg_stroke_resolve_ambiguities(&seg_stroke);
    stroke->classifications[engine->num] = seg_stroke_str_alloc(&seg_stroke, 1);
    seg_stroke_deinit(&seg_stroke);
}

int seg_stroke_init(seg_stroke_t *ss, stroke_t *stroke)
{
    int err;
    int first = 0, last = 0;
    seg_t *seg;

    ss->stroke = stroke;
    seg_list_init(&ss->segs);

    while(1) {
	/* Freed in seg_stroke_deinit */
	seg = malloc(sizeof(seg_t));
	if (seg == NULL) {
	    fprintf(stderr, "%s: out of memory\n", __FUNCTION__);
	    break;
	}
	err = seg_init_from_stroke_at(seg, stroke, first, &last);
	if (err) {
	    if (last < stroke->num_pts) {
		fprintf(stderr, "%s: An error occurred during seg_init_from_stroke_at\n",
			__FUNCTION__);
		return EINVAL;
	    } else {
		/* XXX: For now, ignore errors on the last segment. It
                   was probably just too short to register as anything
                   ambiguous. Technically, we are throwing away stroke
                   information here, which is bad. But it's probably
                   no that much information. One partial solution
                   would be to just merge these last few points in
                   with the previous segment, (that would at least
                   preserve the extra bit of length information). */
		break;
	    }
	}
	seg_list_append(&ss->segs, seg);
	if (last >= stroke->num_pts) {
	    break;
	}
	first = last;
    }

    return 0;
}

static void seg_stroke_deinit(seg_stroke_t *ss)
{
    ss->stroke = NULL;
    seg_list_deinit(&ss->segs);
}

static int seg_list_init(seg_list_t *list)
{
    list->head = NULL;
    list->tail = NULL;

    return 0;
}

static void seg_list_deinit(seg_list_t *list)
{
    seg_t *next;

    while (list->head) {
	next = list->head->next;
	/* malloced in seg_stroke_init */
	seg_deinit(list->head);
	free(list->head);
	list->head = next;
    }
    list->tail = NULL;
}


static void seg_list_append(seg_list_t *list, seg_t *seg)
{
    if (list->head == NULL) {
	list->head = seg;
    }
    if (list->tail) {
	list->tail->next = seg;
    }
    list->tail = seg;
}


static void seg_list_delete_after(seg_list_t *list, seg_t *seg)
{
    seg_t *to_delete;

    to_delete = seg->next;
    if (to_delete == NULL) {
	return;
    }

    seg->next = to_delete->next;
    if (list->tail == to_delete) {
	list->tail = seg;
    }

    seg_deinit(to_delete);
    free(to_delete);
}


static void seg_list_delete(seg_list_t *list, seg_t *seg)
{
    seg_t *prev;

    if (seg == list->head) {
	if (list->tail == list->head) {
	    list->tail = NULL;
	}
	list->head = list->head->next;
	seg_deinit(seg);
	free(seg);
    } else {
	prev = list->head;
	while (prev && prev->next != seg) {
	    prev = prev->next;
	}
	if (prev == NULL) {
	    fprintf(stderr, "%s: Error: Can't delete seg which does not appear in list.\n", __FUNCTION__);
	}
	seg_list_delete_after(list, prev);
    }
}

static void seg_stroke_resolve_ambiguities(seg_stroke_t *ss)
{
    int changes;

    do {
	changes = 0;
	changes += seg_stroke_merge_identical(ss);
	changes += seg_stroke_merge_similar(ss);
    } while (changes);
}

static int seg_stroke_merge_similar(seg_stroke_t *ss)
{
    seg_t *seg, *prev, *next;
    pt_t *pts = ss->stroke->pts;
    int changes = 0;

    seg = ss->segs.head;
    prev = NULL;
    while (seg) {
	next = seg->next;
	if (seg_dir_ambiguous(seg)) {
	    int split_to_prev = 0, split_to_next = 0;
	    if (prev && seg_dir_adjacent(seg, prev)) {
		split_to_prev = 1;
	    }
	    if (next && seg_dir_adjacent(seg, next)) {
		split_to_next = 1;
	    }
	    switch (2 * split_to_prev + split_to_next) {
	    case 0:
	    {
		/* No clues from neighbors, fall back to a closer look at slope */
		int dy = pts[seg->last_pt].y - pts[seg->first_pt].y;
		int dx = pts[seg->last_pt].x - pts[seg->first_pt].x;
		seg->dir = seg_dir_from_slope(dy, dx);
		break;
	    }
	    case 1:
		next->first_pt = seg->first_pt;
		seg_list_delete(&ss->segs, seg);
		seg = prev;
		changes++;
		break;
	    case 2:
		prev->last_pt = seg->last_pt;
		seg_list_delete(&ss->segs, seg);
		seg = prev;
		changes++;
		break;
	    case 3:
	    {
		int mid = seg->first_pt + (seg->last_pt - seg->first_pt) / 2;
		prev->last_pt = mid;
		next->first_pt = mid + 1;
		seg_list_delete(&ss->segs, seg);
		seg = prev;
		changes++;
		break;
	    }
	    }
	}
	prev = seg;
	seg = next;
    }

    return changes;
}

static int seg_stroke_merge_identical(seg_stroke_t *ss)
{
    seg_t *seg;
    int merged = 0;

    seg = ss->segs.head;
    while (seg && seg->next) {
	if (seg->dir == seg->next->dir) {
	    seg->last_pt = seg->next->last_pt;
	    seg_list_delete_after(&ss->segs, seg);
	    merged++;
	    continue;
	}
	seg = seg->next;
    }

    return merged;
}

static void seg_stroke_find_lengths(seg_stroke_t *ss)
{
    seg_t *seg;
    pt_t *pts = ss->stroke->pts;
    pt_t *pt_first, *pt_last;
    int dy, dx;
    int p_bits = ss->stroke->precision_bits;

    for (seg = ss->segs.head; seg; seg = seg->next) {
	pt_first = &pts[seg->first_pt];
	pt_last = &pts[seg->last_pt];
	dx = fixed_to_i_round(pt_last->x - pt_first->x, p_bits);
	dy = fixed_to_i_round(pt_last->y - pt_first->y, p_bits);
	seg->length = sqrt(dx*dx + dy*dy);
    }
}

static char *seg_stroke_str_alloc(seg_stroke_t *ss, int include_length)
{
    int num_segs;
    seg_t *seg;
    char *str, *p;
    int str_max;

    num_segs = 0;
    for (seg = ss->segs.head; seg; seg = seg->next) {
	num_segs++;
    }

    if (include_length) {
	seg_stroke_find_lengths(ss);
	str_max = num_segs * 7 + 1;
    } else {
	str_max = num_segs + 1;
    }
    str = malloc(str_max);
    if (str == NULL) {
	char *stroke_str;
	fprintf(stderr, "%s: out of memory (Failed to malloc %u bytes).\n",\
		__FUNCTION__, str_max);
	stroke_str = stroke_str_alloc(ss->stroke);
	fprintf(stderr, "%s: Here's the offending stroke: %s\n",
		__FUNCTION__, stroke_str);
	free(stroke_str);
	return NULL;
    }

    p = str;
    for (seg = ss->segs.head; seg; seg = seg->next) {
	*p++ = dir_chars[seg->dir];
	if (include_length) {
	    p += sprintf(p, "(%d)", seg->length);
	}
    }
    *p = '\0';

    return str;
}

static int seg_init_from_stroke_at(seg_t *seg, stroke_t *stroke, int first, int *last_ret)
{
    int last;
    int dx, dy;
    pt_t *pt_first, *pt_last;
    int p_bits = stroke->precision_bits;
    int center = DIR_MAX_WINDOW / 2;
    char dir;

    seg->next = NULL;
    seg->first_pt = first;

    pt_first = &stroke->pts[first];

    seg->dir = -1;
    for (last = first + 1; last < stroke->num_pts; last++) {
	pt_last = &stroke->pts[last];
	dx = fixed_to_i_round(pt_last->x - pt_first->x, p_bits);
	dy = fixed_to_i_round(pt_last->y - pt_first->y, p_bits);
	if (abs(dx) > center || abs(dy) > center) {
	    fprintf(stderr, "%s: Error: Distance from pts[%d] (%d, %d) -> pts[%d] (%d, %d) exceeeds %d in one or both dimensions.\n",		    __FUNCTION__,
		    first,
		    fixed_to_i_round(pt_first->x, p_bits),
		    fixed_to_i_round(pt_first->y, p_bits),
		    last,
		    fixed_to_i_round(pt_last->x, p_bits),
		    fixed_to_i_round(pt_last->y, p_bits),
		    center);
	    break;
	}
	dir = dir_lookups[center + dy][center + dx];
	if (dir == '*') {
	    break;
	} else if (dir != '.') {
	    if (dir >= '0' && dir <= '9') {
		seg->dir = dir - '0';
	    } else if (dir >= 'A' && dir <= 'F') {
		seg->dir = 10 + dir - 'A';
	    } else {
		fprintf(stderr, "%s: Unexpected character `%c' found in dir_lookups table\n", __FUNCTION__, dir);
		break;
	    }
	    if (seg_dir_ambiguous(seg) == 0) {
		last++;
		break;
	    }
	}
    }
    *last_ret = last;

    seg->last_pt = last - 1;
    if (seg->dir == -1) {
	return EINVAL;
    } else {
	return 0;
    }
}

static int seg_dir_from_slope(int dy, int dx)
{
    static double dir_thresholds[8] = {
	-M_PI + 2 * M_PI * ( 1.0) / 16.0,
	-M_PI + 2 * M_PI * ( 3.0) / 16.0,
	-M_PI + 2 * M_PI * ( 5.0) / 16.0,
	-M_PI + 2 * M_PI * ( 7.0) / 16.0,
	-M_PI + 2 * M_PI * ( 9.0) / 16.0,
	-M_PI + 2 * M_PI * (11.0) / 16.0,
	-M_PI + 2 * M_PI * (13.0) / 16.0,
	-M_PI + 2 * M_PI * (15.0) / 16.0
    };
    int d;
    double slope = atan2(dy, dx);

    for (d = 0; d < 8; d++) {
	if (dir_thresholds[d] > slope) {
	    break;
	}
    }

    return 2 * d;
}

static int seg_dir_ambiguous(seg_t *seg)
{
    return seg->dir % 2 != 0;
}

static int seg_dir_adjacent(seg_t *seg, seg_t *other)
{
    return (abs(other->dir - seg->dir) % 14) == 1;
}

static void seg_deinit(seg_t *seg)
{
    seg->dir = 0;
    seg->next = NULL;
}

void dir_classify_stroke_old(rec_engine_t *engine, stroke_t *stroke)
{
#define DIR_MAX_SLOPE_WINDOW 27
    static char dir_lookups[DIR_MAX_SLOPE_WINDOW][DIR_MAX_SLOPE_WINDOW]={
	{'`','`','`','.','.','.','.','.','.','.','.','^','^','^','^','^','.','.','.','.','.','.','.','.','\'','\'','\''},
	{'`','`','`','`','.','.','.','.','.','.','.','^','^','^','^','^','.','.','.','.','.','.','.','\'','\'','\'','\''},
	{'`','`','`','`','`','.','.','.','.','.','.','^','^','^','^','^','.','.','.','.','.','.','\'','\'','\'','\'','\''},
	{'.','`','`','`','`','.','.','.','.','.','.','.','^','^','^','.','.','.','.','.','.','.','\'','\'','\'','\'','.'},
	{'.','.','`','`','`','`','.','.','.','.','.','.','^','^','^','.','.','.','.','.','.','\'','\'','\'','\'','.','.'},
	{'.','.','.','.','`','`','`','.','.','.','.','.','^','^','^','.','.','.','.','.','\'','\'','\'','.','.','.','.'},
	{'.','.','.','.','.','`','`','`','.','.','.','.','^','^','^','.','.','.','.','\'','\'','\'','.','.','.','.','.'},
	{'.','.','.','.','.','.','`','`','.','.','.','.','.','^','.','.','.','.','.','\'','\'','.','.','.','.','.','.'},
	{'.','.','.','.','.','.','.','.','`','.','.','.','.','^','.','.','.','.','\'','.','.','.','.','.','.','.','.'},
	{'.','.','.','.','.','.','.','.','.','`','.','.','.','^','.','.','.','\'','.','.','.','.','.','.','.','.','.'},
	{'.','.','.','.','.','.','.','.','.','.','`','.','.','^','.','.','\'','.','.','.','.','.','.','.','.','.','.'},
	{'<','<','<','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','>','>','>'},
	{'<','<','<','<','<','<','<','.','.','.','.','.','.','.','.','.','.','.','.','.','>','>','>','>','>','>','>'},
	{'<','<','<','<','<','<','<','<','<','<','<','.','.','.','.','.','>','>','>','>','>','>','>','>','>','>','>'},
	{'<','<','<','<','<','<','<','.','.','.','.','.','.','.','.','.','.','.','.','.','>','>','>','>','>','>','>'},
	{'<','<','<','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','>','>','>'},
	{'.','.','.','.','.','.','.','.','.','.','/','.','.','v','.','.','l','.','.','.','.','.','.','.','.','.','.'},
	{'.','.','.','.','.','.','.','.','.','/','.','.','.','v','.','.','.','l','.','.','.','.','.','.','.','.','.'},
	{'.','.','.','.','.','.','.','.','/','.','.','.','.','v','.','.','.','.','l','.','.','.','.','.','.','.','.'},
	{'.','.','.','.','.','.','/','/','.','.','.','.','.','v','.','.','.','.','.','l','l','.','.','.','.','.','.'},
	{'.','.','.','.','.','/','/','/','.','.','.','.','v','v','v','.','.','.','.','l','l','l','.','.','.','.','.'},
	{'.','.','.','.','/','/','/','.','.','.','.','.','v','v','v','.','.','.','.','.','l','l','l','.','.','.','.'},
	{'.','.','/','/','/','/','.','.','.','.','.','.','v','v','v','.','.','.','.','.','.','l','l','l','l','.','.'},
	{'.','/','/','/','/','.','.','.','.','.','.','.','v','v','v','.','.','.','.','.','.','.','l','l','l','l','.'},
	{'/','/','/','/','/','.','.','.','.','.','.','v','v','v','v','v','.','.','.','.','.','.','l','l','l','l','l'},
	{'/','/','/','/','.','.','.','.','.','.','.','v','v','v','v','v','.','.','.','.','.','.','.','l','l','l','l'},
	{'/','/','/','.','.','.','.','.','.','.','.','v','v','v','v','v','.','.','.','.','.','.','.','.','l','l','l'},
    };
    int i, j, k;
    int dx, dy;
    char dir;
    pt_t *pt_i, *pt_j;
    int p_bits = stroke->precision_bits;
    int center = DIR_MAX_SLOPE_WINDOW / 2;
    char *seq = NULL;
    int seq_len = 0, seq_max = 0;

    for (i=0; i < stroke->num_pts; i++) {
	pt_i = &stroke->pts[i];

	j=i+1;
	while (1) {
	    if (j >= stroke->num_pts) {
		goto END_OF_SEQ;
	    }
	    pt_j = &stroke->pts[j];
	    dx = fixed_to_i_round(pt_j->x - pt_i->x, p_bits);
	    dy = fixed_to_i_round(pt_j->y - pt_i->y, p_bits);
	    if (abs(dx) > center || abs(dy) > center) {
		fprintf(stderr, "%s: Warning: slope ambiguities have forced me to the edge of my window.\n", __FUNCTION__);
		dir = '.';
		break;
	    }
	    dir = dir_lookups[center + dy][center + dx];
	    if (dir != '.') {
		break;
	    }
	    j++;
	}

	for (k=0; k < j-i; k++) {
	    seq_len++;
	    if ((seq_len+1) > seq_max) {
		seq_max += 10;
		seq = realloc(seq, seq_max);
		if (seq == NULL) {
		    fprintf(stderr, "%s: out of memory", __FUNCTION__);
		    stroke->classifications[engine->num] = NULL;
		    return; 
		}
	    }
	    seq[seq_len - 1] = dir;
	}

	i = j;
    }

  END_OF_SEQ:
    if (seq) {
	seq[seq_len] = '\0';
    }
    stroke->classifications[engine->num] = seq;
}

/*
void dir_hist_classify_stroke(rec_engine_t *engine, stroke_t *stroke)
{
    int i, j, dir, max;
    dir_priv_t *priv = (dir_priv_t *) engine->priv;
    int win_sz = priv->window_size;
    char *seq = NULL;
    int seq_max = 0;
    int seq_len = 0;
    char dir_char, prev_dir_char = '\0';
    int histogram[DIR_QUANTIZATION];
    static char dir_hist_chars[DIR_QUANTIZATION] = {'>','\'','^','`','<','/','v','l'};


    memset(histogram, 0, DIR_QUANTIZATION * sizeof(int));
    for (i=0; i < win_sz && i + 1 < stroke->num_pts; i++) {
	dir = find_stroke_dir_at(stroke, i);
	histogram[dir]++;
    }
    for ( ; (i + 1) < stroke->num_pts; i++) {
	dir = 0;
	max = histogram[dir];
	for (j=0; j < DIR_QUANTIZATION; j++) {
	    if (histogram[j] > max) {
		dir = j;
		max = histogram[dir];
	    }
	}

	dir_char = dir_hist_chars[dir];
/--	if (dir_char != prev_dir_char) { --/
	    seq_len++;
	    if ((seq_len+1) > seq_max) {
		seq_max += 10;
		seq = realloc(seq, seq_max);
		if (seq == NULL) {
		    fprintf(stderr, "%s: out of memory", __FUNCTION__);
		    stroke->classifications[engine->num] = NULL;
		    return; 
		}
	    }
	    seq[seq_len - 1] = dir_char;
	    prev_dir_char = dir_char;
/--	} --/

	dir = find_stroke_dir_at(stroke, i - win_sz);
	histogram[dir]--;
	dir = find_stroke_dir_at(stroke, i);
	histogram[dir]++;
    }
    seq[seq_len] = '\0';
    stroke->classifications[engine->num] = seq;
}
*/

/*
void dir_raw_classify_stroke(rec_engine_t *engine, stroke_t *stroke)
{
    int i;
    dir_priv_t *priv = (dir_priv_t *) engine->priv;
    int win_sz = priv->window_size;
    char *seq = NULL;
    int seq_max = 0;
    int seq_len = 0;
    pt_t *pt_a, *pt_b;
    char dir;

    for (i=0; (i + win_sz) < stroke->num_pts; i++) {
	pt_a = &stroke->pts[i];
	pt_b = &stroke->pts[i + win_sz];

	dir = find_dir(pt_a->x, pt_a->y, pt_b->x, pt_b->y, stroke->precision_bits);

	if (dir != '.') {
	    seq_len++;
	    if ((seq_len+1) > seq_max) {
		seq_max += 10;
		seq = realloc(seq, seq_max);
		if (seq == NULL) {
		    fprintf(stderr, "%s: out of memory", __FUNCTION__);
		    stroke->classifications[engine->num] = NULL;
		    return; 
		}
	    }
	    seq[seq_len - 1] = dir;
	}

	/--comment
	while ((i + 1 + win_sz) < stroke->num_pts) {
	    pt_a = &stroke->pts[i+1];
	    pt_b = &stroke->pts[i+1+win_sz];
	    if (find_dir(pt_a->x, pt_a->y,
			 pt_b->x, pt_b->y, stroke->precision_bits) == dir) {
		i++;
	    } else {
		break;
	    }
	}
	comment--/
    }
    seq[seq_len] = '\0';
    stroke->classifications[engine->num] = seq;
}
*/

char *dir_classification_str_alloc(rec_engine_t *engine, stroke_t *stroke)
{
    if (stroke->classifications[engine->num]) {
	return strdup((char *) stroke->classifications[engine->num]);
    } else {
	return strdup("");
    }
}

void dir_free_classification(rec_engine_t *engine, stroke_t *stroke)
{
    free(stroke->classifications[engine->num]);
}

double dir_recognize_stroke(rec_engine_t *engine, stroke_t *stroke,
			     void *feature_data)
{
    char *sequence = (char *) stroke->classifications[engine->num];
    regex_t *regex = (regex_t *) feature_data;

    return regex_feature_recognize(regex, sequence);
}

int dir_set_option(rec_engine_t *engine, char *name, char *value)
{
/*
    dir_priv_t *priv;

    priv = (dir_priv_t *) engine->priv;
    if (strcmp(name, "WindowSize") == 0) {
	int win_sz =  atoi(value);
	if (priv->window_size > DIR_MAX_WIN_SZ) {
	    fprintf(stderr, "%s: Warning: WindowSize %d > DIR_MAX_WIN_SZ. "
		    "Setting WindowSize to %d\n",
		    __FUNCTION__, win_sz, DIR_MAX_WIN_SZ);
	    win_sz = DIR_MAX_WIN_SZ;
	}
	priv->window_size = win_sz;
	return 0;
    }
*/
    return EINVAL;
}

/*
static char find_dir(int x1, int y1, int x2, int y2, int precision_bits)
{
    static const char dir_table[DIR_MAX_WIN_SZ*2+1][DIR_MAX_WIN_SZ*2+1] = 
    {
	{'`','`','`','^','^','^','^','^','\'','\'','\''},
	{'`','`','`','.','^','^','^','.','\'','\'','\''},
	{'`','`','`','`','^','^','^','\'','\'','\'','\''},
	{'<','.','`','`','.','^','.','\'','\'','.','>'},
	{'<','<','<','.','`','^','\'','.','>','>','>'},
	{'<','<','<','<','<','.','>','>','>','>','>'},
	{'<','<','<','.','/','v','l','.','>','>','>'},
	{'<','.','/','/','.','v','.','l','l','.','>'},
	{'/','/','/','/','v','v','v','l','l','l','l'},
	{'/','/','/','.','v','v','v','.','l','l','l'},
	{'/','/','/','v','v','v','v','v','l','l','l'}
    };

    int dx, dy;

    x1 = fixed_to_i_round(x1, precision_bits);
    y1 = fixed_to_i_round(y1, precision_bits);
    x2 = fixed_to_i_round(x2, precision_bits);
    y2 = fixed_to_i_round(y2, precision_bits);

    dx = x2 - x1;
    dy = y2 - y1;

    if (abs(dx) > DIR_MAX_WIN_SZ || abs(dy) > DIR_MAX_WIN_SZ) {
	fprintf(stderr,
		"%s: Error: Points (%d, %d) - (%d, %d) are too distant.\n"
		"%s:        (ie. greater than %d in one or both dimensions)\n",
		__FUNCTION__, x1, y1, x2, y2, __FUNCTION__, DIR_MAX_WIN_SZ);
	return '.';
    }

    return dir_table[DIR_MAX_WIN_SZ + dy][DIR_MAX_WIN_SZ + dx];
}
*/

