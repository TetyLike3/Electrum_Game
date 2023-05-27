#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <iostream>
#include <map>

#include "VulkanEngine.h"
#include "Utilities.h"

const std::map<std::string, uint32_t> versions = {
	{ "gameVersion", VK_MAKE_API_VERSION(0,1,0,0) },
	{ "engineVersion", VK_MAKE_API_VERSION(0,1,0,0) },
	{ "apiVersion", VK_MAKE_API_VERSION(0,1,0,0) }
};

/*
struct {
	struct {
		const char* windowName = "IndustryChunks";
		uint32_t windowWidth = 1280;
		uint32_t windowHeight = 720;
	} windowSettings;
	struct {
		bool debugMode = true;
		std::vector<const char*> validationLayers = {
			"VK_LAYER_KHRONOS_validation"
		};
		bool enableValidationLayers = true;
	} debugSettings;
	struct {
		bool tripleBuffering = true;
		bool vsync = true;
	} graphicsSettings;
} settings;
*/

sSettings* settings = new sSettings();



int main()
{
	VulkanEngine* pVulkanEngine = VulkanEngine::getInstance();

	// TODO: Compile shaders before running if needed

	try
	{
		pVulkanEngine -> run(versions, settings);
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	VulkanEngine::destroyInstance();

	std::cout << "Program ended successfully.\n";

	return EXIT_SUCCESS;
}