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
	CommandBuffer(LogicalDevice* pLogicalDevice, VkSurfaceKHR* pSurface, VkRenderPass* pRenderPass, Swapchain* pSwapchain, VkPipeline* pGraphicsPipeline, VertexBuffer* pVertexBuffer)
		: m_pLogicalDevice(pLogicalDevice->getLogicalDevice()), m_pPhysicalDevice(pLogicalDevice->getPhysicalDevice()->getPhysicalDevice()), m_pSurface(pSurface),
		m_pRenderPass(pRenderPass), m_pSwapchain(pSwapchain), m_pGraphicsPipeline(pGraphicsPipeline), m_pVertexBuffer(pVertexBuffer), m_pUtilities(Utilities::getInstance()) {};

	void createCommandPool();
	void createCommandBuffers(int MAX_FRAMES_IN_FLIGHT);
	void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

	void cleanup();

	std::vector<VkCommandBuffer>* getCommandBuffers() { return &m_commandBuffers; }

private:
	VkDevice* m_pLogicalDevice = nullptr;
	VkPhysicalDevice* m_pPhysicalDevice = nullptr;
	VkSurfaceKHR* m_pSurface = nullptr;
	VkRenderPass* m_pRenderPass = nullptr;
	Swapchain* m_pSwapchain = nullptr;
	VkPipeline* m_pGraphicsPipeline = nullptr;
	VertexBuffer* m_pVertexBuffer = nullptr;
	Utilities* m_pUtilities = nullptr;

	VkCommandPool m_commandPool = VK_NULL_HANDLE;
	std::vector<VkCommandBuffer> m_commandBuffers = {};
};