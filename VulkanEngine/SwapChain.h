#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "Utilities.h"
#include "Devices.h"
#include "Image.h"
#include "Buffers.h"


class BufferManager;

class Swapchain
{
public:
	Swapchain(VkDevice* pLogicalDevice, PhysicalDevice* pPhysicalDevice, GLFWwindow* pWindow,VkSurfaceKHR* pSurface)
		: m_pLogicalDevice(pLogicalDevice), m_pPhysicalDevice(pPhysicalDevice), m_pWindow(pWindow), m_pSurface(pSurface), m_pUtilities(Utilities::getInstance())
	{
		createSwapchain();
		createImageViews();
	};

	void cleanup();

	void createSwapchain();
	void createImageViews();
	void recreateSwapchain(GLFWwindow* pWindow);

	VkSwapchainKHR* getSwapchain() { return &m_swapchain; }
	VkExtent2D* getSwapchainExtent() { return &m_swapchainExtent; }
	VkFormat* getSwapchainImageFormat() { return &m_swapchainImageFormat; }
	std::vector<VkImageView>* getSwapchainImageViews() { return &m_swapchainImageViews; }

	void setBufferManager(BufferManager* pBufferManager) { m_pBufferManager = pBufferManager; }

private:
	Utilities* m_pUtilities = nullptr;
	VkDevice* m_pLogicalDevice = nullptr;
	PhysicalDevice* m_pPhysicalDevice = nullptr;
	GLFWwindow* m_pWindow = nullptr;
	VkSurfaceKHR* m_pSurface = nullptr;
	VkRenderPass* m_pRenderPass = nullptr;
	BufferManager* m_pBufferManager = nullptr;

	VkSwapchainKHR m_swapchain = nullptr;
	std::vector<VkImage> m_swapchainImages = {};
	VkFormat m_swapchainImageFormat = VK_FORMAT_UNDEFINED;
	VkExtent2D m_swapchainExtent = {};
	std::vector<VkImageView> m_swapchainImageViews = {};

	bool m_firstRun = true;

	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
};

