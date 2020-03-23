#pragma once

#include <vulkan/vulkan_core.h>

extern VkDebugUtilsMessengerEXT messenger;
extern VkDebugUtilsMessengerCreateInfoEXT debugging_info;

/* Set up validation layer debugging messenger.
   Does nothing if validation layers are disabled. */
void init_debugging();
void free_debugging();