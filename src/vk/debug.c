#include "vk/debug.h"

#include "vk/errors.h"
#include "vk/instance.h"

VkDebugUtilsMessengerEXT messenger = VK_NULL_HANDLE;
VkDebugUtilsMessengerCreateInfoEXT debugging_info;

VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
	VkDebugUtilsMessageSeverityFlagBitsEXT severity,
	VkDebugUtilsMessageTypeFlagsEXT type,
	const VkDebugUtilsMessengerCallbackDataEXT* data,
	void *unused) {

	level_t level;
	switch (severity) {
	/* Debug and info are mostly useless */
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT: level = DEBUG; break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: level = WARN; break;
	default: level = ERROR;
	}

	printl(level, "[Validation layer] %s", data->pMessage);

	return VK_FALSE;
}

void init_debugging() {
	debugging_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	debugging_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT;
	debugging_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_FLAG_BITS_MAX_ENUM_EXT;
	debugging_info.pfnUserCallback = debug_callback;

	PFN_vkCreateDebugUtilsMessengerEXT func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (!func) {
		error("Failed to load vkCreateDebugUtilsMessengerEXT, missing debug utils extension?");
	}

	VkResult res;
	if ((res = func(instance, &debugging_info, NULL, &messenger)) != VK_SUCCESS) {
		error("Failed to create debug messenger: %s (%d)", vk_error(res), (int) res);
	}
}

void free_debugging() {
	if (messenger != VK_NULL_HANDLE) {
		PFN_vkDestroyDebugUtilsMessengerEXT func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
		if (!func) {
			error("Failed to unload vkDestroyDebugUtilsMessengerEXT, missing debug utils extension?");
		}

		func(instance, messenger, NULL);
	}
}