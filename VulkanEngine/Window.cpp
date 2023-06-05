#include "Window.h"



void Window::initWindow()
{
	mDebugPrint("Initializing window...");

	sSettings::sWindowSettings windowSettings = StaticMembers::getSettings()->windowSettings;

	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	//glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	m_pWindow = glfwCreateWindow(windowSettings.width, windowSettings.height, windowSettings.title, nullptr, nullptr);
	glfwSetWindowUserPointer(m_pWindow, this);
	glfwSetFramebufferSizeCallback(m_pWindow, framebufferResizeCallback);
}

void Window::framebufferResizeCallback(GLFWwindow* window, int width, int height)
{
	auto app = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
	app->m_framebufferResized = true;
	//app->drawFrame(); // Don't do this or it'll break the window when one of the coordinates is 0
}


void Window::createSurface()
{
	mDebugPrint("Creating surface...");

	if (glfwCreateWindowSurface(*m_pVkInstance, m_pWindow, nullptr, &m_surface) != VK_SUCCESS) {
		throw std::runtime_error("failed to create window surface!");
	}
}

void Window::createSyncObjects()
{
	mDebugPrint("Creating sync objects...");

	// Define these variables here since we need to use them from now on
	m_pLogicalDevice = StaticMembers::getVkDevice();
	m_pGraphicsQueue = StaticMembers::getLogicalDevice()->getGraphicsQueue();
	m_pSwapchain = StaticMembers::getSwapchain();
	m_pCommandBuffer = StaticMembers::getBufferManager()->getCommandBuffer();
	m_pUniformBufferObject = StaticMembers::getBufferManager()->getUniformBufferObject();

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

		// Print FPS
		calculateFPS();
	}
	mDebugPrint("Window closed, waiting for device idle...");

	vkDeviceWaitIdle(*m_pLogicalDevice);
}


void Window::drawFrame()
{
	uint32_t imageIndex;

	std::vector<VkCommandBuffer> commandBuffers = *m_pCommandBuffer->getCommandBuffers();

	vkWaitForFences(*m_pLogicalDevice, 1, &m_inFlightFences[m_currentFrame], VK_TRUE, UINT64_MAX);

	VkResult result = vkAcquireNextImageKHR(*m_pLogicalDevice, *m_pSwapchain->getSwapchain(), UINT64_MAX, m_imageAvailableSemaphores[m_currentFrame], VK_NULL_HANDLE, &imageIndex);

	// Ensure swapchain quality
	if (result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		m_pSwapchain->recreateSwapchain(m_pWindow);
		return;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
		throw std::runtime_error("failed to acquire swap chain image!");
	}

	updateUniformBuffer(m_currentFrame);

	vkResetFences(*m_pLogicalDevice, 1, &m_inFlightFences[m_currentFrame]);

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


	VkSwapchainKHR swapChains[] = { *m_pSwapchain->getSwapchain()};

	VkPresentInfoKHR presentInfo{
		.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = signalSemaphores,
		.swapchainCount = 1,
		.pSwapchains = swapChains,
		.pImageIndices = &imageIndex,
		.pResults = nullptr // Optional
	};

	result = vkQueuePresentKHR(*m_pGraphicsQueue, &presentInfo);

	// Ensure swapchain quality
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result != VK_SUBOPTIMAL_KHR  || m_framebufferResized)
	{
		m_framebufferResized = false;
		m_pSwapchain->recreateSwapchain(m_pWindow);
	}
	else if (result != VK_SUCCESS) {
		throw std::runtime_error("failed to present swap chain image!");
	}

	m_currentFrame = (m_currentFrame + 1) % m_MAX_FRAMES_IN_FLIGHT;
}

void Window::updateUniformBuffer(uint32_t currentImage)
{
	using std::chrono::high_resolution_clock, std::chrono::duration, std::chrono::seconds;

	static auto startTime = high_resolution_clock::now();

	auto currentTime = high_resolution_clock::now();
	float time = duration<float, seconds::period>(currentTime - startTime).count();


	VkExtent2D swapchainExtent = *m_pSwapchain->getSwapchainExtent();

	UniformBufferObject::sUniformBufferObject ubo{
		.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 1.2f, 1.0f)),
		.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
		.proj = glm::perspective(glm::radians(45.0f), swapchainExtent.width / (float)swapchainExtent.height, 0.1f, 10.0f)
	};
	ubo.proj[1][1] *= -1; // Flip the y axis to account for Vulkan's inverted y axis

	std::vector<void*> uniformBuffersMapped = *m_pUniformBufferObject->getUniformBuffersMapped(); // FUCK THIS PIECE OF CODE IN PARTICULAR
	memcpy(uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
}


void Window::calculateFPS()
{
	using std::string, std::to_string;
	double current = glfwGetTime();
	double delta = current - m_lastTime;
	m_frameCounter++;

	if (delta >= 1.0)
	{
		// Print the FPS with a precision of 2 d.p.
		string fpsString = to_string(m_frameCounter/delta);
		mDebugPrint("FPS: " + fpsString.substr(0, fpsString.find(".") + 3));
		m_frameCounter = 0;
		m_lastTime = current;
	}
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
