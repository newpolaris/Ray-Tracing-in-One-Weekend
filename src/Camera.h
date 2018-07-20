#pragma once

#include <glm/glm.hpp>
#include <Math/Ray.h>

class Camera
{
public:
	
	Camera(const glm::vec3& lookfrom, const glm::vec3& lookat, const glm::vec3& vup, float vfov, float aspect);
	Math::Ray ray(float u, float v) const;

private:

	float m_Fov;
	float m_Aspect;
	glm::vec3 m_LowerLeftCorner;
	glm::vec3 m_Horizontal;
	glm::vec3 m_Vertical;
	glm::vec3 m_Origin;
};
