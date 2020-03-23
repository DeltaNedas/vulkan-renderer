#include "vk/passes.h"

#include "vk/devices.h"
#include "vk/errors.h"
#include "vk/swapchain.h"

VkRenderPass *passes = NULL;
unsigned pass_count = 0;

void init_passes() {
	VkResult res;
	pass_count = 1;
	if (!(passes = malloc(sizeof(VkRenderPass) * pass_count))) {
		error("Failed to allocate memory for %u render passes: %s", pass_count, strerror(errno));
	}

	VkAttachmentDescription attachment = {
		.format = swapchain.format.format,
		.samples = VK_SAMPLE_COUNT_1_BIT,
		.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
		.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
		/* Stencil isn't used */
		.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
		/* Previous image is cleared, see loadOp, so do not preserve it */
		.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
		.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
	};

	/* Used in fragment shader, 'location = 0' */
	VkAttachmentReference ref = {
		.attachment = 0,
		.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
	};

	VkSubpassDescription subpass = {
		.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
		.colorAttachmentCount = 1,
		.pColorAttachments = &ref
	};

	VkRenderPassCreateInfo info = {
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
		.attachmentCount = 1,
		.pAttachments = &attachment,
		.subpassCount = 1,
		.pSubpasses = &subpass
	};

	/* So far, this is the only pass created */
	if ((res = vkCreateRenderPass(devices.dev, &info, NULL, passes)) != VK_SUCCESS) {
		error("Failed to create render pass: %s (%d)", vk_error(res), (int) res);
	}
}

void free_passes() {
	for (int i = 0; i < pass_count; i++) {
		vkDestroyRenderPass(devices.dev, passes[i], NULL);
	}
}