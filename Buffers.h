#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <array>
#include <vector>
#include <stdexcept>

#include "Utilities.h"
#include "Swapchain.h"
#include "Devices.h"
#include "GraphicsPipeline.h"


#define mfDebugPrint(x) m_pBufferManager->m_pUtilities->debugPrint(x,this)


class CommandBuffer;
class VertexBuffer;
class UniformBufferObject;
class DescriptorSets;


class BufferManager
{
public:
	BufferManager(LogicalDevice* pLogicalDevice, VkSurfaceKHR* pSurface, GraphicsPipeline* pGraphicsPipeline, Swapchain* pSwapchain, int MAX_FRAMES_IN_FLIGHT)
		: m_pLogicalDevice(pLogicalDevice->getLogicalDevice()), m_pPhysicalDevice(pLogicalDevice->getPhysicalDevice()->getPhysicalDevice()), m_pSurface(pSurface),
		m_pRenderPass(pGraphicsPipeline->getRenderPass()), m_pSwapchain(pSwapchain), m_MAX_FRAMES_IN_FLIGHT(MAX_FRAMES_IN_FLIGHT), m_pGraphicsPipeline(pGraphicsPipeline->getGraphicsPipeline()),
		m_pGraphicsQueue(pLogicalDevice->getGraphicsQueue()), m_pDescriptorSetLayout(pGraphicsPipeline->getDescriptorSetLayout()), m_pPipelineLayout(pGraphicsPipeline->getVkPipelineLayout()),
		m_pUtilities(Utilities::getInstance())
	{};

	void initBuffers();

	// Don't use this, clean up each one individually to avoid nullptr errors.
	void cleanup();

	CommandBuffer* getCommandBuffer() { return m_pCommandBuffer; }
	VertexBuffer* getVertexBuffer() { return m_pVertexBuffer; }
	UniformBufferObject* getUniformBufferObject() { return m_pUniformBufferObject; }
	DescriptorSets* getDescriptorSets() { return m_pDescriptorSets; }

private:
	VkDevice* m_pLogicalDevice = nullptr;
	VkPhysicalDevice* m_pPhysicalDevice = nullptr;
	VkSurfaceKHR* m_pSurface = nullptr;
	VkRenderPass* m_pRenderPass = nullptr;
	Swapchain* m_pSwapchain = nullptr;
	VkPipeline* m_pGraphicsPipeline = nullptr;
	VkPipelineLayout* m_pPipelineLayout = nullptr;
	Utilities* m_pUtilities = nullptr;
	int m_MAX_FRAMES_IN_FLIGHT = 1;
	VkQueue* m_pGraphicsQueue = nullptr;
	VkDescriptorSetLayout* m_pDescriptorSetLayout = nullptr;


	CommandBuffer* m_pCommandBuffer = nullptr;
	VertexBuffer* m_pVertexBuffer = nullptr;
	UniformBufferObject* m_pUniformBufferObject = nullptr;
	DescriptorSets* m_pDescriptorSets = nullptr;

	friend class VulkanEngine;
	friend class CommandBuffer;
	friend class VertexBuffer;
	friend class UniformBufferObject;
	friend class DescriptorSets;

	
	void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& pBuffer, VkDeviceMemory& pDeviceMemory);
	void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
};







class CommandBuffer
{
public:
	/*
	CommandBuffer(VkDevice* pLogicalDevice, VkPhysicalDevice* pPhysicalDevice, VkSurfaceKHR* pSurface, VkRenderPass* pRenderPass, Swapchain* pSwapchain, VkPipeline* pGraphicsPipeline)
		: m_pLogicalDevice(pLogicalDevice), m_pPhysicalDevice(pPhysicalDevice), m_pSurface(pSurface), m_pRenderPass(pRenderPass), m_pSwapchain(pSwapchain), m_pGraphicsPipeline(pGraphicsPipeline),
		m_pUtilities(Utilities::getInstance())
	{
		createCommandPool();
	};
	*/
	CommandBuffer(BufferManager* pBufferManager) : m_pBufferManager(pBufferManager)
	{
		createCommandPool();
	};

	void createCommandPool();
	void createCommandBuffers();
	void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

	void cleanup();

	VkCommandPool* getVkCommandPool() { return &m_commandPool; }
	std::vector<VkCommandBuffer>* getCommandBuffers() { return &m_commandBuffers; }

private:
	BufferManager* m_pBufferManager = nullptr;

	VkCommandPool m_commandPool = VK_NULL_HANDLE;
	std::vector<VkCommandBuffer> m_commandBuffers = {};
};







class VertexBuffer
{
public:
	struct sVertex {
		glm::vec2 pos;
		glm::vec3 color;

		static VkVertexInputBindingDescription getBindingDescription()
		{
			VkVertexInputBindingDescription bindingDescription{
				.binding = 0,
				.stride = sizeof(sVertex),
				.inputRate = VK_VERTEX_INPUT_RATE_VERTEX
			};

			return bindingDescription;
		}

		static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions()
		{
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
	static const std::vector<uint16_t> indices;


	/*
	VertexBuffer(LogicalDevice* pLogicalDevice, VkCommandPool* pCommandPool)
		: m_pLogicalDevice(pLogicalDevice->getLogicalDevice()), m_pPhysicalDevice(pLogicalDevice->getPhysicalDevice()->getPhysicalDevice()),
		m_pGraphicsQueue(pLogicalDevice->getGraphicsQueue()), m_pCommandPool(pCommandPool), m_pUtilities(Utilities::getInstance())
	{
		createVertexBuffer(); createIndexBuffer();
	};
	*/
	VertexBuffer(BufferManager* pBufferManager) : m_pBufferManager(pBufferManager)
	{
		createVertexBuffer();
		createIndexBuffer();
	};


	void createVertexBuffer();
	void createIndexBuffer();

	void cleanup();

	VkBuffer* getVkVertexBuffer() { return &m_vertexBuffer; }
	VkBuffer* getVkIndexBuffer() { return &m_indexBuffer; }

private:
	BufferManager* m_pBufferManager = nullptr;

	VkBuffer m_vertexBuffer = VK_NULL_HANDLE;
	VkDeviceMemory m_vertexBufferMemory = VK_NULL_HANDLE;
	VkBuffer m_indexBuffer = VK_NULL_HANDLE;
	VkDeviceMemory m_indexBufferMemory = VK_NULL_HANDLE;
};






class UniformBufferObject
{
public:
	struct sUniformBufferObject
	{
		alignas(16) glm::mat4 model;
		alignas(16) glm::mat4 view;
		alignas(16) glm::mat4 proj;
	};

	UniformBufferObject(BufferManager* pBufferManager) : m_pBufferManager(pBufferManager)
	{
		createUniformBuffers();
	};

	void createUniformBuffers();

	void cleanup();


	std::vector<VkBuffer>* getUniformBuffers() { return &m_uniformBuffers; };
	std::vector<void*>* getUniformBuffersMapped() { return &m_uniformBuffersMapped; };

private:
	BufferManager* m_pBufferManager = nullptr;


	std::vector<VkBuffer> m_uniformBuffers = {};
	std::vector<VkDeviceMemory> m_uniformBuffersMemory = {};
	std::vector<void*> m_uniformBuffersMapped = {};
};









class DescriptorSets
{
public:
	DescriptorSets(BufferManager* pBufferManager) : m_pBufferManager(pBufferManager)
	{
		createDescriptorPool();
		createDescriptorSets();
	};

	void createDescriptorPool();
	void createDescriptorSets();

	void cleanup();

	std::vector<VkDescriptorSet>* getVkDescriptorSets() { return &m_descriptorSets; }

private:
	BufferManager* m_pBufferManager = nullptr;


	VkDescriptorPool m_descriptorPool = VK_NULL_HANDLE;
	std::vector<VkDescriptorSet> m_descriptorSets = {};
};