/* log.c -- logging facilities for xstroke

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
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

#include "xstroke.h"
#include "args.h"
#include "log.h"

#ifdef DMALLOC
#include "dmalloc.h"
#endif

int log_init(log_t *log, char *filename, int threshold)
{
    int err;
    struct stat stat_buf;
    char *dir;
    char *p;

    log->file = NULL;
    log->cnt = 0;
    log->threshold_min = threshold;
    log->threshold_max = (int) (1.2 * threshold);

    log->filename = strdup(filename);
    dir = strdup(filename);
    if (log->filename == NULL || dir == NULL) {
	fprintf(stderr, "%s: out of memory.\n", __FUNCTION__);
	return ENOMEM;
    }

    strcpy(dir, filename);
    p = strrchr(dir, '/');
    if (p) {
	*p = '\0';
	err = stat(dir, &stat_buf);
	if (err) {
	    if (errno == ENOENT) {
		err = mkdir(dir,
			    S_IRUSR | S_IWUSR | S_IXUSR
			    | S_IRGRP | S_IXUSR
			    | S_IROTH | S_IXOTH);
		if (err) {
		    fprintf(stderr, "%s: Warning: Failed to mkdir %s: %s\n",
			    __FUNCTION__, dir, strerror(errno));
		    goto NO_LOG;
		}
	    } else {
		fprintf(stderr,
			"%s: Warning: An error occurred during stat of %s: %s\n",
			__FUNCTION__, dir, strerror(errno));
		goto NO_LOG;
	    }
	} else {
	    if (! S_ISDIR(stat_buf.st_mode)) {
		fprintf(stderr,
			"%s: Warning: %s is not a directory\n",
			__FUNCTION__, dir);
		goto NO_LOG;
	    }
	}
    }
    
    log->file = fopen(filename, "w+");
    if (log->file == NULL) {
	fprintf(stderr, "%s: Warning: failed to open %s: %s\n",
		__FUNCTION__, filename, strerror(errno));
	goto NO_LOG;
    }

    free(dir);
    return 0;

  NO_LOG:
    free(dir);
    fprintf(stderr, "%s: Warning: logging to %s will be disabled\n",
	    __FUNCTION__, filename);
    return 0; /* Not a real error, we just won't do logging */
}

void log_deinit(log_t *log)
{
    log->cnt = 0;
    if (log->file) {
	fclose(log->file);
    }
    if (log->filename) {
	free(log->filename);
	log->filename = NULL;
    }
}

#define LOG_BUF_SIZE 1024
int log_msg(log_t *log, const char *msg)
{
    char buf[LOG_BUF_SIZE];
    int discard;

    if (log->file == NULL) {
	return EINVAL;
    }

    fputs(msg, log->file);
    fflush(log->file);

    log->cnt++;
    if (log->cnt > log->threshold_max) {
	FILE *old_file;
	char *tmp_filename;

	fclose(log->file);
	log->file = NULL;

	old_file = fopen(log->filename, "r");
	if (old_file == NULL) {
	    fprintf(stderr, "%s: Warning: failed to open %s for reading: %s\n",
		    __FUNCTION__, log->filename, strerror(errno));
	    fprintf(stderr, "%s: Warning: logging to %s will now be disabled.\n",
		    __FUNCTION__, log->filename);
	    return 0; /* Not fatal, message was already logged */
	}

	tmp_filename = malloc(strlen(log->filename)+5);
	if (tmp_filename == NULL) {
	    fprintf(stderr, "%s: out of memory\n", __FUNCTION__);
	    return 0; /* Not fatal, message was already logged */
	}
	sprintf(tmp_filename, "%s.new", log->filename);

	log->file = fopen(tmp_filename, "w+");
	if (log->file) {
	    discard = 0;
	    while (discard < (log->cnt - log->threshold_min)) {
		if (fgets(buf, LOG_BUF_SIZE, old_file) == NULL)
		    break;
		if (buf[strlen(buf)-1] == '\n') {
		    discard++;
		}
	    }
	    log->cnt = 0;
	    while (fgets(buf, LOG_BUF_SIZE, old_file)) {
		fputs(buf, log->file);
		if (buf[strlen(buf)-1] == '\n') {
		    log->cnt++;
		}
	    }
	    fflush(log->file);
	}
	fclose(old_file);
	if (rename(tmp_filename, log->filename)) {
	    fprintf(stderr, "%s: Warning: error renaming %s to %s: %s\n",
		    __FUNCTION__, tmp_filename, log->filename,
		    strerror(errno));
	}
    }
    return 0;
}

