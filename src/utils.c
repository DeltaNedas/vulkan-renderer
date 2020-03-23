#include "utils.h"

#include "engine.h"
#include "log.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void quit(int code) {
	if (!engine.freed) {
		printl(DEBUG, "Cleaning up before shutting down...");
		engine.freed = TRUE;
		free_engine();
	}
	exit(code);
}

nanos_t timestamp() {
	struct timespec spec;
	clock_gettime(CLOCK_REALTIME, &spec);

	return spec.tv_sec * 1e9 + spec.tv_nsec;
}

char *timestamp_strft(const char *fmt, int len, nanos_t at) {
	time_t time = (time_t) (at / 1e9);
	struct tm info;
	char* ret = (char*) malloc(sizeof(char) * len);

	if (!gmtime_r(&time, &info)) {
		fprintf(stderr, "[ERROR] Failed to fill time info for %llu\n", at);
		return NULL;
	}

	fmt = fmt ? fmt : TIMESTAMP_FORMAT;
	if (!strftime(ret, len, fmt, &info)) {
		fprintf(stderr, "[ERROR] Failed to format time for \"%s\" with %llu\n", fmt, at);
		return NULL;
	}
	return ret;
}

char *timestamp_strt(nanos_t at) {
	return timestamp_strft(NULL, TIMESTAMP_LEN, at);
}

char *timestamp_strf(const char *fmt, int len) {
	return timestamp_strft(fmt, len, timestamp());
}

char *timestamp_str() {
	return timestamp_strt(timestamp());
}

char *strdup(const char *str) {
	int len = strlen(str) + 1;
	char *new = malloc(len);
	if (new) {
		memcpy(new, str, len);
	}
	return new;
}