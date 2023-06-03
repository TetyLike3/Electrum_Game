#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan_core.h>

#include <string>
#include <iostream>
#include <chrono>
#include <format>
#include <fstream>

// Macro to print debug messages with class name argument autofilled
#define mDebugPrint(x) m_pUtilities->debugPrint(x, this)


struct sSettings {
	struct sWindowSettings {
		const char* title = "ElectrumGame";
		uint32_t width = 1280;
		uint32_t height = 720;
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
		bool wireframe = true;
		float wireframeThickness = 2.0f;
		bool multisampling = false;
		VkBool32 anisotropicFiltering = false;
		float anisotropyLevel = 4.0f;
	} graphicsSettings;
};


// Singleton class for utility functions
class Utilities
{
public:
	static Utilities* getInstance();

	template<typename CLASSNAME>
	// Prints a debug message with the class name and timestamp.
	inline void debugPrint(std::string message, CLASSNAME* that) { iDebugPrint(message, std::string(typeid(that).name())); };
	inline void debugPrint(std::string message, std::string className) { iDebugPrint(message, className); };

	inline std::string getVkAPIVersionString(uint32_t version)
	{
		using std::string, std::to_string;
		return to_string(VK_API_VERSION_MAJOR(version)) += string(".") += to_string(VK_API_VERSION_MINOR(version)) += string(".") += to_string(VK_API_VERSION_PATCH(version));
	};

	// Generates a timestamp in the format HH:MM:SS.mmm
	std::string generateTimestamp_HH_MM_SS_mmm();


	std::vector<char> readFile(const std::string& filename);

	// Compiles shaders in the given folder
	//void compileShaders(std::filesystem::path folderPath);

private:
	Utilities();

	static Utilities* m_pInstance;

	static std::string m_lastClassPrinted;
	static std::string m_lastMessagePrinted;


	inline void iDebugPrint(std::string message, std::string className)
	{
		using std::string, std::format, std::cerr, std::setw;

		string timestamp = generateTimestamp_HH_MM_SS_mmm();

		// Truncate the "* __ptr64" at the end of the class name
		if (className.find("* __ptr64") != string::npos)
			className = className.substr(0, className.find("* __ptr64"));

		// Print class name only once
		if (m_lastClassPrinted != className)
		{
			m_lastClassPrinted = className;
			cerr << format("\nVulkanEngine \x1b[31;49;1mDEBUG\x1b[39;49m - \x1b[32;49m{}\x1b[39;49m -\n", m_lastClassPrinted);
		};

		m_lastMessagePrinted = format("\x1b[36;49m[{}]\x1b[39;49m - {}\n", generateTimestamp_HH_MM_SS_mmm(), message);
		cerr << m_lastMessagePrinted;
	}
};

