#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <vector>

#include "Utilities.h"
#include "Devices.h"


class UniformBufferObject
{
public:

	struct sUniformBufferObject
	{
		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 proj;
	};

	UniformBufferObject(LogicalDevice* pLogicalDevice)
		: m_pLogicalDevice(pLogicalDevice->getLogicalDevice()), m_pPhysicalDevice(pLogicalDevice->getPhysicalDevice()->getPhysicalDevice()),
		m_pUtilities(Utilities::getInstance())
	{
		createUniformBuffers();
	};

	void createUniformBuffers();
	
	void cleanup();


	std::vector<void*>* getUniformBuffersMapped() { return &m_uniformBuffersMapped; };

private:
	VkDevice* m_pLogicalDevice = nullptr;
	VkPhysicalDevice* m_pPhysicalDevice = nullptr;
	Utilities* m_pUtilities = nullptr;
	int m_MAX_FRAMES_IN_FLIGHT = 1;


	std::vector<VkBuffer> m_uniformBuffers = {};
	std::vector<VkDeviceMemory> m_uniformBuffersMemory = {};
	std::vector<void*> m_uniformBuffersMapped = {};
};

