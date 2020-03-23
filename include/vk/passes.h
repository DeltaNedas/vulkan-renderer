#pragma once

#include <vulkan/vulkan_core.h>

extern VkRenderPass *passes;
extern unsigned pass_count;

void init_passes();
void free_passes();