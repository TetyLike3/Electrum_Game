#include "Block.h"

void Block::buildModel() {
	m_vertices = {
		{{m_pos.x - 1.0f, m_pos.y - 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f}, 0.5f},
		{{m_pos.x + 1.0f, m_pos.y - 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}, 0.5f},
		{{m_pos.x + 1.0f, m_pos.y + 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}, 0.5f},
		{{m_pos.x - 1.0f, m_pos.y + 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}, 0.5f},

		{{m_pos.x + 1.0f, m_pos.y - 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f}, 0.5f},
		{{m_pos.x + 1.0f, m_pos.y - 1.0f, -1.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}, 0.5f},
		{{m_pos.x + 1.0f, m_pos.y + 1.0f, -1.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}, 0.5f},
		{{m_pos.x + 1.0f, m_pos.y + 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}, 0.5f},

		{{m_pos.x + 1.0f, m_pos.y - 1.0f, -1.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f}, 0.5f},
		{{m_pos.x - 1.0f, m_pos.y - 1.0f, -1.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}, 0.5f},
		{{m_pos.x - 1.0f, m_pos.y + 1.0f, -1.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}, 0.5f},
		{{m_pos.x + 1.0f, m_pos.y + 1.0f, -1.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}, 0.5f},

		{{m_pos.x - 1.0f, m_pos.y - 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f}, 0.5f},
		{{m_pos.x - 1.0f, m_pos.y - 1.0f, -1.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}, 0.5f},
		{{m_pos.x + 1.0f, m_pos.y + 1.0f, -1.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}, 0.5f},
		{{m_pos.x + 1.0f, m_pos.y - 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}, 0.5f},
	};

	m_indices = {
		0, 1, 2, 2, 3, 0,
		4, 5, 6, 6, 7, 4,
		8, 9, 10, 10, 11, 8,
		12, 13, 14, 14, 15, 12,
	};

	mDebugPrint("Built model for block");
}

void Block::cleanup() {
	m_vertices = {};
	m_indices = {};
}