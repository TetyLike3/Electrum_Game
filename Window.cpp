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

	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	if (vkCreateSemaphore(*m_pLogicalDevice, &semaphoreInfo, nullptr, &m_imageAvailableSemaphore) != VK_SUCCESS ||
		vkCreateSemaphore(*m_pLogicalDevice, &semaphoreInfo, nullptr, &m_renderFinishedSemaphore) != VK_SUCCESS ||
		vkCreateFence(*m_pLogicalDevice, &fenceInfo, nullptr, &m_inFlightFence) != VK_SUCCESS) {
		throw std::runtime_error("failed to create semaphores!");
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
	vkAcquireNextImageKHR(*m_pLogicalDevice, *m_pSwapchain, UINT64_MAX, m_imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

	vkWaitForFences(*m_pLogicalDevice, 1, &m_inFlightFence, VK_TRUE, UINT64_MAX);
	vkResetFences(*m_pLogicalDevice, 1, &m_inFlightFence);

	vkResetCommandBuffer(*m_pCommandBuffer->getCommandBuffer(), 0);
	m_pCommandBuffer->recordCommandBuffer(*m_pCommandBuffer->getCommandBuffer(),imageIndex);


	VkSemaphore waitSemaphores[] = { m_imageAvailableSemaphore };
	VkSemaphore signalSemaphores[] = { m_renderFinishedSemaphore };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = m_pCommandBuffer->getCommandBuffer();

	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	if (vkQueueSubmit(*m_pGraphicsQueue, 1, &submitInfo, m_inFlightFence) != VK_SUCCESS) {
		throw std::runtime_error("failed to submit draw command buffer!");
	}


	VkSwapchainKHR swapChains[] = { *m_pSwapchain };

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;
	presentInfo.pResults = nullptr; // Optional

	vkQueuePresentKHR(*m_pGraphicsQueue, &presentInfo);
}


void Window::cleanupSyncObjects()
{
	vkDestroySemaphore(*m_pLogicalDevice, m_renderFinishedSemaphore, nullptr);
	vkDestroySemaphore(*m_pLogicalDevice, m_imageAvailableSemaphore, nullptr);
	vkDestroyFence(*m_pLogicalDevice, m_inFlightFence, nullptr);
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
