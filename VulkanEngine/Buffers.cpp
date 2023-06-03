#include "Buffers.h"


//// ----------------------------------------------------- //
/// ------------------ Buffer Manager ------------------- //
// ----------------------------------------------------- //

void BufferManager::initBuffers()
{
	mDebugPrint("Initializing buffers...");
	
	m_pCommandBuffer = new CommandBuffer(this);

	m_pVertexBuffer = new VertexBuffer(this);

	m_pUniformBufferObject = new UniformBufferObject(this);
	m_pDescriptorSets = new DescriptorSets(this);

	m_pCommandBuffer->createCommandBuffers();

	mDebugPrint("Buffers initialized.");
}

void BufferManager::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& pBuffer, VkDeviceMemory& pBufferMemory)
{
	VkBufferCreateInfo bufferInfo{
	.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
	.size = size,
	.usage = usage,
	.sharingMode = VK_SHARING_MODE_EXCLUSIVE
	};

	if (vkCreateBuffer(*m_pLogicalDevice, &bufferInfo, nullptr, &pBuffer) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create buffer.");
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(*m_pLogicalDevice, pBuffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo{
		.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		.allocationSize = memRequirements.size,
		.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties)
	};

	if (vkAllocateMemory(*m_pLogicalDevice, &allocInfo, nullptr, &pBufferMemory) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to allocate vertex buffer memory.");
	}

	vkBindBufferMemory(*m_pLogicalDevice, pBuffer, pBufferMemory, 0);
};

void BufferManager::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
	VkCommandBuffer commandBuffer = m_pCommandBuffer->beginSingleTimeCommands();

	VkBufferCopy copyRegion{
		.size = size
	};

	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

	m_pCommandBuffer->endSingleTimeCommands(commandBuffer);
}

uint32_t BufferManager::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
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

void BufferManager::cleanup()
{
	mDebugPrint("Cleaning up command buffer...");
	m_pCommandBuffer->cleanup();
	delete m_pCommandBuffer;

	mDebugPrint("Cleaning up vertex buffer...");
	m_pVertexBuffer->cleanup();
	delete m_pVertexBuffer;

	mDebugPrint("Cleaning up uniform buffer object...");
	m_pUniformBufferObject->cleanup();
	delete m_pUniformBufferObject;

	mDebugPrint("Cleaning up descriptor sets...");
	m_pDescriptorSets->cleanup();
	delete m_pDescriptorSets;
}




//// ----------------------------------------------------- //
/// ------------------ Command Bufffer ------------------ //
// ----------------------------------------------------- //

void CommandBuffer::createCommandPool()
{
	mfDebugPrint("Creating command pool...");

	QueueFamilyIndices::sQueueFamilyIndices queueFamilyIndices = QueueFamilyIndices::findQueueFamilies(*m_pBufferManager->m_pPhysicalDevice, *m_pBufferManager->m_pSurface);

	VkCommandPoolCreateInfo poolInfo{
		.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		.pNext = nullptr,
		.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
		.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value()
	};

	if (vkCreateCommandPool(*m_pBufferManager->m_pLogicalDevice, &poolInfo, nullptr, &m_commandPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create command pool!");
	}
}

VkCommandBuffer CommandBuffer::beginSingleTimeCommands()
{
	VkCommandBufferAllocateInfo allocInfo{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.commandPool = m_commandPool,
		.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		.commandBufferCount = 1
	};

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(*m_pBufferManager->m_pLogicalDevice, &allocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT
	};

	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	return commandBuffer;
}

void CommandBuffer::endSingleTimeCommands(VkCommandBuffer commandBuffer)
{
	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo{
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.commandBufferCount = 1,
		.pCommandBuffers = &commandBuffer
	};

	vkQueueSubmit(*m_pBufferManager->m_pGraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(*m_pBufferManager->m_pGraphicsQueue);

	vkFreeCommandBuffers(*m_pBufferManager->m_pLogicalDevice, m_commandPool, 1, &commandBuffer);
}

void CommandBuffer::createCommandBuffers()
{
	mfDebugPrint("Creating command buffers...");

	m_commandBuffers.resize(m_pBufferManager->m_MAX_FRAMES_IN_FLIGHT);

	VkCommandBufferAllocateInfo allocInfo{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.commandPool = m_commandPool,
		.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		.commandBufferCount = (uint32_t)m_commandBuffers.size()
	};

	if (vkAllocateCommandBuffers(*m_pBufferManager->m_pLogicalDevice, &allocInfo, m_commandBuffers.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate command buffers!");
	}
}

void CommandBuffer::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex)
{
	//mDebugPrint("Recording command buffer...");

	VkCommandBufferBeginInfo beginInfo{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		.pNext = nullptr,
		.flags = 0, // Optional
		.pInheritanceInfo = nullptr // Optional
	};

	if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
		throw std::runtime_error("failed to begin recording command buffer!");
	}

	//mDebugPrint("Creating render pass...");

	std::vector<VkFramebuffer> swapchainFramebuffers = *m_pBufferManager->m_pSwapchain->getSwapchainFramebuffers();
	VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };

	VkRenderPassBeginInfo renderPassInfo{
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
		.renderPass = *m_pBufferManager->m_pRenderPass,
		.framebuffer = swapchainFramebuffers[imageIndex],
		.renderArea {
			.offset = { 0, 0 },
			.extent = *m_pBufferManager->m_pSwapchain->getSwapchainExtent(),
		},
		.clearValueCount = 1,
		.pClearValues = &clearColor
	};

	vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pBufferManager->m_pGraphicsPipeline);

	VkViewport viewport{
		.x = 0.0f,
		.y = 0.0f,
		.width = static_cast<float>(m_pBufferManager->m_pSwapchain->getSwapchainExtent()->width),
		.height = static_cast<float>(m_pBufferManager->m_pSwapchain->getSwapchainExtent()->height),
		.minDepth = 0.0f,
		.maxDepth = 1.0f
	};
	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

	VkRect2D scissor{
		.offset = { 0, 0 },
		.extent = *m_pBufferManager->m_pSwapchain->getSwapchainExtent()
	};
	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

	VkBuffer vertexBuffers[] = { *m_pBufferManager->m_pVertexBuffer->getVkVertexBuffer() };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
	vkCmdBindIndexBuffer(commandBuffer, *m_pBufferManager->m_pVertexBuffer->getVkIndexBuffer(), 0, VK_INDEX_TYPE_UINT16);
	
	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pBufferManager->m_pPipelineLayout, 0, 1, &(*m_pBufferManager->m_pDescriptorSets->getVkDescriptorSets())[imageIndex], 0, nullptr);
	vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(VertexBuffer::indices.size()), 1, 0, 0, 0);

	vkCmdEndRenderPass(commandBuffer);

	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to record command buffer!");
	}

	//mDebugPrint("Command buffer recorded.");
}


void CommandBuffer::cleanup()
{
	vkDestroyCommandPool(*m_pBufferManager->m_pLogicalDevice, m_commandPool, nullptr);
}








//// ----------------------------------------------------- //
/// ------------------- Vertex Buffer ------------------- //
// ----------------------------------------------------- //


std::vector<VertexBuffer::sVertex> VertexBuffer::vertices = {
	{{-0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f}, 0.0f},
	{{0.5f, -0.5f}, {0.2f, 0.0f, 0.8f}, {0.0f, 0.0f}, 0.0f},
	{{0.5f, 0.5f}, {0.2f, 0.5f, 0.8f}, {0.0f, 1.0f}, 0.0f},
	{{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}, 0.0f}
};

const std::vector<uint16_t> VertexBuffer::indices = {
	0, 1, 2, 2, 3, 0
};


void VertexBuffer::createVertexBuffer()
{
	mfDebugPrint("Creating vertex buffer...");
	
	// TODO: Make this work lol
	// Change the last values in the vertices vector to blend the texture with the vertex colors
	for (auto& vertex : vertices)
	{
		vertex.colorBlendTex = m_pBufferManager->m_pSettings->graphicsSettings.colorBlendTexture; // Implicit bool -> float :)
	}

	VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	m_pBufferManager->createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(*m_pBufferManager->m_pLogicalDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, vertices.data(), (size_t)bufferSize);
	vkUnmapMemory(*m_pBufferManager->m_pLogicalDevice, stagingBufferMemory);

	m_pBufferManager->createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_vertexBuffer, m_vertexBufferMemory);

	m_pBufferManager->copyBuffer(stagingBuffer, m_vertexBuffer, bufferSize);

	vkDestroyBuffer(*m_pBufferManager->m_pLogicalDevice, stagingBuffer, nullptr);
	vkFreeMemory(*m_pBufferManager->m_pLogicalDevice, stagingBufferMemory, nullptr);
}

void VertexBuffer::createIndexBuffer()
{
	mfDebugPrint("Creating index buffer...");

	VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	m_pBufferManager->createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(*m_pBufferManager->m_pLogicalDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, indices.data(), (size_t)bufferSize);
	vkUnmapMemory(*m_pBufferManager->m_pLogicalDevice, stagingBufferMemory);

	m_pBufferManager->createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_indexBuffer, m_indexBufferMemory);

	m_pBufferManager->copyBuffer(stagingBuffer, m_indexBuffer, bufferSize);

	vkDestroyBuffer(*m_pBufferManager->m_pLogicalDevice, stagingBuffer, nullptr);
	vkFreeMemory(*m_pBufferManager->m_pLogicalDevice, stagingBufferMemory, nullptr);
}


void VertexBuffer::cleanup()
{
	vkDestroyBuffer(*m_pBufferManager->m_pLogicalDevice, m_indexBuffer, nullptr);
	vkFreeMemory(*m_pBufferManager->m_pLogicalDevice, m_indexBufferMemory, nullptr);

	vkDestroyBuffer(*m_pBufferManager->m_pLogicalDevice, m_vertexBuffer, nullptr);
	vkFreeMemory(*m_pBufferManager->m_pLogicalDevice, m_vertexBufferMemory, nullptr);
}








//// ----------------------------------------------------- //
/// ------------------ Uniform Buffers ------------------ //
// ----------------------------------------------------- //


void UniformBufferObject::createUniformBuffers()
{
	mfDebugPrint("Creating uniform buffers...");

	VkDeviceSize bufferSize = sizeof(sUniformBufferObject);

	m_uniformBuffers.resize(m_pBufferManager->m_MAX_FRAMES_IN_FLIGHT);
	m_uniformBuffersMemory.resize(m_pBufferManager->m_MAX_FRAMES_IN_FLIGHT);
	m_uniformBuffersMapped.resize(m_pBufferManager->m_MAX_FRAMES_IN_FLIGHT);

	for (size_t i = 0; i < m_pBufferManager->m_MAX_FRAMES_IN_FLIGHT; i++) {
		m_pBufferManager->createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_uniformBuffers[i], m_uniformBuffersMemory[i]);

		vkMapMemory(*m_pBufferManager->m_pLogicalDevice, m_uniformBuffersMemory[i], 0, bufferSize, 0, &m_uniformBuffersMapped[i]);
	}

	m_uniformBuffersMapped.resize(m_pBufferManager->m_MAX_FRAMES_IN_FLIGHT);
}

void UniformBufferObject::cleanup()
{
	for (size_t i = 0; i < m_pBufferManager->m_MAX_FRAMES_IN_FLIGHT; i++) {
		vkDestroyBuffer(*m_pBufferManager->m_pLogicalDevice, m_uniformBuffers[i], nullptr);
		vkFreeMemory(*m_pBufferManager->m_pLogicalDevice, m_uniformBuffersMemory[i], nullptr);
	}
}






//// ----------------------------------------------------- //
/// ------------------ Descriptor Sets ------------------ //
// ----------------------------------------------------- //


void DescriptorSets::createDescriptorPool()
{
	mfDebugPrint("Creating descriptor pool...");

	std::array<VkDescriptorPoolSize, 2> poolSizes{
		VkDescriptorPoolSize{
			.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			.descriptorCount = static_cast<uint32_t>(m_pBufferManager->m_MAX_FRAMES_IN_FLIGHT)
		},
			VkDescriptorPoolSize{
			.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
			.descriptorCount = static_cast<uint32_t>(m_pBufferManager->m_MAX_FRAMES_IN_FLIGHT)
		},
		/* Remember to change the array size when adding this
		* 
			VkDescriptorPoolSize{
			.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
			.descriptorCount = static_cast<uint32_t>(m_pBufferManager->m_MAX_FRAMES_IN_FLIGHT)
		}
		*/
	};

	VkDescriptorPoolCreateInfo poolInfo{
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
		.maxSets = static_cast<uint32_t>(m_pBufferManager->m_MAX_FRAMES_IN_FLIGHT),
		.poolSizeCount = static_cast<uint32_t>(poolSizes.size()),
		.pPoolSizes = poolSizes.data()
	};

	if (vkCreateDescriptorPool(*m_pBufferManager->m_pLogicalDevice, &poolInfo, nullptr, &m_descriptorPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor pool!");
	}
}

void DescriptorSets::createDescriptorSets(VkImageView* pImageView, VkSampler* pImageSampler)
{
	mfDebugPrint("Creating descriptor sets...");

	std::vector<VkDescriptorSetLayout> layouts(m_pBufferManager->m_MAX_FRAMES_IN_FLIGHT, *m_pBufferManager->m_pDescriptorSetLayout);
	VkDescriptorSetAllocateInfo allocInfo{
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
		.descriptorPool = m_descriptorPool,
		.descriptorSetCount = static_cast<uint32_t>(m_pBufferManager->m_MAX_FRAMES_IN_FLIGHT),
		.pSetLayouts = layouts.data()
	};

	m_descriptorSets.resize(m_pBufferManager->m_MAX_FRAMES_IN_FLIGHT);
	if (vkAllocateDescriptorSets(*m_pBufferManager->m_pLogicalDevice, &allocInfo, m_descriptorSets.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate descriptor sets!");
	}

	for (size_t i = 0; i < m_pBufferManager->m_MAX_FRAMES_IN_FLIGHT; i++)
	{
		VkDescriptorBufferInfo bufferInfo{
			.buffer = (*m_pBufferManager->m_pUniformBufferObject->getUniformBuffers())[i],
			.offset = 0,
			.range = sizeof(UniformBufferObject::sUniformBufferObject)
		};

		VkDescriptorImageInfo imageInfo{
			.sampler = *pImageSampler,
			.imageView = *pImageView,
			.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
		};

		/*
		VkDescriptorImageInfo heightmapInfo{
			.sampler = *pHeightmapSampler,
			.imageView = *pHeightmapView,
			.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
		};
		*/

		std::array<VkWriteDescriptorSet, 2> descriptorWrites{
			VkWriteDescriptorSet{
				.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
				.dstSet = m_descriptorSets[i],
				.dstBinding = 0,
				.dstArrayElement = 0,
				.descriptorCount = 1,
				.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				.pBufferInfo = &bufferInfo
			},
				VkWriteDescriptorSet{
				.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
				.dstSet = m_descriptorSets[i],
				.dstBinding = 1,
				.dstArrayElement = 0,
				.descriptorCount = 1,
				.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
				.pImageInfo = &imageInfo
			}
		};

		vkUpdateDescriptorSets(*m_pBufferManager->m_pLogicalDevice, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
	}
}

void DescriptorSets::cleanup()
{
	vkDestroyDescriptorPool(*m_pBufferManager->m_pLogicalDevice, m_descriptorPool, nullptr);
}