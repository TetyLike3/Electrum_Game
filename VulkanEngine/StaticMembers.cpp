#include "Devices.h"
#include "Window.h"
#include "Swapchain.h"
#include "GraphicsPipeline.h"
#include "Buffers.h"

#include "StaticMembers.h"

//StaticMembers* StaticMembers::m_pInstance = nullptr;


VkInstance StaticMembers::m_vkInstance = VK_NULL_HANDLE;
Window* StaticMembers::m_pWindow = nullptr;
VkSurfaceKHR* StaticMembers::m_pVkSurface = nullptr;
PhysicalDevice* StaticMembers::m_pPhysicalDevice = nullptr;
VkPhysicalDevice* StaticMembers::m_pVkPhysicalDevice = nullptr;
LogicalDevice* StaticMembers::m_pLogicalDevice = nullptr;
VkDevice* StaticMembers::m_pVkDevice = nullptr;
Swapchain* StaticMembers::m_pSwapchain = nullptr;
GraphicsPipeline* StaticMembers::m_pGraphicsPipeline = nullptr;
BufferManager* StaticMembers::m_pBufferManager = nullptr;

int StaticMembers::m_MAX_FRAMES_IN_FLIGHT = 0;
sSettings* StaticMembers::m_settings = nullptr;


/*
StaticMembers* StaticMembers::getInstance()
{
	if (m_pInstance == nullptr)
	{
		m_pInstance = new StaticMembers();
	}

	return m_pInstance;
}
*/

// Member Get/Set functions

VkInstance* StaticMembers::getVkInstance() { return &m_vkInstance; };

Window* StaticMembers::getWindow() { return m_pWindow; };

VkSurfaceKHR* StaticMembers::getVkSurfaceKHR() { return m_pVkSurface; };

PhysicalDevice* StaticMembers::getPhysicalDevice() { return m_pPhysicalDevice; };
VkPhysicalDevice* StaticMembers::getVkPhysicalDevice() { return m_pVkPhysicalDevice; };

LogicalDevice* StaticMembers::getLogicalDevice() { return m_pLogicalDevice; };
VkDevice* StaticMembers::getVkDevice() { return m_pVkDevice; };

Swapchain* StaticMembers::getSwapchain() { return m_pSwapchain; };

GraphicsPipeline* StaticMembers::getGraphicsPipeline() { return m_pGraphicsPipeline; };

BufferManager* StaticMembers::getBufferManager() { return m_pBufferManager; };

int StaticMembers::getMAX_FRAMES_IN_FLIGHT() { return m_MAX_FRAMES_IN_FLIGHT; };

sSettings* StaticMembers::getSettings() { return m_settings; };