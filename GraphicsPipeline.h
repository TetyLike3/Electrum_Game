#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "Utilities.h"
#include "Swapchain.h"


class GraphicsPipeline
{
public:
	GraphicsPipeline(VkDevice* pLogicalDevice, Swapchain* pSwapchain, sSettings::sGraphicsSettings* pGraphicsSettings)
		: m_pLogicalDevice(pLogicalDevice), m_pSwapchain(pSwapchain), m_pGraphicsSettings(pGraphicsSettings), m_pUtilities(Utilities::getInstance()) {};

	void createRenderPass();
	void createGraphicsPipeline();
	void cleanup();

private:
	Utilities* m_pUtilities = nullptr;
	VkDevice* m_pLogicalDevice = nullptr;
	Swapchain* m_pSwapchain = nullptr;
	sSettings::sGraphicsSettings* m_pGraphicsSettings = nullptr;

	VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;


	VkShaderModule createShaderModule(const std::vector<char>& code);
};

