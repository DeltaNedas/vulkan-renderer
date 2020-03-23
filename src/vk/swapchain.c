#include "vk/swapchain.h"

#include "options.h"
#include "vk/devices.h"
#include "vk/errors.h"
#include "vk/surface.h"

#include <stdint.h>

#ifdef WINDOW_USE_ALPHA
#	define WINDOW_ALPHA VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR
#else
#	define WINDOW_ALPHA VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR
#endif

swapchain_t swapchain;

bool valid_swapchain(VkPhysicalDevice dev) {
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(dev, surface, &swapchain.caps);

	unsigned old_count = swapchain.format_count;
	vkGetPhysicalDeviceSurfaceFormatsKHR(dev, surface, &swapchain.format_count, NULL);
	if (!swapchain.format_count) {
		return FALSE;
	}
	if (old_count != swapchain.format_count) {
		free(swapchain.formats);
		if (!(swapchain.formats = malloc(sizeof(VkSurfaceFormatKHR) * swapchain.format_count))) {
			printl(ERROR, "Failed to allocate memory for %u device swapchain formats: %s", swapchain.format_count, strerror(errno));
			return FALSE;
		}
	}
	vkGetPhysicalDeviceSurfaceFormatsKHR(dev, surface, &swapchain.format_count, swapchain.formats);

	old_count = swapchain.mode_count;
	vkGetPhysicalDeviceSurfacePresentModesKHR(dev, surface, &swapchain.mode_count, NULL);
	if (!swapchain.mode_count) {
		return FALSE;
	}
	if (old_count != swapchain.mode_count) {
		free(swapchain.modes);
		if (!(swapchain.modes = malloc(sizeof(VkPresentModeKHR) * swapchain.mode_count))) {
			printl(ERROR, "Failed to allocate memory for %u device swapchain present modes: %s", swapchain.mode_count, strerror(errno));
			return FALSE;
		}
	}
	vkGetPhysicalDeviceSurfacePresentModesKHR(dev, surface, &swapchain.mode_count, swapchain.modes);
	return TRUE;
}

int min_image_count() {
	int max = swapchain.caps.maxImageCount,
		n = swapchain.caps.minImageCount + 1;
	if (max > 0 && n > max) {
		n = max;
	}
	return n;
}

VkSurfaceFormatKHR select_format() {
	VkSurfaceFormatKHR format;
	for (unsigned i = 0; i < swapchain.format_count; i++) {
		format = swapchain.formats[i];
		/* Prefer 32 bit BGRA SRGB */
		if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			return format;
		}
	}

	/* Fall back to first format */
	return swapchain.formats[0];
}

VkPresentModeKHR select_mode() {
	VkPresentModeKHR mode;
	for (unsigned i = 0; i < swapchain.mode_count; i++) {
		switch (mode = swapchain.modes[i]) {
		/* prefer triple-buffer */
		case VK_PRESENT_MODE_MAILBOX_KHR:
			return mode;
		default: break;
		}
	}

	/* Fall back to slow vsync */
	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D select_extent() {
	VkSurfaceCapabilitiesKHR caps = swapchain.caps;
	if (caps.currentExtent.width == UINT32_MAX) {
		return (VkExtent2D) {
			clamp(window_x, caps.minImageExtent.width, caps.maxImageExtent.width),
			clamp(window_y, caps.minImageExtent.height, caps.maxImageExtent.height)
		};
	}
	return caps.currentExtent;
}

void build_swapchain() {
	swapchain.format = select_format();
	swapchain.mode = select_mode();
	swapchain.extent = select_extent();
	printl(DEBUG, "Swapchain set to %dx%d px with colour format 0x%x", swapchain.extent.width, swapchain.extent.height, swapchain.format.format);
}

void create_swapchain() {
	build_swapchain();

	VkSwapchainCreateInfoKHR info = {
		.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
		.surface = surface,

		.minImageCount = min_image_count(),
		.imageFormat = swapchain.format.format,
		.imageColorSpace = swapchain.format.colorSpace,
		.imageExtent = swapchain.extent,
		/* Number of views, >1 for a stereoscopic 3D thing */
		.imageArrayLayers = 1,
		/* See VK_IMAGE_USAGE_TRANSFER_DST_BIT */
		.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,

		.preTransform = swapchain.caps.currentTransform,
		.compositeAlpha = WINDOW_ALPHA,
		.presentMode = swapchain.mode,
		/* Don't compute pixels that are covered by windows */
		.clipped = VK_TRUE,
		.oldSwapchain = VK_NULL_HANDLE /*swapchain.swapchain*/
	};
	printl(WARN, "Used to be %p", swapchain.swapchain);

	printl(WARN, "Total families: %u, unique: %u", devices.family_count, devices.unique_family_count);
	if (devices.family_count != devices.unique_family_count) {
		unsigned indices[devices.family_count];
		for (int i = 0; i < devices.family_count; i++) {
			indices[i] = devices.families[i].index;
		}

		info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		info.queueFamilyIndexCount = devices.family_count;
		info.pQueueFamilyIndices = indices;
	} else {
		/* Exclusive is faster.
		   TODO: Prefer mixed queue families. */
		info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	}

	VkResult res;
	/* FIXME: Jump to invalid address NULL, but vkCreateSwapchainKHR isnt null? */
	printl(WARN, "vkCreateSwapchainKHR is %p/%p", vkCreateSwapchainKHR, &vkCreateSwapchainKHR);
	res = vkCreateSwapchainKHR(devices.dev, &info, NULL, &swapchain.swapchain);
	printl(WARN, "Ok it worked!?");
	if ((res = vkCreateSwapchainKHR(devices.dev, &info, NULL, &swapchain.swapchain) != VK_SUCCESS)) {
		error("Failed to create Swapchain: %s (%d)", vk_error(res), (int) res);
	}

	/* Fill pointer to images rendered */
	unsigned last = swapchain.image_count;
	vkGetSwapchainImagesKHR(devices.dev, swapchain.swapchain, &swapchain.image_count, NULL);
	/* But only re-allocate if theres a different number of frames */
	if (last != swapchain.image_count) {
		freeptr(swapchain.images);

		if (!(swapchain.images = malloc(sizeof(VkImage) * swapchain.image_count))) {
			error("Failed to allocate memory for %u renderer images: %s", swapchain.image_count, strerror(errno));
		}
	}
	vkGetSwapchainImagesKHR(devices.dev, swapchain.swapchain, &swapchain.image_count, swapchain.images);
}

void free_swapchain() {
	if (swapchain.swapchain != VK_NULL_HANDLE) {
		vkDestroySwapchainKHR(devices.dev, swapchain.swapchain, NULL);
	}
	freeptr(swapchain.formats);
	freeptr(swapchain.modes);
	freeptr(swapchain.images);
}