#pragma once

#include <iostream>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "Utilities.h"
#include "CommandBuffer.h"


const uint32_t WIDTH = 1280;
const uint32_t HEIGHT = 720;

class Window
{
public:
	Window(VkInstance* pVkInstance) : m_pVkInstance(pVkInstance), m_pUtilities(Utilities::getInstance()) {};

	void initWindow(sSettings::sWindowSettings* windowSettings);
	void createSurface();
	void createSyncObjects(LogicalDevice* pLogicalDevice, VkSwapchainKHR* pSwapchain, CommandBuffer* pCommandBuffer);

	void mainLoop();

	void cleanupSyncObjects();
	void cleanupSurface();
	void cleanupWindow();

	GLFWwindow* getWindow() { return m_pWindow; }
	VkSurfaceKHR* getSurface() { return &m_surface; }
private:
	VkInstance* m_pVkInstance = nullptr;
	Utilities* m_pUtilities = nullptr;
	VkDevice* m_pLogicalDevice = nullptr;
	VkQueue* m_pGraphicsQueue = nullptr;
	VkSwapchainKHR* m_pSwapchain = nullptr;
	CommandBuffer* m_pCommandBuffer = nullptr;

	GLFWwindow* m_pWindow = nullptr;
	VkSurfaceKHR m_surface = nullptr;
	VkSemaphore m_imageAvailableSemaphore = nullptr;
	VkSemaphore m_renderFinishedSemaphore = nullptr;
	VkFence m_inFlightFence = nullptr;


	void drawFrame();
};

