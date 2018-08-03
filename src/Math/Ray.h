#pragma once

#include <glm/glm.hpp>

namespace Math
{

class Ray
{
public:

	Ray() noexcept;
	Ray(const glm::vec3& origin, const glm::vec3& direction) noexcept;
	Ray(const glm::vec3& origin, const glm::vec3& direction, float time) noexcept;

	glm::vec3 origin() const noexcept { return m_Origin; }
	glm::vec3 direction() const noexcept { return m_Direction; }
	glm::vec3 position(float t) const noexcept;
	float time() const noexcept;

private:

	float m_Time;
	glm::vec3 m_Origin;
	glm::vec3 m_Direction;
};

}
