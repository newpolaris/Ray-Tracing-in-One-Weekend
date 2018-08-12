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

// [PBRT] CosineSampleHemisphere
glm::vec3 cosineSampleHemisphere(const glm::vec2& u)
{
	glm::vec2 d = concentricSampleDisk(u);
	float z = sqrt(fmax(0, 1 - d.x*d.x - d.y*d.y));
	return glm::vec3(d.x, d.y, z);
}

}
