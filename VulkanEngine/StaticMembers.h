#pragma once


// This class is used to store all the other classes used in VulkanEngine. It should prevent the need to pass around masses of pointers to all the classes.
// It should also make it easier to add new classes to the engine.

// This class is accessed using :: notation, e.g. StaticMembers::getVkInstance().


#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "Utilities/Utilities.h"


class VulkanEngine;

class Window;
class PhysicalDevice;
class LogicalDevice;
class Swapchain;
class GraphicsPipeline;
class BufferManager;
class Model;
struct sVertex;

class StaticMembers
{
public:
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

	static std::vector<Model>* getModels();

private:
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

	static std::vector<Model>* m_models;

	friend class VulkanEngine;
};

