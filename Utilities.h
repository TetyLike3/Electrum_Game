#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan_core.h>

#include <string>
#include <iostream>
#include <chrono>
#include <format>
#include <filesystem>
#include <fstream>

// Macro to print debug messages with class name argument autofilled
#define mDebugPrint(x) m_pUtilities->debugPrint(x, this)


struct sSettings {
	struct sWindowSettings {
		const char* windowName = "IndustryChunks";
		uint32_t windowWidth = 1280;
		uint32_t windowHeight = 720;
	} windowSettings;
	struct sDebugSettings {
		#ifdef NDEBUG
			bool debugMode = false;
		#else
			bool debugMode = true;
		#endif
		std::vector<const char*> validationLayers = {
			"VK_LAYER_KHRONOS_validation"
		};
		bool enableValidationLayers = true;
	} debugSettings;
	struct sGraphicsSettings {
		bool tripleBuffering = true;
		bool vsync = true;
		bool rasterizerDepthClamp = false;
		bool wireframe = false;
		bool multisampling = false;
	} graphicsSettings;
};


// Singleton class for utility functions
class Utilities
{
public:
	static Utilities* getInstance();

	template<typename CLASSNAME>
	// Prints a debug message with the class name and timestamp.
	void debugPrint(std::string message, CLASSNAME* that);

	inline std::string getVkAPIVersionString(uint32_t version)
	{
		using std::string, std::to_string;
		return to_string(VK_API_VERSION_MAJOR(version)) += string(".") += to_string(VK_API_VERSION_MINOR(version)) += string(".") += to_string(VK_API_VERSION_PATCH(version));
	}

	// Generates a timestamp in the format HH:MM:SS.mmm
	std::string generateTimestamp_HH_MM_SS_mmm();


	std::vector<char> readFile(const std::string& filename);

	// Compiles shaders in the given folder
	void compileShaders(std::filesystem::path folderPath);
private:
	Utilities();

	static Utilities* m_pInstance;

	static std::string lastClassPrinted;
	static std::string lastMessagePrinted;
};

