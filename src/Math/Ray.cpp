#include "Ray.h"

using namespace Math;

Ray::Ray() noexcept
{
}

Ray::Ray(const glm::vec3& origin, const glm::vec3& direction) noexcept
	: m_Origin(origin)
	, m_Direction(direction)
{
}

glm::vec3 Ray::position(float t) const noexcept
{
	return t*m_Direction + m_Origin;
}
