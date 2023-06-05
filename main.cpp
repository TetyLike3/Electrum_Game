#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <iostream>
#include <map>

#include "VulkanEngine/VulkanEngine.h"
#include "VulkanEngine/Utilities.h"

const std::map<std::string, uint32_t> versions = {
	{ "gameVersion", VK_MAKE_API_VERSION(0,0,0,6) },
	{ "engineVersion", VK_MAKE_API_VERSION(0,1,0,6) },
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
		.enabledFeatures = {
			.fillModeNonSolid = true,
			.wideLines = true,
			.samplerAnisotropy = true
		},
		.tripleBuffering = true,
		.vsync = true,
		.rasterizerDepthClamp = false,
		.wireframe = false,
		.wireframeThickness = 4.0f,
		.multisampling = false,
		.anisotropicFiltering = true,
		.anisotropyLevel = 16.0f,
		.colorBlendTexture = true,
	}
};



int main()
{
	VulkanEngine* pVulkanEngine = VulkanEngine::getInstance();

	// TODO: Compile shaders before running if needed

	try
	{
		pVulkanEngine -> run(versions, &settings);
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		system("pause");
		return EXIT_FAILURE;
	}

	VulkanEngine::destroyInstance();

	std::cout << "Program ended successfully.\n";
	system("pause");

	return EXIT_SUCCESS;
}