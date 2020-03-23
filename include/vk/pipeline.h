#pragma once

#include <vulkan/vulkan_core.h>

typedef struct {
	VkShaderModule vert, frag;

	VkViewport viewport;
	VkRect2D scissor;
	VkRenderPass pass;
	VkPipelineLayout layout;
	VkPipeline pipeline;

	/* Various info structs  */
	VkPipelineShaderStageCreateInfo *shaders;
	VkPipelineVertexInputStateCreateInfo vertex_input;
	VkPipelineInputAssemblyStateCreateInfo input_assembly;
} pipeline_t;

void init_pipeline();
void free_pipeline();