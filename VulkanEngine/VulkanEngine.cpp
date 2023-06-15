#include "StaticMembers.h"

#include "VulkanEngine.h"


VulkanEngine* VulkanEngine::m_pInstance = nullptr;


VulkanEngine* VulkanEngine::getInstance()
{
	nativeDebugPrint("Instance requested", true);
	if (m_pInstance == nullptr)
	{
		m_pInstance = new VulkanEngine();
		nativeDebugPrint("Instance created");
	}
	nativeDebugPrint("Instance retrieved");
	return m_pInstance;
}

void VulkanEngine::destroyInstance()
{
	nativeDebugPrint("Instance destruction requested", true);
	if (m_pInstance != nullptr)
	{
		nativeDebugPrint("Cleaning up...");
		m_pInstance->m_state = VkEngineState::CLEANUP;
		m_pInstance->cleanup();

		nativeDebugPrint("Exiting...", true);
		m_pInstance->m_state = VkEngineState::EXIT;
		delete m_pInstance;
		m_pInstance = nullptr;
		nativeDebugPrint("Instance destroyed");
	}
}


VulkanEngine::VulkanEngine() {}


DebugMessenger* VulkanEngine::m_pDebugMessenger = nullptr;



void VulkanEngine::run(std::map<std::string, uint32_t> versions, sSettings* settings)
{
	m_pUtilities = Utilities::getInstance();

	mDebugPrint("Initialising...");
	m_state = VkEngineState::INIT;

	m_versions = versions;
	mDebugPrint("Engine version: " + m_pUtilities->getVkAPIVersionString(m_versions["engineVersion"]));
	mDebugPrint("Game version: " + m_pUtilities->getVkAPIVersionString(m_versions["gameVersion"]));
	mDebugPrint("API version: " + m_pUtilities->getVkAPIVersionString(m_versions["apiVersion"]));
	mDebugPrint("Maximum frames in flight: " + std::to_string(MAX_FRAMES_IN_FLIGHT) + "\n");

	StaticMembers::m_settings = settings;

	mDebugPrint("Creating window...");
	StaticMembers::m_pWindow = new Window();

	mDebugPrint("Initialising Vulkan...");
	initVulkan();

	mDebugPrint("Initialisation successful, running...\n");
	m_state = VkEngineState::RUNNING;
	mainLoop();
}



void VulkanEngine::initVulkan()
{
	createInstance();

	// Debug messenger
	mDebugPrint("Creating debug messenger...");
	m_pDebugMessenger = new DebugMessenger();
	m_pDebugMessenger->setupDebugMessenger(StaticMembers::getVkInstance(), StaticMembers::getSettings()->debugSettings.debugMode);

	// Surface
	StaticMembers::m_pWindow->createSurface();
	StaticMembers::m_pVkSurface = StaticMembers::m_pWindow->getSurface();

	// Devices
	StaticMembers::m_pPhysicalDevice = new PhysicalDevice();
	StaticMembers::m_pVkPhysicalDevice = StaticMembers::m_pPhysicalDevice->getVkPhysicalDevice();

	validateSettings(); // Ensure device supports current settings

	StaticMembers::m_pLogicalDevice = new LogicalDevice();
	StaticMembers::m_pVkDevice = StaticMembers::m_pLogicalDevice->getVkDevice();


	// Swapchain
	StaticMembers::m_pSwapchain = new Swapchain();

	// Graphics pipeline
	StaticMembers::m_pGraphicsPipeline = new GraphicsPipeline();


	// Buffer Manager
	StaticMembers::m_pBufferManager = new BufferManager();
	StaticMembers::m_pBufferManager->initBuffers();

	// Texture Image
	m_pTextureImage = new Image("textures/image.png");

	StaticMembers::m_pBufferManager->m_pDescriptorSets->createDescriptorSets(m_pTextureImage->getVkTextureImageView(), m_pTextureImage->getVkTextureSampler());

	// Command buffer must be created seperately
	StaticMembers::m_pBufferManager->m_pCommandBuffer->createCommandBuffers();

	// Sync objects
	StaticMembers::m_pWindow->createSyncObjects();

}

void VulkanEngine::createInstance()
{
	mDebugPrint("Creating Vulkan instance...");

	if (StaticMembers::m_settings->debugSettings.debugMode && !checkValidationLayerSupport())
	{
		throw std::runtime_error("validation layers requested, but not available!");
	}

	mDebugPrint("Creating application info...");
	VkApplicationInfo appInfo{
		.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pApplicationName = "ElectrumGame",
		.applicationVersion = m_versions["gameVersion"],
		.pEngineName = "VertexEngine",
		.engineVersion = m_versions["engineVersion"],
		.apiVersion = m_versions["apiVersion"]
	};

	VkInstanceCreateInfo createInfo{
		.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		.pApplicationInfo = &appInfo
	};

	mDebugPrint("Creating instance extensions...");
	auto extensions = getRequiredExtensions();
	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();

	if (StaticMembers::m_settings->debugSettings.debugMode)
	{
		auto layerCount = static_cast<uint32_t>(StaticMembers::m_settings->debugSettings.validationLayers.size());
		layerCount == 1 ? mDebugPrint("Enabling 1 validation layer...") : mDebugPrint(std::format("Enabling {} validation layer(s)...", layerCount));
		createInfo.enabledLayerCount = layerCount;
		createInfo.ppEnabledLayerNames = StaticMembers::m_settings->debugSettings.validationLayers.data();
	}
	else
	{
		mDebugPrint("Validation layers not enabled.");
		createInfo.enabledLayerCount = 0;
	}

	if (vkCreateInstance(&createInfo, nullptr, StaticMembers::getVkInstance()) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create instance!");
	}
}




void VulkanEngine::mainLoop()
{
	StaticMembers::m_pWindow->mainLoop();
}


void VulkanEngine::cleanup()
{
	mDebugPrint("Cleaning up sync objects...");
	StaticMembers::m_pWindow->cleanupSyncObjects();

	//mDebugPrint("Cleaning up buffers...");
	//m_pBufferManager->cleanup();

	mDebugPrint("Cleaning up command buffer...");
	StaticMembers::m_pBufferManager->m_pCommandBuffer->cleanup();
	delete StaticMembers::m_pBufferManager->m_pCommandBuffer;

	mDebugPrint("Cleaning up uniform buffer objects...");
	StaticMembers::m_pBufferManager->m_pUniformBufferObject->cleanup();
	delete StaticMembers::m_pBufferManager->m_pUniformBufferObject;

	mDebugPrint("Cleaning up graphics pipeline...");
	StaticMembers::m_pGraphicsPipeline->cleanup();
	delete StaticMembers::m_pGraphicsPipeline;

	mDebugPrint("Cleaning up swapchain...");
	StaticMembers::m_pSwapchain->cleanup();
	delete StaticMembers::m_pSwapchain;

	mDebugPrint("Cleaning up texture image...");
	m_pTextureImage->cleanup();
	delete m_pTextureImage;

	mDebugPrint("Cleaning up descriptor sets...");
	StaticMembers::m_pBufferManager->m_pDescriptorSets->cleanup();
	delete StaticMembers::m_pBufferManager->m_pDescriptorSets;

	mDebugPrint("Cleaning up vertex buffer...");
	StaticMembers::m_pBufferManager->m_pVertexBuffer->cleanup();
	delete StaticMembers::m_pBufferManager->m_pVertexBuffer;
	delete StaticMembers::m_pBufferManager;

	mDebugPrint("Cleaning up logical device...");
	StaticMembers::m_pLogicalDevice->cleanup();
	delete StaticMembers::m_pLogicalDevice;

	if (StaticMembers::m_settings->debugSettings.debugMode)
	{
		mDebugPrint("Cleaning up debug messenger...");
		m_pDebugMessenger->cleanup();
		delete m_pDebugMessenger;
	}

	mDebugPrint("Cleaning up surface...");
	StaticMembers::m_pWindow->cleanupSurface();

	mDebugPrint("Cleaning up Vulkan instance...");
	vkDestroyInstance(StaticMembers::m_vkInstance, nullptr);

	mDebugPrint("Cleaning up window...");
	StaticMembers::m_pWindow->cleanupWindow();
	delete StaticMembers::m_pWindow;
}




bool VulkanEngine::checkValidationLayerSupport()
{
	mDebugPrint("Checking validation layer support...");

	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for (const char* layerName : StaticMembers::m_settings->debugSettings.validationLayers)
	{
		bool layerFound = false;

		for (const auto& layerProperties : availableLayers)
		{
			if (strcmp(layerName, layerProperties.layerName) == 0)
			{

				mDebugPrint(std::format("Found validation layer: {}", layerName));
				layerFound = true;
				break;
			}
		}

		if (!layerFound)
		{
			mDebugPrint("Validation layer not found");
			return false;
		}
	}

	return true;
}

std::vector<const char*> VulkanEngine::getRequiredExtensions()
{
	mDebugPrint("Getting required extensions...");
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	if (StaticMembers::m_settings->debugSettings.debugMode)
	{
		mDebugPrint("Enabling validation layer extensions...");
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	return extensions;
}

void VulkanEngine::validateSettings()
{
	mDebugPrint("Validating settings...");

	int settingsChanged = 0;

	VkPhysicalDeviceProperties properties{};
	vkGetPhysicalDeviceProperties(*StaticMembers::m_pVkPhysicalDevice, &properties);
	VkPhysicalDeviceFeatures features{};
	vkGetPhysicalDeviceFeatures(*StaticMembers::m_pVkPhysicalDevice, &features);

	// Check if the device supports anisotropic filtering
	if (properties.limits.maxSamplerAnisotropy < StaticMembers::m_settings->graphicsSettings.anisotropyLevel)
	{
		mDebugPrint(std::format("Anisotropic filtering level of x{} is not supported by the device. Setting to x{}.", StaticMembers::m_settings->graphicsSettings.anisotropyLevel, properties.limits.maxSamplerAnisotropy));
		StaticMembers::m_settings->graphicsSettings.anisotropyLevel = properties.limits.maxSamplerAnisotropy;
		settingsChanged++;
	}

	// Check if device supports wireframe rendering
	if (!features.fillModeNonSolid)
	{
		mDebugPrint("Wireframe rendering is not supported by the device. Setting to false.");
		StaticMembers::m_settings->graphicsSettings.wireframe = false;
		settingsChanged++;
	}
	if (!features.wideLines)
	{
		mDebugPrint("Wide lines are not supported by the device. Setting wireframe thickness to 1px.");
		StaticMembers::m_settings->graphicsSettings.wireframeThickness = 1.0f;
		settingsChanged++;
	}

	settingsChanged != 1 ? mDebugPrint(std::format("Settings validated with {} changes.", settingsChanged)) : mDebugPrint("Settings validated with 1 change.");
}