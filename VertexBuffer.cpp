#include "VertexBuffer.h"



const std::vector<VertexBuffer::sVertex> VertexBuffer::vertices = {
	{{0.0f, -0.5f}, {1.0f, 1.0f, 1.0f}},
	{{0.5f, 0.5f}, {0.2f, 0.0f, 0.8f}},
	{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
};


void VertexBuffer::createVertexBuffer()
{
	VkBufferCreateInfo bufferInfo{
		.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		.size = sizeof(vertices[0]) * vertices.size(),
		.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE
	};

	if (vkCreateBuffer(*m_pLogicalDevice, &bufferInfo, nullptr, &vertexBuffer) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create vertex buffer.");
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(*m_pLogicalDevice, vertexBuffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo{
		.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		.allocationSize = memRequirements.size,
		.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
	};

	if (vkAllocateMemory(*m_pLogicalDevice, &allocInfo, nullptr, &vertexBufferMemory) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to allocate vertex buffer memory.");
	}

	vkBindBufferMemory(*m_pLogicalDevice, vertexBuffer, vertexBufferMemory, 0);

	void* data;
	vkMapMemory(*m_pLogicalDevice, vertexBufferMemory, 0, bufferInfo.size, 0, &data);
	memcpy(data, vertices.data(), (size_t)bufferInfo.size);
	vkUnmapMemory(*m_pLogicalDevice, vertexBufferMemory);
}


uint32_t VertexBuffer::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(*m_pPhysicalDevice, &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		if (typeFilter & (1 << i) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}

	throw std::runtime_error("failed to find suitable memory type!");
}



void VertexBuffer::cleanup()
{
	vkDestroyBuffer(*m_pLogicalDevice, vertexBuffer, nullptr);
	vkFreeMemory(*m_pLogicalDevice, vertexBufferMemory, nullptr);
}