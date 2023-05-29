#pragma once

#include <iostream>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "Utilities.h"
#include "CommandBuffer.h"
#include "Swapchain.h"


const uint32_t WIDTH = 1280;
const uint32_t HEIGHT = 720;

class Window
{
public:
	Window(VkInstance* pVkInstance, int MAX_FRAMES_IN_FLIGHT) : m_pVkInstance(pVkInstance), m_MAX_FRAMES_IN_FLIGHT(MAX_FRAMES_IN_FLIGHT), m_pUtilities(Utilities::getInstance()) {};

	void initWindow(sSettings::sWindowSettings* windowSettings);
	void createSurface();
	void createSyncObjects(LogicalDevice* pLogicalDevice, Swapchain* pSwapchain, CommandBuffer* pCommandBuffer);

	void mainLoop();

	void cleanupSyncObjects();
	void cleanupSurface();
	void cleanupWindow();

	static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

	GLFWwindow* getWindow() { return m_pWindow; }
	VkSurfaceKHR* getSurface() { return &m_surface; }

	bool m_framebufferResized = false;
private:
	VkInstance* m_pVkInstance = nullptr;
	Utilities* m_pUtilities = nullptr;
	VkDevice* m_pLogicalDevice = nullptr;
	VkQueue* m_pGraphicsQueue = nullptr;
	Swapchain* m_pSwapchain = nullptr;
	CommandBuffer* m_pCommandBuffer = nullptr;

	GLFWwindow* m_pWindow = nullptr;
	VkSurfaceKHR m_surface = nullptr;
	std::vector<VkSemaphore> m_imageAvailableSemaphores = {};
	std::vector<VkSemaphore> m_renderFinishedSemaphores = {};
	std::vector<VkFence> m_inFlightFences = {};
	int m_MAX_FRAMES_IN_FLIGHT = 0;
	uint32_t m_currentFrame = 0;


	void drawFrame();
};

