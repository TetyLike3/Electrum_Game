#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "../StaticMembers.h"
#include "../Utilities/Utilities.h"
#include "Swapchain.h"
#include "Devices.h"


class Swapchain;

class GraphicsPipeline
{
public:

	GraphicsPipeline() : m_pLogicalDevice(StaticMembers::getVkDevice()), m_pPhysicalDevice(StaticMembers::getVkPhysicalDevice()),
		m_pSwapchain(StaticMembers::getSwapchain()), m_pGraphicsSettings(&StaticMembers::getSettings()->graphicsSettings), m_pUtilities(Utilities::getInstance())
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
	VkPipelineLayout* getVkPipelineLayout() { return &m_pipelineLayout; }
	VkRenderPass* getRenderPass() { return &m_renderPass; }
	VkDescriptorSetLayout* getDescriptorSetLayout() { return &m_descriptorSetLayout; }

private:
	Utilities* m_pUtilities = nullptr;
	VkDevice* m_pLogicalDevice = nullptr;
	VkPhysicalDevice* m_pPhysicalDevice = nullptr;
	Swapchain* m_pSwapchain = nullptr;
	sSettings::sGraphicsSettings* m_pGraphicsSettings = nullptr;

	VkDescriptorSetLayout m_descriptorSetLayout = VK_NULL_HANDLE;
	VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
	VkRenderPass m_renderPass = VK_NULL_HANDLE;
	VkPipeline m_graphicsPipeline = VK_NULL_HANDLE;


	VkShaderModule createShaderModule(const std::vector<char>& code);
};

