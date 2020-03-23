#pragma once

#include "types.h"

#include <vulkan/vulkan_core.h>

typedef struct {
	VkSwapchainKHR swapchain;
	VkExtent2D extent;
	VkSurfaceFormatKHR format;
	VkPresentModeKHR mode;

	/* Temporary data for testing physical device validity */
	VkSurfaceCapabilitiesKHR caps;
	VkSurfaceFormatKHR *formats;
	VkPresentModeKHR *modes;
	unsigned format_count, mode_count;

	VkImage *images;
	unsigned image_count;
} swapchain_t;
extern swapchain_t swapchain;

bool valid_swapchain();
/* Not init, because it can be called multiple times.
   TODO: Test this and recreate the swapchain. */
void create_swapchain();
void free_swapchain();