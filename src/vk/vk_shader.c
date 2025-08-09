#include "vk_fun.h"

static uint32_t getShaderSize(const char *fileName){
	FILE *fp = VK_NULL_HANDLE;
	fp = fopen(fileName, "rb+");
	if(fp == VK_NULL_HANDLE){
		printf("failed to open shader!\n");
		exit(EXIT_FAILURE);
	}
	fseek(fp, 0l, SEEK_END);
	uint32_t shaderSize = (uint32_t)ftell(fp);
	fclose(fp);
	return shaderSize;
}

static uint32_t *getShaderCode(const char *fileName, const uint32_t shaderSize){
	FILE *fp =VK_NULL_HANDLE;
	fp = fopen(fileName, "rb+");
	if(fp == VK_NULL_HANDLE){
		printf("failed to open shader!\n");
		exit(EXIT_FAILURE);
	}
	uint32_t *shaderCode = malloc((shaderSize) * sizeof(char));
	fread(shaderCode, 1, shaderSize, fp);
	fclose(fp);
	return shaderCode;
}

static void deleteShaderCode(uint32_t **pShaderCode){
	free(*pShaderCode);
}

static VkShaderModule createShaderModule(const VkDevice device, const uint32_t *shaderCode, const uint32_t shaderSize){
	VkShaderModuleCreateInfo shaderModuleCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		.codeSize = shaderSize,
		.pCode = shaderCode
	};

	VkShaderModule shaderModule;
	if(vkCreateShaderModule(device, &shaderModuleCreateInfo, VK_NULL_HANDLE, &shaderModule) != VK_SUCCESS){
		printf("failed to create shader module!\n");
		exit(EXIT_FAILURE);
	}
	return shaderModule;
}

static void deleteShaderModule(const VkDevice device, VkShaderModule *pShaderModule){
	vkDestroyShaderModule(device, *pShaderModule, VK_NULL_HANDLE);
}

VkShaderModule getShader(const VkDevice device, const char *fileName){
	uint32_t shaderSize = getShaderSize(fileName);
	uint32_t *shaderCode = getShaderCode(fileName, shaderSize);
	VkShaderModule shaderModule = createShaderModule(device, shaderCode, shaderSize);
	deleteShaderCode(&shaderCode);
	return shaderModule;
}

void deleteShader(const VkDevice device, VkShaderModule *pShaderModule){
	deleteShaderModule(device, pShaderModule);
}
