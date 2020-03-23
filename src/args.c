#include "engine.h"

#include "log.h"
#include "options.h"
#include "utils.h"

#include <errno.h>
#include <string.h>
#include <stdlib.h>

void parse_num(int i, int *ptr, char *arg) {
	if (i >= engine.argc) {
		error("-%s requires a number as an argument", arg);
	}

	errno = 0;
	*ptr = atoi(engine.argv[i]);
	if (errno) {
		error("Argument #d '%s' is not a number", i, engine.argv[i]);
	}
}

void parse_str(int i, char **str, char *arg) {
	if (i >= engine.argc) {
		error("-%s requires a window title as an argument", arg);
	}
	*str = engine.argv[i];
}

void parse_args() {
	char *arg;

	for (int i = 1; i < engine.argc; i++) {
		arg = engine.argv[i];
		if (arg[0] == '-') {
			arg++;
			if (!strcmp(arg, "l") || !strcmp(arg, "-log")) {
				if (i + 1 >= engine.argc) {
					error("-%s requires a log path as an argument", arg);
				}
				log_path = strdup(engine.argv[++i]);
				continue;
			} else if (!strcmp(arg, "w") || !strcmp(arg, "-width")) {
				parse_num(++i, &window_x, arg);
				continue;
			} else if (!strcmp(arg, "h") || !strcmp(arg, "-height")) {
				parse_num(++i, &window_y, arg);
				continue;
			} else if (!strcmp(arg, "t") || !strcmp(arg, "-title")) {
				parse_str(++i, &window_title, arg);
				continue;
			} else if (arg[1] == '\0') {
				break;
			}
		}
		error("Unexpected argument #%d '%s'", i, arg - 1);
	}

	/* If log path isnt provided, default to time-based name */
	if (!log_path) {
		if (!(log_path = logname())) {
			quit(errno);
		}
	}

	set_log(log_path);
	free(log_path);
}