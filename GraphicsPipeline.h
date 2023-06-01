#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "Utilities.h"
#include "Swapchain.h"
#include "Buffers.h"


class GraphicsPipeline
{
public:

	GraphicsPipeline(VkDevice* pLogicalDevice, Swapchain* pSwapchain, sSettings::sGraphicsSettings* pGraphicsSettings)
		: m_pLogicalDevice(pLogicalDevice), m_pSwapchain(pSwapchain), m_pGraphicsSettings(pGraphicsSettings), m_pUtilities(Utilities::getInstance())
	{
		createRenderPass();
		createDescriptorSetLayout();
		createGraphicsPipeline();
	};

	void createRenderPass();
	void createDescriptorSetLayout();
	void createGraphicsPipeline();
	void cleanup();

	VkPipeline* getGraphicsPipeline() { return &m_graphicsPipeline; }
	VkRenderPass* getRenderPass() { return &m_renderPass; }
	VkDescriptorSetLayout* getDescriptorSetLayout() { return &m_descriptorSetLayout; }

private:
	Utilities* m_pUtilities = nullptr;
	VkDevice* m_pLogicalDevice = nullptr;
	Swapchain* m_pSwapchain = nullptr;
	sSettings::sGraphicsSettings* m_pGraphicsSettings = nullptr;

	VkDescriptorSetLayout m_descriptorSetLayout = VK_NULL_HANDLE;
	VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
	VkRenderPass m_renderPass = VK_NULL_HANDLE;
	VkPipeline m_graphicsPipeline = VK_NULL_HANDLE;


	VkShaderModule createShaderModule(const std::vector<char>& code);
};

