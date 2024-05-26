#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <iostream>
#include <map>
#include <thread>

#include "VulkanEngine/VulkanEngine.h"
#include "VulkanEngine/Utilities/Utilities.h"

const std::map<std::string, uint32_t> versions = {
	{ "gameVersion", VK_MAKE_API_VERSION(0,0,8,0) },
	{ "engineVersion", VK_MAKE_API_VERSION(0,0,8,0) },
	{ "apiVersion", VK_MAKE_API_VERSION(0,1,0,0) }
};

sSettings settings{
	.windowSettings {
		.title = "ElectrumGame",
		.width = 1280,
		.height = 720,
	},
	.debugSettings {
		#ifdef NDEBUG
		.debugMode = false,
		.validationLayers = {},
		.enableValidationLayers = false,
		#else
		.debugMode = true,
		.validationLayers = {
			"VK_LAYER_KHRONOS_validation"
		},
		.enableValidationLayers = true
		#endif
	},
	.graphicsSettings {
		.maxFramesInFlight = 1,
		.enabledFeatures = {
			.sampleRateShading = true,
			.fillModeNonSolid = true,
			.wideLines = true,
			.samplerAnisotropy = true
		},
		.tripleBuffering = true,
		.vsync = false,
		.maxFramerate = 0,
		.rasterizerDepthClamp = false,
		.wireframe = false,
		.wireframeThickness = 8.0f,
		.multisampling = true,
		.anisotropicFiltering = true,
		.anisotropyLevel = 16.0f,
		.colorBlendTexture = true,
	}
};


void runVulkanEngine(VulkanEngine *pVulkanEngine)
{
	try
	{
		pVulkanEngine->run(versions, &settings);
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		system("pause");
	}

	VulkanEngine::destroyInstance();
}

void enableInputProcessing(VulkanEngine *pVulkanEngine)
{
	Window* pWindow = pVulkanEngine->getWindow();

	while (pVulkanEngine->getState() == VkEngineState::RUNNING) {
		// Process inputs
		if (glfwGetKey(pWindow->getWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS) {
			glfwSetWindowShouldClose(pWindow->getWindow(), GLFW_TRUE);
		}

		// Sleep for a bit to reduce CPU usage
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}


int main()
{
	VulkanEngine* pVulkanEngine = VulkanEngine::getInstance();

	// TODO: Compile shaders before running if needed

	std::thread renderThread(runVulkanEngine, pVulkanEngine);

	// Wait for rendering engine to start running
	while (pVulkanEngine->getState() != VkEngineState::RUNNING) {
	}

	// Allow inputs to be processed
	std::thread inputThread(enableInputProcessing, pVulkanEngine);

	// Wait for rendering engine to clean up before exiting
	while (pVulkanEngine->getState() != VkEngineState::EXIT) {
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	renderThread.join();
	inputThread.join();

	std::cout << "Program ended successfully.\n";
	system("pause");

	return EXIT_SUCCESS;
}