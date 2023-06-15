#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "../Utilities/Utilities.h"
#include "../Graphics/Image.h"



struct sVertex {
	glm::vec3 pos;
	glm::vec3 color;
	glm::vec2 texCoord;
	glm::lowp_f32 colorBlendTex;

	static VkVertexInputBindingDescription getBindingDescription()
	{
		VkVertexInputBindingDescription bindingDescription{
			.binding = 0,
			.stride = sizeof(sVertex),
			.inputRate = VK_VERTEX_INPUT_RATE_VERTEX
		};

		return bindingDescription;
	}

	static std::array<VkVertexInputAttributeDescription, 4> getAttributeDescriptions()
	{
		std::array<VkVertexInputAttributeDescription, 4> attributeDescriptions{
			VkVertexInputAttributeDescription{
				.location = 0,
					.binding = 0,
					.format = VK_FORMAT_R32G32B32_SFLOAT,
					.offset = offsetof(sVertex, pos)
			},
				VkVertexInputAttributeDescription{
					.location = 1,
					.binding = 0,
					.format = VK_FORMAT_R32G32B32_SFLOAT,
					.offset = offsetof(sVertex, color)
			},
				VkVertexInputAttributeDescription{
					.location = 2,
					.binding = 0,
					.format = VK_FORMAT_R32G32_SFLOAT,
					.offset = offsetof(sVertex, texCoord)
			},
				VkVertexInputAttributeDescription{
					.location = 3,
					.binding = 0,
					.format = VK_FORMAT_R16_SFLOAT,
					.offset = offsetof(sVertex, colorBlendTex)
			}
		};

		return attributeDescriptions;
	}
};

class Model
{
public:
	Model(std::string modelPath, std::string texturePath) : m_modelPath(modelPath), m_texturePath(texturePath), m_pUtilities(Utilities::getInstance()) { createModel(); };

	void createModel();

	std::vector<sVertex> getVertices() { return m_vertices; };
	std::vector<uint32_t> getIndices() { return m_indices; };
	std::string getModelPath() { return m_modelPath; };
	std::string getTexturePath() { return m_texturePath; };

private:
	Utilities* m_pUtilities = nullptr;

	std::vector<sVertex> m_vertices;
	std::vector<uint32_t> m_indices;
	std::string m_modelPath = "";
	std::string m_texturePath = "";
	Image* m_pTextureImage = nullptr;
};

