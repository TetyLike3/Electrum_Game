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
	Image(std::string imagePath, LogicalDevice* pLogicalDevice, BufferManager* pBufferManager, sSettings::sGraphicsSettings* pGraphicsSettings)
		: m_imagePath(imagePath), m_pLogicalDevice(pLogicalDevice->getLogicalDevice()), m_pBufferManager(pBufferManager), m_pGraphicsSettings(pGraphicsSettings), m_pUtilities(Utilities::getInstance())
	{
		createTextureImage();
	};

	void createTextureImage();
	void createTextureImageView();
	void createTextureSampler();
	VkImageView createImageView(VkImage image, VkFormat format);
	void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
	void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
	void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

	void cleanup();

	VkImageView* getVkTextureImageView() { return &m_textureImageView; };
	VkSampler* getVkTextureSampler() { return &m_textureSampler; };

private:
	Utilities* m_pUtilities = nullptr;
	VkDevice* m_pLogicalDevice = nullptr;
	BufferManager* m_pBufferManager = nullptr;
	sSettings::sGraphicsSettings* m_pGraphicsSettings = nullptr;


	std::string m_imagePath = "";
	VkImage m_textureImage = VK_NULL_HANDLE;
	VkDeviceMemory m_textureImageMemory = VK_NULL_HANDLE;
	VkImageView m_textureImageView = VK_NULL_HANDLE;
	VkSampler m_textureSampler = VK_NULL_HANDLE;
};

