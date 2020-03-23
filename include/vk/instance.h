#pragma once

#include "log.h"

#include <vulkan/vulkan_core.h>

/* The Vulkan instance itself. */
extern VkInstance instance;
extern char **exts;
extern unsigned ext_count, exts_required;

void print_extensions(level_t level, VkExtensionProperties *props, unsigned count);
void init_instance();
void free_instance();

void init_extensions();
void free_extensions();