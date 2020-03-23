BUILDDIR ?= build
OBJECTDIR ?= objects
SOURCEDIR ?= src
INCLUDEDIR ?= include
SHADERDIR ?= shaders

UNAME := $(shell uname)
CC ?= gcc
STANDARD ?= c99
CFLAGS ?= -O3 -Wall -std=$(STANDARD) -pedantic -I$(INCLUDEDIR) -g
LDFLAGS ?= '-Wl,-rpath,$$ORIGIN' -g
LDFLAGS += $(shell pkg-config --libs vulkan)
GLSLFLAGS ?= --target-env vulkan1.0

BINARY ?= render
SOURCES := $(shell find $(SOURCEDIR)/ -type f -name "*.c")
OBJECTS := $(patsubst $(SOURCEDIR)/%, $(OBJECTDIR)/%.o, $(SOURCES))
DEPENDS := $(patsubst $(SOURCEDIR)/%, $(OBJECTDIR)/%.d, $(SOURCES))
SHADERS := $(shell find $(SHADERDIR)/ -type f -name "*.glsl")
SPIRV   := $(patsubst $(SHADERDIR)/shader.%.glsl, $(BUILDDIR)/%.spv, $(SHADERS))

# Options, pass with NAME=VALUE
# WINDOW BACKEND
## To use SDL for window creation
SUPPORT_SDL ?= 1
ifeq ($(SUPPORT_SDL), 1)
	CFLAGS += -DSUPPORT_SDL
	CFLAGS += $(shell pkg-config --cflags sdl2)
	LDFLAGS += $(shell pkg-config --libs sdl2)
endif

# VULKAN
## Enable validation layers, do not use for releases
USE_LAYERS ?= 1
ifeq ($(USE_LAYERS), 1)
	CFLAGS += -DUSE_LAYERS
	LDFLAGS += -lVkLayer_core_validation
endif

all: $(BUILDDIR)/$(BINARY) $(SPIRV)

$(OBJECTDIR)/%.o: $(SOURCEDIR)/%
	@printf "CC\t$@\n"
	@mkdir -p `dirname $@`
	@$(CC) $(CFLAGS) -c -MMD -MP $< -o $@

-include $(DEPENDS)

$(BUILDDIR)/%.spv: $(SHADERDIR)/shader.%.glsl
	@printf "GLSL\t$@: "
	@glslangValidator $(GLSLFLAGS) -o $@ $^

$(BUILDDIR)/$(BINARY): $(OBJECTS)
	@printf "CCLD\t$@\n"
	@mkdir -p $(BUILDDIR)
	@$(CC) $^ -o $@ $(LDFLAGS)

clean:
	rm -rf $(OBJECTDIR)