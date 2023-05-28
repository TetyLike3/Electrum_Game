#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "Utilities.h"
#include "Devices.h"


class Swapchain
{
public:
	Swapchain(VkDevice* pLogicalDevice, PhysicalDevice* pPhysicalDevice, GLFWwindow* pWindow,VkSurfaceKHR* pSurface)
		: m_pLogicalDevice(pLogicalDevice), m_pPhysicalDevice(pPhysicalDevice), m_pWindow(pWindow), m_pSurface(pSurface), m_pUtilities(Utilities::getInstance()) {};

	void cleanup();

	void createSwapChain();
	void createImageViews();

	VkExtent2D* getSwapChainExtent() { return &m_swapChainExtent; }
	VkFormat* getSwapChainImageFormat() { return &m_swapChainImageFormat; }

private:
	Utilities* m_pUtilities = nullptr;
	VkDevice* m_pLogicalDevice = nullptr;
	PhysicalDevice* m_pPhysicalDevice = nullptr;
	GLFWwindow* m_pWindow = nullptr;
	VkSurfaceKHR* m_pSurface = nullptr;

	VkSwapchainKHR m_swapChain = nullptr;
	std::vector<VkImage> m_swapChainImages = {};
	VkFormat m_swapChainImageFormat = VK_FORMAT_UNDEFINED;
	VkExtent2D m_swapChainExtent = {};
	std::vector<VkImageView> m_swapChainImageViews = {};

	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
};

