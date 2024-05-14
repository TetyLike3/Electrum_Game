#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "../Utilities/Utilities.h"
#include "../Graphics/Image.h"
#include "../Graphics/Vertex.h"

class Model
{
public:
	Model(std::string modelPath, std::string texturePath) : m_modelPath(modelPath), m_texturePath(texturePath), m_pUtilities(Utilities::getInstance()) {
		createModel();
	};

	void createModel();


private:
	Utilities* m_pUtilities = nullptr;
	friend class VulkanEngine;

	std::vector<Vertex> m_vertices;
	std::vector<uint32_t> m_indices;
	std::string m_modelPath = "";
	std::string m_texturePath = "";
	Image* m_pTextureImage = nullptr;
};