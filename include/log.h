#pragma once

#include <stdarg.h>
#include <stdio.h>

typedef enum level_t {
	DEBUG,
	/* INFO+ are written to log file */
	INFO,
	/* WARN+ are printed to stderr */
	WARN,
	ERROR,
	FATAL
} level_t;

/* Terminal colour code for a log level */
const char *level_colour(level_t level);
const char *level_str(level_t level);

void fprintl(level_t level, FILE *f, const char *fmt, va_list args);
/* Print some text to log file and output.
   Output is stderr for levels above INFO.
   Text is prefixed with "[LEVEL] (TIME): "*/
void printl(level_t level, const char *fmt, ...);

char *logname();

/* Switch log file to a path.
   Disables logging if path is NULL */
void set_log(char *path);