#include "engine.h"

#include "log.h"
#include "vk/debug.h"
#include "vk/devices.h"
#include "vk/extensions.h"
#include "vk/instance.h"
#include "vk/layers.h"
#include "vk/passes.h"
#include "vk/pipeline.h"
#include "vk/surface.h"
#include "vk/swapchain.h"
#include "vk/views.h"
#include "window.h"

engine_t engine;

void init_engine(int argc, char **argv) {
	engine.argc = argc;
	engine.argv = argv;
	engine.running = FALSE;
	engine.freed = FALSE;

	parse_args();
	printl(INFO, "Initialising engine...");
	init_window();
	init_layers();
	init_instance();
	init_debugging();
	init_surface();
	init_devices();
	create_swapchain();
	init_views();
	init_passes();
	init_pipeline();
}

void run_engine() {
	printl(INFO, "Running engine...");
	while (engine.running) {
		engine.running = FALSE;
	}
	printl(INFO, "Engine shutting down...");
}

void free_engine() {
	free_pipeline();
	free_passes();
	free_views();
	free_swapchain();
	free_devices();
	free_debugging();
	free_surface();
	free_instance();
	free_layers();
	free_extensions();
	free_window();
	set_log(NULL);
}