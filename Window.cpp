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


void Window::mainLoop()
{
	while (!glfwWindowShouldClose(m_pWindow))
	{
		glfwPollEvents();
	}
	mDebugPrint("Window closed.");
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
