#pragma once


#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <array>
#include <vector>
#include <stdexcept>

#include "Utilities.h"
#include "Devices.h"
#include "CommandBuffer.h"




class VertexBuffer
{
public:
	struct sVertex {
		glm::vec2 pos;
		glm::vec3 color;

		static VkVertexInputBindingDescription getBindingDescription() {
			VkVertexInputBindingDescription bindingDescription{
				.binding = 0,
				.stride = sizeof(sVertex),
				.inputRate = VK_VERTEX_INPUT_RATE_VERTEX
			};

			return bindingDescription;
		}

		static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
			std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};
			attributeDescriptions[0].binding = 0;
			attributeDescriptions[0].location = 0;
			attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
			attributeDescriptions[0].offset = offsetof(sVertex, pos);

			attributeDescriptions[1].binding = 0;
			attributeDescriptions[1].location = 1;
			attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[1].offset = offsetof(sVertex, color);

			return attributeDescriptions;
		}
	};

	static const std::vector<sVertex> vertices;



	VertexBuffer(LogicalDevice* pLogicalDevice, VkCommandPool* pCommandPool)
		: m_pLogicalDevice(pLogicalDevice->getLogicalDevice()), m_pPhysicalDevice(pLogicalDevice->getPhysicalDevice()->getPhysicalDevice()), m_pGraphicsQueue(pLogicalDevice->getGraphicsQueue()), m_pCommandPool(pCommandPool), m_pUtilities(Utilities::getInstance())
	{ createVertexBuffer(); };

	
	void createVertexBuffer();

	void cleanup();

	VkBuffer* getVkBuffer() { return &vertexBuffer; }

private:
	VkDevice* m_pLogicalDevice = nullptr;
	VkPhysicalDevice* m_pPhysicalDevice = nullptr;
	VkQueue* m_pGraphicsQueue = nullptr;
	VkCommandPool* m_pCommandPool = nullptr;
	Utilities* m_pUtilities = nullptr;

	VkBuffer vertexBuffer = VK_NULL_HANDLE;
	VkDeviceMemory vertexBufferMemory = VK_NULL_HANDLE;


	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
	void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
	void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
};