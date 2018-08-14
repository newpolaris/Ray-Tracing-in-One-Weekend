#include "Sampling.h"

namespace Math {

// [PBRT] UniformSampleDisk
glm::vec2 uniformSampleDisk(const glm::vec2& u)
{
	float r = sqrt(u[0]);
	float theta = 2 * M_PI * u[1];
	return glm::vec2(r * cos(theta), r * sin(theta));
}

// [PBRT] UniformSampleDisk
glm::vec2 concentricSampleDisk(const glm::vec2& u)
{
	// Map uniform ramdom numbers to [-1, 1]^2
	glm::vec2 uOffset = 2.f * u - glm::vec2(1.f);
	// Handle degeneracy at the origin
	if (uOffset.x == 0 && uOffset.y == 0)
		return glm::vec2(0.f);
	// Apply concentric mapping to point
	float theta, r;
	if (fabs(uOffset.x) > abs(uOffset.y))
	{
		r = uOffset.x;
		theta = M_PI/4 * (uOffset.y / uOffset.x);
	}
	else
	{
		r = uOffset.y;
		theta = M_PI/2 - M_PI/4 * (uOffset.x / uOffset.y);
	}
	return r * glm::vec2(cos(theta), sin(theta));
}

// [PBRT]
glm::vec3 uniformSampleHemisphere(const glm::vec2& u)
{
	// let. 1 - ux = ux'
	float z = u.x;
	float r = sqrt(fmax(0, 1.f - z * z));
	float phi = 2 * M_PI * u.y;
	float x = r * cos(phi);
	float y = r * sin(phi);
	return glm::vec3(x, y, z);
}

// [PBRT] CosineSampleHemisphere
glm::vec3 cosineSampleHemisphere(const glm::vec2& u)
{
	glm::vec2 d = concentricSampleDisk(u);
	float z = sqrt(fmax(0, 1 - d.x*d.x - d.y*d.y));
	return glm::vec3(d.x, d.y, z);
}

glm::vec3 cosineSamplingSphere(const glm::vec2& u)
{
	float v = 1 - 2*u.y;
	float z = glm::sign(v) * glm::sqrt(glm::abs(v));
	float phi = 2 * M_PI * u.x;
	float r = glm::sqrt(1 - z*z);
	float x = r * cos(phi);
	float y = r * sin(phi);
	return glm::vec3(x, y, z);
}

}
