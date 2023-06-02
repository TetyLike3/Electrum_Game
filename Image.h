#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>

#include "Utilities.h"
#include "Devices.h"
#include "Buffers.h"


class Image
{
public:
	Image(std::string imagePath, LogicalDevice* pLogicalDevice, BufferManager* pBufferManager)
		: m_imagePath(imagePath), m_pLogicalDevice(pLogicalDevice->getLogicalDevice()), m_pBufferManager(pBufferManager), m_pUtilities(Utilities::getInstance())
	{
		createTextureImage();
	};

	void createTextureImage();
	void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
	void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
	void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

	void cleanup();

private:
	Utilities* m_pUtilities = nullptr;
	VkDevice* m_pLogicalDevice = nullptr;
	BufferManager* m_pBufferManager = nullptr;


	std::string m_imagePath = "";
	VkImage m_textureImage = VK_NULL_HANDLE;
	VkDeviceMemory m_textureImageMemory = VK_NULL_HANDLE;
};

