#include "vk/pipeline.h"

#include "vk/devices.h"
#include "vk/errors.h"
#include "vk/shaders.h"
#include "vk/swapchain.h"

pipeline_t pipeline;

VkPipelineViewportStateCreateInfo init_viewport() {
	pipeline.viewport = (VkViewport) {
		.x = 0.0,
		.y = 0.0,
		.width = swapchain.extent.width,
		.height = swapchain.extent.width,
		.minDepth = 0.0,
		.maxDepth = 1.0
	};

	pipeline.scissor = (VkRect2D) {
		.offset = {0, 0},
		.extent = swapchain.extent
	};
	return (VkPipelineViewportStateCreateInfo) {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
		.viewportCount = 1,
		.pViewports = &pipeline.viewport,
		.scissorCount = 1,
		.pScissors = &pipeline.scissor
	};
}

VkPipelineRasterizationStateCreateInfo init_rasterizer() {
	return (VkPipelineRasterizationStateCreateInfo) {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
		.depthClampEnable = VK_FALSE,
		/* Never turn on or it will not render any pixels */
		.rasterizerDiscardEnable = VK_FALSE,
		.polygonMode = VK_POLYGON_MODE_FILL,
		.lineWidth = 1.0,
		.cullMode = VK_CULL_MODE_BACK_BIT,
		.frontFace = VK_FRONT_FACE_CLOCKWISE,
		.depthBiasEnable = VK_FALSE,
		.depthBiasConstantFactor = 0.0,
		.depthBiasClamp = 0.0,
		.depthBiasSlopeFactor = 0.0
	};
}

VkPipelineMultisampleStateCreateInfo init_multisampling() {
	return (VkPipelineMultisampleStateCreateInfo) {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
		.sampleShadingEnable = VK_FALSE,
		.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT/*,
		.minSampleShading = 1.0,
		.pSampleMask = NULL,
		.alphaToCoverageEnable = VK_FALSE,
		.alphaToOneEnable = VK_FALSE*/
	};
}

VkPipelineColorBlendStateCreateInfo init_blending(VkPipelineColorBlendAttachmentState *attachment) {
	*attachment = (VkPipelineColorBlendAttachmentState) {
		.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
		.blendEnable = VK_FALSE,
		.srcColorBlendFactor = VK_BLEND_FACTOR_ONE,
		.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO,
		.colorBlendOp = VK_BLEND_OP_ADD,
		.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
		.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
		.alphaBlendOp = VK_BLEND_OP_ADD
	};
	return (VkPipelineColorBlendStateCreateInfo) {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
		.logicOpEnable = VK_FALSE,
		.logicOp = VK_LOGIC_OP_COPY,
		.attachmentCount = 1,
		.pAttachments = attachment,
		.blendConstants[0] = 0.0,
		.blendConstants[1] = 0.0,
		.blendConstants[2] = 0.0,
		.blendConstants[3] = 0.0
	};
}

VkPipelineDynamicStateCreateInfo init_state() {
	return (VkPipelineDynamicStateCreateInfo) {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
		.dynamicStateCount = 0,
		.pDynamicStates = (VkDynamicState[]) {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_LINE_WIDTH
		}
	};
}

void init_layout() {
	VkResult res;
	VkPipelineLayoutCreateInfo info = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		.setLayoutCount = 0,
		.pSetLayouts = NULL,
		.pushConstantRangeCount = 0,
		.pPushConstantRanges = NULL
	};

	if ((res = vkCreatePipelineLayout(devices.dev, &info, NULL, &pipeline.layout)) != VK_SUCCESS) {
		error("Failed to create pipeline layout: %s (%d)", vk_error(res), (int) res);
	}
}

void init_pipeline() {
	VkResult res;
	init_shaders();
	pipeline.pipeline = VK_NULL_HANDLE;
	VkPipelineViewportStateCreateInfo viewport = init_viewport();
	VkPipelineRasterizationStateCreateInfo rasterizer = init_rasterizer();
	VkPipelineMultisampleStateCreateInfo multisampling = init_multisampling();
	VkPipelineColorBlendAttachmentState blend_attachment;
	VkPipelineColorBlendStateCreateInfo blending = init_blending(&blend_attachment);
	//VkPipelineDynamicStateCreateInfo state = init_state();

	init_layout();

	VkGraphicsPipelineCreateInfo info = {
		.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
		.stageCount = 2,
		.pStages = pipeline.shaders,
		.pVertexInputState = &pipeline.vertex_input,
		.pInputAssemblyState = &pipeline.input_assembly,
		.pViewportState = &viewport,
		.pRasterizationState = &rasterizer,
		.pMultisampleState = &multisampling,
		.pColorBlendState = &blending,
		//.pDynamicState = &state,
		.layout = pipeline.layout,
		.renderPass = pipeline.pass,
		.subpass = 0,
		/* Use if extending an existing pipeline */
		.basePipelineHandle = VK_NULL_HANDLE,
		.basePipelineIndex = -1
	};

	if ((res = vkCreateGraphicsPipelines(devices.dev, VK_NULL_HANDLE, 1, &info, NULL, &pipeline.pipeline)) != VK_SUCCESS) {
		error("Failed to create graphics pipeline: %s (%d)", vk_error(res), (int) res);
	}

	free_shaders();
}

void free_pipeline() {
	if (pipeline.pipeline != VK_NULL_HANDLE) {
		vkDestroyPipeline(devices.dev, pipeline.pipeline, NULL);
	}
	if (pipeline.layout != VK_NULL_HANDLE) {
		vkDestroyPipelineLayout(devices.dev, pipeline.layout, NULL);
	}
}