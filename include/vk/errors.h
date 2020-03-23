#pragma once

/* printl, error macro */
#include "log.h"
#include "utils.h"

#include <vulkan/vulkan_core.h>

/* strerror(errno) */
#include <errno.h>
#include <string.h>

const char *vk_error(VkResult res);