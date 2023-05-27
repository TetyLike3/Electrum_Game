#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "Utilities.h"


class GraphicsPipeline
{
public:
	GraphicsPipeline(VkDevice* pLogicalDevice, VkExtent2D* pSwapChainExtent, sSettings::sGraphicsSettings* pGraphicsSettings)
		: m_pLogicalDevice(pLogicalDevice), m_pSwapChainExtent(pSwapChainExtent), m_pGraphicsSettings(pGraphicsSettings), m_pUtilities(Utilities::getInstance()) {};

	void createGraphicsPipeline();
	void cleanup();

private:
	Utilities* m_pUtilities = nullptr;
	VkDevice* m_pLogicalDevice = nullptr;
	VkExtent2D* m_pSwapChainExtent = {};
	sSettings::sGraphicsSettings* m_pGraphicsSettings = nullptr;

	VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;


	VkShaderModule createShaderModule(const std::vector<char>& code);
};

