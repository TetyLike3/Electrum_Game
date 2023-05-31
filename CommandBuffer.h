#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>


#include "Utilities.h"
#include "QueueFamilyIndices.h"
#include "Swapchain.h"
#include "VertexBuffer.h"


class CommandBuffer
{
public:
	CommandBuffer(LogicalDevice* pLogicalDevice, VkSurfaceKHR* pSurface, VkRenderPass* pRenderPass, Swapchain* pSwapchain, VkPipeline* pGraphicsPipeline)
		: m_pLogicalDevice(pLogicalDevice->getLogicalDevice()), m_pPhysicalDevice(pLogicalDevice->getPhysicalDevice()->getPhysicalDevice()), m_pSurface(pSurface),
		m_pRenderPass(pRenderPass), m_pSwapchain(pSwapchain), m_pGraphicsPipeline(pGraphicsPipeline), m_pUtilities(Utilities::getInstance()) {};

	void createCommandPool();
	void createCommandBuffers(int MAX_FRAMES_IN_FLIGHT, VkBuffer* pVertexBuffer, VkBuffer* pIndexBuffer);
	void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

	void cleanup();

	VkCommandPool* getVkCommandPool() { return &m_commandPool; }
	std::vector<VkCommandBuffer>* getCommandBuffers() { return &m_commandBuffers; }

private:
	VkDevice* m_pLogicalDevice = nullptr;
	VkPhysicalDevice* m_pPhysicalDevice = nullptr;
	VkSurfaceKHR* m_pSurface = nullptr;
	VkRenderPass* m_pRenderPass = nullptr;
	Swapchain* m_pSwapchain = nullptr;
	VkPipeline* m_pGraphicsPipeline = nullptr;
	VkBuffer* m_pVertexBuffer = nullptr;
	VkBuffer* m_pIndexBuffer = nullptr;
	Utilities* m_pUtilities = nullptr;

	VkCommandPool m_commandPool = VK_NULL_HANDLE;
	std::vector<VkCommandBuffer> m_commandBuffers = {};
};