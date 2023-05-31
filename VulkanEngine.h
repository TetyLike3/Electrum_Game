#pragma once

#include <iostream>
#include <string>
#include <map>
#include <vector>


#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "Utilities.h"
#include "Window.h"
#include "DebugMessenger.h"
#include "Devices.h"
#include "Swapchain.h"
#include "Surface.h"
#include "GraphicsPipeline.h"
#include "CommandBuffer.h"
#include "VertexBuffer.h"
#include "UniformBufferObject.h"


const int MAX_FRAMES_IN_FLIGHT = 2;


enum class VkEngineState
{
	NONE,
	INIT,
	RUNNING,
	CLEANUP,
	EXIT
};

class VulkanEngine
{
public:
	static VulkanEngine* getInstance();
	static void destroyInstance();

	void run(std::map<std::string,uint32_t> versions, sSettings* settings);

private:
	VulkanEngine();

	static VulkanEngine* m_pInstance;
	VkEngineState m_state = VkEngineState::NONE;

	Utilities* m_pUtilities = nullptr;

	Window* m_pWindow = nullptr;

	VkInstance m_vkInstance = VK_NULL_HANDLE;
	DebugMessenger* m_pDebugMessenger = nullptr;
	PhysicalDevice* m_pPhysicalDevice = nullptr;
	LogicalDevice* m_pLogicalDevice = nullptr;
	Swapchain* m_pSwapchain = nullptr;
	GraphicsPipeline* m_pGraphicsPipeline = nullptr;
	CommandBuffer* m_pCommandBuffer = nullptr;
	VertexBuffer* m_pVertexBuffer = nullptr;
	UniformBufferObject* m_pUniformBufferObject = nullptr;

	std::map<std::string, uint32_t> m_versions = {};
	sSettings* m_settings = nullptr;


	void initVulkan();
	void createInstance();
	void mainLoop();
	void cleanup();

	bool checkValidationLayerSupport();
	std::vector<const char*> getRequiredExtensions();
};

inline void nativeDebugPrint(std::string message, bool newLine = false) { std::cout << (newLine ? "\n" : "") << "VulkanEngine NATIVEDEBUG - " << message << std::endl; }