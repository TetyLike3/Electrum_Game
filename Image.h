#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

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

	void cleanup();

private:
	Utilities* m_pUtilities = nullptr;
	VkDevice* m_pLogicalDevice = nullptr;
	BufferManager* m_pBufferManager = nullptr;


	std::string m_imagePath = "";
	VkImage m_textureImage = VK_NULL_HANDLE;
	VkDeviceMemory m_textureImageMemory = VK_NULL_HANDLE;


	void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
};

