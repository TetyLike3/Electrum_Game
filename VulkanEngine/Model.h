#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include "Utilities.h"
#include "Buffers.h"
#include "Image.h"




class Model
{
public:
	Model(std::string modelPath, std::string texturePath) : m_modelPath(modelPath), m_texturePath(texturePath), m_pUtilities(Utilities::getInstance()) {};

	void createModel();


private:
	Utilities* m_pUtilities = nullptr;

	std::vector<VertexBuffer::sVertex> m_vertices;
	std::vector<uint32_t> m_indices;
	std::string m_modelPath = "";
	std::string m_texturePath = "";
	Image* m_pTextureImage = nullptr;
};

