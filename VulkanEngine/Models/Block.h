#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "../Utilities/Utilities.h"
#include "../Graphics/Image.h"
#include "../Graphics/Vertex.h"



class Block
{
public:
	Block(glm::vec3 pos, std::string texturePath) : m_pos(pos), m_pUtilities(Utilities::getInstance()) {};

	void buildModel();
	void cleanup();

	std::vector<Vertex> getVertices() { return m_vertices; };
	std::vector<uint32_t> getIndices() { return m_indices; };

private:
	Utilities* m_pUtilities = nullptr;

	glm::vec3 m_pos;

	std::vector<Vertex> m_vertices;
	std::vector<uint32_t> m_indices;
	Image* m_pTextureImage = nullptr;
};