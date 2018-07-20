#pragma once

#include <glm/glm.hpp>
#include <Math/Ray.h>

class Camera
{
public:
	
	Camera(float vfov, float aspect) 
		: m_Fov(vfov)
		, m_Aspect(aspect)
	{
		const float pi = glm::pi<float>();
		const float theta = m_Fov * pi / 180.f; 
		const float halfHeight = glm::tan(theta/2.f);
		const float halfWidth = m_Aspect * halfHeight;

		m_LowerLeftCorner = glm::vec3(-halfWidth, -halfHeight, -1.f);
		m_Horizontal = glm::vec3(2*halfWidth, 0.f, 0.f);
		m_Vertical = glm::vec3(0.f, 2*halfHeight, 0.f);
		m_Origin = glm::vec3(0.f);
	}

	Math::Ray ray(float u, float v) const
	{
		glm::vec3 dir = glm::normalize(m_LowerLeftCorner + u*m_Horizontal + v*m_Vertical);
		return Math::Ray(m_Origin, dir);
	}

private:

	float m_Fov;
	float m_Aspect;
	glm::vec3 m_LowerLeftCorner;
	glm::vec3 m_Horizontal;
	glm::vec3 m_Vertical;
	glm::vec3 m_Origin;
};
