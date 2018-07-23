#pragma once

#include <glm/glm.hpp>
#include <Math/Ray.h>

class Camera
{
public:
	
	Camera(const glm::vec3& lookfrom, const glm::vec3& lookat, const glm::vec3& vup, float vfov, float aspect, float aperture, float focusDistance, float t0, float t1);
	Math::Ray ray(float u, float v) const;

private:

	float m_Fov;
	float m_Aspect;
	float m_LensRadius;
	float m_Time0, m_Time1;
	glm::vec3 m_Right, m_Up, m_Back;
	glm::vec3 m_LowerLeftCorner;
	glm::vec3 m_Horizontal;
	glm::vec3 m_Vertical;
	glm::vec3 m_Origin;
};
