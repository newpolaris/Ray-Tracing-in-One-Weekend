#include "Ray.h"

namespace Math 
{

// wiki snell's law - 'vector form'
//
// l : incident vector
// r : ni over nt, where ni and nt is refractive index
bool refract(const glm::vec3& l, const glm::vec3& n, float r, glm::vec3& refracted)
{
	glm::vec3 ll = glm::normalize(l);

	float c = -glm::dot(ll, n);	
	float discriminant = 1.f - r*r*(1 - c*c);
	if (discriminant <= 0) 
		return false;
	refracted = r * ll + (r*c - glm::sqrt(discriminant)) * n;
	return true;
}

float schlick(float cosine, float refractiveIndex)
{
	float r = (1.f - refractiveIndex) / (1 + refractiveIndex);
	float r0 = r*r;
	float k = 1.f - cosine;
	return r0 + (1.f - r0) * (k*k*k*k*k);
}

}

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
