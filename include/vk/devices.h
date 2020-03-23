#pragma once

#include <vulkan/vulkan_core.h>

/* Require at least Vulkan 1.0.X for a physical device to be considered valid */
#define MAJOR 1
#define MINOR 0

#define GRAPHICS 0
#define PRESENT 1

typedef struct {
	int index;
	VkQueueFamilyProperties props;
	VkQueue queue;
} family_t;

typedef struct {
	/* Extensions are separate from instance extensions.
	   See extensions.c for instance extensions. */
	char **layers, **exts;
	unsigned layer_count, ext_count;

	family_t *families;
	unsigned family_count, unique_family_count;

	VkPhysicalDevice pd;
	VkPhysicalDeviceProperties pd_props;
	VkPhysicalDeviceFeatures pd_feats;
	VkDevice dev;
} devices_t;
extern devices_t devices;

void init_devices();
void free_devices();