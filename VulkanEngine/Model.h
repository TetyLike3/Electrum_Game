#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include "Utilities.h"
#include "Buffers.h"




class Model
{
public:
	Model() : m_pUtilities(Utilities::getInstance()) {};


private:
	Utilities* m_pUtilities = nullptr;

	std::vector<VertexBuffer::sVertex> vertices;
	std::vector<uint32_t> indices;
	std::string m_modelPath = "";
	std::string m_texturePath = "";
};

