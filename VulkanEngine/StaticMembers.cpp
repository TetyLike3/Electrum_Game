#include "StaticMembers.h"


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

std::vector<Model>* StaticMembers::m_models = {};


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

std::vector<Model>* StaticMembers::getModels() { return m_models; };