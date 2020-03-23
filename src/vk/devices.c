#include "vk/devices.h"

#include "options.h"
#include "vk/errors.h"
#include "vk/instance.h"
#include "vk/surface.h"
#include "vk/swapchain.h"

devices_t devices;

bool families_complete() {
	for (int i = 0; i < devices.family_count; i++) {
		if (devices.families[i].index == -1) {
			return FALSE;
		}
	}
	return TRUE;
}

bool find_queue_families(VkPhysicalDevice dev) {
	vkGetPhysicalDeviceQueueFamilyProperties(dev, &devices.family_count, NULL);

	VkQueueFamilyProperties props, *families;
	if (!(families = malloc(sizeof(VkQueueFamilyProperties) * devices.family_count))) {
		printl(ERROR, "Failed to allocate memory for %u queue families: %s", devices.family_count, strerror(errno));
		return FALSE;
	}

	vkGetPhysicalDeviceQueueFamilyProperties(dev, &devices.family_count, families);

	for (int i = 0; i < devices.family_count; i++) {
		/* If queue can compute graphics */
		props = families[i];
		if (props.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			devices.families[GRAPHICS] = (family_t) {
				.index = i,
				.props = props
			};
		}

		/* If queue can draw to window */
		VkBool32 can_present = 0;
		vkGetPhysicalDeviceSurfaceSupportKHR(dev, i, surface, &can_present);
		if (can_present) {
			devices.families[PRESENT] = (family_t) {
				.index = i,
				.props = props
			};
		}

		if (families_complete()) {
			break;
		}
	}
	free(families);
	return TRUE;
}

bool valid_props(VkPhysicalDeviceProperties props) {
	unsigned v = props.apiVersion;
	return (v >> 22) * 10 + ((v >> 12) & 0x3ff) >= MAJOR * 10 + MINOR;
}

bool valid_features(VkPhysicalDeviceFeatures feats) {
	return feats.geometryShader;
}

bool valid_extensions(VkPhysicalDevice dev) {
	uint32_t count;
	vkEnumerateDeviceExtensionProperties(dev, NULL, &count, NULL);

	char *ext;
	VkExtensionProperties *avail;
	if (!(avail = malloc(sizeof(VkExtensionProperties) * count))) {
		printl(ERROR, "Failed to allocate memory for available device extensions: %s", strerror(errno));
		return FALSE;
	}
	vkEnumerateDeviceExtensionProperties(dev, NULL, &count, avail);

	for (int i = 0; i < devices.ext_count; i++) {
		ext = devices.exts[i];
		for (int j = 0; j < count; j++) {
			if (!strcmp(ext, avail[j].extensionName)) {
				goto found_it;
			}
		}

		free(avail);
		return FALSE;
	found_it:
		continue;
	}
	free(avail);
	return TRUE;
}

bool valid_pd(VkPhysicalDevice dev, VkPhysicalDeviceProperties props, VkPhysicalDeviceFeatures feats) {
	return valid_props(props) &&
		valid_features(feats) &&
		valid_extensions(dev) &&
		find_queue_families(dev) &&
		valid_swapchain(dev);
}

void pick_pd() {
	/* TODO: Prioritise dedicated cards over integrated graphics */

	unsigned count = 0;
	vkEnumeratePhysicalDevices(instance, &count, NULL);
	if (!count) {
		error("No Vulkan-compatible devices were found.\n\t(You can install mesa with 'apt install mesa' as root on Debian)");
	}

	VkPhysicalDevice dev, *dev_arr = malloc(sizeof(VkPhysicalDevice) * count);
	if (!dev_arr) {
		error("Failed to allocate memory for physical devices array");
	}
	vkEnumeratePhysicalDevices(instance, &count, dev_arr);

	VkPhysicalDeviceProperties props;
	VkPhysicalDeviceFeatures feats;
	for (int i = 0; i < count; i++) {
		vkGetPhysicalDeviceProperties(dev = dev_arr[i], &props);
		vkGetPhysicalDeviceFeatures(dev, &feats);
		if (valid_pd(dev, props, feats)) {
			devices.pd = dev;
			devices.pd_props = props;
			devices.pd_feats = feats;
			int v = props.apiVersion;
			printl(INFO, "Using device %s - %u", props.deviceName, props.deviceID);
			printl(INFO, "\tVulkan API version %u.%u.%u", v >> 22, (v >> 12) & 0x3ff, v & 0xfff);
			printl(INFO, "\tDriver version 0x%x", props.driverVersion);
			if (props.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
				printl(WARN, "\tNot a dedicated GPU, performance may suffer");
			}
			free(dev_arr);
			return;
		}
	}

	printl(FATAL, "Couldn't find a graphics card that supports the following device extensions:");
	for (unsigned i = 0; i < devices.ext_count; i++) {
		printl(WARN, "\t%s", devices.exts[i]);
	}
	printl(WARN, "Additionally, it must support Vulkan %u.%u.0", MAJOR, MINOR);
	free(dev_arr);
	quit(ENOTSUP);
}

void init_queues() {
	family_t fam;
	for (int i = 0; i < devices.family_count; i++) {
		fam = devices.families[i];
		vkGetDeviceQueue(devices.dev, fam.index, 0, &fam.queue);
	}
}

void init_devices() {
	devices.family_count = 2;
	if (!(devices.families = malloc(sizeof(family_t) * devices.family_count))) {
		error("Failed to allocate memory for %u queue families: %s", devices.family_count, strerror(errno));
	}
	pick_pd();

	/* Set up queues, share 2 queues on one device if possible */
	int index, len = 0, max = devices.family_count;
	int indices[max];
	for (int i = 0; i < max; i++) {
		index = devices.families[i].index;
		bool duped = FALSE;
		for (int j = i; j < max; j++) {
			if (i != j && index == devices.families[j].index) {
				duped = TRUE;
				break;
			}
		}

		if (!duped) {
			indices[len++] = index;
		}
	}

	VkDeviceQueueCreateInfo *queue_infos;
	if (!(queue_infos = malloc(sizeof(VkDeviceQueueCreateInfo) * len))) {
		error("Failed to allocate memory for device queues: %s", strerror(errno));
	}

	devices.unique_family_count = len;

	for (int i = 0; i < len; i++) {
		queue_infos[i] = (VkDeviceQueueCreateInfo) {
			.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
			.queueFamilyIndex = indices[i],
			.queueCount = 1,
			.pQueuePriorities = &queue_priority
		};
	}

	/* Set up device */
	VkPhysicalDeviceFeatures features;
	memset(&features, 0, sizeof(features));
	VkDeviceCreateInfo info = {
		.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
		.pQueueCreateInfos = queue_infos,
		.queueCreateInfoCount = len,
		.pEnabledFeatures = &features,
		.enabledExtensionCount = devices.ext_count,
		.ppEnabledExtensionNames = (const char**) devices.exts,
		.enabledLayerCount = devices.layer_count,
		.ppEnabledLayerNames = (const char**) devices.layers
	};

	VkResult res = vkCreateDevice(devices.pd, &info, NULL, &devices.dev);
	free(queue_infos);
	if (res != VK_SUCCESS) {
		error("Failed to create logical device: %s (%d)", vk_error(res), (int) res);
	}

	init_queues();
}

void free_devices() {
	if (devices.dev != VK_NULL_HANDLE) {
		vkDestroyDevice(devices.dev, NULL);
	}
}