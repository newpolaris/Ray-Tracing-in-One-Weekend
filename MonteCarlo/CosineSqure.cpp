#include <cmath>
#include <math.h>
#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include "../external/glm/glm/glm.hpp"

namespace Math
{
	inline float Random()
	{
		return drand48();
	}

	glm::vec3 RandomInUnitSphere()
	{
		glm::vec3 p;
		do {
			p = 2.f * glm::vec3(Random(), Random(), Random()) - 1.f;
		} while (glm::dot(p, p) >= 1.f);
		return glm::normalize(p);
	}
}

//
// solve intergal cos(theta)^2, over half sphere
//
// RESULT: 
//
// Uniform sampling: 4.200013
//

const float pi = std::acos(-1);

// expect d as uniform vector
inline float f(const glm::vec3& d) 
{
	return d.z * d.z;
}

// distriution itself uniform
inline float pdf(const glm::vec3& d) 
{
	return 1.f / (4 * pi);
}


int main()
{
	const int n = 100000;

	float sum = 0.f;
	for (int i = 0; i < n; i++)
	{
		auto d = Math::RandomInUnitSphere();
		sum += f(d) / pdf(d);
	}
	float ans = sum / n;
	printf("%f", ans);
}
