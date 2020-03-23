#include "engine.h"
#include "utils.h"

int main(int argc, char **argv) {
	init_engine(argc, argv);
	run_engine();
	free_engine();
}