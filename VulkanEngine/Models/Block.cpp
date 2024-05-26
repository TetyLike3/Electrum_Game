#include "Block.h"

void Block::buildModel() {
	// Define the vertices and indices for the block
	// Vertices are defined as {XYZ position, Vertex Color, UV texture position, color/texture blend}
	m_vertices = {
		// Front face - Red
		{{m_pos.x - 1.0f, -1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}, 0.5f},
		{{m_pos.x + 1.0f, -1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}, 0.5f},
		{{m_pos.x + 1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}, 0.5f},
		{{m_pos.x - 1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}, 0.5f},

		// Back face - Green
		{{m_pos.x - 1.0f, -1.0f, -1.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}, 0.5f},
		{{m_pos.x - 1.0f, 1.0f, -1.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}, 0.5f},
		{{m_pos.x + 1.0f, 1.0f, -1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}, 0.5f},
		{{m_pos.x + 1.0f, -1.0f, -1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}, 0.5f},

		// Top face - Blue
		{{m_pos.x + 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}, 0.5f},
		{{m_pos.x + 1.0f, 1.0f, -1.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}, 0.5f},
		{{m_pos.x - 1.0f, 1.0f, -1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}, 0.5f},
		{{m_pos.x - 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}, 0.5f},

		// Bottom face - Yellow
		{{m_pos.x - 1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, 0.0f}, {1.0f, 0.0f}, 0.5f},
		{{m_pos.x + 1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, 0.0f}, {0.0f, 0.0f}, 0.5f},
		{{m_pos.x + 1.0f, -1.0f, 1.0f}, {1.0f, 1.0f, 0.0f}, {0.0f, 1.0f}, 0.5f},
		{{m_pos.x - 1.0f, -1.0f, 1.0f}, {1.0f, 1.0f, 0.0f}, {1.0f, 1.0f}, 0.5f},

		// Right face - Magenta
		{{m_pos.x + 1.0f, -1.0f, 1.0f}, {1.0f, 0.0f, 1.0f}, {0.0f, 1.0f}, 0.5f},
		{{m_pos.x + 1.0f, -1.0f, -1.0f}, {1.0f, 0.0f, 1.0f}, {0.0f, 0.0f}, 0.5f},
		{{m_pos.x + 1.0f, 1.0f, -1.0f}, {1.0f, 0.0f, 1.0f}, {1.0f, 0.0f}, 0.5f},
		{{m_pos.x + 1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 1.0f}, {1.0f, 1.0f}, 0.5f},

		// Left face - Cyan
		{{m_pos.x - 1.0f, 1.0f, -1.0f}, {0.0f, 1.0f, 1.0f}, {0.0f, 0.0f}, 0.5f},
		{{m_pos.x - 1.0f, -1.0f, -1.0f}, {0.0f, 1.0f, 1.0f}, {1.0f, 0.0f}, 0.5f},
		{{m_pos.x - 1.0f, -1.0f, 1.0f}, {0.0f, 1.0f, 1.0f}, {1.0f, 1.0f}, 0.5f},
		{{m_pos.x - 1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 1.0f}, {0.0f, 1.0f}, 0.5f},
	};

	m_indices = {
		0, 1, 2, 2, 3, 0, // Front face
		4, 5, 6, 6, 7, 4, // Back face
		8, 9, 10, 10, 11, 8, // Top face
		12, 13, 14, 14, 15, 12, // Bottom face
		16, 17, 18, 18, 19, 16, // Right face
		20, 21, 22, 22, 23, 20 // Left face
	};

	mDebugPrint("Built model for block");
}

void Block::cleanup() {
	m_vertices = {};
	m_indices = {};
}