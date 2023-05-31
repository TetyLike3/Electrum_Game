#include "UniformBufferObject.h"

void UniformBufferObject::createUniformBuffers()
{
	VkDeviceSize bufferSize = sizeof(UniformBufferObject);

	m_uniformBuffers.resize(m_MAX_FRAMES_IN_FLIGHT);
	m_uniformBuffersMemory.resize(m_MAX_FRAMES_IN_FLIGHT);
	m_uniformBuffersMapped.resize(m_MAX_FRAMES_IN_FLIGHT);

	for (size_t i = 0; i < m_MAX_FRAMES_IN_FLIGHT; i++) {
		m_pUtilities->createBuffer(m_pLogicalDevice, m_pPhysicalDevice, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_uniformBuffers[i], m_uniformBuffersMemory[i]);

		vkMapMemory(*m_pLogicalDevice, m_uniformBuffersMemory[i], 0, bufferSize, 0, &m_uniformBuffersMapped[i]);
	}
}

void UniformBufferObject::cleanup()
{
	for (size_t i = 0; i < m_MAX_FRAMES_IN_FLIGHT; i++) {
		vkDestroyBuffer(*m_pLogicalDevice, m_uniformBuffers[i], nullptr);
		vkFreeMemory(*m_pLogicalDevice, m_uniformBuffersMemory[i], nullptr);
	}
}