#pragma once

#include <vulkan/vulkan_core.h>

extern VkImageView *views;

void init_views();
void free_views();