#include "Camera.h"
#include <glm/gtc/type_ptr.hpp> 

Camera::Camera(const glm::vec3& lookfrom, const glm::vec3& lookat, const glm::vec3& vup, float vfov, float aspect)
	: m_Fov(vfov)
	, m_Aspect(aspect)
{
	const float pi = glm::pi<float>();
	const float theta = m_Fov * pi / 180.f; 
	const float halfHeight = glm::tan(theta/2.f);
	const float halfWidth = m_Aspect * halfHeight;

	m_Origin = lookfrom;

	glm::vec3 w = glm::normalize(lookfrom - lookat);
	glm::vec3 u = glm::normalize(glm::cross(vup, w));
	glm::vec3 v = glm::cross(w, u);

	m_LowerLeftCorner = m_Origin - (u * halfWidth + v * halfHeight + w);
	m_Horizontal = 2.f * u * halfWidth;
	m_Vertical = 2.f * v * halfHeight;
}

Math::Ray Camera::ray(float u, float v) const
{
	glm::vec3 dir = glm::normalize(m_LowerLeftCorner + u*m_Horizontal + v*m_Vertical - m_Origin);
	return Math::Ray(m_Origin, dir);
}
