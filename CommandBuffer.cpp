#include "CommandBuffer.h"


void CommandBuffer::createCommandPool()
{
	mDebugPrint("Creating command pool...");

	QueueFamilyIndices::sQueueFamilyIndices queueFamilyIndices = QueueFamilyIndices::findQueueFamilies(*m_pPhysicalDevice, *m_pSurface);

	VkCommandPoolCreateInfo poolInfo{
		.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		.pNext = nullptr,
		.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
		.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value()
	};

	if (vkCreateCommandPool(*m_pLogicalDevice, &poolInfo, nullptr, &m_commandPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create command pool!");
	}
}

void CommandBuffer::createCommandBuffers(int MAX_FRAMES_IN_FLIGHT, VkBuffer* pVertexBuffer, VkBuffer* pIndexBuffer)
{
	mDebugPrint("Creating command buffers...");

	m_pVertexBuffer = pVertexBuffer;
	m_pIndexBuffer = pIndexBuffer;

	m_commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

	VkCommandBufferAllocateInfo allocInfo{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.commandPool = m_commandPool,
		.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		.commandBufferCount = (uint32_t)m_commandBuffers.size()
	};

	if (vkAllocateCommandBuffers(*m_pLogicalDevice, &allocInfo, m_commandBuffers.data()) != VK_SUCCESS) {
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

	std::vector<VkFramebuffer> swapchainFramebuffers = *m_pSwapchain->getSwapchainFramebuffers();
	VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };

	VkRenderPassBeginInfo renderPassInfo{
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
		.renderPass = *m_pRenderPass,
		.framebuffer = swapchainFramebuffers[imageIndex],
		.renderArea {
			.offset = { 0, 0 },
			.extent = *m_pSwapchain->getSwapchainExtent(),
		},
		.clearValueCount = 1,
		.pClearValues = &clearColor
	};

	vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pGraphicsPipeline);

	VkViewport viewport{
		.x = 0.0f,
		.y = 0.0f,
		.width = static_cast<float>(m_pSwapchain->getSwapchainExtent()->width),
		.height = static_cast<float>(m_pSwapchain->getSwapchainExtent()->height),
		.minDepth = 0.0f,
		.maxDepth = 1.0f
	};
	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

	VkRect2D scissor{
		.offset = { 0, 0 },
		.extent = *m_pSwapchain->getSwapchainExtent()
	};
	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

	VkBuffer vertexBuffers[] = { *m_pVertexBuffer };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
	vkCmdBindIndexBuffer(commandBuffer, *m_pIndexBuffer, 0, VK_INDEX_TYPE_UINT16);

	vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(VertexBuffer::indices.size()), 1, 0, 0, 0);

	vkCmdEndRenderPass(commandBuffer);

	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to record command buffer!");
	}

	//mDebugPrint("Command buffer recorded.");
}


void CommandBuffer::cleanup()
{
	vkDestroyCommandPool(*m_pLogicalDevice, m_commandPool, nullptr);
}