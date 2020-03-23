#pragma once

#include "log.h"

#include <vulkan/vulkan_core.h>

extern char **layers;
extern unsigned layer_count;

void print_layers(level_t level, VkLayerProperties *props, unsigned count);
void init_layers();
void free_layers();