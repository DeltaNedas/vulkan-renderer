#pragma once

#include <vulkan/vulkan_core.h>

#include <stdlib.h>

#define error(...) printl(FATAL, __VA_ARGS__); quit(EXIT_FAILURE);
#define assert(x) if (x) quit(EXIT_FAILURE);
#define freeptr(p) if (p) free(p);
#define clamp(n, min, max) (n > min ? n : min) < max ? n : max

/* Default timestamp buffer size */
#define TIMESTAMP_LEN 64
/* Default format if fmt is NULL */
#define TIMESTAMP_FORMAT "%Y/%m/%d - %I:%M:%S %p"
typedef unsigned long long nanos_t;

void quit(int code);

/* nanoseconds since 1/1/1970 */
nanos_t timestamp();

char *timestamp_strft(const char *fmt, int len, nanos_t at);
char *timestamp_strt(nanos_t at);
char *timestamp_strf(const char *fmt, int len);
char *timestamp_str();

char *strdup(const char *str);

/* Equivalent to strerror(3)
 Vulkan doesnt provide this for some reason.
 (Better than the SDL one because it's a description and not just the enumeration's name) */
const char *vulkan_error(VkResult code);