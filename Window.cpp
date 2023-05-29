#include "Window.h"



void Window::initWindow(sSettings::sWindowSettings* windowSettings)
{
	mDebugPrint("Initializing window...");

	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	m_pWindow = glfwCreateWindow(windowSettings->windowWidth, windowSettings->windowHeight, windowSettings->windowName, nullptr, nullptr);
}

void Window::createSurface()
{
	mDebugPrint("Creating surface...");

	if (glfwCreateWindowSurface(*m_pVkInstance, m_pWindow, nullptr, &m_surface) != VK_SUCCESS) {
		throw std::runtime_error("failed to create window surface!");
	}
}

void Window::createSyncObjects(LogicalDevice* pLogicalDevice, VkSwapchainKHR* pSwapchain, CommandBuffer* pCommandBuffer)
{
	mDebugPrint("Creating sync objects...");

	// Define these variables here since we need to use them from now on
	m_pLogicalDevice = pLogicalDevice->getLogicalDevice();
	m_pGraphicsQueue = pLogicalDevice->getGraphicsQueue();
	m_pSwapchain = pSwapchain;
	m_pCommandBuffer = pCommandBuffer;

	// Resize the vectors to the correct size
	m_imageAvailableSemaphores.resize(m_MAX_FRAMES_IN_FLIGHT);
	m_renderFinishedSemaphores.resize(m_MAX_FRAMES_IN_FLIGHT);
	m_inFlightFences.resize(m_MAX_FRAMES_IN_FLIGHT);

	VkSemaphoreCreateInfo semaphoreInfo{
		.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO
	};

	VkFenceCreateInfo fenceInfo{
		.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		.flags = VK_FENCE_CREATE_SIGNALED_BIT
	};

	for (size_t i = 0; i < m_MAX_FRAMES_IN_FLIGHT; i++)
	{
		if (vkCreateSemaphore(*m_pLogicalDevice, &semaphoreInfo, nullptr, &m_imageAvailableSemaphores[i]) != VK_SUCCESS ||
			vkCreateSemaphore(*m_pLogicalDevice, &semaphoreInfo, nullptr, &m_renderFinishedSemaphores[i]) != VK_SUCCESS ||
			vkCreateFence(*m_pLogicalDevice, &fenceInfo, nullptr, &m_inFlightFences[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create synchronization objects for a frame!");
		}
	}
}


void Window::mainLoop()
{
	while (!glfwWindowShouldClose(m_pWindow))
	{
		glfwPollEvents();
		drawFrame();
	}
	mDebugPrint("Window closed, waiting for device idle...");

	vkDeviceWaitIdle(*m_pLogicalDevice);
}


void Window::drawFrame()
{
	uint32_t imageIndex;

	std::vector<VkCommandBuffer> commandBuffers = *m_pCommandBuffer->getCommandBuffers();

	vkWaitForFences(*m_pLogicalDevice, 1, &m_inFlightFences[m_currentFrame], VK_TRUE, UINT64_MAX);
	vkResetFences(*m_pLogicalDevice, 1, &m_inFlightFences[m_currentFrame]);

	vkAcquireNextImageKHR(*m_pLogicalDevice, *m_pSwapchain, UINT64_MAX, m_imageAvailableSemaphores[m_currentFrame], VK_NULL_HANDLE, &imageIndex);

	vkResetCommandBuffer(commandBuffers[m_currentFrame], 0);
	m_pCommandBuffer->recordCommandBuffer(commandBuffers[m_currentFrame],imageIndex);


	VkSemaphore waitSemaphores[] = { m_imageAvailableSemaphores[m_currentFrame] };
	VkSemaphore signalSemaphores[] = { m_renderFinishedSemaphores[m_currentFrame] };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

	VkSubmitInfo submitInfo{
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = waitSemaphores,
		.pWaitDstStageMask = waitStages,
		.commandBufferCount = 1,
		.pCommandBuffers = &commandBuffers[m_currentFrame],
		.signalSemaphoreCount = 1,
		.pSignalSemaphores = signalSemaphores
	};

	if (vkQueueSubmit(*m_pGraphicsQueue, 1, &submitInfo, m_inFlightFences[m_currentFrame]) != VK_SUCCESS) {
		throw std::runtime_error("failed to submit draw command buffer!");
	}


	VkSwapchainKHR swapChains[] = { *m_pSwapchain };

	VkPresentInfoKHR presentInfo{
		.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = signalSemaphores,
		.swapchainCount = 1,
		.pSwapchains = swapChains,
		.pImageIndices = &imageIndex,
		.pResults = nullptr // Optional
	};

	vkQueuePresentKHR(*m_pGraphicsQueue, &presentInfo);

	m_currentFrame = (m_currentFrame + 1) % m_MAX_FRAMES_IN_FLIGHT;
}


void Window::cleanupSyncObjects()
{
	for (size_t i = 0; i < m_MAX_FRAMES_IN_FLIGHT; i++) {
		vkDestroySemaphore(*m_pLogicalDevice, m_renderFinishedSemaphores[i], nullptr);
		vkDestroySemaphore(*m_pLogicalDevice, m_imageAvailableSemaphores[i], nullptr);
		vkDestroyFence(*m_pLogicalDevice, m_inFlightFences[i], nullptr);
	}
}

void Window::cleanupSurface()
{
	vkDestroySurfaceKHR(*m_pVkInstance, m_surface, nullptr);
}

void Window::cleanupWindow()
{
	glfwDestroyWindow(m_pWindow);
	glfwTerminate();
}
