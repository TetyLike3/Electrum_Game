#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>


#include "Utilities.h"
#include "QueueFamilyIndices.h"
#include "Swapchain.h"


class CommandBuffer
{
public:
	CommandBuffer(VkDevice* pLogicalDevice, VkPhysicalDevice* pPhysicalDevice, VkSurfaceKHR* pSurface)
		: m_pLogicalDevice(pLogicalDevice), m_pPhysicalDevice(pPhysicalDevice), m_pSurface(pSurface), m_pUtilities(Utilities::getInstance()) {};

	void createCommandPool();
	void createCommandBuffer();
	void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex, VkRenderPass* pRenderPass, Swapchain* pSwapchain, VkPipeline* pGraphicsPipeline);

	void cleanup();

private:
	VkDevice* m_pLogicalDevice = nullptr;
	VkPhysicalDevice* m_pPhysicalDevice = nullptr;
	VkSurfaceKHR* m_pSurface = nullptr;
	Utilities* m_pUtilities = nullptr;

	VkCommandPool m_commandPool = VK_NULL_HANDLE;
	VkCommandBuffer m_commandBuffer = VK_NULL_HANDLE;
};