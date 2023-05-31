#include "VertexBuffer.h"



const std::vector<VertexBuffer::sVertex> VertexBuffer::vertices = {
	{{-0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}},
	{{0.5f, -0.5f}, {0.2f, 0.0f, 0.8f}},
	{{0.5f, 0.5f}, {0.2f, 0.5f, 0.8f}},
	{{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
};

const std::vector<uint16_t> VertexBuffer::indices = {
	0, 1, 2, 2, 3, 0
};


void VertexBuffer::createVertexBuffer()
{
	mDebugPrint("Creating vertex buffer...");

	VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	m_pUtilities->createBuffer(m_pLogicalDevice, m_pPhysicalDevice, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(*m_pLogicalDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, vertices.data(), (size_t)bufferSize);
	vkUnmapMemory(*m_pLogicalDevice, stagingBufferMemory);

	m_pUtilities->createBuffer(m_pLogicalDevice, m_pPhysicalDevice, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_vertexBuffer, m_vertexBufferMemory);

	copyBuffer(stagingBuffer, m_vertexBuffer, bufferSize);

	vkDestroyBuffer(*m_pLogicalDevice, stagingBuffer, nullptr);
	vkFreeMemory(*m_pLogicalDevice, stagingBufferMemory, nullptr);
}

void VertexBuffer::createIndexBuffer()
{
	mDebugPrint("Creating index buffer...");

	VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	m_pUtilities->createBuffer(m_pLogicalDevice, m_pPhysicalDevice, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(*m_pLogicalDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, indices.data(), (size_t)bufferSize);
	vkUnmapMemory(*m_pLogicalDevice, stagingBufferMemory);

	m_pUtilities->createBuffer(m_pLogicalDevice, m_pPhysicalDevice, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_indexBuffer, m_indexBufferMemory);

	copyBuffer(stagingBuffer, m_indexBuffer, bufferSize);

	vkDestroyBuffer(*m_pLogicalDevice, stagingBuffer, nullptr);
	vkFreeMemory(*m_pLogicalDevice, stagingBufferMemory, nullptr);
}

void VertexBuffer::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
	VkCommandBufferAllocateInfo allocInfo{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.commandPool = *m_pCommandPool,
		.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		.commandBufferCount = 1
	};

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(*m_pLogicalDevice, &allocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT
	};

	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	VkBufferCopy copyRegion{
		.srcOffset = 0,
		.dstOffset = 0,
		.size = size
	};

	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);
	
	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo{
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.commandBufferCount = 1,
		.pCommandBuffers = &commandBuffer
	};

	vkQueueSubmit(*m_pGraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(*m_pGraphicsQueue);
}



void VertexBuffer::cleanup()
{
	vkDestroyBuffer(*m_pLogicalDevice, m_indexBuffer, nullptr);
	vkFreeMemory(*m_pLogicalDevice, m_indexBufferMemory, nullptr);

	vkDestroyBuffer(*m_pLogicalDevice, m_vertexBuffer, nullptr);
	vkFreeMemory(*m_pLogicalDevice, m_vertexBufferMemory, nullptr);
}