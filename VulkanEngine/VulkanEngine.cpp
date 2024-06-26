#include "VulkanEngine.h"


VulkanEngine* VulkanEngine::m_pInstance = nullptr;


VulkanEngine* VulkanEngine::getInstance()
{
	//nativeDebugPrint("Instance requested", true);
	if (m_pInstance == nullptr)
	{
		m_pInstance = new VulkanEngine();
		nativeDebugPrint("Instance created");
	}
	//nativeDebugPrint("Instance retrieved");
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
		//delete m_pInstance;
		//m_pInstance = nullptr;
		//nativeDebugPrint("Instance destroyed");
	}
}


VulkanEngine::VulkanEngine() {}

void enableInputProcessing() {}

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

	m_settings = settings;
	m_MAX_FRAMES_IN_FLIGHT = m_settings->graphicsSettings.maxFramesInFlight;

	mDebugPrint("Maximum frames in flight: " + std::to_string(m_MAX_FRAMES_IN_FLIGHT) + "\n");

	Image::m_pGraphicsSettings = &m_settings->graphicsSettings;

	mDebugPrint("Creating window...");
	m_pWindow = new Window();

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
	m_pDebugMessenger->setupDebugMessenger(&m_vkInstance, m_settings->debugSettings.debugMode);

	// Surface
	m_pWindow->createSurface();
	m_pVkSurface = m_pWindow->getSurface();

	// Devices
	m_pPhysicalDevice = new PhysicalDevice();
	m_pVkPhysicalDevice = m_pPhysicalDevice->getVkPhysicalDevice();

	validateSettings(); // Ensure device supports current settings

	m_pLogicalDevice = new LogicalDevice();
	m_pVkDevice = m_pLogicalDevice->getVkDevice();
	Image::m_pLogicalDevice = m_pVkDevice;

	// Buffer Manager
	m_pBufferManager = new BufferManager();
	Image::m_pBufferManager = m_pBufferManager;


	// Command buffer
	m_pBufferManager->m_pCommandBuffer = new CommandBuffer(m_pBufferManager);

	// Create blocks
	std::vector<Vertex> loadedVertices;
	std::vector<uint32_t> loadedIndices;
	for (size_t i = 1; i < 4; i++) {
		glm::vec3 newPos = {};
		newPos.x = 1.0f * i;
		newPos.y = 1.0f * i;
		mDebugPrint(std::format("Creating block at position ({}, {}, {})", newPos.x, newPos.y, newPos.z));

		Block* newBlock = new Block(newPos, "textures/image.png");
		newBlock->buildModel();
		m_pLoadedBlocks.push_back(newBlock);
		
		std::vector<Vertex> blockVertices = newBlock->getVertices();
		std::vector<uint32_t> blockIndices = newBlock->getIndices();

		loadedVertices.reserve(loadedVertices.size() + blockVertices.size()); // Preallocate memory
		loadedVertices.insert(loadedVertices.end(), blockVertices.begin(), blockVertices.end());
		loadedIndices.reserve(loadedIndices.size() + blockIndices.size()); // Preallocate memory
		loadedIndices.insert(loadedIndices.end(), blockIndices.begin(), blockIndices.end());
	}
	//m_pTestModel = new Model("models/DTO_Crate.obj", "textures/DTO_Crate_tex_Diffuse.png");


	if (loadedVertices.size() == 0) {
		std::runtime_error("Tried to start without any loaded vertices.");
	}
	if (loadedIndices.size() == 0) {
		std::runtime_error("Tried to start with empty vertex index array.");
	}

	// Vertex buffer
	m_pBufferManager->m_pVertexBuffer = new VertexBuffer(m_pBufferManager, loadedVertices, loadedIndices);

	// Command buffer must be created seperately
	m_pBufferManager->m_pCommandBuffer->createCommandBuffers();

	// Swapchain
	m_pSwapchain = new Swapchain();
	m_pBufferManager->m_pSwapchain = m_pSwapchain;

	// Initialise depth buffer
	m_pBufferManager->m_pDepthBuffer = new DepthBuffer(m_pBufferManager);


	// Graphics pipeline
	m_pGraphicsPipeline = new GraphicsPipeline();
	m_pBufferManager->m_pGraphicsPipeline = m_pGraphicsPipeline->getGraphicsPipeline();
	m_pBufferManager->m_pRenderPass = m_pGraphicsPipeline->getRenderPass();
	m_pBufferManager->m_pDescriptorSetLayout = m_pGraphicsPipeline->getDescriptorSetLayout();
	m_pBufferManager->m_pPipelineLayout = m_pGraphicsPipeline->getVkPipelineLayout();

	// Initialise other buffers
	m_pBufferManager->m_pFramebuffer = new Framebuffer(m_pBufferManager);
	m_pBufferManager->m_pUniformBufferObject = new UniformBufferObject(m_pBufferManager);
	m_pBufferManager->m_pDescriptorSets = new DescriptorSets(m_pBufferManager);


	// Texture Image
	m_pTextureImage = new Image("textures/image.png");

	m_pBufferManager->m_pDescriptorSets->createDescriptorSets(m_pTextureImage->getVkTextureImageView(), m_pTextureImage->getVkTextureSampler());

	// Sync objects
	m_pWindow->createSyncObjects();

}

void VulkanEngine::createInstance()
{
	mDebugPrint("Creating Vulkan instance...");

	if (m_settings->debugSettings.debugMode && !checkValidationLayerSupport())
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
	mDebugPrint("Cleaning up sync objects...");
	m_pWindow->cleanupSyncObjects();

	//mDebugPrint("Cleaning up buffers...");
	//m_pBufferManager->cleanup();

	mDebugPrint("Cleaning up command buffer...");
	m_pBufferManager->m_pCommandBuffer->cleanup();
	delete m_pBufferManager->m_pCommandBuffer;

	mDebugPrint("Cleaning up uniform buffer objects...");
	m_pBufferManager->m_pUniformBufferObject->cleanup();
	delete m_pBufferManager->m_pUniformBufferObject;

	mDebugPrint("Cleaning up graphics pipeline...");
	m_pGraphicsPipeline->cleanup();
	delete m_pGraphicsPipeline;

	mDebugPrint("Cleaning up swapchain...");
	m_pSwapchain->cleanup();
	delete m_pSwapchain;

	mDebugPrint("Cleaning up loaded blocks...");
	for (size_t i = 3; i > 0; i--) {
		Block* block = m_pLoadedBlocks.at(i-1);
		block->cleanup();
		delete block;
	}

	mDebugPrint("Cleaning up texture image...");
	m_pTextureImage->cleanup();
	delete m_pTextureImage;

	mDebugPrint("Cleaning up descriptor sets...");
	m_pBufferManager->m_pDescriptorSets->cleanup();
	delete m_pBufferManager->m_pDescriptorSets;

	mDebugPrint("Cleaning up vertex buffer...");
	m_pBufferManager->m_pVertexBuffer->cleanup();
	delete m_pBufferManager->m_pVertexBuffer;
	delete m_pBufferManager;

	mDebugPrint("Cleaning up logical device...");
	m_pLogicalDevice->cleanup();
	delete m_pLogicalDevice;

	if (m_settings->debugSettings.debugMode)
	{
		mDebugPrint("Cleaning up debug messenger...");
		m_pDebugMessenger->cleanup();
		delete m_pDebugMessenger;
	}

	mDebugPrint("Cleaning up surface...");
	m_pWindow->cleanupSurface();

	mDebugPrint("Cleaning up Vulkan instance...");
	vkDestroyInstance(m_vkInstance, nullptr);

	mDebugPrint("Cleaning up window...");
	m_pWindow->cleanupWindow();
	delete m_pWindow;
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

void VulkanEngine::validateSettings()
{
	mDebugPrint("Validating settings...");

	int settingsChanged = 0;

	VkPhysicalDeviceProperties properties{};
	vkGetPhysicalDeviceProperties(*m_pVkPhysicalDevice, &properties);
	VkPhysicalDeviceFeatures features{};
	vkGetPhysicalDeviceFeatures(*m_pVkPhysicalDevice, &features);

	// Check if the device supports anisotropic filtering
	if (properties.limits.maxSamplerAnisotropy < m_settings->graphicsSettings.anisotropyLevel)
	{
		mDebugPrint(std::format("Anisotropic filtering level of x{} is not supported by the device. Setting to x{}.", m_settings->graphicsSettings.anisotropyLevel, properties.limits.maxSamplerAnisotropy));
		m_settings->graphicsSettings.anisotropyLevel = properties.limits.maxSamplerAnisotropy;
		settingsChanged++;
	}

	// Check if device supports wireframe rendering
	if (!features.fillModeNonSolid)
	{
		mDebugPrint("Wireframe rendering is not supported by the device. Setting to false.");
		m_settings->graphicsSettings.wireframe = false;
		settingsChanged++;
	}
	if (!features.wideLines)
	{
		mDebugPrint("Wide lines are not supported by the device. Setting wireframe thickness to 1px.");
		m_settings->graphicsSettings.wireframeThickness = 1.0f;
		settingsChanged++;
	}

	if (!features.sampleRateShading) {
		mDebugPrint("Sample rate shading is not supported by the device. Disabling multisampling.");
		m_settings->graphicsSettings.multisampling = false;
		settingsChanged++;
	}

	settingsChanged != 1 ? mDebugPrint(std::format("Settings validated with {} changes.", settingsChanged)) : mDebugPrint("Settings validated with 1 change.");
}
