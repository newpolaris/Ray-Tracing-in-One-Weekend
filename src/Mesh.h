#pragma once

#include <Types.h>

class Mesh
{
public:

	Mesh();
	virtual ~Mesh();

	void initialize();
	void destroy();
	void render();

	int32_t m_MaterialIndex = -1;
	unsigned int m_IndexBase = 0;
	unsigned int m_IndexCount = 0;
	unsigned int m_VertexBase = 0;
	MaterialPtr m_Material;
};
