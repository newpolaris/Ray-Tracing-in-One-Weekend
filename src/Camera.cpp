#include "Camera.h"
#include <glm/gtc/type_ptr.hpp> 
#include <Math/Random.h>

Camera::Camera(const glm::vec3& lookfrom, const glm::vec3& lookat, const glm::vec3& vup, float vfov, float aspect, float aperture, float focusDistance, float t0, float t1)
	: m_Fov(vfov)
	, m_Aspect(aspect)
	, m_LensRadius(aperture/2)
	, m_Time0(t0)
	, m_Time1(t1)
{
	const float pi = glm::pi<float>();
	const float theta = m_Fov * pi / 180.f; 
	const float halfHeight = glm::tan(theta/2.f);
	const float halfWidth = m_Aspect * halfHeight;

	m_Origin = lookfrom;

	glm::vec3 w = glm::normalize(lookfrom - lookat);
	glm::vec3 u = glm::normalize(glm::cross(vup, w));
	glm::vec3 v = glm::cross(w, u);

	m_LowerLeftCorner = -(u * halfWidth + v * halfHeight + w) * focusDistance;
	m_Horizontal = 2.f * u * halfWidth * focusDistance;
	m_Vertical = 2.f * v * halfHeight * focusDistance;

	m_Back = w;
	m_Right = u;
	m_Up = v;
}

Math::Ray Camera::ray(float s, float t) const
{
	glm::vec2 rd = m_LensRadius * Math::randomUnitDisk();
	// ray start offset, that affect lens size  
	glm::vec3 offset = m_Right * rd.x + m_Up * rd.y;
	// lens rays are focused at 'focusDistance' virtual plane
	glm::vec3 dir = glm::normalize(m_LowerLeftCorner + s*m_Horizontal + t*m_Vertical - offset);
	float time = m_Time0 + Math::BaseRandom() * (m_Time1 - m_Time0);

	return Math::Ray(m_Origin + offset, dir, time);
}
