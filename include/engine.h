#pragma once

#include "types.h"

#include <stdio.h>

typedef struct {
	int argc;
	char **argv;
	/* File to write INFO+ to. Set to NULL to disable logging to file.
	   Default is <time>.log */
	FILE *log;
	bool running, freed;
} engine_t;

extern engine_t engine;

/* Allocate memory and parse command-line arguments.
   Then initialise Vulkan, etc. */
void init_engine(int argc, char **argv);
void parse_args();
/* create any extra threads and enter render loop. */
void run_engine();
/* Free all memory allocated for engine */
void free_engine();