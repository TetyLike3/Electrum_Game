#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#include <set>
#include <vector>
#include <stdexcept>
#include <optional>

#include "Utilities.h"
#include "QueueFamilyIndices.h"



const std::vector<const char*> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};


struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities = {};
	std::vector<VkSurfaceFormatKHR> formats = {};
	std::vector<VkPresentModeKHR> presentModes = {};
};




class PhysicalDevice
{
public:
	PhysicalDevice(VkInstance* pVkInstance, VkSurfaceKHR* pSurface) : m_pVkInstance(pVkInstance), m_pSurface(pSurface), m_pUtilities(Utilities::getInstance()) {};

	VkPhysicalDevice* pickPhysicalDevice();
	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice physicalDevice);

	VkPhysicalDevice* getPhysicalDevice() { return &m_physicalDevice; };

private:
	VkInstance* m_pVkInstance = nullptr;
	VkSurfaceKHR* m_pSurface = nullptr;

	Utilities* m_pUtilities = nullptr;

	VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;


	bool isDeviceSuitable(VkPhysicalDevice candidateDevice);
	bool checkDeviceExtensionSupport(VkPhysicalDevice candidateDevice);
};


class LogicalDevice
{
public:
	LogicalDevice(PhysicalDevice* pPhysicalDevice, VkSurfaceKHR* pSurface, GLFWwindow* pWindow) : m_pPhysicalDevice(pPhysicalDevice), m_pSurface(pSurface), m_pWindow(pWindow), m_pUtilities(Utilities::getInstance()) {};

	void createLogicalDevice(sSettings::sDebugSettings* pDebugSettings);

	void cleanup();

	VkDevice* getLogicalDevice() { return &m_logicalDevice; };
	VkQueue* getGraphicsQueue() { return &m_graphicsQueue; };

private:
	PhysicalDevice* m_pPhysicalDevice = nullptr;
	VkSurfaceKHR* m_pSurface = nullptr;
	GLFWwindow* m_pWindow = nullptr;

	Utilities* m_pUtilities = nullptr;

	VkDevice m_logicalDevice = VK_NULL_HANDLE;

	VkQueue m_graphicsQueue = VK_NULL_HANDLE;
	VkQueue m_presentQueue = VK_NULL_HANDLE;
};