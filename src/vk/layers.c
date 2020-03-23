#include "vk/layers.h"

#include "vk/errors.h"

char **layers;
unsigned layer_count;

void print_layers(level_t level, VkLayerProperties *props, unsigned count) {
	if (!props) {
		vkEnumerateInstanceLayerProperties(&count, NULL);
		props = (VkLayerProperties*) malloc(sizeof(VkLayerProperties) * count);
		vkEnumerateInstanceLayerProperties(&count, props);
	}

	printl(level, "The following %u Vulkan validation layers are supported:", count);
	for (unsigned i = 0; i < count; i++) {
		printl(level, "\t%s", props[i].layerName);
	}
	free(props);
}

void init_layers() {
	char *req[] = {
		"VK_LAYER_LUNARG_core_validation"
	};
	layer_count = sizeof(req) / sizeof(char*);
	unsigned avail;
	char *layer;

	VkLayerProperties *props;
	vkEnumerateInstanceLayerProperties(&avail, NULL);
	if (!(props = (VkLayerProperties*) malloc(sizeof(VkLayerProperties) * avail))) {
		error("Failed to allocate memory for %u validation layer properties: %s", avail, strerror(errno));
	}
	vkEnumerateInstanceLayerProperties(&avail, props);

	for (unsigned i = 0; i < layer_count; i++) {
		layer = req[i];
		for (unsigned j = 0; j < avail; j++) {
			if (!strcmp(layer, props[j].layerName)) {
				goto found_it;
			}
		}

		printl(FATAL, "Unsupported validation layer '%s'", layer);
		print_layers(WARN, props, avail);
		quit(1);
	found_it:
		continue;
	}
	printl(INFO, "Using %u out of %u available validation layers", layer_count, avail);
	layers = malloc(sizeof(req));
	memcpy(layers, req, sizeof(req));
	free(props);
}

void free_layers() {
	freeptr(layers);
}