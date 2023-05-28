#include "Devices.h"



VkPhysicalDevice* PhysicalDevice::pickPhysicalDevice()
{
	mDebugPrint("Picking physical device...");

	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(*m_pVkInstance, &deviceCount, nullptr);

	if (deviceCount == 0)
	{
		throw std::runtime_error("Failed to find GPUs with Vulkan support!");
	}

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(*m_pVkInstance, &deviceCount, devices.data());

	for (const auto& device : devices)
	{
		if (isDeviceSuitable(device))
		{
			mDebugPrint("Found suitable device!");
			m_physicalDevice = device;
			break;
		}
	}

	if (m_physicalDevice == VK_NULL_HANDLE)
	{
		throw std::runtime_error("Failed to find a suitable GPU!");
	}

	return &m_physicalDevice;
}


bool PhysicalDevice::isDeviceSuitable(VkPhysicalDevice candidateDevice)
{
	mDebugPrint("Checking device suitability...");

	QueueFamilyIndices indices = findQueueFamilies(candidateDevice, *m_pSurface);
	bool extensionsSupported = checkDeviceExtensionSupport(candidateDevice);

	mDebugPrint("Extensions supported: " + std::to_string(extensionsSupported));

	bool swapChainAdequate = false;
	if (extensionsSupported)
	{
		SwapChainSupportDetails swapChainSupport = querySwapChainSupport(candidateDevice);
		swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
	}
	

	bool finalResult = indices.isComplete() && extensionsSupported && swapChainAdequate;
	mDebugPrint("Device suitable: " + std::to_string(finalResult));

	return finalResult;
}

// Returns whether the device supports all of the required extensions.
bool PhysicalDevice::checkDeviceExtensionSupport(VkPhysicalDevice candidateDevice)
{
	mDebugPrint("Checking device extension support...");

	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(candidateDevice, nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(candidateDevice, nullptr, &extensionCount, availableExtensions.data());

	std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

	for (const auto& extension : availableExtensions)
	{
		requiredExtensions.erase(extension.extensionName);
	}

	// Print out the extensions that the device is missing
	std::string debugOutput = "";

	requiredExtensions.empty() ? debugOutput = "All extensions supported!" : debugOutput = "Device missing extensions: ";
	for (const auto& extension : requiredExtensions)
	{
		debugOutput += extension + ", ";
	}
	mDebugPrint(debugOutput);

	return requiredExtensions.empty();
}

// Returns the details of the swap chain support of the device.
SwapChainSupportDetails PhysicalDevice::querySwapChainSupport(VkPhysicalDevice physicalDevice)
{
	mDebugPrint("Querying swap chain support...");

	SwapChainSupportDetails details;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, *m_pSurface, &details.capabilities);

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, *m_pSurface, &formatCount, nullptr);
	if (formatCount != 0)
	{
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, *m_pSurface, &formatCount, details.formats.data());
	}

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, *m_pSurface, &presentModeCount, nullptr);
	if (presentModeCount != 0)
	{
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, *m_pSurface, &presentModeCount, details.presentModes.data());
	}

	return details;
}





QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface)
{
	QueueFamilyIndices indices;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	int i = 0;
	for (const auto& queueFamily : queueFamilies)
	{
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			indices.graphicsFamily = i;
		}

		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

		if (presentSupport)
		{
			indices.presentFamily = i;
		}

		if (indices.isComplete())
		{
			break;
		}

		i++;
	}

	return indices;
}





void LogicalDevice::createLogicalDevice(sSettings::sDebugSettings* pDebugSettings)
{
	mDebugPrint("Creating logical device...");

	QueueFamilyIndices indices = findQueueFamilies(*m_pPhysicalDevice->getPhysicalDevice(), *m_pSurface);

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

	float queuePriority = 1.0f;
	for (uint32_t queueFamily : uniqueQueueFamilies)
	{
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	VkPhysicalDeviceFeatures deviceFeatures{};

	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos = queueCreateInfos.data();

	createInfo.pEnabledFeatures = &deviceFeatures;

	createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
	createInfo.ppEnabledExtensionNames = deviceExtensions.data();

	if (pDebugSettings->debugMode)
	{
		auto layerCount = static_cast<uint32_t>(pDebugSettings->validationLayers.size());
		layerCount == 1 ? mDebugPrint("Enabling 1 validation layer...") : mDebugPrint(std::format("Enabling {} validation layer(s)...", layerCount));
		createInfo.enabledLayerCount = layerCount;
		createInfo.ppEnabledLayerNames = pDebugSettings->validationLayers.data();
	}
	else {
		mDebugPrint("Validation layers not enabled.");
		createInfo.enabledLayerCount = 0;
	}

	if (vkCreateDevice(*m_pPhysicalDevice->getPhysicalDevice(), &createInfo, nullptr, &m_logicalDevice) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create logical device!");
	}

	vkGetDeviceQueue(m_logicalDevice, indices.graphicsFamily.value(), 0, &m_graphicsQueue);
	vkGetDeviceQueue(m_logicalDevice, indices.presentFamily.value(), 0, &m_presentQueue);
}

void LogicalDevice::cleanup()
{
	vkDestroyDevice(m_logicalDevice, nullptr);
}