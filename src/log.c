#include "log.h"

#include "engine.h"
#include "utils.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>

const char *level_colour(level_t level) {
	switch (level) {
		case DEBUG: return "\x1B[32m";
		case INFO: return "\x1B[33m";
		case WARN: return "\x1B[93m";
		case ERROR: return "\x1B[1;31m";
		default: return "\x1B[1;31;7m";
	}
}

const char *level_str(level_t level) {
	switch (level) {
		case DEBUG: return "DEBUG";
		case INFO: return "INFO";
		case WARN: return "WARN";
		case ERROR: return "ERROR";
		default: return "FATAL";
	}
}

void fprintl(level_t level, FILE *f, const char *fmt, va_list args) {
	char *time = timestamp_strf("%H:%M:%S", TIMESTAMP_LEN);
	if (f == stdout || f == stderr) {
		fprintf(f, "[%s%s\x1B[0m] (\x1B[36m%s\x1B[0m) ",
			level_colour(level),
			level_str(level),
			time);
	} else {
		fprintf(f, "[%s] (%s) ",
			level_str(level),
			time);
	}
	free(time);

	vfprintf(f, fmt, args);
	fputc('\n', f);
}

void printl(level_t level, const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	fprintl(level, level > INFO ? stderr : stdout, fmt, args);
	if (engine.log && level > DEBUG) {
		va_end(args);
		va_start(args, fmt);
		fprintl(level, engine.log, fmt, args);
	}
	va_end(args);
}

char *logname() {
	return timestamp_strf("%Y-%m-%d %H.%M.%S.log", TIMESTAMP_LEN);
}

void set_log(char *path) {
	printl(INFO, "Set log path to %s", path);
	if (engine.log) {
		assert(fclose(engine.log));
	}

	if (path) {
		if ((engine.log = fopen(path, "w")) == NULL) {
			fprintf(stderr, "[ERROR] Failed to open %s for log: %s\n", path, strerror(errno));
			exit(errno);
		}
	} else {
		engine.log = NULL;
	}
}