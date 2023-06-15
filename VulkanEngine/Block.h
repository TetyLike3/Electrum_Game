#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "Image.h"
#include "Utilities.h"



class Block
{
public:
	Block(uint8_t size, std::string texturePath) : m_size(size), m_pUtilities(Utilities::getInstance()) {};

private:
	Utilities* m_pUtilities = nullptr;

	uint8_t m_size = 1;
	Image* m_pTextureImage = nullptr;
};