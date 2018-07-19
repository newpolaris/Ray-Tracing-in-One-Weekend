#include "Mesh.h"
#include <tools/gltools.hpp>

Mesh::Mesh()
{
}

Mesh::~Mesh()
{
	destroy();
}

void Mesh::initialize()
{
}

void Mesh::destroy()
{
	m_Material = nullptr;
}

void Mesh::render()
{
	if (m_Material)
	{
	}

	GL_ASSERT(glDrawElementsBaseVertex(
				GL_TRIANGLES,
				m_IndexCount, 
				GL_UNSIGNED_INT,
				(void*)(sizeof(unsigned int)*m_IndexBase), 
				m_VertexBase));

	CHECKGLERROR();
}
