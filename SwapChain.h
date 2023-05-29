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

	void createSwapchain();
	void createImageViews();
	void createFramebuffers(VkRenderPass* pRenderPass);
	void recreateSwapchain();

	VkSwapchainKHR* getSwapchain() { return &m_swapchain; }
	VkExtent2D* getSwapchainExtent() { return &m_swapchainExtent; }
	VkFormat* getSwapchainImageFormat() { return &m_swapchainImageFormat; }
	std::vector<VkFramebuffer>* getSwapchainFramebuffers() { return &m_swapchainFramebuffers; }

private:
	Utilities* m_pUtilities = nullptr;
	VkDevice* m_pLogicalDevice = nullptr;
	PhysicalDevice* m_pPhysicalDevice = nullptr;
	GLFWwindow* m_pWindow = nullptr;
	VkSurfaceKHR* m_pSurface = nullptr;
	VkRenderPass* m_pRenderPass = nullptr;

	VkSwapchainKHR m_swapchain = nullptr;
	std::vector<VkImage> m_swapchainImages = {};
	VkFormat m_swapchainImageFormat = VK_FORMAT_UNDEFINED;
	VkExtent2D m_swapchainExtent = {};
	std::vector<VkImageView> m_swapchainImageViews = {};
	std::vector<VkFramebuffer> m_swapchainFramebuffers = {};

	bool m_firstRun = true;

	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
};

