#pragma once

#include <glm/glm.hpp>
#include <Math/Ray.h>

class Camera
{
public:
	
	Camera() 
		: m_LowerLeftCorner(-2.f, -1.f, -1.f)
		, m_Horizontal(4.f, 0.f, 0.f)
		, m_Vertical(0.f, 2.f, 0.f)
		, m_Origin(0.f)
	{
	}

	Math::Ray ray(float u, float v) const
	{
		glm::vec3 dir = glm::normalize(m_LowerLeftCorner + u*m_Horizontal + v*m_Vertical);
		return Math::Ray(m_Origin, dir);
	}

private:

	glm::vec3 m_LowerLeftCorner;
	glm::vec3 m_Horizontal;
	glm::vec3 m_Vertical;
	glm::vec3 m_Origin;
};
