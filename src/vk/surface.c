#include "vk/surface.h"

#include "log.h"
#include "utils.h"
#include "vk/instance.h"
#include "window.h"

VkSurfaceKHR surface = VK_NULL_HANDLE;

void init_surface() {
#ifdef SUPPORT_SDL
	if (!SDL_Vulkan_CreateSurface(window, instance, &surface)) {
		error("Failed to create surface with SDL: %s", SDL_GetError());
	}
#endif
}

void free_surface() {
	if (surface != VK_NULL_HANDLE) {
		vkDestroySurfaceKHR(instance, surface, NULL);
	}
}