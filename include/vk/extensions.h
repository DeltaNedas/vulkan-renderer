#pragma once

#include <vulkan/vulkan_core.h>

/* Instance extension stuff.
   See devices.c for device extensions. */

extern char **exts;
/* Exts required by the window backend, total exts */
extern unsigned required_exts, ext_count;

void init_extensions();
void free_extensions();