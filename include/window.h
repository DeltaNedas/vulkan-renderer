#pragma once

#ifdef SUPPORT_SDL
#	include <SDL.h>
#	include <SDL_vulkan.h>
#else
#	error At least one window backend must be supported!
#endif

#ifdef SUPPORT_SDL
extern SDL_Window *window;
#endif

void init_window();
void free_window();