#include "vk/shaders.h"

#include "vk/devices.h"
#include "vk/errors.h"

VkShaderModule vert, frag;
VkPipelineShaderStageCreateInfo *shaders;
VkPipelineVertexInputStateCreateInfo vertex_input;
VkPipelineInputAssemblyStateCreateInfo input_assembly;

void load_module(char *data, size_t len, VkShaderModule *shader) {
	VkResult res;
	VkShaderModuleCreateInfo info = {
		.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		.codeSize = len,
		.pCode = (unsigned*) data
	};

	/* FIXME: 2kb memory leak, proportional with shader data read it seems */
	if ((res = vkCreateShaderModule(devices.dev, &info, NULL, shader)) != VK_SUCCESS) {
		error("Failed to load shader module: %s (%d)", vk_error(res), (int) res);
	}
}

void load_shader(VkShaderModule *shader, char *path, int optional) {
	char *data;
	FILE *f;
	if (!(f = fopen(path, "rb"))) {
		if (optional) {
			return;
		}
		error("Failed to load shader '%s': %s", path, strerror(errno));
	};

	if (fseek(f, 0, SEEK_END)) {
		fclose(f);
		error("Failed to get file size of shader '%s': %s", path, strerror(errno));
	}
	size_t size = ftell(f), read;
	rewind(f);

	if (!(data = malloc(size))) {
		fclose(f);
		free(data);
		error("Failed to allocate memory for shader '%s': %s", path, strerror(errno));
	}

	if ((read = fread(data, 1, size, f)) != size) {
		int code = ferror(f);
		fclose(f);
		free(data);
		if (code) {
			error("Failed to read %lu/%lu bytes from shader '%s': %s", read, size, path, strerror(code));
		}
		if (feof(f)) {
			error("Shader '%s' ended unexpectedly at %lu (should end at %lu)", read, size);
		} else {
			error("Unknown error when trying to read shader '%s'", path);
		}
	}

	printl(INFO, "Loading shader '%s': %lu bytes", path, size);
	load_module(data, size, shader);
	fclose(f);
	free(data);
}

void stage_shaders() {
	if (!(shaders = malloc(sizeof(VkPipelineShaderStageCreateInfo) * SHADER_COUNT))) {
		error("Failed to allocate memory for %d shader stage creation structs: %s", SHADER_COUNT, strerror(errno));
	}

	shaders[0] = (VkPipelineShaderStageCreateInfo) {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
		.stage = VK_SHADER_STAGE_VERTEX_BIT,
		.module = vert,
		.pName = "shader"
	};
	shaders[1] = (VkPipelineShaderStageCreateInfo) {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
		.stage = VK_SHADER_STAGE_FRAGMENT_BIT,
		.module = frag,
		.pName = "shader"
	};

	vertex_input = (VkPipelineVertexInputStateCreateInfo) {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
		.vertexBindingDescriptionCount = 0,
		.vertexAttributeDescriptionCount = 0
	};

	input_assembly = (VkPipelineInputAssemblyStateCreateInfo) {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
		.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
		.primitiveRestartEnable = VK_FALSE
	};
}

void init_shaders() {
	load_shader(&vert, "vert.spv", 0);
	load_shader(&frag, "frag.spv", 0);
	stage_shaders();
}

void free_shaders() {
	/* Shaders are loaded into the program, they aren't needed anymore */
	vkDestroyShaderModule(devices.dev, vert, NULL);
	vkDestroyShaderModule(devices.dev, frag, NULL);
}