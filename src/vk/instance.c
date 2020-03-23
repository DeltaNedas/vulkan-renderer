#include "vk/instance.h"

#include "options.h"
#include "vk/debug.h"
#include "vk/errors.h"
#include "vk/extensions.h"
#include "vk/layers.h"
#include "vk/surface.h"

VkInstance instance;
char **exts = NULL;
unsigned ext_count, exts_required;

void print_extensions(level_t level, VkExtensionProperties *props, unsigned count) {
	if (!props) {
		vkEnumerateInstanceExtensionProperties(NULL, &count, NULL);
		props = (VkExtensionProperties*) malloc(sizeof(VkExtensionProperties) * count);
		vkEnumerateInstanceExtensionProperties(NULL, &count, props);
	}

	printl(level, "The following %u Vulkan instance extensions are supported:", count);
	for (unsigned i = 0; i < count; i++) {
		printl(level, "\t%s", props[i].extensionName);
	}
	free(props);
}

void init_extensions() {
	char *requested[] = {
		VK_EXT_DEBUG_UTILS_EXTENSION_NAME
	};
	unsigned exts_requested = sizeof(requested) / sizeof(char*);
	ext_count = exts_required + exts_requested;
	/* Allocate enough memory to fit required and requested extension names in one buffer */
	if (!(exts = (char**) malloc(sizeof(char*) * ext_count))) {
		error("Failed to allocate memory for %d extension names: %s", ext_count, strerror(errno));
	}

	for (unsigned i = exts_required; i < ext_count; i++) {
		exts[i] = requested[i - exts_required];
	}
}

void free_extensions() {
	freeptr(exts);
}

void init_instance() {
	print_extensions(DEBUG, NULL, 0);
	print_layers(DEBUG, NULL, 0);

	VkApplicationInfo app_info = {
		.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pApplicationName = window_title,
		.applicationVersion = VK_MAKE_VERSION(1, 0, 0),
		.pEngineName = "Vulkan Renderer",
		.engineVersion = VK_MAKE_VERSION(1, 0, 0),
		.apiVersion = VK_API_VERSION_1_0
	};

	VkInstanceCreateInfo info = {
		.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		.pApplicationInfo = &app_info,
		.enabledLayerCount = layer_count,
		.ppEnabledLayerNames = (const char**) layers,
		.enabledExtensionCount = ext_count,
		.ppEnabledExtensionNames = (const char**) exts,
		.pNext = &debugging_info
	};
	printl(INFO, "Using %u validation layers and %u extensions", layer_count, ext_count);

	/* Create instance */
	VkResult res = vkCreateInstance(&info, NULL, &instance);
	if (res != VK_SUCCESS) {
		error("Failed to create VkInstance: %s (%d)", vk_error(res), (int) res);
	}
}

void free_instance() {
	if (instance != VK_NULL_HANDLE) {
		vkDestroyInstance(instance, NULL);
	}
}