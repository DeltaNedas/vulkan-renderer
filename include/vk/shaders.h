#pragma once

#include <vulkan/vulkan_core.h>

/* TODO: Find a more permanent solution with an array of shaders */
#define SHADER_COUNT 2

extern VkShaderModule vert, frag;
extern VkPipelineShaderStageCreateInfo *shaders;
extern VkPipelineVertexInputStateCreateInfo vertex_input;
extern VkPipelineInputAssemblyStateCreateInfo input_assembly;

void init_shaders();
/* Called at the end of init_pipeline() */
void free_shaders();