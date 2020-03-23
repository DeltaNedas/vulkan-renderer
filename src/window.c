#include "window.h"

#include "log.h"
#include "options.h"
#include "utils.h"
#include "vk/instance.h"

#ifdef SUPPORT_SDL
#	define BACKEND "SDL"
#	define POS_UNDEF SDL_WINDOWPOS_UNDEFINED
#endif

#ifdef SUPPORT_SDL
SDL_Window *window = NULL;
#endif

void init_window() {
	printl(INFO, "Initialising window with " BACKEND);
#ifdef SUPPORT_SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		error("Failed to initialise SDL: %s", SDL_GetError());
	}

	if (!(window = SDL_CreateWindow(window_title, POS_UNDEF, POS_UNDEF,
			window_x, window_y, SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN))) {
		error("Failed to create %dx%d SDL Window: %s", window_x, window_y, SDL_GetError());
	}

	/* Initialise required and requested extensions */
	if (SDL_Vulkan_GetInstanceExtensions(window, &exts_required, NULL) == SDL_FALSE) {
		error("Failed to get instance extension count of SDL: %s", SDL_GetError());
	}
#endif
	init_extensions();
#ifdef SUPPORT_SDL
	if (SDL_Vulkan_GetInstanceExtensions(window, &exts_required, (const char**) exts) == SDL_FALSE) {
		error("Failed to get %u instance extensions of SDL: %s", exts_required, SDL_GetError());
	}
#endif
}

void free_window() {
#ifdef SUPPORT_SDL
	if (window) {
		SDL_DestroyWindow(window);
	}
	SDL_QuitSubSystem(SDL_INIT_VIDEO);
	SDL_Quit();
#endif
}