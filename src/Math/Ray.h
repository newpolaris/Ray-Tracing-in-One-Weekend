#pragma once


#include <glm/glm.hpp>

namespace Math
{

bool refract(const glm::vec3& l, const glm::vec3& n, float r, glm::vec3& refracted);
float schlick(float cosine, float refractiveIndex);

class Ray
{
public:

	Ray() noexcept;
	Ray(const glm::vec3& origin, const glm::vec3& direction) noexcept;

	glm::vec3 origin() const noexcept { return m_Origin; }
	glm::vec3 direction() const noexcept { return m_Direction; }
	glm::vec3 position(float t) const noexcept;

private:

	glm::vec3 m_Origin;
	glm::vec3 m_Direction;
};

}
