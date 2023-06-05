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
#include "GraphicsPipeline.h"
#include "Buffers.h"
#include "Image.h"


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

	static DebugMessenger* m_pDebugMessenger;
	Image* m_pTextureImage = nullptr;


	std::map<std::string, uint32_t> m_versions = {};


	void initVulkan();
	void createInstance();
	void mainLoop();
	void cleanup();

	bool checkValidationLayerSupport();
	std::vector<const char*> getRequiredExtensions();
	void validateSettings();
};

inline void nativeDebugPrint(std::string message, bool newLine = false) { std::cout << (newLine ? "\n" : "") << "VulkanEngine NATIVEDEBUG - " << message << std::endl; }