#include "VulkanEngine.h"


VulkanEngine* VulkanEngine::m_pInstance = nullptr;


VulkanEngine* VulkanEngine::getInstance()
{
	nativeDebugPrint("Instance requested");
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
	nativeDebugPrint("Instance destruction requested");
	if (m_pInstance != nullptr)
	{
		nativeDebugPrint("Cleaning up...");
		m_pInstance->m_state = VkEngineState::CLEANUP;
		m_pInstance->cleanup();

		nativeDebugPrint("Exiting...");
		m_pInstance->m_state = VkEngineState::EXIT;
		delete m_pInstance;
		m_pInstance = nullptr;
		nativeDebugPrint("Instance destroyed");
	}
}


VulkanEngine::VulkanEngine() {}


void VulkanEngine::run(std::map<std::string, uint32_t> versions, sSettings* settings)
{
	m_pUtilities = Utilities::getInstance();

	mDebugPrint("Initialising...");
	m_state = VkEngineState::INIT;

	m_versions = versions;
	mDebugPrint("Engine version: " + m_pUtilities->getVkAPIVersionString(m_versions["engineVersion"]));
	mDebugPrint("Game version: " + m_pUtilities->getVkAPIVersionString(m_versions["gameVersion"]));
	mDebugPrint("API version: " + m_pUtilities->getVkAPIVersionString(m_versions["apiVersion"]));

	m_settings = settings;

	mDebugPrint("Creating window...");
	m_pWindow = new Window(&m_vkInstance);

	m_pWindow->initWindow(&settings->windowSettings);

	mDebugPrint("Initialising Vulkan...");
	initVulkan();

	mDebugPrint("Initialisation successful, running...");
	m_state = VkEngineState::RUNNING;
	mainLoop();
}



void VulkanEngine::initVulkan()
{
	createInstance();

	mDebugPrint("Creating debug messenger...");
	m_pDebugMessenger = new DebugMessenger();
	m_pDebugMessenger->setupDebugMessenger(&m_vkInstance, m_settings->debugSettings.debugMode);

	m_pWindow->createSurface();

	m_pPhysicalDevice = new PhysicalDevice(&m_vkInstance, m_pWindow->getSurface());
	m_pPhysicalDevice->pickPhysicalDevice();

	m_pLogicalDevice = new LogicalDevice(m_pPhysicalDevice, m_pWindow->getSurface(), m_pWindow->getWindow());
	m_pLogicalDevice->createLogicalDevice(&m_settings->debugSettings);

	m_pSwapchain = new Swapchain(m_pLogicalDevice->getLogicalDevice(), m_pPhysicalDevice, m_pWindow->getWindow(), m_pWindow->getSurface());
	m_pSwapchain->createSwapChain();
	m_pSwapchain->createImageViews();

	m_pGraphicsPipeline = new GraphicsPipeline(m_pLogicalDevice->getLogicalDevice(), m_pSwapchain, &m_settings->graphicsSettings);
	m_pGraphicsPipeline->createRenderPass();
	m_pGraphicsPipeline->createGraphicsPipeline();
}

void VulkanEngine::createInstance()
{
	mDebugPrint("Creating Vulkan instance...");

	if (m_settings->debugSettings.debugMode && !checkValidationLayerSupport())
	{
		throw std::runtime_error("validation layers requested, but not available!");
	}

	mDebugPrint("Creating application info...");
	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "IndustryChunks";
	appInfo.applicationVersion = m_versions["gameVersion"];
	appInfo.pEngineName = "VertexEngine";
	appInfo.engineVersion = m_versions["engineVersion"];
	appInfo.apiVersion = m_versions["apiVersion"];

	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	mDebugPrint("Creating instance extensions...");
	auto extensions = getRequiredExtensions();
	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();

	if (m_settings->debugSettings.debugMode)
	{
		auto layerCount = static_cast<uint32_t>(m_settings->debugSettings.validationLayers.size());
		layerCount == 1 ? mDebugPrint("Enabling 1 validation layer...") : mDebugPrint(std::format("Enabling {} validation layer(s)...", layerCount));
		createInfo.enabledLayerCount = layerCount;
		createInfo.ppEnabledLayerNames = m_settings->debugSettings.validationLayers.data();
	}
	else
	{
		mDebugPrint("Validation layers not enabled.");
		createInfo.enabledLayerCount = 0;
	}

	if (vkCreateInstance(&createInfo, nullptr, &m_vkInstance) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create instance!");
	}
}




void VulkanEngine::mainLoop()
{
	m_pWindow->mainLoop();
}


void VulkanEngine::cleanup()
{
	mDebugPrint("Cleaning up graphics pipeline...");
	m_pGraphicsPipeline->cleanup();

	mDebugPrint("Cleaning up swapchain...");
	m_pSwapchain->cleanup();

	mDebugPrint("Cleaning up logical device...");
	m_pLogicalDevice->cleanup();

	if (m_settings->debugSettings.debugMode)
	{
		mDebugPrint("Cleaning up debug messenger...");
		m_pDebugMessenger->cleanup();
	}

	mDebugPrint("Cleaning up surface...");
	m_pWindow->cleanupSurface();

	mDebugPrint("Cleaning up Vulkan instance...");
	vkDestroyInstance(m_vkInstance, nullptr);

	mDebugPrint("Cleaning up window...");
	m_pWindow->cleanupWindow();
}




bool VulkanEngine::checkValidationLayerSupport()
{
	mDebugPrint("Checking validation layer support...");

	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for (const char* layerName : m_settings->debugSettings.validationLayers)
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

	if (m_settings->debugSettings.debugMode)
	{
		mDebugPrint("Enabling validation layer extensions...");
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	return extensions;
}