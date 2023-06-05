#pragma once



#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "Utilities.h"


class VulkanEngine;

class Window;
class PhysicalDevice;
class LogicalDevice;
class Swapchain;
class GraphicsPipeline;
class BufferManager;


class StaticMembers
{
public:
	//static StaticMembers* getInstance();


	// Member Get/Set functions

	static VkInstance* getVkInstance();

	static Window* getWindow();

	static VkSurfaceKHR* getVkSurfaceKHR();

	static PhysicalDevice* getPhysicalDevice();
	static VkPhysicalDevice* getVkPhysicalDevice();

	static LogicalDevice* getLogicalDevice();
	static VkDevice* getVkDevice();

	static Swapchain* getSwapchain();

	static GraphicsPipeline* getGraphicsPipeline();

	static BufferManager* getBufferManager();

	static int getMAX_FRAMES_IN_FLIGHT();

	static sSettings* getSettings();

private:
	//StaticMembers();
	//static StaticMembers* m_pInstance;

	// Static members
	static VkInstance m_vkInstance;
	static Window* m_pWindow;
	static VkSurfaceKHR* m_pVkSurface;
	static PhysicalDevice* m_pPhysicalDevice;
	static VkPhysicalDevice* m_pVkPhysicalDevice;
	static LogicalDevice* m_pLogicalDevice;
	static VkDevice* m_pVkDevice;
	static Swapchain* m_pSwapchain;
	static GraphicsPipeline* m_pGraphicsPipeline;
	static BufferManager* m_pBufferManager;

	static int m_MAX_FRAMES_IN_FLIGHT;
	static sSettings* m_settings;

	friend class VulkanEngine;
};

