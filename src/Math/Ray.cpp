#include "Ray.h"

using namespace Math;

Ray::Ray() noexcept
{
}

Ray::Ray(const glm::vec3& origin, const glm::vec3& direction) noexcept
	: m_Origin(origin)
	, m_Direction(direction)
	, m_Time(0.f)
{
}

Ray::Ray(const glm::vec3& origin, const glm::vec3& direction, float time) noexcept
	: m_Origin(origin)
	, m_Direction(direction)
	, m_Time(time)
{
}

float Ray::time() const noexcept
{
	return m_Time;
}

glm::vec3 Ray::position(float t) const noexcept
{
	return t*m_Direction + m_Origin;
}
