#include "vk/views.h"

#include "vk/devices.h"
#include "vk/errors.h"
#include "vk/swapchain.h"

VkImageView *views = NULL;

void init_views() {
	if (!(views = malloc(sizeof(VkImageView) * swapchain.image_count))) {
		error("Failed to allocate memory for %d swapchain image views: %s", swapchain.image_count, strerror(errno));
	}

	VkResult res;
	for (int i = 0; i < swapchain.image_count; i++) {
		VkImageViewCreateInfo info = {
			.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			.image = swapchain.images[i],
			.viewType = VK_IMAGE_VIEW_TYPE_2D,
			.format = swapchain.format.format,
			.components.r = VK_COMPONENT_SWIZZLE_IDENTITY,
			.components.g = VK_COMPONENT_SWIZZLE_IDENTITY,
			.components.b = VK_COMPONENT_SWIZZLE_IDENTITY,
			.components.a = VK_COMPONENT_SWIZZLE_IDENTITY,
			.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
			.subresourceRange.baseMipLevel = 0,
			.subresourceRange.levelCount = 1,
			.subresourceRange.baseArrayLayer = 0,
			.subresourceRange.layerCount = 1,
		};

		if ((res = vkCreateImageView(devices.dev, &info, NULL, &views[i])) != VK_SUCCESS) {
			error("Failed to create image view #%u: %s (%d)", i + 1, vk_error(res), (int) res);
		}
	}
}

void free_views() {
	if (views) {
		for (int i = 0 ; i < swapchain.image_count; i++) {
			vkDestroyImageView(devices.dev, views[i], NULL);
		}
		free(views);
	}
}